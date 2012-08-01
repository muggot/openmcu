#include <ptlib.h>

//////////////////////////////////////////////////////
//
//  PTime
//

BOOL PTime::GetTimeAMPM()
{
#if defined(P_USE_LANGINFO)
  return strstr(nl_langinfo(T_FMT), "%p") != NULL;
#elif defined(P_USE_STRFTIME)
  char buf[30];
  struct tm t;
  memset(&t, 0, sizeof(t));
  t.tm_hour = 20;
  t.tm_min = 12;
  t.tm_sec = 11;
  strftime(buf, sizeof(buf), "%X", &t);
  return strstr(buf, "20") != NULL;
#else
#ifdef __NUCLEUS_PLUS__
#pragma message ("No RTC")
PAssertAlways("No RTC");
#else
#warning No AMPM implementation
#endif
  return FALSE;
#endif
}


PString PTime::GetTimeAM()
{
#if defined(P_USE_LANGINFO)
  return PString(nl_langinfo(AM_STR));
#elif defined(P_USE_STRFTIME)
  char buf[30];
  struct tm t;
  memset(&t, 0, sizeof(t));
  t.tm_hour = 10;
  t.tm_min = 12;
  t.tm_sec = 11;
  strftime(buf, sizeof(buf), "%p", &t);
  return buf;
#else
#ifdef __NUCLEUS_PLUS__
#pragma message ("No RTC")
PAssertAlways("No RTC");
#else
#warning Using default AM string
#endif
  return "AM";
#endif
}


PString PTime::GetTimePM()
{
#if defined(P_USE_LANGINFO)
  return PString(nl_langinfo(PM_STR));
#elif defined(P_USE_STRFTIME)
  char buf[30];
  struct tm t;
  memset(&t, 0, sizeof(t));
  t.tm_hour = 20;
  t.tm_min = 12;
  t.tm_sec = 11;
  strftime(buf, sizeof(buf), "%p", &t);
  return buf;
#else
#ifdef __NUCLEUS_PLUS__
#pragma message ("No RTC")
PAssertAlways("No RTC");
#else
#warning Using default PM string
#endif
  return "PM";
#endif
}


PString PTime::GetTimeSeparator()
{
#if defined(P_LINUX) || defined(P_HPUX9) || defined(P_SOLARIS)
#  if defined(P_USE_LANGINFO)
     char * p = nl_langinfo(T_FMT);
#  elif defined(P_LINUX)
     char * p = _time_info->time; 
#  endif
  char buffer[2];
  while (*p == '%' || isalpha(*p))
    p++;
  buffer[0] = *p;
  buffer[1] = '\0';
  return PString(buffer);
#elif defined(P_USE_STRFTIME)
  char buf[30];
  struct tm t;
  memset(&t, 0, sizeof(t));
  t.tm_hour = 10;
  t.tm_min = 11;
  t.tm_sec = 12;
  strftime(buf, sizeof(buf), "%X", &t);
  char * sp = strstr(buf, "11") + 2;
  char * ep = sp;
  while (*ep != '\0' && !isdigit(*ep))
    ep++;
  return PString(sp, ep-sp);
#else
#ifdef __NUCLEUS_PLUS__
#pragma message ("Defaulting time separator")
PAssertAlways("Defaulting time separator");
#else
#warning Using default time separator
#endif
  return ":";
#endif
}

PTime::DateOrder PTime::GetDateOrder()
{
#if defined(P_USE_LANGINFO) || defined(P_LINUX)
#  if defined(P_USE_LANGINFO)
     char * p = nl_langinfo(D_FMT);
#  else
     char * p = _time_info->date; 
#  endif

  while (*p == '%')
    p++;
  switch (tolower(*p)) {
    case 'd':
      return DayMonthYear;
    case 'y':
      return YearMonthDay;
    case 'm':
    default:
      break;
  }
  return MonthDayYear;

#elif defined(P_USE_STRFTIME)
  char buf[30];
  struct tm t;
  memset(&t, 0, sizeof(t));
  t.tm_mday = 22;
  t.tm_mon = 10;
  t.tm_year = 99;
  strftime(buf, sizeof(buf), "%x", &t);
  char * day_pos = strstr(buf, "22");
  char * mon_pos = strstr(buf, "11");
  char * yr_pos = strstr(buf, "99");
  if (yr_pos < day_pos)
    return YearMonthDay;
  if (day_pos < mon_pos)
    return DayMonthYear;
  return MonthDayYear;
#else
#ifdef __NUCLEUS_PLUS__
#pragma message ("Defaulting date order")
PAssertAlways("Defaulting date order");
#else
#warning Using default date order
#endif
  return DayMonthYear;
#endif
}

PString PTime::GetDateSeparator()
{
#if defined(P_USE_LANGINFO) || defined(P_LINUX)
#  if defined(P_USE_LANGINFO)
     char * p = nl_langinfo(D_FMT);
#  else
     char * p = _time_info->date; 
#  endif
  char buffer[2];
  while (*p == '%' || isalpha(*p))
    p++;
  buffer[0] = *p;
  buffer[1] = '\0';
  return PString(buffer);
#elif defined(P_USE_STRFTIME)
  char buf[30];
  struct tm t;
  memset(&t, 0, sizeof(t));
  t.tm_mday = 22;
  t.tm_mon = 10;
  t.tm_year = 99;
  strftime(buf, sizeof(buf), "%x", &t);
  char * sp = strstr(buf, "22") + 2;
  char * ep = sp;
  while (*ep != '\0' && !isdigit(*ep))
    ep++;
  return PString(sp, ep-sp);
#else
#ifdef __NUCLEUS_PLUS__
#pragma message ("Defaulting date separator")
PAssertAlways("Default date separator");
#else
#warning Using default date separator
#endif
  return "/";
#endif
}

PString PTime::GetDayName(PTime::Weekdays day, NameType type)
{
#if defined(P_USE_LANGINFO)
  return PString(
     (type == Abbreviated) ? nl_langinfo((nl_item)(ABDAY_1+(int)day)) :
                   nl_langinfo((nl_item)(DAY_1+(int)day))
                );

#elif defined(P_LINUX)
  return (type == Abbreviated) ? PString(_time_info->abbrev_wkday[(int)day]) :
                       PString(_time_info->full_wkday[(int)day]);

#elif defined(P_USE_STRFTIME)
  char buf[30];
  struct tm t;
  memset(&t, 0, sizeof(t));
  t.tm_wday = day;
  strftime(buf, sizeof(buf), type == Abbreviated ? "%a" : "%A", &t);
  return buf;
#else
#ifdef __NUCLEUS_PLUS__
#pragma message ("Defaulting day names")
PAssertAlways("Defaulting day names");
#else
#warning Using default day names
#endif
  static char *defaultNames[] = {
    "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday",
    "Saturday"
  };

  static char *defaultAbbrev[] = {
    "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
  };
  return (type == Abbreviated) ? PString(defaultNames[(int)day]) :
                       PString(defaultAbbrev[(int)day]);
#endif
}

PString PTime::GetMonthName(PTime::Months month, NameType type) 
{
#if defined(P_USE_LANGINFO)
  return PString(
     (type == Abbreviated) ? nl_langinfo((nl_item)(ABMON_1+(int)month-1)) :
                   nl_langinfo((nl_item)(MON_1+(int)month-1))
                );
#elif defined(P_LINUX)
  return (type == Abbreviated) ? PString(_time_info->abbrev_month[(int)month-1]) :
                       PString(_time_info->full_month[(int)month-1]);
#elif defined(P_USE_STRFTIME)
  char buf[30];
  struct tm t;
  memset(&t, 0, sizeof(t));
  t.tm_mon = month-1;
  strftime(buf, sizeof(buf), type == Abbreviated ? "%b" : "%B", &t);
  return buf;
#else
#ifdef __NUCLEUS_PLUS__
#pragma message ("Using default month names")
PAssertAlways("Using default month names");
#else
#warning Using default monthnames
#endif
  static char *defaultNames[] = {
  "January", "February", "March", "April", "May", "June", "July", "August",
  "September", "October", "November", "December" };

  static char *defaultAbbrev[] = {
  "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug",
  "Sep", "Oct", "Nov", "Dec" };

  return (type == Abbreviated) ? PString(defaultNames[(int)month-1]) :
                       PString(defaultAbbrev[(int)month-1]);
#endif
}


BOOL PTime::IsDaylightSavings()
{
#ifdef __NUCLEUS_PLUS__
#pragma message ("No RTC")
PAssertAlways("No RTC");
return FALSE;
#else
  time_t theTime = ::time(NULL);
  return ::localtime(&theTime)->tm_isdst != 0;
#endif
}

int PTime::GetTimeZone(PTime::TimeZoneType type) 
{
#if defined(P_LINUX) || defined(P_SOLARIS)
  long tz = -::timezone/60;
  if (type == StandardTime)
    return tz;
  else
    return tz + ::daylight*60;
#elif defined(P_FREEBSD)
  time_t t;
  time(&t);
  struct tm  * tm = localtime(&t);
  int tz = tm->tm_gmtoff/60;
  if (type == StandardTime && tm->tm_isdst)
    return tz-60;
  if (type != StandardTime && !tm->tm_isdst)
    return tz + 60;
  return tz;
#elif defined(P_SUN4) 
  struct timeb tb;
  ftime(&tb);
  if (type == StandardTime || tb.dstflag == 0)
    return -tb.timezone;
  else
    return -tb.timezone + 60;
#else
#ifdef __NUCLEUS_PLUS__
#pragma message ("No timezone info")
PAssertAlways("No timezone info");
#else
#warning No timezone information
#endif
  return 0;
#endif
}

PString PTime::GetTimeZoneString(PTime::TimeZoneType type) 
{
#if defined(P_LINUX) || defined(P_SUN4) || defined(P_SOLARIS)
  const char * str = (type == StandardTime) ? ::tzname[0] : ::tzname[1]; 
  if (str != NULL)
    return str;
  return PString(); 
#elif defined(P_USE_STRFTIME)
  char buf[30];
  struct tm t;
  memset(&t, 0, sizeof(t));
  t.tm_isdst = type != StandardTime;
  strftime(buf, sizeof(buf), "%Z", &t);
  return buf;
#else
#ifdef __NUCLEUS_PLUS__
#pragma message ("No timezone info")
PAssertAlways("No timezone info");
#else
#warning No timezone name information
#endif

  return PString(); 
#endif
}

// note that PX_tm is local storage inside the PTime instance
#ifdef P_PTHREADS
struct tm * PTime::os_localtime(const time_t * clock, struct tm * ts)
{
  return ::localtime_r(clock, ts);
#else
struct tm * PTime::os_localtime(const time_t * clock, struct tm *)
{
  return ::localtime(clock);
#endif
}

#ifdef P_PTHREADS
struct tm * PTime::os_gmtime(const time_t * clock, struct tm * ts)
{
  return ::gmtime_r(clock, ts);
#else
struct tm * PTime::os_gmtime(const time_t * clock, struct tm *)
{
  return ::gmtime(clock);
#endif
}
