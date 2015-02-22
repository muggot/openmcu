
#include <ptlib.h>

#include "config.h"
#include "mcu.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

PString JsQuoteScreen(PString s)
{
  PString r="\"";
  for(PINDEX i=0; i<s.GetLength(); i++)
  { BYTE c=(BYTE)s[i];
    if(c>31)
    { if     (c==0x22) r+="\\x22"; // "
      else if(c==0x5c) r+="\\x5c"; // backslash
      else if(c=='<') r+="&lt;";
      else if(c=='>') r+="&gt;";
      else r+=(char)c;
    }
    else
    {
      if(c==9) r+="&nbsp;|&nbsp;"; //tab
      if(c==10) if(r.Right(1)!=" ") r+=" ";
      if(c==13) if(r.Right(1)!=" ") r+=" ";
    }
  }
  r+="\"";
  return r;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

MCUJSON::MCUJSON(JsonTypes type, const char *key, int size)
{
  if(type == JSON_ARRAY || type == JSON_OBJECT)
  {
    json_key = key;
    json_type = type;
    value_array = new MCUSharedList<MCUJSON>(size);
  } else {
    json_key = key;
    json_type = type;
    value_bool = false;
    value_int = 0;
    value_double = 0;
    value_string = "";
    value_array = NULL;
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

MCUJSON::~MCUJSON()
{
  if(value_array)
  {
    for(shared_iterator it = value_array->begin(); it != value_array->end(); ++it)
    {
      MCUJSON *json = *it;
      if(value_array->Erase(it))
        delete json;
    }
    delete value_array;
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

MCUJSON * MCUJSON::Null()
{
  MCUJSON *json = new MCUJSON(JSON_NULL);
  return json;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

MCUJSON * MCUJSON::Bool(const char *key, bool value)
{
  MCUJSON *json = new MCUJSON(JSON_BOOL, key);
  json->value_bool = value;
  return json;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

MCUJSON * MCUJSON::Bool(bool value)
{
  return MCUJSON::Bool(NULL, value);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

MCUJSON * MCUJSON::Int(const char *key, int value)
{
  MCUJSON *json = new MCUJSON(JSON_INT, key);
  json->value_int = value;
  return json;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

MCUJSON * MCUJSON::Int(int value)
{
  return MCUJSON::Int(NULL, value);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

MCUJSON * MCUJSON::Double(const char *key, double value)
{
  MCUJSON *json = new MCUJSON(JSON_DOUBLE, key);
  json->value_double = value;
  return json;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

MCUJSON * MCUJSON::Double(double value)
{
  return MCUJSON::Double(NULL, value);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

MCUJSON * MCUJSON::String(const char *key, const char *value)
{
  MCUJSON *json = new MCUJSON(JSON_STRING, key);
  json->value_string = value;
  return json;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

MCUJSON * MCUJSON::String(const char *value)
{
  return MCUJSON::String(NULL, value);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

MCUJSON * MCUJSON::Array(const char *key)
{
  MCUJSON *json = new MCUJSON(JSON_ARRAY, key);
  return json;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

MCUJSON * MCUJSON::Object(const char *key)
{
  MCUJSON *json = new MCUJSON(JSON_OBJECT, key);
  return json;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool MCUJSON::Insert(MCUJSON *json)
{
  if(json_type != JSON_ARRAY && json_type != JSON_OBJECT)
  {
    delete json;
    return false;
  }
  shared_iterator it = value_array->Insert(json, value_array->GetNextID(), json->json_key);
  if(it == value_array->end())
  {
    delete json;
    return false;
  }
  return true;
}

bool MCUJSON::Insert(const char *key, bool value)
{ return Insert(MCUJSON::Bool(key, value)); }

bool MCUJSON::Insert(bool value)
{ return Insert(MCUJSON::Bool(NULL, value)); }

bool MCUJSON::Insert(const char *key, int value)
{ return Insert(MCUJSON::Int(key, value)); }

bool MCUJSON::Insert(int value)
{ return Insert(MCUJSON::Int(NULL, value)); }

bool MCUJSON::Insert(const char *key, double value)
{ return Insert(MCUJSON::Double(key, value)); }

bool MCUJSON::Insert(double value)
{ return Insert(MCUJSON::Double(NULL, value)); }

bool MCUJSON::Insert(const char *key, const char *value)
{ return Insert(MCUJSON::String(key, value)); }

bool MCUJSON::Insert(const char *value)
{ return Insert(MCUJSON::String(NULL, value)); }

bool MCUJSON::Insert(const char *key, const PString & value)
{ return Insert(MCUJSON::String(key, value)); }

bool MCUJSON::Insert(const PString & value)
{ return Insert(MCUJSON::String(NULL, value)); }

////////////////////////////////////////////////////////////////////////////////////////////////////

bool MCUJSON::Remove(const char *key)
{
  if(value_array)
  {
    shared_iterator it = value_array->Find(key);
    if(it != value_array->end())
    {
      MCUJSON *json = *it;
      if(value_array->Erase(it))
      {
        delete json;
        return true;
      }
    }
  }
  return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool MCUJSON::Replace(const char *key, const char *value)
{
  if(value_array)
  {
    shared_iterator it = value_array->Find(key);
    if(it == value_array->end())
      return false;
    **it = value;
  }
  return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

MCUSharedListSharedIterator<MCUSharedList<MCUJSON>, MCUJSON> MCUJSON::Find(const char *key)
{
  if(value_array)
    return value_array->Find(key);
  return iterator_end;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

PString MCUJSON::AsString()
{
  std::ostringstream ostr;

  if(json_key != "")
    ostr << JsQuoteScreen(json_key) << ":";

  switch(json_type)
  {
    case(JSON_NULL):
      ostr << "null";
      break;
    case(JSON_BOOL):
      ostr << value_bool;
      break;
    case(JSON_INT):
      ostr << value_int;
      break;
    case(JSON_DOUBLE):
      ostr << value_double;
      break;
    case(JSON_STRING):
      ostr << JsQuoteScreen(value_string);
      break;
    case(JSON_ARRAY):
    case(JSON_OBJECT):
    {
      if(json_type == JSON_OBJECT)
        ostr << "{";
      else if(json_type == JSON_ARRAY)
        ostr << "[";

      int i = 0;
      for(shared_iterator it = value_array->begin(); it != value_array->end(); ++it, ++i)
      {
        if(i > 0)
          ostr << ",";
        ostr << it->AsString();
      }

      if(json_type == JSON_OBJECT)
        ostr << "}";
      else if(json_type == JSON_ARRAY)
        ostr << "]";

      break;
    }
    default:
      break;
  }

  return ostr.str();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

MCUJSON & MCUJSON::operator = (bool value)
{
  if(json_type == JSON_BOOL)
    value_bool = value;
  return *this;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

MCUJSON & MCUJSON::operator = (int value)
{
  if(json_type == JSON_INT)
    value_int = value;
  return *this;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

MCUJSON & MCUJSON::operator = (double value)
{
  if(json_type == JSON_DOUBLE)
    value_double = value;
  return *this;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

MCUJSON & MCUJSON::operator = (const char *value)
{
  if(json_type == JSON_STRING)
    value_string = value;
  return *this;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

MCUJSON & MCUJSON::operator = (MCUSharedList<MCUJSON> *value)
{
  if(value_array)
  {
    for(shared_iterator it = value_array->begin(); it != value_array->end(); ++it)
    {
      MCUJSON *json = *it;
      if(value_array->Erase(it))
        delete json;
    }
    for(shared_iterator it = value->begin(); it != value->end(); ++it)
    {
      MCUJSON *json = new MCUJSON(it->json_type, it->json_key);
      *json = **it;
      Insert(json);
    }
  }
  return *this;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

MCUJSON & MCUJSON::operator = (MCUJSON &json)
{
  switch(json.json_type)
  {
    case(JSON_BOOL):
      return (*this)=json.value_bool;
      break;
    case(JSON_INT):
      return (*this)=json.value_int;
      break;
    case(JSON_DOUBLE):
      return (*this)=json.value_double;
      break;
    case(JSON_STRING):
      return (*this)=(const char *)json.value_string;
      break;
    case(JSON_ARRAY):
    case(JSON_OBJECT):
      return (*this)=json.value_array;
      break;
    default:
      break;
  }
  return *this;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

