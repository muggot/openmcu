var fortyTwo=42

  ,STEPS_TO_REMEMBER = 10
  ,START_INTERVAL    = 500//1500
  ,UPDATE_INTERVAL   = 1000//7000
  ,UPDATE_RETRIES    = 1//5
  ,REQUEST_TIMEOUT   = 500//4000
  ,DATA_HANDLE_DELAY = 100//333
  ,WORKPLACE         = null
  ,OFFLINE_PREFIX    = "<B>[Offline] </B>" //localize it :)
  ,OFFLINE_SUFFIX    = ""
  ,ONLINE_PREFIX     = ""
  ,ONLINE_SUFFIX     = ""
  ,AUDIO_OUT_STR     = "Audio Out"
  ,AUDIO_IN_STR      = "Audio In"
  ,VIDEO_OUT_STR     = "Video Out"
  ,VIDEO_IN_STR      = "Video In"
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
  xro.open('GET','http://127.0.0.1:1420/Status/?js',true);
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
  var t=null;
  try {t=document.getElementById('r_t_'+room);} catch(e) {}
  if(t==null) return;
  var online = member[0]!=0;
  var tr=t.insertRow(-1);
  var td=tr.insertCell(0);
  td.innerHTML = (online?ONLINE_PREFIX:OFFLINE_PREFIX) + member[1] + (online?ONLINE_SUFFIX:OFFLINE_SUFFIX);

  td=tr.insertCell(1); //Duration
  td.innerHTML = member[4];

  td=tr.insertCell(2); //RTP Channel: Codec
  td.innerHTML = 
    "<b>" + AUDIO_IN_STR  + ":</b> " + member[ 9] + "<br>" +
    "<b>" + AUDIO_OUT_STR + ":</b> " + member[10] + "<br>" +
    "<b>" + VIDEO_IN_STR  + ":</b> " + member[11] + "<br>" +
    "<b>" + VIDEO_OUT_STR + ":</b> " + member[12];

}

function on_member_delete(key)
{
  var t=null, a = key.split("\t",3);
  var roomName = a[0];
  try {t=document.getElementById('r_t_'+roomName);} catch(e) {}
  if (t==null) return;

  var memberName = a[2], online = (a[1] != 0);
  var searchStr = (online?ONLINE_PREFIX:OFFLINE_PREFIX) + memberName + (online?ONLINE_SUFFIX:OFFLINE_SUFFIX);

  for(var i=t.rows.length-1; i>=0; i--)
  {
    if(t.rows[i].cells[0].innerHTML == searchStr)
    {
      t.deleteRow(i);
    }
  }
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

function status_update_start()
{
  WORKPLACE=document.getElementById('status1');
  timer=setTimeout(get_data, START_INTERVAL);
}
