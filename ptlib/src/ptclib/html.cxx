/*
 * html.cxx
 *
 * HTML classes.
 *
 * Portable Windows Library
 *
 * Copyright (c) 1993-2002 Equivalence Pty. Ltd.
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
 * $Log: html.cxx,v $
 * Revision 1.21  2004/04/12 05:42:25  csoutheren
 * Fixed problem with radio buttons
 *
 * Revision 1.20  2004/04/03 06:54:24  rjongbloed
 * Many and various changes to support new Visual C++ 2003
 *
 * Revision 1.19  2002/11/06 22:47:24  robertj
 * Fixed header comment (copyright etc)
 *
 * Revision 1.18  2001/02/13 04:39:08  robertj
 * Fixed problem with operator= in container classes. Some containers will
 *   break unless the copy is virtual (eg PStringStream's buffer pointers) so
 *   needed to add a new AssignContents() function to all containers.
 *
 * Revision 1.17  1998/11/30 04:51:51  robertj
 * New directory structure
 *
 * Revision 1.16  1998/09/23 06:22:04  robertj
 * Added open source copyright license.
 *
 * Revision 1.15  1998/01/26 02:49:15  robertj
 * GNU support.
 *
 * Revision 1.14  1997/06/16 13:18:03  robertj
 * Set Is() function to be const as it should have been.
 *
 * Revision 1.13  1996/08/19 13:40:31  robertj
 * Fixed incorrect formatting of HTML tags (cosmetic only).
 *
 * Revision 1.12  1996/06/28 13:08:55  robertj
 * Changed PHTML class so can create html fragments.
 * Fixed nesting problem in tables.
 *
 * Revision 1.11  1996/06/01 04:18:45  robertj
 * Fixed bug in RadioButton, having 2 VALUE fields
 *
 * Revision 1.10  1996/04/29 12:21:22  robertj
 * Fixed spelling error in assert.
 * Fixed check box HTML, should always have a value.
 * Added display of value of unclosed HTML element.
 *
 * Revision 1.9  1996/04/14 02:52:04  robertj
 * Added hidden fields to HTML.
 *
 * Revision 1.8  1996/03/31 09:03:07  robertj
 * Changed HTML token so doesn't have trailing CRLF.
 *
 * Revision 1.7  1996/03/16 04:54:06  robertj
 * Made the assert for unclosed HTML elements only on debug version.
 *
 * Revision 1.6  1996/03/12 11:30:33  robertj
 * Fixed resetting of HTML output using operator=.
 *
 * Revision 1.5  1996/03/10 13:14:55  robertj
 * Simplified some of the classes and added catch all string for attributes.
 *
 * Revision 1.4  1996/02/25 11:14:22  robertj
 * Radio button support for forms.
 *
 * Revision 1.3  1996/02/19 13:31:51  robertj
 * Removed MSC_VER test as now completely removed from WIN16 library.
 *
 * Revision 1.2  1996/02/08 12:24:30  robertj
 * Further implementation.
 *
 * Revision 1.1  1996/02/03 11:18:46  robertj
 * Initial revision
 *
 * Revision 1.3  1996/01/28 02:49:16  robertj
 * Further implementation.
 *
 * Revision 1.2  1996/01/26 02:24:30  robertj
 * Further implemetation.
 *
 * Revision 1.1  1996/01/23 13:04:32  robertj
 * Initial revision
 *
 */

#ifdef __GNUC__
#pragma implementation "html.h"
#endif

#include <ptlib.h>
#include <ptclib/html.h>


//////////////////////////////////////////////////////////////////////////////
// PHTML

PHTML::PHTML(ElementInSet initialState)
{
  memset(elementSet, 0, sizeof(elementSet));
  tableNestLevel = 0;
  initialElement = initialState;
  switch (initialState) {
    case NumElementsInSet :
      break;
    case InBody :
      Set(InBody);
      break;
    case InForm :
      Set(InBody);
      Set(InForm);
      break;
    default :
      PAssertAlways(PInvalidParameter);
  }
}


PHTML::PHTML(const char * cstr)
{
  memset(elementSet, 0, sizeof(elementSet));
  tableNestLevel = 0;
  initialElement = NumElementsInSet;
  ostream & this_stream = *this;
  this_stream << Title(cstr) << Body() << Heading(1) << cstr << Heading(1);
}


PHTML::PHTML(const PString & str)
{
  memset(elementSet, 0, sizeof(elementSet));
  tableNestLevel = 0;
  initialElement = NumElementsInSet;
  ostream & this_stream = *this;
  this_stream << Title(str) << Body() << Heading(1) << str << Heading(1);
}


PHTML::~PHTML()
{
#ifndef NDEBUG
  if (initialElement != NumElementsInSet) {
    Clr(initialElement);
    Clr(InBody);
  }
  for (PINDEX i = 0; i < PARRAYSIZE(elementSet); i++)
    PAssert(elementSet[i] == 0, psprintf("Failed to close element %u", i));
#endif
}


void PHTML::AssignContents(const PContainer & cont)
{
  PStringStream::AssignContents(cont);
  memset(elementSet, 0, sizeof(elementSet));
}


BOOL PHTML::Is(ElementInSet elmt) const
{
  return (elementSet[elmt>>3]&(1<<(elmt&7))) != 0;
}


void PHTML::Set(ElementInSet elmt)
{
  elementSet[elmt>>3] |= (1<<(elmt&7));
}


void PHTML::Clr(ElementInSet elmt)
{
  elementSet[elmt>>3] &= ~(1<<(elmt&7));
}


void PHTML::Toggle(ElementInSet elmt)
{
  elementSet[elmt>>3] ^= (1<<(elmt&7));
}


void PHTML::Element::Output(PHTML & html) const
{
  PAssert(reqElement == NumElementsInSet || html.Is(reqElement),
                                                "HTML element out of context");

  if (crlf == BothCRLF || (crlf == OpenCRLF && !html.Is(inElement)))
    html << "\r\n";

  html << '<';
  if (html.Is(inElement))
    html << '/';
  html << name;

  AddAttr(html);

  if (attr != NULL)
    html << ' ' << attr;

  html << '>';
  if (crlf == BothCRLF || (crlf == CloseCRLF && html.Is(inElement)))
    html << "\r\n";

  if (inElement != NumElementsInSet)
    html.Toggle(inElement);
}


void PHTML::Element::AddAttr(PHTML &) const
{
}


PHTML::HTML::HTML(const char * attr)
  : Element("HTML", attr, InHTML, NumElementsInSet, BothCRLF)
{
}

PHTML::Head::Head()
  : Element("HEAD", NULL, InHead, NumElementsInSet, BothCRLF)
{
}

void PHTML::Head::Output(PHTML & html) const
{
  PAssert(!html.Is(InBody), "HTML element out of context");
  if (!html.Is(InHTML))
    html << HTML();
  Element::Output(html);
}


PHTML::Body::Body(const char * attr)
  : Element("BODY", attr, InBody, NumElementsInSet, BothCRLF)
{
}


void PHTML::Body::Output(PHTML & html) const
{
  if (!html.Is(InHTML))
    html << HTML();
  if (html.Is(InTitle))
    html << Title();
  if (html.Is(InHead))
    html << Head();
  Element::Output(html);
  if (!html.Is(InBody))
    html << HTML();
}


PHTML::Title::Title()
  : Element("TITLE", NULL, InTitle, InHead, CloseCRLF)
{
  titleString = NULL;
}

PHTML::Title::Title(const char * titleCStr)
  : Element("TITLE", NULL, InTitle, InHead, CloseCRLF)
{
  titleString = titleCStr;
}

PHTML::Title::Title(const PString & titleStr)
  : Element("TITLE", NULL, InTitle, InHead, CloseCRLF)
{
  titleString = titleStr;
}

void PHTML::Title::Output(PHTML & html) const
{
  PAssert(!html.Is(InBody), "HTML element out of context");
  if (!html.Is(InHead))
    html << Head();
  if (html.Is(InTitle)) {
    if (titleString != NULL)
      html << titleString;
    Element::Output(html);
  }
  else {
    Element::Output(html);
    if (titleString != NULL) {
      html << titleString;
      Element::Output(html);
    }
  }
}


PHTML::Banner::Banner(const char * attr)
  : Element("BANNER", attr, NumElementsInSet, InBody, BothCRLF)
{
}


PHTML::Division::Division(const char * attr)
  : Element("DIV", attr, InDivision, InBody, BothCRLF)
{
}


PHTML::Heading::Heading(int number,
                        int sequence,
                        int skip,
                        const char * attr)
  : Element("H", attr, InHeading, InBody, CloseCRLF)
{
  num = number;
  srcString = NULL;
  seqNum = sequence;
  skipSeq = skip;
}

PHTML::Heading::Heading(int number,
                        const char * image,
                        int sequence,
                        int skip,
                        const char * attr)
  : Element("H", attr, InHeading, InBody, CloseCRLF)
{
  num = number;
  srcString = image;
  seqNum = sequence;
  skipSeq = skip;
}

PHTML::Heading::Heading(int number,
                        const PString & imageStr,
                        int sequence,
                        int skip,
                        const char * attr)
  : Element("H", attr, InHeading, InBody, CloseCRLF)
{
  num = number;
  srcString = imageStr;
  seqNum = sequence;
  skipSeq = skip;
}

void PHTML::Heading::AddAttr(PHTML & html) const
{
  PAssert(num >= 1 && num <= 6, "Bad heading number");
  html << num;
  if (srcString != NULL)
    html << " SRC=\"" << srcString << '"';
  if (seqNum > 0)
    html << " SEQNUM=" << seqNum;
  if (skipSeq > 0)
    html << " SKIP=" << skipSeq;
}


PHTML::BreakLine::BreakLine(const char * attr)
  : Element("BR", attr, NumElementsInSet, InBody, CloseCRLF)
{
}


PHTML::Paragraph::Paragraph(const char * attr)
  : Element("P", attr, NumElementsInSet, InBody, OpenCRLF)
{
}


PHTML::PreFormat::PreFormat(int widthInChars, const char * attr)
  : Element("PRE", attr, InPreFormat, InBody, CloseCRLF)
{
  width = widthInChars;
}


void PHTML::PreFormat::AddAttr(PHTML & html) const
{
  if (width > 0)
    html << " WIDTH=" << width;
}


PHTML::HotLink::HotLink(const char * href, const char * attr)
  : Element("A", attr, InAnchor, InBody, NoCRLF)
{
  hrefString = href;
}

void PHTML::HotLink::AddAttr(PHTML & html) const
{
  if (hrefString != NULL && *hrefString != '\0')
    html << " HREF=\"" << hrefString << '"';
  else
    PAssert(html.Is(InAnchor), PInvalidParameter);
}


PHTML::Target::Target(const char * name, const char * attr)
  : Element("A", attr, NumElementsInSet, InBody, NoCRLF)
{
  nameString = name;
}

void PHTML::Target::AddAttr(PHTML & html) const
{
  if (nameString != NULL && *nameString != '\0')
    html << " NAME=\"" << nameString << '"';
}


PHTML::ImageElement::ImageElement(const char * n,
                                  const char * attr,
                                  ElementInSet elmt,
                                  ElementInSet req,
                                  OptionalCRLF c,
                                  const char * image)
  : Element(n, attr, elmt, req, c)
{
  srcString = image;
}


void PHTML::ImageElement::AddAttr(PHTML & html) const
{
  if (srcString != NULL)
    html << " SRC=\"" << srcString << '"';
}


PHTML::Image::Image(const char * src, int w, int h, const char * attr)
  : ImageElement("IMG", attr, NumElementsInSet, InBody, NoCRLF, src)
{
  altString = NULL;
  width = w;
  height = h;
}

PHTML::Image::Image(const char * src,
                    const char * alt,
                    int w, int h,
                    const char * attr)
  : ImageElement("IMG", attr, NumElementsInSet, InBody, NoCRLF, src)
{
  altString = alt;
  width = w;
  height = h;
}

void PHTML::Image::AddAttr(PHTML & html) const
{
  PAssert(srcString != NULL && *srcString != '\0', PInvalidParameter);
  if (altString != NULL)
    html << " ALT=\"" << altString << '"';
  if (width != 0)
    html << " WIDTH=" << width;
  if (height != 0)
    html << " HEIGHT=" << height;
  ImageElement::AddAttr(html);
}


PHTML::HRule::HRule(const char * image, const char * attr)
  : ImageElement("HR", attr, NumElementsInSet, InBody, BothCRLF, image)
{
}


PHTML::Note::Note(const char * image, const char * attr)
  : ImageElement("NOTE", attr, InNote, InBody, BothCRLF, image)
{
}


PHTML::Address::Address(const char * attr)
  : Element("ADDRESS", attr, InAddress, InBody, BothCRLF)
{
}


PHTML::BlockQuote::BlockQuote(const char * attr)
  : Element("BQ", attr, InBlockQuote, InBody, BothCRLF)
{
}


PHTML::Credit::Credit(const char * attr)
  : Element("CREDIT", attr, NumElementsInSet, InBlockQuote, OpenCRLF)
{
}

PHTML::SetTab::SetTab(const char * id, const char * attr)
  : Element("TAB", attr, NumElementsInSet, InBody, NoCRLF)
{
  ident = id;
}

void PHTML::SetTab::AddAttr(PHTML & html) const
{
  PAssert(ident != NULL && *ident != '\0', PInvalidParameter);
  html << " ID=" << ident;
}


PHTML::Tab::Tab(int indent, const char * attr)
  : Element("TAB", attr, NumElementsInSet, InBody, NoCRLF)
{
  ident = NULL;
  indentSize = indent;
}

PHTML::Tab::Tab(const char * id, const char * attr)
  : Element("TAB", attr, NumElementsInSet, InBody, NoCRLF)
{
  ident = id;
  indentSize = 0;
}

void PHTML::Tab::AddAttr(PHTML & html) const
{
  PAssert(indentSize!=0 || (ident!=NULL && *ident!='\0'), PInvalidParameter);
  if (indentSize > 0)
    html << " INDENT=" << indentSize;
  else
    html << " TO=" << ident;
}


PHTML::SimpleList::SimpleList(const char * attr)
  : Element("UL", attr, InList, InBody, BothCRLF)
{
}

void PHTML::SimpleList::AddAttr(PHTML & html) const
{
  html << " PLAIN";
}


PHTML::BulletList::BulletList(const char * attr)
  : Element("UL", attr, InList, InBody, BothCRLF)
{
}


PHTML::OrderedList::OrderedList(int seqNum, const char * attr)
  : Element("OL", attr, InList, InBody, BothCRLF)
{
  sequenceNum = seqNum;
}

void PHTML::OrderedList::AddAttr(PHTML & html) const
{
  if (sequenceNum > 0)
    html << " SEQNUM=" << sequenceNum;
  if (sequenceNum < 0)
    html << " CONTINUE";
}


PHTML::DefinitionList::DefinitionList(const char * attr)
  : Element("DL", attr, InList, InBody, BothCRLF)
{
}


PHTML::ListHeading::ListHeading(const char * attr)
  : Element("LH", attr, InListHeading, InList, CloseCRLF)
{
}

PHTML::ListItem::ListItem(int skip, const char * attr)
  : Element("LI", attr, NumElementsInSet, InList, OpenCRLF)
{
  skipSeq = skip;
}

void PHTML::ListItem::AddAttr(PHTML & html) const
{
  if (skipSeq > 0)
    html << " SKIP=" << skipSeq;
}


PHTML::DefinitionTerm::DefinitionTerm(const char * attr)
  : Element("DT", attr, NumElementsInSet, InList, NoCRLF)
{
}

void PHTML::DefinitionTerm::Output(PHTML & html) const
{
  PAssert(!html.Is(InDefinitionTerm), "HTML definition item missing");
  Element::Output(html);
  html.Set(InDefinitionTerm);
}


PHTML::DefinitionItem::DefinitionItem(const char * attr)
  : Element("DD", attr, NumElementsInSet, InList, NoCRLF)
{
}

void PHTML::DefinitionItem::Output(PHTML & html) const
{
  PAssert(html.Is(InDefinitionTerm), "HTML definition term missing");
  Element::Output(html);
  html.Clr(InDefinitionTerm);
}


PHTML::TableStart::TableStart(const char * attr)
  : Element("TABLE", attr, InTable, InBody, BothCRLF)
{
  borderFlag = FALSE;
}

PHTML::TableStart::TableStart(BorderCodes border, const char * attr)
  : Element("TABLE", attr, InTable, InBody, BothCRLF)
{
  borderFlag = border == Border;
}

void PHTML::TableStart::Output(PHTML & html) const
{
  if (html.tableNestLevel > 0)
    html.Clr(InTable);
  Element::Output(html);
}

void PHTML::TableStart::AddAttr(PHTML & html) const
{
  if (borderFlag)
    html << " BORDER";
  html.tableNestLevel++;
}


PHTML::TableEnd::TableEnd()
  : Element("TABLE", "", InTable, InBody, BothCRLF)
{
}

void PHTML::TableEnd::Output(PHTML & html) const
{
  PAssert(html.tableNestLevel > 0, "Table nesting error");
  Element::Output(html);
  html.tableNestLevel--;
  if (html.tableNestLevel > 0)
    html.Set(InTable);
}


PHTML::TableRow::TableRow(const char * attr)
  : Element("TR", attr, NumElementsInSet, InTable, OpenCRLF)
{
}


PHTML::TableHeader::TableHeader(const char * attr)
  : Element("TH", attr, NumElementsInSet, InTable, CloseCRLF)
{
}


PHTML::TableData::TableData(const char * attr)
  : Element("TD", attr, NumElementsInSet, InTable, NoCRLF)
{
}


PHTML::Form::Form(const char * method,
                  const char * action,
                  const char * mimeType,
                  const char * script)
  : Element("FORM", NULL, InForm, InBody, BothCRLF)
{
  methodString = method;
  actionString = action;
  mimeTypeString = mimeType;
  scriptString = script;
}

void PHTML::Form::AddAttr(PHTML & html) const
{
  if (methodString != NULL)
    html << " METHOD=" << methodString;
  if (actionString != NULL)
    html << " ACTION=\"" << actionString << '"';
  if (mimeTypeString != NULL)
    html << " ENCTYPE=\"" << mimeTypeString << '"';
  if (scriptString != NULL)
    html << " SCRIPT=\"" << scriptString << '"';
}


PHTML::FieldElement::FieldElement(const char * n,
                                  const char * attr,
                                  ElementInSet elmt,
                                  OptionalCRLF c,
                                  DisableCodes disabled)
  : Element(n, attr, elmt, InForm, c)
{
  disabledFlag = disabled == Disabled;
}

void PHTML::FieldElement::AddAttr(PHTML & html) const
{
  if (disabledFlag)
    html << " DISABLED";
}


PHTML::Select::Select(const char * fname, const char * attr)
  : FieldElement("SELECT", attr, InSelect, BothCRLF, Enabled)
{
  nameString = fname;
}

PHTML::Select::Select(const char * fname,
                      DisableCodes disabled,
                      const char * attr)
  : FieldElement("SELECT", attr, InSelect, BothCRLF, disabled)
{
  nameString = fname;
}

void PHTML::Select::AddAttr(PHTML & html) const
{
  if (!html.Is(InSelect)) {
    PAssert(nameString != NULL && *nameString != '\0', PInvalidParameter);
    html << " NAME=\"" << nameString << '"';
  }
  FieldElement::AddAttr(html);
}


PHTML::Option::Option(const char * attr)
  : FieldElement("OPTION", attr, NumElementsInSet, NoCRLF, Enabled)
{
  selectedFlag = FALSE;
}

PHTML::Option::Option(SelectionCodes select,
                      const char * attr)
  : FieldElement("OPTION", attr, NumElementsInSet, NoCRLF, Enabled)
{
  selectedFlag = select == Selected;
}

PHTML::Option::Option(DisableCodes disabled,
                      const char * attr)
  : FieldElement("OPTION", attr, NumElementsInSet, NoCRLF, disabled)
{
  selectedFlag = FALSE;
}

PHTML::Option::Option(SelectionCodes select,
                      DisableCodes disabled,
                      const char * attr)
  : FieldElement("OPTION", attr, NumElementsInSet, NoCRLF, disabled)
{
  selectedFlag = select == Selected;
}

void PHTML::Option::AddAttr(PHTML & html) const
{
  if (selectedFlag)
    html << " SELECTED";
  FieldElement::AddAttr(html);
}


PHTML::FormField::FormField(const char * n,
                            const char * attr,
                            ElementInSet elmt,
                            OptionalCRLF c,
                            DisableCodes disabled,
                            const char * fname)
  : FieldElement(n, attr, elmt, c, disabled)
{
  nameString = fname;
}

void PHTML::FormField::AddAttr(PHTML & html) const
{
  PAssert(nameString != NULL && *nameString != '\0', PInvalidParameter);
  html << " NAME=\"" << nameString << '"';
  FieldElement::AddAttr(html);
}


PHTML::TextArea::TextArea(const char * fname,
                          DisableCodes disabled,
                          const char * attr)
  : FormField("TEXTAREA", attr, InSelect, BothCRLF, disabled, fname)
{
  numRows = numCols = 0;
}

PHTML::TextArea::TextArea(const char * fname,
                          int rows, int cols,
                          DisableCodes disabled,
                          const char * attr)
  : FormField("TEXTAREA", attr, InSelect, BothCRLF, disabled, fname)
{
  numRows = rows;
  numCols = cols;
}

void PHTML::TextArea::AddAttr(PHTML & html) const
{
  if (numRows > 0)
    html << " ROWS=" << numRows;
  if (numCols > 0)
    html << " COLS=" << numCols;
  FormField::AddAttr(html);
}


PHTML::InputField::InputField(const char * type,
                              const char * fname,
                              DisableCodes disabled,
                              const char * attr)
  : FormField("INPUT", attr, NumElementsInSet, NoCRLF, disabled, fname)
{
  typeString = type;
}

void PHTML::InputField::AddAttr(PHTML & html) const
{
  PAssert(typeString != NULL && *typeString != '\0', PInvalidParameter);
  html << " TYPE=" << typeString;
  FormField::AddAttr(html);
}


PHTML::HiddenField::HiddenField(const char * fname,
                                const char * value,
                                const char * attr)
  : InputField("hidden", fname, Enabled, attr)
{
  valueString = value;
}

void PHTML::HiddenField::AddAttr(PHTML & html) const
{
  InputField::AddAttr(html);
  PAssert(valueString != NULL, PInvalidParameter);
  html << " VALUE=\"" << valueString << '"';
}


PHTML::InputText::InputText(const char * fname,
                            int size,
                            const char * init,
                            const char * attr)
  : InputField("text", fname, Enabled, attr)
{
  width = size;
  length = 0;
  value = init;
}

PHTML::InputText::InputText(const char * fname,
                            int size,
                            DisableCodes disabled,
                            const char * attr)
  : InputField("text", fname, disabled, attr)
{
  width = size;
  length = 0;
  value = NULL;
}

PHTML::InputText::InputText(const char * fname,
                            int size,
                            int maxLength,
                            DisableCodes disabled,
                            const char * attr)
  : InputField("text", fname, disabled, attr)
{
  width = size;
  length = maxLength;
  value = NULL;
}

PHTML::InputText::InputText(const char * fname,
                            int size,
                            const char * init,
                            int maxLength,
                            DisableCodes disabled,
                            const char * attr)
  : InputField("text", fname, disabled, attr)
{
  width = size;
  length = maxLength;
  value = init;
}

PHTML::InputText::InputText(const char * type,
                            const char * fname,
                            int size,
                            const char * init,
                            int maxLength,
                            DisableCodes disabled,
                            const char * attr)
  : InputField(type, fname, disabled, attr)
{
  width = size;
  length = maxLength;
  value = init;
}

void PHTML::InputText::AddAttr(PHTML & html) const
{
  InputField::AddAttr(html);
  html << " SIZE=" << width;
  if (length > 0)
    html << " MAXLENGTH=" << length;
  if (value != NULL)
    html << " VALUE=\"" << value << '"';
}


PHTML::InputPassword::InputPassword(const char * fname,
                                    int size,
                                    const char * init,
                                    const char * attr)
  : InputText("password", fname, size, init, 0, Enabled, attr)
{
}

PHTML::InputPassword::InputPassword(const char * fname,
                                    int size,
                                    DisableCodes disabled,
                                    const char * attr)
  : InputText("password", fname, size, NULL, 0, disabled, attr)
{
}

PHTML::InputPassword::InputPassword(const char * fname,
                                    int size,
                                    int maxLength,
                                    DisableCodes disabled,
                                    const char * attr)
  : InputText("password", fname, size, NULL, maxLength, disabled, attr)
{
}

PHTML::InputPassword::InputPassword(const char * fname,
                                    int size,
                                    const char * init,
                                    int maxLength,
                                    DisableCodes disabled,
                                    const char * attr)
  : InputText("password", fname, size, init, maxLength, disabled, attr)
{
}


PHTML::RadioButton::RadioButton(const char * fname,
                                const char * value,
                                const char * attr)
  : InputField("radio", fname, Enabled, attr)
{
  valueString = value;
  checkedFlag = FALSE;
}

PHTML::RadioButton::RadioButton(const char * fname,
                                const char * value,
                                DisableCodes disabled,
                                const char * attr)
  : InputField("radio", fname, disabled, attr)
{
  valueString = value;
  checkedFlag = FALSE;
}

PHTML::RadioButton::RadioButton(const char * fname,
                                const char * value,
                                CheckedCodes check,
                                DisableCodes disabled,
                                const char * attr)
  : InputField("radio", fname, disabled, attr)
{
  valueString = value;
  checkedFlag = check == Checked;
}

PHTML::RadioButton::RadioButton(const char * type,
                                const char * fname,
                                const char * value,
                                CheckedCodes check,
                                DisableCodes disabled,
                                const char * attr)
  : InputField(type, fname, disabled, attr)
{
  valueString = value;
  checkedFlag = check == Checked;
}

void PHTML::RadioButton::AddAttr(PHTML & html) const
{
  InputField::AddAttr(html);
  PAssert(valueString != NULL, PInvalidParameter);
  html << " VALUE=\"" << valueString << "\"";
  if (checkedFlag)
    html << " CHECKED";
}


PHTML::CheckBox::CheckBox(const char * fname, const char * attr)
  : RadioButton("checkbox", fname, "TRUE", UnChecked, Enabled, attr)
{
}

PHTML::CheckBox::CheckBox(const char * fname,
                          DisableCodes disabled,
                          const char * attr)
  : RadioButton("checkbox", fname, "TRUE", UnChecked, disabled, attr)
{
}

PHTML::CheckBox::CheckBox(const char * fname,
                          CheckedCodes check,
                          DisableCodes disabled,
                          const char * attr)
  : RadioButton("checkbox", fname, "TRUE", check, disabled, attr)
{
}


PHTML::InputRange::InputRange(const char * fname,
                              int min, int max, int value,
                              DisableCodes disabled,
                              const char * attr)
  : InputField("range", fname, disabled, attr)
{
  PAssert(min <= max, PInvalidParameter);
  minValue = min;
  maxValue = max;
  if (value < min)
    initValue = min;
  else if (value > max)
    initValue = max;
  else
    initValue = value;
}

void PHTML::InputRange::AddAttr(PHTML & html) const
{
  InputField::AddAttr(html);
  PINDEX max = PMAX(-minValue, maxValue);
  PINDEX width = 3;
  while (max > 10) {
    width++;
    max /= 10;
  }
  html << " SIZE=" << width
       << " MIN=" << minValue
       << " MAX=" << maxValue
       << " VALUE=\"" << initValue << "\"";
}


PHTML::InputFile::InputFile(const char * fname,
                            const char * accept,
                            DisableCodes disabled,
                            const char * attr)
  : InputField("file", fname, disabled, attr)
{
  acceptString = accept;
}

void PHTML::InputFile::AddAttr(PHTML & html) const
{
  InputField::AddAttr(html);
  if (acceptString != NULL)
    html << " ACCEPT=\"" << acceptString << '"';
}


PHTML::InputImage::InputImage(const char * fname,
                              const char * src,
                              DisableCodes disabled,
                              const char * attr)
  : InputField("image", fname, disabled, attr)
{
  srcString = src;
}

PHTML::InputImage::InputImage(const char * type,
                              const char * fname,
                              const char * src,
                              DisableCodes disabled,
                              const char * attr)
  : InputField(type, fname, disabled, attr)
{
  srcString = src;
}

void PHTML::InputImage::AddAttr(PHTML & html) const
{
  InputField::AddAttr(html);
  if (srcString != NULL)
    html << " SRC=\"" << srcString << '"';
}


PHTML::InputScribble::InputScribble(const char * fname,
                                    const char * src,
                                    DisableCodes disabled,
                                    const char * attr)
  : InputImage("scribble", fname, src, disabled, attr)
{
}

PHTML::ResetButton::ResetButton(const char * title,
                                const char * fname,
                                const char * src,
                                DisableCodes disabled,
                                const char * attr)
  : InputImage("reset", fname != NULL ? fname : "reset", src, disabled, attr)
{
  titleString = title;
}

PHTML::ResetButton::ResetButton(const char * type,
                                const char * title,
                                const char * fname,
                                const char * src,
                                DisableCodes disabled,
                                const char * attr)
  : InputImage(type, fname, src, disabled, attr)
{
  titleString = title;
}

void PHTML::ResetButton::AddAttr(PHTML & html) const
{
  InputImage::AddAttr(html);
  if (titleString != NULL)
    html << " VALUE=\"" << titleString << '"';
}


PHTML::SubmitButton::SubmitButton(const char * title,
                                  const char * fname,
                                  const char * src,
                                  DisableCodes disabled,
                                  const char * attr)
  : ResetButton("submit",
                  title, fname != NULL ? fname : "submit", src, disabled, attr)
{
}

// End Of File ///////////////////////////////////////////////////////////////
