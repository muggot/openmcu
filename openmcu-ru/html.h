#ifndef _MCU_HTML_H
#define _MCU_HTML_H


#include <ptlib.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

class TablePConfigPage : public PConfigPage
{
 public:
   TablePConfigPage(PHTTPServiceProcess & app,const PString & title, const PString & section, const PHTTPAuthority & auth)
     : PConfigPage(app,title,section,auth)
   {
     cfg = MCUConfig(section);
     deleteSection = TRUE;
     columnColor = "#d9e5e3";
     rowColor = "#d9e5e3";
     itemColor = "#f7f4d8";
     itemInfoColor = "#f7f4d8";
     separator = ",";
     firstEditRow = 1;
     firstDeleteRow = 1;
     buttonUp = buttonDown = buttonClone = buttonDelete = 0;
     colStyle = "<td align='middle' style='background-color:"+columnColor+";padding:0px;border-right:inherit;";
     rowStyle = "<td align='left' valign='top' style='background-color:"+rowColor+";padding:0px 4px 0px 4px;border-right:inherit;'>";
     rowFieldStyle = "<td align='left' valign='top' style='background-color:"+rowColor+";padding:0px 4px 0px 4px;border-right:inherit;width:300px;'>";
     itemStyle = "<td align='left' valign='top' style='background-color:"+itemColor+";padding:0px 4px 0px 4px;border-right:inherit;'>";
     itemInfoStyle = "<td rowspan='%ROWSPAN%' align='left' valign='top' style='background-color:"+itemInfoColor+";padding:0px 4px 0px 4px;border-right:inherit;'>";
     textStyle = "margin-top:5px;margin-bottom:5px;padding-left:5px;padding-right:5px;";
     inputStyle = "margin-top:5px;margin-bottom:5px;padding-left:5px;padding-right:5px;";
     buttonStyle = "margin-top:5px;margin-bottom:5px;margin-left:1px;margin-right:1px;width:24px;";
     rowBorders = FALSE;
     rowBordersStyle = "3px ridge;";
     rowArray = "<tr valign='middle'><td align='left' style='background-color:"+itemColor+";padding:0px 4px 0px 4px;'>";
   }

   /////////////////////////////////////////////////////////////////////////////////////////////////
   PString SeparatorField(PString name="")
   {
     PString s = "<tr><td align='left' colspan='3' style='background-color:white;padding:0px;'><p style='text-align:center;"+textStyle+"'><b>"+name+"</b></p></td>";
     return s;
   }
   PString StringField(PString name, PString value, int sizeInput=12, PString info="", int readonly=FALSE, PINDEX rowSpan=1)
   {
     return NewRowField(name) + StringItem(name, value, sizeInput) + InfoItem(info, rowSpan);
   }
   PString PasswordField(PString name, PString value, int sizeInput=12, PString info="", int readonly=FALSE, PINDEX rowSpan=1)
   {
     return NewRowField(name) + PasswordItem(name, value, sizeInput) + InfoItem(info, rowSpan);
   }
   PString IntegerField(PString name, int value, int min=-2147483647, int max=2147483647, int sizeInput=15, PString info="", int readonly=FALSE, PINDEX rowSpan=1)
   {
     return NewRowField(name) + IntegerItem(name, value, min, max) + InfoItem(info, rowSpan);
   }
   PString BoolField(PString name, BOOL value, PString info="", int readonly=FALSE, PINDEX rowSpan=1)
   {
     return NewRowField(name) + BoolItem(name, value) + InfoItem(info, rowSpan);
   }
   PString SelectField(PString name, PString value, PString values, int width=120, PString info="", int readonly=FALSE, PINDEX rowSpan=1)
   {
     return NewRowField(name) + SelectItem(name, value, values, width) + InfoItem(info, rowSpan);
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

   /////////////////////////////////////////////////////////////////////////////////////////////////
   PString Row()
   {
     PString s;
     if(rowBorders)
       s = "<tr style='padding:0px;margin:0px;border-right:2px solid white;border-top:"+rowBordersStyle+";border-bottom:"+rowBordersStyle+";'>";
     else
       s = "<tr style='padding:0px;margin:0px;border-bottom:2px solid white;border-right:2px solid white;'>";
     return s;
   }
   /////////////////////////////////////////////////////////////////////////////////////////////////
   PString NewRowColumn(PString name, int width=250)
   {
     PString s = Row();
     return s+colStyle+"width:"+PString(width)+"px'><p style='"+textStyle+";width:"+PString(width)+"px'>"+name+"</p>";
   }
   PString NewRowText(PString name)
   {
     PString s = Row();
     s += rowStyle+"<input name='"+name+"' value='"+name+"' type='hidden'><p style='"+textStyle+"'>"+name+"</p>";
     s += "</td>";
     if(buttons() != "") s += rowStyle+buttons()+"</td>";
     return s;
   }
   PString NewRowField(PString name)
   {
     PString s = Row();
     s += rowFieldStyle+"<input name='"+name+"' value='"+name+"' type='hidden'><p style='"+textStyle+"'>"+name+"</p>";
     s += "</td>";
     if(buttons() != "") s += rowStyle+buttons()+"</td>";
     return s;
   }
   PString NewRowInputAccount(PString name, int size=15, int readonly=FALSE)
   {
     return NewRowInput(name, size, readonly, "FilterAccount(this)");
   }
   PString NewRowInput(PString name, int size=15, int readonly=FALSE, PString filter="")
   {
     PString value = name;
     if(name == "empty") value = "";
     PString s = Row();
     s += rowStyle+"<input onkeyup='"+filter+"' onchange='"+filter+"' type='text' name='"+name+"' size='"+PString(size)+"' value='"+value+"' style='"+inputStyle+"'";
     if(!readonly) s += "></input>"; else s += "readonly></input>";
     if(!readonly) s += buttons();
     s += "</td>";
     return s;
   }
   PString EndRow() { return "</tr>"; }

   /////////////////////////////////////////////////////////////////////////////////////////////////
   PString ColumnItem(PString name, int width=120)
   {
     return colStyle+"width:"+PString(width)+"px'><p style='"+textStyle+"'>"+name+"</p>";
   }
   PString ColumnItem(PString name, PString optionName)
   {
     if(optionName != "") optionNames.AppendString(optionName);
     return colStyle+"width:120px'><p style='"+textStyle+"'>"+name+"</p>";
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
   PString StringItemIp(PString name, PString value, int size=12, int readonly=FALSE)
   {
     return StringItem(name, value, size, readonly, "FilterIp(this)");
   }
   PString StringItemInteger(PString name, PString value, int size=12, int readonly=FALSE)
   {
     return StringItem(name, value, size, readonly, "FilterInteger(this)");
   }
   PString StringItemAccount(PString name, PString value, int size=12, int readonly=FALSE)
   {
     return StringItem(name, value, size, readonly, "FilterAccount(this)");
   }
   PString StringItem(PString name, PString value, int size=12, int readonly=FALSE, PString filter="")
   {
     PString id = PString(rand());
     PString s = "<input name='TableItemId' value='"+id+"' type='hidden'>";
     s += itemStyle+"<input onkeyup='"+filter+"' onchange='"+filter+"' type='text' name='"+name+"' size='"+PString(size)+"' value='"+value+"' style='"+inputStyle+"'";
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
                    "<input onkeyup='FilterInteger(this)' onchange='FilterInteger(this)' type=number name='"+name+"' size='"+PString(size)+"' min='"+PString(min)+"' max='"+PString(max)+"' value='"+PString(value)+"' style='"+inputStyle+"'";
     if(!readonly) s += "></input></td>"; else s += "readonly></input></td>";
     return s;
   }
   PString BoolItem(PString name, BOOL value, int readonly=FALSE)
   {
     PString id = PString(rand());
     PString s = "<input name='TableItemId' value='"+id+"' type='hidden'>";
     s += itemStyle+"<input name='"+name+"' value='FALSE' type='hidden' style='"+inputStyle+"'>"
                    "<input name='"+name+"' value='TRUE' type='checkbox' style='"+inputStyle+"margin-top:12px;margin-bottom:12px;margin-left:3px;'";
     if(value) s +=" checked='yes'";
     if(!readonly) s += "></input></td>"; else s += " readonly></input></td>";
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
   PString EmptyInputItem(PString name, BOOL hidden=FALSE)
   {
     PString id = PString(rand());
     PString s = "<input name='TableItemId' value='"+id+"' type='hidden'>";
     s += "<td style='height:100%;background-color:"+itemColor+";border-right:inherit;'><input name='"+name+"' type='hidden'>";
     if(hidden) s += "</input>";
     else s += "&nbsp</input>";
     return s;
   }
   PString EmptyTextItem()
   {
     PString s = "<td style='height:100%;background-color:"+itemColor+";border-right:inherit;'>&nbsp</td>";
     return s;
   }

   PString NewItemArray(PString name, PString width="0")
   {
     return "<td height='100%' width='"+width+"%'><table id='"+name+"' cellspacing='0' width='100%' height='100%'><tbody>";
   }
   PString EndItemArray()
   {
     return "<tr></tr></tbody></table></td>";
   }
   PString StringItemArray(PString name, PString value, int size=12)
   {
     PString s = rowArray+"<input type=text name='"+name+"' size='"+PString(size)+"' value='"+value+"' style='"+inputStyle+"'></input>";
     s += "<input type=button value='↑' onClick='rowUp(this,0)' style='"+buttonStyle+"'>";
     s += "<input type=button value='↓' onClick='rowDown(this)' style='"+buttonStyle+"'>";
     s += "<input type=button value='+' onClick='rowClone(this)' style='"+buttonStyle+"'>";
     s += "<input type=button value='-' onClick='rowDelete(this,0)' style='"+buttonStyle+"'>";
     s += "</td>";
     return s;
   }

   /////////////////////////////////////////////////////////////////////////////////////////////////
   PString BeginTable()
   {
     return "<form method='POST'><div id='table_config_page' style='height:100%;display:block;overflow-x:auto;overflow-y:auto;border-bottom:1px solid;border-top:1px solid;'>"
            "<table id='table1' cellspacing='8'><tbody>"
            "<script type='text/javascript'>"
            "  var wh = window.innerHeight || window.clientHeight || 600;"
            "  var ih = 0; if(document.getElementById('quote_info')) ih = document.getElementById('quote_info').offsetHeight;"
            "  document.getElementById('table_config_page').style.height = String(wh-ih-255)+'px';"
            "</script>";
   }
   PString EndTable()
   {
     PString s = "<tr></tr></tbody></table></div><p><input id='button_accept' name='submit' value='Accept' type='submit'><input id='button_reset' name='reset' value='Reset' type='reset'></p></form>";
     s += jsRowDown() + jsRowUp() + jsRowClone()+ jsRowDelete();
     s += Filters();
     return s;
   }
   PString JsLocale(PString locale)
   {
     return "<script type='text/javascript'>document.write("+locale+");</script>";
   }
   PString Filters()
   {
     return "<script type='text/javascript'>"
            "function FilterInteger(obj){ obj.value = obj.value.replace(/[^0-9]/g,''); }"
            "function FilterIp(obj)     { obj.value = obj.value.replace(/[^0-9\\.]/g,''); }"
            "function FilterAccount(obj){ obj.value = obj.value.replace(/[^A-Za-z0-9-_\\.]/g,''); }"
            "</script>";
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

   BOOL FormPost(PHTTPRequest & request, const PStringToString & data, PHTML & msg)
   {
     if(msg.IsEmpty())
       msg << "<script>location.href=\"" << request.url.AsString() << "\"</script>";
     return TRUE;
   }
   BOOL Post(PHTTPRequest & request, const PStringToString & data, PHTML & reply)
   {
     if(sectionPrefix == "")
     {
       if(deleteSection)
         cfg.DeleteSection();
       if(!cfg.GetBoolean("RESTORE DEFAULTS", FALSE))
       {
         for(PINDEX i = 0; i < dataArray.GetSize(); i++)
         {
           PString d = dataArray[i];
           PString key = d.Tokenise("=")[0];
           if(key == "") continue;
           PString value;
           PINDEX valuePos = d.Find("=");
           if(valuePos != P_MAX_INDEX)
             value = d.Right(d.GetSize()-valuePos-2);
           cfg.SetString(key, value);
         }
       }
     } else {
       PStringList sect = cfg.GetSections();
       for(PINDEX i = 0; i < sect.GetSize(); i++)
       {
         if(sect[i].Left(sectionPrefix.GetLength()) == sectionPrefix)
           MCUConfig(sect[i]).DeleteSection();
       }
       for(PINDEX i = 0; i < dataArray.GetSize(); i++)
       {
         PString d = dataArray[i];
         PString key = d.Tokenise("=")[0];
         if(key == "") continue;

         MCUConfig scfg(sectionPrefix+key);
         PString value;
         PINDEX valuePos = d.Find("=");
         if(valuePos != P_MAX_INDEX)
         {
           value = d.Right(d.GetSize()-valuePos-2);
           PStringArray vArray = value.Tokenise(separator);
           for(PINDEX j = 0; j < optionNames.GetSize(); j++)
           {
             PString v;
             if(j < vArray.GetSize())
               v = vArray[j];
             scfg.SetString(optionNames[j], v);
           }
         }
       }
     }
     process.OnContinue();
     FormPost(request, data, reply);
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
   MCUConfig cfg;
   BOOL deleteSection;
   PString separator;
   PString colStyle, rowStyle, rowFieldStyle, itemStyle, itemInfoStyle, itemInfoStyleRowSpan, textStyle, inputStyle, buttonStyle;
   PString rowArray;
   PStringArray dataArray;
   PString columnColor, rowColor, itemColor, itemInfoColor;
   int firstEditRow, firstDeleteRow;
   int buttonUp, buttonDown, buttonClone, buttonDelete;
   PStringToString passwordFields;

   PStringArray optionNames;
   PString sectionPrefix;

   BOOL rowBorders;
   PString rowBordersStyle;
};

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
   MCUConfig cfg;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class GeneralPConfigPage : public TablePConfigPage
{
  public:
    GeneralPConfigPage(PHTTPServiceProcess & app,const PString & title, const PString & section, const PHTTPAuthority & auth);
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class RegistrarPConfigPage : public TablePConfigPage
{
  public:
    RegistrarPConfigPage(PHTTPServiceProcess & app,const PString & title, const PString & section, const PHTTPAuthority & auth);
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class ConferencePConfigPage : public TablePConfigPage
{
  public:
    ConferencePConfigPage(PHTTPServiceProcess & app,const PString & title, const PString & section, const PHTTPAuthority & auth);
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class ManagingUsersPConfigPage : public TablePConfigPage
{
  public:
    ManagingUsersPConfigPage(PHTTPServiceProcess & app,const PString & title, const PString & section, const PHTTPAuthority & auth);
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class ManagingGroupsPConfigPage : public TablePConfigPage
{
  public:
    ManagingGroupsPConfigPage(PHTTPServiceProcess & app,const PString & title, const PString & section, const PHTTPAuthority & auth);
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class ControlCodesPConfigPage : public TablePConfigPage
{
  public:
    ControlCodesPConfigPage(PHTTPServiceProcess & app,const PString & title, const PString & section, const PHTTPAuthority & auth);
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class RoomCodesPConfigPage : public TablePConfigPage
{
  public:
    RoomCodesPConfigPage(PHTTPServiceProcess & app,const PString & title, const PString & section, const PHTTPAuthority & auth);
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class H323PConfigPage : public TablePConfigPage
{
 public:
   H323PConfigPage(PHTTPServiceProcess & app,const PString & title, const PString & section, const PHTTPAuthority & auth);
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class H323EndpointsPConfigPage : public TablePConfigPage
{
 public:
   H323EndpointsPConfigPage(PHTTPServiceProcess & app,const PString & title, const PString & section, const PHTTPAuthority & auth);
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class SIPPConfigPage : public TablePConfigPage
{
 public:
   SIPPConfigPage(PHTTPServiceProcess & app,const PString & title, const PString & section, const PHTTPAuthority & auth);
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class SipEndpointsPConfigPage : public TablePConfigPage
{
 public:
   SipEndpointsPConfigPage(PHTTPServiceProcess & app,const PString & title, const PString & section, const PHTTPAuthority & auth);
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class ProxySIPPConfigPage : public TablePConfigPage
{
 public:
   ProxySIPPConfigPage(PHTTPServiceProcess & app,const PString & title, const PString & section, const PHTTPAuthority & auth);
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class RoomAccessSIPPConfigPage : public TablePConfigPage
{
 public:
   RoomAccessSIPPConfigPage(PHTTPServiceProcess & app,const PString & title, const PString & section, const PHTTPAuthority & auth);
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class VideoPConfigPage : public TablePConfigPage
{
 public:
   VideoPConfigPage(PHTTPServiceProcess & app,const PString & title, const PString & section, const PHTTPAuthority & auth);
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class RecordPConfigPage : public TablePConfigPage
{
 public:
   RecordPConfigPage(PHTTPServiceProcess & app,const PString & title, const PString & section, const PHTTPAuthority & auth);
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
    PStringStream form;
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

#endif // _MCU_HTML_H

////////////////////////////////////////////////////////////////////////////////////////////////////
