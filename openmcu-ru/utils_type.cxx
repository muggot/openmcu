
#include "precompile.h"
#include "mcu.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

MCUURL::MCUURL(const PString & str)
{
  Parse(str);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL MCUURL::Parse(const PString & str)
{
  PINDEX delim1 = str.FindLast("[");
  PINDEX delim2 = str.FindLast("]");
  PINDEX delim3 = str.FindLast("<");
  PINDEX delim4 = str.FindLast(">");
  PINDEX pos;

  if(delim1 != P_MAX_INDEX && delim2 != P_MAX_INDEX)
  {
    display_name = str.Left(delim1-1);
    display_name.Replace("\"","",TRUE,0);
    url_party = str.Mid(delim1+1, delim2-delim1-1);
  }
  else if(delim3 != P_MAX_INDEX && delim4 != P_MAX_INDEX)
  {
    display_name = str.Left(delim3-1);
    display_name.Replace("\"","",TRUE,0);
    url_party = str.Mid(delim3+1, delim4-delim3-1);
  }
  else
    url_party = str;

  pos = str.Find(" ##");
  if(pos != P_MAX_INDEX)
  {
    memberNameSuffix = str.Mid(pos, str.GetLength()-1);
  }

  if(url_party.Left(4) == "sip:") url_scheme = "sip";
  else if(url_party.Left(5) == "sips:") url_scheme = "sip";
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
    return TRUE;
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

  PURL::Parse((const char *)url_party, url_scheme);
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

  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
