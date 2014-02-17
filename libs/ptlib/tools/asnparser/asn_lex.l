%{
/*
 * asn_lex.l
 *
 * ASN lexical analysis file
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
 * $Log: asn_lex.l,v $
 * Revision 1.16  2001/04/23 04:40:14  robertj
 * Added ASN standard types GeneralizedTime and UTCTime
 *
 * Revision 1.15  2001/01/17 00:52:56  robertj
 * Added option to remove warning about isatty() under Win32.
 *
 * Revision 1.14  2001/01/17 00:33:59  robertj
 * Removed extra %option, there need be only one.
 *
 * Revision 1.13  2001/01/16 14:00:17  craigs
 * Changed MIN and MAX tokens to avoid namespace conflicts under BeOS
 *
 * Revision 1.12  2001/01/16 13:26:32  rogerh
 * Remove warnings by caused by unused default output and yyunput definitions.
 * Submitted by Jac Goudsmit <jac_goudsmit@yahoo.com>
 *
 * Revision 1.11  2000/09/11 12:27:37  robertj
 * Fixed missing ! in lexical scanner, thanks Luca Faustin <luca.faustin@ipc.it>
 *
 * Revision 1.10  2000/01/19 12:33:07  robertj
 * Fixed parsing of OID's in IMPORTS section.
 *
 * Revision 1.9  1999/07/22 06:48:55  robertj
 * Added comparison operation to base ASN classes and compiled ASN code.
 * Added support for ANY type in ASN parser.
 *
 * Revision 1.8  1999/06/09 06:58:09  robertj
 * Adjusted heading comments.
 *
 * Revision 1.7  1999/06/07 01:56:25  robertj
 * Added header comment on license.
 *
 * Revision 1.6  1999/06/06 05:30:28  robertj
 * Support for parameterised types and type-dentifier types.
 * Added ability to output multiple .cxx files.
 *
 * Revision 1.5  1999/02/09 01:26:50  robertj
 * Fixed warning problems with #define for identifier "comment"
 *
 * Revision 1.4  1999/01/29 12:18:51  robertj
 * Fixed warning in standard header file, caused by macro defined in flex.skel.
 *
 * Revision 1.3  1998/05/21 08:21:50  robertj
 * GNU C++ compatibility
 *
 * Revision 1.2  1998/05/21 04:21:47  robertj
 * Implementing more of the ASN spec.
 *
 * Revision 1.1  1997/12/13 09:17:49  robertj
 * Initial revision
 *
 *
 */

#include <ptlib.h>
#include "main.h"

#include "asn_grammar.h"

#include <ctype.h>

extern FILE * yyin;

extern unsigned lineNumber;

int LexEcho = FALSE;
#define YY_USER_ACTION if (LexEcho) cout << yytext << flush;


int ReferenceTokenContext = MODULEREFERENCE;
int IdentifierTokenContext = IDENTIFIER;
int BraceTokenContext = '{';
int NullTokenContext = NULL_TYPE;
int InMacroContext = FALSE;
int HasObjectTypeMacro = FALSE;
int InMIBContext = FALSE;
TypesList * CurrentImportList = NULL;

extern "C" int yywrap() { return 1; }

static int TokenSelect(int context, int token1, int token2)
{
  if (context)
    return token1;
  yylval.sval = new PString(yytext);
  return token2;
}

%}

%option nounput nodefault never-interactive

%x comment_line

%%

"::="				   { return ASSIGNMENT; }

"ABSENT"                           { return ABSENT; }              
"ABSTRACT-SYNTAX"                  { return ABSTRACT_SYNTAX; }
"ALL"                              { return ALL; }
"ANY"                              { return ANY; }
"APPLICATION"                      { return APPLICATION; }
"AUTOMATIC"                        { return AUTOMATIC; }
"BEGIN"                            { return BEGIN_t; }
"BIT"                              { return BIT; }
"BMPString"                        { return BMPString; }
"BOOLEAN"                          { return BOOLEAN_t; }
"BY"                               { return BY; }
"CHARACTER"                        { return CHARACTER; }
"CHOICE"                           { return CHOICE; }
"CLASS"                            { return CLASS; }
"COMPONENT"                        { return COMPONENT; }
"COMPONENTS"                       { return COMPONENTS; }
"CONSTRAINED"                      { return CONSTRAINED; }
"DEFAULT"                          { return DEFAULT; }
"DEFINED"                          { return DEFINED; }
"DEFINITIONS"                      { return DEFINITIONS; }
"EMBEDDED"                         { return EMBEDDED; }
"END"                              { return END; }
"ENUMERATED"                       { return ENUMERATED; }
"EXCEPT"                           { return EXCEPT; }
"EXPLICIT"                         { return EXPLICIT; }
"EXPORTS"                          { return EXPORTS; }
"EXTERNAL"                         { return EXTERNAL; }
"FALSE"                            { return FALSE_t; }
"FROM"                             { return FROM; }
"GeneralizedTime"                  { return GeneralizedTime; }
"GeneralString"                    { return GeneralString; }
"GraphicString"                    { return GraphicString; }
"IA5String"                        { return IA5String; }
"TYPE-IDENTIFIER"                  { return TYPE_IDENTIFIER; }
"IDENTIFIER"                       { return IDENTIFIER_t; }
"IMPLICIT"                         { return IMPLICIT; }
"IMPORTS"                          { return IMPORTS; }
"INCLUDES"                         { return INCLUDES; }
"INSTANCE"                         { return INSTANCE; }
"INTEGER"                          { return INTEGER_t; }
"INTERSECTION"                     { return INTERSECTION; }
"ISO646String"                     { return ISO646String; }
"MACRO"				   { return MACRO; }
"MAX"                              { return MAX_t; }
"MIN"                              { return MIN_t; }
"MINUS-INFINITY"                   { return MINUS_INFINITY; }
"NOTATION"			   { return NOTATION; }
"NULL"                             { return NullTokenContext; }
"NumericString"                    { return NumericString; }
"OBJECT"                           { return OBJECT; }
"OCTET"                            { return OCTET; }
"OF"                               { return OF_t; }
"OPTIONAL"                         { return OPTIONAL_t; }
"PDV"                              { return PDV; }
"PLUS-INFINITY"                    { return PLUS_INFINITY; }
"PRESENT"                          { return PRESENT; }
"PrintableString"                  { return PrintableString; }
"PRIVATE"                          { return PRIVATE; }
"REAL"                             { return REAL; }
"SEQUENCE"                         { return SEQUENCE; }
"SET"                              { return SET; }
"SIZE"                             { return SIZE_t; }
"STRING"                           { return STRING; }
"SYNTAX"                           { return SYNTAX; }
"T61String"                        { return T61String; }
"TAGS"                             { return TAGS; }
"TeletexString"                    { return TeletexString; }
"TRUE"                             { return TRUE_t; }
"TYPE"				   { return TYPE_t; }
"UNION"                            { return UNION; }
"UNIQUE"                           { return UNIQUE; }
"UNIVERSAL"                        { return UNIVERSAL; }
"UniversalString"                  { return UniversalString; }
"UTCTime"                          { return UTCTime; }
"VALUE"				   { return VALUE; }
"VideotexString"                   { return VideotexString; }
"VisibleString"                    { return VisibleString; }
"WITH"                             { return WITH; }
"string"                           { return TokenSelect(InMacroContext, string_t, IDENTIFIER); }
"identifier"                       { return TokenSelect(InMacroContext, identifier_t, IDENTIFIER); }
"number"                           { return TokenSelect(InMacroContext, number_t, IDENTIFIER); }
"empty"                            { return TokenSelect(InMacroContext, empty_t, IDENTIFIER); }
"type"                             { return TokenSelect(InMacroContext, type_t, IDENTIFIER); }
"value"                            { return TokenSelect(InMacroContext, value_t, IDENTIFIER); }
"OBJECT-TYPE"			   { return TokenSelect(HasObjectTypeMacro, OBJECT_TYPE, ReferenceTokenContext); }
"TRAP-TYPE"			   { return TokenSelect(HasObjectTypeMacro, TRAP_TYPE, ReferenceTokenContext); }
"ACCESS"			   { return TokenSelect(InMIBContext, ACCESS, ReferenceTokenContext); }
"STATUS"			   { return TokenSelect(InMIBContext, STATUS, ReferenceTokenContext); }
"read-only"			   { return TokenSelect(InMIBContext, read_only_t, IDENTIFIER); }
"read-write"			   { return TokenSelect(InMIBContext, read_write_t, IDENTIFIER); }
"write-only"			   { return TokenSelect(InMIBContext, write_only_t, IDENTIFIER); }
"not-accessible"		   { return TokenSelect(InMIBContext, not_accessible_t, IDENTIFIER); }
"mandatory"			   { return TokenSelect(InMIBContext, mandatory_t, IDENTIFIER); }
"optional"			   { return TokenSelect(InMIBContext, optional_t, IDENTIFIER); }
"obsolete"			   { return TokenSelect(InMIBContext, obsolete_t, IDENTIFIER); }
"deprecated"			   { return TokenSelect(InMIBContext, deprecated_t, IDENTIFIER); }
"DESCRIPTION"			   { return TokenSelect(InMIBContext, DESCRIPTION_t, ReferenceTokenContext); }
"REFERENCE"			   { return TokenSelect(InMIBContext, REFERENCE_t, ReferenceTokenContext); }
"INDEX"				   { return TokenSelect(InMIBContext, INDEX_t, ReferenceTokenContext); }
"DEFVAL"			   { return TokenSelect(InMIBContext, DEFVAL_t, ReferenceTokenContext); }
"ENTERPRISE"			   { return TokenSelect(InMIBContext, ENTERPRISE, ReferenceTokenContext); }
"VARIABLES"			   { return TokenSelect(InMIBContext, VARIABLES, ReferenceTokenContext); }

                                                                 

"--" {
    BEGIN(comment_line);
  }

<comment_line>"--"  {
    BEGIN(INITIAL);
  }

<comment_line>\n  {
    lineNumber++;
    BEGIN(INITIAL);
  }

<comment_line>. {
}


"@"  { return '@'; }

"{"  { return BraceTokenContext; }

"}"  { return '}'; }

"["  { return '['; }

"]"  { return ']'; }

"("  { return '('; }

")"  { return ')'; }

"<"  { return '<'; }

">"  { return '>'; }

"|"  { return '|'; }

";"  { return ';'; }

"+"  { return '+'; }

"-"  { return '-'; }

"*"  { return '*'; }

"/"  { return '/'; }

","  { return ','; }

"."  { return '.'; }

":"  { return ':'; }

"="  { return '='; }

'|'  { return '|'; }

"^"  { return '^'; }

"!"  { return '!'; }

(\"[^\"]*\")(\"[^\"]*\")*  {
    PString s = yytext;
    PINDEX pos;
    while ((pos = s.Find('\n')) != P_MAX_INDEX) {
      PINDEX start = pos;
      while (start > 0 && isspace(s[start-1]))
	start--;
      while (isspace(s[pos]))
	pos++;
      s.Splice(" ", start, pos - start);
      lineNumber++;
    }
    yylval.sval = new PString(s);
    return CSTRING;
  }

[0-9][0-9]*  {
    const char * ptr = yytext;
    yylval.ival = 0;
    while (*ptr != '\0')
      yylval.ival = yylval.ival * 10 + *ptr++ - '0';
    return INTEGER;
  }

([a-z]|([a-z][a-zA-Z0-9_-]*[a-zA-Z0-9_]))  {
    yylval.sval = new PString(yytext);
    if (CurrentImportList != NULL) {
      ImportedType searchArg(new PString(yytext), FALSE);
      if (CurrentImportList->GetValuesIndex(searchArg) != P_MAX_INDEX)
        return IMPORT_IDENTIFIER;
    }
    return IdentifierTokenContext;
  }

"&"([a-z]|([a-z][a-zA-Z0-9_-]*[a-zA-Z0-9_]))  {   /* See X.681 section 7.5 */
    yylval.sval = new PString(yytext);
    return VALUEFIELDREFERENCE;
  }

"&"([A-Z]|([A-Z][a-zA-Z0-9_-]*[a-zA-Z0-9_]))  {	  /* See X.681 section 7.4 */
    yylval.sval = new PString(yytext);
    return TYPEFIELDREFERENCE;
  }

([A-Z]|([A-Z][a-zA-Z0-9_-]*[a-zA-Z0-9_]))  {	  /* See X.680 section 9.2 */
    yylval.sval = new PString(yytext);
    if (CurrentImportList != NULL &&
	CurrentImportList->GetValuesIndex(SearchType(*yylval.sval)) != P_MAX_INDEX)
      return IMPORT_IDENTIFIER;
    return ReferenceTokenContext;
  }

[ \t\r]  { }

\n {
    lineNumber++;
  }

.  { PError << StdError(Fatal) << "unknown token " << yytext << endl; }


%%
/* End PR_LEX.L */
