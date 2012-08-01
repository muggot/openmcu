/*
 * http.h
 *
 * HyperText Transport Protocol classes.
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
 * $Log: http.h,v $
 * Revision 1.61  2007/04/10 05:08:46  rjongbloed
 * Fixed issue with use of static C string variables in DLL environment,
 *   must use functional interface for correct initialisation.
 *
 * Revision 1.60  2005/11/30 12:47:37  csoutheren
 * Removed tabs, reformatted some code, and changed tags for Doxygen
 *
 * Revision 1.59  2005/01/03 12:48:41  csoutheren
 * Added new configure options and ability to disable/enable modules
 *
 * Revision 1.58  2004/10/23 11:34:59  ykiryanov
 * Added ifdef _WIN32_WCE for PocketPC 2003 SDK port
 *
 * Revision 1.57  2002/12/03 22:37:36  robertj
 * Removed get document that just returns a content length as the chunked
 *   transfer encoding makes this very dangerous.
 * Added GetTextDocument() to get a URL content into a PString.
 * Added a version pf PostData() that gets the reponse content into a PString.
 * Added ReadContentBody() that takes a PString, not just PBYTEArray.
 *
 * Revision 1.56  2002/11/06 22:47:23  robertj
 * Fixed header comment (copyright etc)
 *
 * Revision 1.55  2002/10/10 04:43:43  robertj
 * VxWorks port, thanks Martijn Roest
 *
 * Revision 1.54  2002/10/02 08:54:34  craigs
 * Added support for XMLRPC server
 *
 * Revision 1.53  2002/09/16 01:08:59  robertj
 * Added #define so can select if #pragma interface/implementation is used on
 *   platform basis (eg MacOS) rather than compiler, thanks Robert Monaghan.
 *
 * Revision 1.52  2002/05/08 05:38:53  robertj
 * Added PHTTPTailFile resource to do a unix 'tail -f' of a file.
 *
 * Revision 1.51  2001/10/31 01:34:47  robertj
 * Added extra const for constant HTTP tag name strings.
 * Changes to support HTTP v1.1 chunked transfer encoding.
 *
 * Revision 1.50  2001/10/03 00:26:34  robertj
 * Upgraded client to HTTP/1.1 and for chunked mode entity bodies.
 *
 * Revision 1.49  2001/09/28 08:55:15  robertj
 * More changes to support restartable PHTTPClient
 *
 * Revision 1.48  2001/09/28 00:42:54  robertj
 * Added automatic setting of some outward MIME fields.
 * Added "user agent" string field for automatic inclusion.
 * Added function to read the contents of the HTTP request.
 *
 * Revision 1.47  2001/08/28 06:44:30  craigs
 * Added ability to override PHTTPServer creation
 *
 * Revision 1.46  2001/03/14 01:49:54  craigs
 * Added ability to handle multi-part form POST commands
 *
 * Revision 1.45  2001/02/22 05:26:47  robertj
 * Added "nicer" version of GetDocument in HTTP client class.
 *
 * Revision 1.44  2001/01/15 06:16:53  robertj
 * Set HTTP resource members to private to assure are not modified by
 *   dscendents in non-threadsafe manner.
 *
 * Revision 1.43  2000/09/04 03:57:58  robertj
 * Added ability to change the persistent connection parameters (timeout etc).
 *
 * Revision 1.42  2000/05/02 08:28:10  craigs
 * Removed "memory leaks" caused by brain-dead GNU linker
 *
 * Revision 1.41  1999/09/17 01:11:14  robertj
 * Fixed some documentation typos.
 *
 * Revision 1.40  1999/05/13 04:04:04  robertj
 * Fixed problem of initialised commandName in ConnectionInfo.
 *
 * Revision 1.39  1999/05/04 15:26:01  robertj
 * Improved HTTP/1.1 compatibility (pass through user commands).
 * Fixed problems with quicktime installer.
 *
 * Revision 1.38  1999/04/21 01:58:08  robertj
 * Fixed problem with reading data for request using second form of PHTTPRequestInfo constructor.
 *
 * Revision 1.37  1999/03/09 08:01:46  robertj
 * Changed comments for doc++ support (more to come).
 *
 * Revision 1.36  1999/02/16 08:07:10  robertj
 * MSVC 6.0 compatibility changes.
 *
 * Revision 1.35  1998/11/30 02:50:47  robertj
 * New directory structure
 *
 * Revision 1.34  1998/10/31 12:49:21  robertj
 * Added read/write mutex to the HTTP space variable to avoid thread crashes.
 *
 * Revision 1.33  1998/10/25 01:00:46  craigs
 * Added ability to specify per-directory authorisation for PHTTPDirectory
 *
 * Revision 1.32  1998/09/23 06:19:29  robertj
 * Added open source copyright license.
 *
 * Revision 1.31  1998/07/24 06:58:42  robertj
 * Changed PostData function so just has string for data instead of dictionary.
 *
 * Revision 1.30  1998/06/16 03:33:33  robertj
 * Changed TCP connection shutdown to be parameterised.
 * Propagated persistence and proxy flags in new connection info instances.
 *
 * Revision 1.29  1998/04/14 03:42:59  robertj
 * Fixed error code propagation in HTTP client.
 *
 * Revision 1.28  1998/02/03 06:29:38  robertj
 * Added local address and port to PHTTPRequest.
 *
 * Revision 1.27  1998/01/26 00:24:24  robertj
 * Added more information to PHTTPConnectionInfo.
 * Added function to allow HTTPClient to automatically connect if URL has hostname.
 *
 * Revision 1.26  1997/10/30 10:22:52  robertj
 * Added multiple user basic authorisation scheme.
 *
 * Revision 1.25  1997/10/03 13:30:15  craigs
 * Added ability to access client socket from within HTTP resources
 *
 * Revision 1.24  1997/03/28 04:40:22  robertj
 * Added tags for cookies.
 *
 * Revision 1.23  1997/01/12 04:15:19  robertj
 * Globalised MIME tag strings.
 *
 * Revision 1.22  1996/10/26 03:31:05  robertj
 * Changed OnError so can pass in full HTML page as parameter.
 *
 * Revision 1.21  1996/09/14 13:09:10  robertj
 * Major upgrade:
 *   rearranged sockets to help support IPX.
 *   added indirect channel class and moved all protocols to descend from it,
 *   separating the protocol from the low level byte transport.
 *
 * Revision 1.20  1996/08/22 13:20:55  robertj
 * Fixed bug in authorisation, missing virtual prevented polymorphism.
 *
 * Revision 1.19  1996/08/19 13:44:06  robertj
 * Fixed authorisation so if have no user/password on basic authentication, does not require it.
 *
 * Revision 1.18  1996/06/28 13:15:23  robertj
 * Modified HTTPAuthority so gets PHTTPReqest (mainly for URL) passed in.
 * Moved HTTP form resource to another compilation module.
 *
 * Revision 1.17  1996/06/07 13:52:20  robertj
 * Added PUT to HTTP proxy FTP. Necessitating redisign of entity body processing.
 *
 * Revision 1.16  1996/05/23 10:00:52  robertj
 * Added common function for GET and HEAD commands.
 * Fixed status codes to be the actual status code instead of sequential enum.
 * This fixed some problems with proxy pass through of status codes.
 *
 * Revision 1.14  1996/03/31 08:46:51  robertj
 * HTTP 1.1 upgrade.
 *
 * Revision 1.13  1996/03/17 05:41:57  robertj
 * Added hit count to PHTTPResource.
 *
 * Revision 1.12  1996/03/16 04:39:55  robertj
 * Added ParseReponse() for splitting reponse line into code and info.
 * Added client side support for HTTP socket.
 * Added hooks for proxy support in HTTP socket.
 *
 * Revision 1.11  1996/03/10 13:15:23  robertj
 * Redesign to make resources thread safe.
 *
 * Revision 1.10  1996/03/02 03:12:55  robertj
 * Added radio button and selection boxes to HTTP form resource.
 *
 * Revision 1.9  1996/02/25 11:14:21  robertj
 * Radio button support for forms.
 *
 * Revision 1.8  1996/02/25 02:57:48  robertj
 * Removed pass through HTTP resource.
 *
 * Revision 1.7  1996/02/19 13:25:43  robertj
 * Added overwrite option to AddResource().
 * Added get/set string to PHTTPString resource.
 * Moved nested classes from PHTTPForm.
 *
 * Revision 1.6  1996/02/13 13:09:16  robertj
 * Added extra parameters to callback function in PHTTPResources, required
 *   by descendants to make informed decisions on data being loaded.
 *
 * Revision 1.5  1996/02/08 12:04:19  robertj
 * Redesign of resource object callback virtuals.
 * Added HTML form resource type.
 *
 * Revision 1.4  1996/02/03 11:03:32  robertj
 * Added ismodified since and expires time checking.
 * Added PHTTPString that defaults to empty string.
 *
 * Revision 1.3  1996/01/28 14:15:38  robertj
 * Changed PCharArray in OnLoadData to PString for convenience in mangling data.
 * Beginning of pass through resource type.
 *
 * Revision 1.2  1996/01/26 02:24:26  robertj
 * Further implemetation.
 *
 * Revision 1.1  1996/01/23 13:04:20  robertj
 * Initial revision
 *
 */

#ifndef _PHTTP
#define _PHTTP

#ifdef P_USE_PRAGMA
#pragma interface
#endif

#include <ptbuildopts.h>

#include <ptclib/inetprot.h>
#include <ptclib/mime.h>
#include <ptclib/url.h>
#include <ptlib/ipsock.h>

#ifdef P_HTTPSVC

#include <ptclib/html.h>

//////////////////////////////////////////////////////////////////////////////
// PHTTPSpace

class PHTTPResource;

/** This class describes a name space that a Universal Resource Locator operates
   in. Each section of the hierarchy field of the URL points to a leg in the
   tree specified by this class.
 */
class PHTTPSpace : public PContainer
{
  PCONTAINERINFO(PHTTPSpace, PContainer)
  public:
    /// Constructor for HTTP URL Name Space
    PHTTPSpace();


  // New functions for class.
    enum AddOptions {
      /// Generate error if resource already exists
      ErrorOnExist,
      /// Overwrite the existing resource at URL location
      Overwrite
    };


    /** Add a new resource to the URL space. If there is already a resource at
       the location in the tree, or that location in the tree is already in
       the path to another resource then the function will fail.

       The <CODE>overwrite</CODE> flag can be used to replace an existing
       resource. The function will still fail if the resource is on a partial
       path to another resource but not if it is a leaf node.

       @return
       TRUE if resource added, FALSE if failed.
     */
    BOOL AddResource(
      PHTTPResource * resource, ///< Resource to add to the name space.
      AddOptions overwrite = ErrorOnExist
        ///< Flag to overwrite an existing resource if it already exists.
    );

    /** Delete an existing resource to the URL space. If there is not a resource
       at the location in the tree, or that location in the tree is in the
       path to another resource then the function will fail.

       @return
       TRUE if resource deleted, FALSE if failed.
     */
    BOOL DelResource(
      const PURL & url          ///< URL to search for in the name space.
    );

    /** Locate the resource specified by the URL in the URL name space.

       @return
       The resource found or NULL if no resource at that position in hiearchy.
     */
    PHTTPResource * FindResource(
      const PURL & url   ///< URL to search for in the name space.
    );

    /** This function attempts to acquire the mutex for reading.
     */
    void StartRead() const
      { mutex->StartRead(); }

    /** This function attempts to release the mutex for reading.
     */
    void EndRead() const
      { mutex->EndRead(); }

    /** This function attempts to acquire the mutex for writing.
     */
    void StartWrite() const
      { mutex->StartWrite(); }

    /** This function attempts to release the mutex for writing.
     */
    void EndWrite() const
      { mutex->EndWrite(); }


  protected:
    PReadWriteMutex * mutex;

    class Node;
    PSORTED_LIST(ChildList, Node);
    class Node : public PString
    {
      PCLASSINFO(Node, PString)
      public:
        Node(const PString & name, Node * parentNode);
        ~Node();

        Node          * parent;
        ChildList       children;
        PHTTPResource * resource;
    } * root;

  private:
    BOOL SetSize(PINDEX) { return FALSE; }
};

#endif // P_HTTPSVC

#ifdef _WIN32_WCE
#undef TRACE
#endif

//////////////////////////////////////////////////////////////////////////////
// PHTTP

/** A common base class for TCP/IP socket for the HyperText Transfer Protocol
version 1.0 client and server.
 */
class PHTTP : public PInternetProtocol
{
  PCLASSINFO(PHTTP, PInternetProtocol)

  public:
  // New functions for class.
    enum Commands {
      // HTTP/1.0 commands
      GET, HEAD, POST,
      // HTTP/1.1 commands
      PUT, DELETE, TRACE, OPTIONS,
      // HTTPS command
      CONNECT,
      NumCommands
    };

    enum StatusCode {
      Continue = 100,              ///< 100 - Continue
      SwitchingProtocols,          ///< 101 - upgrade allowed
      RequestOK = 200,             ///< 200 - request has succeeded
      Created,                     ///< 201 - new resource created: entity body contains URL
      Accepted,                    ///< 202 - request accepted, but not yet completed
      NonAuthoritativeInformation, ///< 203 - not definitive entity header
      NoContent,                   ///< 204 - no new information
      ResetContent,                ///< 205 - contents have been reset
      PartialContent,              ///< 206 - partial GET succeeded
      MultipleChoices = 300,       ///< 300 - requested resource available elsewehere 
      MovedPermanently,            ///< 301 - resource moved permanently: location field has new URL
      MovedTemporarily,            ///< 302 - resource moved temporarily: location field has new URL
      SeeOther,                    ///< 303 - see other URL
      NotModified,                 ///< 304 - document has not been modified
      UseProxy,                    ///< 305 - proxy redirect
      BadRequest = 400,            ///< 400 - request malformed or not understood
      UnAuthorised,                ///< 401 - request requires authentication
      PaymentRequired,             ///< 402 - reserved 
      Forbidden,                   ///< 403 - request is refused due to unsufficient authorisation
      NotFound,                    ///< 404 - resource cannot be found
      MethodNotAllowed,            ///< 405 - not allowed on this resource
      NoneAcceptable,              ///< 406 - encoding not acceptable
      ProxyAuthenticationRequired, ///< 407 - must authenticate with proxy first
      RequestTimeout,              ///< 408 - server timeout on request
      Conflict,                    ///< 409 - resource conflict on action
      Gone,                        ///< 410 - resource gone away
      LengthRequired,              ///< 411 - no Content-Length
      UnlessTrue,                  ///< 412 - no Range header for TRUE Unless
      InternalServerError = 500,   ///< 500 - server has encountered an unexpected error
      NotImplemented,              ///< 501 - server does not implement request
      BadGateway,                  ///< 502 - error whilst acting as gateway
      ServiceUnavailable,          ///< 503 - server temporarily unable to service request
      GatewayTimeout               ///< 504 - timeout whilst talking to gateway
    };

    // Common MIME header tags
    static const PString & AllowTag();
    static const PString & AuthorizationTag();
    static const PString & ContentEncodingTag();
    static const PString & ContentLengthTag();
    static const PString & ContentTypeTag();
    static const PString & DateTag();
    static const PString & ExpiresTag();
    static const PString & FromTag();
    static const PString & IfModifiedSinceTag();
    static const PString & LastModifiedTag();
    static const PString & LocationTag();
    static const PString & PragmaTag();
    static const PString & PragmaNoCacheTag();
    static const PString & RefererTag();
    static const PString & ServerTag();
    static const PString & UserAgentTag();
    static const PString & WWWAuthenticateTag();
    static const PString & MIMEVersionTag();
    static const PString & ConnectionTag();
    static const PString & KeepAliveTag();
    static const PString & TransferEncodingTag();
    static const PString & ChunkedTag();
    static const PString & ProxyConnectionTag();
    static const PString & ProxyAuthorizationTag();
    static const PString & ProxyAuthenticateTag();
    static const PString & ForwardedTag();
    static const PString & SetCookieTag();
    static const PString & CookieTag();

  protected:
    /** Create a TCP/IP HTTP protocol channel.
     */
    PHTTP();

    /** Parse a response line string into a response code and any extra info
       on the line. Results are placed into the member variables
       <CODE>lastResponseCode</CODE> and <CODE>lastResponseInfo</CODE>.

       The default bahaviour looks for a space or a '-' and splits the code
       and info either side of that character, then returns FALSE.

       @return
       Position of continuation character in response, 0 if no continuation
       lines are possible.
     */
    virtual PINDEX ParseResponse(
      const PString & line    ///< Input response line to be parsed
    );
};


//////////////////////////////////////////////////////////////////////////////
// PHTTPClient

/** A TCP/IP socket for the HyperText Transfer Protocol version 1.0.

   When acting as a client, the procedure is to make the connection to a
   remote server, then to retrieve a document using the following procedure:
      <PRE><CODE>
      PHTTPSocket web("webserver");
      if (web.IsOpen()) {
        PINDEX len;
        if (web.GetDocument("http://www.someone.com/somewhere/url", len)) {
          PString html = web.ReadString(len);
          if (!html.IsEmpty())
            ProcessHTML(html);
        }
        else
           PError << "Could not get page." << endl;
      }
      else
         PError << "HTTP conection failed." << endl;
      </PRE></CODE>
 */
class PHTTPClient : public PHTTP
{
  PCLASSINFO(PHTTPClient, PHTTP)

  public:
    /// Create a new HTTP client channel.
    PHTTPClient();
    PHTTPClient(
      const PString & userAgentName
    );


  // New functions for class.
    /** Send a command and wait for the response header (including MIME fields).
       Note that a body may still be on its way even if lasResponseCode is not
       200!

       @return
       TRUE if all of header returned and ready to receive body.
     */
    int ExecuteCommand(
      Commands cmd,
      const PURL & url,
      PMIMEInfo & outMIME,
      const PString & dataBody,
      PMIMEInfo & replyMime,
      BOOL persist = TRUE
    );
    int ExecuteCommand(
      const PString & cmdName,
      const PURL & url,
      PMIMEInfo & outMIME,
      const PString & dataBody,
      PMIMEInfo & replyMime,
      BOOL persist = TRUE
    );

    /// Write a HTTP command to server
    BOOL WriteCommand(
      Commands cmd,
      const PString & url,
      PMIMEInfo & outMIME,
      const PString & dataBody
    );
    BOOL WriteCommand(
      const PString & cmdName,
      const PString & url,
      PMIMEInfo & outMIME,
      const PString & dataBody
    );

    /// Read a response from the server
    BOOL ReadResponse(
      PMIMEInfo & replyMIME
    );

    /// Read the body of the HTTP command
    BOOL ReadContentBody(
      PMIMEInfo & replyMIME,
      PBYTEArray & body
    );
    BOOL ReadContentBody(
      PMIMEInfo & replyMIME,
      PString & body
    );


    /** Get the document specified by the URL.

       @return
       TRUE if document is being transferred.
     */
    BOOL GetTextDocument(
      const PURL & url,         ///< Universal Resource Locator for document.
      PString & document,       ///< Body read
      BOOL persist = TRUE       ///< if TRUE, enable HTTP persistence
    );

    /** Get the document specified by the URL.

       @return
       TRUE if document is being transferred.
     */
    BOOL GetDocument(
      const PURL & url,         ///< Universal Resource Locator for document.
      PMIMEInfo & outMIME,      ///< MIME info in request
      PMIMEInfo & replyMIME,    ///< MIME info in response
      BOOL persist = TRUE       ///< if TRUE, enable HTTP persistence
    );

    /** Get the header for the document specified by the URL.

       @return
       TRUE if document header is being transferred.
     */
    BOOL GetHeader(
      const PURL & url,         ///< Universal Resource Locator for document.
      PMIMEInfo & outMIME,      ///< MIME info in request
      PMIMEInfo & replyMIME,    ///< MIME info in response
      BOOL persist = TRUE       ///< if TRUE, enable HTTP persistence
    );


    /** Post the data specified to the URL.

       @return
       TRUE if document is being transferred.
     */
    BOOL PostData(
      const PURL & url,       ///< Universal Resource Locator for document.
      PMIMEInfo & outMIME,    ///< MIME info in request
      const PString & data,   ///< Information posted to the HTTP server.
      PMIMEInfo & replyMIME,  ///< MIME info in response
      BOOL persist = TRUE     ///< if TRUE, enable HTTP persistence
    );

    /** Post the data specified to the URL.

       @return
       TRUE if document is being transferred.
     */
    BOOL PostData(
      const PURL & url,       ///< Universal Resource Locator for document.
      PMIMEInfo & outMIME,    ///< MIME info in request
      const PString & data,   ///< Information posted to the HTTP server.
      PMIMEInfo & replyMIME,  ///< MIME info in response
      PString & replyBody,    ///< Body of response
      BOOL persist = TRUE     ///< if TRUE, enable HTTP persistence
    );

  protected:
    BOOL AssureConnect(const PURL & url, PMIMEInfo & outMIME);
    BOOL InternalReadContentBody(
      PMIMEInfo & replyMIME,
      PAbstractArray & body
    );

    PString userAgentName;
};

#ifdef P_HTTPSVC

//////////////////////////////////////////////////////////////////////////////
// PMultipartFormInfo

/** This object describes the information associated with a multi-part
    form entry
  */

class PMultipartFormInfo : public PObject
{
  PCLASSINFO(PMultipartFormInfo, PObject);
  public:
    PMIMEInfo mime;
    PString body;
};

PARRAY(PMultipartFormInfoArray, PMultipartFormInfo);

//////////////////////////////////////////////////////////////////////////////
// PHTTPConnectionInfo

class PHTTPServer;

/** This object describes the connectiono associated with a HyperText Transport
   Protocol request. This information is required by handler functions on
   #PHTTPResource# descendant classes to manage the connection correctly.
*/
class PHTTPConnectionInfo : public PObject
{
  PCLASSINFO(PHTTPConnectionInfo, PObject)
  public:
    PHTTPConnectionInfo();

    PHTTP::Commands GetCommandCode() const { return commandCode; }
    const PString & GetCommandName() const { return commandName; }

    const PURL & GetURL() const       { return url; }

    const PMIMEInfo & GetMIME() const { return mimeInfo; }
    void SetMIME(const PString & tag, const PString & value);

    BOOL IsCompatible(int major, int minor) const;

    BOOL IsPersistant() const         { return isPersistant; }
    BOOL WasPersistant() const        { return wasPersistant; }
    BOOL IsProxyConnection() const    { return isProxyConnection; }
    int  GetMajorVersion() const      { return majorVersion; }
    int  GetMinorVersion() const      { return minorVersion; }

    long GetEntityBodyLength() const  { return entityBodyLength; }

    /**Get the maximum time a persistent connection may persist.
      */
    PTimeInterval GetPersistenceTimeout() const { return persistenceTimeout; }

    /**Set the maximum time a persistent connection may persist.
      */
    void SetPersistenceTimeout(const PTimeInterval & t) { persistenceTimeout = t; }

    /**Get the maximum number of transations (GET/POST etc) for persistent connection.
       If this is zero then there is no maximum.
      */
    unsigned GetPersistenceMaximumTransations() const { return persistenceMaximum; }

    /**Set the maximum number of transations (GET/POST etc) for persistent connection.
       If this is zero then there is no maximum.
      */
    void SetPersistenceMaximumTransations(unsigned m) { persistenceMaximum = m; }

    const PMultipartFormInfoArray & GetMultipartFormInfo() const
      { return multipartFormInfoArray; }

    void ResetMultipartFormInfo()
      { multipartFormInfoArray.RemoveAll(); }

    PString GetEntityBody() const   { return entityBody; }

  protected:
    BOOL Initialise(PHTTPServer & server, PString & args);
    void DecodeMultipartFormInfo(const PString & type, const PString & entityBody);

    PHTTP::Commands commandCode;
    PString         commandName;
    PURL            url;
    PMIMEInfo       mimeInfo;
    BOOL            isPersistant;
    BOOL            wasPersistant;
    BOOL            isProxyConnection;
    int             majorVersion;
    int             minorVersion;
    PString         entityBody;        // original entity body (POST only)
    long            entityBodyLength;
    PTimeInterval   persistenceTimeout;
    unsigned        persistenceMaximum;
    PMultipartFormInfoArray multipartFormInfoArray;

  friend class PHTTPServer;
};


//////////////////////////////////////////////////////////////////////////////
// PHTTPServer

/** A TCP/IP socket for the HyperText Transfer Protocol version 1.0.

    When acting as a server, a descendant class would be created to override
    at least the #HandleOpenMailbox()#, #HandleSendMessage()# and
    #HandleDeleteMessage()# functions. Other functions may be overridden
    for further enhancement to the sockets capabilities, but these will give a
    basic POP3 server functionality.

    The server socket thread would continuously call the
    #ProcessMessage()# function until it returns FALSE. This will then
    call the appropriate virtual function on parsing the POP3 protocol.
 */
class PHTTPServer : public PHTTP
{
  PCLASSINFO(PHTTPServer, PHTTP)

  public:
    /** Create a TCP/IP HTTP protocol socket channel. The form with the single
       <CODE>port</CODE> parameter creates an unopened socket, the form with
       the <CODE>address</CODE> parameter makes a connection to a remote
       system, opening the socket. The form with the <CODE>socket</CODE>
       parameter opens the socket to an incoming call from a "listening"
       socket.
     */
    PHTTPServer();
    PHTTPServer(
     const PHTTPSpace & urlSpace  ///< Name space to use for URLs received.
    );


  // New functions for class.
    /** Get the name of the server.

       @return
       String name of the server.
     */
    virtual PString GetServerName() const;

    /** Get the name space being used by the HTTP server socket.

       @return
       URL name space tree.
     */
    PHTTPSpace & GetURLSpace() { return urlSpace; }

    /// Use a new URL name space for this HTTP socket.
    void SetURLSpace(
      const PHTTPSpace & space   ///< New URL name space to use.
    );


    /** Process commands, dispatching to the appropriate virtual function. This
       is used when the socket is acting as a server.

       @return
       TRUE if the request specified persistant mode and the request version
       allows it, FALSE if the socket closed, timed out, the protocol does not
       allow persistant mode, or the client did not request it
       timed out
     */
    virtual BOOL ProcessCommand();

    /** Handle a GET command from a client.

       The default implementation looks up the URL in the name space declared by
       the #PHTTPSpace# class tree and despatches to the
       #PHTTPResource# object contained therein.

       @return
       TRUE if the connection may persist, FALSE if the connection must close
       If there is no ContentLength field in the response, this value must
       be FALSE for correct operation.
     */
    virtual BOOL OnGET(
      const PURL & url,                    ///< Universal Resource Locator for document.
      const PMIMEInfo & info,              ///< Extra MIME information in command.
      const PHTTPConnectionInfo & conInfo  ///< HTTP connection information
    );



    /** Handle a HEAD command from a client.

       The default implemetation looks up the URL in the name space declared by
       the #PHTTPSpace# class tree and despatches to the
       #PHTTPResource# object contained therein.

       @return
       TRUE if the connection may persist, FALSE if the connection must close
       If there is no ContentLength field in the response, this value must
       be FALSE for correct operation.
     */
    virtual BOOL OnHEAD(
      const PURL & url,                   ///< Universal Resource Locator for document.
      const PMIMEInfo & info,             ///< Extra MIME information in command.
      const PHTTPConnectionInfo & conInfo ///< HTTP connection information
    );

    /** Handle a POST command from a client.

       The default implementation looks up the URL in the name space declared by
       the #PHTTPSpace# class tree and despatches to the
       #PHTTPResource# object contained therein.

       @return
       TRUE if the connection may persist, FALSE if the connection must close
       If there is no ContentLength field in the response, this value must
       be FALSE for correct operation.
     */
    virtual BOOL OnPOST(
      const PURL & url,                   ///< Universal Resource Locator for document.
      const PMIMEInfo & info,             ///< Extra MIME information in command.
      const PStringToString & data,       ///< Variables provided in the POST data.
      const PHTTPConnectionInfo & conInfo ///< HTTP connection information
    );

    /** Handle a proxy command request from a client. This will only get called
       if the request was not for this particular server. If it was a proxy
       request for this server (host and port number) then the appropriate
       #OnGET()#, #OnHEAD()# or #OnPOST()# command is called.

       The default implementation returns OnError(BadGateway).

       @return
       TRUE if the connection may persist, FALSE if the connection must close
       If there is no ContentLength field in the response, this value must
       be FALSE for correct operation.
     */
    virtual BOOL OnProxy(
      const PHTTPConnectionInfo & conInfo   ///<  HTTP connection information
    );


    /** Read the entity body associated with a HTTP request, and close the
       socket if not a persistant connection.

       @return
       The entity body of the command
     */
    virtual PString ReadEntityBody();

    /** Handle an unknown command.

       @return
       TRUE if the connection may persist, FALSE if the connection must close
     */
    virtual BOOL OnUnknown(
      const PCaselessString & command,         ///< Complete command line received.
      const PHTTPConnectionInfo & connectInfo  ///< HTTP connection information
    );

    /** Write a command reply back to the client, and construct some of the
       outgoing MIME fields. The MIME fields are not sent.

       The <CODE>bodySize</CODE> parameter determines the size of the 
       entity body associated with the response. If <CODE>bodySize</CODE> is
       >= 0, then a ContentLength field will be added to the outgoing MIME
       headers if one does not already exist.

       If <CODE>bodySize</CODE> is < 0, then it is assumed that the size of
       the entity body is unknown, or has already been added, and no
       ContentLength field will be constructed. 

       If the version of the request is less than 1.0, then this function does
       nothing.

       @return
       TRUE if requires v1.1 chunked transfer encoding.
     */
    BOOL StartResponse(
      StatusCode code,      ///< Status code for the response.
      PMIMEInfo & headers,  ///< MIME variables included in response.
      long bodySize         ///< Size of the rest of the response.
    );

    /** Write an error response for the specified code.

       Depending on the <CODE>code</CODE> parameter this function will also
       send a HTML version of the status code for display on the remote client
       viewer.

       @return
       TRUE if the connection may persist, FALSE if the connection must close
     */
    virtual BOOL OnError(
      StatusCode code,                         ///< Status code for the error response.
      const PCaselessString & extra,           ///< Extra information included in the response.
      const PHTTPConnectionInfo & connectInfo  ///< HTTP connection information
    );

    /** Set the default mime info
     */
    void SetDefaultMIMEInfo(
      PMIMEInfo & info,      ///< Extra MIME information in command.
      const PHTTPConnectionInfo & connectInfo
    );

    /**Get the connection info for this connection.
      */
    PHTTPConnectionInfo & GetConnectionInfo() { return connectInfo; }

  protected:
    void Construct();

    PHTTPSpace          urlSpace;
    PHTTPConnectionInfo connectInfo;
    unsigned            transactionCount;
    PTimeInterval       nextTimeout;
};


//////////////////////////////////////////////////////////////////////////////
// PHTTPRequest

/** This object describes a HyperText Transport Protocol request. An individual
   request is passed to handler functions on #PHTTPResource# descendant
   classes.
 */
class PHTTPRequest : public PObject
{
  PCLASSINFO(PHTTPRequest, PObject)

  public:
    PHTTPRequest(
      const PURL & url,             ///< Universal Resource Locator for document.
      const PMIMEInfo & inMIME,     ///< Extra MIME information in command.
      const PMultipartFormInfoArray & multipartFormInfo, ///< multipart form information (if any)
      PHTTPServer & server          ///< Server channel that request initiated on
    );

    PHTTPServer & server;           ///< Server channel that request initiated on
    const PURL & url;               ///< Universal Resource Locator for document.
    const PMIMEInfo & inMIME;       ///< Extra MIME information in command.
    const PMultipartFormInfoArray & multipartFormInfo; ///< multipart form information, if any
    PHTTP::StatusCode code;         ///< Status code for OnError() reply.
    PMIMEInfo outMIME;              ///< MIME information used in reply.
    PString entityBody;             ///< original entity body (POST only)
    PINDEX contentSize;             ///< Size of the body of the resource data.
    PIPSocket::Address origin;      ///< IP address of origin host for request
    PIPSocket::Address localAddr;   ///< IP address of local interface for request
    WORD               localPort;   ///< Port number of local server for request
};


//////////////////////////////////////////////////////////////////////////////
// PHTTPAuthority

/** This abstract class describes the authorisation mechanism for a Universal
   Resource Locator.
 */
class PHTTPAuthority : public PObject
{
  PCLASSINFO(PHTTPAuthority, PObject)

  public:
  // New functions for class.
    /** Get the realm or name space for the user authorisation name and
       password as required by the basic authorisation system of HTTP/1.0.

       @return
       String for the authorisation realm name.
     */
    virtual PString GetRealm(
      const PHTTPRequest & request   ///< Request information.
    ) const = 0;

    /** Validate the user and password provided by the remote HTTP client for
       the realm specified by the class instance.

       @return
       TRUE if the user and password are authorised in the realm.
     */
    virtual BOOL Validate(
      const PHTTPRequest & request,  ///< Request information.
      const PString & authInfo       ///< Authority information string.
    ) const = 0;

    /** Determine if the authorisation is to be applied. This could be used to
       distinguish between net requiring authorisation and requiring autorisation
       but having no password.

       The default behaviour is to return TRUE.

       @return
       TRUE if the authorisation in the realm is to be applied.
     */
    virtual BOOL IsActive() const;

  protected:
    static void DecodeBasicAuthority(
      const PString & authInfo,   ///< Authority information string.
      PString & username,         ///< User name decoded from authInfo
      PString & password          ///< Password decoded from authInfo
    );
};


//////////////////////////////////////////////////////////////////////////////
// PHTTPSimpleAuth

/** This class describes the simplest authorisation mechanism for a Universal
   Resource Locator, a fixed realm, username and password.
 */
class PHTTPSimpleAuth : public PHTTPAuthority
{
  PCLASSINFO(PHTTPSimpleAuth, PHTTPAuthority)

  public:
    PHTTPSimpleAuth(
      const PString & realm,      ///< Name space for the username and password.
      const PString & username,   ///< Username that this object wiull authorise.
      const PString & password    ///< Password for the above username.
    );
    // Construct the simple authorisation structure.


  // Overrides from class PObject.
    /** Create a copy of the class on the heap. This is used by the
       #PHTTPResource# classes for maintaining authorisation to
       resources.

       @return
       pointer to new copy of the class instance.
     */
    virtual PObject * Clone() const;


  // Overrides from class PHTTPAuthority.
    /** Get the realm or name space for the user authorisation name and
       password as required by the basic authorisation system of HTTP/1.0.

       @return
       String for the authorisation realm name.
     */
    virtual PString GetRealm(
      const PHTTPRequest & request   ///< Request information.
    ) const;

    /** Validate the user and password provided by the remote HTTP client for
       the realm specified by the class instance.

       @return
       TRUE if the user and password are authorised in the realm.
     */
    virtual BOOL Validate(
      const PHTTPRequest & request,  ///< Request information.
      const PString & authInfo       ///< Authority information string.
    ) const;

    /** Determine if the authorisation is to be applied. This could be used to
       distinguish between net requiring authorisation and requiring autorisation
       but having no password.

       The default behaviour is to return TRUE.

       @return
       TRUE if the authorisation in the realm is to be applied.
     */
    virtual BOOL IsActive() const;

    /** Get the user name allocated to this simple authorisation.

       @return
       String for the authorisation user name.
     */
    const PString & GetUserName() const { return username; }

    /** Get the password allocated to this simple authorisation.

       @return
       String for the authorisation password.
     */
    const PString & GetPassword() const { return password; }


  protected:
    PString realm;
    PString username;
    PString password;
};


//////////////////////////////////////////////////////////////////////////////
// PHTTPMultiSimpAuth

/** This class describes the simple authorisation mechanism for a Universal
   Resource Locator, a fixed realm, multiple usernames and passwords.
 */
class PHTTPMultiSimpAuth : public PHTTPAuthority
{
  PCLASSINFO(PHTTPMultiSimpAuth, PHTTPAuthority)

  public:
    PHTTPMultiSimpAuth(
      const PString & realm      ///< Name space for the username and password.
    );
    PHTTPMultiSimpAuth(
      const PString & realm,           ///< Name space for the usernames.
      const PStringToString & userList ///< List of usernames and passwords.
    );
    // Construct the simple authorisation structure.


  // Overrides from class PObject.
    /** Create a copy of the class on the heap. This is used by the
       #PHTTPResource# classes for maintaining authorisation to
       resources.

       @return
       pointer to new copy of the class instance.
     */
    virtual PObject * Clone() const;


  // Overrides from class PHTTPAuthority.
    /** Get the realm or name space for the user authorisation name and
       password as required by the basic authorisation system of HTTP/1.0.

       @return
       String for the authorisation realm name.
     */
    virtual PString GetRealm(
      const PHTTPRequest & request   ///< Request information.
    ) const;

    /** Validate the user and password provided by the remote HTTP client for
       the realm specified by the class instance.

       @return
       TRUE if the user and password are authorised in the realm.
     */
    virtual BOOL Validate(
      const PHTTPRequest & request,  ///< Request information.
      const PString & authInfo       ///< Authority information string.
    ) const;

    /** Determine if the authirisation is to be applied. This could be used to
       distinguish between net requiring authorisation and requiring autorisation
       but having no password.

       The default behaviour is to return TRUE.

       @return
       TRUE if the authorisation in the realm is to be applied.
     */
    virtual BOOL IsActive() const;

    /** Get the user name allocated to this simple authorisation.

       @return
       String for the authorisation user name.
     */
    void AddUser(
      const PString & username,   ///< Username that this object wiull authorise.
      const PString & password    ///< Password for the above username.
    );


  protected:
    PString realm;
    PStringToString users;
};


//////////////////////////////////////////////////////////////////////////////
// PHTTPResource

/** This object describes a HyperText Transport Protocol resource. A tree of
   these resources are available to the #PHTTPSocket# class.
 */
class PHTTPResource : public PObject
{
  PCLASSINFO(PHTTPResource, PObject)

  protected:
    PHTTPResource(
      const PURL & url               ///< Name of the resource in URL space.
    );
    PHTTPResource(
      const PURL & url,              ///< Name of the resource in URL space.
      const PHTTPAuthority & auth    ///< Authorisation for the resource.
    );
    PHTTPResource(
      const PURL & url,              ///< Name of the resource in URL space.
      const PString & contentType    ///< MIME content type for the resource.
    );
    PHTTPResource(
      const PURL & url,              ///< Name of the resource in URL space.
      const PString & contentType,   ///< MIME content type for the resource.
      const PHTTPAuthority & auth    ///< Authorisation for the resource.
    );
    // Create a new HTTP Resource.


  public:
    virtual ~PHTTPResource();
    // Destroy the HTTP Resource.


  // New functions for class.
    /** Get the URL for this resource.

       @return
       The URL for this resource.
     */
    const PURL & GetURL() const { return baseURL; }

    /** Get the current content type for the resource.

       @return
       string for the current MIME content type.
     */
    const PString & GetContentType() const { return contentType; }

    /** Get the current authority for the resource.

       @return
       Pointer to authority or NULL if unrestricted.
     */

    PHTTPAuthority * GetAuthority() const { return authority; }

    /** Set the current authority for the resource.
     */
    void SetAuthority(
      const PHTTPAuthority & auth      ///< authority to set
    );

    /** Set the current authority for the resource to unrestricted.
     */
    void ClearAuthority();

    /** Get the current hit count for the resource. This is the total number of
       times the resource was asked for by a remote client.

       @return
       Hit count for the resource.
     */
    DWORD GetHitCount() const { return hitCount; }

    void ClearHitCount() { hitCount = 0; }
    // Clear the hit count for the resource.


    /** Handle the GET command passed from the HTTP socket.

       The default action is to check the authorisation for the resource and
       call the virtuals #LoadHeaders()# and #OnGETData()# to get
       a resource to be sent to the socket.

       @return
       TRUE if the connection may persist, FALSE if the connection must close.
       If there is no ContentLength field in the response, this value must
       be FALSE for correct operation.
     */
    virtual BOOL OnGET(
      PHTTPServer & server,       ///< HTTP server that received the request
      const PURL & url,           ///< Universal Resource Locator for document.
      const PMIMEInfo & info,     ///< Extra MIME information in command.
      const PHTTPConnectionInfo & conInfo   ///< HTTP connection information
    );

    /**Send the data associated with a GET command.

       The default action calls #SendData()#.

       @return
       TRUE if the connection may persist, FALSE if the connection must close.
       If there is no ContentLength field in the response, this value must
       be FALSE for correct operation.
    */
    virtual BOOL OnGETData(
      PHTTPServer & server,                       ///< HTTP server that received the request
      const PURL & url,                           ///< Universal Resource Locator for document
      const PHTTPConnectionInfo & connectInfo,    ///< HTTP connection information
      PHTTPRequest & request                      ///< request state information
    );

    /** Handle the HEAD command passed from the HTTP socket.

       The default action is to check the authorisation for the resource and
       call the virtual #LoadHeaders()# to get the header information to
       be sent to the socket.

       @return
       TRUE if the connection may persist, FALSE if the connection must close
       If there is no ContentLength field in the response, this value must
       be FALSE for correct operation.
     */
    virtual BOOL OnHEAD(
      PHTTPServer & server,       ///< HTTP server that received the request
      const PURL & url,           ///< Universal Resource Locator for document.
      const PMIMEInfo & info,     ///< Extra MIME information in command.
      const PHTTPConnectionInfo & conInfo  ///< HTTP connection information
    );

    /** Handle the POST command passed from the HTTP socket.

       The default action is to check the authorisation for the resource and
       call the virtual #Post()# function to handle the data being
       received.

       @return
       TRUE if the connection may persist, FALSE if the connection must close
       If there is no ContentLength field in the response, this value must
       be FALSE for correct operation.
     */
    virtual BOOL OnPOST(
      PHTTPServer & server,         ///< HTTP server that received the request
      const PURL & url,             ///< Universal Resource Locator for document.
      const PMIMEInfo & info,       ///< Extra MIME information in command.
      const PStringToString & data, ///< Variables in the POST data.
      const PHTTPConnectionInfo & conInfo  ///< HTTP connection information
    );

    /**Send the data associated with a POST command.

       The default action calls #Post()#.

       @return
       TRUE if the connection may persist, FALSE if the connection must close.
       If there is no ContentLength field in the response, this value must
       be FALSE for correct operation.
    */
    virtual BOOL OnPOSTData(
      PHTTPRequest & request,        ///< request information
      const PStringToString & data   ///< Variables in the POST data.
    );

    /** Check to see if the resource has been modified since the date
       specified.

       @return
       TRUE if has been modified since.
     */
    virtual BOOL IsModifiedSince(
      const PTime & when    ///< Time to see if modified later than
    );

    /** Get a block of data (eg HTML) that the resource contains.

       @return
       Status of load operation.
     */
    virtual BOOL GetExpirationDate(
      PTime & when          ///< Time that the resource expires
    );

    /** Create a new request block for this type of resource.

       The default behaviour is to create a new PHTTPRequest instance.

       @return
       Pointer to instance of PHTTPRequest descendant class.
     */
    virtual PHTTPRequest * CreateRequest(
      const PURL & url,                   ///< Universal Resource Locator for document.
      const PMIMEInfo & inMIME,           ///< Extra MIME information in command.
      const PMultipartFormInfoArray & multipartFormInfo,  ///< additional information for multi-part posts
      PHTTPServer & socket                                ///< socket used for request
    );

    /** Get the headers for block of data (eg HTML) that the resource contains.
       This will fill in all the fields of the <CODE>outMIME</CODE> parameter
       required by the resource and return the status for the load.

       @return
       TRUE if all OK, FALSE if an error occurred.
     */
    virtual BOOL LoadHeaders(
      PHTTPRequest & request    ///<  Information on this request.
    ) = 0;

    /**Send the data associated with a command.

       The default action is to call the virtual #LoadData()# to get a
       resource to be sent to the socket.
    */
    virtual void SendData(
      PHTTPRequest & request    ///< information for this request
    );

    /** Get a block of data that the resource contains.

       The default behaviour is to call the #LoadText()# function and
       if successful, call the #OnLoadedText()# function.

       @return
       TRUE if there is still more to load.
     */
    virtual BOOL LoadData(
      PHTTPRequest & request,    ///<  Information on this request.
      PCharArray & data          ///<  Data used in reply.
    );

    /** Get a block of text data (eg HTML) that the resource contains.

       The default behaviour is to assert, one of #LoadText()# or
       #LoadData()# functions must be overridden for correct operation.

       @return
       String for loaded text.
     */
    virtual PString LoadText(
      PHTTPRequest & request    ///< Information on this request.
    );

    /** This is called after the text has been loaded and may be used to
       customise or otherwise mangle a loaded piece of text. Typically this is
       used with HTML responses.

       The default action for this function is to do nothing.
     */
    virtual void OnLoadedText(
      PHTTPRequest & request,    ///<  Information on this request.
      PString & text             ///<  Data used in reply.
    );

    /** Get a block of data (eg HTML) that the resource contains.

       The default action for this function is to do nothing and return
       success.

       @return
       TRUE if the connection may persist, FALSE if the connection must close
     */
    virtual BOOL Post(
      PHTTPRequest & request,       ///<  Information on this request.
      const PStringToString & data, ///<  Variables in the POST data.
      PHTML & replyMessage          ///<  Reply message for post.
    );


  protected:
    /** See if the resource is authorised given the mime info
     */
    virtual BOOL CheckAuthority(
      PHTTPServer & server,               ///<  Server to send response to.
      const PHTTPRequest & request,       ///<  Information on this request.
      const PHTTPConnectionInfo & conInfo ///<  Information on the connection
    );
    static BOOL CheckAuthority(
                   PHTTPAuthority & authority,
                      PHTTPServer & server,
               const PHTTPRequest & request,
        const PHTTPConnectionInfo & connectInfo
    );


    /** common code for GET and HEAD commands */
    virtual BOOL OnGETOrHEAD(
      PHTTPServer & server,       ///<  HTTP server that received the request
      const PURL & url,           ///<  Universal Resource Locator for document.
      const PMIMEInfo & info,     ///<  Extra MIME information in command.
      const PHTTPConnectionInfo & conInfo,
      BOOL  IsGet
    );

    /// Base URL for the resource, may accept URLS with a longer hierarchy
    PURL             baseURL;
    /// MIME content type for the resource
    PString          contentType;
    /// Authorisation method for the resource
    PHTTPAuthority * authority;
    /// COunt of number of times resource was accessed.
    volatile DWORD   hitCount;
};


//////////////////////////////////////////////////////////////////////////////
// PHTTPString

/** This object describes a HyperText Transport Protocol resource which is a
   string kept in memory. For instance a pre-calculated HTML string could be
   set in this type of resource.
 */
class PHTTPString : public PHTTPResource
{
  PCLASSINFO(PHTTPString, PHTTPResource)

  public:
    /** Contruct a new simple string resource for the HTTP space. If no MIME
       content type is specified then a default type is "text/html".
     */
    PHTTPString(
      const PURL & url             // Name of the resource in URL space.
    );
    PHTTPString(
      const PURL & url,            // Name of the resource in URL space.
      const PHTTPAuthority & auth  // Authorisation for the resource.
    );
    PHTTPString(
      const PURL & url,            // Name of the resource in URL space.
      const PString & str          // String to return in this resource.
    );
    PHTTPString(
      const PURL & url,            // Name of the resource in URL space.
      const PString & str,         // String to return in this resource.
      const PString & contentType  // MIME content type for the file.
    );
    PHTTPString(
      const PURL & url,            // Name of the resource in URL space.
      const PString & str,         // String to return in this resource.
      const PHTTPAuthority & auth  // Authorisation for the resource.
    );
    PHTTPString(
      const PURL & url,            // Name of the resource in URL space.
      const PString & str,         // String to return in this resource.
      const PString & contentType, // MIME content type for the file.
      const PHTTPAuthority & auth  // Authorisation for the resource.
    );


  // Overrides from class PHTTPResource
    /** Get the headers for block of data (eg HTML) that the resource contains.
       This will fill in all the fields of the <CODE>outMIME</CODE> parameter
       required by the resource and return the status for the load.

       @return
       TRUE if all OK, FALSE if an error occurred.
     */
    virtual BOOL LoadHeaders(
      PHTTPRequest & request    // Information on this request.
    );

    /** Get a block of text data (eg HTML) that the resource contains.

       The default behaviour is to assert, one of #LoadText()# or
       #LoadData()# functions must be overridden for correct operation.

       @return
       String for loaded text.
     */
    virtual PString LoadText(
      PHTTPRequest & request    // Information on this request.
    );

  // New functions for class.
    /** Get the string for this resource.

       @return
       String for resource.
     */
    const PString & GetString() { return string; }

    /** Set the string to be returned by this resource.
     */
    void SetString(
      const PString & str   // New string for the resource.
    ) { string = str; }


  protected:
    PString string;
};


//////////////////////////////////////////////////////////////////////////////
// PHTTPFile

/** This object describes a HyperText Transport Protocol resource which is a
   single file. The file can be anywhere in the file system and is mapped to
   the specified URL location in the HTTP name space defined by the
   #PHTTPSpace# class.
 */
class PHTTPFile : public PHTTPResource
{
  PCLASSINFO(PHTTPFile, PHTTPResource)

  public:
    /** Contruct a new simple file resource for the HTTP space. If no MIME
       content type is specified then a default type is used depending on the
       file type. For example, "text/html" is used of the file type is
       ".html" or ".htm". The default for an unknown type is
       "application/octet-stream".
     */
    PHTTPFile(
      const PString & filename     // file in file system and URL name.
    );
    PHTTPFile(
      const PString & filename,    // file in file system and URL name.
      const PHTTPAuthority & auth  // Authorisation for the resource.
    );
    PHTTPFile(
      const PURL & url,            // Name of the resource in URL space.
      const PFilePath & file       // Location of file in file system.
    );
    PHTTPFile(
      const PURL & url,            // Name of the resource in URL space.
      const PFilePath & file,      // Location of file in file system.
      const PString & contentType  // MIME content type for the file.
    );
    PHTTPFile(
      const PURL & url,            // Name of the resource in URL space.
      const PFilePath & file,      // Location of file in file system.
      const PHTTPAuthority & auth  // Authorisation for the resource.
    );
    PHTTPFile(
      const PURL & url,            // Name of the resource in URL space.
      const PFilePath & file,      // Location of file in file system.
      const PString & contentType, // MIME content type for the file.
      const PHTTPAuthority & auth  // Authorisation for the resource.
    );


  // Overrides from class PHTTPResource
    /** Create a new request block for this type of resource.

       @return
       Pointer to instance of PHTTPRequest descendant class.
     */
    virtual PHTTPRequest * CreateRequest(
      const PURL & url,                  // Universal Resource Locator for document.
      const PMIMEInfo & inMIME,          // Extra MIME information in command.
      const PMultipartFormInfoArray & multipartFormInfo,
      PHTTPServer & socket
    );

    /** Get the headers for block of data (eg HTML) that the resource contains.
       This will fill in all the fields of the <CODE>outMIME</CODE> parameter
       required by the resource and return the status for the load.

       @return
       TRUE if all OK, FALSE if an error occurred.
     */
    virtual BOOL LoadHeaders(
      PHTTPRequest & request    // Information on this request.
    );

    /** Get a block of data that the resource contains.

       @return
       TRUE if more to load.
     */
    virtual BOOL LoadData(
      PHTTPRequest & request,    // Information on this request.
      PCharArray & data          // Data used in reply.
    );

    /** Get a block of text data (eg HTML) that the resource contains.

       The default behaviour is to assert, one of #LoadText()# or
       #LoadData()# functions must be overridden for correct operation.

       @return
       String for loaded text.
     */
    virtual PString LoadText(
      PHTTPRequest & request    // Information on this request.
    );


  protected:
    PHTTPFile(
      const PURL & url,       // Name of the resource in URL space.
      int dummy
    );
    // Constructor used by PHTTPDirectory


    PFilePath filePath;
};


class PHTTPFileRequest : public PHTTPRequest
{
  PCLASSINFO(PHTTPFileRequest, PHTTPRequest)
  public:
    PHTTPFileRequest(
      const PURL & url,             // Universal Resource Locator for document.
      const PMIMEInfo & inMIME,     // Extra MIME information in command.
      const PMultipartFormInfoArray & multipartFormInfo,
      PHTTPServer & server
    );

    PFile file;
};


//////////////////////////////////////////////////////////////////////////////
// PHTTPTailFile

/** This object describes a HyperText Transport Protocol resource which is a
   single file. The file can be anywhere in the file system and is mapped to
   the specified URL location in the HTTP name space defined by the
   #PHTTPSpace# class.

   The difference between this and PHTTPFile is that it continually outputs
   the contents of the file, as per the unix "tail -f" command.
 */
class PHTTPTailFile : public PHTTPFile
{
  PCLASSINFO(PHTTPTailFile, PHTTPFile)

  public:
    /** Contruct a new simple file resource for the HTTP space. If no MIME
       content type is specified then a default type is used depending on the
       file type. For example, "text/html" is used of the file type is
       ".html" or ".htm". The default for an unknown type is
       "application/octet-stream".
     */
    PHTTPTailFile(
      const PString & filename     // file in file system and URL name.
    );
    PHTTPTailFile(
      const PString & filename,    // file in file system and URL name.
      const PHTTPAuthority & auth  // Authorisation for the resource.
    );
    PHTTPTailFile(
      const PURL & url,            // Name of the resource in URL space.
      const PFilePath & file       // Location of file in file system.
    );
    PHTTPTailFile(
      const PURL & url,            // Name of the resource in URL space.
      const PFilePath & file,      // Location of file in file system.
      const PString & contentType  // MIME content type for the file.
    );
    PHTTPTailFile(
      const PURL & url,            // Name of the resource in URL space.
      const PFilePath & file,      // Location of file in file system.
      const PHTTPAuthority & auth  // Authorisation for the resource.
    );
    PHTTPTailFile(
      const PURL & url,            // Name of the resource in URL space.
      const PFilePath & file,      // Location of file in file system.
      const PString & contentType, // MIME content type for the file.
      const PHTTPAuthority & auth  // Authorisation for the resource.
    );


  // Overrides from class PHTTPResource
    /** Get the headers for block of data (eg HTML) that the resource contains.
       This will fill in all the fields of the <CODE>outMIME</CODE> parameter
       required by the resource and return the status for the load.

       @return
       TRUE if all OK, FALSE if an error occurred.
     */
    virtual BOOL LoadHeaders(
      PHTTPRequest & request    // Information on this request.
    );

    /** Get a block of data that the resource contains.

       @return
       TRUE if more to load.
     */
    virtual BOOL LoadData(
      PHTTPRequest & request,    // Information on this request.
      PCharArray & data          // Data used in reply.
    );
};


//////////////////////////////////////////////////////////////////////////////
// PHTTPDirectory

/** This object describes a HyperText Transport Protocol resource which is a
   set of files in a directory. The directory can be anywhere in the file
   system and is mapped to the specified URL location in the HTTP name space
   defined by the #PHTTPSpace# class.

   All subdirectories and files are available as URL names in the HTTP name
   space. This effectively grafts a file system directory tree onto the URL
   name space tree.

   See the #PMIMEInfo# class for more information on the mappings between
   file types and MIME types.
 */
class PHTTPDirectory : public PHTTPFile
{
  PCLASSINFO(PHTTPDirectory, PHTTPFile)

  public:
    PHTTPDirectory(
      const PURL & url,            /// Name of the resource in URL space.
      const PDirectory & dir       /// Location of file in file system.
    );
    PHTTPDirectory(
      const PURL & url,            /// Name of the resource in URL space.
      const PDirectory & dir,      /// Location of file in file system.
      const PHTTPAuthority & auth  /// Authorisation for the resource.
    );
    // Construct a new directory resource for HTTP.


  // Overrides from class PHTTPResource
    /** Create a new request block for this type of resource.

       @return
       Pointer to instance of PHTTPRequest descendant class.
     */
    virtual PHTTPRequest * CreateRequest(
      const PURL & url,                  // Universal Resource Locator for document.
      const PMIMEInfo & inMIME,          // Extra MIME information in command.
      const PMultipartFormInfoArray & multipartFormInfo,
      PHTTPServer & socket
    );

    /** Get the headers for block of data (eg HTML) that the resource contains.
       This will fill in all the fields of the <CODE>outMIME</CODE> parameter
       required by the resource and return the status for the load.

       @return
       TRUE if all OK, FALSE if an error occurred.
     */
    virtual BOOL LoadHeaders(
      PHTTPRequest & request    /// Information on this request.
    );

    /** Get a block of text data (eg HTML) that the resource contains.

       The default behaviour is to assert, one of #LoadText()# or
       #LoadData()# functions must be overridden for correct operation.

       @return
       String for loaded text.
     */
    virtual PString LoadText(
      PHTTPRequest & request    /// Information on this request.
    );

    /** Enable or disable access control using .access files. A directory tree containing
       a _access file will require authorisation to allow access. This file has 
       contains one or more lines, each containing a username and password seperated 
       by a ":" character.

       The parameter sets the realm used for authorisation requests. An empty realm disables
       auhtorisation.
     */
    void EnableAuthorisation(const PString & realm);

    /** Enable or disable directory listings when a default directory file does not exist
     */
    void AllowDirectories(BOOL enable = TRUE);

  protected:
    BOOL CheckAuthority(
      PHTTPServer & server,               // Server to send response to.
      const PHTTPRequest & request,       // Information on this request.
      const PHTTPConnectionInfo & conInfo // Information on the connection
    );

    BOOL FindAuthorisations(const PDirectory & dir, PString & realm, PStringToString & authorisations);

    PDirectory basePath;
    PString authorisationRealm;
    BOOL allowDirectoryListing;
};


class PHTTPDirRequest : public PHTTPFileRequest
{
  PCLASSINFO(PHTTPDirRequest, PHTTPFileRequest)
  public:
    PHTTPDirRequest(
      const PURL & url,             // Universal Resource Locator for document.
      const PMIMEInfo & inMIME,     // Extra MIME information in command.
      const PMultipartFormInfoArray & multipartFormInfo, 
      PHTTPServer & server
    );

    PString fakeIndex;
    PFilePath realPath;
};

#endif // P_HTTPSVC

#endif


// End Of File ///////////////////////////////////////////////////////////////
