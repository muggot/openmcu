/*
 * utils_json.cxx
 *
 * Copyright (C) 2015 Andrey Burbovskiy, OpenMCU-ru, All Rights Reserved
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
 *
 */


#include "precompile.h"
#include "mcu.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

std::string JsQuoteScreen(const std::string &str)
{
  std::string r;
  return JsQuoteScreen(str, r);
}

std::string & JsQuoteScreen(const std::string &str, std::string &r)
{
  r.push_back('"');
  for(unsigned i = 0; i < str.size(); ++i)
  {
    BYTE c = (BYTE)str[i];
    switch(c)
    {
      case(0x22):
        // "
        r.push_back('\\'); r.push_back('\\'); r.push_back('x'); r.push_back('2'); r.push_back('2');
        break;
      case(0x5c):
        // backslash
        r.push_back('\\'); r.push_back('\\'); r.push_back('x'); r.push_back('5'); r.push_back('c');
        break;
      case ('<'):
        r.push_back('&'); r.push_back('l'); r.push_back('t');
        break;
      case ('>'):
        r.push_back('&'); r.push_back('g'); r.push_back('t');
        break;
      case(9):
        // tab
        r.push_back('&'); r.push_back('n'); r.push_back('b'); r.push_back('s'); r.push_back('p'); r.push_back(';'); r.push_back('|');
        r.push_back('&'); r.push_back('n'); r.push_back('b'); r.push_back('s'); r.push_back('p'); r.push_back(';');
        break;
      case(10):
        if(r[r.size() - 1] != ' ')
          r.push_back(' ');
        break;
      case(13):
        if(r[r.size() - 1] != ' ')
          r.push_back(' ');
        break;
      default:
        if(c > 31)
          r.push_back(c);
        break;
    }
  }
  r.push_back('"');
  return r;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

MCUJSON::MCUJSON(JsonTypes type, const std::string &key, int size)
  : value_bool(false), value_int(0), value_double(0), value_array(NULL)
{
  if(type == JSON_ARRAY || type == JSON_OBJECT)
  {
    json_key = key;
    json_type = type;
    value_array = new MCUJSONList(size);
  } else {
    json_key = key;
    json_type = type;
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
  return new MCUJSON(JSON_NULL);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

MCUJSON * MCUJSON::Bool(const std::string &key, bool value)
{
  MCUJSON *json = new MCUJSON(JSON_BOOL, key);
  json->value_bool = value;
  return json;
}

MCUJSON * MCUJSON::Bool(bool value)
{
  return MCUJSON::Bool("", value);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

MCUJSON * MCUJSON::Int(const std::string &key, int value)
{
  MCUJSON *json = new MCUJSON(JSON_INT, key);
  json->value_int = value;
  return json;
}
MCUJSON * MCUJSON::Int(int value)
{
  return MCUJSON::Int("", value);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

MCUJSON * MCUJSON::Int(const std::string &key, unsigned int value)
{
  MCUJSON *json = new MCUJSON(JSON_INT, key);
  json->value_int = value;
  return json;
}
MCUJSON * MCUJSON::Int(unsigned int value)
{
  return MCUJSON::Int("", value);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

MCUJSON * MCUJSON::Int(const std::string &key, long value)
{
  MCUJSON *json = new MCUJSON(JSON_INT, key);
  json->value_int = value;
  return json;
}
MCUJSON * MCUJSON::Int(long value)
{
  return MCUJSON::Int("", value);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

MCUJSON * MCUJSON::Int(const std::string &key, unsigned long value)
{
  MCUJSON *json = new MCUJSON(JSON_INT, key);
  json->value_int = value;
  return json;
}
MCUJSON * MCUJSON::Int(unsigned long value)
{
  return MCUJSON::Int("", value);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

MCUJSON * MCUJSON::Int(const std::string &key, long long value)
{
  MCUJSON *json = new MCUJSON(JSON_INT, key);
  json->value_int = value;
  return json;
}
MCUJSON * MCUJSON::Int(long long value)
{
  return MCUJSON::Int("", value);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

MCUJSON * MCUJSON::Double(const std::string &key, double value)
{
  MCUJSON *json = new MCUJSON(JSON_DOUBLE, key);
  json->value_double = value;
  return json;
}
MCUJSON * MCUJSON::Double(double value)
{
  return MCUJSON::Double("", value);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

MCUJSON * MCUJSON::String(const std::string &key, const std::string &value)
{
  MCUJSON *json = new MCUJSON(JSON_STRING, key);
  json->value_string = value;
  return json;
}
MCUJSON * MCUJSON::String(const std::string &value)
{
  return MCUJSON::String("", value);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

MCUJSON * MCUJSON::Array(const std::string &key, int size)
{
  return new MCUJSON(JSON_ARRAY, key, size);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

MCUJSON * MCUJSON::Object(const std::string &key, int size)
{
  return new MCUJSON(JSON_OBJECT, key, size);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool MCUJSON::Insert(MCUJSON *json)
{
  if(json_type != JSON_ARRAY && json_type != JSON_OBJECT)
  {
    delete json;
    return false;
  }
  shared_iterator it = value_array->Pushback(json, value_array->GetNextID(), json->json_key);
  if(it == value_array->end())
  {
    delete json;
    return false;
  }
  return true;
}

bool MCUJSON::Insert(const std::string &key, bool value)
{
  return Insert(MCUJSON::Bool(key, value));
}
bool MCUJSON::Insert(bool value)
{
  return Insert(MCUJSON::Bool("", value));
}

bool MCUJSON::Insert(const std::string &key, int value)
{
  return Insert(MCUJSON::Int(key, value));
}
bool MCUJSON::Insert(int value)
{
  return Insert(MCUJSON::Int("", value));
}

bool MCUJSON::Insert(const std::string &key, unsigned int value)
{
  return Insert(MCUJSON::Int(key, value));
}
bool MCUJSON::Insert(unsigned int value)
{
  return Insert(MCUJSON::Int("", value));
}

bool MCUJSON::Insert(const std::string &key, long value)
{
  return Insert(MCUJSON::Int(key, value));
}
bool MCUJSON::Insert(long value)
{
  return Insert(MCUJSON::Int("", value));
}

bool MCUJSON::Insert(const std::string &key, unsigned long value)
{
  return Insert(MCUJSON::Int(key, value));
}
bool MCUJSON::Insert(unsigned long value)
{
  return Insert(MCUJSON::Int("", value));
}

bool MCUJSON::Insert(const std::string &key, long long value)
{
  return Insert(MCUJSON::Int(key, value));
}
bool MCUJSON::Insert(long long value)
{
  return Insert(MCUJSON::Int("", value));
}

bool MCUJSON::Insert(const std::string &key, double value)
{
  return Insert(MCUJSON::Double(key, value));
}
bool MCUJSON::Insert(double value)
{
  return Insert(MCUJSON::Double("", value));
}

bool MCUJSON::Insert(const std::string &key, const char *value)
{
  return Insert(MCUJSON::String(key, value));
}
bool MCUJSON::Insert(const char *value)
{
  return Insert(MCUJSON::String("", value));
}

bool MCUJSON::Insert(const std::string &key, const std::string &value)
{
  return Insert(MCUJSON::String(key, value));
}
bool MCUJSON::Insert(const std::string &value)
{
  return Insert(MCUJSON::String("", value));
}

bool MCUJSON::Insert(const std::string &key, const PString &value)
{
  return Insert(MCUJSON::String(key, value));
}
bool MCUJSON::Insert(const PString &value)
{
  return Insert(MCUJSON::String("", value));
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool MCUJSON::Replace(const std::string &key, const std::string &value)
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

bool MCUJSON::Remove(const std::string &key)
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

MCUSharedListSharedIterator<MCUJSONList, MCUJSON> MCUJSON::Find(const std::string &key)
{
  if(value_array)
    return value_array->Find(key);
  return iterator_end;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

MCUJSON & MCUJSON::operator = (bool value)
{
  if(json_type == JSON_BOOL)
    value_bool = value;
  return *this;
}

MCUJSON & MCUJSON::operator = (int value)
{
  if(json_type == JSON_INT)
    value_int = value;
  return *this;
}

MCUJSON & MCUJSON::operator = (unsigned int value)
{
  if(json_type == JSON_INT)
    value_int = value;
  return *this;
}

MCUJSON & MCUJSON::operator = (long value)
{
  if(json_type == JSON_INT)
    value_int = value;
  return *this;
}

MCUJSON & MCUJSON::operator = (unsigned long value)
{
  if(json_type == JSON_INT)
    value_int = value;
  return *this;
}

MCUJSON & MCUJSON::operator = (long long value)
{
  if(json_type == JSON_INT)
    value_int = value;
  return *this;
}

MCUJSON & MCUJSON::operator = (double value)
{
  if(json_type == JSON_DOUBLE)
    value_double = value;
  return *this;
}

MCUJSON & MCUJSON::operator = (const char *value)
{
  if(json_type == JSON_STRING)
    if(value)
      value_string = value;
  return *this;
}

MCUJSON & MCUJSON::operator = (const std::string &value)
{
  if(json_type == JSON_STRING)
    value_string = value;
  return *this;
}

MCUJSON & MCUJSON::operator = (const PString &value)
{
  if(json_type == JSON_STRING)
    value_string = (const char *)value;
  return *this;
}

MCUJSON & MCUJSON::operator = (MCUJSONList *value)
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
      return (*this)=json.value_string;
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

std::string MCUJSON::AsString()
{
  std::string str;
  return ToString(str);
}

std::string & MCUJSON::ToString(std::string &str, bool print_keys, bool print_esc, int esc_level)
{
  if(print_keys && !json_key.empty())
  {
    JsQuoteScreen(json_key, str);
    str.push_back(':');
  }

  switch(json_type)
  {
    case(JSON_NULL):
    {
      str.push_back('n'); str.push_back('u'); str.push_back('l'); str.push_back('l');
      break;
    }
    case(JSON_BOOL):
    {
      if(value_bool)
        str.push_back('1');
      else
        str.push_back('0');
      break;
    }
    case(JSON_INT):
    {
      char buffer[32];
      int digits = snprintf(buffer, 32, "%lld", value_int);
      for(int i = 0; i < digits; ++i)
        str.push_back(buffer[i]);
      break;
    }
    case(JSON_DOUBLE):
    {
      char buffer[32];
      int digits = snprintf(buffer, 32, "%f", value_double);
      for(int i = 0; i < digits; ++i)
        str.push_back(buffer[i]);
      break;
    }
    case(JSON_STRING):
      JsQuoteScreen(value_string, str);
      break;
    case(JSON_ARRAY):
    case(JSON_OBJECT):
    {
      if(print_keys && !json_key.empty())
        PrintEsc(str, print_esc, esc_level);
      if(json_type == JSON_OBJECT)
        str.push_back('{');
      else
        str.push_back('[');

      int i = 0;
      for(shared_iterator it = value_array->begin(); it != value_array->end(); ++it, ++i)
      {
        if(i > 0)
          str.push_back(',');
        PrintEsc(str, print_esc, esc_level+1);
        it->ToString(str, print_keys, print_esc, esc_level+1);
      }

      PrintEsc(str, print_esc, esc_level);
      if(json_type == JSON_OBJECT)
        str.push_back('}');
      else
        str.push_back(']');

      break;
    }
    default:
      break;
  }

  return str;
}

void MCUJSON::PrintEsc(std::string &str, bool print_esc, int esc_level)
{
  if(print_esc)
  {
    str.push_back('\r');
    str.push_back('\n');
    for(int i = 0; i < esc_level; ++i)
      str.push_back(' ');
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

