/*
 * html.h
 *
 * HyperText Markup Language stream classes.
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
 * $Log: html.h,v $
 * Revision 1.27  2005/11/30 12:47:37  csoutheren
 * Removed tabs, reformatted some code, and changed tags for Doxygen
 *
 * Revision 1.26  2005/10/30 23:25:51  csoutheren
 * Fixed formatting
 * Removed throw() declarations (PWLib does not do exceptions)
 * Removed duplicate destructor declarations and definitions
 *
 * Revision 1.25  2005/10/30 19:41:53  dominance
 * fixed most of the warnings occuring during compilation
 *
 * Revision 1.24  2005/03/19 02:52:53  csoutheren
 * Fix warnings from gcc 4.1-20050313 shapshot
 *
 * Revision 1.23  2002/11/06 22:47:23  robertj
 * Fixed header comment (copyright etc)
 *
 * Revision 1.22  2002/09/16 01:08:59  robertj
 * Added #define so can select if #pragma interface/implementation is used on
 *   platform basis (eg MacOS) rather than compiler, thanks Robert Monaghan.
 *
 * Revision 1.21  2001/02/13 04:39:08  robertj
 * Fixed problem with operator= in container classes. Some containers will
 *   break unless the copy is virtual (eg PStringStream's buffer pointers) so
 *   needed to add a new AssignContents() function to all containers.
 *
 * Revision 1.20  1999/03/09 08:01:46  robertj
 * Changed comments for doc++ support (more to come).
 *
 * Revision 1.19  1999/02/16 08:07:10  robertj
 * MSVC 6.0 compatibility changes.
 *
 * Revision 1.18  1998/09/23 06:19:27  robertj
 * Added open source copyright license.
 *
 * Revision 1.17  1997/07/08 13:15:31  robertj
 * DLL support.
 *
 * Revision 1.16  1997/06/16 13:18:02  robertj
 * Set Is() function to be const as it should have been.
 *
 * Revision 1.15  1996/08/17 10:00:18  robertj
 * Changes for Windows DLL support.
 *
 * Revision 1.14  1996/06/28 13:08:41  robertj
 * Changed PHTML class so can create html fragments.
 * Fixed nesting problem in tables.
 *
 * Revision 1.13  1996/06/01 04:18:40  robertj
 * Fixed bug in RadioButton, having 2 VALUE fields
 *
 * Revision 1.12  1996/04/14 02:52:02  robertj
 * Added hidden fields to HTML.
 *
 * Revision 1.11  1996/03/12 11:30:00  robertj
 * Fixed resetting of HTML output using operator=.
 *
 * Revision 1.10  1996/03/10 13:14:53  robertj
 * Simplified some of the classes and added catch all string for attributes.
 *
 * Revision 1.9  1996/03/03 07:36:44  robertj
 * Added missing public's to standard character attribute classes.
 *
 * Revision 1.8  1996/02/25 11:14:19  robertj
 * Radio button support for forms.
 *
 * Revision 1.7  1996/02/19 13:18:25  robertj
 * Removed MSC_VER test as now completely removed from WIN16 library.
 *
 * Revision 1.6  1996/02/08 11:50:38  robertj
 * More implementation.
 *
 * Revision 1.5  1996/02/03 11:01:25  robertj
 * Further implementation.
 *
 * Revision 1.4  1996/01/28 14:15:56  robertj
 * More comments.
 *
 * Revision 1.3  1996/01/28 02:45:38  robertj
 * Further implementation.
 *
 * Revision 1.2  1996/01/26 02:24:24  robertj
 * Further implemetation.
 *
 * Revision 1.1  1996/01/24 23:45:37  robertj
 * Initial revision
 *
 */

#ifndef _PHTML
#define _PHTML

#ifdef P_USE_PRAGMA
#pragma interface
#endif



//////////////////////////////////////////////////////////////////////////////
// PHTML

/** This class describes a HyperText markup Language string as used by the
   World Wide Web and the #PURL# and #PHTTPSocket# class.
   
   All of the standard stream I/O operators, manipulators etc will operate on
   the PString class.
 */
class PHTML : public PStringStream
{
  PCLASSINFO(PHTML, PStringStream)

  public:
    enum ElementInSet {
      InHTML,
      InHead,
      InBody,
      InTitle,
      InHeading,
      InDivision,
      InPreFormat,
      InAnchor,
      InNote,
      InAddress,
      InBlockQuote,
      InCredit,
      InBold,
      InItalic,
      InTeleType,
      InUnderline,
      InStrikeThrough,
      InBig,
      InSmall,
      InSubscript,
      InSuperscript,
      InEmphasis,
      InCite,
      InStrong,
      InCode,
      InSample,
      InKeyboard,
      InVariable,
      InDefinition,
      InQuote,
      InAuthor,
      InPerson,
      InAcronym,
      InAbbrev,
      InInsertedText,
      InDeletedText,
      InList,
      InListHeading,
      InDefinitionTerm,
      InTable,
      InForm,
      InSelect,
      InTextArea,
      NumElementsInSet
    };

    /** Construct a new HTML object. If a title is specified in the
       constructor then the HEAD, TITLE and BODY elements are output and the
       string is used in a H1 element.
     */
    PHTML(
      ElementInSet initialState = NumElementsInSet
    );
    PHTML(
      const char * cstr     ///< C string representation of the title string.
    );
    PHTML(
      const PString & str   ///< String representation of the title string.
    );

    ~PHTML();

    /** Restart the HTML string output using the specified value as the
       new title. If <CODE>title</CODE> is empty then no HEAD or TITLE
       elements are placed into the HTML.
     */
    PHTML & operator=(
      const PHTML & html     ///< HTML stream to make a copy of.
    ) { AssignContents(html); return *this; }
    PHTML & operator=(
      const PString & str    ///< String for title in restating HTML.
    ) { AssignContents(str); return *this; }
    PHTML & operator=(
      const char * cstr    ///< String for title in restating HTML.
    ) { AssignContents(PString(cstr)); return *this; }
    PHTML & operator=(
      char ch    ///< String for title in restating HTML.
    ) { AssignContents(PString(ch)); return *this; }


  // New functions for class.
    BOOL Is(ElementInSet elmt) const;
    void Set(ElementInSet elmt);
    void Clr(ElementInSet elmt);
    void Toggle(ElementInSet elmt);


    class Element {
      public: 
        virtual ~Element() {}
      protected:
        enum OptionalCRLF { NoCRLF, OpenCRLF, CloseCRLF, BothCRLF };
        Element(
          const char * nam,
          const char * att,
          ElementInSet elmt,
          ElementInSet req,
          OptionalCRLF opt
        ) { name = nam; attr= att; inElement = elmt; reqElement = req; crlf = opt; }
        virtual void Output(PHTML & html) const;
        virtual void AddAttr(PHTML & html) const;
      private:
        const char * name;
        const char * attr;
        ElementInSet inElement;
        ElementInSet reqElement;
        OptionalCRLF crlf;
      friend ostream & operator<<(ostream & strm, const Element & elmt)
        { elmt.Output((PHTML&)strm); return strm; }
    };

    class HTML : public Element {
      public:
        HTML(const char * attr = NULL);
        virtual ~HTML() {}
    };

    class Head : public Element {
      public:
        Head();
        virtual ~Head() {}
      protected:
        virtual void Output(PHTML & html) const;
    };

    class Body : public Element {
      public:
        Body(const char * attr = NULL);
        virtual ~Body() {}
      protected:
        virtual void Output(PHTML & html) const;
    };

    class Title : public Element {
      public:
        Title();
        Title(const char * titleCStr);
        Title(const PString & titleStr);
        virtual ~Title() {}
      protected:
        virtual void Output(PHTML & html) const;
      private:
        const char * titleString;
    };

    class Banner : public Element {
      public:
        Banner(const char * attr = NULL);
        virtual ~Banner() {}
    };

    class Division : public Element {
      public:
        Division(const char * attr = NULL);
        virtual ~Division() {}
    };

    class Heading : public Element {
      public:
        Heading(int number,
                int sequence = 0,
                int skip = 0,
                const char * attr = NULL);
        Heading(int number,
                const char * image,
                int sequence = 0,
                int skip = 0,
                const char * attr = NULL);
        Heading(int number,
                const PString & imageStr,
                int sequence = 0,
                int skip = 0,
                const char * attr = NULL);
        virtual ~Heading() {}
      protected:
        virtual void AddAttr(PHTML & html) const;
      private:
        int num;
        const char * srcString;
        int seqNum, skipSeq;
    };

    class BreakLine : public Element {
      public:
        BreakLine(const char * attr = NULL);
        virtual ~BreakLine() {}
    };

    class Paragraph : public Element {
      public:
        Paragraph(const char * attr = NULL);
        virtual ~Paragraph() {}
    };

    class PreFormat : public Element {
      public:
        PreFormat(int widthInChars = 0,
                  const char * attr = NULL);
        virtual ~PreFormat() {}
      protected:
        virtual void AddAttr(PHTML & html) const;
      private:
        int width;
    };

    class HotLink : public Element {
      public:
        HotLink(const char * href = NULL, const char * attr = NULL);
        virtual ~HotLink() {}
      protected:
        virtual void AddAttr(PHTML & html) const;
      private:
        const char * hrefString;
    };

    class Target : public Element {
      public:
        Target(const char * name = NULL, const char * attr = NULL);
        virtual ~Target() {}
      protected:
        virtual void AddAttr(PHTML & html) const;
      private:
        const char * nameString;
    };

    class ImageElement : public Element {
      protected:
        ImageElement(const char * nam,
                     const char * attr,
                     ElementInSet elmt,
                     ElementInSet req,
                     OptionalCRLF opt,
                     const char * image);
        virtual ~ImageElement() {}
        virtual void AddAttr(PHTML & html) const;
        const char * srcString;
    };

    class Image : public ImageElement {
      public:
        Image(const char * src,
              int width = 0,
              int height = 0,
              const char * attr = NULL);
        Image(const char * src,
              const char * alt,
              int width = 0,
              int height = 0,
              const char * attr = NULL);
        virtual ~Image() {}
      protected:
        virtual void AddAttr(PHTML & html) const;
      private:
        const char * altString;
        int width, height;
    };

    class HRule : public ImageElement {
      public:
        HRule(const char * image = NULL, const char * attr = NULL);
        virtual ~HRule() {}
    };

    class Note : public ImageElement {
      public:
        Note(const char * image = NULL, const char * attr = NULL);
        virtual ~Note() {}
    };

    class Address : public Element {
      public:
        Address(const char * attr = NULL);
        virtual ~Address() {}
    };

    class BlockQuote : public Element {
      public:
        BlockQuote(const char * attr = NULL);
        virtual ~BlockQuote() {}
    };

    class Credit : public Element {
      public:
        Credit(const char * attr = NULL);
        virtual ~Credit() {}
    };

    class SetTab : public Element {
      public:
        SetTab(const char * id, const char * attr = NULL);
        virtual ~SetTab() {}
      protected:
        virtual void AddAttr(PHTML & html) const;
      private:
        const char * ident;
    };

    class Tab : public Element {
      public:
        Tab(int indent, const char * attr = NULL);
        Tab(const char * id, const char * attr = NULL);
        virtual ~Tab() {}
      protected:
        virtual void AddAttr(PHTML & html) const;
      private:
        const char * ident;
        int indentSize;
    };


    class Bold : public Element {
      public: Bold() : Element("B", NULL, InBold, InBody, NoCRLF) { }
      virtual ~Bold() {}
    };
    class Italic : public Element {
      public: 
        Italic() 
          : Element("I", NULL, InItalic, InBody, NoCRLF) { }
        virtual ~Italic() {}
    };
    class TeleType : public Element {
      public: 
        TeleType() 
          : Element("TT", NULL, InTeleType, InBody, NoCRLF) { }
        virtual ~TeleType() {}
    };
    class Underline : public Element {
      public: 
        Underline() 
          : Element("U", NULL, InUnderline, InBody, NoCRLF) { }
        virtual ~Underline() {}
    };
    class StrikeThrough : public Element {
      public: 
        StrikeThrough()
          : Element("S", NULL, InStrikeThrough, InBody, NoCRLF) { }
        virtual ~StrikeThrough() {}
    };
    class Big : public Element {
      public: 
        Big() 
          : Element("BIG", NULL, InBig, InBody, NoCRLF) { }
        virtual ~Big() {}
    };
    class Small : public Element {
      public: 
        Small() 
          : Element("SMALL", NULL, InSmall, InBody, NoCRLF) { }
        virtual ~Small() {}
    };
    class Subscript : public Element {
      public: 
        Subscript()
          : Element("SUB", NULL, InSubscript, InBody, NoCRLF) { }
        virtual ~Subscript() {}
    };
    class Superscript : public Element {
      public: 
        Superscript()
          : Element("SUP", NULL, InSuperscript, InBody, NoCRLF) { }
        virtual ~Superscript() {}
    };
    class Emphasis : public Element {
      public: 
        Emphasis() 
          : Element("EM", NULL, InEmphasis, InBody, NoCRLF) { }
        virtual ~Emphasis() {}
    };
    class Cite : public Element {
      public: 
        Cite() 
          : Element("CITE", NULL, InCite, InBody, NoCRLF) { }
      virtual ~Cite() {}
    };
    class Strong : public Element {
      public: 
        Strong() 
          : Element("STRONG", NULL, InStrong, InBody, NoCRLF) { }
        virtual ~Strong() {}
    };
    class Code : public Element {
      public: 
        Code() 
          : Element("CODE", NULL, InCode, InBody, NoCRLF) { }
      virtual ~Code() {}
    };
    class Sample : public Element {
      public: 
        Sample() 
          : Element("SAMP", NULL, InSample, InBody, NoCRLF) { }
      virtual ~Sample() {}
    };
    class Keyboard : public Element {
      public: 
        Keyboard() 
          : Element("KBD", NULL, InKeyboard, InBody, NoCRLF) { }
        virtual ~Keyboard() {}
    };
    class Variable : public Element {
      public: 
        Variable() 
          : Element("VAR", NULL, InVariable, InBody, NoCRLF) { }
        virtual ~Variable() {}
    };
    class Definition : public Element {
      public: 
        Definition()
          : Element("DFN", NULL, InDefinition, InBody, NoCRLF) { }
        virtual ~Definition() {}
    };
    class Quote : public Element {
      public: 
        Quote() 
          : Element("Q", NULL, InQuote, InBody, NoCRLF) { }
        virtual ~Quote() {}
    };
    class Author : public Element {
      public: 
        Author() 
          : Element("AU", NULL, InAuthor, InBody, NoCRLF) { }
        virtual ~Author() {}
    };
    class Person : public Element {
      public: 
        Person() 
          : Element("PERSON", NULL, InPerson, InBody, NoCRLF) { }
        virtual ~Person() {}
    };
    class Acronym : public Element {
      public: 
        Acronym()
          : Element("ACRONYM", NULL, InAcronym, InBody, NoCRLF) { }
        virtual ~Acronym() {}
    };
    class Abbrev : public Element {
      public: 
        Abbrev() 
          : Element("ABBREV", NULL, InAbbrev, InBody, NoCRLF) { }
        virtual ~Abbrev() {}
    };
    class InsertedText : public Element {
      public: 
        InsertedText()
          : Element("INS", NULL, InInsertedText, InBody, NoCRLF) { }
        virtual ~InsertedText() {}
    };
    class DeletedText : public Element {
      public: 
        DeletedText()
          : Element("DEL", NULL, InDeletedText, InBody, NoCRLF) { }
      public: 
        virtual ~DeletedText() {}
    };

    class SimpleList : public Element {
      public:
        SimpleList(const char * attr = NULL);
        virtual ~SimpleList() {}
      protected:
        virtual void AddAttr(PHTML & html) const;
    };

    class BulletList : public Element {
      public:
        BulletList(const char * attr = NULL);
        virtual ~BulletList() {}
    };

    class OrderedList : public Element {
      public:
        OrderedList(int seqNum = 0, const char * attr = NULL);
        virtual ~OrderedList() {}
      protected:
        virtual void AddAttr(PHTML & html) const;
      private:
        int sequenceNum;
    };

    class DefinitionList : public Element {
      public:
        DefinitionList(const char * attr = NULL);
        virtual ~DefinitionList() {}
    };

    class ListHeading : public Element {
      public:
        ListHeading(const char * attr = NULL);
        virtual ~ListHeading() {}
    };

    class ListItem : public Element {
      public:
        ListItem(int skip = 0, const char * attr = NULL);
        virtual ~ListItem() {}
      protected:
        virtual void AddAttr(PHTML & html) const;
      private:
        int skipSeq;
    };

    class DefinitionTerm : public Element {
      public:
        DefinitionTerm(const char * attr = NULL);
        virtual ~DefinitionTerm() {}
      protected:
        virtual void Output(PHTML & html) const;
    };

    class DefinitionItem : public Element {
      public:
        DefinitionItem(const char * attr = NULL);
        virtual ~DefinitionItem() {}
      protected:
        virtual void Output(PHTML & html) const;
    };


    enum BorderCodes {
      NoBorder,
      Border
    };
    class TableStart : public Element {
      public:
        TableStart(const char * attr = NULL);
        TableStart(BorderCodes border, const char * attr = NULL);
        virtual ~TableStart() {}
      protected:
        virtual void Output(PHTML & html) const;
        virtual void AddAttr(PHTML & html) const;
      private:
        BOOL borderFlag;
    };
    friend class TableStart;

    class TableEnd : public Element {
      public:
        TableEnd();
        virtual ~TableEnd() {}
      protected:
        virtual void Output(PHTML & html) const;
    };
    friend class TableEnd;

    class TableRow : public Element {
      public:
        TableRow(const char * attr = NULL);
        virtual ~TableRow() {}
    };

    class TableHeader : public Element {
      public:
        TableHeader(const char * attr = NULL);
        virtual ~TableHeader() {}
    };

    class TableData : public Element {
      public:
        TableData(const char * attr = NULL);
        virtual ~TableData() {}
    };


    class Form : public Element {
      public:
        Form(
          const char * method = NULL,
          const char * action = NULL,
          const char * encoding = NULL,
          const char * script = NULL
        );
        virtual ~Form() {}
      protected:
        virtual void AddAttr(PHTML & html) const;
      private:
        const char * methodString;
        const char * actionString;
        const char * mimeTypeString;
        const char * scriptString;
    };

    enum DisableCodes {
      Enabled,
      Disabled
    };
    class FieldElement : public Element {
      protected:
        FieldElement(
          const char * nam,
          const char * attr,
          ElementInSet elmt,
          OptionalCRLF opt,
          DisableCodes disabled
        );
        virtual ~FieldElement() {}
        virtual void AddAttr(PHTML & html) const;
      private:
        BOOL disabledFlag;
    };

    class Select : public FieldElement {
      public:
        Select(
          const char * fname = NULL,
          const char * attr = NULL
        );
        Select(
          const char * fname,
          DisableCodes disabled,
          const char * attr = NULL
        );
        virtual ~Select() {}
      protected:
        virtual void AddAttr(PHTML & html) const;
      private:
        const char * nameString;
    };

    enum SelectionCodes {
      NotSelected,
      Selected
    };
    class Option : public FieldElement {
      public:
        Option(
          const char * attr = NULL
        );
        Option(
          SelectionCodes select,
          const char * attr = NULL
        );
        Option(
          DisableCodes disabled,
          const char * attr = NULL
        );
        Option(
          SelectionCodes select,
          DisableCodes disabled,
          const char * attr = NULL
        );
        virtual ~Option() {}
      protected:
        virtual void AddAttr(PHTML & html) const;
      private:
        BOOL selectedFlag;
    };

    class FormField : public FieldElement {
      protected:
        FormField(
          const char * nam,
          const char * attr,
          ElementInSet elmt,
          OptionalCRLF opt,
          DisableCodes disabled,
          const char * fname
        );
        virtual ~FormField() {}
        virtual void AddAttr(PHTML & html) const;
      private:
        const char * nameString;
    };

    class TextArea : public FormField {
      public:
        TextArea(
          const char * fname,
          DisableCodes disabled = Enabled,
          const char * attr = NULL
        );
        TextArea(
          const char * fname,
          int rows, int cols,
          DisableCodes disabled = Enabled,
          const char * attr = NULL
        );
        virtual ~TextArea() {}
      protected:
        virtual void AddAttr(PHTML & html) const;
      private:
        int numRows, numCols;
    };

    class InputField : public FormField {
      protected:
        InputField(
          const char * type,
          const char * fname,
          DisableCodes disabled,
          const char * attr
        );
        virtual ~InputField() {}
        virtual void AddAttr(PHTML & html) const;
      private:
        const char * typeString;
    };

    class HiddenField : public InputField {
      public:
        HiddenField(
          const char * fname,
          const char * value,
          const char * attr = NULL
        );
        virtual ~HiddenField() {}
      protected:
        virtual void AddAttr(PHTML & html) const;
      private:
        const char * valueString;
    };

    class InputText : public InputField {
      public:
        InputText(
          const char * fname,
          int size,
          const char * init = NULL,
          const char * attr = NULL
        );
        InputText(
          const char * fname,
          int size,
          DisableCodes disabled,
          const char * attr = NULL
        );
        InputText(
          const char * fname,
          int size,
          int maxLength,
          DisableCodes disabled = Enabled,
          const char * attr = NULL
        );
        InputText(
          const char * fname,
          int size,
          const char * init,
          int maxLength,
          DisableCodes disabled = Enabled,
          const char * attr = NULL
        );
        virtual ~InputText() {}
      protected:
        InputText(
          const char * type,
          const char * fname,
          int size,
          const char * init,
          int maxLength,
          DisableCodes disabled,
          const char * attr
        );
        virtual void AddAttr(PHTML & html) const;
      private:
        const char * value;
        int width, length;
    };

    class InputPassword : public InputText {
      public:
        InputPassword(
          const char * fname,
          int size,
          const char * init = NULL,
          const char * attr = NULL
        );
        InputPassword(
          const char * fname,
          int size,
          DisableCodes disabled,
          const char * attr = NULL
        );
        InputPassword(
          const char * fname,
          int size,
          int maxLength,
          DisableCodes disabled = Enabled,
          const char * attr = NULL
        );
        InputPassword(
          const char * fname,
          int size,
          const char * init,
          int maxLength,
          DisableCodes disabled = Enabled,
          const char * attr = NULL
        );
        virtual ~InputPassword() {}
    };

    enum CheckedCodes {
      UnChecked,
      Checked
    };
    class RadioButton : public InputField {
      public:
        RadioButton(
          const char * fname,
          const char * value,
          const char * attr = NULL
        );
        RadioButton(
          const char * fname,
          const char * value,
          DisableCodes disabled,
          const char * attr = NULL
        );
        RadioButton(
          const char * fname,
          const char * value,
          CheckedCodes check,
          DisableCodes disabled = Enabled,
          const char * attr = NULL
        );
        virtual ~RadioButton() {}
      protected:
        RadioButton(
          const char * type,
          const char * fname,
          const char * value,
          CheckedCodes check,
          DisableCodes disabled,
          const char * attr
        );
        virtual void AddAttr(PHTML & html) const;
      private:
        const char * valueString;
        BOOL checkedFlag;
    };

    class CheckBox : public RadioButton {
      public:
        CheckBox(
          const char * fname,
          const char * attr = NULL
        );
        CheckBox(
          const char * fname,
          DisableCodes disabled,
          const char * attr = NULL
        );
        CheckBox(
          const char * fname,
          CheckedCodes check,
          DisableCodes disabled = Enabled,
          const char * attr = NULL
        );
        virtual ~CheckBox() {}
    };


    class InputRange : public InputField {
      public:
        InputRange(
          const char * fname,
          int min, int max,
          int value = 0,
          DisableCodes disabled = Enabled,
          const char * attr = NULL
        );
        virtual ~InputRange() {}
      protected:
        virtual void AddAttr(PHTML & html) const;
      private:
        int minValue, maxValue, initValue;
    };

    class InputFile : public InputField {
      public:
        InputFile(
          const char * fname,
          const char * accept = NULL,
          DisableCodes disabled = Enabled,
          const char * attr = NULL
        );
        virtual ~InputFile() {}
      protected:
        virtual void AddAttr(PHTML & html) const;
      private:
        const char * acceptString;
    };

    class InputImage : public InputField {
      public:
        InputImage(
          const char * fname,
          const char * src = NULL,
          DisableCodes disabled = Enabled,
          const char * attr = NULL
        );
        virtual ~InputImage() {}
      protected:
        InputImage(
          const char * type,
          const char * fname,
          const char * src,
          DisableCodes disabled,
          const char * attr
        );
        virtual void AddAttr(PHTML & html) const;
      private:
        const char * srcString;
    };

    class InputScribble : public InputImage {
      public:
        InputScribble(
          const char * fname,
          const char * src = NULL,
          DisableCodes disabled = Enabled,
          const char * attr = NULL
        );
        virtual ~InputScribble() {}
    };

    class ResetButton : public InputImage {
      public:
        ResetButton(
          const char * title,
          const char * fname = NULL,
          const char * src = NULL,
          DisableCodes disabled = Enabled,
          const char * attr = NULL
        );
        virtual ~ResetButton() {}
      protected:
        ResetButton(
          const char * type,
          const char * title,
          const char * fname = NULL,
          const char * src = NULL,
          DisableCodes disabled = Enabled,
          const char * attr = NULL
        );
        virtual void AddAttr(PHTML & html) const;
      private:
        const char * titleString;
    };

    class SubmitButton : public ResetButton {
      public:
        SubmitButton(
          const char * title,
          const char * fname = NULL,
          const char * src = NULL,
          DisableCodes disabled = Enabled,
          const char * attr = NULL
        );
        virtual ~SubmitButton() {}
    };


  protected:
    virtual void AssignContents(const PContainer & c);

  private:
    ElementInSet initialElement;
    BYTE elementSet[NumElementsInSet/8+1];
    PINDEX tableNestLevel;
};


#endif


// End Of File ///////////////////////////////////////////////////////////////
