/*
 * contain.cxx
 *
 * Container Classes
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
 * $Log: contain.cxx,v $
 * Revision 1.183  2007/10/08 05:04:10  rjongbloed
 * Fixed bug introduced to prevent execution if had compiler error, did not allow
 *   for getting NoMatch error on execution and using an instance twice.
 * Also made lastError member mutable to clean up lots of cont casts.
 *
 * Revision 1.182  2007/09/27 23:19:21  rjongbloed
 * Fixed PString sprintf appending to string where combined length
 *   is greater than 1000 bytes, thanks Tomas Hoger.
 *
 * Revision 1.181  2007/09/03 22:13:47  shorne
 * Ensure RegularExpression is compiled without error before execute
 *
 * Revision 1.180  2007/08/07 01:37:54  csoutheren
 * Safeguard against weird copy constructors copying themselves
 *
 * Revision 1.179  2007/08/01 05:20:48  rjongbloed
 * Changes to container classes to become compatible with advanced DevStudio 2005 "Visualizers".
 *
 * Revision 1.178  2007/06/29 02:47:28  rjongbloed
 * Added PString::FindSpan() function (strspn equivalent) with slightly nicer semantics.
 *
 * Revision 1.177  2006/11/23 04:13:52  csoutheren
 * Fix problem in PString::Trim
 * Thanks to Guilhem Tardy
 *
 * Revision 1.176  2006/07/14 05:36:53  csoutheren
 * Fixed 1518375 - PString::RightTrim does not work
 *
 * Revision 1.175  2006/06/21 03:28:44  csoutheren
 * Various cleanups thanks for Frederic Heem
 *
 * Revision 1.174  2006/03/19 23:30:09  csoutheren
 * Added patch#1451378
 * Thanks to Borko Jandras
 *
 * Revision 1.173  2005/11/30 12:47:41  csoutheren
 * Removed tabs, reformatted some code, and changed tags for Doxygen
 *
 * Revision 1.172  2005/09/18 11:05:36  dominance
 * include/ptlib/channel.h, include/ptlib/pstring.h, src/ptlib/common/contain.cxx,
 * src/ptlib/common/pchannel.cxx:
 * correct the STL defined checking to use proper syntax.
 *
 * include/ptlib/object.h:
 * re-add typedef to compile on mingw
 *
 * make/ptlib-config.in:
 * import a long-standing fix from the Debian packs which allows usage of
 * ptlib-config without manually adding -lpt for each of the subsequent
 * projects
 *
 * Revision 1.171  2005/08/08 07:01:58  rjongbloed
 * Minor changes to remove possible ambiguity where virtual and non-virtual
 *   functions are overloaded.
 * Removed commented out code.
 *
 * Revision 1.170  2005/05/02 13:10:24  csoutheren
 * Fixed problem with PString::SetMinSize not always uniquing the string
 *
 * Revision 1.169  2005/05/02 09:02:35  csoutheren
 * Fixed previous fix to contain.cxx which broke PString::MakeUnique
 *
 * Revision 1.168  2005/04/28 04:48:41  csoutheren
 * Changed PContainer::SetSize to not unique a container when the size is unchanged
 *
 * Revision 1.167  2005/01/09 06:35:05  rjongbloed
 * Fixed ability to make Clone() or MakeUnique() of a sorted list.
 *
 * Revision 1.166  2004/10/21 13:04:21  rjongbloed
 * Fixed possibility of const operator[] on PStringArray returning a NULL reference. This
 *   function should return a non-lvalue PString anyway as it is const!
 *
 * Revision 1.165  2004/08/04 00:56:16  csoutheren
 * Added protection against signed chars triggering asserts in VS.net in debug mode
 * Thanks to Michal Zygmuntowicz
 *
 * Revision 1.164  2004/07/19 13:55:00  csoutheren
 * Fixed typo flagged as warning by gcc 3.5-20040704
 *
 * Revision 1.163  2004/07/11 07:56:36  csoutheren
 * Applied jumbo VxWorks patch, thanks to Eize Slange
 *
 * Revision 1.162  2004/06/08 01:31:07  csoutheren
 * Make the test sense correct for the init(NULL)
 *
 * Revision 1.161  2004/06/08 01:29:00  csoutheren
 * Removed memory leak on VS.net caused by unobvious iostream allocation
 *
 * Revision 1.160  2004/05/28 23:59:23  csoutheren
 * Added guards for negative offsets and lengths in various PString functions
 *
 * Revision 1.159  2004/05/13 14:52:32  csoutheren
 * Allow PString::IsEmpty to return TRUE when theArray is NULL
 *
 * Revision 1.158  2004/05/04 11:10:37  rjongbloed
 * Fixed usage of MakeEmpty() with PStringStream.
 *
 * Revision 1.157  2004/04/24 06:27:56  rjongbloed
 * Fixed GCC 3.4.0 warnings about PAssertNULL and improved recoverability on
 *   NULL pointer usage in various bits of code.
 *
 * Revision 1.156  2004/04/18 04:33:37  rjongbloed
 * Changed all operators that return BOOL to return standard type bool. This is primarily
 *   for improved compatibility with std STL usage removing many warnings.
 *
 * Revision 1.155  2004/04/15 03:50:35  csoutheren
 * Fixed problem with MakeUnique
 *
 * Revision 1.154  2004/04/14 23:34:52  csoutheren
 * Added plugin for data access
 *
 * Revision 1.153  2004/04/12 00:36:05  csoutheren
 * Added new class PAtomicInteger and added Windows implementation
 *
 * Revision 1.152  2004/04/11 06:15:36  csoutheren
 * Modified to use Atomic_word if available
 *
 * Revision 1.151  2004/04/11 02:55:18  csoutheren
 * Added PCriticalSection for Windows
 * Added compile time option for PContainer to use critical sections to provide thread safety under some circumstances
 *
 * Revision 1.150  2004/04/09 06:38:11  rjongbloed
 * Fixed compatibility with STL based streams, eg as used by VC++2003
 *
 * Revision 1.149  2004/04/03 23:53:09  csoutheren
 * Added various changes to improce compatibility with the Sun Forte compiler
 *   Thanks to Brian Cameron
 * Added detection of readdir_r version
 *
 * Revision 1.148  2004/04/03 08:22:21  csoutheren
 * Remove pseudo-RTTI and replaced with real RTTI
 *
 * Revision 1.147  2004/04/03 06:54:25  rjongbloed
 * Many and various changes to support new Visual C++ 2003
 *
 * Revision 1.146  2004/03/20 04:20:34  rjongbloed
 * Fixed some VxWorks port issues especially underrrun memory access in
 *   the PString::FindLast function,, thanks Eize Slange
 *
 * Revision 1.145  2004/02/23 00:44:38  csoutheren
 * A completely different, other regex include hack to avoid requiring
 * the sources when using a header-file only environment
 *
 * Revision 1.144  2004/02/23 00:26:05  csoutheren
 * Finally, a generic and elegant fix for the regex include hacks.  Thanks to Roger Hardiman
 *
 * Revision 1.143  2004/02/15 03:04:52  rjongbloed
 * Fixed problem with PSortedList nil variable and assignment between instances,
 *   pointed out by Ben Lear.
 *
 * Revision 1.142  2004/02/11 05:09:14  csoutheren
 * Fixed problems with regex libraries on Solaris, and with host OS numbering
 * being a quoted string rather than a number. Thanks to Chad Attermann
 * Fixed problems SSL detection problems thanks to Michal Zygmuntowicz
 *
 * Revision 1.141  2004/02/08 11:13:20  rjongbloed
 * Fixed crash in heavily loaded multi-threaded systems using simultaneous sorted
 *   lists, Thanks Federico Pinna, Fabrizio Ammollo and the gang at Reitek S.p.A.
 *
 * Revision 1.140  2004/01/17 17:28:25  csoutheren
 * Changed PString::Empty to be inline in header file
 *
 * Revision 1.139  2004/01/16 13:24:38  csoutheren
 * Changed PString::Empty to be thread-safe
 * Fixed PContainer::SetMinSize and PAbstractArray::SetSize, thanks to 123@call2ua.com
 * Fixed PString::FindLast, thanks to Andreas Sikkema
 *
 * Revision 1.138  2003/12/14 01:12:00  csoutheren
 * Added return value to PRegularExpression::operator = again (Doh!)
 *
 * Revision 1.137  2003/12/13 23:08:46  csoutheren
 * Changed PRegularExpression to allow a copy constructor and operator =
 *
 * Revision 1.136  2003/12/04 13:12:41  csoutheren
 * Fixed error in PRegularExpression that caused double delete when incorrect regular expression used
 *
 * Revision 1.135  2003/09/17 09:02:13  csoutheren
 * Removed memory leak detection code
 *
 * Revision 1.134  2003/07/28 18:44:01  dsandras
 * Make use of the libc regex on Linux.
 *
 * Revision 1.133  2003/05/14 00:48:33  rjongbloed
 * Added constructor to string lists/arrays etc that takes a single PString.
 * Fixed bug in doing a MakeUnique on a container, it would lose the
 *   DisallowDeleteObjects flag.
 *
 * Revision 1.132  2003/04/28 09:14:14  robertj
 * Fixed bad sign extension problem in PBYTEArray output
 *
 * Revision 1.131  2003/04/15 07:08:37  robertj
 * Changed read and write from streams for base array classes so operates in
 *   the same way for both PIntArray and PArray<int> etc
 *
 * Revision 1.130  2003/03/31 01:24:23  robertj
 * Added ReadFrom functions for standard container classes such as
 *   PIntArray and PStringList etc
 *
 * Revision 1.129  2003/03/05 08:48:32  robertj
 * Added PStringArray::ToCharAray() function at suggestion of Ravelli Rossano
 *
 * Revision 1.128  2003/02/02 23:29:34  robertj
 * Fixed bug in RightTrim() (lost off last non blank char), tnanks Joerg Schoemer
 *
 * Revision 1.127  2002/12/16 08:10:35  robertj
 * Fixed infinite loop when converting an illegal (incomplete) UTF-8 string
 *   to UCS-2, thanks Chih-Wei Huang
 *
 * Revision 1.126  2002/11/26 01:08:33  robertj
 * Fixed problem when using pre-initialised PStringStream greater than 255
 *   bytes, would truncate and lose trailing null. Reported by Thien Nguyen
 *
 * Revision 1.125  2002/11/12 09:18:03  robertj
 * Added PString::NumCompare() as functional equivalent of strncmp().
 * Added PSortedStringList::GetNextStringsIndex() to do searches of binary
 *   tree on partal strings.
 *
 * Revision 1.124  2002/11/01 05:10:01  robertj
 * Fixed bug in UTF-8 to UCS-2 conversion, not compiler portable!
 *
 * Revision 1.123  2002/10/31 07:33:59  robertj
 * Changed UTF-8 to UCS-2 conversion function to not include trailing null.
 *
 * Revision 1.122  2002/10/31 05:55:55  robertj
 * Now comprehensively stated that a PString is ALWAYS an 8 bit string as
 *   there are far too many inheerent assumptions every to make it 16 bit.
 * Added UTF-8/UCS-2 conversion functions to PString.
 *
 * Revision 1.121  2002/10/10 04:43:44  robertj
 * VxWorks port, thanks Martijn Roest
 *
 * Revision 1.120  2002/08/14 00:43:40  robertj
 * Added ability to have fixed maximum length PStringStream's so does not do
 *   unwanted malloc()'s while outputing data.
 *
 * Revision 1.119  2002/08/06 08:51:36  robertj
 * Added missing va_end, thanks Klaus Kaempf
 *
 * Revision 1.118  2002/06/27 06:59:34  robertj
 * Removed memory leak display of static that is not really a leak.
 *
 * Revision 1.117  2002/06/25 02:24:24  robertj
 * Improved assertion system to allow C++ class name to be displayed if
 *   desired, especially relevant to container classes.
 *
 * Revision 1.116  2002/06/24 06:18:36  robertj
 * Fixed bug when getting extra space at start of outputing PBaseArray.
 * Added ability to not include ASCII in PbaseArray output using ios::fixed.
 *
 * Revision 1.115  2002/06/19 04:04:30  robertj
 * Fixed bug in setting/getting bits from PBitArray, could exceed array bounds.
 *
 * Revision 1.114  2002/06/17 09:16:18  robertj
 * Fixed strange deadlock woth gcc 3.0.2, thanks Artis Kugevics
 *
 * Revision 1.113  2002/06/14 13:22:52  robertj
 * Added PBitArray class.
 *
 * Revision 1.112  2002/06/05 12:29:15  craigs
 * Changes for gcc 3.1
 *
 * Revision 1.111  2002/04/09 02:30:18  robertj
 * Removed GCC3 variable as __GNUC__ can be used instead, thanks jason Spence
 *
 * Revision 1.110  2002/02/15 04:30:39  robertj
 * Added PString::Empty() to return the primordial empty string. Saves on a
 *   couple of memory allocations for every empty string ever used.
 * Changed every place where a const char * is used with PString so that a
 *   NULL pointer is treated like an empty string instead of asserting.
 *
 * Revision 1.109  2002/01/26 23:57:45  craigs
 * Changed for GCC 3.0 compatibility, thanks to manty@manty.net
 *
 * Revision 1.108  2002/01/22 01:03:57  craigs
 * Added operator += and operator + functions to PStringArray and PStringList
 * Added AppendString operator to PStringArray
 *
 * Revision 1.107  2001/10/30 00:20:12  robertj
 * Fixed broken signed number conversion from previous change.
 *
 * Revision 1.106  2001/10/18 00:35:08  robertj
 * Fixed problem with Tokenise() includeing empty string at beginning when
 *   not in onePerSeparator mode and a separator starts the string.
 *
 * Revision 1.105  2001/10/17 05:09:22  robertj
 * Added contructors and assigmnent operators so integer types can be
 *   automatically converted to strings.
 *
 * Revision 1.104  2001/08/11 15:01:44  rogerh
 * Add Mac OS Carbon changes from John Woods <jfw@jfwhome.funhouse.com>
 *
 * Revision 1.103  2001/05/08 23:27:12  robertj
 * Fixed, yet again, case significance in hash function.
 *
 * Revision 1.102  2001/04/26 03:45:19  robertj
 * Changed PString has function again, use a prime number for modulus.
 *
 * Revision 1.101  2001/04/24 02:39:18  robertj
 * Fixed problem with new string hash function giving negative indexes.
 *
 * Revision 1.100  2001/04/23 03:13:16  robertj
 * Changed PString hash function to better one, thanks Patrick Koorevaar
 *
 * Revision 1.99  2001/04/18 04:10:15  robertj
 * Removed hash function for caseless strings as confuses mixed dictionaries.
 *
 * Revision 1.98  2001/04/18 01:20:59  robertj
 * Fixed problem with hash function for short strings, thanks Patrick Koorevaar.
 * Also fixed hash function for caseless strings.
 *
 * Revision 1.97  2001/03/14 01:51:01  craigs
 * Changed to handle CRLF at end of PString::ReadFrom as well as LF
 *
 * Revision 1.96  2001/02/26 07:50:18  robertj
 * Updated regular expression parser to latest version from Henry Spencer.
 *
 * Revision 1.95  2001/02/21 03:38:37  robertj
 * Added ability to copy between various string lists/arrays etc during construction.
 *
 * Revision 1.94  2001/02/14 22:21:08  robertj
 * Fixed compiler error on some versions of GCC, thanks Klaus Kaempf.
 *
 * Revision 1.93  2001/02/14 06:50:01  robertj
 * Fixed bug in doing ::flush on a PStringStream, did not set pointers correctly.
 *
 * Revision 1.92  2001/02/13 04:39:08  robertj
 * Fixed problem with operator= in container classes. Some containers will
 *   break unless the copy is virtual (eg PStringStream's buffer pointers) so
 *   needed to add a new AssignContents() function to all containers.
 *
 * Revision 1.91  2000/12/29 07:36:57  craigs
 * Fixed problem with Tokenise function returning NULL entries in array
 *
 * Revision 1.90  2000/10/12 05:14:41  robertj
 * Fixed crash caused by previous change, didn;t work if in constructor.
 *
 * Revision 1.89  2000/10/09 23:43:58  robertj
 * Fixed GNU C++ compatibility on last change.
 *
 * Revision 1.88  2000/10/09 23:37:17  robertj
 * Improved PString sprintf functions so no longer limited to 1000 characters, thanks Yuriy Ershov.
 *
 * Revision 1.87  2000/06/26 11:17:20  robertj
 * Nucleus++ port (incomplete).
 *
 * Revision 1.86  2000/04/07 06:29:46  rogerh
 * Add a short term workaround for an Internal Compiler Error on MAC OS X when
 * returning certain types of PString. Submitted by Kevin Packard.
 *
 * Revision 1.85  2000/02/05 22:36:09  craigs
 * Fixed problem caused by last modification
 *
 * Revision 1.84  2000/02/04 19:34:26  craigs
 * Fixed problem with changing size of referenced objects
 *
 * Revision 1.83  2000/01/25 14:05:35  robertj
 * Added optimisation to array comparisons if referencing same array.
 *
 * Revision 1.82  1999/08/18 01:45:13  robertj
 * Added concatenation function to "base type" arrays.
 *
 * Revision 1.81  1999/08/17 03:46:40  robertj
 * Fixed usage of inlines in optimised version.
 *
 * Revision 1.80  1999/08/12 12:12:47  robertj
 * GCC 2.95 compatibility.
 *
 * Revision 1.79  1999/05/28 14:01:53  robertj
 * Added initialisers to string containers (list, sorted list and set).
 *
 * Revision 1.78  1999/04/18 09:36:31  robertj
 * Get date grammar build.
 *
 * Revision 1.77  1999/04/16 14:38:37  craigs
 * Changes to make getdate.y compile under Linux
 *
 * Revision 1.76  1998/10/28 00:58:40  robertj
 * Changed PStringStream so flush or endl does not clear the string output, this now
 *    just sets the string to minimum size.
 *
 * Revision 1.75  1998/10/13 14:06:18  robertj
 * Complete rewrite of memory leak detection code.
 *
 * Revision 1.74  1998/09/23 06:21:54  robertj
 * Added open source copyright license.
 *
 * Revision 1.73  1998/09/22 02:42:39  robertj
 * Fixed problem treating unsigned integer as signed in PString contructor.
 *
 * Revision 1.72  1998/09/15 08:26:42  robertj
 * Fixed a number of warnings at maximum optimisation.
 *
 * Revision 1.71  1998/09/14 12:36:29  robertj
 * Fixed bug causing memory leak due to uninitialised member variable for dynamic allocation of arrays.
 *
 * Revision 1.70  1998/08/21 05:24:07  robertj
 * Added hex dump capability to base array types.
 * Added ability to have base arrays of static memory blocks.
 *
 * Revision 1.69  1998/03/17 10:13:23  robertj
 * Fixed bug in Trim() should do all white space not just the space character.
 *
 * Revision 1.68  1998/01/26 00:37:48  robertj
 * Fixed PString & operator putting space in if right hand side is empty string, it shouldn't..
 * Added Execute() functions to PRegularExpression that take PINDEX references instead of PIntArrays.
 * Added FindRegEx function to PString that returns position and length.
 *
 * Revision 1.67  1997/12/11 13:32:49  robertj
 * Added AsUnsigned() function to convert string to DWORD.
 *
 * Revision 1.66  1997/07/08 13:14:41  robertj
 * Fixed bug where freeing null pointer.
 *
 * Revision 1.65  1997/06/08 04:48:04  robertj
 * Added regular expressions.
 *
 * Revision 1.64  1997/03/02 03:41:42  robertj
 * Fixed bug in not being able to construct a zero length PStringArray.
 *
 * Revision 1.63  1996/10/08 13:13:25  robertj
 * Added operator += and &= for char so no implicit PString construction.
 *
 * Revision 1.62  1996/09/14 12:45:57  robertj
 * Fixed bug in PString::Splice() function, no end of string put in.
 *
 * Revision 1.61  1996/08/22 13:21:55  robertj
 * Fixed major bug in FindLast(), could scan all of memory in negative direction.
 *
 * Revision 1.60  1996/08/08 10:08:45  robertj
 * Directory structure changes for common files.
 *
 * Revision 1.59  1996/05/26 03:46:27  robertj
 * Compatibility to GNU 2.7.x
 *
 * Revision 1.58  1996/05/15 10:17:02  robertj
 * Fixed idiotic bug in string compare, caseless version always matched.
 *
 * Revision 1.57  1996/05/09 12:17:10  robertj
 * Fixed incorrect use of memcmp/strcmp return value.
 * Added assertion when finding empty string.
 *
 * Revision 1.56  1996/04/14 02:52:39  robertj
 * Fixed bug in PString::FindLast(), never found sub-strings.
 *
 * Revision 1.55  1996/03/31 08:58:49  robertj
 * Fixed hash function for strings to work for caseless strings.
 *
 * Revision 1.54  1996/03/16 04:56:59  robertj
 * Fixed bug in PStringStream assignment oeprator getting pointers wrong.
 *
 * Revision 1.53  1996/03/02 03:20:11  robertj
 * Fixed bug in PString::Find() not finding substring if exactly same as string.
 *
 * Revision 1.52  1996/02/22 10:23:54  robertj
 * Fixed buf in *= operator only comparing up to shortest string.
 * Fixed bug in & operator for if left string is empty.
 *
 * Revision 1.51  1996/02/19 13:34:53  robertj
 * Removed PCaselessString hash function to fix dictionary match failure.
 * Fixed *= operator yet again.
 *
 * Revision 1.50  1996/02/08 12:20:44  robertj
 * Added new operators to PString for case insensitive compare and spaced concatenate.
 * Fixed bug in Find() not finding case insensitive substrings.
 *
 * Revision 1.49  1996/02/03 11:08:51  robertj
 * Changed memcpy to memove to guarentee string operations will work correctly
 *    when moving overlapping strings around eg in PString::Splice().
 *
 * Revision 1.48  1996/01/28 14:12:22  robertj
 * Fixed bug in Tokenise() for first token empty and PINDEX unsigned.
 *
 * Revision 1.47  1996/01/28 02:53:40  robertj
 * Added assert into all Compare functions to assure comparison between compatible objects.
 * Fixed bug in Find() function, subset sum calculation added one to many bytes.
 *
 * Revision 1.46  1996/01/24 14:43:19  robertj
 * Added initialisers to string dictionaries.
 *
 * Revision 1.45  1996/01/23 13:17:38  robertj
 * Added Replace() function to strings.
 * String searching algorithm rewrite.
 *
 * Revision 1.44  1996/01/02 12:51:05  robertj
 * Mac OS compatibility changes.
 * Removed requirement that PArray elements have parameterless constructor..
 *
 * Revision 1.43  1995/10/14 15:07:42  robertj
 * Changed arrays to not break references, but strings still need to.
 *
 * Revision 1.42  1995/06/17 00:46:20  robertj
 * Added flag for PStringArray constructor to create caseless strings.
 * Fixed bug in arrays when size set to zero.
 *
 * Revision 1.41  1995/06/04 12:39:59  robertj
 * Made char * array all const in PStringArray constructor.
 *
 * Revision 1.40  1995/04/25 11:29:38  robertj
 * Fixed Borland compiler warnings.
 *
 * Revision 1.39  1995/04/02 09:27:27  robertj
 * Added "balloon" help.
 *
 * Revision 1.38  1995/03/12 04:46:02  robertj
 * Fixed use of PCaselessString as dictionary key.
 *
 * Revision 1.37  1995/01/15  04:56:28  robertj
 * Fixed PStringStream for correct pointer calculations in output.
 *
 * Revision 1.36  1995/01/10  11:44:13  robertj
 * Removed PString parameter in stdarg function for GNU C++ compatibility.
 *
 * Revision 1.35  1995/01/09  12:32:56  robertj
 * Removed unnecesary return value from I/O functions.
 * Changed function names due to Mac port.
 *
 * Revision 1.34  1995/01/04  10:57:08  robertj
 * Changed for HPUX and GNU2.6.x
 *
 * Revision 1.33  1995/01/03  09:39:08  robertj
 * Put standard malloc style memory allocation etc into memory check system.
 *
 * Revision 1.32  1994/12/13  11:50:56  robertj
 * Added MakeUnique() function to all container classes.
 *
 * Revision 1.31  1994/12/12  13:13:17  robertj
 * Fixed bugs in PString mods just made.
 *
 * Revision 1.30  1994/12/12  10:16:27  robertj
 * Restructuring and documentation of container classes.
 * Renaming of some macros for declaring container classes.
 * Added some extra functionality to PString.
 * Added start to 2 byte characters in PString.
 * Fixed incorrect overrides in PCaselessString.
 *
 * Revision 1.29  1994/12/05  11:19:36  robertj
 * Moved SetMinSize from PAbstractArray to PContainer.
 *
 * Revision 1.28  1994/11/28  12:37:29  robertj
 * Added dummy parameter to container classes.
 *
 * Revision 1.27  1994/10/30  11:50:44  robertj
 * Split into Object classes and Container classes.
 * Changed mechanism for doing notification callback functions.
 *
 * Revision 1.26  1994/10/23  03:43:07  robertj
 * Changed PBaseArray so can have zero elements in it.
 * Added Printf style constructor to PString.
 *
 * Revision 1.25  1994/09/25  10:49:44  robertj
 * Added empty functions for serialisation.
 *
 * Revision 1.24  1994/08/21  23:43:02  robertj
 * Added object serialisation classes.
 * Changed parameter before variable argument list to NOT be a reference.
 *
 * Revision 1.23  1994/08/04  12:57:10  robertj
 * Rewrite of memory check code.
 *
 * Revision 1.22  1994/08/01  03:40:28  robertj
 * Fixed PString() constructor from integer
 *
 * Revision 1.21  1994/07/27  05:58:07  robertj
 * Synchronisation.
 *
 * Revision 1.20  1994/07/25  03:38:38  robertj
 * Added more memory tests.
 *
 * Revision 1.19  1994/07/17  10:46:06  robertj
 * Added number conversions to PString.
 *
 * Revision 1.18  1994/06/25  11:55:15  robertj
 * Unix version synchronisation.
 *
 * Revision 1.17  1994/04/20  12:17:44  robertj
 * assert changes
 *
 * Revision 1.16  1994/04/11  12:08:37  robertj
 * Fixed bug in memory leak hash table hash function, cant have negative numbers.
 *
 * Revision 1.15  1994/04/03  08:34:18  robertj
 * Added help and focus functionality.
 *
 * Revision 1.14  1994/04/01  14:01:11  robertj
 * Streams and stuff.
 *
 * Revision 1.13  1994/03/07  07:47:00  robertj
 * Major upgrade
 *
 * Revision 1.12  1994/01/15  03:14:22  robertj
 * Mac portability problems.
 *
 * Revision 1.11  1994/01/03  04:42:23  robertj
 * Mass changes to common container classes and interactors etc etc etc.
 *
 * Revision 1.10  1993/12/31  06:53:02  robertj
 * Made inlines optional for debugging purposes.
 *
 * Revision 1.9  1993/12/24  04:20:52  robertj
 * Mac CFront port.
 *
 * Revision 1.8  1993/12/16  00:51:46  robertj
 * Made some container functions const.
 *
 * Revision 1.7  1993/12/15  21:10:10  robertj
 * Fixed reference system used by container classes.
 * Plugged memory leaks in PList and PSortedList.
 *
 * Revision 1.6  1993/12/14  18:44:56  robertj
 * Added RemoveAll() function to collections.
 * Fixed bug in list processing when being destroyed (removes the item being
 *     deleted from the list before deleting it).
 * Changed GetIndex() so does not assert if entry not in collection.
 *
 * Revision 1.5  1993/12/04  05:22:38  robertj
 * Added more string functions.
 *
 * Revision 1.4  1993/09/27  16:35:25  robertj
 * Fixed bugs in sorted list.
 * Fixed compatibility problem with sprintf return value (SVR4).
 * Change function for making string array to a constructor.
 *
 * Revision 1.3  1993/08/27  18:17:47  robertj
 * Fixed bugs in PAbstractSortedList (including some formatting).
 *
 * Revision 1.2  1993/08/21  01:50:33  robertj
 * Made Clone() function optional, default will assert if called.
 *
 * Revision 1.8  1993/08/01  14:05:27  robertj
 * Added const to ToLower() and ToUpper() in the PString class.
 *
 * Revision 1.7  1993/07/16  14:40:55  robertj
 * Added PString constructor for individual characters.
 * Added string to C style literal format.
 *
 * Revision 1.6  1993/07/15  05:02:57  robertj
 * Removed redundant word in PString enum for string types.
 *
 * Revision 1.5  1993/07/15  04:29:39  robertj
 * Added new constructor to convert from other string formats.
 * Fixed sprintf variable parameter list bug.
 *
 * Revision 1.4  1993/07/14  12:41:52  robertj
 * Fixed comment leader.
 *
 * Revision 1.3  1993/07/14  02:06:34  robertj
 * Fixed header comment for RCS.
 */

#include <ptlib.h>
#include <ctype.h>


#ifdef __NUCLEUS_PLUS__
extern "C" int vsprintf(char *, const char *, va_list);
#endif

#if P_REGEX
#include <regex.h>
#else
#include "regex/regex.h"
#endif

#define regexpression  ((regex_t *)expression)

#if !P_USE_INLINES
#include "ptlib/contain.inl"
#endif

#define new PNEW
#undef  __CLASS__
#define __CLASS__ GetClass()


///////////////////////////////////////////////////////////////////////////////

PContainer::PContainer(PINDEX initialSize)
{
  reference = new PContainerReference(initialSize);
  PAssert(reference != NULL, POutOfMemory);
}

PContainer::PContainer(int, const PContainer * cont)
{
  if (cont == this)
    return;

  PAssert(cont != NULL, PInvalidParameter);
  PAssert2(cont->reference != NULL, cont->GetClass(), "Clone of deleted container");

#if PCONTAINER_USES_CRITSEC
  PEnterAndLeave m(cont->reference->critSec);
#endif

  reference = new PContainerReference(*cont->reference);   // create a new reference
  PAssert(reference != NULL, POutOfMemory);
}

PContainer::PContainer(const PContainer & cont)
{
  if (&cont == this)
    return;

  PAssert2(cont.reference != NULL, cont.GetClass(), "Copy of deleted container");

#if PCONTAINER_USES_CRITSEC
  PEnterAndLeave m(cont.reference->critSec);
#endif

  ++cont.reference->count;
  reference = cont.reference;  // copy the reference pointer
}


void PContainer::AssignContents(const PContainer & cont)
{
#if PCONTAINER_USES_CRITSEC
  // make sure the critsecs are entered and left in the right order to avoid deadlock
  cont.reference->critSec.Enter();
  reference->critSec.Enter();
#endif

  if(cont.reference == NULL){
    PAssertAlways("container reference is null");
    return;
  } else if(cont.GetClass() == NULL){
    PAssertAlways("container class is null");
    return;
  }

  if (reference == cont.reference) {
#if PCONTAINER_USES_CRITSEC
    reference->critSec.Leave();
    cont.reference->critSec.Leave();
#endif
    return;
  }

  if (--reference->count > 0) {
#if PCONTAINER_USES_CRITSEC
    reference->critSec.Leave();
#endif
  } else {
#if PCONTAINER_USES_CRITSEC
    reference->critSec.Leave();
#endif
    DestroyContents();
    delete reference;
    reference = NULL;
  }

  ++cont.reference->count;
  reference = cont.reference;

#if PCONTAINER_USES_CRITSEC
  cont.reference->critSec.Leave();
#endif
}


void PContainer::Destruct()
{
  if (reference != NULL) {

#if PCONTAINER_USES_CRITSEC
    PContainerReference * ref = reference;
    ref->critSec.Enter();
#endif

    if (--reference->count > 0) {
      reference = NULL;
#if PCONTAINER_USES_CRITSEC
      ref->critSec.Leave();
#endif
    }
    
    else {
#if PCONTAINER_USES_CRITSEC
      ref->critSec.Leave();
#endif
      DestroyContents();
      delete reference;
      reference = NULL;
    }
  }
}


BOOL PContainer::SetMinSize(PINDEX minSize)
{
  PASSERTINDEX(minSize);
  if (minSize < 0)
    minSize = 0;
  if (minSize < GetSize())
    minSize = GetSize();
  return SetSize(minSize);
}


BOOL PContainer::MakeUnique()
{
#if PCONTAINER_USES_CRITSEC
  PEnterAndLeave m(reference->critSec);
#endif

  if (IsUnique())
    return TRUE;

  PContainerReference * oldReference = reference;
  reference = new PContainerReference(*reference);
  --oldReference->count;

  return FALSE;
}


///////////////////////////////////////////////////////////////////////////////

PAbstractArray::PAbstractArray(PINDEX elementSizeInBytes, PINDEX initialSize)
  : PContainer(initialSize)
{
  elementSize = elementSizeInBytes;
  PAssert(elementSize != 0, PInvalidParameter);

  if (GetSize() == 0)
    theArray = NULL;
  else {
    theArray = (char *)calloc(GetSize(), elementSize);
    PAssert(theArray != NULL, POutOfMemory);
  }

  allocatedDynamically = TRUE;
}


PAbstractArray::PAbstractArray(PINDEX elementSizeInBytes,
                               const void *buffer,
                               PINDEX bufferSizeInElements,
                               BOOL dynamicAllocation)
  : PContainer(bufferSizeInElements)
{
  elementSize = elementSizeInBytes;
  PAssert(elementSize != 0, PInvalidParameter);

  allocatedDynamically = dynamicAllocation;

  if (GetSize() == 0)
    theArray = NULL;
  else if (dynamicAllocation) {
    PINDEX sizebytes = elementSize*GetSize();
    theArray = (char *)malloc(sizebytes);
    PAssert(theArray != NULL, POutOfMemory);
    memcpy(theArray, PAssertNULL(buffer), sizebytes);
  }
  else
    theArray = (char *)buffer;
}


void PAbstractArray::DestroyContents()
{
  if (theArray != NULL) {
    if (allocatedDynamically)
      free(theArray);
    theArray = NULL;
  }
}


void PAbstractArray::CopyContents(const PAbstractArray & array)
{
  elementSize = array.elementSize;
  theArray = array.theArray;
  allocatedDynamically = array.allocatedDynamically;
}


void PAbstractArray::CloneContents(const PAbstractArray * array)
{
  elementSize = array->elementSize;
  PINDEX sizebytes = elementSize*GetSize();
  char * newArray = (char *)malloc(sizebytes);
  if (newArray == NULL)
    reference->size = 0;
  else
    memcpy(newArray, array->theArray, sizebytes);
  theArray = newArray;
  allocatedDynamically = TRUE;
}


void PAbstractArray::PrintOn(ostream & strm) const
{
  char separator = strm.fill();
  int width = (int)strm.width();
  for (PINDEX  i = 0; i < GetSize(); i++) {
    if (i > 0 && separator != '\0')
      strm << separator;
    strm.width(width);
    PrintElementOn(strm, i);
  }
  if (separator == '\n')
    strm << '\n';
}


void PAbstractArray::ReadFrom(istream & strm)
{
  PINDEX i = 0;
  while (strm.good()) {
    ReadElementFrom(strm, i);
    if (!strm.fail())
      i++;
  }
  SetSize(i);
}


PObject::Comparison PAbstractArray::Compare(const PObject & obj) const
{
  PAssert(PIsDescendant(&obj, PAbstractArray), PInvalidCast);
  const PAbstractArray & other = (const PAbstractArray &)obj;

  char * otherArray = other.theArray;
  if (theArray == otherArray)
    return EqualTo;

  if (elementSize < other.elementSize)
    return LessThan;

  if (elementSize > other.elementSize)
    return GreaterThan;

  PINDEX thisSize = GetSize();
  PINDEX otherSize = other.GetSize();

  if (thisSize < otherSize)
    return LessThan;

  if (thisSize > otherSize)
    return GreaterThan;

  if (thisSize == 0)
    return EqualTo;

  int retval = memcmp(theArray, otherArray, elementSize*thisSize);
  if (retval < 0)
    return LessThan;
  if (retval > 0)
    return GreaterThan;
  return EqualTo;
}


BOOL PAbstractArray::SetSize(PINDEX newSize)
{
  return InternalSetSize(newSize, FALSE);
}


BOOL PAbstractArray::InternalSetSize(PINDEX newSize, BOOL force)
{
  if (newSize < 0)
    newSize = 0;

  PINDEX newsizebytes = elementSize*newSize;
  PINDEX oldsizebytes = elementSize*GetSize();

  if (!force && (newsizebytes == oldsizebytes))
    return TRUE;

  char * newArray;

#if PCONTAINER_USES_CRITSEC
  PEnterAndLeave m(reference->critSec);
#endif

  if (!IsUnique()) {

    if (newsizebytes == 0)
      newArray = NULL;
    else {
      if ((newArray = (char *)malloc(newsizebytes)) == NULL)
        return FALSE;
  
      if (theArray != NULL)
        memcpy(newArray, theArray, PMIN(oldsizebytes, newsizebytes));
    }

    --reference->count;
    reference = new PContainerReference(newSize);

  } else {

    if (theArray != NULL) {
      if (newsizebytes == 0) {
        if (allocatedDynamically)
          free(theArray);
        newArray = NULL;
      }
      else if (allocatedDynamically) {
        if ((newArray = (char *)realloc(theArray, newsizebytes)) == NULL)
          return FALSE;
      }
      else {
        if ((newArray = (char *)malloc(newsizebytes)) == NULL)
          return FALSE;
        memcpy(newArray, theArray, PMIN(newsizebytes, oldsizebytes));
        allocatedDynamically = TRUE;
      }
    }
    else if (newsizebytes != 0) {
      if ((newArray = (char *)malloc(newsizebytes)) == NULL)
        return FALSE;
    }
    else
      newArray = NULL;

    reference->size = newSize;
  }

  if (newsizebytes > oldsizebytes)
    memset(newArray+oldsizebytes, 0, newsizebytes-oldsizebytes);

  theArray = newArray;
  return TRUE;
}

void PAbstractArray::Attach(const void *buffer, PINDEX bufferSize)
{
  if (allocatedDynamically && theArray != NULL)
    free(theArray);

#if PCONTAINER_USES_CRITSEC
  PEnterAndLeave m(reference->critSec);
#endif

  theArray = (char *)buffer;
  reference->size = bufferSize;
  allocatedDynamically = FALSE;
}


void * PAbstractArray::GetPointer(PINDEX minSize)
{
  PAssert(SetMinSize(minSize), POutOfMemory);
  return theArray;
}


BOOL PAbstractArray::Concatenate(const PAbstractArray & array)
{
  if (!allocatedDynamically || array.elementSize != elementSize)
    return FALSE;

  PINDEX oldLen = GetSize();
  PINDEX addLen = array.GetSize();

  if (!SetSize(oldLen + addLen))
    return FALSE;

  memcpy(theArray+oldLen*elementSize, array.theArray, addLen*elementSize);
  return TRUE;
}


void PAbstractArray::PrintElementOn(ostream & /*stream*/, PINDEX /*index*/) const
{
}


void PAbstractArray::ReadElementFrom(istream & /*stream*/, PINDEX /*index*/)
{
}


///////////////////////////////////////////////////////////////////////////////

void PCharArray::PrintOn(ostream & strm) const
{
  PINDEX width = (PINDEX)strm.width();
  if (width > GetSize())
    width -= GetSize();
  else
    width = 0;

  BOOL left = (strm.flags()&ios::adjustfield) == ios::left;
  if (left)
    strm.write(theArray, GetSize());

  while (width-- > 0)
    strm << (char)strm.fill();

  if (!left)
    strm.write(theArray, GetSize());
}


void PCharArray::ReadFrom(istream &strm)
{
  PINDEX size = 0;
  SetSize(size+100);

  while (strm.good()) {
    strm >> theArray[size++];
    if (size >= GetSize())
      SetSize(size+100);
  }

  SetSize(size);
}


void PBYTEArray::PrintOn(ostream & strm) const
{
  PINDEX line_width = (PINDEX)strm.width();
  if (line_width == 0)
    line_width = 16;
  strm.width(0);

  PINDEX indent = (PINDEX)strm.precision();

  PINDEX val_width = ((strm.flags()&ios::basefield) == ios::hex) ? 2 : 3;

  PINDEX i = 0;
  while (i < GetSize()) {
    if (i > 0)
      strm << '\n';
    PINDEX j;
    for (j = 0; j < indent; j++)
      strm << ' ';
    for (j = 0; j < line_width; j++) {
      if (j == line_width/2)
        strm << ' ';
      if (i+j < GetSize())
        strm << setw(val_width) << (theArray[i+j]&0xff);
      else {
        PINDEX k;
        for (k = 0; k < val_width; k++)
          strm << ' ';
      }
      strm << ' ';
    }
    if ((strm.flags()&ios::floatfield) != ios::fixed) {
      strm << "  ";
      for (j = 0; j < line_width; j++) {
        if (i+j < GetSize()) {
          unsigned val = theArray[i+j]&0xff;
          if (isprint(val))
            strm << (char)val;
          else
            strm << '.';
        }
      }
    }
    i += line_width;
  }
}


void PBYTEArray::ReadFrom(istream &strm)
{
  PINDEX size = 0;
  SetSize(size+100);

  while (strm.good()) {
    unsigned v;
    strm >> v;
    theArray[size] = (BYTE)v;
    if (!strm.fail()) {
      size++;
      if (size >= GetSize())
        SetSize(size+100);
    }
  }

  SetSize(size);
}


///////////////////////////////////////////////////////////////////////////////

PBitArray::PBitArray(PINDEX initialSize)
  : PBYTEArray((initialSize+7)>>3)
{
}


PBitArray::PBitArray(const void * buffer,
                     PINDEX length,
                     BOOL dynamic)
  : PBYTEArray((const BYTE *)buffer, (length+7)>>3, dynamic)
{
}


PObject * PBitArray::Clone() const
{
  return new PBitArray(*this);
}


PINDEX PBitArray::GetSize() const
{
  return PBYTEArray::GetSize()<<3;
}


BOOL PBitArray::SetSize(PINDEX newSize)
{
  return PBYTEArray::SetSize((newSize+7)>>3);
}


BOOL PBitArray::SetAt(PINDEX index, BOOL val)
{
  if (!SetMinSize(index+1))
    return FALSE;

  if (val)
    theArray[index>>3] |= (1 << (index&7));
  else
    theArray[index>>3] &= ~(1 << (index&7));
  return TRUE;
}


BOOL PBitArray::GetAt(PINDEX index) const
{
  PASSERTINDEX(index);
  if (index >= GetSize())
    return FALSE;

  return (theArray[index>>3]&(1 << (index&7))) != 0;
}


void PBitArray::Attach(const void * buffer, PINDEX bufferSize)
{
  PBYTEArray::Attach((const BYTE *)buffer, (bufferSize+7)>>3);
}


BYTE * PBitArray::GetPointer(PINDEX minSize)
{
  return PBYTEArray::GetPointer((minSize+7)>>3);
}


BOOL PBitArray::Concatenate(const PBitArray & array)
{
  return PAbstractArray::Concatenate(array);
}


///////////////////////////////////////////////////////////////////////////////

PString::PString(const char * cstr)
  : PCharArray(cstr != NULL ? strlen(cstr)+1 : 1)
{
  if (cstr != NULL)
    memcpy(theArray, cstr, GetSize());
}


PString::PString(const WORD * ustr)
{
  if (ustr == NULL)
    SetSize(1);
  else {
    PINDEX len = 0;
    while (ustr[len] != 0)
      len++;
    InternalFromUCS2(ustr, len);
  }
}


PString::PString(const char * cstr, PINDEX len)
  : PCharArray(len+1)
{
  if (len > 0)
    memcpy(theArray, PAssertNULL(cstr), len);
}


PString::PString(const WORD * ustr, PINDEX len)
  : PCharArray(len+1)
{
  InternalFromUCS2(ustr, len);
}


PString::PString(const PWORDArray & ustr)
{
  InternalFromUCS2(ustr, ustr.GetSize());
}


static int TranslateHex(char x)
{
  if (x >= 'a')
    return x - 'a' + 10;

  if (x >= 'A')
    return x - 'A' + '\x0a';

  return x - '0';
}


static const unsigned char PStringEscapeCode[]  = {  'a',  'b',  'f',  'n',  'r',  't',  'v' };
static const unsigned char PStringEscapeValue[] = { '\a', '\b', '\f', '\n', '\r', '\t', '\v' };

static void TranslateEscapes(const char * src, char * dst)
{
  if (*src == '"')
    src++;

  while (*src != '\0') {
    int c = *src++ & 0xff;
    if (c == '"' && *src == '\0')
      c  = '\0'; // Trailing '"' is ignored
    else if (c == '\\') {
      c = *src++ & 0xff;
      for (PINDEX i = 0; i < PARRAYSIZE(PStringEscapeCode); i++) {
        if (c == PStringEscapeCode[i])
          c = PStringEscapeValue[i];
      }

      if (c == 'x' && isxdigit(*src & 0xff)) {
        c = TranslateHex(*src++);
        if (isxdigit(*src & 0xff))
          c = (c << 4) + TranslateHex(*src++);
      }
      else if (c >= '0' && c <= '7') {
        int count = c <= '3' ? 3 : 2;
        src--;
        c = 0;
        do {
          c = (c << 3) + *src++ - '0';
        } while (--count > 0 && *src >= '0' && *src <= '7');
      }
    }

    *dst++ = (char)c;
  }
}


PString::PString(ConversionType type, const char * str, ...)
{
  switch (type) {
    case Pascal :
      if (*str != '\0') {
        PINDEX len = *str & 0xff;
        PAssert(SetSize(len+1), POutOfMemory);
        memcpy(theArray, str+1, len);
      }
      break;

    case Basic :
      if (str[0] != '\0' && str[1] != '\0') {
        PINDEX len = (str[0] & 0xff) | ((str[1] & 0xff) << 8);
        PAssert(SetSize(len+1), POutOfMemory);
        memcpy(theArray, str+2, len);
      }
      break;

    case Literal :
      PAssert(SetSize(strlen(str)+1), POutOfMemory);
      TranslateEscapes(str, theArray);
      PAssert(MakeMinimumSize(), POutOfMemory);
      break;

    case Printf : {
      va_list args;
      va_start(args, str);
      vsprintf(str, args);
      va_end(args);
      break;
    }

    default :
      PAssertAlways(PInvalidParameter);
  }
}


template <class T> char * p_unsigned2string(T value, T base, char * str)
{
  if (value >= base)
    str = p_unsigned2string<T>(value/base, base, str);
  value %= base;
  if (value < 10)
    *str = (char)(value + '0');
  else
    *str = (char)(value + 'A'-10);
  return str+1;
}


template <class T> char * p_signed2string(T value, T base, char * str)
{
  if (value >= 0)
    return p_unsigned2string<T>(value, base, str);

  *str = '-';
  return p_unsigned2string<T>(-value, base, str+1);
}


PString::PString(short n)
  : PCharArray(sizeof(short)*3+1)
{
  p_signed2string<int>(n, 10, theArray);
  MakeMinimumSize();
}


PString::PString(unsigned short n)
  : PCharArray(sizeof(unsigned short)*3+1)
{
  p_unsigned2string<unsigned int>(n, 10, theArray);
  MakeMinimumSize();
}


PString::PString(int n)
  : PCharArray(sizeof(int)*3+1)
{
  p_signed2string<int>(n, 10, theArray);
  MakeMinimumSize();
}


PString::PString(unsigned int n)
  : PCharArray(sizeof(unsigned int)*3+1)
{
  p_unsigned2string<unsigned int>(n, 10, theArray);
  MakeMinimumSize();
}


PString::PString(long n)
  : PCharArray(sizeof(long)*3+1)
{
  p_signed2string<long>(n, 10, theArray);
  MakeMinimumSize();
}


PString::PString(unsigned long n)
  : PCharArray(sizeof(unsigned long)*3+1)
{
  p_unsigned2string<unsigned long>(n, 10, theArray);
  MakeMinimumSize();
}


PString::PString(PInt64 n)
  : PCharArray(sizeof(PInt64)*3+1)
{
  p_signed2string<PInt64>(n, 10, theArray);
  MakeMinimumSize();
}


PString::PString(PUInt64 n)
  : PCharArray(sizeof(PUInt64)*3+1)
{
  p_unsigned2string<PUInt64>(n, 10, theArray);
  MakeMinimumSize();
}


PString::PString(ConversionType type, long value, unsigned base)
  : PCharArray(sizeof(long)*3+1)
{
  PAssert(base >= 2 && base <= 36, PInvalidParameter);
  switch (type) {
    case Signed :
      p_signed2string<long>(value, base, theArray);
      break;

    case Unsigned :
      p_unsigned2string<unsigned long>(value, base, theArray);
      break;

    default :
      PAssertAlways(PInvalidParameter);
  }
  MakeMinimumSize();
}


PString::PString(ConversionType type, double value, unsigned places)
{
  switch (type) {
    case Decimal :
      sprintf("%0.*f", (int)places, value);
      break;

    case Exponent :
      sprintf("%0.*e", (int)places, value);
      break;

    default :
      PAssertAlways(PInvalidParameter);
  }
}


PString & PString::operator=(short n)
{
  SetMinSize(sizeof(short)*3+1);
  p_signed2string<int>(n, 10, theArray);
  MakeMinimumSize();
  return *this;
}


PString & PString::operator=(unsigned short n)
{
  SetMinSize(sizeof(unsigned short)*3+1);
  p_unsigned2string<unsigned int>(n, 10, theArray);
  MakeMinimumSize();
  return *this;
}


PString & PString::operator=(int n)
{
  SetMinSize(sizeof(int)*3+1);
  p_signed2string<int>(n, 10, theArray);
  MakeMinimumSize();
  return *this;
}


PString & PString::operator=(unsigned int n)
{
  SetMinSize(sizeof(unsigned int)*3+1);
  p_unsigned2string<unsigned int>(n, 10, theArray);
  MakeMinimumSize();
  return *this;
}


PString & PString::operator=(long n)
{
  SetMinSize(sizeof(long)*3+1);
  p_signed2string<long>(n, 10, theArray);
  MakeMinimumSize();
  return *this;
}


PString & PString::operator=(unsigned long n)
{
  SetMinSize(sizeof(unsigned long)*3+1);
  p_unsigned2string<unsigned long>(n, 10, theArray);
  MakeMinimumSize();
  return *this;
}


PString & PString::operator=(PInt64 n)
{
  SetMinSize(sizeof(PInt64)*3+1);
  p_signed2string<PInt64>(n, 10, theArray);
  MakeMinimumSize();
  return *this;
}


PString & PString::operator=(PUInt64 n)
{
  SetMinSize(sizeof(PUInt64)*3+1);
  p_unsigned2string<PUInt64>(n, 10, theArray);
  MakeMinimumSize();
  return *this;
}


PString & PString::MakeEmpty()
{
  SetSize(1);
  *theArray = '\0';
  return *this;
}


PObject * PString::Clone() const
{
  return new PString(*this);
}


void PString::PrintOn(ostream &strm) const
{
  strm << theArray;
}


void PString::ReadFrom(istream &strm)
{
  SetMinSize(100);
  char * ptr = theArray;
  PINDEX len = 0;
  int c;
  while ((c = strm.get()) != EOF && c != '\n') {
    *ptr++ = (char)c;
    len++;
    if (len >= GetSize()) {
      SetSize(len + 100);
      ptr = theArray + len;
    }
  }
  *ptr = '\0';
  if ((len > 0) && (ptr[-1] == '\r'))
    ptr[-1] = '\0';
  PAssert(MakeMinimumSize(), POutOfMemory);
}


PObject::Comparison PString::Compare(const PObject & obj) const
{
  PAssert(PIsDescendant(&obj, PString), PInvalidCast);
  return InternalCompare(0, P_MAX_INDEX, ((const PString &)obj).theArray);
}


PINDEX PString::HashFunction() const
{
  // Hash function from "Data Structures and Algorithm Analysis in C++" by
  // Mark Allen Weiss, with limit of only executing over first 8 characters to
  // increase speed when dealing with large strings.

  PINDEX hash = 0;
  for (PINDEX i = 0; i < 8 && theArray[i] != 0; i++)
    hash = (hash << 5) ^ tolower(theArray[i] & 0xff) ^ hash;
  return PABSINDEX(hash)%127;
}


BOOL PString::IsEmpty() const
{
  return (theArray == NULL) || (*theArray == '\0');
}


BOOL PString::SetSize(PINDEX newSize)
{
  return InternalSetSize(newSize, TRUE);
}


BOOL PString::MakeUnique()
{
#if PCONTAINER_USES_CRITSEC
  PEnterAndLeave m(reference->critSec);
#endif

  if (IsUnique())
    return TRUE;

  InternalSetSize(GetSize(), TRUE);
  return FALSE;
}


PString PString::operator+(const char * cstr) const
{
  if (cstr == NULL)
    return *this;

  PINDEX olen = GetLength();
  PINDEX alen = strlen(cstr)+1;
  PString str;
  str.SetSize(olen+alen);
  memmove(str.theArray, theArray, olen);
  memcpy(str.theArray+olen, cstr, alen);
  return str;
}


PString PString::operator+(char c) const
{
  PINDEX olen = GetLength();
  PString str;
  str.SetSize(olen+2);
  memmove(str.theArray, theArray, olen);
  str.theArray[olen] = c;
  return str;
}


PString & PString::operator+=(const char * cstr)
{
  if (cstr == NULL)
    return *this;

  PINDEX olen = GetLength();
  PINDEX alen = strlen(cstr)+1;
  SetSize(olen+alen);
  memcpy(theArray+olen, cstr, alen);
  return *this;
}


PString & PString::operator+=(char ch)
{
  PINDEX olen = GetLength();
  SetSize(olen+2);
  theArray[olen] = ch;
  return *this;
}


PString PString::operator&(const char * cstr) const
{
  if (cstr == NULL)
    return *this;

  PINDEX alen = strlen(cstr)+1;
  if (alen == 1)
    return *this;

  PINDEX olen = GetLength();
  PString str;
  PINDEX space = olen > 0 && theArray[olen-1]!=' ' && *cstr!=' ' ? 1 : 0;
  str.SetSize(olen+alen+space);
  memmove(str.theArray, theArray, olen);
  if (space != 0)
    str.theArray[olen] = ' ';
  memcpy(str.theArray+olen+space, cstr, alen);
  return str;
}


PString PString::operator&(char c) const
{
  PINDEX olen = GetLength();
  PString str;
  PINDEX space = olen > 0 && theArray[olen-1] != ' ' && c != ' ' ? 1 : 0;
  str.SetSize(olen+2+space);
  memmove(str.theArray, theArray, olen);
  if (space != 0)
    str.theArray[olen] = ' ';
  str.theArray[olen+space] = c;
  return str;
}


PString & PString::operator&=(const char * cstr)
{
  if (cstr == NULL)
    return *this;

  PINDEX alen = strlen(cstr)+1;
  if (alen == 1)
    return *this;
  PINDEX olen = GetLength();
  PINDEX space = olen > 0 && theArray[olen-1]!=' ' && *cstr!=' ' ? 1 : 0;
  SetSize(olen+alen+space);
  if (space != 0)
    theArray[olen] = ' ';
  memcpy(theArray+olen+space, cstr, alen);
  return *this;
}


PString & PString::operator&=(char ch)
{
  PINDEX olen = GetLength();
  PINDEX space = olen > 0 && theArray[olen-1] != ' ' && ch != ' ' ? 1 : 0;
  SetSize(olen+2+space);
  if (space != 0)
    theArray[olen] = ' ';
  theArray[olen+space] = ch;
  return *this;
}


void PString::Delete(PINDEX start, PINDEX len)
{
  if (start < 0 || len < 0)
    return;

  MakeUnique();

  register PINDEX slen = GetLength();
  if (start > slen)
    return;

  if (len > slen - start)
    SetAt(start, '\0');
  else
    memmove(theArray+start, theArray+start+len, slen-start-len+1);
  MakeMinimumSize();
}


PString PString::operator()(PINDEX start, PINDEX end) const
{
  if (end < 0 || start < 0 || end < start)
    return Empty();

  register PINDEX len = GetLength();
  if (start > len)
    return Empty();

  if (end >= len) {
    if (start == 0)
      return *this;
    end = len-1;
  }
  len = end - start + 1;

  return PString(theArray+start, len);
}


PString PString::Left(PINDEX len) const
{
  if (len <= 0)
    return Empty();

  if (len >= GetLength())
    return *this;

  return PString(theArray, len);
}


PString PString::Right(PINDEX len) const
{
  if (len <= 0)
    return Empty();

  PINDEX srclen = GetLength();
  if (len >= srclen)
    return *this;

  return PString(theArray+srclen-len, len);
}


PString PString::Mid(PINDEX start, PINDEX len) const
{
  if (len <= 0 || start < 0)
    return Empty();

  if (start+len < start) // Beware of wraparound
    return operator()(start, P_MAX_INDEX);
  else
    return operator()(start, start+len-1);
}


bool PString::operator*=(const char * cstr) const
{
  if (cstr == NULL)
    return IsEmpty() != FALSE;

  const char * pstr = theArray;
  while (*pstr != '\0' && *cstr != '\0') {
    if (toupper(*pstr & 0xff) != toupper(*cstr & 0xff))
      return FALSE;
    pstr++;
    cstr++;
  }
  return *pstr == *cstr;
}


PObject::Comparison PString::NumCompare(const PString & str, PINDEX count, PINDEX offset) const
{
  if (offset < 0 || count < 0)
    return LessThan;
  PINDEX len = str.GetLength();
  if (count > len)
    count = len;
  return InternalCompare(offset, count, str);
}


PObject::Comparison PString::NumCompare(const char * cstr, PINDEX count, PINDEX offset) const
{
  if (offset < 0 || count < 0)
    return LessThan;
  PINDEX len = ::strlen(cstr);
  if (count > len)
    count = len;
  return InternalCompare(offset, count, cstr);
}


PObject::Comparison PString::InternalCompare(PINDEX offset, char c) const
{
  if (offset < 0)
    return LessThan;
  const int ch = theArray[offset] & 0xff;
  if (ch < (c & 0xff))
    return LessThan;
  if (ch > (c & 0xff))
    return GreaterThan;
  return EqualTo;
}


PObject::Comparison PString::InternalCompare(
                         PINDEX offset, PINDEX length, const char * cstr) const
{
  if (offset < 0 || length < 0)
    return LessThan;

  if (offset == 0 && theArray == cstr)
    return EqualTo;

  if (offset < 0 || cstr == NULL)
    return IsEmpty() ? EqualTo : LessThan;

  int retval;
  if (length == P_MAX_INDEX)
    retval = strcmp(theArray+offset, cstr);
  else
    retval = strncmp(theArray+offset, cstr, length);

  if (retval < 0)
    return LessThan;

  if (retval > 0)
    return GreaterThan;

  return EqualTo;
}


PINDEX PString::Find(char ch, PINDEX offset) const
{
  if (offset < 0)
    return P_MAX_INDEX;

  register PINDEX len = GetLength();
  while (offset < len) {
    if (InternalCompare(offset, ch) == EqualTo)
      return offset;
    offset++;
  }
  return P_MAX_INDEX;
}


PINDEX PString::Find(const char * cstr, PINDEX offset) const
{
  if (cstr == NULL || *cstr == '\0' || offset < 0)
    return P_MAX_INDEX;

  PINDEX len = GetLength();
  PINDEX clen = strlen(cstr);
  if (clen > len)
    return P_MAX_INDEX;

  if (offset > len - clen)
    return P_MAX_INDEX;

  if (len - clen < 10) {
    while (offset+clen <= len) {
      if (InternalCompare(offset, clen, cstr) == EqualTo)
        return offset;
      offset++;
    }
    return P_MAX_INDEX;
  }

  int strSum = 0;
  int cstrSum = 0;
  for (PINDEX i = 0; i < clen; i++) {
    strSum += toupper(theArray[offset+i] & 0xff);
    cstrSum += toupper(cstr[i] & 0xff);
  }

  // search for a matching substring
  while (offset+clen <= len) {
    if (strSum == cstrSum && InternalCompare(offset, clen, cstr) == EqualTo)
      return offset;
    strSum += toupper(theArray[offset+clen] & 0xff);
    strSum -= toupper(theArray[offset] & 0xff);
    offset++;
  }

  return P_MAX_INDEX;
}


PINDEX PString::FindLast(char ch, PINDEX offset) const
{
  PINDEX len = GetLength();
  if (len == 0 || offset < 0)
    return P_MAX_INDEX;
  if (offset >= len)
    offset = len-1;

  while (InternalCompare(offset, ch) != EqualTo) {
    if (offset == 0)
      return P_MAX_INDEX;
    offset--;
  }

  return offset;
}


PINDEX PString::FindLast(const char * cstr, PINDEX offset) const
{
  if (cstr == NULL || *cstr == '\0' || offset < 0)
    return P_MAX_INDEX;

  PINDEX len = GetLength();
  PINDEX clen = strlen(cstr);
  if (clen > len)
    return P_MAX_INDEX;

  if (offset > len - clen)
    offset = len - clen;

  int strSum = 0;
  int cstrSum = 0;
  for (PINDEX i = 0; i < clen; i++) {
    strSum += toupper(theArray[offset+i] & 0xff);
    cstrSum += toupper(cstr[i] & 0xff);
  }

  // search for a matching substring
  while (strSum != cstrSum || InternalCompare(offset, clen, cstr) != EqualTo) {
    if (offset == 0)
      return P_MAX_INDEX;
    --offset;
    strSum += toupper(theArray[offset] & 0xff);
    strSum -= toupper(theArray[offset+clen] & 0xff);
  }

  return offset;
}


PINDEX PString::FindOneOf(const char * cset, PINDEX offset) const
{
  if (cset == NULL || *cset == '\0' || offset < 0)
    return P_MAX_INDEX;

  PINDEX len = GetLength();
  while (offset < len) {
    const char * p = cset;
    while (*p != '\0') {
      if (InternalCompare(offset, *p) == EqualTo)
        return offset;
      p++;
    }
    offset++;
  }
  return P_MAX_INDEX;
}


PINDEX PString::FindSpan(const char * cset, PINDEX offset) const
{
  if (cset == NULL || *cset == '\0' || offset < 0)
    return P_MAX_INDEX;

  PINDEX len = GetLength();
  while (offset < len) {
    const char * p = cset;
    while (InternalCompare(offset, *p) != EqualTo) {
      if (*++p == '\0')
        return offset;
    }
    offset++;
  }
  return P_MAX_INDEX;
}


PINDEX PString::FindRegEx(const PRegularExpression & regex, PINDEX offset) const
{
  if (offset < 0)
    return P_MAX_INDEX;

  PINDEX pos = 0;
  PINDEX len = 0;
  if (FindRegEx(regex, pos, len, offset))
    return pos;

  return P_MAX_INDEX;
}


BOOL PString::FindRegEx(const PRegularExpression & regex,
                        PINDEX & pos,
                        PINDEX & len,
                        PINDEX offset,
                        PINDEX maxPos) const
{
  if (offset < 0 || maxPos < 0 || offset >= GetLength())
    return FALSE;

  if (!regex.Execute(&theArray[offset], pos, len, 0))
    return FALSE;

  pos += offset;
  if (pos+len > maxPos)
    return FALSE;

  return TRUE;
}


void PString::Replace(const PString & target,
                      const PString & subs,
                      BOOL all, PINDEX offset)
{
  if (offset < 0)
    return;
    
  MakeUnique();

  PINDEX tlen = target.GetLength();
  PINDEX slen = subs.GetLength();
  do {
    PINDEX pos = Find(target, offset);
    if (pos == P_MAX_INDEX)
      return;
    Splice(subs, pos, tlen);
    offset = pos + slen;
  } while (all);
}


void PString::Splice(const char * cstr, PINDEX pos, PINDEX len)
{
  if (len < 0 || pos < 0)
    return;

  register PINDEX slen = GetLength();
  if (pos >= slen)
    operator+=(cstr);
  else {
    MakeUnique();
    PINDEX clen = cstr != NULL ? strlen(cstr) : 0;
    PINDEX newlen = slen-len+clen;
    if (clen > len)
      SetSize(newlen+1);
    if (pos+len < slen)
      memmove(theArray+pos+clen, theArray+pos+len, slen-pos-len+1);
    if (clen > 0)
      memcpy(theArray+pos, cstr, clen);
    theArray[newlen] = '\0';
  }
}


PStringArray
        PString::Tokenise(const char * separators, BOOL onePerSeparator) const
{
  PStringArray tokens;
  
  if (separators == NULL || IsEmpty())  // No tokens
    return tokens;
    
  PINDEX token = 0;
  PINDEX p1 = 0;
  PINDEX p2 = FindOneOf(separators);

  if (p2 == 0) {
    if (onePerSeparator) { // first character is a token separator
      tokens[token] = Empty();
      token++;                        // make first string in array empty
      p1 = 1;
      p2 = FindOneOf(separators, 1);
    }
    else {
      do {
        p1 = p2 + 1;
      } while ((p2 = FindOneOf(separators, p1)) == p1);
    }
  }

  while (p2 != P_MAX_INDEX) {
    if (p2 > p1)
      tokens[token] = operator()(p1, p2-1);
    else
      tokens[token] = Empty();
    token++;

    // Get next separator. If not one token per separator then continue
    // around loop to skip over all the consecutive separators.
    do {
      p1 = p2 + 1;
    } while ((p2 = FindOneOf(separators, p1)) == p1 && !onePerSeparator);
  }

  tokens[token] = operator()(p1, P_MAX_INDEX);

  return tokens;
}


PStringArray PString::Lines() const
{
  PStringArray lines;
  
  if (IsEmpty())
    return lines;
    
  PINDEX line = 0;
  PINDEX p1 = 0;
  PINDEX p2;
  while ((p2 = FindOneOf("\r\n", p1)) != P_MAX_INDEX) {
    lines[line++] = operator()(p1, p2-1);
    p1 = p2 + 1;
    if (theArray[p2] == '\r' && theArray[p1] == '\n') // CR LF pair
      p1++;
  }
  if (p1 < GetLength())
    lines[line] = operator()(p1, P_MAX_INDEX);
  return lines;
}

PStringArray & PStringArray::operator += (const PStringArray & v)
{
  PINDEX i;
  for (i = 0; i < v.GetSize(); i++)
    AppendString(v[i]);

  return *this;
}

PString PString::LeftTrim() const
{
  const char * lpos = theArray;
  while (isspace(*lpos & 0xff))
    lpos++;
  return PString(lpos);
}


PString PString::RightTrim() const
{
  char * rpos = theArray+GetLength()-1;
  if (!isspace(*rpos & 0xff))
    return *this;

  while (isspace(*rpos & 0xff)) {
    if (rpos == theArray)
      return Empty();
    rpos--;
  }

  // make Apple & Tornado gnu compiler happy
  PString retval(theArray, rpos - theArray + 1);
  return retval;
}


PString PString::Trim() const
{
  const char * lpos = theArray;
  while (isspace(*lpos & 0xff))
    lpos++;
  if (*lpos == '\0')
    return Empty();

  const char * rpos = theArray+GetLength()-1;
	if (!isspace(*rpos & 0xff)) {
		if (lpos == theArray)
			return *this;
		else
			return PString(lpos);
	}

  while (isspace(*rpos & 0xff))
    rpos--;
  return PString(lpos, rpos - lpos + 1);
}


PString PString::ToLower() const
{
  PString newStr(theArray);
  for (char *cpos = newStr.theArray; *cpos != '\0'; cpos++) {
    if (isupper(*cpos & 0xff))
      *cpos = (char)tolower(*cpos & 0xff);
  }
  return newStr;
}


PString PString::ToUpper() const
{
  PString newStr(theArray);
  for (char *cpos = newStr.theArray; *cpos != '\0'; cpos++) {
    if (islower(*cpos & 0xff))
      *cpos = (char)toupper(*cpos & 0xff);
  }
  return newStr;
}


long PString::AsInteger(unsigned base) const
{
  PAssert(base >= 2 && base <= 36, PInvalidParameter);
  char * dummy;
  return strtol(theArray, &dummy, base);
}


DWORD PString::AsUnsigned(unsigned base) const
{
  PAssert(base >= 2 && base <= 36, PInvalidParameter);
  char * dummy;
  return strtoul(theArray, &dummy, base);
}


double PString::AsReal() const
{
#ifndef __HAS_NO_FLOAT
  char * dummy;
  return strtod(theArray, &dummy);
#else
  return 0.0;
#endif
}


PWORDArray PString::AsUCS2() const
{
#ifdef P_HAS_G_CONVERT

  gsize g_len = 0;
  gchar * g_ucs2 = g_convert(theArray, GetSize()-1, "UCS-2", "UTF-8", 0, &g_len, 0);
  if (g_ucs2 == NULL)
    return PWORDArray();

  PWORDArray ucs2((const WORD *)g_ucs2, (PINDEX)g_len);
  g_free(g_ucs2)
  return ucs2;

#else

  PWORDArray ucs2(GetSize()); // Always bigger than required

  PINDEX count = 0;
  PINDEX i = 0;
  PINDEX length = GetSize()-1;
  while (i < length) {
    int c = theArray[i];
    if ((c&0x80) == 0)
      ucs2[count++] = (BYTE)theArray[i++];
    else if ((c&0xe0) == 0xc0) {
      if (i < length-1)
        ucs2[count++] = (WORD)(((theArray[i  ]&0x1f)<<6)|
                                (theArray[i+1]&0x3f));
      i += 2;
    }
    else if ((c&0xf0) == 0xe0) {
      if (i < length-2)
        ucs2[count++] = (WORD)(((theArray[i  ]&0x0f)<<12)|
                               ((theArray[i+1]&0x3f)<< 6)|
                                (theArray[i+2]&0x3f));
      i += 3;
    }
    else {
      if ((c&0xf8) == 0xf0)
        i += 4;
      else if ((c&0xfc) == 0xf8)
        i += 5;
      else
        i += 6;
      if (i <= length)
        ucs2[count++] = 0xffff;
    }
  }

  ucs2.SetSize(count);
  return ucs2;

#endif
}


void PString::InternalFromUCS2(const WORD * ptr, PINDEX len)
{
  if (ptr == NULL || len <= 0) {
    *this = Empty();
    return;
  }

#ifdef P_HAS_G_CONVERT

  gsize g_len = 0;
  gchar * g_utf8 = g_convert(ptr, len, "UTF-8", "UCS-2", 0, &g_len, 0);
  if (g_utf8 == NULL) {
    *this = Empty();
    return;
  }

  SetSize(&g_len);
  memcpy(theArray, g_char, g_len);
  g_free(g_utf8);

#else

  PINDEX i;
  PINDEX count = 1;
  for (i = 0; i < len; i++) {
    if (ptr[i] < 0x80)
      count++;
    else if (ptr[i] < 0x800)
      count += 2;
    else
      count += 3;
  }
  SetSize(count);

  count = 0;
  for (i = 0; i < len; i++) {
    unsigned v = *ptr++;
    if (v < 0x80)
      theArray[count++] = (char)v;
    else if (v < 0x800) {
      theArray[count++] = (char)(0xc0+(v>>6));
      theArray[count++] = (char)(0x80+(v&0x3f));
    }
    else {
      theArray[count++] = (char)(0xd0+(v>>12));
      theArray[count++] = (char)(0x80+((v>>6)&0x3f));
      theArray[count++] = (char)(0x80+(v&0x3f));
    }
  }

#endif
}


PBYTEArray PString::ToPascal() const
{
  PINDEX len = GetLength();
  PAssert(len < 256, "Cannot convert to PASCAL string");
  BYTE buf[256];
  buf[0] = (BYTE)len;
  memcpy(&buf[1], theArray, len);
  return PBYTEArray(buf, len+1);
}


PString PString::ToLiteral() const
{
  PString str('"');
  for (char * p = theArray; *p != '\0'; p++) {
    if (*p == '"')
      str += "\\\"";
    else if (isprint(*p & 0xff))
      str += *p;
    else {
      PINDEX i;
      for (i = 0; i < PARRAYSIZE(PStringEscapeValue); i++) {
        if (*p == PStringEscapeValue[i]) {
          str += PString('\\') + (char)PStringEscapeCode[i];
          break;
        }
      }
      if (i >= PARRAYSIZE(PStringEscapeValue))
        str.sprintf("\\%03o", *p & 0xff);
    }
  }
  return str + '"';
}


PString & PString::sprintf(const char * fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  return vsprintf(fmt, args);
}

#if defined(__GNUC__) || defined(__SUNPRO_CC)
#define _vsnprintf vsnprintf
#endif

PString & PString::vsprintf(const char * fmt, va_list arg)
{
  PINDEX len = theArray != NULL ? GetLength() : 0;
#ifdef P_VXWORKS
  // The library provided with tornado 2.0 does not have the implementation
  // for vsnprintf
  // as workaround, just use a array size of 2000
  PAssert(SetSize(2000), POutOfMemory);
  ::vsprintf(theArray+len, fmt, arg);
#else
  PINDEX size = len;
  do {
    size += 1000;
    PAssert(SetSize(size), POutOfMemory);
  } while (_vsnprintf(theArray+len, size-len, fmt, arg) == -1);
#endif // P_VXWORKS

  PAssert(MakeMinimumSize(), POutOfMemory);
  return *this;
}


PString psprintf(const char * fmt, ...)
{
  PString str;
  va_list args;
  va_start(args, fmt);
  return str.vsprintf(fmt, args);
}


PString pvsprintf(const char * fmt, va_list arg)
{
  PString str;
  return str.vsprintf(fmt, arg);
}


///////////////////////////////////////////////////////////////////////////////

PObject * PCaselessString::Clone() const
{
  return new PCaselessString(*this);
}


PObject::Comparison PCaselessString::InternalCompare(PINDEX offset, char c) const
{
  if (offset < 0)
    return LessThan;

  int c1 = toupper(theArray[offset] & 0xff);
  int c2 = toupper(c & 0xff);
  if (c1 < c2)
    return LessThan;
  if (c1 > c2)
    return GreaterThan;
  return EqualTo;
}


PObject::Comparison PCaselessString::InternalCompare(
                         PINDEX offset, PINDEX length, const char * cstr) const
{
  if (offset < 0 || length < 0)
    return LessThan;

  if (cstr == NULL)
    return IsEmpty() ? EqualTo : LessThan;

  while (length-- > 0 && (theArray[offset] != '\0' || *cstr != '\0')) {
    Comparison c = PCaselessString::InternalCompare(offset++, *cstr++);
    if (c != EqualTo)
      return c;
  }
  return EqualTo;
}



///////////////////////////////////////////////////////////////////////////////

PStringStream::Buffer::Buffer(PStringStream & str, PINDEX size)
  : string(str),
    fixedBufferSize(size != 0)
{
  string.SetMinSize(size > 0 ? size : 256);
  sync();
}


int PStringStream::Buffer::overflow(int c)
{
  if (pptr() >= epptr()) {
    if (fixedBufferSize)
      return EOF;

    int gpos = gptr() - eback();
    int ppos = pptr() - pbase();
    char * newptr = string.GetPointer(string.GetSize() + 32);
    setp(newptr, newptr + string.GetSize() - 1);
    pbump(ppos);
    setg(newptr, newptr + gpos, newptr + ppos);
  }

  if (c != EOF) {
    *pptr() = (char)c;
    pbump(1);
  }

  return 0;
}


int PStringStream::Buffer::underflow()
{
  return gptr() >= egptr() ? EOF : *gptr();
}


int PStringStream::Buffer::sync()
{
  char * base = string.GetPointer();
  PINDEX len = string.GetLength();
  setg(base, base, base + len);
  setp(base, base + string.GetSize() - 1);
  pbump(len);
  return 0;
}

#ifdef __USE_STL__
streambuf::pos_type PStringStream::Buffer::seekoff(off_type off, ios_base::seekdir dir, ios_base::openmode mode)
#else
streampos PStringStream::Buffer::seekoff(streamoff off, ios::seek_dir dir, int mode)
#endif
{
  int len = string.GetLength();
  int gpos = gptr() - eback();
  int ppos = pptr() - pbase();
  char * newgptr;
  char * newpptr;
  switch (dir) {
    case ios::beg :
      if (off < 0)
        newpptr = newgptr = eback();
      else if (off >= len)
        newpptr = newgptr = egptr();
      else
        newpptr = newgptr = eback()+off;
      break;

    case ios::cur :
      if (off < -ppos)
        newpptr = eback();
      else if (off >= len-ppos)
        newpptr = epptr();
      else
        newpptr = pptr()+off;
      if (off < -gpos)
        newgptr = eback();
      else if (off >= len-gpos)
        newgptr = egptr();
      else
        newgptr = gptr()+off;
      break;

    case ios::end :
      if (off < -len)
        newpptr = newgptr = eback();
      else if (off >= 0)
        newpptr = newgptr = egptr();
      else
        newpptr = newgptr = egptr()+off;
      break;

    default:
      PAssertAlways2(string.GetClass(), PInvalidParameter);
      newgptr = gptr();
      newpptr = pptr();
  }

  if ((mode&ios::in) != 0)
    setg(eback(), newgptr, egptr());

  if ((mode&ios::out) != 0)
    setp(newpptr, epptr());

  return 0;
}


#ifdef __USE_STL__
streampos PStringStream::Buffer::seekpos(pos_type pos, ios_base::openmode mode)
{
  return seekoff(pos, ios_base::beg, mode);
}
#endif


#ifdef _MSC_VER
#pragma warning(disable:4355)
#endif

PStringStream::PStringStream()
  : iostream(new PStringStream::Buffer(*this, 0))
{
}


PStringStream::PStringStream(PINDEX fixedBufferSize)
  : iostream(new PStringStream::Buffer(*this, fixedBufferSize))
{
}


PStringStream::PStringStream(const PString & str)
  : PString(str),
    iostream(new PStringStream::Buffer(*this, 0))
{
}


PStringStream::PStringStream(const char * cstr)
  : PString(cstr),
    iostream(new PStringStream::Buffer(*this, 0))
{
}

#ifdef _MSC_VER
#pragma warning(default:4355)
#endif


PStringStream::~PStringStream()
{
  delete (PStringStream::Buffer *)rdbuf();
#ifndef _WIN32
  init(NULL);
#endif
}


PString & PStringStream::MakeEmpty()
{
  *theArray = '\0';
  flush();
  return *this;
}


void PStringStream::AssignContents(const PContainer & cont)
{
  PString::AssignContents(cont);
  flush();
}


///////////////////////////////////////////////////////////////////////////////

PStringArray::PStringArray(PINDEX count, char const * const * strarr, BOOL caseless)
{
  if (count == 0)
    return;

  if (PAssertNULL(strarr) == NULL)
    return;

  if (count == P_MAX_INDEX) {
    count = 0;
    while (strarr[count] != NULL)
      count++;
  }

  SetSize(count);
  for (PINDEX i = 0; i < count; i++) {
    PString * newString;
    if (caseless)
      newString = new PCaselessString(strarr[i]);
    else
      newString = new PString(strarr[i]);
    SetAt(i, newString);
  }
}


PStringArray::PStringArray(const PString & str)
{
  SetSize(1);
  (*theArray)[0] = new PString(str);
}


PStringArray::PStringArray(const PStringList & list)
{
  SetSize(list.GetSize());
  for (PINDEX i = 0; i < list.GetSize(); i++)
    (*theArray)[i] = new PString(list[i]);
}


PStringArray::PStringArray(const PSortedStringList & list)
{
  SetSize(list.GetSize());
  for (PINDEX i = 0; i < list.GetSize(); i++)
    (*theArray)[i] = new PString(list[i]);
}


void PStringArray::ReadFrom(istream & strm)
{
  while (strm.good()) {
    PString str;
    strm >> str;
    AppendString(str);
  }
}


PString PStringArray::operator[](PINDEX index) const
{
  PASSERTINDEX(index);
  if (index < GetSize() && (*theArray)[index] != NULL)
    return *(PString *)(*theArray)[index];
  return PString::Empty();
}


PString & PStringArray::operator[](PINDEX index)
{
  PASSERTINDEX(index);
  PAssert(SetMinSize(index+1), POutOfMemory);
  if ((*theArray)[index] == NULL)
    (*theArray)[index] = new PString;
  return *(PString *)(*theArray)[index];
}


char ** PStringArray::ToCharArray(PCharArray * storage) const
{
  PINDEX i;

  PINDEX mySize = GetSize();
  PINDEX storageSize = (mySize+1)*sizeof(char *);
  for (i = 0; i < mySize; i++)
    storageSize += (*this)[i].GetLength()+1;

  char ** storagePtr;
  if (storage != NULL)
    storagePtr = (char **)storage->GetPointer(storageSize);
  else
    storagePtr = (char **)malloc(storageSize);

  if (storagePtr == NULL)
    return NULL;

  char * strPtr = (char *)&storagePtr[GetSize()+1];

  for (i = 0; i < mySize; i++) {
    storagePtr[i] = strPtr;
    const PString & str = (*this)[i];
    PINDEX len = str.GetLength()+1;
    memcpy(strPtr, (const char *)str, len);
    strPtr += len;
  }

  storagePtr[i] = NULL;

  return storagePtr;
}


///////////////////////////////////////////////////////////////////////////////

PStringList::PStringList(PINDEX count, char const * const * strarr, BOOL caseless)
{
  if (count == 0)
    return;

  if (PAssertNULL(strarr) == NULL)
    return;

  for (PINDEX i = 0; i < count; i++) {
    PString * newString;
    if (caseless)
      newString = new PCaselessString(strarr[i]);
    else
      newString = new PString(strarr[i]);
    Append(newString);
  }
}


PStringList::PStringList(const PString & str)
{
  AppendString(str);
}


PStringList::PStringList(const PStringArray & array)
{
  for (PINDEX i = 0; i < array.GetSize(); i++)
    AppendString(array[i]);
}


PStringList::PStringList(const PSortedStringList & list)
{
  for (PINDEX i = 0; i < list.GetSize(); i++)
    AppendString(list[i]);
}

PStringList & PStringList::operator += (const PStringList & v)
{
  PINDEX i;
  for (i = 0; i < v.GetSize(); i++)
    AppendString(v[i]);

  return *this;
}


void PStringList::ReadFrom(istream & strm)
{
  while (strm.good()) {
    PString str;
    strm >> str;
    AppendString(str);
  }
}


///////////////////////////////////////////////////////////////////////////////

PSortedStringList::PSortedStringList(PINDEX count,
                                     char const * const * strarr,
                                     BOOL caseless)
{
  if (count == 0)
    return;

  if (PAssertNULL(strarr) == NULL)
    return;

  for (PINDEX i = 0; i < count; i++) {
    PString * newString;
    if (caseless)
      newString = new PCaselessString(strarr[i]);
    else
      newString = new PString(strarr[i]);
    Append(newString);
  }
}


PSortedStringList::PSortedStringList(const PString & str)
{
  AppendString(str);
}


PSortedStringList::PSortedStringList(const PStringArray & array)
{
  for (PINDEX i = 0; i < array.GetSize(); i++)
    AppendString(array[i]);
}


PSortedStringList::PSortedStringList(const PStringList & list)
{
  for (PINDEX i = 0; i < list.GetSize(); i++)
    AppendString(list[i]);
}



void PSortedStringList::ReadFrom(istream & strm)
{
  while (strm.good()) {
    PString str;
    strm >> str;
    AppendString(str);
  }
}


PINDEX PSortedStringList::GetNextStringsIndex(const PString & str) const
{
  PINDEX len = str.GetLength();

  info->lastIndex = InternalStringSelect(str, len, info->root);

  if (info->lastIndex != 0) {
    Element * prev;
    while ((prev = info->Predecessor(info->lastElement)) != &info->nil &&
                    ((PString *)prev->data)->NumCompare(str, len) >= EqualTo) {
      info->lastElement = prev;
      info->lastIndex--;
    }
  }

  return info->lastIndex;
}


PINDEX PSortedStringList::InternalStringSelect(const char * str,
                                               PINDEX len,
                                               Element * thisElement) const
{
  if (thisElement == &info->nil)
    return 0;

  switch (((PString *)thisElement->data)->NumCompare(str, len)) {
    case PObject::LessThan :
    {
      PINDEX index = InternalStringSelect(str, len, thisElement->right);
      return thisElement->left->subTreeSize + index + 1;
    }

    case PObject::GreaterThan :
      return InternalStringSelect(str, len, thisElement->left);

    default :
      info->lastElement = thisElement;
      return thisElement->left->subTreeSize;
  }
}


///////////////////////////////////////////////////////////////////////////////

PStringSet::PStringSet(PINDEX count, char const * const * strarr, BOOL caseless)
{
  if (count == 0)
    return;

  if (PAssertNULL(strarr) == NULL)
    return;

  for (PINDEX i = 0; i < count; i++) {
    if (caseless)
      Include(PCaselessString(strarr[i]));
    else
      Include(PString(strarr[i]));
  }
}


PStringSet::PStringSet(const PString & str)
{
  Include(str);
}


void PStringSet::ReadFrom(istream & strm)
{
  while (strm.good()) {
    PString str;
    strm >> str;
    Include(str);
  }
}


///////////////////////////////////////////////////////////////////////////////

POrdinalToString::POrdinalToString(PINDEX count, const Initialiser * init)
{
  while (count-- > 0) {
    SetAt(init->key, init->value);
    init++;
  }
}


void POrdinalToString::ReadFrom(istream & strm)
{
  while (strm.good()) {
    POrdinalKey key;
    char equal;
    PString str;
    strm >> key >> ws >> equal >> str;
    if (equal != '=')
      SetAt(key, PString::Empty());
    else
      SetAt(key, str.Mid(equal+1));
  }
}


///////////////////////////////////////////////////////////////////////////////

PStringToOrdinal::PStringToOrdinal(PINDEX count,
                                   const Initialiser * init,
                                   BOOL caseless)
{
  while (count-- > 0) {
    if (caseless)
      SetAt(PCaselessString(init->key), init->value);
    else
      SetAt(init->key, init->value);
    init++;
  }
}


void PStringToOrdinal::ReadFrom(istream & strm)
{
  while (strm.good()) {
    PString str;
    strm >> str;
    PINDEX equal = str.FindLast('=');
    if (equal == P_MAX_INDEX)
      SetAt(str, 0);
    else
      SetAt(str.Left(equal), str.Mid(equal+1).AsInteger());
  }
}


///////////////////////////////////////////////////////////////////////////////

PStringToString::PStringToString(PINDEX count,
                                 const Initialiser * init,
                                 BOOL caselessKeys,
                                 BOOL caselessValues)
{
  while (count-- > 0) {
    if (caselessValues)
      if (caselessKeys)
        SetAt(PCaselessString(init->key), PCaselessString(init->value));
      else
        SetAt(init->key, PCaselessString(init->value));
    else
      if (caselessKeys)
        SetAt(PCaselessString(init->key), init->value);
      else
        SetAt(init->key, init->value);
    init++;
  }
}


void PStringToString::ReadFrom(istream & strm)
{
  while (strm.good()) {
    PString str;
    strm >> str;
    PINDEX equal = str.Find('=');
    if (equal == P_MAX_INDEX)
      SetAt(str, PString::Empty());
    else
      SetAt(str.Left(equal), str.Mid(equal+1));
  }
}


///////////////////////////////////////////////////////////////////////////////

PRegularExpression::PRegularExpression()
{
  lastError   = NotCompiled;
  expression  = NULL;
  flagsSaved  = IgnoreCase;
}


PRegularExpression::PRegularExpression(const PString & pattern, int flags)
{
  expression = NULL;
  Compile(pattern, flags);
}


PRegularExpression::PRegularExpression(const char * pattern, int flags)
{
  expression = NULL;
  Compile(pattern, flags);
}

PRegularExpression::PRegularExpression(const PRegularExpression & from)
{
  expression   = NULL;
  patternSaved = from.patternSaved;
  flagsSaved   = from.flagsSaved;
  Compile(patternSaved, flagsSaved);
}

PRegularExpression & PRegularExpression::operator =(const PRegularExpression & from)
{
  expression   = NULL;
  patternSaved = from.patternSaved;
  flagsSaved   = from.flagsSaved;
  Compile(patternSaved, flagsSaved);
  return *this;
}

PRegularExpression::~PRegularExpression()
{
  if (expression != NULL) {
    regfree(regexpression);
    delete regexpression;
  }
}


PRegularExpression::ErrorCodes PRegularExpression::GetErrorCode() const
{
  return lastError;
}


PString PRegularExpression::GetErrorText() const
{
  PString str;
  regerror(lastError, regexpression, str.GetPointer(256), 256);
  return str;
}


BOOL PRegularExpression::Compile(const PString & pattern, int flags)
{
  return Compile((const char *)pattern, flags);
}


BOOL PRegularExpression::Compile(const char * pattern, int flags)
{
  patternSaved = pattern;
  flagsSaved   = flags;

  if (expression != NULL) {
    regfree(regexpression);
    delete regexpression;
    expression = NULL;
  }
  if (pattern == NULL || *pattern == '\0')
    lastError = BadPattern;
  else {
    expression = new regex_t;
    lastError = (ErrorCodes)regcomp(regexpression, pattern, flags);
  }
  return lastError == NoError;
}


BOOL PRegularExpression::Execute(const PString & str, PINDEX & start, int flags) const
{
  PINDEX dummy;
  return Execute((const char *)str, start, dummy, flags);
}


BOOL PRegularExpression::Execute(const PString & str, PINDEX & start, PINDEX & len, int flags) const
{
  return Execute((const char *)str, start, len, flags);
}


BOOL PRegularExpression::Execute(const char * cstr, PINDEX & start, int flags) const
{
  PINDEX dummy;
  return Execute(cstr, start, dummy, flags);
}


BOOL PRegularExpression::Execute(const char * cstr, PINDEX & start, PINDEX & len, int flags) const
{
  if (expression == NULL) {
    lastError = NotCompiled;
    return FALSE;
  }

  if (lastError != NoError && lastError != NoMatch)
    return FALSE;

  regmatch_t match;

  lastError = (ErrorCodes)regexec(regexpression, cstr, 1, &match, flags);
  if (lastError != NoError)
    return FALSE;

  start = match.rm_so;
  len = match.rm_eo - start;
  return TRUE;
}


BOOL PRegularExpression::Execute(const PString & str, PIntArray & starts, int flags) const
{
  PIntArray dummy;
  return Execute((const char *)str, starts, dummy, flags);
}


BOOL PRegularExpression::Execute(const PString & str,
                                 PIntArray & starts,
                                 PIntArray & ends,
                                 int flags) const
{
  return Execute((const char *)str, starts, ends, flags);
}


BOOL PRegularExpression::Execute(const char * cstr, PIntArray & starts, int flags) const
{
  PIntArray dummy;
  return Execute(cstr, starts, dummy, flags);
}


BOOL PRegularExpression::Execute(const char * cstr,
                                 PIntArray & starts,
                                 PIntArray & ends,
                                 int flags) const
{
  if (expression == NULL) {
    lastError = NotCompiled;
    return FALSE;
  }

  regmatch_t single_match;
  regmatch_t * matches = &single_match;

  PINDEX count = starts.GetSize();
  if (count > 1)
    matches = new regmatch_t[count];
  else
    count = 1;

  lastError = (ErrorCodes)regexec(regexpression, cstr, count, matches, flags);
  if (lastError == NoError) {
    starts.SetMinSize(count);
    ends.SetMinSize(count);
    for (PINDEX i = 0; i < count; i++) {
      starts[i] = matches[i].rm_so;
      ends[i] = matches[i].rm_eo;
    }
  }

  if (matches != &single_match)
    delete [] matches;

  return lastError == NoError;
}


PString PRegularExpression::EscapeString(const PString & str)
{
  PString translated;

  PINDEX lastPos = 0;
  PINDEX nextPos;
  while ((nextPos = str.FindOneOf("\\^$+?*.[]()|{}", lastPos+1)) != P_MAX_INDEX) {
    translated += str(lastPos, nextPos-1) + "\\";
    lastPos = nextPos;
  }

  if (lastPos == 0)
    return str;

  return translated + str.Mid(lastPos);
}


// End Of File ///////////////////////////////////////////////////////////////
