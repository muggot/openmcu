/*
 * http.cxx
 *
 * HTTP ancestor class and common classes.
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
 * $Log: http.cxx,v $
 * Revision 1.127  2007/10/03 01:18:46  rjongbloed
 * Fixed build for Windows Mobile 5 and added Windows Mobile 6
 *
 * Revision 1.126  2007/06/29 02:47:28  rjongbloed
 * Added PString::FindSpan() function (strspn equivalent) with slightly nicer semantics.
 *
 * Revision 1.125  2007/04/20 07:42:12  csoutheren
 * Applied 1703668 - Make RTSP URLs accept the Query component
 * Thanks to Fabrizio Ammollo
 *
 * Revision 1.124  2007/04/10 05:08:48  rjongbloed
 * Fixed issue with use of static C string variables in DLL environment,
 *   must use functional interface for correct initialisation.
 *
 * Revision 1.123  2007/04/08 01:53:28  ykiryanov
 * Build to support ptlib dll creation
 *
 * Revision 1.122  2006/08/29 01:33:25  csoutheren
 * RFC 3261 specifies use of sips for secure SIP, so change URL parsing back...
 *
 * Revision 1.121  2006/08/21 05:23:13  csoutheren
 * Add ssip and change h323s to sh323
 *
 * Revision 1.120  2006/08/21 01:31:03  csoutheren
 * Add support h323s URLs
 *
 * Revision 1.119  2006/07/14 00:57:37  csoutheren
 * Be safe for URLs containing non-ASCII characters
 *
 * Revision 1.118  2006/06/25 11:22:57  csoutheren
 * Add pragmas to automate linking with VS 2005
 *
 * Revision 1.117  2006/06/21 03:28:44  csoutheren
 * Various cleanups thanks for Frederic Heem
 *
 * Revision 1.116  2005/11/30 12:47:41  csoutheren
 * Removed tabs, reformatted some code, and changed tags for Doxygen
 *
 * Revision 1.115  2005/04/20 05:19:48  csoutheren
 * Patch 1185334. Ensure SIP URLs correctly store status of port
 * Thanks to Ted Szoczei
 *
 * Revision 1.114  2005/01/16 20:36:48  csoutheren
 * Changed URLS to put IP address in [] if contains a ":"
 *
 * Revision 1.113  2005/01/04 08:09:41  csoutheren
 * Fixed Linux configure problems
 *
 * Revision 1.112  2004/12/08 00:51:12  csoutheren
 * Move PURLLegacyScheme to header file to allow external usage
 *
 * Revision 1.111  2004/10/23 11:27:24  ykiryanov
 * Added ifdef _WIN32_WCE for PocketPC 2003 SDK port
 *
 * Revision 1.110  2004/08/31 23:40:51  csoutheren
 * Fixed problem with absolute file paths in URLs
 *
 * Revision 1.109  2004/07/14 13:15:45  rjongbloed
 * Fixed minor bug where a URL is "non-empty" if requires a host but has none.
 *   eg could end up with "sip:" or "http://:80" which are illegal.
 *
 * Revision 1.108  2004/07/12 09:17:20  csoutheren
 * Fixed warnings and errors under Linux
 *
 * Revision 1.107  2004/07/06 10:12:52  csoutheren
 * Added static integer o factory template to assist in ensuring factories are instantiated
 *
 * Revision 1.106  2004/06/30 12:17:05  rjongbloed
 * Rewrite of plug in system to use single global variable for all factories to avoid all sorts
 *   of issues with startup orders and Windows DLL multiple instances.
 *
 * Revision 1.105  2004/06/16 07:48:12  csoutheren
 * Added assert to clarify usage of default scheme
 *
 * Revision 1.104  2004/06/03 13:30:58  csoutheren
 * Renamed INSTANTIATE_FACTORY to avoid potential namespace collisions
 * Added documentaton on new PINSTANTIATE_FACTORY macro
 * Added generic form of PINSTANTIATE_FACTORY
 *
 * Revision 1.103  2004/06/03 12:47:58  csoutheren
 * Decomposed PFactory declarations to hopefully avoid problems with Windows DLLs
 *
 * Revision 1.102  2004/06/01 07:28:45  csoutheren
 * Changed URL parsing to use abstract factory code
 *
 * Revision 1.101  2004/04/04 00:21:47  csoutheren
 * FIxed problem with some URL parsing
 *
 * Revision 1.100  2004/04/03 08:22:20  csoutheren
 * Remove pseudo-RTTI and replaced with real RTTI
 *
 * Revision 1.99  2004/03/23 05:08:21  csoutheren
 * Fixed problem with use of ShellExecuteEx function
 *
 * Revision 1.98  2004/03/13 06:30:52  rjongbloed
 * Virtualised parse function.
 *
 * Revision 1.97  2004/02/24 11:14:44  rjongbloed
 * Fixed correct initialisation of some internal variables in URL if parse fails.
 *
 * Revision 1.96  2004/01/17 17:44:17  csoutheren
 * Changed to use PString::MakeEmpty
 *
 * Revision 1.95  2003/11/18 09:22:17  csoutheren
 * Fixed problems with PURL::OpenBrowser, thanks to David Parr
 *
 * Revision 1.94  2003/08/27 03:37:45  dereksmithies
 * Fix initialization of pathStr so it really is empty. BIG thanks to Diego Tartara.
 *
 * Revision 1.93  2003/07/22 03:26:10  csoutheren
 * Fixed problem with parsing default H323 addresses
 *
 * Revision 1.92  2003/06/23 15:31:40  ykiryanov
 * Slightly changed call to ShellExecuteEx to make compatible with Win32
 *
 * Revision 1.91  2003/06/23 14:31:33  ykiryanov
 * Modified for WinCE - used ShellExecuteEx instead of ShellExecute
 *
 * Revision 1.90  2003/06/05 00:15:54  rjongbloed
 * Fixed callto bug created by previous patch.
 *
 * Revision 1.89  2003/06/04 01:42:05  rjongbloed
 * Fixed h323 scheme, does not have a "password" field.
 *
 * Revision 1.88  2003/06/02 02:46:45  rjongbloed
 * Fixed issue with callto URL parsing incorrect username field.
 * Added automatic removal of illegal (though common) "//" in callto URL.
 *
 * Revision 1.87  2003/05/05 07:30:17  craigs
 * Fixed problem with URLs that do not specify schemes
 *
 * Revision 1.86  2003/05/02 13:50:23  craigs
 * Fixed a problem with callto:localhost
 *
 * Revision 1.85  2003/05/02 13:20:33  craigs
 * Fixed callto problems
 *
 * Revision 1.84  2003/04/28 04:41:22  robertj
 * Changed URL parsing so if a default scheme is present then explicit scheme
 *   must be "known" to avoid ambiguity with host:port parsing.
 *
 * Revision 1.83  2003/04/10 00:13:56  robertj
 * Fixed correct decoding of user/password/host/port field, for non h323 schemes.
 *
 * Revision 1.82  2003/04/08 06:28:14  craigs
 * Fixed introduced problem with HTTP server mistaking relative URLs for proxy requests
 *
 * Revision 1.81  2003/04/04 08:03:55  robertj
 * Fixed special case of h323 URL default port changing depending on
 *   if it the host is an endpoint or gatekeeper.
 *
 * Revision 1.80  2003/04/04 05:18:08  robertj
 * Added "callto", "tel" and fixed "h323" URL types.
 *
 * Revision 1.79  2002/12/02 00:17:03  robertj
 * Fixed URL parsing/display problems with non-path URL type eg mailto
 *
 * Revision 1.78  2002/11/22 06:16:49  robertj
 * Fixed usage of URI (relative http/https URL).
 *
 * Revision 1.77  2002/11/20 02:10:56  robertj
 * Fixed some more realtive/absolute path issues.
 *
 * Revision 1.76  2002/11/20 01:01:49  robertj
 * Fixed GNU compatibility
 *
 * Revision 1.75  2002/11/20 00:49:37  robertj
 * Fixed correct interpretation of url re double slashes as per latest RFC,
 *   including file: mapping and relative paths. Probably still more to do.
 *
 * Revision 1.74  2002/11/19 22:45:03  robertj
 * Fixed support for file: scheme under unix
 *
 * Revision 1.73  2002/11/19 10:36:50  robertj
 * Added functions to set anf get "file:" URL. as PFilePath and do the right
 *   things with platform dependent directory components.
 *
 * Revision 1.72  2002/11/06 22:47:25  robertj
 * Fixed header comment (copyright etc)
 *
 * Revision 1.71  2002/09/23 07:17:24  robertj
 * Changes to allow winsock2 to be included.
 *
 * Revision 1.70  2002/08/28 08:06:11  craigs
 * Fixed problem (again) with file:// URLs
 *
 * Revision 1.69  2002/08/28 05:11:23  craigs
 * Fixed problem with file:// URLs
 *
 * Revision 1.68  2002/05/02 05:11:29  craigs
 * Fixed problem with not translating + chars in URL query parameters
 *
 * Revision 1.67  2002/03/19 23:39:57  robertj
 * Fixed string output to include PathOnly variant, lost in previous mod.
 *
 * Revision 1.66  2002/03/19 23:24:08  robertj
 * Fixed problems with backward compatibility on parameters processing.
 *
 * Revision 1.65  2002/03/18 05:02:27  robertj
 * Added functions to set component parts of URL.
 * Fixed output of parameters when more than one ';' involved.
 *
 * Revision 1.64  2001/11/09 05:46:14  robertj
 * Removed double slash on sip URL.
 * Fixed extra : if have username but no password.
 * Added h323: scheme
 *
 * Revision 1.63  2001/11/08 00:32:49  robertj
 * Added parsing of ';' based parameter fields into string dictionary if there are multiple parameters, with '=' values.
 *
 * Revision 1.62  2001/10/31 01:33:07  robertj
 * Added extra const for constant HTTP tag name strings.
 *
 * Revision 1.61  2001/10/03 00:26:34  robertj
 * Upgraded client to HTTP/1.1 and for chunked mode entity bodies.
 *
 * Revision 1.60  2001/09/28 00:45:42  robertj
 * Broke out internal static function for unstranslating URL strings.
 *
 * Revision 1.59  2001/07/16 00:43:06  craigs
 * Added ability to parse other transport URLs
 *
 * Revision 1.58  2000/05/02 08:29:07  craigs
 * Removed "memory leaks" caused by brain-dead GNU linker
 *
 * Revision 1.57  1999/05/11 12:24:18  robertj
 * Fixed URL parser so leading blanks are ignored.
 *
 * Revision 1.56  1999/05/04 15:26:01  robertj
 * Improved HTTP/1.1 compatibility (pass through user commands).
 * Fixed problems with quicktime installer.
 *
 * Revision 1.55  1999/04/21 01:56:13  robertj
 * Fixed problem with escape codes greater that %80
 *
 * Revision 1.54  1999/01/16 12:45:54  robertj
 * Added RTSP schemes to URL's
 *
 * Revision 1.53  1998/11/30 05:38:15  robertj
 * Moved PURL::Open() code to .cxx file to avoid linking unused code.
 *
 * Revision 1.52  1998/11/30 04:51:53  robertj
 * New directory structure
 *
 * Revision 1.51  1998/09/23 06:22:07  robertj
 * Added open source copyright license.
 *
 * Revision 1.50  1998/02/03 10:02:34  robertj
 * Added ability to get scheme, host and port from URL as a string.
 *
 * Revision 1.49  1998/02/03 06:27:26  robertj
 * Fixed URL encoding to be closer to RFC
 *
 * Revision 1.48  1998/01/26 02:49:16  robertj
 * GNU support.
 *
 * Revision 1.47  1997/11/10 12:40:20  robertj
 * Fixed illegal character set for URL's.
 *
 * Revision 1.46  1997/07/14 11:47:10  robertj
 * Added "const" to numerous variables.
 *
 * Revision 1.45  1997/07/12 09:45:01  robertj
 * Fixed bug when URL has + sign in somthing other than parameters.
 *
 * Revision 1.44  1997/06/06 08:54:47  robertj
 * Allowed username/password on http scheme URL.
 *
 * Revision 1.43  1997/04/06 07:46:09  robertj
 * Fixed bug where URL has more than special character ('?', '#' etc).
 *
 * Revision 1.42  1997/03/28 04:40:24  robertj
 * Added tags for cookies.
 *
 * Revision 1.41  1997/03/18 22:03:44  robertj
 * Fixed bug that incorrectly parses URL with double slashes.
 *
 * Revision 1.40  1997/02/14 13:55:44  robertj
 * Fixed bug in URL for reproducing fields with special characters, must be escaped and weren't.
 *
 * Revision 1.39  1997/01/12 04:15:21  robertj
 * Globalised MIME tag strings.
 *
 * Revision 1.38  1996/09/14 13:09:28  robertj
 * Major upgrade:
 *   rearranged sockets to help support IPX.
 *   added indirect channel class and moved all protocols to descend from it,
 *   separating the protocol from the low level byte transport.
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

#ifdef __GNUC__
#pragma implementation "http.h"
#pragma implementation "url.h"
#endif

#include <ptlib.h>

#define P_DISABLE_FACTORY_INSTANCES
#include <ptlib/pfactory.h>
#include <ptlib/sockets.h>
#include <ptclib/http.h>
#include <ptclib/url.h>

#include <ctype.h>

#if defined(WIN32) && !defined(_WIN32_WCE)
#include <shellapi.h>
#pragma comment(lib,"shell32.lib")
#endif


// RFC 1738
// http://host:port/path...
// https://host:port/path....
// gopher://host:port
// wais://host:port
// nntp://host:port
// prospero://host:port
// ftp://user:password@host:port/path...
// telnet://user:password@host:port
// file://hostname/path...

// mailto:user@hostname
// news:string

#define DEFAULT_FTP_PORT      21
#define DEFAULT_TELNET_PORT   23
#define DEFAULT_GOPHER_PORT   70
#define DEFAULT_HTTP_PORT     80
#define DEFAULT_NNTP_PORT     119
#define DEFAULT_WAIS_PORT     210
#define DEFAULT_HTTPS_PORT    443
#define DEFAULT_RTSP_PORT     554
#define DEFAULT_RTSPU_PORT    554
#define DEFAULT_PROSPERO_PORT 1525
#define DEFAULT_H323_PORT     1720
#define DEFAULT_H323S_PORT    1300
#define DEFAULT_H323RAS_PORT  1719
#define DEFAULT_SIP_PORT      5060
#define DEFAULT_SIPS_PORT     5061

#define DEFINE_LEGACY_URL_SCHEME(schemeName, user, pass, host, def, defhost, query, params, frags, path, rel, port) \
class PURLLegacyScheme_##schemeName : public PURLLegacyScheme \
{ \
  public: \
    PURLLegacyScheme_##schemeName() \
    : PURLLegacyScheme(#schemeName )  \
    { \
      hasUsername           = user; \
      hasPassword           = pass; \
      hasHostPort           = host; \
      defaultToUserIfNoAt   = def; \
      defaultHostToLocal    = defhost; \
      hasQuery              = query; \
      hasParameters         = params; \
      hasFragments          = frags; \
      hasPath               = path; \
      relativeImpliesScheme = rel; \
      defaultPort           = port; \
    } \
}; \
  static PFactory<PURLScheme>::Worker<PURLLegacyScheme_##schemeName> schemeName##Factory(#schemeName, true); \

DEFINE_LEGACY_URL_SCHEME(http,      TRUE,  TRUE,  TRUE,  FALSE, TRUE,   TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  DEFAULT_HTTP_PORT )
DEFINE_LEGACY_URL_SCHEME(file,      FALSE, FALSE, TRUE,  FALSE, TRUE,   FALSE, FALSE, FALSE, TRUE,  FALSE, 0)
DEFINE_LEGACY_URL_SCHEME(https,     FALSE, FALSE, TRUE,  FALSE, TRUE,   TRUE,  TRUE,  TRUE,  TRUE,  TRUE,  DEFAULT_HTTPS_PORT)
DEFINE_LEGACY_URL_SCHEME(gopher,    FALSE, FALSE, TRUE,  FALSE, TRUE,   FALSE, FALSE, FALSE, TRUE,  FALSE, DEFAULT_GOPHER_PORT)
DEFINE_LEGACY_URL_SCHEME(wais,      FALSE, FALSE, TRUE,  FALSE, FALSE,  FALSE, FALSE, FALSE, TRUE,  FALSE, DEFAULT_WAIS_PORT)
DEFINE_LEGACY_URL_SCHEME(nntp,      FALSE, FALSE, TRUE,  FALSE, TRUE,   FALSE, FALSE, FALSE, TRUE,  FALSE, DEFAULT_NNTP_PORT)
DEFINE_LEGACY_URL_SCHEME(prospero,  FALSE, FALSE, TRUE,  FALSE, TRUE,   FALSE, FALSE, FALSE, TRUE,  FALSE, DEFAULT_PROSPERO_PORT)
DEFINE_LEGACY_URL_SCHEME(rtsp,      FALSE, FALSE, TRUE,  FALSE, TRUE,   TRUE,  FALSE, FALSE, TRUE,  FALSE, DEFAULT_RTSP_PORT)
DEFINE_LEGACY_URL_SCHEME(rtspu,     FALSE, FALSE, TRUE,  FALSE, TRUE,   FALSE, FALSE, FALSE, TRUE,  FALSE, DEFAULT_RTSPU_PORT)
DEFINE_LEGACY_URL_SCHEME(ftp,       TRUE,  TRUE,  TRUE,  FALSE, TRUE,   FALSE, FALSE, FALSE, TRUE,  FALSE, DEFAULT_FTP_PORT)
DEFINE_LEGACY_URL_SCHEME(telnet,    TRUE,  TRUE,  TRUE,  FALSE, TRUE,   FALSE, FALSE, FALSE, FALSE, FALSE, DEFAULT_TELNET_PORT)
DEFINE_LEGACY_URL_SCHEME(mailto,    FALSE, FALSE, FALSE, FALSE, TRUE,   TRUE,  FALSE, FALSE, FALSE, FALSE, 0)
DEFINE_LEGACY_URL_SCHEME(news,      FALSE, FALSE, FALSE, FALSE, TRUE,   FALSE, FALSE, FALSE, FALSE, FALSE, 0)
DEFINE_LEGACY_URL_SCHEME(h323,      TRUE,  FALSE, TRUE,  TRUE,  FALSE,  FALSE, TRUE,  FALSE, FALSE, FALSE, DEFAULT_H323_PORT)
DEFINE_LEGACY_URL_SCHEME(h323s,     TRUE,  FALSE, TRUE,  TRUE,  FALSE,  FALSE, TRUE,  FALSE, FALSE, FALSE, DEFAULT_H323S_PORT)
DEFINE_LEGACY_URL_SCHEME(sip,       TRUE,  TRUE,  TRUE,  FALSE, FALSE,  FALSE, TRUE,  FALSE, FALSE, FALSE, DEFAULT_SIP_PORT)
DEFINE_LEGACY_URL_SCHEME(sips,      TRUE,  TRUE,  TRUE,  FALSE, FALSE,  FALSE, TRUE,  FALSE, FALSE, FALSE, DEFAULT_SIPS_PORT)
DEFINE_LEGACY_URL_SCHEME(tel,       FALSE, FALSE, FALSE, TRUE,  FALSE,  FALSE, TRUE,  FALSE, FALSE, FALSE, 0)
DEFINE_LEGACY_URL_SCHEME(fax,       FALSE, FALSE, FALSE, TRUE,  FALSE,  FALSE, TRUE,  FALSE, FALSE, FALSE, 0)
DEFINE_LEGACY_URL_SCHEME(callto,    FALSE, FALSE, FALSE, TRUE,  FALSE,  FALSE, TRUE,  FALSE, FALSE, FALSE, 0)

PINSTANTIATE_FACTORY(PURLScheme, PString)

#define DEFAULT_SCHEME "http"
#define FILE_SCHEME    "file"

//////////////////////////////////////////////////////////////////////////////
// PURL

PURL::PURL()
  : //scheme(SchemeTable[DEFAULT_SCHEME].name),
    scheme(DEFAULT_SCHEME),
    port(0),
    portSupplied (FALSE),
    relativePath(FALSE)
{
}


PURL::PURL(const char * str, const char * defaultScheme)
{
  Parse(str, defaultScheme);
}


PURL::PURL(const PString & str, const char * defaultScheme)
{
  Parse(str, defaultScheme);
}


PURL::PURL(const PFilePath & filePath)
  : //scheme(SchemeTable[FILE_SCHEME].name),
    scheme(FILE_SCHEME),
    port(0),
    portSupplied (FALSE),
    relativePath(FALSE)
{
  PStringArray pathArray = filePath.GetDirectory().GetPath();
  hostname = pathArray[0];

  PINDEX i;
  for (i = 1; i < pathArray.GetSize(); i++)
    pathArray[i-1] = pathArray[i];
  pathArray[i-1] = filePath.GetFileName();

  SetPath(pathArray);
}


PObject::Comparison PURL::Compare(const PObject & obj) const
{
  PAssert(PIsDescendant(&obj, PURL), PInvalidCast);
  return urlString.Compare(((const PURL &)obj).urlString);
}


PINDEX PURL::HashFunction() const
{
  return urlString.HashFunction();
}


void PURL::PrintOn(ostream & stream) const
{
  stream << urlString;
}


void PURL::ReadFrom(istream & stream)
{
  PString s;
  stream >> s;
  Parse(s);
}


PString PURL::TranslateString(const PString & str, TranslationType type)
{
  PString xlat = str;

  PString safeChars = "abcdefghijklmnopqrstuvwxyz"
                      "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                      "0123456789$-_.!*'(),";
  switch (type) {
    case LoginTranslation :
      safeChars += "+;?&=";
      break;

    case PathTranslation :
      safeChars += "+:@&=";
      break;

    case QueryTranslation :
      safeChars += ":@";
  }
  PINDEX pos = (PINDEX)-1;
  while ((pos = xlat.FindSpan(safeChars, pos+1)) != P_MAX_INDEX)
    xlat.Splice(psprintf("%%%02X", (BYTE)xlat[pos]), pos, 1);

  if (type == QueryTranslation) {
    PINDEX space = (PINDEX)-1;
    while ((space = xlat.Find(' ', space+1)) != P_MAX_INDEX)
      xlat[space] = '+';
  }

  return xlat;
}


PString PURL::UntranslateString(const PString & str, TranslationType type)
{
  PString xlat = str;
  xlat.MakeUnique();

  PINDEX pos;
  if (type == PURL::QueryTranslation) {
    pos = (PINDEX)-1;
    while ((pos = xlat.Find('+', pos+1)) != P_MAX_INDEX)
      xlat[pos] = ' ';
  }

  pos = (PINDEX)-1;
  while ((pos = xlat.Find('%', pos+1)) != P_MAX_INDEX) {
    int digit1 = xlat[pos+1];
    int digit2 = xlat[pos+2];
    if (isxdigit(digit1) && isxdigit(digit2)) {
      xlat[pos] = (char)(
            (isdigit(digit2) ? (digit2-'0') : (toupper(digit2)-'A'+10)) +
           ((isdigit(digit1) ? (digit1-'0') : (toupper(digit1)-'A'+10)) << 4));
      xlat.Delete(pos+1, 2);
    }
  }

  return xlat;
}


static void SplitVars(const PString & str, PStringToString & vars, char sep1, char sep2)
{
  PINDEX sep1prev = 0;
  do {
    PINDEX sep1next = str.Find(sep1, sep1prev);
    if (sep1next == P_MAX_INDEX)
      sep1next--; // Implicit assumption string is not a couple of gigabytes long ...

    PINDEX sep2pos = str.Find(sep2, sep1prev);
    if (sep2pos > sep1next)
      sep2pos = sep1next;

    PCaselessString key = PURL::UntranslateString(str(sep1prev, sep2pos-1), PURL::QueryTranslation);
    if (!key) {
      PString data = PURL::UntranslateString(str(sep2pos+1, sep1next-1), PURL::QueryTranslation);

      if (vars.Contains(key))
        vars.SetAt(key, vars[key] + ',' + data);
      else
        vars.SetAt(key, data);
    }

    sep1prev = sep1next+1;
  } while (sep1prev != P_MAX_INDEX);
}


void PURL::SplitQueryVars(const PString & queryStr, PStringToString & queryVars)
{
  SplitVars(queryStr, queryVars, '&', '=');
}


BOOL PURL::InternalParse(const char * cstr, const char * defaultScheme)
{
  urlString = cstr;

  scheme.MakeEmpty();
  username.MakeEmpty();
  password.MakeEmpty();
  hostname.MakeEmpty();
  port = 0;
  portSupplied = FALSE;
  relativePath = FALSE;
  pathStr.MakeEmpty();
  path.SetSize(0);
  paramVars.RemoveAll();
  fragment.MakeEmpty();
  queryVars.RemoveAll();

  // copy the string so we can take bits off it
  while (((*cstr & 0x80) == 0x00) && isspace(*cstr))
    cstr++;
  PString url = cstr;

  // Character set as per RFC2396
  PINDEX pos = 0;
  while ( ((*cstr & 0x80) != 0x00) || isalnum(url[pos]) || url[pos] == '+' || url[pos] == '-' || url[pos] == '.')
    pos++;

  PString schemeName;

  // get information which tells us how to parse URL for this
  // particular scheme
  PURLScheme * schemeInfo = NULL;

  // Determine if the URL has an explicit scheme
  if (url[pos] == ':') {

    // get the scheme information, or get the default scheme
    schemeInfo = PFactory<PURLScheme>::CreateInstance(url.Left(pos));
    if (schemeInfo == NULL && defaultScheme == NULL) {
      PFactory<PURLScheme>::KeyList_T keyList = PFactory<PURLScheme>::GetKeyList();
      if (keyList.size() != 0)
        schemeInfo = PFactory<PURLScheme>::CreateInstance(keyList[0]);
    }
    if (schemeInfo != NULL)
      url.Delete(0, pos+1);
  }

  // if we could not match a scheme, then use the specified default scheme
  if (schemeInfo == NULL && defaultScheme != NULL)
    schemeInfo = PFactory<PURLScheme>::CreateInstance(defaultScheme);

  // if that still fails, then use the global default scheme
  if (schemeInfo == NULL)
    schemeInfo = PFactory<PURLScheme>::CreateInstance(DEFAULT_SCHEME);

  // if that fails, then there is nowehere to go
  PAssert(schemeInfo != NULL, "Default scheme not available");
  scheme = schemeInfo->GetName();
  if (!schemeInfo->Parse(url, *this))
    return FALSE;

  return !IsEmpty();
}

BOOL PURL::LegacyParse(const PString & _url, const PURLLegacyScheme * schemeInfo)
{
  PString url = _url;
  PINDEX pos;

  // Super special case!
  if (scheme *= "callto") {

    // Actually not part of MS spec, but a lot of people put in the // into
    // the URL, so we take it out of it is there.
    if (url.GetLength() > 2 && url[0] == '/' && url[1] == '/')
      url.Delete(0, 2);

    // For some bizarre reason callto uses + instead of ; for paramters
    // We do a loop so that phone numbers of the form +61243654666 still work
    do {
      pos = url.Find('+');
    } while (pos != P_MAX_INDEX && isdigit(url[pos+1]));

    if (pos != P_MAX_INDEX) {
      SplitVars(url(pos+1, P_MAX_INDEX), paramVars, '+', '=');
      url.Delete(pos, P_MAX_INDEX);
    }

    hostname = paramVars("gateway");
    if (!hostname)
      username = UntranslateString(url, LoginTranslation);
    else {
      PCaselessString type = paramVars("type");
      if (type == "directory") {
        pos = url.Find('/');
        if (pos == P_MAX_INDEX)
          username = UntranslateString(url, LoginTranslation);
        else {
          hostname = UntranslateString(url.Left(pos), LoginTranslation);
          username = UntranslateString(url.Mid(pos+1), LoginTranslation);
        }
      }
      else {
        // Now look for an @ and split user and host
        pos = url.Find('@');
        if (pos != P_MAX_INDEX) {
          username = UntranslateString(url.Left(pos), LoginTranslation);
          hostname = UntranslateString(url.Mid(pos+1), LoginTranslation);
        }
        else {
          if (type == "ip" || type == "host")
            hostname = UntranslateString(url, LoginTranslation);
          else
            username = UntranslateString(url, LoginTranslation);
        }
      }
    }

    // Allow for [ipv6] form
    pos = hostname.Find(']');
    if (pos == P_MAX_INDEX)
      pos = 0;
    pos = hostname.Find(':', pos);
    if (pos != P_MAX_INDEX) {
      port = (WORD)hostname.Mid(pos+1).AsUnsigned();
      portSupplied = TRUE;
      hostname.Delete(pos, P_MAX_INDEX);
    }

    password = paramVars("password");
    return TRUE;
  }

  // if the URL should have leading slash, then remove it if it has one
  if (schemeInfo != NULL && schemeInfo->hasHostPort && schemeInfo->hasPath) {
    if (url.GetLength() > 2 && url[0] == '/' && url[1] == '/')
      url.Delete(0, 2);
    else
      relativePath = TRUE;
  }

  // parse user/password/host/port
  if (!relativePath && schemeInfo->hasHostPort) {
    PString endHostChars;
    if (schemeInfo->hasPath)
      endHostChars += '/';
    if (schemeInfo->hasQuery)
      endHostChars += '?';
    if (schemeInfo->hasParameters)
      endHostChars += ';';
    if (schemeInfo->hasFragments)
      endHostChars += '#';
    if (endHostChars.IsEmpty())
      pos = P_MAX_INDEX;
    else
      pos = url.FindOneOf(endHostChars);

    PString uphp = url.Left(pos);
    if (pos != P_MAX_INDEX)
      url.Delete(0, pos);
    else
      url.MakeEmpty();

    // if the URL is of type UserPasswordHostPort, then parse it
    if (schemeInfo->hasUsername) {
      // extract username and password
      PINDEX pos2 = uphp.Find('@');
      PINDEX pos3 = P_MAX_INDEX;
      if (schemeInfo->hasPassword)
        pos3 = uphp.Find(':');
      switch (pos2) {
        case 0 :
          uphp.Delete(0, 1);
          break;

        case P_MAX_INDEX :
          if (schemeInfo->defaultToUserIfNoAt) {
            if (pos3 == P_MAX_INDEX)
              username = UntranslateString(uphp, LoginTranslation);
            else {
              username = UntranslateString(uphp.Left(pos3), LoginTranslation);
              password = UntranslateString(uphp.Mid(pos3+1), LoginTranslation);
            }
            uphp.MakeEmpty();
          }
          break;

        default :
          if (pos3 > pos2)
            username = UntranslateString(uphp.Left(pos2), LoginTranslation);
          else {
            username = UntranslateString(uphp.Left(pos3), LoginTranslation);
            password = UntranslateString(uphp(pos3+1, pos2-1), LoginTranslation);
          }
          uphp.Delete(0, pos2+1);
      }
    }

    // if the URL does not have a port, then this is the hostname
    if (schemeInfo->defaultPort == 0)
      hostname = UntranslateString(uphp, LoginTranslation);
    else {
      // determine if the URL has a port number
      // Allow for [ipv6] form
      pos = uphp.Find(']');
      if (pos == P_MAX_INDEX)
        pos = 0;
      pos = uphp.Find(':', pos);
      if (pos == P_MAX_INDEX)
        hostname = UntranslateString(uphp, LoginTranslation);
      else {
        hostname = UntranslateString(uphp.Left(pos), LoginTranslation);
        port = (WORD)uphp.Mid(pos+1).AsUnsigned();
        portSupplied = TRUE;
      }

      if (hostname.IsEmpty() && schemeInfo->defaultHostToLocal)
        hostname = PIPSocket::GetHostName();
    }
  }

  if (schemeInfo->hasQuery) {
    // chop off any trailing query
    pos = url.Find('?');
    if (pos != P_MAX_INDEX) {
      SplitQueryVars(url(pos+1, P_MAX_INDEX), queryVars);
      url.Delete(pos, P_MAX_INDEX);
    }
  }

  if (schemeInfo->hasParameters) {
    // chop off any trailing parameters
    pos = url.Find(';');
    if (pos != P_MAX_INDEX) {
      SplitVars(url(pos+1, P_MAX_INDEX), paramVars, ';', '=');
      url.Delete(pos, P_MAX_INDEX);
    }
  }

  if (schemeInfo->hasFragments) {
    // chop off any trailing fragment
    pos = url.Find('#');
    if (pos != P_MAX_INDEX) {
      fragment = UntranslateString(url(pos+1, P_MAX_INDEX), PathTranslation);
      url.Delete(pos, P_MAX_INDEX);
    }
  }

  if (schemeInfo->hasPath)
    SetPathStr(url);   // the hierarchy is what is left
  else {
  // if the rest of the URL isn't a path, then we are finished!
    pathStr = UntranslateString(url, PathTranslation);
    Recalculate();
  }

  if (port == 0 && schemeInfo->defaultPort != 0 && !relativePath) {
    // Yes another horrible, horrible special case!
    if (scheme == "h323" && paramVars("type") == "gk")
      port = DEFAULT_H323RAS_PORT;
    else
      port = schemeInfo->defaultPort;
    Recalculate();
  }

  return TRUE;
}


PFilePath PURL::AsFilePath() const
{
  //if (scheme != SchemeTable[FILE_SCHEME].name)
  //  return PString::Empty();
  if (scheme != FILE_SCHEME)
    return PString::Empty();

  PStringStream str;

  if (relativePath) {
    for (PINDEX i = 0; i < path.GetSize(); i++) {
      if (i > 0)
        str << PDIR_SEPARATOR;
      str << path[i];
    }
  }
  else {
    if (hostname != "localhost")
      str << PDIR_SEPARATOR << hostname;
    for (PINDEX i = 0; i < path.GetSize(); i++)
      str << PDIR_SEPARATOR << path[i];
  }

  return str;
}


PString PURL::AsString(UrlFormat fmt) const
{
  if (fmt == FullURL)
    return urlString;

  if (scheme.IsEmpty())
    return PString::Empty();

  //const schemeStruct * schemeInfo = GetSchemeInfo(scheme);
  //if (schemeInfo == NULL)
  //  schemeInfo = &SchemeTable[PARRAYSIZE(SchemeTable)-1];
  const PURLScheme * schemeInfo = PFactory<PURLScheme>::CreateInstance(scheme);
  if (schemeInfo == NULL)
    schemeInfo = PFactory<PURLScheme>::CreateInstance(DEFAULT_SCHEME);

  return schemeInfo->AsString(fmt, *this);
}

PString PURL::LegacyAsString(PURL::UrlFormat fmt, const PURLLegacyScheme * schemeInfo) const
{
  PStringStream str;
  PINDEX i;

  if (fmt == HostPortOnly) {
    if (schemeInfo->hasHostPort && hostname.IsEmpty())
      return str;

    str << scheme << ':';

    if (relativePath) {
      if (schemeInfo->relativeImpliesScheme)
        return PString::Empty();
      return str;
    }

    if (schemeInfo->hasPath && schemeInfo->hasHostPort)
      str << "//";

    if (schemeInfo->hasUsername) {
      if (!username) {
        str << TranslateString(username, LoginTranslation);
        if (schemeInfo->hasPassword && !password)
          str << ':' << TranslateString(password, LoginTranslation);
        str << '@';
      }
    }

    if (schemeInfo->hasHostPort) {
      if (hostname.Find(':') != P_MAX_INDEX)
        str << '[' << hostname << ']';
      else
        str << hostname;
    }

    if (schemeInfo->defaultPort != 0) {
      if (port != schemeInfo->defaultPort || portSupplied)
        str << ':' << port;
    }

    return str;
  }

  // URIOnly and PathOnly
  if (schemeInfo->hasPath) {
    for (i = 0; i < path.GetSize(); i++) {
      if (i > 0 || !relativePath)
        str << '/';
      str << TranslateString(path[i], PathTranslation);
    }
  }
  else
    str << TranslateString(pathStr, PathTranslation);

  if (fmt == URIOnly) {
    if (!fragment)
      str << "#" << TranslateString(fragment, PathTranslation);

    for (i = 0; i < paramVars.GetSize(); i++) {
      str << ';' << TranslateString(paramVars.GetKeyAt(i), QueryTranslation);
      PString data = paramVars.GetDataAt(i);
      if (!data)
        str << '=' << TranslateString(data, QueryTranslation);
    }

    if (!queryVars.IsEmpty())
      str << '?' << GetQuery();
  }

  return str;
}


void PURL::SetScheme(const PString & s)
{
  scheme = s;
  Recalculate();
}


void PURL::SetUserName(const PString & u)
{
  username = u;
  Recalculate();
}


void PURL::SetPassword(const PString & p)
{
  password = p;
  Recalculate();
}


void PURL::SetHostName(const PString & h)
{
  hostname = h;
  Recalculate();
}


void PURL::SetPort(WORD newPort)
{
  port = newPort;
  Recalculate();
}


void PURL::SetPathStr(const PString & p)
{
  pathStr = p;

  path = pathStr.Tokenise("/", TRUE);

  if (path.GetSize() > 0 && path[0].IsEmpty()) 
    path.RemoveAt(0);

  for (PINDEX i = 0; i < path.GetSize(); i++) {
    path[i] = UntranslateString(path[i], PathTranslation);
    if (i > 0 && path[i] == ".." && path[i-1] != "..") {
      path.RemoveAt(i--);
      path.RemoveAt(i--);
    }
  }

  Recalculate();
}


void PURL::SetPath(const PStringArray & p)
{
  path = p;

  pathStr.MakeEmpty();
  for (PINDEX i = 0; i < path.GetSize(); i++)
    pathStr += '/' + path[i];

  Recalculate();
}


PString PURL::GetParameters() const
{
  PStringStream str;

  for (PINDEX i = 0; i < paramVars.GetSize(); i++) {
    if (i > 0)
      str << ';';
    str << paramVars.GetKeyAt(i);
    PString data = paramVars.GetDataAt(i);
    if (!data)
      str << '=' << data;
  }

  return str;
}


void PURL::SetParameters(const PString & parameters)
{
  SplitVars(parameters, paramVars, ';', '=');
  Recalculate();
}


void PURL::SetParamVars(const PStringToString & p)
{
  paramVars = p;
  Recalculate();
}


void PURL::SetParamVar(const PString & key, const PString & data)
{
  if (data.IsEmpty())
    paramVars.RemoveAt(key);
  else
    paramVars.SetAt(key, data);
  Recalculate();
}


PString PURL::GetQuery() const
{
  PStringStream str;

  for (PINDEX i = 0; i < queryVars.GetSize(); i++) {
    if (i > 0)
      str << '&';
    str << TranslateString(queryVars.GetKeyAt(i), QueryTranslation)
        << '='
        << TranslateString(queryVars.GetDataAt(i), QueryTranslation);
  }

  return str;
}


void PURL::SetQuery(const PString & queryStr)
{
  SplitQueryVars(queryStr, queryVars);
  Recalculate();
}


void PURL::SetQueryVars(const PStringToString & q)
{
  queryVars = q;
  Recalculate();
}


void PURL::SetQueryVar(const PString & key, const PString & data)
{
  if (data.IsEmpty())
    queryVars.RemoveAt(key);
  else
    queryVars.SetAt(key, data);
  Recalculate();
}


BOOL PURL::OpenBrowser(const PString & url)
{
#ifdef WIN32
  SHELLEXECUTEINFO sei;
  ZeroMemory(&sei, sizeof(SHELLEXECUTEINFO));
  sei.cbSize = sizeof(SHELLEXECUTEINFO);
  sei.lpVerb = TEXT("open");
 #ifndef _WIN32_WCE
   sei.lpFile = url;
 #else
  sei.lpFile = url.AsUCS2();
 #endif // _WIN32_WCE

  if (ShellExecuteEx(&sei) != 0)
    return TRUE;

#ifndef _WIN32_WCE
  MessageBox(NULL, "Unable to open page"&url, PProcess::Current().GetName(), MB_TASKMODAL);
#else
  MessageBox(NULL, _T("Unable to open page"), PProcess::Current().GetName().AsUCS2(), MB_APPLMODAL);
#endif // _WIN32_WCE

#endif // WIN32
  return FALSE;
}


void PURL::Recalculate()
{
  //if (scheme.IsEmpty())
  //  scheme = SchemeTable[DEFAULT_SCHEME].name;
  if (scheme.IsEmpty())
    scheme = DEFAULT_SCHEME;

  urlString = AsString(HostPortOnly) + AsString(URIOnly);
}


//////////////////////////////////////////////////////////////////////////////
// PHTTP

static char const * const HTTPCommands[PHTTP::NumCommands] = {
  // HTTP 1.0 commands
  "GET", "HEAD", "POST",

  // HTTP 1.1 commands
  "PUT",  "DELETE", "TRACE", "OPTIONS",

  // HTTPS command
  "CONNECT"
};


const PString & PHTTP::AllowTag           () { static PString s = "Allow"; return s; }
const PString & PHTTP::AuthorizationTag   () { static PString s = "Authorization"; return s; }
const PString & PHTTP::ContentEncodingTag () { static PString s = "Content-Encoding"; return s; }
const PString & PHTTP::ContentLengthTag   () { static PString s = "Content-Length"; return s; }
const PString & PHTTP::ContentTypeTag     () { static PString s = "Content-Type"; return s; }
const PString & PHTTP::DateTag            () { static PString s = "Date"; return s; }
const PString & PHTTP::ExpiresTag         () { static PString s = "Expires"; return s; }
const PString & PHTTP::FromTag            () { static PString s = "From"; return s; }
const PString & PHTTP::IfModifiedSinceTag () { static PString s = "If-Modified-Since"; return s; }
const PString & PHTTP::LastModifiedTag    () { static PString s = "Last-Modified"; return s; }
const PString & PHTTP::LocationTag        () { static PString s = "Location"; return s; }
const PString & PHTTP::PragmaTag          () { static PString s = "Pragma"; return s; }
const PString & PHTTP::PragmaNoCacheTag   () { static PString s = "no-cache"; return s; }
const PString & PHTTP::RefererTag         () { static PString s = "Referer"; return s; }
const PString & PHTTP::ServerTag          () { static PString s = "Server"; return s; }
const PString & PHTTP::UserAgentTag       () { static PString s = "User-Agent"; return s; }
const PString & PHTTP::WWWAuthenticateTag () { static PString s = "WWW-Authenticate"; return s; }
const PString & PHTTP::MIMEVersionTag     () { static PString s = "MIME-Version"; return s; }
const PString & PHTTP::ConnectionTag      () { static PString s = "Connection"; return s; }
const PString & PHTTP::KeepAliveTag       () { static PString s = "Keep-Alive"; return s; }
const PString & PHTTP::TransferEncodingTag() { static PString s = "Transfer-Encoding"; return s; }
const PString & PHTTP::ChunkedTag         () { static PString s = "chunked"; return s; }
const PString & PHTTP::ProxyConnectionTag () { static PString s = "Proxy-Connection"; return s; }
const PString & PHTTP::ProxyAuthorizationTag(){ static PString s = "Proxy-Authorization"; return s; }
const PString & PHTTP::ProxyAuthenticateTag(){ static PString s = "Proxy-Authenticate"; return s; }
const PString & PHTTP::ForwardedTag       () { static PString s = "Forwarded"; return s; }
const PString & PHTTP::SetCookieTag       () { static PString s = "Set-Cookie"; return s; }
const PString & PHTTP::CookieTag          () { static PString s = "Cookie"; return s; }



PHTTP::PHTTP()
  : PInternetProtocol("www 80", NumCommands, HTTPCommands)
{
}


PINDEX PHTTP::ParseResponse(const PString & line)
{
  PINDEX endVer = line.Find(' ');
  if (endVer == P_MAX_INDEX) {
    lastResponseInfo = "Bad response";
    lastResponseCode = PHTTP::InternalServerError;
    return 0;
  }

  lastResponseInfo = line.Left(endVer);
  PINDEX endCode = line.Find(' ', endVer+1);
  lastResponseCode = line(endVer+1,endCode-1).AsInteger();
  if (lastResponseCode == 0)
    lastResponseCode = PHTTP::InternalServerError;
  lastResponseInfo &= line.Mid(endCode);
  return 0;
}


// End Of File ///////////////////////////////////////////////////////////////
