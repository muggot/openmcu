#include <ptlib.h>
#include "mcu.h"
#include "html.h"
#include <stdio.h>
#include <string.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

static unsigned long html_template_size; // count on zero initialization
char * html_template_buffer;
static unsigned long html_quote_size; // count on zero initialization
char * html_quote_buffer;
PMutex html_mutex;

void BeginPage (PStringStream &html, const char *ptitle, const char *title, const char *quotekey)
{ PWaitAndSignal m(html_mutex);
  if(html_template_size <= 0) // count on zero initialization
  { FILE *fs;
#ifdef SYS_RESOURCE_DIR
#  ifdef _WIN32
    fs=fopen(SYS_RESOURCE_DIR+PString("\\template.html"), "r");
#  else
    fs=fopen(SYS_RESOURCE_DIR+PString("/template.html"), "r");
#  endif
#else
    fs=fopen("template.html", "r");
#endif
    if(fs)
    { fseek(fs, 0L, SEEK_END); html_template_size = ftell(fs); rewind(fs);
      html_template_buffer = new char[html_template_size + 1];
      if(html_template_size != fread(html_template_buffer, 1, html_template_size, fs)) html_template_size = -1;
      else html_template_buffer[html_template_size] = 0;
      fclose(fs);
    }
    else html_template_size = -1; // read error indicator
  }
  if(html_template_size <= 0) { cout << "Can't load HTML template!\n"; PTRACE(1,"WebCtrl\tCan't read HTML template from file"); return; }

  PString lang = MCUConfig("Parameters").GetString("Language").ToLower();

  PString html0(html_template_buffer); html0 = html0.Left(html0.Find("$BODY$"));
  html0.Replace("$LANG$",     lang,     TRUE, 0);
  html0.Replace("$PTITLE$",   ptitle,   TRUE, 0);
  html0.Replace("$TITLE$",    title,    TRUE, 0);
  html0.Replace("$QUOTE$",    quotekey, TRUE, 0);

  html << html0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void EndPage (PStringStream &html, PString copyr) 
{
  PWaitAndSignal m(html_mutex);
  if (html_template_size <= 0) return;
  PString html0(html_template_buffer); html0 = html0.Mid(html0.Find("$BODY$")+6,P_MAX_INDEX);
  html0.Replace("$COPYRIGHT$", copyr,   TRUE, 0);
  html << html0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

PString ErrorPage( //maybe ptlib could already create pages like this? for future: dig into http server part
  PString        ip,            // "192.168.1.1"
  unsigned short port,          // 1420
  unsigned       errorCode,     // 403
  PString        errorText,     // "Forbidden"
  PString        title,         // "Page you tried to access is forbidden, lol"
  PString        description    // detailed: "blablablablablabla \n blablablablablabla"
)
{
  PStringStream p;
  PTime now;
  p << "<html>";
    p << "<head>";
      p << "<title>" << errorCode << " " << errorText << "</title>";
    p << "</head>";
    p << "<body>";

    p << "<h1>" << errorText << "</h1>";
    p << "<h3>" << title << "</h3>";

    PStringArray text = description.Lines();
    for(PINDEX i=0;i<text.GetSize();i++) p << "<p>" << text[i] << "</p>";

    p << "<hr><i>"
      << OpenMCU::Current().GetName() << " "  << OpenMCU::Current().GetVersion(TRUE)
      << " (" << OpenMCU::Current().GetOSName() << " " << OpenMCU::Current().GetOSVersion() << ") "
      << " Server at "
      << ip << " Port " << port
      << ". Generated " << now.AsString(PTime::RFC1123, PTime::GMT)
      << "</i>";
    p << "</body>";
  p << "</html>";
  return p;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

static int SplitConfigKey(const PString & fullName,
                          PString & section, PString & key)
{
  if (fullName.IsEmpty())
    return 0;

  PINDEX slash = fullName.FindLast('\\');
  if (slash == 0 || slash >= fullName.GetLength()-1) {
    key = fullName;
    return 1;
  }

  section = fullName.Left(slash);
  key = fullName.Mid(slash+1);
  if (section.IsEmpty() || key.IsEmpty())
    return 0;

  return 2;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

DefaultPConfigPage::DefaultPConfigPage(PHTTPServiceProcess & app,const PString & title, const PString & section, const PHTTPAuthority & auth)
    : PConfigPage(app,title,section,auth)
{
  MCUConfig cfg(section);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL DefaultPConfigPage::Post(PHTTPRequest & request, const PStringToString & data, PHTML & reply)
{
  // Make sure the internal structure is up to date before accepting new data
  if (!section)
    LoadFromConfig();

  PSortedStringList oldValues;

  // Remember fields that are here now, so can delete removed array fields
  PINDEX fld;
  for (fld = 0; fld < fields.GetSize(); fld++) {
    PHTTPField & field = fields[fld];
    if (&field != keyField && &field != valField && &field != sectionField) {
      PStringList names;
      field.GetAllNames(names);
      oldValues = names;
    }
  }

  PHTTPForm::Post(request, data, reply);
  if (request.code != PHTTP::RequestOK)
    return TRUE;

  if (sectionField != NULL)
    section = sectionPrefix + sectionField->GetValue() + sectionSuffix;

  PString sectionName = request.url.GetQueryVars()("section", section);
  if (sectionName.IsEmpty())
    return TRUE;

  for (fld = 0; fld < fields.GetSize(); fld++) {
    PHTTPField & field = fields[fld];
    if (&field == keyField) {
      PString key = field.GetValue();
      if (!key)
        cfg.SetString(key, valField->GetValue());
    }
    else if (&field != valField && &field != sectionField)
      field.SaveToConfig(cfg);
  }

  // Find out which fields have been removed (arrays elements deleted)
  for (fld = 0; fld < fields.GetSize(); fld++) {
    PHTTPField & field = fields[fld];
    if (&field != keyField && &field != valField && &field != sectionField) {
      PStringList names;
      field.GetAllNames(names);
      for (PINDEX i = 0; i < names.GetSize(); i++) {
        PINDEX idx = oldValues.GetStringsIndex(names[i]);
        if (idx != P_MAX_INDEX)
          oldValues.RemoveAt(idx);
      }
    }
  }

  for (fld = 0; fld < oldValues.GetSize(); fld++) {
    PString section, key;
    switch (SplitConfigKey(oldValues[fld], section, key)) {
      case 1 :
        cfg.DeleteKey(key);
        break;
      case 2 :
        cfg.DeleteKey(section, key);
        if (cfg.GetKeys(section).IsEmpty())
          cfg.DeleteSection(section);
    }
  }

  section = sectionName;

  process.OnContinue();
  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL DefaultPConfigPage::OnPOST(PHTTPServer & server,
                         const PURL & url,
                         const PMIMEInfo & info,
                         const PStringToString & data,
                         const PHTTPConnectionInfo & connectInfo)
{
  PHTTPConfig::OnPOST(server, url, info, data, connectInfo);
  return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////////

GeneralPConfigPage::GeneralPConfigPage(PHTTPServiceProcess & app,const PString & title, const PString & section, const PHTTPAuthority & auth)
    : DefaultPConfigPage(app,title,section,auth)
{
  OpenMCU & mcu = OpenMCU::Current();

  // Language
  Add(new PHTTPStringField("Language", 2, cfg.GetString("Language"), "<td rowspan='1' valign='top' style='background-color:#efe;padding:4px;border-right:2px solid #090;border-top:1px dotted #cfc'>RU, EN"));

  // HTTP authentication username/password
  Add(new PHTTPStringField(UserNameKey, 25, cfg.GetString(UserNameKey), "<td rowspan='4' valign='top' style='background-color:#fee;padding:4px;border-left:2px solid #900;border-top:1px dotted #fcc'><b>Security</b>"));
  Add(new PHTTPPasswordField(PasswordKey, 25, PHTTPPasswordField::Decrypt(cfg.GetString(PasswordKey))));

  // OpenMCU Server Id
  Add(new PHTTPStringField("OpenMCU Server Id", 25, cfg.GetString("OpenMCU Server Id", mcu.GetName()+" v"+mcu.GetVersion())));

  // HTTP Port number to use.
  Add(new PHTTPIntegerField(HttpPortKey, 1, 32767, (WORD)cfg.GetInteger(HttpPortKey, DefaultHTTPPort)));

  // RTP Port Setup
  Add(new PHTTPIntegerField("RTP Base Port", 0, 65535, cfg.GetInteger("RTP Base Port", 0),"<td><td rowspan='2' valign='top' style='background-color:#eec;padding:4px;border-left:1px solid #770;border-right:1px solid #770;border-top:1px dotted #eec'><b>RTP Port Setup</b><br>0 = auto<br>Example: base=5000, max=6000"));
  Add(new PHTTPIntegerField("RTP Max Port", 0, 65535, cfg.GetInteger("RTP Max Port", 0)));

#if PTRACING
  // Trace level
  Add(new PHTTPIntegerField(TraceLevelKey, 0, 6, cfg.GetInteger(TraceLevelKey, DEFAULT_TRACE_LEVEL), "<td><td rowspan='4' valign='top' style='background-color:#efe;padding:4px;border-right:2px solid #090;border-top:1px dotted #cfc'><b>Logging:</b><br><br>Log level: 1=Fatal only, 2=Errors, 3=Warnings, 4=Info, 5=Debug<br>Trace level: 0=No tracing ... 6=Very detailed<br>Event buffer size: 10...1000"));
#endif

#ifdef SERVER_LOGS
  // Log level for messages
  Add(new PHTTPIntegerField(LogLevelKey, PSystemLog::Fatal, PSystemLog::NumLogLevels-1, cfg.GetInteger(LogLevelKey, DEFAULT_LOG_LEVEL)));

  // Log filename
  Add(new PHTTPStringField(CallLogFilenameKey, 40, mcu.logFilename));
#endif

  // Buffered events
  Add(new PHTTPIntegerField(HttpLinkEventBufferKey, 10, 1000, cfg.GetInteger(HttpLinkEventBufferKey, 100)));

#if P_SSL
  // SSL certificate file.
  PString certificateFile = cfg.GetString(HTTPCertificateFileKey, "server.pem");
  Add(new PHTTPStringField(HTTPCertificateFileKey, 25, certificateFile));
  if (!SetServerCertificate(certificateFile, TRUE)) {
    PSYSTEMLOG(Fatal, "MCU\tCould not load certificate \"" << certificateFile << '"');
    return FALSE;
  }
#endif

#if OPENMCU_VIDEO
  Add(new PHTTPBooleanField("Enable video", cfg.GetBoolean("Enable video", TRUE), "<td rowspan='4' valign='top' style='background-color:#fee;padding:4px;border-left:2px solid #900;border-top:1px dotted #fcc'><b>Video Setup</b><br><br>Video frame rate range: 1..30 (for outgoing video)<br />H.264 level value must be one of: 9, 10, 11, 12, 13, 20, 21, 22, 30, 31, 32, 40, 41, 42, 50, 51."));
  Add(new PHTTPIntegerField("Video frame rate", 1, 100, cfg.GetInteger("Video frame rate", DefaultVideoFrameRate)));
  Add(new PHTTPIntegerField("Video quality", 1, 30, cfg.GetInteger("Video quality", DefaultVideoQuality)));
  Add(new PHTTPBooleanField(ForceSplitVideoKey, cfg.GetBoolean(ForceSplitVideoKey, TRUE)));
#endif

  // Default room
  Add(new PHTTPStringField(DefaultRoomKey, 25, cfg.GetString(DefaultRoomKey, DefaultRoom), "<td rowspan='5' valign='top' style='background-color:#efe;padding:4px;border-right:2px solid #090;border-top:1px dotted #cfc'><b>Room Setup</b>"));

  // create/don't create empty room with default name at start
  Add(new PHTTPBooleanField(CreateEmptyRoomKey, cfg.GetBoolean(CreateEmptyRoomKey, FALSE)));

  // recall last template after room created
  Add(new PHTTPBooleanField(RecallLastTemplateKey, cfg.GetBoolean(RecallLastTemplateKey, FALSE)));

  // get conference time limit 
  Add(new PHTTPIntegerField(DefaultRoomTimeLimitKey, 0, 10800, cfg.GetInteger(DefaultRoomTimeLimitKey, 0)));

  // allow/disallow self-invite:
  Add(new PHTTPBooleanField(AllowLoopbackCallsKey, cfg.GetBoolean(AllowLoopbackCallsKey, FALSE)));

  // video recorder setup
  PStringStream recorderInfo;
  recorderInfo
    << "<td rowspan='8' valign='top' style='background-color:#fee;padding:4px;border-left:2px solid #900;border-top:1px dotted #fcc'>"
    << "<b>Video Recorder Setup:</b><br><br>"
    << "Use the following definitions to set ffmpeg command-line options:<br>"
    << "<b>%V</b> - input video stream,<br>"
    << "<b>%A</b> - input audio stream,<br>"
    << "<b>%F</b> - frame size, "
    << "<b>%R</b> - frame rate,<br>"
    << "<b>%S</b> - sample rate, <b>%C</b> - number of channels for audio,<br>"
    << "<b>%O</b> - name without extension"
    << "<br><br>";
  if(!PFile::Exists(mcu.vr_ffmpegPath)) recorderInfo << "<b><font color=red>ffmpeg doesn't exist - check the path!</font></b>";
  else
  { PFileInfo info;
    PFilePath path(mcu.vr_ffmpegPath);
    PFile::GetInfo(path, info);
    if(!(info.type & 3)) recorderInfo << "<b><font color=red>Warning: ffmpeg neither file, nor symlink!</font></b>";
    else if(!(info.permissions & 0111)) recorderInfo << "<b><font color=red>ffmpeg permissions check failed</font></b>";
    else
    {
      if(!PDirectory::Exists(mcu.vr_ffmpegDir)) if(!PFile::Exists(mcu.vr_ffmpegDir)) { PDirectory::Create(mcu.vr_ffmpegDir,0700); PThread::Sleep(50); }
      if(!PDirectory::Exists(mcu.vr_ffmpegDir)) recorderInfo << "<b><font color=red>Directory does not exist: " << mcu.vr_ffmpegDir << "</font></b>";
      else
      { PFileInfo info;
        PFilePath path(mcu.vr_ffmpegDir);
        PFile::GetInfo(path, info);
        if(!(info.type & 6)) recorderInfo << "<b><font color=red>Warning: output directory neither directory, nor symlink!</font></b>";
        else if(!(info.permissions & 0222)) recorderInfo << "<b><font color=red>output directory permissions check failed</font></b>";
        else recorderInfo << "<b><font color=green>Looks good.</font> Execution script preview:</b><br><tt>" << mcu.ffmpegCall << "</tt>";
      }
    }
  }
  Add(new PHTTPStringField(RecorderFfmpegPathKey, 40, mcu.vr_ffmpegPath, recorderInfo));
  Add(new PHTTPStringField(RecorderFfmpegOptsKey, 40, mcu.vr_ffmpegOpts));
  Add(new PHTTPStringField(RecorderFfmpegDirKey, 40, mcu.vr_ffmpegDir));
  Add(new PHTTPIntegerField(RecorderFrameWidthKey, 176, 1920, mcu.vr_framewidth));
  Add(new PHTTPIntegerField(RecorderFrameHeightKey, 144, 1152, mcu.vr_frameheight));
  Add(new PHTTPIntegerField(RecorderFrameRateKey, 1, 100, mcu.vr_framerate));
  Add(new PHTTPIntegerField(RecorderSampleRateKey, 2000, 1000000, mcu.vr_sampleRate));
  Add(new PHTTPIntegerField(RecorderAudioChansKey, 1, 8, mcu.vr_audioChans));

  // get WAV file played to a user when they enter a conference
  //rsrc->Add(new PHTTPStringField(ConnectingWAVFileKey, 50, cfg.GetString(ConnectingWAVFileKey, DefaultConnectingWAVFile)));

  // get WAV file played to a conference when a new user enters
  //rsrc->Add(new PHTTPStringField(EnteringWAVFileKey, 50, cfg.GetString(EnteringWAVFileKey, DefaultEnteringWAVFile)));

  // get WAV file played to a conference when a new user enters
  //rsrc->Add(new PHTTPStringField(LeavingWAVFileKey, 50, cfg.GetString(LeavingWAVFileKey, DefaultLeavingWAVFile)));

  BuildHTML("");
  PStringStream html_begin, html_end;
  BeginPage(html_begin, section, "window.l_param_general","window.l_info_param_general");
  EndPage(html_end,OpenMCU::Current().GetHtmlCopyright());
  PStringStream html_page; html_page << html_begin << string << html_end;
  string = html_page;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

H323PConfigPage::H323PConfigPage(PHTTPServiceProcess & app,const PString & title, const PString & section, const PHTTPAuthority & auth)
    : DefaultPConfigPage(app,title,section,auth)
{
  OpenMCU & mcu = OpenMCU::Current();

  mcu.GetEndpoint().Initialise(cfg, this);
  if(mcu.GetEndpoint().behind_masq){PStringStream msq; msq<<"Masquerading as "<<*(mcu.GetEndpoint().masqAddressPtr); mcu.HttpWriteEvent(msq);}

  BuildHTML("");
  PStringStream html_begin, html_end;
  BeginPage(html_begin, section, "window.l_param_h323", "window.l_info_param_h323");
  EndPage(html_end,OpenMCU::Current().GetHtmlCopyright());
  PStringStream html_page; html_page << html_begin << string << html_end;
  string = html_page;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

SIPPConfigPage::SIPPConfigPage(PHTTPServiceProcess & app,const PString & title, const PString & section, const PHTTPAuthority & auth)
    : DefaultPConfigPage(app,title,section,auth)
{
  OpenMCU & mcu = OpenMCU::Current();

  // SIP Listener setup
  mcu.sipListener = cfg.GetString(SipListenerKey, "0.0.0.0").Trim();
  if(mcu.sipListener=="") mcu.sipListener="0.0.0.0";
  Add(new PHTTPStringField(SipListenerKey, 32, mcu.sipListener,"<td rowspan='2' valign='top' style='background-color:#efe;padding:4px;border-right:2px solid #090;border-top:1px dotted #cfc'><b>SIP Setup</b>"));
  if(mcu.sipListener=="0.0.0.0") mcu.sipListener="0.0.0.0 :5060";
  mcu.sipendpoint->Resume();

#if OPENMCU_VIDEO
  Add(new PHTTPStringField(H264LevelForSIPKey, 2, PString(mcu.h264DefaultLevelForSip)));
#endif

  BuildHTML("");
  PStringStream html_begin, html_end;
  BeginPage(html_begin, section, "window.l_param_sip", "window.l_info_param_sip");
  EndPage(html_end,OpenMCU::Current().GetHtmlCopyright());
  PStringStream html_page; html_page << html_begin << string << html_end;
  string = html_page;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

SectionPConfigPage::SectionPConfigPage(PHTTPServiceProcess & app,const PString & title, const PString & section, const PHTTPAuthority & auth)
    : PConfigPage(app,title,NULL,auth)
{
  cfg = MCUConfig(section);
  PStringList keys = cfg.GetKeys();
  PString data;
  for(PINDEX i = 0; i < keys.GetSize(); i++)
    data += keys[i]+"="+cfg.GetString(keys[i])+"\n";

  if(section == "RoomAccess")
    Add(new PHTTPStringField(" ", 1000, data, "<td><td rowspan='4' valign='top' style='background-color:#efe;padding:4px;border-right:2px solid #090;border-top:1px dotted #cfc'> *=ALLOW<br> room101=allow user1@domain,user2@,@domain,@@via<br> room102=allow user3@domain@via"));
  else if(section == "ProxyServers")
    Add(new PHTTPStringField(" ", 1000, data, "<td><td rowspan='4' valign='top' style='background-color:#efe;padding:4px;border-right:2px solid #090;border-top:1px dotted #cfc'>room_name=proxy_server,username,password,0,3600"));
  else
    Add(new PHTTPStringField(" ", 1000, data, NULL));
  BuildHTML("");

  PStringStream html_begin, html_end;
  if(section == "RoomAccess")
    BeginPage(html_begin, "Access Rules", "window.l_param_access_rules", "window.l_info_param_access_rules");
  else if(section == "ProxyServers")
    BeginPage(html_begin, "SIP proxy-servers", "window.l_param_sip_proxy", "window.l_info_param_sip_proxy");
  else
    BeginPage(html_begin, section, "window.l_param_general", "window.l_info_param_general");
  EndPage(html_end,OpenMCU::Current().GetHtmlCopyright());

  PStringStream html_page; html_page << html_begin << string << html_end;
  string = html_page;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL SectionPConfigPage::Post(PHTTPRequest & request,
                       const PStringToString & data,
                       PHTML & reply)
{

  PHTTPForm::Post(request, data, reply);

  cfg.DeleteSection();
  PString input_data = fields[0].GetValue();

  PStringArray list = input_data.Tokenise("\n");
  for(int i = 0; i < list.GetSize(); i++)
  {
    PString key = list[i].Tokenise("=")[0];
    PString value = list[i].Tokenise("=")[1];
    if(key != "" && value != "")
      cfg.SetString(key, value);
  }
  process.OnContinue();
  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL SectionPConfigPage::OnPOST(PHTTPServer & server,
                         const PURL & url,
                         const PMIMEInfo & info,
                         const PStringToString & data,
                         const PHTTPConnectionInfo & connectInfo)
{
  PHTTPConfig::OnPOST(server, url, info, data, connectInfo);
  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

CodecsPConfigPage::CodecsPConfigPage(PHTTPServiceProcess & app,const PString & title, const PString & section, const PHTTPAuthority & auth)
    : PConfigPage(app,title,section,auth)
{
  cfg = MCUConfig(section);

  PString info, infoStyle = "<td rowspan='1' style='background-color:#efe;padding:15px;border-bottom:2px solid white;'>";
  PStringList keys = cfg.GetKeys();
  for(PINDEX i = 0; i < keys.GetSize(); i++)
  {
    info = "<input type=button value='↑' onClick='rowUp(this)' style='margin-top:10px;margin-left:10px;margin-right:1px;'><input type=button value='↓' onClick='rowDown(this)' style='margin-top:10px;margin-left:1px;margin-right:10px;'>";
    H323Capability *cap = H323Capability::Create(keys[i]);
    if(cap == NULL && keys[i].Find("{sw}") == P_MAX_INDEX)
      cap = H323Capability::Create(keys[i]+"{sw}");
    if(cap)
    {
      const OpalMediaFormat & mf = cap->GetMediaFormat();
      if(mf.GetTimeUnits() == 90)
        info += infoStyle+PString(mf.GetOptionInteger("Frame Width"))+"x"+PString(mf.GetOptionInteger("Frame Height"));
      else
        info += infoStyle+PString(mf.GetTimeUnits()*1000)+"Hz";
      //info += infoStyle+PString(mf.GetBandwidth())+"bit/s";
      info += infoStyle+"default fmtp: ";
      for (PINDEX j = 0; j < mf.GetOptionCount(); j++)
       if(mf.GetOption(j).GetFMTPName() != "")
         info += mf.GetOption(j).GetFMTPName()+"="+mf.GetOption(j).AsString()+";";
    } else {
      info += infoStyle+"<script type='text/javascript'>document.write(window.l_not_found);</script>";
    }
    Add(new PHTTPBooleanField(keys[i], cfg.GetBoolean(keys[i]), info));
  }

  BuildHTML("");
  PStringStream html_begin, html_end;
  if(section == "RECEIVE_SOUND")
    BeginPage(html_begin, "Audio codecs(receive)", "window.l_param_receive_sound", "window.l_info_param_receive_sound");
  else if(section == "TRANSMIT_SOUND")
    BeginPage(html_begin, "Audio codecs(transmit)", "window.l_param_transmit_sound", "window.l_info_param_transmit_sound");
  else if(section == "RECEIVE_VIDEO")
    BeginPage(html_begin, "Video codecs(receive)", "window.l_param_receive_video", "window.l_info_param_receive_video");
  else if(section == "TRANSMIT_VIDEO")
    BeginPage(html_begin, "Video codecs(transmit)", "window.l_param_transmit_video", "window.l_info_param_transmit_video");
  else
    BeginPage(html_begin, section, "", "");
  EndPage(html_end,OpenMCU::Current().GetHtmlCopyright());
  PStringStream html_page; html_page << html_begin << string << html_end;

  html_page << "<script type='text/javascript'>\n"
  "function rowUp(obj)\n"
  "{\n"
  "  var table = obj.parentNode.parentNode.parentNode;\n"
  "  var rowNum=obj.parentNode.parentNode.sectionRowIndex;\n"
  "  if(rowNum!=0) table.rows[rowNum].parentNode.insertBefore(table.rows[rowNum],table.rows[rowNum-1]);\n"
  "}\n"
  "function rowDown(obj)\n"
  "{\n"
  "  var table = obj.parentNode.parentNode.parentNode;\n"
  "  var rowNum=obj.parentNode.parentNode.sectionRowIndex;\n"
  "  var rows=obj.parentNode.parentNode.parentNode.childNodes.length;\n"
  "  if(rowNum!=rows-1) table.rows[rowNum].parentNode.insertBefore(table.rows[rowNum+1],table.rows[rowNum]);\n"
  "}\n"
  "</script>\n";

  string = html_page;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL CodecsPConfigPage::Post(PHTTPRequest & request,
                       const PStringToString & data,
                       PHTML & reply)
{
  PHTTPForm::Post(request, data, reply);

  cfg.DeleteSection();
  for(PINDEX i = 0; i < dataArray.GetSize(); i++)
  {
    PString key = dataArray[i].Tokenise("=")[0];
    if(dataArray.GetStringsIndex(key+"=TRUE") != P_MAX_INDEX)
      cfg.SetBoolean(key, 1);
    else
      cfg.SetBoolean(key, 0);
  }

  process.OnContinue();
  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL CodecsPConfigPage::OnPOST(PHTTPServer & server,
                         const PURL & url,
                         const PMIMEInfo & info,
                         const PStringToString & data,
                         const PHTTPConnectionInfo & connectInfo)
{
  dataArray = PURL::UntranslateString(connectInfo.GetEntityBody(), PURL::QueryTranslation).Tokenise("&");
  for(PINDEX i = 0; dataArray[i] != NULL; i++)
  {
    if(dataArray[i].Tokenise("=")[0] == "submit" ||
       dataArray[i].Tokenise("=")[0] == "" ||
       dataArray[i].Tokenise("=")[1] == "")
    {
      dataArray.RemoveAt(i); i--; continue;
    }
    if(dataArray[i].Tokenise("=")[1] == "TRUE") continue;
    if(dataArray[i].Tokenise("=")[1] == "FALSE")
    {
      for(PINDEX j = i+1; dataArray[j] != NULL; j++)
      {
        if(dataArray[j].Tokenise("=")[0] == dataArray[i].Tokenise("=")[0] && dataArray[j].Tokenise("=")[1] == "TRUE")
        {
          dataArray[i] = dataArray[j]; dataArray.RemoveAt(j); j--; continue;
        }
      }
    }
  }
  if(dataArray[dataArray.GetSize()-1].Tokenise("=")[0] == "") dataArray.RemoveAt(dataArray.GetSize()-1);

  PHTTPConfig::OnPOST(server, url, info, data, connectInfo);
  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
