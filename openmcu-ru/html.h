
#include "precompile.h"

#ifndef _MCU_HTML_H
#define _MCU_HTML_H

#include "mcu.h"
#include "utils_type.h"
#include "utils_list.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

class TablePConfigPage : public PConfigPage
{
  public:
    TablePConfigPage(PHTTPServiceProcess & app,const PString & title, const PString & section, const PHTTPAuthority & auth)
      : PConfigPage(app,title,section,auth), cfg(section)
    {
      deleteSection = TRUE;
      buttonShowAdvanced = FALSE;
      columnColor = "#d9e5e3";
      rowColor = "#d9e5e3";
      itemColor = "#f7f4d8";
      itemInfoColor = "#f7f4d8";
      separator = ",";
      firstEditRow = 1;
      firstDeleteRow = 1;
      tableId = 0;
      fontSize = "12";
      itemSize = "16";
      rowHeight = "28";
      buttonUp = buttonDown = buttonClone = buttonDelete = FALSE;
      colStyle = "<td align='middle' style='background-color:"+columnColor+";padding:0px;border-right:inherit;";
      rowStyle = "<td align='left' valign='top' style='background-color:"+rowColor+";padding:0px 4px 0px 4px;border-right:inherit;'>";
      rowFieldStyle = "<td align='left' valign='top' style='background-color:"+rowColor+";padding:0px 4px 0px 4px;border-right:inherit;width:350px;'>";
      itemStyle = "<td align='left' valign='top' style='background-color:"+itemColor+";padding:0px 4px 0px 4px;border-right:inherit;'>";
      itemInfoStyle = "<td rowspan='%ROWSPAN%' align='left' valign='top' style='background-color:"+itemInfoColor+";padding:0px 4px 0px 4px;border-right:inherit;'>";
      textStyle = "font-size:13px;margin:2px 0px 2px 0px;padding:0px 3px 0px 3px;";
      inputStyle = "font-size:"+fontSize+"px;height:"+itemSize+"px;margin:2px 0px 2px 0px;padding:3px 3px 3px 3px;border-radius:0px;";
      selectStyle = "font-size:"+fontSize+"px;height:"+itemSize+"px;margin:2px 0px 2px 0px;padding:3px 3px 3px 3px;border-radius:0px;box-sizing:content-box;-ms-box-sizing:content-box;-moz-box-sizing:content-box;-webkit-box-sizing:content-box;";
      buttonStyle = "font-size:"+fontSize+"px;height:"+itemSize+"px;width:14px;margin:2px 1px 2px 1px;padding:2px 4px 4px 4px;border-radius:0px;box-sizing:content-box;-ms-box-sizing:content-box;-moz-box-sizing:content-box;-webkit-box-sizing:content-box;";
      checkboxStyle = "margin-top:8px;margin-bottom:8px;margin-left:3px;";
      rowBorders = FALSE;
      rowBordersStyle = "3px ridge;";

      js_filters =
        "function FilterIp(obj)     { obj.value = obj.value.replace(/[^0-9\\.:]/g,''); }"
        "function FilterAccount(obj){ obj.value = obj.value.replace(/[^A-Za-zА-Яа-я0-9-_\\.]/g,''); }"
        "function FilterInteger(obj) { obj.value = obj.value.replace(/[^0-9]/g,''); }"
        "function FilterMinMax(obj, min, max) { FilterInteger(obj); if(obj.value == '') return; if(obj.value < min) obj.value = min; else if(obj.value > max) obj.value = max; }"
        ;
      js_row_up =
        "function rowUp(obj,topRow)\n"
        "{\n"
        "  var table = obj.parentNode.parentNode.parentNode;\n"
        "  var rowNum=obj.parentNode.parentNode.sectionRowIndex;\n"
        "  if(rowNum>topRow) table.rows[rowNum].parentNode.insertBefore(table.rows[rowNum],table.rows[rowNum-1]);\n"
        "}\n";
      js_row_down =
        "function rowDown(obj)\n"
        "{\n"
        "  var table = obj.parentNode.parentNode.parentNode;\n"
        "  var rowNum = obj.parentNode.parentNode.sectionRowIndex;\n"
        "  var rows = obj.parentNode.parentNode.parentNode.childNodes.length;\n"
        "  if(rowNum!=rows-2) table.rows[rowNum].parentNode.insertBefore(table.rows[rowNum+1],table.rows[rowNum]);\n"
        "}\n";
      js_row_clone =
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
        "}\n";
      js_row_delete =
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
        "}\n";
      js_video_receive_res_toggle =
        "function video_receive_res_toggle(id, codec) {\n"
        "  var sel = document.getElementById(id);\n"
        "  var value = sel.value;\n"
        "  var res = Array();\n"
        "  if(codec=='H.261{sw}')     res = Array('','176x144','352x288');\n"
        "  if(codec=='H.263{sw}')     res = Array('','128x96','176x144','352x288','704x576','1408x1152');\n"
        "  if(codec=='H.263p{sw}')    res = Array('','128x96','176x144','352x288','704x576','1408x1152');\n"
        "  if(codec=='H.264{sw}')     res = Array('','128x96','176x144','352x288','704x576','1280x720','1366x768','1920x1080');\n"
        "  if(codec=='MP4V-ES{sw}')   res = Array('','176x144','320x240','352x288','640x360','640x480','704x576','800x600','854x480','1024x768','1280x720','1280x1024','1366x768','1920x1080');\n"
        "  if(codec=='VP8{sw}')       res = Array('','176x144','320x240','352x288','640x360','640x480','704x576','800x600','852x480','1024x768','1280x720','1280x1024','1364x768','1920x1080');\n"
        "  sel.options.length = 0;\n"
        "  for(var i=0; i<res.length; i++) {\n"
        "    sel.options[sel.options.length] = new Option(res[i],res[i]);\n"
        "  }\n"
        "  sel.value = value;\n"
        "}\n";
      js_video_transmit_res_toggle =
        "function video_transmit_res_toggle(id, codec) {\n"
        "  var sel = document.getElementById(id);\n"
        "  var value = sel.value;\n"
        "  var res = Array();\n"
        "  if(codec=='H.261{sw}')     res = Array('','176x144','352x288');\n"
        "  if(codec=='H.263{sw}')     res = Array('','128x96','176x144','352x288','704x576','1408x1152');\n"
        "  if(codec=='H.263p{sw}')    res = Array('','128x96','176x144','352x288','704x576','1408x1152');\n"
        "  if(codec=='H.264{sw}')     res = Array('','128x96','176x144','320x240','352x288','640x360','640x480','704x576','800x600','854x480','1024x768','1280x720','1280x1024','1366x768','1920x1080');\n"
        "  if(codec=='MP4V-ES{sw}')   res = Array('','176x144','320x240','352x288','640x360','640x480','704x576','800x600','854x480','1024x768','1280x720','1280x1024','1366x768','1920x1080');\n"
        "  if(codec=='VP8{sw}')       res = Array('','176x144','320x240','352x288','640x360','640x480','704x576','800x600','852x480','1024x768','1280x720','1280x1024','1364x768','1920x1080');\n"
        "  sel.options.length = 0;\n"
        "  for(var i=0; i<res.length; i++) {\n"
        "    sel.options[sel.options.length] = new Option(res[i],res[i]);\n"
        "  }\n"
        "  sel.value = value;\n"
        "}\n";
     }

   /////////////////////////////////////////////////////////////////////////////////////////////////
   PString RowArray(PString name = "", BOOL advanced = FALSE)
   {
     PString id = GetTableId();
     if(advanced)
       advancedItems += name+"="+id;
     PString display = (advanced ? "none" : "table-row");
     return "<tr id='"+id+"' valign='middle' height='"+rowHeight+"' style='display:"+display+";'><td align='left' style='font-size:"+fontSize+"px;padding:0px 4px 0px 4px;line-height:100%;'>";
   }
   PString SeparatorField(PString name="")
   {
     PString s = "<tr><td align='left' colspan='3' style='background-color:white;padding:0px;'><p style='text-align:center;"+textStyle+"'><b>"+name+"</b></p></td>";
     return s;
   }
   PString StringField(PString key, PString name, PString value, int width=0, PString info="", PINDEX rowSpan=1)
   {
     return NewRowField(key, name) + StringItem(key, value, width) + InfoItem(info, rowSpan);
   }
   PString AccountField(PString key, PString name, PString value, int width=0, PString info="", PINDEX rowSpan=1)
   {
     return NewRowField(key, name) + AccountItem(key, value, width) + InfoItem(info, rowSpan);
   }
   PString PasswordField(PString key, PString name, PString value, int width=0, PString info="", PINDEX rowSpan=1)
   {
     return NewRowField(key, name) + PasswordItem(key, value, width) + InfoItem(info, rowSpan);
   }
   PString IntegerField(PString key, PString name, PString value, int min, int max, int width=0, PString info="", PINDEX rowSpan=1)
   {
     return NewRowField(key, name) + IntegerItem(key, value, min, max, width) + InfoItem(info, rowSpan);
   }
   PString BoolField(PString key, PString name, BOOL value, PString info="", PINDEX rowSpan=1)
   {
     return NewRowField(key, name) + BoolItem(key, value) + InfoItem(info, rowSpan);
   }
   PString SelectField(PString key, PString name, PString value, PString values, int width=0, PString info="", PINDEX rowSpan=1)
   {
     return NewRowField(key, name) + SelectItem(key, value, values, width) + InfoItem(info, rowSpan);
   }
   PString ArrayField(PString key, PString name, PString values, int width=0, PString info="", PINDEX rowSpan=1)
   {
     PStringArray data = values.Tokenise(separator);
     PString s = NewRowText(key, name);
     s += NewItemArray(key);
     if(data.GetSize() == 0)
     {
       s += StringItemArray(key, "", width);
     } else {
       for(PINDEX i = 0; i < data.GetSize(); i++)
         s += StringItemArray(key, data[i], width);
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
       s = "<tr style='padding:0px;margin:0px;height:"+rowHeight+"px;border-right:2px solid white;border-top:"+rowBordersStyle+";border-bottom:"+rowBordersStyle+";'>";
     else
       s = "<tr style='padding:0px;margin:0px;height:"+rowHeight+"px;border-bottom:2px solid white;border-right:2px solid white;'>";
     return s;
   }
   /////////////////////////////////////////////////////////////////////////////////////////////////
   PString NewRowColumn(PString name, int width=0)
   {
     PString w;
     if(width == 0) w = "auto";
     else           w = PString(width)+"px";
     PString s = Row();
     return s+colStyle+"width:"+w+"'><p style='"+textStyle+";width:"+w+"'>"+name+"</p>";
   }
   PString NewRowText(PString key, PString name = "")
   {
     if(name == "")
       name = key;
     PString s = Row();
     s += rowStyle+"<input name='"+key+"' value='"+key+"' type='hidden'><p style='"+textStyle+"'>"+name+"</p>";
     s += "</td>";
     if(buttons() != "") s += rowStyle+buttons()+"</td>";
     return s;
   }
   PString NewRowField(PString key, PString name = "")
   {
     if(name == "")
       name = key;
     PString s = Row();
     s += rowFieldStyle+"<input name='"+key+"' value='"+key+"' type='hidden'><p style='"+textStyle+"'>"+name+"</p>";
     s += "</td>";
     if(buttons() != "") s += rowStyle+buttons()+"</td>";
     return s;
   }
   PString NewRowInputAccount(PString name, int width=0, int readonly=FALSE)
   {
     if(width == 0) width = 90;
     return NewRowInput(name, width, readonly, "FilterAccount(this)");
   }
   PString NewRowInput(PString name, int width=0, int readonly=FALSE, PString filter="")
   {
     if(width == 0) width = 90;
     PString value = name;
     if(name == "empty") value = "";
     PString s = Row();
     s += rowStyle+"<input onkeyup='"+filter+"' onchange='"+filter+"' type='text' name='"+name+"' value='"+value+"' style='width:"+PString(width)+"px;"+inputStyle+"'";
     if(!readonly) s += "></input>"; else s += "readonly></input>";
     if(!readonly) s += buttons(); else s += buttons(TRUE, TRUE, FALSE, FALSE);
     if(buttonShowAdvanced)
       s += "<input type='checkbox' onclick='show_advanced_items(this, \""+name+"\")' style='"+inputStyle+checkboxStyle+"'></input>";
     s += "</td>";
     return s;
   }
   PString EndRow() { return "</tr>"; }

   /////////////////////////////////////////////////////////////////////////////////////////////////
   PString ColumnItem(PString name, int width=0)
   {
     if(width == 0) width = 90;
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
   PString StringItem(PString name, PString value, int width=0, int readonly=FALSE, PString onkeyup="", PString onchange="")
   {
     if(width == 0) width = 90;
     if(onchange == "") onchange = onkeyup;
     PString s = "<input name='TABLEID' value='"+GetTableId()+"' type='hidden'>";
     s += itemStyle+"<input onkeyup='"+onkeyup+"' onchange='"+onchange+"' type='text' name='"+name+"' value='"+value+"' style='width:"+PString(width)+"px;"+inputStyle+"'";
     if(!readonly) s += "></input></td>"; else s += "readonly></input></td>";
     return s;
   }
   PString PasswordItem(PString name, PString value, int width=0, int readonly=FALSE)
   {
     if(width == 0) width = 90;
     value = passwordDecrypt(value);
     PString id = GetTableId();
     PString s = "<input name='TABLEID' value='"+id+"' type='hidden'>";
     //s += itemStyle+"<input type='password' name='"+name+"' value='"+value+"' style='width:"+PString(width)+"px;"+inputStyle+"'";
     s += itemStyle+"<input type='input' name='"+name+"' value='"+value+"' style='width:"+PString(width)+"px;"+inputStyle+"'";
     if(!readonly) s += "></input></td>"; else s += "readonly></input></td>";
     passwordFields.SetAt(id, value);
     return s;
   }
   PString IntegerItem(PString name, PString value, int min=-2147483647, int max=2147483647, int width=0, int readonly=FALSE)
   {
     if(width == 0) width = 90;
     if(min == 0 && max == 0) { min = -2147483647; max = 2147483647; }
     PString s = "<input name='TABLEID' value='"+GetTableId()+"' type='hidden'>";
     s += itemStyle+"<input onkeyup='FilterInteger(this)' onchange='FilterMinMax(this,"+PString(min)+","+PString(max)+")' type='text' name='"+name+"' value='"+value+"' style='width:"+PString(width)+"px;"+inputStyle+"'";
     if(!readonly) s += "></input></td>"; else s += "readonly></input></td>";
     return s;
   }
   PString IpItem(PString name, PString value, int width=0, int readonly=FALSE)
   {
     return StringItem(name, value, width, readonly, "FilterIp(this)");
   }
   PString AccountItem(PString name, PString value, int width=0, int readonly=FALSE)
   {
     return StringItem(name, value, width, readonly, "FilterAccount(this)");
   }
   PString BoolItem(PString name, BOOL value, int readonly=FALSE)
   {
     PString s = "<input name='TABLEID' value='"+GetTableId()+"' type='hidden'>";
     s += itemStyle+"<input name='"+name+"' value='FALSE' type='hidden' style='"+inputStyle+"'>"
                    "<input name='"+name+"' value='TRUE' type='checkbox' style='"+inputStyle+checkboxStyle+"'";
     if(value) s +=" checked='yes'";
     if(!readonly) s += "></input></td>"; else s += " readonly></input></td>";
     return s;
   }
   PString BoolAndStringItem(PString name, BOOL value1, PString value2, int width=0, int readonly=FALSE, PString onkeyup="", PString onchange="")
   {
     if(width == 0) width = 90;
     width -= 19; // checkbox width
     if(onchange == "") onchange = onkeyup;

     PString s = itemStyle;
     s += "<input name='TABLEID' value='"+GetTableId()+"' type='hidden'>";
     s += "<input name='"+name+"' value='FALSE' type='hidden' style='"+inputStyle+"'>"
          "<input name='"+name+"' value='TRUE' type='checkbox' style='"+inputStyle+"margin:0px 3px 0px 3px;'";
     if(value1) s +=" checked='yes'";
     if(readonly) s += " readonly></input>"; else s += "></input>";

     s += "<input name='TABLEID' value='"+GetTableId()+"' type='hidden'>";
     s += "<input onkeyup='"+onkeyup+"' onchange='"+onchange+"' type='text' name='"+name+"' value='"+value2+"' style='width:"+PString(width)+"px;"+inputStyle+"'";
     if(readonly) s += " readonly></input>"; else s += "></input>";

     s += "</td>";
     return s;
   }
   PString SelectItem(PString name, PString value, PString values, int width=0, PString id="", PString onchange="")
   {
     if(width == 0) width = 90;
     PStringArray data = values.Tokenise(",");

     PString readonly;
     if(id == "" && value == values)
        readonly = "readonly";
     if(id == "")
       id = GetTableId();

     PString s = "<input name='TABLEID' value='"+id+"' type='hidden'>"
                 +itemStyle+"<select id='"+id+"' name='"+name+"' "+readonly+" onchange='"+onchange+"' style='width:"+PString(width)+"px;"+selectStyle+"'>";
     for(PINDEX i = 0; i < data.GetSize(); i++)
     {
       if(data[i] == value) s += "<option selected value='"+data[i]+"'>"+data[i]+"</option>";
       else                 s += "<option value='"+data[i]+"'>"+data[i]+"</option>";
     }
     s +="</select>";
     return s;
   }
   PString EmptyInputItem(PString name, BOOL hidden=FALSE)
   {
     PString s = "<input name='TABLEID' value='"+GetTableId()+"' type='hidden'>";
     s += "<td style='border-right:inherit;'><input name='"+name+"' type='hidden'>";
     if(hidden) s += "</input>";
     else s += "&nbsp</input>";
     return s;
   }

   PString NewItemArray(PString name, int width=0)
   {
     return "<td height='100%' width='"+PString(width)+"%'><table id='"+name+"' cellspacing='0' width='100%' height='100%' style='background-color:"+itemColor+";'><tbody>";
   }
   PString EndItemArray()
   {
     return "<tr></tr></tbody></table></td>";
   }
   PString StringItemArray(PString name, PString value, int width=0)
   {
     if(width == 0) width = 90;
     PString id = GetTableId();
     PString s = "<input name='TABLEID' value='"+id+"' type='hidden'>";
     s += RowArray()+"<input type=text name='"+name+"' value='"+value+"' style='width:"+PString(width)+"px;"+inputStyle+"'></input>";
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
            "  var ih = 0; if(document.getElementById('quote_info')) ih = 15+document.getElementById('quote_info').offsetHeight;"
            "  document.getElementById('table_config_page').style.height = String(wh-ih-225)+'px';"
            "</script>";
   }
   PString EndTable()
   {
     PString s = "<tr></tr></tbody></table></div><p><input id='button_accept' name='submit' value='Accept' type='submit'><input id='button_reset' name='reset' value='Reset' type='reset'></p></form>";
     javascript += js_row_up + js_row_down + js_row_clone + js_row_delete;
     javascript += js_filters;
     javascript += "var advanced_items = Array(";
     for(PINDEX i = 0; i <advancedItems.GetSize(); i++)
     {
       if(i > 0)
         javascript += ",";
       javascript += "\""+advancedItems[i]+"\"";
     }
     javascript += ");";
     javascript +=
       "function show_advanced_items(obj, name)\n"
       "{\n"
       "  checked=obj.checked;\n"
       "  for(i=0;i<advanced_items.length;i++)\n"
       "  {\n"
       "    tokens=advanced_items[i].split('=');\n"
       "    if(tokens[0] == name)\n"
       "      if(checked)\n"
       "        document.getElementById(tokens[1]).style.display = 'table-row';\n"
       "      else\n"
       "        document.getElementById(tokens[1]).style.display = 'none';\n"
       "  }\n"
       "}\n";
     s += "<script type='text/javascript'>\n"+javascript+"</script>\n";
     return s;
   }
   PString GetTableId()
   {
     return "TABLEID_"+PString(tableId++);
   }
   PString JsLocal(PString token)
   {
     return "<script type='text/javascript'>document.write(window.l_"+token+");</script>";
   }
   PString buttons(BOOL _buttonUp = TRUE, BOOL _buttonDown = TRUE, BOOL _buttonClone = TRUE, BOOL _buttonDelete = TRUE)
   {
     PString s;
     if(buttonUp && _buttonUp) s += "<input type=button value='↑' onClick='rowUp(this,"+PString(firstEditRow)+")' style='"+buttonStyle+"'>";
     if(buttonDown && _buttonDown) s += "<input type=button value='↓' onClick='rowDown(this)' style='"+buttonStyle+"'>";
     if(buttonClone && _buttonClone) s += "<input type=button value='+' onClick='rowClone(this)' style='"+buttonStyle+"'>";
     if(buttonDelete && _buttonDelete) s += "<input type=button value='-' onClick='rowDelete(this,"+PString(firstDeleteRow)+")' style='"+buttonStyle+"'>";
     return s;
   }
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
       for(PINDEX i = 0; i < dataDict.GetSize(); i++)
       {
         PString value = dataDict.GetValueAt(i);
         if(value == "" && deleteSection)
           continue;
         if(value == "")
           cfg.DeleteKey(dataDict.GetKeyAt(i));
         else
           cfg.SetString(dataDict.GetKeyAt(i), value);
       }
       if(cfg.GetBoolean("RESTORE DEFAULTS", FALSE))
         cfg.DeleteSection();
     } else {
       PStringList sect = cfg.GetSections();
       for(PINDEX i = 0; i < sect.GetSize(); i++)
       {
         if(sect[i].Left(sectionPrefix.GetLength()) == sectionPrefix)
           MCUConfig(sect[i]).DeleteSection();
       }
       for(PINDEX i = 0; i < dataDict.GetSize(); i++)
       {
         MCUConfig scfg(sectionPrefix + dataDict.GetKeyAt(i));
         PString value = dataDict.GetValueAt(i);
         if(value != "")
         {
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
     PString itemId, curKey, saveKey, saveValue;
     PINDEX num = 0;
     for(PINDEX i = 0; i < entityData.GetSize(); i++)
     {
       PString item = PURL::UntranslateString(entityData[i], PURL::QueryTranslation);
       PString key = item.Tokenise("=")[0];
       PString value;
       PINDEX pos = item.Find("=");
       if(pos != P_MAX_INDEX)
         value = item.Mid(pos+1);

       PString nextKey, nextValue;
       if(i+1 < entityData.GetSize())
       {
         PString nextItem = PURL::UntranslateString(entityData[i+1], PURL::QueryTranslation);
         nextKey = nextItem.Tokenise("=")[0];
         PINDEX pos = nextItem.Find("=");
         if(pos != P_MAX_INDEX)
           nextValue = nextItem.Mid(pos+1);
       }

       // skip checkbox
       if(value == "FALSE" && nextValue == "TRUE") continue;
       // skip accept button
       if(key == "submit") continue;
       // skip hidden item
       if(key == "TABLEID")
       {
         itemId = value;
         // подряд два ID если пустой select
         if(nextKey == "TABLEID") { saveValue += separator; num++; }
         continue;
       }

       if(num == 1) curKey = key;
       if(key != curKey)
       {
         // save data
         if(saveKey != "")
           SaveData(saveKey, saveValue);
         // next save key
         saveKey = value;
         saveValue = "";
         num = 1;
         continue;
       }
       // replace separator in value
       if(passwordFields.GetAt(itemId) == NULL)
         value.Replace(separator," ",TRUE);
       // password fields
       if(passwordFields.GetAt(itemId) != NULL)
       {
         if(passwordFields(itemId) != value)
           value = passwordCrypt(value);
       }
       // append separator
       if(num > 1)
         saveValue += separator;
       // append value
       saveValue += value;
       num++;
     }
     // save the latest data
     if(saveKey != "")
       SaveData(saveKey, saveValue);
     //
     PHTTPConfig::OnPOST(server, url, info, data, connectInfo);
     return TRUE;
   }
   void SaveData(PString & key, PString & value)
   {
     dataDict.Append(key, value);
   }

 protected:
   MCUConfig cfg;
   BOOL deleteSection;
   PString separator;
   PString colStyle, rowStyle, rowFieldStyle, itemStyle, itemInfoStyle, itemInfoStyleRowSpan, textStyle, inputStyle, buttonStyle, selectStyle, checkboxStyle;
   MCUStringDictionary dataDict;
   PString columnColor, rowColor, itemColor, itemInfoColor;
   int firstEditRow, firstDeleteRow;
   BOOL buttonUp, buttonDown, buttonClone, buttonDelete;
   BOOL buttonShowAdvanced;
   int tableId;
   PStringToString passwordFields;
   PString javascript;
   PString itemSize, fontSize, rowHeight;
   PStringArray advancedItems;

   PStringArray optionNames;
   PString sectionPrefix;

   BOOL rowBorders;
   PString rowBordersStyle;

   PString js_filters;
   PString js_row_up, js_row_down, js_row_clone, js_row_delete;
   PString js_video_receive_res_toggle, js_video_transmit_res_toggle;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

void BeginPage (PStringStream &html, PString ptitle, PString title, PString quotekey);
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
    virtual BOOL Post(PHTTPRequest & request, const PStringToString & data, PHTML & replyMessage)
    {
      BOOL ret = TablePConfigPage::Post(request, data, replyMessage);
      OpenMCU::Current().GetRegistrar()->SetInitConfig();
      return ret;
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class ConferencePConfigPage : public TablePConfigPage
{
  public:
    ConferencePConfigPage(PHTTPServiceProcess & app,const PString & title, const PString & section, const PHTTPAuthority & auth);
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class TelnetServerPConfigPage : public TablePConfigPage
{
  public:
    TelnetServerPConfigPage(PHTTPServiceProcess & app,const PString & title, const PString & section, const PHTTPAuthority & auth);
    virtual BOOL Post(PHTTPRequest & request, const PStringToString & data, PHTML & replyMessage)
    {
      BOOL ret = TablePConfigPage::Post(request, data, replyMessage);
      OpenMCU::Current().GetTelnetServer()->StartListeners();
      return ret;
    }
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
    virtual BOOL Post(PHTTPRequest & request, const PStringToString & data, PHTML & replyMessage)
    {
      BOOL ret = TablePConfigPage::Post(request, data, replyMessage);
      OpenMCU::Current().GetRegistrar()->SetInitAccounts();
      return ret;
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class RtspPConfigPage : public TablePConfigPage
{
  public:
    RtspPConfigPage(PHTTPServiceProcess & app,const PString & title, const PString & section, const PHTTPAuthority & auth);
    virtual BOOL Post(PHTTPRequest & request, const PStringToString & data, PHTML & replyMessage)
    {
      BOOL ret = TablePConfigPage::Post(request, data, replyMessage);
      OpenMCU::Current().GetRtspServer()->StartListeners();
      return ret;
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class RtspServersPConfigPage : public TablePConfigPage
{
  public:
    RtspServersPConfigPage(PHTTPServiceProcess & app,const PString & title, const PString & section, const PHTTPAuthority & auth);
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class RtspEndpointsPConfigPage : public TablePConfigPage
{
  public:
    RtspEndpointsPConfigPage(PHTTPServiceProcess & app,const PString & title, const PString & section, const PHTTPAuthority & auth);
    virtual BOOL Post(PHTTPRequest & request, const PStringToString & data, PHTML & replyMessage)
    {
      BOOL ret = TablePConfigPage::Post(request, data, replyMessage);
      OpenMCU::Current().GetRegistrar()->SetInitAccounts();
      return ret;
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class SIPPConfigPage : public TablePConfigPage
{
  public:
    SIPPConfigPage(PHTTPServiceProcess & app,const PString & title, const PString & section, const PHTTPAuthority & auth);
    virtual BOOL Post(PHTTPRequest & request, const PStringToString & data, PHTML & replyMessage)
    {
      BOOL ret = TablePConfigPage::Post(request, data, replyMessage);
      OpenMCU::Current().GetSipEndpoint()->SetInitConfig();
      return ret;
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class SipEndpointsPConfigPage : public TablePConfigPage
{
  public:
    SipEndpointsPConfigPage(PHTTPServiceProcess & app,const PString & title, const PString & section, const PHTTPAuthority & auth);
    virtual BOOL Post(PHTTPRequest & request, const PStringToString & data, PHTML & replyMessage)
    {
      BOOL ret = TablePConfigPage::Post(request, data, replyMessage);
      OpenMCU::Current().GetRegistrar()->SetInitAccounts();
      return ret;
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class ProxySIPPConfigPage : public TablePConfigPage
{
  public:
    ProxySIPPConfigPage(PHTTPServiceProcess & app,const PString & title, const PString & section, const PHTTPAuthority & auth);
    virtual BOOL Post(PHTTPRequest & request, const PStringToString & data, PHTML & replyMessage)
    {
      BOOL ret = TablePConfigPage::Post(request, data, replyMessage);
      OpenMCU::Current().GetSipEndpoint()->SetInitProxyAccounts();
      return ret;
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class RoomAccessSIPPConfigPage : public TablePConfigPage
{
 public:
   RoomAccessSIPPConfigPage(PHTTPServiceProcess & app,const PString & title, const PString & section, const PHTTPAuthority & auth);
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class ExportPConfigPage : public TablePConfigPage
{
 public:
   ExportPConfigPage(PHTTPServiceProcess & app,const PString & title, const PString & section, const PHTTPAuthority & auth);
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class VideoPConfigPage : public TablePConfigPage
{
 public:
   VideoPConfigPage(PHTTPServiceProcess & app,const PString & title, const PString & section, const PHTTPAuthority & auth);
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class SIPCodecsPConfigPage : public TablePConfigPage
{
  public:
    SIPCodecsPConfigPage(PHTTPServiceProcess & app,const PString & title, const PString & section, const PHTTPAuthority & auth);
    virtual BOOL Post(PHTTPRequest & request, const PStringToString & data, PHTML & replyMessage)
    {
      BOOL ret = TablePConfigPage::Post(request, data, replyMessage);
      OpenMCU::Current().GetSipEndpoint()->SetInitCaps();
      return ret;
    }
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
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class JpegFrameHTTP : public PServiceHTTPString
{
  public:
    JpegFrameHTTP(OpenMCU & app, PHTTPAuthority & auth);
    BOOL OnGET (PHTTPServer & server, const PURL &url, const PMIMEInfo & info, const PHTTPConnectionInfo & connectInfo);

#if USE_LIBJPEG
    struct my_jpeg_compress_struct : jpeg_compress_struct
    {
      JpegFrameHTTP *context;
    };

    static void jpeg_init_destination(struct jpeg_compress_struct *cinfo)
    {
      MCUSimpleVideoMixer *jpegMixer = JpegFrameHTTP::GetMixer(cinfo);
      if(jpegMixer->myjpeg.GetSize() < 32768)
        jpegMixer->myjpeg.SetSize(32768);
      cinfo->dest->next_output_byte = &jpegMixer->myjpeg[0];
      cinfo->dest->free_in_buffer = jpegMixer->myjpeg.GetSize();
    }

    static boolean jpeg_empty_output_buffer(struct jpeg_compress_struct *cinfo)
    {
      MCUSimpleVideoMixer *jpegMixer = JpegFrameHTTP::GetMixer(cinfo);
      PINDEX oldsize = jpegMixer->myjpeg.GetSize();
      jpegMixer->myjpeg.SetSize(oldsize + 16384);
      cinfo->dest->next_output_byte = &jpegMixer->myjpeg[oldsize];
      cinfo->dest->free_in_buffer = jpegMixer->myjpeg.GetSize() - oldsize;
      return true;
    }

    static void jpeg_term_destination(struct jpeg_compress_struct *cinfo)
    {
      MCUSimpleVideoMixer *jpegMixer = JpegFrameHTTP::GetMixer(cinfo);
      jpegMixer->jpegSize = jpegMixer->myjpeg.GetSize() - cinfo->dest->free_in_buffer;
      jpegMixer->jpegTime = (long)time(0);
    }

    static MCUSimpleVideoMixer * GetMixer(struct jpeg_compress_struct *cinfo)
    {
      my_jpeg_compress_struct *my_cinfo = static_cast<my_jpeg_compress_struct *>(cinfo);
      return my_cinfo->context->jpegMixer;
    }
#endif

  private:
    OpenMCU & app;
    PMutex jpegMixerMutex;
    MCUSimpleVideoMixer *jpegMixer;
};

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
