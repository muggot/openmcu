
#include "precompile.h"
#include "mcu.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

PString GetPluginName(const PString & formatName)
{
  PINDEX sep = formatName.Find("-");
  if(sep == P_MAX_INDEX)
    sep = formatName.Find("_");
  if(sep == P_MAX_INDEX)
    sep = formatName.Find("{");
  if(sep != P_MAX_INDEX)
    return formatName.Left(sep);
  return formatName;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

PString GetSectionParam(PString section_prefix, PString param, PString addr, bool asterisk)
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
  if(value == "" && asterisk == true)
    value = MCUConfig(section_prefix+"*").GetString(param);

  return value;
}

void SetSectionParam(PString section_prefix, PString param, PString addr, PString value)
{
  PString user, host;
  PString httpResource;
  if(section_prefix == "H323 Endpoint ")
  {
    MCUURL url(addr);
    user = url.GetUserName();
    host = url.GetHostName();
    httpResource = "H323EndpointsParameters";
  }
  else if(section_prefix == "SIP Endpoint ")
  {
    MCUURL url(addr);
    user = url.GetUserName();
    host = url.GetHostName();
    httpResource = "SipEndpointsParameters";
  }
  else if(section_prefix == "RTSP Endpoint ")
  {
    user = addr;
    user.Replace("rtsp://","",TRUE,0);
    //httpResource = "RtspEndpoints";
  }

  if(MCUConfig::HasSection(section_prefix+addr))
    MCUConfig(section_prefix+addr).SetString(param, value);
  else if(MCUConfig::HasSection(section_prefix+user))
    MCUConfig(section_prefix+user).SetString(param, value);
  else if(MCUConfig::HasSection(section_prefix+host))
    MCUConfig(section_prefix+host).SetString(param, value);
  else
    MCUConfig(section_prefix+user).SetString(param, value);

  // refresh the settings page
  OpenMCU::Current().CreateHTTPResource(httpResource);
}

PString GetSectionParamFromUrl(PString param, PString addr, bool asterisk)
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

  PString value = GetSectionParam(section_prefix, param, addr, asterisk);
  PTRACE(1, "Get parameter (" << addr << ") \"" << param << "\" = " << value);
  return value;
}

void SaveParameterByURL(PString param, PString addr, PString value)
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
      return;
  }

  SetSectionParam(section_prefix, param, addr, value);
}

PString GetSectionParamFromUrl(PString param, PString addr, PString defaultValue, bool asterisk)
{
  PString value = GetSectionParamFromUrl(param, addr, asterisk);
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

////////////////////////////////////////////////////////////////////////////////////////////////////

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
  if(room.Find(MCU_INTERNAL_CALL_PREFIX) == 0)
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

PString convert_cp1251_to_utf8(PString str) // cp1251 -> utf8 translation based on http://www.linux.org.ru/forum/development/3968525
{
  PStringStream utf8;
  for(PINDEX i = 0; i < str.GetLength(); i++)
  {
    unsigned int charcode=(BYTE)str[i];
    if(charcode&128)
    {
      if((charcode=utf8_cyr_table[charcode&127]))
      {
        utf8 << (char)charcode << (char)(charcode >> 8);
        if(charcode >>= 16) utf8 << (char)charcode;
      }
    }
    else
      utf8 << (char)charcode;
  }
  if(str != utf8)
  {
    PTRACE(3, "Converted cp1251->utf8: " << str << " -> " << utf8);
    return utf8;
  }
  return str;
};

PString convert_utf8_to_cp1251(PString utfstr)
{
  PStringStream cpstr;
  PINDEX i = 0, l = utfstr.GetLength();
  while (i < l)
  {
    unsigned int code = (BYTE)utfstr[i];
    if((code & 0x0F8) == 0x0F0) { i += 4; continue; }
    else if((code & 0x0E0) == 0x0C0) // 2 bytes
    {
      if(i+1 >= l) break;
      code += ((unsigned long)((BYTE)utfstr[i+1]) << 8);
      for(unsigned int j=0;j<128;j++) if(utf8_cyr_table[j] == code) { cpstr << (char)(j|0x80); break; }
      i+=2; continue;
    }
    else if((code & 0x0F0) == 0x0E0) // 3 bytes
    {
      if(i+2 >= l) break;
      code += ((unsigned long)((BYTE)utfstr[i+1]) << 8) + ((unsigned long)((BYTE)utfstr[i+2]) << 16);
      for(unsigned int j=0;j<128;j++) if(utf8_cyr_table[j] == code) { cpstr << (char)(j|0x80); break; }
      i+=3; continue;
    }

    cpstr << (char)code;
    i++;
  }

  PTRACE_IF(3, (cpstr != utfstr), "Converted utf8->cp1251: " << utfstr << " -> " << cpstr);
  return cpstr;
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

char * PStringToChar(const PString & str)
{
  if(str.GetLength() == 0)
    return NULL;
  char *data = (char *)malloc((str.GetLength()+1) * sizeof(char));
  strcpy(data, str);
  return data;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

PString PWORDArrayToPString(const PWORDArray & ar)
{
  PString str;
  for(int i = 0; i < ar.GetSize(); ++i)
    str += ar[i];
  return str;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL CheckCapability(const PString & formatName)
{
  if(H323CapabilityFactory::IsSingleton(formatName) || H323CapabilityFactory::IsSingleton(formatName+"{sw}"))
    return TRUE;
  return FALSE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL SkipCapability(const PString & formatName, MCUConnectionTypes connectionType)
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
  if(connectionType == CONNECTION_TYPE_H323)
  {
    if(formatName.Find("MP4V-ES") == 0)
      return TRUE;
  }
  else if(connectionType == CONNECTION_TYPE_RTSP)
  {
    if(formatName.Find("G.711") != 0 && formatName.Find("Speex") != 0 && formatName.Find("OPUS") != 0 && formatName.Find("AC3") != 0 &&
       formatName.Find("H.263p{sw}") != 0 && formatName.Find("H.264{sw}") != 0 && formatName.Find("MP4V-ES{sw}") != 0
      )
      return TRUE;
  }
  return FALSE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
