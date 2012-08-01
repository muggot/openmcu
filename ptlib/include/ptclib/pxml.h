/*
 * pxml.h
 *
 * XML parser support
 *
 * Portable Windows Library
 *
 * Copyright (c) 2002 Equivalence Pty. Ltd.
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
 * $Log: pxml.h,v $
 * Revision 1.28  2007/05/16 03:34:37  csoutheren
 * Add support for default expat installation on MacOSX
 *
 * Revision 1.27  2007/04/18 23:49:50  csoutheren
 * Add usage of precompiled headers
 *
 * Revision 1.26  2005/11/30 12:47:37  csoutheren
 * Removed tabs, reformatted some code, and changed tags for Doxygen
 *
 * Revision 1.25  2005/08/04 20:10:24  csoutheren
 * Apply patch #1217596
 * Fixed problems with MacOSX Tiger
 * Thanks to Hannes Friederich
 *
 * Revision 1.24  2004/10/12 23:28:07  csoutheren
 * Fixed problem with bogus DOCTYPE being output
 *
 * Revision 1.23  2004/04/21 00:35:02  csoutheren
 * Added a stream parser for protocols like XMPP where each child of the root is to be considered a separate document/message.
 * Thanks to Federico Pinna and Reitek S.p.A.
 *
 * Revision 1.22  2003/04/27 23:54:13  craigs
 * Removed deprecated options
 *
 * Revision 1.21  2003/03/31 07:41:50  craigs
 * Fixed problem with accidental introduced dependency on expat.h
 *
 * Revision 1.20  2003/03/31 06:21:19  craigs
 * Split the expat wrapper from the XML file handling to allow reuse of the parser
 *
 * Revision 1.19  2003/01/13 02:14:02  robertj
 * Improved error logging for auto-loaded XML
 *
 * Revision 1.18  2002/12/16 06:38:24  robertj
 * Added ability to specify certain elemets (by name) that are exempt from
 *   the indent formatting. Useful for XML/RPC where leading white space is
 *   not ignored by all servers.
 *
 * Revision 1.17  2002/11/26 05:53:57  craigs
 * Added ability to auto-reload from URL
 *
 * Revision 1.16  2002/11/21 08:09:04  craigs
 * Changed to not overwrite XML data if load fails
 *
 * Revision 1.15  2002/11/19 07:37:38  craigs
 * Added locking functions and LoadURL function
 *
 * Revision 1.14  2002/11/06 22:47:24  robertj
 * Fixed header comment (copyright etc)
 *
 */

#ifndef _PXML_H
#define _PXML_H

#ifdef P_USE_PRAGMA
#pragma interface
#endif

#ifndef _PTLIB_H
#include <ptlib.h>
#endif

#include <ptbuildopts.h>

#if P_EXPAT

#include <ptclib/http.h>

////////////////////////////////////////////////////////////

class PXMLElement;
class PXMLData;

class PXMLParser : public PObject
{
  PCLASSINFO(PXMLParser, PObject);
  public:
    enum Options {
      Indent              = 1,
      NewLineAfterElement = 2,
      NoIgnoreWhiteSpace  = 4,   ///< ignored
      CloseExtended       = 8,   ///< ignored
      WithNS              = 16,
    };

    PXMLParser(int options = -1);
    ~PXMLParser();
    BOOL Parse(const char * data, int dataLen, BOOL final);
    void GetErrorInfo(PString & errorString, PINDEX & errorCol, PINDEX & errorLine);

    virtual void StartElement(const char * name, const char **attrs);
    virtual void EndElement(const char * name);
    virtual void AddCharacterData(const char * data, int len);
    virtual void XmlDecl(const char * version, const char * encoding, int standAlone);
    virtual void StartDocTypeDecl(const char * docTypeName,
                                  const char * sysid,
                                  const char * pubid,
                                  int hasInternalSubSet);
    virtual void EndDocTypeDecl();
    virtual void StartNamespaceDeclHandler(const char * prefix, const char * uri);
    virtual void EndNamespaceDeclHandler(const char * prefix);

    PString GetVersion() const  { return version; }
    PString GetEncoding() const { return encoding; }
    BOOL GetStandAlone() const  { return standAlone; }

    PXMLElement * GetXMLTree() const;
    PXMLElement * SetXMLTree(PXMLElement * newRoot);

  protected:
    int options;
    void * expat;
    PXMLElement * rootElement;
    PXMLElement * currentElement;
    PXMLData * lastElement;
    PString version, encoding;
    int standAlone;
};

class PXMLObject;
class PXMLElement;
class PXMLData;

////////////////////////////////////////////////////////////

class PXMLBase : public PObject
{
  public:
    PXMLBase(int _options = -1)
      : options(_options) { if (options < 0) options = 0; }

    void SetOptions(int _options)
      { options = _options; }

    int GetOptions() const { return options; }

    virtual BOOL IsNoIndentElement(
      const PString & /*elementName*/
    ) const
    {
      return FALSE;
    }

  protected:
    int options;
};


class PXML : public PXMLBase
{
  PCLASSINFO(PXML, PObject);
  public:

    PXML(
      int options = -1,
      const char * noIndentElements = NULL
    );
    PXML(
      const PString & data,
      int options = -1,
      const char * noIndentElements = NULL
    );

    PXML(const PXML & xml);

    ~PXML();

    BOOL IsDirty() const;

    BOOL Load(const PString & data, int options = -1);

    BOOL StartAutoReloadURL(const PURL & url, 
                            const PTimeInterval & timeout, 
                            const PTimeInterval & refreshTime,
                            int _options = -1);
    BOOL StopAutoReloadURL();
    PString GetAutoReloadStatus() { PWaitAndSignal m(autoLoadMutex); PString str = autoLoadError; return str; }
    BOOL AutoLoadURL();
    virtual void OnAutoLoad(BOOL ok);

    BOOL LoadURL(const PURL & url);
    BOOL LoadURL(const PURL & url, const PTimeInterval & timeout, int _options = -1);
    BOOL LoadFile(const PFilePath & fn, int options = -1);

    virtual void OnLoaded() { }

    BOOL Save(int options = -1);
    BOOL Save(PString & data, int options = -1);
    BOOL SaveFile(const PFilePath & fn, int options = -1);

    void RemoveAll();

    BOOL IsNoIndentElement(
      const PString & elementName
    ) const;

    void PrintOn(ostream & strm) const;

    PXMLElement * GetElement(const PCaselessString & name, PINDEX idx = 0) const;
    PXMLElement * GetElement(PINDEX idx) const;
    PINDEX        GetNumElements() const; 
    PXMLElement * GetRootElement() const { return rootElement; }
    PXMLElement * SetRootElement(PXMLElement * p);
    PXMLElement * SetRootElement(const PString & documentType);
    BOOL          RemoveElement(PINDEX idx);

    PCaselessString GetDocumentType() const;

    PString GetErrorString() const { return errorString; }
    PINDEX  GetErrorColumn() const { return errorCol; }
    PINDEX  GetErrorLine() const   { return errorLine; }

    PString GetDocType() const         { return docType; }
    void SetDocType(const PString & v) { docType = v; }

    PMutex & GetMutex() { return rootMutex; }

    PDECLARE_NOTIFIER(PTimer,  PXML, AutoReloadTimeout);
    PDECLARE_NOTIFIER(PThread, PXML, AutoReloadThread);

    // static methods to create XML tags
    static PString CreateStartTag (const PString & text);
    static PString CreateEndTag (const PString & text);
    static PString CreateTagNoData (const PString & text);
    static PString CreateTag (const PString & text, const PString & data);

  protected:
    void Construct(int options, const char * noIndentElements);
    PXMLElement * rootElement;
    PMutex rootMutex;

    BOOL loadFromFile;
    PFilePath loadFilename;
    PString version, encoding;
    int standAlone;

    PTimer autoLoadTimer;
    PURL autoloadURL;
    PTimeInterval autoLoadWaitTime;
    PMutex autoLoadMutex;
    PString autoLoadError;

    PString errorString;
    PINDEX errorCol;
    PINDEX errorLine;

    PSortedStringList noIndentElements;

    PString docType;
};

////////////////////////////////////////////////////////////

PARRAY(PXMLObjectArray, PXMLObject);

class PXMLObject : public PObject {
  PCLASSINFO(PXMLObject, PObject);
  public:
    PXMLObject(PXMLElement * _parent)
      : parent(_parent) { dirty = FALSE; }

    PXMLElement * GetParent()
      { return parent; }

    PXMLObject * GetNextObject();

    void SetParent(PXMLElement * newParent)
    { 
      PAssert(parent == NULL, "Cannot reparent PXMLElement");
      parent = newParent;
    }

    virtual void Output(ostream & strm, const PXMLBase & xml, int indent) const = 0;

    virtual BOOL IsElement() const = 0;

    void SetDirty();
    BOOL IsDirty() const { return dirty; }

    virtual PXMLObject * Clone(PXMLElement * parent) const = 0;

  protected:
    PXMLElement * parent;
    BOOL dirty;
};

////////////////////////////////////////////////////////////

class PXMLData : public PXMLObject {
  PCLASSINFO(PXMLData, PXMLObject);
  public:
    PXMLData(PXMLElement * _parent, const PString & data);
    PXMLData(PXMLElement * _parent, const char * data, int len);

    BOOL IsElement() const    { return FALSE; }

    void SetString(const PString & str, BOOL dirty = TRUE);

    PString GetString() const           { return value; }

    void Output(ostream & strm, const PXMLBase & xml, int indent) const;

    PXMLObject * Clone(PXMLElement * parent) const;

  protected:
    PString value;
};

////////////////////////////////////////////////////////////

class PXMLElement : public PXMLObject {
  PCLASSINFO(PXMLElement, PXMLObject);
  public:
    PXMLElement(PXMLElement * _parent, const char * name = NULL);
    PXMLElement(PXMLElement * _parent, const PString & name, const PString & data);

    BOOL IsElement() const { return TRUE; }

    void PrintOn(ostream & strm) const;
    void Output(ostream & strm, const PXMLBase & xml, int indent) const;

    PCaselessString GetName() const
      { return name; }

    void SetName(const PString & v)
      { name = v; }

    PINDEX GetSize() const
      { return subObjects.GetSize(); }

    PXMLObject  * AddSubObject(PXMLObject * elem, BOOL dirty = TRUE);

    PXMLElement * AddChild    (PXMLElement * elem, BOOL dirty = TRUE);
    PXMLData    * AddChild    (PXMLData    * elem, BOOL dirty = TRUE);

    void SetAttribute(const PCaselessString & key,
                      const PString & value,
                      BOOL setDirty = TRUE);

    PString GetAttribute(const PCaselessString & key) const;
    PString GetKeyAttribute(PINDEX idx) const;
    PString GetDataAttribute(PINDEX idx) const;
    BOOL HasAttribute(const PCaselessString & key);
    BOOL HasAttributes() const      { return attributes.GetSize() > 0; }
    PINDEX GetNumAttributes() const { return attributes.GetSize(); }

    PXMLElement * GetElement(const PCaselessString & name, PINDEX idx = 0) const;
    PXMLObject  * GetElement(PINDEX idx = 0) const;
    BOOL          RemoveElement(PINDEX idx);

    PINDEX FindObject(PXMLObject * ptr) const;

    BOOL HasSubObjects() const
      { return subObjects.GetSize() != 0; }

    PXMLObjectArray  GetSubObjects() const
      { return subObjects; }

    PString GetData() const;

    PXMLObject * Clone(PXMLElement * parent) const;

  protected:
    PCaselessString name;
    PStringToString attributes;
    PXMLObjectArray subObjects;
    BOOL dirty;
};

////////////////////////////////////////////////////////////

class PConfig;      // stupid gcc 4 does not recognize PConfig as a class

class PXMLSettings : public PXML
{
  PCLASSINFO(PXMLSettings, PXML);
  public:
    PXMLSettings(int options = PXMLParser::NewLineAfterElement);
    PXMLSettings(const PString & data, int options = PXMLParser::NewLineAfterElement);
    PXMLSettings(const PConfig & data, int options = PXMLParser::NewLineAfterElement);

    BOOL Load(const PString & data);
    BOOL LoadFile(const PFilePath & fn);

    BOOL Save();
    BOOL Save(PString & data);
    BOOL SaveFile(const PFilePath & fn);

    void SetAttribute(const PCaselessString & section, const PString & key, const PString & value);

    PString GetAttribute(const PCaselessString & section, const PString & key) const;
    BOOL    HasAttribute(const PCaselessString & section, const PString & key) const;

    void ToConfig(PConfig & cfg) const;
};

////////////////////////////////////////////////////////////

class PXMLStreamParser : public PXMLParser
{
  PCLASSINFO(PXMLStreamParser, PXMLParser);
  public:
    PXMLStreamParser();

    virtual void EndElement(const char * name);
    virtual PXML * Read(PChannel * channel);

  protected:
    BOOL rootOpen;
    PQueue<PXML> messages;
};

#endif // P_EXPAT

#endif
