/*
 * shttpsvc.h
 *
 * Class for secure service applications using HTTPS as the user interface.
 *
 * Portable Windows Library
 *
 * Copyright (c) 1993-2001 Equivalence Pty. Ltd.
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
 * $Log: shttpsvc.h,v $
 * Revision 1.7  2004/04/24 03:58:15  rjongbloed
 * Allow for run time enable/disable of secure web access to HTTP process,
 *   changed from old debug only hack to "correct" usager. Thanks Ben Lear
 *
 * Revision 1.6  2002/09/16 01:08:59  robertj
 * Added #define so can select if #pragma interface/implementation is used on
 *   platform basis (eg MacOS) rather than compiler, thanks Robert Monaghan.
 *
 * Revision 1.5  2002/08/05 05:40:45  robertj
 * Fixed missing pragma interface/implementation
 *
 * Revision 1.4  2001/12/13 09:19:09  robertj
 * Added ability to create HTTP server certificate if one does not exist.
 *
 * Revision 1.3  2001/05/16 06:02:05  craigs
 * Changed to allow detection of non-SSL connection to SecureHTTPServiceProcess
 *
 * Revision 1.2  2001/03/27 03:56:01  craigs
 * Added hack to allow secure servers to act as non-secure servers
 *
 * Revision 1.1  2001/02/15 02:41:14  robertj
 * Added class to do secure HTTP based service process.
 *
 */

#ifndef SHTTPSVC_H
#define SHTTPSVC_H

#ifdef P_USE_PRAGMA
#pragma interface
#endif


#include <ptclib/httpsvc.h>
#include <ptclib/pssl.h>


/////////////////////////////////////////////////////////////////////

class PSecureHTTPServiceProcess : public PHTTPServiceProcess
{
  PCLASSINFO(PSecureHTTPServiceProcess, PHTTPServiceProcess)

  public:
    PSecureHTTPServiceProcess(const Info & inf);
    ~PSecureHTTPServiceProcess();

    virtual PHTTPServer * CreateHTTPServer(PTCPSocket & socket);

    BOOL SetServerCertificate(
      const PFilePath & certFile,
      BOOL create = FALSE,
      const char * dn = NULL
    );

    virtual BOOL OnDetectedNonSSLConnection(PChannel * chan, const PString & line);

    virtual PString CreateNonSSLMessage(const PString & url);
    virtual PString CreateRedirectMessage(const PString & url);

  protected:
    PSSLContext * sslContext;
    BOOL          disableSSL;
};


#endif // SHTTPSVC_H
