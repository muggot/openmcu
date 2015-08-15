
#include "precompile.h"
#include "mcu.h"
#include "html.h"

//////////////////////////////////////////////////////////////////////////////////////////////

PCREATE_SERVICE_MACRO(mcuinfo,P_EMPTY,P_EMPTY)
{
  return OpenMCU::Current().GetEndpoint().GetMonitorText();
}

PCREATE_SERVICE_MACRO_BLOCK(RoomStatus,P_EMPTY,P_EMPTY,block)
{
  return OpenMCU::Current().GetEndpoint().GetRoomStatus(block);
}

PCREATE_SERVICE_MACRO_BLOCK(RoomList,P_EMPTY,P_EMPTY,block)
{
  return OpenMCU::Current().GetEndpoint().GetRoomList(block);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

static unsigned long html_template_size; // count on zero initialization
char * html_template_buffer;
char * html_quote_buffer;
PMutex html_mutex;

void BeginPage (PStringStream &html, PString ptitle, PString title, PString quotekey)
{ PWaitAndSignal m(html_mutex);
  if(html_template_size <= 0) // count on zero initialization
  { FILE *fs;
    fs=fopen(PString(SYS_RESOURCE_DIR) + PATH_SEPARATOR + "template.html", "r");
    if(fs)
    { fseek(fs, 0L, SEEK_END); html_template_size = ftell(fs); rewind(fs);
      html_template_buffer = new char[html_template_size + 1];
      if(html_template_size != fread(html_template_buffer, 1, html_template_size, fs)) html_template_size = -1;
      else html_template_buffer[html_template_size] = 0;
      fclose(fs);
    }
    else html_template_size = -1; // read error indicator
  }
  if(html_template_size <= 0)
  { cout << "Can't load HTML template!\n"; PTRACE(1,"WebCtrl\tCan't read HTML template from file"); return; }

  PString lang = MCUConfig("Parameters").GetString("Language", "").ToLower();

  PString jsInit="defaultProtocol='"+MCUConfig("Parameters").GetString(DefaultProtocolKey, "sip")+"';\n";

  PString html0(html_template_buffer); html0 = html0.Left(html0.Find("$BODY$"));
  html0.Replace("$LANG$",     lang,     TRUE, 0);
  html0.Replace("$PTITLE$",   ptitle,   TRUE, 0);
  html0.Replace("$TITLE$",    title,    TRUE, 0);
  if(quotekey != "")
    html0.Replace("$QUOTE$", "<div class='quote' id='quote_info'><script type='text/javascript'>document.write("+quotekey+");</script></div>", TRUE, 0);
  else
    html0.Replace("$QUOTE$", "", TRUE, 0);

  html0.Replace("$INIT$",     jsInit,   TRUE, 0);
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
    : PConfigPage(app,title,section,auth), cfg(section)
{
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
#if PTLIB_MAJOR == 2 && PTLIB_MINOR > 0
      PStringArray names;
#else
      PStringList names;
#endif
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
#if PTLIB_MAJOR == 2 && PTLIB_MINOR > 0
      PStringArray names;
#else
      PStringList names;
#endif
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

  if(cfg.GetBoolean("RESTORE DEFAULTS", FALSE))
    cfg.DeleteSection();

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
    : TablePConfigPage(app,title,section,auth)
{
  OpenMCU & mcu = OpenMCU::Current();

  PStringStream html_begin, html_end, html_page, s;
  s << BeginTable();

  // Reset section
  s << BoolField("RESTORE DEFAULTS", JsLocal("restore_defaults"), FALSE);
  // Language
  //s << SelectField("Language", JsLocal("language"), cfg.GetString("Language"), ",EN,FR,JP,PT,RU,UK");
  s << SelectField("Language", "Select Language", cfg.GetString("Language"), ",EN,FR,JP,PT,RU,UK");
  // MCU Server Id
  s << StringField(ServerIdKey, JsLocal("server_id"), OpenMCU::Current().GetServerId(), 250);

  s << SelectField(DefaultProtocolKey, JsLocal("default_protocol_for_outgoing_calls"), cfg.GetString(DefaultProtocolKey, "sip"), "h323,sip");

  s << SeparatorField("");
  s << SeparatorField("");
  // HTTP Port number to use.
  s << StringField(HttpIPKey, JsLocal("http_ip"), cfg.GetString(HttpIPKey, "0.0.0.0"));
  s << IntegerField(HttpPortKey, JsLocal("http_port"), cfg.GetInteger(HttpPortKey, DefaultHTTPPort), 1, 32767);
#if P_SSL
  s << BoolField(HTTPSecureKey, JsLocal("http_secure"), cfg.GetBoolean(HTTPSecureKey, FALSE));
  s << StringField(HTTPCertificateFileKey, JsLocal("http_certificate"), cfg.GetString(HTTPCertificateFileKey, DefaultHTTPCertificateFile), 250);
#endif

  s << SeparatorField("");
  s << SeparatorField("");
  // RTP Port Setup
  s << IntegerField(RTPPortBaseKey, JsLocal("rtp_base_port"), cfg.GetInteger(RTPPortBaseKey, 0), 0, 65535, 0, "0 = auto, Example: base=5000, max=6000");
  s << IntegerField(RTPPortMaxKey, JsLocal("rtp_max_port"), cfg.GetInteger(RTPPortMaxKey, 0), 0, 65535);

  s << SeparatorField("");
  s << SeparatorField("");
#if PTRACING
  // Trace level
  s << SelectField(TraceLevelKey, JsLocal("trace_level"), cfg.GetString(TraceLevelKey, DEFAULT_TRACE_LEVEL), "0,1,2,3,4,5,6", 0, "0=No tracing ... 6=Very detailed");
  s << IntegerField(TraceRotateKey, JsLocal("rotate_trace"), cfg.GetInteger(TraceRotateKey, 0), 0, 250, 0, "0 (don't rotate) ... 200");
#endif
#ifdef SERVER_LOGS
  // Log level for messages
  s << SelectField(LogLevelKey, JsLocal("log_level"), cfg.GetString(LogLevelKey, DEFAULT_LOG_LEVEL), "0,1,2,3,4,5", 0, "1=Fatal only, 2=Errors, 3=Warnings, 4=Info, 5=Debug");
  // Log filename
  s << StringField(CallLogFilenameKey, JsLocal("call_log_filename"), cfg.GetString(CallLogFilenameKey, DefaultCallLogFilename), 250);
#endif
  // Buffered events
  s << IntegerField(HttpLinkEventBufferKey, JsLocal("room_control_event_buffer_size"), cfg.GetInteger(HttpLinkEventBufferKey, 100), 10, 1000, 0, "range: 10...1000");
  // Copy web log from Room Control Page to call log
  s << BoolField("Copy web log to call log", JsLocal("copy_web_log"), cfg.GetBoolean("Copy web log to call log", FALSE), "check if you want to store event log from Room Control Page");

  s << SeparatorField("");
  s << SeparatorField("");
  // Default room
  if(cfg.GetString(DefaultRoomKey) == "")
    cfg.SetString(DefaultRoomKey, DefaultRoom);
  s << AccountField(DefaultRoomKey, JsLocal("default_room"), cfg.GetString(DefaultRoomKey, DefaultRoom));
  // reject duplicate name
  s << BoolField(RejectDuplicateNameKey, JsLocal("reject_duplicate_name"), cfg.GetBoolean(RejectDuplicateNameKey, FALSE));
  // allow/disallow self-invite:
  s << BoolField(AllowLoopbackCallsKey, JsLocal("allow_loopback_calls"), cfg.GetBoolean(AllowLoopbackCallsKey, FALSE));
  // auto dial delay:
  s << SelectField(AutoDialDelayKey, JsLocal("auto_dial_delay"), cfg.GetString(AutoDialDelayKey, cfg.GetString(AutoDialDelayKey,1)), "1,2,3,5,8,10,12,15,20,25,30,45,60,90,120,150,180,300,X");

  s << SeparatorField("");
  s << SeparatorField("");
  PString dirInfo;
  if(!PDirectory::Exists(mcu.vr_ffmpegDir) && !PFile::Exists(mcu.vr_ffmpegDir))
  {
    PDirectory::Create(mcu.vr_ffmpegDir,0700);
    MCUTime::Sleep(50);
  }
  if(!PDirectory::Exists(mcu.vr_ffmpegDir))
    dirInfo += "<b><font color=red>Directory does not exist: "+mcu.vr_ffmpegDir+"</font></b>";
  else
  {
    PFileInfo info;
    PFilePath path(mcu.vr_ffmpegDir);
    PFile::GetInfo(path, info);
    if(!(info.type & 6)) dirInfo += "<b><font color=red>Warning: output directory neither directory, nor symlink!</font></b>";
    else if(!(info.permissions & 0222)) dirInfo += "<b><font color=red>output directory permissions check failed</font></b>";
  }

  s << StringField(RecorderFfmpegDirKey, JsLocal("param_record")+": "+JsLocal("directory"), mcu.vr_ffmpegDir, 250, dirInfo);
  s << SelectField(RecorderVideoCodecKey, JsLocal("param_record")+": "+JsLocal("name_video_codec"), cfg.GetString(RecorderVideoCodecKey, RecorderDefaultVideoCodec), GetRecorderCodecs(1));

  // bak 2014.10.20 ////////////////////////////////////
  PString RecorderFrameWidthKey  = "Video Recorder frame width";
  PString RecorderFrameHeightKey = "Video Recorder frame height";
  if(cfg.HasKey(RecorderFrameWidthKey) && cfg.HasKey(RecorderFrameHeightKey))
  {
    int width = cfg.GetInteger(RecorderFrameWidthKey);
    int height = cfg.GetInteger(RecorderFrameHeightKey);
    cfg.SetString(RecorderResolutionKey, PString(width)+"x"+PString(height));
    cfg.DeleteKey(RecorderFrameWidthKey);
    cfg.DeleteKey(RecorderFrameHeightKey);
  }
  //////////////////////////////////////////////////////

  PString resolutions;
  for(int i = 0; recorder_resolutions[i].macroblocks != 0; ++i)
    resolutions += ","+PString(recorder_resolutions[i].width)+"x"+PString(recorder_resolutions[i].height);
  PString res = cfg.GetString(RecorderResolutionKey, PString(DefaultRecorderFrameWidth)+"x"+PString(DefaultRecorderFrameHeight));
  s << SelectField(RecorderResolutionKey, JsLocal("param_record")+": "+JsLocal("name_video_resolution"), res, resolutions);

  s << IntegerField(RecorderFrameRateKey, JsLocal("param_record")+": "+JsLocal("video_frame_rate"), cfg.GetInteger(RecorderFrameRateKey, DefaultRecorderFrameRate), 1, 30);
  s << IntegerField(RecorderVideoBitrateKey, JsLocal("param_record")+": "+JsLocal("video_bitrate"), cfg.GetInteger(RecorderVideoBitrateKey), 0, 4000, 0, "kbit/s, 0 - auto");
  s << SelectField(RecorderAudioCodecKey, JsLocal("param_record")+": "+JsLocal("name_audio_codec"), cfg.GetString(RecorderAudioCodecKey, RecorderDefaultAudioCodec), GetRecorderCodecs(0));
  s << SelectField(RecorderSampleRateKey, JsLocal("param_record")+": "+JsLocal("audio_sample_rate"), cfg.GetString(RecorderSampleRateKey, DefaultRecorderSampleRate), "8000,16000,32000,48000");
  s << SelectField(RecorderAudioChansKey, JsLocal("param_record")+": "+JsLocal("audio_channels"), cfg.GetString(RecorderAudioChansKey, DefaultRecorderAudioChans), "1,2,3,4,5,6,7,8");

  // get WAV file played to a user when they enter a conference
  //s << StringField(ConnectingWAVFileKey, cfg.GetString(ConnectingWAVFileKey, DefaultConnectingWAVFile));
  // get WAV file played to a conference when a new user enters
  //s << StringField(EnteringWAVFileKey, cfg.GetString(EnteringWAVFileKey, DefaultEnteringWAVFile));
  // get WAV file played to a conference when a new user enters
  //s << StringField(LeavingWAVFileKey, cfg.GetString(LeavingWAVFileKey, DefaultLeavingWAVFile));

  s << EndTable();
  BuildHTML("");
  BeginPage(html_begin, section, "window.l_param_general","window.l_info_param_general");
  EndPage(html_end,OpenMCU::Current().GetHtmlCopyright());
  html_page << html_begin << s << html_end;
  string = html_page;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

ConferencePConfigPage::ConferencePConfigPage(PHTTPServiceProcess & app,const PString & title, const PString & section, const PHTTPAuthority & auth)
    : TablePConfigPage(app,title,section,auth)
{
  firstEditRow = 2;
  PStringStream html_begin, html_end, html_page, s;
  buttonUp = buttonDown = buttonClone = buttonDelete = 1;
  s << BeginTable();

  s << NewRowColumn(JsLocal("name_roomname"));
  s << ColumnItem(JsLocal("name_auto_create"));
  s << ColumnItem(JsLocal("room_auto_create_when_connecting"));
  s << ColumnItem(JsLocal("name_force_split_video"));
  s << ColumnItem(JsLocal("name_auto_delete_empty"));
  s << ColumnItem(JsLocal("name_auto_record_start"));
  s << ColumnItem(JsLocal("name_auto_record_stop"));
  s << ColumnItem(JsLocal("name_recall_last_template"));
  s << ColumnItem(JsLocal("lock_tpl_default"));
  s << ColumnItem(JsLocal("name_time_limit"));
  optionNames.AppendString(RoomAutoCreateKey);
  optionNames.AppendString(RoomAutoCreateWhenConnectingKey);
  optionNames.AppendString(ForceSplitVideoKey);
  optionNames.AppendString(RoomAutoDeleteEmptyKey);
  optionNames.AppendString(RoomAutoRecordStartKey);
  optionNames.AppendString(RoomAutoRecordStopKey);
  optionNames.AppendString(RoomRecallLastTemplateKey);
  optionNames.AppendString(LockTemplateKey);
  optionNames.AppendString(RoomTimeLimitKey);

  sectionPrefix = "Conference ";
  PStringList sect = cfg.GetSectionsPrefix(sectionPrefix);

  //
  if(sect.GetStringsIndex(sectionPrefix+"*") == P_MAX_INDEX)
    sect.InsertAt(0, new PString(sectionPrefix+"*"));
  if(sect.GetSize() == 1)
    sect.AppendString(sectionPrefix+OpenMCU::Current().GetDefaultRoomName());

  for(PINDEX i = 0; i < sect.GetSize(); i++)
  {
    MCUConfig scfg(sect[i]);
    PString name = sect[i].Right(sect[i].GetLength()-sectionPrefix.GetLength());

    //
    if(name == "*") s << NewRowInput(name, 0, TRUE); else s << NewRowInputAccount(name);

    // auto create
    if(name == "*") s << EmptyInputItem(name);
    else            s << SelectItem(name, scfg.GetString(RoomAutoCreateKey, ""), ",Enable,Disable");
    // auto create when connecting
    if(name == "*") s << SelectItem(name, scfg.GetString(RoomAutoCreateWhenConnectingKey, "Enable"), "Enable,Disable");
    else            s << SelectItem(name, scfg.GetString(RoomAutoCreateWhenConnectingKey, ""), ",Enable,Disable");
    // split
    if(name == "*") s << SelectItem(name, scfg.GetString(ForceSplitVideoKey, "Enable"), "Enable,Disable");
    else            s << SelectItem(name, scfg.GetString(ForceSplitVideoKey, ""), ",Enable,Disable");
    // auto delete
    if(name == "*") s << SelectItem(name, scfg.GetString(RoomAutoDeleteEmptyKey, "Disable"), "Enable,Disable");
    else            s << SelectItem(name, scfg.GetString(RoomAutoDeleteEmptyKey, ""), ",Enable,Disable");
    // auto record
    // bak 12.11.2014 ///////////////////////////////////////////////////////////////
    if(scfg.GetString("Auto record not empty") == "Enable")
    {
      scfg.SetString(RoomAutoRecordStartKey, "1");
      scfg.SetString(RoomAutoRecordStopKey, "0");
    }
    /////////////////////////////////////////////////////////////////////////////////
    PString select_autorecord = "Disable";
    for(PINDEX i = 0; i < 100; i++)
      select_autorecord += ","+PString(i);
    if(name != "*")
      select_autorecord = "," + select_autorecord;
    s << SelectItem(name, scfg.GetString(RoomAutoRecordStartKey), select_autorecord);
    s << SelectItem(name, scfg.GetString(RoomAutoRecordStopKey), select_autorecord);
    // recall last template after room created
    if(name == "*") s << SelectItem(name, scfg.GetString(RoomRecallLastTemplateKey, "Disable"), "Enable,Disable");
    else            s << SelectItem(name, scfg.GetString(RoomRecallLastTemplateKey, ""), ",Enable,Disable");
    // lock template
    if(name == "*") s << SelectItem(name, scfg.GetString(LockTemplateKey, "Disable"), "Enable,Disable");
    else            s << SelectItem(name, scfg.GetString(LockTemplateKey, ""), ",Enable,Disable");
    // time limit
    s << IntegerItem(name, scfg.GetString(RoomTimeLimitKey, ""), 0, 86400);
  }

  s << EndTable();
  BuildHTML("");
  BeginPage(html_begin, section, "window.l_param_conference", "window.l_info_param_conference");
  EndPage(html_end,OpenMCU::Current().GetHtmlCopyright());
  html_page << html_begin << s << html_end;
  string = html_page;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

TelnetServerPConfigPage::TelnetServerPConfigPage(PHTTPServiceProcess & app, const PString & title, const PString & section, const PHTTPAuthority & auth)
  : TablePConfigPage(app,title,section,auth)
{
  PStringStream html_begin, html_end, html_page, s;
  s << BeginTable();

  s << BoolField("RESTORE DEFAULTS", JsLocal("restore_defaults"), FALSE);

  s << BoolField(EnableKey, "Telnet "+JsLocal("name_enable"), cfg.GetBoolean(EnableKey, TRUE));
  s << ArrayField(TelnetListenerKey, "Telnet "+JsLocal("listener"), cfg.GetString(TelnetListenerKey, TelnetDefaultListener), 150);
  s << StringField(UserNameKey, JsLocal("name_user"), cfg.GetString(UserNameKey, "admin"), 150);
  s << StringField(PasswordKey, JsLocal("name_password"), cfg.GetString(PasswordKey), 150);

  s << EndTable();
  BuildHTML("");
  BeginPage(html_begin, section, "window.l_param_telserver","window.l_info_telserver");
  EndPage(html_end,OpenMCU::Current().GetHtmlCopyright());
  html_page << html_begin << s << html_end;
  string = html_page;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

RegistrarPConfigPage::RegistrarPConfigPage(PHTTPServiceProcess & app,const PString & title, const PString & section, const PHTTPAuthority & auth)
    : TablePConfigPage(app,title,section,auth)
{
  PStringStream html_begin, html_end, html_page, s;
  s << BeginTable();

  // bak 2014.06.03 ////////////////////////////////////
  if(cfg.HasKey("SIP registrar required password authorization"))
  {
    cfg.SetBoolean("SIP allow registration without authentication", !cfg.GetBoolean("SIP registrar required password authorization"));
    cfg.DeleteKey("SIP registrar required password authorization");
  }
  if(cfg.HasKey("SIP allow unauthorized MCU calls"))
  {
    cfg.SetBoolean("SIP allow MCU calls without authentication", cfg.GetBoolean("SIP allow unauthorized MCU calls"));
    cfg.DeleteKey("SIP allow unauthorized MCU calls");
  }
  if(cfg.HasKey("SIP allow unauthorized internal calls"))
  {
    cfg.SetBoolean("SIP allow internal calls without authentication", cfg.GetBoolean("SIP allow unauthorized internal calls"));
    cfg.DeleteKey("SIP allow unauthorized internal calls");
  }
  if(cfg.HasKey("H.323 gatekeeper required password authorization"))
  {
    cfg.SetBoolean("H.323 allow registration without authentication", !cfg.GetBoolean("H.323 gatekeeper required password authorization"));
    cfg.DeleteKey("H.323 gatekeeper required password authorization");
  }
  if(cfg.HasKey("H.323 allow unregistered MCU calls"))
  {
    cfg.SetBoolean("H.323 allow MCU calls without registration", cfg.GetBoolean("H.323 allow unregistered MCU calls"));
    cfg.DeleteKey("H.323 allow unregistered MCU calls");
  }
  if(cfg.HasKey("H.323 allow unregistered internal calls"))
  {
    cfg.SetBoolean("H.323 allow internal calls without registration", cfg.GetBoolean("H.323 allow unregistered internal calls"));
    cfg.DeleteKey("H.323 allow unregistered internal calls");
  }
  //////////////////////////////////////////////////////

  s << BoolField("RESTORE DEFAULTS", JsLocal("restore_defaults"), FALSE);
  s << BoolField("Allow internal calls", JsLocal("allow_internal_calls"), cfg.GetBoolean("Allow internal calls", TRUE));

  s << SeparatorField("SIP");
  s << BoolField("SIP allow registration without authentication", JsLocal("sip_allow_reg_without_auth"), cfg.GetBoolean("SIP allow registration without authentication", TRUE));
  s << BoolField("SIP allow MCU calls without authentication", JsLocal("sip_allow_mcu_calls_without_auth"), cfg.GetBoolean("SIP allow MCU calls without authentication", TRUE));
  s << BoolField("SIP allow internal calls without authentication", JsLocal("sip_allow_internal_calls_without_auth"), cfg.GetBoolean("SIP allow internal calls without authentication", TRUE));
  s << SelectField("SIP registrar minimum expiration", JsLocal("sip_registrar_minimum_expiration"), cfg.GetString("SIP registrar minimum expiration", "60"), "60,120,180,240,300,600,1200,1800,2400,3000,3600");
  s << SelectField("SIP registrar maximum expiration", JsLocal("sip_registrar_maximum_expiration"), cfg.GetString("SIP registrar maximum expiration", "600"), "60,120,180,240,300,600,1200,1800,2400,3000,3600");

  s << SeparatorField("H.323");
  s << BoolField("H.323 gatekeeper enable", JsLocal("h323_gatekeeper_enable"), cfg.GetBoolean("H.323 gatekeeper enable", TRUE));
  s << BoolField("H.323 allow registration without authentication", JsLocal("h323_allow_reg_without_auth"), cfg.GetBoolean("H.323 allow registration without authentication", TRUE));
  s << BoolField("H.323 allow MCU calls without registration", JsLocal("h323_allow_mcu_calls_without_reg"), cfg.GetBoolean("H.323 allow MCU calls without registration", TRUE));
  s << BoolField("H.323 allow internal calls without registration", JsLocal("h323_allow_internal_calls_without_reg"), cfg.GetBoolean("H.323 allow internal calls without registration", TRUE));
  s << BoolField("H.323 allow duplicate aliases", JsLocal("h323_allow_duplicate_aliases"), cfg.GetBoolean("H.323 allow duplicate aliases", FALSE));
  s << SelectField("H.323 gatekeeper minimum Time To Live", JsLocal("h323_gatekeeper_minimum_ttl"), cfg.GetString("H.323 gatekeeper minimum Time To Live", "60"), "60,120,180,240,300,600,1200,1800,2400,3000,3600");
  s << SelectField("H.323 gatekeeper maximum Time To Live", JsLocal("h323_gatekeeper_maximum_ttl"), cfg.GetString("H.323 gatekeeper maximum Time To Live", "600"), "60,120,180,240,300,600,1200,1800,2400,3000,3600");

  s << EndTable();
  BuildHTML("");
  BeginPage(html_begin, section, "window.l_param_registrar","window.l_info_param_registrar");
  EndPage(html_end,OpenMCU::Current().GetHtmlCopyright());
  html_page << html_begin << s << html_end;
  string = html_page;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

ControlCodesPConfigPage::ControlCodesPConfigPage(PHTTPServiceProcess & app,const PString & title, const PString & section, const PHTTPAuthority & auth)
    : TablePConfigPage(app,title,section,auth)
{
  PStringStream html_begin, html_end, html_page, s;
  buttonUp = buttonDown = buttonClone = buttonDelete = 1;
  s << BeginTable();

  s << NewRowColumn(JsLocal("name_code"));
  s << ColumnItem(JsLocal("name_action"));
  s << ColumnItem(JsLocal("name_message"));

  PStringList keys = cfg.GetKeys();
  for(PINDEX i = 0; i < keys.GetSize(); i++)
  {
    PString name = keys[i];
    PStringArray params = cfg.GetString(keys[i]).Tokenise(",");
    s << NewRowInput(name);
    s << SelectItem(name, params[0], MCUControlCodes, 250);
    s << StringItem(name, params[1], 250);
  }
  if(keys.GetSize() == 0)
  {
    s << NewRowInput("1");
    s << SelectItem("1", "", MCUControlCodes, 250);
    s << StringItem("1", "", 250);
  }

  s << EndTable();
  BuildHTML("");
  BeginPage(html_begin, "Control Codes", "window.l_param_control_codes", "window.l_info_param_control_codes");
  EndPage(html_end,OpenMCU::Current().GetHtmlCopyright());
  html_page << html_begin << s << html_end;
  string = html_page;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

RoomCodesPConfigPage::RoomCodesPConfigPage(PHTTPServiceProcess & app,const PString & title, const PString & section, const PHTTPAuthority & auth)
    : TablePConfigPage(app,title,section,auth)
{
  PStringStream html_begin, html_end, html_page, s;
  buttonUp = buttonDown = buttonClone = buttonDelete = 1;
  s << BeginTable();

  s << NewRowColumn(JsLocal("name_code"));
  s << ColumnItem(JsLocal("name_roomname"));

  PStringList keys = cfg.GetKeys();
  for(PINDEX i = 0; i < keys.GetSize(); i++)
  {
    PString name = keys[i];
    PStringArray params = cfg.GetString(keys[i]).Tokenise(",");
    s << NewRowInput(name);
    s << StringItem(name, params[0]);
  }
  if(keys.GetSize() == 0)
  {
    s << NewRowInput("1");
    s << StringItem("1", OpenMCU::Current().GetDefaultRoomName());
  }

  s << EndTable();
  BuildHTML("");
  BeginPage(html_begin, "Room Codes", "window.l_param_room_codes", "window.l_info_param_room_codes");
  EndPage(html_end,OpenMCU::Current().GetHtmlCopyright());
  html_page << html_begin << s << html_end;
  string = html_page;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

ManagingUsersPConfigPage::ManagingUsersPConfigPage(PHTTPServiceProcess & app,const PString & title, const PString & section, const PHTTPAuthority & auth)
    : TablePConfigPage(app,title,section,auth)
{
  PStringStream html_begin, html_end, html_page, s;
  buttonUp = buttonDown = buttonClone = buttonDelete = 1;
  s << BeginTable();

  s << NewRowColumn(JsLocal("name_user"));
  s << ColumnItem(JsLocal("name_password"));
  s << ColumnItem(JsLocal("name_group"));

  PString groups;
  PStringList keysGroups = MCUConfig("Managing Groups").GetKeys();
  for(PINDEX i = 0; i < keysGroups.GetSize(); i++)
  {
    if(i != 0) groups += ",";
    groups += keysGroups[i];
  }

  PStringList keys = cfg.GetKeys();
  for(PINDEX i = 0; i < keys.GetSize(); i++)
  {
    PString name = keys[i];
    PStringArray params = cfg.GetString(keys[i]).Tokenise(",");
    s << NewRowInput(name);
    s << PasswordItem(name, params[0]);
    s << SelectItem(name, params[1], groups, 250);
  }
  if(keys.GetSize() == 0)
  {
    s << NewRowInput("admin");
    s << PasswordItem("admin", "");
    s << SelectItem("admin", "administrator", groups, 250);
  }

  s << EndTable();
  BuildHTML("");
  BeginPage(html_begin, "Managing Users", "window.l_param_managing_users", "window.l_info_param_managing_users");
  EndPage(html_end,OpenMCU::Current().GetHtmlCopyright());
  html_page << html_begin << s << html_end;
  string = html_page;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

ManagingGroupsPConfigPage::ManagingGroupsPConfigPage(PHTTPServiceProcess & app,const PString & title, const PString & section, const PHTTPAuthority & auth)
    : TablePConfigPage(app,title,section,auth)
{
  PStringStream html_begin, html_end, html_page, s;
  s << BeginTable();

  s << NewRowColumn(JsLocal("name_group"));

  PStringList keys = cfg.GetKeys();
  for(PINDEX i = 0; i < keys.GetSize(); i++)
  {
    PString name = keys[i];
    s << NewRowText(name);
    s << InfoItem("");
  }

  s << EndTable();
  BuildHTML("");
  BeginPage(html_begin, "Managing Groups", "window.l_param_managing_groups", "window.l_info_param_managing_groups");
  EndPage(html_end,OpenMCU::Current().GetHtmlCopyright());
  html_page << html_begin << s << html_end;
  string = html_page;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

VideoPConfigPage::VideoPConfigPage(PHTTPServiceProcess & app,const PString & title, const PString & section, const PHTTPAuthority & auth)
    : TablePConfigPage(app,title,section,auth)
{
  PStringStream html_begin, html_end, html_page, s;
  s << BeginTable();

  s << BoolField("RESTORE DEFAULTS", JsLocal("restore_defaults"), FALSE);

#if MCU_VIDEO
  s << BoolField("Enable video", JsLocal("enable_video"), cfg.GetBoolean("Enable video", TRUE));
#endif
  s << IntegerField("Video frame rate", JsLocal("video_frame_rate"), cfg.GetString("Video frame rate", DefaultVideoFrameRate), 1, MCU_MAX_FRAME_RATE, 0, "range: 1.."+PString(MCU_MAX_FRAME_RATE)+" (for outgoing video)");

  s << SeparatorField("H.263");
  s << IntegerField("H.263 Max Bit Rate", "H.263 "+JsLocal("max_bit_rate"), cfg.GetString("H.263 Max Bit Rate"), MCU_MIN_BIT_RATE/1000, MCU_MAX_BIT_RATE/1000, 0, "range "+PString(MCU_MIN_BIT_RATE/1000)+".."+PString(MCU_MAX_BIT_RATE/1000)+" kbit (for outgoing video, 0 disable)");
  s << IntegerField("H.263 Tx Key Frame Period", "H.263 "+JsLocal("tx_key_frame_period"), cfg.GetString("H.263 Tx Key Frame Period"), 0, 600, 0, "range 0..600 (for outgoing video, the number of pictures in a group of pictures, or 0 for intra_only)");

  s << SeparatorField("H.263p");
  s << IntegerField("H.263p Max Bit Rate", "H.263p "+JsLocal("max_bit_rate"), cfg.GetString("H.263p Max Bit Rate"), MCU_MIN_BIT_RATE/1000, MCU_MAX_BIT_RATE/1000, 0, "range "+PString(MCU_MIN_BIT_RATE/1000)+".."+PString(MCU_MAX_BIT_RATE/1000)+" kbit (for outgoing video, 0 disable)");
  s << IntegerField("H.263p Tx Key Frame Period", "H.263p "+JsLocal("tx_key_frame_period"), cfg.GetString("H.263p Tx Key Frame Period"), 0, 600, 0, "range 0..600, default 125 (for outgoing video, the number of pictures in a group of pictures, or 0 for intra_only)");

  s << SeparatorField("H.264");
  s << IntegerField("H.264 Max Bit Rate", "H.264 "+JsLocal("max_bit_rate"), cfg.GetString("H.264 Max Bit Rate"), MCU_MIN_BIT_RATE/1000, MCU_MAX_BIT_RATE/1000, 0, "range "+PString(MCU_MIN_BIT_RATE/1000)+".."+PString(MCU_MAX_BIT_RATE/1000)+" kbit (for outgoing video, 0 disable)");
  s << IntegerField("H.264 Encoding Threads", "H.264 "+JsLocal("encoding_threads"), cfg.GetString("H.264 Encoding Threads"), 0, 64, 0, "range 0..64 (0 auto)");

  s << SeparatorField("VP8");
  s << IntegerField("VP8 Max Bit Rate", "VP8 "+JsLocal("max_bit_rate"), cfg.GetString("VP8 Max Bit Rate"), MCU_MIN_BIT_RATE/1000, MCU_MAX_BIT_RATE/1000, 0, "range "+PString(MCU_MIN_BIT_RATE/1000)+".."+PString(MCU_MAX_BIT_RATE/1000)+" kbit (for outgoing video, 0 disable)");
  s << IntegerField("VP8 Encoding Threads", "VP8 "+JsLocal("encoding_threads"), cfg.GetString("VP8 Encoding Threads"), 0, 64, 0, "range 0..64 (0 default)");
  s << IntegerField("VP8 Encoding CPU Used", "VP8 "+JsLocal("encoding_cpu_used"), cfg.GetString("VP8 Encoding CPU Used"), 0, 16, 0, "range: 0..16 (Values greater than 0 will increase encoder speed at the expense of quality)");

  s << EndTable();
  BuildHTML("");
  BeginPage(html_begin, "Video settings", "window.l_param_video", "window.l_info_param_video");
  EndPage(html_end,OpenMCU::Current().GetHtmlCopyright());
  html_page << html_begin << s << html_end;
  string = html_page;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

ExportPConfigPage::ExportPConfigPage(PHTTPServiceProcess & app,const PString & title, const PString & section, const PHTTPAuthority & auth)
    : TablePConfigPage(app,title,section,auth)
{
  PStringStream html_begin, html_end, html_page, s;
  s << BeginTable();

  s << BoolField("RESTORE DEFAULTS", JsLocal("restore_defaults"), FALSE);

  s << BoolField("Enable export", JsLocal("enable_export"), cfg.GetBoolean("Enable export", FALSE));
  s << IntegerField(VideoFrameWidthKey, JsLocal("video_frame_width"), cfg.GetInteger(VideoFrameWidthKey, 704), 176, 1920);
  s << IntegerField(VideoFrameHeightKey, JsLocal("video_frame_height"), cfg.GetInteger(VideoFrameHeightKey, 576), 144, 1152);
  s << IntegerField(VideoFrameRateKey, JsLocal("video_frame_rate"), cfg.GetInteger(VideoFrameRateKey, 10), 1, 30);
  s << SelectField(AudioSampleRateKey, JsLocal("audio_sample_rate"), cfg.GetInteger(AudioSampleRateKey, 16000), "8000,16000,32000,48000");
  s << SelectField(AudioChannelsKey, JsLocal("audio_channels"), cfg.GetInteger(AudioChannelsKey, 1), "1,2,3,4,5,6,7,8");

  s << EndTable();
  BuildHTML("");
  BeginPage(html_begin, "Export settings", "", "");
  EndPage(html_end,OpenMCU::Current().GetHtmlCopyright());
  html_page << html_begin << s << html_end;
  string = html_page;
}

///////////////////////////////////////////////////////////////

// bak 13.06.2014, restore resolution from capability
// remove it when not needed
void VideoResolutionRestore(PString & capname, PString & res)
{
  if(capname.Find("H.264")==0)
  {
    for(int i = 0; h264_profile_levels[i].level != 0; ++i)
    {
      if(capname != PString(h264_profile_levels[i].capname)+"{sw}")
        continue;
      unsigned macroblocks = h264_profile_levels[i].max_fs;
      for(int j = 0; h264_resolutions[j].macroblocks != 0; ++j)
      {
        if(macroblocks < h264_resolutions[j].macroblocks)
          continue;
        res = PString(h264_resolutions[j].width)+"x"+PString(h264_resolutions[j].height);
        capname = "H.264{sw}";
        return;
      }
    }
  }
  else if(capname.Find("H.261")==0 || capname.Find("H.263")==0)
  {
    PString name = capname;
    name.Replace("{sw}","",TRUE,0);
    capname = name.Tokenise("-")[0]+"{sw}";
    name = name.Tokenise("-")[1];
    if(name == "SQCIF") { res = "128x96"; return; }
    if(name == "QCIF") { res = "176x144"; return; }
    if(name == "CIF") { res = "352x288"; return; }
    if(name == "4CIF") { res = "704x576"; return; }
    if(name == "16CIF") { res = "1408x1152"; return; }
  }
  else if(capname.Find("VP8")==0)
  {
    for(int i = 0; vp8_resolutions[i].width != 0; ++i)
    {
      if(capname != PString(vp8_resolutions[i].capname)+"{sw}")
        continue;
      res = PString(vp8_resolutions[i].width)+"x"+PString(vp8_resolutions[i].height);
      capname = "VP8{sw}";
    }
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

H323EndpointsPConfigPage::H323EndpointsPConfigPage(PHTTPServiceProcess & app,const PString & title, const PString & section, const PHTTPAuthority & auth)
    : TablePConfigPage(app,title,section,auth)
{
  OpenMCU & mcu = OpenMCU::Current();

  firstEditRow = 2;
  rowBorders = TRUE;
  buttonShowAdvanced = TRUE;
  PStringStream html_begin, html_end, html_page, s;
  buttonUp = buttonDown = buttonClone = buttonDelete = 1;
  javascript += js_video_receive_res_toggle + js_video_transmit_res_toggle;

  s << BeginTable();

  s << NewRowColumn(JsLocal("name_user")+"<br>("+JsLocal("name_account")+")", 210);
  s << ColumnItem(JsLocal("settings"));
  s << ColumnItem("H.323");
  s << ColumnItem(JsLocal("name_video"));
  s << ColumnItem(JsLocal("name_codec"));

  optionNames.AppendString(RoomNameKey);
  optionNames.AppendString(DisplayNameKey);
  optionNames.AppendString(PasswordKey);
  optionNames.AppendString(PingIntervalKey);
  optionNames.AppendString("H.323 call processing");

  optionNames.AppendString(HostKey);
  optionNames.AppendString(PortKey);

  optionNames.AppendString(FrameRateFromKey);
  optionNames.AppendString(BandwidthFromKey);
  optionNames.AppendString(BandwidthToKey);
  optionNames.AppendString(RTPInputTimeoutKey);
  optionNames.AppendString(VideoCacheKey);
  optionNames.AppendString(ReceivedVFUDelayKey);

  optionNames.AppendString("Audio codec(receive)");
  optionNames.AppendString("Audio codec(transmit)");
  optionNames.AppendString("Video codec(receive)");
  optionNames.AppendString("Video resolution(receive)");
  optionNames.AppendString("Video codec(transmit)");
  optionNames.AppendString("Video resolution(transmit)");

  PString ra_caps = ",Disabled", rv_caps = ",Disabled", ta_caps = ",Disabled", tv_caps = ",Disabled";
  if(mcu.GetEndpoint().rsCaps != NULL)
  { for(PINDEX i=0; mcu.GetEndpoint().rsCaps[i]!=NULL; i++)
    { PString capname = mcu.GetEndpoint().rsCaps[i]; if(SkipCapability(capname, CONNECTION_TYPE_H323)) continue; ra_caps += ","+capname; } }
  if(mcu.GetEndpoint().rvCaps != NULL)
  { for(PINDEX i=0; mcu.GetEndpoint().rvCaps[i]!=NULL; i++)
    { PString capname = mcu.GetEndpoint().rvCaps[i]; if(SkipCapability(capname, CONNECTION_TYPE_H323)) continue; rv_caps += ","+capname; } }
  if(mcu.GetEndpoint().tsCaps != NULL)
  { for(PINDEX i=0; mcu.GetEndpoint().tsCaps[i]!=NULL; i++)
    { PString capname = mcu.GetEndpoint().tsCaps[i]; if(SkipCapability(capname, CONNECTION_TYPE_H323)) continue; ta_caps += ","+capname; } }
  if(mcu.GetEndpoint().tvCaps != NULL)
  { for(PINDEX i=0; mcu.GetEndpoint().tvCaps[i]!=NULL; i++)
    { PString capname = mcu.GetEndpoint().tvCaps[i]; if(SkipCapability(capname, CONNECTION_TYPE_H323)) continue; tv_caps += ","+capname; } }

  sectionPrefix = "H323 Endpoint ";
  PStringList sect = cfg.GetSectionsPrefix(sectionPrefix);

  // bak, temporarily
  if(sect.GetSize() == 0)
  {
    MCUConfig bcfg("H323 Endpoints");
    PStringList keys = bcfg.GetKeys();
    for(PINDEX i = 0; i < keys.GetSize(); i++)
    {
      PString account;
      PString host;
      if(keys[i].Find("@") != P_MAX_INDEX)
      {
        account = keys[i].Tokenise("@")[0];
        host = keys[i].Tokenise("@")[1];
      } else {
        if(keys[i] == "*")
        {
          account = keys[i];
        } else {
          account = rand()%10000;
          host = keys[i];
        }
      }
      MCUConfig scfg(sectionPrefix+account);
      PStringArray params = bcfg.GetString(keys[i]).Tokenise(",");
      scfg.SetString("Host", host);
      scfg.SetString("Port", params[1]);
      scfg.SetString("Display name", params[3]);
      scfg.SetString("Frame rate from MCU", params[4]);
      scfg.SetString("Bandwidth from MCU", params[5]);
      scfg.SetString("Bandwidth to MCU", params[6]);
      scfg.SetString("Audio codec(receive)", params[8]);
      scfg.SetString("Video codec(receive)", params[9]);
      scfg.SetString("Audio codec(transmit)", params[10]);
      scfg.SetString("Video codec(transmit)", params[11]);
      sect.AppendString(sectionPrefix+account);
    }
  }
  //
  if(sect.GetStringsIndex(sectionPrefix+"*") == P_MAX_INDEX)
    sect.InsertAt(0, new PString(sectionPrefix+"*"));
  if(sect.GetSize() == 1)
    sect.AppendString(sectionPrefix+"empty");

  for(PINDEX i = 0; i < sect.GetSize(); i++)
  {
    MCUConfig scfg(sect[i]);
    PString name = sect[i].Right(sect[i].GetLength()-sectionPrefix.GetLength());

    // options rename, temporarily
    if(scfg.GetString("Display name override") != "") { scfg.SetString("Display name", scfg.GetString("Display name override")); scfg.SetString("Display name override", ""); }
    if(scfg.GetString("Preferred frame rate from MCU") != "") { scfg.SetString("Frame rate from MCU", scfg.GetString("Preferred frame rate from MCU")); scfg.SetString("Preferred frame rate from MCU", ""); }
    if(scfg.GetString("Preferred bandwidth from MCU") != "") { scfg.SetString("Bandwidth from MCU", scfg.GetString("Preferred bandwidth from MCU")); scfg.SetString("Preferred bandwidth from MCU", ""); }
    if(scfg.GetString("Preferred bandwidth to MCU") != "") { scfg.SetString("Bandwidth to MCU", scfg.GetString("Preferred bandwidth to MCU")); scfg.SetString("Preferred bandwidth to MCU", ""); }

    // account
    if(name == "*") s << NewRowInput(name, 0, TRUE); else s << NewRowInputAccount(name);

    // settings
    if(name == "*")
    {
      PString s2;
      s2 += NewItemArray(name, 25);
      s2 += RowArray()+JsLocal("name_roomname")+StringItem(name, scfg.GetString(RoomNameKey))+"</tr>";
      s2 += RowArray()+EmptyInputItem(name)+"</tr>";
      s2 += RowArray()+EmptyInputItem(name)+"</tr>";
      s2 += RowArray()+"Keep-Alive "+JsLocal("interval")+SelectItem(name, scfg.GetString(PingIntervalKey, "Disable"), PingIntervalSelect)+"</tr>";
      s2 += RowArray()+JsLocal("internal_call_processing")+SelectItem(name, scfg.GetString("H.323 call processing", "direct"), "full,direct")+"</tr>";
      s2 += EndItemArray();
      s << s2;
    } else {
      PString s2;
      s2 += NewItemArray(name);
      s2 += RowArray()+JsLocal("name_roomname")+StringItem(name, scfg.GetString(RoomNameKey))+"</tr>";
      s2 += RowArray()+JsLocal("name_display_name")+StringItem(name, scfg.GetString(DisplayNameKey))+"</tr>";
      s2 += RowArray()+JsLocal("name_password")+StringItem(name, scfg.GetString(PasswordKey))+"</tr>";
      s2 += RowArray()+"Keep-Alive "+JsLocal("interval")+SelectItem(name, scfg.GetString(PingIntervalKey), ","+PingIntervalSelect)+"</tr>";
      s2 += RowArray()+JsLocal("internal_call_processing")+SelectItem(name, scfg.GetString("H.323 call processing", ""), ",full,direct")+"</tr>";
      s2 += EndItemArray();
      s << s2;
    }
    // H323
    {
      PString s2;
      s2 += NewItemArray(name, 25);
      //
      if(name == "*") s2 += RowArray()+JsLocal("name_host")+StringItem(name, "", 0, TRUE)+"</tr>";
      else            s2 += RowArray()+JsLocal("name_host")+StringItem(name, scfg.GetString(HostKey))+"</tr>";
      //
      s2 += RowArray()+"H.323 "+JsLocal("name_port")+IntegerItem(name, scfg.GetString(PortKey), 1, 65535)+"</tr>";
      //
      s2 += EndItemArray();
      s << s2;
    }
    // video
    {
      PString s2;
      s2 += NewItemArray(name, 25);
      // frame rate from MCU
      s2 += RowArray()+JsLocal("name_frame_rate_from_mcu")+IntegerItem(name, scfg.GetString(FrameRateFromKey), 1, MCU_MAX_FRAME_RATE, 70)+"</tr>";
      // bandwidth from MCU
      s2 += RowArray()+JsLocal("name_bandwidth_from_mcu")+IntegerItem(name, scfg.GetString(BandwidthFromKey), MCU_MIN_BIT_RATE/1000, MCU_MAX_BIT_RATE/1000, 70)+"</tr>";
      // bandwidth to MCU
      s2 += RowArray()+JsLocal("name_bandwidth_to_mcu")+IntegerItem(name, scfg.GetString(BandwidthToKey), MCU_MIN_BIT_RATE/1000, MCU_MAX_BIT_RATE/1000, 70)+"</tr>";
      // RTP timeout
      if(name == "*") s2 += RowArray()+JsLocal("rtp_input_timeout")+SelectItem(name, scfg.GetString(RTPInputTimeoutKey, DefaultRTPInputTimeout), RTPInputTimeoutSelect, 70)+"</tr>";
      else            s2 += RowArray()+JsLocal("rtp_input_timeout")+SelectItem(name, scfg.GetString(RTPInputTimeoutKey), ","+RTPInputTimeoutSelect, 70)+"</tr>";
      // Video cache
      if(name == "*") s2 += RowArray()+JsLocal("video_cache")+SelectItem(name, scfg.GetString(VideoCacheKey, EnableKey), EnableSelect, 70)+"</tr>";
      else            s2 += RowArray()+JsLocal("video_cache")+SelectItem(name, scfg.GetString(VideoCacheKey), ","+EnableSelect, 70)+"</tr>";
      // VFU delay
      if(name == "*") s2 += RowArray(name, TRUE)+JsLocal("received_vfu_delay")+SelectItem(name, scfg.GetString(ReceivedVFUDelayKey, DisableKey), ReceivedVFUDelaySelect, 70)+"</tr>";
      else            s2 += RowArray(name, TRUE)+JsLocal("received_vfu_delay")+SelectItem(name, scfg.GetString(ReceivedVFUDelayKey), ","+ReceivedVFUDelaySelect, 70)+"</tr>";
      //
      s2 += EndItemArray();
      s << s2;
    }
    // codecs
    {
      PString ra_codec = scfg.GetString("Audio codec(receive)");
      PString ta_codec = scfg.GetString("Audio codec(transmit)");
      PString rv_codec = scfg.GetString("Video codec(receive)");
      PString tv_codec = scfg.GetString("Video codec(transmit)");
      // bak 13.06.2014, restore resolution from capability
      if(SkipCapability(rv_codec, CONNECTION_TYPE_H323))
      {
        PString res;
        VideoResolutionRestore(rv_codec, res);
        scfg.SetString("Video codec(receive)", rv_codec);
        if(scfg.GetString("Video resolution(receive)") == "")
          scfg.SetString("Video resolution(receive)", res);
      }
      if(SkipCapability(tv_codec))
      {
        PString res;
        VideoResolutionRestore(tv_codec, res);
        scfg.SetString("Video codec(transmit)", tv_codec);
        if(scfg.GetString("Video resolution(transmit)") == "")
          scfg.SetString("Video resolution(transmit)", res);
      }
      ////////////////////////////////////////////////////
      if(ra_codec != "" && ra_caps.Find(ra_codec) == P_MAX_INDEX) ra_caps = ra_codec+","+ra_caps;
      if(ta_codec != "" && ta_caps.Find(ta_codec) == P_MAX_INDEX) ta_caps = ta_codec+","+ta_caps;
      if(rv_codec != "" && rv_caps.Find(rv_codec) == P_MAX_INDEX) rv_caps = rv_codec+","+rv_caps;
      if(tv_codec != "" && tv_caps.Find(tv_codec) == P_MAX_INDEX) tv_caps = tv_codec+","+tv_caps;

      PString s2;
      s2 += NewItemArray(name, 25);
      s2 += RowArray()+JsLocal("name_audio_receive")+SelectItem(name, ra_codec, ra_caps)+"</tr>";
      s2 += RowArray()+JsLocal("name_audio_transmit")+SelectItem(name, ta_codec, ta_caps)+"</tr>";
      //
      PString rv_id = GetTableId();
      PString rres_id = GetTableId();
      PString rres_value = scfg.GetString("Video resolution(receive)");
      PString rv_onchange = "video_receive_res_toggle(\""+rres_id+"\", this.value);";
      javascript += "video_receive_res_toggle('"+rres_id+"', '"+rv_codec+"');\n";
      s2 += RowArray()+JsLocal("name_video_receive")+SelectItem(name, rv_codec, rv_caps, 0, rv_id, rv_onchange)+"</tr>";
      s2 += RowArray()+JsLocal("name_video_resolution")+SelectItem(name, rres_value, rres_value, 0, rres_id)+"</tr>";
      //
      PString tv_id = GetTableId();
      PString tres_id = GetTableId();
      PString tres_value = scfg.GetString("Video resolution(transmit)");
      PString tv_onchange = "video_transmit_res_toggle(\""+tres_id+"\", this.value);";
      javascript += "video_transmit_res_toggle('"+tres_id+"', '"+tv_codec+"');\n";
      s2 += RowArray()+JsLocal("name_video_transmit")+SelectItem(name, tv_codec, tv_caps, 0, tv_id, tv_onchange)+"</tr>";
      s2 += RowArray()+JsLocal("name_video_resolution")+SelectItem(name, tres_value, tres_value, 0, tres_id)+"</tr>";
      s2 += EndItemArray();
      s << s2;
    }
    //
  }

  s << EndTable();

  BuildHTML("");
  BeginPage(html_begin, section, "window.l_param_h323_endpoints", "");
  EndPage(html_end,OpenMCU::Current().GetHtmlCopyright());
  html_page << html_begin << s << html_end;
  string = html_page;
}

///////////////////////////////////////////////////////////////

RtspEndpointsPConfigPage::RtspEndpointsPConfigPage(PHTTPServiceProcess & app,const PString & title, const PString & section, const PHTTPAuthority & auth)
    : TablePConfigPage(app,title,section,auth)
{
  firstEditRow = 1;
  PStringStream html_begin, html_end, html_page, s;
  buttonUp = buttonDown = buttonClone = buttonDelete = 1;

  s << BeginTable();
  s << NewRowColumn(JsLocal("name_address"));
  s << ColumnItem(JsLocal("name_user"));
  s << ColumnItem(JsLocal("name_password"));
  s << ColumnItem(JsLocal("name_display_name"));

  optionNames.AppendString(UserNameKey);
  optionNames.AppendString(PasswordKey);
  optionNames.AppendString(DisplayNameKey);

  sectionPrefix = "RTSP Endpoint ";
  PStringList sect = cfg.GetSectionsPrefix(sectionPrefix);

  if(sect.GetSize() == 0)
    sect.AppendString(sectionPrefix+"empty");

  for(PINDEX i = 0; i < sect.GetSize(); i++)
  {
    MCUConfig scfg(sect[i]);
    PString name = sect[i].Right(sect[i].GetLength()-sectionPrefix.GetLength());

    s << NewRowInput(name, 200);
    s << StringItem(name, scfg.GetString(UserNameKey), 120);
    s << StringItem(name, scfg.GetString(PasswordKey), 120);
    s << StringItem(name, scfg.GetString(DisplayNameKey), 120);

  }
  s << EndTable();

  BuildHTML("");
  BeginPage(html_begin, section, "window.l_param_rtsp_endpoints", "");
  EndPage(html_end,OpenMCU::Current().GetHtmlCopyright());
  html_page << html_begin << s << html_end;
  string = html_page;
}

///////////////////////////////////////////////////////////////

SipEndpointsPConfigPage::SipEndpointsPConfigPage(PHTTPServiceProcess & app,const PString & title, const PString & section, const PHTTPAuthority & auth)
    : TablePConfigPage(app,title,section,auth)
{
  firstEditRow = 2;
  rowBorders = TRUE;
  buttonShowAdvanced = TRUE;
  PStringStream html_begin, html_end, html_page, s;
  buttonUp = buttonDown = buttonClone = buttonDelete = 1;
  javascript += js_video_transmit_res_toggle;

  s << BeginTable();
  s << NewRowColumn(JsLocal("name_user")+"<br>("+JsLocal("name_account")+")", 210);
  s << ColumnItem(JsLocal("settings"));
  s << ColumnItem("SIP");
  s << ColumnItem(JsLocal("name_video"));
  s << ColumnItem(JsLocal("name_codec"));

  optionNames.AppendString(RoomNameKey);
  optionNames.AppendString(DisplayNameKey);
  optionNames.AppendString(PasswordKey);
  optionNames.AppendString(PingIntervalKey);
  optionNames.AppendString("SIP call processing");

  optionNames.AppendString(HostKey);
  optionNames.AppendString(PortKey);
  optionNames.AppendString(TransportKey);
  optionNames.AppendString(RtpProtoKey);
  optionNames.AppendString(NATRouterIPKey);
  optionNames.AppendString(NATStunServerKey);

  optionNames.AppendString(FrameRateFromKey);
  optionNames.AppendString(BandwidthFromKey);
  optionNames.AppendString(BandwidthToKey);
  optionNames.AppendString(RTPInputTimeoutKey);
  optionNames.AppendString(VideoCacheKey);
  optionNames.AppendString(ReceivedVFUDelayKey);

  optionNames.AppendString(AudioCodecKey);
  optionNames.AppendString(VideoCodecKey);
  optionNames.AppendString(VideoResolutionKey);
  optionNames.AppendString(VideoFmtpKey);

  PString a_caps = ",Disabled", v_caps = ",Disabled";
  PStringList keys = MCUConfig("SIP Audio").GetKeys();
  for(PINDEX i = 0; i < keys.GetSize(); i++)
  {
    if(keys[i].Right(4) == "fmtp" || keys[i].Right(7) == "payload")
      continue;
    if(SkipCapability(keys[i], CONNECTION_TYPE_SIP))
      continue;
    if(MCUConfig("SIP Audio").GetBoolean(keys[i])) a_caps += ","+keys[i];
  }
  keys = MCUConfig("SIP Video").GetKeys();
  for(PINDEX i = 0; i < keys.GetSize(); i++)
  {
    if(keys[i].Right(4) == "fmtp" || keys[i].Right(7) == "payload")
      continue;
    if(SkipCapability(keys[i], CONNECTION_TYPE_SIP))
      continue;
    PString capname = keys[i];
    if(MCUConfig("SIP Video").GetBoolean(keys[i])) v_caps += ","+capname;
  }


  sectionPrefix = "SIP Endpoint ";
  PStringList sect = cfg.GetSectionsPrefix(sectionPrefix);

  // bak
  if(sect.GetSize() == 0)
  {
    MCUConfig bcfg("SIP Endpoints");
    PStringList keys = bcfg.GetKeys();
    for(PINDEX i = 0; i < keys.GetSize(); i++)
    {
      PString account;
      PString host;
      if(keys[i].Find("@") != P_MAX_INDEX)
      {
        account = keys[i].Tokenise("@")[0];
        host = keys[i].Tokenise("@")[1];
      } else {
        if(keys[i] == "*")
        {
          account = keys[i];
        } else {
          account = rand()%10000;
          host = keys[i];
        }
      }
      MCUConfig scfg(sectionPrefix+account);
      PStringArray params = bcfg.GetString(keys[i]).Tokenise(",");
      scfg.SetString("Host", host);
      scfg.SetString("Port", params[1]);
      scfg.SetString("Transport", params[0].Tokenise("=")[1]);
      scfg.SetString("Display name", params[3]);
      scfg.SetString("Frame rate from MCU", params[4]);
      scfg.SetString("Bandwidth from MCU", params[5]);
      scfg.SetString("Bandwidth to MCU", params[6]);
      scfg.SetString("Audio codec", params[8]);
      scfg.SetString("Video codec", params[9]);
      sect.AppendString(sectionPrefix+account);
    }
  }
  //
  if(sect.GetStringsIndex(sectionPrefix+"*") == P_MAX_INDEX)
    sect.InsertAt(0, new PString(sectionPrefix+"*"));
  if(sect.GetSize() == 1)
    sect.AppendString(sectionPrefix+"empty");

  for(PINDEX i = 0; i < sect.GetSize(); i++)
  {
    MCUConfig scfg(sect[i]);
    PString name = sect[i].Right(sect[i].GetLength()-sectionPrefix.GetLength());

    // options rename, temporarily
    if(scfg.GetString("Display name override") != "") { scfg.SetString("Display name", scfg.GetString("Display name override")); scfg.SetString("Display name override", ""); }
    if(scfg.GetString("Preferred frame rate from MCU") != "") { scfg.SetString("Frame rate from MCU", scfg.GetString("Preferred frame rate from MCU")); scfg.SetString("Preferred frame rate from MCU", ""); }
    if(scfg.GetString("Preferred bandwidth from MCU") != "") { scfg.SetString("Bandwidth from MCU", scfg.GetString("Preferred bandwidth from MCU")); scfg.SetString("Preferred bandwidth from MCU", ""); }
    if(scfg.GetString("Preferred bandwidth to MCU") != "") { scfg.SetString("Bandwidth to MCU", scfg.GetString("Preferred bandwidth to MCU")); scfg.SetString("Preferred bandwidth to MCU", ""); }

    // account
    if(name == "*") s << NewRowInput(name, 0, TRUE); else s << NewRowInputAccount(name);

    // settings
    if(name == "*")
    {
      PString s2;
      s2 += NewItemArray(name, 25);
      s2 += RowArray()+JsLocal("name_roomname")+StringItem(name, scfg.GetString(RoomNameKey))+"</tr>";
      s2 += RowArray()+EmptyInputItem(name)+"</tr>";
      s2 += RowArray()+EmptyInputItem(name)+"</tr>";
      s2 += RowArray()+"Keep-Alive "+JsLocal("interval")+SelectItem(name, scfg.GetString(PingIntervalKey, "Disable"), PingIntervalSelect)+"</tr>";
      s2 += RowArray()+JsLocal("internal_call_processing")+SelectItem(name, scfg.GetString("SIP call processing", "redirect"), "full,redirect")+"</tr>";
      s2 += EndItemArray();
      s << s2;
    } else {
      PString s2;
      s2 += NewItemArray(name);
      s2 += RowArray()+JsLocal("name_roomname")+StringItem(name, scfg.GetString(RoomNameKey))+"</tr>";
      s2 += RowArray()+JsLocal("name_display_name")+StringItem(name, scfg.GetString(DisplayNameKey))+"</tr>";
      s2 += RowArray()+JsLocal("name_password")+StringItem(name, scfg.GetString(PasswordKey))+"</tr>";
      s2 += RowArray()+"Keep-Alive "+JsLocal("interval")+SelectItem(name, scfg.GetString(PingIntervalKey), ","+PingIntervalSelect)+"</tr>";
      s2 += RowArray()+JsLocal("internal_call_processing")+SelectItem(name, scfg.GetString("SIP call processing", ""), ",full,redirect")+"</tr>";
      s2 += EndItemArray();
      s << s2;
    }
    // SIP
    {
      PString s2;
      s2 += NewItemArray(name, 25);
      //
      if(name == "*") s2 += RowArray()+JsLocal("name_host")+StringItem(name, "", 0, TRUE)+"</tr>";
      else            s2 += RowArray()+JsLocal("name_host")+StringItem(name, scfg.GetString(HostKey))+"</tr>";
      //
      s2 += RowArray()+"SIP "+JsLocal("name_port")+IntegerItem(name, scfg.GetString(PortKey), 1, 65535)+"</tr>";
      //
      s2 += RowArray()+JsLocal("name_transport")+SelectItem(name, scfg.GetString(TransportKey), ",udp,tcp,tls")+"</tr>";
      //
      if(name == "*") s2 += RowArray()+"RTP"+SelectItem(name, scfg.GetString(RtpProtoKey, "RTP"), RtpProtoSelect)+"</tr>";
      else            s2 += RowArray()+"RTP"+SelectItem(name, scfg.GetString(RtpProtoKey), ","+RtpProtoSelect)+"</tr>";
      //
      s2 += RowArray()+JsLocal("nat_router_ip")+IpItem(name, scfg.GetString(NATRouterIPKey))+"</tr>";
      //
      PString stun_name = scfg.GetString(NATStunServerKey);
      PString stun_list = MCUConfig("SIP Parameters").GetString(NATStunListKey);
      if(stun_list != "")
        stun_list = ",auto,"+stun_list;
      if(stun_list.Find(stun_name) == P_MAX_INDEX)
        stun_list = stun_name+","+stun_list;
      s2 += RowArray()+"STUN "+JsLocal("server")+SelectItem(name, stun_name, stun_list)+"</tr>";
      //
      s2 += EndItemArray();
      s << s2;
    }
    // video
    {
      PString s2;
      s2 += NewItemArray(name, 25);
      // frame rate from MCU
      s2 += RowArray()+JsLocal("name_frame_rate_from_mcu")+IntegerItem(name, scfg.GetString(FrameRateFromKey), 1, MCU_MAX_FRAME_RATE, 70)+"</tr>";
      // bandwidth from MCU
      s2 += RowArray()+JsLocal("name_bandwidth_from_mcu")+IntegerItem(name, scfg.GetString(BandwidthFromKey), MCU_MIN_BIT_RATE/1000, MCU_MAX_BIT_RATE/1000, 70)+"</tr>";
      // bandwidth to MCU
      s2 += RowArray()+JsLocal("name_bandwidth_to_mcu")+IntegerItem(name, scfg.GetString(BandwidthToKey), MCU_MIN_BIT_RATE/1000, MCU_MAX_BIT_RATE/1000, 70)+"</tr>";
      // RTP timeout
      if(name == "*") s2 += RowArray()+JsLocal("rtp_input_timeout")+SelectItem(name, scfg.GetString(RTPInputTimeoutKey, DefaultRTPInputTimeout), RTPInputTimeoutSelect, 70)+"</tr>";
      else            s2 += RowArray()+JsLocal("rtp_input_timeout")+SelectItem(name, scfg.GetString(RTPInputTimeoutKey), ","+RTPInputTimeoutSelect, 70)+"</tr>";
      // Video cache
      if(name == "*") s2 += RowArray()+JsLocal("video_cache")+SelectItem(name, scfg.GetString(VideoCacheKey, EnableKey), EnableSelect, 70)+"</tr>";
      else            s2 += RowArray()+JsLocal("video_cache")+SelectItem(name, scfg.GetString(VideoCacheKey), ","+EnableSelect, 70)+"</tr>";
      // VFU delay
      if(name == "*") s2 += RowArray(name, TRUE)+JsLocal("received_vfu_delay")+SelectItem(name, scfg.GetString(ReceivedVFUDelayKey, DisableKey), ReceivedVFUDelaySelect, 70)+"</tr>";
      else            s2 += RowArray(name, TRUE)+JsLocal("received_vfu_delay")+SelectItem(name, scfg.GetString(ReceivedVFUDelayKey), ","+ReceivedVFUDelaySelect, 70)+"</tr>";
      //
      s2 += EndItemArray();
      s << s2;
    }
    // codecs
    {
      PString a_codec = scfg.GetString(AudioCodecKey);
      PString v_codec = scfg.GetString(VideoCodecKey);
      // bak 13.06.2014, restore resolution from capability
      if(SkipCapability(v_codec))
      {
        PString res;
        VideoResolutionRestore(v_codec, res);
        scfg.SetString(VideoCodecKey, v_codec);
        if(scfg.GetString(VideoResolutionKey) == "")
          scfg.SetString(VideoResolutionKey, res);
      }
      ////////////////////////////////////////////////////
      if(a_codec != "" && a_caps.Find(a_codec) == P_MAX_INDEX) a_caps = a_codec+","+a_caps;
      if(v_codec != "" && v_caps.Find(v_codec) == P_MAX_INDEX) v_caps = v_codec+","+v_caps;

      PString s2;
      s2 += NewItemArray(name, 25);
      //
      s2 += RowArray()+JsLocal("name_audio")+SelectItem(name, a_codec, a_caps)+"</tr>";
      //
      PString video_id = GetTableId();
      PString res_id = GetTableId();
      PString res_value = scfg.GetString(VideoResolutionKey);
      PString video_onchange = "video_transmit_res_toggle(\""+res_id+"\", this.value);";
      javascript += "video_transmit_res_toggle('"+res_id+"', '"+v_codec+"');\n";
      s2 += RowArray()+JsLocal("name_video")+SelectItem(name, v_codec, v_caps, 0, video_id, video_onchange)+"</tr>";
      //
      s2 += RowArray()+(JsLocal("name_video_resolution"))+SelectItem(name, res_value, res_value, 0, res_id)+"</tr>";
      //
      //PString select_pt; for(int i = 96; i < 128; i++) select_pt += ","+PString(i);
      //s2 += RowArray()+(JsLocal("name_video")+" payload type")+SelectItem(name, scfg.GetString("Video payload type"), select_pt)+"</tr>";
      //
      s2 += RowArray()+(JsLocal("name_video")+" fmtp")+StringItem(name, scfg.GetString("Video fmtp"))+"</tr>";
      //
      s2 += EndItemArray();
      s << s2;
    }
    //
  }

  s << EndTable();

  BuildHTML("");
  BeginPage(html_begin, section, "window.l_param_sip_endpoints", "");
  EndPage(html_end,OpenMCU::Current().GetHtmlCopyright());
  html_page << html_begin << s << html_end;
  string = html_page;
}

///////////////////////////////////////////////////////////////

RtspServersPConfigPage::RtspServersPConfigPage(PHTTPServiceProcess & app,const PString & title, const PString & section, const PHTTPAuthority & auth)
    : TablePConfigPage(app,title,section,auth)
{
  firstEditRow = 2;
  rowBorders = TRUE;
  PStringStream html_begin, html_end, html_page, s;
  buttonUp = buttonDown = buttonClone = buttonDelete = 1;
  javascript += js_video_transmit_res_toggle;

  s << BeginTable();
  s << NewRowColumn(JsLocal("name_path"), 210);
  s << ColumnItem(JsLocal("settings"));
  s << ColumnItem("RTSP");
  s << ColumnItem(JsLocal("name_video"));
  s << ColumnItem(JsLocal("name_codec"));

  optionNames.AppendString(EnableKey);
  optionNames.AppendString(UserNameKey);
  optionNames.AppendString(PasswordKey);
  optionNames.AppendString(RoomNameKey);

  optionNames.AppendString(NATRouterIPKey);
  optionNames.AppendString(NATStunServerKey);

  optionNames.AppendString(FrameRateFromKey);
  optionNames.AppendString(BandwidthFromKey);
  optionNames.AppendString(RTPInputTimeoutKey);

  optionNames.AppendString(AudioCodecKey);
  optionNames.AppendString(VideoCodecKey);
  optionNames.AppendString(VideoResolutionKey);

  PString a_caps = ",Disabled", v_caps = ",Disabled";
  PStringList keys = MCUConfig("SIP Audio").GetKeys();
  for(PINDEX i = 0; i < keys.GetSize(); i++)
  {
    if(keys[i].Right(4) == "fmtp" || keys[i].Right(7) == "payload")
      continue;
    if(SkipCapability(keys[i], CONNECTION_TYPE_RTSP))
      continue;
    if(MCUConfig("SIP Audio").GetBoolean(keys[i])) a_caps += ","+keys[i];
  }
  keys = MCUConfig("SIP Video").GetKeys();
  for(PINDEX i = 0; i < keys.GetSize(); i++)
  {
    if(keys[i].Right(4) == "fmtp" || keys[i].Right(7) == "payload")
      continue;
    if(SkipCapability(keys[i], CONNECTION_TYPE_RTSP))
      continue;
    PString capname = keys[i];
    if(MCUConfig("SIP Video").GetBoolean(keys[i])) v_caps += ","+capname;
  }


  sectionPrefix = "RTSP Server ";
  PStringList sect = cfg.GetSectionsPrefix(sectionPrefix);

  if(sect.GetStringsIndex(sectionPrefix+"*") == P_MAX_INDEX)
    sect.InsertAt(0, new PString(sectionPrefix+"*"));
  if(sect.GetSize() == 1)
    sect.AppendString(sectionPrefix+"empty");

  for(PINDEX i = 0; i < sect.GetSize(); i++)
  {
    MCUConfig scfg(sect[i]);
    PString name = sect[i].Right(sect[i].GetLength()-sectionPrefix.GetLength());

    // account
    if(name == "*") s << NewRowInput(name, 0, TRUE); else s << NewRowInputAccount(name);

    // settings
    if(name == "*")
    {
      PString s2;
      s2 += NewItemArray(name, 25);
      s2 += RowArray()+EmptyInputItem(name)+"</tr>";
      s2 += RowArray()+EmptyInputItem(name)+"</tr>";
      s2 += RowArray()+EmptyInputItem(name)+"</tr>";
      s2 += RowArray()+EmptyInputItem(name)+"</tr>";
      s2 += EndItemArray();
      s << s2;
    } else {
      PString s2;
      s2 += NewItemArray(name);
      s2 += RowArray()+JsLocal("name_enable")+BoolItem(name, scfg.GetBoolean(EnableKey))+"</tr>";
      s2 += RowArray()+JsLocal("name_user")+StringItem(name, scfg.GetString(UserNameKey));
      s2 += RowArray()+JsLocal("name_password")+StringItem(name, scfg.GetString(PasswordKey));
      s2 += RowArray()+JsLocal("name_roomname")+StringItem(name, scfg.GetString(RoomNameKey))+"</tr>";
      s2 += EndItemArray();
      s << s2;
    }
    // RTSP
    {
      PString s2;
      s2 += NewItemArray(name, 25);
      //
      s2 += RowArray()+JsLocal("nat_router_ip")+IpItem(name, scfg.GetString(NATRouterIPKey))+"</tr>";
      //
      PString stun_name = scfg.GetString(NATStunServerKey);
      PString stun_list = MCUConfig("SIP Parameters").GetString(NATStunListKey);
      if(stun_list != "")
        stun_list = ",auto,"+stun_list;
      if(stun_list.Find(stun_name) == P_MAX_INDEX)
        stun_list = stun_name+","+stun_list;
      s2 += RowArray()+"STUN "+JsLocal("server")+SelectItem(name, stun_name, stun_list)+"</tr>";
      //
      s2 += EndItemArray();
      s << s2;
    }
    // video
    {
      PString s2;
      s2 += NewItemArray(name, 25);
      // frame rate from MCU
      s2 += RowArray()+JsLocal("name_frame_rate_from_mcu")+IntegerItem(name, scfg.GetString(FrameRateFromKey), 1, MCU_MAX_FRAME_RATE, 70)+"</tr>";
      // bandwidth from MCU
      s2 += RowArray()+JsLocal("name_bandwidth_from_mcu")+IntegerItem(name, scfg.GetString(BandwidthFromKey), MCU_MIN_BIT_RATE/1000, MCU_MAX_BIT_RATE/1000, 70)+"</tr>";
      // RTP timeout
      if(name == "*") s2 += RowArray()+JsLocal("rtp_input_timeout")+SelectItem(name, scfg.GetString(RTPInputTimeoutKey, DefaultRTPInputTimeout), RTPInputTimeoutSelect, 70)+"</tr>";
      else            s2 += RowArray()+JsLocal("rtp_input_timeout")+SelectItem(name, scfg.GetString(RTPInputTimeoutKey), ","+RTPInputTimeoutSelect, 70)+"</tr>";
      //
      s2 += EndItemArray();
      s << s2;
    }
    // codecs
    {
      PString a_codec = scfg.GetString(AudioCodecKey);
      PString v_codec = scfg.GetString(VideoCodecKey);
      if(a_codec != "" && a_caps.Find(a_codec) == P_MAX_INDEX) a_caps = a_codec+","+a_caps;
      if(v_codec != "" && v_caps.Find(v_codec) == P_MAX_INDEX) v_caps = v_codec+","+v_caps;

      PString s2;
      s2 += NewItemArray(name, 25);
      //
      s2 += RowArray()+JsLocal("name_audio")+SelectItem(name, a_codec, a_caps)+"</tr>";
      //
      PString video_id = GetTableId();
      PString res_id = GetTableId();
      PString res_value = scfg.GetString(VideoResolutionKey);
      PString video_onchange = "video_transmit_res_toggle(\""+res_id+"\", this.value);";
      javascript += "video_transmit_res_toggle('"+res_id+"', '"+v_codec+"');\n";
      s2 += RowArray()+JsLocal("name_video")+SelectItem(name, v_codec, v_caps, 0, video_id, video_onchange)+"</tr>";
      //
      s2 += RowArray()+(JsLocal("name_video_resolution"))+SelectItem(name, res_value, res_value, 0, res_id)+"</tr>";
      //
      s2 += EndItemArray();
      s << s2;
    }
    //
  }

  s << EndTable();

  BuildHTML("");
  BeginPage(html_begin, section, "", "");
  EndPage(html_end,OpenMCU::Current().GetHtmlCopyright());
  html_page << html_begin << s << html_end;
  string = html_page;
}

///////////////////////////////////////////////////////////////

ProxySIPPConfigPage::ProxySIPPConfigPage(PHTTPServiceProcess & app,const PString & title, const PString & section, const PHTTPAuthority & auth)
    : TablePConfigPage(app,title,section,auth)
{
  PStringStream html_begin, html_end, html_page, s;
  buttonUp = buttonDown = buttonClone = buttonDelete = 1;

  s << BeginTable();
  s << NewRowColumn(JsLocal("name_account")+"<br><i>username@domain</i>");
  s << ColumnItem(JsLocal("name_register"));
  s << ColumnItem(JsLocal("name_roomname"));
  s << ColumnItem(JsLocal("name_address_sip_proxy")+"<br><i>hostname or ip</i>");
  s << ColumnItem(JsLocal("name_password"));
  s << ColumnItem(JsLocal("name_expires"));

  optionNames.AppendString(EnableKey);
  optionNames.AppendString(RoomNameKey);
  optionNames.AppendString(AddressKey);
  optionNames.AppendString(PasswordKey);
  optionNames.AppendString(ExpiresKey);

  sectionPrefix = "SIP Proxy Account ";
  PStringList sect = cfg.GetSectionsPrefix(sectionPrefix);

  /////////////////////////////////////////
  // bak
  if(sect.GetSize() == 0)
  {
    MCUConfig bcfg("ProxyServers");
    PStringList keys = bcfg.GetKeys();
    for(PINDEX i = 0; i < keys.GetSize(); i++)
    {
      PString roomname = keys[i];
      MCUConfig scfg(sectionPrefix+roomname);
      PStringArray params = bcfg.GetString(keys[i]).Tokenise(",");
      scfg.SetString("Register", params[0]);
      scfg.SetString("Host", params[1]);
      scfg.SetString("Account", params[2]);
      scfg.SetString("Password", params[3]);
      scfg.SetString("Expires", params[4]);
    }
    bcfg.DeleteSection();
  }
  sect = cfg.GetSectionsPrefix(sectionPrefix);
  // bak2
  for(PINDEX i = 0; i < sect.GetSize(); i++)
  {
    MCUConfig bcfg(sect[i]);
    if(bcfg.HasKey("Enable")) continue;
    PString roomname = sect[i].Right(sect[i].GetLength()-sectionPrefix.GetLength());
    MCUConfig scfg(sectionPrefix+bcfg.GetString("Account"));
    scfg.SetBoolean("Enable", bcfg.GetBoolean("Register"));
    scfg.SetString("Room", roomname);
    scfg.SetString("Address", bcfg.GetString("Host"));
    scfg.SetString("Password", bcfg.GetString("Password"));
    scfg.SetString("Expires", bcfg.GetString("Expires"));
    bcfg.DeleteSection();
  }
  sect = cfg.GetSectionsPrefix(sectionPrefix);
  /////////////////////////////////////////

  // пустая строка
  PString name = "empty";
  s << NewRowInput(name, 150);
  s << BoolItem(name, FALSE);
  s << StringItem(name, OpenMCU::Current().GetDefaultRoomName(), 120);
  s << StringItem(name, "", 120);
  s << StringItem(name, "", 120);
  s << SelectItem(name, "600", "60,120,180,240,300,600,1200,1800,2400,3000,3600", 120);

  for(PINDEX i = 0; i < sect.GetSize(); i++)
  {
    MCUConfig scfg(sect[i]);
    PString name = sect[i].Right(sect[i].GetLength()-sectionPrefix.GetLength());

    PString roomname = scfg.GetString(RoomNameKey);
    PString address = scfg.GetString(AddressKey);
    PString password = scfg.GetString(PasswordKey);
    int expires = scfg.GetInteger(ExpiresKey);

    if(address == "" || roomname == "")
      scfg.SetBoolean(EnableKey, FALSE);
    BOOL enable = scfg.GetBoolean(EnableKey, FALSE);

    if(!enable) s << NewRowInput(name, 150);
    else        s << NewRowInput(name, 150, TRUE);
    s << BoolItem(name, enable);
    if(!enable) s << StringItem(name, roomname, 120);
    else        s << StringItem(name, roomname, 120, TRUE);
    if(!enable) s << StringItem(name, address, 120);
    else        s << StringItem(name, address, 120, TRUE);
    if(!enable) s << PasswordItem(name, password, 120);
    else        s << PasswordItem(name, password, 120, TRUE);
    if(!enable) s << SelectItem(name, expires, "60,120,180,240,300,600,1200,1800,2400,3000,3600", 120);
    else        s << SelectItem(name, expires, expires /* readonly */, 120);
  }
  s << EndTable();

  BuildHTML("");
  BeginPage(html_begin, "SIP proxy-servers", "window.l_param_sip_proxy", "window.l_info_param_sip_proxy");
  EndPage(html_end,OpenMCU::Current().GetHtmlCopyright());
  html_page << html_begin << s << html_end;
  string = html_page;
}

///////////////////////////////////////////////////////////////

RoomAccessSIPPConfigPage::RoomAccessSIPPConfigPage(PHTTPServiceProcess & app,const PString & title, const PString & section, const PHTTPAuthority & auth)
    : TablePConfigPage(app,title,section,auth)
{
  firstEditRow = 2;
  PStringStream html_begin, html_end, html_page, s;
  buttonUp = buttonDown = buttonClone = buttonDelete = 1;
  s << BeginTable();
  s << NewRowColumn(JsLocal("name_roomname"));
  s << ColumnItem(JsLocal("name_access"));
  s << ColumnItem("'user1@domain user2@ @domain @@via'");

  PStringList keys = cfg.GetKeys();
  if(keys.GetStringsIndex("*") == P_MAX_INDEX)
    keys.InsertAt(0, new PString("*"));
  if(keys.GetSize() == 1)
    keys.AppendString(OpenMCU::Current().GetDefaultRoomName());

  for(PINDEX i = 0; i < keys.GetSize(); i++)
  {
    PString name = keys[i];
    PString access = cfg.GetString(keys[i]).Tokenise(",")[0].ToLower();
    PString params = cfg.GetString(keys[i]).Tokenise(",")[1];
    if(name == "*") s << NewRowInput(name, 0, TRUE);
    else            s << NewRowInput(name, 0);
    s << SelectItem(name, access, "allow,deny");
    if(name == "*") s << StringItem(name, "", 300, TRUE);
    else            s << StringItem(name, params, 300);
  }
  s << EndTable();

  BuildHTML("");
  BeginPage(html_begin, "Access Rules", "window.l_param_access_rules", "window.l_info_param_access_rules");
  EndPage(html_end,OpenMCU::Current().GetHtmlCopyright());
  html_page << html_begin << s << html_end;
  string = html_page;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

RtspPConfigPage::RtspPConfigPage(PHTTPServiceProcess & app,const PString & title, const PString & section, const PHTTPAuthority & auth)
    : TablePConfigPage(app,title,section,auth)
{
  PStringStream html_begin, html_end, html_page, s;

  s << BeginTable();
  s << BoolField("RESTORE DEFAULTS", JsLocal("restore_defaults"), FALSE);

  s << BoolField("Enable", "RTSP "+JsLocal("name_enable"), cfg.GetBoolean("Enable", TRUE));
  s << ArrayField("Listener", "RTSP "+JsLocal("listener"), cfg.GetString("Listener", "0.0.0.0:1554"), 150);

  s << EndTable();
  BuildHTML("");
  BeginPage(html_begin, section, "", "");
  EndPage(html_end,OpenMCU::Current().GetHtmlCopyright());
  html_page << html_begin << s << html_end;
  string = html_page;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

H323PConfigPage::H323PConfigPage(PHTTPServiceProcess & app,const PString & title, const PString & section, const PHTTPAuthority & auth)
    : TablePConfigPage(app,title,section,auth)
{
  OpenMCU & mcu = OpenMCU::Current();

  mcu.GetEndpoint().Initialise(cfg);

  PStringStream html_begin, html_end, html_page, s;
  s << BeginTable();
  s << BoolField("RESTORE DEFAULTS", JsLocal("restore_defaults"), FALSE);

  s << ArrayField(InterfaceKey, "H.323 "+JsLocal("listener"), cfg.GetString(InterfaceKey), 150);

  s << StringField(NATRouterIPKey, JsLocal("nat_router_ip"), cfg.GetString(NATRouterIPKey));
  s << StringField(NATTreatAsGlobalKey, JsLocal("treat_as_global_for_nat"), cfg.GetString(NATTreatAsGlobalKey));

  s << BoolField(DisableFastStartKey, JsLocal("disable_fast_start"), cfg.GetBoolean(DisableFastStartKey, TRUE));
  s << BoolField(DisableH245TunnelingKey, JsLocal("disable_h245_tunneling"), cfg.GetBoolean(DisableH245TunnelingKey, FALSE));

  PString labels = "No gatekeeper,Find gatekeeper,Use gatekeeper";
  s << SelectField(GatekeeperModeKey, JsLocal("gk_mode"), cfg.GetString(GatekeeperModeKey, labels[0]), labels, 150);
  s << SelectField(GatekeeperRetryIntervalKey, JsLocal("gk_reg_retry_interval"), cfg.GetString(GatekeeperRetryIntervalKey, "30"), "10,20,30,40,50,60,120,180,240,300");
  s << SelectField(GatekeeperTTLKey, JsLocal("gk_reg_ttl"), cfg.GetString(GatekeeperTTLKey), ",60,120,180,240,300,600,1200,1800,2400,3000,3600");
  s << StringField(GatekeeperKey, JsLocal("gk_host"), cfg.GetString(GatekeeperKey));
  s << StringField(GatekeeperUserNameKey, JsLocal("gk_username"), cfg.GetString(GatekeeperUserNameKey, "MCU"));
  s << PasswordField(GatekeeperPasswordKey, JsLocal("gk_password"), cfg.GetString(GatekeeperPasswordKey));
  s << ArrayField(GatekeeperAliasKey, JsLocal("gk_room_names"), cfg.GetString(GatekeeperAliasKey), 150);

  s << EndTable();
  BuildHTML("");
  BeginPage(html_begin, section, "window.l_param_h323", "window.l_info_param_h323");
  EndPage(html_end,OpenMCU::Current().GetHtmlCopyright());
  html_page << html_begin << s << html_end;
  string = html_page;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

SIPPConfigPage::SIPPConfigPage(PHTTPServiceProcess & app,const PString & title, const PString & section, const PHTTPAuthority & auth)
    : TablePConfigPage(app,title,section,auth)
{
  OpenMCU & mcu = OpenMCU::Current();

  PStringStream html_begin, html_end, html_page, s;
  s << BeginTable();
  s << BoolField("RESTORE DEFAULTS", JsLocal("restore_defaults"), FALSE);

  PStringArray sipListener;
  PStringArray data = cfg.GetString(SipListenerKey, "0.0.0.0,transport=*").Tokenise(",");
  for(PINDEX i = 0, j = 0; i < data.GetSize(); i++)
  {
    if(data[i].Find("transport") != P_MAX_INDEX) { sipListener[j-1] += ";"+data[i]; continue; }
    sipListener.AppendString(data[i]);
    j++;
  }

  PString item = NewRowText(SipListenerKey, "SIP "+JsLocal("listener"));
  item += NewItemArray(SipListenerKey);
  for(PINDEX i = 0; i < sipListener.GetSize(); i++)
  {
    PString url = sipListener[i].Tokenise(";")[0];
    PString transport = sipListener[i].Tokenise(";")[1];
    if(url == "") url = "0.0.0.0";
    item += StringItemArray(SipListenerKey, url, 150);
    item += SelectItem(SipListenerKey, transport, "transport=*,transport=udp,transport=tcp,transport=tls");
    if(url == "0.0.0.0") url += " :5060";
    sipListener[i] = "sip:"+url+";"+transport;
  }
  item += EndItemArray();
  item += InfoItem("");
  s << item;

  s << ArrayField(NATStunListKey, "STUN "+JsLocal("server_list"), cfg.GetString(NATStunListKey), 150);

  mcu.GetSipEndpoint()->sipListenerArray = sipListener;

  s << EndTable();
  BuildHTML("");
  BeginPage(html_begin, "SIP settings", "window.l_param_sip", "window.l_info_param_sip");
  EndPage(html_end,OpenMCU::Current().GetHtmlCopyright());
  html_page << html_begin << s << html_end;
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

SIPCodecsPConfigPage::SIPCodecsPConfigPage(PHTTPServiceProcess & app,const PString & title, const PString & section, const PHTTPAuthority & auth)
    : TablePConfigPage(app,title,section,auth)
{
  PStringStream html_begin, html_end, html_page, s;
  buttonUp = buttonDown = buttonDelete = 1;
  s << BeginTable();
  s << NewRowColumn("");
  s << ColumnItem("", 30);
  s << ColumnItem("", 80);
  if(section == "SIP Audio")
  {
    s << ColumnItem(JsLocal("name_parameters_for_sending"));
    s << ColumnItem(JsLocal("name_codec_parameters"));
  }
  s << ColumnItem(JsLocal("name_default_parameters"), 350);

  PStringList keys = cfg.GetKeys();
  for(PINDEX i = 0; i < keys.GetSize(); i++)
  {
    PString name = keys[i];
    if(name.Right(4) == "fmtp" || name.Right(7) == "payload") continue;
    if(name.Right(4) != "{sw}") name += "{sw}";

    if(SkipCapability(name, CONNECTION_TYPE_SIP))
      continue;

    PString info, fmtp;
    MCUCapability *cap = MCUCapability::Create(name);
    if(cap)
    {
      const OpalMediaFormat & mf = cap->GetMediaFormat();
      if(cap->GetMainType() == MCUCapability::e_Video)
        info += PString(mf.GetOptionInteger(OPTION_FRAME_WIDTH))+"x"+PString(mf.GetOptionInteger(OPTION_FRAME_HEIGHT));
      else
        info += PString(mf.GetTimeUnits()*1000)+"Hz";
      for (PINDEX j = 0; j < mf.GetOptionCount(); j++)
       if(mf.GetOption(j).GetFMTPName() != "")
         fmtp += mf.GetOption(j).GetFMTPName()+"="+mf.GetOption(j).AsString()+";";
      delete cap;
    }
    else
      info = JsLocal("not_found");

    s << NewRowInput(name, 140);
    if(info == JsLocal("not_found")) s << BoolItem(name, FALSE, TRUE);
    else if(cfg.GetBoolean(name))              s << BoolItem(name, TRUE);
    else                                       s << BoolItem(name, FALSE);
    s << InfoItem(info);
    if(section == "SIP Audio")
    {
      // parameters for sending
      if(name.Left(4).ToLower() == "opus" || name.Left(5).ToLower() == "speex")
      {
        PString fname = name+"_fmtp";
        s << "<input name='"+fname+"' value='"+fname+"' type='hidden'>";
        s << StringItem(fname, cfg.GetString(fname), 200);
      }
      // codec parameters
      if(name.Left(4).ToLower() == "opus" || name.Left(5).ToLower() == "speex")
      {
        PString fname = name+"_local_fmtp";
        s << "<input name='"+fname+"' value='"+fname+"' type='hidden'>";
        s << StringItem(fname, cfg.GetString(fname), 200);
      }
    }
    s << InfoItem(fmtp);
  }
  s << EndTable();

  BuildHTML("");
  if(section == "SIP Audio")
    BeginPage(html_begin, "SIP Audio", "SIP Audio", "");
  else if(section == "SIP Video")
    BeginPage(html_begin, "SIP Video", "SIP Video", "");
  EndPage(html_end,OpenMCU::Current().GetHtmlCopyright());
  html_page << html_begin << s << html_end;
  string = html_page;
}
////////////////////////////////////////////////////////////////////////////////////////////////////

CodecsPConfigPage::CodecsPConfigPage(PHTTPServiceProcess & app,const PString & title, const PString & section, const PHTTPAuthority & auth)
    : TablePConfigPage(app,title,section,auth)
{
  buttonUp = buttonDown = buttonDelete = 1;
  firstEditRow = firstDeleteRow = 0;
  PStringStream html_begin, html_end, html_page, s;
  s << BeginTable();

  PStringList keys = cfg.GetKeys();
  for(PINDEX i = 0; i < keys.GetSize(); i++)
  {
    if(SkipCapability(keys[i], CONNECTION_TYPE_H323))
      continue;

    PString info, fmtpInfo;
    MCUCapability *cap = MCUCapability::Create(keys[i]);
    if(cap == NULL && keys[i].Find("{sw}") == P_MAX_INDEX)
      cap = MCUCapability::Create(keys[i]+"{sw}");
    if(cap)
    {
      const OpalMediaFormat & mf = cap->GetMediaFormat();
      if(cap->GetMainType() == MCUCapability::e_Video)
        info += PString(mf.GetOptionInteger(OPTION_FRAME_WIDTH))+"x"+PString(mf.GetOptionInteger(OPTION_FRAME_HEIGHT));
      else
        info += PString(mf.GetTimeUnits()*1000)+"Hz";
      //info += PString(mf.GetBandwidth())+"bit/s";
      for (PINDEX j = 0; j < mf.GetOptionCount(); j++)
       if(mf.GetOption(j).GetFMTPName() != "")
         fmtpInfo += mf.GetOption(j).GetFMTPName()+"="+mf.GetOption(j).AsString()+";";
    } else {
      info += JsLocal("not_found");
    }
    s << NewRowText(keys[i]);
    s << BoolItem(keys[i], cfg.GetBoolean(keys[i]));
    s << InfoItem(info);
    s << InfoItem(fmtpInfo);
    if(cap && (title == "SipSoundCodecs" || title == "SipVideoCodecs"))
      s <<  StringItem("fmtp:"+keys[i], MCUConfig("SIP fmtp remote").GetString(keys[i]), 200);
    s << EndRow();
    if(cap) delete cap;
  }

  s << EndTable();
  BuildHTML("");
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
  html_page << html_begin << s << html_end;
  string = html_page;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL CodecsPConfigPage::Post(PHTTPRequest & request,
                       const PStringToString & data,
                       PHTML & reply)
{
  PHTTPForm::Post(request, data, reply);

  cfg.DeleteSection();
  for(PINDEX i = 0; dataArray[i] != NULL; i++)
  {
    if(dataArray[i].Tokenise("=")[1] == "TRUE")
      cfg.SetBoolean(dataArray[i].Tokenise("=")[0], 1);
    else
      cfg.SetBoolean(dataArray[i].Tokenise("=")[0], 0);
  }
  for(PINDEX i = 0; fmtpArray[i] != NULL; i++)
  {
    PINDEX fmtpPos = fmtpArray[i].Find("=");
    if(fmtpPos != P_MAX_INDEX && fmtpArray[i].GetSize() > fmtpPos)
      MCUConfig("SIP fmtp remote").SetString(fmtpArray[i].Tokenise("=")[0], fmtpArray[i].Right(fmtpArray[i].GetSize()-fmtpPos-2));
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
  dataArray = connectInfo.GetEntityBody().Tokenise("&");
  for(PINDEX i = 0; dataArray[i] != NULL; i++)
  {
    PString key = PURL::UntranslateString(dataArray[i].Tokenise("=")[0], PURL::QueryTranslation);
    PString value = PURL::UntranslateString(dataArray[i].Tokenise("=")[1], PURL::QueryTranslation);
    dataArray[i] = key+"="+value;
    if(key.Left(5) == "fmtp:")
    {
      if(key.GetSize() > 5)
        fmtpArray.AppendString(key.Right(key.GetSize()-6)+"="+value);
      dataArray.RemoveAt(i); i--; continue;
    }
    if(key == "submit" || key == "TableItemId" || key == "" || value == "")
    {
      dataArray.RemoveAt(i); i--; continue;
    }
    if(value == "FALSE")
    {
      for(PINDEX j = i+1; dataArray[j] != NULL; j++)
      {
        PString jkey = PURL::UntranslateString(dataArray[j].Tokenise("=")[0], PURL::QueryTranslation);
        PString jvalue = PURL::UntranslateString(dataArray[j].Tokenise("=")[1], PURL::QueryTranslation);
        if(jkey == key && jvalue == "TRUE")
        {
          dataArray[i] = jkey+"="+jvalue;
          dataArray.RemoveAt(j); j--; continue;
        }
      }
    }
  }

  PHTTPConfig::OnPOST(server, url, info, data, connectInfo);
  return TRUE;
}

///////////////////////////////////////////////////////////////

WelcomePage::WelcomePage(OpenMCU & _app, PHTTPAuthority & auth)
  : PServiceHTTPString("welcome.html", "", "text/html; charset=utf-8", auth),
    app(_app)
{}

BOOL WelcomePage::OnPOST(PHTTPServer & server, const PURL & url, const PMIMEInfo & info, const PStringToString & data, const PHTTPConnectionInfo & connectInfo)
{
  PHTTPRequest * req = CreateRequest(url, info, connectInfo.GetMultipartFormInfo(), server); // check authorization
  if(!CheckAuthority(server, *req, connectInfo)) { delete req; return PServiceHTTPString::OnGET(server, url, info, connectInfo); }
  delete req;

  const PString & eb = connectInfo.GetEntityBody();
  long l = connectInfo.GetEntityBodyLength();
  if(l<1) return FALSE;

  PINDEX o;
  PString extension;
  if((o = eb.Find("image/bmp")) != P_MAX_INDEX)
  {
    o += 10;
    extension = "bmp";
  }
  else if((o = eb.Find("image/jpeg")) != P_MAX_INDEX)
  {
    o += 11;
    extension = "jpeg";
  }
  else if((o = eb.Find("image/pjpeg")) != P_MAX_INDEX)
  {
    o += 12;
    extension = "jpeg";
  }
  else if((o = eb.Find("image/png")) != P_MAX_INDEX)
  {
    o += 10;
    extension = "png";
  }
  else if((o = eb.Find("image/gif")) != P_MAX_INDEX)
  {
    o += 10;
    extension = "gif";
  }
  else
    return FALSE;

  PINDEX bs = 0, be = bs; // bs, be - boundary start, boundary end
  BYTE c = eb[be];
  while ((c != 10) && (c != 13) && (be < l))
  {
    be++;
    c = eb[be];
  }
  if(be == l) return FALSE;
  PINDEX bl = be - bs; // bl - boundary length
  if(bl < 3) return FALSE;

  PINDEX limit=PMIN(l, o+4);
  while((o < limit) && ((eb[o] == 10) || (eb[o] == 13))) o++;

  PINDEX o2 = o;
  BOOL found = FALSE;
  while((!found) && (o2 < l - bl))
  {
    found = TRUE;
    for (PINDEX j = 0; j<bl; j++)
    {
      if(eb[o2+j] != eb[bs+j])
      {
        found = FALSE;
        o2++;
        break;
      }
    }
  }

  if(!found) return FALSE;
  if(o2 > o) if((eb[o2-1] == 10) || (eb[o2-1] == 13))
  { o2--;
    if(o2 > o) if((eb[o2-1] == 10) || (eb[o2-1] == 13)) o2--;
  }
  if(o2<=o) return FALSE;
  size_t cl = o2-o; // content length

  if(extension == "jpeg" && cl > 524288)
    return FALSE; // too big

  // delete files
  PFile::Remove(PString(SYS_CONFIG_DIR) + PATH_SEPARATOR + "logo.bmp");
  PFile::Remove(PString(SYS_CONFIG_DIR) + PATH_SEPARATOR + "logo.jpeg");
  PFile::Remove(PString(SYS_CONFIG_DIR) + PATH_SEPARATOR + "logo.png");
  PFile::Remove(PString(SYS_CONFIG_DIR) + PATH_SEPARATOR + "logo.gif");

  FILE *f;
  size_t written=0;
  f=fopen(PString(SYS_CONFIG_DIR) + PATH_SEPARATOR + "logo." + extension,"wb"); 
  if(f) written=fwrite((const void*)(((const char*)eb)+o), 1, (size_t)o2-o, f);
  fclose(f);

  PTRACE(1,"HTML\tlogo." << extension << " " << written << " byte(s) re-written");

  { PStringStream message; PTime now; message
      << "HTTP/1.1 302 Found\r\n"
      << "Location: /welcome.html\r\n"
      << "\r\n";  //that's the last time we need to type \r\n instead of just \n
    server.Write((const char*)message,message.GetLength());
  }
  server.flush();
  //OpenMCU::Current().RemovePreMediaFrame();

  return FALSE;
}

///////////////////////////////////////////////////////////////

BOOL WelcomePage::OnGET (PHTTPServer & server, const PURL &url, const PMIMEInfo & info, const PHTTPConnectionInfo & connectInfo)
{
  PString peerAddr  = "N/A",
          localAddr = "127.0.0.1";
  WORD    localPort = 80;
  { PHTTPRequest * req = CreateRequest(url, info, connectInfo.GetMultipartFormInfo(), server); // check authorization
    if(!CheckAuthority(server, *req, connectInfo)) {delete req; return FALSE;}
    if(req->origin != 0) peerAddr=req->origin.AsString();
    if(req->localAddr != 0) localAddr=req->localAddr.AsString();
    if(req->localPort != 0) localPort=req->localPort;
    delete req;
  }
  PStringToString data;
  { PString request=url.AsString(); PINDEX q;
    if((q=request.Find("?"))!=P_MAX_INDEX) { request=request.Mid(q+1,P_MAX_INDEX); PURL::SplitQueryVars(request,data); }
  }
  PStringStream shtml;
  BeginPage(shtml,"OpenMCU-ru","window.l_welcome","window.l_info_welcome");

  PString timeFormat = "yyyyMMdd hhmmss z";
  PTime now;
  PTimeInterval upTime = now - OpenMCU::Current().GetStartTime();

  shtml << "<br><b>Monitor Text (<span style='cursor:pointer;text-decoration:underline' onclick='javascript:{if(document.selection){var range=document.body.createTextRange();range.moveToElementText(document.getElementById(\"monitorTextId\"));range.select();}else if(window.getSelection){var range=document.createRange();range.selectNode(document.getElementById(\"monitorTextId\"));window.getSelection().addRange(range);}}'>select all</span>)</b><div style='padding:5px;border:1px dotted #595;width:100%;height:auto;max-height:300px;overflow:auto'><pre style='margin:0px;padding:0px' id='monitorTextId'>"
#ifdef GIT_REVISION
        << "OpenMCU-ru REVISION " << MCU_STRINGIFY(GIT_REVISION) << "\n"
#endif

              << "Program: "          << OpenMCU::Current().GetProductName() << "\n"
              << "Version: "          << OpenMCU::Current().GetVersion(TRUE) << "\n"
              << "Manufacturer: "     << OpenMCU::Current().GetManufacturer() << "\n"
              << "OS: "               << OpenMCU::Current().GetOSClass() << " " << OpenMCU::Current().GetOSName() << "\n"
              << "OS Version: "       << OpenMCU::Current().GetOSVersion() << "\n"
              << "Hardware: "         << OpenMCU::Current().GetOSHardware() << "\n"
              << "Compilation date: " << OpenMCU::Current().GetCompilationDate().AsString(timeFormat, PTime::GMT) << "\n"
              << "Start Date: "       << OpenMCU::Current().GetStartTime().AsString(timeFormat, PTime::GMT) << "\n"
              << "Current Date: "     << now.AsString(timeFormat, PTime::GMT) << "\n"
              << "Up time: "          << upTime << "\n"
              << "Peer Addr: "        << peerAddr << "\n"
              << "Local Host: "       << PIPSocket::GetHostName() << "\n"
              << "Local Addr: "       << localAddr << "\n"
              << "Local Port: "       << localPort << "\n"


        << app.GetEndpoint().GetMonitorText() << "</pre></div>";

  shtml << "<br><form method=\"post\" enctype=\"multipart/form-data\"><script type=\"text/javascript\">"
           "document.write(window.l_welcome_logo);</script><img src=\""+OpenMCU::Current().GetLogoFilename()+"\"><br><script type=\"text/javascript\">"
           "document.write(window.l_changelogo);</script><input name=\"image\" type=\"file\"><input type=\"submit\"></form>";

  EndPage(shtml,OpenMCU::Current().GetHtmlCopyright());
  { PStringStream message; PTime now; message
      << "HTTP/1.1 200 OK\r\n"
      << "Date: " << now.AsString(PTime::RFC1123, PTime::GMT) << "\r\n"
      << "Server: OpenMCU-ru\r\n"
      << "MIME-Version: 1.0\r\n"
      << "Cache-Control: no-cache, must-revalidate\r\n"
      << "Expires: Sat, 26 Jul 1997 05:00:00 GMT\r\n"
      << "Content-Type: text/html;charset=utf-8\r\n"
      << "Content-Length: " << shtml.GetLength() << "\r\n"
      << "Connection: Close\r\n"
      << "\r\n";  //that's the last time we need to type \r\n instead of just \n
    server.Write((const char*)message,message.GetLength());
  }
  server.Write((const char*)shtml,shtml.GetLength());
  server.flush();
  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

MainStatusPage::MainStatusPage(OpenMCU & _app, PHTTPAuthority & auth)
  : PServiceHTTPString("Status", "", "text/html; charset=utf-8", auth),
    app(_app)
{
  PStringStream html;

  html << "<meta http-equiv=\"Refresh\" content=\"30\">\n";
  BeginPage(html,"Connections","window.l_connections","window.l_info_connections");
  html << "<p>"
       << "<table class=\"table table-striped table-bordered table-condensed\">"
       << "<tr>"
       << "<th>"
       << "Room Name"
       << "<th>"
       << "Room Members"

       << "<!--#macrostart RoomStatus-->"
         << "<tr>"
         << "<td>"
         << "<!--#status RoomName-->"
         << "<td>"
         << "<!--#status RoomMembers-->"
       << "<!--#macroend RoomStatus-->"

       << "</table>"

       << "<p>";

         EndPage(html,OpenMCU::Current().GetHtmlCopyright());
  string = html;
}


BOOL MainStatusPage::Post(PHTTPRequest & request, const PStringToString & data, PHTML & msg)
{ return TRUE; }

BOOL MainStatusPage::OnGET (PHTTPServer & server, const PURL &url, const PMIMEInfo & info, const PHTTPConnectionInfo & connectInfo)
{
  PHTTPRequest * req = CreateRequest(url, info, connectInfo.GetMultipartFormInfo(), server); // check authorization
  if(!CheckAuthority(server, *req, connectInfo)) { delete req; return PServiceHTTPString::OnGET(server, url, info, connectInfo); }
  delete req;
  PStringToString data;
  PString request=url.AsString(); PINDEX q;
  if((q=request.Find("?"))!=P_MAX_INDEX) { request=request.Mid(q+1,P_MAX_INDEX); PURL::SplitQueryVars(request,data); }
  if(!data.Contains("js")) return PServiceHTTPString::OnGET(server, url, info, connectInfo);

  PString body;

  if(data.Contains("start"))
    body = OpenMCU::Current().GetEndpoint().GetRoomStatusJSStart();
  else
    body = OpenMCU::Current().GetEndpoint().GetRoomStatusJS();
  PINDEX length = body.GetLength();

  PStringStream message;
  message << "HTTP/1.1 200 OK\r\n"
          << "Date: " << PTime().AsString(PTime::RFC1123, PTime::GMT) << "\r\n"
          << "Server: OpenMCU-ru\r\n"
          << "MIME-Version: 1.0\r\n"
          << "Cache-Control: no-cache, must-revalidate\r\n"
          << "Expires: Sat, 26 Jul 1997 05:00:00 GMT\r\n"
          << "Content-Type: text/html;charset=utf-8\r\n"
          << "Content-Length: " << length << "\r\n"
          << "Connection: Close\r\n"
          << "\r\n";  //that's the last time we need to type \r\n instead of just \n

  server.Write((const char*)message,message.GetLength());
  server.Write((const char*)body, length);
  server.flush();
  return TRUE;
}

///////////////////////////////////////////////////////////////

InvitePage::InvitePage(OpenMCU & _app, PHTTPAuthority & auth)
  : PServiceHTTPString("Invite", "", "text/html; charset=utf-8", auth),
    app(_app)
{
  PStringStream html;

  BeginPage(html,"Invite","window.l_invite","window.l_info_invite");

  form << "<p>"
    << "<form method=\"POST\" class=\"well form-inline\">"
    << "<input type=\"text\" class=\"input-small\" placeholder=\"" << app.GetDefaultRoomName() << "\" name=\"room\" value=\"" << app.GetDefaultRoomName() << "\"> "
    << "<input id='address' type=\"text\" class=\"input-large\" placeholder=\"Address\" name=\"address\">"
    << "<script language='javascript'><!--\ndocument.forms[0].address.focus(); //--></script>"
    << "&nbsp;&nbsp;<input class=\"input-small\" type=\"submit\" class=\"btn\" value=\"Invite\">"
    << "</form>";

  form << "<script type='text/javascript'>\n"
       << "function changeSelect(obj)\n"
       << "{\n"
       << "  document.getElementById('address').value=obj.options[obj.selectedIndex].value\n"
       << "}\n"
       << "</script>\n";

  html << form;
  EndPage(html,OpenMCU::Current().GetHtmlCopyright());

  string = html;
}

///////////////////////////////////////////////////////////////

BOOL InvitePage::Post(PHTTPRequest & request,
                          const PStringToString & data,
                          PHTML & msg)
{
  PString room    = data("room");
  PString address = data("address");
  PStringStream html;

  PStringStream html_invite;
  html_invite << form;

  if (room.IsEmpty() || address.IsEmpty()) {
    BeginPage(html,"Invite failed","window.l_invite_f","window.l_info_invite_f");
    html << html_invite;
    EndPage(html,OpenMCU::Current().GetHtmlCopyright()); msg = html;
    return TRUE;
  }

  MCUH323EndPoint & ep = app.GetEndpoint();
  if(ep.Invite(room, address) == "")
  {
    PTRACE(2,"Conference\tCould not invite " << address);
    BeginPage(html,"Invite failed","window.l_invite_f","window.l_info_invite_f");
    html << html_invite;
    EndPage(html,OpenMCU::Current().GetHtmlCopyright()); msg = html;
    return TRUE;
  }

  BeginPage(html,"Invite succeeded","window.l_invite_s","window.l_info_invite_s");
  html << html_invite;
  EndPage(html,OpenMCU::Current().GetHtmlCopyright()); msg = html;

  return TRUE;
}

///////////////////////////////////////////////////////////////

JpegFrameHTTP::JpegFrameHTTP(OpenMCU & _app, PHTTPAuthority & auth)
  : PServiceHTTPString("Jpeg", "", "image/jpeg", auth),
    app(_app)
{
}

BOOL JpegFrameHTTP::OnGET (PHTTPServer & server, const PURL &url, const PMIMEInfo & info, const PHTTPConnectionInfo & connectInfo)
{
  PHTTPRequest * req = CreateRequest(url, info, connectInfo.GetMultipartFormInfo(), server); // check authorization
  if(!CheckAuthority(server, *req, connectInfo)) {delete req; return FALSE;}
  delete req;

  PString request=url.AsString();
  PINDEX q;
  if((q=request.Find("?"))==P_MAX_INDEX) return FALSE;

  request=request.Mid(q+1,P_MAX_INDEX);
  PStringToString data;
  PURL::SplitQueryVars(request,data);

  PString room=data("room"); if (room.GetLength()==0) return FALSE;

  int width=atoi(data("w"));
  int height=atoi(data("h"));

  long requestedMixer=0;
  if(data.Contains("mixer")) requestedMixer=data("mixer").AsInteger();

  const unsigned long t1=(unsigned long)time(0);

  // lock!
  PWaitAndSignal m(jpegMixerMutex);

  ConferenceManager *cm = app.GetConferenceManager();
  jpegMixer = cm->FindVideoMixerWithLock(room, requestedMixer);
  if(jpegMixer == NULL) // no mixer found
    return FALSE;

  if(t1-(jpegMixer->jpegTime)>1) // artificial limitation to prevent overload: no more than 1 frame per second
  {
    if(width<1||height<1||width>2048||height>2048) //suspicious, it's better to get size from layouts.conf
    {
      width=OpenMCU::vmcfg.vmconf[jpegMixer->GetPositionSet()].splitcfg.mockup_width;
      height=OpenMCU::vmcfg.vmconf[jpegMixer->GetPositionSet()].splitcfg.mockup_height;
    }

    width = (width/2)*2;
    height = (height/2)*2;

#if USE_LIBJPEG
    struct my_jpeg_compress_struct cinfo;
    cinfo.context = this;
    struct jpeg_error_mgr jerr;

    JSAMPROW row_pointer[1];
    int row_stride;
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);
    cinfo.image_width = width;
    cinfo.image_height = height;
    cinfo.input_components = 3;
    cinfo.in_color_space = JCS_RGB;

    PINDEX amount=width*height*3/2;
    unsigned char *videoData=new unsigned char[amount];

    jpegMixer->ReadMixedFrame((void*)videoData,width,height,amount);
    PColourConverter * converter = PColourConverter::Create("YUV420P", "RGB24", width, height);
    converter->SetDstFrameSize(width, height);
    unsigned char * bitmap = new unsigned char[width*height*3];
    converter->Convert(videoData,bitmap);
    delete converter;
    delete[] videoData;

    jpeg_set_defaults(&cinfo);
    cinfo.dest = new jpeg_destination_mgr;
    cinfo.dest->init_destination = &jpeg_init_destination;
    cinfo.dest->empty_output_buffer = &jpeg_empty_output_buffer;
    cinfo.dest->term_destination = &jpeg_term_destination;
    jpeg_start_compress(&cinfo,TRUE);
    row_stride = cinfo.image_width * 3;
    while (cinfo.next_scanline < cinfo.image_height)
    { row_pointer[0] = (JSAMPLE *) & bitmap [cinfo.next_scanline * row_stride];
      (void) jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }
    jpeg_finish_compress(&cinfo);
    delete[] bitmap; delete cinfo.dest; cinfo.dest=NULL;
    jpeg_destroy_compress(&cinfo);

#else
    PINDEX buffer_size = width * height * 3 / 2;
    MCUBuffer buffer(buffer_size);
    jpegMixer->ReadMixedFrame(buffer.GetPointer(), width, height, buffer_size);

    int dst_size = 65536;
    jpegMixer->jpegSize = 0;
    jpegMixer->myjpeg.SetSize(dst_size);
    if(MCU_AVEncodeFrame(AV_CODEC_ID_MJPEG, buffer.GetPointer(), buffer_size, jpegMixer->myjpeg.GetPointer(), dst_size, width, height))
      jpegMixer->jpegSize = dst_size;
#endif

    jpegMixer->jpegTime = t1;
  }

  PTime now;
  PStringStream message;
  message << "HTTP/1.1 200 OK\r\n"
          << "Date: " << now.AsString(PTime::RFC1123, PTime::GMT) << "\r\n"
          << "Server: OpenMCU-ru\r\n"
          << "MIME-Version: 1.0\r\n"
          << "Cache-Control: no-cache, must-revalidate\r\n"
          << "Expires: Sat, 26 Jul 1997 05:00:00 GMT\r\n"
          << "Content-Type: image/jpeg\r\n"
          << "Content-Length: " << jpegMixer->jpegSize << "\r\n"
          << "Connection: Close\r\n"
          << "\r\n";  //that's the last time we need to type \r\n instead of just \n

  server.Write((const char*)message,message.GetLength());
  server.Write(jpegMixer->myjpeg.GetPointer(),jpegMixer->jpegSize);
  server.flush();

  // unlock mixer
  jpegMixer->Unlock();

  return TRUE;
}

///////////////////////////////////////////////////////////////

InteractiveHTTP::InteractiveHTTP(OpenMCU & _app, PHTTPAuthority & auth)
  : PServiceHTTPString("Comm", "", "text/html; charset=utf-8", auth),
    app(_app)
{
}

BOOL InteractiveHTTP::OnGET (PHTTPServer & server, const PURL &url, const PMIMEInfo & info, const PHTTPConnectionInfo & connectInfo)
{
  PTRACE(5,"WebCtrl\tComm flow init");

  PHTTPRequest * req = CreateRequest(url, info, connectInfo.GetMultipartFormInfo(), server); // check authorization
  if(!CheckAuthority(server, *req, connectInfo)) {delete req; return FALSE;}
  delete req;

  PTRACE(5,"WebCtrl\tComm flow auth passed");

  PString request=url.AsString();
  PINDEX q;
  PStringToString data;

  if((q=request.Find("?"))!=P_MAX_INDEX)
  {
    request=request.Mid(q+1,P_MAX_INDEX);
    PURL::SplitQueryVars(request,data);
  }

  PString room=data("room");

  PStringStream message;
  PTime now;
  int idx;

  message << "HTTP/1.1 200 OK\r\n"
          << "Date: " << now.AsString(PTime::RFC1123, PTime::GMT) << "\r\n"
          << "Server: OpenMCU-ru\r\n"
          << "MIME-Version: 1.0\r\n"
          << "Cache-Control: no-cache, must-revalidate\r\n"
          << "Expires: Sat, 26 Jul 1997 05:00:00 GMT\r\n"
          << "Content-Type: text/html;charset=utf-8\r\n"
          << "Connection: Close\r\n"
          << "\r\n";  //that's the last time we need to type \r\n instead of just \n
  server.Write((const char*)message,message.GetLength());
  server.flush();

  PTRACE(5,"WebCtrl\tComm flow headers sent");

  message="<html><body style='font-size:9px;font-family:Verdana,Arial;padding:0px;margin:1px;color:#000'><script>p=parent</script>\n";
  message << OpenMCU::Current().HttpStartEventReading(idx,room);

//PTRACE(1,"!!!!!\tsha1('123')=" << PMessageDigestSHA1::Encode("123")); // sha1 works!! I'll try with websocket in future

  ConferenceManager *cm = OpenMCU::Current().GetConferenceManager();
  MCUH323EndPoint & ep = OpenMCU::Current().GetEndpoint();

  PTRACE(5,"WebCtrl\tComm flow find with lock");

  Conference *conference = cm->FindConferenceWithLock(room);
  if(conference)
  {
    PStringStream conferenceOpts;
    conferenceOpts
        << "p." << ep.GetMemberListOptsJavascript(*conference) << "\n"
        << "p." << ep.GetConferenceOptsJavascript(*conference) << "\n"
        << "p." << ep.GetAddressBookOptsJavascript() << "\n"
        << "p.tl=Array" << conference->GetTemplateList() << "\n"
        << "p.seltpl=\"" << conference->GetSelectedTemplateName() << "\"\n";

    // unlock conference
    conference->Unlock();
    PTRACE(5,"WebCtrl\tComm flow found, unlocked");

    message << "<script>p.splitdata=Array(";
    for (unsigned i=0;i<OpenMCU::vmcfg.vmconfs;i++)
    {
      PString split=OpenMCU::vmcfg.vmconf[i].splitcfg.Id;
      split.Replace("\"","\\x22",TRUE,0);
      if(i!=0) message << ",";
      message << "\"" << split << "\"";
    }
    message << ");\np.splitdata2={";
    for (unsigned i=0;i<OpenMCU::vmcfg.vmconfs;i++)
    {
      PString split=OpenMCU::vmcfg.vmconf[i].splitcfg.Id;
      split.Replace("\"","\\x22",TRUE,0);
      message << "\"" << split << "\"" << ":[" << OpenMCU::vmcfg.vmconf[i].splitcfg.vidnum;
      for(unsigned j=0;j<OpenMCU::vmcfg.vmconf[i].splitcfg.vidnum; j++)
      {
        VMPCfgOptions & vo=OpenMCU::vmcfg.vmconf[i].vmpcfg[j];
        message << ",[" << vo.posx << "," << vo.posy << "," << vo.width << "," << vo.height << "," << vo.border << "," << vo.label_mask << "]";
      }
      message << "]";
      if(i+1<OpenMCU::vmcfg.vmconfs) message << ",";
    }
    message << "};\n"
        << conferenceOpts
        << "p.build_page();\n"
        << "</script>\n";
  }
  else
  { // no (no more) room -- redirect to /
    message << "<script>top.location.href='/';</script>\n";
    server.Write((const char*)message,message.GetLength());
    server.flush();
    PTRACE(5,"WebCtrl\tComm flow not found, not locked, stopped");
    return FALSE;
  }

  PTRACE(5,"WebCtrl\tComm flow is ready");

  while(server.Write((const char*)message,message.GetLength()))
  {
    server.flush();
    int cnt=0;
    message = OpenMCU::Current().HttpGetEvents(idx,room);
    while (message.GetLength()==0 && cnt < 20)
    {
      cnt++;
      MCUTime::Sleep(100);
      message = OpenMCU::Current().HttpGetEvents(idx,room);
    }
    if(message.Find("<script>")==P_MAX_INDEX) message << "<script>p.alive()</script>\n";
  }
  return FALSE;

  PTRACE(5,"WebCtrl\tComm flow stopped");
}

///////////////////////////////////////////////////////////////

SelectRoomPage::SelectRoomPage(OpenMCU & _app, PHTTPAuthority & auth)
  : PServiceHTTPString("Select", "", "text/html; charset=utf-8", auth),
    app(_app)
{
}

///////////////////////////////////////////////////////////////

BOOL SelectRoomPage::OnGET (PHTTPServer & server, const PURL &url, const PMIMEInfo & info, const PHTTPConnectionInfo & connectInfo)
{
  {
    PHTTPRequest * req = CreateRequest(url, info, connectInfo.GetMultipartFormInfo(), server); // check authorization
    if(!CheckAuthority(server, *req, connectInfo)) {delete req; return FALSE;}
    delete req;
  }

  PStringToString data;
  {
    PString request=url.AsString(); PINDEX q;
    if((q=request.Find("?"))!=P_MAX_INDEX) { request=request.Mid(q+1,P_MAX_INDEX); PURL::SplitQueryVars(request,data); }
  }

  ConferenceManager *cm = OpenMCU::Current().GetConferenceManager();
  if(data.Contains("action") && !data("room").IsEmpty())
  {
    PString action = data("action");
    PString room = data("room");
    if(action == "create")
    {
      Conference * conference = cm->MakeConferenceWithLock(room, "", TRUE);
      if(conference)
        conference->Unlock();
    }
    else if(action == "delete")
    {
      cm->RemoveConference(room);
    }
    else if(action == "startRecorder")
    {
      Conference * conference = cm->FindConferenceWithLock(room);
      if(conference)
      {
        conference->StartRecorder();
        conference->Unlock();
      }
    }
    else if(action == "stopRecorder")
    {
      Conference * conference = cm->FindConferenceWithLock(room);
      if(conference)
      {
        conference->StopRecorder();
        conference->Unlock();
      }
    }
  }

  PStringStream html;
  BeginPage(html,"Rooms","window.l_rooms","window.l_info_rooms");

  if(data.Contains("action")) html << "<script language='javascript'>location.href='Select';</script>";

  PString nextRoom;
  MCUConferenceList & conferenceList = cm->GetConferenceList();
  for(MCUConferenceList::shared_iterator it = conferenceList.begin(); it != conferenceList.end(); ++it)
  {
    Conference *conference = it.GetObject();
    PString room0 = conference->GetNumber().Trim();

      if(room0.IsEmpty()) continue;
      if(room0.Find(MCU_INTERNAL_CALL_PREFIX) == 0) continue; // todo: use much more fast boolean check to determine int. call
      PINDEX lastCharPos=room0.GetLength()-1;
      PINDEX i, d1=-1, d2=-1;
      for (i=lastCharPos; i>=0; i--)
      { char c=room0[i];
        BOOL isDigit = (c>='0' && c<='9');
        if (isDigit) { if (d2==-1) d2=i; }
        else { if (d2!=-1) { if (d1==-1) { d1 = i+1; break; } } }
      }
      if(d1==-1 || d2==-1) continue;
      if(d2-d1>6)d1=d2-6;
      PINDEX roomStart=room0.Mid(d1,d2).AsInteger(); PString roomText=room0.Left(d1);
      PString roomText2; if(d2<lastCharPos) roomText2=room0.Mid(d2+1,lastCharPos);

    while(1)
    {
      roomStart++;
      PString testName = roomText + PString(roomStart) + roomText2;

      MCUConferenceList::shared_iterator it2;
      for(it2 = conferenceList.begin(); it2 != conferenceList.end(); ++it2)
      {
        Conference *c = it2.GetObject();
        PString name = c->GetNumber();
        if(name == testName)
          break;
      }
      if(it2 == conferenceList.end())
      {
        nextRoom = testName;
        break;
      }
    }
    break;
  }
  if(nextRoom.IsEmpty()) nextRoom = OpenMCU::Current().GetDefaultRoomName();

  html
    << "<form method=\"post\" onsubmit=\"javascript:{if(document.getElementById('newroom').value!='')location.href='?action=create&room='+encodeURIComponent(document.getElementById('newroom').value);return false;}\"><input name='room' id='room' type=hidden>"
    << "<table class=\"table table-striped table-bordered table-condensed\">"

    << "<tr>"
    << "<td colspan='7'><input type='text' class='input-small' name='newroom' id='newroom' value='" << nextRoom << "' /><input type='button' class='btn btn-large btn-info' id='l_select_create' onclick=\"location.href='?action=create&room='+encodeURIComponent(document.getElementById('newroom').value);\"></td>"
    << "</tr>"

    << "<tr>"
    << "<th style='text-align:center'><script type=\"text/javascript\">document.write(window.l_select_enter);</script><br></th>"
    << "<th style='text-align:center'><script type=\"text/javascript\">document.write(window.l_select_record);</script><br></th>"
    << "<th style='text-align:center'><script type=\"text/javascript\">document.write(window.l_select_moderated);</script><br></th>"
    << "<th style='text-align:center'><script type=\"text/javascript\">document.write(window.l_select_visible);</script><br></th>"
    << "<th style='text-align:center'><script type=\"text/javascript\">document.write(window.l_select_unvisible);</script><br></th>"
    << "<th style='text-align:center'><script type=\"text/javascript\">document.write(window.l_select_duration);</script><br></th>"
    << "<th style='text-align:center'><script type=\"text/javascript\">document.write(window.l_select_delete);</script><br></th>"
    << "</tr>"
  ;

  for(MCUConferenceList::shared_iterator it = conferenceList.begin(); it != conferenceList.end(); ++it)
  {
    Conference *conference = it.GetObject();
      PString roomNumber = conference->GetNumber();
      //BOOL controlled = conference.GetForceScreenSplit();
      BOOL controlled = TRUE;
      BOOL allowRecord = GetConferenceParam(roomNumber, RoomAllowRecordKey, TRUE);
      BOOL moderated=FALSE; PString charModerated = "-";
      if(controlled) { charModerated = conference->IsModerated(); moderated=(charModerated=="+"); }
      if(charModerated=="-") charModerated = "<script type=\"text/javascript\">document.write(window.l_select_moderated_no);</script>";
      else charModerated = "<script type=\"text/javascript\">document.write(window.l_select_moderated_yes);</script>";
      PINDEX   visibleMemberCount = conference->GetVisibleMemberCount();
      PINDEX unvisibleMemberCount = conference->GetMemberCount() - visibleMemberCount;

      PString roomButton = "<span class=\"btn btn-large btn-";
      if(moderated) roomButton+="success";
      else if(controlled) roomButton+="primary";
      else roomButton+="inverse";
      roomButton += "\"";
      if(controlled) roomButton+=" onclick='document.getElementById(\"room\").value=\""
        + roomNumber + "\";document.forms[0].submit();'";
      roomButton += ">" + roomNumber + "</span>";

      PStringStream recordButton;
      if(allowRecord)
      {
        BOOL recState = (conference->conferenceRecorder && conference->conferenceRecorder->IsRunning());
        recordButton
        << "<input type='button' class='btn btn-large "
        << (recState ? "btn-inverse" : "btn-danger")
        << "' style='width:36px;height:36px;"
        << (recState ? "border-radius:0px" : "border-radius:18px")
        << "' value=' ' title='"
        << (recState ? "Stop recording" : "Start recording")
        << "' onclick=\"location.href='?action="
        << (recState ? "stop" : "start")
        << "Recorder&room="
        << PURL::TranslateString(roomNumber,PURL::QueryTranslation)
        << "'\">";
      }

      html << "<tr>"
        << "<td style='text-align:left'>"   << roomButton                            << "</td>"
        << "<td style='text-align:center'>" << recordButton                          << "</td>"
        << "<td style='text-align:center'>" << charModerated                         << "</td>"
        << "<td style='text-align:right'>"  << visibleMemberCount                    << "</td>"
        << "<td style='text-align:right'>"  << unvisibleMemberCount                  << "</td>"
        << "<td style='text-align:right'>"  << (PTime() - conference->GetStartTime()).AsString(0, PTimeInterval::IncludeDays, 10) << "</td>"
        << "<td style='text-align:center'><span class=\"btn btn-large btn-danger\" onclick=\"if(confirm('Вы уверены? Are you sure?')){location.href='?action=delete&room=" << PURL::TranslateString(roomNumber,PURL::QueryTranslation) << "';}\">X</span></td>"
        << "</tr>";
  }

  html << "</table></form>";

  EndPage(html,OpenMCU::Current().GetHtmlCopyright());
  {
    PStringStream message; PTime now; message
      << "HTTP/1.1 200 OK\r\n"
      << "Date: " << now.AsString(PTime::RFC1123, PTime::GMT) << "\r\n"
      << "Server: OpenMCU-ru\r\n"
      << "MIME-Version: 1.0\r\n"
      << "Cache-Control: no-cache, must-revalidate\r\n"
      << "Expires: Sat, 26 Jul 1997 05:00:00 GMT\r\n"
      << "Content-Type: text/html;charset=utf-8\r\n"
      << "Content-Length: " << html.GetLength() << "\r\n"
      << "Connection: Close\r\n"
      << "\r\n";  //that's the last time we need to type \r\n instead of just \n
    server.Write((const char*)message,message.GetLength());
  }
  server.Write((const char*)html,html.GetLength());
  server.flush();
  return TRUE;
}

///////////////////////////////////////////////////////////////

BOOL SelectRoomPage::Post(PHTTPRequest & request,
                          const PStringToString & data,
                          PHTML & msg)
{
  PString room = data("room");
  msg << OpenMCU::Current().GetEndpoint().SetRoomParams(data);
  return TRUE;
}

///////////////////////////////////////////////////////////////

RecordsBrowserPage::RecordsBrowserPage(OpenMCU & _app, PHTTPAuthority & auth)
  : PServiceHTTPString("Records", "", "text/html; charset=utf-8", auth),
    app(_app)
{}

///////////////////////////////////////////////////////////////

BOOL RecordsBrowserPage::OnGET (PHTTPServer & server, const PURL &url, const PMIMEInfo & info, const PHTTPConnectionInfo & connectInfo)
{
  { PHTTPRequest * req = CreateRequest(url, info, connectInfo.GetMultipartFormInfo(), server); // check authorization
    if(!CheckAuthority(server, *req, connectInfo)) {delete req; return FALSE;}
    delete req;
  }
  PStringToString data;
  { PString request=url.AsString(); PINDEX q;
    if((q=request.Find("?"))!=P_MAX_INDEX) { request=request.Mid(q+1,P_MAX_INDEX); PURL::SplitQueryVars(request,data); }
  }
  if(data.Contains("getfile")) // just download
  {
    PString filePathStr = OpenMCU::Current().vr_ffmpegDir + PATH_SEPARATOR + data("getfile");
    if(!PFile::Exists(filePathStr))
    { PHTTPRequest * request = CreateRequest(url, info, connectInfo.GetMultipartFormInfo(), server);
      request->entityBody = connectInfo.GetEntityBody();
      PStringStream msg; msg << ErrorPage(request->localAddr.AsString(), request->localPort, 404, "Not Found", "The file cannot be found",
        "The requested URL <a href=\"" + data("getfile") + "\">" + filePathStr + "</a> was not found on this server.<br/><br/>");
      delete request;
      PStringStream message; PTime now; message
        << "HTTP/1.1 404 Not Found\r\n"
        << "Date: " << now.AsString(PTime::RFC1123, PTime::GMT) << "\r\n"
        << "Server: OpenMCU-ru\r\n"
        << "MIME-Version: 1.0\r\n"
        << "Cache-Control: no-cache, must-revalidate\r\n"
        << "Expires: Sat, 26 Jul 1997 05:00:00 GMT\r\n"
        << "Content-Type: text/html;charset=utf-8\r\n"
        << "Content-Length: " << msg.GetLength() << "\r\n"
        << "Connection: Close\r\n"
        << "\r\n";  //that's the last time we need to type \r\n instead of just \n
      message << msg; server.Write((const char*)message,message.GetLength()); server.flush();
      return FALSE;
    }
    FILE *f=fopen(filePathStr,"rb"); if(f==NULL)
    { PHTTPRequest * request = CreateRequest(url, info, connectInfo.GetMultipartFormInfo(), server);
      request->entityBody = connectInfo.GetEntityBody();
      PStringStream msg; msg << ErrorPage(request->localAddr.AsString(), request->localPort, 403, "Forbidden", "The file cannot be downloaded",
        "You don't have permission to access <a href=\"" + data("getfile") + "\">" + filePathStr + "</a> on this server.<br/><br/>");
      delete request;
      PStringStream message; PTime now; message
        << "HTTP/1.1 403 Forbidden\r\n"
        << "Date: " << now.AsString(PTime::RFC1123, PTime::GMT) << "\r\n"
        << "Server: OpenMCU-ru\r\n"
        << "MIME-Version: 1.0\r\n"
        << "Cache-Control: no-cache, must-revalidate\r\n"
        << "Expires: Sat, 26 Jul 1997 05:00:00 GMT\r\n"
        << "Content-Type: text/html;charset=utf-8\r\n"
        << "Content-Length: " << msg.GetLength() << "\r\n"
        << "Connection: Close\r\n"
        << "\r\n";  //that's the last time we need to type \r\n instead of just \n
      message << msg; server.Write((const char*)message,message.GetLength()); server.flush();
      return FALSE;
    }
    fseek(f, 0, SEEK_END); size_t fileSize = ftell(f); rewind(f);

    PStringStream message; PTime now; message
      << "HTTP/1.1 200 OK\r\n"
      << "Date: " << now.AsString(PTime::RFC1123, PTime::GMT) << "\r\n"
      << "Server: OpenMCU-ru\r\n"
      << "MIME-Version: 1.0\r\n"
      << "Cache-Control: no-cache, must-revalidate\r\n"
      << "Expires: Sat, 26 Jul 1997 05:00:00 GMT\r\n"
      << "Content-Type: application/octet-stream\r\n"
      << "Content-Disposition: attachment; filename=\"" << PURL::TranslateString(data("getfile"),PURL::QueryTranslation) << "\"\r\n"
      << "Content-Transfer-Encoding: binary\r\n"
      << "Content-Length: " << fileSize << "\r\n"
      << "Connection: Close\r\n"
      << "\r\n";
    PTimeInterval oldTimeout=server.GetWriteTimeout();
    server.SetWriteTimeout(150000);
    if(!server.Write((const char*)message, message.GetLength())) {fclose(f); server.SetWriteTimeout(oldTimeout); return FALSE;}
    server.flush();

    char *buffer = (char*)malloc(PMIN(65536, fileSize));
    size_t p=0, result;
    while (p<fileSize)
    { size_t blockSize = PMIN(65536, fileSize-p);
      result=fread(buffer, 1, blockSize, f);
      if(blockSize != result)
      { PTRACE(1,"mcu.cxx\tFile read error: " << p << "/" << fileSize << ", filename: " << filePathStr);
        break;
      }
      if(!server.Write((const char*)buffer, result))
      { PTRACE(1,"mcu.cxx\tServer write error: " << p << "/" << fileSize << ", filename: " << filePathStr);
        break;
      }
      server.flush();
      p+=result;
    }
    fclose(f); free(buffer);
    server.SetWriteTimeout(oldTimeout);
    return TRUE;
  }

  PString dir=OpenMCU::Current().vr_ffmpegDir;
  BOOL isDir=PFile::Exists(dir); if(isDir)
  { PFileInfo info;
    PFile::GetInfo(PFilePath(dir), info);
    isDir=((info.type & 6) != 0);
  }
#ifdef _WIN32
  PString dir0(dir);
  if(dir0.Right(1)!="\\") dir0+='\\'; else dir=dir.Right(dir.GetLength()-1);
  dir0+='*';
  WIN32_FIND_DATA d;
  HANDLE f = FindFirstFileA(dir0, &d); if (f==INVALID_HANDLE_VALUE)
#else
  if(dir.Right(1)=="/") dir=dir.Right(dir.GetLength()-1);
  DIR *d; if(isDir) isDir=((d=opendir(dir)) != NULL); if(!isDir)
#endif
  { PHTTPRequest * request = CreateRequest(url, info, connectInfo.GetMultipartFormInfo(), server);
    request->entityBody = connectInfo.GetEntityBody();
    PStringStream msg; msg << ErrorPage(request->localAddr.AsString(), request->localPort, 404, "Not Found", "The file cannot be found",
      "The requested URL <a href=\"Records\">/Records</a> was not found on this server.<br/><br/>");
    delete request;
    PStringStream message; PTime now; message
      << "HTTP/1.1 404 Not Found\r\n"
      << "Date: " << now.AsString(PTime::RFC1123, PTime::GMT) << "\r\n"
      << "Server: OpenMCU-ru\r\n"
      << "MIME-Version: 1.0\r\n"
      << "Cache-Control: no-cache, must-revalidate\r\n"
      << "Expires: Sat, 26 Jul 1997 05:00:00 GMT\r\n"
      << "Content-Type: text/html;charset=utf-8\r\n"
      << "Content-Length: " << msg.GetLength() << "\r\n"
      << "Connection: Close\r\n"
      << "\r\n";  //that's the last time we need to type \r\n instead of just \n
    message << msg; server.Write((const char*)message,message.GetLength()); server.flush();
    return FALSE;
  }

  PStringStream shtml;
  BeginPage(shtml,"Records","window.l_records","window.l_info_records");
  shtml << "<h1>" << dir << "</h1>";

  PString wtd;
  if(data.Contains("deleteRecord"))
  {
    wtd=data("deleteRecord");
    shtml << "<div style='border:2px solid red;padding:8px;margin:4px;background-color:#fff'>"
      << "<script>document.write(window.l_recwilldeleted.replace(/\\*/g,\"" << wtd << "\") + "
      << "\"<center><a style='color:red' href='/Records?deleteRecordConfirmed=" << wtd
      << "'>\" + window.l_recwilldeleted_ok + \"</a> :: <a href='/Records'>\" + window.l_recwilldeleted_cancel + \"</a></center>"
      << "\");</script></div>";
  }
  else if(data.Contains("deleteRecordConfirmed"))
  {
    wtd=data("deleteRecordConfirmed");
    if(wtd.Find('/')==P_MAX_INDEX) if(wtd.Find('\\')==P_MAX_INDEX)
    {
      unlink(OpenMCU::Current().vr_ffmpegDir + PATH_SEPARATOR + wtd);
      shtml << "<div style='border:1px solid green'>"
      << "<script>document.write(window.l_recwasdeleted.replace(/\\*/g,\"" << wtd << "\"))</script>"
      << "</div>";
    }
  }

// directory d is now opened
  PStringArray fileList;
  unsigned long totalSize = 0;

#ifdef _WIN32
  do if(strcmp(".", d.cFileName) && strcmp("..", d.cFileName))
  {
    FILE *f=fopen(dir + '\\' + d.cFileName,"rb");
    if(f!=NULL)
    {
      fseek(f, 0, SEEK_END);
      long fileSize=ftell(f);
      totalSize+=fileSize;
      PStringStream r;
      r << d.cFileName << "," << fileSize;
      fileList.AppendString(r);
      fclose(f);
    }
  }
  while(FindNextFileA(f, &d) != 0 || GetLastError() != ERROR_NO_MORE_FILES);
  FindClose(f);
#else
  struct dirent *e;
  while((e = readdir(d))) if(strcmp(".", e->d_name) && strcmp("..", e->d_name))
  {
    FILE *f=fopen(dir + '/' + e->d_name,"rb");
    if(f!=NULL)
    {
      fseek(f, 0, SEEK_END);
      long fileSize=ftell(f);
      totalSize+=fileSize;
      PStringStream r; r << e->d_name << "," << fileSize;
      fileList.AppendString(r);
      fclose(f);
    }
  }
  closedir(d);
#endif

  PINDEX sortMode=0;
  if(data.Contains("sort")) {sortMode = data("sort").AsInteger(); if(sortMode<0 || sortMode>7) sortMode=0;}

  PString freeSpace;
  {
    PDirectory pd(dir);
    PInt64 t, f;
    DWORD cs;
    if(pd.GetVolumeSpace(t, f, cs))
    {
      PStringStream q;
      if(!t)t=1;
      q << dec << setprecision(1) << fixed << "<pre><script>document.write("
        << "window.l_totaldrivesize.replace(/\\*/g,\"" << (t        >>30) << "\") + \"<br/>\" + "
        << "window.l_recordstakesup.replace(/\\*/g,\"" << (totalSize>>30) << "\").replace(/\\%/g,\"" << (100.0*totalSize/t) << "%\") + \"<br/>\" + "
        << "window.l_freespaceleft.replace( /\\*/g,\"" << (f        >>30) << "\").replace(/\\%/g,\"" << (100.0*f        /t) << "%\")"
        << ")</script>";
      freeSpace=q;
    }
  }

  if(fileList.GetSize()==0) shtml << "<script>document.write(window.l_dir_no_records);</script> " << freeSpace; else
  {
    PString sm=PString("&nbsp;<span style='color:red'>&") + ((sortMode&1)?'u':'d') + "Arr;</span>";
    shtml << freeSpace << "<table style='border:2px solid #82b8e3'><tr>"
      << "<th class='h1' style='color:#afc'>N</th>"
      << "<th class='h1'><a style='color:#fff' href='/Records?sort=" << ((sortMode!=0)?'0':'1') << "'><script>document.write(window.l_startdatetime)</script>" << ((!(sortMode>>1))?sm:"") << "</a></th>"
      << "<th class='h1'><a style='color:#fff' href='/Records?sort=" << ((sortMode!=2)?'2':'3') << "'><script>document.write(window.l_connections_word_room)</script>" << (((sortMode>>1)==1)?sm:"") << "</a></th>"
      << "<th class='h1'><a style='color:#fff' href='/Records?sort=" << ((sortMode!=4)?'4':'5') << "'><script>document.write(window.l_resolution)</script>" << (((sortMode>>1)==2)?sm:"") << "</a></th>"
      << "<th class='h1'><a style='color:#fff' href='/Records?sort=" << ((sortMode!=6)?'6':'7') << "'><script>document.write(window.l_filesize)</script>" << (((sortMode>>1)==3)?sm:"") << "</a></th>"
      << "<th class='h1' style='color:#afc'><script>document.write(window.l_download)</script></th>"
      << "<th class='h1' style='color:#afc'><script>document.write(window.l_delete)</script></th>"
      << "</tr>";

    for(PINDEX i=0;i<fileList.GetSize()-1; i++)
    { PString s=fileList[i]; PINDEX pos1=s.Find("__"); if(pos1!=P_MAX_INDEX)
      { PString roomName1 = s.Left(pos1); s=s.Mid(pos1+2,P_MAX_INDEX);
        pos1=s.Find("__"); if(pos1!=P_MAX_INDEX)
        { PString dateTime1 = s.Left(pos1); s=s.Mid(pos1+2,P_MAX_INDEX);
          PString videoResolution1; pos1=s.Find('.'); PINDEX pos2=s.Find(',');
          if(pos1!=P_MAX_INDEX) videoResolution1=s.Left(pos1); else videoResolution1=s.Left(pos2);
          unsigned long fileSize1 = strtoul(s.Mid(pos2+1,P_MAX_INDEX),NULL,10);
          for(PINDEX j=i+1;j<fileList.GetSize(); j++)
          { PString s=fileList[j]; pos1=s.Find("__"); if(pos1!=P_MAX_INDEX)
            { PString roomName2 = s.Left(pos1); s=s.Mid(pos1+2,P_MAX_INDEX);
              pos1=s.Find("__"); if(pos1!=P_MAX_INDEX)
              { PString dateTime2 = s.Left(pos1); s=s.Mid(pos1+2,P_MAX_INDEX);
                PString videoResolution2; pos1=s.Find('.'); PINDEX pos2=s.Find(',');
                if(pos1!=P_MAX_INDEX) videoResolution2=s.Left(pos1); else videoResolution2=s.Left(pos2);
                unsigned long fileSize2 = strtoul(s.Mid(pos2+1,P_MAX_INDEX),NULL,10);
                BOOL sw;
                switch(sortMode)
                {
                  case 1:  sw=strcmp(dateTime2, dateTime1)<0; break;
                  case 2:  sw=strcmp(roomName2, roomName1)>0; break;
                  case 3:  sw=strcmp(roomName2, roomName1)<0; break;
                  case 4:  sw=strcmp(videoResolution2, videoResolution1)>0; break;
                  case 5:  sw=strcmp(videoResolution2, videoResolution1)<0; break;
                  case 6:  sw=fileSize2 > fileSize1; break;
                  case 7:  sw=fileSize2 < fileSize1; break;
                  default: sw=strcmp(dateTime2, dateTime1)>0; break;
                }
                if(sw)
                {
                  swap(fileList[i], fileList[j]);
                  dateTime1=dateTime2;
                  roomName1=roomName2;
                  videoResolution1=videoResolution2;
                  fileSize1=fileSize2;
                }
              }
            }
          }
        }
      }
    }

    for(PINDEX i=0;i<fileList.GetSize(); i++)
    { PString s0=fileList[i]; PINDEX pos1=s0.Find("__"); if(pos1!=P_MAX_INDEX)
      { PString roomName = s0.Left(pos1); PString s=s0.Mid(pos1+2,P_MAX_INDEX);
        pos1=s.Find("__"); if(pos1!=P_MAX_INDEX)
        { PString dateTime = s.Left(pos1); s=s.Mid(pos1+2,P_MAX_INDEX);
          PString videoResolution; pos1=s.Find('.'); PINDEX pos2=s.Find(',');
          if(pos1!=P_MAX_INDEX) videoResolution=s.Left(pos1); else videoResolution=s.Left(pos2);
          PString fileSize, fileSize0(s.Mid(pos2+1,P_MAX_INDEX).AsInteger());
          PINDEX l=fileSize0.GetLength();
          for(PINDEX j=l-1; j>=0; j--) { fileSize+=fileSize0[l-j-1]; if(!(j%3)) if(j!=0) fileSize+=' '; }
          PString fileName = s0.Left(s0.Find(','));
          shtml << "<tr>"
            << "<td class='h0' style='text-align:right'>" << (i+1) << "</td>"
            << "<td class='h0'>" << dateTime.Mid(7,2) << '.' << dateTime.Mid(5,2) << '.' << dateTime.Mid(0,4) << ' ' << dateTime.Mid(10,2) << ':' << dateTime.Mid(12,2) << "</td>"
            << "<td class='h0'>" << roomName << "</td>"
            << "<td class='h0' style='text-align:right'>" << videoResolution << "</td>"
            << "<td class='h0' style='text-align:right'>" << fileSize << "</td>"
            << "<td class='h0'><a href='/Records?getfile=" << fileName << "' download style='color:#00a;text-decoration:none'><script>document.write(window.l_download)</script></a></td>"
            << "<td class='h0'><a href='/Records?deleteRecord=" << fileName << "' style='color:#00a;text-decoration:none'><script>document.write(window.l_delete)</script></a></td>"
            << "</tr>";
        }
      }
    }
    shtml << "</table>";

  } // if(fileList.GetSize()==0) ... else {

  EndPage(shtml,OpenMCU::Current().GetHtmlCopyright());
  { PStringStream message; PTime now; message
      << "HTTP/1.1 200 OK\r\n"
      << "Date: " << now.AsString(PTime::RFC1123, PTime::GMT) << "\r\n"
      << "Server: OpenMCU-ru\r\n"
      << "MIME-Version: 1.0\r\n"
      << "Cache-Control: no-cache, must-revalidate\r\n"
      << "Expires: Sat, 26 Jul 1997 05:00:00 GMT\r\n"
      << "Content-Type: text/html;charset=utf-8\r\n"
      << "Content-Length: " << shtml.GetLength() << "\r\n"
      << "Connection: Close\r\n"
      << "\r\n";  //that's the last time we need to type \r\n instead of just \n
    server.Write((const char*)message,message.GetLength());
  }
  server.Write((const char*)shtml,shtml.GetLength());
  server.flush();
  return TRUE;
}

///////////////////////////////////////////////////////////////
