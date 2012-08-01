/*
 * pxml.cxx
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
 * $Log: pxml.cxx,v $
 * Revision 1.48  2007/09/17 11:14:46  rjongbloed
 * Added "No Trace" build configuration.
 *
 * Revision 1.47  2007/09/08 11:34:28  rjongbloed
 * Improved memory checking (leaks etc), especially when using MSVC debug library.
 *
 * Revision 1.46  2007/04/23 01:15:24  csoutheren
 * Removed warnings on Windows
 *
 * Revision 1.45  2007/04/20 07:54:42  csoutheren
 * Applied 1703672 - PXML::PrintOn() prettier print
 * Thanks to Fabrizio Ammollo
 *
 * Revision 1.44  2007/04/10 05:08:48  rjongbloed
 * Fixed issue with use of static C string variables in DLL environment,
 *   must use functional interface for correct initialisation.
 *
 * Revision 1.43  2005/11/30 12:47:41  csoutheren
 * Removed tabs, reformatted some code, and changed tags for Doxygen
 *
 * Revision 1.42  2005/05/12 05:30:16  csoutheren
 * Ensured error location is initialised
 *
 * Revision 1.41  2004/10/23 10:58:15  ykiryanov
 * Added ifdef _WIN32_WCE for PocketPC 2003 SDK port
 *
 * Revision 1.40  2004/10/12 23:28:08  csoutheren
 * Fixed problem with bogus DOCTYPE being output
 *
 * Revision 1.39  2004/04/21 00:35:02  csoutheren
 * Added a stream parser for protocols like XMPP where each child of the root is to be considered a separate document/message.
 * Thanks to Federico Pinna and Reitek S.p.A.
 *
 * Revision 1.38  2004/04/09 06:52:17  rjongbloed
 * Removed #pargma linker command for /delayload of DLL as documentations sais that
 *   you cannot do this.
 *
 * Revision 1.37  2004/02/23 23:52:20  csoutheren
 * Added pragmas to avoid every Windows application needing to include libs explicitly
 *
 * Revision 1.36  2004/01/17 18:12:59  csoutheren
 * Changed to use PString::MakeEmpty
 *
 * Revision 1.35  2003/05/14 02:50:53  rjongbloed
 * Simplified name space initialisation
 *
 * Revision 1.34  2003/05/06 07:55:25  craigs
 * Fixed problem with initialising XML parser for namespaces
 *
 * Revision 1.33  2003/04/27 23:53:30  craigs
 * Removed deprecated options
 *
 * Revision 1.32  2003/04/16 08:00:19  robertj
 * Windoes psuedo autoconf support
 *
 * Revision 1.31  2003/04/08 12:47:07  craigs
 * Fixed problem with handling of CDATA
 *
 * Revision 1.30  2003/04/08 05:08:41  craigs
 * Fixed problems with additional spaces being included with metasequences
 *
 * Revision 1.29  2003/04/02 09:13:55  rogerh
 * Add type casts because the variable 'expat' is now a void *
 *
 * Revision 1.28  2003/03/31 06:20:56  craigs
 * Split the expat wrapper from the XML file handling to allow reuse of the parser
 *
 * Revision 1.27  2003/01/13 02:14:02  robertj
 * Improved error logging for auto-loaded XML
 *
 * Revision 1.26  2002/12/16 06:38:59  robertj
 * Added ability to specify certain elemets (by name) that are exempt from
 *   the indent formatting. Useful for XML/RPC where leading white space is
 *   not ignored by all servers.
 *
 * Revision 1.25  2002/12/10 04:41:16  robertj
 * Added test for URL being empty, don't try and run auto load in background.
 *
 * Revision 1.24  2002/11/26 05:53:45  craigs
 * Added ability to auto-reload from URL
 *
 * Revision 1.23  2002/11/21 08:08:52  craigs
 * Changed to not overwrite XML data if load fails
 *
 * Revision 1.22  2002/11/19 07:37:25  craigs
 * Added locking functions and LoadURL function
 *
 * Revision 1.21  2002/11/06 22:47:25  robertj
 * Fixed header comment (copyright etc)
 *
 */

// This depends on the expat XML library by Jim Clark
// See http://www.jclark.com/xml/expat.html for more information

#include <ptlib.h>

#ifdef __GNUC__
#pragma implementation "pxml.h"
#endif

#include <ptclib/pxml.h>

#if P_EXPAT

#include <expat.h>


#define new PNEW


#define CACHE_BUFFER_SIZE   1024
#define XMLSETTINGS_OPTIONS (NewLineAfterElement)


#ifdef _MSC_VER
#ifndef _WIN32_WCE
#pragma comment(lib, P_EXPAT_LIBRARY)
#endif // !_WIN32_WCE
#endif


////////////////////////////////////////////////////

static void PXML_StartElement(void * userData, const char * name, const char ** attrs)
{
  ((PXMLParser *)userData)->StartElement(name, attrs);
}

static void PXML_EndElement(void * userData, const char * name)
{
  ((PXMLParser *)userData)->EndElement(name);
}

static void PXML_CharacterDataHandler(void * userData, const char * data, int len)
{
  ((PXMLParser *)userData)->AddCharacterData(data, len);
}

static void PXML_XmlDeclHandler(void * userData, const char * version, const char * encoding, int standalone)
{
  ((PXMLParser *)userData)->XmlDecl(version, encoding, standalone);
}

static void PXML_StartDocTypeDecl(void * userData,
                const char * docTypeName,
                const char * sysid,
                const char * pubid,
                    int hasInternalSubSet)
{
  ((PXMLParser *)userData)->StartDocTypeDecl(docTypeName, sysid, pubid, hasInternalSubSet);
}

static void PXML_EndDocTypeDecl(void * userData)
{
  ((PXMLParser *)userData)->EndDocTypeDecl();
}

static void PXML_StartNamespaceDeclHandler(void *userData,
                                 const XML_Char *prefix,
                                 const XML_Char *uri)
{
  ((PXMLParser *)userData)->StartNamespaceDeclHandler(prefix, uri);
}

static void PXML_EndNamespaceDeclHandler(void *userData, const XML_Char *prefix)
{
  ((PXMLParser *)userData)->EndNamespaceDeclHandler(prefix);
}

PXMLParser::PXMLParser(int _options)
  : options(_options)
{
  if (options < 0)
    options = 0;

  if ((options & WithNS) != 0)
    expat = XML_ParserCreateNS(NULL, '|');
  else
    expat = XML_ParserCreate(NULL);

  XML_SetUserData((XML_Parser)expat, this);

  XML_SetElementHandler      ((XML_Parser)expat, PXML_StartElement, PXML_EndElement);
  XML_SetCharacterDataHandler((XML_Parser)expat, PXML_CharacterDataHandler);
  XML_SetXmlDeclHandler      ((XML_Parser)expat, PXML_XmlDeclHandler);
  XML_SetDoctypeDeclHandler  ((XML_Parser)expat, PXML_StartDocTypeDecl, PXML_EndDocTypeDecl);
  XML_SetNamespaceDeclHandler((XML_Parser)expat, PXML_StartNamespaceDeclHandler, PXML_EndNamespaceDeclHandler);

  rootElement = NULL;
  currentElement = NULL;
  lastElement    = NULL;
}

PXMLParser::~PXMLParser()
{
  XML_ParserFree((XML_Parser)expat);
}

PXMLElement * PXMLParser::GetXMLTree() const
{ 
  return rootElement; 
}

PXMLElement * PXMLParser::SetXMLTree(PXMLElement * newRoot)
{ 
  PXMLElement * oldRoot = rootElement;
  rootElement = newRoot; 
  return oldRoot;
}

BOOL PXMLParser::Parse(const char * data, int dataLen, BOOL final)
{
  return XML_Parse((XML_Parser)expat, data, dataLen, final) != 0;  
}

void PXMLParser::GetErrorInfo(PString & errorString, PINDEX & errorCol, PINDEX & errorLine)
{
  XML_Error err = XML_GetErrorCode((XML_Parser)expat);
  errorString = PString(XML_ErrorString(err));
  errorCol    = XML_GetCurrentColumnNumber((XML_Parser)expat);
  errorLine   = XML_GetCurrentLineNumber((XML_Parser)expat);
}

void PXMLParser::StartElement(const char * name, const char **attrs)
{
  PXMLElement * newElement = new PXMLElement(currentElement, name);
  if (currentElement != NULL)
    currentElement->AddSubObject(newElement, FALSE);

  while (attrs[0] != NULL) {
    newElement->SetAttribute(PString(attrs[0]), PString(attrs[1]));
    attrs += 2;
  }

  currentElement = newElement;
  lastElement    = NULL;

  if (rootElement == NULL)
    rootElement = currentElement;
}

void PXMLParser::EndElement(const char * /*name*/)
{
  currentElement = currentElement->GetParent();
  lastElement    = NULL;
}

void PXMLParser::AddCharacterData(const char * data, int len)
{
  PString str(data, len);

  if (lastElement != NULL) {
    PAssert(!lastElement->IsElement(), "lastElement set by non-data element");
    lastElement->SetString(lastElement->GetString() + str, FALSE);
  } else {
    PXMLData * newElement = new PXMLData(currentElement, str);
    if (currentElement != NULL)
      currentElement->AddSubObject(newElement, FALSE);
    lastElement = newElement;
  } 
}


void PXMLParser::XmlDecl(const char * _version, const char * _encoding, int _standAlone)
{
  version    = _version;
  encoding   = _encoding;
  standAlone = _standAlone;
}

void PXMLParser::StartDocTypeDecl(const char * /*docTypeName*/,
                                  const char * /*sysid*/,
                                  const char * /*pubid*/,
                                  int /*hasInternalSubSet*/)
{
}

void PXMLParser::EndDocTypeDecl()
{
}

void PXMLParser::StartNamespaceDeclHandler(const XML_Char * /*prefix*/, 
                                           const XML_Char * /*uri*/)
{
}

void PXMLParser::EndNamespaceDeclHandler(const XML_Char * /*prefix*/)
{
}


///////////////////////////////////////////////////////////////////////////////////////////////


PXML::PXML(int options, const char * noIndentElements)
  : PXMLBase(options) 
{
  Construct(options, noIndentElements);
}

PXML::PXML(const PString & data, int options, const char * noIndentElements)
  : PXMLBase(options) 
{
  Construct(options, noIndentElements);
  Load(data);
}

PXML::~PXML()
{
  autoLoadTimer.Stop();
  RemoveAll();
}

PXML::PXML(const PXML & xml)
  : noIndentElements(xml.noIndentElements)
{
  Construct(xml.options, NULL);

  loadFromFile = xml.loadFromFile;
  loadFilename = xml.loadFilename;
  version      = xml.version;
  encoding     = xml.encoding;
  standAlone   = xml.standAlone;

  PWaitAndSignal m(xml.rootMutex);

  PXMLElement * oldRootElement = xml.rootElement;
  if (oldRootElement != NULL)
    rootElement = (PXMLElement *)oldRootElement->Clone(NULL);
}

void PXML::Construct(int _options, const char * _noIndentElements)
{
  rootElement    = NULL;
  options        = _options > 0 ? _options : 0;
  loadFromFile   = FALSE;
  standAlone     = -2;
  errorCol       = 0;
  errorLine      = 0;

  if (_noIndentElements != NULL)
    noIndentElements = PString(_noIndentElements).Tokenise(' ', FALSE);
}

PXMLElement * PXML::SetRootElement(const PString & documentType)
{
  PWaitAndSignal m(rootMutex);

  if (rootElement != NULL)
    delete rootElement;

  rootElement = new PXMLElement(rootElement, documentType);

  return rootElement;
}

PXMLElement * PXML::SetRootElement(PXMLElement * element)
{
  PWaitAndSignal m(rootMutex);

  if (rootElement != NULL)
    delete rootElement;

  rootElement = element;

  return rootElement;
}

BOOL PXML::IsDirty() const
{
  PWaitAndSignal m(rootMutex);

  if (rootElement == NULL)
    return FALSE;

  return rootElement->IsDirty();
}

PCaselessString PXML::GetDocumentType() const
{ 
  PWaitAndSignal m(rootMutex);

  if (rootElement == NULL)
    return PCaselessString();
  return rootElement->GetName();
}

BOOL PXML::LoadFile(const PFilePath & fn, int _options)
{
  PTRACE(4, "XML\tLoading file " << fn);

  PWaitAndSignal m(rootMutex);

  if (_options >= 0)
    options = _options;

  loadFilename = fn;
  loadFromFile = TRUE;

  PFile file;
  if (!file.Open(fn, PFile::ReadOnly)) {
    errorString = "File open error" & file.GetErrorText();
    return FALSE;
  }

  off_t len = file.GetLength();
  PString data;
  if (!file.Read(data.GetPointer(len + 1), len)) {
    errorString = "File read error" & file.GetErrorText();
    return FALSE;
  }

  data[(PINDEX)len] = '\0';

  return Load(data);
}

BOOL PXML::LoadURL(const PURL & url)
{
  return LoadURL(url, PMaxTimeInterval, -1);
}


BOOL PXML::LoadURL(const PURL & url, const PTimeInterval & timeout, int _options)
{
  if (url.IsEmpty()) {
    errorString = "Cannot load empty URL";
    errorCol = errorLine = 0;
    return FALSE;
  }

  PTRACE(4, "XML\tLoading URL " << url);

  PString data;
  if (url.GetScheme() == "file") 
    return LoadFile(url.AsFilePath());

  PHTTPClient client;
  PINDEX contentLength;
  PMIMEInfo outMIME, replyMIME;

  // make sure we do not hang around for ever
  client.SetReadTimeout(timeout);

  // get the resource header information
  if (!client.GetDocument(url, outMIME, replyMIME)) {
    errorString = PString("Cannot load URL") & url.AsString();
    errorCol = errorLine = 0;
    return FALSE;
  }

  // get the length of the data
  if (!replyMIME.Contains(PHTTPClient::ContentLengthTag()))
    contentLength = (PINDEX)replyMIME[PHTTPClient::ContentLengthTag()].AsUnsigned();
  else
    contentLength = P_MAX_INDEX;

  // download the resource into memory
  PINDEX offs = 0;
  for (;;) {
    PINDEX len;
    if (contentLength == P_MAX_INDEX)
      len = CACHE_BUFFER_SIZE;
    else if (offs == contentLength)
      break;
    else
      len = PMIN(contentLength = offs, CACHE_BUFFER_SIZE);

    if (!client.Read(offs + data.GetPointer(offs + len), len))
      break;

    len = client.GetLastReadCount();

    offs += len;
  }

  return Load(data, _options);
}

BOOL PXML::StartAutoReloadURL(const PURL & url, 
                              const PTimeInterval & timeout, 
                              const PTimeInterval & refreshTime,
                              int _options)
{
  if (url.IsEmpty()) {
    autoLoadError = "Cannot auto-load empty URL";
    return FALSE;
  }

  PWaitAndSignal m(autoLoadMutex);
  autoLoadTimer.Stop();

  SetOptions(_options);
  autoloadURL      = url;
  autoLoadWaitTime = timeout;
  autoLoadError.MakeEmpty();
  autoLoadTimer.SetNotifier(PCREATE_NOTIFIER(AutoReloadTimeout));

  BOOL stat = AutoLoadURL();

  autoLoadTimer = refreshTime;

  return stat;
}

void PXML::AutoReloadTimeout(PTimer &, INT)
{
  PThread::Create(PCREATE_NOTIFIER(AutoReloadThread), PThread::AutoDeleteThread);
}

void PXML::AutoReloadThread(PThread &, INT)
{
  PWaitAndSignal m(autoLoadMutex);
  OnAutoLoad(AutoLoadURL());
  autoLoadTimer.Reset();
}

void PXML::OnAutoLoad(BOOL PTRACE_PARAM(ok))
{
  PTRACE_IF(3, !ok, "XML\tFailed to load XML: " << GetErrorString());
}

BOOL PXML::AutoLoadURL()
{
  BOOL stat = LoadURL(autoloadURL, autoLoadWaitTime);
  if (stat)
    autoLoadError.MakeEmpty();
  else 
    autoLoadError = GetErrorString() + psprintf(" at line %i, column %i", GetErrorLine(), GetErrorColumn());
  return stat;
}

BOOL PXML::StopAutoReloadURL()
{
  PWaitAndSignal m(autoLoadMutex);
  autoLoadTimer.Stop();
  return TRUE;
}


BOOL PXML::Load(const PString & data, int _options)
{
  if (_options >= 0)
    options = _options;

  BOOL stat = FALSE;
  PXMLElement * loadingRootElement = NULL;

  {
    PXMLParser parser(options);
    int done = 1;
    stat = parser.Parse(data, data.GetLength(), done) != 0;
  
    if (!stat)
      parser.GetErrorInfo(errorString, errorCol, errorLine);

    version    = parser.GetVersion();
    encoding   = parser.GetEncoding();
    standAlone = parser.GetStandAlone();

    loadingRootElement = parser.GetXMLTree();
  }

  if (stat) {
    if (loadingRootElement == NULL) {
      errorString = "XML\tFailed to create root node in XML!";
      return FALSE;
    }
    else {
      PWaitAndSignal m(rootMutex);
      if (rootElement != NULL) {
        delete rootElement;
        rootElement = NULL;
      }
      rootElement = loadingRootElement;
      PTRACE(4, "XML\tLoaded XML " << rootElement->GetName());
    }
    OnLoaded();
  }

  return stat;
}

BOOL PXML::Save(int _options)
{
  if (_options >= 0)
    options = _options;

  if (!loadFromFile || !IsDirty())
    return FALSE;

  return SaveFile(loadFilename);
}

BOOL PXML::SaveFile(const PFilePath & fn, int _options)
{
  PWaitAndSignal m(rootMutex);

  PFile file;
  if (!file.Open(fn, PFile::WriteOnly)) 
    return FALSE;

  PString data;
  if (!Save(data, _options))
    return FALSE;

  return file.Write((const char *)data, data.GetLength());
}

BOOL PXML::Save(PString & data, int _options)
{
  PWaitAndSignal m(rootMutex);

  if (_options >= 0)
    options = _options;

  PStringStream strm;
  strm << *this;
  data = strm;
  return TRUE;
}

void PXML::RemoveAll()
{
  PWaitAndSignal m(rootMutex);

  if (rootElement != NULL) {
    delete rootElement;
    rootElement = NULL;
  }
}

PXMLElement * PXML::GetElement(const PCaselessString & name, PINDEX idx) const
{
  if (rootElement == NULL)
    return NULL;

  return rootElement->GetElement(name, idx);
}

PXMLElement * PXML::GetElement(PINDEX idx) const
{
  if (rootElement == NULL)
    return NULL;
  if (idx >= rootElement->GetSize())
    return NULL;

  return (PXMLElement *)(rootElement->GetElement(idx));
}

BOOL PXML::RemoveElement(PINDEX idx)
{
  if (rootElement == NULL)
    return FALSE;

  if (idx >= rootElement->GetSize())
    return FALSE;

  rootElement->RemoveElement(idx);
  return TRUE;
}


PINDEX PXML::GetNumElements() const
{
  if (rootElement == NULL) 
    return 0;
  else 
    return rootElement->GetSize();
}

BOOL PXML::IsNoIndentElement(const PString & elementName) const
{
  return noIndentElements.GetValuesIndex(elementName) != P_MAX_INDEX;
}


void PXML::PrintOn(ostream & strm) const
{
  //BOOL newLine = (options & (PXMLParser::Indent|PXMLParser::NewLineAfterElement)) != 0;

//<?xml version="1.0" encoding="UTF-8" standalone="yes"?>

  PString ver = version;
  PString enc = encoding;
  int salone = standAlone;

  if (ver.IsEmpty())
    ver= "1.0";
  if (enc.IsEmpty())
    enc = "UTF-8";
  if (salone == -2)
    salone = -1;

  strm << "<?xml version=\"" << ver << "\" encoding=\"" << enc << "\"";
  switch (salone) {
    case 0:
      strm << " standalone=\"no\"";
      break;
    case 1:
      strm << " standalone=\"yes\"";
      break;
    default:
      break;
  }

  strm << "?>" << endl;

  if (rootElement != NULL) {
    if (!docType.IsEmpty())
      strm << "<!DOCTYPE " << docType << '>' << endl;

    rootElement->Output(strm, *this, 2);
  }
}

PString PXML::CreateStartTag(const PString & text)
{
  return '<' + text + '>';
}


PString PXML::CreateEndTag(const PString & text)
{
  return "</" + text + '>';
}


PString PXML::CreateTagNoData(const PString & text)
{
  return '<' + text + "/>";
}


PString PXML::CreateTag(const PString & text, const PString & data)
{
  return CreateStartTag(text) + data + CreateEndTag(text);
}


///////////////////////////////////////////////////////
//
void PXMLObject::SetDirty()
{
  dirty = TRUE;
  if (parent != NULL)
    parent->SetDirty();
}

PXMLObject * PXMLObject::GetNextObject()
{
  if (parent == NULL)
    return NULL;

  // find our index in our parent's list
  PINDEX idx = parent->FindObject(this);
  if (idx == P_MAX_INDEX)
    return NULL;

  // get the next object
  ++idx;
  if (idx >= parent->GetSize())
    return NULL;

  return (*parent).GetElement(idx);
}

///////////////////////////////////////////////////////

PXMLData::PXMLData(PXMLElement * _parent, const PString & _value)
 : PXMLObject(_parent)
{
  value = _value;
}

PXMLData::PXMLData(PXMLElement * _parent, const char * data, int len)
 : PXMLObject(_parent)
{
  value = PString(data, len);
}

void PXMLData::Output(ostream & strm, const PXMLBase & xml, int indent) const
{
  int options = xml.GetOptions();
  if (xml.IsNoIndentElement(parent->GetName()))
    options &= ~PXMLParser::Indent;

  if (options & PXMLParser::Indent)
    strm << setw(indent-1) << " ";

  strm << value;

  if ((options & (PXMLParser::Indent|PXMLParser::NewLineAfterElement)) != 0)
    strm << endl;
}

void PXMLData::SetString(const PString & str, BOOL setDirty)
{
  value = str;
  if (setDirty)
    SetDirty();
}

PXMLObject * PXMLData::Clone(PXMLElement * _parent) const
{
  return new PXMLData(_parent, value);
}

///////////////////////////////////////////////////////

PXMLElement::PXMLElement(PXMLElement * _parent, const char * _name)
 : PXMLObject(_parent)
{
  dirty = FALSE;
  if (_name != NULL)
    name = _name;
}

PXMLElement::PXMLElement(PXMLElement * _parent, const PString & _name, const PString & data)
 : PXMLObject(_parent), name(_name)
{
  dirty = FALSE;
  AddSubObject(new PXMLData(this, data));
}

PINDEX PXMLElement::FindObject(PXMLObject * ptr) const
{
  return subObjects.GetObjectsIndex(ptr);
}


PXMLElement * PXMLElement::GetElement(const PCaselessString & name, PINDEX start) const
{
  PINDEX idx;
  PINDEX size = subObjects.GetSize();
  PINDEX count = 0;
  for (idx = 0; idx < size; idx++) {
    if (subObjects[idx].IsElement()) {
      PXMLElement & subElement = ((PXMLElement &)subObjects[idx]);
      if (subElement.GetName() *= name) {
        if (count++ == start)
          return (PXMLElement *)&subObjects[idx];
      }
    }
  }
  return NULL;
}

PXMLObject * PXMLElement::GetElement(PINDEX idx) const
{
  if (idx >= subObjects.GetSize())
    return NULL;

  return &subObjects[idx];
}

BOOL PXMLElement::RemoveElement(PINDEX idx)
{
  if (idx >= subObjects.GetSize())
    return FALSE;

  subObjects.RemoveAt(idx);
  return TRUE;
}


PString PXMLElement::GetAttribute(const PCaselessString & key) const
{
  return attributes(key);
}

PString PXMLElement::GetKeyAttribute(PINDEX idx) const
{
  if (idx < attributes.GetSize())
    return attributes.GetKeyAt(idx);
  else
    return PString();
}

PString PXMLElement::GetDataAttribute(PINDEX idx) const
{
  if (idx < attributes.GetSize())
    return attributes.GetDataAt(idx);
  else
    return PString();
}

void PXMLElement::SetAttribute(const PCaselessString & key,
                           const PString & value,
                          BOOL setDirty)
{
  attributes.SetAt(key, value);
  if (setDirty)
    SetDirty();
}

BOOL PXMLElement::HasAttribute(const PCaselessString & key)
{
  return attributes.Contains(key);
}

void PXMLElement::PrintOn(ostream & strm) const
{
  PXMLBase xml(-1);
  Output(strm, xml, 0);
}

void PXMLElement::Output(ostream & strm, const PXMLBase & xml, int indent) const
{
  int options = xml.GetOptions();

  BOOL newLine = (options & (PXMLParser::Indent|PXMLParser::NewLineAfterElement)) != 0;

  if ((options & PXMLParser::Indent) != 0)
    strm << setw(indent-1) << " ";

  strm << '<' << name;

  PINDEX i;
  if (attributes.GetSize() > 0) {
    for (i = 0; i < attributes.GetSize(); i++) {
      PCaselessString key = attributes.GetKeyAt(i);
      strm << ' ' << key << "=\"" << attributes[key] << '"';
    }
  }

  // this ensures empty elements use the shortened form
  if (subObjects.GetSize() == 0) {
    strm << "/>";
    if (newLine)
      strm << endl;
  }
  else {
    BOOL indenting = (options & PXMLParser::Indent) != 0 && !xml.IsNoIndentElement(name);

    strm << '>';
    if (indenting)
      strm << endl;
  
    for (i = 0; i < subObjects.GetSize(); i++) 
      subObjects[i].Output(strm, xml, indent + 2);

    if (indenting)
      strm << setw(indent-1) << " ";

    strm << "</" << name << '>';
    if (newLine)
      strm << endl;
  }
}

PXMLObject * PXMLElement::AddSubObject(PXMLObject * elem, BOOL setDirty)
{
  subObjects.SetAt(subObjects.GetSize(), elem);
  if (setDirty)
    SetDirty();

  return elem;
}

PXMLElement * PXMLElement::AddChild(PXMLElement * elem, BOOL dirty)
{
  return (PXMLElement *)AddSubObject(elem, dirty);
}

PXMLData * PXMLElement::AddChild(PXMLData * elem, BOOL dirty)
{
  return (PXMLData *)AddSubObject(elem, dirty);
}

PXMLObject * PXMLElement::Clone(PXMLElement * _parent) const
{
  PXMLElement * elem = new PXMLElement(_parent);

  elem->SetName(name);
  elem->attributes = attributes;
  elem->dirty      = dirty;

  PINDEX idx;
  for (idx = 0; idx < subObjects.GetSize(); idx++)
    elem->AddSubObject(subObjects[idx].Clone(elem), FALSE);

  return elem;
}

PString PXMLElement::GetData() const
{
  PString str;
  PINDEX idx;
  for (idx = 0; idx < subObjects.GetSize(); idx++) {
    if (!subObjects[idx].IsElement()) {
      PXMLData & dataElement = ((PXMLData &)subObjects[idx]);
      PStringArray lines = dataElement.GetString().Lines();
      PINDEX j;
      for (j = 0; j < lines.GetSize(); j++)
        str = str & lines[j];
    }
  }
  return str;
}


///////////////////////////////////////////////////////

PXMLSettings::PXMLSettings(int options)
  :PXML(options)
{
}

PXMLSettings::PXMLSettings(const PString & data, int options)
  : PXML(data,options) 
{
}

PXMLSettings::PXMLSettings(const PConfig & data, int options)
  : PXML(options) 
{
  PStringList sects = data.GetSections();

  for (PINDEX i = 0;i < (PINDEX)sects.GetSize();++i) {
    PStringToString keyvals = data.GetAllKeyValues(sects[i]);
    for (PINDEX j = 0; j < (PINDEX)keyvals.GetSize(); ++j) {
      SetAttribute(sects[i],keyvals.GetKeyAt(j),keyvals.GetDataAt(j));
    }
  }
}

BOOL PXMLSettings::Load(const PString & data)
{
  return PXML::Load(data);
}

BOOL PXMLSettings::LoadFile(const PFilePath & fn)
{
  return PXML::LoadFile(fn);
}

BOOL PXMLSettings::Save()
{
  return PXML::Save();
}

BOOL PXMLSettings::Save(PString & data)
{
  return PXML::Save(data);
}

BOOL PXMLSettings::SaveFile(const PFilePath & fn)
{
  return PXML::SaveFile(fn);
}

PString PXMLSettings::GetAttribute(const PCaselessString & section, const PString & key) const
{
  if (rootElement == NULL)
    return PString();

  PXMLElement * element = rootElement->GetElement(section);
  if (element == NULL)
    return PString();

  return element->GetAttribute(key);
}

void PXMLSettings::SetAttribute(const PCaselessString & section, const PString & key, const PString & value)
{
  if (rootElement == NULL) 
    rootElement = new PXMLElement(NULL, "settings");

  PXMLElement * element = rootElement->GetElement(section);
  if (element == NULL) {
    element = new PXMLElement(rootElement, section);
    rootElement->AddSubObject(element);
  }
  element->SetAttribute(key, value);
}

BOOL PXMLSettings::HasAttribute(const PCaselessString & section, const PString & key) const
{
  if (rootElement == NULL)
    return FALSE;

  PXMLElement * element = rootElement->GetElement(section);
  if (element == NULL)
    return FALSE;

  return element->HasAttribute(key);
}

void PXMLSettings::ToConfig(PConfig & cfg) const
{
  for (PINDEX i = 0;i < (PINDEX)GetNumElements();++i) {
    PXMLElement * el = GetElement(i);
    PString sectionName = el->GetName();
    for (PINDEX j = 0; j < (PINDEX)el->GetNumAttributes(); ++j) {
      PString key = el->GetKeyAttribute(j);
      PString dat = el->GetDataAttribute(j);
      if (!key && !dat)
        cfg.SetString(sectionName, key, dat);
    }
  }
}

///////////////////////////////////////////////////////

PXMLStreamParser::PXMLStreamParser() :
  rootOpen(TRUE)
{
}


void PXMLStreamParser::EndElement(const char * name)
{
  PXMLElement * element = currentElement;

  PXMLParser::EndElement(name);

  if (currentElement == rootElement) {
      if (element == rootElement) { // stream closed
        rootOpen = FALSE;
      }
      else {
        PINDEX i = rootElement->FindObject(element);

        if (i != P_MAX_INDEX) {
          PXML tmp;
          element = (PXMLElement *)element->Clone(0);
          rootElement->RemoveElement(i);

          PXML * msg = new PXML;
          msg->SetRootElement(element);
          messages.Enqueue(msg);
        }
     }
  }
}


PXML * PXMLStreamParser::Read(PChannel * channel)
{
  char buf[256];

  channel->SetReadTimeout(1000);

  while (rootOpen) {
    if (messages.GetSize() != 0)
      return messages.Dequeue();

    if (!channel->Read(buf, sizeof(buf) - 1) || !channel->IsOpen())
      return 0;

    buf[channel->GetLastReadCount()] = 0;

    if (!Parse(buf, channel->GetLastReadCount(), FALSE))
      return 0;
  }

  channel->Close();
  return 0;
}

///////////////////////////////////////////////////////

#endif 

