/*
 * mime.h
 *
 * Multipurpose Internet Mail Extensions support classes.
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
 * $Log: mime.h,v $
 * Revision 1.23  2007/09/18 06:20:56  csoutheren
 * Fix spelling mistakes
 *
 * Revision 1.22  2007/02/01 23:56:44  csoutheren
 * Added extra AddMIME with seperate key and value fields
 *
 * Revision 1.21  2005/11/30 12:47:37  csoutheren
 * Removed tabs, reformatted some code, and changed tags for Doxygen
 *
 * Revision 1.20  2004/03/23 06:38:51  csoutheren
 * Update for change in location of Base64 routines
 *
 * Revision 1.19  2004/03/23 05:59:17  csoutheren
 * Moved the Base64 routines into cypher.cxx, which is a more sensible
 * place and reduces the inclusion of unrelated code
 *
 * Revision 1.18  2002/11/06 22:47:24  robertj
 * Fixed header comment (copyright etc)
 *
 * Revision 1.17  2002/09/16 01:08:59  robertj
 * Added #define so can select if #pragma interface/implementation is used on
 *   platform basis (eg MacOS) rather than compiler, thanks Robert Monaghan.
 *
 * Revision 1.16  2001/10/03 00:24:57  robertj
 * Split out function for adding a single line of MIME info, reduces
 *    duplicated code and is useful in some other areas such as HTTP/1.1
 *
 * Revision 1.15  2001/09/28 00:41:18  robertj
 * Added SetInteger() function to set numeric MIME fields.
 * Removed HasKey() as is confusing due to ancestor Contains().
 * Overrides of SetAt() and Contains() to assure PCaselessString used.
 *
 * Revision 1.14  2000/11/09 00:18:26  robertj
 * Cosmetic change: removed blank lines.
 *
 * Revision 1.13  1999/03/09 08:01:46  robertj
 * Changed comments for doc++ support (more to come).
 *
 * Revision 1.12  1999/02/16 08:07:10  robertj
 * MSVC 6.0 compatibility changes.
 *
 * Revision 1.11  1998/11/30 02:50:52  robertj
 * New directory structure
 *
 * Revision 1.10  1998/09/23 06:19:42  robertj
 * Added open source copyright license.
 *
 * Revision 1.9  1997/02/05 11:53:11  robertj
 * Changed construction of MIME dictionary to be delayed untill it is used.
 *
 * Revision 1.8  1996/09/14 13:09:15  robertj
 * Major upgrade:
 *   rearranged sockets to help support IPX.
 *   added indirect channel class and moved all protocols to descend from it,
 *   separating the protocol from the low level byte transport.
 *
 * Revision 1.7  1996/07/15 10:28:31  robertj
 * Changed memory block base64 conversion functions to be void *.
 *
 * Revision 1.6  1996/03/16 04:38:09  robertj
 * Fixed bug in MIME write function, should be const.
 *
 * Revision 1.5  1996/02/25 03:04:32  robertj
 * Added decoding of Base64 to a block of memory instead of PBYTEArray.
 *
 * Revision 1.4  1996/01/28 14:14:30  robertj
 * Further implementation of secure config.
 *
 * Revision 1.3  1996/01/28 02:46:07  robertj
 * Removal of MemoryPointer classes as usage didn't work for GNU.
 *
 * Revision 1.2  1996/01/26 02:24:27  robertj
 * Further implemetation.
 *
 * Revision 1.1  1996/01/23 13:06:18  robertj
 * Initial revision
 *
 */

#ifndef _PMIME
#define _PMIME

#ifdef P_USE_PRAGMA
#pragma interface
#endif

#include <ptclib/inetprot.h>
#include <ptclib/cypher.h>

//////////////////////////////////////////////////////////////////////////////
// PMIMEInfo

/** This class contains the Multipurpose Internet Mail Extensions parameters
   and variables.
 */

#ifdef DOC_PLUS_PLUS
class PMIMEInfo : public PStringToString {
#endif
PDECLARE_STRING_DICTIONARY(PMIMEInfo, PCaselessString);
  public:
    PMIMEInfo(
      istream &strm   ///< Stream to read the objects contents from.
    );
    PMIMEInfo(
      PInternetProtocol & socket   ///< Application socket to read MIME info.
    );
    // Construct a MIME information dictionary from the specified source.


  // Overrides from class PObject
    /** Output the contents of the MIME dictionary to the stream. This is
       primarily used by the standard ##operator<<## function.
     */
    virtual void PrintOn(
      ostream &strm   ///< Stream to print the object into.
    ) const;

    /** Input the contents of the MIME dictionary from the stream. This is
       primarily used by the standard ##operator>>## function.
     */
    virtual void ReadFrom(
      istream &strm   ///< Stream to read the objects contents from.
    );


  // Overrides from class PStringToString
    /**Add a new value to the MIME info. If the value is already in the
       dictionary then this overrides the previous value.

       @return
       TRUE if the object was successfully added.
     */
    BOOL SetAt(
      const char * key,
      const PString value
    ) { return AbstractSetAt(PCaselessString(key), PNEW PString(value)); }

    /**Add a new value to the MIME info. If the value is already in the
       dictionary then this overrides the previous value.

       @return
       TRUE if the object was successfully added.
     */
    BOOL SetAt(
      const PString & key,
      const PString value
    ) { return AbstractSetAt(PCaselessString(key), PNEW PString(value)); }

    /**Add a new value to the MIME info. If the value is already in the
       dictionary then this overrides the previous value.

       @return
       TRUE if the object was successfully added.
     */
    BOOL SetAt(
      const PCaselessString & key,
      const PString value
    ) { return AbstractSetAt(PCaselessString(key), PNEW PString(value)); }

    /** Determine if the specified key is present in the MIME information
       set.

       @return
       TRUE if the MIME variable is present.
     */
    BOOL Contains(
      const char * key       ///< Key into MIME dictionary to get info.
    ) const { return GetAt(PCaselessString(key)) != NULL; }

    /** Determine if the specified key is present in the MIME information
       set.

       @return
       TRUE if the MIME variable is present.
     */
    BOOL Contains(
      const PString & key       ///< Key into MIME dictionary to get info.
    ) const { return GetAt(PCaselessString(key)) != NULL; }

    /** Determine if the specified key is present in the MIME information
       set.

       @return
       TRUE if the MIME variable is present.
     */
    BOOL Contains(
      const PCaselessString & key       ///< Key into MIME dictionary to get info.
    ) const { return GetAt(key) != NULL; }

  // New functions for class.
    /** Read MIME information from the socket.

       @return
       TRUE if the MIME information was successfully read.
     */
    BOOL Read(
      PInternetProtocol & socket   ///< Application socket to read MIME info.
    );

    /** Write MIME information to the socket.

       @return
       TRUE if the MIME information was successfully read.
     */
    BOOL Write(
      PInternetProtocol & socket   ///< Application socket to write MIME info.
    ) const;

    /**Add a MIME field given a "name: value" format string.
       Note that if the field name was already in the MIME dictionary then
       this will append the new value after a '\n' character to the previous
       value.

       @return
       TRUE is a field was added.
      */
    BOOL AddMIME(
      const PString & line
    );
    BOOL AddMIME(
      const PString & fieldName, 
      const PString & _fieldValue
    );

    /** Get a string for the particular MIME info field with checking for
       existance. The #dflt# parameter is substituted if the field
       does not exist in the MIME information read in.

       @return
       String for the value of the MIME variable.
     */
    PString GetString(
      const PString & key,       ///< Key into MIME dictionary to get info.
      const PString & dflt       ///< Default value of field if not in MIME info.
    ) const;

    /** Get an integer value for the particular MIME info field with checking
       for existance. The #dflt# parameter is substituted if the
       field does not exist in the MIME information read in.

       @return
       Integer value for the MIME variable.
     */
    long GetInteger(
      const PString & key,    ///< Key into MIME dictionary to get info.
      long dflt = 0           ///< Default value of field if not in MIME info.
    ) const;

    /** Set an integer value for the particular MIME info field.
     */
    void SetInteger(
      const PCaselessString & key,  ///< Key into MIME dictionary to get info.
      long value                    ///< New value of field.
    );


    /** Set an association between a file type and a MIME content type. The
       content type is then sent for any file in the directory sub-tree that
       has the same extension.

       Note that if the #merge# parameter if TRUE then the
       dictionary is merged into the current association list and is not a
       simple replacement.

       The default values placed in this dictionary are:
\begin{verbatim}

          ".txt", "text/plain"
          ".text", "text/plain"
          ".html", "text/html"
          ".htm", "text/html"
          ".aif", "audio/aiff"
          ".aiff", "audio/aiff"
          ".au", "audio/basic"
          ".snd", "audio/basic"
          ".wav", "audio/wav"
          ".gif", "image/gif"
          ".xbm", "image/x-bitmap"
          ".tif", "image/tiff"
          ".tiff", "image/tiff"
          ".jpg", "image/jpeg"
          ".jpe", "image/jpeg"
          ".jpeg", "image/jpeg"
          ".avi", "video/avi"
          ".mpg", "video/mpeg"
          ".mpeg", "video/mpeg"
          ".qt", "video/quicktime"
          ".mov", "video/quicktime"
\end{verbatim}


       The default content type will be "application/octet-stream".
     */
    static void SetAssociation(
      const PStringToString & allTypes,  ///< MIME content type associations.
      BOOL merge = TRUE                  ///< Flag for merging associations.
    );
    static void SetAssociation(
      const PString & fileType,         ///< File type (extension) to match.
      const PString & contentType       ///< MIME content type string.
    ) { GetContentTypes().SetAt(fileType, contentType); }

    /** Look up the file type to MIME content type association dictionary and
       return the MIME content type string. If the file type is not found in
       the dictionary then the string "application/octet-stream" is returned.

       @return
       MIME content type for file type.
     */
    static PString GetContentType(
      const PString & fileType   ///< File type (extension) to look up.
    );

  private:
    static PStringToString & GetContentTypes();
};



#endif


// End Of File ///////////////////////////////////////////////////////////////
