
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
  else { url_party = "h323:"+url_party; url_scheme = "h323"; }

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

int GetEndpointParamFromUrl(PString param, PString addr, int defaultValue)
{
  PString value = GetEndpointParamFromUrl(param, addr);
  if(value == "")
    return defaultValue;
  return value.AsInteger();
}

PString GetEndpointParamFromUrl(PString param, PString addr, PString defaultValue)
{
  PString value = GetEndpointParamFromUrl(param, addr);
  if(value == "")
    return defaultValue;
  return value;
}

PString GetEndpointParamFromUrl(PString param, PString addr)
{
  PString user, host;
  PString sectionPrefix, section;
  PString value;

  MCUURL url(addr);
  user = url.GetUserName();
  host = url.GetHostName();
  if(url.GetScheme() == "h323")
    sectionPrefix = "H323 Endpoint ";
  else if(url.GetScheme() == "sip")
    sectionPrefix = "SIP Endpoint ";
  else
    return "";

  value = MCUConfig(sectionPrefix+user).GetString(param);
  if(value == "")
    value = MCUConfig(sectionPrefix+"*").GetString(param);

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
    if(param == RoomAutoRecordNotEmptyKey)
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
    if(param == RoomAutoRecordNotEmptyKey)
      return "Disable";
    if(param == RoomAllowRecordKey)
      return "Disable";
  }
#endif
  // internal rooms
  PString internalRoomPrefix = OpenMCU::Current().GetRegistrar()->GetInternalRoomPrefix();
  if(internalRoomPrefix != "" && room.Left(internalRoomPrefix.GetLength()) == internalRoomPrefix)
  {
    if(param == ForceSplitVideoKey)
      return "Disable";
    if(param == RoomAutoDeleteEmptyKey)
      return "Enable";
    if(param == RoomAutoRecordNotEmptyKey)
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
    if(param == RoomAutoRecordNotEmptyKey)
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

void MCUTRACE(unsigned level, PString args)
{
  if(args.GetLength() < 2)
    return;
  if(args.Right(1) == "\n")
    args = args.Left(args.GetLength()-1);

  if(level > 0)
  {
    PTRACE(level, args);
  }
  if(level <= 3)
  {
    PTime uptime = PTime() - (OpenMCU::Current().GetStartTime()-PTime(0));
    cout << uptime.AsString("mm:ss.uuu") << ": " << args << "\n";
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
