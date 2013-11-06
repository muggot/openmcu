var fortyTwo=42

  ,STEPS_TO_REMEMBER = 2
  ,START_DELAY       = 150
  ,UPDATE_INTERVAL   = 5000
  ,UPDATE_RETRIES    = 5
  ,REQUEST_TIMEOUT   = 4000
  ,DATA_HANDLE_DELAY = 25
  ,WORKPLACE         = null
  ,OFFLINE_PREFIX    = "<B>[Offline] </B>"
  ,OFFLINE_SUFFIX    = ""
  ,HIDDEN_PREFIX     = "<B>[Hidden] </B>"
  ,HIDDEN_SUFFIX     = ""
  ,AUDIO_OUT_STR     = "Audio Out"
  ,AUDIO_IN_STR      = "Audio In"
  ,VIDEO_OUT_STR     = "Video Out"
  ,VIDEO_IN_STR      = "Video In"
  ,DAYS_STR          = "day(s)"
  ,COL_NAME          = "Name"
  ,COL_DURATION      = "Duration"
  ,COL_RTP           = "RTP Channel: Codec"
  ,COL_PACKETS       = "Packets"
  ,COL_BYTES         = "Bytes"
  ,COL_KBPS          = "Kbit/s"
  ,COL_FPS           = "FPS"
  ,WORD_ROOM         = "Room"
  ,FILE_RECORDER_NAME= "file recorder"
  ,CACHE_NAME        = "cache"
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
  ,cache_fps         = []
  ;

  if (typeof window.l_connections_OFFLINE_PREFIX != 'undefined')
  {
    OFFLINE_PREFIX = window.l_connections_OFFLINE_PREFIX;
    OFFLINE_SUFFIX = window.l_connections_OFFLINE_SUFFIX;
    HIDDEN_PREFIX  = window.l_connections_HIDDEN_PREFIX ;
    HIDDEN_SUFFIX  = window.l_connections_HIDDEN_SUFFIX ;
    AUDIO_OUT_STR  = window.l_connections_AUDIO_OUT_STR ;
    AUDIO_IN_STR   = window.l_connections_AUDIO_IN_STR  ;
    VIDEO_OUT_STR  = window.l_connections_VIDEO_OUT_STR ;
    VIDEO_IN_STR   = window.l_connections_VIDEO_IN_STR  ;
    DAYS_STR       = window.l_connections_DAYS_STR      ;
    COL_NAME       = window.l_connections_COL_NAME      ;
    COL_DURATION   = window.l_connections_COL_DURATION  ;
    COL_RTP        = window.l_connections_COL_RTP       ;
    COL_PACKETS    = window.l_connections_COL_PACKETS   ;
    COL_BYTES      = window.l_connections_COL_BYTES     ;
    COL_KBPS       = window.l_connections_COL_KBPS      ;
    COL_FPS        = window.l_connections_COL_FPS       ;
    WORD_ROOM      = window.l_connections_word_room     ;
  }


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
  var visible = (memberName!=CACHE_NAME) && (memberName!=FILE_RECORDER_NAME);
  var searchStr = (online?"":OFFLINE_PREFIX) + (visible?"":HIDDEN_PREFIX) + memberName + (visible?"":HIDDEN_SUFFIX) + (online?"":OFFLINE_SUFFIX);
  for(var i=objTable.rows.length-1; i>=0; i--)
  {
    if(objTable.rows[i].cells[0].innerHTML == searchStr) return i;
  }
//special for firefox:
  searchStr = searchStr.toLowerCase(); for(var i=objTable.rows.length-1; i>=0; i--) if(objTable.rows[i].cells[0].innerHTML.toLowerCase() == searchStr) return i;
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
  if(m[1]==FILE_RECORDER_NAME) return "-";
  if(m[1]==CACHE_NAME) return "<nobr><b>" + VIDEO_OUT_STR + ":</b> " + m[14] + "</nobr>";

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
  if(m[1]==CACHE_NAME) return "-";
  if(m[1]==FILE_RECORDER_NAME) return "-";
  return m[18] + "<br>" + m[19] + "<br>" + m[20] + "<br>" + m[21];
}

function member_get_nice_bytes(m)
{
  if(!m[0]) return "-";
  if(m[1]==CACHE_NAME) return "-";
  if(m[1]==FILE_RECORDER_NAME) return "-";
  return m[5] + "<br>" + m[6] + "<br>" + m[7] + "<br>" + m[8];
}

function member_get_nice_kbps(roomName, m)
{
  if(!m[0]) return "-";
  if(m[4]<=0) return "-";
  if(m[1]==CACHE_NAME) return "-";
  if(m[1]==FILE_RECORDER_NAME) return "-";

  if(store.length>1)
  {
    var s=store[store.length-2];
    for(var i=0;i<s.length;i++)
    {
      if(s[i][0]==roomName)
      {
        var j=0;
        while(1)
        {
          var m0=null; try { if(typeof s[i][4][j] != 'undefined') m0=s[i][4][j]; } catch(e) {}
          if(typeof m0 == 'undefined') break; if(m0==null) break;
          if(m0[0]) if(m0[1] == m[1])
          {
            var ms=m[4]-m0[4];
            if(ms<=1) return ":(";
            return "<font color='#001'>" +
              integer_pad_float( (m[5]-m0[5])*8 / ms, 1) + "<br>" +
              integer_pad_float( (m[6]-m0[6])*8 / ms, 1) + "<br>" +
              integer_pad_float( (m[7]-m0[7])*8 / ms, 1) + "<br>" +
              integer_pad_float( (m[8]-m0[8])*8 / ms, 1) + "</font>";
          }
          j++;
        }
      }
    }
  }

  return "<font color='gray'>" +
    integer_pad_float(m[5] * 8 / m[4], 1) + "<br>" +
    integer_pad_float(m[6] * 8 / m[4], 1) + "<br>" +
    integer_pad_float(m[7] * 8 / m[4], 1) + "<br>" +
    integer_pad_float(m[8] * 8 / m[4], 1) + "</font>";
}

function member_get_nice_fps(m)
{
  if(!m[0]) return "-";
  if(m[4]<=0) return "-";
  if(m[1]==FILE_RECORDER_NAME) return "-";
  if(m[1]==CACHE_NAME) return "<nobr><b><font color='green'>" + m[17] + " x</font> </b>" + integer_pad_float(m[16], 2) + "</nobr>";
  var s="<br><br>" +  integer_pad_float(m[15], 2) + "<br>";
  if(m[13]==2)
  {
    return s + member_get_cache_fps(m);
  } else return s+integer_pad_float(m[16],2);
}

function member_get_cache_fps(m)
{
  var fs = m[14]; // formstString
  var fps='';
  try
  {
    if(typeof cache_fps[fs] != 'undefined') fps=cache_fps[fs];
  } catch(e) {}
  if(fps!='') return fps;

  fps="<font color='red'>err</font>";
  cache_fps[fs]=fps;

  var i=0, roomName = fs.substring(fs.indexOf('_')+1, fs.lastIndexOf('/'));
  while (i<roomCount) if(data[i][0] = roomName) break; else i++;
  if(i==roomCount) return fps;

  var j=0;
  while(1)
  {
    var c=null; try { if(typeof data[i][4][j] != 'undefined') c=data[i][4][j]; } catch(e) {}
    if(typeof c == 'undefined') return fps;
    if(c==null) return fps;
    if(!c[0]) { j++; continue; }
    if((c[13] != 1) || (c[14] != fs)) { j++; continue; }

    fps="<font color='green'>" + integer_pad_float(c[16],2) + "</font>";
    cache_fps[fs]=fps;
    return fps;
  }
  return fps;
}

function integer_pad_float(v, f)
{
  var m=10; if(f==2) m=100; if(f==3) m=1000;
  var s=""+(Math.round(v*m)/m)+"";
  if(s.indexOf('.') == -1) s+='.';
  while(s.length - (s.lastIndexOf('.')) < f+1) s+="0";
  return s;
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
  d.innerHTML='<b>' + WORD_ROOM + " " + r + '</b>'
    + '<table id="r_t_' + r + '" class="table table-striped table-bordered table-condensed">'
    + "<tr>"
      + "<th>&nbsp;"+COL_NAME    +"&nbsp;</th>"
      + "<th>&nbsp;"+COL_DURATION+"&nbsp;</th>"
      + "<th>&nbsp;"+COL_RTP     +"&nbsp;</th>"
      + "<th>&nbsp;"+COL_PACKETS +"&nbsp;</th>"
      + "<th>&nbsp;"+COL_BYTES   +"&nbsp;</th>"
      + "<th>&nbsp;"+COL_KBPS    +"&nbsp;</th>"
      + "<th>&nbsp;"+COL_FPS     +"&nbsp;</th>"
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

  try { if(typeof member[2] == 'undefined') member[2]=1; } catch(e) {}

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
  td.innerHTML = member_get_nice_kbps(room, member);

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
  t.rows[row].cells[5].innerHTML = member_get_nice_kbps(room, member);
  t.rows[row].cells[6].innerHTML = member_get_nice_fps(member);
}

function handle_data()
{ // new data block 'data' just received from server
  cache_fps = [];

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
}

WORKPLACE=document.getElementById('status1');
timer=setTimeout(get_data, START_DELAY);
