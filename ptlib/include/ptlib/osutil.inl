/*
 * osutil.inl
 *
 * Operating System Classes Inline Function Definitions
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
 * $Log: osutil.inl,v $
 * Revision 1.92  2006/10/10 07:16:55  csoutheren
 * Tweaked default build options to suite more common configurations
 *
 * Revision 1.91  2005/11/04 06:34:20  csoutheren
 * Added new class PSync as abstract base class for all mutex/sempahore classes
 * Changed PCriticalSection to use Wait/Signal rather than Enter/Leave
 * Changed Wait/Signal to be const member functions
 * Renamed PMutex to PTimedMutex and made PMutex synonym for PCriticalSection.
 * This allows use of very efficient mutex primitives in 99% of cases where timed waits
 * are not needed
 *
 * Revision 1.90  2005/01/26 05:37:45  csoutheren
 * Added ability to remove config file support
 *
 * Revision 1.89  2004/04/18 04:33:36  rjongbloed
 * Changed all operators that return BOOL to return standard type bool. This is primarily
 *   for improved compatibility with std STL usage removing many warnings.
 *
 * Revision 1.88  2003/12/19 04:30:24  csoutheren
 * Changed GetLastReadCount and GetLastWriteCount to be virtual
 *
 * Revision 1.87  2003/09/17 01:18:02  csoutheren
 * Removed recursive include file system and removed all references
 * to deprecated coooperative threading support
 *
 * Revision 1.86  2002/12/18 05:05:43  robertj
 * Fixed problem with returning DWORD time interval when PTimeInterval is
 *   out of range, especially when negative!
 *
 * Revision 1.85  2002/10/29 00:07:21  robertj
 * Added IsValid() function to indicate that a PTime is set correctly.
 *
 * Revision 1.84  2002/10/17 07:17:42  robertj
 * Added ability to increase maximum file handles on a process.
 *
 * Revision 1.83  2002/01/26 23:55:55  craigs
 * Changed for GCC 3.0 compatibility, thanks to manty@manty.net
 *
 * Revision 1.82  2001/11/23 00:55:18  robertj
 * Changed PWaitAndSignal so works inside const functions.
 *
 * Revision 1.81  2001/11/14 06:06:26  robertj
 * Added functions on PTimer to get reset value and restart timer to it.
 *
 * Revision 1.80  2001/09/10 02:51:22  robertj
 * Major change to fix problem with error codes being corrupted in a
 *   PChannel when have simultaneous reads and writes in threads.
 *
 * Revision 1.79  2001/07/10 02:55:16  robertj
 * Added unary minus operator
 *
 * Revision 1.78  2001/04/23 00:34:29  robertj
 * Added ability for PWaitAndSignal to not wait on semaphore.
 *
 * Revision 1.77  2001/02/13 06:55:21  robertj
 * Fixed problem with operator= in PDirectory class, part of larger change previously made.
 *
 * Revision 1.76  2001/02/13 04:39:08  robertj
 * Fixed problem with operator= in container classes. Some containers will
 *   break unless the copy is virtual (eg PStringStream's buffer pointers) so
 *   needed to add a new AssignContents() function to all containers.
 *
 * Revision 1.75  2000/10/05 23:36:26  robertj
 * Fixed compiler ambiguities in PTimeInterval constructor.
 *
 * Revision 1.74  2000/05/25 11:07:26  robertj
 * Added PConfig::HasKey() function to determine if value actually set.
 *
 * Revision 1.73  2000/04/05 02:50:16  robertj
 * Added microseconds to PTime class.
 *
 * Revision 1.72  2000/01/06 14:09:42  robertj
 * Fixed problems with starting up timers,losing up to 10 seconds
 *
 * Revision 1.71  1999/08/22 12:13:43  robertj
 * Fixed warning when using inlines on older GNU compiler
 *
 * Revision 1.70  1999/07/06 04:45:59  robertj
 * Fixed being able to case an unsigned to a PTimeInterval.
 * Improved resolution of PTimer::Tick() to be millisecond accurate.
 *
 * Revision 1.69  1999/03/09 03:08:39  robertj
 * Changes for new documentation system
 *
 * Revision 1.68  1998/11/30 08:57:17  robertj
 * Fixed problem where if += is used on PFilePath, it no longer may be normalised.
 *
 * Revision 1.67  1998/11/19 05:17:35  robertj
 * Added PWaitAndSignal class for easier mutexing.
 *
 * Revision 1.66  1998/11/01 04:56:52  robertj
 * Added BOOl return value to Parse() to indicate there are parameters available.
 *
 * Revision 1.65  1998/10/30 12:23:56  robertj
 * Added ability to get all key values as a dictionary.
 *
 * Revision 1.64  1998/10/30 05:24:30  robertj
 * Added return value to << and >> operators for shifting arguments.
 *
 * Revision 1.63  1998/10/29 05:35:16  robertj
 * Fixed porblem with GetCount() == 0 if do not call Parse() function.
 *
 * Revision 1.62  1998/10/28 03:26:42  robertj
 * Added multi character arguments (-abc style) and options precede parameters mode.
 *
 * Revision 1.61  1998/10/28 00:59:48  robertj
 * New improved argument parsing.
 *
 * Revision 1.60  1998/09/24 07:24:00  robertj
 * Moved structured fiel into separate module so don't need silly implementation file for GNU C.
 *
 * Revision 1.59  1998/09/23 06:21:04  robertj
 * Added open source copyright license.
 *
 * Revision 1.58  1998/01/26 00:31:38  robertj
 * Added functions to get/set 64bit integers from a PConfig.
 * Added multiply and divide operators to PTimeInterval.
 *
 * Revision 1.57  1998/01/04 08:04:27  robertj
 * Changed gmtime and locatime to use operating system specific functions.
 *
 * Revision 1.56  1997/10/03 13:38:26  robertj
 * Fixed race condition on socket close in Select() function.
 *
 * Revision 1.55  1997/08/07 11:58:02  robertj
 * Added ability to get registry data from other applications and anywhere in system registry.
 *
 * Revision 1.54  1997/02/09 03:55:22  robertj
 * Changed PProcess::Current() from pointer to reference.
 *
 * Revision 1.53  1997/01/12 04:21:39  robertj
 * Added IsPast() and IsFuture() functions for time comparison.
 *
 * Revision 1.52  1996/09/14 13:09:23  robertj
 * Major upgrade:
 *   rearranged sockets to help support IPX.
 *   added indirect channel class and moved all protocols to descend from it,
 *   separating the protocol from the low level byte transport.
 *
 * Revision 1.51  1996/05/23 09:59:04  robertj
 * Added mutex to timer list.
 *
 * Revision 1.50  1996/05/18 09:18:25  robertj
 * Added mutex to timer list.
 *
 * Revision 1.49  1996/05/15 10:09:53  robertj
 * Changed millisecond access functions to get 64 bit integer.
 *
 * Revision 1.48  1996/05/09 12:15:34  robertj
 * Resolved C++ problems with 64 bit PTimeInterval for Mac platform.
 *
 * Revision 1.47  1996/04/15 12:33:04  robertj
 * Fixed SetReadTimeout/SetWriteTimeout to use const reference so works with GNU compiler.
 *
 * Revision 1.46  1996/04/15 10:57:57  robertj
 * Moved some functions from INL to serial.cxx so unix linker can make smaller executables.
 *
 * Revision 1.45  1996/04/14 02:53:32  robertj
 * Split serial and pipe channel into separate compilation units for Linux executable size reduction.
 *
 * Revision 1.44  1996/04/09 03:31:33  robertj
 * Fixed bug in config GetTime() cannot use PTime(0) in western hemisphere.
 *
 * Revision 1.43  1996/03/31 08:48:14  robertj
 * Fixed WriteString() so works with sockets.
 *
 * Revision 1.42  1996/03/17 05:43:21  robertj
 * Changed PTimeInterval to 64 bit integer.
 *
 * Revision 1.41  1996/02/25 03:02:45  robertj
 * Added consts to all GetXxxx functions in PConfig.
 *
 * Revision 1.40  1996/02/15 14:47:33  robertj
 * Fixed bugs in time zone compensation (some in the C library).
 *
 * Revision 1.39  1996/02/13 13:06:55  robertj
 * Changed GetTimeZone() so can specify the standard/daylight time.
 *
 * Revision 1.38  1996/02/08 12:12:01  robertj
 * Changed zone parameter in PTime to indicate the time zone as minutes not enum.
 *
 * Revision 1.37  1996/01/28 14:10:12  robertj
 * Added time functions to PConfig.
 *
 * Revision 1.36  1996/01/28 02:51:59  robertj
 * Added assert into all Compare functions to assure comparison between compatible objects.
 *
 * Revision 1.35  1996/01/03 23:15:34  robertj
 * Fixed some PTime bugs.
 *
 * Revision 1.34  1996/01/03 11:09:33  robertj
 * Added Universal Time and Time Zones to PTime class.
 *
 * Revision 1.33  1995/12/23 03:49:46  robertj
 * Chnaged version numbers.
 * Added directory constructor from C string literal.
 *
 * Revision 1.32  1995/12/10 11:32:44  robertj
 * Added extra user information to processes and applications.
 *
 * Revision 1.31  1995/08/12 22:30:05  robertj
 * Work around for  GNU bug: can't have private copy constructor with multiple inheritance.
 *
 * Revision 1.30  1995/07/31 12:15:44  robertj
 * Removed PContainer from PChannel ancestor.
 *
 * Revision 1.29  1995/04/22 00:49:19  robertj
 * Fixed missing common construct code call in edit box constructor.
 *
 * Revision 1.28  1995/03/12 04:41:16  robertj
 * Moved GetHandle() function from PFile to PChannel.
 *
 * Revision 1.27  1995/01/27  11:11:19  robertj
 * Changed single string default constructor to be section name not file name.
 *
 * Revision 1.26  1995/01/18  09:00:40  robertj
 * Added notifiers to timers.
 *
 * Revision 1.25  1995/01/15  04:51:09  robertj
 * Mac compatibility.
 * Added structure function to structured files.
 *
 * Revision 1.24  1995/01/11  09:45:02  robertj
 * Documentation and normalisation.
 *
 * Revision 1.23  1995/01/09  12:34:25  robertj
 * Removed unnecesary return value from I/O functions.
 *
 * Revision 1.22  1994/10/24  00:07:01  robertj
 * Changed PFilePath and PDirectory so descends from either PString or
 *     PCaselessString depending on the platform.
 *
 * Revision 1.21  1994/10/23  04:49:00  robertj
 * Chnaged PDirectory to descend of PString.
 * Added PDirectory Exists() function.
 * Implemented PPipeChannel.
 *
 * Revision 1.20  1994/09/25  10:41:19  robertj
 * Moved PFile::DestroyContents() to cxx file.
 * Added PTextFile constructors for DOS/NT platforms.
 * Added Pipe channel.
 *
 * Revision 1.19  1994/08/21  23:43:02  robertj
 * Added "remove on close" feature for temporary files.
 * Added "force" option to Remove/Rename etc to override write protection.
 * Removed default argument when of PString type (MSC crashes).
 *
 * Revision 1.18  1994/07/27  05:58:07  robertj
 * Synchronisation.
 *
 * Revision 1.17  1994/07/21  12:33:49  robertj
 * Moved cooperative threads to common.
 *
 * Revision 1.16  1994/07/17  10:46:06  robertj
 * Moved file handle to PChannel.
 *
 * Revision 1.15  1994/07/02  03:03:49  robertj
 * Time interval and timer redesign.
 *
 * Revision 1.14  1994/06/25  11:55:15  robertj
 * Unix version synchronisation.
 *
 * Revision 1.13  1994/04/20  12:17:44  robertj
 * assert stuff
 *
 * Revision 1.12  1994/04/01  14:06:48  robertj
 * Text file streams.
 *
 * Revision 1.11  1994/03/07  07:45:40  robertj
 * Major upgrade
 *
 * Revision 1.10  1994/01/13  03:14:51  robertj
 * Added AsString() function to convert a time to a string.
 *
 * Revision 1.9  1994/01/03  04:42:23  robertj
 * Mass changes to common container classes and interactors etc etc etc.
 *
 * Revision 1.8  1993/12/31  06:47:59  robertj
 * Made inlines optional for debugging purposes.
 *
 * Revision 1.7  1993/08/31  03:38:02  robertj
 * Changed PFile::Status to PFile::Info due to X-Windows compatibility.
 * Added copy constructor and assignement operator due to G++ wierdness.
 *
 * Revision 1.6  1993/08/27  18:17:47  robertj
 * Moved a lot of code from MS-DOS platform specific to common files.
 *
 * Revision 1.5  1993/08/21  04:40:19  robertj
 * Added Copy() function.
 *
 * Revision 1.4  1993/08/21  01:50:33  robertj
 * Made Clone() function optional, default will assert if called.
 *
 * Revision 1.3  1993/07/14  12:49:16  robertj
 * Fixed RCS keywords.
 *
 */

#include "ptbuildopts.h"

///////////////////////////////////////////////////////////////////////////////
// PTimeInterval

PINLINE PTimeInterval::PTimeInterval(PInt64 millisecs)
  : milliseconds(millisecs) { }


PINLINE PObject * PTimeInterval::Clone() const
  { return PNEW PTimeInterval(milliseconds); }

PINLINE PInt64 PTimeInterval::GetMilliSeconds() const
  { return milliseconds; }

PINLINE long PTimeInterval::GetSeconds() const
  { return (long)(milliseconds/1000); }

PINLINE long PTimeInterval::GetMinutes() const
  { return (long)(milliseconds/60000); }

PINLINE int PTimeInterval::GetHours() const
  { return (int)(milliseconds/3600000); }

PINLINE int PTimeInterval::GetDays() const
  { return (int)(milliseconds/86400000); }


PINLINE PTimeInterval PTimeInterval::operator-() const
  { return PTimeInterval(-milliseconds); }

PINLINE PTimeInterval PTimeInterval::operator+(const PTimeInterval & t) const
  { return PTimeInterval(milliseconds + t.milliseconds); }

PINLINE PTimeInterval & PTimeInterval::operator+=(const PTimeInterval & t)
  { milliseconds += t.milliseconds; return *this; }

PINLINE PTimeInterval PTimeInterval::operator-(const PTimeInterval & t) const
  { return PTimeInterval(milliseconds - t.milliseconds); }

PINLINE PTimeInterval & PTimeInterval::operator-=(const PTimeInterval & t)
  { milliseconds -= t.milliseconds; return *this; }

PINLINE PTimeInterval PTimeInterval::operator*(int f) const
  { return PTimeInterval(milliseconds * f); }

PINLINE PTimeInterval & PTimeInterval::operator*=(int f)
  { milliseconds *= f; return *this; }

PINLINE PTimeInterval PTimeInterval::operator/(int f) const
  { return PTimeInterval(milliseconds / f); }

PINLINE PTimeInterval & PTimeInterval::operator/=(int f)
  { milliseconds /= f; return *this; }


PINLINE bool PTimeInterval::operator==(const PTimeInterval & t) const
  { return milliseconds == t.milliseconds; }

PINLINE bool PTimeInterval::operator!=(const PTimeInterval & t) const
  { return milliseconds != t.milliseconds; }

PINLINE bool PTimeInterval::operator> (const PTimeInterval & t) const
  { return milliseconds > t.milliseconds; }

PINLINE bool PTimeInterval::operator>=(const PTimeInterval & t) const
  { return milliseconds >= t.milliseconds; }

PINLINE bool PTimeInterval::operator< (const PTimeInterval & t) const
  { return milliseconds < t.milliseconds; }

PINLINE bool PTimeInterval::operator<=(const PTimeInterval & t) const
  { return milliseconds <= t.milliseconds; }

PINLINE bool PTimeInterval::operator==(long msecs) const
  { return (long)milliseconds == msecs; }

PINLINE bool PTimeInterval::operator!=(long msecs) const
  { return (long)milliseconds != msecs; }

PINLINE bool PTimeInterval::operator> (long msecs) const
  { return (long)milliseconds > msecs; }

PINLINE bool PTimeInterval::operator>=(long msecs) const
  { return (long)milliseconds >= msecs; }

PINLINE bool PTimeInterval::operator< (long msecs) const
  { return (long)milliseconds < msecs; }

PINLINE bool PTimeInterval::operator<=(long msecs) const
  { return (long)milliseconds <= msecs; }


///////////////////////////////////////////////////////////////////////////////
// PTime

PINLINE PObject * PTime::Clone() const
  { return PNEW PTime(theTime, microseconds); }

PINLINE void PTime::PrintOn(ostream & strm) const
  { strm << AsString(); }

PINLINE BOOL PTime::IsValid() const
  { return theTime > 46800; }

PINLINE PInt64 PTime::GetTimestamp() const
  { return theTime*(PInt64)1000000 + microseconds; }

PINLINE time_t PTime::GetTimeInSeconds() const
  { return theTime; }

PINLINE long PTime::GetMicrosecond() const
  { return microseconds; }

PINLINE int PTime::GetSecond() const
  { struct tm ts; return os_localtime(&theTime, &ts)->tm_sec; }

PINLINE int PTime::GetMinute() const
  { struct tm ts; return os_localtime(&theTime, &ts)->tm_min; }

PINLINE int PTime::GetHour() const
  { struct tm ts; return os_localtime(&theTime, &ts)->tm_hour; }

PINLINE int PTime::GetDay() const
  { struct tm ts; return os_localtime(&theTime, &ts)->tm_mday; }

PINLINE PTime::Months PTime::GetMonth() const
  { struct tm ts; return (Months)(os_localtime(&theTime, &ts)->tm_mon+January); }

PINLINE int PTime::GetYear() const
  { struct tm ts; return os_localtime(&theTime, &ts)->tm_year+1900; }

PINLINE PTime::Weekdays PTime::GetDayOfWeek() const
  { struct tm ts; return (Weekdays)os_localtime(&theTime, &ts)->tm_wday; }

PINLINE int PTime::GetDayOfYear() const
  { struct tm ts; return os_localtime(&theTime, &ts)->tm_yday; }

PINLINE BOOL PTime::IsPast() const
  { return theTime < time(NULL); }

PINLINE BOOL PTime::IsFuture() const
  { return theTime > time(NULL); }


PINLINE PString PTime::AsString(const PString & format, int zone) const
  { return AsString((const char *)format, zone); }

PINLINE int PTime::GetTimeZone() 
  { return GetTimeZone(IsDaylightSavings() ? DaylightSavings : StandardTime); }


///////////////////////////////////////////////////////////////////////////////
// PTimer

PINLINE BOOL PTimer::IsRunning() const
  { return state == Starting || state == Running; }

PINLINE BOOL PTimer::IsPaused() const
  { return state == Paused; }

PINLINE const PTimeInterval & PTimer::GetResetTime() const
  { return resetTime; }

PINLINE const PNotifier & PTimer::GetNotifier() const
  { return callback; }

PINLINE void PTimer::SetNotifier(const PNotifier & func)
  { callback = func; }


///////////////////////////////////////////////////////////////////////////////

PINLINE PChannelStreamBuffer::PChannelStreamBuffer(const PChannelStreamBuffer & sbuf)
  : channel(sbuf.channel) { }

PINLINE PChannelStreamBuffer &
          PChannelStreamBuffer::operator=(const PChannelStreamBuffer & sbuf)
  { channel = sbuf.channel; return *this; }

PINLINE PChannel::PChannel(const PChannel &) : iostream(cout.rdbuf())
  { PAssertAlways("Cannot copy channels"); }

PINLINE PChannel & PChannel::operator=(const PChannel &)
  { PAssertAlways("Cannot assign channels"); return *this; }

PINLINE void PChannel::SetReadTimeout(const PTimeInterval & time)
  { readTimeout = time; }

PINLINE PTimeInterval PChannel::GetReadTimeout() const
  { return readTimeout; }

PINLINE void PChannel::SetWriteTimeout(const PTimeInterval & time)
  { writeTimeout = time; }

PINLINE PTimeInterval PChannel::GetWriteTimeout() const
  { return writeTimeout; }

PINLINE int PChannel::GetHandle() const
  { return os_handle; }

PINLINE PChannel::Errors PChannel::GetErrorCode(ErrorGroup group) const
  { return lastErrorCode[group]; }

PINLINE int PChannel::GetErrorNumber(ErrorGroup group) const
  { return lastErrorNumber[group]; }

PINLINE void PChannel::AbortCommandString()
  { abortCommandString = TRUE; }


///////////////////////////////////////////////////////////////////////////////
// PIndirectChannel

PINLINE PIndirectChannel::~PIndirectChannel()
  { Close(); }

PINLINE PChannel * PIndirectChannel::GetReadChannel() const
  { return readChannel; }

PINLINE PChannel * PIndirectChannel::GetWriteChannel() const
  { return writeChannel; }


///////////////////////////////////////////////////////////////////////////////
// PDirectory

PINLINE PDirectory::PDirectory()
  : PFilePathString(".") { Construct(); }

PINLINE PDirectory::PDirectory(const char * cpathname)  
  : PFilePathString(cpathname) { Construct(); }
  
PINLINE PDirectory::PDirectory(const PString & pathname)
  : PFilePathString(pathname) { Construct(); }
  
PINLINE PDirectory & PDirectory::operator=(const PString & str)
  { AssignContents(PDirectory(str)); return *this; }

PINLINE PDirectory & PDirectory::operator=(const char * cstr)
  { AssignContents(PDirectory(cstr)); return *this; }


PINLINE void PDirectory::DestroyContents()
  { Close(); PFilePathString::DestroyContents(); }

PINLINE BOOL PDirectory::Exists() const
  { return Exists(*this); }

PINLINE BOOL PDirectory::Change() const
  { return Change(*this); }

PINLINE BOOL PDirectory::Create(int perm) const
  { return Create(*this, perm); }

PINLINE BOOL PDirectory::Remove()
  { Close(); return Remove(*this); }


///////////////////////////////////////////////////////////////////////////////

PINLINE PFilePath::PFilePath()
  { }

PINLINE PFilePath::PFilePath(const PFilePath & path)
  : PFilePathString(path) { }

PINLINE PFilePath & PFilePath::operator=(const PFilePath & path)
  { AssignContents(path); return *this; }

PINLINE PFilePath & PFilePath::operator=(const PString & str)
  { AssignContents(str); return *this; }

PINLINE PFilePath & PFilePath::operator=(const char * cstr)
  { AssignContents(PString(cstr)); return *this; }

PINLINE PFilePath & PFilePath::operator+=(const PString & str)
  { AssignContents(*this + str); return *this; }

PINLINE PFilePath & PFilePath::operator+=(const char * cstr)
  { AssignContents(*this + cstr); return *this; }


///////////////////////////////////////////////////////////////////////////////

PINLINE PFile::PFile()
  { os_handle = -1; removeOnClose = FALSE; }

PINLINE PFile::PFile(OpenMode mode, int opts)
  { os_handle = -1; removeOnClose = FALSE; Open(mode, opts); }

PINLINE PFile::PFile(const PFilePath & name, OpenMode mode, int opts)
  { os_handle = -1; removeOnClose = FALSE; Open(name, mode, opts); }


PINLINE BOOL PFile::Exists() const
  { return Exists(path); }

PINLINE BOOL PFile::Access(OpenMode mode)
  { return ConvertOSError(Access(path, mode) ? 0 : -1); }

PINLINE BOOL PFile::Remove(BOOL force)
  { Close(); return ConvertOSError(Remove(path, force) ? 0 : -1); }

PINLINE BOOL PFile::Copy(const PFilePath & newname, BOOL force)
  { return ConvertOSError(Copy(path, newname, force) ? 0 : -1); }

PINLINE BOOL PFile::GetInfo(PFileInfo & info)
  { return ConvertOSError(GetInfo(path, info) ? 0 : -1); }

PINLINE BOOL PFile::SetPermissions(int permissions)
  { return ConvertOSError(SetPermissions(path, permissions) ? 0 : -1); }


PINLINE const PFilePath & PFile::GetFilePath() const
  { return path; }
      

PINLINE PString PFile::GetName() const
  { return path; }

PINLINE off_t PFile::GetPosition() const
  { return _lseek(GetHandle(), 0, SEEK_CUR); }


///////////////////////////////////////////////////////////////////////////////

PINLINE PTextFile::PTextFile()
  { }

PINLINE PTextFile::PTextFile(OpenMode mode, int opts)
  { Open(mode, opts); }

PINLINE PTextFile::PTextFile(const PFilePath & name, OpenMode mode, int opts)
  { Open(name, mode, opts); }


///////////////////////////////////////////////////////////////////////////////
// PConfig

#ifdef P_CONFIG_FILE

PINLINE PConfig::PConfig(Source src)
  : defaultSection("Options") { Construct(src, "", ""); }

PINLINE PConfig::PConfig(Source src, const PString & appname)
  : defaultSection("Options") { Construct(src, appname, ""); }

PINLINE PConfig::PConfig(Source src, const PString & appname, const PString & manuf)
  : defaultSection("Options") { Construct(src, appname, manuf); }

PINLINE PConfig::PConfig(const PString & section, Source src)
  : defaultSection(section) { Construct(src, "", ""); }

PINLINE PConfig::PConfig(const PString & section, Source src, const PString & appname)
  : defaultSection(section) { Construct(src, appname, ""); }

PINLINE PConfig::PConfig(const PString & section,
                         Source src,
                         const PString & appname,
                         const PString & manuf)
  : defaultSection(section) { Construct(src, appname, manuf); }

PINLINE PConfig::PConfig(const PFilePath & filename, const PString & section)
  : defaultSection(section) { Construct(filename); }

PINLINE void PConfig::SetDefaultSection(const PString & section)
  { defaultSection = section; }

PINLINE PString PConfig::GetDefaultSection() const
  { return defaultSection; }

PINLINE PStringList PConfig::GetKeys() const
  { return GetKeys(defaultSection); }

PINLINE PStringToString PConfig::GetAllKeyValues() const
  { return GetAllKeyValues(defaultSection); }

PINLINE void PConfig::DeleteSection()
  { DeleteSection(defaultSection); }

PINLINE void PConfig::DeleteKey(const PString & key)
  { DeleteKey(defaultSection, key); }

PINLINE BOOL PConfig::HasKey(const PString & key) const
  { return HasKey(defaultSection, key); }

PINLINE PString PConfig::GetString(const PString & key) const
  { return GetString(defaultSection, key, PString()); }

PINLINE PString PConfig::GetString(const PString & key, const PString & dflt) const
  { return GetString(defaultSection, key, dflt); }

PINLINE void PConfig::SetString(const PString & key, const PString & value)
  { SetString(defaultSection, key, value); }

PINLINE BOOL PConfig::GetBoolean(const PString & key, BOOL dflt) const
  { return GetBoolean(defaultSection, key, dflt); }

PINLINE void PConfig::SetBoolean(const PString & key, BOOL value)
  { SetBoolean(defaultSection, key, value); }

PINLINE long PConfig::GetInteger(const PString & key, long dflt) const
  { return GetInteger(defaultSection, key, dflt); }

PINLINE void PConfig::SetInteger(const PString & key, long value)
  { SetInteger(defaultSection, key, value); }

PINLINE PInt64 PConfig::GetInt64(const PString & key, PInt64 dflt) const
  { return GetInt64(defaultSection, key, dflt); }

PINLINE void PConfig::SetInt64(const PString & key, PInt64 value)
  { SetInt64(defaultSection, key, value); }

PINLINE double PConfig::GetReal(const PString & key, double dflt) const
  { return GetReal(defaultSection, key, dflt); }

PINLINE void PConfig::SetReal(const PString & key, double value)
  { SetReal(defaultSection, key, value); }

PINLINE PTime PConfig::GetTime(const PString & key) const
  { return GetTime(defaultSection, key); }

PINLINE PTime PConfig::GetTime(const PString & key, const PTime & dflt) const
  { return GetTime(defaultSection, key, dflt); }

PINLINE void PConfig::SetTime(const PString & key, const PTime & value)
  { SetTime(defaultSection, key, value); }


#endif // P_CONFIG_FILE


///////////////////////////////////////////////////////////////////////////////
// PArgList

PINLINE void PArgList::SetArgs(int argc, char ** argv)
  { SetArgs(PStringArray(argc, argv)); }

PINLINE BOOL PArgList::Parse(const PString & theArgumentSpec, BOOL optionsBeforeParams)
  { return Parse((const char *)theArgumentSpec, optionsBeforeParams); }

PINLINE BOOL PArgList::HasOption(char option) const
  { return GetOptionCount(option) != 0; }

PINLINE BOOL PArgList::HasOption(const char * option) const
  { return GetOptionCount(option) != 0; }

PINLINE BOOL PArgList::HasOption(const PString & option) const
  { return GetOptionCount(option) != 0; }

PINLINE PINDEX PArgList::GetCount() const
  { return parameterIndex.GetSize()-shift; }

PINLINE PString PArgList::operator[](PINDEX num) const
  { return GetParameter(num); }

PINLINE PArgList & PArgList::operator<<(int sh)
  { Shift(sh); return *this; }

PINLINE PArgList & PArgList::operator>>(int sh)
  { Shift(-sh); return *this; }

///////////////////////////////////////////////////////////////////////////////
// PProcess

PINLINE PArgList & PProcess::GetArguments()
  { return arguments; }

PINLINE const PString & PProcess::GetManufacturer() const
  { return manufacturer; }

PINLINE const PString & PProcess::GetName() const
  { return productName; }

PINLINE const PFilePath & PProcess::GetFile() const
  { return executableFile; }

PINLINE int PProcess::GetMaxHandles() const
  { return maxHandles; }

PINLINE PTimerList * PProcess::GetTimerList()
  { return &timers; }

PINLINE void PProcess::SetTerminationValue(int value)
  { terminationValue = value; }

PINLINE int PProcess::GetTerminationValue() const
  { return terminationValue; }



// End Of File ///////////////////////////////////////////////////////////////
