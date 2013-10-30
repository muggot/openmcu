var fortyTwo=42

  ,STEPS_TO_REMEMBER = 10
  ,START_INTERVAL    = 500//1500
  ,UPDATE_INTERVAL   = 1000//7000
  ,UPDATE_RETRIES    = 1//5
  ,REQUEST_TIMEOUT   = 500//4000
  ,DATA_HANDLE_DELAY = 100//333

  ,data              = []
  ,gettingData       = 0
  ,getDataErrorCount = 0
  ,requestTimer      = null
  ,rooms             = 0
  ,roomList          = []
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
}

function on_delete_room(r)
{
}

function handle_data()
{ // new data block 'data' just received from server
  rooms=data.length;
  var newRooms=[], i=0;
  for(i=0; i<rooms; i++)
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
    
//  document.getElementById('status').innerHTML=Math.random();
  var s='';
//  alert(store.length);
//  for(var i=0;i<store.length;i++) s+=store[i][0][4][0][5]+"<br>";
  for(var i=0;i<rooms;i++) s+=roomList[i]+"<br>";
  document.getElementById('status').innerHTML=s;
}

function status_update_start()
{
  timer=setTimeout(get_data, START_INTERVAL);
}
