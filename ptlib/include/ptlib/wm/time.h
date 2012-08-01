//////////////////////////////////////////////////////
//
// VisualStudio 2005 PWLib Port, 
// (c) 2007 Dinsk.net
// developer@dinsk.net 
//
//////////////////////////////////////////////////////
//
// (c) Yuriy Gorvitovskiy
// for Openh323, www.Openh323.org
//
// Windows CE Port
//
// Definitions/declarations for time routines 
//
// [Microsoft]
// [ANSI/System V]
// [Public]
// 

#ifndef _TIME_H
#define _TIME_H

#define _INC_TIME // for wce.h

#ifndef _TM_DEFINED
#define _TM_DEFINED
struct tm {
        int tm_sec;     /* seconds after the minute - [0,59] */
        int tm_min;     /* minutes after the hour - [0,59] */
        int tm_hour;    /* hours since midnight - [0,23] */
        int tm_mday;    /* day of the month - [1,31] */
        int tm_mon;     /* months since January - [0,11] */
        int tm_year;    /* years since 1900 */
        int tm_wday;    /* days since Sunday - [0,6] */
        int tm_yday;    /* days since January 1 - [0,365] */
        int tm_isdst;   /* daylight savings time flag */
        };
#endif // _TM_DEFINED

#ifndef _CLOCK_T_DEFINED
typedef long clock_t;
#define _CLOCK_T_DEFINED
#endif

/* Clock ticks macro - ANSI version */
#define CLOCKS_PER_SEC  1000

#ifndef _TIME_T_DEFINED
typedef long  time_t;
#define _TIME_T_DEFINED 	/* avoid multiple def's of time_t */
#endif


#ifdef  __cplusplus
extern "C" {
#endif

/* Function prototypes */
clock_t     clock(void);
struct tm * gmtime(const time_t* t);
struct tm * localtime(const time_t* t);
time_t      mktime(struct tm* t);
time_t      time(time_t* t);

#if _WIN32_WCE < 0x501
size_t wcsftime(
   wchar_t *strDest,
   size_t maxsize,
   const wchar_t *format,
   const struct tm *timeptr 
);
#endif

#ifdef  __cplusplus
};

time_t	FileTimeToTime(const FILETIME FileTime);
time_t	SystemTimeToTime(const LPSYSTEMTIME pSystemTime);

#endif

#endif  /* _INC_TIME */
