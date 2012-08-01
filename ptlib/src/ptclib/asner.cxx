/*
 * asner.cxx
 *
 * Abstract Syntax Notation 1 Encoding Rules
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
 * $Log: asner.cxx,v $
 * Revision 1.93  2005/11/30 12:47:41  csoutheren
 * Removed tabs, reformatted some code, and changed tags for Doxygen
 *
 * Revision 1.92  2005/11/25 01:01:15  csoutheren
 * Applied patch #1351168
 * PWlib various fixes
 *
 * Revision 1.91  2005/06/07 06:25:53  csoutheren
 * Applied patch 1199897 to increase speed of ASN parser debugging output
 * Thanks to Dmitriy <ddv@abinet.com>
 *
 * Revision 1.90  2004/07/12 03:42:22  csoutheren
 * Fixed problem with checking character set constraints too aggressively
 *
 * Revision 1.89  2004/07/12 01:56:10  csoutheren
 * Fixed incorrect asn decoding check
 *
 * Revision 1.88  2004/07/11 14:19:07  csoutheren
 * More bulletproofing of ASN routines against random data attacks
 *
 * Revision 1.87  2004/07/11 12:33:47  csoutheren
 * Added guards against illegal PDU values causing crashes
 *
 * Revision 1.86  2004/04/22 07:54:01  csoutheren
 * Fix problem with VS.net asserting on in isprint when chars outside normal range
 *
 * Revision 1.85  2004/04/18 04:33:37  rjongbloed
 * Changed all operators that return BOOL to return standard type bool. This is primarily
 *   for improved compatibility with std STL usage removing many warnings.
 *
 * Revision 1.84  2004/04/03 08:22:20  csoutheren
 * Remove pseudo-RTTI and replaced with real RTTI
 *
 * Revision 1.83  2004/01/17 09:21:21  csoutheren
 * Added protection against NULL ptr to PASN_Stream::BlockDecode
 *
 * Revision 1.82  2003/08/01 02:11:38  csoutheren
 * Changed to allow easy isolation of PER, BER and XER encoding/decoding routines
 *
 * Revision 1.81  2003/04/28 02:50:33  robertj
 * Fixed problem with spaces in type name, thanks Federico Pinna
 *
 * Revision 1.80  2003/02/26 04:37:21  robertj
 * Tidied some comments
 *
 * Revision 1.79  2003/02/26 01:57:44  robertj
 * Added XML encoding rules to ASN system, thanks Federico Pinna
 *
 * Revision 1.78  2003/01/24 23:43:43  robertj
 * Fixed subtle problems with the use of MAX keyword for unsigned numbers,
 *   should beUINT_MAX not INT_MAX, thanks Stevie Gray for pointing it out.
 *
 * Revision 1.77  2002/12/17 07:00:15  robertj
 * Fixed incorrect encoding of arrays greater than 8192 and less than 16384
 *
 * Revision 1.76  2002/12/13 03:57:17  robertj
 * Fixed crash if enable extension fields beyond the "known" extensions.
 *
 * Revision 1.75  2002/12/02 01:03:33  robertj
 * Fixed bug were if setting the size of a constrained bit string, it
 *   actually sets the size of the underlying byte array correctly.
 *
 * Revision 1.74  2002/11/26 23:29:32  robertj
 * Added missing const to DecodeSubType() function.
 *
 * Revision 1.73  2002/11/22 09:43:32  robertj
 * Fixed encoding of a ASN NULL sequence extension field, eg fastConnectRefused
 *
 * Revision 1.72  2002/11/21 03:46:22  robertj
 * Changed to encode only the minimum number of bits required, this improves
 *   compatibility with some brain dead ASN decoders.
 *
 * Revision 1.71  2002/11/06 22:47:24  robertj
 * Fixed header comment (copyright etc)
 *
 * Revision 1.70  2002/10/31 05:51:10  robertj
 * Changed to use new UTF-8/UCS-2 conversion functions on PString.
 *
 * Revision 1.69  2002/10/29 08:12:44  robertj
 * Fixed MSVC warnings.
 *
 * Revision 1.68  2002/10/29 07:26:45  robertj
 * Fixed subtle bug when encoding or decoding Octet String with 1 or 2 bytes
 *   in it, was not byte aligned correctly.
 *
 * Revision 1.67  2002/09/26 23:53:20  robertj
 * Fixed incorrect asserts in PASN_Enumerated, thanks Platzer Wolfgang
 *
 * Revision 1.66  2002/09/13 08:16:15  robertj
 * Fixed missing line feed when dumping hex octet strings.
 *
 * Revision 1.65  2002/08/06 02:27:58  robertj
 * GNU C++ v3 compatibility.
 *
 * Revision 1.64  2002/07/25 10:52:49  robertj
 * Changes to allow more granularity in PDU dumps, hex output increasing
 *   with increasing trace level.
 *
 * Revision 1.63  2002/06/05 12:29:15  craigs
 * Changes for gcc 3.1
 *
 * Revision 1.62  2002/05/29 01:22:35  robertj
 * Added ability to set object id from unsigned integer arrays.
 *
 * Revision 1.61  2002/05/21 04:23:40  robertj
 * Fixed problem with ASN encoding/decoding unsconstrained negative numbers,
 *
 * Revision 1.60  2002/05/14 08:34:29  robertj
 * Fixed problem encoding unsigned where value==lower bound, thanks Greg Adams.
 *
 * Revision 1.59  2002/05/14 06:59:50  robertj
 * Added more bullet proofing so a malformed PDU cannot cause teh decoder
 *   to try and allocate huge arrays and consume all CPU and memory on a
 *   system. A configurable limit of 100 is set for things like SEQUENCE OF.
 *
 * Revision 1.58  2002/02/08 12:47:19  robertj
 * Fixed incorrect encoding of integer, did not allow for sign bit, thanks Kevin Tran.
 *
 * Revision 1.57  2002/02/01 01:17:36  robertj
 * Fixed bug in encoding empty strings (H.450 issue), thanks Frans Dams, Frank Derks et al.
 *
 * Revision 1.56  2002/01/30 08:40:55  robertj
 * Fixed incorrect decode function in BER string decode, thanks ct_dev@sohu.com
 *
 * Revision 1.55  2001/12/13 09:13:57  robertj
 * Added function get get oid as a string.
 *
 * Revision 1.54  2001/11/26 03:07:13  robertj
 * Fixed decode of extendable constrained integer types.
 *
 * Revision 1.53  2001/09/14 05:26:11  robertj
 * Fixed problem with assigning a PASN_Choice to itself, thanks Chih-Wei Huang
 *
 * Revision 1.52  2001/09/14 01:59:59  robertj
 * Fixed problem with incorrectly initialised PASN_Choice sub-object.
 *
 * Revision 1.51  2001/08/08 04:19:28  robertj
 * Fixed PString<->BMPString conversion so can have embedded nulls.
 *
 * Revision 1.50  2001/08/07 04:37:03  robertj
 * Simplified &#num; parsing.
 *
 * Revision 1.49  2001/08/07 02:49:05  robertj
 * Fixed incorrect alignment if constrained string upper bound is exactly
 *     16 bits long. thanks Guntram Diehl & Thomas Arimont.
 *
 * Revision 1.48  2001/08/06 09:35:25  robertj
 * Fixed GNU compatibility.
 *
 * Revision 1.47  2001/08/06 09:31:48  robertj
 * Added conversion of BMPString to PString without losing special characters.
 *
 * Revision 1.46  2001/08/06 01:39:02  robertj
 * Added assignement operator with RHS of PASN_BMPString to classes
 *   descended from PASN_BMPString.
 *
 * Revision 1.45  2001/06/14 02:14:12  robertj
 * Added functions to encode and decode another ASN type that is inside
 *   an octet string, useful for ANY or EXTERNAL types etc.
 *
 * Revision 1.44  2001/05/29 00:59:16  robertj
 * Fixed excessive padding on constrained strings.
 *
 * Revision 1.43  2001/05/22 23:37:42  robertj
 * Fixed problem with assigning a constrained string value to itself, which
 *   can occur when changing constraints.
 *
 * Revision 1.42  2001/04/30 10:47:33  robertj
 * Fixed stupid error in last patch.
 *
 * Revision 1.41  2001/04/30 06:47:04  robertj
 * Fixed problem with en/decoding more than 16 extension fields in a sequence.
 *
 * Revision 1.40  2001/04/26 08:15:58  robertj
 * Fixed problem with ASN compile of single constraints on enumerations.
 *
 * Revision 1.39  2001/04/23 05:46:06  robertj
 * Fixed problem with unconstrained PASN_NumericString coding in 8 bits
 *   instead of 4, thanks Chew Kuan.
 *
 * Revision 1.38  2001/04/23 04:40:14  robertj
 * Added ASN standard types GeneralizedTime and UTCTime
 *
 * Revision 1.37  2001/04/12 03:26:59  robertj
 * Fixed PASN_Boolean cosntructor to be compatible with usage in ASN parser.
 * Changed all PASN_xxx types so constructor can take real type as only
 *   parameter. eg PASN_OctetString s = "fred";
 * Changed block encode/decode so does not do a ByteAlign() if zero
 *   length, required for interoperability even though spec implies otherwise..
 *
 * Revision 1.36  2001/01/24 04:37:07  robertj
 * Added more bulletproofing to ASN structures to obey constraints.
 *
 * Revision 1.35  2001/01/03 01:20:13  robertj
 * Fixed error in BlockEncode, should ByteAlign() even on zero length strings.
 *
 * Revision 1.34  2000/10/26 11:09:16  robertj
 * More bullet proofing of PER decoder, changed bit type to be unsigned.
 *
 * Revision 1.33  2000/10/26 01:29:32  robertj
 * Fixed MSVC warning.
 *
 * Revision 1.32  2000/10/25 04:05:38  robertj
 * More bullet proofing of PER decoder.
 *
 * Revision 1.31  2000/09/29 04:11:51  robertj
 * Fixed possible out of range memory access, thanks Petr Parýzek <paryzek@wo.cz>
 *
 * Revision 1.30  2000/02/29 06:32:12  robertj
 * Added ability to remove optional field in sequence, thanks Dave Harvey.
 *
 * Revision 1.29  2000/01/20 06:22:22  robertj
 * Fixed boundary condition error for constrained integer encoding (values 1, 256 etc)
 *
 * Revision 1.28  1999/11/22 23:15:43  robertj
 * Fixed bug in PASN_Choice::Compare(), should make sure choices are the same before comparing.
 *
 * Revision 1.27  1999/08/19 15:43:07  robertj
 * Fixed incorrect size of OID if zero length encoded.
 *
 * Revision 1.26  1999/08/09 13:02:45  robertj
 * dded ASN compiler #defines for backward support of pre GCC 2.9 compilers.
 * Added ASN compiler #defines to reduce its memory footprint.
 *
 * Revision 1.25  1999/08/08 15:45:59  robertj
 * Fixed incorrect encoding of unknown extensions.
 *
 * Revision 1.24  1999/08/05 00:44:28  robertj
 * Fixed PER encoding problems for large integer values.
 *
 * Revision 1.23  1999/07/22 06:48:54  robertj
 * Added comparison operation to base ASN classes and compiled ASN code.
 * Added support for ANY type in ASN parser.
 *
 * Revision 1.22  1999/07/08 08:39:12  robertj
 * Fixed bug when assigning negative number ot cosntrained PASN_Integer
 *
 * Revision 1.21  1999/06/30 08:58:12  robertj
 * Fixed bug in encoding/decoding OID greater than 2.39
 *
 * Revision 1.20  1999/06/17 13:27:09  robertj
 * Fixed bug causing crashes on pass through of unknown extensions.
 *
 * Revision 1.19  1999/06/07 00:31:25  robertj
 * Fixed signed/unsigned problem with number of unknown extensions check.
 *
 * Revision 1.18  1999/04/26 05:58:48  craigs
 * Fixed problems with encoding of extensions
 *
 * Revision 1.17  1999/03/09 08:12:38  robertj
 * Fixed problem with closing a steam encoding twice.
 *
 * Revision 1.16  1999/01/16 01:28:25  robertj
 * Fixed problems with reading stream multiple times.
 *
 * Revision 1.15  1998/11/30 04:50:44  robertj
 * New directory structure
 *
 * Revision 1.14  1998/10/22 04:33:11  robertj
 * Fixed bug in constrained strings and PER, incorrect order of character set.
 *
 * Revision 1.13  1998/09/23 06:21:49  robertj
 * Added open source copyright license.
 *
 * Revision 1.12  1998/05/26 05:29:23  robertj
 * Workaroung for g++ iostream bug.
 *
 * Revision 1.11  1998/05/21 04:58:54  robertj
 * GCC comptaibility.
 *
 * Revision 1.10  1998/05/21 04:26:54  robertj
 * Fixed numerous PER problems.
 *
 * Revision 1.9  1998/05/11 06:01:55  robertj
 * Why did this compile under MSC?
 *
 * Revision 1.8  1998/05/07 05:19:29  robertj
 * Fixed problems with using copy constructor/assignment oeprator on PASN_Objects.
 *
 * Revision 1.7  1998/03/05 12:49:50  robertj
 * MemCheck fixes.
 *
 * Revision 1.6  1998/02/03 06:28:27  robertj
 * Fixed length calculation of integers in BER.
 * Added new function to read a block with minimum number of bytes.
 *
 * Revision 1.5  1998/01/26 01:51:20  robertj
 * Removed uninitialised variable warnings.
 *
 * Revision 1.4  1997/12/18 05:07:56  robertj
 * Fixed bug in choice name display.
 * Added function to get choice discriminator name.
 * Fixed bug in encoding extensions.
 *
 * Revision 1.3  1997/12/11 10:36:22  robertj
 * Support for new ASN parser.
 *
 */

#include <ptlib.h>

#ifdef __GNUC__
#pragma implementation "asner.h"
#endif

#include <ptclib/asner.h>

#if P_EXPAT
#include <ptclib/pxml.h>
#endif

#define new PNEW


static PINDEX MaximumArraySize     = 128;
static PINDEX MaximumStringSize    = 16*1024;
static PINDEX MaximumSetSize       = 512;


static PINDEX CountBits(unsigned range)
{
  switch (range) {
    case 0 :
      return sizeof(unsigned)*8;
    case 1:
      return 1;
  }

  size_t nBits = 0;
  while (nBits < (sizeof(unsigned)*8) && range > (unsigned)(1 << nBits))
    nBits++;
  return nBits;
}

inline BOOL CheckByteOffset(PINDEX offset, PINDEX upper = MaximumStringSize)
{
  // a 1mbit PDU has got to be an error
  return (0 <= offset && offset <= upper);
}

static PINDEX FindNameByValue(const PASN_Names *names, unsigned namesCount, PINDEX value)
{
  if (names != NULL) {
    for (unsigned int i = 0;i < namesCount;i++) {
      if (names[i].value == value)
        return i;
    }
  }
  return P_MAX_INDEX;
}

///////////////////////////////////////////////////////////////////////

PASN_Object::PASN_Object(unsigned theTag, TagClass theTagClass, BOOL extend)
{
  extendable = extend;

  tag = theTag;

  if (theTagClass != DefaultTagClass)
    tagClass = theTagClass;
  else
    tagClass = ContextSpecificTagClass;
}


void PASN_Object::SetTag(unsigned newTag, TagClass tagClass_)
{
  tag = newTag;
  if (tagClass_ != DefaultTagClass)
    tagClass = tagClass_;
}


PINDEX PASN_Object::GetObjectLength() const
{
  PINDEX len = 1;

  if (tag >= 31)
    len += (CountBits(tag)+6)/7;

  PINDEX dataLen = GetDataLength();
  if (dataLen < 128)
    len++;
  else
    len += (CountBits(dataLen)+7)/8 + 1;

  return len + dataLen;
}


void PASN_Object::SetConstraintBounds(ConstraintType, int, unsigned)
{
}


void PASN_Object::SetCharacterSet(ConstraintType, const char *)
{
}


void PASN_Object::SetCharacterSet(ConstraintType, unsigned, unsigned)
{
}


PINDEX PASN_Object::GetMaximumArraySize()
{
  return MaximumArraySize;
}


void PASN_Object::SetMaximumArraySize(PINDEX sz)
{
  MaximumArraySize = sz;
}


PINDEX PASN_Object::GetMaximumStringSize()
{
  return MaximumStringSize;
}


void PASN_Object::SetMaximumStringSize(PINDEX sz)
{
  MaximumStringSize = sz;
}


///////////////////////////////////////////////////////////////////////

PASN_ConstrainedObject::PASN_ConstrainedObject(unsigned tag, TagClass tagClass)
  : PASN_Object(tag, tagClass)
{
  constraint = Unconstrained;
  lowerLimit = 0;
  upperLimit =  UINT_MAX;
}


void PASN_ConstrainedObject::SetConstraintBounds(ConstraintType ctype,
                                                 int lower, unsigned upper)
{
  constraint = ctype;
  if (constraint == Unconstrained) {
    lower = 0;
    upper = UINT_MAX;
  }

  extendable = ctype == ExtendableConstraint;
//  if ((lower >= 0 && upper < 0x7fffffff) ||
//     (lower < 0 && (unsigned)lower <= upper)) {
    lowerLimit = lower;
    upperLimit = upper;
//  }
}


///////////////////////////////////////////////////////////////////////

PASN_Null::PASN_Null(unsigned tag, TagClass tagClass)
  : PASN_Object(tag, tagClass)
{
}


PObject::Comparison PASN_Null::Compare(const PObject & obj) const
{
  PAssert(PIsDescendant(&obj, PASN_Null), PInvalidCast);
  return EqualTo;
}


PObject * PASN_Null::Clone() const
{
  PAssert(IsClass(PASN_Null::Class()), PInvalidCast);
  return new PASN_Null(*this);
}


void PASN_Null::PrintOn(ostream & strm) const
{
  strm << "<<null>>";
}


PString PASN_Null::GetTypeAsString() const
{
  return "Null";
}


PINDEX PASN_Null::GetDataLength() const
{
  return 0;
}


BOOL PASN_Null::Decode(PASN_Stream & strm)
{
  return strm.NullDecode(*this);
}


void PASN_Null::Encode(PASN_Stream & strm) const
{
  strm.NullEncode(*this);
}


///////////////////////////////////////////////////////////////////////

PASN_Boolean::PASN_Boolean(BOOL val)
  : PASN_Object(UniversalBoolean, UniversalTagClass)
{
  value = val;
}


PASN_Boolean::PASN_Boolean(unsigned tag, TagClass tagClass, BOOL val)
  : PASN_Object(tag, tagClass)
{
  value = val;
}


PObject::Comparison PASN_Boolean::Compare(const PObject & obj) const
{
  PAssert(PIsDescendant(&obj, PASN_Boolean), PInvalidCast);
  return value == ((const PASN_Boolean &)obj).value ? EqualTo : GreaterThan;
}


PObject * PASN_Boolean::Clone() const
{
  PAssert(IsClass(PASN_Boolean::Class()), PInvalidCast);
  return new PASN_Boolean(*this);
}


void PASN_Boolean::PrintOn(ostream & strm) const
{
  if (value)
    strm << "TRUE";
  else
    strm << "FALSE";
}


PString PASN_Boolean::GetTypeAsString() const
{
  return "Boolean";
}


PINDEX PASN_Boolean::GetDataLength() const
{
  return 1;
}


BOOL PASN_Boolean::Decode(PASN_Stream & strm)
{
  return strm.BooleanDecode(*this);
}


void PASN_Boolean::Encode(PASN_Stream & strm) const
{
  strm.BooleanEncode(*this);
}


///////////////////////////////////////////////////////////////////////

PASN_Integer::PASN_Integer(unsigned val)
  : PASN_ConstrainedObject(UniversalInteger, UniversalTagClass)
{
  value = val;
}


PASN_Integer::PASN_Integer(unsigned tag, TagClass tagClass, unsigned val)
  : PASN_ConstrainedObject(tag, tagClass)
{
  value = val;
}


BOOL PASN_Integer::IsUnsigned() const
{
  return constraint != Unconstrained && lowerLimit >= 0;
}


PASN_Integer & PASN_Integer::operator=(unsigned val)
{
  if (constraint == Unconstrained)
    value = val;
  else if (lowerLimit >= 0) { // Is unsigned integer
    if (val < (unsigned)lowerLimit)
      value = lowerLimit;
    else if (val > upperLimit)
      value = upperLimit;
    else
      value = val;
  }
  else {
    int ival = (int)val;
    if (ival < lowerLimit)
      value = lowerLimit;
    else if (upperLimit < INT_MAX && ival > (int)upperLimit)
      value = upperLimit;
    else
      value = val;
  }

  return *this;
}


PObject::Comparison PASN_Integer::Compare(const PObject & obj) const
{
  PAssert(PIsDescendant(&obj, PASN_Integer), PInvalidCast);
  const PASN_Integer & other = (const PASN_Integer &)obj;

  if (IsUnsigned()) {
    if (value < other.value)
      return LessThan;
    if (value > other.value)
      return GreaterThan;
  }
  else {
    if ((int)value < (int)other.value)
      return LessThan;
    if ((int)value > (int)other.value)
      return GreaterThan;
  }
  return EqualTo;
}


PObject * PASN_Integer::Clone() const
{
  PAssert(IsClass(PASN_Integer::Class()), PInvalidCast);
  return new PASN_Integer(*this);
}


void PASN_Integer::PrintOn(ostream & strm) const
{
  if (constraint == Unconstrained || lowerLimit < 0)
    strm << (int)value;
  else
    strm << value;
}


void PASN_Integer::SetConstraintBounds(ConstraintType type, int lower, unsigned upper)
{
  PASN_ConstrainedObject::SetConstraintBounds(type, lower, upper);
  operator=(value);
}


PString PASN_Integer::GetTypeAsString() const
{
  return "Integer";
}


static PINDEX GetIntegerDataLength(int value)
{
  // create a mask which is the top nine bits of a DWORD, or 0xFF800000
  // on a big endian machine
  int shift = (sizeof(value)-1)*8-1;

  // remove all sequences of nine 0's or 1's at the start of the value
  while (shift > 0 && ((value >> shift)&0x1ff) == (value < 0 ? 0x1ff : 0))
    shift -= 8;

  return (shift+9)/8;
}


PINDEX PASN_Integer::GetDataLength() const
{
  return GetIntegerDataLength(value);
}


BOOL PASN_Integer::Decode(PASN_Stream & strm)
{
  return strm.IntegerDecode(*this);
}


void PASN_Integer::Encode(PASN_Stream & strm) const
{
  strm.IntegerEncode(*this);
}


///////////////////////////////////////////////////////////////////////

PASN_Enumeration::PASN_Enumeration(unsigned val)
: PASN_Object(UniversalEnumeration, UniversalTagClass, FALSE),names(NULL),namesCount(0)
{
  value = val;
  maxEnumValue = P_MAX_INDEX;
}


PASN_Enumeration::PASN_Enumeration(unsigned tag, TagClass tagClass,
                                   unsigned maxEnum, BOOL extend,
                                   unsigned val)
  : PASN_Object(tag, tagClass, extend),names(NULL),namesCount(0)
{
  value = val;
  maxEnumValue = maxEnum;
}



PASN_Enumeration::PASN_Enumeration(unsigned tag, TagClass tagClass,
                                   unsigned maxEnum, BOOL extend,
                                   const PASN_Names * nameSpec,
                                   unsigned namesCnt,
                                   unsigned val)
  : PASN_Object(tag, tagClass, extend),
  names(nameSpec),namesCount(namesCnt)
{
  maxEnumValue = maxEnum;

  PAssert(val <= maxEnum, PInvalidParameter);
  value = val;
}


PObject::Comparison PASN_Enumeration::Compare(const PObject & obj) const
{
  PAssert(PIsDescendant(&obj, PASN_Enumeration), PInvalidCast);
  const PASN_Enumeration & other = (const PASN_Enumeration &)obj;

  if (value < other.value)
    return LessThan;

  if (value > other.value)
    return GreaterThan;

  return EqualTo;
}


PObject * PASN_Enumeration::Clone() const
{
  PAssert(IsClass(PASN_Enumeration::Class()), PInvalidCast);
  return new PASN_Enumeration(*this);
}


void PASN_Enumeration::PrintOn(ostream & strm) const
{
  PINDEX idx = FindNameByValue(names, namesCount, value);
  if (idx != P_MAX_INDEX)
    strm << names[idx].name;
  else
    strm << '<' << value << '>';
}


PString PASN_Enumeration::GetTypeAsString() const
{
  return "Enumeration";
}


PINDEX PASN_Enumeration::GetDataLength() const
{
  return GetIntegerDataLength(value);
}


BOOL PASN_Enumeration::Decode(PASN_Stream & strm)
{
  return strm.EnumerationDecode(*this);
}


void PASN_Enumeration::Encode(PASN_Stream & strm) const
{
  strm.EnumerationEncode(*this);
}

PINDEX PASN_Enumeration::GetValueByName(PString name) const
{
  for(unsigned uiIndex = 0; uiIndex < namesCount; uiIndex++){
    if(strcmp(names[uiIndex].name, name) == 0){
      return (maxEnumValue - namesCount + uiIndex + 1);
    }
  }
  return UINT_MAX;
}

///////////////////////////////////////////////////////////////////////

PASN_Real::PASN_Real(double val)
  : PASN_Object(UniversalReal, UniversalTagClass)
{
  value = val;
}


PASN_Real::PASN_Real(unsigned tag, TagClass tagClass, double val)
  : PASN_Object(tag, tagClass)
{
  value = val;
}


PObject::Comparison PASN_Real::Compare(const PObject & obj) const
{
  PAssert(PIsDescendant(&obj, PASN_Real), PInvalidCast);
  const PASN_Real & other = (const PASN_Real &)obj;

  if (value < other.value)
    return LessThan;

  if (value > other.value)
    return GreaterThan;

  return EqualTo;
}


PObject * PASN_Real::Clone() const
{
  PAssert(IsClass(PASN_Real::Class()), PInvalidCast);
  return new PASN_Real(*this);
}


void PASN_Real::PrintOn(ostream & strm) const
{
  strm << value;
}


PString PASN_Real::GetTypeAsString() const
{
  return "Real";
}


PINDEX PASN_Real::GetDataLength() const
{
  PAssertAlways(PUnimplementedFunction);
  return 0;
}


BOOL PASN_Real::Decode(PASN_Stream & strm)
{
  return strm.RealDecode(*this);
}


void PASN_Real::Encode(PASN_Stream & strm) const
{
  strm.RealEncode(*this);
}


///////////////////////////////////////////////////////////////////////

PASN_ObjectId::PASN_ObjectId(const char * dotstr)
  : PASN_Object(UniversalObjectId, UniversalTagClass)
{
  if (dotstr != NULL)
    SetValue(dotstr);
}


PASN_ObjectId::PASN_ObjectId(unsigned tag, TagClass tagClass)
  : PASN_Object(tag, tagClass)
{
}


PASN_ObjectId::PASN_ObjectId(const PASN_ObjectId & other)
  : PASN_Object(other),
    value(other.value, other.GetSize())
{
}


PASN_ObjectId & PASN_ObjectId::operator=(const PASN_ObjectId & other)
{
  PASN_Object::operator=(other);
  value = PUnsignedArray(other.value, other.GetSize());
  return *this;
}


PASN_ObjectId & PASN_ObjectId::operator=(const char * dotstr)
{
  if (dotstr != NULL)
    SetValue(dotstr);
  else
    value.SetSize(0);
  return *this;
}


PASN_ObjectId & PASN_ObjectId::operator=(const PString & dotstr)
{
  SetValue(dotstr);
  return *this;
}


PASN_ObjectId & PASN_ObjectId::operator=(const PUnsignedArray & numbers)
{
  SetValue(numbers);
  return *this;
}


void PASN_ObjectId::SetValue(const PString & dotstr)
{
  PStringArray parts = dotstr.Tokenise('.');
  value.SetSize(parts.GetSize());
  for (PINDEX i = 0; i < parts.GetSize(); i++)
    value[i] = parts[i].AsUnsigned();
}


void PASN_ObjectId::SetValue(const unsigned * numbers, PINDEX size)
{
  value = PUnsignedArray(numbers, size);
}


bool PASN_ObjectId::operator==(const char * dotstr) const
{
  PASN_ObjectId id;
  id.SetValue(dotstr);
  return *this == id;
}


PObject::Comparison PASN_ObjectId::Compare(const PObject & obj) const
{
  PAssert(PIsDescendant(&obj, PASN_ObjectId), PInvalidCast);
  const PASN_ObjectId & other = (const PASN_ObjectId &)obj;
  return value.Compare(other.value);
}


PObject * PASN_ObjectId::Clone() const
{
  PAssert(IsClass(PASN_ObjectId::Class()), PInvalidCast);
  return new PASN_ObjectId(*this);
}


void PASN_ObjectId::PrintOn(ostream & strm) const
{
  for (PINDEX i = 0; i < value.GetSize(); i++) {
    strm << (unsigned)value[i];
    if (i < value.GetSize()-1)
      strm << '.';
  }
}


PString PASN_ObjectId::AsString() const
{
  PStringStream s;
  PrintOn(s);
  return s;
}


PString PASN_ObjectId::GetTypeAsString() const
{
  return "Object ID";
}


BOOL PASN_ObjectId::CommonDecode(PASN_Stream & strm, unsigned dataLen)
{
  value.SetSize(0);

  // handle zero length strings correctly
  if (dataLen == 0)
    return TRUE;

  unsigned subId;

  // start at the second identifier in the buffer, because we will later
  // expand the first number into the first two IDs
  PINDEX i = 1;
  while (dataLen > 0) {
    unsigned byte;
    subId = 0;
    do {    /* shift and add in low order 7 bits */
      if (strm.IsAtEnd())
        return FALSE;
      byte = strm.ByteDecode();
      subId = (subId << 7) + (byte & 0x7f);
      dataLen--;
    } while ((byte & 0x80) != 0);
    value.SetAt(i++, subId);
  }

  /*
   * The first two subidentifiers are encoded into the first component
   * with the value (X * 40) + Y, where:
   *  X is the value of the first subidentifier.
   *  Y is the value of the second subidentifier.
   */
  subId = value[1];
  if (subId < 40) {
    value[0] = 0;
    value[1] = subId;
  }
  else if (subId < 80) {
    value[0] = 1;
    value[1] = subId-40;
  }
  else {
    value[0] = 2;
    value[1] = subId-80;
  }

  return TRUE;
}


void PASN_ObjectId::CommonEncode(PBYTEArray & encodecObjectId) const
{
  PINDEX length = value.GetSize();
  const unsigned * objId = value;

  if (length < 2) {
    // Thise case is really illegal, but we have to do SOMETHING
    encodecObjectId.SetSize(0);
    return;
  }

  unsigned subId = (objId[0] * 40) + objId[1];
  objId += 2;

  PINDEX outputPosition = 0;

  while (--length > 0) {
    if (subId < 128)
      encodecObjectId[outputPosition++] = (BYTE)subId;
    else {
      unsigned mask = 0x7F; /* handle subid == 0 case */
      int bits = 0;

      /* testmask *MUST* !!!! be of an unsigned type */
      unsigned testmask = 0x7F;
      int      testbits = 0;
      while (testmask != 0) {
        if (subId & testmask) {  /* if any bits set */
          mask = testmask;
          bits = testbits;
        }
        testmask <<= 7;
        testbits += 7;
      }

      /* mask can't be zero here */
      while (mask != 0x7F) {
        /* fix a mask that got truncated above */
        if (mask == 0x1E00000)
          mask = 0xFE00000;

        encodecObjectId[outputPosition++] = (BYTE)(((subId & mask) >> bits) | 0x80);

        mask >>= 7;
        bits -= 7;
      }

      encodecObjectId[outputPosition++] = (BYTE)(subId & mask);
    }

    if (length > 1)
      subId = *objId++;
  }
}


PINDEX PASN_ObjectId::GetDataLength() const
{
  PBYTEArray dummy;
  CommonEncode(dummy);
  return dummy.GetSize();
}


BOOL PASN_ObjectId::Decode(PASN_Stream & strm)
{
  return strm.ObjectIdDecode(*this);
}


void PASN_ObjectId::Encode(PASN_Stream & strm) const
{
  strm.ObjectIdEncode(*this);
}


///////////////////////////////////////////////////////////////////////

PASN_BitString::PASN_BitString(unsigned nBits, const BYTE * buf)
  : PASN_ConstrainedObject(UniversalBitString, UniversalTagClass),
    totalBits(nBits),
    bitData((totalBits+7)/8)
{
  if (buf != NULL)
    memcpy(bitData.GetPointer(), buf, bitData.GetSize());
}


PASN_BitString::PASN_BitString(unsigned tag, TagClass tagClass, unsigned nBits)
  : PASN_ConstrainedObject(tag, tagClass),
    totalBits(nBits),
    bitData((totalBits+7)/8)
{
}


PASN_BitString::PASN_BitString(const PASN_BitString & other)
  : PASN_ConstrainedObject(other),
    bitData(other.bitData, other.bitData.GetSize())
{
  totalBits = other.totalBits;
}


PASN_BitString & PASN_BitString::operator=(const PASN_BitString & other)
{
  PASN_ConstrainedObject::operator=(other);
  totalBits = other.totalBits;
  bitData = PBYTEArray(other.bitData, other.bitData.GetSize());
  return *this;
}


void PASN_BitString::SetData(unsigned nBits, const PBYTEArray & bytes)
{
  if ((PINDEX)nBits >= MaximumStringSize)
    return;

  bitData = bytes;
  SetSize(nBits);
}


void PASN_BitString::SetData(unsigned nBits, const BYTE * buf, PINDEX size)
{
  if ((PINDEX)nBits >= MaximumStringSize)
    return;

  if (size == 0)
    size = (nBits+7)/8;
  memcpy(bitData.GetPointer(size), buf, size);
  SetSize(nBits);
}


BOOL PASN_BitString::SetSize(unsigned nBits)
{
  if (!CheckByteOffset(nBits))
    return FALSE;

  if (constraint == Unconstrained)
    totalBits = nBits;
  else if (totalBits < (unsigned)lowerLimit) {
    if (lowerLimit < 0)
      return FALSE;
    totalBits = lowerLimit;
  } else if ((unsigned)totalBits > upperLimit) {
    if (upperLimit > (unsigned)MaximumSetSize)
      return FALSE;
    totalBits = upperLimit;
  } else
    totalBits = nBits;
  return bitData.SetSize((totalBits+7)/8);
}


bool PASN_BitString::operator[](PINDEX bit) const
{
  if ((unsigned)bit < totalBits)
    return (bitData[bit>>3] & (1 << (7 - (bit&7)))) != 0;
  return FALSE;
}


void PASN_BitString::Set(unsigned bit)
{
  if (bit < totalBits)
    bitData[(PINDEX)(bit>>3)] |= 1 << (7 - (bit&7));
}


void PASN_BitString::Clear(unsigned bit)
{
  if (bit < totalBits)
    bitData[(PINDEX)(bit>>3)] &= ~(1 << (7 - (bit&7)));
}


void PASN_BitString::Invert(unsigned bit)
{
  if (bit < totalBits)
    bitData[(PINDEX)(bit>>3)] ^= 1 << (7 - (bit&7));
}


PObject::Comparison PASN_BitString::Compare(const PObject & obj) const
{
  PAssert(PIsDescendant(&obj, PASN_BitString), PInvalidCast);
  const PASN_BitString & other = (const PASN_BitString &)obj;
  if (totalBits < other.totalBits)
    return LessThan;
  if (totalBits > other.totalBits)
    return GreaterThan;
  return bitData.Compare(other.bitData);
}


PObject * PASN_BitString::Clone() const
{
  PAssert(IsClass(PASN_BitString::Class()), PInvalidCast);
  return new PASN_BitString(*this);
}


void PASN_BitString::PrintOn(ostream & strm) const
{
  int indent = strm.precision() + 2;
  _Ios_Fmtflags flags = strm.flags();

  if (totalBits > 128)
    strm << "Hex {\n"
         << hex << setfill('0') << resetiosflags(ios::floatfield) << setiosflags(ios::fixed)
         << setw(16) << setprecision(indent) << bitData
         << dec << setfill(' ') << resetiosflags(ios::floatfield)
         << setw(indent-1) << "}";
  else if (totalBits > 32)
    strm << "Hex:"
         << hex << setfill('0') << resetiosflags(ios::floatfield) << setiosflags(ios::fixed)
         << setprecision(2) << setw(16) << bitData
         << dec << setfill(' ') << resetiosflags(ios::floatfield);
  else {
    BYTE mask = 0x80;
    PINDEX offset = 0;
    for (unsigned i = 0; i < totalBits; i++) {
      strm << ((bitData[offset]&mask) != 0 ? '1' : '0');
      mask >>= 1;
      if (mask == 0) {
        mask = 0x80;
        offset++;
      }
    }
  }

  strm.flags(flags);
}


void PASN_BitString::SetConstraintBounds(ConstraintType type, int lower, unsigned upper)
{
  if (lower < 0)
    return;

  PASN_ConstrainedObject::SetConstraintBounds(type, lower, upper);
  SetSize(GetSize());
}


PString PASN_BitString::GetTypeAsString() const
{
  return "Bit String";
}


PINDEX PASN_BitString::GetDataLength() const
{
  return (totalBits+7)/8 + 1;
}


BOOL PASN_BitString::Decode(PASN_Stream & strm)
{
  return strm.BitStringDecode(*this);
}


void PASN_BitString::Encode(PASN_Stream & strm) const
{
  strm.BitStringEncode(*this);
}


///////////////////////////////////////////////////////////////////////

PASN_OctetString::PASN_OctetString(const char * str, PINDEX size)
  : PASN_ConstrainedObject(UniversalOctetString, UniversalTagClass)
{
  if (str != NULL) {
    if (size == 0)
      size = ::strlen(str);
    SetValue((const BYTE *)str, size);
  }
}


PASN_OctetString::PASN_OctetString(unsigned tag, TagClass tagClass)
  : PASN_ConstrainedObject(tag, tagClass)
{
}


PASN_OctetString::PASN_OctetString(const PASN_OctetString & other)
  : PASN_ConstrainedObject(other),
    value(other.value, other.GetSize())
{
}


PASN_OctetString & PASN_OctetString::operator=(const PASN_OctetString & other)
{
  PASN_ConstrainedObject::operator=(other);
  value = PBYTEArray(other.value, other.GetSize());
  return *this;
}


PASN_OctetString & PASN_OctetString::operator=(const char * str)
{
  if (str == NULL)
    value.SetSize(lowerLimit);
  else
    SetValue((const BYTE *)str, strlen(str));
  return *this;
}


PASN_OctetString & PASN_OctetString::operator=(const PString & str)
{
  SetValue((const BYTE *)(const char *)str, str.GetSize()-1);
  return *this;
}


PASN_OctetString & PASN_OctetString::operator=(const PBYTEArray & arr)
{
  PINDEX len = arr.GetSize();
  if ((unsigned)len > upperLimit || (int)len < lowerLimit)
    SetValue(arr, len);
  else
    value = arr;
  return *this;
}


void PASN_OctetString::SetValue(const BYTE * data, PINDEX len)
{
  if ((unsigned)len > upperLimit)
    len = upperLimit;
  if (SetSize((int)len < lowerLimit ? lowerLimit : len))
    memcpy(value.GetPointer(), data, len);
}


PString PASN_OctetString::AsString() const
{
  if (value.IsEmpty())
    return PString();
  return PString((const char *)(const BYTE *)value, value.GetSize());
}


PObject::Comparison PASN_OctetString::Compare(const PObject & obj) const
{
  PAssert(PIsDescendant(&obj, PASN_OctetString), PInvalidCast);
  const PASN_OctetString & other = (const PASN_OctetString &)obj;
  return value.Compare(other.value);
}


PObject * PASN_OctetString::Clone() const
{
  PAssert(IsClass(PASN_OctetString::Class()), PInvalidCast);
  return new PASN_OctetString(*this);
}


void PASN_OctetString::PrintOn(ostream & strm) const
{
  int indent = strm.precision() + 2;
  _Ios_Fmtflags flags = strm.flags();

  strm << ' ' << value.GetSize() << " octets {\n"
       << hex << setfill('0') << resetiosflags(ios::floatfield)
       << setprecision(indent) << setw(16);

  if (value.GetSize() <= 32 || (flags&ios::floatfield) != ios::fixed)
    strm << value << '\n';
  else {
    PBYTEArray truncatedArray(value, 32);
    strm << truncatedArray << '\n'
         << setfill(' ')
         << setw(indent+4) << "...\n";
  }

  strm << dec << setfill(' ')
       << setw(indent-1) << "}";

  strm.flags(flags);
}


void PASN_OctetString::SetConstraintBounds(ConstraintType type, int lower, unsigned upper)
{
  if (lower < 0)
    return;

  PASN_ConstrainedObject::SetConstraintBounds(type, lower, upper);
  SetSize(GetSize());
}


PString PASN_OctetString::GetTypeAsString() const
{
  return "Octet String";
}


PINDEX PASN_OctetString::GetDataLength() const
{
  return value.GetSize();
}


BOOL PASN_OctetString::SetSize(PINDEX newSize)
{
  if (!CheckByteOffset(newSize, MaximumStringSize))
    return FALSE;

  if (constraint != Unconstrained) {
    if (newSize < (PINDEX)lowerLimit) {
      if (lowerLimit < 0)
        return FALSE;
      newSize = lowerLimit;
    } else if ((unsigned)newSize > upperLimit) {
      if (upperLimit > (unsigned)MaximumStringSize)
        return FALSE;
      newSize = upperLimit;
    }
  }

  return value.SetSize(newSize);
}


BOOL PASN_OctetString::Decode(PASN_Stream & strm)
{
  return strm.OctetStringDecode(*this);
}


void PASN_OctetString::Encode(PASN_Stream & strm) const
{
  strm.OctetStringEncode(*this);
}

///////////////////////////////////////////////////////////////////////

PASN_ConstrainedString::PASN_ConstrainedString(const char * canonical, PINDEX size,
                                               unsigned tag, TagClass tagClass)
  : PASN_ConstrainedObject(tag, tagClass)
{
  canonicalSet = canonical;
  canonicalSetSize = size;
  canonicalSetBits = CountBits(size);
  SetCharacterSet(canonicalSet, canonicalSetSize, Unconstrained);
}


PASN_ConstrainedString & PASN_ConstrainedString::operator=(const char * str)
{
  if (str == NULL)
    str = "";

  PStringStream newValue;

  PINDEX len = strlen(str);

  // Can't copy any more characters than the upper constraint
  if ((unsigned)len > upperLimit)
    len = upperLimit;

  // Now copy individual characters, if they are in character set constraint
  for (PINDEX i = 0; i < len; i++) {
    PINDEX sz = characterSet.GetSize();
    if (sz == 0 || memchr(characterSet, str[i], sz) != NULL)
      newValue << str[i];
  }

  // Make sure string meets minimum length constraint
  while ((int)len < lowerLimit) {
    newValue << characterSet[0];
    len++;
  }

  value = newValue;
  value.MakeMinimumSize();
  return *this;
}


void PASN_ConstrainedString::SetCharacterSet(ConstraintType ctype, const char * set)
{
  SetCharacterSet(set, strlen(set), ctype);
}


void PASN_ConstrainedString::SetCharacterSet(ConstraintType ctype, unsigned firstChar, unsigned lastChar)
{
  char buffer[256];
  for (unsigned i = firstChar; i < lastChar; i++)
    buffer[i] = (char)i;
  SetCharacterSet(buffer, lastChar - firstChar + 1, ctype);
}


void PASN_ConstrainedString::SetCharacterSet(const char * set, PINDEX setSize, ConstraintType ctype)
{
  if (ctype == Unconstrained) {
    characterSet.SetSize(canonicalSetSize);
    memcpy(characterSet.GetPointer(), canonicalSet, canonicalSetSize);
  }
  else if (setSize >= MaximumSetSize ||
           canonicalSetSize >= MaximumSetSize ||
           characterSet.GetSize() >= MaximumSetSize)
    return;
  else {
    characterSet.SetSize(setSize);
    PINDEX count = 0;
    for (PINDEX i = 0; i < canonicalSetSize; i++) {
      if (memchr(set, canonicalSet[i], setSize) != NULL)
        characterSet[count++] = canonicalSet[i];
    }
    if (count < 0)
      return;
    characterSet.SetSize(count);
  }

  charSetUnalignedBits = CountBits(characterSet.GetSize());

  charSetAlignedBits = 1;
  while (charSetUnalignedBits > charSetAlignedBits)
    charSetAlignedBits <<= 1;

  operator=((const char *)value);
}


PObject::Comparison PASN_ConstrainedString::Compare(const PObject & obj) const
{
  PAssert(PIsDescendant(&obj, PASN_ConstrainedString), PInvalidCast);
  const PASN_ConstrainedString & other = (const PASN_ConstrainedString &)obj;
  return value.Compare(other.value);
}


void PASN_ConstrainedString::PrintOn(ostream & strm) const
{
  strm << value.ToLiteral();
}


void PASN_ConstrainedString::SetConstraintBounds(ConstraintType type,
                                                 int lower, unsigned upper)
{
  if (lower < 0)
    return;

  PASN_ConstrainedObject::SetConstraintBounds(type, lower, upper);
  if (constraint != Unconstrained) {
    if (value.GetSize() < (PINDEX)lowerLimit)
      value.SetSize(lowerLimit);
    else if ((unsigned)value.GetSize() > upperLimit)
      value.SetSize(upperLimit);
  }
}


PINDEX PASN_ConstrainedString::GetDataLength() const
{
  return value.GetSize()-1;
}


BOOL PASN_ConstrainedString::Decode(PASN_Stream & strm)
{
  return strm.ConstrainedStringDecode(*this);
}


void PASN_ConstrainedString::Encode(PASN_Stream & strm) const
{
  strm.ConstrainedStringEncode(*this);
}


#define DEFINE_STRING_CLASS(name, set) \
  static const char name##StringSet[] = set; \
  PASN_##name##String::PASN_##name##String(const char * str) \
    : PASN_ConstrainedString(name##StringSet, sizeof(name##StringSet)-1, \
                             Universal##name##String, UniversalTagClass) \
    { PASN_ConstrainedString::SetValue(str); } \
  PASN_##name##String::PASN_##name##String(unsigned tag, TagClass tagClass) \
    : PASN_ConstrainedString(name##StringSet, sizeof(name##StringSet)-1, tag, tagClass) \
    { } \
  PASN_##name##String & PASN_##name##String::operator=(const char * str) \
    { PASN_ConstrainedString::SetValue(str); return *this; } \
  PASN_##name##String & PASN_##name##String::operator=(const PString & str) \
    { PASN_ConstrainedString::SetValue(str); return *this; } \
  PObject * PASN_##name##String::Clone() const \
    { PAssert(IsClass(PASN_##name##String::Class()), PInvalidCast); \
      return new PASN_##name##String(*this); } \
  PString PASN_##name##String::GetTypeAsString() const \
    { return #name " String"; }

DEFINE_STRING_CLASS(Numeric,   " 0123456789")
DEFINE_STRING_CLASS(Printable, " '()+,-./0123456789:=?"
                               "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                               "abcdefghijklmnopqrstuvwxyz")
DEFINE_STRING_CLASS(Visible,   " !\"#$%&'()*+,-./0123456789:;<=>?"
                               "@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_"
                               "`abcdefghijklmnopqrstuvwxyz{|}~")
DEFINE_STRING_CLASS(IA5,       "\000\001\002\003\004\005\006\007"
                               "\010\011\012\013\014\015\016\017"
                               "\020\021\022\023\024\025\026\027"
                               "\030\031\032\033\034\035\036\037"
                               " !\"#$%&'()*+,-./0123456789:;<=>?"
                               "@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_"
                               "`abcdefghijklmnopqrstuvwxyz{|}~\177")
DEFINE_STRING_CLASS(General,   "\000\001\002\003\004\005\006\007"
                               "\010\011\012\013\014\015\016\017"
                               "\020\021\022\023\024\025\026\027"
                               "\030\031\032\033\034\035\036\037"
                               " !\"#$%&'()*+,-./0123456789:;<=>?"
                               "@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_"
                               "`abcdefghijklmnopqrstuvwxyz{|}~\177"
                               "\200\201\202\203\204\205\206\207"
                               "\210\211\212\213\214\215\216\217"
                               "\220\221\222\223\224\225\226\227"
                               "\230\231\232\233\234\235\236\237"
                               "\240\241\242\243\244\245\246\247"
                               "\250\251\252\253\254\255\256\257"
                               "\260\261\262\263\264\265\266\267"
                               "\270\271\272\273\274\275\276\277"
                               "\300\301\302\303\304\305\306\307"
                               "\310\311\312\313\314\315\316\317"
                               "\320\321\322\323\324\325\326\327"
                               "\330\331\332\333\334\335\336\337"
                               "\340\341\342\343\344\345\346\347"
                               "\350\351\352\353\354\355\356\357"
                               "\360\361\362\363\364\365\366\367"
                               "\370\371\372\373\374\375\376\377")


///////////////////////////////////////////////////////////////////////

PASN_BMPString::PASN_BMPString(const char * str)
  : PASN_ConstrainedObject(UniversalBMPString, UniversalTagClass)
{
  Construct();
  if (str != NULL)
    SetValue(str);
}


PASN_BMPString::PASN_BMPString(const PWORDArray & wstr)
  : PASN_ConstrainedObject(UniversalBMPString, UniversalTagClass)
{
  Construct();
  SetValue(wstr);
}


PASN_BMPString::PASN_BMPString(unsigned tag, TagClass tagClass)
  : PASN_ConstrainedObject(tag, tagClass)
{
  Construct();
}


void PASN_BMPString::Construct()
{
  firstChar = 0;
  lastChar = 0xffff;
  charSetAlignedBits = 16;
  charSetUnalignedBits = 16;
}


PASN_BMPString::PASN_BMPString(const PASN_BMPString & other)
  : PASN_ConstrainedObject(other),
    value(other.value, other.value.GetSize()),
    characterSet(other.characterSet)
{
  firstChar = other.firstChar;
  lastChar = other.lastChar;
  charSetAlignedBits = other.charSetAlignedBits;
  charSetUnalignedBits = other.charSetUnalignedBits;
}


PASN_BMPString & PASN_BMPString::operator=(const PASN_BMPString & other)
{
  PASN_ConstrainedObject::operator=(other);

  value = PWORDArray(other.value, other.value.GetSize());
  characterSet = other.characterSet;
  firstChar = other.firstChar;
  lastChar = other.lastChar;
  charSetAlignedBits = other.charSetAlignedBits;
  charSetUnalignedBits = other.charSetUnalignedBits;

  return *this;
}


BOOL PASN_BMPString::IsLegalCharacter(WORD ch)
{
  if (ch < firstChar)
    return FALSE;

  if (ch > lastChar)
    return FALSE;

  if (characterSet.IsEmpty())
    return TRUE;

  const WORD * wptr = characterSet;
  PINDEX count = characterSet.GetSize();
  while (count-- > 0) {
    if (*wptr == ch)
      return TRUE;
    wptr++;
  }

  return FALSE;
}


PASN_BMPString & PASN_BMPString::operator=(const PWORDArray & array)
{
  PINDEX paramSize = array.GetSize();

  // Can't copy any more than the upper constraint
  if ((unsigned)paramSize > upperLimit)
    paramSize = upperLimit;

  // Number of bytes must be at least lhe lower constraint
  PINDEX newSize = (int)paramSize < lowerLimit ? lowerLimit : paramSize;
  value.SetSize(newSize);

  PINDEX count = 0;
  for (PINDEX i = 0; i < paramSize; i++) {
    WORD c = array[i];
    if (IsLegalCharacter(c))
      value[count++] = c;
  }

  // Pad out with the first character till required size
  while (count < newSize)
    value[count++] = firstChar;

  return *this;
}


void PASN_BMPString::SetCharacterSet(ConstraintType ctype, const char * charSet)
{
  PWORDArray array(strlen(charSet));

  PINDEX count = 0;
  while (*charSet != '\0')
    array[count++] = (BYTE)*charSet++;

  SetCharacterSet(ctype, array);
}


void PASN_BMPString::SetCharacterSet(ConstraintType ctype, const PWORDArray & charSet)
{
  if (ctype == Unconstrained) {
    firstChar = 0;
    lastChar = 0xffff;
    characterSet.SetSize(0);
  }
  else {
    characterSet = charSet;

    charSetUnalignedBits = CountBits(lastChar - firstChar + 1);
    if (!charSet.IsEmpty()) {
      unsigned count = 0;
      for (PINDEX i = 0; i < charSet.GetSize(); i++) {
        if (characterSet[i] >= firstChar && characterSet[i] <= lastChar)
          count++;
      }
      count = CountBits(count);
      if (charSetUnalignedBits > count)
        charSetUnalignedBits = count;
    }

    charSetAlignedBits = 1;
    while (charSetUnalignedBits > charSetAlignedBits)
      charSetAlignedBits <<= 1;

    SetValue(value);
  }
}


void PASN_BMPString::SetCharacterSet(ConstraintType ctype, unsigned first, unsigned last)
{
  if (ctype != Unconstrained) {
    PAssert(first < 0x10000 && last < 0x10000 && last > first, PInvalidParameter);
    firstChar = (WORD)first;
    lastChar = (WORD)last;
  }
  SetCharacterSet(ctype, characterSet);
}


PObject * PASN_BMPString::Clone() const
{
  PAssert(IsClass(PASN_BMPString::Class()), PInvalidCast);
  return new PASN_BMPString(*this);
}


PObject::Comparison PASN_BMPString::Compare(const PObject & obj) const
{
  PAssert(PIsDescendant(&obj, PASN_BMPString), PInvalidCast);
  const PASN_BMPString & other = (const PASN_BMPString &)obj;
  return value.Compare(other.value);
}


void PASN_BMPString::PrintOn(ostream & strm) const
{
  int indent = strm.precision() + 2;
  PINDEX sz = value.GetSize();
  strm << ' ' << sz << " characters {\n";
  PINDEX i = 0;
  while (i < sz) {
    strm << setw(indent) << " " << hex << setfill('0');
    PINDEX j;
    for (j = 0; j < 8; j++)
      if (i+j < sz)
        strm << setw(4) << value[i+j] << ' ';
      else
        strm << "     ";
    strm << "  ";
    for (j = 0; j < 8; j++) {
      if (i+j < sz) {
        WORD c = value[i+j];
        if (c < 128 && isprint(c))
          strm << (char)c;
        else
          strm << ' ';
      }
    }
    strm << dec << setfill(' ') << '\n';
    i += 8;
  }
  strm << setw(indent-1) << "}";
}


PString PASN_BMPString::GetTypeAsString() const
{
  return "BMP String";
}


PINDEX PASN_BMPString::GetDataLength() const
{
  return value.GetSize()*2;
}


BOOL PASN_BMPString::Decode(PASN_Stream & strm)
{
  return strm.BMPStringDecode(*this);
}


void PASN_BMPString::Encode(PASN_Stream & strm) const
{
  strm.BMPStringEncode(*this);
}


///////////////////////////////////////////////////////////////////////

PASN_GeneralisedTime & PASN_GeneralisedTime::operator=(const PTime & time)
{
  value = time.AsString("yyyyMMddhhmmss.uz");
  value.Replace("GMT", "Z");
  return *this;
}


PTime PASN_GeneralisedTime::GetValue() const
{
  int year = value(0,3).AsInteger();
  int month = value(4,5).AsInteger();
  int day = value(6,7).AsInteger();
  int hour = value(8,9).AsInteger();
  int minute = value(10,11).AsInteger();
  int seconds = 0;
  int zonePos = 12;

  if (isdigit(value[12])) {
    seconds = value(12,13).AsInteger();
    if (value[14] != '.')
      zonePos = 14;
    else {
      zonePos = 15;
      while (isdigit(value[zonePos]))
        zonePos++;
    }
  }

  int zone = PTime::Local;
  switch (value[zonePos]) {
    case 'Z' :
      zone = PTime::UTC;
      break;
    case '+' :
    case '-' :
      zone = value(zonePos+1,zonePos+2).AsInteger()*60 +
             value(zonePos+3,zonePos+4).AsInteger();
  }

  return PTime(seconds, minute, hour, day, month, year, zone);
}


///////////////////////////////////////////////////////////////////////

PASN_UniversalTime & PASN_UniversalTime::operator=(const PTime & time)
{
  value = time.AsString("yyMMddhhmmssz");
  value.Replace("GMT", "Z");
  value.MakeMinimumSize();
  return *this;
}


PTime PASN_UniversalTime::GetValue() const
{
  int year = value(0,1).AsInteger();
  if (year < 36)
    year += 2000;
  else
    year += 1900;

  int month = value(2,3).AsInteger();
  int day = value(4,5).AsInteger();
  int hour = value(6,7).AsInteger();
  int minute = value(8,9).AsInteger();
  int seconds = 0;
  int zonePos = 10;

  if (isdigit(value[10])) {
    seconds = value(10,11).AsInteger();
    zonePos = 12;
  }

  int zone = PTime::UTC;
  if (value[zonePos] != 'Z')
    zone = value(zonePos+1,zonePos+2).AsInteger()*60 +
           value(zonePos+3,zonePos+4).AsInteger();

  return PTime(seconds, minute, hour, day, month, year, zone);
}


///////////////////////////////////////////////////////////////////////

PASN_Choice::PASN_Choice(unsigned nChoices, BOOL extend)
  : PASN_Object(0, ApplicationTagClass, extend),names(NULL),namesCount(0)
{
  numChoices = nChoices;
  choice = NULL;
}


PASN_Choice::PASN_Choice(unsigned tag, TagClass tagClass,
                         unsigned upper, BOOL extend)
  : PASN_Object(tag, tagClass, extend),names(NULL),namesCount(0)
{
  numChoices = upper;
  choice = NULL;
}


PASN_Choice::PASN_Choice(unsigned tag, TagClass tagClass,
                         unsigned upper, BOOL extend, const PASN_Names * nameSpec,unsigned namesCnt)
  : PASN_Object(tag, tagClass, extend),
    names(nameSpec),namesCount(namesCnt)
{
  numChoices = upper;
  choice = NULL;
}


PASN_Choice::PASN_Choice(const PASN_Choice & other)
  : PASN_Object(other),
  names(other.names),namesCount(other.namesCount)
{
  numChoices = other.numChoices;

  if (other.CheckCreate())
    choice = (PASN_Object *)other.choice->Clone();
  else
    choice = NULL;
}


PASN_Choice & PASN_Choice::operator=(const PASN_Choice & other)
{
  if (&other == this) // Assigning to ourself, just do nothing.
    return *this;

  delete choice;

  PASN_Object::operator=(other);

  numChoices = other.numChoices;
  names = other.names;
  namesCount = other.namesCount;

  if (other.CheckCreate())
    choice = (PASN_Object *)other.choice->Clone();
  else
    choice = NULL;

  return *this;
}


PASN_Choice::~PASN_Choice()
{
  delete choice;
}


void PASN_Choice::SetTag(unsigned newTag, TagClass tagClass)
{
  PASN_Object::SetTag(newTag, tagClass);

  delete choice;

  if (CreateObject())
    choice->SetTag(newTag, tagClass);
}


PString PASN_Choice::GetTagName() const
{
  PINDEX idx = FindNameByValue(names, namesCount, tag);
  if (idx != P_MAX_INDEX)
    return names[idx].name;

  if (CheckCreate() &&
      PIsDescendant(choice, PASN_Choice) &&
      choice->GetTag() == tag &&
      choice->GetTagClass() == tagClass)
    return PString(choice->GetClass()) + "->" + ((PASN_Choice *)choice)->GetTagName();

  return psprintf("<%u>", tag);
}


BOOL PASN_Choice::CheckCreate() const
{
  if (choice != NULL)
    return TRUE;

  return ((PASN_Choice *)this)->CreateObject();
}


PASN_Object & PASN_Choice::GetObject() const
{
  PAssert(CheckCreate(), "NULL Choice");
  return *choice;
}


#if defined(__GNUC__) && __GNUC__ <= 2 && __GNUC_MINOR__ < 9

#define CHOICE_CAST_OPERATOR(cls) \
  PASN_Choice::operator cls &() const \
  { \
    PAssert(CheckCreate(), "Cast of NULL choice"); \
    PAssert(choice->IsDescendant(cls::Class()), PInvalidCast); \
    return *(cls *)choice; \
  } \

#else

#define CHOICE_CAST_OPERATOR(cls) \
  PASN_Choice::operator cls &() \
  { \
    PAssert(CheckCreate(), "Cast of NULL choice"); \
    PAssert(PIsDescendant(choice, cls), PInvalidCast); \
    return *(cls *)choice; \
  } \
  PASN_Choice::operator const cls &() const \
  { \
    PAssert(CheckCreate(), "Cast of NULL choice"); \
    PAssert(PIsDescendant(choice, cls), PInvalidCast); \
    return *(const cls *)choice; \
  } \

#endif


CHOICE_CAST_OPERATOR(PASN_Null)
CHOICE_CAST_OPERATOR(PASN_Boolean)
CHOICE_CAST_OPERATOR(PASN_Integer)
CHOICE_CAST_OPERATOR(PASN_Enumeration)
CHOICE_CAST_OPERATOR(PASN_Real)
CHOICE_CAST_OPERATOR(PASN_ObjectId)
CHOICE_CAST_OPERATOR(PASN_BitString)
CHOICE_CAST_OPERATOR(PASN_OctetString)
CHOICE_CAST_OPERATOR(PASN_NumericString)
CHOICE_CAST_OPERATOR(PASN_PrintableString)
CHOICE_CAST_OPERATOR(PASN_VisibleString)
CHOICE_CAST_OPERATOR(PASN_IA5String)
CHOICE_CAST_OPERATOR(PASN_GeneralString)
CHOICE_CAST_OPERATOR(PASN_BMPString)
CHOICE_CAST_OPERATOR(PASN_Sequence)


PObject::Comparison PASN_Choice::Compare(const PObject & obj) const
{
  PAssert(PIsDescendant(&obj, PASN_Choice), PInvalidCast);
  const PASN_Choice & other = (const PASN_Choice &)obj;

  CheckCreate();
  other.CheckCreate();

  if (choice == other.choice)
    return EqualTo;

  if (choice == NULL)
    return LessThan;

  if (other.choice == NULL)
    return GreaterThan;

  if (tag < other.tag)
    return LessThan;

  if (tag > other.tag)
    return GreaterThan;

  return choice->Compare(*other.choice);
}


void PASN_Choice::PrintOn(ostream & strm) const
{
  strm << GetTagName();

  if (choice != NULL)
    strm << ' ' << *choice;
  else
    strm << " (NULL)";
}


PString PASN_Choice::GetTypeAsString() const
{
  return "Choice";
}


PINDEX PASN_Choice::GetDataLength() const
{
  if (CheckCreate())
    return choice->GetDataLength();

  return 0;
}


BOOL PASN_Choice::IsPrimitive() const
{
  if (CheckCreate())
    return choice->IsPrimitive();
  return FALSE;
}


BOOL PASN_Choice::Decode(PASN_Stream & strm)
{
  return strm.ChoiceDecode(*this);
}


void PASN_Choice::Encode(PASN_Stream & strm) const
{
  strm.ChoiceEncode(*this);
}


PINDEX PASN_Choice::GetValueByName(PString name) const
{
  for(unsigned uiIndex = 0; uiIndex < numChoices; uiIndex++){
    if(strcmp(names[uiIndex].name, name) == 0){
      return names[uiIndex].value;
    }
  }
  return UINT_MAX;
}
///////////////////////////////////////////////////////////////////////

PASN_Sequence::PASN_Sequence(unsigned tag, TagClass tagClass,
                             unsigned nOpts, BOOL extend, unsigned nExtend)
  : PASN_Object(tag, tagClass, extend)
{
  optionMap.SetConstraints(PASN_ConstrainedObject::FixedConstraint, nOpts);
  knownExtensions = nExtend;
  totalExtensions = 0;
  endBasicEncoding = 0;
}


PASN_Sequence::PASN_Sequence(const PASN_Sequence & other)
  : PASN_Object(other),
    fields(other.fields.GetSize()),
    optionMap(other.optionMap),
    extensionMap(other.extensionMap)
{
  for (PINDEX i = 0; i < other.fields.GetSize(); i++)
    fields.SetAt(i, other.fields[i].Clone());

  knownExtensions = other.knownExtensions;
  totalExtensions = other.totalExtensions;
  endBasicEncoding = 0;
}


PASN_Sequence & PASN_Sequence::operator=(const PASN_Sequence & other)
{
  PASN_Object::operator=(other);

  fields.SetSize(other.fields.GetSize());
  for (PINDEX i = 0; i < other.fields.GetSize(); i++)
    fields.SetAt(i, other.fields[i].Clone());

  optionMap = other.optionMap;
  knownExtensions = other.knownExtensions;
  totalExtensions = other.totalExtensions;
  extensionMap = other.extensionMap;

  return *this;
}


BOOL PASN_Sequence::HasOptionalField(PINDEX opt) const
{
  if (opt < (PINDEX)optionMap.GetSize())
    return optionMap[opt];
  else
    return extensionMap[opt - optionMap.GetSize()];
}


void PASN_Sequence::IncludeOptionalField(PINDEX opt)
{
  if (opt < (PINDEX)optionMap.GetSize())
    optionMap.Set(opt);
  else {
    PAssert(extendable, "Must be extendable type");
    opt -= optionMap.GetSize();
    if (opt >= (PINDEX)extensionMap.GetSize())
      extensionMap.SetSize(opt+1);
    extensionMap.Set(opt);
  }
}


void PASN_Sequence::RemoveOptionalField(PINDEX opt)
{
  if (opt < (PINDEX)optionMap.GetSize())
    optionMap.Clear(opt);
  else {
    PAssert(extendable, "Must be extendable type");
    opt -= optionMap.GetSize();
    extensionMap.Clear(opt);
  }
}


PObject::Comparison PASN_Sequence::Compare(const PObject & obj) const
{
  PAssert(PIsDescendant(&obj, PASN_Sequence), PInvalidCast);
  const PASN_Sequence & other = (const PASN_Sequence &)obj;
  return fields.Compare(other.fields);
}


PObject * PASN_Sequence::Clone() const
{
  PAssert(IsClass(PASN_Sequence::Class()), PInvalidCast);
  return new PASN_Sequence(*this);
}


void PASN_Sequence::PrintOn(ostream & strm) const
{
  int indent = strm.precision() + 2;
  strm << "{\n";
  for (PINDEX i = 0; i < fields.GetSize(); i++) {
    strm << setw(indent+6) << "field[" << i << "] <";
    switch (fields[i].GetTagClass()) {
      case UniversalTagClass :
        strm << "Universal";
        break;
      case ApplicationTagClass :
        strm << "Application";
        break;
      case ContextSpecificTagClass :
        strm << "ContextSpecific";
        break;
      case PrivateTagClass :
        strm << "Private";
      default :
        break;
    }
    strm << '-' << fields[i].GetTag() << '-'
         << fields[i].GetTypeAsString() << "> = "
         << fields[i] << '\n';
  }
  strm << setw(indent-1) << "}";
}


PString PASN_Sequence::GetTypeAsString() const
{
  return "Sequence";
}


PINDEX PASN_Sequence::GetDataLength() const
{
  PINDEX len = 0;
  for (PINDEX i = 0; i < fields.GetSize(); i++)
    len += fields[i].GetObjectLength();
  return len;
}


BOOL PASN_Sequence::IsPrimitive() const
{
  return FALSE;
}


BOOL PASN_Sequence::Decode(PASN_Stream & strm)
{
  return PreambleDecode(strm) && UnknownExtensionsDecode(strm);
}


void PASN_Sequence::Encode(PASN_Stream & strm) const
{
  PreambleEncode(strm);
  UnknownExtensionsEncode(strm);
}


BOOL PASN_Sequence::PreambleDecode(PASN_Stream & strm)
{
  return strm.SequencePreambleDecode(*this);
}


void PASN_Sequence::PreambleEncode(PASN_Stream & strm) const
{
  strm.SequencePreambleEncode(*this);
}


BOOL PASN_Sequence::KnownExtensionDecode(PASN_Stream & strm, PINDEX fld, PASN_Object & field)
{
  return strm.SequenceKnownDecode(*this, fld, field);
}


void PASN_Sequence::KnownExtensionEncode(PASN_Stream & strm, PINDEX fld, const PASN_Object & field) const
{
  strm.SequenceKnownEncode(*this, fld, field);
}


BOOL PASN_Sequence::UnknownExtensionsDecode(PASN_Stream & strm)
{
  return strm.SequenceUnknownDecode(*this);
}


void PASN_Sequence::UnknownExtensionsEncode(PASN_Stream & strm) const
{
  strm.SequenceUnknownEncode(*this);
}


///////////////////////////////////////////////////////////////////////

PASN_Set::PASN_Set(unsigned tag, TagClass tagClass,
                   unsigned nOpts, BOOL extend, unsigned nExtend)
  : PASN_Sequence(tag, tagClass, nOpts, extend, nExtend)
{
}


PObject * PASN_Set::Clone() const
{
  PAssert(IsClass(PASN_Set::Class()), PInvalidCast);
  return new PASN_Set(*this);
}


PString PASN_Set::GetTypeAsString() const
{
  return "Set";
}

///////////////////////////////////////////////////////////////////////

PASN_Array::PASN_Array(unsigned tag, TagClass tagClass)
  : PASN_ConstrainedObject(tag, tagClass)
{
}


PASN_Array::PASN_Array(const PASN_Array & other)
  : PASN_ConstrainedObject(other),
    array(other.array.GetSize())
{
  for (PINDEX i = 0; i < other.array.GetSize(); i++)
    array.SetAt(i, other.array[i].Clone());
}


PASN_Array & PASN_Array::operator=(const PASN_Array & other)
{
  PASN_ConstrainedObject::operator=(other);

  array.SetSize(other.array.GetSize());
  for (PINDEX i = 0; i < other.array.GetSize(); i++)
    array.SetAt(i, other.array[i].Clone());

  return *this;
}


BOOL PASN_Array::SetSize(PINDEX newSize)
{
  if (newSize > MaximumArraySize)
    return FALSE;

  PINDEX originalSize = array.GetSize();
  if (!array.SetSize(newSize))
    return FALSE;

  for (PINDEX i = originalSize; i < newSize; i++) {
    PASN_Object * obj = CreateObject();
    if (obj == NULL)
      return FALSE;

    array.SetAt(i, obj);
  }

  return TRUE;
}


PObject::Comparison PASN_Array::Compare(const PObject & obj) const
{
  PAssert(PIsDescendant(&obj, PASN_Array), PInvalidCast);
  const PASN_Array & other = (const PASN_Array &)obj;
  return array.Compare(other.array);
}


void PASN_Array::PrintOn(ostream & strm) const
{
  int indent = strm.precision() + 2;
  strm << array.GetSize() << " entries {\n";
  for (PINDEX i = 0; i < array.GetSize(); i++)
    strm << setw(indent+1) << "[" << i << "]=" << setprecision(indent) << array[i] << '\n';
  strm << setw(indent-1) << "}";
}


void PASN_Array::SetConstraintBounds(ConstraintType type, int lower, unsigned upper)
{
  if (lower < 0)
    return;

  PASN_ConstrainedObject::SetConstraintBounds(type, lower, upper);
  if (constraint != Unconstrained) {
    if (GetSize() < (PINDEX)lowerLimit)
      SetSize(lowerLimit);
    else if (GetSize() > (PINDEX)upperLimit)
      SetSize(upperLimit);
  }
}


PString PASN_Array::GetTypeAsString() const
{
  return "Array";
}


PINDEX PASN_Array::GetDataLength() const
{
  PINDEX len = 0;
  for (PINDEX i = 0; i < array.GetSize(); i++)
    len += array[i].GetObjectLength();
  return len;
}


BOOL PASN_Array::IsPrimitive() const
{
  return FALSE;
}


BOOL PASN_Array::Decode(PASN_Stream & strm)
{
  return strm.ArrayDecode(*this);
}


void PASN_Array::Encode(PASN_Stream & strm) const
{
  strm.ArrayEncode(*this);
}


///////////////////////////////////////////////////////////////////////

PASN_Stream::PASN_Stream()
{
  Construct();
}


PASN_Stream::PASN_Stream(const PBYTEArray & bytes)
  : PBYTEArray(bytes)
{
  Construct();
}


PASN_Stream::PASN_Stream(const BYTE * buf, PINDEX size)
  : PBYTEArray(buf, size)
{
  Construct();
}


void PASN_Stream::Construct()
{
  byteOffset = 0;
  bitOffset = 8;
}


void PASN_Stream::PrintOn(ostream & strm) const
{
  int indent = strm.precision() + 2;
  strm << " size=" << GetSize()
       << " pos=" << byteOffset << '.' << (8-bitOffset)
       << " {\n";
  PINDEX i = 0;
  while (i < GetSize()) {
    strm << setw(indent) << " " << hex << setfill('0');
    PINDEX j;
    for (j = 0; j < 16; j++)
      if (i+j < GetSize())
        strm << setw(2) << (unsigned)(BYTE)theArray[i+j] << ' ';
      else
        strm << "   ";
    strm << "  ";
    for (j = 0; j < 16; j++) {
      if (i+j < GetSize()) {
        BYTE c = theArray[i+j];
        if (c < 128 && isprint(c))
          strm << c;
        else
          strm << ' ';
      }
    }
    strm << dec << setfill(' ') << '\n';
    i += 16;
  }
  strm << setw(indent-1) << "}";
}


void PASN_Stream::SetPosition(PINDEX newPos)
{
  if (!CheckByteOffset(byteOffset))
    return;

  if (newPos > GetSize())
    byteOffset = GetSize();
  else
    byteOffset = newPos;
  bitOffset = 8;
}


void PASN_Stream::ResetDecoder()
{
  byteOffset = 0;
  bitOffset = 8;
}


void PASN_Stream::BeginEncoding()
{
  bitOffset = 8;
  byteOffset = 0;
  PBYTEArray::operator=(PBYTEArray(20));
}


void PASN_Stream::CompleteEncoding()
{
  if (byteOffset != P_MAX_INDEX) {
    if (bitOffset != 8) {
      bitOffset = 8;
      byteOffset++;
    }
    SetSize(byteOffset);
    byteOffset = P_MAX_INDEX;
  }
}


BYTE PASN_Stream::ByteDecode()
{
  if (!CheckByteOffset(byteOffset, GetSize()))
    return 0;

  bitOffset = 8;
  return theArray[byteOffset++];
}


void PASN_Stream::ByteEncode(unsigned value)
{
  if (!CheckByteOffset(byteOffset))
    return;

  if (bitOffset != 8) {
    bitOffset = 8;
    byteOffset++;
  }
  if (byteOffset >= GetSize())
    SetSize(byteOffset+10);
  theArray[byteOffset++] = (BYTE)value;
}


unsigned PASN_Stream::BlockDecode(BYTE * bufptr, unsigned nBytes)
{
  if (nBytes == 0 || bufptr == NULL || !CheckByteOffset(byteOffset+nBytes))
    return 0;

  ByteAlign();

  if (byteOffset+nBytes > (unsigned)GetSize()) {
    nBytes = GetSize() - byteOffset;
    if (nBytes <= 0)
      return 0;
  }

  memcpy(bufptr, &theArray[byteOffset], nBytes);
  byteOffset += nBytes;
  return nBytes;
}


void PASN_Stream::BlockEncode(const BYTE * bufptr, PINDEX nBytes)
{
  if (!CheckByteOffset(byteOffset, GetSize()))
    return;

  if (nBytes == 0)
    return;

  ByteAlign();

  if (byteOffset+nBytes >= GetSize())
    SetSize(byteOffset+nBytes+10);

  memcpy(theArray+byteOffset, bufptr, nBytes);
  byteOffset += nBytes;
}


void PASN_Stream::ByteAlign()
{
  if (!CheckByteOffset(byteOffset, GetSize()))
    return;

  if (bitOffset != 8) {
    bitOffset = 8;
    byteOffset++;
  }
}


///////////////////////////////////////////////////////////////////////

#ifdef  P_INCLUDE_PER
#include "asnper.cxx"
#endif

#ifdef  P_INCLUDE_BER
#include "asnber.cxx"
#endif

#ifdef  P_INCLUDE_XER
#include "asnxer.cxx"
#endif

// End of file ////////////////////////////////////////////////////////////////
