
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
