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
     columnColor = "#d9e5e3";
     rowColor = "#d9e5e3";
     itemColor = "#f7f4d8";
     itemInfoColor = "#f7f4d8";
     separator = ",";
     firstEditRow = 1;
     firstDeleteRow = 1;
     buttonUp = buttonDown = buttonClone = buttonDelete = 0;
     columnStyle = "<td align='middle' style='background-color:"+columnColor+";padding:0px;border-bottom:2px solid white;border-right:2px solid white;";
     rowStyle = "<td align='left' style='background-color:"+rowColor+";padding:4px;border-bottom:2px solid white;border-right:2px solid white;'>";
     rowTextStyle = "<td align='left' style='background-color:"+rowColor+";padding:4px;border-bottom:2px solid white;border-right:2px solid white;'>";
     rowArrayStyle = "<td align='left' style='background-color:"+itemColor+";padding:4px;'>";
     itemStyle = "<td align='left' style='background-color:"+itemColor+";padding:4px;border-bottom:2px solid white;border-right:2px solid white;'>";
     itemInfoStyle = "<td rowspan='%ROWSPAN%' valign='top' align='left' style='background-color:"+itemInfoColor+";padding:4px;border-bottom:2px solid white;border-right:2px solid white;'>";
     textStyle = "margin-top:5px;margin-bottom:5px;padding-left:5px;padding-right:5px;";
     inputStyle = "margin-top:5px;margin-bottom:5px;padding-left:5px;padding-right:5px;";
     buttonStyle = "margin-top:5px;margin-bottom:5px;margin-left:1px;margin-right:1px;width:24px;";
   }

   PString NewRowColumn(PString name, int width=255)
   {
     return "<tr>"+columnStyle+"width:"+PString(width)+"px'><p style='"+textStyle+"'>"+name+"</p>";
   }
   PString NewRowText(PString name)
   {
     PString s = "<tr>"+rowTextStyle+"<input name='"+name+"' value='"+name+"' type='hidden'><p style='"+textStyle+"'>"+name+"</p>";
     s += buttons()+"</td>";
     return s;
   }
   PString NewRowInput(PString name, int size=15, int readonly=FALSE)
   {
     PString s = "<tr style='padding:0px;margin:0px;'>"+rowStyle+"<input type=text name='"+name+"' size='"+PString(size)+"' value='"+name+"' style='"+inputStyle+"'";
     if(!readonly) s += "></input>"; else s += "readonly></input>";
     s += buttons()+"</td>";
     return s;
   }
   PString EndRow() { return "</tr>"; }

   PString SeparatorField(PString name="")
   {
     PString s = "<tr>"
                 "<td align='left' style='background-color:white;padding:0px;'></td>"
                 "<td align='left' style='background-color:white;padding:0px;'><p style='text-align:center;"+textStyle+"'><b>"+name+"</b></p></td>"
                 "<td align='left' style='background-color:white;padding:0px;'></td></tr>";
     return s;
   }
   PString StringField(PString name, PString value, int sizeInput=12, PString info="", int readonly=FALSE, PINDEX rowSpan=1)
   {
     return NewRowText(name) + StringItem(name, value, sizeInput) + InfoItem(info, rowSpan);
   }
   PString PasswordField(PString name, PString value, int sizeInput=12, PString info="", int readonly=FALSE, PINDEX rowSpan=1)
   {
     return NewRowText(name) + PasswordItem(name, value, sizeInput) + InfoItem(info, rowSpan);
   }
   PString IntegerField(PString name, int value, int min=-2147483647, int max=2147483647, int sizeInput=15, PString info="", int readonly=FALSE, PINDEX rowSpan=1)
   {
     return NewRowText(name) + IntegerItem(name, value, min, max) + InfoItem(info, rowSpan);
   }
   PString BoolField(PString name, BOOL value, PString info="", int readonly=FALSE, PINDEX rowSpan=1)
   {
     return NewRowText(name) + BoolItem(name, value) + InfoItem(info, rowSpan);
   }
   PString SelectField(PString name, PString value, PString values, int width=120, PString info="", int readonly=FALSE, PINDEX rowSpan=1)
   {
     return NewRowText(name) + SelectItem(name, value, values, width) + InfoItem(info, rowSpan);
   }
   PString ArrayField(PString name, PString values, int size=12, PString info="", int readonly=FALSE, PINDEX rowSpan=1)
   {
     PStringArray data = values.Tokenise(separator);
     PString s = NewRowText(name);
     s += NewItemArray(name);
     if(data.GetSize() == 0)
     {
       s += StringItemArray(name, "", size);
     } else {
       for(PINDEX i = 0; i < data.GetSize(); i++)
         s += StringItemArray(name, data[i], size);
     }
     s += EndItemArray();
     s += InfoItem(info, rowSpan);
     return s;
   }

   PString ColumnItem(PString name, int width=120)
   {
     return columnStyle+"width:"+PString(width)+"px'><p style='"+textStyle+"'>"+name+"</p>";
   }
   PString InfoItem(PString name, PINDEX rowSpan=1)
   {
     PString s;
     if(rowSpan>0)
     {
       s = itemInfoStyle;
       if(rowSpan == 1) s.Replace("rowspan='%ROWSPAN%' valign='top'","",TRUE,0);
       else s.Replace("%ROWSPAN%",PString(rowSpan),TRUE,0);
       s += "<p style='"+textStyle+"'>"+name+"</p></td>";
     }
     return s;
   }
   PString StringItem(PString name, PString value, int size=12, int readonly=FALSE)
   {
     PString id = PString(rand());
     PString s = "<input name='TableItemId' value='"+id+"' type='hidden'>";
     s += itemStyle+"<input type='text' name='"+name+"' size='"+PString(size)+"' value='"+value+"' style='"+inputStyle+"'";
     if(!readonly) s += "></input></td>"; else s += "readonly></input></td>";
     return s;
   }
   PString PasswordItem(PString name, PString value, int size=12, int readonly=FALSE)
   {
     if(passwordDecrypt(value) == value)
       value = passwordCrypt(value);
     PString id = PString(rand());
     PString s = "<input name='TableItemId' value='"+id+"' type='hidden'>";
     s += itemStyle+"<input type='password' name='"+name+"' size='"+PString(size)+"' value='"+value+"' style='"+inputStyle+"'";
     if(!readonly) s += "></input></td>"; else s += "readonly></input></td>";
     passwordFields.SetAt(id, value);
     return s;
   }
   PString IntegerItem(PString name, int value, int min=-2147483647, int max=2147483647, int size=12, int readonly=FALSE)
   {
     PString id = PString(rand());
     PString s = "<input name='TableItemId' value='"+id+"' type='hidden'>";
     s += itemStyle+"<input name='MIN' value='"+PString(min)+"' type='hidden'>"
                    "<input name='MAX' value='"+PString(max)+"' type='hidden'>"
                    "<input type=number name='"+name+"' size='"+PString(size)+"' min='"+PString(min)+"' max='"+PString(max)+"' value='"+PString(value)+"' style='"+inputStyle+"'";
     if(!readonly) s += "></input></td>"; else s += "readonly></input></td>";
     return s;
   }
   PString BoolItem(PString name, BOOL value)
   {
     PString id = PString(rand());
     PString s = "<input name='TableItemId' value='"+id+"' type='hidden'>";
     s += itemStyle+"<input name='"+name+"' value='FALSE' type='hidden' style='"+inputStyle+"'>"
                    "<input name='"+name+"' value='TRUE' type='checkbox' style='"+inputStyle+"'";
     if(value) s +=" checked='yes'></input></td>"; else s +="></input></td>";
     return s;
   }
   PString SelectItem(PString name, PString value, PString values, int width=120)
   {
     PString id = PString(rand());
     PString s = "<input name='TableItemId' value='"+id+"' type='hidden'>";
     PStringArray data = values.Tokenise(",");
     s += itemStyle+"<select name='"+name+"' width='"+PString(width)+"' style='width:"+PString(width)+"px;"+inputStyle+"'>";
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

   PString NewItemArray(PString name)
   {
     return "<td><table id='"+name+"' cellspacing='0' style='margin-top:-1px;margin-left:-1px;margin-right:1px;'><tbody>";
   }
   PString EndItemArray()
   {
     return "<tr></tr></tbody></table></td>";
   }
   PString StringItemArray(PString name, PString value, int size=12)
   {
     PString s = "<tr>"+rowArrayStyle+"<input type=text name='"+name+"' size='"+PString(size)+"' value='"+value+"' style='"+inputStyle+"'></input>";
     s += "<input type=button value='↑' onClick='rowUp(this,0)' style='"+buttonStyle+"'>";
     s += "<input type=button value='↓' onClick='rowDown(this)' style='"+buttonStyle+"'>";
     s += "<input type=button value='+' onClick='rowClone(this)' style='"+buttonStyle+"'>";
     s += "<input type=button value='-' onClick='rowDelete(this,0)' style='"+buttonStyle+"'>";
     s += "</td>";
     return s;
   }

   PString JsLocale(PString locale)
   {
     return "<script type='text/javascript'>document.write("+locale+");</script>";
   }

   PString BeginTable()
   { return "<form method='POST'><table id='table1' cellspacing='8'><tbody>"; }
   PString EndTable()
   {
     PString s = "<tr></tr></tbody></table><p><input name='submit' value='Accept' type='submit'><input name='reset' value='Reset' type='reset'></p></form>";
     s += jsRowDown() + jsRowUp() + jsRowClone()+ jsRowDelete();
     return s;
   }
   PString buttons()
   {
     PString s;
     if(buttonUp) s += "<input type=button value='↑' onClick='rowUp(this,"+PString(firstEditRow)+")' style='"+buttonStyle+"'>";
     if(buttonDown) s += "<input type=button value='↓' onClick='rowDown(this)' style='"+buttonStyle+"'>";
     if(buttonClone) s += "<input type=button value='+' onClick='rowClone(this)' style='"+buttonStyle+"'>";
     if(buttonDelete) s += "<input type=button value='-' onClick='rowDelete(this,"+PString(firstDeleteRow)+")' style='"+buttonStyle+"'>";
     return s;
   }
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
                     "  if(rowNum!=rows-2) table.rows[rowNum].parentNode.insertBefore(table.rows[rowNum+1],table.rows[rowNum]);\n"
                     "}\n"
                     "</script>\n"; }
   PString jsRowClone() { return "<script type='text/javascript'>\n"
                     "function rowClone(obj)\n"
                     "{\n"
                     "  var table = obj.parentNode.parentNode.parentNode.parentNode;\n"
                     "  var rowNum = obj.parentNode.parentNode.sectionRowIndex;\n"
                     "  var node = table.rows[rowNum].cloneNode(true);\n"
                     "  if(table.id == 'table1')\n"
                     "  {\n"
                     "    var seconds = new Date().getTime();\n"
                     "    node.cells[0].childNodes[0].name = seconds\n"
                     "  }\n"
                     "  table.rows[rowNum].parentNode.insertBefore(node, table.rows[rowNum+1]);\n"
                     "}\n"
                     "</script>\n"; }
   PString jsRowDelete() { return "<script type='text/javascript'>\n"
                     "function rowDelete(obj,firstDeleteRow)\n"
                     "{\n"
                     "  var table = obj.parentNode.parentNode.parentNode.parentNode;\n"
                     "  var rowNum = obj.parentNode.parentNode.sectionRowIndex;\n"
                     "  if(table.id == 'table1')\n"
                     "  {\n"
                     "    if(rowNum < firstDeleteRow)\n"
                     "      return;\n"
                     "    table.deleteRow(rowNum);\n"
                     "  } else {\n"
                     "    var table2 = obj.parentNode.parentNode.parentNode;\n"
                     "    var rows = table2.childNodes.length;\n"
                     "    if(rows < 3)\n"
                     "      return;\n"
                     "    table2.deleteRow(rowNum);\n"
                     "  }\n"
                     "}\n"
                     "</script>\n"; }

   PString passwordCrypt(PString pass)
   {
     PTEACypher crypt(CypherKey);
     return crypt.Encode(pass);
   }
   PString passwordDecrypt(PString pass)
   {
     PString clear;
     PTEACypher crypt(CypherKey);
     return crypt.Decode(pass, clear) ? clear : pass;
   }

   BOOL Post(PHTTPRequest & request, const PStringToString & data, PHTML & reply)
   {
     PHTTPForm::Post(request, data, reply);
     cfg.DeleteSection();
     for(PINDEX i = 0; i < dataArray.GetSize(); i++)
     {
       PString key = dataArray[i].Tokenise("=")[0];
       if(key == "") continue;
       PString value = "";
       PINDEX valuePos = dataArray[i].Find("=");
       if(valuePos != P_MAX_INDEX)
         value = dataArray[i].Right(dataArray[i].GetSize()-valuePos-2);
       cfg.SetString(key, value);
     }
     if(cfg.GetBoolean("RESTORE DEFAULTS", FALSE))
       cfg.DeleteSection();
     process.OnContinue();
     return TRUE;
   }
   BOOL OnPOST(PHTTPServer & server, const PURL & url, const PMIMEInfo & info, const PStringToString & data, const PHTTPConnectionInfo & connectInfo)
   {
     PStringArray entityData = connectInfo.GetEntityBody().Tokenise("&");
     PString TableItemId = "";
     PINDEX num = 0;
     int integerMin=-2147483647, integerMax=2147483647, integerIndex=-1;
     PString curKey = "";
     for(PINDEX i = 0; i < entityData.GetSize(); i++)
     {
       PString item = PURL::UntranslateString(entityData[i], PURL::QueryTranslation);
       PString key = item.Tokenise("=")[0];
       PString value = "";
       PINDEX valuePos = item.Find("=");
       if(valuePos != P_MAX_INDEX)
         value = item.Right(item.GetSize()-valuePos-2);

       PString valueNext;
       if((i+1) < entityData.GetSize()) valueNext = PURL::UntranslateString(entityData[i+1].Tokenise("=")[1], PURL::QueryTranslation);
       if(value == "FALSE" && valueNext == "TRUE") continue;
       if(key == "submit") continue;
       if(key == "MIN") { integerMin = atoi(value); continue; }
       if(key == "MAX") { integerMax = atoi(value); integerIndex=i+1; continue; }
       if(key == "TableItemId") { TableItemId = value; continue; }

       if(num == 1) curKey = key;
       PINDEX asize = dataArray.GetSize();
       if(key != curKey)
       {
         num = 0;
         dataArray.AppendString(value+"=");
       } else {
         if(passwordFields.GetAt(TableItemId) == NULL) value.Replace(separator," ",TRUE);
         if(num != 1) dataArray[asize-1] += separator;
         if(integerIndex == i)
         {
           int tmp = atoi(value);
           if(tmp < integerMin) tmp = integerMin;
           if(tmp > integerMax) tmp = integerMax;
           dataArray[asize-1] += PString(tmp);
         } else {
           if(passwordFields.GetAt(TableItemId) != NULL)
           {
             if(passwordFields(TableItemId) != value)
               value = passwordCrypt(value);
           }
           dataArray[asize-1] += value;
         }
       }
       num++;
     }
     PHTTPConfig::OnPOST(server, url, info, data, connectInfo);
     return TRUE;
   }
 protected:
   PConfig cfg;
   PString separator;
   PString columnStyle, rowStyle, rowTextStyle, rowArrayStyle, itemStyle, itemInfoStyle, itemInfoStyleRowSpan, textStyle, inputStyle, buttonStyle;
   PStringArray dataArray;
   PString columnColor, rowColor, itemColor, itemInfoColor;
   int firstEditRow, firstDeleteRow;
   int buttonUp, buttonDown, buttonClone, buttonDelete;
   PStringToString passwordFields;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class GeneralPConfigPage : public TablePConfigPage
{
  public:
    GeneralPConfigPage(PHTTPServiceProcess & app,const PString & title, const PString & section, const PHTTPAuthority & auth);
  private:
    PConfig cfg;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class ManagingUsersPConfigPage : public TablePConfigPage
{
  public:
    ManagingUsersPConfigPage(PHTTPServiceProcess & app,const PString & title, const PString & section, const PHTTPAuthority & auth);
  private:
    PConfig cfg;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class ManagingGroupsPConfigPage : public TablePConfigPage
{
  public:
    ManagingGroupsPConfigPage(PHTTPServiceProcess & app,const PString & title, const PString & section, const PHTTPAuthority & auth);
  private:
    PConfig cfg;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class ControlCodesPConfigPage : public TablePConfigPage
{
  public:
    ControlCodesPConfigPage(PHTTPServiceProcess & app,const PString & title, const PString & section, const PHTTPAuthority & auth);
  private:
    PConfig cfg;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class RoomCodesPConfigPage : public TablePConfigPage
{
  public:
    RoomCodesPConfigPage(PHTTPServiceProcess & app,const PString & title, const PString & section, const PHTTPAuthority & auth);
  private:
    PConfig cfg;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class H323EndpointsPConfigPage : public TablePConfigPage
{
 public:
   H323EndpointsPConfigPage(PHTTPServiceProcess & app,const PString & title, const PString & section, const PHTTPAuthority & auth);
  private:
    PConfig cfg;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class SipEndpointsPConfigPage : public TablePConfigPage
{
 public:
   SipEndpointsPConfigPage(PHTTPServiceProcess & app,const PString & title, const PString & section, const PHTTPAuthority & auth);
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

class VideoPConfigPage : public TablePConfigPage
{
 public:
   VideoPConfigPage(PHTTPServiceProcess & app,const PString & title, const PString & section, const PHTTPAuthority & auth);
  private:
    PConfig cfg;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class RecordPConfigPage : public TablePConfigPage
{
 public:
   RecordPConfigPage(PHTTPServiceProcess & app,const PString & title, const PString & section, const PHTTPAuthority & auth);
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class H323PConfigPage : public TablePConfigPage
{
 public:
   H323PConfigPage(PHTTPServiceProcess & app,const PString & title, const PString & section, const PHTTPAuthority & auth);
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class SIPPConfigPage : public TablePConfigPage
{
 public:
   SIPPConfigPage(PHTTPServiceProcess & app,const PString & title, const PString & section, const PHTTPAuthority & auth);
  private:
    PConfig cfg;
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
    BOOL OnPOST(PHTTPServer & server, const PURL & url, const PMIMEInfo & info, const PStringToString & data, const PHTTPConnectionInfo & connectInfo);
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
