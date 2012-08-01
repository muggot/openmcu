/*
 * main.h
 *
 * PWLib application header file for asnparser
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
 * $Log: main.h,v $
 * Revision 1.15  2003/10/03 00:13:04  rjongbloed
 * Added ability to specify CHOICE field selection by function rather than operator as the operator technique does not work with some dumb compilers.
 * Added ability to specify that the header file name be different from the module name and module prefix string.
 *
 * Revision 1.14  2003/02/27 04:04:14  robertj
 * Added ability to have alternate directories for header file
 *   includes in generated C++ code.
 *
 * Revision 1.13  2001/08/03 09:01:02  robertj
 * Added assignement operator with RHS of PWORDArray to classes
 *   descended from PASN_BMPString.
 *
 * Revision 1.12  2001/06/14 02:09:20  robertj
 * Corrected support for ASN object class type reference constructs
 *   ie TYPE-IDENTIFIER.&Type encoded as octet string.
 *
 * Revision 1.11  2000/03/21 21:23:24  robertj
 * Added option to rename imported module names, allows include filename matching.
 *
 * Revision 1.10  1999/09/18 04:17:40  robertj
 * Added generation of C++ inlines for some  functions.
 * Optimised CreateObject() switch statements, collapsing common cases.
 *
 * Revision 1.9  1999/08/09 13:02:36  robertj
 * Added ASN compiler #defines for backward support of pre GCC 2.9 compilers.
 * Added ASN compiler #defines to reduce its memory footprint.
 * Added ASN compiler code generation of assignment operators for string classes.
 *
 * Revision 1.8  1999/07/22 06:48:56  robertj
 * Added comparison operation to base ASN classes and compiled ASN code.
 * Added support for ANY type in ASN parser.
 *
 * Revision 1.7  1999/06/30 08:57:20  robertj
 * Fixed bug in encodeing sequence of constrained primitive type. Constraint not set.
 * Fixed bug in not emitting namespace use clause.
 * Added "normalisation" of separate sequence of <base type> to be single class.
 *
 * Revision 1.6  1999/06/09 06:58:09  robertj
 * Adjusted heading comments.
 *
 * Revision 1.5  1999/06/07 01:56:26  robertj
 * Added header comment on license.
 *
 */

#ifndef _MAIN_H
#define _MAIN_H

extern unsigned lineNumber;
extern PString  fileName;
extern FILE * yyin;
extern int yyparse();

void yyerror(char * str);


/////////////////////////////////////////
//
//  standard error output from parser
//

enum StdErrorType { Warning, Fatal };

class StdError {
  public:
    StdError(StdErrorType ne) : e(ne) { }
    //StdError(StdErrorType ne, unsigned ln) : e(ne), l(ln) { }
    friend ostream & operator<<(ostream & out, const StdError & e);

  protected:
    StdErrorType e;
    //unsigned     l;
};


/////////////////////////////////////////
//
//  intermediate structures from parser
//


class NamedNumber : public PObject
{
    PCLASSINFO(NamedNumber, PObject);
  public:
    NamedNumber(PString * nam);
    NamedNumber(PString * nam, int num);
    NamedNumber(PString * nam, const PString & ref);
    void PrintOn(ostream &) const;

    void SetAutoNumber(const NamedNumber & prev);
    const PString & GetName() const { return name; }
    int GetNumber() const { return number; }

  protected:
    PString name;
    PString reference;
    int number;
    BOOL autonumber;
};

PLIST(NamedNumberList, NamedNumber);


// Types

class TypeBase;

PLIST(TypesList, TypeBase);
PSORTED_LIST(SortedTypesList, TypeBase);

class Tag : public PObject
{
    PCLASSINFO(Tag, PObject);
  public:
    enum Type {
      Universal,
      Application,
      ContextSpecific,
      Private
    };
    enum UniversalTags {
      IllegalUniversalTag,
      UniversalBoolean,
      UniversalInteger,
      UniversalBitString,
      UniversalOctetString,
      UniversalNull,
      UniversalObjectId,
      UniversalObjectDescriptor,
      UniversalExternalType,
      UniversalReal,
      UniversalEnumeration,
      UniversalEmbeddedPDV,
      UniversalSequence = 16,
      UniversalSet,
      UniversalNumericString,
      UniversalPrintableString,
      UniversalTeletexString,
      UniversalVideotexString,
      UniversalIA5String,
      UniversalUTCTime,
      UniversalGeneralisedTime,
      UniversalGraphicString,
      UniversalVisibleString,
      UniversalGeneralString,
      UniversalUniversalString,
      UniversalBMPString = 30
    };
    enum Mode {
      Implicit,
      Explicit,
      Automatic
    };
    Tag(unsigned tagNum);

    void PrintOn(ostream &) const;

    Type type;
    unsigned number;
    Mode mode;

    static const char * classNames[];
    static const char * modeNames[];
};


class ConstraintElementBase;

PLIST(ConstraintElementList, ConstraintElementBase);


class Constraint : public PObject
{
    PCLASSINFO(Constraint, PObject);
  public:
    Constraint(ConstraintElementBase * elmt);
    Constraint(ConstraintElementList * std, BOOL extend, ConstraintElementList * ext);

    void PrintOn(ostream &) const;

    BOOL IsExtendable() const { return extendable; }
    void GenerateCplusplus(const PString & fn, ostream & hdr, ostream & cxx);
    BOOL ReferencesType(const TypeBase & type);

  protected:
    ConstraintElementList standard;
    BOOL                  extendable;
    ConstraintElementList extensions;
};

PLIST(ConstraintList, Constraint);


class ConstraintElementBase : public PObject
{
    PCLASSINFO(ConstraintElementBase, PObject);
  public:
    ConstraintElementBase();
    void SetExclusions(ConstraintElementBase * excl) { exclusions = excl; }

    virtual void GenerateCplusplus(const PString & fn, ostream & hdr, ostream & cxx);
    virtual BOOL ReferencesType(const TypeBase & type);

  protected:
    ConstraintElementBase * exclusions;
};


class ConstrainAllConstraintElement : public ConstraintElementBase
{
    PCLASSINFO(ConstrainAllConstraintElement, ConstraintElementBase);
  public:
    ConstrainAllConstraintElement(ConstraintElementBase * excl);
};



class ElementListConstraintElement : public ConstraintElementBase
{
    PCLASSINFO(ElementListConstraintElement, ConstraintElementBase);
  public:
    ElementListConstraintElement(ConstraintElementList * list);
    void PrintOn(ostream &) const;

    virtual void GenerateCplusplus(const PString & fn, ostream & hdr, ostream & cxx);
    virtual BOOL ReferencesType(const TypeBase & type);

  protected:
    ConstraintElementList elements;
};


class ValueBase;

class SingleValueConstraintElement : public ConstraintElementBase
{
    PCLASSINFO(SingleValueConstraintElement, ConstraintElementBase);
  public:
    SingleValueConstraintElement(ValueBase * val);
    ~SingleValueConstraintElement();
    void PrintOn(ostream &) const;

    virtual void GenerateCplusplus(const PString & fn, ostream & hdr, ostream & cxx);

  protected:
    ValueBase * value;
};


class ValueRangeConstraintElement : public ConstraintElementBase
{
    PCLASSINFO(ValueRangeConstraintElement, ConstraintElementBase);
  public:
    ValueRangeConstraintElement(ValueBase * lowerBound, ValueBase * upperBound);
    ~ValueRangeConstraintElement();
    void PrintOn(ostream &) const;

    virtual void GenerateCplusplus(const PString & fn, ostream & hdr, ostream & cxx);

  protected:
    ValueBase * lower;
    ValueBase * upper;
};


class TypeBase;

class SubTypeConstraintElement : public ConstraintElementBase
{
    PCLASSINFO(SubTypeConstraintElement, ConstraintElementBase);
  public:
    SubTypeConstraintElement(TypeBase * typ);
    ~SubTypeConstraintElement();
    void PrintOn(ostream &) const;
    void GenerateCplusplus(const PString &, ostream &, ostream &);
    virtual BOOL ReferencesType(const TypeBase & type);
  protected:
    TypeBase * subtype;
};


class NestedConstraintConstraintElement : public ConstraintElementBase
{
    PCLASSINFO(NestedConstraintConstraintElement, ConstraintElementBase);
  public:
    NestedConstraintConstraintElement(Constraint * con);
    ~NestedConstraintConstraintElement();

    virtual BOOL ReferencesType(const TypeBase & type);

  protected:
    Constraint * constraint;
};


class SizeConstraintElement : public NestedConstraintConstraintElement
{
    PCLASSINFO(SizeConstraintElement, NestedConstraintConstraintElement);
  public:
    SizeConstraintElement(Constraint * constraint);
    void PrintOn(ostream &) const;
    virtual void GenerateCplusplus(const PString & fn, ostream & hdr, ostream & cxx);
};


class FromConstraintElement : public NestedConstraintConstraintElement
{
    PCLASSINFO(FromConstraintElement, NestedConstraintConstraintElement);
  public:
    FromConstraintElement(Constraint * constraint);
    void PrintOn(ostream &) const;
    virtual void GenerateCplusplus(const PString & fn, ostream & hdr, ostream & cxx);
};


class WithComponentConstraintElement : public NestedConstraintConstraintElement
{
    PCLASSINFO(WithComponentConstraintElement, NestedConstraintConstraintElement);
  public:
    WithComponentConstraintElement(PString * name, Constraint * constraint, int presence);
    void PrintOn(ostream &) const;
    virtual void GenerateCplusplus(const PString & fn, ostream & hdr, ostream & cxx);

    enum {
      Present,
      Absent,
      Optional,
      Default
    };

  protected:
    PString name;
    int     presence;
};


class InnerTypeConstraintElement : public ElementListConstraintElement
{
    PCLASSINFO(InnerTypeConstraintElement, ElementListConstraintElement);
  public:
    InnerTypeConstraintElement(ConstraintElementList * list, BOOL partial);

    void PrintOn(ostream &) const;
    virtual void GenerateCplusplus(const PString & fn, ostream & hdr, ostream & cxx);

  protected:
    BOOL partial;
};


class UserDefinedConstraintElement : public ConstraintElementBase
{
    PCLASSINFO(UserDefinedConstraintElement, ConstraintElementBase);
  public:
    UserDefinedConstraintElement(TypesList * types);
    void PrintOn(ostream &) const;
    virtual void GenerateCplusplus(const PString & fn, ostream & hdr, ostream & cxx);
  protected:
    TypesList types;
};



class TypeBase : public PObject
{
    PCLASSINFO(TypeBase, PObject);
  public:
    Comparison Compare(const PObject & obj) const;
    void PrintOn(ostream &) const;

    virtual int GetIdentifierTokenContext() const;
    virtual int GetBraceTokenContext() const;

    const PString & GetName() const { return name; }
    void SetName(PString * name);
    PString GetIdentifier() const { return identifier; }
    void SetTag(Tag::Type cls, unsigned num, Tag::Mode mode);
    const Tag & GetTag() const { return tag; }
    BOOL HasNonStandardTag() const { return tag != defaultTag; }
    void SetParameters(PStringList * list);
    void AddConstraint(Constraint * constraint) { constraints.Append(constraint); }
    BOOL HasConstraints() const { return constraints.GetSize() > 0; }
    void MoveConstraints(TypeBase * from);
    BOOL HasParameters() const { return !parameters.IsEmpty(); }
    BOOL IsOptional() const { return isOptional; }
    void SetOptional() { isOptional = TRUE; }
    void SetDefaultValue(ValueBase * value) { defaultValue = value; }
    const PString & GetTemplatePrefix() const { return templatePrefix; }
    const PString & GetClassNameString() const { return classNameString; }

    virtual void AdjustIdentifier();
    virtual void FlattenUsedTypes();
    virtual TypeBase * FlattenThisType(const TypeBase & parent);
    virtual BOOL IsChoice() const;
    virtual BOOL IsParameterizedType() const;
    virtual BOOL IsPrimitiveType() const;
    virtual void GenerateCplusplus(ostream & hdr, ostream & cxx);
    virtual void GenerateForwardDecls(ostream & hdr);
    virtual void GenerateOperators(ostream & hdr, ostream & cxx, const TypeBase & actualType);
    virtual const char * GetAncestorClass() const = 0;
    virtual PString GetTypeName() const;
    virtual BOOL CanReferenceType() const;
    virtual BOOL ReferencesType(const TypeBase & type);
    virtual void SetImportPrefix(const PString &);
    virtual BOOL IsParameterisedImport() const;

    BOOL IsGenerated() const { return isGenerated; }
    void BeginGenerateCplusplus(ostream & hdr, ostream & cxx);
    void EndGenerateCplusplus(ostream & hdr, ostream & cxx);
    void GenerateCplusplusConstructor(ostream & hdr, ostream & cxx);
    void GenerateCplusplusConstraints(const PString & prefix, ostream & hdr, ostream & cxx);

  protected:
    TypeBase(unsigned tagNum);
    TypeBase(TypeBase * copy);

    void PrintStart(ostream &) const;
    void PrintFinish(ostream &) const;

    PString        name;
    PString        identifier;
    Tag            tag;
    Tag            defaultTag;
    ConstraintList constraints;
    BOOL           isOptional;
    ValueBase    * defaultValue;
    BOOL           isGenerated;
    PStringList    parameters;
    PString        templatePrefix;
    PString        classNameString;
};


class DefinedType : public TypeBase
{
    PCLASSINFO(DefinedType, TypeBase);
  public:
    DefinedType(PString * name, BOOL parameter);
    DefinedType(TypeBase * refType, TypeBase * bType);
    DefinedType(TypeBase * refType, const PString & name);
    DefinedType(TypeBase * refType, const TypeBase & parent);

    void PrintOn(ostream &) const;

    virtual BOOL IsChoice() const;
    virtual BOOL IsParameterizedType() const;
    virtual void GenerateOperators(ostream & hdr, ostream & cxx, const TypeBase & actualType);
    virtual const char * GetAncestorClass() const;
    virtual PString GetTypeName() const;
    virtual BOOL CanReferenceType() const;
    virtual BOOL ReferencesType(const TypeBase & type);

  protected:
    void ConstructFromType(TypeBase * refType, const PString & name);

    PString referenceName;
    TypeBase * baseType;
    BOOL unresolved;
};


class ParameterizedType : public DefinedType
{
    PCLASSINFO(ParameterizedType, DefinedType);
  public:
    ParameterizedType(PString * name, TypesList * args);

    void PrintOn(ostream &) const;

    virtual BOOL IsParameterizedType() const;
    virtual PString GetTypeName() const;
    virtual BOOL ReferencesType(const TypeBase & type);

  protected:
    TypesList arguments;
};


class SelectionType : public TypeBase
{
    PCLASSINFO(SelectionType, TypeBase);
  public:
    SelectionType(PString * name, TypeBase * base);
    ~SelectionType();

    void PrintOn(ostream &) const;

    virtual void FlattenUsedTypes();
    virtual TypeBase * FlattenThisType(const TypeBase & parent);
    virtual void GenerateCplusplus(ostream & hdr, ostream & cxx);
    virtual const char * GetAncestorClass() const;
    virtual BOOL CanReferenceType() const;
    virtual BOOL ReferencesType(const TypeBase & type);

  protected:
    PString selection;
    TypeBase * baseType;
};


class BooleanType : public TypeBase
{
    PCLASSINFO(BooleanType, TypeBase);
  public:
    BooleanType();
    virtual void GenerateOperators(ostream & hdr, ostream & cxx, const TypeBase & actualType);
    virtual const char * GetAncestorClass() const;
};


class IntegerType : public TypeBase
{
    PCLASSINFO(IntegerType, TypeBase);
  public:
    IntegerType();
    IntegerType(NamedNumberList *);
    virtual void GenerateOperators(ostream & hdr, ostream & cxx, const TypeBase & actualType);
    virtual const char * GetAncestorClass() const;
  protected:
    NamedNumberList allowedValues;
};


class EnumeratedType : public TypeBase
{
    PCLASSINFO(EnumeratedType, TypeBase);
  public:
    EnumeratedType(NamedNumberList * enums, BOOL extend, NamedNumberList * ext);
    void PrintOn(ostream &) const;
    virtual TypeBase * FlattenThisType(const TypeBase & parent);
    virtual void GenerateCplusplus(ostream & hdr, ostream & cxx);
    virtual void GenerateOperators(ostream & hdr, ostream & cxx, const TypeBase & actualType);
    virtual const char * GetAncestorClass() const;
  protected:
    NamedNumberList enumerations;
    PINDEX numEnums;
    BOOL extendable;
};


class RealType : public TypeBase
{
    PCLASSINFO(RealType, TypeBase);
  public:
    RealType();
    virtual const char * GetAncestorClass() const;
};


class BitStringType : public TypeBase
{
    PCLASSINFO(BitStringType, TypeBase);
  public:
    BitStringType();
    BitStringType(NamedNumberList *);
    virtual int GetIdentifierTokenContext() const;
    virtual int GetBraceTokenContext() const;
    virtual const char * GetAncestorClass() const;
  protected:
    NamedNumberList allowedBits;
};


class OctetStringType : public TypeBase
{
    PCLASSINFO(OctetStringType, TypeBase);
  public:
    OctetStringType();
    virtual void GenerateOperators(ostream & hdr, ostream & cxx, const TypeBase & actualType);
    virtual const char * GetAncestorClass() const;
};


class NullType : public TypeBase
{
    PCLASSINFO(NullType, TypeBase);
  public:
    NullType();
    virtual const char * GetAncestorClass() const;
};


class SequenceType : public TypeBase
{
    PCLASSINFO(SequenceType, TypeBase);
    void PrintOn(ostream &) const;
  public:
    SequenceType(TypesList * std,
                 BOOL extendable,
                 TypesList * extensions,
                 unsigned tagNum = Tag::UniversalSequence);
    virtual void FlattenUsedTypes();
    virtual TypeBase * FlattenThisType(const TypeBase & parent);
    virtual BOOL IsPrimitiveType() const;
    virtual void GenerateCplusplus(ostream & hdr, ostream & cxx);
    virtual const char * GetAncestorClass() const;
    virtual BOOL CanReferenceType() const;
    virtual BOOL ReferencesType(const TypeBase & type);
  protected:
    TypesList fields;
    PINDEX numFields;
    BOOL extendable;
};


class SequenceOfType : public TypeBase
{
    PCLASSINFO(SequenceOfType, TypeBase);
  public:
    SequenceOfType(TypeBase * base, Constraint * constraint, unsigned tag = Tag::UniversalSequence);
    ~SequenceOfType();
    void PrintOn(ostream &) const;
    virtual void FlattenUsedTypes();
    virtual TypeBase * FlattenThisType(const TypeBase & parent);
    virtual BOOL IsPrimitiveType() const;
    virtual void GenerateCplusplus(ostream & hdr, ostream & cxx);
    virtual void GenerateForwardDecls(ostream & hdr);
    virtual const char * GetAncestorClass() const;
    virtual BOOL CanReferenceType() const;
    virtual BOOL ReferencesType(const TypeBase & type);
  protected:
    TypeBase * baseType;
};


class SetType : public SequenceType
{
    PCLASSINFO(SetType, SequenceType);
  public:
    SetType();
    SetType(SequenceType * seq);
    virtual const char * GetAncestorClass() const;
};


class SetOfType : public SequenceOfType
{
    PCLASSINFO(SetOfType, SequenceOfType);
  public:
    SetOfType(TypeBase * base, Constraint * constraint);
};


class ChoiceType : public SequenceType
{
    PCLASSINFO(ChoiceType, SequenceType);
  public:
    ChoiceType(TypesList * std = NULL,
               BOOL extendable = FALSE,
               TypesList * extensions = NULL);
    virtual void GenerateCplusplus(ostream & hdr, ostream & cxx);
    virtual void GenerateForwardDecls(ostream & hdr);
    virtual BOOL IsPrimitiveType() const;
    virtual BOOL IsChoice() const;
    virtual const char * GetAncestorClass() const;
    virtual BOOL ReferencesType(const TypeBase & type);
};


class EmbeddedPDVType : public TypeBase
{
    PCLASSINFO(EmbeddedPDVType, TypeBase);
  public:
    EmbeddedPDVType();
    virtual const char * GetAncestorClass() const;
};


class ExternalType : public TypeBase
{
    PCLASSINFO(ExternalType, TypeBase);
  public:
    ExternalType();
    virtual const char * GetAncestorClass() const;
};


class AnyType : public TypeBase
{
    PCLASSINFO(AnyType, TypeBase);
  public:
    AnyType(PString * ident);
    void PrintOn(ostream & strm) const;
    virtual const char * GetAncestorClass() const;
  protected:
    PString identifier;
};


class StringTypeBase : public TypeBase
{
    PCLASSINFO(StringTypeBase, TypeBase);
  public:
    StringTypeBase(int tag);
    virtual int GetBraceTokenContext() const;
    virtual void GenerateOperators(ostream & hdr, ostream & cxx, const TypeBase & actualType);
};


class BMPStringType : public StringTypeBase
{
    PCLASSINFO(BMPStringType, StringTypeBase);
  public:
    BMPStringType();
    virtual const char * GetAncestorClass() const;
    virtual void GenerateOperators(ostream & hdr, ostream & cxx, const TypeBase & actualType);
};


class GeneralStringType : public StringTypeBase
{
    PCLASSINFO(GeneralStringType, StringTypeBase);
  public:
    GeneralStringType();
    virtual const char * GetAncestorClass() const;
};


class GraphicStringType : public StringTypeBase
{
    PCLASSINFO(GraphicStringType, StringTypeBase);
  public:
    GraphicStringType();
    virtual const char * GetAncestorClass() const;
};


class IA5StringType : public StringTypeBase
{
    PCLASSINFO(IA5StringType, StringTypeBase);
  public:
    IA5StringType();
    virtual const char * GetAncestorClass() const;
};


class ISO646StringType : public StringTypeBase
{
    PCLASSINFO(ISO646StringType, StringTypeBase);
  public:
    ISO646StringType();
    virtual const char * GetAncestorClass() const;
};


class NumericStringType : public StringTypeBase
{
    PCLASSINFO(NumericStringType, StringTypeBase);
  public:
    NumericStringType();
    virtual const char * GetAncestorClass() const;
};


class PrintableStringType : public StringTypeBase
{
    PCLASSINFO(PrintableStringType, StringTypeBase);
  public:
    PrintableStringType();
    virtual const char * GetAncestorClass() const;
};


class TeletexStringType : public StringTypeBase
{
    PCLASSINFO(TeletexStringType, StringTypeBase);
  public:
    TeletexStringType();
    virtual const char * GetAncestorClass() const;
};


class T61StringType : public StringTypeBase
{
    PCLASSINFO(T61StringType, StringTypeBase);
  public:
    T61StringType();
    virtual const char * GetAncestorClass() const;
};


class UniversalStringType : public StringTypeBase
{
    PCLASSINFO(UniversalStringType, StringTypeBase);
  public:
    UniversalStringType();
    virtual const char * GetAncestorClass() const;
};


class VideotexStringType : public StringTypeBase
{
    PCLASSINFO(VideotexStringType, StringTypeBase);
  public:
    VideotexStringType();
    virtual const char * GetAncestorClass() const;
};


class VisibleStringType : public StringTypeBase
{
    PCLASSINFO(VisibleStringType, StringTypeBase);
  public:
    VisibleStringType();
    virtual const char * GetAncestorClass() const;
};


class UnrestrictedCharacterStringType : public StringTypeBase
{
    PCLASSINFO(UnrestrictedCharacterStringType, StringTypeBase);
  public:
    UnrestrictedCharacterStringType();
    virtual const char * GetAncestorClass() const;
};


class GeneralizedTimeType : public TypeBase
{
    PCLASSINFO(GeneralizedTimeType, TypeBase);
  public:
    GeneralizedTimeType();
    virtual const char * GetAncestorClass() const;
};


class UTCTimeType : public TypeBase
{
    PCLASSINFO(UTCTimeType, TypeBase);
  public:
    UTCTimeType();
    virtual const char * GetAncestorClass() const;
};


class ObjectDescriptorType : public TypeBase
{
    PCLASSINFO(ObjectDescriptorType, TypeBase);
  public:
    ObjectDescriptorType();
    virtual const char * GetAncestorClass() const;
};


class ObjectIdentifierType : public TypeBase
{
    PCLASSINFO(ObjectIdentifierType, TypeBase);
  public:
    ObjectIdentifierType();
    virtual int GetIdentifierTokenContext() const;
    virtual int GetBraceTokenContext() const;
    virtual const char * GetAncestorClass() const;
};


class ObjectClassFieldType : public TypeBase
{
    PCLASSINFO(ObjectClassFieldType, TypeBase);
  public:
    ObjectClassFieldType(PString * objclass, PString * field);
    virtual const char * GetAncestorClass() const;
    void PrintOn(ostream &) const;
    virtual TypeBase * FlattenThisType(const TypeBase & parent);
    virtual BOOL IsPrimitiveType() const;
    virtual void GenerateCplusplus(ostream & hdr, ostream & cxx);
    virtual BOOL CanReferenceType() const;
    virtual BOOL ReferencesType(const TypeBase & type);
  protected:
    PString asnObjectClassName;
    PString asnObjectClassField;
};


class ImportedType : public TypeBase
{
    PCLASSINFO(ImportedType, TypeBase);
  public:
    ImportedType(PString * name, BOOL parameterised);
    virtual const char * GetAncestorClass() const;
    virtual void AdjustIdentifier();
    virtual void GenerateCplusplus(ostream & hdr, ostream & cxx);
    virtual void SetImportPrefix(const PString &);
    virtual BOOL IsParameterisedImport() const;
  protected:
    PString modulePrefix;
    BOOL    parameterised;
};


class SearchType : public TypeBase
{
    PCLASSINFO(SearchType, TypeBase);
  public:
    SearchType(const PString & name);
    virtual const char * GetAncestorClass() const;
};


// Values

class ValueBase : public PObject
{
    PCLASSINFO(ValueBase, PObject);
  public:
    void SetValueName(PString * name);
    const PString & GetName() const { return valueName; }

    virtual void GenerateCplusplus(ostream & hdr, ostream & cxx);

  protected:
    void PrintBase(ostream &) const;
    PString valueName;
};

PLIST(ValuesList, ValueBase);


class DefinedValue : public ValueBase
{
    PCLASSINFO(DefinedValue, ValueBase);
  public:
    DefinedValue(PString * name);
    void PrintOn(ostream &) const;
    const PString & GetReference() const { return referenceName; }
    virtual void GenerateCplusplus(ostream & hdr, ostream & cxx);
  protected:
    PString referenceName;
    ValueBase * actualValue;
    BOOL unresolved;
};


class BooleanValue : public ValueBase
{
    PCLASSINFO(BooleanValue, ValueBase);
  public:
    BooleanValue(BOOL newVal);
    void PrintOn(ostream &) const;
    virtual void GenerateCplusplus(ostream & hdr, ostream & cxx);
  protected:
    BOOL value;
};


class IntegerValue : public ValueBase
{
    PCLASSINFO(IntegerValue, ValueBase);
  public:
    IntegerValue(PInt64 newVal);
    void PrintOn(ostream &) const;
    virtual void GenerateCplusplus(ostream & hdr, ostream & cxx);

    operator PInt64() const { return value; }
    operator long() const { return (long)value; }

  protected:
    PInt64 value;
};


class RealValue : public ValueBase
{
    PCLASSINFO(RealValue, ValueBase);
  public:
    RealValue(double newVal);
  protected:
    double value;
};


class OctetStringValue : public ValueBase
{
    PCLASSINFO(OctetStringValue, ValueBase);
  public:
    OctetStringValue() { }
    OctetStringValue(PString * newVal);
  protected:
    PBYTEArray value;
};


class BitStringValue : public ValueBase
{
    PCLASSINFO(BitStringValue, ValueBase);
  public:
    BitStringValue() { }
    BitStringValue(PString * newVal);
    BitStringValue(PStringList * newVal);
  protected:
    PBYTEArray value;
};


class NullValue : public ValueBase
{
    PCLASSINFO(NullValue, ValueBase);
};


class CharacterValue : public ValueBase
{
    PCLASSINFO(CharacterValue, ValueBase);
  public:
    CharacterValue(BYTE c);
    CharacterValue(BYTE t1, BYTE t2);
    CharacterValue(BYTE q1, BYTE q2, BYTE q3, BYTE q4);
    void PrintOn(ostream &) const;
    virtual void GenerateCplusplus(ostream & hdr, ostream & cxx);
  protected:
    unsigned value;
};


class CharacterStringValue : public ValueBase
{
    PCLASSINFO(CharacterStringValue, ValueBase);
  public:
    CharacterStringValue() { }
    CharacterStringValue(PString * newVal);
    CharacterStringValue(PStringList * newVal);
    void PrintOn(ostream &) const;
    virtual void GenerateCplusplus(ostream & hdr, ostream & cxx);
  protected:
    PString value;
};


class ObjectIdentifierValue : public ValueBase
{
    PCLASSINFO(ObjectIdentifierValue, ValueBase);
  public:
    ObjectIdentifierValue(PString * newVal);
    ObjectIdentifierValue(PStringList * newVal);
    void PrintOn(ostream &) const;
  protected:
    PStringList value;
};


class MinValue : public ValueBase
{
    PCLASSINFO(MinValue, ValueBase);
  public:
    void PrintOn(ostream &) const;
    virtual void GenerateCplusplus(ostream & hdr, ostream & cxx);
};


class MaxValue : public ValueBase
{
    PCLASSINFO(MaxValue, ValueBase);
  public:
    void PrintOn(ostream &) const;
    virtual void GenerateCplusplus(ostream & hdr, ostream & cxx);
};


class SequenceValue : public ValueBase
{
    PCLASSINFO(SequenceValue, ValueBase);
  public:
    SequenceValue(ValuesList * list = NULL);
    void PrintOn(ostream &) const;
  protected:
    ValuesList values;
};


class MibBase : public PObject
{
    PCLASSINFO(MibBase, PObject);
  public:
    MibBase(PString * name, PString * descr, PString * refer, ValueBase * val);
    virtual ~MibBase();
  protected:
    PString name;
    PString description;
    PString reference;
    ValueBase * value;
};

PLIST(MibList, MibBase);


class MibObject : public MibBase
{
    PCLASSINFO(MibObject, MibBase);
  public:
    enum Access {
      read_only,
      read_write,
      write_only,
      not_accessible,
    };
    enum Status {
      mandatory,
      optional,
      obsolete,
      deprecated
    };
    MibObject(PString * name, TypeBase * type, Access acc, Status stat,
              PString * descr, PString * refer, PStringList * idx,
              ValueBase * defVal,
              ValueBase * setVal);
    ~MibObject();
    void PrintOn(ostream &) const;
  protected:
    TypeBase * type;
    Access access;
    Status status;
    PStringList index;
    ValueBase * defaultValue;
};


class MibTrap : public MibBase
{
    PCLASSINFO(MibTrap, MibBase);
  public:
    MibTrap(PString * nam, ValueBase * ent, ValuesList * var,
            PString * descr, PString * refer, ValueBase * val);
    ~MibTrap();
    void PrintOn(ostream &) const;
  protected:
    ValueBase * enterprise;
    ValuesList variables;
};


class ImportModule : public PObject
{
    PCLASSINFO(ImportModule, PObject);
  public:
    ImportModule(PString * name, TypesList * syms);

    void PrintOn(ostream &) const;

    void GenerateCplusplus(ostream & hdr, ostream & cxx);

  protected:
    PString   fullModuleName;
    PString   shortModuleName;
    PString   filename;
    PString   directoryPrefix;
    TypesList symbols;
};

PLIST(ImportsList, ImportModule);


class ModuleDefinition : public PObject
{
    PCLASSINFO(ModuleDefinition, PObject);
  public:
    ModuleDefinition(PString * name, PStringList * id, Tag::Mode defTagMode);

    void PrintOn(ostream &) const;

    Tag::Mode GetDefaultTagMode() const { return defaultTagMode; }

    void SetExportAll();
    void SetExports(TypesList * syms);

    void AddImport(ImportModule * mod)  { imports.Append(mod); }
    void AddType(TypeBase * type)       { types.Append(type); }
    void AddValue(ValueBase * val)      { values.Append(val); }
    void AddMIB(MibBase * mib)          { mibs.Append(mib); }

    void AppendType(TypeBase * type);
    TypeBase * FindType(const PString & name);
    const ValuesList & GetValues() const { return values; }

    const PString & GetModuleName() const { return moduleName; }
    const PString & GetPrefix()     const { return classNamePrefix; }

    PString GetImportModuleName(const PString & moduleName);

    int GetIndentLevel() const { return indentLevel; }
    void SetIndentLevel(int delta) { indentLevel += delta; }

    BOOL UsingInlines() const { return usingInlines; }
    BOOL UsingOperators() const { return usingOperators; }

    void GenerateCplusplus(const PFilePath & path,
                           const PString & modName,
                           const PString & headerDir,
                           unsigned numFiles,
                           BOOL useNamespaces,
                           BOOL useInlines,
                           BOOL useOperators,
                           BOOL verbose);


  protected:
    PString         moduleName;
    PString         classNamePrefix;
    BOOL            separateClassFiles;
    PStringList     definitiveId;
    Tag::Mode       defaultTagMode;
    TypesList       exports;
    BOOL            exportAll;
    ImportsList     imports;
    PStringToString importNames;
    TypesList       types;
    SortedTypesList sortedTypes;
    ValuesList      values;
    MibList         mibs;
    int             indentLevel;
    BOOL            usingInlines;
    BOOL            usingOperators;
};


extern ModuleDefinition * Module;


#endif
