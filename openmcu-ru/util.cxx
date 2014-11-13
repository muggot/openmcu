
#include <ptlib.h>

#include "config.h"
#include "mcu.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

MCUURL::MCUURL()
{
}

MCUURL::MCUURL(PString str)
{
  PINDEX delim1 = str.FindLast("[");
  PINDEX delim2 = str.FindLast("]");
  PINDEX delim3 = str.FindLast("<sip:");
  PINDEX delim4 = str.FindLast(">");

  if(delim3 != P_MAX_INDEX && delim4 != P_MAX_INDEX)
  {
    display_name = str.Left(delim3-1);
    display_name.Replace("\"","",TRUE,0);
    url_party = str.Mid(delim3+1, delim4-delim3-1);
  }
  else if(delim1 != P_MAX_INDEX && delim2 != P_MAX_INDEX)
  {
    display_name = str.Left(delim1-1);
    url_party = str.Mid(delim1+1, delim2-delim1-1);
  } else {
    url_party = str;
  }

  if(url_party.Left(4) == "sip:") url_scheme = "sip";
  else if(url_party.Left(4) == "sips:") url_scheme = "sip";
  else if(url_party.Left(5) == "h323:") url_scheme = "h323";
  else if(url_party.Left(5) == "rtsp:") url_scheme = "rtsp";
  else if(url_party.Left(5) == "http:") url_scheme = "http";
  else if(url_party.Left(4) == "tcp:") url_scheme = "listener";
  else if(url_party.Left(4) == "udp:") url_scheme = "listener";
  else { url_party = "h323:"+url_party; url_scheme = "h323"; }

  if(url_scheme == "listener")
  {
    transport = url_party.Tokenise(":")[0];
    hostname = url_party.Tokenise(":")[1];
    port = url_party.Tokenise(":")[2].AsInteger();
    return;
  }

  if(url_scheme == "sip" || url_scheme == "h323")
  {
    // добавить "@" для парсинга
    if(url_party.Find("@") == P_MAX_INDEX)
    {
      // если не указан порт
      if(url_party.FindLast(":") == url_scheme.GetLength())
        url_party += "@";
      else
        url_party.Replace(url_scheme+":",url_scheme+":@",TRUE,0);
    }
  }

  Parse((const char *)url_party, url_scheme);
  // parse old H.323 scheme
  if(url_scheme == "h323" && url_party.Left(5) != "h323" && url_party.Find("@") == P_MAX_INDEX &&
     hostname == "" && username != "")
  {
    hostname = username;
    username = "";
  }

  if(url_scheme == "sip")
  {
    transport = "*";
    if(url_party.Find("transport=udp") != P_MAX_INDEX) transport = "udp";
    if(url_party.Find("transport=tcp") != P_MAX_INDEX) transport = "tcp";
    if(url_party.Find("transport=tls") != P_MAX_INDEX) transport = "tls";
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

MCUStringDictionary::MCUStringDictionary(const PString & str)
{
  delim1 = ";";
  delim2 = "=";
  if(str != "")
    Parse(str);
}
MCUStringDictionary::MCUStringDictionary(const PString & str, const PString & _delim1, const PString & _delim2)
{
  delim1 = _delim1;
  delim2 = _delim2;
  Parse(str);
}
void MCUStringDictionary::Parse(const PString & str)
{
  PStringArray array = str.Tokenise(delim1);
  for(PINDEX i = 0; i < array.GetSize(); ++i)
  {
    PString key = array[i].Tokenise(delim2)[0];
    PString value;
    PINDEX pos = array[i].Find(delim2);
    if(pos != P_MAX_INDEX)
      value = array[i].Mid(pos+1);
    Append(key, value);
  }
}
void MCUStringDictionary::Append(PString name, const PString & value)
{
  if(name == "")
    return;
  PINDEX index = keys.GetStringsIndex(name);
  if(index == P_MAX_INDEX)
  {
    keys.AppendString(name);
    values.AppendString(value);
  } else {
    values.SetAt(index, new PString(value));
  }
}
void MCUStringDictionary::Remove(const PString & name)
{
  if(name == "")
    return;
  PINDEX index = keys.GetStringsIndex(name);
  if(index == P_MAX_INDEX)
    return;
  if(index >= values.GetSize())
    return;
  keys.RemoveAt(index);
  values.RemoveAt(index);
}
void MCUStringDictionary::SetValueAt(PINDEX index, const PString & value)
{
  if(index > values.GetSize())
    return;
  values.SetAt(index, new PString(value));
}
PString MCUStringDictionary::GetKeyAt(PINDEX index)
{
  if(index > keys.GetSize())
    return "";
  return keys[index];
}
PString MCUStringDictionary::GetValueAt(PINDEX index)
{
  if(index > values.GetSize())
    return "";
  return values[index];
}
PString MCUStringDictionary::AsString(const PString & _delim1, const PString & _delim2)
{
  PString str;
  for(PINDEX index = 0; index < keys.GetSize(); ++index)
    str += keys[index] + _delim2 + values[index] + _delim1;
  return str;
}
PString MCUStringDictionary::operator()(const PString & key, const char *defvalue) const
{
  PINDEX index = keys.GetStringsIndex(key);
  if(index == P_MAX_INDEX)
    return defvalue;
  return values[index];
}

////////////////////////////////////////////////////////////////////////////////////////////////////

PString GetSectionParam(PString section_prefix, PString param, PString addr)
{
  PString user, host;
  PString value;

  if(section_prefix == "RTSP Endpoint ")
  {
    user = addr;
    user.Replace("rtsp://","",TRUE,0);
  }
  else
  {
    MCUURL url(addr);
    user = url.GetUserName();
    host = url.GetHostName();
  }

  if(value == "")
    value = MCUConfig(section_prefix+addr).GetString(param);
  if(value == "")
    value = MCUConfig(section_prefix+user).GetString(param);
  if(value == "")
    value = MCUConfig(section_prefix+host).GetString(param);
  if(value == "")
    value = MCUConfig(section_prefix+"*").GetString(param);

  return value;
}

PString GetSectionParamFromUrl(PString param, PString addr)
{
  PString section_prefix;
  if(addr.Find("RTSP Server ") == 0)
  {
    section_prefix = "RTSP Server ";
    addr.Replace("RTSP Server ","",TRUE,0);
  } else {
    MCUURL url(addr);
    if(url.GetScheme() == "h323")
      section_prefix = "H323 Endpoint ";
    else if(url.GetScheme() == "sip")
      section_prefix = "SIP Endpoint ";
    else if(url.GetScheme() == "rtsp")
      section_prefix = "RTSP Endpoint ";
    else
      return "";
  }

  PString value = GetSectionParam(section_prefix, param, addr);
  MCUTRACE(1, "Get parameter (" << addr << ") \"" << param << "\" = " << value);
  return value;
}

int GetSectionParamFromUrl(PString param, PString addr, int defaultValue)
{
  PString value = GetSectionParamFromUrl(param, addr);
  if(value == "")
    return defaultValue;

  if(value.ToLower() == "true")
    value = "1";
  else if(value.ToLower() == "false")
    value = "0";

  return value.AsInteger();
}

PString GetSectionParamFromUrl(PString param, PString addr, PString defaultValue)
{
  PString value = GetSectionParamFromUrl(param, addr);
  if(value == "")
    return defaultValue;
  return value;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int GetConferenceParam(PString room, PString param, int defaultValue)
{
  PString value = GetConferenceParam(room, param, "");
  if(value.ToLower() == "enable" || value.ToLower() == "true")
    return 1;
  else if(value.ToLower() == "disable" || value.ToLower() == "false")
    return 0;
  else if(value != "")
    return atoi(value);
  else
    return defaultValue;
}

PString GetConferenceParam(PString room, PString param, PString defaultValue)
{
#if ENABLE_TEST_ROOMS
  if(room.Left(8) == "testroom")
  {
    if(param == ForceSplitVideoKey)
      return "Disable";
    if(param == RoomAutoDeleteEmptyKey)
      return "Enable";
    if(param == RoomAutoRecordStartKey)
      return "Disable";
    if(param == RoomAutoRecordStopKey)
      return "Disable";
    if(param == RoomAllowRecordKey)
      return "Disable";
  }
#endif
#if ENABLE_ECHO_MIXER
  if(room.Left(4) *= "echo")
  {
    if(param == ForceSplitVideoKey)
      return "Disable";
    if(param == RoomAutoDeleteEmptyKey)
      return "Enable";
    if(param == RoomAutoRecordStartKey)
      return "Disable";
    if(param == RoomAutoRecordStopKey)
      return "Disable";
    if(param == RoomAllowRecordKey)
      return "Disable";
  }
#endif
  // internal rooms
  if(room.Left(MCU_INTERNAL_CALL_PREFIX.GetLength()) == MCU_INTERNAL_CALL_PREFIX)
  {
    if(param == ForceSplitVideoKey)
      return "Disable";
    if(param == RoomAutoDeleteEmptyKey)
      return "Enable";
    if(param == RoomAutoRecordStartKey)
      return "Disable";
    if(param == RoomAutoRecordStopKey)
      return "Disable";
    if(param == RoomAllowRecordKey)
      return "Enable";
  }

  PString section;
  PString value;
  PString sectionPrefix = "Conference ";

  value = MCUConfig(sectionPrefix+room).GetString(param);
  if(value == "")
    value = MCUConfig(sectionPrefix+"*").GetString(param);

  if(value == "")
  {
    if(param == ForceSplitVideoKey)
      value = "Enable";
    if(param == RoomAutoDeleteEmptyKey)
      value = "Disable";
    if(param == RoomAutoRecordStartKey)
      value = "Disable";
    if(param == RoomAutoRecordStopKey)
      value = "Disable";
    if(param == RoomAllowRecordKey)
      value = "Enable";
  }
  return value;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

PString convert_cp1251_to_utf8(PString str)
{
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

  PStringStream utf8;
  for(PINDEX i = 0; i < str.GetLength(); i++)
  {
    unsigned int charcode=(BYTE)str[i];
    if(charcode&128)
    {
      if((charcode=table[charcode&127]))
      {
        utf8 << (char)charcode << (char)(charcode >> 8);
        if(charcode >>= 16) utf8 << (char)charcode;
      }
    } else {
      utf8 << (char)charcode;
    }
  }
  if(str != utf8)
  {
    PTRACE(1, "Converted cp1251->utf8: " << str << " -> " << utf8);
    return utf8;
  } else {
    return str;
  }
};

////////////////////////////////////////////////////////////////////////////////////////////////////

PString convert_ucs2_to_utf8(PString str)
{
  PStringStream utf8;
  for(PINDEX i = 0; i < str.GetLength(); i++)
  {
    unsigned int charcode = (BYTE)str[i];
    if(charcode == 0xc3)
    {
      charcode = (BYTE)str[++i];
      if(charcode >= 128 && charcode <= 191)
      {
        if(charcode < 176)
          utf8 << (char)0xd0 << (char)(charcode+16);
        else
          utf8 << (char)0xd1 << (char)(charcode-48);
      }
    }
    else
    {
      utf8 << (char)charcode;
    }
  }
  if(str != utf8)
  {
    PTRACE(1, "Converted ucs2->utf8: " << str << " -> " << utf8);
    return utf8;
  } else {
    return str;
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

char * PStringToChar(PString str)
{
  if(str.GetLength() == 0)
    return NULL;
  char *data = (char *)malloc((str.GetLength()+1) * sizeof(char));
  strcpy(data, str);
  return data;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int PTimedMutexTryLock(PTimedMutex & mutex, const PTimeInterval & timeout, PString info)
{
#ifdef _WIN32
  mutex.Wait();
  return 1;
#else
  if(!mutex.Wait(timeout))
  {
    if(info != "")
      MCUTRACE(0, info);
    return 0;
  }
  return 1;
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////

unsigned GetVideoMacroBlocks(unsigned width, unsigned height)
{
  if(width == 0 || height == 0) return 0;
  return ((width+15)/16) * ((height+15)/16);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL GetParamsH263(PString & mpiname, unsigned & width, unsigned & height)
{
  for(int i = 0; h263_resolutions[i].width != 0; ++i)
  {
    // from mpiname
    if(mpiname != "" && mpiname != h263_resolutions[i].mpiname)
      continue;
    // from width && height
    if(width && height && width != h263_resolutions[i].width && height != h263_resolutions[i].height)
      continue;
    mpiname = h263_resolutions[i].mpiname;
    width = h263_resolutions[i].width;
    height = h263_resolutions[i].height;
    return TRUE;
  }
  return FALSE;
}

BOOL GetParamsMpeg4(unsigned & profile_level, unsigned & profile, unsigned & level, unsigned & max_fs)
{
  unsigned width = 0, height = 0;
  return GetParamsMpeg4(profile_level, profile, level, max_fs, width, height);
}
BOOL GetParamsMpeg4(unsigned & profile_level, unsigned & profile, unsigned & level, unsigned & max_fs, unsigned & width, unsigned & height)
{
  for(int i = 0; mpeg4_profile_levels[i].level != 0; ++i)
  {
    // from profile_level
    if(profile_level && profile_level != mpeg4_profile_levels[i].profile_level && mpeg4_profile_levels[i+1].level != 0)
      continue;
    // from max_fs
    if(max_fs && max_fs > mpeg4_profile_levels[i].max_fs && mpeg4_profile_levels[i+1].level != 0)
      continue;
    if(!profile) profile = mpeg4_profile_levels[i].profile;
    if(!level) level = mpeg4_profile_levels[i].level;
    if(!max_fs) max_fs = mpeg4_profile_levels[i].max_fs;
    if(!width) width = mpeg4_profile_levels[i].width;
    if(!height) height = mpeg4_profile_levels[i].height;
    return TRUE;
  }
  return FALSE;
}

BOOL GetParamsH264(unsigned & level, unsigned & level_h241, unsigned & max_fs)
{
  unsigned max_mbps = 0, max_br = 0;
  return GetParamsH264(level, level_h241, max_fs, max_mbps, max_br);
}
BOOL GetParamsH264(unsigned & level, unsigned & level_h241, unsigned & max_fs, unsigned & max_mbps, unsigned & max_br)
{
  unsigned width = 0, height = 0;
  return GetParamsH264(level, level_h241, max_fs, max_mbps, max_br, width, height);
}
BOOL GetParamsH264(unsigned & level, unsigned & level_h241, unsigned & max_fs, unsigned & max_mbps, unsigned & max_br, unsigned & width, unsigned & height)
{
  for(int i = 0; h264_profile_levels[i].level != 0; ++i)
  {
    // from level
    if(level && level > h264_profile_levels[i].level && h264_profile_levels[i+1].level != 0)
      continue;
    // from level h241
    if(level_h241 && level_h241 > h264_profile_levels[i].level_h241 && h264_profile_levels[i+1].level != 0)
      continue;
    // from max_fs
    if(max_fs && max_fs > h264_profile_levels[i].max_fs && h264_profile_levels[i+1].level != 0)
      continue;
    if(!level) level = h264_profile_levels[i].level;
    if(!level_h241) level_h241 = h264_profile_levels[i].level_h241;
    if(!max_fs) max_fs = h264_profile_levels[i].max_fs;
    if(!max_mbps) max_mbps = h264_profile_levels[i].max_mbps;
    if(!max_br) max_br = h264_profile_levels[i].max_br;
    break;
  }
  for(int i = 0; h264_resolutions[i].macroblocks != 0; ++i)
  {
    if(max_fs < h264_resolutions[i].macroblocks)
      continue;
    width = h264_resolutions[i].width;
    height = h264_resolutions[i].height;
    break;
  }
  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void SetFormatParamsH261(OpalMediaFormat & wf, unsigned width, unsigned height)
{
  SetFormatParamsH263(wf, width, height);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void SetFormatParamsH263(OpalMediaFormat & wf, unsigned width, unsigned height)
{
  PString mpiname;
  GetParamsH263(mpiname, width, height);
  if(mpiname != "")
  {
    wf.SetOptionInteger("SQCIF MPI", 0);
    wf.SetOptionInteger("QCIF MPI", 0);
    wf.SetOptionInteger("CIF MPI", 0);
    wf.SetOptionInteger("CIF4 MPI", 0);
    wf.SetOptionInteger("CIF16 MPI", 0);
    wf.SetOptionInteger(mpiname+" MPI", 1);
  }
  wf.SetOptionInteger(OPTION_FRAME_WIDTH, width);
  wf.SetOptionInteger(OPTION_FRAME_HEIGHT, height);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void SetFormatParamsH264(OpalMediaFormat & wf, unsigned width, unsigned height)
{
  unsigned level = 0, level_h241 = 0, max_fs = 0, max_mbps = 0, max_br = 0;
  max_fs = GetVideoMacroBlocks(width, height);
  GetParamsH264(level, level_h241, max_fs, max_mbps, max_br);
  wf.SetOptionInteger("Generic Parameter 42", level_h241);
  wf.SetOptionInteger("Generic Parameter 4", (max_fs/256)+1);
  wf.SetOptionInteger("Generic Parameter 3", max_mbps/500);
  wf.SetOptionInteger("Generic Parameter 6", max_br/25000);
  wf.SetOptionInteger("Custom Resolution", 1);
  wf.SetOptionInteger(OPTION_FRAME_WIDTH, width);
  wf.SetOptionInteger(OPTION_FRAME_HEIGHT, height);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void SetFormatParamsMPEG4(OpalMediaFormat & wf, unsigned width, unsigned height)
{
  unsigned profile_level = 0, profile = 0, level = 0, max_fs = 0;
  max_fs = GetVideoMacroBlocks(width, height);
  GetParamsMpeg4(profile_level, profile, level, max_fs);
  wf.SetOptionInteger("profile", profile);
  wf.SetOptionInteger("level", level);
  wf.SetOptionInteger(OPTION_FRAME_WIDTH, width);
  wf.SetOptionInteger(OPTION_FRAME_HEIGHT, height);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void SetFormatParamsVP8(OpalMediaFormat & wf, unsigned width, unsigned height)
{
  wf.SetOptionInteger("Generic Parameter 1", width);
  wf.SetOptionInteger("Generic Parameter 2", height);
  wf.SetOptionInteger(OPTION_FRAME_WIDTH, width);
  wf.SetOptionInteger(OPTION_FRAME_HEIGHT, height);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void SetFormatParams(OpalMediaFormat & wf, unsigned width, unsigned height)
{
  unsigned frame_rate = 0, bandwidth = 0;
  SetFormatParams(wf, width, height, frame_rate, bandwidth);
}

void SetFormatParams(OpalMediaFormat & wf, unsigned width, unsigned height, unsigned frame_rate, unsigned bandwidth)
{
  if(width != 0 && height != 0)
  {
    if(wf.Find("H.261") == 0)
      SetFormatParamsH261(wf, width, height);
    else if(wf.Find("H.263") == 0)
      SetFormatParamsH263(wf, width, height);
    else if(wf.Find("H.264") == 0)
      SetFormatParamsH264(wf, width, height);
    else if(wf.Find("MP4V-ES") == 0)
      SetFormatParamsMPEG4(wf, width, height);
    else if(wf.Find("VP8") == 0)
      SetFormatParamsVP8(wf, width, height);
  }

  MCUH323EndPoint & ep = OpenMCU::Current().GetEndpoint();

  if(frame_rate == 0) frame_rate = ep.GetVideoFrameRate();
  if(frame_rate < 1) frame_rate = 1;
  if(frame_rate > MCU_MAX_FRAME_RATE) frame_rate = MCU_MAX_FRAME_RATE;
  wf.SetOptionInteger(OPTION_FRAME_RATE, frame_rate);
  wf.SetOptionInteger(OPTION_FRAME_TIME, 90000/frame_rate);

  if(bandwidth == 0) bandwidth = 256;
  if(bandwidth < MCU_MIN_BIT_RATE/1000) bandwidth = MCU_MIN_BIT_RATE/1000;
  if(bandwidth > MCU_MAX_BIT_RATE/1000) bandwidth = MCU_MAX_BIT_RATE/1000;
  wf.SetOptionInteger(OPTION_MAX_BIT_RATE, bandwidth*1000);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL CreateCustomVideoCache(PString requestedRoom, H323Capability *cap)
{
  // creating conference if needed
  MCUH323EndPoint & ep = OpenMCU::Current().GetEndpoint();
  ConferenceManager & manager = ((MCUH323EndPoint &)ep).GetConferenceManager();
  Conference *conf = manager.MakeAndLockConference(requestedRoom);
  manager.UnlockConference();

  // starting new cache thread
  unsigned videoMixerNumber=0;
  new ConferenceCacheMember(conf, cap->GetMediaFormat(), videoMixerNumber);

  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL CheckCapability(const PString & formatName)
{
  if(H323CapabilityFactory::IsSingleton(formatName) || H323CapabilityFactory::IsSingleton(formatName+"{sw}"))
    return TRUE;
  return FALSE;
}

///////////////////////////////////////////////////////////////////////////

BOOL SkipCapability(const PString & formatName, MCUH323Connection::ConnectionTypes connectionType)
{
  if(!CheckCapability(formatName))
    return TRUE;
  if(  (formatName.Find("H.261-") == 0 && CheckCapability("H.261"))
     ||(formatName.Find("H.263-") == 0 && CheckCapability("H.263"))
     ||(formatName.Find("H.263p-") == 0 && CheckCapability("H.263p"))
     ||(formatName.Find("H.264-") == 0 && CheckCapability("H.264"))
     ||(formatName.Find("VP8-") == 0 && CheckCapability("VP8"))
    )
  {
    return TRUE;
  }
  if(connectionType == MCUH323Connection::CONNECTION_TYPE_H323)
  {
    if(formatName.Find("MP4V-ES") == 0)
      return TRUE;
  }
  else if(connectionType == MCUH323Connection::CONNECTION_TYPE_RTSP)
  {
    if(formatName.Find("G.711") != 0 && formatName.Find("Speex") != 0 && formatName.Find("OPUS") != 0 && formatName.Find("AC3") != 0 &&
       formatName.Find("H.263p{sw}") != 0 && formatName.Find("H.264{sw}") != 0 && formatName.Find("MP4V-ES{sw}") != 0
      )
      return TRUE;
  }
  return FALSE;
}

///////////////////////////////////////////////////////////////////////////
