/*
 * main.cxx
 *
 * PWLib application source file for asnparser
 *
 * ASN.1 compiler to produce C++ classes.
 *
 * Copyright (c) 1997-1999 Equivalence Pty. Ltd.
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
 * The Original Code is ASN Parser.
 *
 * The Initial Developer of the Original Code is Equivalence Pty. Ltd.
 *
 * Portions of this code were written with the assisance of funding from
 * Vovida Networks, Inc. http://www.vovida.com.
 *
 * Portions are Copyright (C) 1993 Free Software Foundation, Inc.
 * All Rights Reserved.
 *
 * Contributor(s): ______________________________________.
 *
 * $Log: main.cxx,v $
 * Revision 1.55  2006/09/07 04:01:21  csoutheren
 * Fixed compilation with latest include files
 *
 * Revision 1.54  2005/06/07 06:25:53  csoutheren
 * Applied patch 1199897 to increase speed of ASN parser debugging output
 * Thanks to Dmitriy <ddv@abinet.com>
 *
 * Revision 1.53  2005/03/08 03:48:06  csoutheren
 * Fixed problem with incorrect parameter to PIsDescendant in generated code
 *
 * Revision 1.52  2004/08/13 00:18:34  csoutheren
 * Fixed problem with indent when outputting single elements
 *
 * Revision 1.51  2004/04/25 08:58:58  rjongbloed
 * Fixed GCC 3.4 warning
 *
 * Revision 1.50  2004/04/21 00:32:02  csoutheren
 * Fixed problem with XER and the new RTTI system
 * Thanks to Federico Pinna and Reitek S.p.A.
 *
 * Revision 1.49  2004/04/13 11:33:55  csoutheren
 * Fixed XER output, thanks to Federico Pinna
 *
 * Revision 1.48  2004/04/03 08:22:23  csoutheren
 * Remove pseudo-RTTI and replaced with real RTTI
 *
 * Revision 1.47  2004/02/17 10:24:04  rjongbloed
 * Updated version number so will rebuild ASN files.
 *
 * Revision 1.46  2004/02/17 09:38:24  csoutheren
 * Added change to remove duplicate forward declarations
 * See SourceForge bug 832245
 * Thanks to Vyacheslav E. Andrejev
 *
 * Revision 1.45  2003/10/03 00:13:04  rjongbloed
 * Added ability to specify CHOICE field selection by function rather than operator as the operator technique does not work with some dumb compilers.
 * Added ability to specify that the header file name be different from the module name and module prefix string.
 *
 * Revision 1.44  2003/02/27 04:05:30  robertj
 * Added ability to have alternate directories for header file
 *   includes in generated C++ code.
 * Added constructors to PASN_OctetString descendant classes to help
 *   with doing simple assignments.
 *
 * Revision 1.43  2003/02/26 01:57:44  robertj
 * Added XML encoding rules to ASN system, thanks Federico Pinna
 *
 * Revision 1.42  2003/02/19 14:18:55  craigs
 * Fixed ifdef problem with multipart cxx files
 *
 * Revision 1.41  2003/02/18 10:50:41  craigs
 * Added minor optimisation of outputted ASN code
 * Added automatic insertion of defines to allow disabling of generated code
 *
 * Revision 1.40  2002/11/27 11:42:52  robertj
 * Rearranged code to avoid GNU compiler problem.
 * Changed new classheader parameters to be full C literal like string for
 *   improved backslash conversion.
 * Incremented version number.
 *
 * Revision 1.39  2002/11/26 11:39:10  craigs
 * Added option to allow adding functions to generated header files
 *
 * Revision 1.38  2002/09/16 01:08:59  robertj
 * Added #define so can select if #pragma interface/implementation is used on
 *   platform basis (eg MacOS) rather than compiler, thanks Robert Monaghan.
 *
 * Revision 1.37  2001/10/02 00:56:14  robertj
 * Fixed problem with autonumering enumerated types.
 *
 * Revision 1.36  2001/08/06 01:39:02  robertj
 * Added assignement operator with RHS of PASN_BMPString to classes
 *   descended from PASN_BMPString.
 *
 * Revision 1.35  2001/08/03 09:01:02  robertj
 * Added assignement operator with RHS of PWORDArray to classes
 *   descended from PASN_BMPString.
 *
 * Revision 1.34  2001/06/14 02:09:20  robertj
 * Corrected support for ASN object class type reference constructs
 *   ie TYPE-IDENTIFIER.&Type encoded as octet string.
 *
 * Revision 1.33  2001/04/26 08:15:58  robertj
 * Fixed problem with ASN compile of single constraints on enumerations.
 *
 * Revision 1.32  2001/04/23 04:40:14  robertj
 * Added ASN standard types GeneralizedTime and UTCTime
 *
 * Revision 1.31  2001/02/22 23:31:42  robertj
 * Fixed display of version number just added.
 *
 * Revision 1.30  2001/02/22 22:31:01  robertj
 * Added command line flag to display version number only.
 *
 * Revision 1.29  2000/10/12 23:11:48  robertj
 * Fixed problem with BER encoding of ASN with optional fields.
 *
 * Revision 1.28  2000/06/26 13:14:46  robertj
 * Nucleus++ port.
 *
 * Revision 1.27  2000/03/21 21:23:23  robertj
 * Added option to rename imported module names, allows include filename matching.
 *
 * Revision 1.26  2000/01/19 12:33:07  robertj
 * Fixed parsing of OID's in IMPORTS section.
 *
 * Revision 1.25  2000/01/19 03:38:12  robertj
 * Fixed support for parsing multiple IMPORTS
 *
 * Revision 1.24  1999/09/18 04:17:40  robertj
 * Added generation of C++ inlines for some  functions.
 * Optimised CreateObject() switch statements, collapsing common cases.
 *
 * Revision 1.23  1999/09/18 02:42:27  craigs
 * Added optimisation to collapse switch arms in CreateObject functions
 *
 * Revision 1.22  1999/09/07 09:56:04  robertj
 * Fixed failure to put "using anmespace" in every generated .cxx file.
 *
 * Revision 1.21  1999/08/28 01:48:55  robertj
 * Fixed anomaly to always include non-optional extensions in encodings.
 *
 * Revision 1.20  1999/08/09 13:02:36  robertj
 * Added ASN compiler #defines for backward support of pre GCC 2.9 compilers.
 * Added ASN compiler #defines to reduce its memory footprint.
 * Added ASN compiler code generation of assignment operators for string classes.
 *
 * Revision 1.19  1999/07/22 06:48:55  robertj
 * Added comparison operation to base ASN classes and compiled ASN code.
 * Added support for ANY type in ASN parser.
 *
 * Revision 1.18  1999/07/06 05:00:26  robertj
 * Incremented release number
 *
 * Revision 1.17  1999/07/01 12:21:46  robertj
 * Changed PASN_Choice cast operators so no longer "break" const-ness of object.
 *
 * Revision 1.16  1999/06/30 08:57:19  robertj
 * Fixed bug in encodeing sequence of constrained primitive type. Constraint not set.
 * Fixed bug in not emitting namespace use clause.
 * Added "normalisation" of separate sequence of <base type> to be single class.
 *
 * Revision 1.15  1999/06/14 13:00:15  robertj
 * Fixed bug in code generation for string constraints.
 *
 * Revision 1.14  1999/06/09 06:58:09  robertj
 * Adjusted heading comments.
 *
 * Revision 1.13  1999/06/09 02:07:49  robertj
 * Fixed backward compatibility of generated template code with G++ 2.7.x
 *
 * Revision 1.12  1999/06/07 01:56:25  robertj
 * Added header comment on license.
 *
 */

#include <ptlib.h>

#include <ptlib/pprocess.h>

#include "main.h"
#include "asn_grammar.h"


#define MAJOR_VERSION 1
#define MINOR_VERSION 9
#define BUILD_TYPE    ReleaseCode
#define BUILD_NUMBER 4


unsigned lineNumber;
PString  fileName;

unsigned fatals, warnings;

extern FILE * yyin;
extern int yydebug;
extern int LexEcho;

ModuleDefinition * Module;


static const char * UniversalTagClassNames[] = {
  "UniversalTagClass",
  "ApplicationTagClass",
  "ContextSpecificTagClass",
  "PrivateTagClass"
};

static const char * UniversalTagNames[] = {
  NULL,
  "UniversalBoolean",
  "UniversalInteger",
  "UniversalBitString",
  "UniversalOctetString",
  "UniversalNull",
  "UniversalObjectId",
  "UniversalObjectDescriptor",
  "UniversalExternalType",
  "UniversalReal",
  "UniversalEnumeration",
  "UniversalEmbeddedPDV",
  NULL,
  NULL,
  NULL,
  NULL,
  "UniversalSequence",
  "UniversalSet",
  "UniversalNumericString",
  "UniversalPrintableString",
  "UniversalTeletexString",
  "UniversalVideotexString",
  "UniversalIA5String",
  "UniversalUTCTime",
  "UniversalGeneralisedTime",
  "UniversalGraphicString",
  "UniversalVisibleString",
  "UniversalGeneralString",
  "UniversalUniversalString",
  NULL,
  "UniversalBMPString"
};


static const char * const StandardClasses[] = {
  "PASN_Null",
  "PASN_Boolean",
  "PASN_Integer",
  "PASN_Enumeration",
  "PASN_Real",
  "PASN_ObjectId",
  "PASN_BitString",
  "PASN_OctetString",
  "PASN_NumericString",
  "PASN_PrintableString",
  "PASN_VisibleString",
  "PASN_IA5String",
  "PASN_GeneralString",
  "PASN_BMPString",
  "PASN_Sequence"
};


/////////////////////////////////////////
//
//  yyerror
//  required function for flex
//

void yyerror(char * str)
{
  extern char * yytext;
  PError << StdError(Fatal) << str << " near token \"" << yytext <<"\"\n";
}

ostream & operator<<(ostream & out, const StdError & e)
{
  out << fileName << '(' << lineNumber << ") : ";
  if (e.e == Fatal) {
    fatals++;
    out << "error";
  }
  else {
    warnings++;
    out << "warning";
  }
  return out << ": ";
}


/////////////////////////////////////////////////////////
//
//  Utility
//

static PString MakeIdentifierC(const PString & identifier)
{
  PString s = identifier;
  s.Replace("-", "_", TRUE);
  return s;
}


class OutputFile : public PTextFile
{
    PCLASSINFO(OutputFile, PTextFile);
  public:
    ~OutputFile() { Close(); }

    BOOL Open(const PFilePath & path, const PString & suffix, const char * extension);
    BOOL Close();
};


BOOL OutputFile::Open(const PFilePath & path,
                      const PString & suffix,
                      const char * extension)
{
  PFilePath fn = path.GetDirectory() + path.GetTitle() + suffix;
  fn.SetType(extension);

  if (PTextFile::Open(fn, WriteOnly))
    *this << "//\n"
             "// " << GetFilePath().GetFileName() << "\n"
             "//\n"
             "// Code automatically generated by asnparse.\n"
             "//\n"
             "\n";
  else
    PError << PProcess::Current().GetName() << ": cannot create \"" 
         << GetFilePath() << "\" :" << GetErrorText() << endl;

  return IsOpen();
}


BOOL OutputFile::Close()
{
  if (IsOpen())
    *this << "\n"
             "// End of " << GetFilePath().GetFileName() << '\n';

  return PTextFile::Close();
}


/////////////////////////////////////////////////////////
//
//  Application
//

class App : public PProcess
{
    PCLASSINFO(App, PProcess);
  public:
    App();
    void Main();
    BOOL SetClassHeaderFile(PArgList & args);
    BOOL SetClassHeader(PArgList & args);
    void OutputAdditionalHeaders(ostream & hdr, const PString & className);
  protected:
    PStringToString classToHeader;
};

PCREATE_PROCESS(App);

App::App()
  : PProcess("Equivalence", "ASNParse", MAJOR_VERSION, MINOR_VERSION, BUILD_TYPE, BUILD_NUMBER)
{
}

void App::Main()
{
  cout << GetName() << " version " << GetVersion(TRUE)
       << " for " << GetOSClass() << ' ' << GetOSName()
       << " by " << GetManufacturer() << endl;

  PArgList & args = GetArguments();
  args.Parse("c-c++."
             "d-debug."
             "e-echo."
             "h-hdr-prefix:"
             "i-inlines."
             "m-module:"
             "n-namespace."
             "o-output:"
             "r-rename:"
             "s-split;"
             "V-version."
             "v-verbose."
             "x-xml."
             "-no-operators."
             "-classheader:"
             "-classheaderfile:");

  if (args.HasOption('V'))
    return;

  unsigned numFiles = 1;
  if (args.HasOption('s')) {
    PString numFilesStr = args.GetOptionString('s');
    if (numFilesStr.IsEmpty())
      numFiles = 2;
    else
      numFiles = numFilesStr.AsUnsigned();
  }

  if (args.GetCount() < 1 || args.GetCount() > 1 || numFiles == 0) {
    PError << "usage: asnparse [options] asnfile\n"
              "  -V --version        Display version and exit\n"
              "  -v --verbose        Verbose output (multiple times for more verbose)\n"
              "  -e --echo           Echo input file\n"
              "  -d --debug          Debug output (copious!)\n"
              "  -c --c++            Generate C++ files\n"
              "  -n --namespace      Use C++ namespace\n"
              "  -h --hdr-prefix str Prefix for C++ include of header (eg directory)\n"
              "  -i --inlines        Use C++ inlines\n"
              "  -s[n] --split[n]    Split output into n (default 2) files\n"
              "  -m --module name    Module name prefix/namespace\n"
              "  -r --rename args    Rename import module where arg is:\n"
              "                        from=name[,prefix[,fname]]\n"
              "                          from is module name in ASN file\n"
              "                          name is target header file name\n"
              "                          prefix is optional prefix for include\n"
              "                              (eg header directory)\n"
              "                          fname is optional base name for header files\n"
              "  --no-operators      Generate functions instead of operators for choice\n"
              "                        sub-object extraction.\n"
              "  -x --xml            X.693 support (XER)\n"
              "  -o --output file    Output filename/directory\n"
           << endl;
    return;
  }

  PTextFile prcFile;
  if (!prcFile.Open(args[0], PFile::ReadOnly)) {
    PError << GetName() << ": cannot open \"" 
         << prcFile.GetFilePath() << "\" :" << prcFile.GetErrorText() << endl;
    return;
  }

  if (args.HasOption('d'))
    yydebug = 1;
  if (args.HasOption('e'))
    LexEcho = TRUE;

  fileName   = prcFile.GetFilePath();
  lineNumber = 1;
  fatals     = 0;
  warnings   = 0;

  if (args.HasOption("classheaderfile")) {
    if (!SetClassHeaderFile(args))
      return;
  }

  if (args.HasOption("classheader")) {
    if (!SetClassHeader(args))
      return;
  }

  if (args.HasOption('v'))
    cout << "Parsing..." << endl;

  yyin = _fdopen(prcFile.GetHandle(), "r");
  PAssert(yyin != NULL, "fdopen failed!");
  yyparse();

  if (Module != NULL) {
    if (args.GetOptionCount('v') > 1)
      PError << "Module " << *Module << endl;

    if (args.HasOption('c'))
      Module->GenerateCplusplus(args.GetOptionString('o', args[0]),
                                args.GetOptionString('m'),
                                args.GetOptionString('h'),
                                numFiles,
                                args.HasOption('n'),
                                args.HasOption('i'),
                                !args.HasOption("no-operators"),
                                args.HasOption('v'));
  }
}


BOOL App::SetClassHeaderFile(PArgList & args)
{
  PStringArray lines = args.GetOptionString("classheaderfile").Lines();
  if (lines.IsEmpty()) {
    PError << GetName() << ": malformed --classheaderfile option\n";
    return FALSE;
  }

  for (PINDEX i = 0; i < lines.GetSize(); i++) {
    PString str = lines[i];
    PINDEX pos = str.Find("=");
    if (pos == P_MAX_INDEX) {
      PError << GetName() << ": malformed --classheaderfile option\n";
      return FALSE;
    }

    PFilePath fn = str.Right(pos+1);
    PTextFile file(fn, PFile::ReadOnly);
    if (!file.IsOpen()) {
      PError << GetName() << ": cannot open file required for --classheaderfile option \"" << fn
                          << "\" :" << file.GetErrorText() << '\n';
      return FALSE;
    }

    PString text;
    PString line;
    while (file.ReadLine(line))
      text += PString(PString::Literal, (const char *)line) + '\n';
    classToHeader.SetAt(str.Left(pos), text);
  }

  return TRUE;
}


BOOL App::SetClassHeader(PArgList & args)
{
  PStringArray lines = args.GetOptionString("classheader").Lines();
  if (lines.IsEmpty()) {
    PError << GetName() << ": malformed --classheader option\n";
    return FALSE;
  }

  for (PINDEX i = 0; i < lines.GetSize(); i++) {
    PString str = lines[i];
    PINDEX pos = str.Find("=");
    if (pos == P_MAX_INDEX) {
      PError << GetName() << ": malformed --classheader option\n";
      return FALSE;
    }

    PString text(PString::Literal, (const char *)str.Mid(pos+1));
    classToHeader.SetAt(str.Left(pos), text);
  }

  return TRUE;
}


void App::OutputAdditionalHeaders(ostream & hdr, const PString & className)
{
  if (classToHeader.Contains(className)) {
    hdr << "// following code added by command line option\n"
           "\n"
        << classToHeader[className] << "\n"
           "\n"
           "// end of added code\n"
           "\n";
  }
}

/////////////////////////////////////////
//
//  miscellaneous
//

class indent
{
  public:
    indent() { }
    friend ostream & operator<<(ostream & s, const indent &)
    { return s << setw(Module->GetIndentLevel()*3) << ' '; }
};


/////////////////////////////////////////
//
//  intermediate structures from parser
//

NamedNumber::NamedNumber(PString * nam)
  : name(*nam)
{
  delete nam;
  number = 0;
  autonumber = TRUE;
}


NamedNumber::NamedNumber(PString * nam, int num)
  : name(*nam)
{
  delete nam;
  number = num;
  autonumber = FALSE;
}


NamedNumber::NamedNumber(PString * nam, const PString & ref)
  : name(*nam), reference(ref)
{
  delete nam;
  number = 0;
  autonumber = FALSE;
}


void NamedNumber::PrintOn(ostream & strm) const
{
  strm << name << " (";
  if (reference.IsEmpty())
    strm << number;
  else
    strm << reference;
  strm << ')';
}


void NamedNumber::SetAutoNumber(const NamedNumber & prev)
{
  if (autonumber) {
    number = prev.number + 1;
    autonumber = FALSE;
  }
}


/////////////////////////////////////////////////////////

Tag::Tag(unsigned tagNum)
{
  type = Universal;
  number = tagNum;
  mode = Module->GetDefaultTagMode();
}


const char * Tag::classNames[] = {
  "UNIVERSAL", "APPLICATION", "CONTEXTSPECIFIC", "PRIVATE"
};


const char * Tag::modeNames[] = {
  "IMPLICIT", "EXPLICIT", "AUTOMATIC"
};


void Tag::PrintOn(ostream & strm) const
{
  if (type != Universal || number != IllegalUniversalTag) {
    strm << '[';
    if (type != ContextSpecific)
      strm << classNames[type] << ' ';
    strm << number << "] " << modeNames[mode] << ' ';
  }
}


/////////////////////////////////////////////////////////

Constraint::Constraint(ConstraintElementBase * elmt)
{
  standard.Append(elmt);
  extendable = FALSE;
}


Constraint::Constraint(ConstraintElementList * stnd, BOOL extend, ConstraintElementList * ext)
{
  if (stnd != NULL) {
    standard = *stnd;
    delete stnd;
  }
  extendable = extend;
  if (ext != NULL) {
    extensions = *ext;
    delete ext;
  }
}


void Constraint::PrintOn(ostream & strm) const
{
  strm << '(';
  for (PINDEX i = 0; i < standard.GetSize(); i++)
    strm << standard[i];
  if (extendable) {
    strm << indent();
    if (standard.GetSize() > 0)
      strm << ", ";
    strm << "..., ";
    for (PINDEX i = 0; i < extensions.GetSize(); i++)
      strm << extensions[i];
  }
  strm << ')';
}


void Constraint::GenerateCplusplus(const PString & fn, ostream & hdr, ostream & cxx)
{
  switch (standard.GetSize()) {
    case 0 :
      return;
    case 1 :
      break;
    default :
      PError << StdError(Warning) << "unsupported UNION constraints, ignored." << endl;
  }

  if (extensions.GetSize() > 0)
    PError << StdError(Warning) << "unsupported extension constraints, ignored." << endl;

  PString fn2 = fn;
  if (fn.Find("PASN_Object::") == P_MAX_INDEX) {
    if (extendable)
      fn2 += "PASN_Object::ExtendableConstraint";
    else
      fn2 += "PASN_Object::FixedConstraint";
  }

  standard[0].GenerateCplusplus(fn2, hdr, cxx);
}


BOOL Constraint::ReferencesType(const TypeBase & type)
{
  PINDEX i;

  for (i = 0; i < standard.GetSize(); i++) {
    if (standard[i].ReferencesType(type))
      return TRUE;
  }

  for (i = 0; i < extensions.GetSize(); i++) {
    if (extensions[i].ReferencesType(type))
      return TRUE;
  }

  return FALSE;
}


/////////////////////////////////////////////////////////

ConstraintElementBase::ConstraintElementBase()
{
  exclusions = NULL;
}


void ConstraintElementBase::GenerateCplusplus(const PString &, ostream &, ostream &)
{
  PError << StdError(Warning) << "unsupported constraint, ignored." << endl;
}


BOOL ConstraintElementBase::ReferencesType(const TypeBase &)
{
  return FALSE;
}


/////////////////////////////////////////////////////////

ConstrainAllConstraintElement::ConstrainAllConstraintElement(ConstraintElementBase * excl)
{
  SetExclusions(excl);
}


/////////////////////////////////////////////////////////

ElementListConstraintElement::ElementListConstraintElement(ConstraintElementList * list)
  : elements(*list)
{
  delete list;
}


void ElementListConstraintElement::PrintOn(ostream & strm) const
{
  elements.PrintOn(strm);
}


void ElementListConstraintElement::GenerateCplusplus(const PString & fn, ostream & hdr, ostream & cxx)
{
  for (PINDEX i = 0; i < elements.GetSize(); i++)
    elements[i].GenerateCplusplus(fn, hdr, cxx);
}


BOOL ElementListConstraintElement::ReferencesType(const TypeBase & type)
{
  for (PINDEX i = 0; i < elements.GetSize(); i++) {
    if (elements[i].ReferencesType(type))
      return TRUE;
  }
  return FALSE;
}


/////////////////////////////////////////////////////////

SingleValueConstraintElement::SingleValueConstraintElement(ValueBase * val)
{
  value = val;
}


SingleValueConstraintElement::~SingleValueConstraintElement()
{
  delete value;
}


void SingleValueConstraintElement::PrintOn(ostream & strm) const
{
  strm << *value;
}


void SingleValueConstraintElement::GenerateCplusplus(const PString & fn, ostream & hdr, ostream & cxx)
{
  cxx << fn << ", ";
  value->GenerateCplusplus(hdr, cxx);
  cxx << ");\n";
}


/////////////////////////////////////////////////////////

ValueRangeConstraintElement::ValueRangeConstraintElement(ValueBase * lowerBound, ValueBase * upperBound)
{
  lower = lowerBound;
  upper = upperBound;
}


ValueRangeConstraintElement::~ValueRangeConstraintElement()
{
  delete lower;
  delete upper;
}


void ValueRangeConstraintElement::PrintOn(ostream & strm) const
{
  strm << *lower << ".." << *upper;
}


void ValueRangeConstraintElement::GenerateCplusplus(const PString & fn, ostream & hdr, ostream & cxx)
{
  cxx << fn << ", ";
  lower->GenerateCplusplus(hdr, cxx);
  cxx << ", ";
  upper->GenerateCplusplus(hdr, cxx);
  cxx << ");\n";
}


/////////////////////////////////////////////////////////

SubTypeConstraintElement::SubTypeConstraintElement(TypeBase * typ)
{
  subtype = typ;
}


SubTypeConstraintElement::~SubTypeConstraintElement()
{
  delete subtype;
}


void SubTypeConstraintElement::PrintOn(ostream & strm) const
{
  strm << subtype->GetTypeName();
}


void SubTypeConstraintElement::GenerateCplusplus(const PString &, ostream & hdr, ostream &)
{
  hdr << subtype->GetTypeName();
}


BOOL SubTypeConstraintElement::ReferencesType(const TypeBase & type)
{
  return subtype->ReferencesType(type);
}


/////////////////////////////////////////////////////////

NestedConstraintConstraintElement::NestedConstraintConstraintElement(Constraint * con)
{
  constraint = con;
}


NestedConstraintConstraintElement::~NestedConstraintConstraintElement()
{
  delete constraint;
}


BOOL NestedConstraintConstraintElement::ReferencesType(const TypeBase & type)
{
  if (constraint == NULL)
    return FALSE;

  return constraint->ReferencesType(type);
}


/////////////////////////////////////////////////////////

SizeConstraintElement::SizeConstraintElement(Constraint * constraint)
  : NestedConstraintConstraintElement(constraint)
{
}


void SizeConstraintElement::PrintOn(ostream & strm) const
{
  strm << "SIZE" << *constraint;
}


void SizeConstraintElement::GenerateCplusplus(const PString & fn, ostream & hdr, ostream & cxx)
{
  constraint->GenerateCplusplus(fn, hdr, cxx);
}


/////////////////////////////////////////////////////////

FromConstraintElement::FromConstraintElement(Constraint * constraint)
  : NestedConstraintConstraintElement(constraint)
{
}


void FromConstraintElement::PrintOn(ostream & strm) const
{
  strm << "FROM" << *constraint;
}


void FromConstraintElement::GenerateCplusplus(const PString & fn, ostream & hdr, ostream & cxx)
{
  PString newfn = fn;
  newfn.Replace("SetConstraints(", "SetCharacterSet(");
  constraint->GenerateCplusplus(newfn, hdr, cxx);
}


/////////////////////////////////////////////////////////

WithComponentConstraintElement::WithComponentConstraintElement(PString * newName,
                                                               Constraint * constraint,
                                                               int pres)
  : NestedConstraintConstraintElement(constraint)
{
  if (newName != NULL) {
    name = *newName;
    delete newName;
  }

  presence = pres;
}


void WithComponentConstraintElement::PrintOn(ostream & strm) const
{
  if (name.IsEmpty())
    strm << "WITH COMPONENT";
  else
    strm << name;

  if (constraint != NULL)
    strm << *constraint;

  switch (presence) {
    case Present :
      strm << " PRESENT";
      break;
    case Absent :
      strm << " ABSENT";
      break;
    case Optional :
      strm << " OPTIONAL";
      break;
  }
}


void WithComponentConstraintElement::GenerateCplusplus(const PString &, ostream &, ostream & cxx)
{
  if (presence == Present)
    cxx << "  IncludeOptionalField(e_" << name << ");\n";
}


/////////////////////////////////////////////////////////

InnerTypeConstraintElement::InnerTypeConstraintElement(ConstraintElementList * list,
                                                       BOOL part)
  : ElementListConstraintElement(list)
{
  partial = part;
}


void InnerTypeConstraintElement::PrintOn(ostream & strm) const
{
  strm << "WITH COMPONENTS { ";

  if (partial)
    strm << "..., ";

  for (PINDEX i = 0; i < elements.GetSize(); i++) {
    if (i > 0)
      strm << ", ";
    elements[i].PrintOn(strm);
  }

  strm << " }";
}


void InnerTypeConstraintElement::GenerateCplusplus(const PString & fn, ostream & hdr, ostream & cxx)
{
  for (PINDEX i = 0; i < elements.GetSize(); i++)
    elements[i].GenerateCplusplus(fn, hdr, cxx);
}


/////////////////////////////////////////////////////////

UserDefinedConstraintElement::UserDefinedConstraintElement(TypesList * t)
{
  if (t != NULL) {
    types = *t;
    delete t;
  }
}


void UserDefinedConstraintElement::PrintOn(ostream & strm) const
{
  strm << "CONSTRAINED BY { ";
  for (PINDEX i = 0; i < types.GetSize(); i++) {
    if (i > 0)
      strm << ", ";
    strm << types[i].GetTypeName();
  }
  strm << " }";
}


void UserDefinedConstraintElement::GenerateCplusplus(const PString &, ostream &, ostream &)
{
}


/////////////////////////////////////////////////////////

TypeBase::TypeBase(unsigned tagNum)
  : tag(tagNum), defaultTag(tagNum)
{
  isOptional = FALSE;
  defaultValue = NULL;
  isGenerated = FALSE;
}


TypeBase::TypeBase(TypeBase * copy)
  : name(copy->name),
    identifier(MakeIdentifierC(name)),
    tag(copy->tag),
    defaultTag(copy->tag)
{
  isOptional = copy->isOptional;
  defaultValue = NULL;
  isGenerated = FALSE;
}


PObject::Comparison TypeBase::Compare(const PObject & obj) const
{
  return name.Compare(((const TypeBase &)obj).name);
}


void TypeBase::PrintOn(ostream & strm) const
{
  PrintStart(strm);
  PrintFinish(strm);
}


void TypeBase::PrintStart(ostream & strm) const
{
  strm << indent();
  if (!name) {
    strm << name;
    if (!parameters.IsEmpty()) {
      strm << " { ";
      for (PINDEX i = 0; i < parameters.GetSize(); i++) {
        if (i > 0)
          strm << ", ";
        strm << parameters[i];
      }
      strm << " } ";
    }
    strm << ": ";
  }
  strm << tag << GetClass() << ' ';
  Module->SetIndentLevel(1);
}


void TypeBase::PrintFinish(ostream & strm) const
{
  Module->SetIndentLevel(-1);
  strm << ' ' << constraints;
  if (isOptional)
    strm << " OPTIONAL";
  if (defaultValue != NULL)
    strm << " DEFAULT " << *defaultValue;
  strm << '\n';
}


int TypeBase::GetIdentifierTokenContext() const
{
  return IDENTIFIER;
}


int TypeBase::GetBraceTokenContext() const
{
  return '{';
}


void TypeBase::SetName(PString * newName)
{
  name = *newName;
  delete newName;
  identifier = MakeIdentifierC(name);
}


void TypeBase::AdjustIdentifier()
{
  identifier = Module->GetPrefix() + MakeIdentifierC(name);
}


void TypeBase::SetTag(Tag::Type type, unsigned num, Tag::Mode mode)
{
  tag.type = type;
  tag.number = num;
  tag.mode = mode;
}


void TypeBase::SetParameters(PStringList * list)
{
  parameters = *list;
  delete list;
}


void TypeBase::MoveConstraints(TypeBase * from)
{
  from->constraints.DisallowDeleteObjects();

  while (!from->constraints.IsEmpty())
    constraints.Append(from->constraints.RemoveAt(0));

  from->constraints.AllowDeleteObjects();
}


void TypeBase::FlattenUsedTypes()
{
}


TypeBase * TypeBase::FlattenThisType(const TypeBase &)
{
  return this;
}


BOOL TypeBase::IsChoice() const
{
  return FALSE;
}


BOOL TypeBase::IsParameterizedType() const
{
  return FALSE;
}


BOOL TypeBase::IsPrimitiveType() const
{
  return TRUE;
}


void TypeBase::GenerateCplusplus(ostream & hdr, ostream & cxx)
{
  BeginGenerateCplusplus(hdr, cxx);

  // Close off the constructor implementation
  cxx << ")\n"
         "{\n";
  GenerateCplusplusConstraints(PString(), hdr, cxx);

  EndGenerateCplusplus(hdr, cxx);
}


void TypeBase::GenerateForwardDecls(ostream &)
{
}


void TypeBase::GenerateOperators(ostream &, ostream &, const TypeBase &)
{
}


PString TypeBase::GetTypeName() const
{
  return GetAncestorClass();
}


BOOL TypeBase::CanReferenceType() const
{
  return FALSE;
}


BOOL TypeBase::ReferencesType(const TypeBase &)
{
  return FALSE;
}


void TypeBase::SetImportPrefix(const PString &)
{
}


BOOL TypeBase::IsParameterisedImport() const
{
  return FALSE;
}


void TypeBase::BeginGenerateCplusplus(ostream & hdr, ostream & cxx)
{
  classNameString = GetIdentifier();

  if (!parameters.IsEmpty()) {
    templatePrefix = "template <";
    classNameString += '<';
    for (PINDEX i = 0; i < parameters.GetSize(); i++) {
      if (i > 0) {
        templatePrefix += ", ";
        classNameString += ", ";
      }
      PString ident = MakeIdentifierC(parameters[i]);
      templatePrefix += "class " + ident;
      classNameString += ident;
    }
    templatePrefix += ">\n";
    classNameString += '>';
  }

  // Output header file declaration of class
  hdr << "//\n"
         "// " << GetName() << "\n"
         "//\n"
         "\n";
  GenerateForwardDecls(hdr);
  hdr << templatePrefix
      << "class " << GetIdentifier() << " : public " << GetTypeName() << "\n"
         "{\n"
         "#ifndef PASN_LEANANDMEAN\n"
         "    PCLASSINFO(" << GetIdentifier() << ", " << GetTypeName() << ");\n"
         "#endif\n"
         "  public:\n"
         "    " << GetIdentifier() << "(unsigned tag = ";
  if (tag.type == Tag::Universal &&
      tag.number < PARRAYSIZE(UniversalTagNames) &&
      UniversalTagNames[tag.number] != NULL)
    hdr << UniversalTagNames[tag.number];
  else
    hdr << tag.number;
  hdr << ", TagClass tagClass = " << UniversalTagClassNames[tag.type] << ");\n\n";

  App & app = (App &)PProcess::Current();
  app.OutputAdditionalHeaders(hdr, GetIdentifier());

  // Output cxx file implementation of class
  cxx << "//\n"
         "// " << GetName() << "\n"
         "//\n"
         "\n"
      << GetTemplatePrefix()
      << GetClassNameString() << "::" << GetIdentifier() << "(unsigned tag, PASN_Object::TagClass tagClass)\n"
         "  : " << GetTypeName() << "(tag, tagClass";
}


void TypeBase::EndGenerateCplusplus(ostream & hdr, ostream & cxx)
{
  cxx << "}\n"
         "\n"
         "\n";

  GenerateOperators(hdr, cxx, *this);

  // Output header file declaration of class
  hdr << "    PObject * Clone() const;\n"
         "};\n"
         "\n"
         "\n";

  // Output cxx file implementation of class
  cxx << GetTemplatePrefix()
      << "PObject * " << GetClassNameString() << "::Clone() const\n"
         "{\n"
         "#ifndef PASN_LEANANDMEAN\n"
         "  PAssert(IsClass(" << GetClassNameString() << "::Class()), PInvalidCast);\n"
         "#endif\n"
         "  return new " << GetClassNameString() << "(*this);\n"
         "}\n"
         "\n"
         "\n";

  isGenerated = TRUE;
}


void TypeBase::GenerateCplusplusConstructor(ostream &, ostream & cxx)
{
  cxx << '(';
  if (HasNonStandardTag()) {
    if (tag.type == Tag::Universal &&
        tag.number < PARRAYSIZE(UniversalTagNames) &&
        UniversalTagNames[tag.number] != NULL)
      cxx << UniversalTagNames[tag.number];
    else
      cxx << tag.number;
    cxx << ", " << UniversalTagClassNames[tag.type];
  }
  cxx << ')';
}


void TypeBase::GenerateCplusplusConstraints(const PString & prefix, ostream & hdr, ostream & cxx)
{
  for (PINDEX i = 0; i < constraints.GetSize(); i++)
    constraints[i].GenerateCplusplus("  " + prefix + "SetConstraints(", hdr, cxx);
}


/////////////////////////////////////////////////////////

DefinedType::DefinedType(PString * name, BOOL parameter)
  : TypeBase(Tag::IllegalUniversalTag),
    referenceName(*name)
{
  delete name;
  baseType = NULL;
  unresolved = !parameter;
}


DefinedType::DefinedType(TypeBase * refType, TypeBase * bType)
  : TypeBase(refType),
    referenceName(bType->GetName())
{
  MoveConstraints(refType);

  baseType = bType;
  unresolved = FALSE;
}


DefinedType::DefinedType(TypeBase * refType, const PString & refName)
  : TypeBase(refType)
{
  MoveConstraints(refType);
  ConstructFromType(refType, refName);
}


DefinedType::DefinedType(TypeBase * refType, const TypeBase & parent)
  : TypeBase(refType)
{
  if (!name)
    ConstructFromType(refType, parent.GetName() + '_' + name);
  else
    ConstructFromType(refType, parent.GetName() + "_subtype");
}


void DefinedType::ConstructFromType(TypeBase * refType, const PString & name)
{
  referenceName = name;
  refType->SetName(new PString(name));

  Module->AppendType(refType);

  baseType = refType;
  unresolved = FALSE;
}


void DefinedType::PrintOn(ostream & strm) const
{
  PrintStart(strm);
  strm << referenceName << ' ';
  PrintFinish(strm);
}


BOOL DefinedType::CanReferenceType() const
{
  return TRUE;
}


BOOL DefinedType::IsChoice() const
{
  if (baseType != NULL)
    return baseType->IsChoice();
  return FALSE;
}


BOOL DefinedType::IsParameterizedType() const
{
  if (baseType != NULL)
    return baseType->IsParameterizedType();
  return FALSE;
}


BOOL DefinedType::ReferencesType(const TypeBase & type)
{
  if (unresolved) {
    unresolved = FALSE;

    if ((baseType = Module->FindType(referenceName)) == NULL)
      PError << StdError(Warning) << "unresolved symbol: " << referenceName << endl;
    else {
      if (!HasNonStandardTag())
        defaultTag = tag = baseType->GetTag();
    }
  }

  return type.GetName() == referenceName;
}


void DefinedType::GenerateOperators(ostream & hdr, ostream & cxx, const TypeBase & actualType)
{
  if (baseType != NULL)
    baseType->GenerateOperators(hdr, cxx, actualType);
}


const char * DefinedType::GetAncestorClass() const
{
  if (baseType != NULL)
    return baseType->GetAncestorClass();
  return NULL;
}


PString DefinedType::GetTypeName() const
{
  if (baseType == NULL)
    return referenceName;

  if (HasConstraints() && baseType->IsPrimitiveType())
    return baseType->GetTypeName();

  return baseType->GetIdentifier();
}


/////////////////////////////////////////////////////////

ParameterizedType::ParameterizedType(PString * name, TypesList * args)
  : DefinedType(name, FALSE),
    arguments(*args)
{
  delete args;
}


void ParameterizedType::PrintOn(ostream & strm) const
{
  PrintStart(strm);
  strm << referenceName << " { ";
  for (PINDEX i = 0; i < arguments.GetSize(); i++) {
    if (i > 0)
      strm << ", ";
    strm << arguments[i].GetTypeName();
  }
  strm << " }";
  PrintFinish(strm);
}


BOOL ParameterizedType::IsParameterizedType() const
{
  return TRUE;
}


BOOL ParameterizedType::ReferencesType(const TypeBase & type)
{
  for (PINDEX i = 0; i < arguments.GetSize(); i++) {
    if (arguments[i].ReferencesType(type))
      return TRUE;
  }

  return DefinedType::ReferencesType(type);
}


PString ParameterizedType::GetTypeName() const
{
  PString typeName = DefinedType::GetTypeName();
  if (!arguments.IsEmpty()) {
    typeName += '<';
    for (PINDEX i = 0; i < arguments.GetSize(); i++) {
      if (i > 0)
        typeName += ", ";
      typeName += arguments[i].GetTypeName();
    }
    typeName += '>';
  }
  return typeName;
}


/////////////////////////////////////////////////////////

SelectionType::SelectionType(PString * name, TypeBase * base)
  : TypeBase(Tag::IllegalUniversalTag),
    selection(*name)
{
  delete name;
  baseType = PAssertNULL(base);
}


SelectionType::~SelectionType()
{
  delete baseType;
}


void SelectionType::PrintOn(ostream & strm) const
{
  PrintStart(strm);
  strm << selection << '<' << *baseType;
  PrintFinish(strm);
}


void SelectionType::FlattenUsedTypes()
{
  baseType = baseType->FlattenThisType(*this);
}


TypeBase * SelectionType::FlattenThisType(const TypeBase & parent)
{
  return new DefinedType(this, parent);
}


void SelectionType::GenerateCplusplus(ostream &, ostream &)
{
  PError << StdError(Fatal) << "Cannot generate code for Selection type" << endl;
  isGenerated = TRUE;
}


const char * SelectionType::GetAncestorClass() const
{
  return "";
}


BOOL SelectionType::CanReferenceType() const
{
  return TRUE;
}


BOOL SelectionType::ReferencesType(const TypeBase & type)
{
  return baseType->ReferencesType(type);
}


/////////////////////////////////////////////////////////

BooleanType::BooleanType()
  : TypeBase(Tag::UniversalBoolean)
{
}


void BooleanType::GenerateOperators(ostream & hdr, ostream & cxx, const TypeBase & actualType)
{
  hdr << "    " << actualType.GetIdentifier() << " & operator=(BOOL v)";
  if (Module->UsingInlines())
    hdr << " { SetValue(v);  return *this; }\n";
  else {
    hdr << ";\n";
    cxx << actualType.GetTemplatePrefix()
        << actualType.GetIdentifier() << " & "
        << actualType.GetClassNameString() << "::operator=(BOOL v)\n"
           "{\n"
           "  SetValue(v);\n"
           "  return *this;\n"
           "}\n"
           "\n"
           "\n";
  }
}


const char * BooleanType::GetAncestorClass() const
{
  return "PASN_Boolean";
}


/////////////////////////////////////////////////////////

IntegerType::IntegerType()
  : TypeBase(Tag::UniversalInteger)
{
}


IntegerType::IntegerType(NamedNumberList * lst)
  : TypeBase(Tag::UniversalInteger),
    allowedValues(*lst)
{
  delete lst;
}


void IntegerType::GenerateOperators(ostream & hdr, ostream & cxx, const TypeBase & actualType)
{
  hdr << "    " << actualType.GetIdentifier() << " & operator=(int v)";
  if (Module->UsingInlines())
    hdr << " { SetValue(v);  return *this; }\n";
  else {
    hdr << ";\n";
    cxx << actualType.GetTemplatePrefix()
        << actualType.GetIdentifier() << " & "
        << actualType.GetClassNameString() << "::operator=(int v)\n"
           "{\n"
           "  SetValue(v);\n"
           "  return *this;\n"
           "}\n"
           "\n"
           "\n";
  }

  hdr << "    " << actualType.GetIdentifier() << " & operator=(unsigned v)";
  if (Module->UsingInlines())
    hdr << " { SetValue(v);  return *this; }\n";
  else {
    hdr << ";\n";
    cxx  << actualType.GetTemplatePrefix()
        << actualType.GetIdentifier() << " & "
        << actualType.GetClassNameString() << "::operator=(unsigned v)\n"
           "{\n"
           "  SetValue(v);\n"
           "  return *this;\n"
           "}\n"
           "\n"
           "\n";
  }
}


const char * IntegerType::GetAncestorClass() const
{
  return "PASN_Integer";
}


/////////////////////////////////////////////////////////

EnumeratedType::EnumeratedType(NamedNumberList * enums, BOOL extend, NamedNumberList * ext)
  : TypeBase(Tag::UniversalEnumeration),
    enumerations(*enums)
{
  numEnums = enums->GetSize();
  delete enums;
  extendable = extend;
  if (ext != NULL) {
    ext->DisallowDeleteObjects();
    for (PINDEX i = 0; i < ext->GetSize(); i++)
      enumerations.Append(ext->GetAt(i));
    delete ext;
  }
}


void EnumeratedType::PrintOn(ostream & strm) const
{
  PrintStart(strm);
  strm << '\n';
  PINDEX i;
  for (i = 0; i < numEnums; i++)
    strm << indent() << enumerations[i] << '\n';
  if (extendable) {
    strm << "...\n";
    for (; i < enumerations.GetSize(); i++)
      strm << indent() << enumerations[i] << '\n';
  }
  PrintFinish(strm);
}


TypeBase * EnumeratedType::FlattenThisType(const TypeBase & parent)
{
  return new DefinedType(this, parent);
}


void EnumeratedType::GenerateCplusplus(ostream & hdr, ostream & cxx)
{
  PINDEX i;
  PArgList & args = PProcess::Current().GetArguments();
  BOOL xml_output = args.HasOption('x');

  cxx << "#ifndef PASN_NOPRINTON\n"
      "const static PASN_Names Names_"<< GetIdentifier() << "[]={\n";


  for (i = 0; i < enumerations.GetSize(); i++) {
    if (i > 0) 
        cxx << "       ,{\"";
    else
        cxx << "        {\"";

    int num = enumerations[i].GetNumber();
    cxx << enumerations[i].GetName() << "\"," << num << "}\n";
  }

  cxx << "};\n#endif\n";

  BeginGenerateCplusplus(hdr, cxx);

  int maxEnumValue = 0;
  for (i = 0; i < enumerations.GetSize(); i++) {
    int num = enumerations[i].GetNumber();
    if (maxEnumValue < num)
      maxEnumValue = num;
  }

  // Generate enumerations and complete the constructor implementation
  hdr << "    enum Enumerations {\n";
  cxx << ", " << maxEnumValue << ", " << (extendable ? "TRUE" : "FALSE") << "\n"
         "#ifndef PASN_NOPRINTON\n    ,(const PASN_Names *)Names_" << GetIdentifier() << "," <<enumerations.GetSize()<<"\n";

  int prevNum = -1;
  for (i = 0; i < enumerations.GetSize(); i++) {
    if (i > 0) {
      hdr << ",\n";
    }

    hdr << "      e_" << MakeIdentifierC(enumerations[i].GetName());

    int num = enumerations[i].GetNumber();
    if (num != prevNum+1) {
      hdr << " = " << num;
    }
    prevNum = num;

  }

  hdr << "\n"
         "    };\n"
         "\n";
  cxx << "#endif\n"
         "    )\n"
         "{\n";
  GenerateCplusplusConstraints(PString(), hdr, cxx);

  if (xml_output)
  {
    hdr << "    BOOL DecodeXER(PXER_Stream & strm);\n"
           "    void EncodeXER(PXER_Stream & strm) const;\n";

    cxx << "}\n"
           "\n"
        << GetTemplatePrefix()
        << "BOOL " << GetClassNameString() << "::DecodeXER(PXER_Stream & strm)\n"
           "{\n"
           "  PXMLElement * elem = strm.GetCurrentElement();\n"
           "  PXMLObject * sub_elem = elem->GetElement();\n"
           "\n"
           "  if (!elem || !elem->IsElement())\n"
           "    return FALSE;\n"
           "\n"
           "  PCaselessString id = ((PXMLElement *)sub_elem)->GetName();\n"
           "\n"
           " ";

    for (i = 0 ; i < enumerations.GetSize() ; i++) {
      cxx << " if (id == \"" << enumerations[i].GetName() << "\") {\n"
             "    value = " << enumerations[i].GetNumber() << ";\n"
             "    return TRUE;\n"
             "  }\n"
             "  else";
    }

    cxx << "\n"
           "    return FALSE;\n"
           "}\n"
           "\n";

    cxx << GetTemplatePrefix()
        << "void " << GetClassNameString() << "::EncodeXER(PXER_Stream & strm) const\n"
           "{\n"
           "  PXMLElement * elem = strm.GetCurrentElement();\n"
           "  PString id;\n"
           "\n"
           "  switch(value)\n"
           "  {\n";

    for (i = 0 ; i < enumerations.GetSize() ; i++) {
      cxx << "  case " << enumerations[i].GetNumber() << ":\n"
             "    elem->AddChild(new PXMLElement(elem, \"" << enumerations[i].GetName() << "\"));\n"
             "    break;\n";
    }

    cxx << "  default:\n"
           "    break;\n"
           "  }\n";
  }

  EndGenerateCplusplus(hdr, cxx);
}


void EnumeratedType::GenerateOperators(ostream & hdr, ostream & cxx, const TypeBase & actualType)
{
  hdr << "    " << actualType.GetIdentifier() << " & operator=(unsigned v)";
  if (Module->UsingInlines())
    hdr << " { SetValue(v);  return *this; }\n";
  else {
    hdr << ";\n";
    cxx << actualType.GetTemplatePrefix()
        << actualType.GetIdentifier() << " & "
        << actualType.GetClassNameString() << "::operator=(unsigned v)\n"
           "{\n"
           "  SetValue(v);\n"
           "  return *this;\n"
           "}\n"
           "\n"
           "\n";
  }
}


const char * EnumeratedType::GetAncestorClass() const
{
  return "PASN_Enumeration";
}


/////////////////////////////////////////////////////////

RealType::RealType()
  : TypeBase(Tag::UniversalReal)
{
}


const char * RealType::GetAncestorClass() const
{
  return "PASN_Real";
}


/////////////////////////////////////////////////////////

BitStringType::BitStringType()
  : TypeBase(Tag::UniversalBitString)
{
}


BitStringType::BitStringType(NamedNumberList * lst)
  : TypeBase(Tag::UniversalBitString),
    allowedBits(*lst)
{
  delete lst;
}

int BitStringType::GetIdentifierTokenContext() const
{
  return OID_IDENTIFIER;
}


int BitStringType::GetBraceTokenContext() const
{
  return BITSTRING_BRACE;
}


const char * BitStringType::GetAncestorClass() const
{
  return "PASN_BitString";
}


/////////////////////////////////////////////////////////

OctetStringType::OctetStringType()
  : TypeBase(Tag::UniversalOctetString)
{
}


void OctetStringType::GenerateOperators(ostream & hdr, ostream & cxx, const TypeBase & actualType)
{
  static const char * const types[] = {
    "char *", "PString &", "PBYTEArray &"
  };

  PINDEX i;
  for (i = 0; i < PARRAYSIZE(types); i++) {
    hdr << "    " << actualType.GetIdentifier() << "(const " << types[i] << " v)";
    if (Module->UsingInlines())
      hdr << " { SetValue(v);  }\n";
    else {
      hdr << ";\n";
      cxx << actualType.GetTemplatePrefix()
          << actualType.GetIdentifier() << "::" << actualType.GetIdentifier() << "(const " << types[i] << " v)\n"
             "{\n"
             "  SetValue(v);\n"
             "}\n"
             "\n"
             "\n";
    }
  }

  hdr << '\n';

  for (i = 0; i < PARRAYSIZE(types); i++) {
    hdr << "    " << actualType.GetIdentifier() << " & operator=(const " << types[i] << " v)";
    if (Module->UsingInlines())
      hdr << " { SetValue(v);  return *this; }\n";
    else {
      hdr << ";\n";
      cxx << actualType.GetTemplatePrefix()
          << actualType.GetIdentifier() << " & "
          << actualType.GetClassNameString() << "::operator=(const " << types[i] << " v)\n"
             "{\n"
             "  SetValue(v);\n"
             "  return *this;\n"
             "}\n"
             "\n"
             "\n";
    }
  }
}


const char * OctetStringType::GetAncestorClass() const
{
  return "PASN_OctetString";
}


/////////////////////////////////////////////////////////

NullType::NullType()
  : TypeBase(Tag::UniversalNull)
{
}


const char * NullType::GetAncestorClass() const
{
  return "PASN_Null";
}


/////////////////////////////////////////////////////////

SequenceType::SequenceType(TypesList * stnd,
                           BOOL extend,
                           TypesList * ext,
                           unsigned tagNum)
  : TypeBase(tagNum)
{
  if (stnd != NULL) {
    numFields = stnd->GetSize();
    fields = *stnd;
    delete stnd;
  }
  else
    numFields = 0;
  extendable = extend;
  if (ext != NULL) {
    ext->DisallowDeleteObjects();
    for (PINDEX i = 0; i < ext->GetSize(); i++)
      fields.Append(ext->GetAt(i));
    delete ext;
  }
}


void SequenceType::PrintOn(ostream & strm) const
{
  PrintStart(strm);
  strm << '\n';
  PINDEX i;
  for (i = 0; i < numFields; i++)
    strm << fields[i];
  if (extendable) {
    strm << indent() << "...\n";
    for (; i < fields.GetSize(); i++)
      strm << fields[i];
  }
  PrintFinish(strm);
}


void SequenceType::FlattenUsedTypes()
{
  for (PINDEX i = 0; i < fields.GetSize(); i++)
    fields.SetAt(i, fields[i].FlattenThisType(*this));
}


TypeBase * SequenceType::FlattenThisType(const TypeBase & parent)
{
  return new DefinedType(this, parent);
}


BOOL SequenceType::IsPrimitiveType() const
{
  return FALSE;
}


void SequenceType::GenerateCplusplus(ostream & hdr, ostream & cxx)
{
  PArgList & args = PProcess::Current().GetArguments();
  BOOL xml_output = args.HasOption('x');

  PINDEX i;

  BeginGenerateCplusplus(hdr, cxx);

  PINDEX baseOptions = 0;
  for (i = 0; i < fields.GetSize(); i++) {
    if (i < numFields && fields[i].IsOptional())
      baseOptions++;
  }

  // Complete ancestor constructor parameters
  cxx << ", " << baseOptions << ", "
      << (extendable ? "TRUE" : "FALSE") << ", "
      << fields.GetSize() - numFields
      << ')';

  // Output enum for optional parameters
  BOOL outputEnum = FALSE;
  for (i = 0; i < fields.GetSize(); i++) {
    if (i >= numFields || fields[i].IsOptional()) {
      if (outputEnum)
        hdr << ",\n";
      else {
        hdr << "    enum OptionalFields {\n";
        outputEnum = TRUE;
      }
      hdr << "      e_" << fields[i].GetIdentifier();
    }
  }

  if (outputEnum)
    hdr << "\n"
           "    };\n"
           "\n";

  // Output the declarations and constructors for member variables
  for (i = 0; i < fields.GetSize(); i++) {
    PString varname = "m_" + fields[i].GetIdentifier();
    hdr << "    " << fields[i].GetTypeName() << ' ' << varname << ";\n";
    if (fields[i].HasNonStandardTag()) {
      cxx << ",\n"
             "    " << varname;
      fields[i].GenerateCplusplusConstructor(hdr, cxx);
    }
  }

  // Output declarations for generated functions
  hdr << "\n"
         "    PINDEX GetDataLength() const;\n"
         "    BOOL Decode(PASN_Stream & strm);\n"
         "    void Encode(PASN_Stream & strm) const;\n"
         "#ifndef PASN_NOPRINTON\n"
         "    void PrintOn(ostream & strm) const;\n"
         "#endif\n";

  if (xml_output)
  {
    hdr << "    BOOL PreambleDecodeXER(PXER_Stream & strm);\n";

    if (fields.GetSize())
      hdr << "    void PreambleEncodeXER(PXER_Stream &) const;\n";
  }


  if (numFields > 0)
    hdr << "    Comparison Compare(const PObject & obj) const;\n";

  cxx << "\n"
         "{\n";
  GenerateCplusplusConstraints(PString(), hdr, cxx);
  for (i = 0; i < fields.GetSize(); i++) {
    PString ident = fields[i].GetIdentifier();
    fields[i].GenerateCplusplusConstraints("m_" + ident + ".", hdr, cxx);
    if (i >= numFields && !fields[i].IsOptional())
      cxx << "  IncludeOptionalField(e_" << ident << ");\n";
  }
  cxx << "}\n"
         "\n"
         "\n"
         "#ifndef PASN_NOPRINTON\n"
      << GetTemplatePrefix()
      << "void " << GetClassNameString() << "::PrintOn(ostream & strm) const\n"
         "{\n"
         "  int indent = strm.precision() + 2;\n"
         "  strm << \"{\\n\";\n";

  for (i = 0; i < fields.GetSize(); i++) {
    PString id = fields[i].GetIdentifier();
    if (i >= numFields || fields[i].IsOptional())
      cxx << "  if (HasOptionalField(e_" << id << "))\n"
             "  ";
    cxx << "  strm << setw(indent+" << id.GetLength()+3 << ") << \""
        << id << " = \" << setprecision(indent) << m_" << id << " << '\\n';\n";
  }

  cxx << "  strm << setw(indent-1) << setprecision(indent-2) << \"}\";\n"
         "}\n"
         "#endif\n"
         "\n"
         "\n";

  if (xml_output)
  {
    cxx << GetTemplatePrefix()
        << "BOOL " << GetClassNameString() << "::PreambleDecodeXER(PXER_Stream & strm)\n"
           "{\n";

    if (fields.GetSize())
    {
      cxx << "  PXMLElement * elem = strm.GetCurrentElement();\n"
             "  PXMLElement * sub_elem;\n"
             "  BOOL result;\n"
             "\n";

      for (i = 0; i < fields.GetSize(); i++)
      {
        PString id = fields[i].GetIdentifier();
        cxx << "  if ((sub_elem = (PXMLElement *)elem->GetElement(\"" << id << "\")) && sub_elem->IsElement())\n"
               "  {\n";

        if (i >= numFields || fields[i].IsOptional())
          cxx << "    IncludeOptionalField(e_" << id << ");\n";

        cxx << "    strm.SetCurrentElement(sub_elem);\n"
               "    result = m_" << id << ".Decode(strm);\n"
               "    strm.SetCurrentElement(sub_elem);\n"
               "    if (!result)\n"
               "      return FALSE;\n"
               "  }\n"
               "\n";
      }
    }

    cxx << "  return TRUE;\n"
           "}\n"
           "\n";

    if (fields.GetSize())
    {
      cxx << GetTemplatePrefix()
          << "void " << GetClassNameString() << "::PreambleEncodeXER(PXER_Stream & strm) const\n"
             "{\n";

      cxx << "  PXMLElement * elem = strm.GetCurrentElement();\n"
             "  PXMLElement * sub_elem;\n"
             "\n";

      for (i = 0; i < fields.GetSize(); i++)
      {
        PString id = fields[i].GetIdentifier();

        if (i >= numFields || fields[i].IsOptional())
          cxx << "  if (HasOptionalField(e_" << id << "))\n"
                 "  {\n";

        cxx << "    sub_elem = elem->AddChild(new PXMLElement(elem, \"" << id << "\"));\n"
               "    strm.SetCurrentElement(sub_elem);\n"
               "    m_" << id << ".Encode(strm);\n";

        if (i >= numFields || fields[i].IsOptional())
          cxx << "  }\n";

        cxx << "\n";
      }

      cxx << "  strm.SetCurrentElement(elem);\n"
             "}\n"
             "\n";
    }
  }

  if (numFields > 0) {
    cxx << GetTemplatePrefix()
        << "PObject::Comparison " << GetClassNameString() << "::Compare(const PObject & obj) const\n"
           "{\n"
           "#ifndef PASN_LEANANDMEAN\n"
           "  PAssert(PIsDescendant(&obj, " << GetClassNameString() << "), PInvalidCast);\n"
           "#endif\n"
           "  const " << GetClassNameString() << " & other = (const " << GetClassNameString() << " &)obj;\n"
           "\n"
           "  Comparison result;\n"
           "\n";

    for (i = 0; i < numFields; i++) {
      PString identifier = fields[i].GetIdentifier();
      cxx << "  if ((result = m_" << identifier << ".Compare(other.m_" << identifier << ")) != EqualTo)\n"
             "    return result;\n";
    }

    cxx << "\n"
           "  return PASN_Sequence::Compare(other);\n"
           "}\n"
           "\n"
           "\n";
  }

  cxx << GetTemplatePrefix()
      << "PINDEX " << GetClassNameString() << "::GetDataLength() const\n"
         "{\n"
         "  PINDEX length = 0;\n";

  for (i = 0; i < numFields; i++) {
    if (fields[i].IsOptional())
      cxx << "  if (HasOptionalField(e_" << fields[i].GetIdentifier() << "))\n  ";
    cxx << "  length += m_" << fields[i].GetIdentifier() << ".GetObjectLength();\n";
  }

  cxx << "  return length;\n"
         "}\n"
         "\n"
         "\n"
      << GetTemplatePrefix()
      << "BOOL " << GetClassNameString() << "::Decode(PASN_Stream & strm)\n"
         "{\n"
         "  if (!PreambleDecode(strm))\n"
         "    return FALSE;\n\n";

  if (xml_output)
  {
    cxx << "  if (PIsDescendant(&strm, PXER_Stream))\n"
           "    return TRUE;\n\n";
  }

  for (i = 0; i < numFields; i++) {
    cxx << "  if (";
    if (fields[i].IsOptional())
      cxx << "HasOptionalField(e_" << fields[i].GetIdentifier() << ") && ";
    cxx << "!m_" << fields[i].GetIdentifier() << ".Decode(strm))\n"
           "    return FALSE;\n";
  }

  for (; i < fields.GetSize(); i++)
    cxx << "  if (!KnownExtensionDecode(strm, e_"
        << fields[i].GetIdentifier()
        << ", m_" << fields[i].GetIdentifier() << "))\n"
           "    return FALSE;\n";

  cxx << "\n"
         "  return UnknownExtensionsDecode(strm);\n"
         "}\n"
         "\n"
         "\n"
      << GetTemplatePrefix()
      << "void " << GetClassNameString() << "::Encode(PASN_Stream & strm) const\n"
         "{\n"
         "  PreambleEncode(strm);\n\n";

  if (xml_output)
  {
    cxx << "  if (PIsDescendant(&strm, PXER_Stream))\n"
           "    return;\n\n";
  }

  for (i = 0; i < numFields; i++) {
    if (fields[i].IsOptional())
      cxx << "  if (HasOptionalField(e_" << fields[i].GetIdentifier() << "))\n"
             "  ";
    cxx << "  m_" << fields[i].GetIdentifier() << ".Encode(strm);\n";
  }

  for (; i < fields.GetSize(); i++)
    cxx << "  KnownExtensionEncode(strm, e_"
        << fields[i].GetIdentifier()
        << ", m_" << fields[i].GetIdentifier() << ");\n";

  cxx << "\n"
         "  UnknownExtensionsEncode(strm);\n";

  EndGenerateCplusplus(hdr, cxx);
}


const char * SequenceType::GetAncestorClass() const
{
  return "PASN_Sequence";
}


BOOL SequenceType::CanReferenceType() const
{
  return TRUE;
}


BOOL SequenceType::ReferencesType(const TypeBase & type)
{
  for (PINDEX i = 0; i < fields.GetSize(); i++)
    if (fields[i].ReferencesType(type))
      return TRUE;
  return FALSE;
}


/////////////////////////////////////////////////////////

SequenceOfType::SequenceOfType(TypeBase * base, Constraint * constraint, unsigned tag)
  : TypeBase(tag)
{
  baseType = base;
  if (constraint != NULL)
    AddConstraint(constraint);
}


SequenceOfType::~SequenceOfType()
{
  delete baseType;
}


void SequenceOfType::PrintOn(ostream & strm) const
{
  PrintStart(strm);
  if (baseType == NULL)
    strm << "!!Null Type!!\n";
  else
    strm << *baseType << '\n';
  PrintFinish(strm);
}


void SequenceOfType::FlattenUsedTypes()
{
  baseType = baseType->FlattenThisType(*this);
}


TypeBase * SequenceOfType::FlattenThisType(const TypeBase & parent)
{
  if (!baseType->IsPrimitiveType() || baseType->HasConstraints())
    return new DefinedType(this, parent);

  // Search for an existing sequence of type
  PString seqName = "ArrayOf_" + baseType->GetTypeName();

  TypeBase * existingType = Module->FindType(seqName);
  if (existingType != NULL)
    return new DefinedType(this, existingType);

  return new DefinedType(this, seqName);
}


BOOL SequenceOfType::IsPrimitiveType() const
{
  return FALSE;
}


void SequenceOfType::GenerateCplusplus(ostream & hdr, ostream & cxx)
{
  BeginGenerateCplusplus(hdr, cxx);
  cxx << ")\n"
         "{\n";
  GenerateCplusplusConstraints(PString(), hdr, cxx);
  cxx << "}\n"
         "\n"
         "\n";

  PString baseTypeName = baseType->GetTypeName();

  // Generate declarations for generated functions
  hdr << "    PASN_Object * CreateObject() const;\n"
         "    " << baseTypeName << " & operator[](PINDEX i) const";
  if (Module->UsingInlines())
    hdr << " { return (" << baseTypeName << " &)array[i]; }\n";
  else
    hdr << ";\n";

  // Generate implementation for functions
  cxx << GetTemplatePrefix()
      << "PASN_Object * " << GetClassNameString() << "::CreateObject() const\n"
         "{\n";

  if (baseType->HasConstraints()) {
    cxx << "  " << baseTypeName << " * obj = new " << baseTypeName << ";\n";
    baseType->GenerateCplusplusConstraints("obj->", hdr, cxx);
    cxx << "  return obj;\n";
  }
  else
    cxx << "  return new " << baseTypeName << ";\n";

  if (!Module->UsingInlines())
    cxx << "}\n"
           "\n"
           "\n"
        << GetTemplatePrefix()
        << baseTypeName << " & " << GetClassNameString() << "::operator[](PINDEX i) const\n"
           "{\n"
           "  return (" << baseTypeName << " &)array[i];\n";

  EndGenerateCplusplus(hdr, cxx);
}


void SequenceOfType::GenerateForwardDecls(ostream & hdr)
{
  if (baseType->IsParameterizedType())
    return;

  PString baseTypeName = baseType->GetTypeName();

  PStringSet typesOutput(PARRAYSIZE(StandardClasses), StandardClasses);
  typesOutput += GetIdentifier();

  if (!typesOutput.Contains(baseTypeName))
    hdr << "class " << baseTypeName << ";\n\n";
}


const char * SequenceOfType::GetAncestorClass() const
{
  return "PASN_Array";
}


BOOL SequenceOfType::CanReferenceType() const
{
  return TRUE;
}


BOOL SequenceOfType::ReferencesType(const TypeBase & type)
{
  return baseType->ReferencesType(type) && baseType->IsParameterizedType();
}


/////////////////////////////////////////////////////////

SetType::SetType()
  : SequenceType(NULL, FALSE, NULL, Tag::UniversalSet)
{
}


SetType::SetType(SequenceType * seq)
  : SequenceType(*seq)
{
  tag.number = Tag::UniversalSet;
}


const char * SetType::GetAncestorClass() const
{
  return "PASN_Set";
}


/////////////////////////////////////////////////////////

SetOfType::SetOfType(TypeBase * base, Constraint * constraint)
  : SequenceOfType(base, constraint, Tag::UniversalSet)
{
}


/////////////////////////////////////////////////////////

ChoiceType::ChoiceType(TypesList * stnd,
                       BOOL extendable,
                       TypesList * extensions)
  : SequenceType(stnd, extendable, extensions, Tag::IllegalUniversalTag)
{
}

void ChoiceType::GenerateCplusplus(ostream & hdr, ostream & cxx)
{
  PINDEX i;

  // Generate the enum's for each choice discriminator, and include strings for
  // PrintOn() debug output into acncestor constructor
  unsigned namesCount=0;
  int prevNum = -1;
  BOOL outputEnum = FALSE;
  for (i = 0; i < fields.GetSize(); i++) {
    const Tag & fieldTag = fields[i].GetTag();
    if (fieldTag.mode == Tag::Automatic || !fields[i].IsChoice()) {
      if (outputEnum) {
        cxx << "     ,{\"";
      }
      else {
        cxx << "\n"
               "#ifndef PASN_NOPRINTON\n"
               "const static PASN_Names Names_" <<GetIdentifier()<<"[]={\n"
               "      {\"";
        outputEnum = TRUE;
      }

      cxx << fields[i].GetIdentifier() << "\",";

      if (fieldTag.mode != Tag::Automatic)
      {
        cxx <<  fieldTag.number;
        prevNum = fieldTag.number;
      }
      else
        cxx << ++prevNum;
      cxx << "}\n";
      namesCount++;
    }
  }

  if (outputEnum) {
      cxx << "};\n#endif\n";
  }

  BeginGenerateCplusplus(hdr, cxx);

  // Complete the ancestor constructor parameters
  cxx << ", " << numFields << ", " << (extendable ? "TRUE" : "FALSE");

  // Generate the enum's for each choice discriminator, and include strings for
  // PrintOn() debug output into acncestor constructor
  outputEnum = FALSE;
  prevNum = -1;
  for (i = 0; i < fields.GetSize(); i++) {
    const Tag & fieldTag = fields[i].GetTag();
    if (fieldTag.mode == Tag::Automatic || !fields[i].IsChoice()) {
      if (outputEnum) {
        hdr << ",\n";
      }
      else {
        hdr << "    enum Choices {\n";
        outputEnum = TRUE;
      }

      hdr << "      e_" << fields[i].GetIdentifier();

      if (fieldTag.mode != Tag::Automatic && fieldTag.number != (unsigned)(prevNum+1)) {
        hdr << " = " << fieldTag.number;
      }
      prevNum = fieldTag.number;
    }
  }

  if (outputEnum) {
    hdr << "\n"
           "    };\n"
           "\n";
  }
  cxx << "\n"
         "#ifndef PASN_NOPRINTON\n"
         "    ,(const PASN_Names *)Names_" <<GetIdentifier() << "," << namesCount << "\n#endif\n";

  cxx << ")\n"
         "{\n";
  GenerateCplusplusConstraints(PString(), hdr, cxx);
  cxx << "}\n"
         "\n"
         "\n";

  // Generate code for type safe cast operators of selected choice object
  BOOL needExtraLine = FALSE;

  if (Module->UsingOperators()) {
    PStringSet typesOutput(PARRAYSIZE(StandardClasses), StandardClasses);
    typesOutput += GetIdentifier();

    for (i = 0; i < fields.GetSize(); i++) {
      PString type = fields[i].GetTypeName();
      if (!typesOutput.Contains(type)) {
        if (Module->UsingInlines()) {
          hdr << "#if defined(__GNUC__) && __GNUC__ <= 2 && __GNUC_MINOR__ < 9\n"
                 "    operator " << type << " &() const { return *(" << type << " *)choice; }\n"
                 "#else\n"
                 "    operator " << type << " &() { return *(" << type << " *)choice; }\n"
                 "    operator const " << type << " &() const { return *(const " << type << " *)choice; }\n"
                 "#endif\n";
        }
        else {
          hdr << "#if defined(__GNUC__) && __GNUC__ <= 2 && __GNUC_MINOR__ < 9\n"
                 "    operator " << type << " &() const;\n"
                 "#else\n"
                 "    operator " << type << " &();\n"
                 "    operator const " << type << " &() const;\n"
                 "#endif\n";
          cxx << "#if defined(__GNUC__) && __GNUC__ <= 2 && __GNUC_MINOR__ < 9\n"
              << GetTemplatePrefix()
              << GetClassNameString() << "::operator " << type << " &() const\n"
                 "#else\n"
              << GetTemplatePrefix()
              << GetClassNameString() << "::operator " << type << " &()\n"
                 "{\n"
                 "#ifndef PASN_LEANANDMEAN\n"
                 "  PAssert(PIsDescendant(PAssertNULL(choice), " << type << "), PInvalidCast);\n"
                 "#endif\n"
                 "  return *(" << type << " *)choice;\n"
                 "}\n"
                 "\n"
                 "\n"
              << GetTemplatePrefix()
              << GetClassNameString() << "::operator const " << type << " &() const\n"
                 "#endif\n"
                 "{\n"
                 "#ifndef PASN_LEANANDMEAN\n"
                 "  PAssert(PIsDescendant(PAssertNULL(choice), " << type << "), PInvalidCast);\n"
                 "#endif\n"
                 "  return *(" << type << " *)choice;\n"
                 "}\n"
                 "\n"
                 "\n";
        }
        typesOutput += type;
        needExtraLine = TRUE;
      }
    }
  }
  else {
    for (i = 0; i < fields.GetSize(); i++) {
      PString type = fields[i].GetTypeName();
      PString fieldName = fields[i].GetIdentifier();
      if (Module->UsingInlines()) {
        hdr << "    "       << type << " & m_" << fieldName << "() { return *(" << type << " *)choice; }\n"
               "    const " << type << " & m_" << fieldName << "() const { return *(const " << type << " *)choice; }\n";
      }
      else {
        hdr << "    "       << type << " & m_" << fieldName << "();\n"
               "    const " << type << " & m_" << fieldName << "() const;\n";
        cxx << GetTemplatePrefix() << type << " & "
            << GetClassNameString() << "::m_" << fieldName << "()\n"
               "{\n"
               "#ifndef PASN_LEANANDMEAN\n"
               "  PAssert(PIsDescendant(PAssertNULL(choice), " << type << "), PInvalidCast);\n"
               "#endif\n"
               "  return *(" << type << " *)choice;\n"
               "}\n"
               "\n"
               "\n"
            << GetTemplatePrefix() << type << " const & "
            << GetClassNameString() << "::m_" << fieldName << "() const\n"
               "{\n"
               "#ifndef PASN_LEANANDMEAN\n"
               "  PAssert(PIsDescendant(PAssertNULL(choice), " << type << "), PInvalidCast);\n"
               "#endif\n"
               "  return *(" << type << " *)choice;\n"
               "}\n"
               "\n"
               "\n";
      }
    }
    needExtraLine = TRUE;
  }

  if (needExtraLine)
    hdr << '\n';


  // Generate virtual function to create chosen object based on discriminator
  hdr << "    BOOL CreateObject();\n";
  cxx << GetTemplatePrefix()
      << "BOOL " << GetClassNameString() << "::CreateObject()\n"
         "{\n";

  // special case: if choice is all NULLs then simply output code
  BOOL allNull = TRUE;
  for (i = 0; allNull && i < fields.GetSize(); i++) 
    allNull = allNull && strcmp(fields[i].GetAncestorClass(), "PASN_Null") == 0;

  if (allNull) {
    cxx << "  choice = (tag <= e_" << fields[fields.GetSize()-1].GetIdentifier() << ") ? new PASN_Null() : NULL;\n"
        << "  return choice != NULL;\n";
  }

  else {
    // declare an array of flags indicating whether the tag has been output or not
    PBYTEArray flags(fields.GetSize());
    for (i = 0; i < fields.GetSize(); i++)
      flags[i] = 0;

    // keep
    outputEnum = FALSE;
    for (i = 0; i < fields.GetSize(); i++) {

      if (fields[i].GetTag().mode == Tag::Automatic || !fields[i].IsChoice()) {

        // ignore this tag if output previously
        if (flags[i] != 0)
          continue;
    
        if (!outputEnum) {
          cxx << "  switch (tag) {\n";
          outputEnum = TRUE;
        }

        // if the field has constraints, then output it alone
        // otherwise, look for all fields with the same type
        PString name = fields[i].GetTypeName();
        if (fields[i].HasConstraints()) {
          cxx << "    case e_" << fields[i].GetIdentifier() << " :\n";
          flags[i] = 1;
        } else {
          PINDEX j;
          for (j = i; j < fields.GetSize(); j++) {
            if (fields[j].GetTypeName() == name) {
              cxx << "    case e_" << fields[j].GetIdentifier() << " :\n";
              flags[j] = 1;
            }
          }
        }

        cxx << "      choice = new " << name;
        fields[i].GenerateCplusplusConstructor(hdr, cxx);
        cxx << ";\n";
        fields[i].GenerateCplusplusConstraints("    choice->", hdr, cxx);
        cxx << "      return TRUE;\n";
      }
    }

    if (outputEnum)
      cxx << "  }\n"
             "\n";

    for (i = 0; i < fields.GetSize(); i++) {
      if (fields[i].GetTag().mode != Tag::Automatic && fields[i].IsChoice())
        cxx << "  choice = new " << fields[i].GetTypeName() << "(tag, tagClass);\n"
               "  if (((PASN_Choice*)choice)->CreateObject())\n"
               "    return TRUE;\n"
               "  delete choice;\n"
               "\n";
    }

    cxx << "  choice = NULL;\n"
           "  return FALSE;\n";
  }

  EndGenerateCplusplus(hdr, cxx);
}


void ChoiceType::GenerateForwardDecls(ostream & hdr)
{
  // Output forward declarations for choice pointers, but not standard classes
  BOOL needExtraLine = FALSE;

  PStringSet typesOutput(PARRAYSIZE(StandardClasses), StandardClasses);
  typesOutput += GetIdentifier();

  PStringSet forwards;

  for (PINDEX i = 0; i < fields.GetSize(); i++) {
    PString type = fields[i].GetTypeName();
    if (!fields[i].IsParameterizedType() &&
        !typesOutput.Contains(type) &&
        !forwards.Contains(type)) {
      hdr << "class " << type << ";\n";
      needExtraLine = TRUE;
      forwards.Include(type);
    }
  }

  if (needExtraLine)
    hdr << '\n';
}


BOOL ChoiceType::IsPrimitiveType() const
{
  return FALSE;
}


BOOL ChoiceType::IsChoice() const
{
  return TRUE;
}


const char * ChoiceType::GetAncestorClass() const
{
  return "PASN_Choice";
}


BOOL ChoiceType::ReferencesType(const TypeBase & type)
{
  for (PINDEX i = 0; i < fields.GetSize(); i++) {
    if (fields[i].ReferencesType(type) && fields[i].IsParameterizedType())
      return TRUE;
  }
  return FALSE;
}


/////////////////////////////////////////////////////////

EmbeddedPDVType::EmbeddedPDVType()
  : TypeBase(Tag::UniversalEmbeddedPDV)
{
}


const char * EmbeddedPDVType::GetAncestorClass() const
{
  return "PASN_OctetString";
}


/////////////////////////////////////////////////////////

ExternalType::ExternalType()
  : TypeBase(Tag::UniversalExternalType)
{
}


const char * ExternalType::GetAncestorClass() const
{
  return "PASN_OctetString";
}


/////////////////////////////////////////////////////////

AnyType::AnyType(PString * ident)
  : TypeBase(Tag::UniversalExternalType)
{
  if (ident != NULL) {
    identifier = *ident;
    delete ident;
  }
}


void AnyType::PrintOn(ostream & strm) const
{
  PrintStart(strm);
  if (!identifier)
    strm << "Defined by " << identifier;
  PrintFinish(strm);
}


const char * AnyType::GetAncestorClass() const
{
  return "PASN_OctetString";
}


/////////////////////////////////////////////////////////

StringTypeBase::StringTypeBase(int tag)
  : TypeBase(tag)
{
}


int StringTypeBase::GetBraceTokenContext() const
{
  return STRING_BRACE;
}


static void GenerateOperator(const char * rhsType, ostream & hdr, ostream & cxx, const TypeBase & actualType)
{
  hdr << "    " << actualType.GetIdentifier() << " & operator=(const " << rhsType << " v)";
  if (Module->UsingInlines())
    hdr << " { SetValue(v);  return *this; }\n";
  else {
    hdr << ";\n";
    cxx << actualType.GetTemplatePrefix()
        << actualType.GetIdentifier() << " & "
        << actualType.GetClassNameString() << "::operator=(const " << rhsType << " v)\n"
           "{\n"
           "  SetValue(v);\n"
           "  return *this;\n"
           "}\n"
           "\n"
           "\n";
  }
}


void StringTypeBase::GenerateOperators(ostream & hdr, ostream & cxx, const TypeBase & actualType)
{
  GenerateOperator("char *", hdr, cxx, actualType);
  GenerateOperator("PString &", hdr, cxx, actualType);
}


/////////////////////////////////////////////////////////

BMPStringType::BMPStringType()
  : StringTypeBase(Tag::UniversalBMPString)
{
}


const char * BMPStringType::GetAncestorClass() const
{
  return "PASN_BMPString";
}


void BMPStringType::GenerateOperators(ostream & hdr, ostream & cxx, const TypeBase & actualType)
{
  StringTypeBase::GenerateOperators(hdr, cxx, actualType);
  GenerateOperator("PWORDArray &", hdr, cxx, actualType);
  GenerateOperator("PASN_BMPString &", hdr, cxx, actualType);
}


/////////////////////////////////////////////////////////

GeneralStringType::GeneralStringType()
  : StringTypeBase(Tag::UniversalGeneralString)
{
}


const char * GeneralStringType::GetAncestorClass() const
{
  return "PASN_GeneralString";
}


/////////////////////////////////////////////////////////

GraphicStringType::GraphicStringType()
  : StringTypeBase(Tag::UniversalGraphicString)
{
}


const char * GraphicStringType::GetAncestorClass() const
{
  return "PASN_GraphicString";
}


/////////////////////////////////////////////////////////

IA5StringType::IA5StringType()
  : StringTypeBase(Tag::UniversalIA5String)
{
}


const char * IA5StringType::GetAncestorClass() const
{
  return "PASN_IA5String";
}


/////////////////////////////////////////////////////////

ISO646StringType::ISO646StringType()
  : StringTypeBase(Tag::UniversalVisibleString)
{
}


const char * ISO646StringType::GetAncestorClass() const
{
  return "PASN_ISO646String";
}


/////////////////////////////////////////////////////////

NumericStringType::NumericStringType()
  : StringTypeBase(Tag::UniversalNumericString)
{
}


const char * NumericStringType::GetAncestorClass() const
{
  return "PASN_NumericString";
}


/////////////////////////////////////////////////////////

PrintableStringType::PrintableStringType()
  : StringTypeBase(Tag::UniversalPrintableString)
{
}


const char * PrintableStringType::GetAncestorClass() const
{
  return "PASN_PrintableString";
}


/////////////////////////////////////////////////////////

TeletexStringType::TeletexStringType()
  : StringTypeBase(Tag::UniversalTeletexString)
{
}


const char * TeletexStringType::GetAncestorClass() const
{
  return "PASN_TeletexString";
}


/////////////////////////////////////////////////////////

T61StringType::T61StringType()
  : StringTypeBase(Tag::UniversalTeletexString)
{
}


const char * T61StringType::GetAncestorClass() const
{
  return "PASN_T61String";
}


/////////////////////////////////////////////////////////

UniversalStringType::UniversalStringType()
  : StringTypeBase(Tag::UniversalUniversalString)
{
}


const char * UniversalStringType::GetAncestorClass() const
{
  return "PASN_UniversalString";
}


/////////////////////////////////////////////////////////

VideotexStringType::VideotexStringType()
  : StringTypeBase(Tag::UniversalVideotexString)
{
}


const char * VideotexStringType::GetAncestorClass() const
{
  return "PASN_VideotexString";
}


/////////////////////////////////////////////////////////

VisibleStringType::VisibleStringType()
  : StringTypeBase(Tag::UniversalVisibleString)
{
}


const char * VisibleStringType::GetAncestorClass() const
{
  return "PASN_VisibleString";
}


/////////////////////////////////////////////////////////

UnrestrictedCharacterStringType::UnrestrictedCharacterStringType()
  : StringTypeBase(Tag::UniversalUniversalString)
{
}


const char * UnrestrictedCharacterStringType::GetAncestorClass() const
{
  return "PASN_UnrestrictedString";
}


/////////////////////////////////////////////////////////

GeneralizedTimeType::GeneralizedTimeType()
  : TypeBase(Tag::UniversalGeneralisedTime)
{
}


const char * GeneralizedTimeType::GetAncestorClass() const
{
  return "PASN_GeneralisedTime";
}


/////////////////////////////////////////////////////////

UTCTimeType::UTCTimeType()
  : TypeBase(Tag::UniversalUTCTime)
{
}


const char * UTCTimeType::GetAncestorClass() const
{
  return "PASN_UniversalTime";
}


/////////////////////////////////////////////////////////

ObjectDescriptorType::ObjectDescriptorType()
  : TypeBase(Tag::UniversalObjectDescriptor)
{
}


const char * ObjectDescriptorType::GetAncestorClass() const
{
  return "PASN_ObectDescriptor";
}


/////////////////////////////////////////////////////////

ObjectIdentifierType::ObjectIdentifierType()
  : TypeBase(Tag::UniversalObjectId)
{
}


int ObjectIdentifierType::GetIdentifierTokenContext() const
{
  return OID_IDENTIFIER;
}


int ObjectIdentifierType::GetBraceTokenContext() const
{
  return OID_BRACE;
}


const char * ObjectIdentifierType::GetAncestorClass() const
{
  return "PASN_ObjectId";
}


/////////////////////////////////////////////////////////

ObjectClassFieldType::ObjectClassFieldType(PString * objclass, PString * field)
  : TypeBase(Tag::IllegalUniversalTag),
    asnObjectClassName(*objclass),
    asnObjectClassField(*field)
{
  delete objclass;
  delete field;
}


const char * ObjectClassFieldType::GetAncestorClass() const
{
  return "PASN_OctetString";
}


void ObjectClassFieldType::PrintOn(ostream & strm) const
{
  PrintStart(strm);
  strm << asnObjectClassName << '.' << asnObjectClassField;
  PrintFinish(strm);
}


TypeBase * ObjectClassFieldType::FlattenThisType(const TypeBase & parent)
{
  return new DefinedType(this, parent);
}


BOOL ObjectClassFieldType::IsPrimitiveType() const
{
  return FALSE;
}


void ObjectClassFieldType::GenerateCplusplus(ostream & hdr, ostream & cxx)
{
  BeginGenerateCplusplus(hdr, cxx);

  hdr << "    BOOL DecodeSubType(";
  GenerateCplusplusConstraints(PString(), hdr, cxx);
  hdr << " & obj) { return PASN_OctetString::DecodeSubType(obj); }\n"
         "    void EncodeSubType(const ";
  GenerateCplusplusConstraints(PString(), hdr, cxx);
  hdr << " & obj) { PASN_OctetString::EncodeSubType(obj); } \n"
         "\n";

  cxx << ")\n"
         "{\n";

  EndGenerateCplusplus(hdr, cxx);
}


BOOL ObjectClassFieldType::CanReferenceType() const
{
  return TRUE;
}


BOOL ObjectClassFieldType::ReferencesType(const TypeBase & type)
{
  for (PINDEX i = 0; i < constraints.GetSize(); i++) {
    if (constraints[i].ReferencesType(type))
      return TRUE;
  }
  return FALSE;
}


/////////////////////////////////////////////////////////

ImportedType::ImportedType(PString * theName, BOOL param)
  : TypeBase(Tag::IllegalUniversalTag)
{
  identifier = name = *theName;
  delete theName;

  parameterised = param;
}


const char * ImportedType::GetAncestorClass() const
{
  return identifier;
}


void ImportedType::AdjustIdentifier()
{
  identifier = modulePrefix + '_' + MakeIdentifierC(name);
}


void ImportedType::GenerateCplusplus(ostream &, ostream &)
{
}


void ImportedType::SetImportPrefix(const PString & prefix)
{
  modulePrefix = prefix;
}


BOOL ImportedType::IsParameterisedImport() const
{
  return parameterised;
}


/////////////////////////////////////////////////////////

SearchType::SearchType(const PString & theName)
  : TypeBase(Tag::IllegalUniversalTag)
{
  identifier = name = theName;
}


const char * SearchType::GetAncestorClass() const
{
  return identifier;
}


/////////////////////////////////////////////////////////

void ValueBase::SetValueName(PString * name)
{
  valueName = *name;
  delete name;
}


void ValueBase::PrintBase(ostream & strm) const
{
  if (!valueName)
    strm << '\n' << indent() << valueName << '=';
}


void ValueBase::GenerateCplusplus(ostream &, ostream &)
{
  PError << StdError(Warning) << "unsupported value type." << endl;
}


/////////////////////////////////////////////////////////

DefinedValue::DefinedValue(PString * name)
  : referenceName(*name)
{
  delete name;
  actualValue = NULL;
  unresolved = TRUE;
}


void DefinedValue::PrintOn(ostream & strm) const
{
  PrintBase(strm);
  strm << referenceName;
}


void DefinedValue::GenerateCplusplus(ostream & hdr, ostream & cxx)
{
  if (unresolved) {
    unresolved = FALSE;

    const ValuesList & values = Module->GetValues();
    for (PINDEX i = 0; i < values.GetSize(); i++) {
      if (values[i].GetName() == referenceName) {
        actualValue = &values[i];
        break;
      }
    }
  }

  if (actualValue != NULL)
    actualValue->GenerateCplusplus(hdr, cxx);
  else
    cxx << "e_" << referenceName;
}


/////////////////////////////////////////////////////////

BooleanValue::BooleanValue(BOOL newVal)
{
  value = newVal;
}


void BooleanValue::PrintOn(ostream & strm) const
{
  PrintBase(strm);
  strm << (value ? "TRUE" : "FALSE");
}


void BooleanValue::GenerateCplusplus(ostream &, ostream & cxx)
{
  cxx << (value ? "TRUE" : "FALSE");
}


/////////////////////////////////////////////////////////

IntegerValue::IntegerValue(PInt64 newVal)
{
  value = newVal;
}


void IntegerValue::PrintOn(ostream & strm) const
{
  PrintBase(strm);
  
  strm << value;
}


void IntegerValue::GenerateCplusplus(ostream &, ostream & cxx)
{
  cxx << value;
  if (value > INT_MAX)
    cxx << 'U';
}


/////////////////////////////////////////////////////////

RealValue::RealValue(double newVal)
{
  value = newVal;
}


/////////////////////////////////////////////////////////

OctetStringValue::OctetStringValue(PString * newVal)
{
  // value = *newVal;
  delete newVal;
}


/////////////////////////////////////////////////////////

BitStringValue::BitStringValue(PString * newVal)
{
  // value = *newVal;
  delete newVal;
}


BitStringValue::BitStringValue(PStringList * newVal)
{
  // value = *newVal;
  delete newVal;
}


/////////////////////////////////////////////////////////

CharacterValue::CharacterValue(BYTE c)
{
  value = c;
}


CharacterValue::CharacterValue(BYTE t1, BYTE t2)
{
  value = (t1<<8) + t2;
}


CharacterValue::CharacterValue(BYTE q1, BYTE q2, BYTE q3, BYTE q4)
{
  value = (q1<<24) + (q2<<16) + (q3<<8) + q4;
}


void CharacterValue::PrintOn(ostream & strm) const
{
  strm << "'\\x" << hex << value << '\'';
}


void CharacterValue::GenerateCplusplus(ostream &, ostream & cxx)
{
  cxx << value;
}


/////////////////////////////////////////////////////////

CharacterStringValue::CharacterStringValue(PString * newVal)
{
  value = *newVal;
  delete newVal;
}


CharacterStringValue::CharacterStringValue(PStringList * newVal)
{
  for (PINDEX i = 0; i < newVal->GetSize(); i++)
    value += (*newVal)[i];
  delete newVal;
}


void CharacterStringValue::PrintOn(ostream & strm) const
{
  strm << value;
}


void CharacterStringValue::GenerateCplusplus(ostream &, ostream & cxx)
{
  cxx << value;
}


/////////////////////////////////////////////////////////

ObjectIdentifierValue::ObjectIdentifierValue(PString * newVal)
{
  value.Append(newVal);
}


ObjectIdentifierValue::ObjectIdentifierValue(PStringList * newVal)
{
  value = *newVal;
  delete newVal;
}


void ObjectIdentifierValue::PrintOn(ostream & strm) const
{
  PrintBase(strm);
  if (value.IsEmpty())
    strm << "empty object identifier";
  else {
    strm << value[0];
    for (PINDEX i = 1; i < value.GetSize(); i++)
      strm << '.' << value[i];
  }
  strm << '\n';
}


/////////////////////////////////////////////////////////

void MinValue::PrintOn(ostream & strm) const
{
  strm << "MIN";
}


void MinValue::GenerateCplusplus(ostream &, ostream & cxx)
{
  cxx << "MinimumValue";
}


/////////////////////////////////////////////////////////

void MaxValue::PrintOn(ostream & strm) const
{
  strm << "MAX";
}


void MaxValue::GenerateCplusplus(ostream &, ostream & cxx)
{
  cxx << "MaximumValue";
}


/////////////////////////////////////////////////////////

SequenceValue::SequenceValue(ValuesList * list)
{
  if (list != NULL) {
    values = *list;
    delete list;
  }
}


void SequenceValue::PrintOn(ostream & strm) const
{
  strm << "{ ";
  for (PINDEX i = 0; i < values.GetSize(); i++) {
    if (i > 0)
      strm << ", ";
    strm << values[i];
  }
  strm << " }";
}


/////////////////////////////////////////////////////////

MibBase::MibBase(PString * nam,
                 PString * descr,
                 PString * refer,
                 ValueBase * val)
  : name(*nam)
{
  delete nam;
  if (descr != NULL) {
    description = *descr;
    delete descr;
  }
  if (refer != NULL) {
    reference = *refer;
    delete refer;
  }
  value = val;
}


MibBase::~MibBase()
{
  delete value;
}


/////////////////////////////////////////////////////////

MibObject::MibObject(PString * nam,
                     TypeBase * typ,
                     Access acc,
                     Status stat,
                     PString * descr,
                     PString * refer,
                     PStringList * idx,
                     ValueBase * defVal,
                     ValueBase * setVal)
  : MibBase(nam, descr, refer, setVal)
{
  type = typ;
  access = acc;
  status = stat;
  if (idx != NULL) {
    index = *idx;
    delete idx;
  }
  defaultValue = defVal;
}


MibObject::~MibObject()
{
  delete type;
  delete defaultValue;
}


void MibObject::PrintOn(ostream & strm) const
{
  strm << "  Object: " << name << "\n  " << *type
       << "    " << description << "\n"
          "    " << *value << '\n';
}


/////////////////////////////////////////////////////////

MibTrap::MibTrap(PString * nam, ValueBase * ent, ValuesList * var,
                 PString * descr, PString * refer, ValueBase * val)
  : MibBase(nam, descr, refer, val)
{
  enterprise = ent;
  if (var != NULL) {
    variables = *var;
    delete var;
  }
}


MibTrap::~MibTrap()
{
  delete enterprise;
}


void MibTrap::PrintOn(ostream & strm) const
{
  strm << "  Trap: " << name << "\n  " << *enterprise
       << "    " << description << "\n"
          "    " << *value << '\n';
}


/////////////////////////////////////////////////////////

ImportModule::ImportModule(PString * name, TypesList * syms)
  : fullModuleName(*name),
    shortModuleName(Module->GetImportModuleName(*name)),
    filename(shortModuleName.ToLower())
{
  delete name;
  symbols = *syms;
  delete syms;

  PStringArray renameArgs = shortModuleName.Tokenise(',');
  switch (renameArgs.GetSize())
  {
    case 3 :
      filename = renameArgs[2];
    case 2 :
      directoryPrefix = renameArgs[1];
      shortModuleName = renameArgs[0];
  }

  for (PINDEX i = 0; i < symbols.GetSize(); i++) {
    symbols[i].SetImportPrefix(shortModuleName);
    Module->AppendType(&symbols[i]);
  }
}


void ImportModule::PrintOn(ostream & strm) const
{
  strm << "  " << fullModuleName << " (" << shortModuleName << "):\n";
  for (PINDEX i = 0; i < symbols.GetSize(); i++)
    strm << "    " << symbols[i];
  strm << '\n';
}


void ImportModule::GenerateCplusplus(ostream & hdr, ostream & cxx)
{
  hdr << "#include \"" << directoryPrefix << filename << ".h\"\n";

  for (PINDEX i = 0; i < symbols.GetSize(); i++) {
    if (symbols[i].IsParameterisedImport()) {
      cxx << "#include \"" << filename << "_t.cxx\"\n";
      break;
    }
  }
}


/////////////////////////////////////////////////////////

ModuleDefinition::ModuleDefinition(PString * name, PStringList * id, Tag::Mode defTagMode)
  : moduleName(*name),
    definitiveId(*id)
{
  delete name;
  delete id;
  defaultTagMode = defTagMode;
  exportAll = FALSE;
  indentLevel = 1;

  PArgList & args = PProcess::Current().GetArguments();
  if (args.HasOption('r')) {
    PStringArray renames = args.GetOptionString('r').Lines();
    for (PINDEX i = 0; i < renames.GetSize(); i++) {
      PINDEX equal = renames[i].Find('=');
      if (equal > 0 && equal != P_MAX_INDEX)
        importNames.SetAt(renames[i].Left(equal).Trim(), renames[i].Mid(equal+1).Trim());
    }
  }
}


void ModuleDefinition::SetExportAll()
{
  exportAll = TRUE;
}


void ModuleDefinition::SetExports(TypesList * syms)
{
  exports = *syms;
  delete syms;
}


void ModuleDefinition::PrintOn(ostream & strm) const
{
  strm << moduleName << "\n"
          "Default Tags: " << Tag::modeNames[defaultTagMode] << "\n"
          "Exports:";
  if (exportAll)
    strm << " ALL";
  else {
    strm << "\n  ";
    for (PINDEX i = 0; i < exports.GetSize(); i++)
      strm << exports[i] << ' ';
    strm << '\n';
  }
  strm << "Imports:\n" << imports << "\n"
          "Types:\n" << types << "\n"
          "Values:\n" << values << "\n"
          "MIBs:\n" << mibs << endl;
}


void ModuleDefinition::AppendType(TypeBase * type)
{
  types.Append(type);
  sortedTypes.Append(type);
}


TypeBase * ModuleDefinition::FindType(const PString & name)
{
  PINDEX pos = sortedTypes.GetValuesIndex(SearchType(name));
  if (pos != P_MAX_INDEX)
    return &sortedTypes[pos];
  return NULL;
}


PString ModuleDefinition::GetImportModuleName(const PString & moduleName)
{
  if (importNames.Contains(moduleName))
    return importNames[moduleName];

  PINDEX pos = moduleName.Find('-');
  if (pos > 32)
    pos = 32;
  return moduleName.Left(pos);
}


void ModuleDefinition::GenerateCplusplus(const PFilePath & path,
                                         const PString & modName,
                                         const PString & headerPrefix,
                                         unsigned numFiles,
                                         BOOL useNamespaces,
                                         BOOL useInlines,
                                         BOOL useOperators,
                                         BOOL verbose)
{
  PArgList & args = PProcess::Current().GetArguments();
  BOOL xml_output = args.HasOption('x');
  PINDEX i;

  usingInlines = useInlines;
  usingOperators = useOperators;

  // Adjust the module name to what is specified to a default
  if (!modName)
    moduleName = modName;
  else
    moduleName = MakeIdentifierC(moduleName);

  // Set the prefix on all external class names
  if (!useNamespaces)
    classNamePrefix = moduleName + '_';


  if (verbose)
    cout << "Sorting " << types.GetSize() << " types..." << endl;


  // Create sorted list for faster searching.
  sortedTypes.DisallowDeleteObjects();
  for (i = 0; i < types.GetSize(); i++)
    sortedTypes.Append(&types[i]);

  // Flatten types by generating types for "inline" definitions
  for (i = 0; i < types.GetSize(); i++)
    types[i].FlattenUsedTypes();

  // Reorder types
  // Determine if we need a separate file for template closure
  BOOL hasTemplates = FALSE;
  types.DisallowDeleteObjects();
  PINDEX loopDetect = 0;
  PINDEX bubble = 0;
  while (bubble < types.GetSize()) {
    BOOL makesReference = FALSE;

    TypeBase & bubbleType = types[bubble];
    if (bubbleType.CanReferenceType()) {
      for (i = bubble; i < types.GetSize(); i++) {
        if (bubbleType.ReferencesType(types[i])) {
          makesReference = TRUE;
          break;
        }
      }
    }

    if (makesReference) {
      types.Append(types.RemoveAt(bubble));
      if (loopDetect > types.GetSize()) {
        PError << StdError(Fatal)
               << "Recursive type definition: " << bubbleType.GetName() << endl;
        break;
      }
      loopDetect++;
    }
    else {
      loopDetect = bubble;
      bubble++;
    }

    if (bubbleType.HasParameters())
      hasTemplates = TRUE;
  }
  types.AllowDeleteObjects();

  // Adjust all of the C++ identifiers prepending module name
  for (i = 0; i < types.GetSize(); i++)
    types[i].AdjustIdentifier();


  // Generate the code
  if (verbose)
    cout << "Generating code (" << types.GetSize() << " classes) ..." << endl;


  // Output the special template closure file, if necessary
  PString templateFilename;
  if (hasTemplates) {
    OutputFile templateFile;
    if (!templateFile.Open(path, "_t", ".cxx"))
      return;

    for (i = 0; i < types.GetSize(); i++) {
      if (types[i].HasParameters()) {
        PStringStream dummy;
        types[i].GenerateCplusplus(dummy, templateFile);
      }
    }

    if (verbose)
      cout << "Completed " << templateFile.GetFilePath() << endl;

    templateFilename = templateFile.GetFilePath().GetFileName();
  }

  // Start the header file
  OutputFile hdrFile;
  if (!hdrFile.Open(path, "", ".h"))
    return;

  hdrFile << "#if ! H323_DISABLE_" << moduleName.ToUpper() << "\n\n";

  hdrFile << "#ifndef __" << moduleName.ToUpper() << "_H\n"
             "#define __" << moduleName.ToUpper() << "_H\n"
             "\n"
             "#ifdef P_USE_PRAGMA\n"
             "#pragma interface\n"
             "#endif\n"
             "\n";

  if (xml_output)
    hdrFile << "#define P_EXPAT 1\n"
               "#include <ptclib/pxml.h>\n";

  hdrFile << "#include <ptclib/asner.h>\n"
             "\n";

  // Start the first (and maybe only) cxx file
  OutputFile cxxFile;
  if (!cxxFile.Open(path, numFiles > 1 ? "_1" : "", ".cxx"))
    return;

  PString headerName = hdrFile.GetFilePath().GetFileName();

  cxxFile << "#ifdef P_USE_PRAGMA\n"
             "#pragma implementation \"" << headerName << "\"\n"
             "#endif\n"
             "\n"
             "#include <ptlib.h>\n"
             "#include \"" << headerPrefix << headerName << "\"\n"
             "\n"
             "#define new PNEW\n"
             "\n"
             "\n";

  cxxFile << "#if ! H323_DISABLE_" << moduleName.ToUpper() << "\n\n";

  // Include the template closure file.
  if (hasTemplates)
    cxxFile << "#include \"" << templateFilename << "\"\n\n";


  for (i = 0; i < imports.GetSize(); i++)
    imports[i].GenerateCplusplus(hdrFile, cxxFile);
  if (!imports.IsEmpty()) {
    hdrFile << "\n\n";
    cxxFile << "\n\n";
  }


  if (useNamespaces) {
    hdrFile << "namespace " << moduleName << " {\n"
               "\n";
    cxxFile << "using namespace " << moduleName << ";\n"
               "\n";
  }


  PINDEX classesPerFile = (types.GetSize()+numFiles-1)/numFiles;
  for (i = 0; i < types.GetSize(); i++) {
    if (i > 0 && i%classesPerFile == 0) {

      cxxFile << "#endif // if ! H323_DISABLE_" << moduleName.ToUpper() << "\n"
                 "\n";

      cxxFile.Close();

      if (verbose)
        cout << "Completed " << cxxFile.GetFilePath() << endl;

      if (!cxxFile.Open(path, psprintf("_%u", i/classesPerFile+1), ".cxx"))
        return;

      cxxFile << "#include <ptlib.h>\n"
                 "#include \"" << headerPrefix << headerName << "\"\n"
                 "\n";

      if (useNamespaces)
        cxxFile << "using namespace " << moduleName << ";\n"
                   "\n";
      cxxFile << "#define new PNEW\n"
                 "\n"
                 "\n";

      cxxFile << "#if ! H323_DISABLE_" << moduleName.ToUpper() << "\n\n";

    }

    if (types[i].HasParameters()) {
      PStringStream dummy;
      types[i].GenerateCplusplus(hdrFile, dummy);
    }
    else
      types[i].GenerateCplusplus(hdrFile, cxxFile);
  }


  // Close off the files
  if (useNamespaces)
    hdrFile << "};\n"
               "\n";

  hdrFile << "#endif // __" << moduleName.ToUpper() << "_H\n"
             "\n";

  hdrFile << "#endif // if ! H323_DISABLE_" << moduleName.ToUpper() << "\n"
             "\n";

  cxxFile << "#endif // if ! H323_DISABLE_" << moduleName.ToUpper() << "\n"
             "\n";

  if (verbose)
    cout << "Completed " << cxxFile.GetFilePath() << endl;
}


//////////////////////////////////////////////////////////////////////////////
