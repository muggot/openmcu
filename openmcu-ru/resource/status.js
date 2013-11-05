var fortyTwo=42

  ,STEPS_TO_REMEMBER = 10
  ,START_INTERVAL    = 1500
  ,UPDATE_INTERVAL   = 7000
  ,UPDATE_RETRIES    = 5
  ,REQUEST_TIMEOUT   = 4000
  ,DATA_HANDLE_DELAY = 333
  ,WORKPLACE         = null
  ,OFFLINE_PREFIX    = "<B>[Offline] </B>" //localize it :)
  ,OFFLINE_SUFFIX    = ""
  ,HIDDEN_PREFIX     = "<B>[Hidden] </B>"
  ,HIDDEN_SUFFIX     = ""
  ,AUDIO_OUT_STR     = "Audio Out"
  ,AUDIO_IN_STR      = "Audio In"
  ,VIDEO_OUT_STR     = "Video Out"
  ,VIDEO_IN_STR      = "Video In"
  ,DAYS_STR          = "day(s)"
  ,data              = []
  ,gettingData       = 0
  ,getDataErrorCount = 0
  ,requestTimer      = null
  ,roomCount         = 0
  ,roomList          = []
  ,memberList        = []
  ,store             = []
  ,timer             = null
  ,xro               = null
  ;

function in_array(needle, haystack)
{ for(var i=0; i<haystack.length; i++) if(haystack[i]==needle) return true;
  return false;
}

function getTableByRoomName(roomName)
{
  var t=null;
  try
  {
    t=document.getElementById('r_t_'+roomName);
  }
  catch(e)
  {
    t=null;
  }
  return t;
}

function findMemberInTable(objTable, memberName, memberId)
{
  var online = memberId != 0;
  var searchStr = (online?"":OFFLINE_PREFIX) + memberName + (online?"":OFFLINE_SUFFIX);
  for(var i=objTable.rows.length-1; i>=0; i--)
  {
    if(objTable.rows[i].cells[0].innerHTML == searchStr) return i;
  }
  return -1;
}

function member_get_nice_duration(m)
{
  if(!m[0]) return "-";
  var d = m[4] + 0;
  if(m<1000) return "0."+m;
  var f=d%1000; var s=Math.floor(d/1000);
  if(s<60) return ""+s+"."+Math.floor(f/100);
  var m=Math.floor(s/60); s=s%60;
  var line=":"; if(s<10) line+="0"; line+=s;

  if(m<60) return ""+m+line;
  var h=Math.floor(m/60); m=m%60;
  if(m<10) m="0"+m+"";
  line=":"+m+line;
  if(h<24) return h+line;

  var d=Math.floor(h/24); h=h%24;
  if(h<10) h="0"+h+"";
  line=""+h+line+"";

  return "" + d + DAYS_STR + " " + line;
}

function member_get_nice_codecs(m)
{
  if(!m[0]) return "-";
  if(m[1]=="file recorder") return "-";
  if(m[1]=="cache") return "<nobr><b>" + VIDEO_OUT_STR + ":</b> " + m[14] + "</nobr>";

  var s=
    "<nobr><b>" + AUDIO_IN_STR  + ":</b> " + m[ 9] + "</nobr><br>" +
    "<nobr><b>" + AUDIO_OUT_STR + ":</b> " + m[10] + "</nobr><br>" +
    "<nobr><b>" + VIDEO_IN_STR  + ":</b> " + m[11] + "</nobr><br>";
  var s2="<nobr><b>"; if(m[13]==2) s2+="<font color='green'>";
  s2+=VIDEO_OUT_STR; if(m[13]==2) s2+="</font>";
  s2+=":</b> " + m[12] + "</nobr>";
  return s+s2;
}

function member_get_nice_packets(m)
{
  if(!m[0]) return "-";
  if(m[1]=="cache") return "-";
  if(m[1]=="file recorder") return "-";
  return m[18] + "<br>" + m[19] + "<br>" + m[20] + "<br>" + m[21];
}

function member_get_nice_bytes(m)
{
  if(!m[0]) return "-";
  if(m[1]=="cache") return "-";
  if(m[1]=="file recorder") return "-";
  return m[5] + "<br>" + m[6] + "<br>" + m[7] + "<br>" + m[8];
}

function member_get_nice_kbps(m)
{
  if(!m[0]) return "-";
  if(m[4]<=0) return "-";
  if(m[1]=="cache") return "-";
  if(m[1]=="file recorder") return "-";
  return "" +
    Math.floor(m[5] * 80 / m[4] + 0.5)/10 + "<br>" +
    Math.floor(m[6] * 80 / m[4] + 0.5)/10 + "<br>" +
    Math.floor(m[7] * 80 / m[4] + 0.5)/10 + "<br>" +
    Math.floor(m[8] * 80 / m[4] + 0.5)/10;
}

function member_get_nice_fps(m)
{
  if(!m[0]) return "-";
  if(m[4]<=0) return "-";
  if(m[1]=="file recorder") return "-";
  if(m[1]=="cache") return "<nobr><b><font color='green'>" + m[17] + " x</font> </b>" + Math.floor(m[16]*100 + 0.55)/100 + "</nobr>";
  return "<br><br>" +
    Math.floor(m[15]*100 + 0.55)/100 + "<br>" + 
    Math.floor(m[16]*100 + 0.55)/100 + "<br>";
}


function add_unique(needle, haystack)
{ for(var i=0; i<haystack.length; i++) if(haystack[i]==needle) return haystack;
  haystack.push(needle);
  return haystack;
}

function createRequestObject()
{ if (typeof XMLHttpRequest==='undefined')
  { XMLHttpRequest=function()
    { try { return new ActiveXObject("Msxml2.XMLHTTP.6.0"); } catch(e) {}
      try { return new ActiveXObject("Msxml2.XMLHTTP.3.0"); } catch(e) {}
      try { return new ActiveXObject("Msxml2.XMLHTTP")    ; } catch(e) {}
      try { return new ActiveXObject("Microsoft.XMLHTTP") ; } catch(e) {}
      return false;
    }
  }
  return new XMLHttpRequest();
}

function show_error()
{
  alert('Communitation lost');
  location.href='/';
}

function get_data()
{ if(gettingData)
  {
    if((getDataErrorCount++) <= UPDATE_RETRIES) timer=setTimeout(get_data, UPDATE_INTERVAL);
    else show_error();
    return;
  }
  gettingData=1;
  if(xro == null) xro=createRequestObject();
  requestTimer=setTimeout(get_data_fail, REQUEST_TIMEOUT);
  xro.open('GET','Status/?js',true);
  xro.onreadystatechange=got_data;
  xro.send(null);
}

function get_data_fail()
{ xro.abort();
  if((getDataErrorCounter++) <= UPDATE_RETRIES) timer=setTimeout(get_data, UPDATE_INTERVAL);
  else show_error();
}

function got_data()
{ try
  {
    if(xro.readyState!=4) return;
    clearTimeout(requestTimer);
    if(xro.status==200)
    {
      eval("data="+xro.responseText+";");
      while(store.length >= STEPS_TO_REMEMBER) on_delete_data(store.shift());
      store.push(data);
      getDataErrorCounter=0;
    }
    else
    {
      getDataErrorCount++;
    }
    xro.abort();
    gettingData=0;
    if(getDataErrorCount <= UPDATE_RETRIES) timer=setTimeout(get_data, UPDATE_INTERVAL);
    else show_error();
  }
  catch(e){}
  if(!getDataErrorCount) setTimeout(handle_data, DATA_HANDLE_DELAY);
}

function on_delete_data(d)
{
}

function on_create_new_room(r)
{
  var s='r_b_'+r; on_delete_room(r);

  var d=document.createElement('DIV');
  d.id=s;
  d.innerHTML='<b>' + r + '</b>'
    + '<table id="r_t_' + r + '" class="table table-striped table-bordered table-condensed">'
    + "<tr>"
      + "<th>&nbsp;Name&nbsp;</th>"
      + "<th>&nbsp;Duration&nbsp;</th>"
      + "<th>&nbsp;RTP Channel: Codec&nbsp;</th>"
      + "<th>&nbsp;Packets&nbsp;</th>"
      + "<th>&nbsp;Bytes&nbsp;</th>"
      + "<th>&nbsp;Kbit/s&nbsp;</th>"
      + "<th>&nbsp;FPS&nbsp;</th>"
    + "</tr>"

    + '</table>';
  WORKPLACE.appendChild(d);
}

function on_delete_room(r)
{
  var s='r_b_'+r;
  try { WORKPLACE.removeChild(document.getElementById(s)); } catch(e) {}
}

function on_member_add(room, member)
{
  var t=getTableByRoomName(room); if(t==null) return;

  var
    memberName  = member[1],
    online      = member[0]!=0, 
    visible     = member[2], 
    tr          = t.insertRow(-1);

  var td=tr.insertCell(0);
  td.innerHTML = (online?"":OFFLINE_PREFIX) + (visible?"":HIDDEN_PREFIX) + member[1] + (visible?"":HIDDEN_SUFFIX) + (online?"":OFFLINE_SUFFIX);

  td=tr.insertCell(1); //Duration
  td.style.textAlign='right';
  td.innerHTML = member_get_nice_duration(member);

  td=tr.insertCell(2); //RTP Channel: Codec
  td.innerHTML = member_get_nice_codecs(member);

  td=tr.insertCell(3); //Packets
  td.style.textAlign='right';
  td.innerHTML = member_get_nice_packets(member);

  td=tr.insertCell(4); //Bytes
  td.style.textAlign='right';
  td.innerHTML = member_get_nice_bytes(member);

  td=tr.insertCell(5); //Kbit/s
  td.style.textAlign='right';
  td.innerHTML = member_get_nice_kbps(member);

  td=tr.insertCell(6); //FPS
  td.style.textAlign='right';
  td.innerHTML = member_get_nice_fps(member)
}

function on_member_delete(key)
{
  var t=null, a = key.split("\t",3);
  var roomName = a[0];
  var t=getTableByRoomName(roomName); if(t==null) return;

  var row = findMemberInTable(t, a[2], a[1]);
  if(row>=0) t.deleteRow(row);
}

function update_member(room, member)
{
  var t=getTableByRoomName(room); if(t==null) return;
  var row = findMemberInTable(t, member[1], member[0]);
  if(row<0) return;
  t.rows[row].cells[1].innerHTML = member_get_nice_duration(member);
  t.rows[row].cells[2].innerHTML = member_get_nice_codecs(member);
  t.rows[row].cells[3].innerHTML = member_get_nice_packets(member);
  t.rows[row].cells[4].innerHTML = member_get_nice_bytes(member);
  t.rows[row].cells[5].innerHTML = member_get_nice_kbps(member);
  t.rows[row].cells[6].innerHTML = member_get_nice_fps(member);
}

function handle_data()
{ // new data block 'data' just received from server
  roomCount=data.length;
  var newRooms=[], i=0;
  for(i=0; i<roomCount; i++)
  {
    newRooms.push(data[i][0]);
    if(!in_array(data[i][0], roomList))
    {
      on_create_new_room(data[i][0]);
      roomList.push(data[i][0]);
    }
  }
  for(i=roomList.length-1; i>=0; i--)
  {
    if(!in_array(roomList[i], newRooms))
    {
      on_delete_room(roomList[i]);
      roomList.splice(i,1);
    }
  }

  var newMemberList=[];

  for(i=0; i<roomCount; i++)
  {
    var j=0, roomName=data[i][0];
    while(1)
    {
      var m=null; try { if(typeof data[i][4][j] != 'undefined') m=data[i][4][j]; } catch(e) {}
      if(typeof m == 'undefined') break; if(m==null) break;
      var memberKey = roomName + "\t" + m[0] + "\t" + m[1];
      newMemberList.push(memberKey);
      if(!in_array(memberKey, memberList))
      {
        on_member_add(roomName, m);
        memberList.push(memberKey);
      }
      else
      {
        if(m[0]) update_member(roomName, m);
      }
      j++;
    }
  }

  for(i=memberList.length-1; i>=0; i--)
  {
    if(!in_array(memberList[i], newMemberList))
    {
      on_member_delete(memberList[i]);
      memberList.splice(i,1);
    }
  }
//  alert(data);

}

function status_update_start()
{
  WORKPLACE=document.getElementById('status1');
  timer=setTimeout(get_data, START_INTERVAL);
}
