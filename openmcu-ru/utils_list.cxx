/*
 * utils_list.cxx
 *
 * Copyright (C) 2014-2015 Andrey Burbovskiy, OpenMCU-ru, All Rights Reserved
 * Copyright (C) 2015 Konstantin Yeliseyev, OpenMCU-ru, All Rights Reserved
 *
 * The Initial Developer of the Original Code is Andrey Burbovskiy (andrewb@yandex.ru), All Rights Reserved
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
 * Contributor(s):  Andrey Burbovskiy (andrewb@yandex.ru)
 *                  Konstantin Yeliseyev (kay27@bk.ru)
 *
 */


#include "precompile.h"
#include "mcu.h"

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
