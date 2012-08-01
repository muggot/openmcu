/*
 * pasn.cxx
 *
 * ASN classes in support of the SNMP code.
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
 * $Log: pasn.cxx,v $
 * Revision 1.17  2005/11/30 12:47:41  csoutheren
 * Removed tabs, reformatted some code, and changed tags for Doxygen
 *
 * Revision 1.16  2004/04/03 06:54:25  rjongbloed
 * Many and various changes to support new Visual C++ 2003
 *
 * Revision 1.15  2002/11/06 22:47:25  robertj
 * Fixed header comment (copyright etc)
 *
 * Revision 1.14  2000/05/05 10:08:29  robertj
 * Fixed some GNU compiler warnings
 *
 * Revision 1.13  1999/05/01 11:29:20  robertj
 * Alpha linux port changes.
 *
 * Revision 1.12  1999/05/01 03:52:20  robertj
 * Fixed various egcs warnings.
 *
 * Revision 1.11  1999/03/02 01:53:38  craigs
 * Fixed problem with creating IpAddress objects
 *
 * Revision 1.10  1999/02/16 08:08:06  robertj
 * MSVC 6.0 compatibility changes.
 *
 * Revision 1.9  1998/11/30 04:52:04  robertj
 * New directory structure
 *
 * Revision 1.8  1998/10/13 14:06:31  robertj
 * Complete rewrite of memory leak detection code.
 *
 * Revision 1.7  1998/09/23 06:22:27  robertj
 * Added open source copyright license.
 *
 * Revision 1.6  1998/02/16 06:57:05  robertj
 * Moved pragma implemenetation in here so do not need upasn.cxx file.
 *
 * Revision 1.7  1998/01/26 02:49:21  robertj
 * GNU support.
 *
 * Revision 1.6  1998/01/26 01:45:36  robertj
 * Removed unused variable.
 *
 * Revision 1.5  1997/08/20 09:00:37  craigs
 * Fixed problems with decoding of PASNNull
 *
 * Revision 1.4  1997/07/20 08:34:37  craigs
 * Added ASN NULL type
 *
 * Revision 1.3  1997/07/16 05:52:48  craigs
 * Changed ASN constructors to store value length separately so
 * ASNString consctructor will worki correctly
 *
 * Revision 1.2  1996/11/04 03:58:34  robertj
 * Added ASN types to class.
 *
 * Revision 1.1  1996/09/14 13:02:18  robertj
 * Initial revision
 *
 */

#ifdef __GNUC__
#pragma implementation "pasn.h"
#endif

#include <ptlib.h>
#include <ptclib/pasn.h>

#define new PNEW


#define ASN_BOOLEAN         (0x01)
#define ASN_INTEGER         (0x02)
#define ASN_BIT_STR         (0x03)
#define ASN_OCTET_STR       (0x04)
#define ASN_NULL            (0x05)
#define ASN_OBJECT_ID       (0x06)
#define ASN_SEQUENCE        (0x10)
#define ASN_SET             (0x11)

#define ASN_UNIVERSAL       (0x00)
#define ASN_APPLICATION     (0x40)
#define ASN_CONTEXT         (0x80)
#define ASN_PRIVATE         (0xC0)

#define ASN_PRIMITIVE       (0x00)
#define ASN_CONSTRUCTOR     (0x20)

#define ASN_LONG_LEN        (0x80)
#define ASN_EXTENSION_ID    (0x1F)
#define ASN_BIT8            (0x80)

#define MAX_OID_LEN   64


static char cannotPerformOnBaseTypeMsg[] =
   "Cannot perform operation on base type PASNObject";


BYTE PASNObject::ASNTypeToType[] = {
  ASN_INTEGER   | ASN_UNIVERSAL | ASN_PRIMITIVE,    // Integer
  ASN_OCTET_STR | ASN_UNIVERSAL | ASN_PRIMITIVE,    // String
  ASN_OBJECT_ID | ASN_UNIVERSAL | ASN_PRIMITIVE,    // ObjectID
  ASN_CONSTRUCTOR | ASN_SEQUENCE,                   // Sequence
  ASN_CONSTRUCTOR | ASN_CONTEXT,                    // Choice
  ASN_APPLICATION | 0,                              // IPAddress
  ASN_APPLICATION | 1,                              // Counter32
  ASN_APPLICATION | 2,                              // Gauge32
  ASN_APPLICATION | 3,                              // TimeTicks

// SNMP v2 types
  ASN_APPLICATION | 4,                              // Opaque
  ASN_APPLICATION | 5,                              // NsapAddress
  ASN_APPLICATION | 6,                              // Counter64
  ASN_APPLICATION | 7,                              // UInteger32
  
// Oops - missed the Null type  
  ASN_NULL | ASN_UNIVERSAL | ASN_PRIMITIVE,         // Null

  0,                // Unknown
};



//////////////////////////////////////////////////////////////////////////
//
//  PASNObject
//     All ASN objects descend from this class. It is the primitive type
//  which can be put into the ASNSequence class
//

PASNObject::PASNObject()
{
}


void PASNObject::PrintOn(ostream &) const
{
  PAssertAlways(cannotPerformOnBaseTypeMsg);
}


void PASNObject::Encode(PBYTEArray &) 
{
  PAssertAlways(cannotPerformOnBaseTypeMsg);
}


WORD PASNObject::GetEncodedLength() 
{
  PAssertAlways(cannotPerformOnBaseTypeMsg);
  return 0;
}


PASNObject::ASNType PASNObject::GetType() const
{
  PAssertAlways(cannotPerformOnBaseTypeMsg);
  return Unknown;
}


PString PASNObject::GetTypeAsString() const
{
  PAssertAlways(cannotPerformOnBaseTypeMsg);
  return PString();
}


PObject * PASNObject::Clone() const
{
  PAssertAlways(cannotPerformOnBaseTypeMsg);
  return NULL;
}


void PASNObject::EncodeASNLength (PBYTEArray & buffer, WORD length)
{
  PINDEX offs = buffer.GetSize();

  // handle lengths less then 128
  if (length < 128) 
    buffer[offs++] = (BYTE)length;

  // handle lengths less than 256
  else if (length < 256) {
    buffer[offs++] = (BYTE)(0x01 | ASN_LONG_LEN);
    buffer[offs++] = (BYTE)length;
  }

  // handle lengths up to 0xffff
  else {
    buffer[offs++] = (u_char)(0x02 | ASN_LONG_LEN);
    buffer[offs++] = (u_char)((length >> 8) & 0xFF);
    buffer[offs++] = (u_char)(length & 0xFF);
  }
}


BOOL PASNObject::DecodeASNLength (const PBYTEArray & buffer, PINDEX & ptr, WORD & len)
{
  PINDEX s = buffer.GetSize();

  if (ptr >= s)
    return FALSE;

  BYTE ch = buffer[ptr++];

  if ((ch & ASN_LONG_LEN) == 0)
    len = (WORD)ch;
  else if ((ch & ~ASN_LONG_LEN) == 0x01) {
    if (ptr >= s)
      return FALSE;
    len = (WORD)buffer[ptr++];
  } else {
    if (ptr + 1 >= s)
      return FALSE;
    len = (WORD)((buffer[ptr] << 8) + buffer[ptr+1]);
    ptr += 2;
  }
  return TRUE;
}


WORD PASNObject::GetASNLengthLength (WORD  length) 
{
  // handle lengths less then 128
  if (length < 128) 
    return 1;

  // handle lengths less than 256
  else if (length < 256) 
    return 2;

  // handle lengths up to 0xffff
  else 
    return 3;
}


void PASNObject::EncodeASNSequenceStart(PBYTEArray & buffer, BYTE type, WORD length) 
{
  buffer[buffer.GetSize()] = type;
  EncodeASNLength(buffer, length);
}


WORD PASNObject::GetASNSequenceStartLength(WORD length) 
{
  return (WORD)(1 + GetASNLengthLength(length));
}


void PASNObject::EncodeASNHeader(PBYTEArray & buffer, PASNObject::ASNType type, WORD length)

{
  buffer[buffer.GetSize()] = ASNTypeToType[type];
  EncodeASNLength(buffer, length);
}


WORD PASNObject::GetASNHeaderLength(WORD length) 
{
  return (WORD)(1 + GetASNLengthLength(length));
}


void PASNObject::EncodeASNInteger (PBYTEArray & buffer, PASNInt data, PASNObject::ASNType type)
{
  DWORD mask;
  WORD  intsize = sizeof(data);

  // create a mask which is the top nine bits of a DWORD, or 0xFF800000
  // on a big endian machine
  mask = 0x1FFUL << ((8 * (sizeof(DWORD) - 1)) - 1);

  // remove all sequences of nine 0's or 1's at the start of the value
  while ((((data & mask) == 0) || ((data & mask) == mask))
    && intsize > 1) {
    intsize--;
    data <<= 8;
  }

  // insert the header
  EncodeASNHeader(buffer, type, intsize);

  // insert the data
  PINDEX offs = buffer.GetSize();
  mask = 0xFFUL << (8 * (sizeof(DWORD) - 1));
  while (intsize--) {
    buffer[offs++] = (u_char)((data & mask) >> (8 * (sizeof(DWORD) - 1)));
    data <<= 8;
  }
}


void PASNObject::EncodeASNUnsigned (PBYTEArray & buffer, PASNUnsigned data, PASNObject::ASNType type)
{
  long mask;
  WORD intsize = sizeof(data);
  int  add_null_byte = 0;

  mask = 0xFFUL << (8 * (sizeof(long) - 1));
  /* mask is 0xFF000000 on a big-endian machine */
  if ((u_char)((data & mask) >> (8 * (sizeof(PASNUnsigned) - 1))) & 0x80){
    /* if MSB is set */
    add_null_byte = 1;
    intsize++;
  }

  // create a mask which is the top nine bits of a DWORD, or 0xFF800000
  // on a big endian machine
  mask = 0x1FFL << ((8 * (sizeof(DWORD) - 1)) - 1);

  // remove all sequences of nine 0's or 1's at the start of the value
  while ((((data & mask) == 0) || (((long)data & mask) == mask))
    && intsize > 1) {
    intsize--;
    data <<= 8;
  }

  // insert the header
  EncodeASNHeader(buffer, type, intsize);

  // insert the data
  PINDEX offs = buffer.GetSize();
  mask = 0xFFL << (8 * (sizeof(DWORD) - 1));
  while (intsize--) {
    buffer[offs++] = (u_char)((data & mask) >> (8 * (sizeof(DWORD) - 1)));
    data <<= 8;
  }

  if (add_null_byte == 1)
    buffer[offs++] = 0;
}


BOOL PASNObject::DecodeASNInteger(const PBYTEArray & buffer, PINDEX & ptr, PASNInt & value, PASNObject::ASNType theType)
{
  if (buffer[ptr++] != ASNTypeToType[theType])
    return FALSE;

  WORD len;
  if (!DecodeASNLength(buffer, ptr, len))
    return FALSE;

  if (ptr + len > buffer.GetSize())
    return FALSE;

  if (buffer[ptr] & 0x80)
    value = -1; /* integer is negative */
  else
    value = 0;

  while (len--)
    value = (value << 8) | buffer[ptr++];
  return TRUE;
}


BOOL PASNObject::DecodeASNUnsigned(const PBYTEArray & buffer, PINDEX & ptr, PASNUnsigned & value, PASNObject::ASNType theType)
{
  if (buffer[ptr++] != ASNTypeToType[theType])
    return FALSE;

  WORD len;
  if (!DecodeASNLength(buffer, ptr, len))
    return FALSE;

  if (ptr + len > buffer.GetSize())
    return FALSE;

//  if (buffer[ptr] & 0x80)
//    value = -1; /* integer is negative */

  value = 0;
  while (len--)
    value = (value << 8) | buffer[ptr++];
  return TRUE;
}


WORD PASNObject::GetASNIntegerLength(PASNInt data) 
{
  DWORD mask;
  WORD  intsize = sizeof(data);

  // create a mask which is the top nine bits of a DWORD, or 0xFF800000
  // on a big endian machine
  mask = 0x1FFUL << ((8 * (sizeof(DWORD) - 1)) - 1);

  // remove all sequences of nine 0's or 1's at the start of the value
  while ((((data & mask) == 0) || ((data & mask) == mask))
    && intsize > 1) {
    intsize--;
    data <<= 8;
  }

  // get the length of the header
  return (WORD)(intsize + GetASNHeaderLength(intsize));
}


WORD PASNObject::GetASNUnsignedLength (PASNUnsigned data)
{
  long mask;
  WORD intsize = sizeof(data);
  int  add_null_byte = 0;

  mask = 0xFFL << (8 * (sizeof(long) - 1));
  /* mask is 0xFF000000 on a big-endian machine */
  if ((u_char)((data & mask) >> (8 * (sizeof(PASNUnsigned) - 1))) & 0x80) {
    /* if MSB is set */
    add_null_byte = 1;
    intsize++;
  }

  // create a mask which is the top nine bits of a DWORD, or 0xFF800000
  // on a big endian machine
  mask = 0x1FFL << ((8 * (sizeof(DWORD) - 1)) - 1);

  // remove all sequences of nine 0's or 1's at the start of the value
  while ((((data & mask) == 0) || (((long)data & mask) == mask))
    && intsize > 1) {
    intsize--;
    data <<= 8;
  }

  // insert the header
  return (WORD)(intsize + GetASNHeaderLength(intsize) + add_null_byte);
}


PASNInt PASNObject::GetInteger () const
{
  PAssertAlways("Cannot return ASN object as Integer");
  return 0;
}


PString PASNObject::GetString  () const
{
  PAssertAlways("Cannot return ASN object as String");
  return PString();
}


PASNUnsigned PASNObject::GetUnsigned() const
{
  PAssertAlways("Cannot return ASN object as Unsigned");
  return 0;
}


const PASNSequence & PASNObject::GetSequence() const
{
  PAssertAlways("Cannot return ASN object as Sequence");
  PASNSequence * ptr = NULL;
  return (PASNSequence &)*ptr;
}


PIPSocket::Address PASNObject::GetIPAddress () const
{
  PAssertAlways("Cannot return ASN object as IP Address");
  return PIPSocket::Address();
}


//////////////////////////////////////////////////////////////////////////
//
//  PASNInteger
//     A descendant of PASNObject which is a simple ASN integer type
//

PASNInteger::PASNInteger(PASNInt val)
{
  value = val;
}


PASNInteger::PASNInteger(const PBYTEArray & buffer, PINDEX & ptr)
{
  DecodeASNInteger(buffer, ptr, value, Integer);
}


void PASNInteger::PrintOn(ostream & strm) const
{
  strm << "Integer: "
       << value
       << endl;
}

void PASNInteger::Encode(PBYTEArray & buffer)
{
  EncodeASNInteger(buffer, value, Integer);
}


WORD PASNInteger::GetEncodedLength()
{
  return GetASNIntegerLength(value);
}


PASNObject::ASNType PASNInteger::GetType() const
{
  return Integer;
}


PString PASNInteger::GetTypeAsString() const
{
  return PString("Integer");
}


PASNInt PASNInteger::GetInteger () const
{
  return value;
}


PString PASNInteger::GetString () const
{
  return PString(PString::Signed, (long)value);
}


PObject * PASNInteger::Clone() const
{
  return new PASNInteger(*this);
}


//////////////////////////////////////////////////////////////////////////
//
//  PASNString
//     A descendant of PASNObject which is a simple ASN OctetStr type
//

PASNString::PASNString(const PString & str)
{
  value    = str;
  valueLen = (WORD)str.GetLength();
}

PASNString::PASNString(const BYTE * ptr, int len)
{
  value = PString((const char *)ptr, len);
  valueLen = (WORD)len;
}

PASNString::PASNString(const PBYTEArray & buffer, PASNObject::ASNType type)
{
  PINDEX ptr = 0;
  Decode(buffer, ptr, type);
}


PASNString::PASNString(const PBYTEArray & buffer, PINDEX & ptr, PASNObject::ASNType type)
{
  Decode(buffer, ptr, type);
}


BOOL PASNString::Decode(const PBYTEArray & buffer, PINDEX & ptr, PASNObject::ASNType type)
{
  valueLen = 0;
  if (buffer[ptr++] != ASNTypeToType[type])
    return FALSE;

  if (!DecodeASNLength(buffer, ptr, valueLen))
    return FALSE;

  if (ptr + valueLen > buffer.GetSize())
    return FALSE;

  value = PString(ptr + (const char *)(const BYTE *)buffer, valueLen);
  ptr += valueLen;

  return TRUE;
}


void PASNString::PrintOn(ostream & strm) const
{
  strm << GetTypeAsString()
       << ": "
       << value
       << endl; 
}


void PASNString::Encode(PBYTEArray & buffer, PASNObject::ASNType type)
{
  // insert the header
  EncodeASNHeader(buffer, type, valueLen);

  // add the string
  PINDEX offs = buffer.GetSize();
  for (PINDEX i = 0; i < valueLen; i++)
    buffer[offs+i] = value[i];
}


WORD PASNString::GetEncodedLength()
{
  return (WORD)(GetASNHeaderLength(valueLen) + (int)valueLen);
}


PASNObject::ASNType PASNString::GetType() const
{
  return String;
}


PString PASNString::GetTypeAsString() const
{
  return PString("String");
}


PString PASNString::GetString () const
{
  return value;
}


PObject * PASNString::Clone() const
{
  return new PASNString(*this);
}


//////////////////////////////////////////////////////////////////////////
//
//  PASNUnsignedInteger
//     A descendant of PASNObject which is an unsigned integer

BOOL PASNUnsignedInteger::Decode(const PBYTEArray & buffer, PINDEX & ptr, PASNObject::ASNType theType)
{
  return DecodeASNUnsigned(buffer, ptr, value, theType);
}


void PASNUnsignedInteger::Encode(PBYTEArray & buffer, PASNObject::ASNType theType)
{
  EncodeASNUnsigned(buffer, value, theType);
}


void PASNUnsignedInteger::PrintOn(ostream & strm) const
{
  strm << GetTypeAsString()
       << " : "
       << value
       << endl; 
}


WORD PASNUnsignedInteger::GetEncodedLength()
{
  return GetASNUnsignedLength(value);
}


PString PASNUnsignedInteger::GetString() const
{
  return PString(PString::Unsigned, (long)value);
}


PASNUnsigned PASNUnsignedInteger::GetUnsigned() const
{
  return value;
}


//////////////////////////////////////////////////////////////////////////
//
//  PASNObjectID
//     A descendant of PASNObject which is a simple ASN ObjID type
//

PASNObjectID::PASNObjectID(PASNOid * val, BYTE theLen)
{
  value.SetSize(theLen);
  memcpy(value.GetPointer(theLen), val, theLen * sizeof(PASNOid)); 
}


PASNObjectID::PASNObjectID(const PString & str)
{
  PINDEX strLen = str.GetLength();

  PINDEX i = 0;
  PINDEX len = 0;
  while (i < strLen) {

    // find the first non-dot character
    while (str[i] == '.' && i < strLen)
      i++;

    // find the next dot
    PINDEX j = str.Find('.', i);

    // convert to a PASNOid
    value.SetSize(len+1);
    value.SetAt(len++, str(i, j).AsInteger());
    i = j;
  }
}


PASNObjectID::PASNObjectID(const PBYTEArray & buffer)
{
  PINDEX ptr = 0;
  Decode(buffer, ptr);
}


PASNObjectID::PASNObjectID(const PBYTEArray & buffer, PINDEX & ptr)
{
  Decode(buffer, ptr);
}


void PASNObjectID::PrintOn(ostream & strm) const
{
  strm << "ObjectId: ";
  for (PINDEX i = 0 ; i < value.GetSize(); i++) {
    strm << value[i];
    if (i != value.GetSize()-1)
      strm << '.';
  }
  strm << endl;
}


void PASNObjectID::Encode(PBYTEArray & buffer)
{
  PBYTEArray  eObjId;
  PINDEX      offs = 0;
  PASNOid     subId, mask, testmask;
  int         bits, testbits;
  PINDEX      objIdLen = value.GetSize();
  PASNOid     *objId = value.GetPointer();

  if (objIdLen < 2) {
    eObjId [offs++] = 0;
    objIdLen = 0;
  } else {
    eObjId [offs++] = (BYTE)(objId[1] + (objId[0] * 40));
    objIdLen -= 2;
    objId += 2;
  }

  while (objIdLen-- > 0) {
    subId = *objId++;
    if (subId < 128) 
      eObjId [offs++] = (BYTE)subId;
    else {
      mask = 0x7F; /* handle subid == 0 case */
      bits = 0;

      /* testmask *MUST* !!!! be of an unsigned type */
      for (testmask = 0x7F, testbits = 0;
           testmask != 0;
           testmask <<= 7, testbits += 7) {
        if (subId & testmask) {  /* if any bits set */
          mask = testmask;
          bits = testbits;
        }
      }

      /* mask can't be zero here */
      for(;mask != 0x7F; mask >>= 7, bits -= 7) {
        /* fix a mask that got truncated above */
        if (mask == 0x1E00000)
          mask = 0xFE00000;
        eObjId [offs++] = (u_char)(((subId & mask) >> bits) | ASN_BIT8);
      }
      eObjId [offs++] = (u_char)(subId & mask);
    }
  }

  PINDEX s = eObjId.GetSize();
  EncodeASNHeader (buffer, ObjectID, (WORD)s);
  offs = buffer.GetSize();
  for (PINDEX i = 0; i < s; i++)
    buffer [offs + i] = eObjId[i];
}


WORD PASNObjectID::GetEncodedLength()
{
  PASNOid    subId, mask, testmask;
  int        bits, testbits;
  PINDEX     objIdLen = value.GetSize();
  WORD       theLen = 0;
  PASNOid    *objId = value.GetPointer();

  if (objIdLen < 2) {
    theLen++;
    objIdLen = 0;
  } else {
    theLen++;
    objIdLen -= 2;
    objId += 2;
  }

  while (objIdLen-- > 0) {
    subId = *objId++;
    if (subId < 128) 
      theLen++;
    else {
      mask = 0x7F; /* handle subid == 0 case */
      bits = 0;

      /* testmask *MUST* !!!! be of an unsigned type */
      for (testmask = 0x7F, testbits = 0;
           testmask != 0;
           testmask <<= 7, testbits += 7) {
        if (subId & testmask) {  /* if any bits set */
          mask = testmask;
          bits = testbits;
        }
      }

      /* mask can't be zero here */
      for(;mask != 0x7F; mask >>= 7, bits -= 7) {
        /* fix a mask that got truncated above */
        if (mask == 0x1E00000)
          mask = 0xFE00000;
        theLen++;
      }
      theLen++;
    }
  }

  return (WORD)(theLen + GetASNHeaderLength(theLen));
}


PASNObject::ASNType PASNObjectID::GetType() const
{
  return ObjectID;
}


PString PASNObjectID::GetTypeAsString() const
{
  return PString("Object ID");
}


PString PASNObjectID::GetString() const
{
  PStringStream str;

  for (PINDEX i = 0; i < value.GetSize(); i++) {
    if (i > 0)
      str << '.';
    str << value[i];
  }

  return str;
}


BOOL PASNObjectID::Decode(const PBYTEArray & buffer, PINDEX & offs)
{
  BYTE type = buffer[offs++];
  PAssert(type == (ASN_OBJECT_ID | ASN_UNIVERSAL | ASN_PRIMITIVE),
          "Attempt to decode non-objectID");
  PASNOid subId;
  
  WORD dataLen;
  if (!DecodeASNLength(buffer, offs, dataLen))
    return FALSE;

  value.SetSize(2);

  // handle zero length strings correctly
  if (dataLen != 0)  {

    // start at the second identifier in the buffer, because we will later
    // expand the first number into the first two IDs
    PINDEX i = 1;
    PINDEX s = buffer.GetSize();

    while (dataLen > 0) {
      subId = 0;
      do {    /* shift and add in low order 7 bits */
        if (dataLen == 0 || offs >= s)
          return FALSE;
        subId = (subId << 7) + (buffer[offs] & ~ASN_BIT8);
        dataLen--;
      } while (buffer[offs++] & ASN_BIT8);
      value.SetAt(i++, subId);
    }

    /*
     * The first two subidentifiers are encoded into the first component
     * with the value (X * 40) + Y, where:
     *  X is the value of the first subidentifier.
     *  Y is the value of the second subidentifier.
     */
    subId = value[1];
    if (subId == 0x2B) {
      value[0] = 1;
      value[1] = 3;
    } else {
      value[1] = subId % 40;
      value[0] = (subId - value[1]) / 40;
    }
  }

  return TRUE;
}


PObject * PASNObjectID::Clone() const
{
  return new PASNObjectID(*this);
}



//////////////////////////////////////////////////////////////////////////
//
//  PASNSequence
//     A descendant of PASNObject which is the complex sequence type
//

PASNSequence::PASNSequence()
{
  encodedLen = 0;
  type    = ASNTypeToType[Sequence];
  asnType = Sequence;
}


PASNSequence::PASNSequence(BYTE selector)
{
  encodedLen = 0;
  PAssert(selector < ASN_CONSTRUCTOR, "Sequence selector too big");
  type    = (BYTE)(ASNTypeToType[Choice] | selector);
  asnType = Choice;
}


void PASNSequence::Append(PASNObject * obj)
{
  sequence.Append(obj);
}


void PASNSequence::AppendInteger(PASNInt value)
{
  Append(new PASNInteger(value));
}


void PASNSequence::AppendString (const PString & str)
{
  Append(new PASNString(str));
}


void PASNSequence::AppendObjectID(const PString & str)
{
  Append(new PASNObjectID(str));
}


void PASNSequence::AppendObjectID(PASNOid * val, BYTE len)
{
  Append(new PASNObjectID(val, len));
}


void PASNSequence::PrintOn(ostream & strm) const
{
  strm << "Sequence:" << endl;
  for (PINDEX i = 0; i < sequence.GetSize(); i++)
    strm << sequence[i];
  strm << "End Sequence" << endl; 
}


void PASNSequence::Encode(PBYTEArray & buffer) 
{
  // calculate the length of the sequence, if it hasn't already been done
  if (encodedLen == 0)
    (void)GetEncodedLength();

  // create the header for the sequence. Note that seqLen was calculated
  // by the call to GetEncodedLength above
  EncodeASNSequenceStart(buffer, type, seqLen);

  // now encode the sequence itself
  for (PINDEX i = 0; i < sequence.GetSize(); i++)
    sequence[i].Encode(buffer);
}

BOOL PASNSequence::Encode(PBYTEArray & buffer, PINDEX maxLen) 
{
  // calculate the length of the sequence, if it hasn't already been done
  if (encodedLen == 0)
    (void)GetEncodedLength();

  // create the header for the sequence. Note that seqLen was calculated
  // by the call to GetEncodedLength above
  EncodeASNSequenceStart(buffer, type, seqLen);

  // now encode the sequence itself
  for (PINDEX i = 0; i < sequence.GetSize(); i++) {
    sequence[i].Encode(buffer);
    if (buffer.GetSize() > maxLen)
      return FALSE;
  }

  return TRUE;
}


WORD PASNSequence::GetEncodedLength()
{
  // calculate the length of the sequence
  if (encodedLen == 0) {
    seqLen = 0;
    for (PINDEX i = 0; i < sequence.GetSize(); i++)
      seqLen = (WORD)(seqLen + sequence[i].GetEncodedLength());
    encodedLen = (WORD)(GetASNSequenceStartLength(seqLen) + seqLen);
  }
  return encodedLen;
}


PASNObject::ASNType PASNSequence::GetType() const
{
  return asnType;
}


int PASNSequence::GetChoice() const
{
  return type;
}


PString PASNSequence::GetTypeAsString() const
{
  return PString("Sequence");
}


PASNSequence::PASNSequence(const PBYTEArray & buffer)

{
  PINDEX ptr = 0;
  if (!Decode(buffer, ptr))
    sequence.RemoveAll();
}


PASNSequence::PASNSequence(const PBYTEArray & buffer, PINDEX & ptr)
{
  if (!Decode(buffer, ptr))
    sequence.RemoveAll();
}


BOOL PASNSequence::Decode(const PBYTEArray & buffer, PINDEX & ptr)
{
  PINDEX s = buffer.GetSize();
  BYTE   c;

  // all sequences start with a sequence start
  if (ptr >= s)
    return FALSE;

  // get the sequence header
  c = buffer[ptr++];
  if (c == (ASN_CONSTRUCTOR | ASN_SEQUENCE)) 
    asnType = Sequence;
  else if ((c & ~ASN_EXTENSION_ID) == (ASN_CONSTRUCTOR | ASN_CONTEXT)) {
    type    = (BYTE)(c & ASN_EXTENSION_ID);
    asnType = Choice;
  } else
    return FALSE;

  // get the sequence length
  WORD len;
  if (!DecodeASNLength(buffer, ptr, len))
    return FALSE;

  // check the length
  if (ptr + len > s)
    return FALSE;

  // set new length
  s = ptr + len;

  // now decode the elements
  BOOL   ok = TRUE;
  while (ptr < s && ok) {
    c = buffer[ptr];
    if ((c & ~ASN_EXTENSION_ID) == (ASN_CONSTRUCTOR | ASN_CONTEXT)) 
      sequence.Append(new PASNSequence(buffer, ptr));
    else switch (c) {

      // Integer
      case ASN_INTEGER | ASN_UNIVERSAL | ASN_PRIMITIVE:
        sequence.Append(new PASNInteger(buffer, ptr));
        break;

      // Octet String
      case ASN_OCTET_STR | ASN_UNIVERSAL | ASN_PRIMITIVE:
        sequence.Append(new PASNString(buffer, ptr));
        break;

      // NULL
      case ASN_NULL | ASN_UNIVERSAL | ASN_PRIMITIVE:
        sequence.Append(new PASNNull(buffer, ptr));
        break;

      // Object ID
      case ASN_OBJECT_ID | ASN_UNIVERSAL | ASN_PRIMITIVE:
        sequence.Append(new PASNObjectID(buffer, ptr));
        break;

      // Sequence
      case ASN_CONSTRUCTOR | ASN_SEQUENCE:
        sequence.Append(new PASNSequence(buffer, ptr));
        break;

      // TimeTicks
      case ASN_APPLICATION | 3:
        sequence.Append(new PASNTimeTicks(buffer, ptr));
        break;

      // Counter
      case ASN_APPLICATION | 1:
        sequence.Append(new PASNCounter(buffer, ptr));
        break;

      // Gauge
      case ASN_APPLICATION | 2:
        sequence.Append(new PASNGauge(buffer, ptr));
        break;

      // IP Address
      case ASN_APPLICATION | 0:
        sequence.Append(new PASNIPAddress(buffer, ptr));
        break;

      default:
        return TRUE;
    }
  }

  return ok;
}


PINDEX PASNSequence::GetSize() const
{
  return sequence.GetSize();
}


PASNObject & PASNSequence::operator [] (PINDEX idx) const
{
  return sequence[idx];
}


const PASNSequence & PASNSequence::GetSequence() const
{
  return *this;
}


PString PASNTimeTicks::GetTypeAsString() const
{
  return PString("TimeTicks");
}


PString PASNCounter::GetTypeAsString() const
{
  return PString("Counter");
}


PString PASNGauge::GetTypeAsString() const
{
  return PString("Gauge");
}


PString PASNIPAddress::GetTypeAsString() const
{
  return PString("IPAddress");
}


PString PASNIPAddress::GetString() const
{
  PINDEX len = value.GetSize();
  if (len == 0) 
    return "(empty)";

  if (len < 4) {
    PString out = "Hex";
    for (PINDEX i = 0; i < len; i++)
      out &= psprintf("%02x", (BYTE)value[i]);
    return out;
  }

  return psprintf("%i.%i.%i.%i",
         (BYTE)value[0], (BYTE)value[1],
         (BYTE)value[2], (BYTE)value[3]);
}


PASNIPAddress::PASNIPAddress(const PString & str)
  : PASNString("")
{
  value.SetSize(4);

  PIPSocket::Address addr;
  if (!PIPSocket::GetHostAddress(str, addr))
    addr = 0;

  int i;
  for (i = 0; i < 4; i++)
    value[i] = addr[i];

  valueLen = 4;
}


PIPSocket::Address PASNIPAddress::GetIPAddress () const
{
  return PIPSocket::Address((BYTE)value[0], (BYTE)value[1],
                            (BYTE)value[2], (BYTE)value[3]);
}

PASNNull::PASNNull()
{
}

PASNNull::PASNNull(const PBYTEArray & buffer, PINDEX & ptr)
{
  PAssert(((buffer.GetSize() - ptr) >= 2) &&
          (buffer[ptr+0] == 0x05) &&
          (buffer[ptr+1] == 0x00),
    "Attempt to decode non-null");
  ptr += 2 ;
}

void PASNNull::PrintOn(ostream & strm) const
{
  strm << "Null"
       << endl;
}

void PASNNull::Encode(PBYTEArray & buffer)
{
  EncodeASNHeader(buffer, Null, 0);
}

WORD PASNNull::GetEncodedLength()
{
  return 2;
}

PObject * PASNNull::Clone() const
{
  return new PASNNull();
}

PASNObject::ASNType PASNNull::GetType() const
{
  return Null;
}

PString PASNNull::GetTypeAsString() const
{
  return PString("Null");
}

PString PASNNull::GetString() const
{
  return PString();
}



// End Of File ///////////////////////////////////////////////////////////////
