/*
 * httpform.h
 *
 * Forms management using HTTP User Interface.
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
 * $Log: httpform.h,v $
 * Revision 1.18  2005/11/30 12:47:37  csoutheren
 * Removed tabs, reformatted some code, and changed tags for Doxygen
 *
 * Revision 1.17  2003/03/24 04:30:41  robertj
 * Added function to set and get strings from PConfig in correct format for
 *   use with HTTP form array contsructs.
 *
 * Revision 1.16  2002/11/06 22:47:24  robertj
 * Fixed header comment (copyright etc)
 *
 * Revision 1.15  2002/09/16 01:08:59  robertj
 * Added #define so can select if #pragma interface/implementation is used on
 *   platform basis (eg MacOS) rather than compiler, thanks Robert Monaghan.
 *
 * Revision 1.14  2001/10/10 08:07:48  robertj
 * Fixed large memory leak of strings when doing POST to a form.
 *
 * Revision 1.13  2000/12/18 07:12:37  robertj
 * Added ability to have fixed length array fields.
 *
 * Revision 1.12  1999/03/09 08:01:46  robertj
 * Changed comments for doc++ support (more to come).
 *
 * Revision 1.11  1999/02/16 08:07:10  robertj
 * MSVC 6.0 compatibility changes.
 *
 * Revision 1.10  1998/11/30 02:50:48  robertj
 * New directory structure
 *
 * Revision 1.9  1998/09/23 06:19:31  robertj
 * Added open source copyright license.
 *
 * Revision 1.8  1998/08/20 05:45:33  robertj
 * Fixed bug where substitutions did not always occur if near end of macro block.
 *
 * Revision 1.7  1998/01/26 00:25:24  robertj
 * Major rewrite of HTTP forms management.
 *
 * Revision 1.6  1997/08/09 07:46:51  robertj
 * Fixed problems with value of SELECT fields in form
 *
 * Revision 1.5  1997/07/26 11:38:17  robertj
 * Support for overridable pages in HTTP service applications.
 *
 * Revision 1.4  1997/07/08 13:16:12  robertj
 * Major HTTP form enhancements for lists and arrays of fields.
 *
 * Revision 1.3  1997/06/08 04:49:40  robertj
 * Adding new llist based form field.
 *
 * Revision 1.2  1997/04/01 06:01:39  robertj
 * Allowed value list in drop down box to be modified once created.
 *
 * Revision 1.1  1996/06/28 12:55:56  robertj
 * Initial revision
 *
 */

#ifndef _PHTTPFORM
#define _PHTTPFORM

#ifdef P_USE_PRAGMA
#pragma interface
#endif

#include <ptclib/http.h>


///////////////////////////////////////////////////////////////////////////////
// PHTTPField

/** This class is the abstract base class for fields in a #PHTTPForm#
   resource type.
 */
class PHTTPField : public PObject
{
  PCLASSINFO(PHTTPField, PObject)
  public:
    PHTTPField(
      const char * bname,  // base name (identifier) for the field.
      const char * title,  // Title text for field (defaults to name).
      const char * help    // Help text for the field.
    );
    // Create a new field in a HTTP form.

    /** Compare the fields using the field names.

       @return
       Comparison of the name fields of the two fields.
     */
    virtual Comparison Compare(
      const PObject & obj
    ) const;

    /** Get the identifier name of the field.

       @return
       String for field name.
     */
    const PCaselessString & GetName() const { return fullName; }

    /** Get the identifier name of the field.

       @return
       String for field name.
     */
    const PCaselessString & GetBaseName() const { return baseName; }

    /** Set the name for the field.
     */
    virtual void SetName(
      const PString & newName   // New name for field
    );

    /** Locate the field naem, recusing down for composite fields.

       @return
       Pointer to located field, or NULL if not found.
     */
    virtual const PHTTPField * LocateName(
      const PString & name    // Full field name to locate
    ) const;

    /** Get the title of the field.

       @return
       String for title placed next to the field.
     */
    const PString & GetTitle() const { return title; }

    /** Get the title of the field.

       @return
       String for title placed next to the field.
     */
    const PString & GetHelp() const { return help; }

    void SetHelp(
      const PString & text        // Help text.
    ) { help = text; }
    void SetHelp(
      const PString & hotLinkURL, // URL for link to help page.
      const PString & linkText    // Help text in the link.
    );
    void SetHelp(
      const PString & hotLinkURL, // URL for link to help page.
      const PString & imageURL,   // URL for image to be displayed in link.
      const PString & imageText   // Text in the link when image unavailable.
    );
    // Set the help text for the field.

    /** Create a new field of the same class as the current field.

       @return
       New field object instance.
     */
    virtual PHTTPField * NewField() const = 0;

    virtual void ExpandFieldNames(PString & text, PINDEX start, PINDEX & finish) const;
    // Splice expanded macro substitutions into text string

    /** Convert the field to HTML form tag for inclusion into the HTTP page.
     */
    virtual void GetHTMLTag(
      PHTML & html    // HTML to receive the fields HTML tag.
    ) const = 0;

    /** Convert the field input to HTML for inclusion into the HTTP page.
     */
    virtual PString GetHTMLInput(
      const PString & input // Source HTML text for input tag.
    ) const;

    /** Convert the field input to HTML for inclusion into the HTTP page.
     */
    virtual PString GetHTMLSelect(
      const PString & selection // Source HTML text for input tag.
    ) const;

    /** Convert the field to HTML for inclusion into the HTTP page.
     */
    virtual void GetHTMLHeading(
      PHTML & html    // HTML to receive the field info.
    ) const;

    /** Get the string value of the field.

       @return
       String for field value.
     */
    virtual PString GetValue(BOOL dflt = FALSE) const = 0;

    /** Set the value of the field.
     */
    virtual void SetValue(
      const PString & newValue   // New value for the field.
    ) = 0;

    /** Get the value of the PConfig to the sub-field. If the field is not
       composite then it always sets the value as for the non-indexed version.
     */
    virtual void LoadFromConfig(
      PConfig & cfg   // Configuration for value transfer.
    );

    /** Set the value of the sub-field into the PConfig. If the field is not
       composite then it always sets the value as for the non-indexed version.
     */
    virtual void SaveToConfig(
      PConfig & cfg   // Configuration for value transfer.
    ) const;

    /** Validate the new field value before #SetValue()# is called.

       @return
       BOOL if the new field value is OK.
     */
    virtual BOOL Validated(
      const PString & newVal, // Proposed new value for the field.
      PStringStream & msg     // Stream to take error HTML if value not valid.
    ) const;


    /** Retrieve all the names in the field and subfields.

       @return
       List of strings for each subfield.
     */
    virtual void GetAllNames(PStringList & list) const;

    /** Set the value of the field in a list of fields.
     */
    virtual void SetAllValues(
      const PStringToString & data   // New value for the field.
    );

    /** Validate the new field value in a list before #SetValue()# is called.

       @return
       BOOL if the all the new field values are OK.
     */
    virtual BOOL ValidateAll(
      const PStringToString & data, // Proposed new value for the field.
      PStringStream & msg     // Stream to take error HTML if value not valid.
    ) const;


    BOOL NotYetInHTML() const { return notInHTML; }
    void SetInHTML() { notInHTML = FALSE; }

  protected:
    PCaselessString baseName;
    PCaselessString fullName;
    PString title;
    PString help;
    BOOL notInHTML;
};


PLIST(PHTTPFieldList, PHTTPField);

class PHTTPCompositeField : public PHTTPField
{
  PCLASSINFO(PHTTPCompositeField, PHTTPField)
  public:
    PHTTPCompositeField(
      const char * name,          // Name (identifier) for the field.
      const char * title = NULL,  // Title text for field (defaults to name).
      const char * help = NULL    // Help text for the field.
    );

    virtual void SetName(
      const PString & name   // New name for field
    );

    virtual const PHTTPField * LocateName(
      const PString & name    // Full field name to locate
    ) const;

    virtual PHTTPField * NewField() const;

    virtual void ExpandFieldNames(PString & text, PINDEX start, PINDEX & finish) const;

    virtual void GetHTMLTag(
      PHTML & html    // HTML to receive the field info.
    ) const;

    virtual PString GetHTMLInput(
      const PString & input // Source HTML text for input tag.
    ) const;

    virtual void GetHTMLHeading(
      PHTML & html    // HTML to receive the field info.
    ) const;

    virtual PString GetValue(BOOL dflt = FALSE) const;

    virtual void SetValue(
      const PString & newValue   // New value for the field.
    );

    virtual void LoadFromConfig(
      PConfig & cfg   // Configuration for value transfer.
    );
    virtual void SaveToConfig(
      PConfig & cfg   // Configuration for value transfer.
    ) const;

    virtual void GetAllNames(PStringList & list) const;
    virtual void SetAllValues(
      const PStringToString & data   // New value for the field.
    );

    virtual BOOL ValidateAll(
      const PStringToString & data, // Proposed new value for the field.
      PStringStream & msg     // Stream to take error HTML if value not valid.
    ) const;


    /** Get the number of sub-fields in the composite field. Note that this is
       the total including any composite sub-fields, ie, it is the size of the
       whole tree of primitive fields.

       @return
       Returns field count.
     */
    virtual PINDEX GetSize() const;

    void Append(PHTTPField * fld);
    PHTTPField & operator[](PINDEX idx) const { return fields[idx]; }
    void RemoveAt(PINDEX idx) { fields.RemoveAt(idx); }
    void RemoveAll() { fields.RemoveAll(); }

  protected:
    PHTTPFieldList fields;
};


class PHTTPSubForm : public PHTTPCompositeField
{
  PCLASSINFO(PHTTPSubForm, PHTTPCompositeField)
  public:
    PHTTPSubForm(
      const PString & subFormName, // URL for the sub-form
      const char * name,           // Name (identifier) for the field.
      const char * title = NULL,   // Title text for field (defaults to name).
      PINDEX primaryField = 0,     // Pimary field whove value is in hot link
      PINDEX secondaryField = P_MAX_INDEX   // Seconary field next to hotlink
    );

  PHTTPField * NewField() const;
  void GetHTMLTag(PHTML & html) const;
  void GetHTMLHeading(PHTML & html) const;

  protected:
    PString subFormName;
    PINDEX primary;
    PINDEX secondary;
};


class PHTTPFieldArray : public PHTTPCompositeField
{
  PCLASSINFO(PHTTPFieldArray, PHTTPCompositeField)
  public:
    PHTTPFieldArray(
      PHTTPField * baseField,
      BOOL ordered,
      PINDEX fixedSize = 0
    );

    ~PHTTPFieldArray();


    virtual PHTTPField * NewField() const;

    virtual void ExpandFieldNames(PString & text, PINDEX start, PINDEX & finish) const;

    virtual void GetHTMLTag(
      PHTML & html    // HTML to receive the field info.
    ) const;

    virtual void LoadFromConfig(
      PConfig & cfg   // Configuration for value transfer.
    );
    virtual void SaveToConfig(
      PConfig & cfg   // Configuration for value transfer.
    ) const;


    virtual void SetAllValues(
      const PStringToString & data   // New value for the field.
    );

    virtual PINDEX GetSize() const;
    void SetSize(PINDEX newSize);

    PStringArray GetStrings(
      PConfig & cfg   ///< Config file to get strings from
    );

    void SetStrings(
      PConfig & cfg,   ///< Config file to Set strings to
      const PStringArray & values ///< Strings to set
    );

  protected:
    void AddBlankField();
    void AddArrayControlBox(PHTML & html, PINDEX fld) const;
    void SetArrayFieldName(PINDEX idx) const;

    PHTTPField * baseField;
    BOOL orderedArray;
    BOOL canAddElements;
};


class PHTTPStringField : public PHTTPField
{
  PCLASSINFO(PHTTPStringField, PHTTPField)
  public:
    PHTTPStringField(
      const char * name,
      PINDEX size,
      const char * initVal = NULL,
      const char * help = NULL
    );
    PHTTPStringField(
      const char * name,
      const char * title,
      PINDEX size,
      const char * initVal = NULL,
      const char * help = NULL
    );

    virtual PHTTPField * NewField() const;

    virtual void GetHTMLTag(
      PHTML & html    ///< HTML to receive the field info.
    ) const;

    virtual PString GetValue(BOOL dflt = FALSE) const;

    virtual void SetValue(
      const PString & newVal
    );


  protected:
    PString value;
    PString initialValue;
    PINDEX size;
};


class PHTTPPasswordField : public PHTTPStringField
{
  PCLASSINFO(PHTTPPasswordField, PHTTPStringField)
  public:
    PHTTPPasswordField(
      const char * name,
      PINDEX size,
      const char * initVal = NULL,
      const char * help = NULL
    );
    PHTTPPasswordField(
      const char * name,
      const char * title,
      PINDEX size,
      const char * initVal = NULL,
      const char * help = NULL
    );

    virtual PHTTPField * NewField() const;

    virtual void GetHTMLTag(
      PHTML & html    ///< HTML to receive the field info.
    ) const;

    virtual PString GetValue(BOOL dflt = FALSE) const;

    virtual void SetValue(
      const PString & newVal
    );

    static PString Decrypt(const PString & pword);
};


class PHTTPIntegerField : public PHTTPField
{
  PCLASSINFO(PHTTPIntegerField, PHTTPField)
  public:
    PHTTPIntegerField(
      const char * name,
      int low, int high,
      int initVal = 0,
      const char * units = NULL,
      const char * help = NULL
    );
    PHTTPIntegerField(
      const char * name,
      const char * title,
      int low, int high,
      int initVal = 0,
      const char * units = NULL,
      const char * help = NULL
    );

    virtual PHTTPField * NewField() const;

    virtual void GetHTMLTag(
      PHTML & html    ///< HTML to receive the field info.
    ) const;

    virtual PString GetValue(BOOL dflt = FALSE) const;

    virtual void SetValue(
      const PString & newVal
    );

    virtual void LoadFromConfig(
      PConfig & cfg   ///< Configuration for value transfer.
    );
    virtual void SaveToConfig(
      PConfig & cfg   ///< Configuration for value transfer.
    ) const;

    virtual BOOL Validated(
      const PString & newVal,
      PStringStream & msg
    ) const;


  protected:
    int low, high, value;
    int initialValue;
    PString units;
};


class PHTTPBooleanField : public PHTTPField
{
  PCLASSINFO(PHTTPBooleanField, PHTTPField)
  public:
    PHTTPBooleanField(
      const char * name,
      BOOL initVal = FALSE,
      const char * help = NULL
    );
    PHTTPBooleanField(
      const char * name,
      const char * title,
      BOOL initVal = FALSE,
      const char * help = NULL
    );

    virtual PHTTPField * NewField() const;

    virtual void GetHTMLTag(
      PHTML & html    ///< HTML to receive the field info.
    ) const;

    virtual PString GetHTMLInput(
      const PString & input
    ) const;

    virtual PString GetValue(BOOL dflt = FALSE) const;

    virtual void SetValue(
      const PString & newVal
    );

    virtual void LoadFromConfig(
      PConfig & cfg   ///< Configuration for value transfer.
    );
    virtual void SaveToConfig(
      PConfig & cfg   ///< Configuration for value transfer.
    ) const;


  protected:
    BOOL value, initialValue;
};


class PHTTPRadioField : public PHTTPField
{
  PCLASSINFO(PHTTPRadioField, PHTTPField)
  public:
    PHTTPRadioField(
      const char * name,
      const PStringArray & valueArray,
      PINDEX initVal = 0,
      const char * help = NULL
    );
    PHTTPRadioField(
      const char * name,
      const PStringArray & valueArray,
      const PStringArray & titleArray,
      PINDEX initVal = 0,
      const char * help = NULL
    );
    PHTTPRadioField(
      const char * name,
      PINDEX count,
      const char * const * valueStrings,
      PINDEX initVal = 0,
      const char * help = NULL
    );
    PHTTPRadioField(
      const char * name,
      PINDEX count,
      const char * const * valueStrings,
      const char * const * titleStrings,
      PINDEX initVal = 0,
      const char * help = NULL
    );
    PHTTPRadioField(
      const char * name,
      const char * groupTitle,
      const PStringArray & valueArray,
      PINDEX initVal = 0,
      const char * help = NULL
    );
    PHTTPRadioField(
      const char * name,
      const char * groupTitle,
      const PStringArray & valueArray,
      const PStringArray & titleArray,
      PINDEX initVal = 0,
      const char * help = NULL
    );
    PHTTPRadioField(
      const char * name,
      const char * groupTitle,
      PINDEX count,
      const char * const * valueStrings,
      PINDEX initVal = 0,
      const char * help = NULL
    );
    PHTTPRadioField(
      const char * name,
      const char * groupTitle,
      PINDEX count,
      const char * const * valueStrings,
      const char * const * titleStrings,
      PINDEX initVal = 0,
      const char * help = NULL
    );

    virtual PHTTPField * NewField() const;

    virtual void GetHTMLTag(
      PHTML & html    ///< HTML to receive the field info.
    ) const;

    virtual PString GetHTMLInput(
      const PString & input
    ) const;

    virtual PString GetValue(BOOL dflt = FALSE) const;

    virtual void SetValue(
      const PString & newVal
    );


  protected:
    PStringArray values;
    PStringArray titles;
    PString value;
    PString initialValue;
};


class PHTTPSelectField : public PHTTPField
{
  PCLASSINFO(PHTTPSelectField, PHTTPField)
  public:
    PHTTPSelectField(
      const char * name,
      const PStringArray & valueArray,
      PINDEX initVal = 0,
      const char * help = NULL
    );
    PHTTPSelectField(
      const char * name,
      PINDEX count,
      const char * const * valueStrings,
      PINDEX initVal = 0,
      const char * help = NULL
    );
    PHTTPSelectField(
      const char * name,
      const char * title,
      const PStringArray & valueArray,
      PINDEX initVal = 0,
      const char * help = NULL
    );
    PHTTPSelectField(
      const char * name,
      const char * title,
      PINDEX count,
      const char * const * valueStrings,
      PINDEX initVal = 0,
      const char * help = NULL
    );

    virtual PHTTPField * NewField() const;

    virtual void GetHTMLTag(
      PHTML & html    ///< HTML to receive the field info.
    ) const;

    virtual PString GetValue(BOOL dflt = FALSE) const;

    virtual void SetValue(
      const PString & newVal
    );


    PStringArray values;


  protected:
    PString value;
    PINDEX initialValue;
};


///////////////////////////////////////////////////////////////////////////////
// PHTTPForm

class PHTTPForm : public PHTTPString
{
  PCLASSINFO(PHTTPForm, PHTTPString)
  public:
    PHTTPForm(
      const PURL & url
    );
    PHTTPForm(
      const PURL & url,
      const PHTTPAuthority & auth
    );
    PHTTPForm(
      const PURL & url,
      const PString & html
    );
    PHTTPForm(
      const PURL & url,
      const PString & html,
      const PHTTPAuthority & auth
    );


    virtual void OnLoadedText(
      PHTTPRequest & request,    ///< Information on this request.
      PString & text             ///< Data used in reply.
    );
    virtual BOOL Post(
      PHTTPRequest & request,       ///< Information on this request.
      const PStringToString & data, ///< Variables in the POST data.
      PHTML & replyMessage          ///< Reply message for post.
    );


    PHTTPField * Add(
      PHTTPField * fld
    );
    void RemoveAllFields()
      { fields.RemoveAll(); fieldNames.RemoveAll(); }

    enum BuildOptions {
      CompleteHTML,
      InsertIntoForm,
      InsertIntoHTML
    };

    void BuildHTML(
      const char * heading
    );
    void BuildHTML(
      const PString & heading
    );
    void BuildHTML(
      PHTML & html,
      BuildOptions option = CompleteHTML
    );


  protected:
    PHTTPCompositeField fields;
    PStringSet fieldNames;
};


//////////////////////////////////////////////////////////////////////////////
// PHTTPConfig

class PHTTPConfig : public PHTTPForm
{
  PCLASSINFO(PHTTPConfig, PHTTPForm)
  public:
    PHTTPConfig(
      const PURL & url,
      const PString & section
    );
    PHTTPConfig(
      const PURL & url,
      const PString & section,
      const PHTTPAuthority & auth
    );
    PHTTPConfig(
      const PURL & url,
      const PString & section,
      const PString & html
    );
    PHTTPConfig(
      const PURL & url,
      const PString & section,
      const PString & html,
      const PHTTPAuthority & auth
    );

    virtual void OnLoadedText(
      PHTTPRequest & request,    ///< Information on this request.
      PString & text             ///< Data used in reply.
    );
    virtual BOOL Post(
      PHTTPRequest & request,       ///< Information on this request.
      const PStringToString & data, ///< Variables in the POST data.
      PHTML & replyMessage          ///< Reply message for post.
    );


    /** Load all of the values for the resource from the configuration.
     */
    void LoadFromConfig();

    /** Get the configuration file section that the page will alter.

       @return
       String for config file section.
     */
    const PString & GetConfigSection() const { return section; }

    void SetConfigSection(
      const PString & sect   ///< New section for the config page.
    ) { section = sect; }
    // Set the configuration file section.

    /** Add a field that will determine the name opf the secontion into which
       the other fields are to be added as keys. The section is not created and
       and error generated if the section already exists.
     */
    PHTTPField * AddSectionField(
      PHTTPField * sectionFld,     ///< Field to set as the section name
      const char * prefix = NULL,  ///< String to attach before the field value
      const char * suffix = NULL   ///< String to attach after the field value
    );

    /** Add fields to the HTTP form for adding a new key to the config file
       section.
     */
    void AddNewKeyFields(
      PHTTPField * keyFld,  ///< Field for the key to be added.
      PHTTPField * valFld   ///< Field for the value of the key yto be added.
    );


  protected:
    PString section;
    PString sectionPrefix;
    PString sectionSuffix;
    PHTTPField * sectionField;
    PHTTPField * keyField;
    PHTTPField * valField;

  private:
    void Construct();
};


//////////////////////////////////////////////////////////////////////////////
// PHTTPConfigSectionList

class PHTTPConfigSectionList : public PHTTPString
{
  PCLASSINFO(PHTTPConfigSectionList, PHTTPString)
  public:
    PHTTPConfigSectionList(
      const PURL & url,
      const PHTTPAuthority & auth,
      const PString & sectionPrefix,
      const PString & additionalValueName,
      const PURL & editSection,
      const PURL & newSection,
      const PString & newSectionTitle,
      PHTML & heading
    );

    virtual void OnLoadedText(
      PHTTPRequest & request,    ///< Information on this request.
      PString & text             ///< Data used in reply.
    );
    virtual BOOL Post(
      PHTTPRequest & request,       ///< Information on this request.
      const PStringToString & data, ///< Variables in the POST data.
      PHTML & replyMessage          ///< Reply message for post.
    );

  protected:
    PString sectionPrefix;
    PString additionalValueName;
    PString newSectionLink;
    PString newSectionTitle;
    PString editSectionLink;
};


#endif


// End Of File ///////////////////////////////////////////////////////////////
