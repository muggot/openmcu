/*
   ITU-T G.729A Speech Coder    ANSI-C Source Code
   Version 1.1    Last modified: September 1996

   Copyright (c) 1996,
   AT&T, France Telecom, NTT, Universite de Sherbrooke
   All rights reserved.
*/

#include "typedef.h"
#include "basic_op.h"

#include "ld8a.h"
#include "tab_ld8a.h"

/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : Pow2()                                                  |
 |                                                                           |
 |     L_x = pow(2.0, exponent.fraction)                                     |
 |---------------------------------------------------------------------------|
 |  Algorithm:                                                               |
 |                                                                           |
 |   The function Pow2(L_x) is approximated by a table and linear            |
 |   interpolation.                                                          |
 |                                                                           |
 |   1- i = bit10-b15 of fraction,   0 <= i <= 31                            |
 |   2- a = bit0-b9   of fraction                                            |
 |   3- L_x = tabpow[i]<<16 - (tabpow[i] - tabpow[i+1]) * a * 2                 |
 |   4- L_x = L_x >> (30-exponent)     (with rounding)                       |
 |___________________________________________________________________________|
*/


Word32
Pow2 (				/* (o) Q0  : result       (range: 0<=val<=0x7fffffff) */
       Word16 exponent,		/* (i) Q0  : Integer part.      (range: 0<=val<=30)   */
       Word16 fraction		/* (i) Q15 : Fractional part.   (range: 0.0<=val<1.0) */
  )
{
  Word16 exp, i, a, tmp;
  Word32 L_x;

  L_x = L_mult (fraction, 32);	/* L_x = fraction<<6           */
  i = extract_h (L_x);		/* Extract b10-b15 of fraction */
  L_x = L_shr (L_x, 1);
  a = extract_l (L_x);		/* Extract b0-b9   of fraction */
  a = a & (Word16) 0x7fff;

  L_x = L_deposit_h (tabpow[i]);	/* tabpow[i] << 16        */
  tmp = sub (tabpow[i], tabpow[i + 1]);	/* tabpow[i] - tabpow[i+1] */
  L_x = L_msu (L_x, tmp, a);	/* L_x -= tmp*a*2        */

  exp = sub (30, exponent);
  L_x = L_shr_r (L_x, exp);

  return (L_x);
}

/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : Log2()                                                  |
 |                                                                           |
 |       Compute log2(L_x).                                                  |
 |       L_x is positive.                                                    |
 |                                                                           |
 |       if L_x is negative or zero, result is 0.                            |
 |---------------------------------------------------------------------------|
 |  Algorithm:                                                               |
 |                                                                           |
 |   The function Log2(L_x) is approximated by a table and linear            |
 |   interpolation.                                                          |
 |                                                                           |
 |   1- Normalization of L_x.                                                |
 |   2- exponent = 30-exponent                                               |
 |   3- i = bit25-b31 of L_x,    32 <= i <= 63  ->because of normalization.  |
 |   4- a = bit10-b24                                                        |
 |   5- i -=32                                                               |
 |   6- fraction = tablog[i]<<16 - (tablog[i] - tablog[i+1]) * a * 2            |
 |___________________________________________________________________________|
*/

void
Log2 (Word32 L_x,		/* (i) Q0 : input value                                 */
      Word16 * exponent,	/* (o) Q0 : Integer part of Log2.   (range: 0<=val<=30) */
      Word16 * fraction		/* (o) Q15: Fractional  part of Log2. (range: 0<=val<1) */
  )
{
  Word16 exp, i, a, tmp;
  Word32 L_y;

  if (L_x <= (Word32) 0) {
    *exponent = 0;
    *fraction = 0;
    return;
  }

  exp = norm_l (L_x);
  L_x = L_shl (L_x, exp);	/* L_x is normalized */

  *exponent = sub (30, exp);

  L_x = L_shr (L_x, 9);
  i = extract_h (L_x);		/* Extract b25-b31 */
  L_x = L_shr (L_x, 1);
  a = extract_l (L_x);		/* Extract b10-b24 of fraction */
  a = a & (Word16) 0x7fff;

  i = sub (i, 32);

  L_y = L_deposit_h (tablog[i]);	/* tablog[i] << 16        */
  tmp = sub (tablog[i], tablog[i + 1]);	/* tablog[i] - tablog[i+1] */
  L_y = L_msu (L_y, tmp, a);	/* L_y -= tmp*a*2        */

  *fraction = extract_h (L_y);

  return;
}

/*___________________________________________________________________________
 |                                                                           |
 |   Function Name : Inv_sqrt                                                |
 |                                                                           |
 |       Compute 1/sqrt(L_x).                                                |
 |       L_x is positive.                                                    |
 |                                                                           |
 |       if L_x is negative or zero, result is 1 (3fff ffff).                |
 |---------------------------------------------------------------------------|
 |  Algorithm:                                                               |
 |                                                                           |
 |   The function 1/sqrt(L_x) is approximated by a table and linear          |
 |   interpolation.                                                          |
 |                                                                           |
 |   1- Normalization of L_x.                                                |
 |   2- If (30-exponent) is even then shift right once.                      |
 |   3- exponent = (30-exponent)/2  +1                                       |
 |   4- i = bit25-b31 of L_x,    16 <= i <= 63  ->because of normalization.  |
 |   5- a = bit10-b24                                                        |
 |   6- i -=16                                                               |
 |   7- L_y = tabsqr[i]<<16 - (tabsqr[i] - tabsqr[i+1]) * a * 2                 |
 |   8- L_y >>= exponent                                                     |
 |___________________________________________________________________________|
*/


Word32
Inv_sqrt (			/* (o) Q30 : output value   (range: 0<=val<1)           */
	   Word32 L_x		/* (i) Q0  : input value    (range: 0<=val<=7fffffff)   */
  )
{
  Word16 exp, i, a, tmp;
  Word32 L_y;

  if (L_x <= (Word32) 0)
    return ((Word32) 0x3fffffffL);

  exp = norm_l (L_x);
  L_x = L_shl (L_x, exp);	/* L_x is normalize */

  exp = sub (30, exp);
  if ((exp & 1) == 0)		/* If exponent even -> shift right */
    L_x = L_shr (L_x, 1);

  exp = shr (exp, 1);
  exp = add (exp, 1);

  L_x = L_shr (L_x, 9);
  i = extract_h (L_x);		/* Extract b25-b31 */
  L_x = L_shr (L_x, 1);
  a = extract_l (L_x);		/* Extract b10-b24 */
  a = a & (Word16) 0x7fff;

  i = sub (i, 16);

  L_y = L_deposit_h (tabsqr[i]);	/* tabsqr[i] << 16          */
  tmp = sub (tabsqr[i], tabsqr[i + 1]);	/* tabsqr[i] - tabsqr[i+1])  */
  L_y = L_msu (L_y, tmp, a);	/* L_y -=  tmp*a*2         */

  L_y = L_shr (L_y, exp);	/* denormalization */

  return (L_y);
}
