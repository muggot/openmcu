/*
 * pasn.h
 *
 * Abstract Syntax Notation 1 classes for support of SNMP only.
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
 * $Log: pasn.h,v $
 * Revision 1.15  2005/12/22 02:29:22  csoutheren
 * Fixed PASNIPAddress constructor that accepts PIPSocket::Address
 *
 * Revision 1.14  2005/11/30 12:47:37  csoutheren
 * Removed tabs, reformatted some code, and changed tags for Doxygen
 *
 * Revision 1.13  2002/11/06 22:47:24  robertj
 * Fixed header comment (copyright etc)
 *
 * Revision 1.12  2002/09/16 01:08:59  robertj
 * Added #define so can select if #pragma interface/implementation is used on
 *   platform basis (eg MacOS) rather than compiler, thanks Robert Monaghan.
 *
 * Revision 1.11  1999/05/01 03:52:20  robertj
 * Fixed various egcs warnings.
 *
 * Revision 1.10  1999/03/09 08:01:46  robertj
 * Changed comments for doc++ support (more to come).
 *
 * Revision 1.9  1999/02/16 08:07:10  robertj
 * MSVC 6.0 compatibility changes.
 *
 * Revision 1.8  1998/11/30 02:50:54  robertj
 * New directory structure
 *
 * Revision 1.7  1998/09/23 06:19:44  robertj
 * Added open source copyright license.
 *
 * Revision 1.6  1997/08/20 08:48:56  craigs
 * Added GetString() to PASNNull
 *
 * Revision 1.5  1997/07/20 08:34:14  craigs
 * Added ASN NULL type
 *
 * Revision 1.4  1997/07/16 05:51:08  craigs
 * Added PASNString constructor with ptr and length
 *
 * Revision 1.3  1996/11/04 09:45:08  robertj
 * Fixed bug in IP number ASN type, should be binary not dot format string.
 *
 * Revision 1.2  1996/11/04 03:56:00  robertj
 * Added ASN types to class.
 *
 * Revision 1.1  1996/09/14 12:58:57  robertj
 * Initial revision
 *
 */

#ifndef _PASN_H
#define _PASN_H

#ifdef P_USE_PRAGMA
#pragma interface
#endif

#include <ptlib/sockets.h>

//
// define some types used by the ASN classes
//
typedef PInt32  PASNInt;
typedef DWORD   PASNUnsigned;
typedef DWORD           PASNOid;

class PASNObject;
class PASNSequence;

PLIST(PASNObjectList, PASNObject);


//////////////////////////////////////////////////////////////////////////

/** This class defines the common behviour of all ASN objects. It also contains
   several functions which are used for encoding common ASN primitives.

   This class will never be instantiated directly. See the <A>PASNInteger</A>,
   <A>PASNSequence</A>, <A>PASNString</A> and <A>PASNObjectID</A> classes for examples
   of ASN objects that can be created.

   Only descendants of this class can be put into the <A>ASNSequence</A> class.
*/
class PASNObject : public PObject
{
  PCLASSINFO(PASNObject, PObject)

  public:
    /** Value returned by the <A>GetType()</A> function to indicate the type of
       an ASN object
     */
    enum ASNType {
      Integer,      ///< ASN Integer object
      String,       ///< ASN Octet String object
      ObjectID,     ///< ASN Object ID object
      Sequence,     ///< ASN Sequence object
      Choice,       ///< ASN Sequence with discriminator
      IPAddress,    ///< ASN IPAddress object
      Counter,      ///< ASN Counter object
      Gauge,        ///< ASN Gauge object
      TimeTicks,    ///< ASN TimeTicks object
      Opaque,       ///< ASN Opaque object
      NsapAddress,  ///< ASN NsapAddress
      Counter64,    ///< ASN Counter64
      UInteger32,   ///< ASN Unsigned integer 32
      Null,         ///< ASN Null
      Unknown,      ///< unknown ASN object type
      ASNTypeMax    ///< maximum of number of ASN object types
    };

    /** Return a value of type <A>enum ASNType</A> which indicates the type
       of the object
     */
    virtual ASNType GetType() const;


    /** Return the descriminator for Choice sequences
    */
    int GetChoice() const;

    /** Return a string giving the type of the object */
    virtual PString GetTypeAsString() const;

    /** Return the value of the ASN object as a PASNInt.

       This function will assert if the object is not a descendant of
       <A>PASNInteger</A>.
     */
    virtual PASNInt GetInteger () const;

    /** Return the value of the object as a PASNUnsigned

       This function will assert if the object is not a descendant of
       <A>PASNTimeTicks</A> or <P>PASNCounter</A>.
     */
    virtual PASNUnsigned GetUnsigned () const;

    /** Return the value of the object as a PString. This function can
       be use for all ASN object types
     */
    virtual PString GetString () const;

    /** Return the value of the object as a PString

       This function will assert if the object is not a descendant of
       <A>PASNSequence</A>.
     */
    virtual const PASNSequence & GetSequence() const;

    /** Return the value of the object as an IPAddress

       This function will assert if the object is not a descendant of
       <A>PASNIPAddress</A>.
     */
    virtual PIPSocket::Address GetIPAddress () const;

    /** Virtual functions used by the <A>PObject::operator<<</A> function to
       print the value of the object.
    */
    virtual void PrintOn(
      ostream & strm    ///< stream to print on
    ) const;

    /** Virtual function used to encode the object into ASN format */
    virtual void Encode(
      PBYTEArray & buffer  ///< buffer to encode into
    );

    /** Virtual function used to get the length of object when encoded into
       ASN format 
    */
    virtual WORD GetEncodedLength();

    /** Virtual function used to duplicate objects */
    virtual PObject * Clone() const;

    /** Encode an ASN length value */
    static void EncodeASNLength (
      PBYTEArray & buffer,    ///< buffer to encode into
      WORD length             ///< ASN length to encode
    );

    /** Return the length of an encoded ASN length value */
    static WORD GetASNLengthLength (
      WORD length             ///< length to find length of
    );

    /** Decode an ASN length in the buffer at the given ptr. The ptr is moved
       to the byte after the end of the encoded length.
     */
    static BOOL DecodeASNLength (
      const PBYTEArray & buffer,  ///< buffer to decode data from
      PINDEX & ptr,               ///< ptr to decode from
      WORD & len                  ///< returned length
    );

    /** Encode a sequence header into the buffer at the specified offset. */
    static void EncodeASNSequenceStart (
      PBYTEArray & buffer,       ///< buffer to encode data into
      BYTE type,                 ///< sequence type
      WORD length                ///< length of sequence data
    );

    /** Return the encoded length of a sequence if it has the specified length */
    static WORD GetASNSequenceStartLength (
      WORD length               ///< length of sequence data
    );

    /** Encode an ASN object header into the buffer */
    static void EncodeASNHeader(
      PBYTEArray & buffer,        ///< buffer to encode into
      PASNObject::ASNType type,   ///< ASN type of the object
      WORD length                 ///< length of the object
    );

    /** Return the length of an ASN object header if the object is the specified length */
    static WORD GetASNHeaderLength (
      WORD length              ///< length of object
    );

    static void EncodeASNInteger    (
      PBYTEArray & buffer,     ///< buffer to encode into
      PASNInt data,            ///< value to encode
      PASNObject::ASNType type  ///< actual integer type
    );
    // Encode an ASN integer value into the specified buffer */

    static void EncodeASNUnsigned (
      PBYTEArray & buffer,     ///< buffer to encode into
      PASNUnsigned data,       ///< value to encode
      PASNObject::ASNType type ///< actual integer type
    );
    // Encode an ASN integer value into the specified buffer */

    static WORD GetASNIntegerLength (
      PASNInt data      ///< value to get length of
    );
    // Return the length of an encoded ASN integer with the specified value 

    static WORD GetASNUnsignedLength (
      PASNUnsigned data   ///< value to get length of
    );
    // Return the length of an encoded ASN integer with the specified value 

    static BOOL DecodeASNInteger (
      const PBYTEArray & buffer,  ///< buffer to decode from
      PINDEX & ptr,               ///< ptr to data in buffer
      PASNInt & value,            ///< returned value
      ASNType type = Integer         ///< actual integer type
    );
    // Decode an ASN integer value in the specified buffer 

    static BOOL DecodeASNUnsigned (
      const PBYTEArray & buffer,  ///< buffer to decode from
      PINDEX & ptr,               ///< ptr to data in buffer
      PASNUnsigned & value,       ///< returned value
      ASNType type = TimeTicks         ///< actual integer type
    );
    // Decode an ASN integer value in the specified buffer 

  protected:
    /** Create an empty ASN object. Used only by descendant constructors */
    PASNObject();

    /** Table to map <A>enum ASNType</A> values to ASN identifiers */
    static BYTE ASNTypeToType[ASNTypeMax];

};


//////////////////////////////////////////////////////////////////////////

/** A descendant of PASNObject which is a simple ASN integer type.
 */
class PASNInteger : public PASNObject
{
  PCLASSINFO(PASNInteger, PASNObject)
  public:
    PASNInteger(PASNInt val);
    PASNInteger(const PBYTEArray & buffer, PINDEX & ptr);

    void PrintOn(ostream & strm) const;
    void Encode(PBYTEArray & buffer);
    WORD GetEncodedLength();
    PObject * Clone() const;

    PASNInt GetInteger() const;
    PString GetString () const;
    ASNType GetType() const;
    PString GetTypeAsString() const;

  private:
    PASNInt value;
};


//////////////////////////////////////////////////////////////////////////

/** A descendant of PASNObject which is a simple ASN OctetStr type
 */
class PASNString : public PASNObject
{
  PCLASSINFO(PASNString, PASNObject)
  public:
    PASNString(const PString & str);
    PASNString(const BYTE * ptr, int len);
    PASNString(const PBYTEArray & buffer,               PASNObject::ASNType = String);
    PASNString(const PBYTEArray & buffer, PINDEX & ptr, PASNObject::ASNType = String);

    void PrintOn(ostream & strm) const;

    void Encode(PBYTEArray & buffer)
      { Encode(buffer, String); }

    WORD GetEncodedLength();
    PObject * Clone() const;

    PString GetString() const;
    ASNType GetType() const;
    PString GetTypeAsString() const;

  protected:
    BOOL Decode(const PBYTEArray & buffer, PINDEX & i, PASNObject::ASNType type);
    void Encode(PBYTEArray & buffer,             PASNObject::ASNType type);

    PString value;
    WORD    valueLen;
};


//////////////////////////////////////////////////////////////////////////

/** A descendant of PASNObject which is an IP address type
 */
class PASNIPAddress : public PASNString
{
  PCLASSINFO(PASNIPAddress, PASNString)
  public:
    PASNIPAddress(const PIPSocket::Address & addr)
      : PASNString((const BYTE *)addr.GetPointer(), addr.GetSize()) { }

    PASNIPAddress(const PString & str);

    PASNIPAddress(const PBYTEArray & buffer)
      : PASNString(buffer, IPAddress) { }

    PASNIPAddress(const PBYTEArray & buffer, PINDEX & ptr)
      : PASNString(buffer, ptr, IPAddress) { }

    PASNObject::ASNType GetType() const
      { return IPAddress; }

    void Encode(PBYTEArray & buffer)
      { PASNString::Encode(buffer, IPAddress); }

    PString GetString() const;

    PString GetTypeAsString() const;

    PObject * Clone() const
      { return PNEW PASNIPAddress(*this); }

    PIPSocket::Address GetIPAddress () const;
};


//////////////////////////////////////////////////////////////////////////

/** A descendant of PASNObject which is an unsigned ASN integer type.
 */
class PASNUnsignedInteger : public PASNObject
{
  PCLASSINFO(PASNUnsignedInteger, PASNObject)
  public:
    PASNUnsignedInteger(PASNUnsigned val)
      { value = val; }

    PASNUnsignedInteger(const PBYTEArray & buffer, PINDEX & ptr);

    void PrintOn(ostream & strm) const;
    WORD GetEncodedLength();
    PString GetString () const;
    PASNUnsigned GetUnsigned() const;

  protected:
    PASNUnsignedInteger()
      { value = 0; }

    BOOL Decode(const PBYTEArray & buffer, PINDEX & i, PASNObject::ASNType theType);
    void Encode(PBYTEArray & buffer, PASNObject::ASNType theType);

  private:
    PASNUnsigned value;
};


//////////////////////////////////////////////////////////////////////////

/** A descendant of PASNObject which is an unsigned ASN time tick type.
 */
class PASNTimeTicks : public PASNUnsignedInteger
{
  PCLASSINFO(PASNTimeTicks, PASNUnsignedInteger)
  public:
    PASNTimeTicks(PASNUnsigned val) 
      : PASNUnsignedInteger(val) { }

    PASNTimeTicks(const PBYTEArray & buffer, PINDEX & ptr)
      { PASNUnsignedInteger::Decode(buffer, ptr, TimeTicks); }

    void Encode(PBYTEArray & buffer)
      { PASNUnsignedInteger::Encode(buffer, TimeTicks); }

    PObject * Clone() const
      { return PNEW PASNTimeTicks(*this); }

    PASNObject::ASNType GetType() const
      { return TimeTicks; }

    PString GetTypeAsString() const;
};


//////////////////////////////////////////////////////////////////////////

/** A descendant of PASNObject which is an unsigned ASN counter type.
 */
class PASNCounter : public PASNUnsignedInteger
{
  PCLASSINFO(PASNCounter, PASNUnsignedInteger)
  public:
    PASNCounter(PASNUnsigned val) 
      : PASNUnsignedInteger(val) { }

    PASNCounter(const PBYTEArray & buffer, PINDEX & ptr)
      {  PASNUnsignedInteger::Decode(buffer, ptr, Counter); }

    void Encode(PBYTEArray & buffer)
      { PASNUnsignedInteger::Encode(buffer, Counter); }

    PObject * Clone() const
      { return PNEW PASNCounter(*this); }

    PASNObject::ASNType GetType() const
      { return Counter; }

    PString GetTypeAsString() const;
};


//////////////////////////////////////////////////////////////////////////

/** A descendant of PASNObject which is an unsigned ASN guage type.
 */
class PASNGauge : public PASNUnsignedInteger
{
  PCLASSINFO(PASNGauge, PASNUnsignedInteger)
  public:
    PASNGauge(PASNUnsigned val) 
      : PASNUnsignedInteger(val) { }

    PASNGauge(const PBYTEArray & buffer, PINDEX & ptr)
      { Decode(buffer, ptr); }

    BOOL Decode(const PBYTEArray & buffer, PINDEX & i)
      { return PASNUnsignedInteger::Decode(buffer, i, Gauge); }

    void Encode(PBYTEArray & buffer)
      { PASNUnsignedInteger::Encode(buffer, Gauge); }

    PObject * Clone() const
      { return PNEW PASNGauge(*this); }

    PASNObject::ASNType GetType() const
      { return Gauge; }

    PString GetTypeAsString() const;
};



//////////////////////////////////////////////////////////////////////////

/** A descendant of PASNObject which is an unsigned ASN ObjID type.
 */
class PASNObjectID : public PASNObject
{
  PCLASSINFO(PASNObjectID, PASNObject)
  public:
    PASNObjectID(const PString & str);
    PASNObjectID(PASNOid * val, BYTE theLen);
    PASNObjectID(const PBYTEArray & buffer);
    PASNObjectID(const PBYTEArray & buffer, PINDEX & ptr);

    void PrintOn(ostream & strm) const;
    void Encode(PBYTEArray & buffer);
    WORD GetEncodedLength();
    PObject * Clone() const;

    ASNType GetType() const;
    PString GetString () const;
    PString GetTypeAsString() const;

  protected:
    BOOL Decode(const PBYTEArray & buffer, PINDEX & i);

  private:
    PDWORDArray value;
};


//////////////////////////////////////////////////////////////////////////

/** A descendant of PASNObject which is the NULL type
 */
class PASNNull : public PASNObject
{
  PCLASSINFO(PASNNull, PASNObject)
  public:
    PASNNull();
    PASNNull(const PBYTEArray & buffer, PINDEX & ptr);

    void PrintOn(ostream & strm) const;

    void Encode(PBYTEArray & buffer);
    WORD GetEncodedLength();

    PObject * Clone() const;

    ASNType GetType() const;
    PString GetString () const;
    PString GetTypeAsString() const;
};


//////////////////////////////////////////////////////////////////////////

/** A descendant of PASNObject which is the complex sequence type
 */
class PASNSequence : public PASNObject
{
  PCLASSINFO(PASNSequence, PASNObject)
  public:
    PASNSequence();
    PASNSequence(BYTE selector);
    PASNSequence(const PBYTEArray & buffer);
    PASNSequence(const PBYTEArray & buffer, PINDEX & i);

    void Append(PASNObject * obj);
    PINDEX GetSize() const;
    PASNObject & operator [] (PINDEX idx) const;
    const PASNSequence & GetSequence() const;

    void AppendInteger (PASNInt value);
    void AppendString  (const PString & str);
    void AppendObjectID(const PString & str);
    void AppendObjectID(PASNOid * val, BYTE len);

    int GetChoice() const;

//    PASNInt GetInteger (PINDEX idx) const;
//    PString GetString  (PINDEX idx) const;

    void PrintOn(ostream & strm) const;
    void Encode(PBYTEArray & buffer);
    BOOL Decode(const PBYTEArray & buffer, PINDEX & i);
    WORD GetEncodedLength();
    ASNType GetType() const;
    PString GetTypeAsString() const;

    BOOL Encode(PBYTEArray & buffer, PINDEX maxLen) ;

  private:
    PASNObjectList sequence;
    BYTE     type;
    ASNType  asnType;
    WORD     encodedLen;
    WORD     seqLen;
};

#endif


// End of File.

