%{
/*
**  Originally written by Steven M. Bellovin <smb@research.att.com> while
**  at the University of North Carolina at Chapel Hill.  Later tweaked by
**  a couple of people on Usenet.  Completely overhauled by Rich $alz
**  <rsalz@bbn.com> and Jim Berets <jberets@bbn.com> in August, 1990;
**
**  Major hack to coerce it into use with the Equivalence Portable
**  Windows Library.
**
**  This grammar has 10 shift/reduce conflicts.
**
**  This code is in the public domain and has no copyright.
*/
/* SUPPRESS 287 on yaccpar_sccsid *//* Unused static variable */
/* SUPPRESS 288 on yyerrlab *//* Label unused */


#include <time.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#ifndef EOF
#include <stdio.h>
#endif


#ifdef _WIN32
#ifdef _MSC_VER
#pragma warning(disable:4131 4701 4996)
#endif
#define STDAPICALLTYPE __stdcall
#define MSDOS
#else
#define STDAPICALLTYPE
#endif


extern int  STDAPICALLTYPE PTimeGetChar(void * stream);
extern void STDAPICALLTYPE PTimeUngetChar(void * stream, int c);
int STDAPICALLTYPE PTimeGetDateOrder();
int STDAPICALLTYPE PTimeIsMonthName(const char *, int, int);
int STDAPICALLTYPE PTimeIsDayName(const char *, int, int);


#define EPOCH		1970
#define HOUR(x)		((time_t)(x) * 60)
#define SECSPERDAY	(24L * 60L * 60L)


/*
**  An entry in the lexical lookup table.
*/
typedef struct _TABLE {
    char	*name;
    int		type;
    time_t	value;
} TABLE;


/*
**  Daylight-savings mode:  on, off, or not yet known.
*/
typedef enum _DSTMODE {
    DSTon, DSToff, DSTmaybe
} DSTMODE;

/*
**  Meridian:  am, pm, or 24-hour style.
*/
typedef enum _MERIDIAN {
    MERam, MERpm, MER24
} MERIDIAN;


/*
**  Global variables.  We could get rid of most of these by using a good
**  union as the yacc stack.  (This routine was originally written before
**  yacc had the %union construct.)  Maybe someday; right now we only use
**  the %union very rarely.
*/
struct Variables {
    void	*yyInput;
    DSTMODE	yyDSTmode;
    time_t	yyDayOrdinal;
    time_t	yyDayNumber;
    int	yyHaveDate;
    int	yyHaveDay;
    int	yyHaveRel;
    int	yyHaveTime;
    int	yyHaveZone;
    time_t	yyTimezone;
    time_t	yyDay;
    time_t	yyHour;
    time_t	yyMinutes;
    time_t	yyMonth;
    time_t	yySeconds;
    time_t	yyYear;
    MERIDIAN	yyMeridian;
    time_t	yyRelMonth;
    time_t	yyRelSeconds;
};

#define VARIABLE ((struct Variables*)parseParam)


#define YYPURE		1
#define YYLEX_PARAM	VARIABLE->yyInput
#define YYPARSE_PARAM	parseParam

#define yyparse		PTime_yyparse
#define yylex		PTime_yylex
#define yyerror		PTime_yyerror

static int yyparse(void *); 
static int yylex();

#ifdef __GNUC__
static int yyerror();
#else
static void yyerror();
#endif


static void SetPossibleDate(struct Variables*, time_t, time_t, time_t);


%}

%pure_parser

%union {
    time_t		Number;
    enum _MERIDIAN	Meridian;
}

%token	tAGO tDAY tDAYZONE tID tMERIDIAN tMINUTE_UNIT tMONTH tMONTH_UNIT
%token	tSNUMBER tUNUMBER t4DIGITNUMBER t6DIGITNUMBER
%token	tSEC_UNIT tZONE tMILZONE tDST

%type	<Number>	tDAY tDAYZONE tMINUTE_UNIT tMONTH tMONTH_UNIT
%type	<Number>	tSNUMBER tUNUMBER t4DIGITNUMBER t6DIGITNUMBER unumber
%type	<Number>	tSEC_UNIT tZONE tMILZONE
%type	<Meridian>	tMERIDIAN o_merid

%%

spec	: /* NULL */
	| spec item
	;

item	: time {
	    VARIABLE->yyHaveTime++;
	}
	| zone {
	    VARIABLE->yyHaveZone++;
	}
	| date {
	    VARIABLE->yyHaveDate++;
	}
	| day {
	    VARIABLE->yyHaveDay++;
	}
	| rel {
	    VARIABLE->yyHaveRel++;
	}
	| number
	;

time	: tUNUMBER tMERIDIAN {
	    VARIABLE->yyHour = $1;
	    VARIABLE->yyMinutes = 0;
	    VARIABLE->yySeconds = 0;
	    VARIABLE->yyMeridian = $2;
	}
        | t4DIGITNUMBER tSNUMBER {
	    VARIABLE->yyHour = $1/100;
	    VARIABLE->yyMinutes = $1%100;
	    VARIABLE->yySeconds = 0;
	    VARIABLE->yyMeridian = MER24;
	    VARIABLE->yyDSTmode = DSToff;
	    VARIABLE->yyTimezone = - ($2 % 100 + ($2 / 100) * 60);
        }
        | t6DIGITNUMBER tSNUMBER {
	    VARIABLE->yyHour = $1/10000;
	    VARIABLE->yyMinutes = ($1/100)%100;
	    VARIABLE->yySeconds = $1 % 100;
	    VARIABLE->yyMeridian = MER24;
	    VARIABLE->yyDSTmode = DSToff;
	    VARIABLE->yyTimezone = - ($2 % 100 + ($2 / 100) * 60);
        }
	|  unumber ':' unumber o_merid {
	    VARIABLE->yyHour = $1;
	    VARIABLE->yyMinutes = $3;
	    VARIABLE->yySeconds = 0;
	    VARIABLE->yyMeridian = $4;
	}
	| unumber ':' unumber tSNUMBER {
	    VARIABLE->yyHour = $1;
	    VARIABLE->yyMinutes = $3;
	    VARIABLE->yyMeridian = MER24;
	    VARIABLE->yyDSTmode = DSToff;
	    VARIABLE->yyTimezone = - ($4 % 100 + ($4 / 100) * 60);
	}
	| unumber ':' unumber ':' unumber o_merid {
	    VARIABLE->yyHour = $1;
	    VARIABLE->yyMinutes = $3;
	    VARIABLE->yySeconds = $5;
	    VARIABLE->yyMeridian = $6;
	}
	| unumber ':' unumber ':' unumber tSNUMBER {
	    VARIABLE->yyHour = $1;
	    VARIABLE->yyMinutes = $3;
	    VARIABLE->yySeconds = $5;
	    VARIABLE->yyMeridian = MER24;
	    VARIABLE->yyDSTmode = DSToff;
	    VARIABLE->yyTimezone = - ($6 % 100 + ($6 / 100) * 60);
	}
	;

zone	: tZONE {
	    VARIABLE->yyTimezone = $1;
	    VARIABLE->yyDSTmode = DSToff;
	}
	| tDAYZONE {
	    VARIABLE->yyTimezone = $1;
	    VARIABLE->yyDSTmode = DSTon;
	}
	|
	  tZONE tDST {
	    VARIABLE->yyTimezone = $1;
	    VARIABLE->yyDSTmode = DSTon;
	}
        | tMILZONE {
            if (VARIABLE->yyHaveTime > 0) {
	      VARIABLE->yyTimezone = $1;
	      VARIABLE->yyDSTmode = DSToff;
            }
            else
              VARIABLE->yyHaveZone--;
        }
	;

day	: tDAY {
	    VARIABLE->yyDayOrdinal = 1;
	    VARIABLE->yyDayNumber = $1;
	}
	| tDAY ',' {
	    VARIABLE->yyDayOrdinal = 1;
	    VARIABLE->yyDayNumber = $1;
	}
	| unumber tDAY {
	    VARIABLE->yyDayOrdinal = $1;
	    VARIABLE->yyDayNumber = $2;
	}
	;

date	: unumber '/' unumber {
	    SetPossibleDate(VARIABLE, $1, $3, VARIABLE->yyYear);
	}
	| unumber '/' unumber '/' unumber {
	    SetPossibleDate(VARIABLE, $1, $3, $5);
	}
	| unumber tSNUMBER tSNUMBER {
	    /* ISO 8601 format.  yyyy-mm-dd.  */
	    if ($1 > 31) {
		VARIABLE->yyYear = $1;
		VARIABLE->yyMonth = -$2;
		VARIABLE->yyDay = -$3;
	    }
	    else
		SetPossibleDate(VARIABLE, $1, -$2, -$3);
	}
	| unumber tMONTH tSNUMBER {
	    /* e.g. 17-JUN-1992.  */
	    VARIABLE->yyDay = $1;
	    VARIABLE->yyMonth = $2;
	    VARIABLE->yyYear = -$3;
	}
	| tMONTH unumber {
	    VARIABLE->yyMonth = $1;
	    VARIABLE->yyDay = $2;
	}
	| tMONTH unumber ',' unumber {
	    VARIABLE->yyMonth = $1;
	    VARIABLE->yyDay = $2;
	    VARIABLE->yyYear = $4;
	}
	| unumber tMONTH {
	    VARIABLE->yyMonth = $2;
	    VARIABLE->yyDay = $1;
	}
	| unumber tMONTH unumber {
	    VARIABLE->yyMonth = $2;
	    VARIABLE->yyDay = $1;
	    VARIABLE->yyYear = $3;
	}
	;

rel	: relunit tAGO {
	    VARIABLE->yyRelSeconds = -VARIABLE->yyRelSeconds;
	    VARIABLE->yyRelMonth = -VARIABLE->yyRelMonth;
	}
	| relunit
	;

relunit	: unumber tMINUTE_UNIT {
	    VARIABLE->yyRelSeconds += $1 * $2 * 60L;
	}
	| tSNUMBER tMINUTE_UNIT {
	    VARIABLE->yyRelSeconds += $1 * $2 * 60L;
	}
	| tMINUTE_UNIT {
	    VARIABLE->yyRelSeconds += $1 * 60L;
	}
	| tSNUMBER tSEC_UNIT {
	    VARIABLE->yyRelSeconds += $1;
	}
	| unumber tSEC_UNIT {
	    VARIABLE->yyRelSeconds += $1;
	}
	| tSEC_UNIT {
	    VARIABLE->yyRelSeconds++;
	}
	| tSNUMBER tMONTH_UNIT {
	    VARIABLE->yyRelMonth += $1 * $2;
	}
	| unumber tMONTH_UNIT {
	    VARIABLE->yyRelMonth += $1 * $2;
	}
	| tMONTH_UNIT {
	    VARIABLE->yyRelMonth += $1;
	}
	;

unumber : tUNUMBER {
	    $$ = $1;
	}
        | t4DIGITNUMBER {
	    $$ = $1;
	}
        | t6DIGITNUMBER {
	    $$ = $1;
	}
        ;

number	: tUNUMBER {
	    if (VARIABLE->yyHaveTime && VARIABLE->yyHaveDate && !VARIABLE->yyHaveRel)
		VARIABLE->yyYear = $1;
	    else {
		if($1>240000) {
		    VARIABLE->yyHaveDate++;
		    VARIABLE->yyDay= ($1)%100;
		    VARIABLE->yyMonth= ($1/100)%100;
		    VARIABLE->yyYear = $1/10000;
		}
		else {
		    VARIABLE->yyHaveTime++;
		    if ($1 < 10000) {
		    	VARIABLE->yyHour = $1 / 100;
		    	VARIABLE->yyMinutes = $1 % 100;
		        VARIABLE->yySeconds = 0;
		    }
		    else {
	                VARIABLE->yyHour = $1/10000;
	                VARIABLE->yyMinutes = ($1/100)%100;
	                VARIABLE->yySeconds = $1 % 100;
                    }
		    VARIABLE->yyMeridian = MER24;
	        }
	    }
	}
        | t4DIGITNUMBER {
	    if (VARIABLE->yyHaveTime && VARIABLE->yyHaveDate && !VARIABLE->yyHaveRel)
		VARIABLE->yyYear = $1;
	    else {
	        VARIABLE->yyHaveTime++;
	        VARIABLE->yyHour = $1/100;
	        VARIABLE->yyMinutes = $1%100;
	        VARIABLE->yySeconds = 0;
	        VARIABLE->yyMeridian = MER24;
            }
        }
        | t6DIGITNUMBER {
	    if (!VARIABLE->yyHaveDate && $1>240000) {
		VARIABLE->yyHaveDate++;
		VARIABLE->yyDay= ($1)%100;
		VARIABLE->yyMonth= ($1/100)%100;
		VARIABLE->yyYear = $1/10000;
	    }
	    else if (!VARIABLE->yyHaveTime) {
	        VARIABLE->yyHaveTime++;
	        VARIABLE->yyHour = $1/10000;
	        VARIABLE->yyMinutes = ($1/100)%100;
	        VARIABLE->yySeconds = $1 % 100;
	        VARIABLE->yyMeridian = MER24;
            }
        }
	;

o_merid	: /* NULL */ {
	    $$ = MER24;
	}
	| tMERIDIAN {
	    $$ = $1;
	}
	;

%%

/* Month and day table. */
static TABLE const MonthDayTable[] = {
    { "january",	tMONTH,  1 },
    { "february",	tMONTH,  2 },
    { "march",		tMONTH,  3 },
    { "april",		tMONTH,  4 },
    { "may",		tMONTH,  5 },
    { "june",		tMONTH,  6 },
    { "july",		tMONTH,  7 },
    { "august",		tMONTH,  8 },
    { "september",	tMONTH,  9 },
    { "sept",		tMONTH,  9 },
    { "october",	tMONTH, 10 },
    { "november",	tMONTH, 11 },
    { "december",	tMONTH, 12 },
    { "sunday",		tDAY, 0 },
    { "monday",		tDAY, 1 },
    { "tuesday",	tDAY, 2 },
    { "tues",		tDAY, 2 },
    { "wednesday",	tDAY, 3 },
    { "wednes",		tDAY, 3 },
    { "thursday",	tDAY, 4 },
    { "thur",		tDAY, 4 },
    { "thurs",		tDAY, 4 },
    { "friday",		tDAY, 5 },
    { "saturday",	tDAY, 6 },
    { NULL }
};

/* Time units table. */
static TABLE const UnitsTable[] = {
    { "year",		tMONTH_UNIT,	12 },
    { "month",		tMONTH_UNIT,	1 },
    { "fortnight",	tMINUTE_UNIT,	14 * 24 * 60 },
    { "week",		tMINUTE_UNIT,	7 * 24 * 60 },
    { "day",		tMINUTE_UNIT,	1 * 24 * 60 },
    { "hour",		tMINUTE_UNIT,	60 },
    { "minute",		tMINUTE_UNIT,	1 },
    { "min",		tMINUTE_UNIT,	1 },
    { "second",		tSEC_UNIT,	1 },
    { "sec",		tSEC_UNIT,	1 },
    { NULL }
};

/* Assorted relative-time words. */
static TABLE const OtherTable[] = {
    { "tomorrow",	tMINUTE_UNIT,	1 * 24 * 60 },
    { "yesterday",	tMINUTE_UNIT,	-1 * 24 * 60 },
    { "today",		tMINUTE_UNIT,	0 },
    { "now",		tMINUTE_UNIT,	0 },
    { "last",		tUNUMBER,	-1 },
    { "this",		tMINUTE_UNIT,	0 },
    { "next",		tUNUMBER,	2 },
    { "first",		tUNUMBER,	1 },
/*  { "second",		tUNUMBER,	2 }, */
    { "third",		tUNUMBER,	3 },
    { "fourth",		tUNUMBER,	4 },
    { "fifth",		tUNUMBER,	5 },
    { "sixth",		tUNUMBER,	6 },
    { "seventh",	tUNUMBER,	7 },
    { "eighth",		tUNUMBER,	8 },
    { "ninth",		tUNUMBER,	9 },
    { "tenth",		tUNUMBER,	10 },
    { "eleventh",	tUNUMBER,	11 },
    { "twelfth",	tUNUMBER,	12 },
    { "ago",		tAGO,	1 },
    { NULL }
};

/* The timezone table. */
/* Some of these are commented out because a time_t can't store a float. */
static TABLE const TimezoneTable[] = {
    { "gmt",	tZONE,     HOUR( 0) },	/* Greenwich Mean */
    { "ut",	tZONE,     HOUR( 0) },	/* Universal (Coordinated) */
    { "utc",	tZONE,     HOUR( 0) },
    { "wet",	tZONE,     HOUR( 0) },	/* Western European */
    { "bst",	tDAYZONE,  HOUR( 0) },	/* British Summer */
    { "wat",	tZONE,     HOUR( 1) },	/* West Africa */
    { "at",	tZONE,     HOUR( 2) },	/* Azores */
#if	0
    /* For completeness.  BST is also British Summer, and GST is
     * also Guam Standard. */
    { "bst",	tZONE,     HOUR( 3) },	/* Brazil Standard */
    { "gst",	tZONE,     HOUR( 3) },	/* Greenland Standard */
#endif
#if 0
    { "nft",	tZONE,     HOUR(3.5) },	/* Newfoundland */
    { "nst",	tZONE,     HOUR(3.5) },	/* Newfoundland Standard */
    { "ndt",	tDAYZONE,  HOUR(3.5) },	/* Newfoundland Daylight */
#endif
    { "ast",	tZONE,     HOUR( 4) },	/* Atlantic Standard */
    { "adt",	tDAYZONE,  HOUR( 4) },	/* Atlantic Daylight */
    { "est",	tZONE,     HOUR( 5) },	/* Eastern Standard */
    { "edt",	tDAYZONE,  HOUR( 5) },	/* Eastern Daylight */
    { "cst",	tZONE,     HOUR( 6) },	/* Central Standard */
    { "cdt",	tDAYZONE,  HOUR( 6) },	/* Central Daylight */
    { "mst",	tZONE,     HOUR( 7) },	/* Mountain Standard */
    { "mdt",	tDAYZONE,  HOUR( 7) },	/* Mountain Daylight */
    { "pst",	tZONE,     HOUR( 8) },	/* Pacific Standard */
    { "pdt",	tDAYZONE,  HOUR( 8) },	/* Pacific Daylight */
    { "yst",	tZONE,     HOUR( 9) },	/* Yukon Standard */
    { "ydt",	tDAYZONE,  HOUR( 9) },	/* Yukon Daylight */
    { "hst",	tZONE,     HOUR(10) },	/* Hawaii Standard */
    { "hdt",	tDAYZONE,  HOUR(10) },	/* Hawaii Daylight */
    { "cat",	tZONE,     HOUR(10) },	/* Central Alaska */
    { "ahst",	tZONE,     HOUR(10) },	/* Alaska-Hawaii Standard */
    { "nt",	tZONE,     HOUR(11) },	/* Nome */
    { "idlw",	tZONE,     HOUR(12) },	/* International Date Line West */
    { "cet",	tZONE,     -HOUR(1) },	/* Central European */
    { "met",	tZONE,     -HOUR(1) },	/* Middle European */
    { "mewt",	tZONE,     -HOUR(1) },	/* Middle European Winter */
    { "mest",	tDAYZONE,  -HOUR(1) },	/* Middle European Summer */
    { "swt",	tZONE,     -HOUR(1) },	/* Swedish Winter */
    { "sst",	tDAYZONE,  -HOUR(1) },	/* Swedish Summer */
    { "fwt",	tZONE,     -HOUR(1) },	/* French Winter */
    { "fst",	tDAYZONE,  -HOUR(1) },	/* French Summer */
    { "eet",	tZONE,     -HOUR(2) },	/* Eastern Europe, USSR Zone 1 */
    { "bt",	tZONE,     -HOUR(3) },	/* Baghdad, USSR Zone 2 */
#if 0
    { "it",	tZONE,     -HOUR(3.5) },/* Iran */
#endif
    { "zp4",	tZONE,     -HOUR(4) },	/* USSR Zone 3 */
    { "zp5",	tZONE,     -HOUR(5) },	/* USSR Zone 4 */
#if 0
    { "ist",	tZONE,     -HOUR(5.5) },/* Indian Standard */
#endif
    { "zp6",	tZONE,     -HOUR(6) },	/* USSR Zone 5 */
#if	0
    /* For completeness.  NST is also Newfoundland Stanard, and SST is
     * also Swedish Summer. */
    { "nst",	tZONE,     -HOUR(6.5) },/* North Sumatra */
    { "sst",	tZONE,     -HOUR(7) },	/* South Sumatra, USSR Zone 6 */
#endif	/* 0 */
    { "wast",	tZONE,     -HOUR(7) },	/* West Australian Standard */
    { "wadt",	tDAYZONE,  -HOUR(7) },	/* West Australian Daylight */
#if 0
    { "jt",	tZONE,     -HOUR(7.5) },/* Java (3pm in Cronusland!) */
#endif
    { "cct",	tZONE,     -HOUR(8) },	/* China Coast, USSR Zone 7 */
    { "jst",	tZONE,     -HOUR(9) },	/* Japan Standard, USSR Zone 8 */
#if 0
    { "cast",	tZONE,     -HOUR(9.5) },/* Central Australian Standard */
    { "cadt",	tDAYZONE,  -HOUR(9.5) },/* Central Australian Daylight */
#endif
    { "east",	tZONE,     -HOUR(10) },	/* Eastern Australian Standard */
    { "eadt",	tDAYZONE,  -HOUR(10) },	/* Eastern Australian Daylight */
    { "gst",	tZONE,     -HOUR(10) },	/* Guam Standard, USSR Zone 9 */
    { "nzt",	tZONE,     -HOUR(12) },	/* New Zealand */
    { "nzst",	tZONE,     -HOUR(12) },	/* New Zealand Standard */
    { "nzdt",	tDAYZONE,  -HOUR(12) },	/* New Zealand Daylight */
    { "idle",	tZONE,     -HOUR(12) },	/* International Date Line East */
    {  NULL  }
};

/* Military timezone table. */
static TABLE const MilitaryTable[] = {
    { "a",	tMILZONE,	HOUR(  1) },
    { "b",	tMILZONE,	HOUR(  2) },
    { "c",	tMILZONE,	HOUR(  3) },
    { "d",	tMILZONE,	HOUR(  4) },
    { "e",	tMILZONE,	HOUR(  5) },
    { "f",	tMILZONE,	HOUR(  6) },
    { "g",	tMILZONE,	HOUR(  7) },
    { "h",	tMILZONE,	HOUR(  8) },
    { "i",	tMILZONE,	HOUR(  9) },
    { "k",	tMILZONE,	HOUR( 10) },
    { "l",	tMILZONE,	HOUR( 11) },
    { "m",	tMILZONE,	HOUR( 12) },
    { "n",	tMILZONE,	HOUR(- 1) },
    { "o",	tMILZONE,	HOUR(- 2) },
    { "p",	tMILZONE,	HOUR(- 3) },
    { "q",	tMILZONE,	HOUR(- 4) },
    { "r",	tMILZONE,	HOUR(- 5) },
    { "s",	tMILZONE,	HOUR(- 6) },
    { "t",	tMILZONE,	HOUR(- 7) },
    { "u",	tMILZONE,	HOUR(- 8) },
    { "v",	tMILZONE,	HOUR(- 9) },
    { "w",	tMILZONE,	HOUR(-10) },
    { "x",	tMILZONE,	HOUR(-11) },
    { "y",	tMILZONE,	HOUR(-12) },
    { "z",	tZONE,		HOUR(  0) }, /* Deliberately tZONE */
    { NULL }
};

static int LookupWord(char * buff, YYSTYPE * yylval)
{
    register char	*p;
    register char	*q;
    register const TABLE	*tp;
    int			i;
    int			abbrev;

    /* Make it lowercase. */
    for (p = buff; *p != '\0'; p++)
        *p = (char)tolower(*p);

    if (strcmp(buff, "am") == 0 || strcmp(buff, "a.m.") == 0) {
	yylval->Meridian = MERam;
	return tMERIDIAN;
    }
    if (strcmp(buff, "pm") == 0 || strcmp(buff, "p.m.") == 0) {
	yylval->Meridian = MERpm;
	return tMERIDIAN;
    }

    /* See if we have an abbreviation for a month. */
    if (strlen(buff) == 3)
	abbrev = 1;
    else if (strlen(buff) == 4 && buff[3] == '.') {
	abbrev = 1;
	buff[3] = '\0';
    }
    else
	abbrev = 0;

    for (tp = MonthDayTable; tp->name; tp++) {
	if (abbrev) {
	    if (strncmp(buff, tp->name, 3) == 0) {
		yylval->Number = tp->value;
		return tp->type;
	    }
	}
	else if (strcmp(buff, tp->name) == 0) {
	    yylval->Number = tp->value;
	    return tp->type;
	}
    }

    for (tp = TimezoneTable; tp->name; tp++)
	if (strcmp(buff, tp->name) == 0) {
	    yylval->Number = tp->value;
	    return tp->type;
	}

    if (strcmp(buff, "dst") == 0) 
	return tDST;

    for (tp = UnitsTable; tp->name; tp++)
	if (strcmp(buff, tp->name) == 0) {
	    yylval->Number = tp->value;
	    return tp->type;
	}

    /* Strip off any plural and try the units table again. */
    i = strlen(buff) - 1;
    if (buff[i] == 's') {
	buff[i] = '\0';
	for (tp = UnitsTable; tp->name; tp++)
	    if (strcmp(buff, tp->name) == 0) {
		yylval->Number = tp->value;
		return tp->type;
	    }
	buff[i] = 's';		/* Put back for "this" in OtherTable. */
    }

    for (tp = OtherTable; tp->name; tp++)
	if (strcmp(buff, tp->name) == 0) {
	    yylval->Number = tp->value;
	    return tp->type;
	}

    /* Military timezones. */
    if (buff[1] == '\0' && isalpha(*buff)) {
	for (tp = MilitaryTable; tp->name; tp++)
	    if (strcmp(buff, tp->name) == 0) {
		yylval->Number = tp->value;
		return tp->type;
	    }
    }

    /* Drop out any periods and try the timezone table again. */
    for (i = 0, p = q = buff; *q; q++)
	if (*q != '.')
	    *p++ = *q;
	else
	    i++;
    *p = '\0';
    if (i)
	for (tp = TimezoneTable; tp->name; tp++)
	    if (strcmp(buff, tp->name) == 0) {
		yylval->Number = tp->value;
		return tp->type;
	    }

    for (i = 1; i <= 12; i++)
	for (abbrev = 0; abbrev < 2; abbrev++)
	    if (PTimeIsMonthName(buff, i, abbrev)) {
		yylval->Number = i;
		return tMONTH;
	    }

    for (i = 1; i <= 7; i++)
	for (abbrev = 0; abbrev < 2; abbrev++)
	    if (PTimeIsDayName(buff, i, abbrev)) {
		yylval->Number = i;
		return tDAY;
	    }

    return tID;
}


#ifdef _MSC_VER
#pragma warning(disable:4211)
#endif

#ifndef __GNUC__
static
#endif
int yylex(YYSTYPE * yylval, void * yyInput)
{
    register char	*p;
    char		buff[20];
    int			Count;
    int			sign;
    register int	c = PTimeGetChar(yyInput);

    while (c != EOF && c != '\0' && c != '\n') {
	while (isspace(c))
	    c = PTimeGetChar(yyInput);

	if (isdigit(c) || c == '-' || c == '+') {
	    if (c == '-' || c == '+') {
		sign = c == '-' ? -1 : 1;
		if (!isdigit(c = PTimeGetChar(yyInput)))
		    /* skip the '-' sign */
		    continue;
	    }
	    else
		sign = 0;
	    yylval->Number = 0;
            Count = 0; /* Count number of digits */
	    while (isdigit(c)) {
		yylval->Number = 10 * yylval->Number + c - '0';
		c = PTimeGetChar(yyInput);
                Count++;
	    }
	    PTimeUngetChar(yyInput, c);
	    if (sign < 0)
		yylval->Number = -yylval->Number;
	    if (sign)
              return tSNUMBER;
            if (Count == 4)
              return t4DIGITNUMBER;
            if (Count == 6)
              return t6DIGITNUMBER;
            return tUNUMBER;
	}

	if (isalpha(c)) {
	    for (p = buff; isalpha(c) || c == '.'; c = PTimeGetChar(yyInput)) {
		if (p < &buff[sizeof(buff)-1])
		    *p++ = (char)c;
	    }
	    *p = '\0';
	    PTimeUngetChar(yyInput, c);
	    return LookupWord(buff, yylval);
	}

	if (c != '(')
	    return c;

	Count = 0;
	do {
	    c = PTimeGetChar(yyInput);
	    if (c == '\0' || c == EOF)
		return c;
	    if (c == '(')
		Count++;
	    else if (c == ')')
		Count--;
	} while (Count > 0);
    }

    return EOF;
}

#ifdef _MSC_VER
#pragma warning(default:4211)
#endif


static time_t ToSeconds(time_t Hours, time_t Minutes, time_t Seconds,
			MERIDIAN Meridian)
{
    if (Minutes < 0 || Minutes > 59 || Seconds < 0 || Seconds > 59)
	return -1;
    switch (Meridian) {
    case MER24:
	if (Hours < 0 || Hours > 23)
	    return -1;
	return (Hours * 60L + Minutes) * 60L + Seconds;
    case MERam:
	if (Hours < 1 || Hours > 12)
	    return -1;
	if (Hours == 12)
	    Hours = 0;
	return (Hours * 60L + Minutes) * 60L + Seconds;
    case MERpm:
	if (Hours < 1 || Hours > 12)
	    return -1;
	if (Hours == 12)
	    Hours = 0;
	return ((Hours + 12) * 60L + Minutes) * 60L + Seconds;
    }

    return -1;
}


static time_t Convert(time_t Month, time_t Day, time_t Year,
		      time_t Hours, time_t Minutes, time_t Seconds,
		      MERIDIAN Meridian, DSTMODE DSTmode, time_t yyTimezone)
{
    static int DaysInMonth[12] = {
	31, 0, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
    };
    time_t	tod;
    time_t	Julian;
    int		i;

    if (Year < 0)
	Year = -Year;
    if (Year < 70)
	Year += 2000;
    else if (Year < 100)
	Year += 1900;
    DaysInMonth[1] = Year % 4 == 0 && (Year % 100 != 0 || Year % 400 == 0)
		    ? 29 : 28;
    /* Checking for 2038 bogusly assumes that time_t is 32 bits.  But
       I'm too lazy to try to check for time_t overflow in another way.  */
    if (Year < EPOCH || Year > 2038
     || Month < 1 || Month > 12
     /* Lint fluff:  "conversion from long may lose accuracy" */
     || Day < 1 || Day > DaysInMonth[(int)--Month])
	return -1;

    for (Julian = Day - 1, i = 0; i < Month; i++)
	Julian += DaysInMonth[i];
    for (i = EPOCH; i < Year; i++)
	Julian += 365 + (i % 4 == 0);
    Julian *= SECSPERDAY;
    Julian += yyTimezone * 60L;
    if ((tod = ToSeconds(Hours, Minutes, Seconds, Meridian)) < 0)
	return -1;
    Julian += tod;
    if (DSTmode == DSTon
     || (DSTmode == DSTmaybe && localtime(&Julian)->tm_isdst))
	Julian -= 60 * 60;
    return Julian;
}


static time_t DSTcorrect(time_t Start, time_t Future)
{
    time_t	StartDay;
    time_t	FutureDay;

    StartDay = (localtime(&Start)->tm_hour + 1) % 24;
    FutureDay = (localtime(&Future)->tm_hour + 1) % 24;
    return (Future - Start) + (StartDay - FutureDay) * 60L * 60L;
}


static time_t RelativeDate(time_t Start, time_t DayOrdinal, time_t DayNumber)
{
    struct tm	*tm;
    time_t	now;

    now = Start;
    tm = localtime(&now);
    now += SECSPERDAY * ((DayNumber - tm->tm_wday + 7) % 7);
    now += 7 * SECSPERDAY * (DayOrdinal <= 0 ? DayOrdinal : DayOrdinal - 1);
    return DSTcorrect(Start, now);
}


static time_t RelativeMonth(time_t Start, time_t RelMonth, time_t yyTimezone)
{
    struct tm	*tm;
    time_t	Month;
    time_t	Year;

    if (RelMonth == 0)
	return 0;
    tm = localtime(&Start);
    Month = 12 * tm->tm_year + tm->tm_mon + RelMonth;
    Year = Month / 12 + 1900;
    Month = Month % 12 + 1;
    return DSTcorrect(Start,
	    Convert(Month, (time_t)tm->tm_mday, Year,
		(time_t)tm->tm_hour, (time_t)tm->tm_min, (time_t)tm->tm_sec,
		MER24, DSTmaybe, yyTimezone));
}


static void SetPossibleDate(struct Variables * var,
			      time_t possible_day,
			      time_t possible_month,
			      time_t possible_year)
{
    int date_order;

    if (possible_day > 31) /* test for ymd */
	date_order = 2;
    else if (possible_day > 12) /* test for dmy */
	date_order = 1;
    else if (possible_month > 12) /* test for mdy */
	date_order = 0;
    else {
	static int default_date_order = -1;
	if (default_date_order < 0)
	    default_date_order = PTimeGetDateOrder();
	date_order = default_date_order;
    }

    switch (date_order) {
      case 0 :
	var->yyDay   = possible_month;
	var->yyMonth = possible_day;
	var->yyYear  = possible_year;
	break;
      case 1 :
	var->yyDay   = possible_day;
	var->yyMonth = possible_month;
	var->yyYear  = possible_year;
	break;
      default :
	var->yyDay   = possible_year;
	var->yyMonth = possible_month;
	var->yyYear  = possible_day;
    }
}


time_t STDAPICALLTYPE PTimeParse(void * inputStream, struct tm * now, int timezone)
{
    time_t		Start;
    struct Variables	var;


    var.yyInput = inputStream;
    var.yyYear = now->tm_year + 1900;
    var.yyMonth = now->tm_mon + 1;
    var.yyDay = now->tm_mday;
    var.yyTimezone = -timezone;
    var.yyDSTmode = DSTmaybe;
    var.yyHour = 0;
    var.yyMinutes = 0;
    var.yySeconds = 0;
    var.yyMeridian = MER24;
    var.yyRelSeconds = 0;
    var.yyRelMonth = 0;
    var.yyHaveDate = 0;
    var.yyHaveDay = 0;
    var.yyHaveRel = 0;
    var.yyHaveTime = 0;
    var.yyHaveZone = 0;

    yyparse(&var);

    if (var.yyHaveTime > 1 || var.yyHaveZone > 1 ||
	var.yyHaveDate > 1 || var.yyHaveDay > 1)
	return -1;

    if (var.yyHaveTime == 0 && var.yyHaveZone == 0 &&
	var.yyHaveDate == 0 && var.yyHaveDay == 0 && var.yyHaveRel == 0)
	return -1;

    if (var.yyHaveDate || var.yyHaveTime || var.yyHaveDay) {
	Start = Convert(var.yyMonth, var.yyDay, var.yyYear,
			var.yyHour, var.yyMinutes, var.yySeconds,
			var.yyMeridian, var.yyDSTmode, var.yyTimezone);
	if (Start < 0)
	    return -1;
    }
    else {
	time(&Start);
	if (!var.yyHaveRel)
	    Start -= ((now->tm_hour * 60L + now->tm_min) * 60L) + now->tm_sec;
    }

    Start += var.yyRelSeconds;
    Start += RelativeMonth(Start, var.yyRelMonth, var.yyTimezone);

    if (var.yyHaveDay && !var.yyHaveDate)
	Start += RelativeDate(Start, var.yyDayOrdinal, var.yyDayNumber);

    /* Have to do *something* with a legitimate -1 so it's distinguishable
     * from the error return value.  (Alternately could set errno on error.) */
    return Start == -1 ? 0 : Start;
}


#ifdef _MSC_VER
#pragma warning(disable:4100 4211)
#endif

#ifdef __GNUC__
int yyerror(const char * s)
{
  return 0;
}
#else
static void yyerror(const char * s)
{
}
#endif

#ifdef _MSC_VER
#pragma warning(default:4100 4211)
#endif


/* End of file ***************************************************************/
