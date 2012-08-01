/*
 * contain.inl
 *
 * Container Class Inline Function Definitions
 *
 * Portable Windows Library
 *
 * Copyright (c) 1993-1998 Equivalence Pty. Ltd.
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
 * Portions are Copyright (C) 1993 Free Software Foundation, Inc.
 * All Rights Reserved.
 *
 * Contributor(s): ______________________________________.
 *
 * $Log: contain.inl,v $
 * Revision 1.56  2007/08/01 05:20:48  rjongbloed
 * Changes to container classes to become compatible with advanced DevStudio 2005 "Visualizers".
 *
 * Revision 1.55  2007/06/29 02:47:28  rjongbloed
 * Added PString::FindSpan() function (strspn equivalent) with slightly nicer semantics.
 *
 * Revision 1.54  2004/06/01 05:21:38  csoutheren
 * Added conversions between std::string and PString, and vector<PString> and PStringArray
 *
 * Revision 1.53  2004/05/04 11:10:36  rjongbloed
 * Fixed usage of MakeEmpty() with PStringStream.
 *
 * Revision 1.52  2004/04/24 06:27:56  rjongbloed
 * Fixed GCC 3.4.0 warnings about PAssertNULL and improved recoverability on
 *   NULL pointer usage in various bits of code.
 *
 * Revision 1.51  2004/04/18 04:33:36  rjongbloed
 * Changed all operators that return BOOL to return standard type bool. This is primarily
 *   for improved compatibility with std STL usage removing many warnings.
 *
 * Revision 1.50  2004/04/09 03:42:34  csoutheren
 * Removed all usages of "virtual inline" and "inline virtual"
 *
 * Revision 1.49  2004/01/18 13:43:48  rjongbloed
 * Fixed broken PString::MakeEmpty() function and moved implementations to .inl file.
 *
 * Revision 1.48  2002/10/31 05:52:37  robertj
 * Now comprehensively stated that a PString is ALWAYS an 8 bit string as
 *   there are far too many inheerent assumptions every to make it 16 bit.
 *
 * Revision 1.47  2002/10/04 01:47:44  robertj
 * Added various increment and decrement operators to POrdinalKey.
 *
 * Revision 1.46  2002/08/14 00:43:39  robertj
 * Added ability to have fixed maximum length PStringStream's so does not do
 *   unwanted malloc()'s while outputing data.
 *
 * Revision 1.45  2002/02/15 04:29:49  robertj
 * Added PString::Empty() to return the primordial empty string. Saves on a
 *   couple of memory allocations for every empty string ever used.
 *
 * Revision 1.44  2002/01/22 01:03:57  craigs
 * Added operator += and operator + functions to PStringArray and PStringList
 * Added AppendString operator to PStringArray
 *
 * Revision 1.43  2001/02/13 04:39:08  robertj
 * Fixed problem with operator= in container classes. Some containers will
 *   break unless the copy is virtual (eg PStringStream's buffer pointers) so
 *   needed to add a new AssignContents() function to all containers.
 *
 * Revision 1.42  1999/08/22 12:13:42  robertj
 * Fixed warning when using inlines on older GNU compiler
 *
 * Revision 1.41  1999/03/09 03:08:39  robertj
 * Changes for new documentation system
 *
 * Revision 1.40  1998/09/23 06:20:25  robertj
 * Added open source copyright license.
 *
 * Revision 1.39  1997/12/11 10:27:15  robertj
 * Added type correct Contains() function to dictionaries.
 *
 * Revision 1.38  1997/02/14 13:53:58  robertj
 * Major rewrite of sorted list to use sentinel record instead of NULL pointers.
 *
 * Revision 1.37  1996/09/14 12:54:18  robertj
 * Added operator! for !IsEmpty().
 *
 * Revision 1.36  1996/07/15 10:32:49  robertj
 * Fixed bug in sorted list (crash on remove).
 *
 * Revision 1.35  1996/02/08 11:47:57  robertj
 * Moved Contains function from PSet to PHashTable so available for dictionaries.
 * Added caseless compare operator and spaced concatenation operator.
 *
 * Revision 1.34  1996/01/23 13:10:45  robertj
 * String searching algorithm rewrite.
 * Added Replace() function to strings.
 *
 * Revision 1.33  1995/12/23 03:48:40  robertj
 * Added operators for string set include and exclude.
 *
 * Revision 1.32  1995/08/24 12:34:09  robertj
 * Added assert for list index out of bounds.
 *
 * Revision 1.31  1995/03/25 02:12:16  robertj
 * Fixed PStringXXX containers so can correctly contain PCaselessString etc.
 *
 * Revision 1.30  1995/03/12 04:38:01  robertj
 * Added assignment operator for const char * for efficiency.
 *
 * Revision 1.29  1995/01/09  12:36:28  robertj
 * Changes due to Mac port.
 *
 * Revision 1.28  1994/12/12  13:13:12  robertj
 * Fixed bugs in PString mods just made.
 *
 * Revision 1.27  1994/12/12  10:16:20  robertj
 * Restructuring and documentation of container classes.
 * Renaming of some macros for declaring container classes.
 * Added some extra functionality to PString.
 * Added start to 2 byte characters in PString.
 * Fixed incorrect overrides in PCaselessString.
 *
 * Revision 1.26  1994/12/05  11:19:09  robertj
 * Moved SetMinSize from PAbstractArray to PContainer.
 *
 * Revision 1.25  1994/11/28  12:33:46  robertj
 * Added dummy parameter for cls* constructor in containers. This prevents some very
 * strange an undesirable default construction of clones.
 *
 * Revision 1.24  1994/10/30  11:50:27  robertj
 * Split into Object classes and Container classes.
 * Changed mechanism for doing notification callback functions.
 *
 * Revision 1.23  1994/10/23  04:41:45  robertj
 * Added implemtation for PString constructor used by Clone().
 * Added PStringDictionary function.
 *
 * Revision 1.22  1994/07/27  05:58:07  robertj
 * Synchronisation.
 *
 * Revision 1.21  1994/07/25  03:31:00  robertj
 * Fixed missing PINLINEs.
 *
 * Revision 1.20  1994/07/17  10:46:06  robertj
 * Added string container functions for searching.
 *
 * Revision 1.19  1994/07/02  03:03:49  robertj
 * Addition of container searching facilities.
 *
 * Revision 1.18  1994/06/25  11:55:15  robertj
 * Unix version synchronisation.
 *
 * Revision 1.17  1994/04/20  12:17:44  robertj
 * assert stuff
 *
 * Revision 1.16  1994/04/01  14:05:46  robertj
 * Added PString specific containers.
 *
 * Revision 1.15  1994/03/07  07:45:40  robertj
 * Major upgrade
 *
 * Revision 1.14  1994/01/15  02:48:55  robertj
 * Rearranged PString assignment operator for NT portability.
 *
 * Revision 1.13  1994/01/13  08:42:29  robertj
 * Fixed missing copy constuctor and assignment operator for PString.
 *
 * Revision 1.12  1994/01/13  05:33:41  robertj
 * Added contructor to get caseless string from ordinary string.
 *
 * Revision 1.11  1994/01/03  04:42:23  robertj
 * Mass changes to common container classes and interactors etc etc etc.
 *
 * Revision 1.10  1993/12/31  06:48:46  robertj
 * Made inlines optional for debugging purposes.
 * Added PImgIcon class.
 *
 * Revision 1.9  1993/12/24  04:20:52  robertj
 * Mac CFront port.
 *
 * Revision 1.8  1993/12/22  05:54:08  robertj
 * Checked for severe out of memory condition in containers.
 *
 * Revision 1.7  1993/12/16  00:51:46  robertj
 * Made some container functions const.
 *
 * Revision 1.6  1993/12/15  21:10:10  robertj
 * Fixed reference system used by container classes.
 *
 * Revision 1.5  1993/08/27  18:17:47  robertj
 * Fixed bugs in PSortedList default size.
 *
 * Revision 1.4  1993/07/16  14:40:55  robertj
 * Added PString constructor for individual characters.
 * Added string to C style literal format.
 *
 * Revision 1.3  1993/07/14  12:49:16  robertj
 * Fixed RCS keywords.
 *
 */


///////////////////////////////////////////////////////////////////////////////

PINLINE PContainer & PContainer::operator=(const PContainer & cont)
  { AssignContents(cont); return *this; }

PINLINE void PContainer::CloneContents(const PContainer *)
  { }

PINLINE void PContainer::CopyContents(const PContainer &)
  { }

PINLINE PINDEX PContainer::GetSize() const
  { return PAssertNULL(reference)->size; }

PINLINE BOOL PContainer::IsEmpty() const
  { return GetSize() == 0; }

PINLINE BOOL PContainer::IsUnique() const
  { return PAssertNULL(reference)->count <= 1; }


///////////////////////////////////////////////////////////////////////////////

PINLINE PString::PString()
  : PCharArray(1) { }

PINLINE PString::PString(const PString & str)
  : PCharArray(str) { }

PINLINE PString::PString(int, const PString * str)
  : PCharArray(*str) { }

PINLINE PString::PString(const std::string & str)
  : PCharArray(PString(str.c_str())) { }

PINLINE PString::PString(char c)
  : PCharArray(2) { SetAt(0, c); }

PINLINE PString PString::Empty()
  { return PString(); }

PINLINE PString & PString::operator=(const PString & str)
  { AssignContents(str); return *this; }

PINLINE PString & PString::operator=(const char * cstr)
  { AssignContents(PString(cstr)); return *this; }

PINLINE PString & PString::operator=(char ch)
  { AssignContents(PString(ch)); return *this; }

PINLINE BOOL PString::MakeMinimumSize()
  { return SetSize(GetLength()+1); }

PINLINE PINDEX PString::GetLength() const
  { return strlen(theArray); }

PINLINE bool PString::operator!() const
  { return !IsEmpty(); }

PINLINE PString PString::operator+(const PString & str) const
  { return operator+((const char *)str); }

PINLINE PString operator+(const char * cstr, const PString & str)
  { return PString(cstr) + str; }
  
PINLINE PString operator+(char c, const PString & str)
  { return PString(c) + str; }
  
PINLINE PString & PString::operator+=(const PString & str)
  { return operator+=((const char *)str); }

PINLINE PString PString::operator&(const PString & str) const
  { return operator&((const char *)str); }

PINLINE PString operator&(const char * cstr, const PString & str)
  { return PString(cstr) & str; }
  
PINLINE PString operator&(char c, const PString & str)
  { return PString(c) & str; }
  
PINLINE PString & PString::operator&=(const PString & str)
  { return operator&=((const char *)str); }

PINLINE bool PString::operator==(const PObject & obj) const
  { return PObject::operator==(obj); }

PINLINE bool PString::operator!=(const PObject & obj) const
  { return PObject::operator!=(obj); }

PINLINE bool PString::operator<(const PObject & obj) const
  { return PObject::operator<(obj); }

PINLINE bool PString::operator>(const PObject & obj) const
  { return PObject::operator>(obj); }

PINLINE bool PString::operator<=(const PObject & obj) const
  { return PObject::operator<=(obj); }

PINLINE bool PString::operator>=(const PObject & obj) const
  { return PObject::operator>=(obj); }

PINLINE bool PString::operator*=(const PString & str) const
  { return operator*=((const char *)str); }

PINLINE bool PString::operator==(const char * cstr) const
  { return InternalCompare(0, P_MAX_INDEX, cstr) == EqualTo; }

PINLINE bool PString::operator!=(const char * cstr) const
  { return InternalCompare(0, P_MAX_INDEX, cstr) != EqualTo; }

PINLINE bool PString::operator<(const char * cstr) const
  { return InternalCompare(0, P_MAX_INDEX, cstr) == LessThan; }

PINLINE bool PString::operator>(const char * cstr) const
  { return InternalCompare(0, P_MAX_INDEX, cstr) == GreaterThan; }

PINLINE bool PString::operator<=(const char * cstr) const
  { return InternalCompare(0, P_MAX_INDEX, cstr) != GreaterThan; }

PINLINE bool PString::operator>=(const char * cstr) const
  { return InternalCompare(0, P_MAX_INDEX, cstr) != LessThan; }

PINLINE PINDEX PString::Find(const PString & str, PINDEX offset) const
  { return Find((const char *)str, offset); }

PINLINE PINDEX PString::FindLast(const PString & str, PINDEX offset) const
  { return FindLast((const char *)str, offset); }

PINLINE PINDEX PString::FindOneOf(const PString & str, PINDEX offset) const
  { return FindOneOf((const char *)str, offset); }

PINLINE PINDEX PString::FindSpan(const PString & str, PINDEX offset) const
  { return FindSpan((const char *)str, offset); }

PINLINE void PString::Splice(const PString & str, PINDEX pos, PINDEX len)
  { Splice((const char *)str, pos, len); }

PINLINE PStringArray
      PString::Tokenise(const PString & separators, BOOL onePerSeparator) const
  { return Tokenise((const char *)separators, onePerSeparator); }

PINLINE PString::operator const unsigned char *() const
  { return (const unsigned char *)theArray; }

PINLINE PString & PString::vsprintf(const PString & fmt, va_list args)
  { return vsprintf((const char *)fmt, args); }

PINLINE PString pvsprintf(const PString & fmt, va_list args)
  { return pvsprintf((const char *)fmt, args); }


///////////////////////////////////////////////////////////////////////////////

PINLINE PCaselessString::PCaselessString()
  : PString() { }

PINLINE PCaselessString::PCaselessString(const char * cstr)
  : PString(cstr) { }

PINLINE PCaselessString::PCaselessString(const PString & str)
  : PString(str) { }

PINLINE PCaselessString::PCaselessString(int dummy,const PCaselessString * str)
  : PString(dummy, str) { }

PINLINE PCaselessString & PCaselessString::operator=(const PString & str)
  { AssignContents(str); return *this; }

PINLINE PCaselessString & PCaselessString::operator=(const char * cstr)
  { AssignContents(PString(cstr)); return *this; }

PINLINE PCaselessString & PCaselessString::operator=(char ch)
  { AssignContents(PString(ch)); return *this; }


///////////////////////////////////////////////////////////////////////////////

PINLINE PStringStream::Buffer::Buffer(const Buffer & b)
  : string(b.string) { }

PINLINE PStringStream::Buffer& PStringStream::Buffer::operator=(const Buffer&b)
  { string = b.string; return *this; }

PINLINE PStringStream & PStringStream::operator=(const PStringStream & strm)
  { AssignContents(strm); return *this; }

PINLINE PStringStream & PStringStream::operator=(const PString & str)
  { AssignContents(str); return *this; }

PINLINE PStringStream & PStringStream::operator=(const char * cstr)
  { AssignContents(PString(cstr)); return *this; }

PINLINE PStringStream & PStringStream::operator=(char ch)
  { AssignContents(PString(ch)); return *this; }


///////////////////////////////////////////////////////////////////////////////

PINLINE PCollection::PCollection(PINDEX initialSize)
  : PContainer(initialSize) { }

PINLINE PCollection::PCollection(int dummy, const PCollection * c)
  : PContainer(dummy, c) { }

PINLINE void PCollection::AllowDeleteObjects(BOOL yes)
  { reference->deleteObjects = yes; }

PINLINE void PCollection::DisallowDeleteObjects()
  { AllowDeleteObjects(FALSE); }


///////////////////////////////////////////////////////////////////////////////

PINLINE PArrayObjects::PArrayObjects(PINDEX initialSize)
  : theArray(PNEW ObjPtrArray(initialSize)) { }


///////////////////////////////////////////////////////////////////////////////

PINLINE PINDEX PStringArray::AppendString(const PString & str)
  { return Append(str.Clone()); }

PINLINE PStringArray & PStringArray::operator += (const PString & str)
  { Append(str.Clone()); return *this; }

PINLINE PStringArray PStringArray::operator + (const PStringArray & v)
  { PStringArray arr = *this; arr += v; return arr; }

PINLINE PStringArray PStringArray::operator + (const PString & v)
  { PStringArray arr = *this; arr += v; return arr; }

PINLINE PINDEX PStringArray::GetStringsIndex(const PString & str) const
  { return GetValuesIndex(str); }

///////////////////////////////////////////////////////////////////////////////

PINLINE PAbstractList::PAbstractList()
  : info(new PListInfo) { PAssert(info != NULL, POutOfMemory); }

PINLINE PObject & PAbstractList::GetReferenceAt(PINDEX index) const
  { PObject * obj = GetAt(index);
                       PAssert(obj != NULL, PInvalidArrayIndex); return *obj; }

///////////////////////////////////////////////////////////////////////////////

PINLINE PINDEX PStringList::AppendString(const PString & str)
  { return Append(str.Clone()); }

PINLINE PINDEX PStringList::InsertString(
                                   const PString & before, const PString & str)
  { return Insert(before, str.Clone()); }

PINLINE PStringList & PStringList::operator += (const PString & str)
  { Append(str.Clone()); return *this; }

PINLINE PStringList PStringList::operator + (const PStringList & v)
  { PStringList arr = *this; arr += v; return arr; }

PINLINE PStringList PStringList::operator + (const PString & v)
  { PStringList arr = *this; arr += v; return arr; }

PINLINE PINDEX PStringList::GetStringsIndex(const PString & str) const
  { return GetValuesIndex(str); }

///////////////////////////////////////////////////////////////////////////////

PINLINE PINDEX PSortedStringList::AppendString(const PString & str)
  { return Append(str.Clone()); }

PINLINE PINDEX PSortedStringList::GetStringsIndex(const PString & str) const
  { return GetValuesIndex(str); }


///////////////////////////////////////////////////////////////////////////////

PINLINE POrdinalKey::POrdinalKey(PINDEX newKey)
  : theKey(newKey) { }

PINLINE POrdinalKey & POrdinalKey::operator=(PINDEX newKey)
  { theKey = newKey; return *this; }

PINLINE POrdinalKey::operator PINDEX() const
  { return theKey; }

PINLINE PINDEX POrdinalKey::operator++()
  { return ++theKey; }

PINLINE PINDEX POrdinalKey::operator++(int)
  { return theKey++; }

PINLINE PINDEX POrdinalKey::operator--()
  { return --theKey; }

PINLINE PINDEX POrdinalKey::operator--(int)
  { return theKey--; }

PINLINE POrdinalKey & POrdinalKey::operator+=(PINDEX add)
  { theKey += add; return *this; }

PINLINE POrdinalKey & POrdinalKey::operator-=(PINDEX minus)
  { theKey -= minus; return *this; }


///////////////////////////////////////////////////////////////////////////////

PINLINE BOOL PHashTable::AbstractContains(const PObject & key) const
  { return hashTable->GetElementAt(key) != NULL; }


///////////////////////////////////////////////////////////////////////////////

PINLINE PAbstractSet::PAbstractSet()
  { hashTable->deleteKeys = reference->deleteObjects; }
  

PINLINE void PStringSet::Include(const PString & str)
  { PAbstractSet::Append(str.Clone()); }

PINLINE PStringSet & PStringSet::operator+=(const PString & str)
  { PAbstractSet::Append(str.Clone()); return *this; }

PINLINE void PStringSet::Exclude(const PString & str)
  { PAbstractSet::Remove(&str); }

PINLINE PStringSet & PStringSet::operator-=(const PString & str)
  { PAbstractSet::Remove(&str); return *this; }


///////////////////////////////////////////////////////////////////////////////

PINLINE PAbstractDictionary::PAbstractDictionary()
  { hashTable->deleteKeys = TRUE; }
  
PINLINE PAbstractDictionary::PAbstractDictionary(int dummy,
                                                 const PAbstractDictionary * c)
  : PHashTable(dummy, c) { }


// End Of File ///////////////////////////////////////////////////////////////
