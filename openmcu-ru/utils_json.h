
#ifndef _MCU_UTILS_JSON_H
#define _MCU_UTILS_JSON_H

#include "config.h"
#include "utils_list.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

PString JsQuoteScreen(PString s);

////////////////////////////////////////////////////////////////////////////////////////////////////

class MCUJSON
{
  public:

    typedef MCUSharedListSharedIterator<MCUSharedList<MCUJSON>, MCUJSON> shared_iterator;
    const shared_iterator & end() const { return iterator_end; }

    enum JsonTypes
    {
      JSON_NULL = 0,
      JSON_BOOL,
      JSON_INT,
      JSON_DOUBLE,
      JSON_STRING,
      JSON_ARRAY,
      JSON_OBJECT
    };

    MCUJSON(JsonTypes type = JSON_OBJECT, const char *key = NULL, int size = 256);
    ~MCUJSON();

    static MCUJSON * Null();
    static MCUJSON * Bool(const char *key, bool value);
    static MCUJSON * Bool(bool value);
    static MCUJSON * Int(const char *key, int value);
    static MCUJSON * Int(int value);
    static MCUJSON * Double(const char *key, double value);
    static MCUJSON * Double(double value);
    static MCUJSON * String(const char *key, const char *value);
    static MCUJSON * String(const char *value);
    static MCUJSON * Array(const char *key = NULL);
    static MCUJSON * Object(const char *key = NULL);

    bool Insert(MCUJSON *json);
    bool Insert(const char *key, bool value);
    bool Insert(bool value);
    bool Insert(const char *key, int value);
    bool Insert(int value);
    bool Insert(const char *key, double value);
    bool Insert(double value);
    bool Insert(const char *key, const char *value);
    bool Insert(const char *value);
    bool Insert(const char *key, const PString & value);
    bool Insert(const PString & value);

    bool Remove(const char *key);
    bool Replace(const char *key, const char *value);
    shared_iterator Find(const char *key);
    PString AsString();

    MCUJSON & operator = (bool value);
    MCUJSON & operator = (int value);
    MCUJSON & operator = (double value);
    MCUJSON & operator = (const char *value);
    MCUJSON & operator = (MCUSharedList<MCUJSON> *value);
    MCUJSON & operator = (MCUJSON &json);

    bool operator == (MCUJSON &json)
    { return (AsString() == json.AsString()); }

    bool operator != (MCUJSON &json)
    { return (AsString() != json.AsString()); }

    friend ostream & operator << (ostream &ostr, MCUJSON &json)
    { ostr << json.AsString(); return ostr; }

  protected:
    PString json_key;
    JsonTypes json_type;
    shared_iterator iterator_end;

    bool value_bool;
    int value_int;
    double value_double;
    PString value_string;
    MCUSharedList<MCUJSON> * value_array;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // _MCU_UTILS_JSON_H
