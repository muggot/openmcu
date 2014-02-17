/*
 * httpsvc.h
 *
 * Common classes for service applications using HTTP as the user interface.
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
 * $Log: httpsvc.h,v $
 * Revision 1.45  2005/11/30 12:47:37  csoutheren
 * Removed tabs, reformatted some code, and changed tags for Doxygen
 *
 * Revision 1.44  2003/07/15 11:17:45  csoutheren
 * Added additional constructor for PServiceHTTPString,
 *   Thanks to Federico Pinna
 *
 * Revision 1.43  2003/02/19 07:23:17  robertj
 * Changes to allow for single threaded HTTP service processes.
 *
 * Revision 1.42  2002/11/06 22:47:24  robertj
 * Fixed header comment (copyright etc)
 *
 * Revision 1.41  2002/07/30 03:47:13  craigs
 * Added PHTTPServiceString with content type
 *
 * Revision 1.40  2002/07/17 08:02:53  robertj
 * Allowed for adjustable copyright holder
 *
 * Revision 1.39  2002/02/05 22:53:14  robertj
 * Changed default for HTTP listener to reuse addresses.
 *
 * Revision 1.38  2001/10/10 08:06:34  robertj
 * Fixed problem with not shutting down threads when closing listener.
 *
 * Revision 1.37  2001/08/28 06:44:30  craigs
 * Added ability to override PHTTPServer creation
 *
 * Revision 1.36  2001/06/23 00:32:15  robertj
 * Added parameter to be able to set REUSEADDR on listener socket.
 *
 * Revision 1.35  2001/03/04 02:24:44  robertj
 * Removed default OnControl() from http service as cannot get port number.
 *
 * Revision 1.34  2001/02/20 02:32:41  robertj
 * Added PServiceMacro version that can do substitutions on blocks of HTML.
 *
 * Revision 1.33  2001/02/15 01:12:15  robertj
 * Moved some often repeated HTTP service code into PHTTPServiceProcess.
 *
 * Revision 1.32  2001/02/14 02:30:59  robertj
 * Moved HTTP Service Macro facility to public API so can be used by apps.
 * Added ability to specify the service macro keyword, defaults to "macro".
 * Added machine macro to get the OS version and hardware.
 *
 * Revision 1.31  2000/12/14 08:09:41  robertj
 * Fixed missing immediate expiry date on string and file service HTTP resourcer.
 *
 * Revision 1.30  2000/12/11 13:15:17  robertj
 * Added macro to include signed or unsigned chunks of HTML.
 * Added flag to globally ignore HTML signatures (useful for develeopment).
 *
 * Revision 1.29  2000/11/02 21:57:00  craigs
 * Added extra constructor
 *
 * Revision 1.28  2000/08/04 12:48:13  robertj
 * Added mechanism by which a service can get at new HTTP connections, eg to add SSL.
 *
 * Revision 1.27  1999/02/16 08:07:10  robertj
 * MSVC 6.0 compatibility changes.
 *
 * Revision 1.26  1998/11/30 02:50:50  robertj
 * New directory structure
 *
 * Revision 1.25  1998/10/29 11:58:51  robertj
 * Added ability to configure the HTTP threads stack size.
 *
 * Revision 1.24  1998/09/23 06:19:34  robertj
 * Added open source copyright license.
 *
 * Revision 1.23  1998/04/01 01:56:47  robertj
 * Added PServiceHTTPFile constructor so file path and URL can be different.
 *
 * Revision 1.22  1998/03/20 03:16:09  robertj
 * Added special classes for specific sepahores, PMutex and PSyncPoint.
 *
 * Revision 1.21  1998/03/17 10:16:00  robertj
 * Allowed registration page to have HTML override.
 *
 * Revision 1.20  1998/02/16 00:15:13  robertj
 * Major rewrite of application info passed in PHTTPServiceProcess constructor.
 *
 * Revision 1.19  1998/01/26 00:28:32  robertj
 * Removed POrderPage completely from httpsvc.
 * Added PHTTPAuthority to PHTTPServiceString constructor.
 * Added option flags to ProcessMacros to automatically load from file etc.
 *
 * Revision 1.18  1997/11/10 12:40:05  robertj
 * Changed SustituteEquivalSequence so can override standard macros.
 *
 * Revision 1.17  1997/11/04 06:02:56  robertj
 * Allowed help gif file name to overridable in PServiceHTML, so can be in subdirectory.
 *
 * Revision 1.16  1997/10/30 10:22:35  robertj
 * Added ability to customise regisration text by application.
 *
 * Revision 1.15  1997/08/28 13:56:34  robertj
 * Fixed bug where HTTP directory was not processed for macros.
 *
 * Revision 1.13  1997/08/20 08:48:18  craigs
 * Added PHTTPServiceDirectory & PHTTPServiceString
 *
 * Revision 1.12  1997/07/26 11:38:18  robertj
 * Support for overridable pages in HTTP service applications.
 *
 * Revision 1.11  1997/06/16 14:12:55  robertj
 * Changed private to protected.
 *
 * Revision 1.10  1997/06/16 13:20:14  robertj
 * Fixed bug where PHTTPThread crashes on exit.
 *
 * Revision 1.9  1997/03/02 03:42:19  robertj
 * Added error logging to standard HTTP Service HTTP Server.
 * Removed extraneous variables that set GIF file size to zero.
 *
 * Revision 1.8  1997/02/05 11:54:52  robertj
 * Added support for order form page overridiing.
 *
 * Revision 1.7  1997/01/27 10:22:33  robertj
 * Numerous changes to support OEM versions of products.
 *
 * Revision 1.6  1996/11/04 03:55:20  robertj
 * Changed to accept separate copyright and manufacturer strings.
 *
 * Revision 1.5  1996/09/14 13:09:12  robertj
 * Major upgrade:
 *   rearranged sockets to help support IPX.
 *   added indirect channel class and moved all protocols to descend from it,
 *   separating the protocol from the low level byte transport.
 *
 * Revision 1.4  1996/08/19 13:43:46  robertj
 * Fixed race condition in system restart logic.
 *
 * Revision 1.3  1996/08/08 13:36:38  robertj
 * Fixed Registation page so no longer has static link, ie can be DLLed.
 *
 * Revision 1.2  1996/06/28 13:15:39  robertj
 * Moved HTTP form resource to another compilation module.
 *
 * Revision 1.1  1996/06/13 13:33:14  robertj
 * Initial revision
 *
 */

#ifndef APPCOMM_H
#define APPCOMM_H

#include <ptlib/svcproc.h>
#include <ptlib/sockets.h>
#include <ptclib/httpform.h>
#include <ptclib/cypher.h>


class PHTTPServiceProcess;


/////////////////////////////////////////////////////////////////////

class PHTTPServiceThread : public PThread
{
  PCLASSINFO(PHTTPServiceThread, PThread)
  public:
    PHTTPServiceThread(PINDEX stackSize,
                       PHTTPServiceProcess & app);
    ~PHTTPServiceThread();

    void Main();
    void Close();

  protected:
    PINDEX                myStackSize;
    PHTTPServiceProcess & process;
    PTCPSocket          * socket;
};


/////////////////////////////////////////////////////////////////////

class PHTTPServiceProcess : public PServiceProcess
{
  PCLASSINFO(PHTTPServiceProcess, PServiceProcess)

  public:
    enum {
      MaxSecuredKeys = 10
    };
    struct Info {
      const char * productName;
      const char * manufacturerName;

      WORD majorVersion;
      WORD minorVersion;
      CodeStatus buildStatus;    ///< AlphaCode, BetaCode or ReleaseCode
      WORD buildNumber;
      const char * compilationDate;

      PTEACypher::Key productKey;  ///< Poduct key for registration
      const char * securedKeys[MaxSecuredKeys]; ///< Product secured keys for registration
      PINDEX securedKeyCount;

      PTEACypher::Key signatureKey;   ///< Signature key for encryption of HTML files

      const char * manufHomePage; ///< WWW address of manufacturers home page
      const char * email;         ///< contact email for manufacturer
      const char * productHTML;   ///< HTML for the product name, if NULL defaults to
                                  ///<   the productName variable.
      const char * gifHTML;       ///< HTML to show GIF image in page headers, if NULL
                                  ///<   then the following are used to build one
      const char * gifFilename;   ///< File name for the products GIF file
      int gifWidth;               ///< Size of GIF image, if zero then none is used
      int gifHeight;              ///<   in the generated HTML.

      const char * copyrightHolder;   ///< Name of copyright holder
      const char * copyrightHomePage; ///< Home page for copyright holder
      const char * copyrightEmail;    ///< E-Mail address for copyright holder
    };

    PHTTPServiceProcess(const Info & inf);
    ~PHTTPServiceProcess();

    BOOL OnStart();
    void OnStop();
    BOOL OnPause();
    void OnContinue();
    const char * GetServiceDependencies() const;

    virtual void OnConfigChanged() = 0;
    virtual BOOL Initialise(const char * initMsg) = 0;

    BOOL ListenForHTTP(
      WORD port,
      PSocket::Reusability reuse = PSocket::CanReuseAddress,
      PINDEX stackSize = 0x4000
    );
    BOOL ListenForHTTP(
      PSocket * listener,
      PSocket::Reusability reuse = PSocket::CanReuseAddress,
      PINDEX stackSize = 0x4000
    );

    virtual PString GetPageGraphic();
    void GetPageHeader(PHTML &);
    void GetPageHeader(PHTML &, const PString & title);

    virtual PString GetCopyrightText();

    const PString & GetMacroKeyword() const { return macroKeyword; }
    const PTime & GetCompilationDate() const { return compilationDate; }
    const PString & GetHomePage() const { return manufacturersHomePage; }
    const PString & GetEMailAddress() const { return manufacturersEmail; }
    const PString & GetProductName() const { return productNameHTML; }
    const PTEACypher::Key & GetProductKey() const { return productKey; }
    const PStringArray & GetSecuredKeys() const { return securedKeys; }
    const PTEACypher::Key & GetSignatureKey() const { return signatureKey; }
    BOOL ShouldIgnoreSignatures() const { return ignoreSignatures; }
    void SetIgnoreSignatures(BOOL ig) { ignoreSignatures = ig; }

    static PHTTPServiceProcess & Current();

    virtual void AddRegisteredText(PHTML & html);
    virtual void AddUnregisteredText(PHTML & html);
    virtual BOOL SubstituteEquivalSequence(PHTTPRequest & request, const PString &, PString &);
    virtual PHTTPServer * CreateHTTPServer(PTCPSocket & socket);
    virtual PHTTPServer * OnCreateHTTPServer(const PHTTPSpace & urlSpace);
    PTCPSocket * AcceptHTTP();
    BOOL ProcessHTTP(PTCPSocket & socket);

  protected:
    PSocket  * httpListeningSocket;
    PHTTPSpace httpNameSpace;
    PString    macroKeyword;

    PTEACypher::Key productKey;
    PStringArray    securedKeys;
    PTEACypher::Key signatureKey;
    BOOL            ignoreSignatures;

    PTime      compilationDate;
    PString    manufacturersHomePage;
    PString    manufacturersEmail;
    PString    productNameHTML;
    PString    gifHTML;
    PString    copyrightHolder;
    PString    copyrightHomePage;
    PString    copyrightEmail;

    void ShutdownListener();
    void BeginRestartSystem();
    void CompleteRestartSystem();

    PThread *  restartThread;

    PLIST(ThreadList, PHTTPServiceThread);
    ThreadList httpThreads;
    PMutex     httpThreadsMutex;

  friend class PConfigPage;
  friend class PConfigSectionsPage;
  friend class PHTTPServiceThread;
};


/////////////////////////////////////////////////////////////////////

class PConfigPage : public PHTTPConfig
{
  PCLASSINFO(PConfigPage, PHTTPConfig)
  public:
    PConfigPage(
      PHTTPServiceProcess & app,
      const PString & section,
      const PHTTPAuthority & auth
    );
    PConfigPage(
      PHTTPServiceProcess & app,
      const PString & title,
      const PString & section,
      const PHTTPAuthority & auth
    );

    void OnLoadedText(PHTTPRequest &, PString & text);

    BOOL OnPOST(
      PHTTPServer & server,
      const PURL & url,
      const PMIMEInfo & info,
      const PStringToString & data,
      const PHTTPConnectionInfo & connectInfo
    );

    virtual BOOL Post(
      PHTTPRequest & request,       ///< Information on this request.
      const PStringToString & data, ///< Variables in the POST data.
      PHTML & replyMessage          ///< Reply message for post.
    );

  protected:
    virtual BOOL GetExpirationDate(
      PTime & when          ///< Time that the resource expires
    );

    PHTTPServiceProcess & process;
};


/////////////////////////////////////////////////////////////////////

class PConfigSectionsPage : public PHTTPConfigSectionList
{
  PCLASSINFO(PConfigSectionsPage, PHTTPConfigSectionList)
  public:
    PConfigSectionsPage(
      PHTTPServiceProcess & app,
      const PURL & url,
      const PHTTPAuthority & auth,
      const PString & prefix,
      const PString & valueName,
      const PURL & editSection,
      const PURL & newSection,
      const PString & newTitle,
      PHTML & heading
    );

    void OnLoadedText(PHTTPRequest &, PString & text);

    BOOL OnPOST(
      PHTTPServer & server,
      const PURL & url,
      const PMIMEInfo & info,
      const PStringToString & data,
      const PHTTPConnectionInfo & connectInfo
    );

    virtual BOOL Post(
      PHTTPRequest & request,       ///< Information on this request.
      const PStringToString & data, ///< Variables in the POST data.
      PHTML & replyMessage          ///< Reply message for post.
    );

  protected:
    virtual BOOL GetExpirationDate(
      PTime & when          ///< Time that the resource expires
    );

    PHTTPServiceProcess & process;
};


/////////////////////////////////////////////////////////////////////

class PRegisterPage : public PConfigPage
{
  PCLASSINFO(PRegisterPage, PConfigPage)
  public:
    PRegisterPage(
      PHTTPServiceProcess & app,
      const PHTTPAuthority & auth
    );

    PString LoadText(
      PHTTPRequest & request        ///< Information on this request.
    );
    void OnLoadedText(PHTTPRequest & request, PString & text);

    virtual BOOL Post(
      PHTTPRequest & request,       ///< Information on this request.
      const PStringToString & data, ///< Variables in the POST data.
      PHTML & replyMessage          ///< Reply message for post.
    );

    virtual void AddFields(
      const PString & prefix        ///< Prefix on field names
    ) = 0;

  protected:
    PHTTPServiceProcess & process;
};


/////////////////////////////////////////////////////////////////////S

class PServiceHTML : public PHTML
{
  PCLASSINFO(PServiceHTML, PHTML)
  public:
    PServiceHTML(const char * title,
                 const char * help = NULL,
                 const char * helpGif = "help.gif");

    PString ExtractSignature(PString & out);
    static PString ExtractSignature(const PString & html,
                                    PString & out,
                                    const char * keyword = "#equival");

    PString CalculateSignature();
    static PString CalculateSignature(const PString & out);
    static PString CalculateSignature(const PString & out, const PTEACypher::Key & sig);

    BOOL CheckSignature();
    static BOOL CheckSignature(const PString & html);

    enum MacroOptions {
      NoOptions           = 0,
      NeedSignature       = 1,
      LoadFromFile        = 2,
      NoURLOverride       = 4,
      NoSignatureForFile  = 8
    };
    static BOOL ProcessMacros(PHTTPRequest & request,
                              PString & text,
                              const PString & filename,
                              unsigned options);
};


///////////////////////////////////////////////////////////////

class PServiceMacro : public PObject
{
  public:
    PServiceMacro(const char * name, BOOL isBlock);
    PServiceMacro(const PCaselessString & name, BOOL isBlock);
    Comparison Compare(const PObject & obj) const;
    virtual PString Translate(
      PHTTPRequest & request,
      const PString & args,
      const PString & block
    ) const;
  protected:
    const char * macroName;
    BOOL isMacroBlock;
    PServiceMacro * link;
    static PServiceMacro * list;
  friend class PServiceMacros_list;
};


#define P_EMPTY

#define PCREATE_SERVICE_MACRO(name, request, args) \
  class PServiceMacro_##name : public PServiceMacro { \
    public: \
      PServiceMacro_##name() : PServiceMacro(#name, FALSE) { } \
      PString Translate(PHTTPRequest &, const PString &, const PString &) const; \
  }; \
  static const PServiceMacro_##name serviceMacro_##name; \
  PString PServiceMacro_##name::Translate(PHTTPRequest & request, const PString & args, const PString &) const



#define PCREATE_SERVICE_MACRO_BLOCK(name, request, args, block) \
  class PServiceMacro_##name : public PServiceMacro { \
    public: \
      PServiceMacro_##name() : PServiceMacro(#name, TRUE) { } \
      PString Translate(PHTTPRequest &, const PString &, const PString &) const; \
  }; \
  static const PServiceMacro_##name serviceMacro_##name; \
  PString PServiceMacro_##name::Translate(PHTTPRequest & request, const PString & args, const PString & block) const



///////////////////////////////////////////////////////////////

class PServiceHTTPString : public PHTTPString
{
  PCLASSINFO(PServiceHTTPString, PHTTPString)
  public:
    PServiceHTTPString(const PURL & url, const PString & string)
      : PHTTPString(url, string) { }

    PServiceHTTPString(const PURL & url, const PHTTPAuthority & auth)
      : PHTTPString(url, auth) { }

    PServiceHTTPString(const PURL & url, const PString & string, const PHTTPAuthority & auth)
      : PHTTPString(url, string, auth) { }

    PServiceHTTPString(const PURL & url, const PString & string, const PString & contentType)
      : PHTTPString(url, string, contentType) { }

    PServiceHTTPString(const PURL & url, const PString & string, const PString & contentType, const PHTTPAuthority & auth)
      : PHTTPString(url, string, contentType, auth) { }

    PString LoadText(PHTTPRequest &);

  protected:
    virtual BOOL GetExpirationDate(
      PTime & when          ///< Time that the resource expires
    );
};


class PServiceHTTPFile : public PHTTPFile
{
  PCLASSINFO(PServiceHTTPFile, PHTTPFile)
  public:
    PServiceHTTPFile(const PString & filename, BOOL needSig = FALSE)
      : PHTTPFile(filename) { needSignature = needSig; }
    PServiceHTTPFile(const PString & filename, const PFilePath & file, BOOL needSig = FALSE)
      : PHTTPFile(filename, file) { needSignature = needSig; }
    PServiceHTTPFile(const PString & filename, const PHTTPAuthority & auth, BOOL needSig = FALSE)
      : PHTTPFile(filename, auth) { needSignature = needSig; }
    PServiceHTTPFile(const PString & filename, const PFilePath & file, const PHTTPAuthority & auth, BOOL needSig = FALSE)
      : PHTTPFile(filename, file, auth) { needSignature = needSig; }

    void OnLoadedText(PHTTPRequest &, PString & text);

  protected:
    virtual BOOL GetExpirationDate(
      PTime & when          ///< Time that the resource expires
    );

    BOOL needSignature;
};

class PServiceHTTPDirectory : public PHTTPDirectory
{
  PCLASSINFO(PServiceHTTPDirectory, PHTTPDirectory)
  public:
    PServiceHTTPDirectory(const PURL & url, const PDirectory & dirname, BOOL needSig = FALSE)
      : PHTTPDirectory(url, dirname) { needSignature = needSig; }

    PServiceHTTPDirectory(const PURL & url, const PDirectory & dirname, const PHTTPAuthority & auth, BOOL needSig = FALSE)
      : PHTTPDirectory(url, dirname, auth) { needSignature = needSig; }

    void OnLoadedText(PHTTPRequest &, PString & text);

  protected:
    virtual BOOL GetExpirationDate(
      PTime & when          ///< Time that the resource expires
    );

    BOOL needSignature;
};


#endif
