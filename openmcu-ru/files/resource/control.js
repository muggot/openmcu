var max_subframes=100;
var default_page_width=942; //CHANGE IT IF YOU WISH, IT'LL BE AUTO-INCREASED IF NEEDED
var page_border_width=70;   //for detect panel width
var debug=0;
var MIXER_PANEL_BGCOLOR='#575';
var MIXER_PANEL_BGCOLOR2='#242';
var MIXER_PANEL_MIXER_STYLE='font-weight:bold;background-color:'+MIXER_PANEL_BGCOLOR2+';padding-left:3px;padding-right:3px;border-radius:2px;border:2px solid #484;color:#fc5';
var MIXER_PANEL_HEIGHT=33;
var panel_width=200; // init anyhow :)
var PANEL_ITEM_HEIGHT=34; // height of BOLVANCHIK
var PANEL_ICON_HEIGHT=(PANEL_ITEM_HEIGHT/2)-2;
var PANEL_ICON_WIDTH=PANEL_ICON_HEIGHT;
var SCROLLER_WIDTH=22; // browser specific, maybe calculated, but not yet
var panel_ip_width=120; // width of IP address bar in panel
var PANEL_MIXERID_WIDTH=14; // width of mixer id bar in panel
var PANEL_HIDEBUTTON_WIDTH=15;
var MIXER_LAYOUT_SCROLL_LEFT_STYLE="line-height:10px;font-weight:bold;color:#fff;cursor:pointer";
var MIXER_LAYOUT_SCROLL_RIGHT_STYLE=MIXER_LAYOUT_SCROLL_LEFT_STYLE;
var MIXER_LAYOUT_SCROLL_LEFT_BUTTON="&#9668;";
var MIXER_LAYOUT_SCROLL_RIGHT_BUTTON="&#9658;";
var VAD_FIELD_STYLE="width:40px;background-color:#fce;border-radius:5px;color:blue;font-weight:bold";
var MIX_BORDER_COLOR='#5ce';


var OTFC_UNMUTE                  =  0; // corresponds to mcu.h
var OTFC_MUTE                    =  1;
var OTFC_MUTE_ALL                =  2;
var OTFC_REMOVE_FROM_VIDEOMIXERS =  3;
var OTFC_REFRESH_VIDEO_MIXERS    =  4;
var OTFC_ADD_AND_INVITE          =  5;
var OTFC_DROP_MEMBER             =  7;
var OTFC_VAD_NORMAL              =  8;
var OTFC_VAD_CHOSEN_VAN          =  9;
var OTFC_VAD_DISABLE_VAD         = 10;
var OTFC_REMOVE_VMP              = 11;
var OTFC_MOVE_VMP                = 12;
var OTFC_SET_VMP_STATIC          = 13;
var OTFC_VAD_CLICK               = 14;
var OTFC_MIXER_ARRANGE_VMP       = 15;
var OTFC_MIXER_SCROLL_LEFT       = 16;
var OTFC_MIXER_SHUFFLE_VMP       = 17;
var OTFC_MIXER_SCROLL_RIGHT      = 18;
var OTFC_MIXER_CLEAR             = 19;
var OTFC_MIXER_REVERT            = 20;
var OTFC_GLOBAL_MUTE             = 21;
var OTFC_SET_VAD_VALUES          = 22;
var OTFC_TEMPLATE_RECALL         = 23;
var OTFC_SAVE_TEMPLATE           = 24;
var OTFC_DELETE_TEMPLATE         = 25;
var OTFC_REMOVE_VMP_MEMBER       = 26;
var OTFC_CHANGE_RESIZER_RULE     = 27;
var OTFC_INVITE                  = 32;
var OTFC_REMOVE_OFFLINE_MEMBER   = 33;
var OTFC_DIAL                    = 34;
var OTFC_DROP_ALL_ACTIVE_MEMBERS = 64;
var OTFC_INVITE_ALL_INACT_MMBRS  = 65;
var OTFC_REMOVE_ALL_INACT_MMBRS  = 66;
var OTFC_YUV_FILTER_MODE         = 68;
var OTFC_TAKE_CONTROL            = 69;
var OTFC_DECONTROL               = 70;
var OTFC_ADD_VIDEO_MIXER         = 71;
var OTFC_DELETE_VIDEO_MIXER      = 72;
var OTFC_SET_VIDEO_MIXER_LAYOUT  = 73;
var OTFC_SET_MEMBER_VIDEO_MIXER  = 74;
var OTFC_VIDEO_RECORDER_START    = 75;
var OTFC_VIDEO_RECORDER_STOP     = 76;
var OTFC_TOGGLE_TPL_LOCK         = 77;
var OTFC_UNMUTE_ALL              = 78;
var OTFC_AUDIO_GAIN_LEVEL_SET    = 79;
var OTFC_OUTPUT_GAIN_SET         = 80;
var OTFC_ADD_TO_ABOOK            = 90;
var OTFC_REMOVE_FROM_ABOOK       = 91;

var mmw = -1; // build_page() initializer
var visible_ids='';
var mixers=0, bfw=704, bfh=576, room='';

var mixer_refresh_timer = null;

//
var members_list_display = 0;
var abook_list_display = 0;

if(debug)document.write('<div style="width:100%;height:80px;overflow:auto;border:1px dotted red" id="debug1"></div>'); function dmsg(s){if(debug){document.getElementById('debug1').innerHTML+=s+'. ';document.getElementById('debug1').scrollTop=document.getElementById('debug1').scrollHeight;}}

var dd_in_progress=false;
var query_active=false;
var appendedflying=false;
var dd_over=0;
var dd_over_idx=-1;
var dd_over_substance=-1;
var dd_final_idx=0;
var dd_final_substance='?';
var prvnt=0;
var mrefreshing=false;
var staticMemberSelecting=false;
var selectingObject=0;

var otfrq=Array(); // on the fly control command queue
var otf_in_progress = false; // on the fly control request flag

var dd_flying_idx=false; // position for d&d
var dd_flying_substance=0; // panel marker for d&d
var dd_ex_marker=0;

var vad_setup_mode=false;
var tpl_save_mode=false;
var vad1=100, vad2=1000, vad3=10000;

var hl_links=[];
var hl_id = -1;
var hl_state = -1;

var mlgctr1=0;

var isTemplateLocked=0;

var splitSelectorWindow=null;
var splitSelectorWindowMixerNumber=-1;

var gainSelecting=false;
var gainSelector=null;

var roomName, globalMute, isModerated;

var classicMode = false;

var sortMode = parseInt(getcookie('sortMode')); if(sortMode!=0) sortMode=1;

function index_exists(a, i)
{
  try
  {
    if(typeof a[i] == 'undefined') return false;
    else return true;
  }
  catch (error){}
  return false;
}

function getcookie(name)
{
  var regexp=new RegExp("(?:; )?"+name+"=([^;]*);?");
  if (regexp.test(document.cookie)) return decodeURIComponent(RegExp["$1"]);
  return false;
}

function my_trim(s){ s+=""; return s.replace(/(^[\s\t\n\r]+)|([\s\t\n\r]+$)/g, ""); }

function checkcontrol()
{
  if(isModerated)return true;
  my_alert(window.l_pleasetakecontrol);
  return false;
}

function ddstart(e,o,substance,idx)
{
  if(prvnt) return false;
  if(!checkcontrol()) return false;
  if(e.preventDefault)e.preventDefault(); else e.returnValue=false;
//something like mutex:
  if(dd_in_progress)return false;
  if(query_active)return false;
  dd_in_progress=true;

  dd_flying_idx=idx; dd_flying_substance=substance;

  dd_over_idx=-1; dd_over_substance=substance;
  draggingobject=o;
  o.style.backgroundColor='#557';
  somethingflying=document.createElement('div');
  somethingflying.className='mmbrdd';
  var s=somethingflying.style;
  s.width=o.style.width;
  s.height=''+(o.offsetHeight-2)+'px';
  somethingflying.innerHTML=o.innerHTML;
  s.opacity='0.5';
  sfo_x=e.clientX; sfo_y=e.clientY; if(substance=='panel') sfo_y-=53;
  if((substance!=='panel')&&(substance!=='panel_top')){ // from mockup:
   base_offset_x=document.getElementById('pp'+substance+'_'+idx).offsetLeft-document.getElementById('pbase').offsetLeft;
   base_offset_y=document.getElementById('pp'+substance+'_'+idx).offsetTop-document.getElementById('pbase').offsetTop;
  } else { // from right list panel:
   var id=idid(idx);
   base_offset_x=document.getElementById('rpan_'+id).offsetLeft+document.getElementById('pp_2').offsetLeft-document.getElementById('pbase').offsetLeft;
   base_offset_y=document.getElementById('rpan_'+id).offsetTop+document.getElementById('pp_2').offsetTop-document.getElementById('pbase').offsetTop;
   if(dd_flying_substance=='panel_top')
   {
     if(dd_flying_idx==-1)base_offset_x+=23;
     if(dd_flying_idx==-2)base_offset_x+=56;
   }
   base_offset_y-=document.getElementById('right_scroller').scrollTop;
  }
  appendedflying=false;
  s.left=base_offset_x;
  s.top=base_offset_y;
}

function ddprogress(e){
 if(!dd_in_progress) return true;
 if(e.preventDefault)e.preventDefault(); else e.returnValue=false;
 if(!appendedflying){
  appendedflying=true;
  document.getElementById('pbase').appendChild(somethingflying);
 }
 somethingflying.style.left=base_offset_x-sfo_x+e.clientX+'px';
 somethingflying.style.top=base_offset_y-sfo_y+e.clientY+'px';
 return false;
}

function ddstop(e){
 if(!dd_in_progress) return false;
 dd_final_idx=dd_over_idx;
 dd_final_substance=dd_over_substance;
 if(dd_over!==0)ddout(e,dd_over,dd_over_idx);
 if(appendedflying) document.getElementById('pbase').removeChild(somethingflying);
 appendedflying=false;
 dd_in_progress=false;
 draggingobject.style.backgroundColor='';
 dd_do_it(dd_flying_substance,dd_flying_idx,dd_final_substance,dd_final_idx);
 return false;
}

function ddover(e,o,substance,idx){
 if(!dd_in_progress) return true;
 if(dd_over===o)return false; //already complete
 if((dd_flying_substance==substance)&&(dd_flying_idx===idx)) {dd_over_idx=-1;return false;}
 if(dd_over!==0)ddout(event,dd_over,idx);
 dd_over=o;
 dd_over_bgbkp=o.style.backgroundColor;
 if(idx==-2) {
  if(dd_flying_idx>0) o.style.backgroundColor='#ccc';
 } else o.style.backgroundColor='#f00';
 dd_over_idx=idx;
 dd_over_substance=substance;
}

function ddout(e,o,substance,idx){
 if(!dd_in_progress) return true;
 if(dd_over===0) return true;
 dd_over.style.backgroundColor=dd_over_bgbkp;
 dd_over=0;
 return false;
}

function dd_do_it(s1, p1, s2, p2)
{
  if((s1===s2)&&(p1===p2)) return true; // nothing to do

  if(((s1==='panel')||(s1=='panel_top'))&&((s2==='panel')||(s2==='panel_top'))) return true; //panel->panel

  if(s1==='panel_top') // top panel -> ...
  { alert('Header -> somewhere: '+s1+'/'+p1+' -> '+s2+'/'+p2); return; }

  if(s1==='panel') // panel -> ...
  { queue_otf_request(OTFC_SET_VMP_STATIC,p1,s2,p2); return true; }

  if((s2==='panel')||(s2==='panel_top'))
  { queue_otf_request(OTFC_REMOVE_VMP_MEMBER,s1,p1); return true; }

  queue_otf_request(OTFC_MOVE_VMP,s1,p1,s2,p2);
}

function createRequestObject(){
 if (typeof XMLHttpRequest==='undefined'){
  XMLHttpRequest=function(){
   try { return new ActiveXObject("Msxml2.XMLHTTP.6.0"); } catch(e) {}
   try { return new ActiveXObject("Msxml2.XMLHTTP.3.0"); } catch(e) {}
   try { return new ActiveXObject("Msxml2.XMLHTTP"); } catch(e) {}
   try { return new ActiveXObject("Microsoft.XMLHTTP"); } catch(e) {}
   return false;
 }} return new XMLHttpRequest();
}

function queue_otf_request(action, value, option, option2, option3) // options are optional
{
  var q=Array(action,value);
  if(typeof option!='undefined')
  { q[2]=option;
    if(typeof option2!='undefined')
    { q[3]=option2;
      if(typeof option3!='undefined') q[4]=option3;
    }
  }
  var len=otfrq.push(q);
  dmsg('Request queued, queue length is '+len);
  if(len==1) dmsg('Starting On-the-Fly queue');
  if(len==1) start_otf_control();
}

function start_otf_control()
{
  if(otf_in_progress)
  {
    dmsg('Request will be delayed for 333 ms');
    return setTimeout(start_otf_control,333);
  }
  otf_in_progress=true;
  if(otfrq.length==0) { otf_in_progress=false; return; }

  var data=otfrq.pop();
  var cmd='room='+encodeURIComponent(roomName)
    + '&otfc=1'
    + '&action='+data[0]
    + '&v='+encodeURIComponent(data[1]);
  if(2 in data) cmd+='&o='+encodeURIComponent(data[2]);
  if(3 in data) cmd+='&o2='+encodeURIComponent(data[3]);
  if(4 in data) cmd+='&o3='+encodeURIComponent(data[4]);
  dmsg('Sending <font color="green">'+cmd+'</font>');
  otf_timer=setTimeout(otf_fail,5555);
  cro_otf=createRequestObject();
  cro_otf.open('POST','Select',true);
  cro_otf.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');
  cro_otf.setRequestHeader("Content-length", cmd.length);
  cro_otf.setRequestHeader("Connection", "close");
  cro_otf.onreadystatechange=otf_result;
  cro_otf.send(cmd);
  dmsg('Sent');
}

function otf_result()
{
  try
  {
    if(cro_otf.readyState==4)
    {
      if(cro_otf.status==200) //OK
      {
        clearTimeout(otf_timer);
        dmsg('Server responded: <b>'+cro_otf.responseText+'</b>, queue length: '+otfrq.length);
        cro_otf.abort();
        otf_in_progress=false;
        if(otfrq.length>0) start_otf_control();
      }
      else //NOT OK
      {
        otfrq=Array();
        cro_otf.abort();
        clearTimeout(otf_timer);
        my_alert('On-the-Fly Control error: reply code='+cro_otf.status+'.');
        otf_in_progress=false;
      }
    }
  } catch (e) {}
}

function otf_fail()
{
  cro_otf.abort(); otf_in_progress=false; otfrq=Array();
  my_alert('On-the-Fly Control request cancelled by timeout.');
}

function member_modify_by_id(id,index,value)
{
  if(typeof members==='undefined') return false;
  for(var i=0;i<members.length;i++) if(members[i][1]==id)
  {
    members[i][index]=value;
    return true;
  }
  return false;
}

function member_read_by_id(id,index)
{
  if(typeof members!='undefined')
    for(var i=0;i<members.length;i++)
      if(members[i][1]==id) return members[i][index];
  return false;
}

function muteunmute(obj,mid,mask)
{
  dmsg('Executing MUTE for member id '+mid);
  var cmd, mute=member_read_by_id(mid,3)&mask;
  if(!mute)cmd=OTFC_MUTE;else cmd=OTFC_UNMUTE;
  if(obj)obj.className='launchspr';
  queue_otf_request(cmd,mid,mask);
  return false;
}

function vadoptions(obj,mid)
{
  var
    disv=member_read_by_id(mid,4),
    cvan=member_read_by_id(mid,5),
    cmd;
  if(!(cvan|disv)) cmd=OTFC_VAD_CHOSEN_VAN;
  else if(cvan) cmd=OTFC_VAD_DISABLE_VAD;
  else cmd=OTFC_VAD_NORMAL;
  if(obj) obj.className='launchspr';
  queue_otf_request(cmd,mid);
  return false;
}

function checking_failed(){
  check_alive=0;
  if(!iframe_reconnection){
    var ifr=document.getElementById('loggingframe');
    var doc=ifr.contentWindow.document.open('text/html','replace');
    doc.write('<body style="font-face:Verdana,Arial;color:red;font-size:10px;font-weight:bold">Connection dropped :(<br>Trying to reconnect...');
    if(typeof link_was_lost==='undefined')link_was_lost=document.getElementById('logging1').innerHTML;
  }
  setTimeout(reconnect_iframe,1500);
}

function reconnect_iframe(){
  iframe_reconnection=1;
  var ifr=document.getElementById('loggingframe');
  try{
  document.getElementById('logging1').removeChild(ifr);
  document.getElementById('logging1').innerHTML=link_was_lost;
  check_alive=setTimeout(checking_failed,5500);
  } catch(e){ location.href=location.href; }
}

function alive(){
  iframe_reconnection=0;
  if(typeof check_alive !== 'undefined') if(check_alive!==0) clearTimeout(check_alive);
  if(typeof check_alive_counter === 'undefined') check_alive_counter=0; else check_alive_counter++;
  var ifr=document.getElementById('loggingframe');
  if(check_alive_counter>=2000){
    check_alive_counter=0;
    ifr.contentWindow.location.href=ifr.contentWindow.location.href;
  }
  ifr.contentWindow.scrollTo(0,999999);
  check_alive=setTimeout(checking_failed,5000);
}

function inviteoffline(obj,mname,add)
{
  if(typeof add == 'undefined') add=0;
  if(add)queue_otf_request(OTFC_ADD_AND_INVITE,mname);
  else queue_otf_request(OTFC_INVITE,mname);
  if(obj)
    if(obj.className=='topdspr') obj.className='topdspr_w';
    else obj.src='launched.gif';
}

function notice_deletion(stage, room)
{
  if(typeof window.l_room_deletion_text == 'undefined')
  {
    my_alert('Stopping and deleting &laquo;'+room+'&raquo;: ' + (5-stage));
  }
  else
  {
    var s=window.l_room_deletion_text[stage];
    s=s.replace(/\%/g, room);
    my_alert(s);
  }
  if(stage==5) location.href='/Select';
}

function removeoffline(obj,mname)
{
  if(!confirm(window.l_room_remove_from_list+" \""+decodeURIComponent(mname)+"\" ?")) return;
  queue_otf_request(OTFC_REMOVE_OFFLINE_MEMBER,mname);
  obj.src='launched.gif';
}

function kick_confirm(obj,mid,mname)
{
  if(!confirm(window.l_room_drop_connection_with+" \""+decodeURIComponent(mname)+"\" ?")) return;
  queue_otf_request(OTFC_DROP_MEMBER,mid);
  obj.src='launched.gif';
}

function drop_all0(obj)
{
  if(confirm(window.l_room_drop_all_active_members+"?"))
  {
    for(var i=0;i<members.length;i++) members[i][14]=0;
    queue_otf_request(OTFC_DROP_ALL_ACTIVE_MEMBERS,0);
  }
  members_refresh();
}

function invite_all(obj, startDial)
{
  var
    dial = 0,
    question = window.l_room_invite_all_inactive_members + "?",
    borderBackup;
  if(obj)
  {
    borderBackup = obj.style.border;
    obj.style.border = '1px dotted red';
  }
  if(typeof startDial != 'undefined')
    if(startDial)
    {
      dial = 1;
      question = window.l_room_dial_all_members + "?";
    }
  if(!confirm(question))
  {
    if(obj) obj.style.border=borderBackup;
    return;
  }
  for(var i = 0; i < members.length; i++) members[i][14] = dial;
  queue_otf_request(OTFC_INVITE_ALL_INACT_MMBRS, dial);
  if(obj) obj.style.border=borderBackup;
  members_refresh();
}

function remove_all0(obj)
{
  if(!confirm(window.l_room_remove_all_inactive_members+"?")) return;
  queue_otf_request(OTFC_REMOVE_ALL_INACT_MMBRS,0);
}

function highlight(id, state)
{
  if((hl_state == state) && (hl_id == id)) return;
  if(state==0)
  {
    if((id != hl_id)||(hl_state==0)) return;
    if(index_exists(hl_links,hl_id))
    for(var i=0;i<hl_links[hl_id].length;i++)
    {
      document.getElementById(hl_links[hl_id][i]).style.opacity="0.5";
      document.getElementById(hl_links[id][i]).style.filter='alpha(opacity=50)';
    }
  }
  else
  {
    if(index_exists(hl_links,id))
    {
      for(var i=0;i<hl_links[id].length;i++)
      {
        document.getElementById(hl_links[id][i]).style.opacity="0";
        document.getElementById(hl_links[id][i]).style.filter='alpha(opacity=0)';
      }
    }
  }
  hl_state = state;
  hl_id = id;
}

function on_abook_check_all(obj){
  var checked = obj.checked;
  for(i=0;i<addressbook.length;i++)
  {
    var check_box = document.getElementById("abook_check_"+addressbook[i][1]);
    if(check_box)
      check_box.checked = checked;
  }
}
function on_abook_check(obj){
  if(obj.checked) return;
  check_box = document.getElementById("abook_check_all");
  if(check_box)
      check_box.checked = obj.checked;
}
function invite_checked_abook(obj){
  check_box = document.getElementById("abook_check_all");
  if(check_box)
    check_box.checked =false;
  for(i=0;i<addressbook.length;i++)
  {
    check_box = document.getElementById("abook_check_"+addressbook[i][1]);
    if(check_box)
    {
      if(check_box.checked)
      {
        inviteoffline(document.getElementById('abook_inv_'+addressbook[i][1]),addressbook[i][2],1);
        check_box.checked = false;
      }
    }
  }
}
function get_dial_input_address(obj)
{
  if(!document.getElementById("invite_input")) return;
  var addr = document.getElementById("invite_input").value;
  if(addr=="") return;

  var delim = addr.indexOf("[");
  if(delim == -1) delim = addr.indexOf("<");
  if(delim == -1) delim = 0;
  else            delim++;

  var setProto = (addr.substr(delim,5).toLowerCase()=='h323:') || (addr.substr(delim,5).toLowerCase()=='rtsp:') || (addr.substr(delim,4).toLowerCase()=='sip:');
  if(!setProto)
  {
    if(document.getElementById('divInvProto').innerHTML=='h323')
      addr = addr.substr(0,delim)+'h323:'+addr.substr(delim);
    else if(document.getElementById('divInvProto').innerHTML=='rtsp')
      addr = addr.substr(0,delim)+'rtsp://'+addr.substr(delim);
    else if(document.getElementById('divInvProto').innerHTML=='sip')
      addr = addr.substr(0,delim)+'sip:'+addr.substr(delim);
  }
  return addr;
}
function dial_from_input(obj)
{
  var addr = get_dial_input_address(obj);
  if(!addr) return;
  inviteoffline(obj,addr);
}

function add_to_abook(obj, addr)
{
  if(!addr) return;
  queue_otf_request(OTFC_ADD_TO_ABOOK, addr);
}
function add_to_abook_input(obj)
{
  var addr = get_dial_input_address(obj);
  if(!addr) return;
  queue_otf_request(OTFC_ADD_TO_ABOOK, addr);
}
function remove_from_abook(obj, addr)
{
  if(!addr) return;
  queue_otf_request(OTFC_REMOVE_FROM_ABOOK, addr);
}

function rtp_state(id, bit, state)
{
  for(var i=0;i<members.length;i++)
  {
    if(members[i][1]==id)
    {
      if(state)
        members[i][9]|=bit;
      else
        members[i][9]&=~bit;
      if(bit < 16)
      {
        obj=document.getElementById('mrpan'+bit+'_'+idid(id));
        if(obj)
          if(state) obj.style.borderStyle='none';
          else      obj.style.borderStyle='solid';
      }
      else if(bit&16)
      {
        obj=document.getElementById('srpan_'+idid(id));
        if(obj)
          if(state) obj.className='mutespr10';
          else      obj.className='vlevel';
      }
      break;
    }
  }
  alive();
}

function gain_selector_mouse_out()
{
  document.body.removeChild(gainSelector);
  gainSelecting=false;
}

function gain_selector_select(id, value, dir)
{
  dmsg('gain_s_s'+id+"="+value);
  gain_selector_mouse_out();
  if(dir) queue_otf_request(OTFC_OUTPUT_GAIN_SET     ,id,(value+20));
  else    queue_otf_request(OTFC_AUDIO_GAIN_LEVEL_SET,id,(value+20));
}

function gain_selector(obj, id, dir0)
{
  var dir=0;
  if(typeof dir0!='undefined') if(dir0) dir=1;
  var def=member_read_by_id(id,10+dir);
  dmsg('Creating gain selector for member id '+id+', sel '+def);
  if(def===false) return;
  if(gainSelecting) document.body.removeChild(gainSelector);

  gainSelector=document.createElement('div');
  var selWidth=58;
  var selHeight=300;
  gainSelector.style.width=selWidth+'px';
  gainSelector.style.height=selHeight+'px';
  gainSelector.style.position='absolute';
  gainSelector.style.left=getLeftPos(obj)+'px';
  offset = document.getElementById('right_scroller').scrollTop;
  gainSelector.style.top=(getTopPos(obj)-offset)+'px';
  gainSelector.style.overflowY='scroll';
  gainSelector.style.overflowX='hidden';
  gainSelector.style.border='1px solid #000';
  gainSelector.style.borderRadius='5px';
  gainSelector.style.backgroundColor='#fff';
  gainSelector.onmouseleave=gain_selector_mouse_out;

  var r="";
  var i;
  for (i=-20;i<=60;i+=2)
  {
    var s="";
    if(i>=0) s+="+"; else s+="–";
    var j=i; if(j<0) j=-j;
    if(j<10) s+="0";
    s+=j;
    r+="<p ";
    if(i!=def) r+="onclick='javascript:{gain_selector_select(" + id + "," + i + "," + dir + ");}' ";
    else r+="onclick='javascript:gain_selector_mouse_out();' ";
    r+=" class='kdb";
    if(i==def) r+="s";
    else if(i<-6) r+="1";
    else if(i>6) r+="2";
    else if(i==0) r+="0";
    r+="'>";
    r+="<nobr>"+s+" dB"+"</nobr></p>";
  }
  gainSelector.innerHTML=r;
  document.body.appendChild(gainSelector);
  gainSelecting = true;
}

function nice_db(level)
{
  var s;
  if(level<0) {s='&minus;'; level=-level;} else s='+';
  if(level<10) s+="0";
  return "<nobr>"+s+level+" dB</nobr>";
}

function setagl(id, level)
{
  var obj=null;
  try { obj=document.getElementById('agl_'+idid(id)); } catch(e) { obj=null; }
  if(obj) obj.innerHTML=nice_db(level);
  member_modify_by_id(id,10,level);
  alive();
}

function setogl(id, level)
{
  var obj=null;
  try { obj=document.getElementById('ogl_'+idid(id)); } catch(e) { obj=null; }
  if(obj) obj.innerHTML=nice_db(level);
  member_modify_by_id(id,11,level);
  alive();
}

function idid(id){ if(id<0) return '_'+(-id); return ''+id; }

function mute_button(member, st, bitN)
{
  var id=idid(member[1]);
  return "<div"
    + " id='mrpan" + bitN + "_" + id + "'"
    + " style='border:1px red " + ((!st||member[9]&bitN)?"none":"solid") + ";'"
    + " onmouseover='prvnt=1'"
    + " onmouseout='prvnt=0'"
    + " onclick='muteunmute(this," + member[1] + "," + bitN + ")'"
    + " class='mutespr" + bitN + ((member[3]&bitN)?"0":"1") + "'"
    + " title='" + ((member[3]&bitN)?"Unmute":"Mute") + "'"
  + "></div>";
}

function vad_button(member)
{
  var cmd=10;
  if(!(member[4]|member[5])) cmd=8;
  else if(member[5]) cmd=9;
  return "<div"
    + " id='vrpan_" + idid(member[1]) + "'"
    + " class='vadspr" + cmd + "'"
    + " onmouseover='prvnt=1'"
    + " onmouseout='prvnt=0'"
    + " onclick='vadoptions(this," + member[1] + ")'"
    + " title='" + ((cmd==8)?"VAD":"") + ((cmd==9)?"Chosen Van":"") + ((cmd==10)?"VAD disabled":"") + "'"
  + "></div>";
}

function format_mmbr_button(m,st)
{
  var sname='mmbr'+st, id=idid(m[1]), memberName=''+m[2];

  var s="<div class='"+sname+"' style='width:"+(panel_width-4)+"px'";
  s+=" id='rpan_"+id+"' onmousedown='{highlight("+m[1]+",0);ddstart(event,this,\"panel\","+m[1]+");}' onmouseover='highlight("+m[1]+",1)' onmouseout='highlight("+m[1]+",0)'";
  s+=">";

  var ip=get_addr_url_without_param(memberName);
  var uname=get_addr_name(memberName);
  var kick, hide, kdb, kdb2, mixer=m[7], mixerb, levelb, remove, invite, resize;

  if(mixer==-1) mixer='va';
  ip="<span style='color:"+((st==0)?"#576":"blue")+";font-size:10px'>"+ip+"</span>";

  var kdb_width=35, w=15, h=15, sp=5;
  var width=panel_width-4;
  var bwidth=20;
  var name_width=width-9*bwidth; if(name_width<10) name_width=10;

  var dpre="<div class='mbrpos1' style='left:";
  var dpre2="<div class='mbrpos11' style='left:";
  var b1style="style='cursor:pointer;width:"+w+"px;height:"+h+"px;'";
  var b2style="style='border:1px inset #E6E6FA;cursor:pointer;width:"+(kdb_width)+"px;height:"+(h-1)+"px;font-size:10px;'";
  var b3style="style='border:1px inset #E6E6FA;cursor:pointer;width:"+(kdb_width)+"px;height:"+(h-1)+"px;font-size:11px;'";
  var b4style="style='border:1px inset #E6E6FA;width:"+(w)+"px;height:"+(h-1)+"px;'";
  var namestyle="style='width:"+name_width+"px;height:"+h+"px;line-height:"+h+"px'";

  // states
  reg_state = 0;
  reg_info = "";
  conn_state = 0;
  conn_info = "";
  ping_state = 0;
  ping_info = "";
  account = null;
  for(var i=0;i<addressbook.length;i++)
    if(m[2] == addressbook[i][2]) { account = addressbook[i]; break; }
  if(!account)
  {
    addr_url = get_addr_url(memberName);
    for(var i=0;i<addressbook.length;i++)
      if(addr_url == get_addr_url(addressbook[i][2])) { account = addressbook[i]; break; }
  }
  if(!account)
  {
    addr_nameid = get_addr_nameid(memberName);
    for(var i=0;i<addressbook.length;i++)
      if(addr_nameid == get_addr_nameid(addressbook[i][2])) { account = addressbook[i]; break; }
  }
  if(account)
  {
    reg_state = account[5];
    reg_info = window.l_name_registered+": "+account[6];
    conn_state = account[7];
    conn_info = window.l_name_connected+": "+account[8];
    ping_state = account[9];
    ping_info = window.l_name_last_ping_response+": "+account[10];
  }
  reg_icon = "";
  st_style = "style='width:16px;height:16px;cursor:help'";
  if(reg_state == 1)       reg_icon = "<img src='i16_status_gray.png' title='"+reg_info+"' "+st_style+">";
  else if(reg_state == 2)  reg_icon = "<img src='i16_status_green.png' title='"+reg_info+"' "+st_style+">";
  conn_icon = "";
  if(conn_state == 1)      conn_icon = "<img src='i16_status_blue.png' title='"+conn_info+"' "+st_style+">";
  else if(conn_state == 2) conn_icon = "<img src='i16_status_red.png' title='"+conn_info+"' "+st_style+">";
  else if(reg_state != 0)  conn_icon = "<img src='i16_status_gray.png' title='"+conn_info+"' "+st_style+">";
  ping_icon = "";
  if(ping_state == 1)      ping_icon = "<img src='i16_status_green.png' title='"+ping_info+"' "+st_style+">";
  else if(ping_state == 2) ping_icon = "<img src='i16_status_red.png' title='"+ping_info+"' "+st_style+">";
  //

  var prCode = " onmouseover='prvnt=1' onmouseout='prvnt=0'";

  var mute1=mute_button(m, st, 1);
  var mute2=mute_button(m, st, 2);
  var mute4=mute_button(m, st, 4);
  var mute8=mute_button(m, st, 8);
  var vad=vad_button(m);
  kick   ="<img "+b1style+" src='i16_close_red.png' onclick='kick_confirm(this,"+m[1]+",\""+m[2]+"\");' onmouseover='prvnt=1' onmouseout='prvnt=0' alt='Drop'>";
  hide   ="<div class='hidespr'" + prCode + " title='Remove from video mixers' onclick='if(checkcontrol())queue_otf_request("+OTFC_REMOVE_FROM_VIDEOMIXERS+","+m[1]+")'></div>";
  resize ="<div id='rsz_"+id+"' class='rszspr" + m[15] + "'" + prCode + " title='&laquo;&raquo;/&uArr;&dArr;' onclick='this.className=\"launchspr\";queue_otf_request("+OTFC_CHANGE_RESIZER_RULE+","+m[1]+")'></div>";
  kdb    ="<div "+b2style+" id='agl_"+id+"' class='kdb'" + prCode + " onclick='javascript:{gain_selector(this,"+m[1]+"  );return false;}'>"+nice_db(m[10])+"</div>";
  kdbo   ="<div "+b2style+" id='ogl_"+id+"' class='kdb'" + prCode + " onclick='javascript:{gain_selector(this,"+m[1]+",1);return false;}'>"+nice_db(m[11])+"</div>";
  mixerb ="<div "+b3style+" onclick='javascript:{if(checkcontrol())queue_otf_request("+OTFC_SET_MEMBER_VIDEO_MIXER+","+m[1]+","+(m[7]+1)+");}' class='mmbrmi'>#"+mixer+"</div>";
  levelb ="<div "+b4style+" class='"+((m[9]&16)?"mutespr10":"vlevel")+"' id='srpan_"+id+"'>&nbsp;</div>";
  var autoDial=m[14], adspr="adspr"+(st?"1":"0")+(m[14]?"1":"0");
  var invite = "<div" + prCode + " onclick='this.className=\"launchspr\";queue_otf_request(" + OTFC_DIAL + "," + m[1] + ")' id='dial_"+id+"' class='"+adspr+"'></div>";
  remove ="<img"+prCode+" "+b1style+" onclick='removeoffline(this,\""+m[2]+"\")' src='i16_close_gray.png' alt='Remove' title='"+l_room_remove_from_list+"'>";

  s+=dpre+"2px'><div class='mmbrname' "+namestyle+">"+uname+"</div></div>";
  s+=dpre2+"2px'><div class='mmbrip' "+namestyle+">"+ip+"</div></div>";

  var mtype = m[13];

  if(mtype!=4)
  {
    s+=dpre+(width-10*bwidth)+"px'>"+vad+"</div>";
    if(st)
      s+=dpre+(width-9*bwidth)+"px'>"+levelb+"</div>";
    s+=dpre+(width-8*bwidth)+"px'>"+kdb+"</div>";
    s+=dpre+(width-6*bwidth)+"px'>"+mute1+"</div>";
    s+=dpre+(width-5*bwidth)+"px'>"+mute4+"</div>";
    s+=dpre+(width-4*bwidth)+"px'>"+resize+"</div>";
    if(st)
      s+=dpre+(width-3*bwidth)+"px'>"+hide+"</div>";
//    if(!st)
      s+=dpre+(width-2*bwidth)+"px'>"+invite+"</div>";
  }
    s+=dpre2+(width-8*bwidth)+"px'>"+kdbo+"</div>";
    s+=dpre2+(width-6*bwidth)+"px'>"+mute2+"</div>";
    s+=dpre2+(width-5*bwidth)+"px'>"+mute8+"</div>";
  if(!classicMode)
    s+=dpre2+(width-4*bwidth)+"px'>"+mixerb+"</div>";
  if(!st)
    s+=dpre2+(width-2*bwidth)+"px'>"+remove+"</div>";
  if(st)
    s+=dpre2+(width-2*bwidth)+"px'>"+kick+"</div>";

  spos=width-8*bwidth-2;
  s+=dpre2+(spos-16)+"px'>"+conn_icon+"</div>";
  s+=dpre2+(spos-32)+"px'>"+reg_icon+"</div>";
  s+=dpre2+(spos-48)+"px'>"+ping_icon+"</div>";

  s+="</div>";
  return s;
}

function format_mmbr_abook(num,mmbr)
{
  var bgcolors=Array('#F5F5F5','#E6E6FA');
  var state_color;
  var bgcolor=bgcolors[num&1];

  var height = 16;
  var width = 16;

  var is_abook = mmbr[3];
  var is_account = mmbr[11];
  var is_saved_account = mmbr[12];
  var remote_application = mmbr[4];
  var reg_state = mmbr[5]; // 1=unregistered, 2=registered
  var reg_info = window.l_name_registered+": "+mmbr[6];
  var conn_state = mmbr[7]; // 1=wait, 2=busy
  var conn_info = window.l_name_connected+": "+mmbr[8];
  var ping_state = mmbr[9]; // 0=disable, 1=online, 2=offline
  var ping_info = window.l_name_last_ping_response+": "+mmbr[10];

  var info = "";
  if(remote_application != "") { info += remote_application; }
  if(reg_info != "") { if(info != "") info += "&#10;"; info += reg_info; }
  if(conn_info != "") { if(info != "") info += "&#10;"; info += conn_info; }
  if(ping_info != "") { if(info != "") info += "&#10;"; info += ping_info; }

  var s=
    "<div title='"+info+"' "+
    "style='margin-left:2px;padding:2px 0px 2px 0px;"+
    "width:"+(panel_width)+"px;overflow:hidden;height:"+(height+2)+
    "px;text-align:left;background-color:"+bgcolor + "'>";

  var uname=mmbr[2]+"";
  var name=get_addr_name(uname);
  var ip=get_addr_url_without_param(uname);

  st_style = "style='width:16px;height:16px;cursor:help'";
  var invite = "", check = "";
  if(conn_state == 0)
  {
    var checked = "";
    check_box = document.getElementById("abook_check_"+mmbr[1]);
    if(check_box && check_box.checked) checked = "checked";

    check="<input id='abook_check_"+mmbr[1]+"' onclick='on_abook_check(this)' type='checkbox' "+checked+" width="+width+" height="+height+" style='margin:2px;'>";
    invite="<img id='abook_inv_"+mmbr[1]+"' onclick='inviteoffline(this,\""+encodeURIComponent(mmbr[2])+"\",1)' style='cursor:pointer' src='i15_inv.gif' width='15' height='15' alt='Invite'>";
  }
  else if(conn_state == 1) invite = "<img src='i16_status_blue.png' title='"+conn_info+"' "+st_style+">";
  else if(conn_state == 2) invite = "<img src='i16_status_red.png' title='"+conn_info+"' "+st_style+">";

  var ping_icon = "";
  if(ping_state == 1)      ping_icon = "<img src='i16_status_green.png' title='"+ping_info+"' "+st_style+">";
  else if(ping_state == 2) ping_icon = "<img src='i16_status_red.png' title='"+ping_info+"' "+st_style+">";

  var reg_icon = "";
  if(reg_state == 1)       reg_icon = "<img src='i16_status_gray.png' title='"+reg_info+"' "+st_style+">";
  else if(reg_state == 2)  reg_icon = "<img src='i16_status_green.png' title='"+reg_info+"' "+st_style+">";

  var save_icon = "";
  if(!is_abook)
    save_icon = "<img src='i16_abook_plus.png' style='cursor:pointer' onclick='add_to_abook(this,\""+mmbr[2]+"\")' title='"+window.l_add_to_abook+"' >";
  else if(is_abook && abook_list_display == 1)
    save_icon = "<img src='i16_close_gray.png' style='cursor:pointer' onclick='remove_from_abook(this,\""+mmbr[2]+"\")' title='"+window.l_delete+"' >";

  var ip_decor = "";
  if(!is_saved_account && abook_list_display == 2)
    ip_decor = "text-decoration:line-through;";

  var posx_check  = 8;
  var posx_invite = posx_check       + width + 16;
  var posx_status = posx_invite      + width;
  var posx_name   = posx_status      + 38;
  var free        = panel_width      - posx_name - SCROLLER_WIDTH;
  var width_name  = free/2 - 16 - 8;
  var width_ip    = free/2 - 8;
  var posx_ip     = posx_name        + width_name + 8;
  var posx_abook  = panel_width      - SCROLLER_WIDTH - 16;

  if(width_name<10){my_alert('Exception: maybe screen resolution too low?'); if(width_name<1)width_name=1;}

  var dpre="<div style='width:0px;height:0px;position:relative;top:0px;left:";
  s+=dpre+posx_check+"px'>"+check+"</div>";
  s+=dpre+posx_invite+"px'>"+invite+"</div>";
  s+=dpre+(posx_status+2)+"px'>"+ping_icon+"</div>";
  s+=dpre+(posx_status+20)+"px'>"+reg_icon+"</div>";
  s+=dpre+posx_name+"px'><div style='overflow:hidden;font-size:12px;width:"+width_name+"px;'><nobr>"+name+"</nobr></div></div>";
  s+=dpre+posx_ip+"px'><div style='overflow:hidden;color:#576;font-size:10px;"+ip_decor+"width:"+width_ip+"px;'><nobr>"+ip+"</nobr></div></div>";
  s+=dpre+posx_abook+"px'>"+save_icon+"</div>";

  s+='</div>';
  return s;
}

function invite_panel(){
  var dpre="<div style='width:0px;height:0px;position:relative;top:0px;left:";
  var height = PANEL_ICON_HEIGHT; // 15
  var width = PANEL_ICON_WIDTH; // 15
  var bwidth = 28;
  var dbutton="<div class='btn btn-small' style='border-width:1px;border-radius:0px;padding:2px 0px 2px 0px;height:"+(height+1)+"px;line-height:"+(height+1)+"px;text-align:center;cursor:pointer;";
  var s="<form onsubmit='return false' id='invite_panel' style='width:"+panel_width+"px;height:22px;padding:0px 0px 4px 0px;margin:0px'>";

  var proto_posx = 2;
  var proto_width = 50;
  var input_posx = proto_posx+proto_width;
  var input_width = panel_width - input_posx - (bwidth*2) - 4;
  if(input_width > 250) input_width = 250;
  var b1x = input_posx + input_width;
  var b2x = b1x + bwidth;
  s+=dpre+proto_posx+"px'><div id='divInvProto' class='btn' style='font-size:12px;width:"+proto_width+"px;height:20px;padding:0px;border-radius:0px;' onclick='javascript:{if(this.innerHTML==\"h323\")this.innerHTML=\"rtsp\";else if(this.innerHTML==\"rtsp\")this.innerHTML=\"sip\";else if(this.innerHTML==\"sip\")this.innerHTML=\"h323\";document.getElementById(\"invite_input\").focus();}'>"+get_default_proto()+"</div></div>";
  s+=dpre+input_posx+"px'><input id='invite_input' type='text' style='font-size:12px;width:"+input_width+"px;height:20px;padding:0px;border-radius:0px;border-right:0px;' onkeyup='javascript:{if(mlgctr1){document.getElementById(\"binpinv\").className=\"topdspr\";mlgctr1=0;};if(event.keyCode==13){mlgctr1=1;dial_from_input(document.getElementById(\"binpinv\"));}}' /></div>";
  s+=dpre+b1x+"px'>"+dbutton+"width:"+(bwidth)+"px' onclick='mlgctr1=1;dial_from_input(document.getElementById(\"binpinv\"));'>"
    +"<div class='topdspr' id='binpinv' title='"+window.l_invite+"'></div></div></div>";
  s+=dpre+b2x+"px'>"+dbutton+"width:"+(bwidth)+"px;' onclick='add_to_abook_input()'><img src='i16_abook_plus.png' title='"+window.l_add_to_abook+"' /></div></div>";
  s+="</form>";
  return s;
}

function additional_panel(){
  var dpre="<div style='width:0px;height:0px;position:relative;top:0px;left:";
  var height = PANEL_ICON_HEIGHT; // 15
  var width = PANEL_ICON_WIDTH; // 15
  var bwidth = 28;
  var dbutton="<div class='btn btn-small' style='border-width:1px;border-radius:0px;padding:2px 0px 2px 0px;height:"+(height+1)+"px;line-height:"+(height+1)+"px;text-align:center;cursor:pointer;";
  var s="<form onsubmit='return false' id='additional_panel' style='display:block;width:"+panel_width+"px;height:22px;padding:0px 0px 4px 0px;border-bottom:1px solid #E6E6FA;margin:0px'>"
   +dpre+ "2px;'>"+dbutton+"width:"+bwidth+"px' onclick='queue_otf_request("+OTFC_MUTE_ALL+")' title='"+window.l_room_mute_all+"'><div class='mutespr10' style='margin-left:7px'></div></div></div>"
   +dpre+"34px;'>"+dbutton+"width:"+bwidth+"px' onclick='queue_otf_request("+OTFC_UNMUTE_ALL+")' title='"+window.l_room_unmute_all+"'><div class='mutespr11' style='margin-left:7px'></div></div></div>"
   +dpre+"66px;'>"+dbutton+"width:"+bwidth+"px' onclick='invite_all(this)' title='"+window.l_room_invite_all_inactive_members+"'><div class='adspr00' style='margin-left:6px'></div></div></div>"
   +dpre+"98px;'>"+dbutton+"width:"+bwidth+"px' onclick='invite_all(this,1)' title='"+window.l_room_dial_all_members+"'><div class='adspr01' style='margin-left:6px'></div></div></div>"
   +dpre+"130px;'>"+dbutton+"width:"+bwidth+"px' onclick='remove_all0(this)' title='"+window.l_room_remove_all_inactive_members+"'><div class='xsgray' style='margin-left:7px'></div></div></div>"
   +dpre+"162px;'>"+dbutton+"width:"+(2*PANEL_ICON_WIDTH)+"px;' onclick='drop_all0(this)' title='"+window.l_room_drop_all_active_members+"'><div class='xsred' style='margin-left:7px'></div></div></div>"
//   +dpre+"0px;'>"+dbutton+"width:20px' id='rpan_0' name='rpan_0' onmousedown='ddstart(event,this,\"panel_top\",0)'>[ ]</div></div>"
//   +dpre+"23px;'>"+dbutton+"width:30px' id='rpan__1' name='rpan__1' onmousedown='ddstart(event,this,\"panel_top\",-1)'>VAD</div></div>"
//   +dpre+"56px;'>"+dbutton+"width:36px' id='rpan__2' name='rpan__2' onmousedown='ddstart(event,this,\"panel_top\",-2)'>VAD2</div></div>"
  s+="</form>";
  return s;
}

function get_default_proto()
{
  if(typeof defaultProtocol=='undefined') defaultProtocol='sip';
  if(defaultProtocol=='sip') return 'sip';
  return 'h323';
}

function additional_panel_abook(){
  var dpre="<div style='width:0px;height:0px;position:relative;top:0px;left:";
  var height = PANEL_ICON_HEIGHT; // 15
  var width = PANEL_ICON_WIDTH; // 15
  var bwidth = 28;
  var dbutton="<div class='btn btn-small' style='border-width:1px;border-radius:0px;padding:2px 0px 2px 0px;height:"+(height+1)+"px;line-height:"+(height+1)+"px;text-align:center;cursor:pointer;";
  var s="<form onsubmit='return false' id='additional_panel_abook' style='display:none;width:"+panel_width+"px;height:22px;padding:0px 0px 4px 0px;border-bottom:1px solid #E6E6FA;'>";
  s+=dpre+"2px;'>"+dbutton+"width:"+bwidth+"px;' ><input id='abook_check_all' onclick='on_abook_check_all(this)' type='checkbox' height="+height+" style='margin:2px;' /></div></div>";
  s+=dpre+"34px;'>"+dbutton+"width:"+bwidth+"px;' onclick='invite_checked_abook(this)'><img style='opacity:1;' width="+width+" height="+height+" alt='Inv.' src='i15_inv.gif' /></div></div>";

  s+="</form>";
  return s;
}

function tab_panel(){
  var dpre="<div style='width:0px;height:0px;position:relative;top:0px;left:";
  var dmain="px'><div style='width:120px;padding:0;border-bottom:solid 1px #CDC9C9;border-top-left-radius:10px;border-top-right-radius:10px;height:22px;line-height:22px;text-align:center;cursor:pointer;";
  var tab1_name = window.l_connections_word_room;
  var tab2_name = window.l_name_address_book;
  var s ="<div id='tab_panel' style='border-bottom:solid 1px #E6E6FA;width:"+panel_width+"px;height:22px;margin-bottom:4px'>"
   +dpre+"2"+dmain+"background-color:#E6E6FA' id='tab_members' onclick='on_tab_members()'>"+tab1_name+"</div></div>"
   +dpre+"124"+dmain+"background-color:#F5F5F5' id='tab_abook' onclick='on_tab_abook()'>"+tab2_name+"</div></div>"
   +dpre+"246"+dmain+"background-color:#F5F5F5' id='tab_accounts' onclick='on_tab_accounts()'>"+window.l_accounts+"</div></div>"
   +"</div>";
  return s;
}
function on_tab_members(){
  if(document.getElementById('right_scroller')) document.getElementById('right_scroller').style.display = "block";
  if(document.getElementById('right_scroller_abook')) document.getElementById('right_scroller_abook').style.display = "none";
  document.getElementById('tab_members').style.backgroundColor = "#E6E6FA";
  document.getElementById('tab_abook').style.backgroundColor = "#F5F5F5";
  document.getElementById('tab_accounts').style.backgroundColor = "#F5F5F5";
  document.getElementById('additional_panel').style.display = "block";
  document.getElementById('additional_panel_abook').style.display = "none";
  members_list_display = 1;
  abook_list_display = 0;
  members_refresh();
}
function on_tab_abook(){
  if(document.getElementById('right_scroller')) document.getElementById('right_scroller').style.display = "none";
  if(document.getElementById('right_scroller_abook')) document.getElementById('right_scroller_abook').style.display = "block";
  document.getElementById('tab_members').style.backgroundColor = "#F5F5F5";
  document.getElementById('tab_abook').style.backgroundColor = "#E6E6FA";
  document.getElementById('tab_accounts').style.backgroundColor = "#F5F5F5";
  document.getElementById('additional_panel').style.display = "none";
  document.getElementById('additional_panel_abook').style.display = "block";
  members_list_display = 0;
  abook_list_display = 1;
  abook_refresh();
}
function on_tab_accounts(){
  if(document.getElementById('right_scroller')) document.getElementById('right_scroller').style.display = "none";
  if(document.getElementById('right_scroller_abook')) document.getElementById('right_scroller_abook').style.display = "block";
  document.getElementById('tab_members').style.backgroundColor = "#F5F5F5";
  document.getElementById('tab_abook').style.backgroundColor = "#F5F5F5";
  document.getElementById('tab_accounts').style.backgroundColor = "#E6E6FA";
  document.getElementById('additional_panel').style.display = "none";
  document.getElementById('additional_panel_abook').style.display = "block";
  members_list_display = 0;
  abook_list_display = 2;
  abook_refresh();
}

function members_sort_name_asc_func(i, j)
{
  if(i[2] > j[2])
    return 1;
  else if(i[2] < j[2])
    return -1;
  else
    return 0;
}

function abook_sort_name_asc_func(i, j)
{
  if(i[2] > j[2])
    return 1;
  else if(i[2] < j[2])
    return -1;
  else
    return 0;
}

function members_refresh()
{
  if(members_list_display == 0)
    return true;

 if(typeof members==='undefined'){
  document.getElementById('members_pan').innerHTML='ERROR: <i>members</i> variable not set';
  return false;
 }
 var tab_height = 23;
 var invite_panel_height = 27;
 var addpanel_height = 27;
 //if(document.getElementById('tab_panel')) tab_height = document.getElementById('tab_panel').offsetHeight;
 //if(document.getElementById('additional_panel')) addpanel_height = document.getElementById('additional_panel').offsetHeight;

 var p_height = 200;
 if(typeof total_height!='undefined') p_height = total_height - tab_height - invite_panel_height - addpanel_height - 4;

 if(!document.getElementById('right_scroller'))
   document.getElementById('members_pan').innerHTML='<div id="right_scroller" style="width:'+panel_width+';height:'+p_height+'px;overflow:hidden;overflow-y:auto;border-bottom:1px solid #E6E6FA;">Initializing panel...</div>';

 members.sort(members_sort_name_asc_func);

 offliners=false;
 var vmr='';
 var amr='';
 var imr='';
 for(i=0;i<members.length;i++)
 {
   mmbr=members[i];
   if(sortMode==0)
   {
     if(mmbr[0])
       if(visible_ids.indexOf(','+mmbr[1]+',')>=0) vmr+=format_mmbr_button(mmbr,2);
       else amr+=format_mmbr_button(mmbr,1);
     else imr+=format_mmbr_button(mmbr,0);
   }
   else
   {
     if(mmbr[0])
       if(visible_ids.indexOf(','+mmbr[1]+',')>=0) amr+=format_mmbr_button(mmbr,2);
       else amr+=format_mmbr_button(mmbr,1);
     else amr+=format_mmbr_button(mmbr,0);
   }
 }
 if(sortMode==0)
   result='<div style="width:"+panel_width+"px" id="right_pan">'+amr+vmr+imr+'</div>';
 else
   result='<div style="width:"+panel_width+"px" id="right_pan">'+amr+'</div>';

 var mp=document.getElementById('right_scroller');
 if(mp.innerHTML!=result)
   mp.innerHTML=result;

 for(i=0;i<members.length;i++) if(members[i][0]&&members[i][6])audio(members[i][1],members[i][6]);
 return true;
}

function abook_refresh()
{
  if(abook_list_display == 0)
    return true;

  if(typeof addressbook==='undefined')
  {
    document.getElementById('members_pan').innerHTML='ERROR: <i>addressbook</i> variable not set';
    return false;
  }

  var tab_height = 23;
  var invite_panel_height = 27;
  var addpanel_height = 27;
  //if(document.getElementById('tab_panel')) tab_height = document.getElementById('tab_panel').offsetHeight;
  //if(document.getElementById('additional_panel')) addpanel_height = document.getElementById('additional_panel').offsetHeight;

  var p_height=200;
  if(typeof total_height!='undefined') p_height = total_height - tab_height - invite_panel_height - addpanel_height - 4;

  if(!document.getElementById('right_scroller_abook'))
    document.getElementById('members_pan').innerHTML+='<div id="right_scroller_abook" style="width:'+panel_width+';height:'+p_height+'px;overflow:hidden;overflow-y:auto;border-bottom:1px solid #E6E6FA;"></div>';

  addressbook.sort(abook_sort_name_asc_func);

  var imr='';
  for(i=0,j=0,k=0;i<addressbook.length;i++)
  {
    mmbr = addressbook[i];
    var is_abook = mmbr[3];
    var is_account = mmbr[11];
    if(abook_list_display == 1 && is_abook)
      imr+=format_mmbr_abook(j++,mmbr);
    if(abook_list_display == 2 && is_account)
      imr+=format_mmbr_abook(k++,mmbr);
  }
  result="<div style='width:"+panel_width+"px' id='right_pan_abook'>"+imr+"</div>";

  var abook=document.getElementById("right_scroller_abook");
  if(abook.innerHTML!=result)
    abook.innerHTML=result;

  return true;
}

function abook_change(account)
{
  if(account[0] == 2)
  {
    for(i=0;i<addressbook.length;i++)
    {
      if(addressbook[i][1] == account[1])
      {
        addressbook.splice(i,1);
        break;
      }
    }
  } else {
    for(i=0;i<addressbook.length;i++)
    {
      if(addressbook[i][1] == account[1])
      {
        addressbook[i] = account;
        account = null
        break;
      }
    }
    if(account != null)
      addressbook[addressbook.length] = account;
  }
  alive();
  members_refresh();
  abook_refresh();
}

function audio(id,vol)
{
  if(vol>32767)vol=32767; // signal has been amplified with overload - fix it
  for(var i=0;i<members.length;i++) if(members[i][1]==id)
  {
    members[i][6]=vol;
    var o='rpan_'+idid(id), o2;
    try
    {
      if(document.getElementById(o)==='undefined')return alive();
      if(document.getElementById('s'+o)==='undefined')return alive();
      o2=document.getElementById('s'+o);
      o=document.getElementById(o);
    } catch(e) { return alive(); }

    var v16=Math.floor(vol/1928), htmlCode;
    var v17=0,vol0=vol; while(vol0>0){vol0=vol0>>1;v17++;}
    v16=Math.floor((v16+v17)/2+1);
    if(v16==0) htmlCode='';
    else htmlCode="<div style='position:relative;top:" + (16-v16) + "px' class='audl" + v16 + "'></div>";
    try { o2.innerHTML=htmlCode; } catch(e) {}
    break;
  }
  alive();
}

function get_addr_name(addr){
  return addr.substring(0, addr.lastIndexOf('['));
}
function get_addr_url(addr){
  return addr.substring(addr.lastIndexOf('[')+1, addr.lastIndexOf(']'));
}
function get_addr_url_without_param(addr){
  var url = get_addr_url(addr);
  if(url.lastIndexOf(";") != -1)
    url = url.substring(0, url.lastIndexOf(';'));
  return url;
}
function get_addr_nameid(addr)
{
  url = get_addr_url(addr)
  if(url.substring(0,5) == "rtsp:")
    return url;
  return url.substring(0, url.indexOf('@'));
}

function addmmbr(st,id,name,mute,dvad,cvan,al,mixr,membername_id,chmask,gl,og,mixconf,mtype,autoDial,resizerRule)
{
  if(typeof members==='undefined') return alive();
  var j=members.length;
  dmsg('addmmbr('+st+','+id+','+name+'...); l1='+j);

  var nameLength = name.length;
  var dhPos = name.lastIndexOf(" ##");
  var dup = (dhPos >= nameLength-8);
  var online = (st!=0);

  var i;
  for(i=j-1;i>=0;i--)
  {
    if((!dup) && (members[i][0] == 0))
    { // нормальный:
      if(online)
      {
        if((members[i][1] == id) || (members[i][2] == name)) members.splice(i,1);
      }
      else
      { // offline: skip id check
        if(members[i][2] == name) members.splice(i,1);
      }
    }
    else
    { // дубль:
      if((members[i][1] == id) || (members[i][2] == name)) members.splice(i,1);
    }
  }
  j=members.length;
  dmsg('l2='+j);

  members[j]=Array(st,id,name,mute,dvad,cvan,al,mixr,membername_id,chmask,gl,og,mixconf,mtype,autoDial,resizerRule);
  alive();
  members_refresh();
  top_panel();
  var check=false, i=0;
  do
  {
    if(mixer_refresh_timer == null)
    {
      mixer_refresh_timer = setTimeout(function(){mixer_refresh_timer=null;queue_otf_request(OTFC_REFRESH_VIDEO_MIXERS,0);},250);
      check = true;
    }
    i++;
  } while ((!check) && (mixer_refresh_timer==null) && (i < 12));
}

function chmix(id,mx){
  if(typeof members!='undefined')
  for(var i=0;i<members.length;i++) if(members[i][1]==id){
    members[i][7]=mx;
    members_refresh();
    break;
  }
  alive();
}

function remmmbr(st,id,name,mute,dvad,cvan,al,mixr,membername_id,chmask,gl,og,mixconf,mtype,autoDial,resizerRule)
{
  if(typeof members==='undefined') return alive();
  var found=0; var j=members.length;
  for(var i=j-1;i>=0;i--)
  if((members[i][2]==name)||(members[i][1]==id))
  if(found){ members.splice(i,1); j--; } else { found=1; j=i; }
  members[j]=Array(st,id,name,mute,dvad,cvan,al,mixr,membername_id,chmask,gl,og,mixconf,mtype,autoDial,resizerRule);
  alive();
  members_refresh();
  top_panel();
  var check=false, i=0;
  do
  {
    if(mixer_refresh_timer == null)
    {
      mixer_refresh_timer = setTimeout(function(){mixer_refresh_timer=null;queue_otf_request(OTFC_REFRESH_VIDEO_MIXERS,0);},250);
      check = true;
    }
    i++;
  } while ((!check) && (mixer_refresh_timer==null) && (i < 12));
}

function remove_all()
{
  if(typeof members==='undefined') return alive();
  for(var i=members.length-1;i>=0;i--) if(!members[i][0]) members.splice(i,1);
  members_refresh();
  alive();
}

function drop_all()
{
  if(typeof members==='undefined') return alive();
  for(var i=members.length-1;i>=0;i--) if(members[i][0]) {members[i][0]=0; members[i][1]=0;}
  for(var i=members.length-1;i>0;i--) for(var j=i-1;i>=0;j--) if(members[i][2]==members[j][2]) {members.splice(j,1); break;}
  members_refresh();
  alive();
}

function set_mute_sprite(bitMask,id,value)
{
  var o=document.getElementById('mrpan'+bitMask+'_'+idid(id));
  if(!o) return;
  if(value)
  {
    o.className='mutespr'+bitMask+'0';
    o.title='Unmute';
  }
  else
  {
    o.className='mutespr'+bitMask+'1';
    o.title='Mute';
  }
}

function imute(id,mask)
{
  for(var i=0;i<members.length;i++)
    if(members[i][1]==id)
    {
      members[i][3]|=mask;
      for(var bit=1;bit<32;bit=bit<<1)
        if(mask&bit) set_mute_sprite(bit,id,bit);
    }
  alive();
}

function imute_all(v,mask0)
{
  var mask=1; if(typeof mask0!='undefined') mask=mask0;
  var cl='mutespr' + mask + (v?'0':'1');
  for(var i=0;i<members.length;i++)
  {
    if(v)
    {
      members[i][3]|=mask;
      set_mute_sprite(mask,members[i][1],mask);
    }
    else
    {
      members[i][3]&=(~mask);
      set_mute_sprite(mask,members[i][1],0);
    }
  }
  return alive();
}

function iunmute(id,mask)
{
  for(var i=0;i<members.length;i++)
    if(members[i][1]==id)
    {
      members[i][3]&=(~mask);
      for(var bit=1;bit<32;bit=bit<<1)
        if(mask&bit) set_mute_sprite(bit,id,0);
    }
  alive();
}

function ivad(id,v)
{
  dmsg('VAD switch for id '+id+', value '+v);
  for(var i=0;i<members.length;i++)
  if(members[i][1]==id)
  {
    var o = document.getElementById('vrpan_'+idid(id));
    if(o) o.className='vadspr'+(v+8);
    if(v==2) //disable vad
    {
      members[i][4]=1;
      members[i][5]=0;
      if(o) o.title='VAD disabled';
    }
    else if(v==1)
    {
      members[i][4]=0;
      members[i][5]=1;
      if(o) o.title='Chosen Van';
    }
    else
    {
      members[i][4]=0;
      members[i][5]=0;
      if(o) o.title='VAD';
    }
    break;
  }
  alive();
}

function button_control()
{
  if(classicMode) return false;
  if(isModerated)
  {
    if(mixers>1)if(!confirm(window.l_decontrolmixersconfirm)) return false;
    queue_otf_request(OTFC_DECONTROL,0);
  } else queue_otf_request(OTFC_TAKE_CONTROL,0);
  return false;
}

function room_change(newRoom)
{
  try
  {
    var oldUrl=document.getElementById('loggingframe').contentWindow.location.href;
    var newUrl=oldUrl.substr(0,oldUrl.indexOf('room='))+'room='+encodeURIComponent(newRoom);
    if(oldUrl!=newUrl) document.getElementById('loggingframe').contentWindow.location.href=newUrl;
  } catch(e){
    location.href='Select';
  }
}

function get_tpllock_src()
{ if(isTemplateLocked) return "i32_lock.png";
  return "i32_lockopen.png";
}
function get_template_lock()
{ try{ isTemplateLocked=conf[0][12]; } catch(e){}
  return "<img id='imgTplLock' src='" + get_tpllock_src() + "' width=32 height=32 onclick='queue_otf_request("
  + OTFC_TOGGLE_TPL_LOCK + ",document.getElementById(\"templateSelector\").value)' style='vertical-align:middle;cursor:pointer' />";
}
function tpllck(i)
{ isTemplateLocked = i;
  document.getElementById('imgTplLock').src=get_tpllock_src();
  alive();
}

function top_panel()
{
  if(vad_setup_mode||tpl_save_mode) return;
  try
  {
    var t=document.getElementById('cb1');
    roomName    =  conf[0][3];
    isModerated = (conf[0][4] != '-');
    globalMute  = (conf[0][5] != '0');
    vad1 = 0+conf[0][6]+0; vad2 = 0+conf[0][7]+0; vad3 = 0+conf[0][8]+0;

    roomList=[]; roomStrings=[];
    for(var i=0;i<conf[0][9].length;i++)
    {
      roomList[i]=conf[0][9][i][0];
      roomStrings[i]=conf[0][9][i][0]+conf[0][9][i][2]+' ('+(conf[0][9][i][1])+')';
    }
  } catch(e){ return false;}

  var title;
  
  var c="<table width='100%'><tr><td width='70%'>";
  c+="<form style='margin:0px;margin-left:8px;padding:0px' name='FakeForm1'>";

  var who='robot'; if(isModerated) {who='human'; title=window.l_decontrol;} else title=window.l_takecontrol;
  c+="<button title='" + title + "' onclick='javascript:{button_control();return false;}' class='"+who+"spr'></button>";

  c+=" ";

  c+="<select class='btn btn-small btn-";
  if(isModerated)c+="success"; else c+="primary";
  c+="' style='height:28px;' name='RoomSelector' onchange='room_change(this.value);return false'>";
  for(i=0;i<roomList.length;i++)
  {
    c+="<option value=\""+roomList[i]+"\"";
    if(roomList[i]==roomName) c+=" selected";
    c+=">"+roomStrings[i]+"</option>";
  }
  c+="</select>";

  c+=" ";

  var j; if(globalMute) {j=0; title=window.l_globalunmute;} else {j=1; title=window.l_globalmute;}
  c+="<button title='" + title + "' onclick='javascript:{if(checkcontrol())queue_otf_request(OTFC_GLOBAL_MUTE,"+(!globalMute)+");return false;}' class='mutespr" + j + "'></button>";

  c+=" ";

  title=window.l_vadsetup;
  c+="<button title='" + title + "' onclick='javascript:{if(checkcontrol())vad_setup();return false;}' class='vadsetupspr'></button>";

  try{ recState=conf[0][11]; } catch(e) { recState=0; }
  if(recState==1) title=window.l_videorecorderstop; else title=window.l_videorecorder;
  c+=" <button title='" + title + "' onclick='javascript:{queue_otf_request(OTFC_VIDEO_RECORDER_" + (recState?"STOP":"START") + ",0);return false;}' class='recordspr" + recState + "'></button>";

  c+=" <button title='Sort mode'"
    +" onclick='javascript:{sortMode=(sortMode+1)%2;this.className=\"sortspr\"+sortMode;"
    + "document.cookie=\"sortMode=\"+sortMode+\"; expires=Fri, 31 Dec 2999 23:59:59 GMT\";members_refresh();return false;}'"
    +" class='sortspr" + sortMode + "'></button>";

  var i; try{ i=conf[0][10]; }catch(e){i=0;} //if((i<1)||(i>3))i=0;
  title=window.l_filtermode[i-1];
  c+=' '+ get_resizer(i);

  c+="</td><td width='30%' align=right id='savetpl' name='savetpl'><nobr>";

  c+=get_template_lock();
  c+="<input type='button' class='btn btn-large btn-danger' style='width:20px;padding-left:0px;padding-right:0px;margin-right:1px' value='&ndash;' onclick='" +
    "javascript:{if(confirm(window.l_templatedeleteconfirm.replace(/\\*/g, document.getElementById(\"templateSelector\").value)))queue_otf_request("+OTFC_DELETE_TEMPLATE+",document.getElementById(\"templateSelector\").value);}'>";
  c+="<select class='btn btn-large btn-disabled' style='margin-left:1px;height:39px' id='templateSelector' name='templateSelector' onchange='queue_otf_request("+OTFC_TEMPLATE_RECALL+",this.value)'>";
   if(typeof tl=='undefined') tl=Array();
   if(typeof seltpl=='undefined') seltpl='';
    c+="<option value=\"\""; if(seltpl=="") c+=" selected"; c+=">&nbsp;</option>";
    for(var i=0;i<tl.length;i++)
    {
      c+="<option value=\"" + tl[i] +"\"";
      if(tl[i]==seltpl) c+=" selected";
      c+=">"+tl[i]+"</option>";
    }
  c+="</select>";
  c+="<input onclick='javascript:{save_template();}' type='button' class='btn btn-large btn-inverse' value='"+window.l_name_save+"'>";
  c+="</form>";

  c+="</nobr></td></tr></table>";

  t.innerHTML=c;
}

function save_template(finalName)
{
  if(typeof finalName!='undefined') //stage 2
  {
    while ( (finalName!="") && (finalName[0]==' ') ) finalName=finalName.substr(1);
    while ( (finalName!="") && (finalName[finalName.length-1]==' ') ) finalName=finalName.substring(0,finalName.length-1);
    if(finalName!="") queue_otf_request(OTFC_SAVE_TEMPLATE,finalName);
    tpl_save_mode=false;
    top_panel();
    return;
  }

  if(tpl_save_mode) return false; else tpl_save_mode=true;

  var tDefName=document.getElementById('templateSelector').value;
  if(tDefName[tDefName.length-1]=='*') tDefName=tDefName.substring(0,tDefName.length-1);

  var s=""
    + "<table border=0 cellpadding=0 cellspacing=0 width='100%'><tr><td width='20%' valign='middle'>"
    + "<b></b></td><td width='80%' align='right' valign='middle'>"
    + "<input type='button' disabled class='btn-large btn-info' value='"+window.l_enter_template_name+": '>"
    + "<input name='tplname' id='tplname' class='btn btn-large' value=\"" + tDefName + "\" style='width:60px'"
    + " onkeyup='javascript:{if(event.keyCode==13){save_template(document.getElementById(\"tplname\").value);}else if(event.keyCode==27){tpl_save_mode=false;top_panel();}}'"
    + ">"
    + "<input type='button' onclick='javascript:{save_template(document.getElementById(\"tplname\").value);}' type='button' class='btn btn-large btn-danger' value='"+window.l_name_save+"'>"
    + "<input type='button' onclick='javascript:{tpl_save_mode=false;top_panel();}' class='btn btn-large btn-inverse' value='"+window.l_name_cancel+"'>";
    + "</td></tr></table>";

  document.getElementById('cb1').innerHTML=s;
  document.getElementById('tplname').focus();
  document.getElementById('tplname').select();
}

function vad_save(){
  var vad11=document.getElementById('vad1').value;
  var vad22=document.getElementById('vad2').value;
  var vad33=document.getElementById('vad3').value;
  vad_setup_mode=false;
  queue_otf_request(OTFC_SET_VAD_VALUES,vad11,vad22,vad33);
  top_panel();
}

function vad_setup(){
  if(tpl_save_mode) {my_alert('Please close "Template Saving" dialog first'); return false;}
  vad_setup_mode=true;
  var inputst="<input class='span2' style='"+VAD_FIELD_STYLE+"' ";
  var pfx3="<span class='add-on'>";
  var sfx="</span>&nbsp;&nbsp;&nbsp;";
  
  var c ="<div style='border-radius:15px;background-color:#dec;border:1px solid #acb;padding-left:15px'><form name='fakeform2'><div class='input-prepend input-append'>";
    c  +="<br><b>Voice Activation Volume:</b> "+inputst+"name='vad1' id='vad1' value='"+vad1+"' size=5 maxlength=5>"+pfx3+"level: 25...25000"+sfx;
    c  +="<b>Delay:</b> " +inputst+"name='vad2' id='vad2' value='"+vad2+"' size=5 maxlength=5>"+pfx3+"ms: 0...65535"   +sfx;
    c  +="<b>Timeout:</b> "          +inputst+"name='vad3' id='vad3' value='"+vad3+"' size=5 maxlength=5>"+pfx3+"ms: 0...65535"   +sfx;
  c+="<input type='button' onclick='javascript:vad_save()' class='btn btn-danger' value='Save'> ";
  c+="<input type='button' onclick='javascript:{vad_setup_mode=false;top_panel();}' class='btn btn-inverse' value='Cancel'>";
  c+="</div></form></div>";
  document.getElementById('cb1').innerHTML=c;
  document.getElementById('vad1').focus();
  document.getElementById('vad1').select();
}

function frameload(i)
{
  jpegframes[i][1].onload=function(){reframe(i);};
  jpegframes[i][1].src=jpegframes[i][0]+'&r='+Math.random();
}
function reframe(i)
{
  document.getElementById('frame'+i).src=jpegframes[i][1].src;
  jpegframes[i][2]=setTimeout(function(){frameload(i);},1999);
}

function selector_window(mixerNumber)
{
  if(typeof splitdata2=='undefined') {my_alert('Server error: no split data received'); return;}
  if(splitSelectorWindow!=null)
  {
    splitSelectorWindow.close();
    splitSelectorWindow=null;
  }
  splitSelectorWindow=window.open('','Thumbnails','menubar=0,status=0,titlebar=0,scrollbars=1');
  splitSelectorWindowMixerNumber=mixerNumber;

  var s="<!DOCTYPE HTML PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n"
    + "<html xmlns=\"http://www.w3.org/1999/xhtml\">\n"
    + "<head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" />"
    + "<title>Thumbnails</title>"
    + "<style>"
    + ".tn{width:176px;height:176px;overflow:hidden;float:left;margin:8px;cursor:pointer;border:2px solid #ccc;border-radius:6px}"
    + ".tn:hover{border:6px solid #bfb;margin:4px;border-radius:12px;background-color:#bfb}"
    + ".tnp{position:relative;width:0px;height:0px}"
    + ".tni{overflow:hidden;text-align:center;vertical-align:middle;display:table-cell;border:1px dotted black;font-size:12px}"
    + ".tns{overflow:hidden;text-align:center;vertical-align:middle;display:table-cell}"
    + "</style>\n"
    + "<script>"
    + "  function block(x,y,w,h,n)"
    + "  {"
    + "    document.write(\"<div class='tnp' style='left:\"+x+\"px;top:\"+y+\"px'><div class=tni style='background-color:#\"+(\"faaafafafffa\".substr(Math.floor(Math.random()*9.9999999999),3))+\";width:\"+w+\"px;height:\"+h+\"px;min-width:\"+w+\"px'>\"+n+\"</div></div>\");"
    + "  }"
    + "  function sign(s)"
    + "  {"
    + "    document.write(\"<div class='tnp' style='left:0px;top:0px'><div style='width:176px;height:144px;min-width:176px;background-color:#000'></div></div>\");"
    + "    document.write(\"<div class='tnp' style='left:0px;top:144px'><div class=tns style='width:176px;height:32px;min-width:176px'><nobr>\"+s+\"</nobr></div></div>\");"
    + "  }"
    + "</script>\n"
    + "</head>\n\n"
    + "<body>";
  var i;
  for(i=0;i<splitdata.length;i++)
  {
    var name=splitdata[i].replace(/'/g, "\\'");
    s+="<div class='tn' onclick='window.opener.set_split_close("+i+")'><script>sign('"+name+"');";
    var j;
    var t=splitdata2[splitdata[i]];
    for(j=0;j<t[0];j++)
    {
      s+="block("+(t[j+1][0]>>2)+","+(t[j+1][1]>>2)+","+(t[j+1][2]>>2)+","+(t[j+1][3]>>2)+","+j+");";
    }
    s+="</script></div>";
  }
  + "</body>"
  + "</html>";
  splitSelectorWindow.document.write(s);
}

function set_split_close(i)
{
  if(splitSelectorWindow!=null)
  {
    splitSelectorWindow.close();
    splitSelectorWindow=null;
  }
  var mixerNumber=splitSelectorWindowMixerNumber;
  splitSelectorWindowMixerNumber=-1;
  if(checkcontrol())queue_otf_request(OTFC_SET_VIDEO_MIXER_LAYOUT,i,mixerNumber);
}

function split_selector(mixer, splitSelected)
{
  if(mixer<0) selector_index='_'+(0-mixer)+''; else selector_index=''+mixer+'';
  if(typeof splitdata=='undefined')return 'No splits available';
  var r="<span style='"+MIXER_LAYOUT_SCROLL_LEFT_STYLE+"' onclick='javascript:{if(checkcontrol())queue_otf_request("
      +OTFC_SET_VIDEO_MIXER_LAYOUT
      +",(parseInt(document.getElementById(\"splitselector"+selector_index+"\").value)+splitdata.length-1)%splitdata.length,"+mixer+");}'"
    +">"+MIXER_LAYOUT_SCROLL_LEFT_BUTTON+"</span>";
  r+="<select id='splitselector"+selector_index+"' name='splitselector"+selector_index+"' style='width:70px;padding-left:0px;padding-right:0px;font-size:12px;letter-spacing:-1px' onchange='javascript:{if(checkcontrol())queue_otf_request("+OTFC_SET_VIDEO_MIXER_LAYOUT+",this.value,"+mixer+"); else this.value="+splitSelected+";}'>";
  for(var i=0;i<splitdata.length;i++)
  {
    r+="<option value="+i;
    if(i==splitSelected) r+=" selected";
    r+=">"+splitdata[i]+"</option>";
  }
  r+="</select>";
  r+="<button class='lgalspr' onclick='selector_window("+mixer+")'>&nbsp;</button>";
  r+="<span style='"+MIXER_LAYOUT_SCROLL_RIGHT_STYLE+"' onclick='javascript:{if(checkcontrol())queue_otf_request("
      +OTFC_SET_VIDEO_MIXER_LAYOUT
      +",(parseInt(document.getElementById(\"splitselector"+selector_index+"\").value)+1)%splitdata.length,"+mixer+");}'"
      +">"+MIXER_LAYOUT_SCROLL_RIGHT_BUTTON+"</span>";
  return r;
}

function build_page()
{ try {mixer_out(0);} catch(e){};
  try{ if(typeof jpegframes!='undefined')for(i=jpegframes.length-1;i>=0;i--)if(jpegframes[i][2])clearTimeout(jpegframes[i][2]); } catch(e){};
  jpegframes=Array();
  top_panel();
  if(typeof conf=='undefined' || typeof document.getElementById('cb3')=='undefined') return;
  b=document.getElementById('cb3');
  mixers=conf[0][0]; bfw=conf[0][1]; bfh=conf[0][2]; room=conf[0][3]; roomLink=encodeURIComponent(room);
  classicMode=(mixers==0);
  dmsg('*BUILDPAGE|mixers='+mixers+'|bfw='+bfw+'|roomlink='+roomLink+'|CM='+classicMode+'*');

  var page_width = document.body.clientWidth || default_page_width;
  if(page_width != default_page_width) {
    page_width = page_width-page_border_width;
  }

  var i;

  if(typeof mmw == 'undefined') mmw=-1; if(mmw==-1)
  { mmw=0; total_height=0;
    if(!classicMode)
    {
      for(i=0;i<mixers;i++)
      {
        var mw=conf[i+1][0][0]; var mh=conf[i+1][0][1];
        if(mw>mmw) mmw=mw;
        total_height+=mh;
        total_height+=MIXER_PANEL_HEIGHT;
      }
    }
    else // classic mode:
    {
      if(typeof members=='undefined') alert('error, undefined members :(');
      for(i=0;i<members.length;i++)
      {
        if(!members[i][0]) continue;
        var mw=members[i][12][0][0]; var mh=members[i][12][0][1];
        if(mw>mmw) mmw=mw;
        total_height+=mh;
        total_height+=MIXER_PANEL_HEIGHT;
      }
    }
    if(mmw<200)mmw=200; //minimal mixer width
    var free_x=page_width-mmw;
    if(free_x<444)
      page_width=mmw+444;
    log_width=free_x/2;
    panel_width=page_width-log_width-mmw-5;
    if(panel_width<275){ log_width-=(275-panel_width); panel_width=275; }
  }

  if(total_height < 460) total_height = 460;

  try {
    var l0=document.getElementById('logging0');
    var l1=document.getElementById('logging1');
    var l2=document.getElementById('loggingframe');
    l0.style.left='-'+(log_width-5)+'px';
    l1.style.width=(log_width-8)+'px'; l1.style.height=(total_height-2)+'px';
    l2.style.width=(log_width-8)+'px'; l2.style.height=(total_height-2)+'px';
    l0=document.getElementById('cb2');
    l0.style.width=''+mmw+'px'; l0.style.height=total_height+'px';
  } catch(e) {};

  mockup_content=""; var pos_y=0;
  if(!classicMode)
  {
    for(i=0;i<mixers;i++)
    {
      var mw=conf[i+1][0][0]; var mh=conf[i+1][0][1]; var pos_x=(mmw-mw)>>1;
      mixer_number=conf[i+1][0][3];
      mockup_content+="<div style='position:relative;top:"+pos_y+"px;left:"+pos_x+"px;width:0px;height:0px'>"; // pointing block for mockup[i]
       mockup_content+="<img style='position:absolute' id='frame"+mixer_number+"' name='frame"+mixer_number+"'"+ // rectangle for mockup[i]
        " src='Jpeg?room="+roomLink+"&w="+mw+"&h="+mh+"&mixer="+mixer_number+"'"+
        " alt='Video Mixer #"+mixer_number+"'"+
       " />";
      mockup_content+="</div>";
      pos_y+=mh+MIXER_PANEL_HEIGHT;
      jpegframes[mixer_number]=Array("Jpeg?room="+roomLink+"&w="+mw+"&h="+mh+"&mixer="+mixer_number,new Image(),null);
    }
  }
  else // classic mode
  {
    if(typeof members=='undefined') alert('error 2, undefined members :(((');
    for(i=0;i<members.length;i++)
    {
      if(!members[i][0]) continue;
      var mw=members[i][12][0][0]; var mh=members[i][12][0][1]; var pos_x=(mmw-mw)>>1;
      mockup_content+="<div style='position:relative;top:"+pos_y+"px;left:"+pos_x+"px;width:0px;height:0px'>"; // pointing block for mockup[i]
       mockup_content+="<img style='position:absolute' id='frame"+i+"' name='frame"+i+"'"+ // rectangle for mockup[i]
        " src='Jpeg?room="+roomLink+"&w="+mw+"&h="+mh+"&mixer="+members[i][1]+"'"+
        " alt='Video Mixer #"+i+"'"+
       " />";
      mockup_content+="</div>";
      pos_y+=mh+MIXER_PANEL_HEIGHT;
      jpegframes[i]=Array("Jpeg?room="+roomLink+"&w="+mw+"&h="+mh+"&mixer="+members[i][1],new Image(),null);
    }
  }

  workplace_content="<div id='pbase' style='position:relative;top:0px;left:0px;width:0px;height:0px'>"; // just workplace pointer
    workplace_content+="&nbsp;";
  workplace_content+="</div>";

  panel_content="<div id='pp_2' style='position:relative;top:0px;left:"+mmw+"px;width:0px;height:0px'>"; // pointing block for panel
    panel_content += tab_panel();
    panel_content += invite_panel();
    panel_content += additional_panel();
    panel_content += additional_panel_abook();
    panel_content+="<div"+ // rectangle for panel
      " onmouseover='ddover(event,this,\"panel\",-1)'"+
      " onmouseout='ddout(event,this,\"panel\",-1)'"+
      " id='members_pan'"+
      " name='members_pan'"+
      " style='position:absolute;top:81px;width:"+panel_width+"px;height:"+total_height+"px'>";
      panel_content+="...";
    panel_content+="</div>";
  panel_content+="</div>";

  if(!classicMode)
    vmp_content = get_mixers_content(); //sets visible_ids
  else
    vmp_content = get_mixers_content_classic(); //sets visible_ids

  main_content = mockup_content;
  main_content += workplace_content;
  main_content += panel_content;
  main_content += vmp_content;

  b.innerHTML=main_content;
  on_tab_members();
  alive();
  if(!classicMode)
  {
    for(i=0;i<mixers;i++)
    {
      mixer_number=conf[i+1][0][3];
      frameload(mixer_number);
    }
  }
  else // classic mode:
  {
    for(i=0;i<members.length;i++)frameload(i);
  }
}

function get_mixers_content()
{
  var s='', pos_y=0;
  visible_ids=',';
  hl_links=[];
  for(i=0;i<conf[0][0];i++)
  {
    var mw=conf[i+1][0][0]; var mh=conf[i+1][0][1]; var pos_x=(mmw-mw)>>1;
    mixer_number=conf[i+1][0][3];
    s+="<div id='mixercontrol"+mixer_number+"'>";
// walking through the mixer's positions
    for(var j=0;j<conf[i+1][1].length;j++)
    { var id=false; try { if(typeof conf[i+1][2][0][j] != 'undefined') id=conf[i+1][2][0][j]; } catch(e) {id=false;}
      if(id!==false) if((id!=0)&&(id!=-1)&&(id!=-2)) visible_ids += ''+id+',';
      var type=false; if(id!==false) try { if(typeof conf[i+1][2][1][j]!='undefined') type=conf[i+1][2][1][j]; } catch(e) {type=false;}
      var x=Math.round(conf[i+1][1][j][0]/bfw*mw); var y=Math.round(conf[i+1][1][j][1]/bfh*mh);
      var w=Math.round(conf[i+1][1][j][2]/bfw*mw); var h=Math.round(conf[i+1][1][j][3]/bfh*mh);
      var border='1px solid '+MIX_BORDER_COLOR; if(type===2)border='1px solid red'; else if(type===3) border='0;border-right:2px solid yellow;border-bottom:2px solid yellow';
      if(id!==false)if(id!=-1)if(id!=-2){ if(index_exists(hl_links,id)) hl_links[id].push("pr"+mixer_number+"_"+j); else { hl_links[id]=[]; hl_links[id][0]="pr"+mixer_number+"_"+j; }}
      s+="<div id='pp"+mixer_number+"_"+j+"' style='position:relative;top:"+(pos_y+y)+"px;left:"+(pos_x+x)+"px;width:0px;height:0px'>"; // pointing block for member's mockup
        s+="<div id='pr"+mixer_number+"_"+j+"'"+ // rectangle for member's mockup
          " onmousedown='ddstart(event,this,"+mixer_number+","+j+")' onmouseover='ddover(event,this,"+mixer_number+","+j+")' onmouseout='ddout(event,this,"+mixer_number+","+j+")'"+
          " style='overflow:hidden;opacity:0.5;filter:alpha(opacity=50);position:absolute;background-color:#F2F2F2;text-align:center;"+
           "border:"+border+";padding:0;cursor:move;width:"+(w-2)+"px;height:"+(h-2)+"px'>";
        s+=get_mixer_position_html(i, j);
        s+="</div>";
      s+="</div>";
    }
    s+="</div>";

    // mixer panel:
      s+="<div id='mxp"+mixer_number+"' style='position:relative;top:"+(pos_y+mh)+"px;left:0px;width:0px;height:0px'>"; // pointing block for mixer panel
        s+="<div onmouseover='mixer_over("+mixer_number+")' onmouseout='mixer_out("+mixer_number+")' style='width:"+(mmw-2)+"px;height:"+(MIXER_PANEL_HEIGHT-1)+"px;background-color:"+MIXER_PANEL_BGCOLOR+";overflow:hidden;text-align:left;line-height:"+(MIXER_PANEL_HEIGHT-1)+"px;padding-left:2px'>";
          s+="<nobr><span style='"+MIXER_PANEL_MIXER_STYLE+"'>#"+mixer_number;

          s+=" <span style='color:red;cursor:pointer;position:relative;left:0px' onclick='javascript:{if(checkcontrol())queue_otf_request("+OTFC_ADD_VIDEO_MIXER+","+mixer_number+");}'>+</span> ";
          if(mixers>1)s+=" / <span style='color:blue;cursor:pointer;position:relative;left:0px' onclick='javascript:{if(checkcontrol())queue_otf_request("+OTFC_DELETE_VIDEO_MIXER+","+mixer_number+");}'>&ndash;</span>";

          s+="</span>&nbsp;";
          s+=split_selector(mixer_number, conf[i+1][0][2]);

          var str0="&nbsp;<img src='i24_";
          var str1=" width=24 height=24 style='vertical-align:middle;cursor:pointer' onclick='javascript:{if(checkcontrol())queue_otf_request(";
          var str2=","+mixer_number+");}' onmouseover='this.style.backgroundColor=\""+MIXER_PANEL_BGCOLOR2+"\"' onmouseout='this.style.backgroundColor=\""+MIXER_PANEL_BGCOLOR+"\"' />&nbsp;&nbsp;";

          s += str0 + "mix.gif' alt='Arrange members'" + str1 + OTFC_MIXER_ARRANGE_VMP + str2
            +  str0 + "revert.gif' alt='Revert'"       + str1 + OTFC_MIXER_REVERT + str2
            +  str0 + "left.gif' alt='Rotate left'"    + str1 + OTFC_MIXER_SCROLL_LEFT + str2
            +  str0 + "shuff.gif' alt='Shuffle'"       + str1 + OTFC_MIXER_SHUFFLE_VMP + str2
            +  str0 + "right.gif' alt='Rotate right'"  + str1 + OTFC_MIXER_SCROLL_RIGHT + str2
            +  str0 + "clr.gif' alt='Clear positions'" + str1 + OTFC_MIXER_CLEAR + str2;

          s+="</nobr>";
        s+="</div>";
      s+="</div>";
    pos_y+=mh+MIXER_PANEL_HEIGHT;
  }
  return s;
}

function get_mixers_content_classic()
{
  var s='', pos_y=0;
  visible_ids=',';
  hl_links=[];
  for(i=0;i<members.length;i++)
  { s+="<div id='mixercontrol"+i+"'>";
    var mw=members[i][12][0][0]; var mh=members[i][12][0][1]; var pos_x=(mmw-mw)>>1;
// walking through the mixer's positions
    for(var j=0;j<members[i][12][1].length;j++)
    {
      var id=false; try { if(typeof members[i][12][2][0][j] != 'undefined') id=members[i][12][2][0][j]; } catch(e) {id=false;}
      if(id!==false) if((id!=0)&&(id!=-1)&&(id!=-2)) visible_ids += ''+id+',';
      var type=false; if(id!==false) try { if(typeof members[i][12][2][1][j]!='undefined') type=members[i][12][2][1][j]; } catch(e) {type=false;}
      var x=Math.round(members[i][12][1][j][0]/bfw*mw); var y=Math.round(members[i][12][1][j][1]/bfh*mh);
      var w=Math.round(members[i][12][1][j][2]/bfw*mw); var h=Math.round(members[i][12][1][j][3]/bfh*mh);
      var border='1px solid '+MIX_BORDER_COLOR; if(type===2)border='1px solid red'; else if(type===3) border='0;border-right:2px solid yellow;border-bottom:2px solid yellow';
      if(id!==false)if(id!=-1)if(id!=-2){ if(index_exists(hl_links,id)) hl_links[id].push("pr"+i+"_"+j); else { hl_links[id]=[]; hl_links[id][0]="pr"+i+"_"+j; }}
      s+="<div id='pp"+i+"_"+j+"' style='position:relative;top:"+(pos_y+y)+"px;left:"+(pos_x+x)+"px;width:0px;height:0px'>"; // pointing block for member's mockup
        s+="<div id='pr"+i+"_"+j+"'"+ // rectangle for member's mockup
          " onmousedown='ddstart(event,this,"+i+","+j+")' onmouseover='ddover(event,this,"+i+","+j+")' onmouseout='ddout(event,this,"+i+","+j+")'"+
          " style='overflow:hidden;opacity:0.5;filter:alpha(opacity=50);position:absolute;background-color:#F2F2F2;text-align:center;"+
           "border:"+border+";padding:0;cursor:move;width:"+(w-2)+"px;height:"+(h-2)+"px'>";
        s+=get_mixer_position_html(i,j);
        s+="</div>";
      s+="</div>";
    }
    s+="</div>";

    // mixer panel:
      s+="<div id='mxp"+i+"' style='position:relative;top:"+(pos_y+mh)+"px;left:0px;width:0px;height:0px'>"; // pointing block for mixer panel
        s+="<div onmouseover='mixer_over("+i+")' onmouseout='mixer_out("+i+")' style='width:"+(mmw-2)+"px;height:"+(MIXER_PANEL_HEIGHT-1)+"px;background-color:"+MIXER_PANEL_BGCOLOR+";overflow:hidden;text-align:left;line-height:"+(MIXER_PANEL_HEIGHT-1)+"px;padding-left:2px'>";
          s+="<nobr><span style='"+MIXER_PANEL_MIXER_STYLE+"'>#"+i;

          s+=" <span style='color:red;cursor:pointer;position:relative;left:0px' onclick='javascript:{if(checkcontrol())queue_otf_request("+OTFC_ADD_VIDEO_MIXER+","+i+");}'>+</span> ";
          if(mixers>1)s+=" / <span style='color:blue;cursor:pointer;position:relative;left:0px' onclick='javascript:{if(checkcontrol())queue_otf_request("+OTFC_DELETE_VIDEO_MIXER+","+i+");}'>&ndash;</span>";

          s+="</span>&nbsp;";
          s+=split_selector(members[i][1],members[i][12][0][2]);

          var str0="&nbsp;<img src='i24_";
          var str1=" width=24 height=24 style='vertical-align:middle;cursor:pointer' onclick='javascript:{if(checkcontrol())queue_otf_request(";
          var str2=","+i+");}' onmouseover='this.style.backgroundColor=\""+MIXER_PANEL_BGCOLOR2+"\"' onmouseout='this.style.backgroundColor=\""+MIXER_PANEL_BGCOLOR+"\"' />&nbsp;&nbsp;";

          s += str0 + "mix.gif' alt='Arrange members'" + str1 + OTFC_MIXER_ARRANGE_VMP + str2
            +  str0 + "revert.gif' alt='Revert'"       + str1 + OTFC_MIXER_REVERT + str2
            +  str0 + "left.gif' alt='Rotate left'"    + str1 + OTFC_MIXER_SCROLL_LEFT + str2
            +  str0 + "shuff.gif' alt='Shuffle'"       + str1 + OTFC_MIXER_SHUFFLE_VMP + str2
            +  str0 + "right.gif' alt='Rotate right'"  + str1 + OTFC_MIXER_SCROLL_RIGHT + str2
            +  str0 + "clr.gif' alt='Clear positions'" + str1 + OTFC_MIXER_CLEAR + str2;

          s+="</nobr>";
        s+="</div>";
      s+="</div>";
    pos_y+=mh+MIXER_PANEL_HEIGHT;
  }
  return s;
}

function mixrfr()
{
  var pos_y=0;
  var old_visible_ids=visible_ids;
  visible_ids=',';
  hl_links=[];
  mixerCount=mixers;
  if(mixerCount==0) mixerCount=members.length;
  for(i=0;i<mixerCount;i++)
  {
    var mixerData;
    if(classicMode) mixerData=members[i][12];
    else mixerData=conf[i+1];

    try
    {
      var mw=mixerData[0][0]; var mh=mixerData[0][1]; var pos_x=(mmw-mw)>>1;
      var mixer_number=mixerData[0][3];
    }
    catch(e) { return alive(); }

    var obj = null;
    try { obj=document.getElementById('mixercontrol'+mixer_number); } catch(e) {obj=null;}
    if(obj==null) continue;

    var s='';
// walking through the mixer's positions
    for(var j=0;j<mixerData[1].length;j++)
    { var id=false; try { if(typeof mixerData[2][0][j]!='undefined') id=mixerData[2][0][j]; } catch(e) {id=false;}
      if(id !== false) if((id!=0)&&(id!=-1)&&(id!=-2)) visible_ids+=''+id+',';
      var type=false; if(id!==false) try {if(typeof mixerData[2][1][j]!='undefined') type=mixerData[2][1][j];} catch(e) {type=false;}
      var x=0, y=0, w=0, h=0;
      try
      { x=Math.round(mixerData[1][j][0]/bfw*mw); y=Math.round(mixerData[1][j][1]/bfh*mh);
        w=Math.round(mixerData[1][j][2]/bfw*mw); h=Math.round(mixerData[1][j][3]/bfh*mh);
      }
      catch(e)
      {
        continue;
      }
      var border='1px solid '+MIX_BORDER_COLOR; if(id==-1)border='1px solid red'; else if(id==-2) border='1px dotted #f00';
      if(id!==false)if(id!=-1)if(id!=-2){ if(index_exists(hl_links,id)) hl_links[id].push("pr"+mixer_number+"_"+j); else { hl_links[id]=[]; hl_links[id][0]="pr"+mixer_number+"_"+j; }}
      s+="<div id='pp"+mixer_number+"_"+j+"' style='position:relative;top:"+(pos_y+y)+"px;left:"+(pos_x+x)+"px;width:0px;height:0px'>"; // pointing block for member's mockup
        s+="<div id='pr"+mixer_number+"_"+j+"'"+ // rectangle for member's mockup
          " onmousedown='ddstart(event,this,"+mixer_number+","+j+")' onmouseover='ddover(event,this,"+mixer_number+","+j+")' onmouseout='ddout(event,this,"+mixer_number+","+j+")'"+
          " style='overflow:hidden;opacity:0.5;filter:alpha(opacity=50);position:absolute;background-color:#F2F2F2;text-align:center;"+
           "border:"+border+";padding:0;cursor:move;width:"+(w-2)+"px;height:"+(h-2)+"px'>";
        s+=get_mixer_position_html(i,j);
        s+="</div>";
      s+="</div>";
    }
    obj.innerHTML=s;
    pos_y+=mh+MIXER_PANEL_HEIGHT;
  }
  if(old_visible_ids != visible_ids) members_refresh();
  alive();
}

function get_mixer_position_html(mixer, position)
{ var s='';
  var c;
  var mixer_number;
  if(!classicMode)
  {
    if(typeof conf=='undefined') return s;
    var mi=mixer+1;
    if(typeof conf[mi]=='undefined') return s;
    c=conf[mi];
    mixer_number = c[0][3];
  }
  else
  {
    if(typeof members=='undefined') return s;
    if(!members[mixer][0]) return s;
    if(typeof members[mixer][12]=='undefined') return s;
    c=members[mixer][12];
    mixer=members[mixer][1];
    mixer_number = mixer;
  }
  var mw=c[0][0]; var mh=c[0][1]; var p=position;

  var id=false; if(typeof c[2][0][p]!='undefined') id=c[2][0][p];

  var type=false; if(id!==false) if(typeof c[2][1][p]!='undefined') type=c[2][1][p];

  var x=Math.round(c[1][p][0]/bfw*mw); var y=Math.round(c[1][p][1]/bfh*mh); //where?
  var w=Math.round(c[1][p][2]/bfw*mw); var h=Math.round(c[1][p][3]/bfh*mh);

  var label_width= w; var label_height=20; if(label_height>h) label_height=10; if(label_height>h) label_height=8;
  var clear_width=20; if(id===false) clear_width=0;
  var  type_width=20;
  var  user_width=w-type_width-clear_width; if(user_width<10)user_width=5;

  if(user_width+clear_width+type_width>w)
  { var halfwidth=w>>1;
    if(halfwidth>clear_width) { user_width=w-clear_width-type_width+2; clear_width--; type_width--; }
    else
    { user_width=0;
      if(halfwidth>2){ clear_width=halfwidth; type_width=w-halfwidth;} else { clear_width=0; type_width=w; }
    }
  }

  if(type===false)
  { s+="<div style='position:relative;top:0px;left:0px;width:0px;height:0px'>";
      s+="<div id='plus"+mixer_number+"_"+p+"'"
        +" onmouseover='prvnt=1' onmouseout='prvnt=0' onclick='vmp_vad_click("+mixer_number+","+p+")'"
        +" style='overflow:hidden;cursor:pointer;width:20px;height:"+(label_height-2)+"px'>";
        s+="<img src='i20_plus.gif' title='Add video position' width=20 height=20>";
    s+="</div></div>";
    return s;
  }


  if (type_width>0)
  { s+="<div style='position:relative;top:0px;left:0px;width:0px;height:0px'>";
      s+="<div id='tp"+mixer_number+"_"+p+"'"
        +" onmouseover='prvnt=1' onmouseout='prvnt=0' onclick='vmp_vad_click("+mixer_number+","+p+","+type+")'"
        +" style='cursor:pointer;width:"+(type_width)+"px;height:"+(label_height-2)+"px'>";
        s+="<img src='i20_";
          if(type==2) s+='vad'; else if(type==3) s+='vad2'; else s+='static';
          s+=".gif' width="+type_width+" height="+label_height+">";
    s+="</div></div>";
  }

  var memberName = member_read_by_id(id,2);
  if(user_width>0) if(type==1) if(memberName !== false)
  { s+="<div style='position:relative;top:0px;left:"+(type_width)+"px;width:0px;height:0px'>";
     s+="<div id='us"+mixer_number+"_"+p+"'"
       +" onmouseout='javascript:{prvnt=0;}' onmouseover='javascript:{prvnt=1;}' onclick='javascript:{member_selector(this,"+mixer_number+","+p+","+id+")}'"
       +" style='x-overflow:hidden;white-space:nowrap;cursor:pointer;width:"+(user_width)+"px;height:"+(label_height-2)+"px'>";
    s+=memberName;
    s+="</div></div>";
  }

  if(clear_width>0)
  { s+="<div style='position:relative;top:0px;left:"+(type_width+user_width)+"px;width:0px;height:0px'>";
     s+="<div id='tp"+mixer_number+"_"+p+"'"
       +" onmouseover='prvnt=1' onmouseout='prvnt=0' onclick='if(checkcontrol()){queue_otf_request("+OTFC_REMOVE_VMP+","+mixer_number+","+p+");prvnt=0;}'"
       +" style='cursor:pointer;width:"+(clear_width)+"px;height:"+(label_height-2)+"px'>";
      s+="<img src='i20_close.gif' width="+clear_width+" height="+label_height+">";
    s+="</div></div>";
  }

  return s;
}

function r_moder(){
  conf[0][4]='+'; 
  for(var i=0;i<conf[0][9].length;i++) if(conf[0][9][i][0]==roomName) conf[0][9][i][2]='+';
  top_panel();
  alive();
}

function r_unmoder(){
  conf[0][4]='-';
  for(var i=0;i<conf[0][9].length;i++) if(conf[0][9][i][0]==roomName) conf[0][9][i][2]='-';
  top_panel();
  alive();
}

function mixer_over(mixer)
{ if(dd_in_progress) return false;
  if(typeof mixerover !='undefined') if(mixerover) mixer_out();
  mixerover=1+mixer;
  mixerbackup=document.getElementById('mixercontrol'+mixer).innerHTML;
  document.getElementById('mixercontrol'+mixer).innerHTML='';
}

function mixer_out(mixer)
{
  if(typeof mixerover=='undefined') return false;
  if(mixerover==0) return false;
  mixer=mixerover-1;
  if(typeof mixerbackup!='undefined') document.getElementById('mixercontrol'+mixer).innerHTML=mixerbackup;
  mixerover=0;
}

function vmp_vad_click(mixer,position,type){
  if(!checkcontrol()) return false;
  if(typeof type=='undefined') type=2; else type++;
  if(type>3) type=1;
  queue_otf_request(OTFC_VAD_CLICK,mixer,position,type);
}

function shuffle(mixer){
  alert('Shuffle, mixer '+mixer);
}

function mixer_arrange(mixer){
  alert('Arrange, mixer '+mixer);
}

function member_selector_mouse_out()
{
  document.body.removeChild(memberSelector);
  staticMemberSelecting=false;
}

function member_selector_select(mixer, position, id)
{
  member_selector_mouse_out();
  queue_otf_request(OTFC_SET_VMP_STATIC,id,mixer,position);
}

function member_selector(obj,mixer,position,id)
{
  dmsg('Creating member selector for mixer '+mixer+', position '+position);
  if(!checkcontrol()) return false;
  if(staticMemberSelecting) document.body.removeChild(memberSelector);

  memberSelector=document.createElement('div');
  var selWidth=250;
  var selHeight=300;
  memberSelector.style.width=selWidth+'px';
  memberSelector.style.height=selHeight+'px';
  memberSelector.style.position='absolute';
  memberSelector.style.left=getLeftPos(obj)+'px';
  memberSelector.style.top=getTopPos(obj)+'px';
  memberSelector.style.overflowY='scroll';
  memberSelector.style.overflowX='hidden';
  memberSelector.style.border='1px solid #000';
  memberSelector.style.borderRadius='5px';
  memberSelector.style.backgroundColor='#fff';
  memberSelector.onmouseleave=member_selector_mouse_out;

  var selector="";
  for(var i=0;i<members.length;i++)
  {
    if(members[i][0])
    {
      var current=(id==members[i][1]);
      selector+="<p ";
      if(!current) selector+="onclick='javascript:{member_selector_select(" + mixer + "," + position + "," + members[i][1] + ");}' ";
      else selector+="onclick='javascript:member_selector_mouse_out();' ";
      selector+=" class='btn btn-large";
      if(current) selector+=" btn-success";
      selector+="' style='margin:1px;width:"+(selWidth-8)+"px;text-align:left'><nobr>"+members[i][2]+"</nobr></p>";
    }
  }

  memberSelector.innerHTML=selector;

  document.body.appendChild(memberSelector);

  staticMemberSelecting = true;
}

function my_alerting()
{
  myAlertingCounter++;
  if(myAlertingCounter>=8)
  {
    myAlertObject.innerHTML=myAlertBackup;
    myAlertObject.style.color=myAlertBackupColor;
    myAlertObject.style.backgroundColor=myAlertBackupBGColor;
    myAlertBackup='ale';
    return;
  }
  if(myAlertingCounter&1)
  {
    myAlertObject.style.color='#922';
    myAlertObject.style.backgroundColor='#fff';
  }
  else
  {
    myAlertObject.style.color='#fff';
    myAlertObject.style.backgroundColor='#922';
  }
  myAlertTimeout=setTimeout(my_alerting,444);
}

function my_alert(s)
{
  try { clearTimeout(myAlertTimeout); } catch (e) {};
  myAlertString=s;
  myAlertObject=document.getElementById('tip');
  if(typeof myAlertBackup=='undefined') myAlertBackup='ale';
  if(myAlertBackup=='ale')
  {
    myAlertBackup=myAlertObject.innerHTML;
    myAlertBackupColor=myAlertObject.style.color;
    myAlertBackupBGColor=myAlertObject.style.backgroundColor;
  }
  myAlertObject.innerHTML=s;
  myAlertObject.style.color='#fff';
  myAlertObject.style.backgroundColor='#922';
  myAlertingCounter=0;
  myAlertTimeout=setTimeout(my_alerting,444);
}

function getTopPos(el) {
  for (var topPos=0; el!=null; topPos+=el.offsetTop, el=el.offsetParent);
  return topPos;
}

function getLeftPos(el) {
  for (var leftPos=0; el!=null; leftPos+=el.offsetLeft, el=el.offsetParent);
  return leftPos;
}

function resize_timing(s)
{
  document.getElementById('ScaleTiming').innerHTML=Math.floor(s/1000)+'K CPU cycles avg.';
  alive();
}

function get_resizer(i)
{
  var a=[
    'openmcu-ru built-in',
    "libyuv|kFilterNone",
    "libyuv|kFilterBilinear",
    "libyuv|kFilterBox",
    "swscale|SWS_FAST_BILINEAR",
    "swscale|SWS_BILINEAR",
    "swscale|SWS_BICUBIC",
    "swscale|SWS_X",
    "swscale|SWS_POINT",
    "swscale|SWS_AREA",
    "swscale|SWS_BICUBLIN",
    "swscale|SWS_GAUSS",
    "swscale|SWS_SINC",
    "swscale|SWS_LANCZOS",
    "swscale|SWS_SPLINE"
  ];
  
  var r = "&nbsp;Filter: <select class='btn btn-small' style='height:28px;' name='FilterSelector' onchange='{queue_otf_request("
    + OTFC_YUV_FILTER_MODE + ",this.value);return false;}'>";
  
  for(j=0;j<a.length;j++)
  {
    r+="<option value=\""+j+"\"";
    if(i==j) r+=" selected";
    r+=">"+a[j]+"</option>";
  }
  r+="</select>";

  r+="&nbsp;<span id='ScaleTiming'>-</span>";

  return r;
}

function dspr(id,spr)
{
  var o=document.getElementById('dial_'+idid(id));
  if(o)o.className='adspr'+spr;
  var ad=spr.charAt(1);
  var r='other_sprite';
  if(ad=='1') r=member_modify_by_id(id,14,1);
  if(ad=='0') r=member_modify_by_id(id,14,0);
  dmsg('dial sprite['+id+']='+ad+', result: '+r);
  alive();
}

function rszspr(id,state)
{
  var o=document.getElementById('rsz_'+idid(id));
  if(o)o.className='rszspr'+state;
  member_modify_by_id(id,15,state);
  alive();
}
