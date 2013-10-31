#include <ptlib.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

void BeginPage (PStringStream &html, const char *ptitle, const char *title, const char *quotekey);
void EndPage (PStringStream &html, PString copyr);

PString ErrorPage( //maybe ptlib could provide pages like this? for future: dig http server part
  PString        ip,            // "192.168.1.1"
  unsigned short port,          // 1420
  unsigned       errorCode,     // 403
  PString        errorText,     // "Forbidden"
  PString        title,         // "Page you tried to access is forbidden, lol"
  PString        description    // detailed: "blablablablablabla \n blablablablablabla"
);

////////////////////////////////////////////////////////////////////////////////////////////////////

class DefaultPConfigPage : public PConfigPage
{
 public:
   DefaultPConfigPage(PHTTPServiceProcess & app,const PString & title, const PString & section, const PHTTPAuthority & auth);
    virtual BOOL Post(
      PHTTPRequest & request,
      const PStringToString & data,
      PHTML & replyMessage
    );
    BOOL OnPOST(
      PHTTPServer & server,
      const PURL & url,
      const PMIMEInfo & info,
      const PStringToString & data,
      const PHTTPConnectionInfo & connectInfo
    );
 protected:
   PConfig cfg;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class GeneralPConfigPage : public DefaultPConfigPage
{
 public:
   GeneralPConfigPage(PHTTPServiceProcess & app,const PString & title, const PString & section, const PHTTPAuthority & auth);
   void SetString(PString str){string=str;};
   PString GetString(){return string;};
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class EndpointsPConfigPage : public PConfigPage
{
 public:
   EndpointsPConfigPage(PHTTPServiceProcess & app,const PString & title, const PString & section, const PHTTPAuthority & auth);
    virtual BOOL Post(
      PHTTPRequest & request,
      const PStringToString & data,
      PHTML & replyMessage
    );
    BOOL OnPOST(
      PHTTPServer & server,
      const PURL & url,
      const PMIMEInfo & info,
      const PStringToString & data,
      const PHTTPConnectionInfo & connectInfo
    );
  private:
    PConfig cfg;
    PStringArray dataArray;
    PINDEX numParams;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class VideoPConfigPage : public DefaultPConfigPage
{
 public:
   VideoPConfigPage(PHTTPServiceProcess & app,const PString & title, const PString & section, const PHTTPAuthority & auth);
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class RecordPConfigPage : public DefaultPConfigPage
{
 public:
   RecordPConfigPage(PHTTPServiceProcess & app,const PString & title, const PString & section, const PHTTPAuthority & auth);
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class H323PConfigPage : public DefaultPConfigPage
{
 public:
   H323PConfigPage(PHTTPServiceProcess & app,const PString & title, const PString & section, const PHTTPAuthority & auth);
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class SIPPConfigPage : public DefaultPConfigPage
{
 public:
   SIPPConfigPage(PHTTPServiceProcess & app,const PString & title, const PString & section, const PHTTPAuthority & auth);
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class CodecsPConfigPage : public PConfigPage
{
  public:
    CodecsPConfigPage(PHTTPServiceProcess & app,const PString & title, const PString & section, const PHTTPAuthority & auth);
    virtual BOOL Post(
      PHTTPRequest & request,
      const PStringToString & data,
      PHTML & replyMessage
    );
    BOOL OnPOST(
      PHTTPServer & server,
      const PURL & url,
      const PMIMEInfo & info,
      const PStringToString & data,
      const PHTTPConnectionInfo & connectInfo
    );
  private:
    PConfig cfg;
    PStringArray dataArray;
    PStringArray fmtpArray;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class SectionPConfigPage : public PConfigPage
{
  public:
    SectionPConfigPage(PHTTPServiceProcess & app,const PString & title, const PString & section, const PHTTPAuthority & auth);
    virtual BOOL Post(
      PHTTPRequest & request,
      const PStringToString & data,
      PHTML & replyMessage
    );
    BOOL OnPOST(
      PHTTPServer & server,
      const PURL & url,
      const PMIMEInfo & info,
      const PStringToString & data,
      const PHTTPConnectionInfo & connectInfo
    );
  private:
    PConfig cfg;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

#if USE_LIBJPEG
class JpegFrameHTTP : public PServiceHTTPString
{
  public:
    JpegFrameHTTP(OpenMCU & app, PHTTPAuthority & auth);
    BOOL OnGET (PHTTPServer & server, const PURL &url, const PMIMEInfo & info, const PHTTPConnectionInfo & connectInfo);
    PMutex mutex;
  private:
    OpenMCU & app;
};
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////

class InteractiveHTTP : public PServiceHTTPString
{
  public:
    InteractiveHTTP(OpenMCU & app, PHTTPAuthority & auth);
    BOOL OnGET (PHTTPServer & server, const PURL &url, const PMIMEInfo & info, const PHTTPConnectionInfo & connectInfo);
  private:
    OpenMCU & app;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class MainStatusPage : public PServiceHTTPString
{
 // PCLASSINFO(MainStatusPage, PServiceHTTPString);

  public:
    MainStatusPage(OpenMCU & app, PHTTPAuthority & auth);
    virtual BOOL Post(PHTTPRequest & request, const PStringToString &, PHTML & msg);
    BOOL OnGET (PHTTPServer & server, const PURL &url, const PMIMEInfo & info, const PHTTPConnectionInfo & connectInfo);

  private:
    OpenMCU & app;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class InvitePage : public PServiceHTTPString
{
  public:
    InvitePage(OpenMCU & app, PHTTPAuthority & auth);

    virtual BOOL Post(
      PHTTPRequest & request,       // Information on this request.
      const PStringToString & data, // Variables in the POST data.
      PHTML & replyMessage          // Reply message for post.
    );

  private:
    OpenMCU & app;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class SelectRoomPage : public PServiceHTTPString
{
  public:
    SelectRoomPage(OpenMCU & app, PHTTPAuthority & auth);

    BOOL OnGET(
      PHTTPServer & server,
      const PURL &url,
      const PMIMEInfo & info,
      const PHTTPConnectionInfo & connectInfo
    );

    virtual BOOL Post(
      PHTTPRequest & request,
      const PStringToString &,
      PHTML & msg
    );

  private:
    OpenMCU & app;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class WelcomePage : public PServiceHTTPString
{
  public:
    WelcomePage(OpenMCU & app, PHTTPAuthority & auth);
    BOOL OnGET (PHTTPServer & server, const PURL &url, const PMIMEInfo & info, const PHTTPConnectionInfo & connectInfo);

  private:
    OpenMCU & app;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class RecordsBrowserPage : public PServiceHTTPString
{
  public:
    RecordsBrowserPage(OpenMCU & app, PHTTPAuthority & auth);
    BOOL OnGET (PHTTPServer & server, const PURL &url, const PMIMEInfo & info, const PHTTPConnectionInfo & connectInfo);

  private:
    OpenMCU & app;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
