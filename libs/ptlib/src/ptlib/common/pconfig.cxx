/*
 * pconfig.cxx
 *
 * Operating System utilities.
 *
 * Portable Windows Library
 *
 * Copyright (c) 1993-1998 Equivalence Pty. Ltd.
 *
 * The contents of this file are subject to the Mozilla Public License
 * Version 1.0 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See
 * the License for the specific language governing rights and limitations
 * under the License.
 *
 * The Original Code is Portable Windows Library.
 *
 * The Initial Developer of the Original Code is Equivalence Pty. Ltd.
 *
 * Portions are Copyright (C) 1993 Free Software Foundation, Inc.
 * All Rights Reserved.
 *
 * Contributor(s): ______________________________________.
 *
 * $Log: pconfig.cxx,v $
 * Revision 1.2  2000/06/26 11:17:21  robertj
 * Nucleus++ port (incomplete).
 *
 * Revision 1.1  1998/11/30 12:46:17  robertj
 * Initial revision
 *
 */

#include <ptlib.h>

#include <ctype.h>


///////////////////////////////////////////////////////////////////////////////
// PConfig

PStringToString PConfig::GetAllKeyValues(const PString & section) const
{
  PStringToString dict;

  PStringList keys = GetKeys(section);
  for (PINDEX i = 0; i < keys.GetSize(); i++)
    dict.SetAt(keys[i], GetString(section, keys[i], ""));

  return dict;
}


#if !defined(_WIN32) || defined (__NUCLEUS_MNT__)

BOOL PConfig::GetBoolean(const PString & section, const PString & key, BOOL dflt) const
{
  PString str = GetString(section, key, dflt ? "T" : "F").ToUpper();
  return str[0] == 'T' || str[0] == 'Y' || str.AsInteger() != 0;
}


void PConfig::SetBoolean(const PString & section, const PString & key, BOOL value)
{
  SetString(section, key, value ? "True" : "False");
}


long PConfig::GetInteger(const PString & section, const PString & key, long dflt) const
{
  PString str(PString::Signed, dflt);
  return GetString(section, key, str).AsInteger();
}


void PConfig::SetInteger(const PString & section, const PString & key, long value)
{
  PString str(PString::Signed, value);
  SetString(section, key, str);
}

#endif


PInt64 PConfig::GetInt64(const PString & section, const PString & key, PInt64 dflt) const
{
  PString str = GetString(section, key, "");
  if (!str)
    return str.AsInt64();
  return dflt;
}


void PConfig::SetInt64(const PString & section, const PString & key, PInt64 value)
{
  PStringStream strm;
  strm << value;
  SetString(section, key, strm);
}


double PConfig::GetReal(const PString & section, const PString & key, double dflt) const
{
  PString str(PString::Printf, "%g", dflt);
  return GetString(section, key, str).AsReal();
}


void PConfig::SetReal(const PString & section, const PString & key, double value)
{
  PString str(PString::Printf, "%g", value);
  SetString(section, key, str);
}


PTime PConfig::GetTime(const PString & section, const PString & key) const
{
  return GetString(section, key, "1 Jan 1996");
}


PTime PConfig::GetTime(const PString & section, const PString & key, const PTime & dflt) const
{
  return GetString(section, key, dflt.AsString());
}


void PConfig::SetTime(const PString & section, const PString & key, const PTime & value)
{
  SetString(section, key, value.AsString());
}


// End Of File ///////////////////////////////////////////////////////////////
