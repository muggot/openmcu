#include <stdlibx.h>

/*  A Bison parser, made from ..\..\common\getdate.y with Bison version GNU Bison version 1.24
  */

#define YYBISON 1  /* Identify Bison output.  */

#define	tAGO	258
#define	tDAY	259
#define	tDAYZONE	260
#define	tID	261
#define	tMERIDIAN	262
#define	tMINUTE_UNIT	263
#define	tMONTH	264
#define	tMONTH_UNIT	265
#define	tSEC_UNIT	266
#define	tSNUMBER	267
#define	tUNUMBER	268
#define	tZONE	269
#define	tDST	270

#ifdef _MSC_VER
#pragma warning(disable:4131 4701 4129 4146 4307 4018)
#define STDAPICALLTYPE __stdcall
#define MSDOS
#else
#define STDAPICALLTYPE
#endif

#line 3 "..\..\common\getdate.y"

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

extern "C" {
extern int  STDAPICALLTYPE PTimeGetChar(void * stream);
extern void STDAPICALLTYPE PTimeUngetChar(void * stream, int c);
int STDAPICALLTYPE PTimeGetDateOrder();
int STDAPICALLTYPE PTimeIsMonthName(const char *, int, int);
int STDAPICALLTYPE PTimeIsDayName(const char *, int, int);
};

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


#define YYPURE
#define YYLEX_PARAM	VARIABLE->yyInput
#define YYPARSE_PARAM	parseParam

#define yyparse		PTime_yyparse
#define yylex		PTime_yylex
#define yyerror		PTime_yyerror

static int yyparse(void *); 
#ifdef __GNUC__
static int yyerror();
static int yylex();
#endif
static void SetPossibleDate(struct Variables*, time_t, time_t, time_t);


#line 119 "..\..\common\getdate.y"
typedef union {
    time_t		Number;
    enum _MERIDIAN	Meridian;
} YYSTYPE;

#ifndef YYLTYPE
typedef
  struct yyltype
    {
      int timestamp;
      int first_line;
      int first_column;
      int last_line;
      int last_column;
      char *text;
   }
  yyltype;

#define YYLTYPE yyltype
#endif

#include <stdio.h>

#ifndef __cplusplus
#ifndef __STDC__
#define const
#endif
#endif



#define	YYFINAL		52
#define	YYFLAG		-32768
#define	YYNTBASE	19

#define YYTRANSLATE(x) ((unsigned)(x) <= 270 ? yytranslate[x] : 29)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,    17,     2,     2,    18,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,    16,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     1,     2,     3,     4,     5,
     6,     7,     8,     9,    10,    11,    12,    13,    14,    15
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     1,     4,     6,     8,    10,    12,    14,    16,    19,
    24,    29,    36,    43,    45,    47,    50,    52,    55,    58,
    62,    68,    72,    76,    79,    84,    87,    91,    94,    96,
    99,   102,   104,   107,   110,   112,   115,   118,   120,   122,
   123
};

static const short yyrhs[] = {    -1,
    19,    20,     0,    21,     0,    22,     0,    24,     0,    23,
     0,    25,     0,    27,     0,    13,     7,     0,    13,    16,
    13,    28,     0,    13,    16,    13,    12,     0,    13,    16,
    13,    16,    13,    28,     0,    13,    16,    13,    16,    13,
    12,     0,    14,     0,     5,     0,    14,    15,     0,     4,
     0,     4,    17,     0,    13,     4,     0,    13,    18,    13,
     0,    13,    18,    13,    18,    13,     0,    13,    12,    12,
     0,    13,     9,    12,     0,     9,    13,     0,     9,    13,
    17,    13,     0,    13,     9,     0,    13,     9,    13,     0,
    26,     3,     0,    26,     0,    13,     8,     0,    12,     8,
     0,     8,     0,    12,    11,     0,    13,    11,     0,    11,
     0,    12,    10,     0,    13,    10,     0,    10,     0,    13,
     0,     0,     7,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
   133,   134,   137,   140,   143,   146,   149,   152,   155,   161,
   167,   174,   180,   190,   194,   198,   205,   209,   213,   219,
   222,   225,   235,   241,   245,   250,   254,   261,   265,   268,
   271,   274,   277,   280,   283,   286,   289,   292,   297,   324,
   327
};

static const char * const yytname[] = {   "$","error","$undefined.","tAGO","tDAY",
"tDAYZONE","tID","tMERIDIAN","tMINUTE_UNIT","tMONTH","tMONTH_UNIT","tSEC_UNIT",
"tSNUMBER","tUNUMBER","tZONE","tDST","':'","','","'/'","spec","item","time",
"zone","day","date","rel","relunit","number","o_merid",""
};
#endif

static const short yyr1[] = {     0,
    19,    19,    20,    20,    20,    20,    20,    20,    21,    21,
    21,    21,    21,    22,    22,    22,    23,    23,    23,    24,
    24,    24,    24,    24,    24,    24,    24,    25,    25,    26,
    26,    26,    26,    26,    26,    26,    26,    26,    27,    28,
    28
};

static const short yyr2[] = {     0,
     0,     2,     1,     1,     1,     1,     1,     1,     2,     4,
     4,     6,     6,     1,     1,     2,     1,     2,     2,     3,
     5,     3,     3,     2,     4,     2,     3,     2,     1,     2,
     2,     1,     2,     2,     1,     2,     2,     1,     1,     0,
     1
};

static const short yydefact[] = {     1,
     0,    17,    15,    32,     0,    38,    35,     0,    39,    14,
     2,     3,     4,     6,     5,     7,    29,     8,    18,    24,
    31,    36,    33,    19,     9,    30,    26,    37,    34,     0,
     0,     0,    16,    28,     0,    23,    27,    22,    40,    20,
    25,    41,    11,     0,    10,     0,    40,    21,    13,    12,
     0,     0
};

static const short yydefgoto[] = {     1,
    11,    12,    13,    14,    15,    16,    17,    18,    45
};

static const short yypact[] = {-32768,
     0,    -1,-32768,-32768,     4,-32768,-32768,    25,    11,    -8,
-32768,-32768,-32768,-32768,-32768,-32768,    21,-32768,-32768,     9,
-32768,-32768,-32768,-32768,-32768,-32768,   -10,-32768,-32768,    16,
    19,    24,-32768,-32768,    26,-32768,-32768,-32768,    18,    13,
-32768,-32768,-32768,    27,-32768,    28,    -6,-32768,-32768,-32768,
    38,-32768
};

static const short yypgoto[] = {-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,    -5
};


#define	YYLAST		42


static const short yytable[] = {    51,
    42,    36,    37,     2,     3,    49,    33,     4,     5,     6,
     7,     8,     9,    10,    24,    19,    20,    25,    26,    27,
    28,    29,    30,    34,    42,    35,    31,    38,    32,    43,
    46,    39,    21,    44,    22,    23,    40,    52,    41,    47,
    48,    50
};

static const short yycheck[] = {     0,
     7,    12,    13,     4,     5,    12,    15,     8,     9,    10,
    11,    12,    13,    14,     4,    17,    13,     7,     8,     9,
    10,    11,    12,     3,     7,    17,    16,    12,    18,    12,
    18,    13,     8,    16,    10,    11,    13,     0,    13,    13,
    13,    47
};
/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "bison.simple"

/* Skeleton output parser for bison,
   Copyright (C) 1984, 1989, 1990 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

#ifdef __GNUC__
#define alloca __builtin_alloca
#else /* not __GNUC__ */
#if HAVE_ALLOCA_H
#include <alloca.h>
#else /* not HAVE_ALLOCA_H */
#ifdef _AIX
 #pragma alloca
#else /* not _AIX */
char *alloca ();
#endif /* not _AIX */
#endif /* not HAVE_ALLOCA_H */
#endif /* not __GNUC__ */

extern int yylex(YYSTYPE *, void *);
extern void yyerror(const char*);

#ifndef alloca
#ifdef __GNUC__
#define alloca __builtin_alloca
#else /* not GNU C.  */
#if (!defined (__STDC__) && defined (sparc)) || defined (__sparc__) || defined (__sparc) || defined (__sgi)
#include <alloca.h>
#else /* not sparc */
#if defined (MSDOS) && !defined (__TURBOC__)
#include <malloc.h>
#else /* not MSDOS, or __TURBOC__ */
#if defined(_AIX)
#include <malloc.h>
 #pragma alloca
#else /* not MSDOS, __TURBOC__, or _AIX */
#ifdef __hpux
#ifdef __cplusplus
extern "C" {
void *alloca (unsigned int);
};
#else /* not __cplusplus */
void *alloca ();
#endif /* not __cplusplus */
#endif /* __hpux */
#endif /* not _AIX */
#endif /* not MSDOS, or __TURBOC__ */
#endif /* not sparc.  */
#endif /* not GNU C.  */
#endif /* alloca not defined.  */

/* This is the parser code that is written into each bison parser
  when the %semantic_parser declaration is not specified in the grammar.
  It was written by Richard Stallman by simplifying the hairy parser
  used when %semantic_parser is specified.  */

/* Note: there must be only one dollar sign in this file.
   It is replaced by the list of actions, each action
   as one case of the switch.  */

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0
#define YYACCEPT	return(0)
#define YYABORT 	return(1)
#define YYERROR		goto yyerrlab1
/* Like YYERROR except do call yyerror.
   This remains here temporarily to ease the
   transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */
#define YYFAIL		goto yyerrlab
#define YYRECOVERING()  (!!yyerrstatus)
#define YYBACKUP(token, value) \
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    { yychar = (token), yylval = (value);			\
      yychar1 = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { yyerror ("syntax error: cannot back up"); YYERROR; }	\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

#ifndef YYPURE
#define YYLEX		yylex()
#endif

#ifdef YYPURE
#ifdef YYLSP_NEEDED
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, &yylloc, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval, &yylloc)
#endif
#else /* not YYLSP_NEEDED */
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval)
#endif
#endif /* not YYLSP_NEEDED */
#endif

/* If nonreentrant, generate the variables here */

#ifndef YYPURE

int	yychar;			/*  the lookahead symbol		*/
YYSTYPE	yylval;			/*  the semantic value of the		*/
				/*  lookahead symbol			*/

#ifdef YYLSP_NEEDED
YYLTYPE yylloc;			/*  location data for the lookahead	*/
				/*  symbol				*/
#endif

int yynerrs;			/*  number of parse errors so far       */
#endif  /* not YYPURE */

#if YYDEBUG != 0
int yydebug;			/*  nonzero means print parse trace	*/
/* Since this is uninitialized, it does not stop multiple parsers
   from coexisting.  */
#endif

/*  YYINITDEPTH indicates the initial size of the parser's stacks	*/

#ifndef	YYINITDEPTH
#define YYINITDEPTH 200
#endif

/*  YYMAXDEPTH is the maximum size the stacks can grow to
    (effective only if the built-in stack extension method is used).  */

#if YYMAXDEPTH == 0
#undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
#define YYMAXDEPTH 10000
#endif

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
int yyparse (void);
#endif

#if __GNUC__ > 1		/* GNU C and GNU C++ define this.  */
#define __yy_memcpy(FROM,TO,COUNT)	__builtin_memcpy(TO,FROM,COUNT)
#else				/* not GNU C or C++ */
#ifndef __cplusplus

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (from, to, count)
     char *from;
     char *to;
     int count;
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#else /* __cplusplus */

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (char *from, char *to, int count)
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#endif
#endif

#line 192 "bison.simple"

/* The user can define YYPARSE_PARAM as the name of an argument to be passed
   into yyparse.  The argument should have type void *.
   It should actually point to an object.
   Grammar actions can access the variable by casting it
   to the proper pointer type.  */

#ifdef YYPARSE_PARAM
#define YYPARSE_PARAM_DECL void *YYPARSE_PARAM;
#else
#define YYPARSE_PARAM
#define YYPARSE_PARAM_DECL
#endif

int
yyparse(void* YYPARSE_PARAM)
     // YYPARSE_PARAM_DECL
{
  register int yystate;
  register int yyn;
  register short *yyssp;
  register YYSTYPE *yyvsp;
  int yyerrstatus;	/*  number of tokens to shift before error messages enabled */
  int yychar1 = 0;		/*  lookahead token as an internal (translated) token number */

  short	yyssa[YYINITDEPTH];	/*  the state stack			*/
  YYSTYPE yyvsa[YYINITDEPTH];	/*  the semantic value stack		*/

  short *yyss = yyssa;		/*  refer to the stacks thru separate pointers */
  YYSTYPE *yyvs = yyvsa;	/*  to allow yyoverflow to reallocate them elsewhere */

#ifdef YYLSP_NEEDED
  YYLTYPE yylsa[YYINITDEPTH];	/*  the location stack			*/
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;

#define YYPOPSTACK   (yyvsp--, yyssp--, yylsp--)
#else
#define YYPOPSTACK   (yyvsp--, yyssp--)
#endif

  int yystacksize = YYINITDEPTH;

#ifdef YYPURE
  int yychar;
  YYSTYPE yylval;
  int yynerrs;
#ifdef YYLSP_NEEDED
  YYLTYPE yylloc;
#endif
#endif

  YYSTYPE yyval;		/*  the variable used to return		*/
				/*  semantic values from the action	*/
				/*  routines				*/

  int yylen;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Starting parse\n");
#endif

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss - 1;
  yyvsp = yyvs;
#ifdef YYLSP_NEEDED
  yylsp = yyls;
#endif

/* Push a new state, which is found in  yystate  .  */
/* In all cases, when you get here, the value and location stacks
   have just been pushed. so pushing a state here evens the stacks.  */
yynewstate:

  *++yyssp = (short)yystate;

  if (yyssp >= yyss + yystacksize - 1)
    {
      /* Give user a chance to reallocate the stack */
      /* Use copies of these so that the &'s don't force the real ones into memory. */
      YYSTYPE *yyvs1 = yyvs;
      short *yyss1 = yyss;
#ifdef YYLSP_NEEDED
      YYLTYPE *yyls1 = yyls;
#endif

      /* Get the current used size of the three stacks, in elements.  */
      int size = yyssp - yyss + 1;

#ifdef yyoverflow
      /* Each stack pointer address is followed by the size of
	 the data in use in that stack, in bytes.  */
#ifdef YYLSP_NEEDED
      /* This used to be a conditional around just the two extra args,
	 but that might be undefined if yyoverflow is a macro.  */
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yyls1, size * sizeof (*yylsp),
		 &yystacksize);
#else
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yystacksize);
#endif

      yyss = yyss1; yyvs = yyvs1;
#ifdef YYLSP_NEEDED
      yyls = yyls1;
#endif
#else /* no yyoverflow */
      /* Extend the stack our own way.  */
      if (yystacksize >= YYMAXDEPTH)
	{
	  yyerror("parser stack overflow");
	  return 2;
	}
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;
      yyss = (short *) alloca (yystacksize * sizeof (*yyssp));
      __yy_memcpy ((char *)yyss1, (char *)yyss, size * sizeof (*yyssp));
      yyvs = (YYSTYPE *) alloca (yystacksize * sizeof (*yyvsp));
      __yy_memcpy ((char *)yyvs1, (char *)yyvs, size * sizeof (*yyvsp));
#ifdef YYLSP_NEEDED
      yyls = (YYLTYPE *) alloca (yystacksize * sizeof (*yylsp));
      __yy_memcpy ((char *)yyls1, (char *)yyls, size * sizeof (*yylsp));
#endif
#endif /* no yyoverflow */

      yyssp = yyss + size - 1;
      yyvsp = yyvs + size - 1;
#ifdef YYLSP_NEEDED
      yylsp = yyls + size - 1;
#endif

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Stack size increased to %d\n", yystacksize);
#endif

      if (yyssp >= yyss + yystacksize - 1)
	YYABORT;
    }

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Entering state %d\n", yystate);
#endif

  goto yybackup;
 yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* yychar is either YYEMPTY or YYEOF
     or a valid token in external form.  */

  if (yychar == YYEMPTY)
    {
#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Reading a token: ");
#endif
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with */

  if (yychar <= 0)		/* This means end of input. */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more */

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Now at end of input.\n");
#endif
    }
  else
    {
      yychar1 = YYTRANSLATE(yychar);

#if YYDEBUG != 0
      if (yydebug)
	{
	  fprintf (stderr, "Next token is %d (%s", yychar, yytname[yychar1]);
	  /* Give the individual parser a way to print the precise meaning
	     of a token, for further debugging info.  */
#ifdef YYPRINT
	  YYPRINT (stderr, yychar, yylval);
#endif
	  fprintf (stderr, ")\n");
	}
#endif
    }

  yyn += yychar1;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != yychar1)
    goto yydefault;

  yyn = yytable[yyn];

  /* yyn is what to do for this token type in this state.
     Negative => reduce, -yyn is rule number.
     Positive => shift, yyn is new state.
       New state is final state => don't bother to shift,
       just return success.
     0, or most negative number => error.  */

  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrlab;

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting token %d (%s), ", yychar, yytname[yychar1]);
#endif

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  /* count tokens shifted since error; after three, turn off error status.  */
  if (yyerrstatus) yyerrstatus--;

  yystate = yyn;
  goto yynewstate;

/* Do the default action for the current state.  */
yydefault:

  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;

/* Do a reduction.  yyn is the number of a rule to reduce with.  */
yyreduce:
  yylen = yyr2[yyn];
  if (yylen > 0)
    yyval = yyvsp[1-yylen]; /* implement default value of the action */

#if YYDEBUG != 0
  if (yydebug)
    {
      int i;

      fprintf (stderr, "Reducing via rule %d (line %d), ",
	       yyn, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (i = yyprhs[yyn]; yyrhs[i] > 0; i++)
	fprintf (stderr, "%s ", yytname[yyrhs[i]]);
      fprintf (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif


  switch (yyn) {

case 3:
#line 137 "..\..\common\getdate.y"
{
	    VARIABLE->yyHaveTime++;
	;
    break;}
case 4:
#line 140 "..\..\common\getdate.y"
{
	    VARIABLE->yyHaveZone++;
	;
    break;}
case 5:
#line 143 "..\..\common\getdate.y"
{
	    VARIABLE->yyHaveDate++;
	;
    break;}
case 6:
#line 146 "..\..\common\getdate.y"
{
	    VARIABLE->yyHaveDay++;
	;
    break;}
case 7:
#line 149 "..\..\common\getdate.y"
{
	    VARIABLE->yyHaveRel++;
	;
    break;}
case 9:
#line 155 "..\..\common\getdate.y"
{
	    VARIABLE->yyHour = yyvsp[-1].Number;
	    VARIABLE->yyMinutes = 0;
	    VARIABLE->yySeconds = 0;
	    VARIABLE->yyMeridian = yyvsp[0].Meridian;
	;
    break;}
case 10:
#line 161 "..\..\common\getdate.y"
{
	    VARIABLE->yyHour = yyvsp[-3].Number;
	    VARIABLE->yyMinutes = yyvsp[-1].Number;
	    VARIABLE->yySeconds = 0;
	    VARIABLE->yyMeridian = yyvsp[0].Meridian;
	;
    break;}
case 11:
#line 167 "..\..\common\getdate.y"
{
	    VARIABLE->yyHour = yyvsp[-3].Number;
	    VARIABLE->yyMinutes = yyvsp[-1].Number;
	    VARIABLE->yyMeridian = MER24;
	    VARIABLE->yyDSTmode = DSToff;
	    VARIABLE->yyTimezone = - (yyvsp[0].Number % 100 + (yyvsp[0].Number / 100) * 60);
	;
    break;}
case 12:
#line 174 "..\..\common\getdate.y"
{
	    VARIABLE->yyHour = yyvsp[-5].Number;
	    VARIABLE->yyMinutes = yyvsp[-3].Number;
	    VARIABLE->yySeconds = yyvsp[-1].Number;
	    VARIABLE->yyMeridian = yyvsp[0].Meridian;
	;
    break;}
case 13:
#line 180 "..\..\common\getdate.y"
{
	    VARIABLE->yyHour = yyvsp[-5].Number;
	    VARIABLE->yyMinutes = yyvsp[-3].Number;
	    VARIABLE->yySeconds = yyvsp[-1].Number;
	    VARIABLE->yyMeridian = MER24;
	    VARIABLE->yyDSTmode = DSToff;
	    VARIABLE->yyTimezone = - (yyvsp[0].Number % 100 + (yyvsp[0].Number / 100) * 60);
	;
    break;}
case 14:
#line 190 "..\..\common\getdate.y"
{
	    VARIABLE->yyTimezone = yyvsp[0].Number;
	    VARIABLE->yyDSTmode = DSToff;
	;
    break;}
case 15:
#line 194 "..\..\common\getdate.y"
{
	    VARIABLE->yyTimezone = yyvsp[0].Number;
	    VARIABLE->yyDSTmode = DSTon;
	;
    break;}
case 16:
#line 199 "..\..\common\getdate.y"
{
	    VARIABLE->yyTimezone = yyvsp[-1].Number;
	    VARIABLE->yyDSTmode = DSTon;
	;
    break;}
case 17:
#line 205 "..\..\common\getdate.y"
{
	    VARIABLE->yyDayOrdinal = 1;
	    VARIABLE->yyDayNumber = yyvsp[0].Number;
	;
    break;}
case 18:
#line 209 "..\..\common\getdate.y"
{
	    VARIABLE->yyDayOrdinal = 1;
	    VARIABLE->yyDayNumber = yyvsp[-1].Number;
	;
    break;}
case 19:
#line 213 "..\..\common\getdate.y"
{
	    VARIABLE->yyDayOrdinal = yyvsp[-1].Number;
	    VARIABLE->yyDayNumber = yyvsp[0].Number;
	;
    break;}
case 20:
#line 219 "..\..\common\getdate.y"
{
	    SetPossibleDate(VARIABLE, yyvsp[-2].Number, yyvsp[0].Number, VARIABLE->yyYear);
	;
    break;}
case 21:
#line 222 "..\..\common\getdate.y"
{
	    SetPossibleDate(VARIABLE, yyvsp[-4].Number, yyvsp[-2].Number, yyvsp[0].Number);
	;
    break;}
case 22:
#line 225 "..\..\common\getdate.y"
{
	    /* ISO 8601 format.  yyyy-mm-dd.  */
	    if (yyvsp[-2].Number > 31) {
		VARIABLE->yyYear = yyvsp[-2].Number;
		VARIABLE->yyMonth = -yyvsp[-1].Number;
		VARIABLE->yyDay = -yyvsp[0].Number;
	    }
	    else
		SetPossibleDate(VARIABLE, yyvsp[-2].Number, -yyvsp[-1].Number, -yyvsp[0].Number);
	;
    break;}
case 23:
#line 235 "..\..\common\getdate.y"
{
	    /* e.g. 17-JUN-1992.  */
	    VARIABLE->yyDay = yyvsp[-2].Number;
	    VARIABLE->yyMonth = yyvsp[-1].Number;
	    VARIABLE->yyYear = -yyvsp[0].Number;
	;
    break;}
case 24:
#line 241 "..\..\common\getdate.y"
{
	    VARIABLE->yyMonth = yyvsp[-1].Number;
	    VARIABLE->yyDay = yyvsp[0].Number;
	;
    break;}
case 25:
#line 245 "..\..\common\getdate.y"
{
	    VARIABLE->yyMonth = yyvsp[-3].Number;
	    VARIABLE->yyDay = yyvsp[-2].Number;
	    VARIABLE->yyYear = yyvsp[0].Number;
	;
    break;}
case 26:
#line 250 "..\..\common\getdate.y"
{
	    VARIABLE->yyMonth = yyvsp[0].Number;
	    VARIABLE->yyDay = yyvsp[-1].Number;
	;
    break;}
case 27:
#line 254 "..\..\common\getdate.y"
{
	    VARIABLE->yyMonth = yyvsp[-1].Number;
	    VARIABLE->yyDay = yyvsp[-2].Number;
	    VARIABLE->yyYear = yyvsp[0].Number;
	;
    break;}
case 28:
#line 261 "..\..\common\getdate.y"
{
	    VARIABLE->yyRelSeconds = -VARIABLE->yyRelSeconds;
	    VARIABLE->yyRelMonth = -VARIABLE->yyRelMonth;
	;
    break;}
case 30:
#line 268 "..\..\common\getdate.y"
{
	    VARIABLE->yyRelSeconds += yyvsp[-1].Number * yyvsp[0].Number * 60L;
	;
    break;}
case 31:
#line 271 "..\..\common\getdate.y"
{
	    VARIABLE->yyRelSeconds += yyvsp[-1].Number * yyvsp[0].Number * 60L;
	;
    break;}
case 32:
#line 274 "..\..\common\getdate.y"
{
	    VARIABLE->yyRelSeconds += yyvsp[0].Number * 60L;
	;
    break;}
case 33:
#line 277 "..\..\common\getdate.y"
{
	    VARIABLE->yyRelSeconds += yyvsp[-1].Number;
	;
    break;}
case 34:
#line 280 "..\..\common\getdate.y"
{
	    VARIABLE->yyRelSeconds += yyvsp[-1].Number;
	;
    break;}
case 35:
#line 283 "..\..\common\getdate.y"
{
	    VARIABLE->yyRelSeconds++;
	;
    break;}
case 36:
#line 286 "..\..\common\getdate.y"
{
	    VARIABLE->yyRelMonth += yyvsp[-1].Number * yyvsp[0].Number;
	;
    break;}
case 37:
#line 289 "..\..\common\getdate.y"
{
	    VARIABLE->yyRelMonth += yyvsp[-1].Number * yyvsp[0].Number;
	;
    break;}
case 38:
#line 292 "..\..\common\getdate.y"
{
	    VARIABLE->yyRelMonth += yyvsp[0].Number;
	;
    break;}
case 39:
#line 297 "..\..\common\getdate.y"
{
	    if (VARIABLE->yyHaveTime && VARIABLE->yyHaveDate && !VARIABLE->yyHaveRel)
		VARIABLE->yyYear = yyvsp[0].Number;
	    else {
		if(yyvsp[0].Number>10000) {
		    VARIABLE->yyHaveDate++;
		    VARIABLE->yyDay= (yyvsp[0].Number)%100;
		    VARIABLE->yyMonth= (yyvsp[0].Number/100)%100;
		    VARIABLE->yyYear = yyvsp[0].Number/10000;
		}
		else {
		    VARIABLE->yyHaveTime++;
		    if (yyvsp[0].Number < 100) {
			VARIABLE->yyHour = yyvsp[0].Number;
			VARIABLE->yyMinutes = 0;
		    }
		    else {
		    	VARIABLE->yyHour = yyvsp[0].Number / 100;
		    	VARIABLE->yyMinutes = yyvsp[0].Number % 100;
		    }
		    VARIABLE->yySeconds = 0;
		    VARIABLE->yyMeridian = MER24;
	        }
	    }
	;
    break;}
case 40:
#line 324 "..\..\common\getdate.y"
{
	    yyval.Meridian = MER24;
	;
    break;}
case 41:
#line 327 "..\..\common\getdate.y"
{
	    yyval.Meridian = yyvsp[0].Meridian;
	;
    break;}
}
   /* the action file gets copied in in place of this dollarsign */
#line 487 "bison.simple"

  yyvsp -= yylen;
  yyssp -= yylen;
#ifdef YYLSP_NEEDED
  yylsp -= yylen;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

  *++yyvsp = yyval;

#ifdef YYLSP_NEEDED
  yylsp++;
  if (yylen == 0)
    {
      yylsp->first_line = yylloc.first_line;
      yylsp->first_column = yylloc.first_column;
      yylsp->last_line = (yylsp-1)->last_line;
      yylsp->last_column = (yylsp-1)->last_column;
      yylsp->text = 0;
    }
  else
    {
      yylsp->last_line = (yylsp+yylen-1)->last_line;
      yylsp->last_column = (yylsp+yylen-1)->last_column;
    }
#endif

  /* Now "shift" the result of the reduction.
     Determine what state that goes to,
     based on the state we popped back to
     and the rule number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTBASE] + *yyssp;
  if (yystate >= 0 && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTBASE];

  goto yynewstate;

yyerrlab:   /* here on detecting error */

  if (! yyerrstatus)
    /* If not already recovering from an error, report this error.  */
    {
      ++yynerrs;

#ifdef YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (yyn > YYFLAG && yyn < YYLAST)
	{
	  int size = 0;
	  char *msg;
	  int x, count;

	  count = 0;
	  /* Start X at -yyn if nec to avoid negative indexes in yycheck.  */
	  for (x = (yyn < 0 ? -yyn : 0);
	       x < (sizeof(yytname) / sizeof(char *)); x++)
	    if (yycheck[x + yyn] == x)
	      size += strlen(yytname[x]) + 15, count++;
	  msg = (char *) malloc(size + 15);
	  if (msg != 0)
	    {
	      strcpy(msg, "parse error");

	      if (count < 5)
		{
		  count = 0;
		  for (x = (yyn < 0 ? -yyn : 0);
		       x < (sizeof(yytname) / sizeof(char *)); x++)
		    if (yycheck[x + yyn] == x)
		      {
			strcat(msg, count == 0 ? ", expecting `" : " or `");
			strcat(msg, yytname[x]);
			strcat(msg, "'");
			count++;
		      }
		}
	      yyerror(msg);
	      free(msg);
	    }
	  else
	    yyerror ("parse error; also virtual memory exceeded");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror("parse error");
    }

  goto yyerrlab1;
yyerrlab1:   /* here on error raised explicitly by an action */

  if (yyerrstatus == 3)
    {
      /* if just tried and failed to reuse lookahead token after an error, discard it.  */

      /* return failure if at end of input */
      if (yychar == YYEOF)
	YYABORT;

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Discarding token %d (%s).\n", yychar, yytname[yychar1]);
#endif

      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token
     after shifting the error token.  */

  yyerrstatus = 3;		/* Each real token shifted decrements this */

  goto yyerrhandle;

yyerrdefault:  /* current state does not do anything special for the error token. */

#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */
  yyn = yydefact[yystate];  /* If its default is to accept any token, ok.  Otherwise pop it.*/
  if (yyn) goto yydefault;
#endif

yyerrpop:   /* pop the current state because it cannot handle the error token */

  if (yyssp == yyss) YYABORT;
  yyvsp--;
  yystate = *--yyssp;
#ifdef YYLSP_NEEDED
  yylsp--;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "Error: state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

yyerrhandle:

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yyerrdefault;

  yyn += YYTERROR;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != YYTERROR)
    goto yyerrdefault;

  yyn = yytable[yyn];
  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrpop;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrpop;

  if (yyn == YYFINAL)
    YYACCEPT;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting error token, ");
#endif

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  yystate = yyn;
  goto yynewstate;
}
#line 332 "..\..\common\getdate.y"


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
    { "a",	tZONE,	HOUR(  1) },
    { "b",	tZONE,	HOUR(  2) },
    { "c",	tZONE,	HOUR(  3) },
    { "d",	tZONE,	HOUR(  4) },
    { "e",	tZONE,	HOUR(  5) },
    { "f",	tZONE,	HOUR(  6) },
    { "g",	tZONE,	HOUR(  7) },
    { "h",	tZONE,	HOUR(  8) },
    { "i",	tZONE,	HOUR(  9) },
    { "k",	tZONE,	HOUR( 10) },
    { "l",	tZONE,	HOUR( 11) },
    { "m",	tZONE,	HOUR( 12) },
    { "n",	tZONE,	HOUR(- 1) },
    { "o",	tZONE,	HOUR(- 2) },
    { "p",	tZONE,	HOUR(- 3) },
    { "q",	tZONE,	HOUR(- 4) },
    { "r",	tZONE,	HOUR(- 5) },
    { "s",	tZONE,	HOUR(- 6) },
    { "t",	tZONE,	HOUR(- 7) },
    { "u",	tZONE,	HOUR(- 8) },
    { "v",	tZONE,	HOUR(- 9) },
    { "w",	tZONE,	HOUR(-10) },
    { "x",	tZONE,	HOUR(-11) },
    { "y",	tZONE,	HOUR(-12) },
    { "z",	tZONE,	HOUR(  0) },
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
	    while (isdigit(c)) {
		yylval->Number = 10 * yylval->Number + c - '0';
		c = PTimeGetChar(yyInput);
	    }
	    PTimeUngetChar(yyInput, c);
	    if (sign < 0)
		yylval->Number = -yylval->Number;
	    return sign ? tSNUMBER : tUNUMBER;
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
    Year = Month / 12;
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

extern "C" {
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
