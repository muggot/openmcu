/*
 * ptime.h
 *
 * Time and date class.
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
 * $Log: ptime.h,v $
 * Revision 1.35  2005/11/25 03:43:47  csoutheren
 * Fixed function argument comments to be compatible with Doxygen
 *
 * Revision 1.34  2003/09/17 05:41:59  csoutheren
 * Removed recursive includes
 *
 * Revision 1.33  2003/09/17 01:18:02  csoutheren
 * Removed recursive include file system and removed all references
 * to deprecated coooperative threading support
 *
 * Revision 1.32  2002/12/10 04:45:14  robertj
 * Added support in PTime for ISO 8601 format.
 *
 * Revision 1.31  2002/10/29 00:07:03  robertj
 * Added IsValid() function to indicate that a PTime is set correctly.
 *
 * Revision 1.30  2002/09/16 01:08:59  robertj
 * Added #define so can select if #pragma interface/implementation is used on
 *   platform basis (eg MacOS) rather than compiler, thanks Robert Monaghan.
 *
 * Revision 1.29  2001/05/22 12:49:32  robertj
 * Did some seriously wierd rewrite of platform headers to eliminate the
 *   stupid GNU compiler warning about braces not matching.
 *
 * Revision 1.28  2000/04/29 08:14:52  robertj
 * Added some documentation on string formats that can be parsed into a time.
 *
 * Revision 1.27  2000/04/29 04:49:00  robertj
 * Added microseconds to string output.
 *
 * Revision 1.26  2000/04/05 02:50:16  robertj
 * Added microseconds to PTime class.
 *
 * Revision 1.25  1999/03/09 02:59:50  robertj
 * Changed comments to doc++ compatible documentation.
 *
 * Revision 1.24  1999/02/16 08:11:10  robertj
 * MSVC 6.0 compatibility changes.
 *
 * Revision 1.23  1998/09/23 06:21:15  robertj
 * Added open source copyright license.
 *
 * Revision 1.22  1998/01/04 08:04:27  robertj
 * Changed gmtime and locatime to use operating system specific functions.
 *
 * Revision 1.21  1997/01/12 04:21:40  robertj
 * Added IsPast() and IsFuture() functions for time comparison.
 *
 * Revision 1.20  1996/05/09 12:16:06  robertj
 * Fixed syntax error found by Mac platform.
 *
 * Revision 1.19  1996/02/15 14:47:34  robertj
 * Fixed bugs in time zone compensation (some in the C library).
 *
 * Revision 1.18  1996/02/13 12:58:43  robertj
 * Changed GetTimeZone() so can specify standard/daylight time.
 *
 * Revision 1.17  1996/02/08 12:13:03  robertj
 * Changed zone parameter in PTime to indicate the time zone as minutes not enum.
 * Staticised some functions that are system global.
 *
 * Revision 1.16  1996/02/03 11:04:52  robertj
 * Added string constructor for times, parses date/time from string.
 *
 * Revision 1.15  1996/01/03 11:09:34  robertj
 * Added Universal Time and Time Zones to PTime class.
 *
 * Revision 1.14  1995/06/17 11:13:10  robertj
 * Documentation update.
 *
 * Revision 1.13  1995/03/14 12:42:18  robertj
 * Updated documentation to use HTML codes.
 *
 * Revision 1.12  1995/01/11  09:45:12  robertj
 * Documentation and normalisation.
 *
 * Revision 1.11  1995/01/09  12:34:05  robertj
 * Removed unnecesary return value from I/O functions.
 *
 * Revision 1.10  1994/08/23  11:32:52  robertj
 * Oops
 *
 * Revision 1.9  1994/08/22  00:46:48  robertj
 * Added pragma fro GNU C++ compiler.
 *
 * Revision 1.8  1994/07/27  05:58:07  robertj
 * Synchronisation.
 *
 * Revision 1.7  1994/06/25  11:55:15  robertj
 * Unix version synchronisation.
 *
 * Revision 1.6  1994/01/13  03:16:09  robertj
 * Added function to return time as a string.
 *
 * Revision 1.5  1994/01/03  04:42:23  robertj
 * Mass changes to common container classes and interactors etc etc etc.
 *
 * Revision 1.4  1993/12/31  06:45:38  robertj
 * Made inlines optional for debugging purposes.
 *
 * Revision 1.3  1993/08/27  18:17:47  robertj
 * Made time functions common to all platforms.
 * Moved timer resolution function to PTimeInterval wher it belongs.
 *
 * Revision 1.2  1993/07/14  12:49:16  robertj
 * Fixed RCS keywords.
 *
 */

#ifndef _PTIME
#define _PTIME

#ifdef P_USE_PRAGMA
#pragma interface
#endif


#include <time.h>


///////////////////////////////////////////////////////////////////////////////
// System time and date class

class PTimeInterval;


/**This class defines an absolute time and date. It has a number of time and
   date rendering and manipulation functions. It is based on the standard C
   library functions for time. Thus it is based on a number of seconds since
   1 January 1970.
 */
class PTime : public PObject
{
  PCLASSINFO(PTime, PObject);

  public:
  /**@name Construction */
  //@{
    /** Time Zone special codes. The value for a time zone is usually in minutes
        from UTC, this enum are special values for specific areas.
      */
    enum {
      /// Universal Coordinated Time.
      UTC   = 0,
      /// Greenwich Mean Time, effectively UTC.
      GMT   = UTC,
      /// Local Time.
      Local = 9999
    };

    /**Create a time object instance.
       This initialises the time with the current time in the current time zone.
     */
    PTime();

    /**Create a time object instance.
       This initialises the time to the specified time.
     */
    PTime(
      time_t tsecs,          ///< Time in seconds since 00:00:00 1/1/70 UTC
      long usecs = 0
    ) { theTime = tsecs; microseconds = usecs; }

    /**Create a time object instance.
       This initialises the time to the specified time, parsed from the
       string. The string may be in many different formats, for example:
          "5/03/1999 12:34:56"
          "15/06/1999 12:34:56"
          "15/06/01 12:34:56 PST"
          "5/06/02 12:34:56"
          "5/23/1999 12:34am"
          "5/23/00 12:34am"
          "1999/23/04 12:34:56"
          "Mar 3, 1999 12:34pm"
          "3 Jul 2004 12:34pm"
          "12:34:56 5 December 1999"
          "10 minutes ago"
          "2 weeks"
     */
    PTime(
      const PString & str   ///< Time and data as a string
    );

    /**Create a time object instance.
       This initialises the time to the specified time.
     */
    PTime(
      int second,           ///< Second from 0 to 59.
      int minute,           ///< Minute from 0 to 59.
      int hour,             ///< Hour from 0 to 23.
      int day,              ///< Day of month from 1 to 31.
      int month,            ///< Month from 1 to 12.
      int year,             ///< Year from 1970 to 2038
      int tz = Local        ///< local time or UTC
    );
  //@}

  /**@name Overrides from class PObject */
  //@{
    /**Create a copy of the time on the heap. It is the responsibility of the
       caller to delete the created object.
    
       @return
       pointer to new time.
     */
    PObject * Clone() const;

    /**Determine the relative rank of the specified times. This ranks the
       times as you would expect.
       
       @return
       rank of the two times.
     */
    virtual Comparison Compare(
      const PObject & obj   ///< Other time to compare against.
    ) const;

    /**Output the time to the stream. This uses the #AsString()# function
       with the #ShortDateTime# parameter.
     */
    virtual void PrintOn(
      ostream & strm    ///< Stream to output the time to.
    ) const;

    /**Input the time from the specified stream. If a parse error occurs the
       time is set to the current time. The string may be in many different
       formats, for example:
          "5/03/1999 12:34:56"
          "15/06/1999 12:34:56"
          "15/06/01 12:34:56 PST"
          "5/06/02 12:34:56"
          "5/23/1999 12:34am"
          "5/23/00 12:34am"
          "1999/23/04 12:34:56"
          "Mar 3, 1999 12:34pm"
          "3 Jul 2004 12:34pm"
          "12:34:56 5 December 1999"
          "10 minutes ago"
          "2 weeks"
     */
    virtual void ReadFrom(
      istream & strm    ///< Stream to input the time from.
    );
  //@}

  /**@name Access functions */
  //@{
    /**Determine if the timestamp is valid.
       This will return TRUE if the timestamp can be represented as a time
       in the epoch. The epoch is the 1st January 1970.

       In practice this means the time is > 13 hours to allow for time zones.
      */
    BOOL IsValid() const;

    /**Get the total microseconds since the epoch. The epoch is the 1st
       January 1970.

       @return
       microseconds.
     */
    PInt64 GetTimestamp() const;

    /**Get the total seconds since the epoch. The epoch is the 1st
       January 1970.

       @return
       seconds.
     */
    time_t GetTimeInSeconds() const;

    /**Get the microsecond part of the time.

       @return
       integer in range 0..999999.
     */
    long GetMicrosecond() const;

    /**Get the second of the time.

       @return
       integer in range 0..59.
     */
    int GetSecond() const;

    /**Get the minute of the time.

       @return
       integer in range 0..59.
     */
    int GetMinute() const;

    /**Get the hour of the time.

       @return
       integer in range 0..23.
     */
    int GetHour() const;

    /**Get the day of the month of the date.

       @return
       integer in range 1..31.
     */
    int GetDay() const;

    /// Month codes.
    enum Months {
      January = 1,
      February,
      March,
      April,
      May,
      June,
      July,
      August,
      September,
      October,
      November,
      December
    };

    /**Get the month of the date.

       @return
       enum for month.
     */
    Months GetMonth() const;

    /**Get the year of the date.

       @return
       integer in range 1970..2038.
     */
    int GetYear() const;

    /// Days of the week.
    enum Weekdays {
      Sunday,
      Monday,
      Tuesday,
      Wednesday,
      Thursday,
      Friday,
      Saturday
    };

    /**Get the day of the week of the date.
    
       @return
       enum for week days with 0=Sun, 1=Mon, ..., 6=Sat.
     */
    Weekdays GetDayOfWeek() const;

    /**Get the day in the year of the date.
    
       @return
       integer from 1..366.
     */
    int GetDayOfYear() const;

    /**Determine if the time is in the past or in the future.

       @return
       TRUE if time is before the current real time.
     */
    BOOL IsPast() const;

    /**Determine if the time is in the past or in the future.

       @return
       TRUE if time is after the current real time.
     */
    BOOL IsFuture() const;
  //@}

  /**@name Time Zone configuration functions */
  //@{
    /**Get flag indicating daylight savings is current.
    
       @return
       TRUE if daylight savings time is active.
     */
    static BOOL IsDaylightSavings();

    /// Flag for time zone adjustment on daylight savings.
    enum TimeZoneType {
      StandardTime,
      DaylightSavings
    };

    /// Get the time zone offset in minutes.
    static int GetTimeZone();
    /**Get the time zone offset in minutes.
       This is the number of minutes to add to UTC (previously known as GMT) to
       get the local time. The first form automatically adjusts for daylight
       savings time, whilst the second form returns the specified time.

       @return
       Number of minutes.
     */
    static int GetTimeZone(
       TimeZoneType type  ///< Daylight saving or standard time.
    );

    /**Get the text identifier for the local time zone .

       @return
       Time zone identifier string.
     */
    static PString GetTimeZoneString(
       TimeZoneType type = StandardTime ///< Daylight saving or standard time.
    );
  //@}

  /**@name Operations */
  //@{
    /**Add the interval to the time to yield a new time.
    
       @return
       Time altered by the interval.
     */
    PTime operator+(
      const PTimeInterval & time   ///< Time interval to add to the time.
    ) const;

    /**Add the interval to the time changing the instance.
    
       @return
       reference to the current time instance.
     */
    PTime & operator+=(
      const PTimeInterval & time   ///< Time interval to add to the time.
    );

    /**Calculate the difference between two times to get a time interval.
    
       @return
       Time intervale difference between the times.
     */
    PTimeInterval operator-(
      const PTime & time   ///< Time to subtract from the time.
    ) const;

    /**Subtract the interval from the time to yield a new time.
    
       @return
       Time altered by the interval.
     */
    PTime operator-(
      const PTimeInterval & time   ///< Time interval to subtract from the time.
    ) const;

    /**Subtract the interval from the time changing the instance.

       @return
       reference to the current time instance.
     */
    PTime & operator-=(
      const PTimeInterval & time   ///< Time interval to subtract from the time.
    );
  //@}

  /**@name String conversion functions */
  //@{
    /// Standard time formats for string representations of a time and date.
    enum TimeFormat {
      /// Internet standard format.
      RFC1123,
      /// Short form ISO standard format.
      ShortISO8601,
      /// Long form ISO standard format.
      LongISO8601,
      /// Date with weekday, full month names and time with seconds.
      LongDateTime,
      /// Date with weekday, full month names and no time.
      LongDate,
      /// Time with seconds.
      LongTime,
      /// Date with abbreviated month names and time without seconds.
      MediumDateTime,
      /// Date with abbreviated month names and no time.
      MediumDate,
      /// Date with numeric month name and time without seconds.
      ShortDateTime,
      /// Date with numeric month and no time.
      ShortDate,
      /// Time without seconds.
      ShortTime,
      NumTimeStrings
    };

    /** Convert the time to a string representation. */
    PString AsString(
      TimeFormat formatCode = RFC1123,  ///< Standard format for time.
      int zone = Local                  ///< Time zone for the time.
    ) const;

    /** Convert the time to a string representation. */
    PString AsString(
      const PString & formatStr, ///< Arbitrary format string for time.
      int zone = Local           ///< Time zone for the time.
    ) const;
    /* Convert the time to a string using the format code or string as a
       formatting template. The special characters in the formatting string
       are:
\begin{description}
       \item[h]         hour without leading zero
       \item[hh]        hour with leading zero
       \item[m]         minute without leading zero
       \item[mm]        minute with leading zero
       \item[s]         second without leading zero
       \item[ss]        second with leading zero
       \item[u]         tenths of second
       \item[uu]        hundedths of second with leading zero
       \item[uuu]       millisecond with leading zeros
       \item[uuuu]      microsecond with leading zeros
       \item[a]         the am/pm string
       \item[w/ww/www]  abbreviated day of week name
       \item[wwww]      full day of week name
       \item[d]         day of month without leading zero
       \item[dd]        day of month with leading zero
       \item[M]         month of year without leading zero
       \item[MM]        month of year with leading zero
       \item[MMM]       month of year as abbreviated text
       \item[MMMM]      month of year as full text
       \item[y/yy]      year without century
       \item[yyy/yyyy]  year with century
       \item[z]         the time zone description
\end{description}

       All other characters are copied to the output string unchanged.
       
       Note if there is an 'a' character in the string, the hour will be in 12
       hour format, otherwise in 24 hour format.
     */
    PString AsString(
      const char * formatPtr,    ///< Arbitrary format C string pointer for time.
      int zone = Local           ///< Time zone for the time.
    ) const;
  //@}

  /**@name Internationalisation functions */
  //@{
    /**Get the internationalised time separator.
    
       @return
       string for time separator.
     */
    static PString GetTimeSeparator();

    /**Get the internationalised time format: AM/PM or 24 hour.
    
       @return
       TRUE is 12 hour, FALSE if 24 hour.
     */
    static BOOL GetTimeAMPM();

    /**Get the internationalised time AM string.
    
       @return
       string for AM.
     */
    static PString GetTimeAM();

    /**Get the internationalised time PM string.
    
       @return
       string for PM.
     */
    static PString GetTimePM();

    /// Flag for returning language dependent string names.
    enum NameType {
      FullName,
      Abbreviated
    };

    /**Get the internationalised day of week day name (0=Sun etc).
    
       @return
       string for week day.
     */
    static PString GetDayName(
      Weekdays dayOfWeek,       ///< Code for day of week.
      NameType type = FullName  ///< Flag for abbreviated or full name.
    );

    /**Get the internationalised date separator.
    
       @return
       string for date separator.
     */
    static PString GetDateSeparator();

    /**Get the internationalised month name string (1=Jan etc).
    
       @return
       string for month.
     */
    static PString GetMonthName(
      Months month,             ///< Code for month in year.
      NameType type = FullName  ///< Flag for abbreviated or full name.
    );

    /// Possible orders for date components.
    enum DateOrder {
      MonthDayYear,   ///< Date is ordered month then day then year.
      DayMonthYear,   ///< Date is ordered day then month then year.
      YearMonthDay    ///< Date is ordered year then day month then day.
    };

    /**Return the internationalised date order.
    
       @return
       code for date ordering.
     */
    static DateOrder GetDateOrder();
  //@}

    static struct tm * os_localtime(const time_t * clock, struct tm * t);
    static struct tm * os_gmtime(const time_t * clock, struct tm * t);
    /*
      Threadsafe version of localtime library call.
      We could make these calls non-static if we could put the struct tm inside the
      instance. But these calls are usually made with const objects so that's not possible,
      and we would require per-thread storage otherwise. Sigh...
    */

  protected:
    // Member variables
    /// Number of seconds since 1 January 1970.
    time_t theTime;
    long   microseconds;


// Include platform dependent part of class
#ifdef _WIN32
#include "msos/ptlib/ptime.h"
#else
#include "unix/ptlib/ptime.h"
#endif
};

#endif

// End Of File ///////////////////////////////////////////////////////////////
