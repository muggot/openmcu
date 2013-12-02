/*
   ITU-T G.729A Speech Coder    ANSI-C Source Code
   Version 1.1    Last modified: September 1996

   Copyright (c) 1996,
   AT&T, France Telecom, NTT, Universite de Sherbrooke
   All rights reserved.
*/

/*-----------------------------------------------------*
 * Function Autocorr()                                 *
 *                                                     *
 *   Compute autocorrelations of signal with windowing *
 *                                                     *
 *-----------------------------------------------------*/

#include "typedef.h"
#include "basic_op.h"
#include "oper_32b.h"

#include "ld8a.h"
#include "tab_ld8a.h"

void
Autocorr (Word16 x[],		/* (i)    : Input signal                      */
	  Word16 m,		/* (i)    : LPC order                         */
	  Word16 r_h[],		/* (o)    : Autocorrelations  (msb)           */
	  Word16 r_l[]		/* (o)    : Autocorrelations  (lsb)           */
  )
{
  Word16 i, j, norm;
  Word16 y[L_WINDOW];
  Word32 sum;
  Flag Overflow;

  /* Windowing of signal */

  for (i = 0; i < L_WINDOW; i++) {
    y[i] = mult_r (x[i], hamwindow[i]);
  }

  /* Compute r[0] and test for overflow */

  do {
    Overflow = 0;
    sum = 1;			/* Avoid case of all zeros */
    for (i = 0; i < L_WINDOW; i++)
      sum = L_mac_o (sum, y[i], y[i], &Overflow);

    /* If overflow divide y[] by 4 */

    if (Overflow != 0) {
      for (i = 0; i < L_WINDOW; i++) {
	y[i] = shr (y[i], 2);
      }
    }
  } while (Overflow != 0);

  /* Normalization of r[0] */

  norm = norm_l (sum);
  sum = L_shl (sum, norm);
  L_Extract (sum, &r_h[0], &r_l[0]);	/* Put in DPF format (see oper_32b) */

  /* r[1] to r[m] */

  for (i = 1; i <= m; i++) {
    sum = 0;
    for (j = 0; j < L_WINDOW - i; j++)
      sum = L_mac (sum, y[j], y[j + i]);

    sum = L_shl (sum, norm);
    L_Extract (sum, &r_h[i], &r_l[i]);
  }
  return;
}


/*-------------------------------------------------------*
 * Function Lag_window()                                 *
 *                                                       *
 * Lag_window on autocorrelations.                       *
 *                                                       *
 * r[i] *= lag_wind[i]                                   *
 *                                                       *
 *  r[i] and lag_wind[i] are in special double precision.*
 *  See "oper_32b.c" for the format                      *
 *                                                       *
 *-------------------------------------------------------*/

void
Lag_window (Word16 m,		/* (i)     : LPC order                        */
	    Word16 r_h[],	/* (i/o)   : Autocorrelations  (msb)          */
	    Word16 r_l[]	/* (i/o)   : Autocorrelations  (lsb)          */
  )
{
  Word16 i;
  Word32 x;

  for (i = 1; i <= m; i++) {
    x = Mpy_32 (r_h[i], r_l[i], lag_h[i - 1], lag_l[i - 1]);
    L_Extract (x, &r_h[i], &r_l[i]);
  }
  return;
}


/*___________________________________________________________________________
 |                                                                           |
 |      LEVINSON-DURBIN algorithm in double precision                        |
 |      ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~                        |
 |---------------------------------------------------------------------------|
 |                                                                           |
 | Algorithm                                                                 |
 |                                                                           |
 |       R[i]    autocorrelations.                                           |
 |       A[i]    filter coefficients.                                        |
 |       K       reflection coefficients.                                    |
 |       Alpha   prediction gain.                                            |
 |                                                                           |
 |       Initialization:                                                     |
 |               A[0] = 1                                                    |
 |               K    = -R[1]/R[0]                                           |
 |               A[1] = K                                                    |
 |               Alpha = R[0] * (1-K**2]                                     |
 |                                                                           |
 |       Do for  i = 2 to M                                                  |
 |                                                                           |
 |            S =  SUM ( R[j]*A[i-j] ,j=1,i-1 ) +  R[i]                      |
 |                                                                           |
 |            K = -S / Alpha                                                 |
 |                                                                           |
 |            An[j] = A[j] + K*A[i-j]   for j=1 to i-1                       |
 |                                      where   An[i] = new A[i]             |
 |            An[i]=K                                                        |
 |                                                                           |
 |            Alpha=Alpha * (1-K**2)                                         |
 |                                                                           |
 |       END                                                                 |
 |                                                                           |
 | Remarks on the dynamics of the calculations.                              |
 |                                                                           |
 |       The numbers used are in double precision in the following format :  |
 |       A = AH <<16 + AL<<1.  AH and AL are 16 bit signed integers.         |
 |       Since the LSB's also contain a sign bit, this format does not       |
 |       correspond to standard 32 bit integers.  We use this format since   |
 |       it allows fast execution of multiplications and divisions.          |
 |                                                                           |
 |       "DPF" will refer to this special format in the following text.      |
 |       See oper_32b.c                                                      |
 |                                                                           |
 |       The R[i] were normalized in routine AUTO (hence, R[i] < 1.0).       |
 |       The K[i] and Alpha are theoretically < 1.0.                         |
 |       The A[i], for a sampling frequency of 8 kHz, are in practice        |
 |       always inferior to 16.0.                                            |
 |                                                                           |
 |       These characteristics allow straigthforward fixed-point             |
 |       implementation.  We choose to represent the parameters as           |
 |       follows :                                                           |
 |                                                                           |
 |               R[i]    Q31   +- .99..                                      |
 |               K[i]    Q31   +- .99..                                      |
 |               Alpha   Normalized -> mantissa in Q31 plus exponent         |
 |               A[i]    Q27   +- 15.999..                                   |
 |                                                                           |
 |       The additions are performed in 32 bit.  For the summation used      |
 |       to calculate the K[i], we multiply numbers in Q31 by numbers        |
 |       in Q27, with the result of the multiplications in Q27,              |
 |       resulting in a dynamic of +- 16.  This is sufficient to avoid       |
 |       overflow, since the final result of the summation is                |
 |       necessarily < 1.0 as both the K[i] and Alpha are                    |
 |       theoretically < 1.0.                                                |
 |___________________________________________________________________________|
*/


/* Last A(z) for case of unstable filter */

void
Levinson (CodState *coder,
	  Word16 Rh[],		/* (i)     : Rh[M+1] Vector of autocorrelations (msb) */
	  Word16 Rl[],		/* (i)     : Rl[M+1] Vector of autocorrelations (lsb) */
	  Word16 A[],		/* (o) Q12 : A[M]    LPC coefficients  (m = 10)       */
	  Word16 rc[]		/* (o) Q15 : rc[M]   Reflection coefficients.         */
  )
{
  Word16 i, j;
  Word16 hi, lo;
  Word16 Kh, Kl;		/* reflection coefficient; hi and lo           */
  Word16 alp_h, alp_l, alp_exp;	/* Prediction gain; hi lo and exponent         */
  Word16 Ah[M + 1], Al[M + 1];	/* LPC coef. in double prec.                   */
  Word16 Anh[M + 1], Anl[M + 1];	/* LPC coef.for next iteration in double prec. */
  Word32 t0, t1, t2;		/* temporary variable                          */


/* K = A[1] = -R[1] / R[0] */

  t1 = L_Comp (Rh[1], Rl[1]);	/* R[1] in Q31      */
  t2 = L_abs (t1);		/* abs R[1]         */
  t0 = Div_32 (t2, Rh[0], Rl[0]);	/* R[1]/R[0] in Q31 */
  if (t1 > 0)
    t0 = L_negate (t0);		/* -R[1]/R[0]       */
  L_Extract (t0, &Kh, &Kl);	/* K in DPF         */
  rc[0] = Kh;
  t0 = L_shr (t0, 4);		/* A[1] in Q27      */
  L_Extract (t0, &Ah[1], &Al[1]);	/* A[1] in DPF      */

/*  Alpha = R[0] * (1-K**2) */

  t0 = Mpy_32 (Kh, Kl, Kh, Kl);	/* K*K      in Q31 */
  t0 = L_abs (t0);		/* Some case <0 !! */
  t0 = L_sub ((Word32) 0x7fffffffL, t0);	/* 1 - K*K  in Q31 */
  L_Extract (t0, &hi, &lo);	/* DPF format      */
  t0 = Mpy_32 (Rh[0], Rl[0], hi, lo);	/* Alpha in Q31    */

/* Normalize Alpha */

  alp_exp = norm_l (t0);
  t0 = L_shl (t0, alp_exp);
  L_Extract (t0, &alp_h, &alp_l);	/* DPF format    */

/*--------------------------------------*
 * ITERATIONS  I=2 to M                 *
 *--------------------------------------*/

  for (i = 2; i <= M; i++) {

    /* t0 = SUM ( R[j]*A[i-j] ,j=1,i-1 ) +  R[i] */

    t0 = 0;
    for (j = 1; j < i; j++)
      t0 = L_add (t0, Mpy_32 (Rh[j], Rl[j], Ah[i - j], Al[i - j]));

    t0 = L_shl (t0, 4);		/* result in Q27 -> convert to Q31 */
    /* No overflow possible            */
    t1 = L_Comp (Rh[i], Rl[i]);
    t0 = L_add (t0, t1);	/* add R[i] in Q31                 */

    /* K = -t0 / Alpha */

    t1 = L_abs (t0);
    t2 = Div_32 (t1, alp_h, alp_l);	/* abs(t0)/Alpha                   */
    if (t0 > 0)
      t2 = L_negate (t2);	/* K =-t0/Alpha                    */
    t2 = L_shl (t2, alp_exp);	/* denormalize; compare to Alpha   */
    L_Extract (t2, &Kh, &Kl);	/* K in DPF                        */
    rc[i - 1] = Kh;

    /* Test for unstable filter. If unstable keep old A(z) */

    if (sub (abs_s (Kh), 32750) > 0) {
      for (j = 0; j <= M; j++) {
	A[j] = coder->old_A[j];
      }
      rc[0] = coder->old_rc[0];	/* only two rc coefficients are needed */
      rc[1] = coder->old_rc[1];
      return;
    }

    /*------------------------------------------*
     *  Compute new LPC coeff. -> An[i]         *
     *  An[j]= A[j] + K*A[i-j]     , j=1 to i-1 *
     *  An[i]= K                                *
     *------------------------------------------*/


    for (j = 1; j < i; j++) {
      t0 = Mpy_32 (Kh, Kl, Ah[i - j], Al[i - j]);
      t0 = L_add (t0, L_Comp (Ah[j], Al[j]));
      L_Extract (t0, &Anh[j], &Anl[j]);
    }
    t2 = L_shr (t2, 4);		/* t2 = K in Q31 ->convert to Q27  */
    L_Extract (t2, &Anh[i], &Anl[i]);	/* An[i] in Q27                    */

    /*  Alpha = Alpha * (1-K**2) */

    t0 = Mpy_32 (Kh, Kl, Kh, Kl);	/* K*K      in Q31 */
    t0 = L_abs (t0);		/* Some case <0 !! */
    t0 = L_sub ((Word32) 0x7fffffffL, t0);	/* 1 - K*K  in Q31 */
    L_Extract (t0, &hi, &lo);	/* DPF format      */
    t0 = Mpy_32 (alp_h, alp_l, hi, lo);	/* Alpha in Q31    */

    /* Normalize Alpha */

    j = norm_l (t0);
    t0 = L_shl (t0, j);
    L_Extract (t0, &alp_h, &alp_l);	/* DPF format    */
    alp_exp = add (alp_exp, j);	/* Add normalization to alp_exp */

    /* A[j] = An[j] */

    for (j = 1; j <= i; j++) {
      Ah[j] = Anh[j];
      Al[j] = Anl[j];
    }
  }

  /* Truncate A[i] in Q27 to Q12 with rounding */

  A[0] = 4096;
  for (i = 1; i <= M; i++) {
    t0 = L_Comp (Ah[i], Al[i]);
    coder->old_A[i] = A[i] = wround (L_shl (t0, 1));
  }
  coder->old_rc[0] = rc[0];
  coder->old_rc[1] = rc[1];

  return;
}



/*-------------------------------------------------------------*
 *  procedure Az_lsp:                                          *
 *            ~~~~~~                                           *
 *   Compute the LSPs from  the LPC coefficients  (order=10)   *
 *-------------------------------------------------------------*/

Word16 Chebps_11 (Word16 x, Word16 f[], Word16 n);
Word16 Chebps_10 (Word16 x, Word16 f[], Word16 n);

void
Az_lsp (Word16 a[],		/* (i) Q12 : predictor coefficients              */
	Word16 lsp[],		/* (o) Q15 : line spectral pairs                 */
	Word16 old_lsp[]	/* (i)     : old lsp[] (in case not found 10 roots) */
  )
{
  Flag Overflow;
  Word16 i, j, nf, ip;
  Word16 xlow, ylow, xhigh, yhigh, xmid, ymid, xint;
  Word16 x, y, sign, exp;
  Word16 *coef;
  Word16 f1[M / 2 + 1], f2[M / 2 + 1];
  Word32 t0, L_temp;
  Flag ovf_coef;
  Word16 (*pChebps) (Word16 x, Word16 f[], Word16 n);

/*-------------------------------------------------------------*
 *  find the sum and diff. pol. F1(z) and F2(z)                *
 *    F1(z) <--- F1(z)/(1+z**-1) & F2(z) <--- F2(z)/(1-z**-1)  *
 *                                                             *
 * f1[0] = 1.0;                                                *
 * f2[0] = 1.0;                                                *
 *                                                             *
 * for (i = 0; i< NC; i++)                                     *
 * {                                                           *
 *   f1[i+1] = a[i+1] + a[M-i] - f1[i] ;                       *
 *   f2[i+1] = a[i+1] - a[M-i] + f2[i] ;                       *
 * }                                                           *
 *-------------------------------------------------------------*/

  ovf_coef = 0;
  pChebps = Chebps_11;

  f1[0] = 2048;			/* f1[0] = 1.0 is in Q11 */
  f2[0] = 2048;			/* f2[0] = 1.0 is in Q11 */

  for (i = 0; i < NC; i++) {
    Overflow = 0;
    t0 = L_mult_o (a[i + 1], 16384, &Overflow);	/* x = (a[i+1] + a[M-i]) >> 1        */
    t0 = L_mac_o (t0, a[M - i], 16384, &Overflow);	/*    -> From Q12 to Q11             */
    x = extract_h (t0);
    if (Overflow) {
      ovf_coef = 1;
    }

    Overflow = 0;
    f1[i + 1] = sub_o (x, f1[i], &Overflow);	/* f1[i+1] = a[i+1] + a[M-i] - f1[i] */
    if (Overflow) {
      ovf_coef = 1;
    }

    Overflow = 0;
    t0 = L_mult_o (a[i + 1], 16384, &Overflow);	/* x = (a[i+1] - a[M-i]) >> 1        */
    t0 = L_msu_o (t0, a[M - i], 16384, &Overflow); /*    -> From Q12 to Q11             */
    x = extract_h (t0);
    if (Overflow) {
      ovf_coef = 1;
    }

    Overflow = 0;
    f2[i + 1] = add_o (x, f2[i], &Overflow);	/* f2[i+1] = a[i+1] - a[M-i] + f2[i] */
    if (Overflow) {
      ovf_coef = 1;
    }
  }

  if (ovf_coef) {
    /*printf("===== OVF ovf_coef =====\n"); */

    pChebps = Chebps_10;

    f1[0] = 1024;		/* f1[0] = 1.0 is in Q10 */
    f2[0] = 1024;		/* f2[0] = 1.0 is in Q10 */

    for (i = 0; i < NC; i++) {
      t0 = L_mult (a[i + 1], 8192);	/* x = (a[i+1] + a[M-i]) >> 1        */
      t0 = L_mac (t0, a[M - i], 8192);	/*    -> From Q11 to Q10             */
      x = extract_h (t0);
      f1[i + 1] = sub (x, f1[i]);	/* f1[i+1] = a[i+1] + a[M-i] - f1[i] */

      t0 = L_mult (a[i + 1], 8192);	/* x = (a[i+1] - a[M-i]) >> 1        */
      t0 = L_msu (t0, a[M - i], 8192);	/*    -> From Q11 to Q10             */
      x = extract_h (t0);
      f2[i + 1] = add (x, f2[i]);	/* f2[i+1] = a[i+1] - a[M-i] + f2[i] */
    }
  }

/*-------------------------------------------------------------*
 * find the LSPs using the Chebichev pol. evaluation           *
 *-------------------------------------------------------------*/

  nf = 0;			/* number of found frequencies */
  ip = 0;			/* indicator for f1 or f2      */

  coef = f1;

  xlow = grid[0];
  ylow = (*pChebps) (xlow, coef, NC);

  j = 0;
  while ((nf < M) && (j < GRID_POINTS)) {
    j = add (j, 1);
    xhigh = xlow;
    yhigh = ylow;
    xlow = grid[j];
    ylow = (*pChebps) (xlow, coef, NC);

    L_temp = L_mult (ylow, yhigh);
    if (L_temp <= (Word32) 0) {

      /* divide 2 times the interval */

      for (i = 0; i < 2; i++) {
	xmid = add (shr (xlow, 1), shr (xhigh, 1));	/* xmid = (xlow + xhigh)/2 */

	ymid = (*pChebps) (xmid, coef, NC);

	L_temp = L_mult (ylow, ymid);
	if (L_temp <= (Word32) 0) {
	  yhigh = ymid;
	  xhigh = xmid;
	}
	else {
	  ylow = ymid;
	  xlow = xmid;
	}
      }

    /*-------------------------------------------------------------*
     * Linear interpolation                                        *
     *    xint = xlow - ylow*(xhigh-xlow)/(yhigh-ylow);            *
     *-------------------------------------------------------------*/

      x = sub (xhigh, xlow);
      y = sub (yhigh, ylow);

      if (y == 0) {
	xint = xlow;
      }
      else {
	sign = y;
	y = abs_s (y);
	exp = norm_s (y);
	y = shl (y, exp);
	y = div_s ((Word16) 16383, y);
	t0 = L_mult (x, y);
	t0 = L_shr (t0, sub (20, exp));
	y = extract_l (t0);	/* y= (xhigh-xlow)/(yhigh-ylow) in Q11 */

	if (sign < 0)
	  y = negate (y);

	t0 = L_mult (ylow, y);	/* result in Q26 */
	t0 = L_shr (t0, 11);	/* result in Q15 */
	xint = sub (xlow, extract_l (t0));	/* xint = xlow - ylow*y */
      }

      lsp[nf] = xint;
      xlow = xint;
      nf = add (nf, 1);

      if (ip == 0) {
	ip = 1;
	coef = f2;
      }
      else {
	ip = 0;
	coef = f1;
      }
      ylow = (*pChebps) (xlow, coef, NC);

    }
  }

  /* Check if M roots found */

  if (sub (nf, M) < 0) {
    for (i = 0; i < M; i++) {
      lsp[i] = old_lsp[i];
    }

    /* printf("\n !!Not 10 roots found in Az_lsp()!!!\n"); */
  }

  return;
}

/*--------------------------------------------------------------*
 * function  Chebps_11, Chebps_10:                              *
 *           ~~~~~~~~~~~~~~~~~~~~                               *
 *    Evaluates the Chebichev polynomial series                 *
 *--------------------------------------------------------------*
 *                                                              *
 *  The polynomial order is                                     *
 *     n = M/2   (M is the prediction order)                    *
 *  The polynomial is given by                                  *
 *    C(x) = T_n(x) + f(1)T_n-1(x) + ... +f(n-1)T_1(x) + f(n)/2 *
 * Arguments:                                                   *
 *  x:     input value of evaluation; x = cos(frequency) in Q15 *
 *  f[]:   coefficients of the pol.                             *
 *                         in Q11(Chebps_11), in Q10(Chebps_10) *
 *  n:     order of the pol.                                    *
 *                                                              *
 * The value of C(x) is returned. (Saturated to +-1.99 in Q14)  *
 *                                                              *
 *--------------------------------------------------------------*/
Word16
Chebps_11 (Word16 x, Word16 f[], Word16 n)
{
  Word16 i, cheb;
  Word16 b0_h, b0_l, b1_h, b1_l, b2_h, b2_l;
  Word32 t0;

  /* Note: All computation are done in Q24. */

  b2_h = 256;			/* b2 = 1.0 in Q24 DPF */
  b2_l = 0;

  t0 = L_mult (x, 512);		/* 2*x in Q24          */
  t0 = L_mac (t0, f[1], 4096);	/* + f[1] in Q24       */
  L_Extract (t0, &b1_h, &b1_l);	/* b1 = 2*x + f[1]     */

  for (i = 2; i < n; i++) {
    t0 = Mpy_32_16 (b1_h, b1_l, x);	/* t0 = 2.0*x*b1              */
    t0 = L_shl (t0, 1);
    t0 = L_mac (t0, b2_h, (Word16) - 32768L);	/* t0 = 2.0*x*b1 - b2         */
    t0 = L_msu (t0, b2_l, 1);
    t0 = L_mac (t0, f[i], 4096);	/* t0 = 2.0*x*b1 - b2 + f[i]; */

    L_Extract (t0, &b0_h, &b0_l);	/* b0 = 2.0*x*b1 - b2 + f[i]; */

    b2_l = b1_l;		/* b2 = b1; */
    b2_h = b1_h;
    b1_l = b0_l;		/* b1 = b0; */
    b1_h = b0_h;
  }

  t0 = Mpy_32_16 (b1_h, b1_l, x);	/* t0 = x*b1;              */
  t0 = L_mac (t0, b2_h, (Word16) - 32768L);	/* t0 = x*b1 - b2          */
  t0 = L_msu (t0, b2_l, 1);
  t0 = L_mac (t0, f[i], 2048);	/* t0 = x*b1 - b2 + f[i]/2 */

  t0 = L_shl (t0, 6);		/* Q24 to Q30 with saturation */
  cheb = extract_h (t0);	/* Result in Q14              */


  return (cheb);
}


Word16
Chebps_10 (Word16 x, Word16 f[], Word16 n)
{
  Word16 i, cheb;
  Word16 b0_h, b0_l, b1_h, b1_l, b2_h, b2_l;
  Word32 t0;

  /* Note: All computation are done in Q23. */

  b2_h = 128;			/* b2 = 1.0 in Q23 DPF */
  b2_l = 0;

  t0 = L_mult (x, 256);		/* 2*x in Q23          */
  t0 = L_mac (t0, f[1], 4096);	/* + f[1] in Q23       */
  L_Extract (t0, &b1_h, &b1_l);	/* b1 = 2*x + f[1]     */

  for (i = 2; i < n; i++) {
    t0 = Mpy_32_16 (b1_h, b1_l, x);	/* t0 = 2.0*x*b1              */
    t0 = L_shl (t0, 1);
    t0 = L_mac (t0, b2_h, (Word16) - 32768L);	/* t0 = 2.0*x*b1 - b2         */
    t0 = L_msu (t0, b2_l, 1);
    t0 = L_mac (t0, f[i], 4096);	/* t0 = 2.0*x*b1 - b2 + f[i]; */

    L_Extract (t0, &b0_h, &b0_l);	/* b0 = 2.0*x*b1 - b2 + f[i]; */

    b2_l = b1_l;		/* b2 = b1; */
    b2_h = b1_h;
    b1_l = b0_l;		/* b1 = b0; */
    b1_h = b0_h;
  }

  t0 = Mpy_32_16 (b1_h, b1_l, x);	/* t0 = x*b1;              */
  t0 = L_mac (t0, b2_h, (Word16) - 32768L);	/* t0 = x*b1 - b2          */
  t0 = L_msu (t0, b2_l, 1);
  t0 = L_mac (t0, f[i], 2048);	/* t0 = x*b1 - b2 + f[i]/2 */

  t0 = L_shl (t0, 7);		/* Q23 to Q30 with saturation */
  cheb = extract_h (t0);	/* Result in Q14              */


  return (cheb);
}
