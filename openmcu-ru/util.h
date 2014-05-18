
#ifndef _MCU_UTIL_H
#define _MCU_UTIL_H

#include "config.h"
#include "mcu.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

PString convert_cp1251_to_utf8(PString str);
PString convert_ucs2_to_utf8(PString str);

PString GetEndpointParamFromUrl(PString param, PString url);
PString GetEndpointParamFromUrl(PString param, PString url, PString defaultValue);
int GetEndpointParamFromUrl(PString param, PString url, int defaultValue);

PString GetConferenceParam(PString room, PString param, PString defaultValue);
int GetConferenceParam(PString room, PString param, int defaultValue);

void MCUTRACE(unsigned level, PString args);

////////////////////////////////////////////////////////////////////////////////////////////////////

class MCUConfig: public PConfig
{
 public:
   MCUConfig()
    : PConfig() { };
   MCUConfig(const PString & section)
    : PConfig(CONFIG_PATH, section) { };

   PStringList GetSectionsPrefix(PString prefix) const
   {
     PStringList sect = MCUConfig("Parameters").GetSections();
     for(PINDEX i = 0; i < sect.GetSize(); )
     {
       if(sect[i].Left(prefix.GetLength()) != prefix)
         sect.RemoveAt(i);
       else
        i++;
     }
     return sect;
   }
   BOOL HasSection(PString section) const
   {
     PStringList sect = MCUConfig("Parameters").GetSections();
     if(sect.GetStringsIndex(section) != P_MAX_INDEX)
       return TRUE;
     return FALSE;
   }
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class MCUURL : public PURL
{
  public:
    MCUURL();
    MCUURL(PString str);

    void SetDisplayName(PString name) { display_name = name; }

    virtual const PString & GetDisplayName() const { return display_name; }
    virtual const PString & GetUrl() const { return url_party; }
    virtual const PString & GetSipProto() const { return transport; }
    virtual const PString GetPort() const { return PString(port); }
    virtual const PString GetMemberName() const { return display_name+" ["+url_party+"]"; }
    virtual const PString GetMemberNameId() const
    {
      if(url_scheme == "sip")
       return username+"@"+hostname;
      else if(url_scheme == "h323")
        return display_name+"@"+hostname;
      else
        return username+"@"+hostname;
    }

  protected:
    PString display_name;
    PString url_scheme;
    PString url_party;
    PString transport;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // _MCU_UTIL_H
