
#ifndef _MCU_UTILS_H
#define _MCU_UTILS_H

#include "config.h"
#include "utils_av.h"
#include "utils_list.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

PString convert_cp1251_to_utf8(PString str);
PString convert_ucs2_to_utf8(PString str);

PString GetSectionParamFromUrl(PString param, PString url, bool asterisk = true);
PString GetSectionParamFromUrl(PString param, PString url, PString defaultValue, bool asterisk = true);

PString GetConferenceParam(PString room, PString param, PString defaultValue);
int GetConferenceParam(PString room, PString param, int defaultValue);

char * PStringToChar(const PString & str);
PString PWORDArrayToPString(const PWORDArray & ar);

BOOL CheckCapability(const PString & formatName);
BOOL SkipCapability(const PString & formatName, MCUConnectionTypes connectionType = CONNECTION_TYPE_NONE);

PString GetPluginName(const PString & format);

PString JsQuoteScreen(PString s);

////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // _MCU_UTILS_H
