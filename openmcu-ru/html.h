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

class TablePConfigPage : public PConfigPage
{
 public:
   TablePConfigPage(PHTTPServiceProcess & app,const PString & title, const PString & section, const PHTTPAuthority & auth)
     : PConfigPage(app,title,section,auth)
   {
     cfg = MCUConfig(section);
     numCol = 0;
     firstEditRow = 1;
     columnStyle = "<td align='middle' style='background-color:#d9e5e3;padding:4px;border-bottom:2px solid white;border-right:2px solid white;";
     rowStyle = "<td align='left' style='background-color:#d9e5e3;padding:4px;border-bottom:2px solid white;border-right:2px solid white;'>";
     itemStyle = "<td align='middle' style='background-color:#efe;padding:4px;border-bottom:2px solid white;border-right:2px solid white;'>";
     inputStyle = "style='margin-top:5px;margin-bottom:5px;'";
     buttonStyle = "style='margin-top:5px;margin-bottom:5px;margin-left:1px;margin-right:1px;'";
   }

   PString column(PString name, int width)
   { return "<p>"+columnStyle+"width:"+PString(width)+"px'>"+name+"</p>"; }
   PString rowInput(PString name, int size, int readonly = FALSE, int editable = TRUE)
   {
     PString s = "<tr>"+rowStyle+"<input type=text name='"+name+"' size='"+PString(size)+"' value='"+name+"' "+inputStyle;
     if(!readonly) s += ">"; else s += "readonly>";
     if(editable) s += buttons();
     s += "</td>";
     return s;
   }
   PString inputItem(PString name, PString value, int size, int readonly = FALSE)
   {
     PString s = itemStyle+"<input type=text name='"+name+"' size='"+PString(size)+"' value='"+value+"' "+inputStyle;
     if(!readonly) s += "></td>"; else s += "readonly></td>";
     return s;
   }
   PString checkBoxItem(PString name, PString value)
   {
     PString s = itemStyle+"<input name='"+name+"' value='FALSE' type='hidden'><input name='"+name+"' value='TRUE' type='checkbox'";
     if(value == "TRUE") s +=" checked='yes'></td>";
     else s +="></td>";
     return s;
   }
   PString selectItem(PString name, PString value, PString values)
   {
     PStringArray data = values.Tokenise(",");
     PString s = itemStyle+"<select name='"+name+"' "+inputStyle+">";
     for(PINDEX i = 0; i < data.GetSize(); i++)
     {
       if(data[i] == value)
         s += "<option selected value='"+data[i]+"'>"+data[i]+"</option>";
       else
         s += "<option value='"+data[i]+"'>"+data[i]+"</option>";
     }
     s +="</select>";
     return s;
   }

   BOOL Post(PHTTPRequest & request, const PStringToString & data, PHTML & reply)
   {
     PHTTPForm::Post(request, data, reply);
     cfg.DeleteSection();
     for(PINDEX i = 0; dataArray[i] != NULL; i++)
       cfg.SetString(dataArray[i].Tokenise("=")[0], dataArray[i].Tokenise("=")[1]);
     process.OnContinue();
     return TRUE;
   }
   BOOL OnPOST(PHTTPServer & server, const PURL & url, const PMIMEInfo & info, const PStringToString & data, const PHTTPConnectionInfo & connectInfo)
   {
     PStringArray entityData = connectInfo.GetEntityBody().Tokenise("&");
     PINDEX num = 0;
     for(PINDEX i = 0; i < entityData.GetSize(); i++)
     {
       PString key = PURL::UntranslateString(entityData[i].Tokenise("=")[0], PURL::QueryTranslation);
       PString value = PURL::UntranslateString(entityData[i].Tokenise("=")[1], PURL::QueryTranslation);

       PString valueNext;
       if((i+1) < entityData.GetSize()) valueNext = PURL::UntranslateString(entityData[i+1].Tokenise("=")[1], PURL::QueryTranslation);
       if(value == "FALSE" && valueNext == "TRUE") continue;
       if(key == "submit") continue;

       key.Replace(","," ",TRUE);
       value.Replace(","," ",TRUE);
       PINDEX asize = dataArray.GetSize();
       if(num == 0) dataArray.AppendString(value+"=");
       else dataArray[asize-1] += value+",";
       if(num == numCol) num = 0;
       else num++;
     }
     PHTTPConfig::OnPOST(server, url, info, data, connectInfo);
     return TRUE;
   }
   PString buttons() { return "<input type=button value='↑' onClick='rowUp(this,"+PString(firstEditRow)+")' "+buttonStyle+">"
                    "<input type=button value='↓' onClick='rowDown(this)' "+buttonStyle+">"
                    "<input type=button value='+' onClick='rowClone(this)' "+buttonStyle+">"
                    "<input type=button value='-' onClick='rowDelete(this)' "+buttonStyle+">"; }
   PString jsRowUp() { return "<script type='text/javascript'>\n"
                     "function rowUp(obj,topRow)\n"
                     "{\n"
                     "  var table = obj.parentNode.parentNode.parentNode;\n"
                     "  var rowNum=obj.parentNode.parentNode.sectionRowIndex;\n"
                     "  if(rowNum>topRow) table.rows[rowNum].parentNode.insertBefore(table.rows[rowNum],table.rows[rowNum-1]);\n"
                     "}\n"
                     "</script>\n"; }
   PString jsRowDown() { return "<script type='text/javascript'>\n"
                     "function rowDown(obj)\n"
                     "{\n"
                     "  var table = obj.parentNode.parentNode.parentNode;\n"
                     "  var rowNum = obj.parentNode.parentNode.sectionRowIndex;\n"
                     "  var rows = obj.parentNode.parentNode.parentNode.childNodes.length;\n"
                     "  if(rowNum!=rows-1) table.rows[rowNum].parentNode.insertBefore(table.rows[rowNum+1],table.rows[rowNum]);\n"
                     "}\n"
                     "</script>\n"; }
   PString jsRowClone() { return "<script type='text/javascript'>\n"
                     "function rowClone(obj)\n"
                     "{\n"
                     "  var table = obj.parentNode.parentNode.parentNode;\n"
                     "  var rowNum = obj.parentNode.parentNode.sectionRowIndex;\n"
                     "  var node = table.rows[rowNum].cloneNode(true);\n"
                     "  table.insertBefore(node, table.rows[rowNum+1]);\n"
                     "}\n"
                     "</script>\n"; }
   PString jsRowDelete() { return "<script type='text/javascript'>\n"
                     "function rowDelete(obj)\n"
                     "{\n"
                     "  var table = obj.parentNode.parentNode.parentNode;\n"
                     "  var rowNum = obj.parentNode.parentNode.sectionRowIndex;\n"
                     "  table.deleteRow(rowNum);\n"
                     "}\n"
                     "</script>\n"; }

 protected:
   PConfig cfg;
   PString columnStyle, rowStyle, itemStyle, inputStyle, buttonStyle;
   PStringArray dataArray;
   int numCol;
   int firstEditRow;
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

class EndpointsPConfigPage : public TablePConfigPage
{
 public:
   EndpointsPConfigPage(PHTTPServiceProcess & app,const PString & title, const PString & section, const PHTTPAuthority & auth);
  private:
    PConfig cfg;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class ProxySIPPConfigPage : public TablePConfigPage
{
 public:
   ProxySIPPConfigPage(PHTTPServiceProcess & app,const PString & title, const PString & section, const PHTTPAuthority & auth);
  private:
    PConfig cfg;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class RoomAccessSIPPConfigPage : public TablePConfigPage
{
 public:
   RoomAccessSIPPConfigPage(PHTTPServiceProcess & app,const PString & title, const PString & section, const PHTTPAuthority & auth);
  private:
    PConfig cfg;
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

class CodecsPConfigPage : public TablePConfigPage
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

class SectionPConfigPage : public DefaultPConfigPage
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
