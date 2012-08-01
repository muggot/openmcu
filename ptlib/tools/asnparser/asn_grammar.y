%expect 14

%{

/*
 * asn_grammar.y
 *
 * ASN grammar file
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
 * $Log: asn_grammar.y,v $
 * Revision 1.13  2004/04/03 08:22:23  csoutheren
 * Remove pseudo-RTTI and replaced with real RTTI
 *
 * Revision 1.12  2001/10/02 00:56:04  robertj
 * Fixed problem with autonumering enumerated types.
 *
 * Revision 1.11  2001/01/16 14:00:17  craigs
 * Changed MIN and MAX tokens to avoid namespace conflicts under BeOS
 *
 * Revision 1.10  2000/05/08 08:55:44  robertj
 * Fixed production error for ValueSet, thanks Huang-Ming Huang
 *
 * Revision 1.9  2000/01/19 12:33:07  robertj
 * Fixed parsing of OID's in IMPORTS section.
 *
 * Revision 1.8  2000/01/19 03:38:12  robertj
 * Fixed support for parsing multiple IMPORTS
 *
 * Revision 1.7  1999/07/22 06:48:54  robertj
 * Added comparison operation to base ASN classes and compiled ASN code.
 * Added support for ANY type in ASN parser.
 *
 * Revision 1.6  1999/06/09 06:58:08  robertj
 * Adjusted heading comments.
 *
 * Revision 1.5  1999/06/07 01:56:25  robertj
 * Added header comment on license.
 *
 * Revision 1.4  1999/06/06 05:30:28  robertj
 * Support for parameterised types and type-dentifier types.
 * Added ability to output multiple .cxx files.
 *
 * Revision 1.3  1998/12/14 06:47:55  robertj
 * New memory check code support.
 *
 * Revision 1.2  1998/05/21 04:21:46  robertj
 * Implementing more of the ASN spec.
 *
 * Revision 1.1  1997/12/13 09:17:47  robertj
 * Initial revision
 *
 */

#include <ptlib.h>

#undef malloc
#undef calloc
#undef realloc
#undef free

#include "main.h"

extern int yylex();

extern int ReferenceTokenContext;
extern int IdentifierTokenContext;
extern int BraceTokenContext;
extern int NullTokenContext;
extern int InMacroContext;
extern int HasObjectTypeMacro;
extern int InMIBContext;
extern TypesList * CurrentImportList;

static int UnnamedFieldCount = 1;
static PStringList * DummyParameters;

static PString * ConcatNames(PString * s1, char c, PString * s2)
{
  *s1 += c;
  *s1 += *s2;
  delete s2;
  return s1;
}

#ifdef _MSC_VER
#pragma warning(disable:4701)
#endif

%}

%token IDENTIFIER
%token BIT_IDENTIFIER
%token OID_IDENTIFIER
%token IMPORT_IDENTIFIER
%token MODULEREFERENCE
%token TYPEREFERENCE
%token OBJECTCLASSREFERENCE
%token TYPEFIELDREFERENCE
%token VALUEFIELDREFERENCE
%token VALUESETFIELDREFERENCE
%token OBJECTFIELDREFERENCE
%token OBJECTSETFIELDREFERENCE
%token OBJECTREFERENCE
%token OBJECTSETREFERENCE
%token INTEGER

%token CSTRING
%token OS_BSTRING
%token BS_BSTRING
%token OS_HSTRING
%token BS_HSTRING

%token STRING_BRACE
%token BITSTRING_BRACE
%token OID_BRACE

%token ABSENT           
%token ABSTRACT_SYNTAX  
%token ALL              
%token ANY
%token APPLICATION      
%token ASSIGNMENT
%token AUTOMATIC        
%token BEGIN_t
%token BIT              
%token BMPString
%token BOOLEAN_t
%token BY
%token CHARACTER        
%token CHOICE           
%token CLASS            
%token COMPONENT        
%token COMPONENTS       
%token CONSTRAINED      
%token DEFAULT          
%token DEFINED
%token DEFINITIONS      
%token EMBEDDED         
%token END
%token ENUMERATED       
%token EXCEPT           
%token EXPLICIT         
%token EXPORTS          
%token EXTERNAL         
%token FALSE_t           
%token FROM             
%token GeneralString    
%token GraphicString    
%token IA5String        
%token TYPE_IDENTIFIER 
%token IDENTIFIER_t
%token IMPLICIT         
%token IMPORTS          
%token INCLUDES         
%token INSTANCE         
%token INTEGER_t        
%token INTERSECTION     
%token ISO646String
%token MACRO
%token MAX_t
%token MIN_t
%token MINUS_INFINITY
%token NOTATION
%token NULL_VALUE
%token NULL_TYPE
%token NumericString    
%token OBJECT           
%token OCTET            
%token OF_t              
%token OPTIONAL_t
%token PDV              
%token PLUS_INFINITY    
%token PRESENT          
%token PrintableString  
%token PRIVATE          
%token REAL             
%token SEQUENCE         
%token SET              
%token SIZE_t            
%token STRING           
%token SYNTAX           
%token T61String        
%token TAGS             
%token TeletexString    
%token TRUE_t
%token TYPE_t
%token UNION            
%token UNIQUE           
%token UNIVERSAL        
%token UniversalString  
%token VideotexString   
%token VisibleString    
%token GeneralizedTime
%token UTCTime
%token VALUE
%token WITH
%token string_t
%token identifier_t
%token number_t
%token empty_t
%token type_t
%token value_t
%token OBJECT_TYPE
%token TRAP_TYPE
%token ACCESS
%token STATUS
%token read_only_t
%token read_write_t
%token write_only_t
%token not_accessible_t
%token mandatory_t
%token optional_t
%token obsolete_t
%token deprecated_t
%token DESCRIPTION_t
%token REFERENCE_t
%token INDEX_t
%token DEFVAL_t
%token ENTERPRISE
%token VARIABLES

%token ObjectDescriptor_t


%type <ival> INTEGER
%type <ival> TagDefault
%type <ival> SignedNumber
%type <ival> ObjectTypeAccess ObjectTypeStatus
%type <ival> Class ClassNumber
%type <ival> PresenceConstraint

%type <sval> CSTRING
%type <sval> BS_BSTRING
%type <sval> OS_BSTRING
%type <sval> BS_HSTRING
%type <sval> OS_HSTRING
%type <sval> IDENTIFIER
%type <sval> BIT_IDENTIFIER
%type <sval> OID_IDENTIFIER
%type <sval> IMPORT_IDENTIFIER
%type <sval> TYPEREFERENCE
%type <sval> MODULEREFERENCE
%type <sval> OBJECTCLASSREFERENCE
%type <sval> TYPEFIELDREFERENCE
%type <sval> VALUEFIELDREFERENCE
%type <sval> VALUESETFIELDREFERENCE
%type <sval> OBJECTFIELDREFERENCE
%type <sval> OBJECTSETFIELDREFERENCE
%type <sval> OBJECTREFERENCE
%type <sval> OBJECTSETREFERENCE
%type <sval> DefinitiveObjIdComponent
%type <sval> DefinitiveNameAndNumberForm
%type <sval> GlobalModuleReference
%type <sval> Reference
%type <sval> ExternalTypeReference ExternalValueReference
%type <sval> ObjIdComponent
%type <sval> NumberForm
%type <sval> SimpleDefinedType
%type <sval> ComponentIdList
%type <sval> CharsDefn
%type <sval> SimpleDefinedValue
%type <sval> FieldName PrimitiveFieldName
%type <sval> DefinedObjectClass
%type <sval> ExternalObjectClassReference
%type <sval> UsefulObjectClassReference
%type <sval> Parameter
%type <sval> MibIndexType MibDescrPart MibReferPart

%type <slst> DefinitiveIdentifier
%type <slst> DefinitiveObjIdComponentList
%type <slst> ObjIdComponentList
%type <slst> BitIdentifierList
%type <slst> CharSyms
%type <slst> ParameterList Parameters
%type <slst> MibIndexTypes MibIndexPart

%type <tval> Type BuiltinType ReferencedType NamedType
%type <tval> DefinedType
%type <tval> ConstrainedType
%type <tval> TypeWithConstraint
%type <tval> BitStringType
%type <tval> BooleanType
%type <tval> CharacterStringType 
%type <tval> RestrictedCharacterStringType
%type <tval> UnrestrictedCharacterStringType
%type <tval> ChoiceType AlternativeTypeLists
%type <tval> EmbeddedPDVType
%type <tval> EnumeratedType Enumerations
%type <tval> ExternalType
%type <tval> AnyType
%type <tval> IntegerType
%type <tval> NullType
%type <tval> ObjectClassFieldType
%type <tval> ObjectIdentifierType
%type <tval> OctetStringType 
%type <tval> RealType
%type <tval> SequenceType ComponentType ComponentTypeLists
%type <tval> SequenceOfType
%type <tval> SetType
%type <tval> SetOfType
%type <tval> TaggedType
%type <tval> ParameterizedType
%type <tval> SelectionType
%type <tval> UsefulType
%type <tval> TypeFromObject
%type <tval> ContainedSubtype
%type <tval> ActualParameter
%type <tval> UserDefinedConstraintParameter
%type <tval> Symbol
%type <tval> ParameterizedReference

%type <tlst> AlternativeTypeList
%type <tlst> ComponentTypeList
%type <tlst> ActualParameterList ActualParameters
%type <tlst> UserDefinedConstraintParameters
%type <tlst> SymbolList

%type <vval> Value BuiltinValue
%type <vval> AssignedIdentifier
%type <vval> DefinedValue DefinedValue_Import
%type <vval> ObjectIdentifierValue
%type <vval> OctetStringValue
%type <vval> BitStringValue
%type <vval> ExceptionSpec
%type <vval> ExceptionIdentification
%type <vval> MibDefValPart
%type <vval> LowerEndpoint LowerEndValue UpperEndpoint UpperEndValue
%type <vval> ReferencedValue
%type <vval> BooleanValue
%type <vval> CharacterStringValue RestrictedCharacterStringValue
%type <vval> CharacterStringList Quadruple Tuple
%type <vval> ChoiceValue
%type <vval> NullValue
%type <vval> RealValue NumericRealValue SpecialRealValue 
%type <vval> SequenceValue NamedValue
/*!!!! %type <vval> SequenceOfValue */
%type <vval> ParameterizedValue

%type <vlst> ComponentValueList
%type <vlst> MibVarPart MibVarTypes

%type <nval> NamedBit
%type <nval> EnumerationItem
%type <nval> NamedNumber

%type <nlst> NamedBitList
%type <nlst> Enumeration
%type <nlst> NamedNumberList

%type <elmt> IntersectionElements
%type <elmt> Elements
%type <elmt> Exclusions
%type <elmt> SubtypeElements
%type <elmt> ObjectSetElements
%type <elmt> ValueRange
%type <elmt> PermittedAlphabet
%type <elmt> InnerTypeConstraints
%type <elmt> MultipleTypeConstraints
%type <elmt> SizeConstraint
%type <elmt> UserDefinedConstraintParameterList
%type <elmt> NamedConstraint

%type <elst> ElementSetSpec Unions Intersections TypeConstraints

%type <cons> Constraint
%type <cons> ConstraintSpec
%type <cons> ElementSetSpecs 
%type <cons> GeneralConstraint
%type <cons> UserDefinedConstraint
%type <cons> TableConstraint
%type <cons> ComponentRelationConstraint
%type <cons> ObjectSet

%type <tagv> Tag


%union {
  PInt64	          ival;
  PString	        * sval;
  PStringList	        * slst;
  TypeBase	        * tval;
  TypesList	        * tlst;
  ValueBase	        * vval;
  ValuesList            * vlst;
  NamedNumber	        * nval;
  NamedNumberList       * nlst;
  Constraint            * cons;
  ConstraintElementList * elst;
  ConstraintElementBase * elmt;
  struct {
    Tag::Type tagClass;
    unsigned tagNumber;
  } tagv;
}


%%

ModuleDefinition
  : MODULEREFERENCE DefinitiveIdentifier DEFINITIONS TagDefault ASSIGNMENT BEGIN_t
      {
	Module = new ModuleDefinition($1, $2, (Tag::Mode)$4);
	ReferenceTokenContext = TYPEREFERENCE;
      }
    ModuleBody END
  ;

DefinitiveIdentifier
  : '{' DefinitiveObjIdComponentList '}'
	{
	  $$ = $2;
	}
  | /* empty */
	{
	  $$ = new PStringList;
	}
  ;

DefinitiveObjIdComponentList
  : DefinitiveObjIdComponent
      {
	$$ = new PStringList;
	$$->Append($1);
      }
  | DefinitiveObjIdComponent DefinitiveObjIdComponentList
      {
	$2->InsertAt(0, $1);
	$$ = $2;
      }
  ;

DefinitiveObjIdComponent
  : IDENTIFIER	
  | INTEGER
      {
	$$ = new PString(PString::Unsigned, (int)$1);
      }
  | DefinitiveNameAndNumberForm
  ;

DefinitiveNameAndNumberForm
  : IDENTIFIER '(' INTEGER ')'
      {
	delete $1;
	$$ = new PString(PString::Unsigned, (int)$3);
      }
  ;

TagDefault
  : EXPLICIT TAGS
      {
	$$ = Tag::Explicit;
      }
  | IMPLICIT TAGS 
      {
	$$ = Tag::Implicit;
      }
  | AUTOMATIC TAGS 
      {
	$$ = Tag::Automatic;
      }
  | /* empty */
      {
	$$ = Tag::Explicit;
      }
  ;


/*************************************/

ModuleBody
  : Exports Imports AssignmentList
  | /* empty */
  ;


Exports
  : EXPORTS SymbolsExported ';'
  | /* empty */
  ;


SymbolsExported
  : SymbolList
      {
	Module->SetExports($1);
      }
  | /* empty */
      {
	Module->SetExportAll();
      }
  ;


Imports
  : IMPORTS SymbolsImported ';' 
  | /* empty */
  ;


SymbolsImported
  : SymbolsFromModuleList
  | /* empty */
  ;

SymbolsFromModuleList
  : SymbolsFromModule
  | SymbolsFromModuleList SymbolsFromModule
  ;


SymbolsFromModule
  : SymbolList FROM
      {
	CurrentImportList = $1;
	ReferenceTokenContext = MODULEREFERENCE;
      }
    GlobalModuleReference
      {
	if (!HasObjectTypeMacro) {
	  HasObjectTypeMacro = $1->GetValuesIndex(SearchType("OBJECT-TYPE")) != P_MAX_INDEX;
	  if (HasObjectTypeMacro)
	    PError << "Info: including OBJECT-TYPE macro" << endl;
	}
	Module->AddImport(new ImportModule($4, $1));
	ReferenceTokenContext = TYPEREFERENCE;
	CurrentImportList = NULL;
      }
  ;


GlobalModuleReference
  : MODULEREFERENCE
      {
	ReferenceTokenContext = TYPEREFERENCE;
        BraceTokenContext = OID_BRACE;
      }
    AssignedIdentifier
      {
        BraceTokenContext = '{';
	delete $3;
      }
  ;


AssignedIdentifier
  : DefinedValue_Import
  | ObjectIdentifierValue
  | /* empty */
      {
	$$ = NULL;
      }
  ;

DefinedValue_Import
  : ExternalValueReference
      {
	$$ = new DefinedValue($1);
      }
  | IMPORT_IDENTIFIER
      {
	$$ = new DefinedValue($1);
      }
  ;



SymbolList
  : Symbol
      {
	$$ = new TypesList;
	$$->Append($1);
      }
  | Symbol ',' SymbolList
      {
	$3->Append($1);
	$$ = $3;
      }
  ;


Symbol
  : Reference
      {
	$$ = new ImportedType($1, FALSE);
      }
  | ParameterizedReference    /* only required for X.683 */
  ;


/*************************************/

AssignmentList: Assignment 
  | AssignmentList Assignment
  ;


Assignment
  : TypeAssignment
  | ValueAssignment
  | ValueSetTypeAssignment 
  | ObjectClassAssignment
  | ObjectAssignment
  | ObjectSetAssignment
  | ParameterizedAssignment
  /* We do not have "real" macros, so fake MIB ones */
  | MacroDefinition
  | ObjectTypeDefinition
  | TrapTypeDefinition
  ;


ValueSetTypeAssignment
  : TYPEREFERENCE Type
      {
	$2->SetName($1);
	Module->AddType($2);
	IdentifierTokenContext = $2->GetIdentifierTokenContext();
	BraceTokenContext = $2->GetBraceTokenContext();
      }
    ASSIGNMENT ValueSet
      {
	IdentifierTokenContext = IDENTIFIER;
	BraceTokenContext = '{';
      }
  ;




/********/

TypeAssignment
  : TYPEREFERENCE ASSIGNMENT Type
      {
	$3->SetName($1);
	Module->AddType($3);
      }
  ;


Type
  : ConstrainedType
  | ReferencedType
  | BuiltinType
  ;


BuiltinType
  : BitStringType
  | BooleanType 
  | CharacterStringType 
  | ChoiceType 
  | EmbeddedPDVType 
  | EnumeratedType 
  | ExternalType 
  | AnyType 
  | InstanceOfType
    { }
  | IntegerType 
  | NullType 
  | ObjectClassFieldType
  | ObjectIdentifierType 
  | OctetStringType 
  | RealType 
  | SequenceType 
  | SequenceOfType 
  | SetType 
  | SetOfType 
  | TaggedType
  ;


ReferencedType
  : DefinedType
  | UsefulType
  | SelectionType
  | TypeFromObject
/*!!! syntactically identical to TypeFromObject
  | ValueSetFromObjects
*/
  ;


DefinedType
  : ExternalTypeReference
      {
	$$ = new DefinedType($1, FALSE);
      }
  | TYPEREFERENCE 
      {
	$$ = new DefinedType($1,
			     DummyParameters != NULL &&
			     DummyParameters->GetValuesIndex(*$1) != P_MAX_INDEX);
      }
  | ParameterizedType
/*| ParameterizedValueSetType	synonym for ParameterizedType */
  ;


ExternalTypeReference
  : MODULEREFERENCE '.' TYPEREFERENCE
      {
	*$1 += *$3;
	delete $3;
      }
  ;


BitStringType
  : BIT STRING 
      {
	$$ = new BitStringType;
      }
  | BIT STRING '{' NamedBitList '}'
      {
	$$ = new BitStringType($4);
      }
  ;


NamedBitList
  : NamedBit 
      {
	$$ = new NamedNumberList;
	$$->Append($1);
      }
  | NamedBitList ',' NamedBit
      {
	$1->InsertAt(0, $3);
      }
  ;

NamedBit
  : IDENTIFIER '(' INTEGER ')' 
      {
	$$ = new NamedNumber($1, (int)$3);
      }
  | IDENTIFIER '(' DefinedValue ')'
      {
	$$ = new NamedNumber($1, ((DefinedValue*)$3)->GetReference());
	delete $3;
      }
  ;


BooleanType
  : BOOLEAN_t
      {
	$$ = new BooleanType;
      }
  ;


CharacterStringType
  : RestrictedCharacterStringType
  | UnrestrictedCharacterStringType
  ;

RestrictedCharacterStringType
  : BMPString
      {
	$$ = new BMPStringType;
      }
  | GeneralString
      {
	$$ = new GeneralStringType;
      }
  | GraphicString
      {
	$$ = new GraphicStringType;
      }
  | IA5String
      {
	$$ = new IA5StringType;
      }
  | ISO646String
      {
	$$ = new ISO646StringType;
      }
  | NumericString
      {
	$$ = new NumericStringType;
      }
  | PrintableString
      {
	$$ = new PrintableStringType;
      }
  | TeletexString
      {
	$$ = new TeletexStringType;
      }
  | T61String
      {
	$$ = new T61StringType;
      }
  | UniversalString
      {
	$$ = new UniversalStringType;
      }
  | VideotexString
      {
	$$ = new VideotexStringType;
      }
  | VisibleString
      {
	$$ = new VisibleStringType;
      }
  ;


UnrestrictedCharacterStringType
  : CHARACTER STRING
      {
	$$ = new UnrestrictedCharacterStringType;
      }
  ;


ChoiceType
  : CHOICE '{' AlternativeTypeLists '}'
      {
	$$ = $3;
      }
  ;

AlternativeTypeLists
  : AlternativeTypeList
      {
	$$ = new ChoiceType($1);
      }
  | AlternativeTypeList ',' ExtensionAndException
      {
	$$ = new ChoiceType($1, TRUE);
      }
  | AlternativeTypeList ',' ExtensionAndException  ','  AlternativeTypeList
      {
	$$ = new ChoiceType($1, TRUE, $5);
      }
  ;

AlternativeTypeList
  : NamedType	
      {
	$$ = new TypesList;
	$$->Append($1);
      }
  | AlternativeTypeList ',' NamedType
      {
	$1->Append($3);
      }
  ;


ExtensionAndException
  : '.' '.' '.' ExceptionSpec
  ;


NamedType
  : IDENTIFIER Type
      {
	$2->SetName($1);
	$$ = $2;
      }
  | Type	     /* ITU-T Rec. X.680 Appendix H.1 */
      {
	PError << StdError(Warning) << "unnamed field." << endl;
	$1->SetName(new PString(PString::Printf, "_unnamed%u", UnnamedFieldCount++));
      }
/*| SelectionType    /* Unnecessary as have rule in Type for this */
  ;


EmbeddedPDVType
  : EMBEDDED PDV
      {
	$$ = new EmbeddedPDVType;
      }
  ;


EnumeratedType
  : ENUMERATED '{' Enumerations '}'
      {
	$$ = $3;
      }
  ;

Enumerations
  : Enumeration
      {
	$$ = new EnumeratedType($1, FALSE, NULL);
      }
  | Enumeration  ',' '.' '.' '.'
      {
	$$ = new EnumeratedType($1, TRUE, NULL);
      }
  | Enumeration  ',' '.' '.' '.' ',' Enumeration
      {
	$$ = new EnumeratedType($1, TRUE, $7);
      }
  ;

Enumeration
  : EnumerationItem
      {
	$$ = new NamedNumberList;
	$$->Append($1);
      }
  | Enumeration ',' EnumerationItem
      {
	$1->Append($3);
        PINDEX sz = $1->GetSize();
        if (sz > 1)
          $3->SetAutoNumber((*$1)[sz-2]);
	$$ = $1;
      }
  ;

EnumerationItem
  : IDENTIFIER
      {
	$$ = new NamedNumber($1);
      }
  | NamedNumber
  ;


ExternalType
  : EXTERNAL
      {
	$$ = new ExternalType;
      }
  ;


AnyType
  : ANY
      {
	$$ = new AnyType(NULL);
      }
  | ANY DEFINED BY IDENTIFIER
      {
	$$ = new AnyType($4);
      }
  ;


InstanceOfType
  : INSTANCE OF_t DefinedObjectClass
  ;


IntegerType
  : INTEGER_t
      {
	$$ = new IntegerType;
      }
  | INTEGER_t '{' NamedNumberList '}'
      {
	$$ = new IntegerType($3);
      }
  ;


NullType
  : NULL_TYPE
      {
	$$ = new NullType;
      }
  ;


ObjectClassFieldType
  : DefinedObjectClass '.' FieldName
      {
	$$ = new ObjectClassFieldType($1, $3);
      }
  ;


ObjectIdentifierType
  : OBJECT IDENTIFIER_t
      {
	$$ = new ObjectIdentifierType;
      }
  ;

OctetStringType
  : OCTET STRING
      {
	$$ = new OctetStringType;
      }
  ;


RealType
  : REAL
      {
	$$ = new RealType;
      }
  ;


SequenceType
  : SEQUENCE '{' ComponentTypeLists '}'
      {
	$$ = $3;
      }
  | SEQUENCE '{'  '}'
      {
	$$ = new SequenceType(NULL, FALSE, NULL);
      }
  | SEQUENCE '{' ExtensionAndException '}'
      {
	$$ = new SequenceType(NULL, TRUE, NULL);
      }
  ;

ComponentTypeLists
  : ComponentTypeList
      {
	$$ = new SequenceType($1, FALSE, NULL);
      }
  | ComponentTypeList ',' ExtensionAndException
      {
	$$ = new SequenceType($1, TRUE, NULL);
      }
  | ComponentTypeList ',' ExtensionAndException ',' ComponentTypeList
      {
	$$ = new SequenceType($1, TRUE, $5);
      }
  | ExtensionAndException ',' ComponentTypeList
      {
	$$ = new SequenceType(NULL, TRUE, $3);
      }
  ;

ComponentTypeList
  : ComponentType
      {
	$$ = new TypesList;
	$$->Append($1);
      }
  | ComponentTypeList ',' ComponentType
      {
	$1->Append($3);
      }
  ;

ComponentType
  : NamedType
  | NamedType OPTIONAL_t
      {
	$1->SetOptional();
      }
  | NamedType DEFAULT
      {
	IdentifierTokenContext = $1->GetIdentifierTokenContext();
      }
    Value   
      {
	IdentifierTokenContext = IDENTIFIER;
	$1->SetDefaultValue($4);
      }
  | COMPONENTS OF_t Type
      {
	$$ = $3;
      }
  ;


SequenceOfType
  : SEQUENCE OF_t Type
      {
	$$ = new SequenceOfType($3, NULL);
      }
  ;


SetType
  : SET '{' ComponentTypeLists '}' 
      {
	$$ = new SetType((SequenceType*)$3);
      }
  | SET '{'  '}'
      {
	$$ = new SetType;
      }
  ;


SetOfType
  : SET OF_t Type
      {
	$$ = new SetOfType($3, NULL);
      }
  ;


TaggedType
  : Tag Type
      {
	$2->SetTag($1.tagClass, $1.tagNumber, Module->GetDefaultTagMode());
	$$ = $2;
      }
  | Tag IMPLICIT Type  
      {
	$3->SetTag($1.tagClass, $1.tagNumber, Tag::Implicit);
	$$ = $3;
      }
  | Tag EXPLICIT Type
      {
	$3->SetTag($1.tagClass, $1.tagNumber, Tag::Explicit);
	$$ = $3;
      }
  ;

Tag
  : '[' Class ClassNumber ']'
      {
	$$.tagClass = (Tag::Type)$2;
	$$.tagNumber = (int)$3;
      }
  ;

ClassNumber
  : INTEGER 
  | DefinedValue
      {
	if (PIsDescendant($1, IntegerValue))
	  $$ = *(IntegerValue*)$1;
	else
	  PError << StdError(Fatal) << "incorrect value type." << endl;
      }
  ;

Class
  : UNIVERSAL
      {
	$$ = Tag::Universal;
      }
  | APPLICATION
      {
	$$ = Tag::Application;
      }
  | PRIVATE
      {
	$$ = Tag::Private;
      }
  | /* empty */
      {
	$$ = Tag::ContextSpecific;
      }
  ;


SelectionType
  : IDENTIFIER '<' Type
      {
	$$ = new SelectionType($1, $3);
      }
  ;


UsefulType
  : GeneralizedTime
      {
	$$ = new GeneralizedTimeType;
      }
  | UTCTime
      {
	$$ = new UTCTimeType;
      }
  | ObjectDescriptor_t
      {
	$$ = new ObjectDescriptorType;
      }
  ;


TypeFromObject
  : ReferencedObjects '.' FieldName
    { }
  ;

/*!!!
ValueSetFromObjects
  : ReferencedObjects '.' FieldName
  ;
*/

ReferencedObjects
  : DefinedObject
  | ParameterizedObject
  | DefinedObjectSet
    { }
  | ParameterizedObjectSet
    { }
  ;

ParameterizedObject
  : DefinedObject ActualParameterList
  ;



/********/

ConstrainedType
  : Type Constraint
      {
	$1->AddConstraint($2);
      }
  | TypeWithConstraint
  ;

TypeWithConstraint
  : SET Constraint OF_t Type
      {
	$$ = new SetOfType($4, $2);
      }
  | SET SizeConstraint OF_t Type
      {
	$$ = new SetOfType($4, new Constraint($2));
      }
  | SEQUENCE Constraint OF_t Type
      {
	$$ = new SequenceOfType($4, $2);
      }
  | SEQUENCE SizeConstraint OF_t Type
      {
	$$ = new SequenceOfType($4, new Constraint($2));
      }
  ;

Constraint
  : '(' ConstraintSpec ExceptionSpec ')'
      {
	$$ = $2;
      }
  ;

ConstraintSpec
  : ElementSetSpecs
  | GeneralConstraint
  ;


ExceptionSpec
  : '!' ExceptionIdentification 
      {
	$$ = $2;
      }
  | /* empty */
      {
	$$ = NULL;
      }
  ;


ExceptionIdentification
  : SignedNumber
      {
	$$ = new IntegerValue($1);
      }
  | DefinedValue
  | Type ':' Value
      {
	delete $1;
        PError << StdError(Warning) << "Typed exception unsupported" << endl;
	$$ = $3;
      }
  ;


ElementSetSpecs
  : ElementSetSpec
      {
	$$ = new Constraint($1, FALSE, NULL);
      }
  | ElementSetSpec  ',' '.' '.' '.'
      {
	$$ = new Constraint($1, TRUE, NULL);
      }
  | '.' '.' '.' ',' ElementSetSpec
      {
	$$ = new Constraint(NULL, TRUE, $5);
      }
  | ElementSetSpec  ',' '.' '.' '.' ElementSetSpec
      {
	$$ = new Constraint($1, TRUE, $6);
      }
  ;


ElementSetSpec
  : Unions
  | ALL Exclusions
      {
	$$ = new ConstraintElementList;
	$$->Append(new ConstrainAllConstraintElement($2));
      }
  ;


Unions
  : Intersections
      {
	$$ = new ConstraintElementList;
	$$->Append(new ElementListConstraintElement($1));
      }
  | Unions UnionMark Intersections
      {
	$1->Append(new ElementListConstraintElement($3));
      }
  ;

Intersections
  : IntersectionElements 
      {
	$$ = new ConstraintElementList;
	$$->Append($1);
      }
  | Intersections IntersectionMark IntersectionElements
      {
	$1->Append($3);
      }
  ;

IntersectionElements
  : Elements 
  | Elements Exclusions
      {
	$1->SetExclusions($2);
      }
  ;

Exclusions
  : EXCEPT Elements
      {
	$$ = $2;
      }
  ;

UnionMark
  : '|'	
  | UNION
  ;

IntersectionMark
  : '^'	
  | INTERSECTION
  ;

Elements
  : SubtypeElements
  | ObjectSetElements
  | '(' ElementSetSpec ')'
      {
	$$ = new ElementListConstraintElement($2);
      }
  ;


SubtypeElements
  : Value
      {
	$$ = new SingleValueConstraintElement($1);
      }
  | ContainedSubtype
      {
	$$ = new SubTypeConstraintElement($1);
      }
  | ValueRange
  | PermittedAlphabet
  | SizeConstraint
/*| TypeConstraint  This is really Type and causes ambiguity with ContainedSubtype */
  | InnerTypeConstraints
  ;

ValueRange
  : LowerEndpoint '.' '.' UpperEndpoint
      {
	$$ = new ValueRangeConstraintElement($1, $4);
      }
  ;

LowerEndpoint
  : LowerEndValue
  | LowerEndValue '<'
  ;

UpperEndpoint
  : UpperEndValue
  | '<' UpperEndValue
      {
	$$ = $2;
      }
  ;

LowerEndValue
  : Value 
  | MIN_t
      {
	$$ = new MinValue;
      }
  ;

UpperEndValue
  : Value 
  | MAX_t
      {
	$$ = new MaxValue;
      }
  ;

PermittedAlphabet
  : FROM Constraint
      {
	$$ = new FromConstraintElement($2);
      }
  ;

ContainedSubtype
  : INCLUDES Type
      {
	$$ = $2;
      }
/*| Type	 Actual grammar has INCLUDES keyword optional but this is
		 horribly ambiguous, so only support  a few specific Type
		 definitions */
  | ConstrainedType
  | BuiltinType
  | DefinedType
  | UsefulType
  ;


SizeConstraint
  : SIZE_t Constraint
      {
	$$ = new SizeConstraintElement($2);
      }
  ;


InnerTypeConstraints
  : WITH COMPONENT Constraint
      {
	$$ = new WithComponentConstraintElement(NULL, $3, WithComponentConstraintElement::Default);
      }
  | WITH COMPONENTS MultipleTypeConstraints
      {
	$$ = $3;
      }
  ;

MultipleTypeConstraints
  : '{' TypeConstraints '}'			/* FullSpecification */
      {
	$$ = new InnerTypeConstraintElement($2, FALSE);
      }
  | '{'  '.' '.' '.' ',' TypeConstraints '}'	/* PartialSpecification */
      {
	$$ = new InnerTypeConstraintElement($6, TRUE);
      }
  ;

TypeConstraints
  : NamedConstraint
      {
	$$ = new ConstraintElementList;
	$$->Append($1);
      }
  | NamedConstraint ',' TypeConstraints
      {
	$3->Append($1);
	$$ = $3;
      }
  ;

NamedConstraint
  : IDENTIFIER PresenceConstraint
      {
	$$ = new WithComponentConstraintElement($1, NULL, (int)$2);
      }
  | IDENTIFIER Constraint PresenceConstraint 
      {
	$$ = new WithComponentConstraintElement($1, $2, (int)$3);
      }
  ;

PresenceConstraint
  : PRESENT
      {
	$$ = WithComponentConstraintElement::Present;
      }
  | ABSENT 
      {
	$$ = WithComponentConstraintElement::Absent;
      }
  | OPTIONAL_t
      {
	$$ = WithComponentConstraintElement::Optional;
      }
  | /* empty */
      {
	$$ = WithComponentConstraintElement::Default;
      }
  ;


GeneralConstraint
  : UserDefinedConstraint
  | TableConstraint
  ;

UserDefinedConstraint
  : CONSTRAINED BY '{' UserDefinedConstraintParameterList '}'
    {
      $$ = new Constraint($4);
    }
  ;

UserDefinedConstraintParameterList
  : /* empty */
      {
	$$ = new UserDefinedConstraintElement(NULL);
      }
  | UserDefinedConstraintParameters
      {
	$$ = new UserDefinedConstraintElement($1);
      }
  ;
 
UserDefinedConstraintParameters
  : UserDefinedConstraintParameter ',' UserDefinedConstraintParameters
      {
	$3->Append($1);
	$$ = $3;
      }
  | UserDefinedConstraintParameter
      {
	$$ = new TypesList;
	$$->Append($1);
      }
  ;

UserDefinedConstraintParameter
  : Governor ':' ActualParameter
      {
	$$ = $3;
      }
  | ActualParameter
  ;


TableConstraint
  : ObjectSet /* SimpleTableConstraint */
  | ComponentRelationConstraint
  ;

ComponentRelationConstraint
  : '{' DefinedObjectSet '}' '{' AtNotations '}'
    { $$ = NULL; }
  ;

AtNotations
  : AtNotations ',' AtNotation
  | AtNotation
  ;

AtNotation
  : '@' ComponentIdList
  | '@' '.' ComponentIdList
  ;

ComponentIdList
  : ComponentIdList '.' IDENTIFIER
  | IDENTIFIER
  ;


/********/

ObjectClassAssignment
  : OBJECTCLASSREFERENCE ASSIGNMENT ObjectClass
    { }
  ;

ObjectAssignment
  : OBJECTREFERENCE DefinedObjectClass ASSIGNMENT Object
    { }
  ;

ObjectSetAssignment
  : OBJECTSETREFERENCE DefinedObjectClass ASSIGNMENT ObjectSet
    { }
  ;


ObjectClass
  : DefinedObjectClass
    { }
  | ObjectClassDefn 
  | ParameterizedObjectClass
  ;

DefinedObjectClass
  : ExternalObjectClassReference
  | OBJECTCLASSREFERENCE
  | UsefulObjectClassReference
  ;


ExternalObjectClassReference
  : MODULEREFERENCE '.' OBJECTCLASSREFERENCE
      {
	$$ = ConcatNames($1, '.', $3);
      }
  ;

UsefulObjectClassReference
  : TYPE_IDENTIFIER
      {
	$$ = new PString("TYPE-IDENTIFIER");
      }
  | ABSTRACT_SYNTAX
      {
	$$ = new PString("ABSTRACT-SYNTAX");
      }
  ;


ObjectClassDefn
  : CLASS  '{'  FieldSpecs '}'  WithSyntaxSpec
  ;

FieldSpecs
  : FieldSpecs ',' FieldSpec
  | FieldSpec
  ;

FieldSpec
  : TypeFieldSpec
  | FixedTypeValueFieldSpec
  | VariableTypeValueFieldSpec
  | FixedTypeValueSetFieldSpec
  | VariableTypeValueSetFieldSpec
  | ObjectFieldSpec
  | ObjectSetFieldSpec
  ;

TypeFieldSpec
  : TYPEFIELDREFERENCE TypeOptionalitySpec
    { }
  ;

TypeOptionalitySpec
  : OPTIONAL_t
  | DEFAULT Type
  | /* empty */
  ;

FixedTypeValueFieldSpec
  : VALUEFIELDREFERENCE Type Unique ValueOptionalitySpec
    { }
  ;

Unique
  : UNIQUE
  | /* empty */
  ;

ValueOptionalitySpec
  : OPTIONAL_t
  | DEFAULT Value
  | /* empty */
  ;

VariableTypeValueFieldSpec
  : VALUEFIELDREFERENCE FieldName ValueOptionalitySpec
    { }
  ;

FixedTypeValueSetFieldSpec
  : VALUESETFIELDREFERENCE Type ValueSetOptionalitySpec
    { }
  ;

ValueSetOptionalitySpec
  : OPTIONAL_t
  | DEFAULT ValueSet
  | /* empty */
  ;

VariableTypeValueSetFieldSpec
  : VALUESETFIELDREFERENCE FieldName ValueSetOptionalitySpec
    { }
  ;

ObjectFieldSpec
  : OBJECTFIELDREFERENCE DefinedObjectClass ObjectOptionalitySpec
    { }
  ;

ObjectOptionalitySpec
  : OPTIONAL_t
  | DEFAULT Object
  | /* empty */
  ;

ObjectSetFieldSpec
  : OBJECTSETFIELDREFERENCE DefinedObjectClass ObjectSetOptionalitySpec
    { }
  ;

ObjectSetOptionalitySpec
  : OPTIONAL_t
  | DEFAULT ObjectSet
  | /* empty */
  ;

WithSyntaxSpec
  : WITH SYNTAX SyntaxList
  | /* empty */
  ;

SyntaxList
  : '{' TokenOrGroupSpecs '}'
  | '{' '}'
  ;

TokenOrGroupSpecs
  : TokenOrGroupSpecs TokenOrGroupSpec
  | TokenOrGroupSpec
  ;

TokenOrGroupSpec
  : RequiredToken
  | OptionalGroup
  ;

OptionalGroup
  : '[' TokenOrGroupSpecs ']'
  ;

RequiredToken
  : Literal
  | PrimitiveFieldName
    { }
  ;

Literal
  : BIT
  | BOOLEAN_t
  | CHARACTER
  | CHOICE
  | EMBEDDED
  | END
  | ENUMERATED
  | EXTERNAL
  | FALSE_t
  | INSTANCE
  | INTEGER
    { }
  | INTERSECTION
  | MINUS_INFINITY
  | NULL_TYPE
  | OBJECT
  | OCTET
  | PLUS_INFINITY
  | REAL
  | SEQUENCE
  | SET
  | TRUE_t
  | UNION
  | ','
  ;


DefinedObject
  : ExternalObjectReference
  | OBJECTREFERENCE
    { }
  ;

ExternalObjectReference
  : MODULEREFERENCE '.' OBJECTREFERENCE
    { }
  ;


ParameterizedObjectClass
  : DefinedObjectClass ActualParameterList
    { }
  ;


DefinedObjectSet
  : ExternalObjectSetReference
    { }
  | OBJECTSETREFERENCE
    { }
  ;

ExternalObjectSetReference
  : MODULEREFERENCE '.' OBJECTSETREFERENCE
    { }
  ;


ParameterizedObjectSet
  : DefinedObjectSet ActualParameterList
  ;


FieldName
  : FieldName '.' PrimitiveFieldName
      {
	$$ = ConcatNames($1, '.', $3);
      }
  | PrimitiveFieldName
  ;


PrimitiveFieldName
  : TYPEFIELDREFERENCE
  | VALUEFIELDREFERENCE
  | VALUESETFIELDREFERENCE
  | OBJECTFIELDREFERENCE
  | OBJECTSETFIELDREFERENCE
  ;


Object
  : DefinedObject
    { }
/*!!!
  | ObjectDefn
    { }
*/
  | ObjectFromObject
    { }
  | ParameterizedObject
    { }
  ;


/*!!!!
ObjectDefn
  : DefaultSyntax
  | DefinedSyntax
  ;

DefaultSyntax
  : '{' FieldSettings '}
  : '{' '}
  ;

FieldSettings
  : FieldSettings ',' FieldSetting
  | FieldSetting
  ;

FieldSetting
  : PrimitiveFieldName Setting
  ;

DefinedSyntax
  : '{' DefinedSyntaxTokens '}'
  '

DefinedSyntaxTokens
  : DefinedSyntaxTokens DefinedSyntaxToken
  | /* empty *//*
  ;

DefinedSyntaxToken
  : Literal
  | Setting
  ;

Setting
  : Type
  | Value
  | ValueSet
  | Object
  | ObjectSet
  ;
*/


ObjectSet
  : '{' ElementSetSpec '}'
    { }
  ;


ObjectFromObject
  : ReferencedObjects '.' FieldName
  ;


ObjectSetElements
  : Object
    { }
  | DefinedObjectSet
    { }
/*!!!
  | ObjectSetFromObjects
*/
  | ParameterizedObjectSet
    { }
  ;

/*!!!
ObjectSetFromObjects
  : ReferencedObjects '.' FieldName
  ;
*/



/********/

ParameterizedAssignment
  : ParameterizedTypeAssignment
  | ParameterizedValueAssignment
  | ParameterizedValueSetTypeAssignment
  | ParameterizedObjectClassAssignment
  | ParameterizedObjectAssignment
  | ParameterizedObjectSetAssignment
  ;

ParameterizedTypeAssignment
  : TYPEREFERENCE ParameterList
      {
	DummyParameters = $2;
      }
    ASSIGNMENT Type
      {
	DummyParameters = NULL;
	$5->SetName($1);
	$5->SetParameters($2);
	Module->AddType($5);
      }
  ;

ParameterizedValueAssignment
  : IDENTIFIER ParameterList Type ASSIGNMENT Value
    { }
  ;

ParameterizedValueSetTypeAssignment
  : TYPEREFERENCE ParameterList Type ASSIGNMENT ValueSet
    { }
  ;

ParameterizedObjectClassAssignment
  : OBJECTCLASSREFERENCE ParameterList ASSIGNMENT ObjectClass
    { }
  ;

ParameterizedObjectAssignment
  : OBJECTREFERENCE ParameterList DefinedObjectClass ASSIGNMENT Object
    { }
  ;

ParameterizedObjectSetAssignment
  : OBJECTSETREFERENCE ParameterList DefinedObjectClass ASSIGNMENT ObjectSet
    { }
  ;

ParameterList
  : '{' Parameters '}'
      {
	$$ = $2;
      }
  ;

Parameters
  : Parameters ',' Parameter
      {
	$$ = $1;
	$$->Append($3);
      }
  | Parameter
      {
	$$ = new PStringList;
	$$->Append($1);
      }
  ;

Parameter
  : Governor ':' Reference
      {
	$$ = $3;
      }
  | Reference
  ;

Governor
  : Type
    { }
  | DefinedObjectClass
    { }
  ;


ParameterizedType
  : SimpleDefinedType ActualParameterList
      {
	$$ = new ParameterizedType($1, $2);
      }
  ;

SimpleDefinedType
  : ExternalTypeReference
  | TYPEREFERENCE
  ;


ActualParameterList
  : '{' ActualParameters '}'
      {
	$$ = $2;
      }
  ;

ActualParameters
  : ActualParameters ',' ActualParameter
      {
	$1->Append($3);
	$$ = $1;
      }
  | ActualParameter
      {
	$$ = new TypesList;
	$$->Append($1);
      }
  ;

ActualParameter
  : Type
  | Value
    { }
  | ValueSet
    { }
  | DefinedObjectClass
    { }
/*!!!
  | Object
    { }
  | ObjectSet
*/
  ;



/********/

ValueAssignment 
  : IDENTIFIER Type
      {
	IdentifierTokenContext = $2->GetIdentifierTokenContext();
	BraceTokenContext = $2->GetBraceTokenContext();
	NullTokenContext = NULL_VALUE;
      }
    ASSIGNMENT Value
      {
	$5->SetValueName($1);
	Module->AddValue($5);
	IdentifierTokenContext = IDENTIFIER;
	BraceTokenContext = '{';
	NullTokenContext = NULL_TYPE;
      }
  ;


Value
  : BuiltinValue
  | ReferencedValue
  ;


BuiltinValue
  : BitStringValue 
  | BooleanValue 
  | CharacterStringValue 
  | ChoiceValue 
/*| EmbeddedPDVValue  synonym to SequenceValue */
/*| EnumeratedValue   synonym to IDENTIFIER    */
/*| ExternalValue     synonym to SequenceValue */
/*| InstanceOfValue   synonym to Value */
  | SignedNumber      /* IntegerValue */
      {
	$$ = new IntegerValue($1);
      }
  | NullValue 
/*!!!
  | ObjectClassFieldValue
*/
  | ObjectIdentifierValue
  | OctetStringValue
  | RealValue 
  | SequenceValue 
/*!!!! 
  | SequenceOfValue
*/
/*| SetValue	      synonym to SequenceValue */
/*| SetOfValue	      synonym to SequenceOfValue */
/*| TaggedValue	      synonym to Value */
  ;


DefinedValue
  : ExternalValueReference
      {
	$$ = new DefinedValue($1);
      }
  | IDENTIFIER
      {
	$$ = new DefinedValue($1);
      }
  | ParameterizedValue
  ;


ExternalValueReference
  : MODULEREFERENCE '.' IDENTIFIER
      {
	*$1 += *$3;
	delete $3;
      }
  ;


ObjectIdentifierValue
  : OID_BRACE
      {
        IdentifierTokenContext = OID_IDENTIFIER;
      }
    ObjIdComponentList '}'
      {
	$$ = new ObjectIdentifierValue($3);
	IdentifierTokenContext = IDENTIFIER;
      }
/*!!!
  | '{' DefinedValue_OID ObjIdComponentList '}'
      {
	$$ = new ObjectIdentifierValue($2);
      }
*/
  ;


ObjIdComponentList
  : ObjIdComponent
      {
	$$ = new PStringList;
	$$->Append($1);
      }
  | ObjIdComponent ObjIdComponentList
      {
	$2->InsertAt(0, $1);
	$$ = $2;
      }
  ;

ObjIdComponent
  : OID_IDENTIFIER
  | INTEGER
      {
	$$ = new PString(PString::Unsigned, (int)$1);
      }
  | OID_IDENTIFIER '(' NumberForm ')'
      {
	delete $1;
	$$ = $3;
      }
  ;

NumberForm
  : INTEGER
      {
	$$ = new PString(PString::Unsigned, (int)$1);
      }
  | ExternalValueReference
  | OID_IDENTIFIER
  ;


OctetStringValue
  : OS_BSTRING
      {
	$$ = new OctetStringValue($1);
      }
  | OS_HSTRING
      {
	$$ = new OctetStringValue($1);
      }
  ;

BitStringValue
  : BS_BSTRING
      {
	$$ = new BitStringValue($1);
      }
  | BS_HSTRING
      {
	$$ = new BitStringValue($1);
      }
  | BITSTRING_BRACE BitIdentifierList '}' 
      {
	$$ = new BitStringValue($2);
      }
  | BITSTRING_BRACE  '}'
      {
	$$ = new BitStringValue;
      }
  ;


BitIdentifierList
  : BIT_IDENTIFIER
      {
	$$ = new PStringList;
      }
  | BitIdentifierList ',' BIT_IDENTIFIER
      {
	// Look up $3
	$1->SetAt($1->GetSize(), 0);
      }
  ;


BooleanValue
  : TRUE_t
      {
	$$ = new BooleanValue(TRUE);
      }
  | FALSE_t
      {
	$$ = new BooleanValue(FALSE);
      }
  ;


CharacterStringValue
  : RestrictedCharacterStringValue
/*!!!
  | UnrestrictedCharacterStringValue
*/
  ;

RestrictedCharacterStringValue
  : CSTRING
      {
	$$ = new CharacterStringValue($1);
      }
  | CharacterStringList
  | Quadruple
  | Tuple
  ;

CharacterStringList
  : STRING_BRACE CharSyms '}'
      {
	$$ = new CharacterStringValue($2);
      }
  ;

CharSyms
  : CharsDefn
      {
	$$ = new PStringList;
	$$->Append($1);
      }
  | CharSyms ',' CharsDefn
      {
	$1->Append($3);
      }
  ;

CharsDefn
  : CSTRING 
  | DefinedValue
      {
	PError << StdError(Warning) << "DefinedValue in string unsupported" << endl;
      }
  ;

Quadruple
  :  STRING_BRACE  INTEGER  ','  INTEGER  ','  INTEGER  ','  INTEGER '}'
      {
	if ($2 != 0 || $4 != 0 || $6 > 255 || $8 > 255)
	  PError << StdError(Warning) << "Illegal value in Character Quadruple" << endl;
	$$ = new CharacterValue((BYTE)$2, (BYTE)$4, (BYTE)$6, (BYTE)$8);
      }
  ;

Tuple
  :  STRING_BRACE INTEGER ',' INTEGER '}'
      {
	if ($2 > 255 || $4 > 255)
	  PError << StdError(Warning) << "Illegal value in Character Tuple" << endl;
	$$ = new CharacterValue((BYTE)$2, (BYTE)$4);
      }
  ;


ChoiceValue
  : IDENTIFIER ':' Value
      {
	$3->SetValueName($1);
	$$ = $3;
      }
  ;


NullValue
  : NULL_VALUE
      {
	$$ = new NullValue;
      }
  ;


RealValue
  : NumericRealValue 
  | SpecialRealValue
  ;

NumericRealValue
  :  '0'
      {
	$$ = new RealValue(0);
      }
/*!!!
  | SequenceValue
*/
  ;

SpecialRealValue
  : PLUS_INFINITY
      {
	$$ = new RealValue(0);
      }
  | MINUS_INFINITY
      {
	$$ = new RealValue(0);
      }
  ;


SequenceValue
  : '{' ComponentValueList '}' 
      {
	$$ = new SequenceValue($2);
      }
  | '{'  '}'
      {
	$$ = new SequenceValue;
      }
  ;

ComponentValueList
  : NamedValue
      {
	$$ = new ValuesList;
	$$->Append($1);
      }
  | ComponentValueList ',' NamedValue
      {
	$1->Append($3);
      }
  ;

NamedValue
  : IDENTIFIER Value
      {
	$2->SetValueName($1);
	$$ = $2;
      }
  ;


/*!!!! 
SequenceOfValue
  : '{' ValueList '}' 
      {
	$$ = NULL;
      }
  | '{'  '}'
      {
	$$ = NULL;
      }
  ;

ValueList
  : Value
      { }
  | ValueList ',' Value
      { }
  ;
*/ 


/*!!!
ObjectClassFieldValue
  : OpenTypeFieldVal
  | Value
  ;

OpenTypeFieldVal
  : Type ':' Value
  ;
*/


ReferencedValue
  : DefinedValue
/*!!!!
  | ValueFromObject
*/
  ;

/*!!!!
ValueFromObject
  : ReferencedObjects '.' FieldName
    { }
  ;
*/


ParameterizedValue
  : SimpleDefinedValue ActualParameterList
    { }
  ;

SimpleDefinedValue
  : ExternalValueReference
  | IDENTIFIER
  ;



/********/

ValueSet
  : '{' ElementSetSpecs '}'
  ;


/********/

MacroDefinition
  : TYPEREFERENCE MACRO ASSIGNMENT MacroSubstance
      {
	PError << StdError(Warning) << "MACRO unsupported" << endl;
      }
  ;

MacroSubstance
  : BEGIN_t
      {
	InMacroContext = TRUE;
      }
    MacroBody END
      {
	InMacroContext = FALSE;
      }
  | TYPEREFERENCE
      {}
  | TYPEREFERENCE '.' TYPEREFERENCE
      {}
  ;

MacroBody
  : TypeProduction ValueProduction /*SupportingProductions*/
  ;

TypeProduction
  : TYPE_t NOTATION ASSIGNMENT MacroAlternativeList
  ;

ValueProduction
  : VALUE NOTATION ASSIGNMENT MacroAlternativeList
  ;


/*
SupportingProductions
  : ProductionList
  | /* empty *//*
  ;

ProductionList
  : Production
  | ProductionList Production
  ;

Production
  : TYPEREFERENCE ASSIGNMENT MacroAlternativeList

  ;
*/

MacroAlternativeList
  : MacroAlternative
  | MacroAlternative '|' MacroAlternativeList
  ;

MacroAlternative
  : SymbolElement
  | SymbolElement MacroAlternative
  ;

SymbolElement
  : SymbolDefn
  | EmbeddedDefinitions
  ;

SymbolDefn
  : CSTRING
      {}
  | TYPEREFERENCE
      {}
  | TYPEREFERENCE ASSIGNMENT
      {}
  | string_t
  | identifier_t
  | number_t
  | empty_t
  | type_t
  | type_t '(' TYPE_t TYPEREFERENCE ')'
  | value_t '(' Type ')'
  | value_t '(' IDENTIFIER Type ')'
  | value_t '(' VALUE Type ')'
  ;

EmbeddedDefinitions
  : '<' EmbeddedDefinitionList '>'
  ;

EmbeddedDefinitionList
  : EmbeddedDefinition
  | EmbeddedDefinitionList EmbeddedDefinition
  ;

EmbeddedDefinition
  : LocalTypeAssignment
  | LocalValueAssignment
  ;

LocalTypeAssignment
  : TYPEREFERENCE ASSIGNMENT Type
      {}
  ;

LocalValueAssignment
  : IDENTIFIER Type ASSIGNMENT Value
      {}
  ;


/********/

ObjectTypeDefinition
  : IDENTIFIER OBJECT_TYPE
      {
	InMIBContext = TRUE;
      }
    SYNTAX Type
    ACCESS ObjectTypeAccess
    STATUS ObjectTypeStatus
    MibDescrPart
    MibReferPart
    MibIndexPart
    MibDefValPart
      {
	IdentifierTokenContext = OID_IDENTIFIER;
      }
    ASSIGNMENT Value
      {
	Module->AddMIB(new MibObject($1, $5, (MibObject::Access)$7, (MibObject::Status)$9, $10, $11, $12, $13, $16));
	InMIBContext = FALSE;
	IdentifierTokenContext = IDENTIFIER;
      }
  ;

ObjectTypeAccess
  : read_only_t
      {
	$$ = MibObject::read_only;
      }
  | read_write_t
      {
	$$ = MibObject::read_write;
      }
  | write_only_t
      {
	$$ = MibObject::write_only;
      }
  | not_accessible_t
      {
	$$ = MibObject::not_accessible;
      }
  ;

ObjectTypeStatus
  : mandatory_t
      {
	$$ = MibObject::mandatory;
      }
  | optional_t
      {
	$$ = MibObject::optional;
      }
  | obsolete_t
      {
	$$ = MibObject::obsolete;
      }
  | deprecated_t
      {
	$$ = MibObject::deprecated;
      }
  ;

MibDescrPart
  : DESCRIPTION_t CSTRING
      {
	$$ = $2;
      }
  | /* empty */
      {
	$$ = NULL;
      }
  ;

MibReferPart
  : REFERENCE_t CSTRING
      {
	$$ = $2;
      }
  | /* empty */
      {
	$$ = NULL;
      }
  ;

MibIndexPart
  : INDEX_t '{' MibIndexTypes '}'
      {
	$$ = $3;
      }
  | /* empty */
      {
	$$ = NULL;
      }
  ;

MibIndexTypes
  : MibIndexType
      {
	$$ = new PStringList;
	$$->Append($1);
      }
  | MibIndexTypes ',' MibIndexType
      {
	$1->Append($3);
      }
  ;

MibIndexType
  : IDENTIFIER
  | TYPEREFERENCE
  ;

MibDefValPart
  : DEFVAL_t '{' Value '}'
      {
	$$ = $3;
      }
  | /* empty */
      {
	$$ = NULL;
      }
  ;

TrapTypeDefinition
  : IDENTIFIER TRAP_TYPE
      {
	InMIBContext = TRUE;
	IdentifierTokenContext = OID_IDENTIFIER;
      }
    ENTERPRISE Value
    MibVarPart
    MibDescrPart
    MibReferPart
    ASSIGNMENT Value
      {
	Module->AddMIB(new MibTrap($1, $5, $6, $7, $8, $10));
	IdentifierTokenContext = IDENTIFIER;
	InMIBContext = FALSE;
      }
  ;

MibVarPart
  : VARIABLES '{' MibVarTypes '}'
      {
	$$ = $3;
      }
  | /* empty */
      {
	$$ = NULL;
      }
  ;

MibVarTypes
  : Value
      {
	$$ = new ValuesList;
	$$->Append($1);
      }
  | MibVarTypes ',' Value
      {
	$1->Append($3);
      }
  ;



/********/

/*!!! Not actually referenced by any other part of grammar
AbsoluteReference
  : '@' GlobalModuleReference '.' ItemSpec
  ;

ItemSpec
  : TYPEREFERENCE
  |  ItemId '.' ComponentId
  ;

ItemId
  : ItemSpec
  ;

ComponentId 
  : IDENTIFIER
  | INTEGER
  | '*'
  ;
*/


Reference
  : TYPEREFERENCE
  | IDENTIFIER 
  | OBJECTCLASSREFERENCE
  | OBJECTREFERENCE
  | OBJECTSETREFERENCE
  ;

ParameterizedReference
  : Reference '{' '}'
      {
	$$ = new ImportedType($1, TRUE);
      }
  ;


NamedNumberList
  : NamedNumber		
      {
	$$ = new NamedNumberList;
	$$->Append($1);
      }
  | NamedNumberList ',' NamedNumber
      {
	$1->Append($3);
      }
  ;

NamedNumber
  : IDENTIFIER '(' SignedNumber ')'	
      {
	$$ = new NamedNumber($1, (int)$3);
      }
  | IDENTIFIER '(' DefinedValue ')'
      {
	$$ = new NamedNumber($1, ((DefinedValue*)$3)->GetReference());
	delete $3;
      }
  ;


SignedNumber
  :  INTEGER 
  | '-' INTEGER
      {
	$$ = -$2;
      }
  ;


/** End of File ****/
