
#include <ptlib.h>
#include <ptlib/video.h>

#include "version.h"
#include "mcu.h"
#include "h323.h"
#include "html.h"

#include <h323pdu.h>
#include <math.h>
#include <stdio.h>

#if OPENMCU_VIDEO

#if USE_LIBYUV
#include <libyuv/scale.h>
#endif

#endif  // OPENMCU_VIDEO

static const char * GKModeLabels[] = { 
   "No gatekeeper", 
   "Find gatekeeper", 
   "Use gatekeeper", 
};

enum {
  Gatekeeper_None,
  Gatekeeper_Find,
  Gatekeeper_Explicit
};

#define GKMODE_LABEL_COUNT   (sizeof(GKModeLabels)/sizeof(char *))

#define new PNEW


///////////////////////////////////////////////////////////////

void SpliceMacro(PString & text, const PString & token, const PString & value)
{
  PRegularExpression RegEx("<?!--#status[ \t\r\n]+" + token + "[ \t\r\n]*-->?",
                           PRegularExpression::Extended|PRegularExpression::IgnoreCase);
  PINDEX pos, len;
  while (text.FindRegEx(RegEx, pos, len))
    text.Splice(value, pos, len);
}

///////////////////////////////////////////////////////////////

#ifdef _WIN32
PluginLoaderStartup2  OpenMCU::pluginLoader;
H323PluginCodecManager * OpenMCU::plugmgr=NULL;
#endif

OpenMCUH323EndPoint::OpenMCUH323EndPoint(ConferenceManager & _conferenceManager)
  : conferenceManager(_conferenceManager)
{
#if OPENMCU_VIDEO
	terminalType = e_MCUWithAVMP;
  enableVideo  = TRUE;
  videoRate    = 10;
#else
	terminalType = e_MCUWithAudioMP;
#endif

   gkAlias = PString();

#ifdef _WIN32
  // MFC applications are not at all plugin friendly
  // You need to manually add the plugins
  OpenMCU::Current().LoadPluginMgr();
  OpenMCU::Current().pluginLoader.OnStartup();
#endif
}

OpenMCUH323EndPoint::~OpenMCUH323EndPoint()
{
#ifdef _WIN32
  // You need to manually remove the plugins
  OpenMCU::Current().RemovePluginMgr();
  OpenMCU::Current().pluginLoader.OnShutdown();
#endif
}

void OpenMCUH323EndPoint::Initialise(PConfig & cfg)
{

#ifdef HAS_AEC
  SetAECEnabled(FALSE);
#endif

///////////////////////////////////////////
// Listeners
  PString defaultInterface = "*:1720";
  H323TransportAddressArray interfaces;
  PStringArray interfacesArray = cfg.GetString(InterfaceKey).Tokenise(",");
  BOOL interfacesEmpty = TRUE;
  for (int i = 0; i < interfacesArray.GetSize(); i++)
  {
    if(interfacesArray[i] != "")
    {
      interfacesEmpty = FALSE;
      interfaces.Append(new H323TransportAddress(interfacesArray[i]));
    }
  }
  if (interfacesArray.GetSize() == 0 || interfacesEmpty)
  {
    interfaces.Append(new H323TransportAddress(defaultInterface));
    cfg.SetString(InterfaceKey, defaultInterface);
  }
  StartListeners(interfaces);

  if (listeners.IsEmpty()) {
    PSYSTEMLOG(Fatal, "Main\tCould not open H.323 Listener");
  }

  AliasList.RemoveAll();
  localAliasNames.RemoveAll();

///////////////////////////////////////////
// NAT Router IP
  PString nat_ip = cfg.GetString(NATRouterIPKey);
  if (nat_ip.Trim().IsEmpty()) {
    behind_masq = FALSE;
  } else {
    masqAddressPtr = new PIPSocket::Address(nat_ip);
    behind_masq = TRUE;
    cout << "Masquerading as address " << *(masqAddressPtr) << endl;
  }

  nat_lag_ip = cfg.GetString(NATTreatAsGlobalKey);
  if(!nat_lag_ip.IsEmpty())
  {
    nat_lag_ip=","+nat_lag_ip+",";
    nat_lag_ip.Replace(" ","", TRUE, 0);
  }

///////////////////////////////////////////
// RTP Port Setup
  unsigned rtpPortBase = cfg.GetInteger(RTPPortBaseKey, 0);
  unsigned rtpPortMax = cfg.GetInteger(RTPPortMaxKey, 0);
  if((rtpPortBase>65532)||(rtpPortBase==0)) rtpPortBase=5000;
  if(rtpPortMax<=rtpPortBase) rtpPortMax=PMIN(rtpPortBase+5000,65532);
  SetRtpIpPorts(rtpPortBase, rtpPortMax);

///////////////////////////////////////////
// Enable/Disable Fast Start & H.245 Tunneling
  BOOL disableFastStart = cfg.GetBoolean(DisableFastStartKey, TRUE);
  BOOL disableH245Tunneling = cfg.GetBoolean(DisableH245TunnelingKey, FALSE);
  DisableFastStart(disableFastStart);
  DisableH245Tunneling(disableH245Tunneling);

//////////////////////////////////////////////////////
// Gatekeeper mode
  PStringArray labels(GKMODE_LABEL_COUNT, GKModeLabels); 
  PINDEX idx = labels.GetStringsIndex(cfg.GetString(GatekeeperModeKey, labels[0]));  
  PINDEX gkMode = (idx == P_MAX_INDEX) ? 0 : idx;

  // Gatekeeper 
  PString gkName = cfg.GetString(GatekeeperKey);

  // OpenMCU Server Id
  PString serverId = MCUConfig("Parameters").GetString("OpenMCU Server Id",OpenMCU::Current().GetName() + " v" + OpenMCU::Current().GetVersion());
  if (gkMode == Gatekeeper_None ) {
    // Local alias name for H.323 endpoint
    //SetLocalUserName(cfg.GetString(LocalUserNameKey, OpenMCU::Current().GetName() + " v" + OpenMCU::Current().GetVersion()));
    SetLocalUserName(serverId);
  } else {
    SetLocalUserName(serverId);
  }

  // Gatekeeper UserName
  PString gkUserName = cfg.GetString(GatekeeperUserNameKey,"MCU");
  localAliasNames.AppendString(gkUserName);

  // Gatekeeper password
  PString gkPassword = PHTTPPasswordField::Decrypt(cfg.GetString(GatekeeperPasswordKey));
  SetGatekeeperPassword(gkPassword);

  // Gatekeeper Alias
  gkAlias = cfg.GetString(GatekeeperAliasKey,"MCU*");

  for (PINDEX k=0; k< OpenMCU::defaultRoomCount; k++) {
	  PString alias = gkAlias;
	  alias.Replace("*",k);
	  AddAliasName(alias);   // Add the alias to the endpoint aliaslist
      AliasList.AppendString(alias);  
  }

  PStringArray gkArray = cfg.GetString(GatekeeperPrefixesKey).Tokenise(",");
  for (int i = 0; i < gkArray.GetSize(); i++)
  {
    if(gkArray[i] != "")
    {
      PString prefix = gkArray[i];
      PrefixList.AppendString(prefix);
      for (PINDEX k=0; k < OpenMCU::defaultRoomCount; k++)
      {
	PString alias = prefix + PString(k);
	AliasList.AppendString(alias);
      }
    }
  }

   // Setup capabilities
   if(capabilities.GetSize() == 0)
   {
     //AddAllCapabilities(0, 0, "*");
     PTRACE(3, "H323\tAdd all capabilities");
     H323CapabilityFactory::KeyList_T stdCaps = H323CapabilityFactory::GetKeyList();
     for (H323CapabilityFactory::KeyList_T::const_iterator r = stdCaps.begin(); r != stdCaps.end(); ++r)
     {
        PString capName(*r);
        OpalMediaFormat mediaFormat(capName);
        if(!mediaFormat.IsValid() && (capName.Right(4) == "{sw}") && capName.GetLength() > 4)
          mediaFormat = OpalMediaFormat(capName.Left(capName.GetLength()-4));
        if(mediaFormat.IsValid())
        {
          H323Capability * cap = H323Capability::Create(capName);
          if(cap) AddCapability(cap);
        }
     }
   }

   unsigned rsConfig=1, tsConfig=1, rvConfig=1, tvConfig=1;
   if(MCUConfig("RECEIVE_SOUND").GetKeys().GetSize() == 0) rsConfig = 0;
   if(MCUConfig("TRANSMIT_SOUND").GetKeys().GetSize() == 0) tsConfig = 0;
   if(MCUConfig("RECEIVE_VIDEO").GetKeys().GetSize() == 0) rvConfig = 0;
   if(MCUConfig("TRANSMIT_VIDEO").GetKeys().GetSize() == 0) tvConfig = 0;

   for(PINDEX i = 0; i < capabilities.GetSize(); i++)
   {
     H323Capability *cap = &capabilities[i];
     if(rsConfig == 0 && cap->GetMainType() == 0)
     {
       if(cap->GetFormatName().Right(4) == "{sw}")
         MCUConfig("RECEIVE_SOUND").SetBoolean(cap->GetFormatName(), 1);
       else
         MCUConfig("RECEIVE_SOUND").SetBoolean(cap->GetFormatName()+"{sw}", 1);
     }
     if(tsConfig == 0 && cap->GetMainType() == 0)
       MCUConfig("TRANSMIT_SOUND").SetBoolean(cap->GetFormatName(), 1);
     if(rvConfig == 0 && cap->GetMainType() == 1)
       MCUConfig("RECEIVE_VIDEO").SetBoolean(cap->GetFormatName(), 1);
     if(tvConfig == 0 && cap->GetMainType() == 1)
       MCUConfig("TRANSMIT_VIDEO").SetBoolean(cap->GetFormatName(), 1);

     if(rsConfig == 1 && cap->GetMainType() == 0)
     {
       if(cap->GetFormatName().Right(4) == "{sw}" && MCUConfig("RECEIVE_SOUND").HasKey(cap->GetFormatName()) == 0)
         MCUConfig("RECEIVE_SOUND").SetBoolean(cap->GetFormatName(), 1);
       if(cap->GetFormatName().Right(4) != "{sw}" && MCUConfig("RECEIVE_SOUND").HasKey(cap->GetFormatName()+"{sw}") == 0)
         MCUConfig("RECEIVE_SOUND").SetBoolean(cap->GetFormatName()+"{sw}", 1);
     }
     if(tsConfig == 1 && cap->GetMainType() == 0 && MCUConfig("TRANSMIT_SOUND").HasKey(cap->GetFormatName()) == 0)
       MCUConfig("TRANSMIT_SOUND").SetBoolean(cap->GetFormatName(), 1);
     if(rvConfig == 1 && cap->GetMainType() == 1 && MCUConfig("RECEIVE_VIDEO").HasKey(cap->GetFormatName()) == 0)
       MCUConfig("RECEIVE_VIDEO").SetBoolean(cap->GetFormatName(), 1);
     if(tvConfig == 1 && cap->GetMainType() == 1 && MCUConfig("TRANSMIT_VIDEO").HasKey(cap->GetFormatName()) == 0)
       MCUConfig("TRANSMIT_VIDEO").SetBoolean(cap->GetFormatName(), 1);
   }

   capabilities.RemoveAll();

   int capsNum = 5;
   capsNum += MCUConfig("RECEIVE_SOUND").GetKeys().GetSize()+
             MCUConfig("TRANSMIT_SOUND").GetKeys().GetSize()+
             MCUConfig("RECEIVE_VIDEO").GetKeys().GetSize()+
             MCUConfig("TRANSMIT_VIDEO").GetKeys().GetSize();
   rsCaps = (char **)calloc(capsNum,sizeof(char *));
   tsCaps = (char **)calloc(capsNum,sizeof(char *));
   rvCaps = (char **)calloc(capsNum,sizeof(char *));
   tvCaps = (char **)calloc(capsNum,sizeof(char *));
   listCaps = (char *)calloc(capsNum,64*sizeof(char));

   char buf[64];
   capsNum = 0;
   PStringList keys;
   keys = MCUConfig("RECEIVE_SOUND").GetKeys();
   for(PINDEX i = 0, j = 0; i < keys.GetSize(); i++)
   {
     if(MCUConfig("RECEIVE_SOUND").GetBoolean(keys[i]) != 1) continue;
     strcpy(buf, keys[i]);
     strcpy(&(listCaps[64*capsNum]),buf);
     rsCaps[j]=&(listCaps[64*capsNum]);
     j++; capsNum++;
   }

   keys = MCUConfig("TRANSMIT_SOUND").GetKeys();
   for(PINDEX i = 0, j = 0; i < keys.GetSize(); i++)
   {
     if(MCUConfig("TRANSMIT_SOUND").GetBoolean(keys[i]) != 1) continue;
     strcpy(buf, keys[i]);
     strcpy(&(listCaps[64*capsNum]),buf);
     tsCaps[j]=&(listCaps[64*capsNum]);
     j++; capsNum++;
   }

   keys = MCUConfig("RECEIVE_VIDEO").GetKeys();
   for(PINDEX i = 0, j = 0; i < keys.GetSize(); i++)
   {
     if(MCUConfig("RECEIVE_VIDEO").GetBoolean(keys[i]) != 1) continue;
     strcpy(buf, keys[i]);
     strcpy(&(listCaps[64*capsNum]),buf);
     rvCaps[j]=&(listCaps[64*capsNum]);
     j++; capsNum++;
   }

   keys = MCUConfig("TRANSMIT_VIDEO").GetKeys();
   for(PINDEX i = 0, j = 0; i < keys.GetSize(); i++)
   {
     if(MCUConfig("TRANSMIT_VIDEO").GetBoolean(keys[i]) != 1) continue;
     strcpy(buf, keys[i]);
     strcpy(&(listCaps[64*capsNum]),buf);
     tvCaps[j]=&(listCaps[64*capsNum]);
     j++; capsNum++;
   }

   int listNum = 0;
   cout << "[RECEIVE_SOUND]= "; listNum=0; 
   while(rsCaps[listNum]!=NULL) { cout << rsCaps[listNum] << ", "; listNum++; }
   cout << "\n";
   cout << "[TRANSMIT_SOUND]= "; listNum=0; 
   while(tsCaps[listNum]!=NULL) { cout << tsCaps[listNum] << ", "; listNum++; }
   cout << "\n";
   cout << "[RECEIVE_VIDEO]= "; listNum=0; 
   while(rvCaps[listNum]!=NULL) { cout << rvCaps[listNum] << ", "; listNum++; }
   cout << "\n";
   cout << "[TRANSMIT_VIDEO]= "; listNum=0; 
   while(tvCaps[listNum]!=NULL) { cout << tvCaps[listNum] << ", "; listNum++; }
   cout << "\n";
   AddCapabilities(0,0,(const char **)rsCaps);
   AddCapabilities(0,1,(const char **)rvCaps);
   cout << capabilities;

#if 0 //  old MCU options
  int videoTxQual = 10;
  if (args.HasOption("videotxquality")) 
      videoTxQual = args.GetOptionString("videotxquality").AsInteger();
  endpoint.videoTxQuality = PMAX(1, PMIN(31, videoTxQual));

  int videoF = 2;
  if (args.HasOption("videofill")) 
    videoF = args.GetOptionString("videofill").AsInteger();
  endpoint.videoFill = PMAX(1, PMIN(99, videoF));

  int videoFPS = 10;
  if (args.HasOption("videotxfps")) 
    videoFPS = args.GetOptionString("videotxfps").AsInteger();
  endpoint.videoFramesPS = PMAX(1,PMIN(100,videoFPS));

  int videoBitRate = 0; //disable setting videoBitRate.
  if (args.HasOption("videobitrate")) {
    videoBitRate = args.GetOptionString("videobitrate").AsInteger();
    videoBitRate = 1024 * PMAX(16, PMIN(2048, videoBitRate));
  }
  endpoint.videoBitRate = videoBitRate;
#endif


  switch (gkMode) {
    default:
    case Gatekeeper_None:
      break;

    case Gatekeeper_Find:
      if (!DiscoverGatekeeper(new H323TransportUDP(*this)))
        PSYSTEMLOG(Error, "No gatekeeper found");
	  else
		PSYSTEMLOG(Info, "Found Gatekeeper: " << gatekeeper);
      break;

    case Gatekeeper_Explicit:
      if (!SetGatekeeper(gkName, new H323TransportUDP(*this)))
        PSYSTEMLOG(Error, "Error registering with gatekeeper at \"" << gkName << '"');
	  else
		PSYSTEMLOG(Info, "Registered with Gatekeeper: " << gkName);
  }

  PTRACE(2, "MCU\tCodecs (in preference order):\n" << setprecision(2) << GetCapabilities());;
}

H323Connection * OpenMCUH323EndPoint::CreateConnection(
      unsigned callReference,
      void * userData,
      H323Transport *,
      H323SignalPDU *)

{
  return new OpenMCUH323Connection(*this, callReference, userData);
}

void OpenMCUH323EndPoint::TranslateTCPAddress(PIPSocket::Address &localAddr, const PIPSocket::Address &remoteAddr)
{
  if (this->behind_masq)
  {
    BYTE byte1=localAddr.Byte1();
    BYTE byte2=localAddr.Byte2();
    const BOOL local_mcu=(
       (byte1==10)                       // LAN class A
     ||((byte1==172)&&((byte2&240)==16)) // LAN class B
     ||((byte1==192)&&(byte2==168))      // LAN class C
     ||((byte1==169)&&(byte2==254))      // APIPA/IPAC/zeroconf (probably LAN)
    );
    if(!local_mcu){
      PTRACE(3,"H323\tNAT remIP=" << remoteAddr << ", locIP=" << localAddr << ": n/a (src is global)");
      return;
    }

    if(!nat_lag_ip.IsEmpty())
    if(nat_lag_ip.Find(","+remoteAddr.AsString()+",")!=P_MAX_INDEX)
    { PTRACE(3,"H323\tNAT remIP=" << remoteAddr << ", locIP=" << localAddr << ": ***change to " << *(this->masqAddressPtr) << " (treating as global)");
      localAddr=*(this->masqAddressPtr);
      return;
    }

    byte1=remoteAddr.Byte1();
    byte2=remoteAddr.Byte2();
    if((byte1==10)                       // LAN class A
     ||((byte1==172)&&((byte2&240)==16)) // LAN class B
     ||((byte1==192)&&(byte2==168))      // LAN class C
     ||((byte1==169)&&(byte2==254))      // APIPA/IPAC/zeroconf (probably LAN)
    ) { PTRACE(3,"H323\tNAT remIP=" << remoteAddr << ", locIP=" << localAddr << ": n/a (dest. is LAN)"); }
    else
    { PTRACE(3,"H323\tNAT remIP=" << remoteAddr << ", locIP=" << localAddr << ": ***change to " << *(this->masqAddressPtr));
      localAddr=*(this->masqAddressPtr);
    }
  } else
  { PTRACE(3,"H323\tNAT remIP=" << remoteAddr << ", locIP=" << localAddr << ": n/a (NAT IP not set)"); }
  return;
}

PString JsQuoteScreen(PString s)
{
  PString r="\"";
  for(PINDEX i=0; i<s.GetLength(); i++)
  { BYTE c=(BYTE)s[i];
    if(c>31)
    { if     (c==0x22) r+="\\x22"; // "
      else if(c==0x5c) r+="\\x5c"; // backslash
      else r+=(char)c;
    }
  }
  r+="\"";
  return r;
}

PString OpenMCUH323EndPoint::GetRoomStatusJS()
{
  PString str = "Array(";
  PTime now;
  PWaitAndSignal m(conferenceManager.GetConferenceListMutex());
  ConferenceListType & conferenceList = conferenceManager.GetConferenceList();
  BOOL notFirstConference = FALSE;
  for (ConferenceListType::iterator r=conferenceList.begin(), re=conferenceList.end(); r!=re; ++r)
  { Conference & conference = *(r->second);
    PStringStream c;
    { PWaitAndSignal m(conference.GetMutex());
      Conference::MemberList & memberList = conference.GetMemberList();
      Conference::MemberNameList & memberNameList = conference.GetMemberNameList();
      c << "Array("
        << JsQuoteScreen(conference.GetNumber())                               // c[r][0]: room name
        << "," << memberList.size()                                            // c[r][1]: memberList size
        << "," << memberNameList.size()                                        // c[r][2]: memberNameList size
        << "," << PString((now - conference.GetStartTime()).GetMilliSeconds()) // c[r][3]: duration
        << ",Array("                                                           // c[r][4]: member descriptors
      ;
      BOOL notFirstMember = FALSE;
      for (Conference::MemberList::const_iterator t=memberList.begin(), te=memberList.end(); t!=te; ++t) 
      { ConferenceMember * member = t->second; if(member==NULL) continue;
        PString name=member->GetName();
        ConferenceMemberId id = member->GetID();
        if(notFirstMember) c << ",";
        c << "Array("                                                          // c[r][4][m]: member m descriptor
          << (long)id                                                          // c[r][4][m][0]: member id
          << "," << JsQuoteScreen(name)                                        // c[r][4][m][1]: member name
          << "," << (member->IsVisible() ? "1" : "0")                          // c[r][4][m][2]: is member visible: 1/0
          << "," << (member->IsMCU() ? "1" : "0")                              // c[r][4][m][3]: is MCU: 1/0
        ;

        PTimeInterval duration;
        PString formatString, audioCodecR, audioCodecT, videoCodecR, videoCodecT;
        int codecCacheMode=-1, cacheUsersNumber=-1;
        OpenMCUH323Connection * conn = NULL;
        H323Connection_ConferenceMember * connMember = NULL;
        DWORD orx=0, otx=0, vorx=0, votx=0, prx=0, ptx=0, vprx=0, vptx=0;
        if(name=="file recorder")
        {
          duration = now - member->GetStartTime();
        }
        else if(name=="cache")
        { ConferenceFileMember * fileMember = dynamic_cast<ConferenceFileMember *>(member);
          if(fileMember!=NULL)
          if(fileMember->codec!=NULL)
          if(fileMember->codec->cacheMode==1)
          { formatString=fileMember->codec->formatString;
            cacheUsersNumber=fileMember->codec->GetCacheUsersNumber();
            codecCacheMode=fileMember->codec->cacheMode;
          }
          duration = now - member->GetStartTime();
        }
        else // real (visible, external) endpoint
        { connMember = dynamic_cast<H323Connection_ConferenceMember *>(member);
          if (connMember != NULL)
          { conn = (OpenMCUH323Connection *)FindConnectionWithLock(connMember->GetH323Token());
            if(conn!=NULL)
            { duration = now - conn->GetConnectionStartTime();
              audioCodecR = conn->GetAudioReceiveCodecName();
              audioCodecT = conn->GetAudioTransmitCodecName();
              RTP_Session *sess=conn->GetSession(RTP_Session::DefaultAudioSessionID);
              if(sess != NULL)
              { orx = sess->GetOctetsReceived(); otx = sess->GetOctetsSent();
                prx = sess->GetPacketsReceived(); ptx = sess->GetPacketsSent();
              }
#             if OPENMCU_VIDEO
                videoCodecR = conn->GetVideoReceiveCodecName() + "@" + connMember->GetVideoRxFrameSize();
                videoCodecT = conn->GetVideoTransmitCodecName();
                RTP_Session* vSess=conn->GetSession(RTP_Session::DefaultVideoSessionID);
                if(vSess != NULL)
                { vorx=vSess->GetOctetsReceived(); votx=vSess->GetOctetsSent();
                  vprx=vSess->GetPacketsReceived(); vptx=vSess->GetPacketsSent();

                }
                if(conn->GetVideoTransmitCodec()!=NULL)
                { codecCacheMode=conn->GetVideoTransmitCodec()->cacheMode;
                  formatString=conn->GetVideoTransmitCodec()->formatString;
                }
#             endif
              conn->Unlock();
            }
          }
        }

        c << "," << duration.GetMilliSeconds()                                 // c[r][4][m][4]: member duration
          << "," << orx << "," << otx << "," << vorx << "," << votx            // c[r][4][m][5-8]: orx, otx, vorx, votx
          << "," << JsQuoteScreen(audioCodecR)                                 // c[r][4][m][9]: audio receive codec name
          << "," << JsQuoteScreen(audioCodecT)                                 // c[r][4][m][10]: audio transmit codec name
          << "," << JsQuoteScreen(videoCodecR)                                 // c[r][4][m][11]: video receive codec name
          << "," << JsQuoteScreen(videoCodecT)                                 // c[r][4][m][12]: video transmit codec name
          << "," << codecCacheMode << "," << JsQuoteScreen(formatString)       // c[r][4][m][13,14]: codecCacheMode, formatString
          << "," << member->GetVideoRxFrameRate()                              // c[r][4][m][15]: video rx frame rate
          << "," << member->GetVideoTxFrameRate()                              // c[r][4][m][16]: video tx frame rate
          << "," << cacheUsersNumber                                           // c[r][4][m][17]: cache users number
          << "," << prx << "," << ptx << "," << vprx << "," << vptx            // c[r][4][m][18-21]: prx, ptx, vprx, vptx
          << ")";
        notFirstMember = TRUE;
      }
          
      for(Conference::MemberNameList::const_iterator s=memberNameList.begin(), se=memberNameList.end(); s!=se; ++s)
      { if(s->second != NULL) continue;
        c << (notFirstMember ? "," : "") << "Array("                           // c[r][4][m]: member m descriptor
          << "0"                                                               // c[r][4][m][0]: member id: 0 (offline)
          << "," << JsQuoteScreen(s->first)                                    // c[r][4][m][1]: member name
          << ")";
        notFirstMember = TRUE;
      }
      c << "))";
    }

    if(notFirstConference) str += ",";
    notFirstConference = TRUE;
    str += c;
  }

  str += ")";
  return str;
}


PString OpenMCUH323EndPoint::GetRoomStatusJSStart()
{
  PStringStream html;
  BeginPage(html,"Connections","window.l_connections","window.l_info_connections");
  html
    << "<script>var loadCounter=0;function page_reload(){if(loadCounter<=0) location.href=location.href;document.getElementById('status2').innerHTML=loadCounter;loadCounter--;setTimeout(page_reload, 990);}function status_init(){if(window.status_update_start) setTimeout(status_update_start,500);else{document.getElementById(\"status1\").innerHTML=\"<h1>ERROR: Can not load <font color=red>status.js</font></h1><h2>Page will reload after <span id='status2'>30</span> s</h2>\";loadCounter=30;setTimeout(page_reload, 990);}}</script>"
    << "<div id=\"status1\"></div><script src='status.js'></script>";
  EndPage(html,OpenMCU::Current().GetHtmlCopyright());
  return html;
}




PString OpenMCUH323EndPoint::GetRoomStatus(const PString & block)
{ 
  PString substitution;
  PWaitAndSignal m(conferenceManager.GetConferenceListMutex());
  ConferenceListType & conferenceList = conferenceManager.GetConferenceList();

  ConferenceListType::iterator r;
  for (r = conferenceList.begin(); r != conferenceList.end(); ++r) {

    // make a copy of the repeating html chunk
    PString insert = block;
    PStringStream members;
    members << 
	"<table class=\"table table-striped table-bordered table-condensed\">"
               "<tr>"
                 "<th>"
                 "&nbsp;Name&nbsp;"
                 "</th><th>"
                 "&nbsp;Duration&nbsp;"
                 "</th><th>"
                 "&nbsp;RTP Channel: Codec&nbsp;"
                 "</th><th>"
                 "&nbsp;Packets&nbsp;"
                 "</th><th>"
                 "&nbsp;Bytes&nbsp;"
                 "</th><th>"
                 "&nbsp;Kbit/s&nbsp;"
                 "</th>"
#if OPENMCU_VIDEO
                 "<th>"
                 "&nbsp;FPS&nbsp;"
                 "</th>"
#endif
                 "</tr>";

    Conference & conference = *(r->second);
    size_t memberNameListSize = 0;
    PStringArray targets, subses, errors;

    { PWaitAndSignal m(conference.GetMutex());
      Conference::MemberList & memberList = conference.GetMemberList();
      for (Conference::MemberList::const_iterator t = memberList.begin(); t != memberList.end(); ++t) 
      { ConferenceMember * member = t->second;
        if(member==NULL) continue;
        PString memberName=member->GetName();
        PString formatString;
        int codecCacheMode=-1, cacheUsersNumber=0;
        BOOL visible=member->IsVisible();
        BOOL cache=(memberName=="cache");
        members << "<tr><td>";
        if(cache) members << "<nobr><b>[Hidden]</b> cache</nobr></td>";
        else
          members
            << (visible? "" : "<b>[Hidden]</b> ")
            << (member->IsMCU() ? "<b>[MCU]</b> " : "")
            << memberName << "</td>";

        OpenMCUH323Connection * conn = NULL;
        H323Connection_ConferenceMember * connMember = NULL;
        if ((!cache)&&(memberName!="file recorder")) connMember = dynamic_cast<H323Connection_ConferenceMember *>(member);
        if (connMember != NULL) conn = (OpenMCUH323Connection *)FindConnectionWithLock(connMember->GetH323Token());
        PTime now;

        if(conn!=NULL)
        {
#if OPENMCU_VIDEO
          BOOL connCodecNotNull = (conn->GetVideoTransmitCodec()!=NULL);
          if(connCodecNotNull) { codecCacheMode=conn->GetVideoTransmitCodec()->cacheMode; formatString=conn->GetVideoTransmitCodec()->formatString; }
          else formatString="NO_CODEC";
#endif
          PTimeInterval duration = now - conn->GetConnectionStartTime();
          PStringStream d;
          d << duration;
          d=d.Left(d.Find('.'));
          if(d.GetLength()==1) d="00:0"+d;
          else if(d.GetLength()==2) d="00:"+d;
          else if(d.GetLength()==4) d="0"+d;
          members
            << "<td style='text-align:right'>" << d << "</td>"
            << "<td><nobr>"
              << "<b>Audio In: </b>"  << conn->GetAudioReceiveCodecName()
              << "<br /><b>Audio Out: </b>" << conn->GetAudioTransmitCodecName()
#if OPENMCU_VIDEO
              << "<br /><b>Video In: </b>"  << conn->GetVideoReceiveCodecName() << "@" << connMember->GetVideoRxFrameSize()
              << "<br /><b>"
                << ((codecCacheMode==2)? "<font color=green>":"")
                << "Video Out"
                << ((codecCacheMode==2)? "</font>":"")
                << ": </b>" << conn->GetVideoTransmitCodecName()
#endif
              << "</nobr></td><td style='text-align:right'>";

          DWORD orx=0, otx=0; RTP_Session * session = conn->GetSession(RTP_Session::DefaultAudioSessionID);
#if OPENMCU_VIDEO
          DWORD vorx=0, votx=0; RTP_Session * v_session = conn->GetSession(RTP_Session::DefaultVideoSessionID);
#endif
          if(session!=NULL)
          { orx = session->GetOctetsReceived(); otx = session->GetOctetsSent();
            members << session->GetPacketsReceived() << "<br />" << session->GetPacketsSent();
          } else members << "-<br />-";
#if OPENMCU_VIDEO
          if(v_session!=NULL)
          { vorx = v_session->GetOctetsReceived(); votx = v_session->GetOctetsSent();
            members << "<br />" << v_session->GetPacketsReceived() << "<br />" << v_session->GetPacketsSent();
          } else members << "<br />-<br />-";
#endif
          members << "</td><td style='text-align:right'>";
          if(session!=NULL) members << orx << "<br />" << otx; else members << "-<br />-";
#if OPENMCU_VIDEO
          if(v_session!=NULL) members << "<br />" << vorx << "<br />" << votx; else members << "<br />-<br />-";
#endif
          members << "</td><td style='text-align:right'><nobr>";
          if(session!=NULL) members << psprintf("%6.1f",floor(orx * 80.0 / duration.GetMilliSeconds() + 0.5) / 10)
                        << "<br />" << psprintf("%6.1f",floor(otx * 80.0 / duration.GetMilliSeconds() + 0.5) / 10);
          else members << "-<br />-";
#if OPENMCU_VIDEO
          if(v_session!=NULL) members << "<br />" << psprintf("%6.1f",floor(vorx * 80.0 / duration.GetMilliSeconds() + 0.5) / 10)
                                      << "<br />" << psprintf("%6.1f",floor(votx * 80.0 / duration.GetMilliSeconds() + 0.5) / 10);
          else members << "<br />-<br />-";
#endif
          members << "</nobr></td>";

          conn->Unlock();
        }

        if(conn==NULL)
        { formatString="NO_CODEC";
          if(cache)
          { cache=FALSE;
            ConferenceFileMember * fileMember = dynamic_cast<ConferenceFileMember *>(member);
            if(fileMember!=NULL)
            if(fileMember->codec!=NULL)
            if(fileMember->codec->cacheMode==1)
            { cache=TRUE; formatString=fileMember->codec->formatString; cacheUsersNumber=fileMember->codec->GetCacheUsersNumber(); }
          }
          PStringStream d;
          d << (now - member->GetStartTime());
          d=d.Left(d.Find('.'));
          if(d.GetLength()==1) d="00:0"+d;
          else if(d.GetLength()==2) d="00:"+d;
          else if(d.GetLength()==4) d="0"+d;
          members
            << "<td style='text-align:right'>" << d << "</td>"
            << "<td>" << (cache? ("<nobr><b>Video Out:  </b>"+formatString+"</nobr>") : "-") << "</td>"
            << "<td>-</td><td>-</td><td>-</td>";

        }

#if OPENMCU_VIDEO
        members << "<td style='text-align:right'>";
        if(visible) members << "<br /><br />";
        if(cache)
        { PString target="%%[" + formatString +"]";
          PStringStream subs; subs << psprintf("%4.2f",floor(member->GetVideoTxFrameRate()*100+0.55)/100);
          targets.AppendString(target);
          subses.AppendString(subs);
          members << "<nobr><b><font color=green>" << cacheUsersNumber << " x </font></b>" << subs << "</nobr>";
        }
        else if(visible)
        { members << "<nobr>" << psprintf("%4.2f",floor(member->GetVideoRxFrameRate()*100+0.55)/100) << "<br />";
          if(codecCacheMode==2)
          { PString t = "%%[" + formatString + "]";
            members << t;
            if(errors.GetStringsIndex(t)==P_MAX_INDEX) errors.AppendString(t);
          }
          else members << psprintf("%4.2f",floor(member->GetVideoTxFrameRate()*100+0.55)/100);
          members << "</nobr>";
        }
        else members << "-";
        members << "</td>";
#endif
        members 
          << "</tr>";
      }
    }
          
    Conference::MemberNameList & memberNameList = conference.GetMemberNameList();
    memberNameListSize = memberNameList.size();
    Conference::MemberNameList::const_iterator s;
    for (s = memberNameList.begin(); s != memberNameList.end(); ++s) 
    {
      ConferenceMember * member = s->second;
#if OPENMCU_VIDEO
      if(member == NULL) {members << "<tr><td colspan='7'><b>[Offline]</b> <font color='gray'>" << s->first << "</font></td></tr>"; continue; }
#else
      if(member == NULL) {members << "<tr><td colspan='6'><b>[Offline]</b> <font color='gray'>" << s->first << "</font></td></tr>"; continue; }
#endif
    }

    members << "</table>";
    for(PINDEX i=0; i<errors.GetSize(); i++)
    { PString target=errors[i], subs;
      PINDEX j = targets.GetStringsIndex(target);
      if(j!=P_MAX_INDEX) subs="<font color='green'>" + subses[j] + "</font>"; else subs="<font color=red>Error</font>";
      members.Replace(target, subs, TRUE, 0);
    }

    SpliceMacro(insert, "RoomName",        conference.GetNumber());
    SpliceMacro(insert, "RoomMemberCount", PString(PString::Unsigned, memberNameListSize));
    SpliceMacro(insert, "RoomMembers",     members);
    substitution += insert;
  }

  return substitution;
}

PString OpenMCUH323EndPoint::GetMemberList(Conference & conference, ConferenceMemberId id)
{
 PStringStream members;
 PWaitAndSignal m(conference.GetMutex());
 Conference::MemberNameList & memberNameList = conference.GetMemberNameList();
 Conference::MemberNameList::const_iterator s;
 members << "<option value=\"0\"></option>";
 members << "<option" << ((id==(void *)(-1))?" selected ":" ") << "value=\"-1\">VAD</option>";
 members << "<option" << ((id==(void *)(-2))?" selected ":" ") << "value=\"-2\">VAD2</option>";
 for (s = memberNameList.begin(); s != memberNameList.end(); ++s) 
 {
  ConferenceMember * member = s->second;
  if(member==NULL) continue;
  long mint=(long)member->GetID();
  if(mint!=0)
  {
   PString username=s->first;
   members << "<option"
	<< ((mint==(long)id)?" selected ":" ")
	<< "value=\"" << mint << "\">"
	<< username << "</option>";
  }
 }
 return members;
}

BOOL OpenMCUH323EndPoint::MemberExist(Conference & conference, ConferenceMemberId id)
{
 PWaitAndSignal m(conference.GetMutex());
 Conference::MemberNameList & memberNameList = conference.GetMemberNameList();
 Conference::MemberNameList::const_iterator s;
 for (s = memberNameList.begin(); s != memberNameList.end(); ++s) 
 {
  ConferenceMember * member = s->second;
  if(member==NULL) continue;
  if(member->GetID()==id) return true;
 }
 PTRACE(6,"!MEMBER_EXIST_MATCH!\tid=" << id << " conference=" << conference);
 return false;
}

PString OpenMCUH323EndPoint::GetMemberListOpts(Conference & conference)
{
 PStringStream members;
// size_t memberListSize = 0;
 PWaitAndSignal m(conference.GetMutex());
 Conference::MemberNameList & memberNameList = conference.GetMemberNameList();
 Conference::MemberNameList::const_iterator s;
 members << "<table class=\"table table-striped table-bordered table-condensed\"><tr><td valign=top style='padding-left:12px'>";
 members << "<font color=green><b>Active Members</b></font>";
 members << "<table class=\"table table-striped table-bordered table-condensed\">";
 members << "<tr><th>Active Members<th>Mute voice<th>Disable VAD<th>Chosen Van<th>Drop"; 
 int i=0;
 for (s = memberNameList.begin(); s != memberNameList.end(); ++s) 
 {
  ConferenceMember * member = s->second;
  if(member==NULL) continue;
  long mint=(long)member->GetID();
   members << "<tr>"; 
  members << "<th id=\"tam_" << i << "\" >";
  members << s->first;
  members << "<th>";
  members << "<input type=\"checkbox\" name=\"m" << mint << "\" value=\"+\" " << ((member->muteIncoming)?"checked":"") << ">";
  members << "<th>";
  members << "<input type=\"checkbox\" name=\"v" << mint << "\" value=\"+\" " << ((member->disableVAD)?"checked":"") << ">";
  members << "<th>";
  members << "<input type=\"checkbox\" name=\"c" << mint << "\" value=\"+\" " << ((member->chosenVan)?"checked":"") << ">";
  members << "<th>";
  members << "<input type=\"checkbox\" name=\"d" << mint << "\" value=\"+\">";
  i++;
 }
 members << "<tr>"; 
 members << "<th>";
 members << "ALL Active!!!";
 members << "<th>";
 members << "<th>";
 members << "<th>";
 members << "<th>";
 members << "<input type=\"checkbox\" name=\"d" << "ALL" << "\" value=\"+\">";
 members  << "</table>";

 members << "</td><td valign=top style='padding-right:12px'>";

 members << "<font color=red><b>Inactive Members</b></font>";
 members << "<table class=\"table table-striped table-bordered table-condensed\">";
 members << "<tr><th>Inactive Members<th>Remove<th>Invite"; 
 members << "<tr>"; 
 members << "<th>";
 members << "ALL Inactive!!!";
 members << "<th>";
 members << "<input type=\"checkbox\" name=\"r" << "ALL" << "\" value=\"+\">";
 members << "<th>";
 members << "<input type=\"checkbox\" name=\"i" << "ALL" << "\" value=\"+\">";
 i=0;
 for (s = memberNameList.begin(); s != memberNameList.end(); ++s) 
 {
  ConferenceMember * member = s->second;
  if(member!=NULL) continue;
  members << "<tr>"; 
  members << "<th id=\"tim_" << i << "\">";
  members << s->first;
  members << "<th>";
  members << "<input type=\"checkbox\" name=\"r" << s->first << "\" value=\"+\">";
  members << "<th>";
  members << "<input id=\"iinv_" << i << "\" type=\"checkbox\" name=\"i" << s->first << "\" value=\"+\">";
  i++;
 }
 members  << "</table>";
 members << "</td></table>";

 return members;
}

PString OpenMCUH323EndPoint::GetConferenceOptsJavascript(Conference & c)
{
  PStringStream r; //conf[0]=[videoMixerCount,bfw,bfh):
  PString jsRoom=c.GetNumber(); jsRoom.Replace("&","&amp;",TRUE,0); jsRoom.Replace("\"","&quot;",TRUE,0);
  r << "conf=Array(Array(" //l1&l2 open
    << c.videoMixerCount                                                // [0][0]  = mixerCount
    << "," << OpenMCU::vmcfg.bfw                                        // [0][1]  = base frame width
    << "," << OpenMCU::vmcfg.bfh                                        // [0][2]  = base frame height
    << ",\"" << jsRoom << "\""                                          // [0][3]  = room name
    << ",'" << c.IsModerated() << "'"                                   // [0][4]  = control
    << ",'" << c.IsMuteUnvisible() << "'"                               // [0][5]  = global mute
    << "," << c.VAlevel << "," << c.VAdelay << "," << c.VAtimeout       // [0][6-8]= vad

    << ",Array("; // l3 open
      conferenceManager.GetConferenceListMutex().Wait();
      ConferenceListType & conferenceList = conferenceManager.GetConferenceList();
      ConferenceListType::iterator l;
      for (l = conferenceList.begin(); l != conferenceList.end(); ++l) {
        jsRoom=(*(l->second)).GetNumber();
        jsRoom.Replace("&","&amp;",TRUE,0); jsRoom.Replace("\"","&quot;",TRUE,0);
        if(l!=conferenceList.begin()) r << ",";                         // [0][9][ci][0-2] roomName & memberCount & isModerated
        r << "Array(\"" << jsRoom << "\"," << (*(l->second)).GetVisibleMemberCount() << ",\"" << (*(l->second)).IsModerated() << "\")";
      }
      conferenceManager.GetConferenceListMutex().Signal();
    r << ")"; // l3 close

#if USE_LIBYUV
    r << "," << OpenMCU::Current().GetScaleFilter();                      // [0][10] = libyuv resizer filter mode
#else
    r << ",-1";
#endif

  if(c.externalRecorder != NULL) r << ",1"; else r << ",0";               // [0][11] = external video recording state (1=recording, 0=NO)

  r << ")"; //l2 close

  Conference::VideoMixerRecord * vmr = c.videoMixerList;
  while (vmr!=NULL) {
    r << ",Array("; //l2 open
    MCUVideoMixer * mixer = vmr->mixer;
    unsigned n=mixer->GetPositionSet();
    VMPCfgSplitOptions & split=OpenMCU::vmcfg.vmconf[n].splitcfg;
    VMPCfgOptions      * p    =OpenMCU::vmcfg.vmconf[n].vmpcfg;


    r << "Array(" //l3 open                                             // conf[n][0]: base parameters:
      << split.mockup_width << "," << split.mockup_height                 // [n][0][0-1]= mw*mh
      << "," << n                                                         // [n][0][2]  = position set (layout)
    << "),Array("; //l3 reopen

    for(unsigned i=0;i<split.vidnum;i++)
      r << "Array(" << p[i].posx //l4 open                              // conf[n][1]: frame geometry for each position i:
        << "," << p[i].posy                                               // [n][1][i][0-1]= posx & posy
        << "," << p[i].width                                              // [n][1][i][2-3]= width & height
        << "," << p[i].height
        << "," << p[i].border                                             // [n][1][i][4]  = border
      << ")" << ((i==split.vidnum-1)?"":","); //l4 close

    r << ")," << mixer->VMPListScanJS() //l3 close                      // conf[n][2], conf[n][3]: members' ids & types
    << ")"; //l2 close

    vmr=vmr->next;
  }
  r << ");"; //l1 close
  return r;
}

PString OpenMCUH323EndPoint::GetMemberListOptsJavascript(Conference & conference)
{
 PStringStream members;
 PWaitAndSignal m(conference.GetMutex());
 Conference::MemberNameList & memberNameList = conference.GetMemberNameList();
 Conference::MemberNameList::const_iterator s;
 members << "members=Array(";
 int i=0;
 for (s = memberNameList.begin(); s != memberNameList.end(); ++s) 
 {
  PString username=s->first;
  username.Replace("&","&amp;",TRUE,0);
  username.Replace("\"","&quot;",TRUE,0);
  ConferenceMember * member = s->second;
  if(member==NULL){ // inactive member
    if(i>0) members << ",";
    members << "Array(0"
      << ",0"
      << ",\"" << username << "\""
      << ",0"
      << ",0"
      << ",0"
      << ",0"
      << ",0"
      << ",\"" << MCUURL(s->first).GetUrlId() << "\""
      << ")";
    i++;
  } else {          //   active member
    if(i>0) members << ",";
    members << "Array(1"                                // [i][0] = 1 : ONLINE
      << ",\"" << dec << (long)member->GetID() << "\""  // [i][1] = long id
      << ",\"" << username << "\""                      // [i][2] = name [ip]
      << "," << member->muteIncoming                    // [i][3] = mute
      << "," << member->disableVAD                      // [i][4] = disable vad
      << "," << member->chosenVan                       // [i][5] = chosen van
      << "," << member->GetAudioLevel()                 // [i][6] = audiolevel (peak)
      << "," << member->GetVideoMixerNumber()           // [i][7] = number of mixer member receiving
      << ",\"" << MCUURL(s->first).GetUrlId() << "\""
      << ")";
    i++;
  }
 }
 members << ");";

 members << "\np.addressbook=Array(";
 PStringArray abook = OpenMCU::Current().addressBook;
 for(PINDEX i = 0; i < abook.GetSize(); i++)
 {
   if(i>0) members << ",";
   PString username = abook[i];
   username.Replace("&","&amp;",TRUE,0);
   username.Replace("\"","&quot;",TRUE,0);
   members << "Array("
      << "0"
      << ",\"" << MCUURL(abook[i]).GetUrlId() << "\""
      << ",\"" << username << "\""
      << ")";
 }
 members << ");";

 return members;
}

BOOL OpenMCUH323EndPoint::SetMemberVideoMixer(Conference & conference, ConferenceMember * victim, unsigned newMixerNumber)
{ // formatString: VIDEOCAP @ W x H : BITRATE x FRAMERATE _ ROOM / MIXER
  unsigned oldMixerNumber=victim->GetVideoMixerNumber();
  if(oldMixerNumber == newMixerNumber) return TRUE;
  if(conference.VMLFind(newMixerNumber) == NULL) return FALSE;

  H323Connection_ConferenceMember *connMember = dynamic_cast<H323Connection_ConferenceMember *>(victim); if(connMember==NULL) return FALSE;
  OpenMCUH323Connection *conn=(OpenMCUH323Connection *)FindConnectionWithoutLock(connMember->GetH323Token()); if(conn==NULL) return FALSE;

  PString newFormatString; int codecCacheMode; PString oldFormatString;
  BOOL connCodecNotNull = (conn->GetVideoTransmitCodec()!=NULL);
  if(connCodecNotNull) { codecCacheMode=conn->GetVideoTransmitCodec()->cacheMode; oldFormatString=conn->GetVideoTransmitCodec()->formatString; }
  else { codecCacheMode=-1; oldFormatString="NO_CODEC"; }

  PINDEX pos=oldFormatString.Find("/"); if((codecCacheMode==-1)||(pos==P_MAX_INDEX))
  { PTRACE(1,"MixerCtrl\tFail, f/s unrecognized or codec=NULL: c/m=" << codecCacheMode << ", f/s=" << oldFormatString); return FALSE; }


  PStringStream str; str << newMixerNumber;
  newFormatString=oldFormatString.Left(pos+1) + str;

  PWaitAndSignal m(conn->GetVideoTransmitCodec()->GetVideoHandlerMutex());

  if(codecCacheMode==2) { PTRACE(4,"MixerCtrl\tWorst: he's cached, detaching"); conn->GetVideoTransmitCodec()->DetachCacheRTP(); /* PThread::Sleep(50); */ }
  conn->videoMixerNumber=newMixerNumber; conn->GetConferenceMember()->SetVideoMixerNumber(newMixerNumber);
  if(connCodecNotNull){ conn->GetVideoTransmitCodec()->formatString=newFormatString; conn->SetVideoTransmitCodecName(newFormatString); }

  if(codecCacheMode==2) conn->OpenVideoChannel(TRUE, *(conn->GetVideoTransmitCodec()));

  return TRUE;
}

ConferenceMember * OpenMCUH323EndPoint::GetConferenceMemberById(Conference * conference, long id)
{ // safer than just memberList.search (?)
//  PWaitAndSignal m(conference.GetMutex()); // memberListMutex
  Conference::MemberList & memberList = conference->GetMemberList();
  Conference::MemberList::const_iterator r;
  for (r = memberList.begin(); r != memberList.end(); ++r)
  {
    ConferenceMember * member = r->second;
    if(member->GetName()=="file recorder") continue;
    if(member->GetName()=="cache") continue;
    if ((long)(member->GetID()) == id) return member;
  }
  return NULL;
}

PString OpenMCUH323EndPoint::OTFControl(const PString room, const PStringToString & data)
{
  PTRACE(6,"WebCtrl\tRoom " << room << ", action " << data("action") << ", value " << data("v") << ", options " << data("o") << ", " << data("o2") << ", " << data("o3"));

  if(!data.Contains("action")) return "FAIL"; int action=data("action").AsInteger();

  if(!data.Contains("v")) return "FAIL"; PString value=data("v");
  long v = value.AsInteger();

#if USE_LIBYUV
  if(action == OTFC_YUV_FILTER_MODE)
  {
    if(v==1) OpenMCU::Current().SetScaleFilter(libyuv::kFilterBilinear);
    else if(v==2) OpenMCU::Current().SetScaleFilter(libyuv::kFilterBox);
    else OpenMCU::Current().SetScaleFilter(libyuv::kFilterNone);
    PStringStream cmd;
    cmd << "conf[0][10]=" << v;
    OpenMCU::Current().HttpWriteCmdRoom(cmd,room);
    OpenMCU::Current().HttpWriteCmdRoom("top_panel()",room);
    return "OK";
  }
#endif

#define OTF_RET_OK { conferenceManager.UnlockConference(); return "OK"; }
#define OTF_RET_FAIL { conferenceManager.UnlockConference(); return "FAIL"; }

  Conference * conference = conferenceManager.MakeAndLockConference(room); // hope it already created: we'll just get it

  if(action == OTFC_REFRESH_VIDEO_MIXERS)
  {
    OpenMCU::Current().HttpWriteCmdRoom(GetConferenceOptsJavascript(*conference),room);
    OpenMCU::Current().HttpWriteCmdRoom("mixrfr()",room);
    OTF_RET_OK;
  }
  if(action == OTFC_VIDEO_RECORDER_START)
  { if(conference->externalRecorder == NULL)
    { conference->externalRecorder = new ExternalVideoRecorderThread(room);
      PThread::Sleep(500);
      if(conference->externalRecorder->running)
      { OpenMCU::Current().HttpWriteEventRoom("Video recording started",room);
        OpenMCU::Current().HttpWriteCmdRoom(GetConferenceOptsJavascript(*conference),room);
        OpenMCU::Current().HttpWriteCmdRoom("build_page()",room);
        OTF_RET_OK;
      }
      conference->externalRecorder = NULL;
    }
    OTF_RET_FAIL;
  }
  if(action == OTFC_VIDEO_RECORDER_STOP)
  { if(conference->externalRecorder != NULL)
    { conference->externalRecorder->running=FALSE;
      PThread::Sleep(1000);
      conference->externalRecorder = NULL;
      OpenMCU::Current().HttpWriteEventRoom("Video recording stopped",room);
      OpenMCU::Current().HttpWriteCmdRoom(GetConferenceOptsJavascript(*conference),room);
      OpenMCU::Current().HttpWriteCmdRoom("build_page()",room);
      OTF_RET_OK;
    }
    OTF_RET_FAIL;
  }
  if(action == OTFC_TEMPLATE_RECALL)
  {
    OpenMCU::Current().HttpWriteCmdRoom("alive()",room);

    if(conference->IsModerated()=="-")
    { conference->SetModerated(TRUE);
      conference->videoMixerList->mixer->SetForceScreenSplit(TRUE);
      conference->PutChosenVan();
      OpenMCU::Current().HttpWriteEventRoom("<span style='background-color:#bfb'>Operator took the control</span>",room);
      OpenMCU::Current().HttpWriteCmdRoom("r_moder()",room);
    }

    conference->confTpl = conference->ExtractTemplate(value);
    conference->LoadTemplate(conference->confTpl);
    conference->SetLastUsedTemplate(value);
    PStringStream msg;
    msg << GetMemberListOptsJavascript(*conference) << "\n"
        << "p." << GetConferenceOptsJavascript(*conference) << "\n"
        << "p.tl=Array" << conference->GetTemplateList() << "\n"
        << "p.seltpl=\"" << conference->GetSelectedTemplateName() << "\"\n"
        << "p.build_page()";
      OpenMCU::Current().HttpWriteCmdRoom(msg,room);
    OTF_RET_OK;
  }
  if(action == OTFC_SAVE_TEMPLATE)
  {
    OpenMCU::Current().HttpWriteCmdRoom("alive()",room);
    PString templateName=value.Trim();
    if(templateName=="") OTF_RET_FAIL;
    if(templateName.Right(1) == "*") templateName=templateName.Left(templateName.GetLength()-1).RightTrim();
    if(templateName=="") OTF_RET_FAIL;
    conference->confTpl = conference->SaveTemplate(templateName);
    conference->TemplateInsertAndRewrite(templateName, conference->confTpl);
    conference->LoadTemplate(conference->confTpl);
    PStringStream msg;
    msg << GetMemberListOptsJavascript(*conference) << "\n"
        << "p." << GetConferenceOptsJavascript(*conference) << "\n"
        << "p.tl=Array" << conference->GetTemplateList() << "\n"
        << "p.seltpl=\"" << conference->GetSelectedTemplateName() << "\"\n"
        << "p.build_page()";
      OpenMCU::Current().HttpWriteCmdRoom(msg,room);
    OTF_RET_OK;
  }
  if(action == OTFC_DELETE_TEMPLATE)
  {
    OpenMCU::Current().HttpWriteCmdRoom("alive()",room);
    PString templateName=value.Trim();
    if(templateName=="") OTF_RET_FAIL;
    if(templateName.Right(1) == "*") OTF_RET_FAIL;
    conference->DeleteTemplate(templateName);
    conference->LoadTemplate("");
    conference->SetLastUsedTemplate("");
    PStringStream msg;
    msg << GetMemberListOptsJavascript(*conference) << "\n"
        << "p." << GetConferenceOptsJavascript(*conference) << "\n"
        << "p.tl=Array" << conference->GetTemplateList() << "\n"
        << "p.seltpl=\"" << conference->GetSelectedTemplateName() << "\"\n"
        << "p.build_page()";
      OpenMCU::Current().HttpWriteCmdRoom(msg,room);
    OTF_RET_OK;
  }
  if(action == OTFC_INVITE)
  { conference->InviteMember(value); OTF_RET_OK; }
  if(action == OTFC_REMOVE_OFFLINE_MEMBER)
  {
    conference->RemoveOfflineMemberFromNameList(value);
    PStringStream msg;
    msg << GetMemberListOptsJavascript(*conference) << "\n"
        << "p.members_refresh()";
    OpenMCU::Current().HttpWriteCmdRoom(msg,room);
    OTF_RET_OK;
  }
  if(action == OTFC_DROP_ALL_ACTIVE_MEMBERS)
  {
    conferenceManager.UnlockConference();
    PWaitAndSignal m(conference->GetMutex());
    Conference::MemberList & memberList = conference->GetMemberList();
    Conference::MemberList::iterator r;
    for (r = memberList.begin(); r != memberList.end(); ++r)
    {
      ConferenceMember * member = r->second;
      if(member->GetName()=="file recorder") continue;
      if(member->GetName()=="cache") continue;
      member->Close();
//      memberList.erase(r->first);
    }
    OpenMCU::Current().HttpWriteEventRoom("Active members dropped by operator",room);
    OpenMCU::Current().HttpWriteCmdRoom("drop_all()",room);
    return "OK";
  }
  if(action == OTFC_MUTE_ALL)
  {
    conferenceManager.UnlockConference();
    PWaitAndSignal m(conference->GetMutex());
    Conference::MemberList & memberList = conference->GetMemberList();
    Conference::MemberList::iterator r;
    for (r = memberList.begin(); r != memberList.end(); ++r)
    {
      ConferenceMember * member = r->second;
      if(member->GetName()=="file recorder") continue;
      if(member->GetName()=="cache") continue;
      member->muteIncoming=TRUE;
    }
    OpenMCU::Current().HttpWriteCmdRoom("imute_all()",room);
    return "OK";
  }
  if(action == OTFC_INVITE_ALL_INACT_MMBRS)
  { Conference::MemberNameList & memberNameList = conference->GetMemberNameList();
    Conference::MemberNameList::const_iterator r;
    for (r = memberNameList.begin(); r != memberNameList.end(); ++r) if(r->second==NULL) conference->InviteMember(r->first);
    OTF_RET_OK;
  }
  if(action == OTFC_REMOVE_ALL_INACT_MMBRS)
  { Conference::MemberNameList & memberNameList = conference->GetMemberNameList();
    Conference::MemberNameList::const_iterator r;
    for (r = memberNameList.begin(); r != memberNameList.end(); ++r) if(r->second==NULL) conference->RemoveOfflineMemberFromNameList((PString &)(r->first));
    OpenMCU::Current().HttpWriteEventRoom("Offline members removed by operator",room);
    OpenMCU::Current().HttpWriteCmdRoom("remove_all()",room);
    OTF_RET_OK;
  }
  if(action == OTFC_SAVE_MEMBERS_CONF)
  { FILE *membLst;
    PString name="members_"+room+".conf";
    membLst = fopen(name,"w");
    if(membLst==NULL) { OpenMCU::Current().HttpWriteEventRoom("<font color=red>Error: Can't save member list</font>",room); OTF_RET_FAIL; }
    fputs(conference->membersConf,membLst);
    fclose(membLst);
    OpenMCU::Current().HttpWriteEventRoom("Member list saved",room);
    OTF_RET_OK;
  }
  if(action == OTFC_TAKE_CONTROL)
  { if(conference->IsModerated()=="-")
    { conference->SetModerated(TRUE);
      conference->videoMixerList->mixer->SetForceScreenSplit(TRUE);
      conference->PutChosenVan();
      OpenMCU::Current().HttpWriteEventRoom("<span style='background-color:#bfb'>Operator took the control</span>",room);
      OpenMCU::Current().HttpWriteCmdRoom("r_moder()",room);
    }
    OTF_RET_OK;
  }
  if(action == OTFC_DECONTROL)
  { if(conference->IsModerated()=="+")
    { conference->SetModerated(FALSE);
      conference->videoMixerList->mixer->SetForceScreenSplit(OpenMCU::Current().GetForceScreenSplit());
      conferenceManager.UnlockConference();  // we have to UnlockConference
      UnmoderateConference(*conference);  // before conference.GetMutex() usage
      OpenMCU::Current().HttpWriteEventRoom("<span style='background-color:#acf'>Operator resigned</span>",room);
      OpenMCU::Current().HttpWriteCmdRoom("r_unmoder()",room);
      OpenMCU::Current().HttpWriteCmdRoom(GetConferenceOptsJavascript(*conference),room);
      OpenMCU::Current().HttpWriteCmdRoom("build_page()",room);
      return "OK";
    }
    OTF_RET_OK;
  }
  if(action == OTFC_ADD_VIDEO_MIXER)
  { if(conference->IsModerated()=="+" && conference->GetNumber() != "testroom")
    { unsigned n = conference->VMLAdd();
      PStringStream msg; msg << "Video mixer " << n << " added";
      OpenMCU::Current().HttpWriteEventRoom(msg,room);
      OpenMCU::Current().HttpWriteCmdRoom(GetConferenceOptsJavascript(*conference),room);
      OpenMCU::Current().HttpWriteCmdRoom("build_page()",room);
      OTF_RET_OK;
    }
    OTF_RET_FAIL;
  }
  if(action == OTFC_DELETE_VIDEO_MIXER)
  { if(conference->IsModerated()=="+" && conference->GetNumber() != "testroom")
    {
      unsigned n_old=conference->videoMixerCount;
      unsigned n = conference->VMLDel(v);
      if(n_old!=n)
      { PStringStream msg; msg << "Video mixer " << v << " removed";
        OpenMCU::Current().HttpWriteEventRoom(msg,room);
        OpenMCU::Current().HttpWriteCmdRoom(GetConferenceOptsJavascript(*conference),room);
        OpenMCU::Current().HttpWriteCmdRoom("build_page()",room);
        OTF_RET_OK;
      }
    }
    OTF_RET_FAIL;
  }
  if (action == OTFC_SET_VIDEO_MIXER_LAYOUT)
  { unsigned option = data("o").AsInteger();
    MCUVideoMixer * mixer = conference->VMLFind(option);
    if(mixer!=NULL)
    { mixer->MyChangeLayout(v);
      conference->PutChosenVan();
      conference->FreezeVideo(NULL);
      OpenMCU::Current().HttpWriteCmdRoom(GetConferenceOptsJavascript(*conference),room);
      OpenMCU::Current().HttpWriteCmdRoom("build_page()",room);
      OTF_RET_OK;
    }
    OTF_RET_FAIL;
  }
  if (action == OTFC_REMOVE_VMP)
  { MCUVideoMixer * mixer = conference->VMLFind(v); if(mixer==NULL) OTF_RET_FAIL;
    unsigned pos = data("o").AsInteger();
    mixer->MyRemoveVideoSource(pos,TRUE);
    conference->FreezeVideo(NULL);
    OpenMCU::Current().HttpWriteCmdRoom(GetConferenceOptsJavascript(*conference),room);
    OpenMCU::Current().HttpWriteCmdRoom("build_page()",room);
    OTF_RET_OK;
  }
  if(action == OTFC_MIXER_ARRANGE_VMP)
  { MCUVideoMixer * mixer = conference->VMLFind(v); if(mixer==NULL) OTF_RET_FAIL;
    Conference::MemberList & memberList = conference->GetMemberList();
    for (Conference::MemberList::const_iterator r = memberList.begin(); r != memberList.end(); ++r)
    if(r->second != NULL) if(r->second->IsVisible())
    { if (mixer->AddVideoSourceToLayout(r->second->GetID(), *(r->second))) r->second->SetFreezeVideo(FALSE); else break; }
    OpenMCU::Current().HttpWriteCmdRoom(GetConferenceOptsJavascript(*conference),room);
    OpenMCU::Current().HttpWriteCmdRoom("build_page()",room);
    OTF_RET_OK;
  }
  if(action == OTFC_MIXER_CLEAR)
  { MCUVideoMixer * mixer = conference->VMLFind(v); if(mixer==NULL) OTF_RET_FAIL;
    mixer->MyRemoveAllVideoSource();
    OpenMCU::Current().HttpWriteCmdRoom(GetConferenceOptsJavascript(*conference),room);
    OpenMCU::Current().HttpWriteCmdRoom("build_page()",room);
    OTF_RET_OK;
  }
  if(action == OTFC_MIXER_SHUFFLE_VMP)
  { MCUVideoMixer * mixer = conference->VMLFind(v); if(mixer==NULL) OTF_RET_FAIL;
    mixer->Shuffle();
    OpenMCU::Current().HttpWriteCmdRoom(GetConferenceOptsJavascript(*conference),room);
    OpenMCU::Current().HttpWriteCmdRoom("build_page()",room);
    OTF_RET_OK;
  }
  if(action == OTFC_MIXER_SCROLL_LEFT)
  { MCUVideoMixer * mixer = conference->VMLFind(v); if(mixer==NULL) OTF_RET_FAIL;
    mixer->Scroll(TRUE);
    OpenMCU::Current().HttpWriteCmdRoom(GetConferenceOptsJavascript(*conference),room);
    OpenMCU::Current().HttpWriteCmdRoom("build_page()",room);
    OTF_RET_OK;
  }
  if(action == OTFC_MIXER_SCROLL_RIGHT)
  { MCUVideoMixer * mixer = conference->VMLFind(v); if(mixer==NULL) OTF_RET_FAIL;
    mixer->Scroll(FALSE);
    OpenMCU::Current().HttpWriteCmdRoom(GetConferenceOptsJavascript(*conference),room);
    OpenMCU::Current().HttpWriteCmdRoom("build_page()",room);
    OTF_RET_OK;
  }
  if(action == OTFC_MIXER_REVERT)
  { MCUVideoMixer * mixer = conference->VMLFind(v); if(mixer==NULL) OTF_RET_FAIL;
    mixer->Revert();
    OpenMCU::Current().HttpWriteCmdRoom(GetConferenceOptsJavascript(*conference),room);
    OpenMCU::Current().HttpWriteCmdRoom("build_page()",room);
    OTF_RET_OK;
  }
  if(action == OTFC_GLOBAL_MUTE)
  { if(data("v")=="true")v=1; if(data("v")=="false") v=0; conference->SetMuteUnvisible((BOOL)v);
    OpenMCU::Current().HttpWriteCmdRoom(GetConferenceOptsJavascript(*conference),room);
    OpenMCU::Current().HttpWriteCmdRoom("build_page()",room);
    OTF_RET_OK;
  }
  if(action == OTFC_SET_VAD_VALUES)
  { conference->VAlevel   = (unsigned short int) v;
    conference->VAdelay   = (unsigned short int) (data("o").AsInteger());
    conference->VAtimeout = (unsigned short int) (data("o2").AsInteger());
    OpenMCU::Current().HttpWriteCmdRoom(GetConferenceOptsJavascript(*conference),room);
    OpenMCU::Current().HttpWriteCmdRoom("build_page()",room);
    OTF_RET_OK;
  }
  if (action == OTFC_MOVE_VMP)
  { MCUVideoMixer * mixer1 = conference->VMLFind(v); if(mixer1==NULL) OTF_RET_FAIL;
    if(!data.Contains("o2")) OTF_RET_FAIL; MCUVideoMixer * mixer2=conference->VMLFind(data("o2").AsInteger()); if(mixer2==NULL) OTF_RET_FAIL;
    int pos1 = data("o").AsInteger(); int pos2 = data("o3").AsInteger();
    if(mixer1==mixer2) mixer1->Exchange(pos1,pos2);
    else
    {
      ConferenceMemberId id = mixer1->GetHonestId(pos1); if(((long)id<100)&&((long)id>=0)) id=NULL;
      ConferenceMemberId id2 = mixer2->GetHonestId(pos2); if(((long)id2<100)&&((long)id2>=0)) id2=NULL;
      mixer2->PositionSetup(pos2, 1, GetConferenceMemberById(conference, (long)id));
      mixer1->PositionSetup(pos1, 1, GetConferenceMemberById(conference, (long)id2));
    }
    OpenMCU::Current().HttpWriteCmdRoom(GetConferenceOptsJavascript(*conference),room);
    OpenMCU::Current().HttpWriteCmdRoom("build_page()",room);
    OTF_RET_OK;
  }
  if (action == OTFC_VAD_CLICK)
  { MCUVideoMixer * mixer = conference->VMLFind(v); if(mixer==NULL) OTF_RET_FAIL;
    unsigned pos = data("o").AsInteger();
    int type = data("o2").AsInteger();
    if((type<1)||(type>3)) type=2;
    long id = (long)mixer->GetHonestId(pos);
    if((type==1)&&(id>=0)&&(id<100)) //static but no member
    {
      Conference::MemberList & memberList = conference->GetMemberList();
      Conference::MemberList::const_iterator r;
      for (r = memberList.begin(); r != memberList.end(); ++r)
      {
        if(r->second != NULL)
        {
          if(r->second->IsVisible())
          {
            if (mixer->VMPListFindVMP(r->second->GetID())==NULL)
            {
              mixer->PositionSetup(pos, 1, r->second);
              r->second->SetFreezeVideo(FALSE);
              break;
            }
          }
        }
      }
      if(r==memberList.end()) mixer->PositionSetup(pos,1,NULL);
    }
    else if((id>=0)&&(id<100)) mixer->PositionSetup(pos,type,NULL);
    else mixer->SetPositionType(pos,type);
    conference->FreezeVideo(NULL);
    OpenMCU::Current().HttpWriteCmdRoom(GetConferenceOptsJavascript(*conference),room);
    OpenMCU::Current().HttpWriteCmdRoom("build_page()",room);
    OTF_RET_OK;
  }

  ConferenceMember * member = GetConferenceMemberById(conference, v); if(member==NULL) OTF_RET_FAIL;
  PStringStream cmd;

  if( action == OTFC_SET_VMP_STATIC )
  { unsigned n=data("o").AsInteger(); MCUVideoMixer * mixer = conference->VMLFind(n); if(mixer==NULL) OTF_RET_FAIL;
    int pos = data("o2").AsInteger(); mixer->PositionSetup(pos, 1, member);
    H323Connection_ConferenceMember *connMember = dynamic_cast<H323Connection_ConferenceMember *>(member);
    if(connMember==NULL) OTF_RET_FAIL;
    connMember->SetFreezeVideo(FALSE);
    OpenMCU::Current().HttpWriteCmdRoom(GetConferenceOptsJavascript(*conference),room);
    OpenMCU::Current().HttpWriteCmdRoom("build_page()",room);
    OTF_RET_OK;
  }
  if( action == OTFC_UNMUTE )
  { member->muteIncoming=FALSE; cmd << "iunmute(" << v << ")"; OpenMCU::Current().HttpWriteCmdRoom(cmd,room); OTF_RET_OK; }
  if( action == OTFC_MUTE)
  { member->muteIncoming=TRUE; cmd << "imute(" << v << ")"; OpenMCU::Current().HttpWriteCmdRoom(cmd,room); OTF_RET_OK; }
  if( action == OTFC_REMOVE_FROM_VIDEOMIXERS)
  { if(conference->IsModerated()=="+" && conference->GetNumber() != "testroom")
    {
      Conference::VideoMixerRecord * vmr = conference->videoMixerList;
      while( vmr!=NULL ) 
      { MCUVideoMixer * mixer = vmr->mixer;
        int oldPos = mixer->GetPositionNum(member->GetID());
        if(oldPos != -1) mixer->MyRemoveVideoSource(oldPos, TRUE);
        vmr=vmr->next;
      }
      H323Connection_ConferenceMember *connMember = dynamic_cast<H323Connection_ConferenceMember *>(member);
      if(connMember!=NULL) connMember->SetFreezeVideo(TRUE);
      OpenMCU::Current().HttpWriteCmdRoom(GetConferenceOptsJavascript(*conference),room);
      OpenMCU::Current().HttpWriteCmdRoom("build_page()",room);
      OTF_RET_OK;
    }
  }
  if( action == OTFC_DROP_MEMBER )
  {
    // MAY CAUSE DEADLOCK PWaitAndSignal m(conference->GetMutex();
//    conference->GetMemberList().erase(member->GetID());
    member->Close();
    OTF_RET_OK;
  }
  if (action == OTFC_VAD_NORMAL)
  {
    member->disableVAD=FALSE;
    member->chosenVan=FALSE;
    conferenceManager.UnlockConference();
    conference->PutChosenVan();
    cmd << "ivad(" << v << ",0)";
    OpenMCU::Current().HttpWriteCmdRoom(cmd,room);
    return "OK";
  }
  if (action == OTFC_VAD_CHOSEN_VAN)
  {
    member->disableVAD=FALSE;
    member->chosenVan=TRUE;
    conferenceManager.UnlockConference();
    conference->PutChosenVan();
    conference->FreezeVideo(member->GetID());
    cmd << "ivad(" << v << ",1)";
    OpenMCU::Current().HttpWriteCmdRoom(cmd,room);
    return "OK";
  }
  if (action == OTFC_VAD_DISABLE_VAD)
  {
    member->disableVAD=TRUE;
    member->chosenVan=FALSE;
    conferenceManager.UnlockConference();
    conference->PutChosenVan();
#if 1 // DISABLING VAD WILL CAUSE MEMBER REMOVAL FROM VAD POSITIONS
    {
      PWaitAndSignal m(conference->GetMutex());
      ConferenceMemberId id = member->GetID();
      Conference::VideoMixerRecord * vmr = conference->videoMixerList;
      while(vmr!=NULL)
      {
        int type = vmr->mixer->GetPositionType(id);
        if(type<2 || type>3) { vmr=vmr->next; continue;} //-1:not found, 1:static, 2&3:VAD
        vmr->mixer->MyRemoveVideoSourceById(id, FALSE);
        vmr = vmr->next;
      }
      conference->FreezeVideo(id);
    }
#endif
    cmd << "ivad(" << v << ",2)";
    OpenMCU::Current().HttpWriteCmdRoom(cmd,room);
    return "OK";
  }
  if (action == OTFC_SET_MEMBER_VIDEO_MIXER)
  { unsigned option = data("o").AsInteger();
    if(SetMemberVideoMixer(*conference,member,option))
    { cmd << "chmix(" << v << "," << option << ")";
      OpenMCU::Current().HttpWriteCmdRoom(cmd,room);
      OTF_RET_OK;
    }
    if(SetMemberVideoMixer(*conference,member,0)) // rotate back to 0
    { cmd << "chmix(" << v << ",0)";
      OpenMCU::Current().HttpWriteCmdRoom(cmd,room);
      OTF_RET_OK;
    }
    OTF_RET_FAIL;
  }

  OTF_RET_FAIL;
}

void OpenMCUH323EndPoint::SetMemberListOpts(Conference & conference,const PStringToString & data)
{
 PWaitAndSignal m(conference.GetMutex());
 Conference::MemberList & memberList = conference.GetMemberList();
 Conference::MemberList::const_iterator s;
 for (s = memberList.begin(); s != memberList.end(); ++s) 
 {
  ConferenceMember * member = s->second;
  if(member->GetName()=="file recorder") continue;
  if(member->GetName()=="cache") continue;
  ConferenceMemberId mid = member->GetID();

  PString arg = (long)mid;
  PString arg1 = "m" + arg;
  PString opt = data(arg1);
  member->muteIncoming = (opt == "+")?TRUE:FALSE; 
  arg1 = "v" + arg;
  opt = data(arg1);
  member->disableVAD = (opt == "+")?TRUE:FALSE; 
  arg1 = "c" + arg;
  opt = data(arg1);
  member->chosenVan = (opt == "+")?1:0; 
 }
/* 
 for (s = memberList.begin(); s != memberList.end(); ++s) 
 {
  ConferenceMember * member = s->second;
  if(member->GetName()=="file recorder") continue;
  ConferenceMemberId mid = member->GetID();
  
  PString arg = (int)mid;
  PString arg1 = "d" + arg;
  PString opt = data(arg1);
  if(opt == "Drop") 
  { 
   conference.RemoveMember(member);
   member->Close();
//   member->WaitForClose();
   return; 
  }
 }
*/

 int i = memberList.size();
 s = memberList.end(); s--;
 PString dall = data("dALL");
 while(i!=0)
 {
  ConferenceMember * member = s->second;
  s--; i--;
  if(member->GetName()=="file recorder") continue;
  if(member->GetName()=="cache") continue;
  ConferenceMemberId mid = member->GetID();
  
  PString arg = (long)mid;
  PString arg1 = "d" + arg;
  PString opt = data(arg1);
  if(opt == "+" || dall == "+") 
  {
//   PThread::Sleep(500);
//   conference.RemoveMember(member);
   memberList.erase(member->GetID());
   member->Close();
//   member->WaitForClose();
//   return; 
  }
 }

}

void OpenMCUH323EndPoint::OfflineMembersManager(Conference & conference,const PStringToString & data)
{
 PWaitAndSignal m(conference.GetMutex());
 Conference::MemberNameList & memberNameList = conference.GetMemberNameList();
 Conference::MemberNameList::const_iterator s;
 PString iall = data("iALL");
 for (s = memberNameList.begin(); s != memberNameList.end(); ++s) 
 {
  ConferenceMember * member = s->second;
  if(member!=NULL) continue; //online member
  PString arg = "i" + s->first;
  PString opt = data(arg);
  const char * name=s->first;
  if(opt=="+" || iall=="+") conference.InviteMember(name);
 }

 PString rall = data("rALL");
 for (s = memberNameList.begin(); s != memberNameList.end(); ) 
 {
  PString opt = data("r" + s->first);
  PString name = s->first;
  ConferenceMember * member = s->second;
  s++;
  if(member!=NULL) continue; //online member
  if(opt=="+" || rall=="+") 
    conference.RemoveOfflineMemberFromNameList(name);
 }
}


PString OpenMCUH323EndPoint::GetRoomList(const PString & block)
{
  PString substitution;
  PWaitAndSignal m(conferenceManager.GetConferenceListMutex());
  ConferenceListType & conferenceList = conferenceManager.GetConferenceList();

  PString insert = block;
  PStringStream members;
  members << "<input name='room' id='room' type=hidden>";
  ConferenceListType::iterator r;
  for (r = conferenceList.begin(); r != conferenceList.end(); ++r)
  {
    Conference & conference = *(r->second);
    PString roomNumber = conference.GetNumber();
#if ENABLE_TEST_ROOMS
    if((roomNumber.Left(8)=="testroom") && (roomNumber.GetLength() > 8)) continue;
#endif
#if ENABLE_ECHO_MIXER
    if(roomNumber.Left(4)*="echo") continue;
#endif
    // make a copy of the repeating html chunk
    members << "<span class=\"btn btn-large";
    if(conference.IsModerated()=="+") members << " btn-success";
    else members << " btn-primary";
    members << "\" onclick='document.getElementById(\"room\").value=\"" << roomNumber << "\";document.forms[0].submit();'>"
            << roomNumber << " " << conference.IsModerated() << " " << conference.GetVisibleMemberCount()
	    << "</span>";
  }
  members << "";
  SpliceMacro(insert, "List", members);
  substitution += insert;
  return substitution;
}

PString OpenMCUH323EndPoint::RoomCtrlPage(const PString room, BOOL ctrl, int n, Conference & conference, ConferenceMemberId *idp)
{
// int i,j;
 PStringStream page;

 BeginPage(page,"Room Control","window.l_control","window.l_info_control");

 page << "<script src=\"control.js\"></script>";

 page << "<div id='cb1' name='cb1' class='input-append'>&nbsp;</div>"
   << "<div style='text-align:center'>"
     << "<div id='cb2' name='cb2' style='margin-left:auto;margin-right:auto;width:100px;height:100px;background-color:#ddd'>"
       << "<div id='logging0' style='position:relative;top:0px;left:-50px;width:0px;height:0px'>"
         << "<div id='logging1' style='position:absolute;width:50px;height:50px'>"
           << "<iframe style='background-color:#eef;border:1px solid #55c;padding:0px;margin:0px' id='loggingframe' name='loggingframe' src='Comm?room=" << room << "' width=50 height=50>"
             << "Your browser does not support IFRAME tag :("
           << "</iframe>"
         << "</div>"
       << "</div>"
       << "<div id='cb3' name='cb3' style='position:relative;top:0px;left:0px;width:0px;height:0px'>"
         << "&nbsp;"
       << "</div>"
     << "</div>"
   << "</div>"
 ;
 EndPage(page,OpenMCU::Current().GetHtmlCopyright());
 return page;
}


void OpenMCUH323EndPoint::UnmoderateConference(Conference & conference)
{
  PWaitAndSignal m(conference.GetMutex());

  MCUVideoMixer * mixer = conference.videoMixerList->mixer;
  mixer->MyRemoveAllVideoSource();

  Conference::MemberList & memberList = conference.GetMemberList();
  Conference::MemberList::const_iterator s;
  for (s = memberList.begin(); s != memberList.end(); ++s) 
  {
    ConferenceMember * member = s->second;
    if(member->GetName()!="cache") SetMemberVideoMixer(conference, member, 0);
    if(!member->IsVisible()) continue;
    if(mixer->AddVideoSource(member->GetID(), *member)) member->SetFreezeVideo(FALSE);
    else member->SetFreezeVideo(TRUE);
  }

  while(conference.videoMixerCount>1) conference.VMLDel(conference.videoMixerCount-1);
}


PString OpenMCUH323EndPoint::SetRoomParams(const PStringToString & data)
{
  PString room = data("room");

  // "On-the-Fly" Control via XMLHTTPRequest:
  if(data.Contains("otfc")) return OTFControl(room, data);

  if(data.Contains("refresh")) // JavaScript data refreshing
  {
    PTRACE(6,"WebCtrl\tJS refresh");
    ConferenceListType::iterator r;
    PWaitAndSignal m(conferenceManager.GetConferenceListMutex());
    ConferenceListType & conferenceList = conferenceManager.GetConferenceList();
    for (r = conferenceList.begin(); r != conferenceList.end(); ++r) if(r->second->GetNumber() == room) break;
    if(r == conferenceList.end() ) return "";
    Conference & conference = *(r->second);
    return GetMemberListOptsJavascript(conference);
  }

  OpenMCU::Current().HttpWriteEventRoom("MCU Operator connected",room);
  PTRACE(6,"WebCtrl\tOperator connected");
  ConferenceListType::iterator r;
  PWaitAndSignal m(conferenceManager.GetConferenceListMutex());
  ConferenceListType & conferenceList = conferenceManager.GetConferenceList();
  for (r = conferenceList.begin(); r != conferenceList.end(); ++r) if(r->second->GetNumber() == room) break;
  if(r == conferenceList.end() ) return "OpenMCU.ru: Bad room";
  Conference & conference = *(r->second);
  MCUVideoMixer * mixer = conference.videoMixerList->mixer;
  ConferenceMemberId idr[100];
  for(int i=0;i<100;i++) idr[i]=mixer->GetPositionId(i);
  return RoomCtrlPage(room,conference.IsModerated()=="+",mixer->GetPositionSet(),conference,idr);
}



PString OpenMCUH323EndPoint::GetMonitorText()
{ PStringStream output;

  PWaitAndSignal m(conferenceManager.GetConferenceListMutex());
  ConferenceListType & conferenceList = conferenceManager.GetConferenceList();

  output << "Room Count: " << (int)conferenceList.size() << "\n"
         << "Max Room Count: " << conferenceManager.GetMaxConferenceCount() << "\n";

  ConferenceListType::iterator r;
  PINDEX confNum = 0;
  for (r = conferenceList.begin(); r != conferenceList.end(); ++r)
  { Conference & conference = *(r->second); Conference::MemberList & memberList = conference.GetMemberList();

    output << "\n[Conference "     << ++confNum << "]\n"
           << "Title: "            <<  conference.GetNumber() << "\n"
           << "ID: "               << conference.GetID() << "\n"
           << "Duration: "         << (PTime() - conference.GetStartTime()) << "\n"
           << "Member Count: "     << (int)memberList.size() << "\n"
           << "Max Member Count: " << conference.GetMaxMemberCount() << "\n";

    Conference::MemberList::const_iterator s;
    PINDEX num = 0;
    for (s = memberList.begin(); s != memberList.end(); ++s)
    { ConferenceMember * member = s->second;
      if (member != NULL)
      { output << "[Member " << ++num << "]\n";
        PStringStream hdr; hdr << "  ";
        PString name = member->GetName();
        BOOL isFileMember = (name=="cache" || name == "file recorder");
        output << hdr << "Title: " << hex << member->GetTitle() << "\n";
        if (isFileMember) output << " (file object)" << "\n";
        output << hdr << "Name: " << name << "\n"
               << hdr << "Outgoing video mixer: " << member->GetVideoMixerNumber() << "\n"
               << hdr << "Duration: " << (PTime() - member->GetStartTime()) << "\n"
               << member->GetMonitorInfo(hdr);
        H323Connection_ConferenceMember * connMember = NULL;
        if(!isFileMember) connMember = dynamic_cast<H323Connection_ConferenceMember *>(member);
        if(connMember!=NULL)
        { output << hdr << "H323 Token: " << connMember->GetH323Token() << "\n";
          OpenMCUH323Connection * conn = (OpenMCUH323Connection *)FindConnectionWithoutLock(connMember->GetH323Token());
          output << hdr << "Connection: " << hex << conn << "\n";
        }
        if(isFileMember)
        {
          ConferenceFileMember * fileMember = dynamic_cast<ConferenceFileMember *>(member);
          if(fileMember!=NULL)
          {
            if(name != "cache") output << hdr << "Format: " << fileMember->GetFormat() << "\n";
            if(name != "file recorder")output << hdr << "VFormat: " << fileMember->GetVFormat() << "\n";
            output << hdr << "IsVisible: " << fileMember->IsVisible() << "\n";
            output << hdr << "Status: " << (fileMember->status?"Awake":"Sleeping") << "\n";
//#ifndef _WIN32
//            if(fileMember->codec) output << hdr << "EncoderSeqN: " << dec << fileMember->codec->GetEncoderSeqN() << "\n";
//#endif
          }
        }
        if(member->videoMixer!=NULL)
        { output << hdr << "Video Mixer ID: " << member->videoMixer << "\n";
          int n=member->videoMixer->GetPositionSet();
          output << hdr << "Video Mixer Layout ID: " << OpenMCU::vmcfg.vmconf[n].splitcfg.Id << "\n"
            << hdr << "Video Mixer Layout capacity: " << OpenMCU::vmcfg.vmconf[n].splitcfg.vidnum << "\n";
          MCUVideoMixer::VideoMixPosition *r=member->videoMixer->vmpList->next;
          while(r!=NULL)
          { output << hdr << "[Position " << r->n << "]\n"
              << hdr << "  Conference Member Id: " << r->id << "\n"
              << hdr << "  Position type: " << r->type << "\n"
              << hdr << "  Position status: " << r->status << "\n";
#if USE_FREETYPE
            output << hdr << "  Frame counter: " << std::dec << r->fc << "\n";
#endif
           r=r->next;
          }
        }
      }
    }

    Conference::VideoMixerRecord * vmr = conference.videoMixerList;
    while (vmr!=NULL)
    { output << "[Mixer " << vmr->id << "]\n";
      MCUSimpleVideoMixer * mixer = (MCUSimpleVideoMixer*) vmr->mixer;
      int n=mixer->GetPositionSet();
      output << "  Layout ID: "       << OpenMCU::vmcfg.vmconf[n].splitcfg.Id << "\n"
             << "  Layout capacity: " << OpenMCU::vmcfg.vmconf[n].splitcfg.vidnum << "\n"
             << mixer->GetFrameStoreMonitorList();
      
      MCUVideoMixer::VideoMixPosition *r=mixer->vmpList->next;
      while(r!=NULL)
      {
        output << "  [Position " << r->n << "]\n"
               << "    Conference Member Id: " << r->id << "\n"
               << "    Position type: " << r->type << "\n"
               << "    Position status: " << r->status << "\n";
#if USE_FREETYPE
        output << "    Frame counter: " << std::dec << r->fc << "\n";
#endif
        r=r->next;
      }
      vmr=vmr->next;
    }

  }

  return output;
}

Conference * OpenMCUH323EndPoint::MakeConference(const PString & room)
{
  // create/find the conference
  Conference * c = conferenceManager.MakeAndLockConference(room);
  conferenceManager.UnlockConference();
  return c;
}

PString OpenMCUH323EndPoint::IncomingConferenceRequest(H323Connection & connection, 
                                                  const H323SignalPDU & setupPDU,
                                                  unsigned & videoMixerNumber)
{
  const H225_Setup_UUIE & setup = setupPDU.m_h323_uu_pdu.m_h323_message_body;

  /*
   Here is the algorithm used for joining a conference.

   - If the conference goal is e_invite, then refuse the call

   - if the conference ID matches the ID of an existing conference, then join that conference

   - If there is no destination address, join the default conference

   - If there is a destination address, try and match the destination address
     to a conference number. If there is a match, join to that conference

   - If the destination address does not match any conference, create a new conference
  */

  // get the conference ID from the incoming call
  OpalGloballyUniqueID conferenceID = setup.m_conferenceID;

  PString roomToJoin;

  // check the conference ID
  if (conferenceManager.HasConference(conferenceID, roomToJoin)) {
    PTRACE(3, "MCU\tUsing conference ID to join existing room " << roomToJoin);
    videoMixerNumber = 0;
    return roomToJoin;
  }

  // look at destination addresses
  PINDEX i;
  for (i = 0; (i < setup.m_destinationAddress.GetSize()); i++) {
    roomToJoin = H323GetAliasAddressString(setup.m_destinationAddress[i]);
    // allow calls to room/VIDEOMIXER@openmcu
    { PINDEX slashPos = roomToJoin.Find('/');
      if(slashPos != P_MAX_INDEX)
      { PString mixerStr = roomToJoin.Right(roomToJoin.GetLength()-slashPos-1);
        PINDEX mixerStrLen = mixerStr.GetLength();
        if(mixerStrLen > 0 && mixerStrLen < 3)
        { PINDEX mixer = mixerStr.AsInteger();
          if(mixer > 0 && mixer < 100)
          { roomToJoin=roomToJoin.Left(slashPos);
            videoMixerNumber = mixer;
    } } } }
    if (conferenceManager.HasConference(roomToJoin)) {
      PTRACE(3, "MCU\tJoining room specified by destination address " << roomToJoin);
      return roomToJoin;
    }
  }

  // look at Q931 called party number
//  if (roomToJoin.IsEmpty() && !setupPDU.GetQ931().GetCalledPartyNumber(roomToJoin) && roomToJoin.IsEmpty()) {
  if (roomToJoin.IsEmpty())
  if (setupPDU.GetQ931().GetCalledPartyNumber(roomToJoin))
  if (!roomToJoin.IsEmpty())
  {
    // allow calls to room/VIDEOMIXER@openmcu
    { PINDEX slashPos = roomToJoin.Find('/');
      if(slashPos != P_MAX_INDEX)
      { PString mixerStr = roomToJoin.Right(roomToJoin.GetLength()-slashPos-1);
        PINDEX mixerStrLen = mixerStr.GetLength();
        if(mixerStrLen > 0 && mixerStrLen < 3)
        { PINDEX mixer = mixerStr.AsInteger();
          if(mixer > 0 && mixer < 100)
          { roomToJoin=roomToJoin.Left(slashPos);
            videoMixerNumber = mixer;
    } } } }
    if (conferenceManager.HasConference(roomToJoin)) {
      PTRACE(3, "MCU\tJoining room specified by Q.931 called party " << roomToJoin);
      return roomToJoin;
    }
  }

  // if there is a room to create, then join this call to that conference
  if (roomToJoin.IsEmpty()) 
    roomToJoin = OpenMCU::Current().GetDefaultRoomName();

  if (!roomToJoin.IsEmpty()) {
    PTRACE(3, "MCU\tJoining default room " << roomToJoin);
    videoMixerNumber = 0;
    return roomToJoin;
  }

  PTRACE(3, "MCU\tRefusing call because no room specified, and no default room");
  return PString::Empty();
}


void OpenMCUH323EndPoint::OnIncomingSipConnection(PString &callToken, H323Connection &connection)
{
 connectionsMutex.Wait();
  PTRACE(3, "MCU\tSip connection");
 connectionsActive.SetAt(callToken, &connection);
 connectionsMutex.Signal();
}


///////////////////////////////////////////////////////////////

NotifyH245Thread::NotifyH245Thread(Conference & conference, BOOL _join, ConferenceMember * _memberToIgnore)
  : PThread(10000, AutoDeleteThread), join(_join), memberToIgnore(_memberToIgnore)
{ 
  mcuNumber  = conference.GetMCUNumber();
  terminalIdToSend = memberToIgnore->GetTerminalNumber();

  // create list of connections to notify
  Conference::MemberList::const_iterator r;
  for (r = conference.GetMemberList().begin(); r != conference.GetMemberList().end(); r++) {
    ConferenceMember * mbr = r->second;
    PString memberName=mbr->GetName();
    if ((memberName!="cache")&&(memberName!="file recorder"))
    if (mbr != memberToIgnore) {
      H323Connection_ConferenceMember * h323Mbr = dynamic_cast<H323Connection_ConferenceMember *>(mbr);
      if (h323Mbr != NULL)
        tokens += h323Mbr->GetH323Token();
    }
  }

  Resume(); 
}



void NotifyH245Thread::Main()
{
  OpenMCUH323EndPoint & ep = OpenMCU::Current().GetEndpoint();

  // send H.245 message on each connection in turn
  PINDEX i;
  for (i = 0; i < tokens.GetSize(); ++i) {
    H323Connection * conn = ep.FindConnectionWithLock(tokens[i]);
    if (conn != NULL) {
      OpenMCUH323Connection * h323Conn = dynamic_cast<OpenMCUH323Connection *>(conn);

      H323ControlPDU pdu;
      H245_ConferenceIndication & ind = pdu.Build(H245_IndicationMessage::e_conferenceIndication);
      ind.SetTag(join ? H245_ConferenceIndication::e_terminalJoinedConference : H245_ConferenceIndication::e_terminalLeftConference);
      H245_TerminalLabel & terminalId = ind;
      terminalId.m_mcuNumber      = mcuNumber;
      terminalId.m_terminalNumber = terminalIdToSend;

      h323Conn->WriteControlPDU(pdu);

      h323Conn->Unlock();
    }
  }
}

///////////////////////////////////////////////////////////////

OpenMCUH323Connection::OpenMCUH323Connection(OpenMCUH323EndPoint & _ep, unsigned callReference, void * userData)
  : H323Connection(_ep, callReference), ep(_ep), isMCU(FALSE)
{
  conference       = NULL;
  conferenceMember = NULL;
  welcomeState     = NotStartedYet;

#if OPENMCU_VIDEO
  videoMixerNumber = 0;
#endif

  if (userData != NULL) {
    requestedRoom    = *(PString *)userData;
    PINDEX slashPos = requestedRoom.Find("/");
    if(slashPos!=P_MAX_INDEX)
    {
#if OPENMCU_VIDEO
      videoMixerNumber=requestedRoom.Mid(slashPos+1,P_MAX_INDEX).Trim().AsInteger();
#endif
      requestedRoom=requestedRoom.Left(slashPos);
    }
    delete (PString *)userData;
  }

  audioReceiveCodecName = audioTransmitCodecName = "none";

#if OPENMCU_VIDEO
  videoGrabber = NULL;
  videoDisplay = NULL;
  videoReceiveCodecName = videoTransmitCodecName = "none";
  videoReceiveCodec = NULL;
  videoTransmitCodec = NULL;
#endif
}

OpenMCUH323Connection::~OpenMCUH323Connection()
{
 PThread::Sleep(500);
 connMutex.Wait();
}

void OpenMCUH323Connection::SetupCacheConnection(PString & format, Conference * conf, ConferenceMember * memb)
{
 remoteName = format;
 remotePartyName = format;
 conference = conf;
 conferenceIdentifier = conference->GetID();
 conferenceMember = (H323Connection_ConferenceMember *)memb; // fix it
 requestedRoom = conference->GetNumber();
}

BOOL OpenMCUH323Connection::OnReceivedCapabilitySet(const H323Capabilities & remoteCaps, const H245_MultiplexCapability * muxCap, H245_TerminalCapabilitySetReject & rejectPDU)
{
  PString prefAudioCap = GetEndpointParam("Audio codec(transmit)");
  PString prefVideoCap = GetEndpointParam("Video codec(transmit)");
  if(prefAudioCap != "") { PTRACE(1, "OpenMCUH323Connection\tSet endpoint custom transmit audio: " << prefAudioCap); }
  if(prefVideoCap != "") { PTRACE(1, "OpenMCUH323Connection\tSet endpoint custom transmit video: " << prefVideoCap); }

  BOOL prefVideoCodecAgreed = FALSE;
  unsigned bandwidthFrom = 0;

  H323Capabilities _remoteCaps;
  for(PINDEX i = 0; i < remoteCaps.GetSize(); i++)
  {
    PString capName = remoteCaps[i].GetFormatName();
    if(remoteCaps[i].GetMainType() == H323Capability::e_Video)
    {
      if(prefVideoCap == "")
      {
        _remoteCaps.Copy(remoteCaps[i]);
        //_remoteCaps.SetCapability(0, remoteCaps[i].GetMainType(), (H323Capability*)remoteCaps[i].Clone());
      } else {
        if(bandwidthFrom == 0)
          bandwidthFrom = remoteCaps[i].GetMediaFormat().GetOptionInteger("Max Bit Rate");
        if(prefVideoCap.Left(4) == capName.Left(4))
          prefVideoCodecAgreed = TRUE;
      }
    }
    else if(remoteCaps[i].GetMainType() == H323Capability::e_Audio)
    {
      if(prefAudioCap == "" || (prefAudioCap != "" && capName == prefAudioCap))
        _remoteCaps.Copy(remoteCaps[i]);
        //_remoteCaps.SetCapability(0, 0, (H323Capability*)remoteCaps[i].Clone());
    }
    else
    {
      _remoteCaps.Copy(remoteCaps[i]);
      //_remoteCaps.SetCapability(0, remoteCaps[i].GetMainType(), (H323Capability*)remoteCaps[i].Clone());
    }
  }
  // replace video capability with default parameters
  if(prefVideoCap != "" && prefVideoCodecAgreed)
  {
    H323Capability *newCap = H323Capability::Create(prefVideoCap);
    if(newCap)
    {
      OpalMediaFormat & wf = newCap->GetWritableMediaFormat();
      if(prefVideoCap.Find("H.264") != P_MAX_INDEX)
      {
        unsigned h264BaseLevel = wf.GetOptionInteger("Generic Parameter 42");
        unsigned h264MaxFs = 0;
        if(h264BaseLevel == 15) h264MaxFs = 99;
        else if(h264BaseLevel == 22) h264MaxFs = 396;
        else if(h264BaseLevel == 29) h264MaxFs = 396;
        else if(h264BaseLevel == 36) h264MaxFs = 396;
        else if(h264BaseLevel == 43) h264MaxFs = 396;
        else if(h264BaseLevel == 50) h264MaxFs = 792;
        else if(h264BaseLevel == 57) h264MaxFs = 1620;
        else if(h264BaseLevel == 64) h264MaxFs = 1620;
        else if(h264BaseLevel == 71) h264MaxFs = 3600;
        else if(h264BaseLevel == 78) h264MaxFs = 5120;
        else if(h264BaseLevel == 85) h264MaxFs = 8192;
        else if(h264BaseLevel == 92) h264MaxFs = 8192;
        else if(h264BaseLevel == 99) h264MaxFs = 8704;
        else if(h264BaseLevel == 106) h264MaxFs = 22080;
        else if(h264BaseLevel == 113) h264MaxFs = 36864;
        if(h264MaxFs != 0) wf.SetOptionInteger("Generic Parameter 4", (h264MaxFs/256)+1);
      }
      wf.SetOptionInteger("Max Bit Rate", bandwidthFrom);
      _remoteCaps.Add(newCap);
      //_remoteCaps.SetCapability(0, 1, newCap);
    }
  }
  //cout << "OnReceivedCapabilitySet\n" << remoteCaps << "\n";
  //cout << "OnReceivedCapabilitySet\n" << _remoteCaps << "\n";

  return H323Connection::OnReceivedCapabilitySet(_remoteCaps, muxCap, rejectPDU);
}

void OpenMCUH323Connection::OnSendCapabilitySet(H245_TerminalCapabilitySet & pdu)
{
  H323Connection::OnSendCapabilitySet(pdu);
  //cout << "OnSendCapabilitySet\n" << pdu << "\n";
}

BOOL OpenMCUH323Connection::StartControlNegotiations(BOOL renegotiate)
{
  // set endpoint capability
  PString prefAudioCap = GetEndpointParam("Audio codec(receive)");
  PString prefVideoCap = GetEndpointParam("Video codec(receive)");
  unsigned bandwidthTo = atoi(GetEndpointParam("Preferred bandwidth to MCU"));
  if(prefAudioCap != "") { PTRACE(1, "OpenMCUH323Connection\tSet endpoint custom receive audio: " << prefAudioCap); }
  if(prefVideoCap != "") { PTRACE(1, "OpenMCUH323Connection\tSet endpoint custom receive video: " << prefVideoCap); }
  if(bandwidthTo != 0) { PTRACE(1, "OpenMCUH323Connection\tSet endpoint bandwidth to mcu: " << bandwidthTo); }

  if(prefAudioCap.ToLower().Find("ulaw") != P_MAX_INDEX || prefAudioCap.ToLower().Find("alaw") != P_MAX_INDEX)
    prefAudioCap = prefAudioCap.Left(prefAudioCap.GetLength()-4);

  for(PINDEX i = 0; i < localCapabilities.GetSize(); )
  {
    PString capName = localCapabilities[i].GetFormatName();
    if(localCapabilities[i].GetMainType() == H323Capability::e_Audio && prefAudioCap != "" && capName != prefAudioCap)
    { localCapabilities.Remove(&localCapabilities[i]); continue; }
    if(localCapabilities[i].GetMainType() == H323Capability::e_Video && prefVideoCap != "" && capName != prefVideoCap)
    { localCapabilities.Remove(&localCapabilities[i]); continue; }
    if(localCapabilities[i].GetMainType() == H323Capability::e_Video)
    {
      if(bandwidthTo != 0)
      {
        if(bandwidthTo < 64) bandwidthTo = 64;
        if(bandwidthTo > 4000) bandwidthTo = 4000;
        localCapabilities[i].GetWritableMediaFormat().SetOptionInteger("Max Bit Rate", bandwidthTo*1000);
      }
    }
    i++;
  }
  //cout << "StartControlNegotiations\n" << localCapabilities << "\n";
  return H323Connection::StartControlNegotiations(renegotiate);
}
BOOL OpenMCUH323Connection::OnReceivedSignalSetup(const H323SignalPDU & setupPDU)
{
  const H225_Setup_UUIE & setup = setupPDU.m_h323_uu_pdu.m_h323_message_body;
  isMCU = setup.m_sourceInfo.m_mc;

  BOOL ret = H323Connection::OnReceivedSignalSetup(setupPDU);
  SetRemoteName(setupPDU);
  return ret;
}

BOOL OpenMCUH323Connection::OnReceivedCallProceeding(const H323SignalPDU & proceedingPDU)
{
  const H225_CallProceeding_UUIE & proceeding = proceedingPDU.m_h323_uu_pdu.m_h323_message_body;
  isMCU = proceeding.m_destinationInfo.m_mc;

  BOOL ret = H323Connection::OnReceivedCallProceeding(proceedingPDU);
  SetRemoteName(proceedingPDU);
  return ret;
}

BOOL OpenMCUH323Connection::OnReceivedSignalConnect(const H323SignalPDU & pdu)
{
  BOOL ret = H323Connection::OnReceivedSignalConnect(pdu);
  SetRemoteName(pdu);
  return ret;
}

class TplCleanCheckThread : public PThread
{
  public:
    TplCleanCheckThread(Conference * _c, const PString & _n, const PString & _a)
      : PThread(10000, AutoDeleteThread), c(_c), n(_n), a(_a)
    {
      Resume();
    }
    void Main()
    {
      PThread::Sleep(3333); // previous connection may be still actvie
      if(c!=NULL) c->OnConnectionClean(n, a);
    }
  protected:
    Conference * c;
    PString n, a;
};

void OpenMCUH323Connection::CleanUpOnCallEnd()
{
  PTRACE(1, "OpenMCUH323Connection\tCleanUpOnCallEnd");

  new TplCleanCheckThread(conference, remotePartyName, remotePartyAddress);

  videoReceiveCodecName = videoTransmitCodecName = "none";
  videoReceiveCodec = NULL;
  videoTransmitCodec = NULL;

  LeaveConference();
  H323Connection::CleanUpOnCallEnd();
}

H323Connection::AnswerCallResponse OpenMCUH323Connection::OnAnswerCall(const PString & /*caller*/,
                                                                  const H323SignalPDU & setupPDU,
                                                                  H323SignalPDU & /*connectPDU*/)
{
  requestedRoom = ep.IncomingConferenceRequest(*this, setupPDU, videoMixerNumber);

  if (requestedRoom.IsEmpty())
    return AnswerCallDenied;

  return AnswerCallNow;
}

void OpenMCUH323Connection::OnEstablished()
{
  H323Connection::OnEstablished();
}

class MemberDeleteThread : public PThread
{
  public:
    MemberDeleteThread(OpenMCUH323EndPoint * _ep, Conference * _conf, ConferenceMember * _cm)
      : PThread(10000, AutoDeleteThread), ep(_ep), conf(_conf), cm(_cm)
    {
      Resume();
    }

    void Main()
    {
      cm->WaitForClose();

      PString name=cm->GetName();
      BOOL removeConf=FALSE;

      if (conf->RemoveMember(cm))
      {
        if (conf->autoDelete
#       if ENABLE_TEST_ROOMS
          || (conf->GetNumber().Left(8) == "testroom")
#       endif
#       if ENABLE_ECHO_MIXER
          || (conf->GetNumber().Left(4) *= "echo")
#       endif
        )
        {
          ep->GetConferenceManager().RemoveConference(conf->GetID());
          removeConf=TRUE;
        }
      }
      if(!removeConf) new TplCleanCheckThread(conf, name, ""); //???
      delete cm;
    }

  protected:
    OpenMCUH323EndPoint * ep;
    Conference * conf;
    ConferenceMember * cm;
};

void OpenMCUH323Connection::JoinConference(const PString & roomToJoin)
{
  PWaitAndSignal m(connMutex);

  if (conference != NULL)
    return;

  BOOL joinSuccess = FALSE;

  if (!roomToJoin.IsEmpty()) {
    // create or join the conference
    ConferenceManager & manager = ((OpenMCUH323EndPoint &)ep).GetConferenceManager();
    Conference * newConf = manager.MakeAndLockConference(roomToJoin);
    if (newConf != NULL) {
      conference = newConf;
      conferenceIdentifier = conference->GetID();

      if(videoTransmitCodec!=NULL)
//       videoTransmitCodec->encoderCacheKey = ((long)conference&0xFFFFFF00)|(videoTransmitCodec->encoderCacheKey&0x000000FF);
       videoTransmitCodec->encoderCacheKey = ((long)(conference->videoMixerList)<<8)|(videoTransmitCodec->encoderCacheKey&0x000000FF);
      conferenceMember = new H323Connection_ConferenceMember(conference, ep, GetCallToken(), this, isMCU);

      if (!conferenceMember->IsJoined())
      { PTRACE(1, "MCU\tMember connection refused"); }
      else
        joinSuccess = TRUE;

      manager.UnlockConference();

      if(!joinSuccess) {
        new MemberDeleteThread(&ep, conference, conferenceMember);
        conferenceMember = NULL;
        conference = NULL;
      }
    }
  }

  if(!joinSuccess)
    ChangeWelcomeState(JoinFailed);
}

void OpenMCUH323Connection::LeaveConference()
{
  PWaitAndSignal m(connMutex);

  if (conference != NULL && conferenceMember != NULL) {
    LogCall();

    new MemberDeleteThread(&ep, conference, conferenceMember);
    conferenceMember = NULL;
    conference = NULL;

    // - called from another thread than usual
    // - may clear the call immediately
    ChangeWelcomeState(ConferenceEnded);
  }
}

BOOL OpenMCUH323Connection::OpenAudioChannel(BOOL isEncoding, unsigned /* bufferSize */, H323AudioCodec & codec)
{
  PWaitAndSignal m(connMutex);

  unsigned codecChannels = 1;
  {
    PString OptionValue;
    if(isEncoding)
    {
      if(codec.GetMediaFormat().GetOptionValue((const PString)"Encoder Channels", OptionValue))
        codecChannels = atoi(OptionValue);
    }
    else
    {
      if(codec.GetMediaFormat().GetOptionValue((const PString)"Decoder Channels", OptionValue))
        codecChannels = atoi(OptionValue);
    }
  }

  unsigned codecSampleRate = codec.GetSampleRate();

  if(codecSampleRate == 0) codecSampleRate = 8000; // built-in g711

  if(codecSampleRate > 1000000) // calculated as: [audible 20KHz]*[perfect&unreachable 50 counts per sine period]
  {
    PTRACE(1,"OpenAudioChannel\tError: codec sample rate " << codecSampleRate << " Hz looks too big");
    if (isEncoding) audioTransmitCodecName = "Error@" + PString(codecSampleRate) + "Hz/" + PString(codecChannels);
    else            audioReceiveCodecName  = "Error@" + PString(codecSampleRate) + "Hz/" + PString(codecChannels);
    return FALSE;
  }

  if(codecChannels < 1 || codecChannels > 8) // supported layouts from mono up to 7+1 (ffmpeg limitations)
  {
    PTRACE(1,"OpenAudioChannel\tError: codec channels " << codecChannels << " out of range 1-8");
    if (isEncoding) audioTransmitCodecName = "Error@" + PString(codecSampleRate) + "Hz/" + PString(codecChannels);
    else            audioReceiveCodecName  = "Error@" + PString(codecSampleRate) + "Hz/" + PString(codecChannels);
    return FALSE;
  }

  PString codecName = codec.GetMediaFormat() + "@" + PString(codecSampleRate) + "Hz";
  if(codecChannels==2) codecName+="/stereo";
  else if(codecChannels>2) codecName+="/"+PString(codecChannels)+"channels";

  codec.SetSilenceDetectionMode( H323AudioCodec::NoSilenceDetection );

  if (!isEncoding) {
    audioReceiveCodecName = codecName;
    codec.AttachChannel(new IncomingAudio(ep, *this, codecSampleRate, codecChannels), TRUE);
  } else {
    audioTransmitCodecName = codecName;
    codec.AttachChannel(new OutgoingAudio(ep, *this, codecSampleRate, codecChannels), TRUE);
  }

  return TRUE;
}

void OpenMCUH323Connection::OpenVideoCache(H323VideoCodec & srcCodec)
{
  Conference *conf = conference;
     //  const H323Capabilities &caps = ep.GetCapabilities();
  if(conf == NULL) 
  { // creating conference if needed
    OpenMCUH323EndPoint & ep = OpenMCU::Current().GetEndpoint();
    ConferenceManager & manager = ((OpenMCUH323EndPoint &)ep).GetConferenceManager();
    conf = manager.MakeAndLockConference(requestedRoom);
    manager.UnlockConference();
  }
// starting new cache thread

  unsigned videoMixerNumber=0;
  PINDEX slashPos=srcCodec.formatString.Find("/");
  if(slashPos!=P_MAX_INDEX) videoMixerNumber=atoi(srcCodec.formatString.Mid(slashPos+1,P_MAX_INDEX));

  PTRACE(2,"MCU\tOpenVideoCache(" << srcCodec.formatString << ")");

  new ConferenceFileMember(conf, srcCodec.GetMediaFormat(), PFile::WriteOnly, videoMixerNumber); 
}


void OpenMCUH323Connection::SetEndpointDefaultVideoParams()
{
  H323VideoCodec *codec = GetVideoTransmitCodec();
  if(codec == NULL) return;
  OpalMediaFormat & mf = codec->GetWritableMediaFormat();

  // default & video parameters
  unsigned fr = ep.GetVideoFrameRate();
  if(fr < 1 || fr > MAX_FRAME_RATE) fr = DefaultVideoFrameRate;
  codec->SetTargetFrameTimeMs(1000/fr);
  mf.SetOptionInteger("Frame Rate", fr);
  mf.SetOptionInteger("Frame Time", 90000/fr);

  mf.SetOptionInteger("Encoding Quality", DefaultVideoQuality);

  PStringList keys = MCUConfig("Video").GetKeys();
  for(PINDEX i = 0; i < keys.GetSize(); i++)
  {
    if(keys[i].Tokenise(" ")[0] == videoTransmitCodecName.Tokenise("-")[0])
    {
      PINDEX pos = keys[i].Find(" ");
      if(pos == P_MAX_INDEX)
        continue;
      PString option = keys[i].Right(keys[i].GetSize()-pos-2);
      int value = MCUConfig("Video").GetInteger(keys[i], 0);
      if(option == "Max Bit Rate")
      {
        value = value*1000;
        if(value == 0 || value > mf.GetOptionInteger(option))
          continue;
        if(value < 64000)
          value = 64000;
      }
      mf.SetOptionInteger(option, value);
    }
  }
}

void OpenMCUH323Connection::SetEndpointPrefVideoParams()
{
  if(GetRemotePartyAddress() == "") return;
  H323VideoCodec *codec = GetVideoTransmitCodec();
  if(codec == NULL) return;
  OpalMediaFormat & mf = codec->GetWritableMediaFormat();

  unsigned fr = atoi(GetEndpointParam("Preferred frame rate from MCU"));
  if(fr != 0)
  {
    if(fr > MAX_FRAME_RATE) fr = MAX_FRAME_RATE;
    codec->SetTargetFrameTimeMs(1000/fr);
    mf.SetOptionInteger("Frame Rate", fr);
    mf.SetOptionInteger("Frame Time", 90000/fr);
  }

  unsigned bwFrom = atoi(GetEndpointParam("Preferred bandwidth from MCU"));
  if(bwFrom != 0)
  {
    if(bwFrom < 64) bwFrom = 64;
    if(bwFrom > 4000) bwFrom = 4000;
    mf.SetOptionInteger("Max Bit Rate", bwFrom*1000);
  }
}

PString OpenMCUH323Connection::GetEndpointParam(PString param)
{
  PString url = remotePartyAddress;
  if(remotePartyAddress.Left(4) != "sip:")
  {
    PINDEX pos = url.Find("ip$");
    if(pos != P_MAX_INDEX) url=url.Mid(pos+3);
    url = GetRemoteNumber()+"@"+url;
  }
  return OpenMCU::Current().GetEndpointParamFromUrl(param, url);
}

#if OPENMCU_VIDEO
BOOL OpenMCUH323Connection::OpenVideoChannel(BOOL isEncoding, H323VideoCodec & codec)
{
  ConferenceMember * conferenceMember = GetConferenceMember();
  if(conferenceMember!=NULL)
  {
    if(conferenceMember->IsVisible())
    {
      videoMixerNumber=conferenceMember->GetVideoMixerNumber();
    }
  }

  PWaitAndSignal m(connMutex);

  if (isEncoding)
  {
    videoTransmitCodec = &codec;
    videoTransmitCodecName = codec.GetMediaFormat();

    PVideoChannel * channel = new PVideoChannel;

    if (videoGrabber!=NULL)
    {
      channel->CloseVideoReader();
      codec.cacheMode=0;
    }

    videoGrabber = new PVideoInputDevice_OpenMCU(*this);
    if (videoGrabber == NULL) {
      PTRACE(3, "Cannot create MCU video input driver");
      return FALSE;
    }

    if(GetRemotePartyAddress() != "")
    {
      SetEndpointDefaultVideoParams();
      SetEndpointPrefVideoParams();
    }

    // get frame time from codec
    OpalMediaFormat & mf = codec.GetWritableMediaFormat();
    unsigned fr;
    if(mf.GetOptionInteger("Frame Time") != 0)
      fr = 90000/mf.GetOptionInteger("Frame Time");
    else
      fr = ep.GetVideoFrameRate();

    // update format string
    PString formatWH = codec.formatString.Left(codec.formatString.FindLast(":"));
    codec.formatString = formatWH+":"+PString(mf.GetOptionInteger(OpalVideoFormat::MaxBitRateOption))+"x";


    // SetTxQualityLevel not send the value in encoder
    //codec.SetTxQualityLevel(ep.GetVideoTxQuality());

    if(
      OpenMCU::Current().GetForceScreenSplit()
#if ENABLE_TEST_ROOMS
      && (requestedRoom.Left(8)!="testroom")
#endif
#if ENABLE_ECHO_MIXER
      && (!(requestedRoom.Left(4)*="echo"))
#endif
    )
    {
      PINDEX slashPos=codec.formatString.Find("/");
      if(slashPos==P_MAX_INDEX){
        codec.formatString+=(PString)fr + "_" + requestedRoom + "/" + (PString)videoMixerNumber;
      } else {
        videoMixerNumber=atoi(codec.formatString.Mid(slashPos+1,P_MAX_INDEX));
      }

      PTRACE(6,"MixerCtrl\tOpenVideoChannel codec.formatString=" << codec.formatString);

      videoTransmitCodecName = codec.formatString; // override previous definition

      if(GetRemoteApplication().Find("PCS-") != P_MAX_INDEX && codec.formatString.Find("H.264") != P_MAX_INDEX) 
        codec.cacheMode = 3;

      if(!codec.cacheMode) 
      {
        if(!codec.CheckCacheRTP()) 
        {
          OpenVideoCache(codec);
          while(!codec.CheckCacheRTP()) { PThread::Sleep(100); }
        }
        codec.AttachCacheRTP();
      }
    }
    else codec.cacheMode=1; // I WANTED TO SET 0 HERE, REALLY. BUT IT FAILS :( h323pluginmgr.cxx does not support "no cache" mode

    if (!InitGrabber(videoGrabber, codec.GetWidth(), codec.GetHeight(), fr)) {
      delete videoGrabber;
      videoGrabber = NULL;
      return FALSE;
    }

    videoGrabber->Start();
    channel->AttachVideoReader(videoGrabber);

    if (!codec.AttachChannel(channel,TRUE))
      return FALSE;

   } else {


    videoReceiveCodec = &codec;
    if(conference && conference->IsModerated() == "+") conference->FreezeVideo(this);

    videoReceiveCodecName = codec.GetMediaFormat();

    videoDisplay = new PVideoOutputDevice_OpenMCU(*this);

    if (!videoDisplay->Open("")) {
      delete videoDisplay;
      return FALSE;
    }

    videoDisplay->SetFrameSize(codec.GetWidth(), codec.GetHeight()); // needed to enable resize
    videoDisplay->SetColourFormatConverter("YUV420P");

    PVideoChannel * channel = new PVideoChannel; 
    channel->AttachVideoPlayer(videoDisplay); 
    if (!codec.AttachChannel(channel,TRUE))
      return FALSE;
  }
 
  return TRUE;
}

void OpenMCUH323Connection::OnClosedLogicalChannel(const H323Channel & channel)
{
 H323Codec * codec = channel.GetCodec();
 if(codec == videoTransmitCodec) videoTransmitCodec = NULL; 
 if(codec == videoReceiveCodec) videoReceiveCodec = NULL;
}

void OpenMCUH323Connection::RestartGrabber() { videoGrabber->Restart(); }

BOOL OpenMCUH323Connection::InitGrabber(PVideoInputDevice * grabber, int newFrameWidth, int newFrameHeight, int newFrameRate)
{
  PTRACE(4, "Video grabber set to " << newFrameWidth << "x" << newFrameHeight);

  //if (!(pfdColourFormat.IsEmpty()))
  //  grabber->SetPreferredColourFormat(pfdColourFormat);

  if (!grabber->Open("", FALSE)) {
    PTRACE(3, "Failed to open the video input device");
    return FALSE;
  }

  //if (!grabber->SetChannel(ep.GetVideoPlayMode())) {
  //  PTRACE(3, "Failed to set channel to " << ep.GetVideoPlayMode());
  //  return FALSE;
  //}

  //if (!grabber->SetVideoFormat(
  //    ep.GetVideoIsPal() ? PVideoDevice::PAL : PVideoDevice::NTSC)) {
  //  PTRACE(3, "Failed to set format to " << (ep.GetVideoIsPal() ? "PAL" : "NTSC"));
  //  return FALSE;
  //}

  if (!grabber->SetColourFormatConverter("YUV420P") ) {
    PTRACE(3,"Failed to set format to yuv420p");
    return FALSE;
  }


  if (newFrameRate != 0) {
    if (!grabber->SetFrameRate(newFrameRate)) {
      PTRACE(3, "Failed to set framerate to " << newFrameRate);
      return FALSE;
    }
  }

  if (!grabber->SetFrameSizeConverter(newFrameWidth,newFrameHeight,FALSE)) {
    PTRACE(3, "Failed to set frame size to " << newFrameWidth << "x" << newFrameHeight);
    return FALSE;
  }

  return TRUE;
}

#endif

void OpenMCUH323Connection::OnUserInputString(const PString & str)
{
  PWaitAndSignal m(connMutex);

  if (conferenceMember == NULL)
    return;

  PString signalTypes = "1234567890*#ABCD";
  if(str.GetLength() == 1 && signalTypes.Find(str) != P_MAX_INDEX)
  {
    dtmfBuffer += str;
    if(str != "#") return;
  } else {
    dtmfBuffer = str;
  }

  cout << "Receive DTMF command: " << dtmfBuffer << "\n";
  PString dtmfTmp = dtmfBuffer;
  dtmfTmp.Replace("#","",TRUE,0);
  ConferenceMember *codeConferenceMember;
  PString code, codeAction, codeRoom, codeRoomName, codePos;
  PStringStream codeMsg;
  PStringArray codeArray = dtmfTmp.Tokenise("*");
  if(codeArray.GetSize() == 2)
  {
    code = codeArray[1];
  //} else if(codeArray.GetSize() == 3) {
    //codePos = codeArray[1];
    //code = codeArray[2];
  } else if(codeArray.GetSize() == 4) {
    codeRoom = codeArray[1];
    codePos = codeArray[2];
    code = codeArray[3];
  }
  if(code != "" && MCUConfig("Control Codes").HasKey(code))
  {
    PString name, text;
    PStringArray params = MCUConfig("Control Codes").GetString(code).Tokenise(",");
    if(params.GetSize() >= 2)
    {
      codeAction = params[0];
      if(params[1] != "") text = params[1];
      else text = params[0];
    }
    if(GetRemotePartyAddress().Left(4) == "sip:")
      name = GetRemotePartyName()+" ["+GetRemotePartyAddress()+"]";
    else
      name = GetRemotePartyName();
    codeMsg << "<font color=blue><b>" << name << "</b>: " << text;

    if(codeRoom == "")
    {
      codeConferenceMember = conferenceMember;
    } else {
      codeConferenceMember = NULL;
      codeRoomName = MCUConfig("Room Codes").GetString(codeRoom);
    }

    if(codeRoomName != "" && codePos != "")
    {
      {
        ep.GetConferenceManager().GetConferenceListMutex().Wait();
        ConferenceListType & conferenceList = ep.GetConferenceManager().GetConferenceList();
        for(ConferenceListType::iterator r = conferenceList.begin(); r != conferenceList.end(); ++r)
        {
          Conference *conference = r->second;
          if(conference->GetNumber() == codeRoomName)
          {
            Conference::MemberList & memberList = conference->GetMemberList();
            for(Conference::MemberList::const_iterator t = memberList.begin(); t != memberList.end(); ++t)
            {
              ConferenceMember *member = t->second;
              if(member->GetName() == "file recorder" || member->GetName() == "cache") continue;
              MCUVideoMixer *mixer = conference->VMLFind(member->GetVideoMixerNumber());
              if(mixer == NULL) continue;
              int pos = mixer->GetPositionNum(member->GetID());
              if(pos < 0) continue;
              if(pos == atoi(codePos))
              {
                codeConferenceMember = member;
                codeMsg << "<br>-> action:"+codeAction+" room:"+codeRoomName+" pos:"+codePos+" found:" << member->GetName();
                break;
              }
            }
            break;
          }
        }
      }
      ep.GetConferenceManager().GetConferenceListMutex().Signal();
    }

    if(codeConferenceMember != NULL)
    {
      if(codeAction == "close")
        codeConferenceMember->Close();
      else if(codeAction == "mute")
        codeConferenceMember->muteIncoming = TRUE;
      else if(codeAction == "unmute")
        codeConferenceMember->muteIncoming = FALSE;
      codeMsg << "</font>";
      OpenMCU::Current().HttpWriteEvent(codeMsg);
      //OpenMCU::Current().HttpWriteEventRoom(codeMsg, conference->GetNumber());
    }
  } else {
    // the old "H.245/User Input Indication/DTMF" by kay27
    conferenceMember->SendUserInputIndication(dtmfBuffer);
  }
  dtmfBuffer = "";
}

BOOL OpenMCUH323Connection::OnIncomingAudio(const void * buffer, PINDEX amount, unsigned sampleRate, unsigned channels)
{
  PWaitAndSignal m(connMutex);
/*
  // If record file is open, write data to it
  if (recordFile.IsOpen()) {
    recordFile.Write(buffer, amount);

    recordDuration += amount / 2;
    if (recordDuration > recordLimit) {
      recordFile.Close();
      OnFinishRecording();
    }
    else {
      const WORD * samples = (const WORD *)buffer;
      PINDEX sampleCount = amount / 2;
      BOOL silence = TRUE;
      while (sampleCount-- > 0 && silence) {
        if (*samples > 100 || *samples < -100)
          silence = FALSE;
        ++samples;
      }
      if (!silence)
        recordSilenceCount = 0;
      else {
        recordSilenceCount += amount / 2;
        if ((recordSilenceThreshold > 0) && (recordSilenceCount >= recordSilenceThreshold)) {
          recordFile.Close();
          OnFinishRecording();
        }
      }
    }
  }

  else */ if (conferenceMember != NULL)
    conferenceMember->WriteAudio(buffer, amount, sampleRate, channels);

  return TRUE;
}

/*
void OpenMCUH323Connection::StartRecording(const PFilePath & filename, unsigned limit, unsigned threshold)
{
  if (!recordFile.Open(filename, PFile::ReadWrite, PFile::Create | PFile::Truncate))
    return;

  recordSilenceCount = 0;
  recordDuration = 0;

  recordSilenceThreshold = threshold * 8000;
  recordLimit            = limit * 8000;
}

void OpenMCUH323Connection::OnFinishRecording()
{
}
*/

BOOL OpenMCUH323Connection::OnOutgoingAudio(void * buffer, PINDEX amount, unsigned sampleRate, unsigned channels)
{
  // When the prodedure begins, play the welcome file
  if (welcomeState == NotStartedYet) {
    ChangeWelcomeState(PlayingWelcome);
  }

  for (;;) {
    // Do actions that are not triggered by events
    OnWelcomeProcessing();

    // If a wave is not playing, we may continue now
    if (!playFile.IsOpen())
      break;

    // Wait for wave file completion
    if (playFile.Read(buffer, amount)) {
      int len = playFile.GetLastReadCount();
      if (len < amount) {
        memset(((BYTE *)buffer)+len, 0, amount-len);
      }
      //playDelay.Delay(amount/16);

      // Exit now since the buffer is ready
      return TRUE;
    }

    PTRACE(4, "MCU\tFinished playing file");
    playFile.Close();

    // Wave completed, if no event should be fired
    //  then we may continue now
    if(!wavePlayingInSameState)
      break;

    // Fire wave completion event
    OnWelcomeWaveEnded();

    // We should repeat the loop now because the callback
    //  above might have started a new wave file
  }

  PWaitAndSignal m(connMutex);

  // If a we are connected to a conference and no wave
  //  is playing, read data from the conference
  if (conferenceMember != NULL) {
    conferenceMember->ReadAudio(buffer, amount, sampleRate, channels);
    return TRUE;
  }

  // Generate silence
  return FALSE;
}

///////////////////////////////////////////////////////////////////////////////////////

void OpenMCUH323Connection::ChangeWelcomeState(int newState)
{
  PWaitAndSignal m(connMutex);

  if(welcomeState != newState)
  {
    PTRACE(4, "MCU\tEntering welcome state " << newState);
    welcomeState = newState;
    wavePlayingInSameState = FALSE;
    OnWelcomeStateChanged();
  }
}

void OpenMCUH323Connection::PlayWelcomeFile(BOOL useTheFile, PFilePath & fileToPlay)
{
  playFile.Close();

  wavePlayingInSameState = TRUE;

  if(useTheFile) {
    if(playFile.Open(fileToPlay, PFile::ReadOnly))
    {
      PTRACE(4, "MCU\tPlaying welcome procedure file " << fileToPlay);
      return;
    }
    else
      PTRACE(3, "MCU\tFailed to play welcome procedure file " << fileToPlay);
   }

  // File not played, call the wave end callback anyway
  OnWelcomeWaveEnded();
}

void OpenMCUH323Connection::OnWelcomeStateChanged()
{
  PFilePath fn = OpenMCU::Current().connectingWAVFile;

  switch(welcomeState) {

    case PlayingWelcome:
      if(GetRemotePartyAddress().Left(4) != "sip:")
        JoinConference(requestedRoom);
      // Welcome file not implemented yet
      PlayWelcomeFile(FALSE, fn);
      break;

    case PlayingConnecting:
      PlayWelcomeFile(FALSE, fn);
      //PlayWelcomeFile(OpenMCU::Current().GetConnectingWAVFile(fn), fn);
      break;

    case CompleteConnection:
//        JoinConference(requestedRoom);
      break;

    case JoinFailed:
    case ConferenceEnded:
      // Goodbye file not implemented yet
      PlayWelcomeFile(FALSE, fn);
      break;

    default:
      // Do nothing
      break;
  }
}

void OpenMCUH323Connection::OnWelcomeProcessing()
{
}

void OpenMCUH323Connection::OnWelcomeWaveEnded()
{
  switch(welcomeState) {

    case PlayingWelcome:
      ChangeWelcomeState(PlayingConnecting);
      break;

    case PlayingConnecting:
      ChangeWelcomeState(CompleteConnection);
      break;

    case JoinFailed:
    case ConferenceEnded:
      ClearCall();
      break;

    default:
      // Do nothing
      break;
  }
}

PString OpenMCUH323Connection::GetRemoteNumber()
{
  PString number;
  if(remotePartyAddress.Left(4) == "url:" && ep.IsRegisteredWithGatekeeper())
  {
    PURL url(remotePartyAddress.Right(remotePartyAddress.GetLength()-4), "h323");
    number = url.GetUserName();
  }
  if(number == "") number = remotePartyNumber;
  if(number == "")
  {
    if(remoteAliasNames.GetSize() == 1)
      number = remoteAliasNames[0];
    else if(remoteAliasNames.GetSize() > 1)
      number = remoteAliasNames[1];
  }
  return number;
}

void OpenMCUH323Connection::SetRemoteName(const H323SignalPDU & pdu)
{
  // endpoint display name override
  PString overrideName = GetEndpointParam("Display name override");
  if(overrideName != "")
  {
    PTRACE(1, "OpenMCUH323Connection\tSet endpoint display name override: " << overrideName);
    remoteName = overrideName;
    remotePartyName = overrideName;
  }

  if(overrideName == "")
  {
    // get a good name from the other end
    remoteName = pdu.GetQ931().GetDisplayName();
    if(remoteName.IsEmpty())
    {
      if(remoteAliasNames.GetSize() > 0)
        remoteName = remoteAliasNames[0];
    }
    if(remoteName.IsEmpty())
    {
      if(!pdu.GetQ931().GetCallingPartyNumber(remoteName))
        remoteName.MakeEmpty();
    }
    if(remoteName.IsEmpty())
      remoteName = GetRemotePartyName();

    BOOL isMyPhone    = (P_MAX_INDEX != remoteApplication.Find("MyPhone"));
    BOOL isPolycomPVX = (P_MAX_INDEX != remoteApplication.Find("Polycom ViaVideo\tRelease 8.0"));

    if(isMyPhone || isPolycomPVX)
    { // Fast bad fix of presence of redundant aliases in remote party name on incoming direction
      PINDEX brPos = remotePartyName.Find(" (");
      if(brPos != P_MAX_INDEX)
      {
        PINDEX br2Pos = remotePartyName.FindLast(")", P_MAX_INDEX);
        if((br2Pos != P_MAX_INDEX) && (br2Pos > brPos))
        {
          PString newName = remotePartyName.Left(brPos) + remotePartyName.Mid(br2Pos+1,P_MAX_INDEX);
          PTRACE(5,"H225\t(Aliases) removed from remote party name: " << remotePartyName << " -> " << newName);
          remotePartyName = newName;
        }
      }
    }

    if(isMyPhone || isPolycomPVX)
    { // good appliances supports UTF-8 by default :)
      static const int table[128] = { // cp1251 -> utf8 translation based on http://www.linux.org.ru/forum/development/3968525
        0x82D0,0x83D0,  0x9A80E2,0x93D1,  0x9E80E2,0xA680E2,0xA080E2,0xA180E2,0xAC82E2,0xB080E2,0x89D0,0xB980E2,0x8AD0,0x8CD0,0x8BD0,0x8FD0,
        0x92D1,0x9880E2,0x9980E2,0x9C80E2,0x9D80E2,0xA280E2,0x9380E2,0x9480E2,0,       0xA284E2,0x99D1,0xBA80E2,0x9AD1,0x9CD1,0x9BD1,0x9FD1,
        0xA0C2,0x8ED0,  0x9ED1,  0x88D0,  0xA4C2,  0x90D2,  0xA6C2,  0xA7C2,  0x81D0,  0xA9C2,  0x84D0,0xABC2,  0xACC2,0xADC2,0xAEC2,0x87D0,
        0xB0C2,0xB1C2,  0x86D0,  0x96D1,  0x91D2,  0xB5C2,  0xB6C2,  0xB7C2,  0x91D1,  0x9684E2,0x94D1,0xBBC2,  0x98D1,0x85D0,0x95D1,0x97D1,
        0x90D0,0x91D0,  0x92D0,  0x93D0,  0x94D0,  0x95D0,  0x96D0,  0x97D0,  0x98D0,  0x99D0,  0x9AD0,0x9BD0,  0x9CD0,0x9DD0,0x9ED0,0x9FD0,
        0xA0D0,0xA1D0,  0xA2D0,  0xA3D0,  0xA4D0,  0xA5D0,  0xA6D0,  0xA7D0,  0xA8D0,  0xA9D0,  0xAAD0,0xABD0,  0xACD0,0xADD0,0xAED0,0xAFD0,
        0xB0D0,0xB1D0,  0xB2D0,  0xB3D0,  0xB4D0,  0xB5D0,  0xB6D0,  0xB7D0,  0xB8D0,  0xB9D0,  0xBAD0,0xBBD0,  0xBCD0,0xBDD0,0xBED0,0xBFD0,
        0x80D1,0x81D1,  0x82D1,  0x83D1,  0x84D1,  0x85D1,  0x86D1,  0x87D1,  0x88D1,  0x89D1,  0x8AD1,0x8BD1,  0x8CD1,0x8DD1,0x8ED1,0x8FD1
      };
      PStringStream utfname;
      for(PINDEX i=0;i<remotePartyName.GetLength();i++){
        unsigned int charcode=(BYTE)remotePartyName[i];
        if(charcode&128){
          if((charcode=table[charcode&127])){
            utfname << (char)charcode << (char)(charcode >> 8);
            if(charcode >>= 16) utfname << (char)charcode;
          }
        } else utfname << (char)charcode;
      }
      if(utfname!=remotePartyName){
        remotePartyName = utfname;
        PTRACE(5, "H225\tremotePartyName converted (cp1251->utf8): " << remotePartyName);
      }

      utfname = "";
      for(PINDEX i=0;i<remoteName.GetLength();i++){
        unsigned int charcode=(BYTE)remoteName[i];
        if(charcode&128){
          if((charcode=table[charcode&127])){
            utfname << (char)charcode << (char)(charcode >> 8);
            if(charcode >>= 16) utfname << (char)charcode;
          }
        } else utfname << (char)charcode;
      }
      if(utfname!=remoteName){
        remoteName = utfname;
        PTRACE(5, "H225\tremoteName converted (cp1251->utf8): " << remoteName);
      }
    }
    if(remotePartyName.Find("[") == P_MAX_INDEX && remotePartyName.Find("]") == P_MAX_INDEX) // ???
      remoteName = remotePartyName;
  }
  SetMemberName();
}

void OpenMCUH323Connection::SetMemberName()
{
  PString address;
  if(remotePartyAddress.Left(4) == "sip:")
  {
    address = remotePartyAddress;
  } else {
    PString alias, host, port;
    if(remotePartyAddress.Left(4) == "url:" && ep.IsRegisteredWithGatekeeper())
    {
      PURL url(remotePartyAddress.Right(remotePartyAddress.GetLength()-4), "h323");
      host = url.GetHostName();
      port = url.GetPort();
      host.Replace("@","",TRUE,0);
    }
    else if(remotePartyAddress.Find("ip$") == P_MAX_INDEX && ep.IsRegisteredWithGatekeeper())
    {
      H323Gatekeeper *gk = ep.GetGatekeeper();
      PURL url(gk->GetName(), "h323");
      host = url.GetHostName();
      port = url.GetPort();
    } else {
      host = remotePartyAddress;
      PINDEX pos = host.Find("ip$");
      if(pos != P_MAX_INDEX) host = host.Mid(pos+3);
      port = host.Tokenise(":")[1];
      host = host.Tokenise(":")[0];
    }
    if(!HadAnsweredCall() && port != "" && port != "1720") host += ":"+port;

    alias = GetRemoteNumber();

    PRegularExpression RegEx("[^0-9]");
    if(alias.FindRegEx(RegEx) != P_MAX_INDEX) alias = "";

    if(remoteApplication.Find("MyPhone") != P_MAX_INDEX && remoteName == alias) // ???
      alias = "";

    address = "h323:"+alias+"@"+host;
  }
  memberName = remoteName+" ["+address+"]";
  PTRACE(1, "SetMemberName remoteName: " << remoteName);
  PTRACE(1, "SetMemberName remotePartyName: " << remotePartyName);
  PTRACE(1, "SetMemberName remotePartyAddress: " << remotePartyAddress);
  PTRACE(1, "SetMemberName remotePartyAliases: " << remoteAliasNames);
  PTRACE(1, "SetMemberName remotePartyNumber: " << remotePartyNumber);
  PTRACE(1, "SetMemberName memberName: " << memberName);
}

///////////////////////////////////////////////////////////////////////////////////////

#if OPENMCU_VIDEO

//
// this function is called whenever a connection needs a frame of video for output
//

BOOL OpenMCUH323Connection::OnOutgoingVideo(void * buffer, int width, int height, PINDEX & amount)
{
  PWaitAndSignal m(connMutex);

  if (conferenceMember != NULL)
    conferenceMember->ReadVideo(buffer, width, height, amount);
  else return FALSE;
/*  
  else if (!GetPreMediaFrame(buffer, width, height, amount)) {
    if ((width == CIF4_WIDTH) && (height == CIF4_HEIGHT))
      MCUVideoMixer::FillCIF4YUVFrame(buffer, 0, 0, 0);
    else if ((width == CIF_WIDTH) && (height == CIF_HEIGHT))
      MCUVideoMixer::FillCIFYUVFrame(buffer, 0, 0, 0);
    else if ((width == QCIF_WIDTH) && (height == QCIF_HEIGHT))
      MCUVideoMixer::FillQCIFYUVFrame(buffer, 0, 0, 0);
  }
*/
  return TRUE;
}

BOOL OpenMCUH323Connection::GetPreMediaFrame(void * buffer, int width, int height, PINDEX & amount)
{
  return OpenMCU::Current().GetPreMediaFrame(buffer, width, height, amount);
}

//
// this function is called whenever a connection receives a frame of video
//

BOOL OpenMCUH323Connection::OnIncomingVideo(const void * buffer, int width, int height, PINDEX amount)
{
  if (conferenceMember != NULL)
    conferenceMember->WriteVideo(buffer, width, height, amount);
  return TRUE;
}

#endif // OPENMCU_VIDEO


///////////////////////////////////////////////////////////////

H323Connection_ConferenceMember::H323Connection_ConferenceMember(Conference * _conference, OpenMCUH323EndPoint & _ep, const PString & _h323Token, ConferenceMemberId _id, BOOL _isMCU)
  : ConferenceMember(_conference, _id, _isMCU), ep(_ep), h323Token(_h323Token)
{ 
  conference->AddMember(this);
}

H323Connection_ConferenceMember::~H323Connection_ConferenceMember()
{
  PTRACE(4, "H323Connection_ConferenceMember deleted");
}

void H323Connection_ConferenceMember::Close()
{
  OpenMCUH323Connection * conn = (OpenMCUH323Connection *)ep.FindConnectionWithLock(h323Token);
  if (conn != NULL) {
    conn->LeaveConference();
    conn->Unlock();
  }
}

PString H323Connection_ConferenceMember::GetTitle() const
{
  PString output;
  if(id!=this)
  {
   OpenMCUH323Connection * conn = (OpenMCUH323Connection *)ep.FindConnectionWithLock(h323Token);
   if(conn == NULL) return h323Token;
   if(conn->GetConferenceMember() == this || conn->GetConferenceMember() == NULL) 
   {
    output = conn->GetRemoteName(); 
   }
   else PTRACE(1, "MCU\tWrong connection in GetTitle for " << h323Token);
   conn->Unlock();
  }
  return output;
}

PString H323Connection_ConferenceMember::GetMonitorInfo(const PString & hdr)
{ 
  PStringStream output;
  OpenMCUH323Connection * conn = (OpenMCUH323Connection *)ep.FindConnectionWithLock(h323Token);
  if (conn != NULL) {
    output << hdr << "Remote Address: " << conn->GetRemotePartyAddress() << "\n"
           << hdr << "AudioCodecs: " << conn->GetAudioTransmitCodecName() << '/' << conn->GetAudioReceiveCodecName() << "\n"
#if OPENMCU_VIDEO
           << hdr << "VideoCodecs: " << conn->GetVideoTransmitCodecName() << '/' << conn->GetVideoReceiveCodecName() << "\n"
#endif           
           ;
    conn->Unlock();
  }
  return output;
}

void H323Connection_ConferenceMember::SetName()
{
  if(id!=this)
  {
    cout << "SetName " << h323Token << "\n";
    int connLock = 0;
    OpenMCUH323Connection * conn = (OpenMCUH323Connection *)ep.FindConnectionWithLock(h323Token);
    if(conn == NULL)
    {
      conn = (OpenMCUH323Connection *)ep.FindConnectionWithoutLock(h323Token);
      if(conn == NULL) return;
      PTRACE(1,"Could not lock connection in SetName(): " << h323Token << flush);
    }
    else connLock = 1;

    H323Connection_ConferenceMember * connConferenceMemeber = conn->GetConferenceMember();
    if( connConferenceMemeber == this || connConferenceMemeber == NULL)
    {
      name = conn->GetMemberName();
      PTRACE(1, "SetName name: " << name);
    }
    else PTRACE(1, "MCU\tWrong connection in SetName for " << h323Token);

    if(connLock != 0) conn->Unlock();
  }
}

// signal to codec plugin for disable(enable) decoding incoming video from unvisible(visible) member
void H323Connection_ConferenceMember::SetFreezeVideo(BOOL disable) const
{
 if(id!=this)
 {
  cout << id << "->SetFreezeVideo(" << disable << ")\n";
  OpenMCUH323Connection * conn = (OpenMCUH323Connection *)ep.FindConnectionWithLock(h323Token);
  if(conn == NULL) return;
  H323Connection_ConferenceMember * connConferenceMemeber = conn->GetConferenceMember();
  if( connConferenceMemeber == this || connConferenceMemeber == NULL) 
  {
   H323VideoCodec *codec = conn->GetVideoReceiveCodec();
   if(codec) codec->OnFreezeVideo(disable);
  }
  else PTRACE(1, "MCU\tWrong connection in SetFreezeVideo for " << h323Token);
  conn->Unlock();
 }
}

void H323Connection_ConferenceMember::SendUserInputIndication(const PString & str)
{ 
  PTRACE(3, "Conference\tConnection " << id << " sending user indication " << str);
  int lockcount = 3;
  OpenMCUH323Connection * conn = (OpenMCUH323Connection *)ep.FindConnectionWithLock(h323Token);
  while(conn == NULL && lockcount > 0)
  {
   conn = (OpenMCUH323Connection *)ep.FindConnectionWithoutLock(h323Token);
   if(conn == NULL) return;
   conn = (OpenMCUH323Connection *)ep.FindConnectionWithLock(h323Token);
   PTRACE(1, "MCU\tDeadlock in SendUserInputIndication for " << h323Token);
   lockcount--;
  }
  if(conn == NULL) return;

  PStringStream msg; PStringStream utfmsg; if(conn->GetRemoteApplication().Find("MyPhone")!=P_MAX_INDEX){
    static const int table[128] = { // cp1251 -> utf8 translation based on http://www.linux.org.ru/forum/development/3968525
      0x82D0,0x83D0,  0x9A80E2,0x93D1,  0x9E80E2,0xA680E2,0xA080E2,0xA180E2,0xAC82E2,0xB080E2,0x89D0,0xB980E2,0x8AD0,0x8CD0,0x8BD0,0x8FD0,
      0x92D1,0x9880E2,0x9980E2,0x9C80E2,0x9D80E2,0xA280E2,0x9380E2,0x9480E2,0,       0xA284E2,0x99D1,0xBA80E2,0x9AD1,0x9CD1,0x9BD1,0x9FD1,
      0xA0C2,0x8ED0,  0x9ED1,  0x88D0,  0xA4C2,  0x90D2,  0xA6C2,  0xA7C2,  0x81D0,  0xA9C2,  0x84D0,0xABC2,  0xACC2,0xADC2,0xAEC2,0x87D0,
      0xB0C2,0xB1C2,  0x86D0,  0x96D1,  0x91D2,  0xB5C2,  0xB6C2,  0xB7C2,  0x91D1,  0x9684E2,0x94D1,0xBBC2,  0x98D1,0x85D0,0x95D1,0x97D1,
      0x90D0,0x91D0,  0x92D0,  0x93D0,  0x94D0,  0x95D0,  0x96D0,  0x97D0,  0x98D0,  0x99D0,  0x9AD0,0x9BD0,  0x9CD0,0x9DD0,0x9ED0,0x9FD0,
      0xA0D0,0xA1D0,  0xA2D0,  0xA3D0,  0xA4D0,  0xA5D0,  0xA6D0,  0xA7D0,  0xA8D0,  0xA9D0,  0xAAD0,0xABD0,  0xACD0,0xADD0,0xAED0,0xAFD0,
      0xB0D0,0xB1D0,  0xB2D0,  0xB3D0,  0xB4D0,  0xB5D0,  0xB6D0,  0xB7D0,  0xB8D0,  0xB9D0,  0xBAD0,0xBBD0,  0xBCD0,0xBDD0,0xBED0,0xBFD0,
      0x80D1,0x81D1,  0x82D1,  0x83D1,  0x84D1,  0x85D1,  0x86D1,  0x87D1,  0x88D1,  0x89D1,  0x8AD1,0x8BD1,  0x8CD1,0x8DD1,0x8ED1,0x8FD1
    };
    for(PINDEX i=0;i<str.GetLength();i++){
      unsigned int charcode=(BYTE)str[i];
      if(charcode&128){
        if((charcode=table[charcode&127])){
          utfmsg << (char)charcode << (char)(charcode >> 8);
          if(charcode >>= 16) utfmsg << (char)charcode;
        }
      } else utfmsg << (char)charcode;
    }
  } else utfmsg << str;
  msg << "<font color=blue><b>" << name << "</b>: " << utfmsg << "</font>"; OpenMCU::Current().HttpWriteEvent(msg);

  if (conn->GetConferenceMember() == this || conn->GetConferenceMember() == NULL) 
  {
    if(str.GetLength()<10)
    {
      iISequence << str.Trim();
      iISequence=iISequence.Right(10);
    }
    else iISequence=str.Trim();
    cout << "*uii: " << iISequence << "\n";
    PINDEX hashPos=iISequence.FindLast("#");
    if(hashPos != P_MAX_INDEX)
    {
      PINDEX astPos=iISequence.FindLast("*");
      if(astPos < hashPos)
      {
        PINDEX astPos2=iISequence.Left(astPos-1).FindLast("*");
        PINDEX hashPos2=iISequence.Left(astPos-1).FindLast("#");
        if(astPos2!=P_MAX_INDEX) if((hashPos2==P_MAX_INDEX)||(hashPos2<astPos2)) astPos=astPos2;
        Conference * conference=GetConference();
        if(conference!=NULL)
        {
          conference->HandleFeatureAccessCode(*this,iISequence(astPos+1,hashPos-1));
          iISequence=iISequence.Mid(hashPos+1,P_MAX_INDEX);
          conn->Unlock();
          return;
        }
      }
    }
    PString msg = "[" + conn->GetRemotePartyName() + "]: " + str;
    if (lock.Wait()) 
    {
      MemberListType::iterator r;
      for (r = memberList.begin(); r != memberList.end(); ++r)
        if (r->second != NULL) r->second->OnReceivedUserInputIndication(msg);
      lock.Signal();
    }
  }
  else PTRACE(1, "MCU\tWrong connection in SendUserInputIndication for " << h323Token);
  conn->Unlock();
}

///////////////////////////////////////////////////////////////

void OpenMCUH323Connection::LogCall(const BOOL accepted)
{
  if(!controlChannel && !signallingChannel) return;
  H323TransportAddress address = GetControlChannel().GetRemoteAddress();
  PIPSocket::Address ip;
  WORD port;
  PStringStream stringStream, timeStream;
  address.GetIpAndPort(ip, port);
  timeStream << GetConnectionStartTime().AsString("hh:mm:ss");
  stringStream << ' ' << "caller-ip:" << ip << ':' << port << ' '
	             << GetRemotePartyName() 
               << " room:" << ((conference != NULL) ? conference->GetNumber() : PString());

  if (accepted) {
    PStringStream connectionDuration;
    connectionDuration << setprecision(0) << setw(5) << (PTime() - GetConnectionStartTime());
    OpenMCU::Current().LogMessage(timeStream + stringStream	+ " connection duration:" + connectionDuration);
  }
  else 
    OpenMCU::Current().LogMessage(timeStream + " Call denied:" + stringStream);		
}

///////////////////////////////////////////////////////////////

OutgoingAudio::OutgoingAudio(H323EndPoint & _ep, OpenMCUH323Connection & _conn, unsigned int _sampleRate, unsigned _channels)
  : ep(_ep), conn(_conn), sampleRate(_sampleRate), channels(_channels)
{
  os_handle = 0;
/*
#if USE_SWRESAMPLE
  swrc = NULL;
#else
  swrc = 0;
#endif
  sampleRate = _sampleRate;
  if(sampleRate == 0) sampleRate = 8000; // g711
  if(sampleRate != 16000)
  {
#if USE_SWRESAMPLE
   swrc = swr_alloc_set_opts(NULL, AV_CH_LAYOUT_MONO, AV_SAMPLE_FMT_S16, sampleRate,
                            AV_CH_LAYOUT_MONO, AV_SAMPLE_FMT_S16, 16000,
                            0, NULL);
   swr_init(swrc);
#else
   swrc = 1;
#endif //USE_SWRESAMPLE
  }
*/
}

void OutgoingAudio::CreateSilence(void * buffer, PINDEX amount)
{
  memset(buffer, 0, amount);
//  lastReadCount = amount;
}

BOOL OutgoingAudio::Read(void * buffer, PINDEX amount)
{
  PWaitAndSignal mutexR(audioChanMutex);
  if (!IsOpen()) return FALSE;
  if (!conn.OnOutgoingAudio(buffer, amount, sampleRate, channels)) CreateSilence(buffer, amount);

  unsigned msPerFrame = (amount*1000)/(sampleRate*channels*sizeof(short));
  delay.Delay(msPerFrame);

  lastReadCount = amount;
  return TRUE;

/*
  PINDEX amount16 = amount;
  void * buffer16 = buffer;
*/

//  cout << "SEND amount=" << amount << "\n";
  

/*
#if USE_SWRESAMPLE
  if(swrc != NULL) //8000
#else
  if(swrc) //8000
#endif
  {
   amount16 = amount*16000/sampleRate; if((amount16&1) != 0) amount16+=1;
   if((amount16>>1) > swr_buf.GetSize()) swr_buf.SetSize(amount16>>1);
   buffer16 = swr_buf.GetPointer();
  }

  // do the read call here, by calling conn.OnOutgoingAudio():
  if (!conn.OnOutgoingAudio(buffer16, amount16))
    CreateSilence(buffer16, amount16);

#if USE_SWRESAMPLE
  if(swrc != NULL) //8000
   swr_convert(swrc, (uint8_t **)&buffer, amount>>1, (const uint8_t **)&buffer16, amount16>>1);
#else
  if(swrc) //8000
    for(PINDEX i=0;i<(amount>>1);i++) ((short*)buffer)[i] = ((short*)buffer16)[i*16000/sampleRate];
#endif


//  delay.Delay(amount16 / 32);
*/

}

BOOL OutgoingAudio::Close()
{
  if (!IsOpen()) 
    return FALSE;

  PWaitAndSignal mutexC(audioChanMutex);
  os_handle = -1;
/*
#if USE_SWRESAMPLE
  if(swrc != NULL) swr_free(&swrc);
#else
  swrc = 0;
#endif
*/
  return TRUE;
}

///////////////////////////////////////////////////////////////////////////

IncomingAudio::IncomingAudio(H323EndPoint & _ep, OpenMCUH323Connection & _conn, unsigned int _sampleRate, unsigned _channels)
  : sampleRate(_sampleRate), channels(_channels), ep(_ep), conn(_conn)
{
  os_handle = 0;
/*
#if USE_SWRESAMPLE
  swrc = NULL;
#else
  swrc = 0;
#endif
  sampleRate = _sampleRate;
  if(sampleRate == 0) sampleRate = 8000; // g711
#if USE_SWRESAMPLE
  if(sampleRate != 16000)
  {
   swrc = swr_alloc_set_opts(NULL, AV_CH_LAYOUT_MONO, AV_SAMPLE_FMT_S16, 16000,
                            AV_CH_LAYOUT_MONO, AV_SAMPLE_FMT_S16, sampleRate,
                            0, NULL);
   swr_init(swrc);
  }
#else
  swrc = 1;
#endif
*/
}

BOOL IncomingAudio::Write(const void * buffer, PINDEX amount)
{
  PWaitAndSignal mutexW(audioChanMutex);
  if (!IsOpen()) return FALSE;
  conn.OnIncomingAudio(buffer, amount, sampleRate, channels);

  unsigned msPerFrame = (amount*1000)/(sampleRate*channels*sizeof(short));
  delay.Delay(msPerFrame);

  return TRUE;

//  PINDEX amount16 = amount;

//  cout << "RECV amount=" << amount << "\n";

/*
#if USE_SWRESAMPLE
  if(swrc != NULL) //8000
#else
  if(swrc) //8000
#endif
  {
   void * buffer16;
   amount16 = amount*16000/sampleRate; if((amount16&1) != 0) amount16+=1;
   if((amount16>>1) > swr_buf.GetSize()) swr_buf.SetSize(amount16>>1);
   buffer16 = swr_buf.GetPointer();
#if USE_SWRESAMPLE
   swr_convert(swrc, (uint8_t **)&buffer16, amount16>>1, (const uint8_t **)&buffer, amount>>1);
#else
   for(PINDEX i=0;i<(amount16>>1);i++) ((short*)buffer16)[i] = ((short*)buffer)[i*sampleRate/16000];
#endif
   conn.OnIncomingAudio(buffer16, amount16, sampleRate);
  }
  else */ 

//  delay.Delay(amount16 / 32);
}

BOOL IncomingAudio::Close()
{
  if (!IsOpen())
    return FALSE;

  PWaitAndSignal mutexA(audioChanMutex);
  os_handle = -1;
/*
#if USE_SWRESAMPLE
  if(swrc != NULL) swr_free(&swrc);
#else
  swrc = 0;
#endif
*/
  return TRUE;
}

///////////////////////////////////////////////////////////////////////////

