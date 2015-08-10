var fortyTwo=42

  ,STEPS_TO_REMEMBER = 12
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
  ,AI_NEG_ERR        = "A/I Neg. Error"
  ,AO_NEG_ERR        = "A/O Neg. Error"
  ,VI_NEG_ERR        = "V/I Neg. Error"
  ,VO_NEG_ERR        = "V/O Neg. Error"
  ,BUTTON_TEXT       = "Get Text"
  ,BUTTON_FORUM      = "Get BBCode"
  ,BUTTON_HTML       = "Get HTML"
  ,BUTTON_CLOSE      = "X"
  ,CODE_TOOLTIP      = "Ctrl+C to copy"
  ,DAYS_STR          = "day(s)"
  ,COL_NAME          = "Name"
  ,COL_DURATION      = "Duration"
  ,COL_RTP           = "RTP Channel: Codec"
  ,COL_PACKETS       = "Packets"
  ,COL_BYTES         = "Bytes"
  ,COL_KBPS          = "Kbit/s"
  ,COL_LOSTPCN       = "60s losses"
  ,COL_FPS           = "FPS"
  ,WORD_ROOM         = "Room"
  ,FILE_RECORDER_NAME= "file recorder"
  ,CACHE_NAME        = "cache"
  ,RECORDER_NAME     = "conference recorder"
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
  ,haveCode          = 0
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
    AI_NEG_ERR     = window.l_connections_AI_NEG_ERR    ;
    AO_NEG_ERR     = window.l_connections_AO_NEG_ERR    ;
    VI_NEG_ERR     = window.l_connections_VI_NEG_ERR    ;
    VO_NEG_ERR     = window.l_connections_VO_NEG_ERR    ;
    BUTTON_TEXT    = window.l_connections_BUTTON_TEXT   ;
    BUTTON_FORUM   = window.l_connections_BUTTON_FORUM  ;
    BUTTON_HTML    = window.l_connections_BUTTON_HTML   ;
    BUTTON_CLOSE   = window.l_connections_BUTTON_CLOSE  ;
    CODE_TOOLTIP   = window.l_connections_CODE_TOOLTIP  ;
    DAYS_STR       = window.l_connections_DAYS_STR      ;
    COL_NAME       = window.l_connections_COL_NAME      ;
    COL_DURATION   = window.l_connections_COL_DURATION  ;
    COL_RTP        = window.l_connections_COL_RTP       ;
    COL_PACKETS    = window.l_connections_COL_PACKETS   ;
    COL_BYTES      = window.l_connections_COL_BYTES     ;
    COL_KBPS       = window.l_connections_COL_KBPS      ;
    COL_FPS        = window.l_connections_COL_FPS       ;
    WORD_ROOM      = window.l_connections_word_room     ;
    COL_LOSTPCN    = window.l_connections_COL_LOSTPCN   ;
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
  var visible = (memberName!=CACHE_NAME) && (memberName!=FILE_RECORDER_NAME) && (memberName!=RECORDER_NAME);
  var searchStr = (online?"":OFFLINE_PREFIX) + (visible?"":HIDDEN_PREFIX) + memberName + (visible?"":HIDDEN_SUFFIX) + (online?"":OFFLINE_SUFFIX);
  for(var i=objTable.rows.length-1; i>=0; i--)
  {
    var m=objTable.rows[i].cells[0].innerHTML;
    var ltPos=m.indexOf('<');
    if(ltPos>0) m=m.substr(0,ltPos);
    while((m.charAt(m.length-1)<' ')&&(m.length)) m=m.substr(0,m.length-1);
    if(m == searchStr) if(objTable.rows[i].cells[0].id==memberId) return i;
  }
//special for firefox:
  searchStr = searchStr.toLowerCase();
  for(var i=objTable.rows.length-1; i>=0; i--)
  {
    var m=objTable.rows[i].cells[0].innerHTML;
    var ltPos=m.indexOf('<');
    if(ltPos>0) m=m.substr(0,ltPos);
    if(m.toLowerCase() == searchStr) if(objTable.rows[i].cells[0].id==memberId)  return i;
  }
  return -1;
}

function member_get_nice_name(m)
{
  var online=m[0]!=0;
  var visible=m[2];
  return "" +
    (online?"":OFFLINE_PREFIX) + (visible?"":HIDDEN_PREFIX) +
    m[1] +
    (visible?"":HIDDEN_SUFFIX) + (online?"":OFFLINE_SUFFIX) +
    ((online&&visible)?("<p class='remapp'>"+m[22]+"</p>"):"");
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

function member_get_nice_stream(isAudio, isCached, streamName, streamText, streamNegErrorText)
{ // функция посвящается уважаемому palexa, http://openmcu.ru/forum/index.php/topic,611.0.html
  // её можно и нужно настроить по своему усмотрению, как это сделать - см. комментарии далее:

  var r='<nobr><b>'; // будем формировать строку <nobr><b>ПОТОК:</b> кодек</nobr>

  var atPos=streamText.lastIndexOf('@'); // ищем @, делим streamText на две части
  var leftPart=streamText;
  var rightPart='';
  if(atPos!=-1)
  {
    leftPart=streamText.substr(0,atPos);
    rightPart=streamText.substr(atPos+1);
  }

  var negError = ((leftPart=='none')||(rightPart=='0x0')); // слева none или справа 0x0 - признаки ошибки согласования

  if(negError) // если ошибка, то красим красным, текст ошибки
  {
    r+="<font color='red'>"+streamNegErrorText+"</font>";
  }
  else // иначе красим как обычно и текст обычный
  {
    if(isCached) r+="<font color='green'>";
    r+=streamName;
    if(isCached) r+="</font>";
  }

  r+=":</b> "; // ну и так далее и тому подобное :)
  if(negError)
  {
    if(leftPart=='none') r+="<font color='brown'>"+streamText+"</font>";
    else                 r+="<font color='red'>" +streamText+"</font>";
  }
  else
  {
    r+=streamText;
  }

  r+="</nobr>";
  return r;
}

function member_get_nice_codecs(m)
{
  if(!m[0]) return "-";
  if(m[1]==RECORDER_NAME) return "-";
  if(m[1]==FILE_RECORDER_NAME) return "-";
  if(m[1]==CACHE_NAME && m[27] == 0) return "<nobr><b>" + VIDEO_OUT_STR + ":</b> " + m[14] + "</nobr>";
  if(m[1]==CACHE_NAME && m[27] == 1) return "<nobr><b>" + AUDIO_OUT_STR + ":</b> " + m[14] + "</nobr>";

  return member_get_nice_stream(1,0         ,AUDIO_IN_STR ,m[ 9], AI_NEG_ERR ) + "<br>" +
         member_get_nice_stream(1,0         ,AUDIO_OUT_STR,m[10], AO_NEG_ERR) + "<br>" +
         member_get_nice_stream(0,0         ,VIDEO_IN_STR ,m[11], VI_NEG_ERR ) + "<br>" +
         member_get_nice_stream(0,(m[13]==2),VIDEO_OUT_STR,m[12],VO_NEG_ERR);
}

function member_get_nice_packets(m)
{
  if(!m[0]) return "-";
  if(m[1]==CACHE_NAME) return "-";
  if(m[1]==RECORDER_NAME) return "-";
  if(m[1]==FILE_RECORDER_NAME) return "-";
  var plost='', vplost='', plostTx='', vplostTx='';
  if(m[23])  plost  ="<font color='red'>/"+m[23]+"</font>";
  if(m[24]) vplost  ="<font color='red'>/"+m[24]+"</font>";
  if(m[25])  plostTx="<font color='red'>/"+m[25]+"</font>";
  if(m[26]) vplostTx="<font color='red'>/"+m[26]+"</font>";
  return m[18]+plost + "<br>" + m[19]+plostTx + "<br>" + m[20]+vplost + "<br>" + m[21]+vplostTx;
}

function member_get_nice_bytes(m)
{
  if(!m[0]) return "-";
  if(m[1]==CACHE_NAME) return "-";
  if(m[1]==RECORDER_NAME) return "-";
  if(m[1]==FILE_RECORDER_NAME) return "-";
  return m[5] + "<br>" + m[6] + "<br>" + m[7] + "<br>" + m[8];
}

function wrong_check(m)
{
  return (!m[0]) || (m[4]<=0) || (m[1]==CACHE_NAME) || (m[1]==FILE_RECORDER_NAME) || (m[1]==RECORDER_NAME);
}

function member_get_nice_kbps(roomName, m)
{
  if(wrong_check(m)) return '-';
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

function calc_lost_percent(l1, l0, p1, p0)
{
  var t1=l1+p1, t0=l0+p0;
  var dt=t1-t0, dl=l1-l0;
  if((dl<0)||(dt<=1)) return ':(';
  if(dl==0) return '0%';
  var p=dl*100/dt;
  var s;
  if(p<0.1)      s="<font color='#409541'>%</font>";
  else if(p<1)   s="<font color='#727140'>%</font>";
  else if(p<3)   s="<font color='#FF0000'>%</font>";
  else           s="<font color='#FF0000'><b>% !!!</b></font>";
  return s.replace(/(%)/g, (""+integer_pad_float(p,2)+"%"));
}

function member_get_nice_lost_percent(roomName, m)
{
  var sl=store.length, ui=UPDATE_INTERVAL;
  if(wrong_check(m) || sl<=1) return '-';
  if(ui<1) ui=1;
  if(sl>=60000/ui) sl=Math.round(60000/ui);
  if(sl>store.length) sl=store.length;
  var s=store[store.length-sl];
  var i, j=0, m0=null;
  for(i=0;i<s.length;i++) if(s[i][0]==roomName) break;
  if(s[i][0]!=roomName) return '-';
  while(m0===null)
  {
    try { if(typeof s[i][4][j] != 'undefined') m0=s[i][4][j]; } catch(e) {}
    if(typeof m0 == 'undefined') return '-';
    if(m0==null) return '-';
    if(m[0]) if(m0[1] == m[1]) break;
    j++;
    if(j<9999) m0=null; else return 'err_mem10K';
  }
  if(m0===null) return '-';
  if(!m0[0]) return '-';

  var ms=m[4]-m0[4];
  if(ms<=1) return ":(";

  return ""+
    calc_lost_percent(m[23], m0[23], m[18], m0[18]) + "<br/>" +
    calc_lost_percent(m[25], m0[25], m[20], m0[20]) + "<br/>" +
    calc_lost_percent(m[24], m0[24], m[19], m0[19]) + "<br/>" +
    calc_lost_percent(m[26], m0[26], m[21], m0[21]);
}

function member_get_nice_fps(m)
{
  if(!m[0]) return "-";
  if(m[4]<=0) return "-";
  if(m[1]==RECORDER_NAME) return "-";
  if(m[1]==FILE_RECORDER_NAME) return "-";
  if(m[1]==CACHE_NAME && m[27] == 0) return "<nobr><b><font color='green'>" + m[17] + " x</font> </b>" + integer_pad_float(m[16], 2) + "</nobr>";
  if(m[1]==CACHE_NAME && m[27] == 1) return "<nobr><b><font color='green'>" + m[17] + "</font> </b></nobr>";
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
  while (i<roomCount) if(data[i][0] == roomName) break; else i++;
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
  var dp=s.lastIndexOf('.');
  while(s.length - dp < f+1) s+="0";
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
  if((getDataErrorCount++) <= UPDATE_RETRIES) timer=setTimeout(get_data, UPDATE_INTERVAL);
  else show_error();
}

function sort_rooms(a,b)
{
  if(a[0]>b[0]) return 1;
  if(a[0]<b[0]) return -1;
  return 0;
}

function get_order_key(m)
{
  if(m[1]==RECORDER_NAME) return "0";
  if(m[1]==FILE_RECORDER_NAME) return "0";
  var r="1";
  var cache=(m[1]==CACHE_NAME);
  var cc; if(cache)cc=m[17]; // cache capacity
  if(cache)if(cc)r+="0";else r+="3";else if(m[0])r+="1";else r+="2";
  if(cache)
  {
    if(cc)
    {
      var pad='0000';
      cc=""+(9999-cc);
      return r+pad.substr(0,4-cc.length)+cc+m[14];
    }
    else return r+m[14];
  }
  return r+m[1];
}

function sort_members(a,b)
{
  var a0=get_order_key(a), b0=get_order_key(b);
  if(a0<b0) return -1;
  if(a0>b0) return  1;
  return 0
}

function data_sort_function(data)
{
  data.sort(sort_rooms);
  var i;
  for(i=0;i<data.length;i++)
  {
    data[i][4].sort(sort_members);
  }
  return data;
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
      data=data_sort_function(data);
      store.push(data);
      getDataErrorCount=0;
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

function RoomControlPage(r)
{
  document.getElementById('clicker').room.value=decodeURIComponent(r);
  document.getElementById('clicker').submit();
}

function on_create_new_room(r)
{
  var s='r_b_'+r; on_delete_room(r);

  var d=document.createElement('DIV');
  d.id=s;
  d.innerHTML="<p onclick='javascript:RoomControlPage(\""+encodeURIComponent(r)+"\")' class='roomname'>" + WORD_ROOM + " " + r + "</p>"
    + '<table id="r_t_' + r + '" class="table table-striped table-bordered table-condensed">'
    + "<tr>"
      + "<th>&nbsp;"+COL_NAME    +"&nbsp;</th>"
      + "<th>&nbsp;"+COL_DURATION+"&nbsp;</th>"
      + "<th>&nbsp;"+COL_RTP     +"&nbsp;</th>"
      + "<th>&nbsp;"+COL_PACKETS +"&nbsp;</th>"
      + "<th>&nbsp;"+COL_BYTES   +"&nbsp;</th>"
      + "<th>&nbsp;"+COL_KBPS    +"&nbsp;</th>"
      + "<th>&nbsp;"+COL_FPS     +"&nbsp;</th>"
      + "<th>&nbsp;"+COL_LOSTPCN +"&nbsp;</th>"
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

  var tr=t.insertRow(-1);

  var td=tr.insertCell(0); //Name
  td.innerHTML = member_get_nice_name(member);
  td.id=member[0];

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

  td=tr.insertCell(7); //Lost%
  td.style.textAlign='right';
  td.innerHTML = member_get_nice_lost_percent(room, member)
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
  t.rows[row].cells[0].innerHTML = member_get_nice_name(member);
  t.rows[row].cells[1].innerHTML = member_get_nice_duration(member);
  t.rows[row].cells[2].innerHTML = member_get_nice_codecs(member);
  t.rows[row].cells[3].innerHTML = member_get_nice_packets(member);
  t.rows[row].cells[4].innerHTML = member_get_nice_bytes(member);
  t.rows[row].cells[5].innerHTML = member_get_nice_kbps(room, member);
  t.rows[row].cells[6].innerHTML = member_get_nice_fps(member);
  t.rows[row].cells[7].innerHTML = member_get_nice_lost_percent(room, member)
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

function get_code(codeType)
{
  if(haveCode) return;
  haveCode=1;
  var text='', result='', openTag='', closeTag='';
  if(codeType==1)
  {
    openTag='[';
    closeTag=']';
  }
  else if(codeType==2)
  {
    openTag='<';
    closeTag='>';
  }
  var rooms=WORKPLACE.getElementsByTagName('div');
  for (var i=0;i<rooms.length;i++)
  {
    var roomName=rooms[i].id;
    if(roomName.substr(0,4)!='r_b_') continue;
    text+=rooms[i].innerHTML;
  }
  var i=0, skip=0, openFont='font', cellCount=0;

  while(i<text.length)
  {
    var c=text.charAt(i);
    if(c=='<')
    {
      var j=text.indexOf('>',i+1);
      if(j==-1) break;
      var tag=text.substring(i+1,j).toLowerCase();
      i=j;
      var close=(tag.charAt(0)=='/');
      if(close) tag=tag.substr(1);
      var offs=0;
      if(tag.substr(0,4)=='font') offs=5;
      j=tag.indexOf('/',offs);
      if(j>0) tag=tag.substr(0,j);
      j=tag.indexOf(' ',offs);
      if(j>0) tag=tag.substr(0,j);
      if(tag.length>0)
      {
        if(!codeType)
        {
          if(close)
          {
                 if(tag=="th")    { skip=0; }
            else if(tag=="table") { result+="\n\n"; }
          }
          else
          {
                 if(tag=="tr")    { cellCount=0; result+="\n"; }
            else if(tag=="p" )    { if(cellCount==1) result+=" -- "; else result+="\n"; }
            else if(tag=="td")    { cellCount++; if(cellCount>1) result+='; '; }
            else if(tag=="th")    { cellCount++; skip=1; }
            else if(tag=="br")    { result+="|"; }
          }
          i++;
          continue;
        }
        if(codeType==1)
        {
          if(tag=='th') tag='td';
          else
          if(tag.substr(0,10)=='font color')
          {
            tag=tag.substr(5);
            tag=tag.replace(/("|')/g, "");
            openFont='color';
          }
          else
          if(tag.substr(0,9)=='font size')
          {
            tag=tag.substr(5);
            tag=tag.replace(/("|')/g, "");
            openFont='size';
          }
          else
          if(tag=='font') tag=openFont;
          else
          if(tag=='p') {if(!close) result+="\n"; i++; continue;}
        }
        if((tag=='p')||(tag=='br')||(tag=='b')||(tag=='i')||(tag=='s')||(tag=='table')||(tag=='tr')||(tag=='td')||(tag=='th')||(tag=='font')||(tag.substr(0,9)=='font size')||(tag.substr(0,10)=='font color')||(tag.substr(0,5)=='color')||(tag.substr(0,4)=='size')||(tag=='hr'))
        {
          result+=openTag+(close?'/':'')+tag+closeTag;
        }
      }
    }
    else
    if(c=='>')
    {
      break;
    }
    else
    if(c=='&')
    {
      if(text.substr(i,6)=='&nbsp;')
      {
        if(codeType==1) result+=' ';
        else if(codeType==2) result+='&nbsp;';
        i+=5;
      }
      else
      {
        var j=text.indexOf(';',i+1);
        if(j==-1) break;
        i=j;
      }
    }
    else
    {
      if(!skip)
      {
        if(c>=" ")result+=c;
      }
    }
    i++;
  }
  var d=document.createElement('DIV');
  d.id='statusCode';
  d.innerHTML='<textarea onblur="close_code()" onkeyup="close_code()" title="'+CODE_TOOLTIP+'" id="statusCodeSelector" style="width:90%;height:200px">'+result+'</textarea><br>'
    +'<span class="btn" onclick="close_code()">'+BUTTON_CLOSE+'</span>';
  document.getElementById('buttons').appendChild(d);
  var s=document.getElementById('statusCodeSelector');
  s.focus();
  s.select();
}

function close_code()
{
  document.getElementById('buttons').removeChild(document.getElementById('statusCode'));
  haveCode=0;
}



WORKPLACE=document.getElementById('status1');

{
  var d=document.createElement('DIV');
  d.id='buttons';
  d.innerHTML=
      '<form method="post" id="clicker" name="clicker" action="/Select"><input type="hidden" name="room" value="room101">'
    + '<span class="btn" onclick="javascript:get_code(0)">' + BUTTON_TEXT + '</span>'
    + ' <span class="btn" onclick="javascript:get_code(1)">' + BUTTON_FORUM+ '</span>'
    + ' <span class="btn" onclick="javascript:get_code(2)">' + BUTTON_HTML + '</span>'
    + '</form>';
  ;
  WORKPLACE.appendChild(d);
}

timer=setTimeout(get_data, START_DELAY);
