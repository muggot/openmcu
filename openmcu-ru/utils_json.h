
#include "precompile.h"

#ifndef _MCU_UTILS_JSON_H
#define _MCU_UTILS_JSON_H

#include "utils_list.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

std::string   JsQuoteScreen(const std::string &str);
std::string & JsQuoteScreen(const std::string &str, std::string &r);

////////////////////////////////////////////////////////////////////////////////////////////////////

class MCUJSON
{
  public:

    typedef MCUSharedListSharedIterator<MCUJSONList, MCUJSON> shared_iterator;
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

    MCUJSON(JsonTypes type = JSON_OBJECT, const std::string &key = "", int size = MCU_SHARED_LIST_SIZE);
    ~MCUJSON();

    static MCUJSON * Null();
    static MCUJSON * Bool(const std::string &key, bool value);
    static MCUJSON * Bool(bool value);
    static MCUJSON * Int(const std::string &key, int value);
    static MCUJSON * Int(int value);
    static MCUJSON * Int(const std::string &key, unsigned int value);
    static MCUJSON * Int(unsigned int value);
    static MCUJSON * Int(const std::string &key, long value);
    static MCUJSON * Int(long value);
    static MCUJSON * Int(const std::string &key, unsigned long value);
    static MCUJSON * Int(unsigned long value);
    static MCUJSON * Int(const std::string &key, long long value);
    static MCUJSON * Int(long long value);
    static MCUJSON * Double(const std::string &key, const double value);
    static MCUJSON * Double(const double value);
    static MCUJSON * String(const std::string &key, const std::string &value);
    static MCUJSON * String(const std::string &value);
    static MCUJSON * Array(const std::string &key = "", int size = MCU_SHARED_LIST_SIZE);
    static MCUJSON * Object(const std::string &key = "", int size = MCU_SHARED_LIST_SIZE);

    bool Insert(MCUJSON *json);
    bool Insert(const std::string &key, bool value);
    bool Insert(bool value);
    bool Insert(const std::string &key, int value);
    bool Insert(int value);
    bool Insert(const std::string &key, unsigned int value);
    bool Insert(unsigned int value);
    bool Insert(const std::string &key, long value);
    bool Insert(long value);
    bool Insert(const std::string &key, unsigned long value);
    bool Insert(unsigned long value);
    bool Insert(const std::string &key, long long value);
    bool Insert(long long value);
    bool Insert(const std::string &key, double value);
    bool Insert(double value);
    bool Insert(const std::string &key, const char *value);
    bool Insert(const char *value);
    bool Insert(const std::string &key, const std::string &value);
    bool Insert(const std::string &value);
    bool Insert(const std::string &key, const PString &value);
    bool Insert(const PString &value);

    bool Replace(const std::string &key, const std::string &value);
    bool Remove(const std::string &key);
    shared_iterator Find(const std::string &key);

    MCUJSON & operator = (bool value);
    MCUJSON & operator = (int value);
    MCUJSON & operator = (unsigned int value);
    MCUJSON & operator = (long value);
    MCUJSON & operator = (unsigned long value);
    MCUJSON & operator = (long long value);
    MCUJSON & operator = (double value);
    MCUJSON & operator = (const char *value);
    MCUJSON & operator = (const std::string &value);
    MCUJSON & operator = (const PString &value);
    MCUJSON & operator = (MCUJSONList *value);
    MCUJSON & operator = (MCUJSON &json);

    bool operator == (MCUJSON &json)
    { return (AsString() == json.AsString()); }

    bool operator != (MCUJSON &json)
    { return (AsString() != json.AsString()); }

    friend ostream & operator << (ostream &ostr, MCUJSON &json)
    { ostr << json.AsString(); return ostr; }

    std::string   AsString();
    std::string & ToString(std::string &str, bool print_keys = true, bool print_esc = false, int esc_level = 0);

    void PrintEsc(std::string &str, bool print_esc, int esc_level);

  protected:
    std::string json_key;
    JsonTypes json_type;
    shared_iterator iterator_end;

    bool value_bool;
    long long value_int;
    double value_double;
    std::string value_string;
    MCUJSONList * value_array;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // _MCU_UTILS_JSON_H
