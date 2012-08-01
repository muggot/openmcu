//
// (c) Yuriy Gorvitovskiy
// for Openh323, www.Openh323.org
//
// Windows CE Port
//
// time routines implementation
//

#include <stdlibx.h>

#include <ptlib/wince/time.h>

static tm tb;
static int _lpdays[] = { -1, 30, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 };
static int _days[] = { -1, 30, 58, 89, 119, 150, 180, 211, 242, 272, 303, 333, 364 };


#define _DAY_SEC           (24L * 60L * 60L)    /* secs in a day */
#define _YEAR_SEC          (365L * _DAY_SEC)    /* secs in a year */
#define _FOUR_YEAR_SEC     (1461L * _DAY_SEC)   /* secs in a 4 year interval */
#define _BASE_DOW          4                    /* 01-01-70 was a Thursday */

static time_t _inittime=time(NULL);

struct tm * __cdecl gmtime (const time_t *timp)
{
	long caltim = *timp;            /* calendar time to convert */
    int islpyr = 0;                 /* is-current-year-a-leap-year flag */
    int tmptim;
    int *mdays;                /* pointer to days or lpdays */

    struct tm *ptb = &tb;

    if ( caltim < 0L )
        return(NULL);

    /*
     * Determine years since 1970. First, identify the four-year interval
     * since this makes handling leap-years easy (note that 2000 IS a
     * leap year and 2100 is out-of-range).
     */
    tmptim = (int)(caltim / _FOUR_YEAR_SEC);
    caltim -= ((long)tmptim * _FOUR_YEAR_SEC);

    /*
     * Determine which year of the interval
     */
    tmptim = (tmptim * 4) + 70;         /* 1970, 1974, 1978,...,etc. */

    if ( caltim >= _YEAR_SEC ) 
	{
        tmptim++;                       /* 1971, 1975, 1979,...,etc. */
        caltim -= _YEAR_SEC;

        if ( caltim >= _YEAR_SEC ) 
		{

            tmptim++;                   /* 1972, 1976, 1980,...,etc. */
            caltim -= _YEAR_SEC;

            /*
             * Note, it takes 366 days-worth of seconds to get past a leap
             * year.
             */
            if ( caltim >= (_YEAR_SEC + _DAY_SEC) ) 
			{

                    tmptim++;           /* 1973, 1977, 1981,...,etc. */
                    caltim -= (_YEAR_SEC + _DAY_SEC);
            }
            else 
			{
                    /*
                     * In a leap year after all, set the flag.
                     */
                    islpyr++;
            }
        }
    }

    /*
     * tmptim now holds the value for tm_year. caltim now holds the
     * number of elapsed seconds since the beginning of that year.
     */
    ptb->tm_year = tmptim;

    /*
     * Determine days since January 1 (0 - 365). This is the tm_yday value.
     * Leave caltim with number of elapsed seconds in that day.
     */
    ptb->tm_yday = (int)(caltim / _DAY_SEC);
    caltim -= (long)(ptb->tm_yday) * _DAY_SEC;

    /*
     * Determine months since January (0 - 11) and day of month (1 - 31)
     */
    if ( islpyr )
        mdays = _lpdays;
    else
        mdays = _days;


    for ( tmptim = 1 ; mdays[tmptim] < ptb->tm_yday ; tmptim++ ) ;

    ptb->tm_mon = --tmptim;

    ptb->tm_mday = ptb->tm_yday - mdays[tmptim];

    /*
     * Determine days since Sunday (0 - 6)
     */
    ptb->tm_wday = ((int)(*timp / _DAY_SEC) + _BASE_DOW) % 7;

    /*
     *  Determine hours since midnight (0 - 23), minutes after the hour
     *  (0 - 59), and seconds after the minute (0 - 59).
     */
    ptb->tm_hour = (int)(caltim / 3600);
    caltim -= (long)ptb->tm_hour * 3600L;

    ptb->tm_min = (int)(caltim / 60);
    ptb->tm_sec = (int)(caltim - (ptb->tm_min) * 60);

    ptb->tm_isdst = 0;
    return( (struct tm *)ptb );

}

clock_t __cdecl clock (void)
{
    clock_t elapsed;

    /* Calculate the difference between the initial time and now. */
    time_t t = time(NULL);
    elapsed = (t - _inittime) * CLOCKS_PER_SEC;
    return(elapsed);
}

time_t FileTimeToTime(const FILETIME& FileTime)
{
	SYSTEMTIME SystemTime;
	FileTimeToSystemTime(&FileTime,&SystemTime);
	return SystemTimeToTime(&SystemTime);
}

time_t	SystemTimeToTime(const LPSYSTEMTIME pSystemTime)
{	
	tm aTm;
	aTm.tm_sec = pSystemTime->wSecond;  
	aTm.tm_min = pSystemTime->wMinute;  
	aTm.tm_hour = pSystemTime->wHour; 
	aTm.tm_mday = pSystemTime->wDay; 
	aTm.tm_mon = pSystemTime->wMonth;  
	aTm.tm_year = pSystemTime->wYear; 
	aTm.tm_wday = pSystemTime->wDayOfWeek; 	
	aTm.tm_isdst = 1;

	//[YG] Will work for a next 100 years
    if (((aTm.tm_year>>2)<<2)==aTm.tm_year)
        aTm.tm_yday = _lpdays[aTm.tm_mon] + aTm.tm_mday;
    else
        aTm.tm_yday = _days[aTm.tm_mon] + aTm.tm_mday;

	return mktime(&aTm);
}

const __int64 n1SecIn100NS = (__int64)10000000;

static __int64 GetDeltaSecs(FILETIME f1, FILETIME f2)
{
	__int64 t1 = f1.dwHighDateTime;
	t1 <<= 32;				
	t1 |= f1.dwLowDateTime;

	__int64 t2 = f2.dwHighDateTime;
	t2 <<= 32;				
	t2 |= f2.dwLowDateTime;

	__int64 iTimeDiff = (t2 - t1) / n1SecIn100NS;
	return iTimeDiff;
}

static SYSTEMTIME TmToSystemTime(tm &t)
{
	SYSTEMTIME s;

	s.wYear      = t.tm_year + 1900;
	s.wMonth     = t.tm_mon+1;
	s.wDayOfWeek = t.tm_wday;
	s.wDay       = t.tm_mday;
	s.wHour      = t.tm_hour;
	s.wMinute    = t.tm_min;
	s.wSecond    = t.tm_sec;
	s.wMilliseconds = 0;

	return s;
}

static TIME_ZONE_INFORMATION gTZInfoCache;
static BOOL gbTZInfoCacheInitialized = FALSE;

static void GetTZBias(int* pTZBiasSecs = NULL, int* pDSTBiasSecs = NULL)
{
	if(!gbTZInfoCacheInitialized)
	{
		if( GetTimeZoneInformation(&gTZInfoCache) == 0xFFFFFFFF)
			return;
		gbTZInfoCacheInitialized = TRUE;
	}

	if(pTZBiasSecs != NULL)
	{
		*pTZBiasSecs = gTZInfoCache.Bias * 60;
		if (gTZInfoCache.StandardDate.wMonth != 0)
			*pTZBiasSecs += (gTZInfoCache.StandardBias * 60);
	}

	if(pDSTBiasSecs != NULL)
	{
		if ((gTZInfoCache.DaylightDate.wMonth != 0) && (gTZInfoCache.DaylightBias != 0))
			*pDSTBiasSecs = (gTZInfoCache.DaylightBias - gTZInfoCache.StandardBias) * 60;
		else
			*pDSTBiasSecs = 0;
	}
}

static FILETIME YearToFileTime(WORD wYear)
{	
	SYSTEMTIME sbase;

	sbase.wYear         = wYear;
	sbase.wMonth        = 1;
	sbase.wDayOfWeek    = 1;
	sbase.wDay          = 1;
	sbase.wHour         = 0;
	sbase.wMinute       = 0;
	sbase.wSecond       = 0;
	sbase.wMilliseconds = 0;

	FILETIME fbase;
	SystemTimeToFileTime( &sbase, &fbase );

	return fbase;
}

static FILETIME Int64ToFileTime(__int64 iTime)
{
	FILETIME f;

	f.dwHighDateTime = (DWORD)((iTime >> 32) & 0x00000000FFFFFFFF);
	f.dwLowDateTime  = (DWORD)( iTime        & 0x00000000FFFFFFFF);

	return f;
}

static time_t SystemTimeToYDay(SYSTEMTIME s)
{
	FILETIME fMidnightJan1 = YearToFileTime(s.wYear);
	FILETIME f;              SystemTimeToFileTime(&s, &f);
	return (time_t)(GetDeltaSecs(fMidnightJan1, f) / (__int64)86400);
}

static tm SystemTimeToTm(SYSTEMTIME &s)
{
	tm t;

	t.tm_year  = s.wYear - 1900;
	t.tm_mon   = s.wMonth-1;
	t.tm_wday  = s.wDayOfWeek;
	t.tm_mday  = s.wDay;
	t.tm_yday  = SystemTimeToYDay(s);
	t.tm_hour  = s.wHour;
	t.tm_min   = s.wMinute;
	t.tm_sec   = s.wSecond;
	t.tm_isdst = 0;

	return t;
}

typedef struct {
		int  yr;
		int  yd;
		long ms;
} transitionTime;

static void cvtdate (int trantype, int year, int month, int week, int dayofweek,
			             int date, int hour, int min, int sec, int msec,
						 transitionTime* pDST)
{
	const int days[]           = {-1, 30, 58, 89, 119, 150, 180, 211, 242, 272, 303, 333, 364};
	const int leapYearDays[]   = {-1, 30, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365};
	const int DAY_MILLISEC     = (24L * 60L * 60L * 1000L);
	const int BASE_DOW         = 4; //
	const int LEAP_YEAR_ADJUST = 17L; 
	BOOL bIsLeapYear = ((year & 3) == 0);

	int yearday;
	int monthdow;
	int DSTBiasSecs;
	GetTZBias(NULL, &DSTBiasSecs);

	yearday = 1 + (bIsLeapYear ? leapYearDays[month - 1] : days[month - 1]);
	monthdow = (yearday + ((year - 70) * 365) + ((year - 1) >> 2) -
				LEAP_YEAR_ADJUST + BASE_DOW) % 7;
	if ( monthdow <= dayofweek )
		yearday += (dayofweek - monthdow) + (week - 1) * 7;
	else 
		yearday += (dayofweek - monthdow) + week * 7;

	if ((week == 5) && (yearday > (bIsLeapYear ? leapYearDays[month] : days[month])))
		yearday -= 7;

	if ( trantype == 1 ) 
	{   
		pDST->yd = yearday;
		pDST->ms = (long)msec + (1000L * (sec + 60L * (min + 60L * hour)));
		pDST->yr = year;
	}
	else 
	{   
		pDST->yd = yearday;
		pDST->ms = (long)msec + (1000L * (sec + 60L * (min + 60L * hour)));
		if ((pDST->ms += (DSTBiasSecs * 1000L)) < 0) 
		{
			pDST->ms += DAY_MILLISEC;
			pDST->ms--;
		}
		else if (pDST->ms >= DAY_MILLISEC) 
		{
			pDST->ms -= DAY_MILLISEC;
			pDST->ms++;
		}
		pDST->yr = year;
	}
}

static gbUseDST = TRUE;

int isindst(struct tm *pt)
{
	transitionTime DSTStart = { -1, 0, 0L }, DSTEnd = { -1, 0, 0L };

	if(!gbUseDST) 
		return 0;

	if(!gbTZInfoCacheInitialized)
		GetTZBias();

	if((pt->tm_year != DSTStart.yr) || (pt->tm_year != DSTEnd.yr)) 
	{	
		if (gTZInfoCache.DaylightDate.wYear != 0 || gTZInfoCache.StandardDate.wYear != 0)
			return 0;

		cvtdate(1,
					pt->tm_year,
					gTZInfoCache.DaylightDate.wMonth,
					gTZInfoCache.DaylightDate.wDay,
					gTZInfoCache.DaylightDate.wDayOfWeek,
					0,
					gTZInfoCache.DaylightDate.wHour,
					gTZInfoCache.DaylightDate.wMinute,
					gTZInfoCache.DaylightDate.wSecond,
					gTZInfoCache.DaylightDate.wMilliseconds,
					&DSTStart);

		cvtdate(0,
					pt->tm_year,
					gTZInfoCache.StandardDate.wMonth,
					gTZInfoCache.StandardDate.wDay,
					gTZInfoCache.StandardDate.wDayOfWeek,
					0,
					gTZInfoCache.StandardDate.wHour,
					gTZInfoCache.StandardDate.wMinute,
					gTZInfoCache.StandardDate.wSecond,
					gTZInfoCache.StandardDate.wMilliseconds,
					&DSTEnd);
	}

	if (DSTStart.yd < DSTEnd.yd) 
	{
		if ((pt->tm_yday < DSTStart.yd) || (pt->tm_yday > DSTEnd.yd))
			return 0;
		if ((pt->tm_yday > DSTStart.yd) && (pt->tm_yday < DSTEnd.yd))
			return 1;
	}
	else 
	{
		if ( (pt->tm_yday < DSTEnd.yd) || (pt->tm_yday > DSTStart.yd) )
			return 1;
		if ( (pt->tm_yday > DSTEnd.yd) && (pt->tm_yday < DSTStart.yd) )
			return 0;
	}

	long ms = 1000L * (pt->tm_sec + 60L * pt->tm_min + 3600L * pt->tm_hour);

	if ( pt->tm_yday == DSTStart.yd ) 
	{
		if ( ms >= DSTStart.ms )
			return 1;
		else
			return 0;
	}
	else 
	{
		// pt->tm_yday == DSTEnd.yd
		if ( ms < DSTEnd.ms )
			return 1;
		else
			return 0;
	}
}

tm* localtime(const time_t *ptime)
{
	static const __int64 iOffset = 
		GetDeltaSecs(YearToFileTime(1601), YearToFileTime(1970));

	static tm  t;
	FILETIME   f;
	SYSTEMTIME s;
	int		   TZBiasSecs;
	int        DSTBiasSecs;

	memset(&t, 0, sizeof(tm));
	GetTZBias(&TZBiasSecs, &DSTBiasSecs);

	__int64 iTime = ((__int64)*ptime + iOffset - (__int64)TZBiasSecs) * n1SecIn100NS;

	f = Int64ToFileTime(iTime);

	if(FileTimeToSystemTime(&f, &s))
	{
		struct tm t2 = SystemTimeToTm(s);
		if(isindst(&t2))
		{
			f = Int64ToFileTime(iTime - DSTBiasSecs * n1SecIn100NS);
			FileTimeToSystemTime(&f, &s);
		}
		t = SystemTimeToTm(s);
	}

	return &t;
}

time_t mktime(struct tm* pt)
{
	static const FILETIME f1970 = YearToFileTime(1970);

	SYSTEMTIME s = TmToSystemTime(*pt);

	pt->tm_yday = SystemTimeToYDay(s);

	FILETIME f;
	SystemTimeToFileTime( &s, &f );

	int TZBiasSecs;
	int DSTBiasSecs;
	GetTZBias(&TZBiasSecs, &DSTBiasSecs);
	if (isindst(pt))
		TZBiasSecs += DSTBiasSecs;
	
	return (time_t)(GetDeltaSecs(f1970, f) + TZBiasSecs);
}

time_t time( time_t *timer )
{
	SYSTEMTIME s;
	GetLocalTime( &s );
	tm t = SystemTimeToTm(s);

	return mktime( &t );
}
