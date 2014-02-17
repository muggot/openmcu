#include <ptlib.h>

#define	LINE_SIZE_STEP	100

#define	DEFAULT_FILE_MODE	(S_IRUSR|S_IWUSR|S_IROTH|S_IRGRP)

#if !P_USE_INLINES
#pragma message ("!P_USE_INLINES including ptlib.inl, osutil.inl")
#include <ptlib/osutil.inl>
#include <ptlib/ptlib.inl>
#endif

#ifdef P_USE_LANGINFO
#pragma message ("H")
#include <langinfo.h>
#endif

#define	LINE_SIZE_STEP	100

#define	DEFAULT_FILE_MODE	(S_IRUSR|S_IWUSR|S_IROTH|S_IRGRP)

#define new PNEW

//NU_MEMORY_POOL System_Memory;

ostream & operator<<(ostream & s, PInt64 v)
{
/*  char buffer[25];*/

  if ((s.flags()&ios::hex) != 0)
    return s/* << _ui64toa(v, buffer, 16)*/;

  if ((s.flags()&ios::oct) != 0)
    return s/* << _ui64toa(v, buffer, 8)*/;

  if (v < 0) {
    s << '-';
    v = -v;
  }

  return s/* << _i64toa(v, buffer, 10)*/;
}


ostream & operator<<(ostream & s, PUInt64 v)
{
/*  char buffer[25];*/
  return s/* << _ui64toa(v, buffer, (s.flags()&ios::oct) ? 8 : ((s.flags()&ios::hex) ? 16 : 10))*/;
}


PInt64 PString::AsInt64(unsigned base) const
{
//  if (base == 10)
//    return _atoi64(theArray);

  PAssert(base >= 2 && base <= 36, PInvalidParameter);

  PInt64 total = 0;
  const char * ptr = theArray;

  while (isspace(*ptr))
    ptr++;

  BOOL negative = *ptr == '-';
  if (*ptr == '-' || *ptr == '+')
    ptr++;

  for (;;) {
    unsigned c = *ptr++;
    if (c < '0')
      break;

    if (c <= '9')
      c -= '0';
    else
      c = toupper(c) - 'A' + 10;

    if (c >= base)
      break;

    total = base * total + c;

    c = *ptr++;
  }

  if (negative)
    return -total;
  else
    return total;
}
