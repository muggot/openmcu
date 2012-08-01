/*
 * pldap.h
 *
 * Lightweight Directory Access Protocol interface class.
 *
 * Portable Windows Library
 *
 * Copyright (c) 1993-2003 Equivalence Pty. Ltd.
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
 * Contributor(s): ______________________________________.
 *
 * $Log: pldap.h,v $
 * Revision 1.13  2007/07/22 23:54:38  rjongbloed
 * Fixed MSVC compiler warning.
 *
 * Revision 1.12  2007/07/20 21:44:03  shorne
 * Removed virtual on PLDAPSchema::SchemaName
 *
 * Revision 1.11  2007/07/19 15:05:25  shorne
 * Added Factory loaded LDAP schemas
 *
 * Revision 1.10  2006/01/16 19:52:05  dsandras
 * Applied patch from Brian Lu <brian lu sun com> to allow compilation on
 * Solaris using SUN's LDAP. Thanks!!
 *
 * Revision 1.9  2004/05/24 12:02:49  csoutheren
 * Add function to permit setting a limit on the number of results returned
 * from an LDAP query. Change the default number of results to unlimited,
 * rather than MAX_INT which apparently is clamped to some arbitrary low value.
 * Thanks to Damien Sandras
 *
 * Revision 1.8  2004/02/20 16:28:27  ykiryanov
 * if'd LDAP code to enable non-LDAP builds
 *
 * Revision 1.7  2003/06/05 23:17:07  rjongbloed
 * Added functions to get and set LDAP operation timeout.
 *
 * Revision 1.6  2003/06/05 05:29:30  rjongbloed
 * Fixed LDAP bind authentication methods, thanks Ravelli Rossano
 *
 * Revision 1.5  2003/04/07 12:00:04  robertj
 * Fixed search function returning an error if can't find anything for filter.
 *
 * Revision 1.4  2003/04/01 07:05:29  robertj
 * Added ability to specify host:port in opening an LDAP server
 *
 * Revision 1.3  2003/03/31 09:02:43  robertj
 * Added missing return for error number.
 *
 * Revision 1.2  2003/03/31 03:32:41  robertj
 * Major addition of functionality.
 *
 * Revision 1.1  2003/03/28 01:15:44  robertj
 * OpenLDAP support.
 *
 *
 */

#ifndef _PLDAP_H
#define _PLDAP_H

#ifdef P_USE_PRAGMA
#pragma interface
#endif

#if P_LDAP

#include <ptlib/sockets.h>
#include <ptlib/pluginmgr.h>
#include <map>
#include <list>

struct ldap;
struct ldapmsg;
struct ldapmod;
struct berval;

class PLDAPStructBase;


/**This class will create an LDAP client to access a remote LDAP server.
  */
class PLDAPSession : public PObject
{
  PCLASSINFO(PLDAPSession, PObject);
  public:
    /**Create a LDAP client.
      */
    PLDAPSession(
      const PString & defaultBaseDN = PString::Empty()
    );

    /**Close the sesison on destruction
      */
    ~PLDAPSession();

    /**Open the LDAP session to the specified server.
       The server name string is of the form hostip[:port] where hostip is a
       host name or IP address and the :port is an optional port number. If
       not present then the port variable is used. If that is also zero then
       the default port of 369 is used.
      */
    BOOL Open(
      const PString & server,
      WORD port = 0
    );

    /**Close the LDAP session
      */
    BOOL Close();

    /**Determine of session is open.
      */
    BOOL IsOpen() const { return ldapContext != NULL; }

    /**Set LDAP option parameter (OpenLDAp specific values)
      */
    BOOL SetOption(
      int optcode,
      int value
    );

    /**Set LDAP option parameter (OpenLDAP specific values)
      */
    BOOL SetOption(
      int optcode,
      void * value
    );

    enum AuthenticationMethod {
      AuthSimple,
      AuthSASL,
      AuthKerberos,
#ifdef SOLARIS
      NumAuthenticationMethod1,
      NumAuthenticationMethod2
#else
      NumAuthenticationMethod
#endif
    };

    /**Bind to the remote LDAP server.
      */
    BOOL Bind(
      const PString & who = PString::Empty(),
      const PString & passwd = PString::Empty(),
      AuthenticationMethod authMethod = AuthSimple
    );

    class ModAttrib : public PObject {
        PCLASSINFO(ModAttrib, PObject);
      public:
        enum Operation {
          Add,
          Replace,
          Delete,
          NumOperations
        };

      protected:
        ModAttrib(
          const PString & name,
          Operation op = NumOperations
        );

      public:
        const PString & GetName() const { return name; }

        Operation GetOperation() const { return op; }

        void SetLDAPMod(
          struct ldapmod & mod,
          Operation defaultOp
        );

      protected:
        virtual BOOL IsBinary() const = 0;
        virtual void SetLDAPModVars(struct ldapmod & mod) = 0;

        PString   name;
        Operation op;
    };

    class StringModAttrib : public ModAttrib {
        PCLASSINFO(StringModAttrib, ModAttrib);
      public:
        StringModAttrib(
          const PString & name,
          Operation op = NumOperations
        );
        StringModAttrib(
          const PString & name,
          const PString & value,
          Operation op = NumOperations
        );
        StringModAttrib(
          const PString & name,
          const PStringList & values,
          Operation op = NumOperations
        );
        void SetValue(
          const PString & value
        );
        void AddValue(
          const PString & value
        );
      protected:
        virtual BOOL IsBinary() const;
        virtual void SetLDAPModVars(struct ldapmod & mod);

        PStringList values;
        PBaseArray<char *> pointers;
    };

    class BinaryModAttrib : public ModAttrib {
        PCLASSINFO(BinaryModAttrib, ModAttrib);
      public:
        BinaryModAttrib(
          const PString & name,
          Operation op = Add
        );
        BinaryModAttrib(
          const PString & name,
          const PBYTEArray & value,
          Operation op = Add
        );
        BinaryModAttrib(
          const PString & name,
          const PList<PBYTEArray> & values,
          Operation op = Add
        );
        void SetValue(
          const PBYTEArray & value
        );
        void AddValue(
          const PBYTEArray & value
        );
      protected:
        virtual BOOL IsBinary() const;
        virtual void SetLDAPModVars(struct ldapmod & mod);

        PList<PBYTEArray> values;
        PBaseArray<struct berval *> pointers;
        PBYTEArray bervals;
    };

    /**Add a new distringuished name to LDAP dirctory.
      */
    BOOL Add(
      const PString & dn,
      const PList<ModAttrib> & attributes
    );

    /**Add a new distringuished name to LDAP dirctory.
      */
    BOOL Add(
      const PString & dn,
      const PStringToString & attributes
    );

    /**Add a new distringuished name to LDAP dirctory.
       The attributes list is a string array of the form attr=value
      */
    BOOL Add(
      const PString & dn,
      const PStringArray & attributes
    );

    /**Add a new distringuished name to LDAP dirctory.
       The attributes list is a string array of the form attr=value
      */
    BOOL Add(
      const PString & dn,
      const PLDAPStructBase & data
    );

    /**Modify an existing distringuished name to LDAP dirctory.
      */
    BOOL Modify(
      const PString & dn,
      const PList<ModAttrib> & attributes
    );

    /**Add a new distringuished name to LDAP dirctory.
      */
    BOOL Modify(
      const PString & dn,
      const PStringToString & attributes
    );

    /**Add a new distringuished name to LDAP dirctory.
       The attributes list is a string array of the form attr=value
      */
    BOOL Modify(
      const PString & dn,
      const PStringArray & attributes
    );

    /**Add a new distringuished name to LDAP dirctory.
       The attributes list is a string array of the form attr=value
      */
    BOOL Modify(
      const PString & dn,
      const PLDAPStructBase & data
    );

    /**Delete the distinguished name from LDAP directory.
      */
    BOOL Delete(
      const PString & dn
    );


    enum SearchScope {
      ScopeBaseOnly,
      ScopeSingleLevel,
      ScopeSubTree,
      NumSearchScope
    };

    class SearchContext {
      public:
        SearchContext();
        ~SearchContext();

        BOOL IsCompleted() const { return completed; }

      private:
        int              msgid;
        struct ldapmsg * result;
        struct ldapmsg * message;
        BOOL             found;
        BOOL             completed;

      friend class PLDAPSession;
    };

    /**Start search for specified information.
      */
    BOOL Search(
      SearchContext & context,
      const PString & filter,
      const PStringArray & attributes = PStringList(),
      const PString & base = PString::Empty(),
      SearchScope scope = ScopeSubTree
    );

    /**Get the current search result entry.
      */
    BOOL GetSearchResult(
      SearchContext & context,
      PStringToString & data
    );

    /**Get an attribute of the current search result entry.
      */
    BOOL GetSearchResult(
      SearchContext & context,
      const PString & attribute,
      PString & data
    );

    /**Get an attribute of the current search result entry.
      */
    BOOL GetSearchResult(
      SearchContext & context,
      const PString & attribute,
      PStringArray & data
    );

    /**Get an attribute of the current search result entry.
      */
    BOOL GetSearchResult(
      SearchContext & context,
      const PString & attribute,
      PArray<PBYTEArray> & data
    );

    /**Get all attributes of the current search result entry.
      */
    BOOL GetSearchResult(
      SearchContext & context,
      PLDAPStructBase & data
    );

    /**Get the current search result distinguished name entry.
      */
    PString GetSearchResultDN(
      SearchContext & context
    );

    /**Get the next search result.
      */
    BOOL GetNextSearchResult(
      SearchContext & context
    );

    /**Search for specified information, returning all matches.
       This can be used for simple LDAP databases where all attributes are
       represented by a string.
      */
    PList<PStringToString> Search(
      const PString & filter,
      const PStringArray & attributes = PStringList(),
      const PString & base = PString::Empty(),
      SearchScope scope = ScopeSubTree
    );


    /**Set the default base DN for use if not specified for searches.
      */
    void SetBaseDN(
      const PString & dn
    ) { defaultBaseDN = dn; }

    /**Set the default base DN for use if not specified for searches.
      */
    const PString & GetBaseDN() const { return defaultBaseDN; }

    /**Get the last OpenLDAP error code.
      */
    int GetErrorNumber() const { return errorNumber; }

    /**Get the last OpenLDAP error as text string.
      */
    PString GetErrorText() const;

    /**Get the OpenLDAP context structure.
      */
    struct ldap * GetOpenLDAP() const { return ldapContext; }

    /**Get the timeout for LDAP operations.
      */
    const PTimeInterval & GetTimeout() const { return timeout; }

    /**Set the timeout for LDAP operations.
      */
    void SetTimeout(
      const PTimeInterval & t
    ) { timeout = t; }

    /**Set a limit on the number of results to return
      */
     void SetSearchLimit(
      const unsigned s
    ) { searchLimit = s; }

  protected:
    struct ldap * ldapContext;
    int           errorNumber;
    unsigned      protocolVersion;
    PString       defaultBaseDN;
    unsigned      searchLimit;
    PTimeInterval timeout;
    PString       multipleValueSeparator;
};



class PLDAPStructBase;

class PLDAPAttributeBase : public PObject
{
    PCLASSINFO(PLDAPAttributeBase, PObject);
  public:
    PLDAPAttributeBase(const char * name, void * pointer, PINDEX size);

    const char * GetName() const { return name; }
    BOOL IsBinary() const { return pointer != NULL; }

    virtual void Copy(const PLDAPAttributeBase & other) = 0;

    virtual PString ToString() const;
    virtual void FromString(const PString & str);
    virtual PBYTEArray ToBinary() const;
    virtual void FromBinary(const PArray<PBYTEArray> & data);

  protected:
    const char * name;
    void       * pointer;
    PINDEX       size;
};


class PLDAPStructBase : public PObject {
    PCLASSINFO(PLDAPStructBase, PObject);
  protected:
    PLDAPStructBase();
    PLDAPStructBase & operator=(const PLDAPStructBase &);
    PLDAPStructBase & operator=(const PStringArray & array);
    PLDAPStructBase & operator=(const PStringToString & dict);
  private:
    PLDAPStructBase(const PLDAPStructBase &) { }

  public:
    void PrintOn(ostream & strm) const;

    PINDEX GetNumAttributes() const { return attributes.GetSize(); }
    PLDAPAttributeBase & GetAttribute(PINDEX idx) const { return attributes.GetDataAt(idx); }
    PLDAPAttributeBase * GetAttribute(const char * name) const { return attributes.GetAt(name); }

    void AddAttribute(PLDAPAttributeBase * var);
    static PLDAPStructBase & GetInitialiser() { return *PAssertNULL(initialiserInstance); }

  protected:
    void EndConstructor();

    PDictionary<PString, PLDAPAttributeBase> attributes;

    PLDAPStructBase        * initialiserStack;
    static PMutex            initialiserMutex;
    static PLDAPStructBase * initialiserInstance;
};

///////////////////////////////////////////////////////////////////////////

class PLDAPSchema : public PObject
{
  public:
    PLDAPSchema();

    enum AttributeType {
        AttibuteUnknown = -1,
        AttributeString,
        AttributeBinary,
        AttributeNumeric
    };

    class Attribute
    {
    public:
        Attribute() : m_type(AttibuteUnknown) { }
        Attribute(const PString & name, AttributeType type);
        PString       m_name;
        AttributeType m_type;
    };

    typedef std::list<Attribute> attributeList;

    static PLDAPSchema * CreateSchema(const PString & schemaname, PPluginManager * pluginMgr = NULL);
    static PStringList GetSchemaNames(PPluginManager * pluginMgr = NULL);
    static PStringList GetSchemaFriendlyNames(const PString & schema, PPluginManager * pluginMgr = NULL);

    void OnReceivedAttribute(const PString & attribute, const PString & value);

    void OnSendSchema(PList<PLDAPSession::ModAttrib> & attributes,
        PLDAPSession::ModAttrib::Operation op=PLDAPSession::ModAttrib::Add);

    void LoadSchema();

    PStringList SchemaName() { return PStringList(); }
    virtual void AttributeList(attributeList & /*attrib*/) {};


    PStringList GetAttributeList();
    BOOL Exists(const PString & attribute);

    BOOL SetAttribute(const PString & attribute, const PString & value);
    BOOL SetAttribute(const PString & attribute, const PBYTEArray & value);

    BOOL GetAttribute(const PString & attribute, PString & value);
    BOOL GetAttribute(const PString & attribute, PBYTEArray & value);

    AttributeType GetAttributeType(const PString & attribute);


  protected:
    typedef std::map<PString,PString> ldapAttributes;
    typedef std::map<PString,PBYTEArray> ldapBinAttributes;


    attributeList           attributelist;
    ldapAttributes          attributes;
    ldapBinAttributes       binattributes;   
};


template <class className> class LDAPPluginServiceDescriptor : public PDevicePluginServiceDescriptor
{
  public:
    virtual PObject *   CreateInstance(int /*userData*/) const { return new className; }
    virtual PStringList GetDeviceNames(int /*userData*/) const { return className::SchemaName(); } 
};

#define LDAP_Schema(name)    \
static LDAPPluginServiceDescriptor<##name##_schema> ##name##_schema_descriptor; \
PCREATE_PLUGIN(##name##_schema, PLDAPSchema, &##name##_schema_descriptor); \
PWLIB_STATIC_LOAD_PLUGIN(##name##_schema, PLDAPSchema); \

////////////////////////////////////////////////////////////////////////////////////////////

#define PLDAP_STRUCT_BEGIN(name) \
  class name : public PLDAPStructBase { \
    public: name() { EndConstructor(); } \
    public: name(const name & other) { EndConstructor(); operator=(other); } \
    public: name(const PStringArray & array) { EndConstructor(); operator=(array); } \
    public: name(const PStringToString & dict) { EndConstructor(); operator=(dict); } \
    public: name & operator=(const name & other) { PLDAPStructBase::operator=(other); return *this; } \
    public: name & operator=(const PStringArray & array) { PLDAPStructBase::operator=(array); return *this; } \
    public: name & operator=(const PStringToString & dict) { PLDAPStructBase::operator=(dict); return *this; } \
    PLDAP_ATTR_INIT(name, PString, objectClass, #name);

#define PLDAP_ATTRIBUTE(base, type, attribute, pointer, init) \
    public: type attribute; \
    private: struct PLDAPAttr_##attribute : public PLDAPAttributeBase { \
      PLDAPAttr_##attribute() \
        : PLDAPAttributeBase(#attribute, pointer, sizeof(type)), \
          instance(((base &)base::GetInitialiser()).attribute) \
        { init } \
      virtual void PrintOn (ostream & s) const { s << instance; } \
      virtual void ReadFrom(istream & s)       { s >> instance; } \
      virtual void Copy(const PLDAPAttributeBase & other) \
                    { instance = ((PLDAPAttr_##attribute &)other).instance; } \
      type & instance; \
    } pldapvar_##attribute

#define PLDAP_ATTR_SIMP(base, type, attribute) \
        PLDAP_ATTRIBUTE(base, type, attribute, NULL, ;)

#define PLDAP_ATTR_INIT(base, type, attribute, init) \
        PLDAP_ATTRIBUTE(base, type, attribute, NULL, instance = init;)

#define PLDAP_BINATTRIB(base, type, attribute) \
        PLDAP_ATTRIBUTE(base, type, attribute, &((base &)base::GetInitialiser()).attribute, ;)

#define PLDAP_STRUCT_END() \
  };

#endif // P_LDAP

#endif // _PLDAP_H


// End of file ////////////////////////////////////////////////////////////////
