/*
 * guid.h
 *
 * Globally Unique Identifier
 *
 * Open H323 Library
 *
 * Copyright (c) 1998-2001 Equivalence Pty. Ltd.
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
 * The Original Code is Open H323 Library.
 *
 * The Initial Developer of the Original Code is Equivalence Pty. Ltd.
 *
 * Contributor(s): ______________________________________.
 *
 * $Log: guid.h,v $
 * Revision 1.1  2006/09/20 05:03:41  csoutheren
 * Migrated GUID functions to PWLib
 *
 * Revision 2.5  2005/11/30 13:35:26  csoutheren
 * Changed tags for Doxygen
 *
 * Revision 2.4  2004/03/11 06:54:27  csoutheren
 * Added ability to disable SIP or H.323 stacks
 *
 * Revision 2.3  2002/11/10 11:33:17  robertj
 * Updated to OpenH323 v1.10.3
 *
 * Revision 2.2  2002/09/16 02:52:35  robertj
 * Added #define so can select if #pragma interface/implementation is used on
 *   platform basis (eg MacOS) rather than compiler, thanks Robert Monaghan.
 *
 * Revision 2.1  2002/09/04 06:01:47  robertj
 * Updated to OpenH323 v1.9.6
 *
 * Revision 2.0  2001/07/27 15:48:24  robertj
 * Conversion of OpenH323 to Open Phone Abstraction Library (OPAL)
 *
 * Revision 1.4  2002/09/16 01:14:15  robertj
 * Added #define so can select if #pragma interface/implementation is used on
 *   platform basis (eg MacOS) rather than compiler, thanks Robert Monaghan.
 *
 * Revision 1.3  2002/08/05 10:03:47  robertj
 * Cosmetic changes to normalise the usage of pragma interface/implementation.
 *
 * Revision 1.2  2001/03/19 05:51:35  robertj
 * Added ! operator to do !IsNULL(), so cannot use it accidentally.
 *
 * Revision 1.1  2001/03/02 06:59:57  robertj
 * Enhanced the globally unique identifier class.
 *
 */

#ifndef _PGUID_H
#define _PGUID_H

#ifdef P_USE_PRAGMA
#pragma interface
#endif

class PASN_OctetString;


///////////////////////////////////////////////////////////////////////////////

/**Globally unique ID definition.
   This implements a 128 bit globally unique ID as required by many protocols
   and software systems. The value is constructed in such a way as to make a
   duplicate anywhere in the world highly unlikely.
 */
class PGloballyUniqueID : public PBYTEArray
{
  PCLASSINFO(PGloballyUniqueID, PBYTEArray);

  public:
  /**@name Construction */
  //@{
    /**Create a new ID.
       The ID created with this will be initialised to a globally unique ID
       as per specification.
     */
    PGloballyUniqueID();

    /**Create an ID from a C string of hex (as produced by AsString()).
       A useful construct is to construct a OpalGloballyUniqueID() with
       NULL which produces an all zero GUID, etectable with the isNULL()
       function.
     */
    PGloballyUniqueID(
      const char * cstr    ///<  C string to convert
    );
    /**Create an ID from a PString of hex (as produced by AsString()).
     */
    PGloballyUniqueID(
      const PString & str  ///<  String of hex to convert
    );
    /**Create an ID from an octet string in an ASN PDU.
     */
    PGloballyUniqueID(
      const PASN_OctetString & ostr  ///<  Octet string from ASN to convert
    );
  //@}

  /**@name Overrides from PObject */
  //@{
    /**Standard stream print function.
       The PObject class has a << operator defined that calls this function
       polymorphically.
      */
    virtual void PrintOn(
      ostream & strm    ///<  Stream to output text representation
    ) const;

    /**Standard stream read function.
       The PObject class has a >> operator defined that calls this function
       polymorphically.
      */
    virtual void ReadFrom(
      istream & strm    ///<  Stream to output text representation
    );

    /**Create a clone of the ID.
       The duplicate ID has the same value as the source. Required for having
       this object as a key in dictionaries.
      */
    virtual PObject * Clone() const;

    /**Get the hash value for the ID.
       Creates a number based on the ID value for use in the hash table of
       a dictionary. Required for having this object as a key in dictionaries.
      */
    virtual PINDEX HashFunction() const;
  //@}

  /**@name Operations */
  //@{
    /**Convert the ID to human readable string.
      */
    PString AsString() const;

    /**Test if the GUID is null, ie consists of all zeros.
      */
    BOOL IsNULL() const;

    BOOL operator!() const { return !IsNULL(); }
  //@}
};


#endif // _PGUID_H


/////////////////////////////////////////////////////////////////////////////
