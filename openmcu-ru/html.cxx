#include <ptlib.h>
#include "mcu.h"
#include "html.h"
#include <stdio.h>
#include <string.h>

#if USE_LIBJPEG
extern "C" {
#include <jpeglib.h>
}
#endif

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

void BeginPage (PStringStream &html, const char *ptitle, const char *title, const char *quotekey)
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
  cfg = MCUConfig(section);
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
  cfg = MCUConfig(section);

  PStringStream html_begin, html_end, html_page, s;
  s << BeginTable();

  // Reset section
  s << BoolField("RESTORE DEFAULTS", FALSE);
  // Language
  s << SelectField("Language", cfg.GetString("Language"), ",EN,RU");
  // OpenMCU Server Id
  s << StringField("OpenMCU Server Id", cfg.GetString("OpenMCU Server Id", mcu.GetName()+" v"+mcu.GetVersion()), 35);

#if P_SSL
  // SSL certificate file.
  s << SeparatorField("Security");
  PString certificateFile = cfg.GetString(HTTPCertificateFileKey, "server.pem");
  s << StringField(HTTPCertificateFileKey, certificateFile);
  if (!SetServerCertificate(certificateFile, TRUE)) {
    PSYSTEMLOG(Fatal, "MCU\tCould not load certificate \"" << certificateFile << '"');
    return FALSE;
  }
#endif

  s << SeparatorField("Port setup");
  // HTTP Port number to use.
  s << SeparatorField();
  s << IntegerField(HttpPortKey, cfg.GetInteger(HttpPortKey, DefaultHTTPPort), 1, 32767);
  // RTP Port Setup
  s << IntegerField("RTP Base Port", cfg.GetInteger("RTP Base Port", 0), 0, 65535, 10, "0 = auto, Example: base=5000, max=6000");
  s << IntegerField("RTP Max Port", cfg.GetInteger("RTP Max Port", 0), 0, 65535);

  s << SeparatorField("Log setup");
#if PTRACING
  // Trace level
  s << SelectField(TraceLevelKey, cfg.GetString(TraceLevelKey, DEFAULT_TRACE_LEVEL), "0,1,2,3,4,5,6,9", 120, "0=No tracing ... 6=Very detailed");
  s << IntegerField(TraceRotateKey, cfg.GetInteger(TraceRotateKey, 0), 0, 200, 10, "0 (don't rotate) ... 200");
#endif
#ifdef SERVER_LOGS
  // Log level for messages
  s << SelectField(LogLevelKey, cfg.GetString(LogLevelKey, DEFAULT_LOG_LEVEL), "0,1,2,3,4,5", 120, "1=Fatal only, 2=Errors, 3=Warnings, 4=Info, 5=Debug");
  // Log filename
  s << StringField(CallLogFilenameKey, mcu.logFilename, 35);
#endif
  // Buffered events
  s << IntegerField(HttpLinkEventBufferKey, cfg.GetInteger(HttpLinkEventBufferKey, 100), 10, 1000, 10, "range: 10...1000");
  // Copy web log from Room Control Page to call log
  s << BoolField("Copy web log to call log", mcu.copyWebLogToLog, "check if you want to store event log from Room Control Page");

#if OPENMCU_VIDEO
  s << SeparatorField("Video setup");
  s << BoolField("Enable video", cfg.GetBoolean("Enable video", TRUE));
  s << BoolField(ForceSplitVideoKey, cfg.GetBoolean(ForceSplitVideoKey, TRUE));
#endif

  s << SeparatorField("Room setup");
  // Default room
  s << StringField(DefaultRoomKey, cfg.GetString(DefaultRoomKey, DefaultRoom));
  // create/don't create empty room with default name at start
  s << BoolField(CreateEmptyRoomKey, cfg.GetBoolean(CreateEmptyRoomKey, FALSE));
  // recall last template after room created
  s << BoolField(RecallLastTemplateKey, cfg.GetBoolean(RecallLastTemplateKey, FALSE));
  // reject duplicate name
  s << BoolField(RejectDuplicateNameKey, cfg.GetBoolean(RejectDuplicateNameKey, TRUE));
  // get conference time limit 
  s << IntegerField(DefaultRoomTimeLimitKey, cfg.GetInteger(DefaultRoomTimeLimitKey, 0), 0, 10800);
  // allow/disallow self-invite:
  s << BoolField(AllowLoopbackCallsKey, cfg.GetBoolean(AllowLoopbackCallsKey, FALSE));

  s << SeparatorField("Video recorder setup");
  PString pathInfo, dirInfo;
  pathInfo =
      "<b>%V</b> - input video stream<br><b>%A</b> - input audio stream<br><br><b>%F</b> - frame size<br>"
      "<b>%R</b> - video frame rate<br><br><b>%S</b> - audio sample rate<br><b>%C</b> - number of channels for audio<br><br>";

  if(!PFile::Exists(mcu.vr_ffmpegPath)) pathInfo += "<b><font color=red>ffmpeg doesn't exist - check the path!</font></b>";
  else
  { PFileInfo info;
    PFilePath path(mcu.vr_ffmpegPath);
    PFile::GetInfo(path, info);
    if(!(info.type & 3)) pathInfo += "<b><font color=red>Warning: ffmpeg neither file, nor symlink!</font></b>";
    else if(!(info.permissions & 0111)) pathInfo += "<b><font color=red>ffmpeg permissions check failed</font></b>";
    else
    {
      if(!PDirectory::Exists(mcu.vr_ffmpegDir)) if(!PFile::Exists(mcu.vr_ffmpegDir)) { PDirectory::Create(mcu.vr_ffmpegDir,0700); PThread::Sleep(50); }
      if(!PDirectory::Exists(mcu.vr_ffmpegDir)) dirInfo += "<b><font color=red>Directory does not exist: "+mcu.vr_ffmpegDir+"</font></b>";
      else
      { PFileInfo info;
        PFilePath path(mcu.vr_ffmpegDir);
        PFile::GetInfo(path, info);
        if(!(info.type & 6)) dirInfo += "<b><font color=red>Warning: output directory neither directory, nor symlink!</font></b>";
        else if(!(info.permissions & 0222)) dirInfo += "<b><font color=red>output directory permissions check failed</font></b>";
        else pathInfo += "<b><font color=green>Looks good.</font> Execution script preview:</b><br><tt>"+mcu.ffmpegCall+"</tt>";
      }
    }
  }

  s << StringField(RecorderFfmpegDirKey, mcu.vr_ffmpegDir, 35, dirInfo);
  s << StringField(RecorderFfmpegPathKey, mcu.vr_ffmpegPath, 35, pathInfo, FALSE, 7);
  s << StringField(RecorderFfmpegOptsKey, mcu.vr_ffmpegOpts, 35, "", FALSE, 0);
  s << IntegerField(RecorderFrameWidthKey, mcu.vr_framewidth, 176, 1920, 35,"",FALSE,0);
  s << IntegerField(RecorderFrameHeightKey, mcu.vr_frameheight, 144, 1152, 35,"",FALSE,0);
  s << IntegerField(RecorderFrameRateKey, mcu.vr_framerate, 1, 100, 35,"",FALSE,0);
  s << IntegerField(RecorderSampleRateKey, mcu.vr_sampleRate,8000,192000,35,"",FALSE,0);
  s << SelectField(RecorderAudioChansKey, mcu.vr_audioChans, "1,2,3,4,5,6,7,8",120,"",FALSE,0);


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

ControlCodesPConfigPage::ControlCodesPConfigPage(PHTTPServiceProcess & app,const PString & title, const PString & section, const PHTTPAuthority & auth)
    : TablePConfigPage(app,title,section,auth)
{
  cfg = MCUConfig(section);
  PStringStream html_begin, html_end, html_page, s;
  buttonUp = buttonDown = buttonClone = buttonDelete = 1;
  s << BeginTable();

  s << NewRowColumn(JsLocale("window.l_name_code"));
  s << ColumnItem(JsLocale("window.l_name_action"));
  s << ColumnItem(JsLocale("window.l_name_message"));

  PStringList keys = cfg.GetKeys();
  for(PINDEX i = 0; i < keys.GetSize(); i++)
  {
    PString name = keys[i];
    PStringArray params = cfg.GetString(keys[i]).Tokenise(",");
    s << NewRowInput(name);
    s << SelectItem(name, params[0], MCUControlCodes, 280);
    s << StringItem(name, params[1], 35);
  }
  if(keys.GetSize() == 0)
  {
    s << NewRowInput("1");
    s << SelectItem("1", "", MCUControlCodes, 280);
    s << StringItem("1", "", 35);
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
  cfg = MCUConfig(section);
  PStringStream html_begin, html_end, html_page, s;
  buttonUp = buttonDown = buttonClone = buttonDelete = 1;
  s << BeginTable();

  s << NewRowColumn(JsLocale("window.l_name_code"));
  s << ColumnItem(JsLocale("window.l_name_roomname"));

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
    s << StringItem("1", "room101");
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
  cfg = MCUConfig(section);
  PStringStream html_begin, html_end, html_page, s;
  buttonUp = buttonDown = buttonClone = buttonDelete = 1;
  s << BeginTable();

  s << NewRowColumn(JsLocale("window.l_name_user"));
  s << ColumnItem(JsLocale("window.l_name_password"));
  s << ColumnItem(JsLocale("window.l_name_group"));

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
    s << PasswordItem(name, PHTTPPasswordField::Decrypt(params[0]));
    s << SelectItem(name, params[1], groups, 300);
  }
  if(keys.GetSize() == 0)
  {
    s << NewRowInput("admin");
    s << PasswordItem("admin", PHTTPPasswordField::Decrypt(""));
    s << SelectItem("admin", "administrator", groups, 300);
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
  cfg = MCUConfig(section);
  PStringStream html_begin, html_end, html_page, s;
  s << BeginTable();

  s << NewRowColumn(JsLocale("window.l_name_group"));

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
  cfg = MCUConfig(section);
  PStringStream html_begin, html_end, html_page, s;
  s << BeginTable();

  s << BoolField("RESTORE DEFAULTS", FALSE);
  s << IntegerField("Video frame rate", cfg.GetInteger("Video frame rate", DefaultVideoFrameRate), 1, MAX_FRAME_RATE, 12, "range: 1.."+PString(MAX_FRAME_RATE)+" (for outgoing video)");

  s << SeparatorField("H.263");
  s << IntegerField("H.263 Max Bit Rate", cfg.GetInteger("H.263 Max Bit Rate", 0), 0, 4000, 12, "range 64..4000 kbit (for outgoing video, 0 disable)");
  s << IntegerField("H.263 Tx Key Frame Period", cfg.GetInteger("H.263 Tx Key Frame Period", 125), 0, 600, 12, "range 0..600 (for outgoing video, the number of pictures in a group of pictures, or 0 for intra_only)");

  s << SeparatorField("H.263p");
  s << IntegerField("H.263p Max Bit Rate", cfg.GetInteger("H.263p Max Bit Rate", 0), 0, 4000, 12, "range 64..4000 kbit (for outgoing video, 0 disable)");
  s << IntegerField("H.263p Tx Key Frame Period", cfg.GetInteger("H.263p Tx Key Frame Period", 125), 0, 600, 12, "range 0..600 (for outgoing video, the number of pictures in a group of pictures, or 0 for intra_only)");

  s << SeparatorField("H.264");
  s << IntegerField("H.264 Max Bit Rate", cfg.GetInteger("H.264 Max Bit Rate", 0), 0, 4000, 12, "range 64..4000 kbit (for outgoing video, 0 disable)");
  s << IntegerField("H.264 Encoding Threads", cfg.GetInteger("H.264 Encoding Threads", 0), 0, 64, 12, "range 0..64 (0 auto)");

  s << SeparatorField("VP8");
  s << IntegerField("VP8 Max Bit Rate", cfg.GetInteger("VP8 Max Bit Rate", 0), 0, 4000, 12, "range 64..4000 kbit (for outgoing video, 0 disable)");
  s << IntegerField("VP8 Encoding Threads", cfg.GetInteger("VP8 Encoding Threads", 0), 0, 64, 12, "range 0..64 (0 default)");
  s << IntegerField("VP8 Encoding CPU Used", cfg.GetInteger("VP8 Encoding CPU Used", 0), 0, 16, 12, "range: 0..16 (Values greater than 0 will increase encoder speed at the expense of quality)");

  s << EndTable();
  BuildHTML("");
  BeginPage(html_begin, "Video settings", "window.l_param_video", "window.l_info_param_video");
  EndPage(html_end,OpenMCU::Current().GetHtmlCopyright());
  html_page << html_begin << s << html_end;
  string = html_page;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

RecordPConfigPage::RecordPConfigPage(PHTTPServiceProcess & app,const PString & title, const PString & section, const PHTTPAuthority & auth)
    : TablePConfigPage(app,title,section,auth)
{
  OpenMCU & mcu = OpenMCU::Current();
  cfg = MCUConfig(section);

  PStringStream html_begin, html_end, html_page, s;
  s << BeginTable();

  // video recorder setup
  PString pathInfo, dirInfo;
  pathInfo =
      "<b>%V</b> - input video stream<br><b>%A</b> - input audio stream<br><br><b>%F</b> - frame size<br>"
      "<b>%R</b> - video frame rate<br><br><b>%S</b> - audio sample rate<br><b>%C</b> - number of channels for audio<br><br>";

  if(!PFile::Exists(mcu.vr_ffmpegPath)) pathInfo += "<b><font color=red>ffmpeg doesn't exist - check the path!</font></b>";
  else
  { PFileInfo info;
    PFilePath path(mcu.vr_ffmpegPath);
    PFile::GetInfo(path, info);
    if(!(info.type & 3)) pathInfo += "<b><font color=red>Warning: ffmpeg neither file, nor symlink!</font></b>";
    else if(!(info.permissions & 0111)) pathInfo += "<b><font color=red>ffmpeg permissions check failed</font></b>";
    else
    {
      if(!PDirectory::Exists(mcu.vr_ffmpegDir)) if(!PFile::Exists(mcu.vr_ffmpegDir)) { PDirectory::Create(mcu.vr_ffmpegDir,0700); PThread::Sleep(50); }
      if(!PDirectory::Exists(mcu.vr_ffmpegDir)) dirInfo += "<b><font color=red>Directory does not exist: "+mcu.vr_ffmpegDir+"</font></b>";
      else
      { PFileInfo info;
        PFilePath path(mcu.vr_ffmpegDir);
        PFile::GetInfo(path, info);
        if(!(info.type & 6)) dirInfo += "<b><font color=red>Warning: output directory neither directory, nor symlink!</font></b>";
        else if(!(info.permissions & 0222)) dirInfo += "<b><font color=red>output directory permissions check failed</font></b>";
        else pathInfo += "<b><font color=green>Looks good.</font> Execution script preview:</b><br><tt>"+mcu.ffmpegCall+"</tt>";
      }
    }
  }

  s << BoolField("RESTORE DEFAULTS", FALSE);
  s << StringField(RecorderFfmpegDirKey, mcu.vr_ffmpegDir, 35, dirInfo);
  s << StringField(RecorderFfmpegPathKey, mcu.vr_ffmpegPath, 35, pathInfo, FALSE, 7);
  s << StringField(RecorderFfmpegOptsKey, mcu.vr_ffmpegOpts, 35, "", FALSE, 0);
  s << IntegerField(RecorderFrameWidthKey, mcu.vr_framewidth, 176, 1920, 35,"",FALSE,0);
  s << IntegerField(RecorderFrameHeightKey, mcu.vr_frameheight, 144, 1152, 35,"",FALSE,0);
  s << IntegerField(RecorderFrameRateKey, mcu.vr_framerate, 1, 100, 35,"",FALSE,0);
  s << IntegerField(RecorderSampleRateKey, mcu.vr_sampleRate,8000,192000,35,"",FALSE,0);
  s << SelectField(RecorderAudioChansKey, mcu.vr_audioChans, "1,2,3,4,5,6,7,8",120,"",FALSE,0);

  s << EndTable();
  BuildHTML("");
  BeginPage(html_begin, "Record", "window.l_param_record","window.l_info_param_record");
  EndPage(html_end,OpenMCU::Current().GetHtmlCopyright());
  html_page << html_begin << s << html_end;
  string = html_page;
}

///////////////////////////////////////////////////////////////

H323EndpointsPConfigPage::H323EndpointsPConfigPage(PHTTPServiceProcess & app,const PString & title, const PString & section, const PHTTPAuthority & auth)
    : TablePConfigPage(app,title,section,auth)
{
  cfg = MCUConfig(section);
  OpenMCU & mcu = OpenMCU::Current();

  firstEditRow = 2;
  PStringStream html_begin, html_end, html_page, s;
  buttonUp = buttonDown = buttonClone = buttonDelete = 1;
  s << BeginTable();
  s << NewRowColumn(JsLocale("window.l_name_address"));
  //s << "";
  //s << "";
  //s << "";
  s << ColumnItem(JsLocale("window.l_name_display_name_override"));
  s << ColumnItem(JsLocale("window.l_name_preferred_frame_rate_from_mcu"));
  s << ColumnItem(JsLocale("window.l_name_preferred_bandwidth_from_mcu"));
  //s << "";
  //s << "";
  s << ColumnItem(JsLocale("window.l_name_audio_codec"));
  s << ColumnItem(JsLocale("window.l_name_video_codec"));

  PString audioCaps, videoCaps;
  if(mcu.GetEndpoint().tsCaps != NULL && mcu.GetEndpoint().tvCaps != NULL)
  {
    PINDEX tsNum = 0; while(mcu.GetEndpoint().tsCaps[tsNum]!=NULL) { audioCaps += ","+PString(mcu.GetEndpoint().tsCaps[tsNum]); tsNum++; }
    PINDEX tvNum = 0; while(mcu.GetEndpoint().tvCaps[tvNum]!=NULL) { videoCaps += ","+PString(mcu.GetEndpoint().tvCaps[tvNum]); tvNum++; }
  }

  PStringList keys = cfg.GetKeys();
  if(keys.GetStringsIndex("*") == P_MAX_INDEX)
    keys.InsertAt(0, new PString("*"));
  if(keys.GetSize() == 1)
    keys.AppendString("test");

  for(PINDEX i = 0; i < keys.GetSize(); i++)
  {
    PString name = keys[i];
    PString params = cfg.GetString(keys[i]);

    if(name == "*") s << NewRowInput(name, 15, TRUE);
    else s << NewRowInput(name, 15);
    s << HiddenItem(name);
    s << HiddenItem(name);
    s << HiddenItem(name);
    if(name == "*") s << StringItem(name, "", 12, TRUE);
    else s << StringItem(name, params.Tokenise(",")[h323EndpointOptionsOrder.GetStringsIndex("Display name override")]);
    s << StringItem(name, params.Tokenise(",")[h323EndpointOptionsOrder.GetStringsIndex("Preferred frame rate from MCU")]);
    s << StringItem(name, params.Tokenise(",")[h323EndpointOptionsOrder.GetStringsIndex("Preferred bandwidth from MCU")]);
    s << HiddenItem(name);
    s << HiddenItem(name);
    s << SelectItem(name, params.Tokenise(",")[h323EndpointOptionsOrder.GetStringsIndex("Audio codec")], audioCaps);
    s << SelectItem(name, params.Tokenise(",")[h323EndpointOptionsOrder.GetStringsIndex("Video codec")], videoCaps);
  }

  s << EndTable();
  BuildHTML("");
  BeginPage(html_begin, section, "window.l_param_h323_endpoints", "window.l_info_param_h323_endpoints");
  EndPage(html_end,OpenMCU::Current().GetHtmlCopyright());
  html_page << html_begin << s << html_end;
  string = html_page;
}

///////////////////////////////////////////////////////////////

SipEndpointsPConfigPage::SipEndpointsPConfigPage(PHTTPServiceProcess & app,const PString & title, const PString & section, const PHTTPAuthority & auth)
    : TablePConfigPage(app,title,section,auth)
{
  cfg = MCUConfig(section);
  OpenMCU & mcu = OpenMCU::Current();

  firstEditRow = 2;
  PStringStream html_begin, html_end, html_page, s;
  buttonUp = buttonDown = buttonClone = buttonDelete = 1;
  s << BeginTable();
  s << NewRowColumn(JsLocale("window.l_name_address"));
  s << ColumnItem(JsLocale("window.l_name_outgoing_transport"));
  //s << "";
  //s << "";
  s << ColumnItem(JsLocale("window.l_name_display_name_override"));
  s << ColumnItem(JsLocale("window.l_name_preferred_frame_rate_from_mcu"));
  s << ColumnItem(JsLocale("window.l_name_preferred_bandwidth_from_mcu"));
  s << ColumnItem(JsLocale("window.l_name_preferred_bandwidth_to_mcu"));
  //s << "";
  s << ColumnItem(JsLocale("window.l_name_audio_codec"));
  s << ColumnItem(JsLocale("window.l_name_video_codec"));

  PString audioCaps, videoCaps;
  if(mcu.GetEndpoint().tsCaps != NULL && mcu.GetEndpoint().tvCaps != NULL)
  {
    PINDEX tsNum = 0; while(mcu.GetEndpoint().tsCaps[tsNum]!=NULL) { audioCaps += ","+PString(mcu.GetEndpoint().tsCaps[tsNum]); tsNum++; }
    PINDEX tvNum = 0; while(mcu.GetEndpoint().tvCaps[tvNum]!=NULL) { videoCaps += ","+PString(mcu.GetEndpoint().tvCaps[tvNum]); tvNum++; }
  }

  PStringList keys = cfg.GetKeys();
  if(keys.GetStringsIndex("*") == P_MAX_INDEX)
    keys.InsertAt(0, new PString("*"));
  if(keys.GetSize() == 1)
    keys.AppendString("test");

  for(PINDEX i = 0; i < keys.GetSize(); i++)
  {
    PString name = keys[i];
    PString params = cfg.GetString(keys[i]);

    if(name == "*") s << NewRowInput(name, 15, TRUE);
    else s << NewRowInput(name, 15);
    s << SelectItem(name, params.Tokenise(",")[sipEndpointOptionsOrder.GetStringsIndex("Outgoing transport")], "transport=*,transport=udp,transport=tcp");
    s << HiddenItem(name);
    s << HiddenItem(name);
    if(name == "*") s << StringItem(name, "", 12, TRUE);
    else s << StringItem(name, params.Tokenise(",")[sipEndpointOptionsOrder.GetStringsIndex("Display name override")]);
    s << StringItem(name, params.Tokenise(",")[sipEndpointOptionsOrder.GetStringsIndex("Preferred frame rate from MCU")]);
    s << StringItem(name, params.Tokenise(",")[sipEndpointOptionsOrder.GetStringsIndex("Preferred bandwidth from MCU")]);
    s << StringItem(name, params.Tokenise(",")[sipEndpointOptionsOrder.GetStringsIndex("Preferred bandwidth to MCU")]);
    s << HiddenItem(name);
    s << SelectItem(name, params.Tokenise(",")[sipEndpointOptionsOrder.GetStringsIndex("Audio codec")], audioCaps);
    s << SelectItem(name, params.Tokenise(",")[sipEndpointOptionsOrder.GetStringsIndex("Video codec")], videoCaps);
  }
  s << EndTable();

  BuildHTML("");
  BeginPage(html_begin, section, "window.l_param_sip_endpoints", "window.l_info_param_sip_endpoints");
  EndPage(html_end,OpenMCU::Current().GetHtmlCopyright());
  html_page << html_begin << s << html_end;
  string = html_page;
}

///////////////////////////////////////////////////////////////

ProxySIPPConfigPage::ProxySIPPConfigPage(PHTTPServiceProcess & app,const PString & title, const PString & section, const PHTTPAuthority & auth)
    : TablePConfigPage(app,title,section,auth)
{
  cfg = MCUConfig(section);

  PStringStream html_begin, html_end, html_page, s;
  buttonUp = buttonDown = buttonClone = buttonDelete = 1;
  s << BeginTable();
  s << NewRowColumn(JsLocale("window.l_name_roomname"));
  s << ColumnItem(JsLocale("window.l_name_register"));
  s << ColumnItem(JsLocale("window.l_name_address_sip_proxy"));
  s << ColumnItem(JsLocale("window.l_name_user"));
  s << ColumnItem(JsLocale("window.l_name_password"));
  s << ColumnItem(JsLocale("window.l_name_expires"));

  PStringList keys = cfg.GetKeys();
  for(PINDEX i = 0; i < keys.GetSize(); i++)
  {
    PString name = keys[i];
    PString params = cfg.GetString(keys[i]);
    s << NewRowInput(name);
    if(params.Tokenise(",")[0] == "TRUE") s << BoolItem(name, 1);
    else s << BoolItem(name, 0);
    s << StringItem(name, params.Tokenise(",")[1]);
    s << StringItem(name, params.Tokenise(",")[2]);
    s << StringItem(name, params.Tokenise(",")[3]);
    s << IntegerItem(name, atoi(params.Tokenise(",")[4]), 60, 3600);
  }
  if(keys.GetSize() == 0)
  {
    s << NewRowInput("room101");
    s << BoolItem("room101", 0);
    s << StringItem("room101", "");
    s << StringItem("room101", "");
    s << StringItem("room101", "");
    s << IntegerItem("room101", 60, 60, 3600);
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
  cfg = MCUConfig(section);

  firstEditRow = 2;
  PStringStream html_begin, html_end, html_page, s;
  buttonUp = buttonDown = buttonClone = buttonDelete = 1;
  s << BeginTable();
  s << NewRowColumn(JsLocale("window.l_name_roomname"));
  s << ColumnItem(JsLocale("window.l_name_access"));
  s << ColumnItem("'user1@domain user2@ @domain @@via'");

  PStringList keys = cfg.GetKeys();
  if(keys.GetStringsIndex("*") == P_MAX_INDEX)
    keys.InsertAt(0, new PString("*"));
  if(keys.GetSize() == 1)
    keys.AppendString("room101");

  for(PINDEX i = 0; i < keys.GetSize(); i++)
  {
    PString name = keys[i];
    PString access = cfg.GetString(keys[i]).Tokenise(",")[0].ToLower();
    PString params = cfg.GetString(keys[i]).Tokenise(",")[1];
    if(name == "*") s << NewRowInput(name, 15, TRUE);
    else s << NewRowInput(name, 15);
    s << SelectItem(name, access, "allow,deny");
    if(name == "*") s << StringItem(name, "", 50, TRUE);
    else s << StringItem(name, params, 50);
  }
  s << EndTable();

  BuildHTML("");
  BeginPage(html_begin, "Access Rules", "window.l_param_access_rules", "window.l_info_param_access_rules");
  EndPage(html_end,OpenMCU::Current().GetHtmlCopyright());
  html_page << html_begin << s << html_end;
  string = html_page;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

H323PConfigPage::H323PConfigPage(PHTTPServiceProcess & app,const PString & title, const PString & section, const PHTTPAuthority & auth)
    : TablePConfigPage(app,title,section,auth)
{
  OpenMCU & mcu = OpenMCU::Current();
  cfg = MCUConfig(section);

  mcu.GetEndpoint().Initialise(cfg);
  if(mcu.GetEndpoint().behind_masq){PStringStream msq; msq<<"Masquerading as "<<*(mcu.GetEndpoint().masqAddressPtr); mcu.HttpWriteEvent(msq);}

  PStringStream html_begin, html_end, html_page, s;
  s << BeginTable();
  s << BoolField("RESTORE DEFAULTS", FALSE);

  s << ArrayField(InterfaceKey, cfg.GetString(InterfaceKey));

  s << StringField(NATRouterIPKey, cfg.GetString(NATRouterIPKey));
  s << StringField(NATTreatAsGlobalKey, cfg.GetString(NATTreatAsGlobalKey));

  s << BoolField(DisableFastStartKey, cfg.GetBoolean(DisableFastStartKey, TRUE));
  s << BoolField(DisableH245TunnelingKey, cfg.GetBoolean(DisableH245TunnelingKey, FALSE));

  PString labels = "No gatekeeper,Find gatekeeper,Use gatekeeper";
  s << SelectField(GatekeeperModeKey, cfg.GetString(GatekeeperModeKey, labels[0]), labels, 160);
  s << StringField(GatekeeperKey, cfg.GetString(GatekeeperKey));
  s << StringField(GatekeeperUserNameKey, cfg.GetString(GatekeeperUserNameKey, "MCU"));
  s << StringField(GatekeeperPasswordKey, PHTTPPasswordField::Decrypt(cfg.GetString(GatekeeperPasswordKey)));
  s << StringField(GatekeeperAliasKey, cfg.GetString(GatekeeperAliasKey,"MCU*"));
  s << ArrayField(GatekeeperPrefixesKey, cfg.GetString(GatekeeperPrefixesKey));

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
  cfg = MCUConfig(section);
  PStringStream html_begin, html_end, html_page, s;
  s << BeginTable();
  s << BoolField("RESTORE DEFAULTS", FALSE);

  PStringArray sipListener;
  PStringArray data = cfg.GetString(SipListenerKey, "0.0.0.0,transport=*").Tokenise(",");
  for(PINDEX i = 0, j = 0; i < data.GetSize(); i++)
  {
    if(data[i].Find("transport") != P_MAX_INDEX) { sipListener[j-1] += ";"+data[i]; continue; }
    sipListener.AppendString(data[i]);
    j++;
  }

  PString item = NewRowText(SipListenerKey);
  item += NewItemArray(SipListenerKey);
  for(PINDEX i = 0; i < sipListener.GetSize(); i++)
  {
    PString url = sipListener[i].Tokenise(";")[0];
    PString transport = sipListener[i].Tokenise(";")[1];
    if(url == "") url = "0.0.0.0";
    item += StringItemArray(SipListenerKey, url, 20);
    item += SelectItem(SipListenerKey, transport, "transport=*,transport=udp,transport=tcp");
    if(url == "0.0.0.0") url += " :5060";
    sipListener[i] = "sip:"+url+";"+transport;
  }
  item += EndItemArray();
  item += InfoItem("");
  s << item;

  s << BoolField(SIPReInviteKey, cfg.GetBoolean(SIPReInviteKey, TRUE));

#if OPENMCU_VIDEO
  mcu.h264DefaultLevelForSip = cfg.GetString(H264LevelForSIPKey, "9").AsInteger();
  if(mcu.h264DefaultLevelForSip < 9) mcu.h264DefaultLevelForSip=9;
  else if(mcu.h264DefaultLevelForSip>13 && mcu.h264DefaultLevelForSip<20) mcu.h264DefaultLevelForSip=13;
  else if(mcu.h264DefaultLevelForSip>22 && mcu.h264DefaultLevelForSip<30) mcu.h264DefaultLevelForSip=22;
  else if(mcu.h264DefaultLevelForSip>32 && mcu.h264DefaultLevelForSip<40) mcu.h264DefaultLevelForSip=32;
  else if(mcu.h264DefaultLevelForSip>42 && mcu.h264DefaultLevelForSip<50) mcu.h264DefaultLevelForSip=42;
  else if(mcu.h264DefaultLevelForSip>51) mcu.h264DefaultLevelForSip=51;
  s << SelectField(H264LevelForSIPKey, PString(mcu.h264DefaultLevelForSip), "9,10,11,12,13,20,21,22,30,31,32,40,41,42,50,51");
#endif

  mcu.sipendpoint->Resume();
  mcu.sipendpoint->sipListener = sipListener;
  mcu.sipendpoint->restart = 1;

  s << EndTable();
  BuildHTML("");
  BeginPage(html_begin, "SIP settings", "window.l_param_sip", "window.l_info_param_sip");
  EndPage(html_end,OpenMCU::Current().GetHtmlCopyright());
  html_page << html_begin << s << html_end;
  string = html_page;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

SectionPConfigPage::SectionPConfigPage(PHTTPServiceProcess & app,const PString & title, const PString & section, const PHTTPAuthority & auth)
    : DefaultPConfigPage(app,title,"",auth)
{
  cfg = MCUConfig(section);
  PStringList keys = cfg.GetKeys();
  PString data;
  for(PINDEX i = 0; i < keys.GetSize(); i++)
    data += keys[i]+"="+cfg.GetString(keys[i])+"\n";

  Add(new PHTTPStringField(" ", 1000, data, NULL));
  BuildHTML("");

  PStringStream html_begin, html_end;
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
    : TablePConfigPage(app,title,section,auth)
{
  cfg = MCUConfig(section);

  PString info, infoStyle = "<td rowspan='1' style='background-color:#efe;padding:15px;border-bottom:2px solid white;'>";
  PStringList keys = cfg.GetKeys();
  for(PINDEX i = 0; i < keys.GetSize(); i++)
  {
    info = "<input type=button value='↑' onClick='rowUp(this,0)' style='margin-top:10px;margin-left:10px;margin-right:1px;'><input type=button value='↓' onClick='rowDown(this)' style='margin-top:10px;margin-left:1px;margin-right:10px;'>";
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
      info += infoStyle;
      for (PINDEX j = 0; j < mf.GetOptionCount(); j++)
       if(mf.GetOption(j).GetFMTPName() != "")
         info += mf.GetOption(j).GetFMTPName()+"="+mf.GetOption(j).AsString()+";";
      if(title == "SipSoundCodecs" || title == "SipVideoCodecs")
        info += "<td align='left' style='background-color:#efe;border-bottom:2px solid white;'><input name='fmtp:"+keys[i]+"' size='25' value='"+MCUConfig("CODEC_OPTIONS").GetString(keys[i])+"' type='text' style='margin-top:10px;margin-right:10px;'></td>";
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
  html_page << jsRowUp() << jsRowDown();

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
      MCUConfig("CODEC_OPTIONS").SetString(fmtpArray[i].Tokenise("=")[0], fmtpArray[i].Right(fmtpArray[i].GetSize()-fmtpPos-2));
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
    if(key == "submit" || key == "" || value == "")
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
#if USE_LIBJPEG
  const PString & eb = connectInfo.GetEntityBody();
  long l = connectInfo.GetEntityBodyLength();
  if(l<1) return FALSE;
  PINDEX o = eb.Find("image/jpeg");
  if(o != P_MAX_INDEX)
  {
    o += 11;
  }
  else
  {
    o = eb.Find("image/pjpeg");
    if(o == P_MAX_INDEX) return FALSE;
    o += 12;
  }

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
  if(cl > 524288) return FALSE; // too big

  FILE *f;
  size_t written=0;
  f=fopen(PString(SYS_RESOURCE_DIR) + PATH_SEPARATOR + "logo.jpeg","wb"); 
  if(f) written=fwrite((const void*)(((const char*)eb)+o), 1, (size_t)o2-o, f);
  fclose(f);

  PTRACE(1,"HTML\tlogo.jpeg " << written << " byte(s) re-written");

  { PStringStream message; PTime now; message
      << "HTTP/1.1 302 Found\r\n"
      << "Location: /welcome.html\r\n"
      << "\r\n";  //that's the last time we need to type \r\n instead of just \n
    server.Write((const char*)message,message.GetLength());
  }
  server.flush();
  OpenMCU::Current().RemovePreMediaFrame();
#endif
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
#  ifdef GIT_REVISION
#    define _QUOTE_MACRO_VALUE1(x) #x
#    define _QUOTE_MACRO_VALUE(x) _QUOTE_MACRO_VALUE1(x)
        << "OpenMCU REVISION " << _QUOTE_MACRO_VALUE(GIT_REVISION) << "\n"
#    undef _QUOTE_MACRO_VALUE
#    undef _QUOTE_MACRO_VALUE1
#  endif

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

#if USE_LIBJPEG
  shtml << "<br><form method=\"post\" enctype=\"multipart/form-data\"><h1>Prefatory Frame</h1>JPEG, max 500K:<br><img src=\"logo.jpeg\"><br>Change: <input name=\"image\" type=\"file\"><input type=\"submit\"></form>";
#endif

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

  html << "<p>"
    << "<form method=\"POST\" class=\"well form-inline\">"
    << "<input type=\"text\" class=\"input-small\" placeholder=\"" << app.GetDefaultRoomName() << "\" name=\"room\" value=\"" << app.GetDefaultRoomName() << "\"> "
    << "<input type=\"text\" class=\"input-large\" placeholder=\"Address\" name=\"address\"><script language='javascript'><!--\ndocument.forms[0].address.focus(); //--></script>"
    << "&nbsp;&nbsp;<input type=\"submit\" class=\"btn\" value=\"Invite\">"
    << "</form>";

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
  html_invite << "<p>"
    << "<form method=\"POST\" class=\"well form-inline\">"
    << "<input type=\"text\" class=\"input-small\" placeholder=\"" << room << "\" name=\"room\" value=\"" << room << "\"> "
    << "<input type=\"text\" class=\"input-large\" placeholder=\"Address\" name=\"address\"><script language='javascript'><!--\ndocument.forms[0].address.focus(); //--></script>"
    << "&nbsp;&nbsp;<input type=\"submit\" class=\"btn\" value=\"Invite\">"
    << "</form>";

  if (room.IsEmpty() || address.IsEmpty()) {
    BeginPage(html,"Invite failed","window.l_invite_f","window.l_info_invite_f");
    html << html_invite;
    EndPage(html,OpenMCU::Current().GetHtmlCopyright()); msg = html;
    return TRUE;
  }

  OpenMCUH323EndPoint & ep = app.GetEndpoint();
  BOOL created = ep.OutgoingConferenceRequest(room);

  if (!created) {
    BeginPage(html,"Invite failed","window.l_invite_f","window.l_info_invite_f");
    html << html_invite;
    EndPage(html,OpenMCU::Current().GetHtmlCopyright()); msg = html;
    return TRUE;
  }

  if(address.Find("sip:") == 0) {
    while(OpenMCU::Current().sipendpoint->sipCallData != "") continue;
    OpenMCU::Current().sipendpoint->sipCallData = room+","+address;
  } else {
    PString h323Token;
    PString * userData = new PString(room);
    if (ep.MakeCall(address, h323Token, userData) == NULL) {
      BeginPage(html,"Invite failed","window.l_invite_f","window.l_info_invite_f");
      html << html_invite;
      EndPage(html,OpenMCU::Current().GetHtmlCopyright()); msg = html;
      ep.GetConferenceManager().RemoveConference(room);
      return TRUE;
    }
  }

  BeginPage(html,"Invite succeeded","window.l_invite_s","window.l_info_invite_s");
  html << html_invite;
  EndPage(html,OpenMCU::Current().GetHtmlCopyright()); msg = html;

  return TRUE;
}

///////////////////////////////////////////////////////////////

#if USE_LIBJPEG

MCUVideoMixer* jpegMixer;

void jpeg_init_destination(j_compress_ptr cinfo){
  if(jpegMixer->myjpeg.GetSize()<32768)jpegMixer->myjpeg.SetSize(32768);
  cinfo->dest->next_output_byte=&jpegMixer->myjpeg[0];
  cinfo->dest->free_in_buffer=jpegMixer->myjpeg.GetSize();
}

boolean jpeg_empty_output_buffer(j_compress_ptr cinfo){
  PINDEX oldsize=jpegMixer->myjpeg.GetSize();
  jpegMixer->myjpeg.SetSize(oldsize+16384);
  cinfo->dest->next_output_byte = &jpegMixer->myjpeg[oldsize];
  cinfo->dest->free_in_buffer = jpegMixer->myjpeg.GetSize() - oldsize;
  return true;
}

void jpeg_term_destination(j_compress_ptr cinfo){
  jpegMixer->jpegSize=jpegMixer->myjpeg.GetSize() - cinfo->dest->free_in_buffer;
  jpegMixer->jpegTime=(long)time(0);
}

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

  unsigned requestedMixer=0;
  if(data.Contains("mixer")) requestedMixer=(unsigned)data("mixer").AsInteger();

  const unsigned long t1=(unsigned long)time(0);

//  PWaitAndSignal m(mutex); // no more required: the following mutex will do the same:
  app.GetEndpoint().GetConferenceManager().GetConferenceListMutex().Wait(); // fix it - browse read cause access_v on serv. stop

  ConferenceListType & conferenceList = app.GetEndpoint().GetConferenceManager().GetConferenceList();
  for(ConferenceListType::iterator r = conferenceList.begin(); r != conferenceList.end(); ++r)
  {
    Conference & conference = *(r->second);
    if(conference.GetNumber()==room)
    {
      if(conference.videoMixerList==NULL){ app.GetEndpoint().GetConferenceManager().GetConferenceListMutex().Signal(); return FALSE; }
      jpegMixer=conference.VMLFind(requestedMixer);
      if(jpegMixer==NULL) { app.GetEndpoint().GetConferenceManager().GetConferenceListMutex().Signal(); return FALSE; }
      if(t1-(jpegMixer->jpegTime)>1)
      {
        if(width<1||height<1||width>2048||height>2048)
        { width=OpenMCU::vmcfg.vmconf[jpegMixer->GetPositionSet()].splitcfg.mockup_width;
          height=OpenMCU::vmcfg.vmconf[jpegMixer->GetPositionSet()].splitcfg.mockup_height;
        }
        struct jpeg_compress_struct cinfo; struct jpeg_error_mgr jerr;
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

        ((MCUSimpleVideoMixer*)jpegMixer)->ReadMixedFrame((void*)videoData,width,height,amount);
        PColourConverter * converter = PColourConverter::Create("YUV420P", "RGB24", width, height);
        converter->SetDstFrameSize(width, height);
        unsigned char * bitmap = new unsigned char[width*height*3];
        converter->Convert(videoData,bitmap);
        delete converter;
        delete videoData;

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
        delete bitmap; delete cinfo.dest; cinfo.dest=NULL;
        jpeg_destroy_compress(&cinfo);
        jpegMixer->jpegTime=t1;
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

      app.GetEndpoint().GetConferenceManager().GetConferenceListMutex().Signal();
      server.flush();
      return TRUE;
    }
  }
  app.GetEndpoint().GetConferenceManager().GetConferenceListMutex().Signal();
  return FALSE;
}
#endif //#if USE_LIBJPEG

///////////////////////////////////////////////////////////////

InteractiveHTTP::InteractiveHTTP(OpenMCU & _app, PHTTPAuthority & auth)
  : PServiceHTTPString("Comm", "", "text/html; charset=utf-8", auth),
    app(_app)
{
}

BOOL InteractiveHTTP::OnGET (PHTTPServer & server, const PURL &url, const PMIMEInfo & info, const PHTTPConnectionInfo & connectInfo)
{
  PHTTPRequest * req = CreateRequest(url, info, connectInfo.GetMultipartFormInfo(), server); // check authorization
  if(!CheckAuthority(server, *req, connectInfo)) {delete req; return FALSE;}
  delete req;

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

  message="<html><body style='font-size:9px;font-family:Verdana,Arial;padding:0px;margin:1px;color:#000'><script>p=parent</script>\n";
  message << OpenMCU::Current().HttpStartEventReading(idx,room);

//PTRACE(1,"!!!!!\tsha1('123')=" << PMessageDigestSHA1::Encode("123")); // sha1 works!! I'll try with websocket in future

  if(room!="")
  {
    OpenMCU::Current().GetEndpoint().GetConferenceManager().GetConferenceListMutex().Wait();
    ConferenceListType & conferenceList = OpenMCU::Current().GetEndpoint().GetConferenceManager().GetConferenceList();
    ConferenceListType::iterator r;
    for (r = conferenceList.begin(); r != conferenceList.end(); ++r) if(r->second->GetNumber() == room) break;
    if(r != conferenceList.end() )
    {
      Conference & conference = *(r->second);
      message << "<script>p.splitdata=Array(";
      for (unsigned i=0;i<OpenMCU::vmcfg.vmconfs;i++)
      {
        PString split=OpenMCU::vmcfg.vmconf[i].splitcfg.Id;
        split.Replace("\"","\\x22",TRUE,0);
        if(i!=0) message << ",";
        message << "\"" << split << "\"";
      }
      message << ");\n"
        << "p." << OpenMCU::Current().GetEndpoint().GetMemberListOptsJavascript(conference) << "\n"
        << "p." << OpenMCU::Current().GetEndpoint().GetConferenceOptsJavascript(conference) << "\n"
        << "p.tl=Array" << conference.GetTemplateList() << "\n"
        << "p.seltpl=\"" << conference.GetSelectedTemplateName() << "\"\n"
        << "p.build_page();\n"
        << "</script>\n";
    }
    else
    { // no (no more) room -- redirect to /
      OpenMCU::Current().GetEndpoint().GetConferenceManager().GetConferenceListMutex().Signal();
      message << "<script>top.location.href='/';</script>\n";
      server.Write((const char*)message,message.GetLength());
      server.flush();
      return FALSE;
    }
    OpenMCU::Current().GetEndpoint().GetConferenceManager().GetConferenceListMutex().Signal();
  }

  while(server.Write((const char*)message,message.GetLength())) {
    server.flush();
    int count=0;
    message = OpenMCU::Current().HttpGetEvents(idx,room);
    while (message.GetLength()==0 && count < 20){
      count++;
      PThread::Sleep(100);
      message = OpenMCU::Current().HttpGetEvents(idx,room);
    }
    if(message.Find("<script>")==P_MAX_INDEX) message << "<script>p.alive()</script>\n";
  }
  return FALSE;
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
  { PHTTPRequest * req = CreateRequest(url, info, connectInfo.GetMultipartFormInfo(), server); // check authorization
    if(!CheckAuthority(server, *req, connectInfo)) {delete req; return FALSE;}
    delete req;
  }

  PStringToString data;
  { PString request=url.AsString(); PINDEX q;
    if((q=request.Find("?"))!=P_MAX_INDEX) { request=request.Mid(q+1,P_MAX_INDEX); PURL::SplitQueryVars(request,data); }
  }

  if(data.Contains("action"))
  {
    PString action=data("action");
    PString room=data("room");
    if(action == "create" && (!room.IsEmpty()))
    { ConferenceManager & cm = OpenMCU::Current().GetEndpoint().GetConferenceManager();
      cm.MakeAndLockConference(room);
      cm.UnlockConference();
    }
    else if(action == "delete" && (!room.IsEmpty()))
    { ConferenceManager & cm = OpenMCU::Current().GetEndpoint().GetConferenceManager();
      if(cm.HasConference(room))
      { Conference * conference = cm.MakeAndLockConference(room); // find & get locked
        if(conference != NULL)
        { cm.RemoveConference(conference->GetID());
          cm.UnlockConference();
        }
      }
    }
    else if(action == "startRecorder" && (!room.IsEmpty()))
    { ConferenceManager & cm = OpenMCU::Current().GetEndpoint().GetConferenceManager();
      if(cm.HasConference(room))
      { Conference * conference = cm.MakeAndLockConference(room); // find & get locked
        if(conference != NULL)
        { if(conference->externalRecorder == NULL)
          { conference->externalRecorder = new ExternalVideoRecorderThread(room);
            PThread::Sleep(500);
            if(conference->externalRecorder->running)
            { OpenMCU::Current().HttpWriteEventRoom("Video recording started",room);
              OpenMCU::Current().HttpWriteCmdRoom(OpenMCU::Current().GetEndpoint().GetConferenceOptsJavascript(*conference),room);
              OpenMCU::Current().HttpWriteCmdRoom("build_page()",room);
            } else conference->externalRecorder = NULL;
          }
          cm.UnlockConference();
        }
      }
    }
    else if(action == "stopRecorder" && (!room.IsEmpty()))
    { ConferenceManager & cm = OpenMCU::Current().GetEndpoint().GetConferenceManager();
      if(cm.HasConference(room))
      { Conference * conference = cm.MakeAndLockConference(room); // find & get locked
        if(conference != NULL)
        { if(conference->externalRecorder != NULL)
          { conference->externalRecorder->running=FALSE;
            PThread::Sleep(1000);
            conference->externalRecorder = NULL;
            OpenMCU::Current().HttpWriteEventRoom("Video recording stopped",room);
            OpenMCU::Current().HttpWriteCmdRoom(OpenMCU::Current().GetEndpoint().GetConferenceOptsJavascript(*conference),room);
            OpenMCU::Current().HttpWriteCmdRoom("build_page()",room);
          }
          cm.UnlockConference();
        }
      }
    }
  }

  OpenMCUH323EndPoint & ep=OpenMCU::Current().GetEndpoint();

  PStringStream html;
  BeginPage(html,"Rooms","window.l_rooms","window.l_info_rooms");

  if(data.Contains("action")) html << "<script language='javascript'>location.href='Select';</script>";

  PString nextRoom;
  { ConferenceManager & cm = ep.GetConferenceManager();
    ConferenceListType::const_iterator r;
    PWaitAndSignal m(cm.GetConferenceListMutex());
    for(r = cm.GetConferenceList().begin(); r != cm.GetConferenceList().end(); ++r)
    { PString room0 = r->second->GetNumber().Trim(); PINDEX lastCharPos=room0.GetLength()-1;
      if(room0.IsEmpty()) continue;
#     if ENABLE_ECHO_MIXER
        if(room0.Left(4) *= "echo") continue;
#     endif
#     if ENABLE_TEST_ROOMS
        if(room0.Left(8) == "testroom") continue;
#     endif
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
      { roomStart++;
        PString testName = roomText + PString(roomStart) + roomText2;
        for (r = cm.GetConferenceList().begin(); r != cm.GetConferenceList().end(); ++r) if(r->second->GetNumber()==testName) break;
        if(r == cm.GetConferenceList().end()) { nextRoom = testName; break; }
      }
      break;
    }
    if(nextRoom.IsEmpty()) nextRoom = OpenMCU::Current().GetDefaultRoomName();
  }

  html
    << "<form method=\"post\"><input name='room' id='room' type=hidden>"
    << "<table class=\"table table-striped table-bordered table-condensed\">"

    << "<tr>"
    << "<td colspan='6'><input class='btn btn-large' name='newroom' id='newroom' value='" << nextRoom << "' /><input type='button' class='btn btn-large btn-info' id='l_select_create' onclick=\"location.href='?action=create&room='+encodeURIComponent(document.getElementById('newroom').value);\"></td>"
    << "</tr>"

    << "<tr>"
    << "<th><script type=\"text/javascript\">document.write(window.l_select_enter);</script><br></th>"
    << "<th><script type=\"text/javascript\">document.write(window.l_select_moderated);</script><br></th>"
    << "<th><script type=\"text/javascript\">document.write(window.l_select_visible);</script><br></th>"
    << "<th><script type=\"text/javascript\">document.write(window.l_select_unvisible);</script><br></th>"
    << "<th><script type=\"text/javascript\">document.write(window.l_select_duration);</script><br></th>"
    << "<th><script type=\"text/javascript\">document.write(window.l_select_delete);</script><br></th>"
    << "</tr>"
  ;
  
  { PWaitAndSignal m(ep.GetConferenceManager().GetConferenceListMutex());
    ConferenceListType & conferenceList = ep.GetConferenceManager().GetConferenceList();

    ConferenceListType::iterator r;
    for (r = conferenceList.begin(); r != conferenceList.end(); ++r)
    {
      BOOL controlled = TRUE;
      Conference & conference = *(r->second);
      PString roomNumber = conference.GetNumber();
#if ENABLE_TEST_ROOMS
      controlled &= (!((roomNumber.Left(8)=="testroom") && (roomNumber.GetLength()>8))) ;
#endif
#if ENABLE_ECHO_MIXER
      controlled &= (!(roomNumber.Left(4)*="echo"));
#endif
      BOOL moderated=FALSE; PString charModerated = "-";
      if(controlled) { charModerated = conference.IsModerated(); moderated=(charModerated=="+"); }
      PINDEX   visibleMemberCount = conference.GetVisibleMemberCount();
      PINDEX unvisibleMemberCount = conference.GetMemberCount() - visibleMemberCount;

      PString roomButton = "<span class=\"btn btn-large btn-";
      if(moderated) roomButton+="success";
      else if(controlled) roomButton+="primary";
      else roomButton+="inverse";
      roomButton += "\"";
      if(controlled) roomButton+=" onclick='document.getElementById(\"room\").value=\""
        + roomNumber + "\";document.forms[0].submit();'";
      roomButton += ">" + roomNumber + "</span>";

      PStringStream recordButton; if(controlled)
      { BOOL recState = conference.externalRecorder!=NULL; recordButton
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
        << "<td>" << roomButton << "&nbsp" << recordButton << "</td>"
        << "<td>" << moderated                             << "</td>"
        << "<td>" << visibleMemberCount                    << "</td>"
        << "<td>" << unvisibleMemberCount                  << "</td>"
        << "<td>" << (PTime() - conference.GetStartTime()) << "</td>"
        << "<td><span class=\"btn btn-large btn-danger\" onclick=\"if(confirm('Вы уверены? Are you sure?')){location.href='?action=delete&room=" << PURL::TranslateString(roomNumber,PURL::QueryTranslation) << "';}\">X</span></td>"
        << "</tr>";
    }
  }

  html << "</table></form>";

  EndPage(html,OpenMCU::Current().GetHtmlCopyright());
  { PStringStream message; PTime now; message
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
  if(OpenMCU::Current().GetForceScreenSplit())
  {
    msg << OpenMCU::Current().GetEndpoint().SetRoomParams(data);
  }
  else msg << ErrorPage(request.localAddr.AsString(),request.localPort,423,"Locked","Room Control feature is locked","To unlock the page: click &laquo;<a href='/Parameters'>Parameters</a>&raquo;, check &laquo;Force split screen video and enable Room Control feature&raquo; and accept.<br/><br/>");
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
      << wtd << " will deleted"
      << "<center><a style='color:red' href='/Records?deleteRecordConfirmed=" << wtd << "'>[OK]</a> :: <a href='/Records'>Cancel</a></center>"
      << "</div>";
  }
  else if(data.Contains("deleteRecordConfirmed"))
  {
    wtd=data("deleteRecordConfirmed");
    if(wtd.Find('/')==P_MAX_INDEX) if(wtd.Find('\\')==P_MAX_INDEX)
    {
      unlink(OpenMCU::Current().vr_ffmpegDir + PATH_SEPARATOR + wtd);
      shtml << "<div style='border:1px solid green'>"
      << wtd << " deleted"
      << "</div>";
    }
  }

// directory d is now opened
  PStringArray fileList;

#ifdef _WIN32
  do if(strcmp(".", d.cFileName) && strcmp("..", d.cFileName))
  {
    FILE *f=fopen(dir + '\\' + d.cFileName,"rb");
    if(f!=NULL)
    {
      fseek(f, 0, SEEK_END);
      PStringStream r; r << d.cFileName << "," << ftell(f);
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
      PStringStream r; r << e->d_name << "," << ftell(f);
      fileList.AppendString(r);
      fclose(f);
    }
  }
  closedir(d);
#endif

  PINDEX sortMode=0;
  if(data.Contains("sort")) {sortMode = data("sort").AsInteger(); if(sortMode<0 || sortMode>7) sortMode=0;}

  PString freeSpace;
#ifdef _WIN32
#else
  FILE* pfs = popen(PString("df -h ")+dir, "r");
  if (pfs)
  {
    PString fs0;
    char buffer[128];
    while(!feof(pfs)) if(fgets(buffer, 128, pfs) != NULL) fs0+=buffer;
    pclose(pfs);
    PINDEX slashPos=fs0.Find('/', 20);
    if(slashPos!=P_MAX_INDEX)
    {
      BOOL space=FALSE; PINDEX sc=0;
      for(PINDEX i=slashPos;i<fs0.GetLength();i++)
      {
        char c = fs0[i];
        BOOL csp = c<=32;
        if(csp && (!space)) { space=TRUE; sc++;}
        space &= csp;
        if(sc==3 && (!space))
        {
          PINDEX j=i;
          while((j<fs0.GetLength()) && ((c=fs0[j]) > 32)) {freeSpace += c; j++;}
          freeSpace += " free.";
          break;
        }
      }
    }
  }
#endif

  if(fileList.GetSize()==0) shtml << "The direcory does not contain records at the moment. " << freeSpace; else
  {
    shtml << freeSpace << "<table style='border:2px solid #82b8e3'><tr>"
      << "<th class='h1' style='color:#afc'>N</th>"
      << "<th class='h1'><a style='color:#fff' href='/Records?sort=" << ((sortMode!=0)?'0':'1') << "'>Date/Time</a></th>"
      << "<th class='h1'><a style='color:#fff' href='/Records?sort=" << ((sortMode!=2)?'2':'3') << "'>Room</a></th>"
      << "<th class='h1'><a style='color:#fff' href='/Records?sort=" << ((sortMode!=4)?'4':'5') << "'>Resolution</th>"
      << "<th class='h1'><a style='color:#fff' href='/Records?sort=" << ((sortMode!=6)?'6':'7') << "'>File Size</th>"
      << "<th class='h1' style='color:#afc'>Delete</th>"
      << "</tr>";

    for(PINDEX i=0;i<fileList.GetSize()-1; i++)
    { PString s=fileList[i]; PINDEX pos1=s.Find("__"); if(pos1!=P_MAX_INDEX)
      { PString roomName1 = s.Left(pos1); s=s.Mid(pos1+2,P_MAX_INDEX);
        pos1=s.Find("__"); if(pos1!=P_MAX_INDEX)
        { PString dateTime1 = s.Left(pos1); s=s.Mid(pos1+2,P_MAX_INDEX);
          PString videoResolution1; pos1=s.Find('.'); PINDEX pos2=s.Find(',');
          if(pos1!=P_MAX_INDEX) videoResolution1=s.Left(pos1); else videoResolution1=s.Left(pos2);
          PINDEX fileSize1 = s.Mid(pos2+1,P_MAX_INDEX).AsInteger();
          for(PINDEX j=i+1;j<fileList.GetSize(); j++)
          { PString s=fileList[j]; pos1=s.Find("__"); if(pos1!=P_MAX_INDEX)
            { PString roomName2 = s.Left(pos1); s=s.Mid(pos1+2,P_MAX_INDEX);
              pos1=s.Find("__"); if(pos1!=P_MAX_INDEX)
              { PString dateTime2 = s.Left(pos1); s=s.Mid(pos1+2,P_MAX_INDEX);
                PString videoResolution2; pos1=s.Find('.'); PINDEX pos2=s.Find(',');
                if(pos1!=P_MAX_INDEX) videoResolution2=s.Left(pos1); else videoResolution2=s.Left(pos2);
                PINDEX fileSize2 = s.Mid(pos2+1,P_MAX_INDEX).AsInteger();

                if     ((!sortMode) && (!(dateTime2 >= dateTime1))){}
                else if((sortMode==1) && (dateTime2 >= dateTime1)) {}
                else if((sortMode==2) && (roomName2 >= roomName1)) {}
                else if((sortMode==3) && (roomName2 <= roomName1)) {}
                else if((sortMode==4) && (videoResolution2 <= videoResolution1)) {}
                else if((sortMode==5) && (videoResolution2 >= videoResolution1)) {}
                else if((sortMode==6) && (fileSize2 <= fileSize1)) {}
                else if((sortMode==7) && (fileSize2 >= fileSize1)) {}
                else { PString s=fileList[i]; fileList[i]=fileList[j]; fileList[j]=s; } // or just swap PString pointers?

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
            << "<td class='h0'><a href='/Records?getfile=" << fileName << "' download>" << (i+1) << "</a></td>"
            << "<td class='h0'>" << dateTime.Mid(7,2) << '.' << dateTime.Mid(5,2) << '.' << dateTime.Mid(0,4) << ' ' << dateTime.Mid(10,2) << ':' << dateTime.Mid(12,2) << "</td>"
            << "<td class='h0'>" << roomName << "</td>"
            << "<td class='h0'>" << videoResolution << "</td>"
            << "<td class='h0' style='text-align:right'><a href='/Records?getfile=" << fileName << "' download>" << fileSize << "</a></td>"
            << "<td class='h0'><a href='/Records?deleteRecord=" << fileName << "' style='color:red;text-decoration:none'>x</a></td>"
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
