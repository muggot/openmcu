/*
   ITU-T G.729A Speech Coder    ANSI-C Source Code
   Version 1.1    Last modified: September 1996

   Copyright (c) 1996,
   AT&T, France Telecom, NTT, Universite de Sherbrooke
   All rights reserved.
*/

/*---------------------------------------------------------------------------*
 * Pitch related functions                                                   *
 * ~~~~~~~~~~~~~~~~~~~~~~~                                                   *
 *---------------------------------------------------------------------------*/

#include "typedef.h"
#include "basic_op.h"
#include "oper_32b.h"
#include "ld8a.h"
#include "tab_ld8a.h"
#include "util.h"

/*---------------------------------------------------------------------------*
 * Function  Pitch_ol_fast                                                   *
 * ~~~~~~~~~~~~~~~~~~~~~~~                                                   *
 * Compute the open loop pitch lag. (fast version)                           *
 *                                                                           *
 *---------------------------------------------------------------------------*/


Word16
Pitch_ol_fast (			/* output: open loop pitch lag                        */
		Word16 signal[],	/* input : signal used to compute the open loop pitch */
		/*     signal[-pit_max] to signal[-1] should be known */
		Word16 pit_max,	/* input : maximum pitch lag                          */
		Word16 L_frame	/* input : length of frame to compute pitch           */
  )
{
  Flag Overflow;
  Word16 i, j;
  Word16 max1, max2, max3;
  Word16 max_h, max_l, ener_h, ener_l;
  Word16 T1, T2, T3;
  Word16 *p, *p1;
  Word32 max, sum, L_temp;

  /* Scaled signal */

  Word16 scaled_signal[L_FRAME + PIT_MAX];
  Word16 *scal_sig;

  scal_sig = &scaled_signal[pit_max];

  /*--------------------------------------------------------*
   *  Verification for risk of overflow.                    *
   *--------------------------------------------------------*/

  Overflow = 0;
  sum = 0;

  for (i = -pit_max; i < L_frame; i += 2)
    sum = L_mac_o (sum, signal[i], signal[i], &Overflow);

  /*--------------------------------------------------------*
   * Scaling of input signal.                               *
   *                                                        *
   *   if Overflow        -> scal_sig[i] = signal[i]>>3     *
   *   else if sum < 1^20 -> scal_sig[i] = signal[i]<<3     *
   *   else               -> scal_sig[i] = signal[i]        *
   *--------------------------------------------------------*/

  if (Overflow == 1) {
    for (i = -pit_max; i < L_frame; i++) {
      scal_sig[i] = shr (signal[i], 3);
    }
  }
  else {
    L_temp = L_sub (sum, (Word32) 1048576L);
    if (L_temp < (Word32) 0) {	/* if (sum < 2^20) */
      for (i = -pit_max; i < L_frame; i++) {
	scal_sig[i] = shl (signal[i], 3);
      }
    }
    else {
      for (i = -pit_max; i < L_frame; i++) {
	scal_sig[i] = signal[i];
      }
    }
  }

   /*--------------------------------------------------------------------*
    *  The pitch lag search is divided in three sections.                *
    *  Each section cannot have a pitch multiple.                        *
    *  We find a maximum for each section.                               *
    *  We compare the maxima of each section by favoring small lag.      *
    *                                                                    *
    *  First section:  lag delay = 20 to 39                              *
    *  Second section: lag delay = 40 to 79                              *
    *  Third section:  lag delay = 80 to 143                             *
    *--------------------------------------------------------------------*/

  /* First section */

  max = MIN_32;
  T1 = 20;			/* Only to remove warning from some compilers */
  for (i = 20; i < 40; i++) {
    p = scal_sig;
    p1 = &scal_sig[-i];
    sum = 0;
    for (j = 0; j < L_frame; j += 2, p += 2, p1 += 2)
      sum = L_mac (sum, *p, *p1);
    L_temp = L_sub (sum, max);
    if (L_temp > 0) {
      max = sum;
      T1 = i;
    }
  }

  /* compute energy of maximum */

  sum = 1;			/* to avoid division by zero */
  p = &scal_sig[-T1];
  for (i = 0; i < L_frame; i += 2, p += 2)
    sum = L_mac (sum, *p, *p);

  /* max1 = max/sqrt(energy)                  */
  /* This result will always be on 16 bits !! */

  sum = Inv_sqrt (sum);		/* 1/sqrt(energy),    result in Q30 */
  L_Extract (max, &max_h, &max_l);
  L_Extract (sum, &ener_h, &ener_l);
  sum = Mpy_32 (max_h, max_l, ener_h, ener_l);
  max1 = extract_l (sum);

  /* Second section */

  max = MIN_32;
  T2 = 40;			/* Only to remove warning from some compilers */
  for (i = 40; i < 80; i++) {
    p = scal_sig;
    p1 = &scal_sig[-i];
    sum = 0;
    for (j = 0; j < L_frame; j += 2, p += 2, p1 += 2)
      sum = L_mac (sum, *p, *p1);
    L_temp = L_sub (sum, max);
    if (L_temp > 0) {
      max = sum;
      T2 = i;
    }
  }

  /* compute energy of maximum */

  sum = 1;			/* to avoid division by zero */
  p = &scal_sig[-T2];
  for (i = 0; i < L_frame; i += 2, p += 2)
    sum = L_mac (sum, *p, *p);

  /* max2 = max/sqrt(energy)                  */
  /* This result will always be on 16 bits !! */

  sum = Inv_sqrt (sum);		/* 1/sqrt(energy),    result in Q30 */
  L_Extract (max, &max_h, &max_l);
  L_Extract (sum, &ener_h, &ener_l);
  sum = Mpy_32 (max_h, max_l, ener_h, ener_l);
  max2 = extract_l (sum);

  /* Third section */

  max = MIN_32;
  T3 = 80;			/* Only to remove warning from some compilers */
  for (i = 80; i < 143; i += 2) {
    p = scal_sig;
    p1 = &scal_sig[-i];
    sum = 0;
    for (j = 0; j < L_frame; j += 2, p += 2, p1 += 2)
      sum = L_mac (sum, *p, *p1);
    L_temp = L_sub (sum, max);
    if (L_temp > 0) {
      max = sum;
      T3 = i;
    }
  }

  /* Test around max3 */

  i = T3;
  p = scal_sig;
  p1 = &scal_sig[-(i + 1)];
  sum = 0;
  for (j = 0; j < L_frame; j += 2, p += 2, p1 += 2)
    sum = L_mac (sum, *p, *p1);
  L_temp = L_sub (sum, max);
  if (L_temp > 0) {
    max = sum;
    T3 = i + (Word16) 1;
  }

  p = scal_sig;
  p1 = &scal_sig[-(i - 1)];
  sum = 0;
  for (j = 0; j < L_frame; j += 2, p += 2, p1 += 2)
    sum = L_mac (sum, *p, *p1);
  L_temp = L_sub (sum, max);
  if (L_temp > 0) {
    max = sum;
    T3 = i - (Word16) 1;
  }

  /* compute energy of maximum */

  sum = 1;			/* to avoid division by zero */
  p = &scal_sig[-T3];
  for (i = 0; i < L_frame; i += 2, p += 2)
    sum = L_mac (sum, *p, *p);

  /* max1 = max/sqrt(energy)                  */
  /* This result will always be on 16 bits !! */

  sum = Inv_sqrt (sum);		/* 1/sqrt(energy),    result in Q30 */
  L_Extract (max, &max_h, &max_l);
  L_Extract (sum, &ener_h, &ener_l);
  sum = Mpy_32 (max_h, max_l, ener_h, ener_l);
  max3 = extract_l (sum);

   /*-----------------------*
    * Test for multiple.    *
    *-----------------------*/

  /* if( abs(T2*2 - T3) < 5)  */
  /*    max2 += max3 * 0.25;  */

  i = sub (shl (T2, 1), T3);
  j = sub (abs_s (i), 5);
  if (j < 0)
    max2 = add (max2, shr (max3, 2));

  /* if( abs(T2*3 - T3) < 7)  */
  /*    max2 += max3 * 0.25;  */

  i = add (i, T2);
  j = sub (abs_s (i), 7);
  if (j < 0)
    max2 = add (max2, shr (max3, 2));

  /* if( abs(T1*2 - T2) < 5)  */
  /*    max1 += max2 * 0.20;  */

  i = sub (shl (T1, 1), T2);
  j = sub (abs_s (i), 5);
  if (j < 0)
    max1 = add (max1, mult (max2, 6554));

  /* if( abs(T1*3 - T2) < 7)  */
  /*    max1 += max2 * 0.20;  */

  i = add (i, T1);
  j = sub (abs_s (i), 7);
  if (j < 0)
    max1 = add (max1, mult (max2, 6554));

   /*--------------------------------------------------------------------*
    * Compare the 3 sections maxima.                                     *
    *--------------------------------------------------------------------*/

  if (sub (max1, max2) < 0) {
    max1 = max2;
    T1 = T2;
  }
  if (sub (max1, max3) < 0) {
    T1 = T3;
  }

  return T1;
}




/*--------------------------------------------------------------------------*
 *  Function  Dot_Product()                                                 *
 *  ~~~~~~~~~~~~~~~~~~~~~~                                                  *
 *--------------------------------------------------------------------------*/

Word32
Dot_Product (			/* (o)   :Result of scalar product. */
	      Word16 x[],	/* (i)   :First vector.             */
	      Word16 y[],	/* (i)   :Second vector.            */
	      Word16 lg		/* (i)   :Number of point.          */
  )
{
  Word16 i;
  Word32 sum;

  sum = 0;
  for (i = 0; i < lg; i++)
    sum = L_mac (sum, x[i], y[i]);

  return sum;
}

/*--------------------------------------------------------------------------*
 *  Function  Pitch_fr3_fast()                                              *
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~                                              *
 * Fast version of the pitch close loop.                                    *
 *--------------------------------------------------------------------------*/

Word16
Pitch_fr3_fast (		/* (o)     : pitch period.                          */
		 Word16 exc[],	/* (i)     : excitation buffer                      */
		 Word16 xn[],	/* (i)     : target vector                          */
		 Word16 h[],	/* (i) Q12 : impulse response of filters.           */
		 Word16 L_subfr,	/* (i)     : Length of subframe                     */
		 Word16 t0_min,	/* (i)     : minimum value in the searched range.   */
		 Word16 t0_max,	/* (i)     : maximum value in the searched range.   */
		 Word16 i_subfr,	/* (i)     : indicator for first subframe.          */
		 Word16 * pit_frac	/* (o)     : chosen fraction.                       */
  )
{
  Word16 t, t0;
  Word16 Dn[L_SUBFR];
  Word16 exc_tmp[L_SUBFR];
  Word32 max, corr, L_temp;

 /*-----------------------------------------------------------------*
  * Compute correlation of target vector with impulse response.     *
  *-----------------------------------------------------------------*/

  Cor_h_X (h, xn, Dn);

 /*-----------------------------------------------------------------*
  * Find maximum integer delay.                                     *
  *-----------------------------------------------------------------*/

  max = MIN_32;
  t0 = t0_min;			/* Only to remove warning from some compilers */

  for (t = t0_min; t <= t0_max; t++) {
    corr = Dot_Product (Dn, &exc[-t], L_subfr);
    L_temp = L_sub (corr, max);
    if (L_temp > 0) {
      max = corr;
      t0 = t;
    }
  }

 /*-----------------------------------------------------------------*
  * Test fractions.                                                 *
  *-----------------------------------------------------------------*/

  /* Fraction 0 */

  Pred_lt_3 (exc, t0, 0, L_subfr);
  max = Dot_Product (Dn, exc, L_subfr);
  *pit_frac = 0;

  /* If first subframe and lag > 84 do not search fractional pitch */

  if ((i_subfr == 0) && (sub (t0, 84) > 0))
    return t0;

  Copy (exc, exc_tmp, L_subfr);

  /* Fraction -1/3 */

  Pred_lt_3 (exc, t0, -1, L_subfr);
  corr = Dot_Product (Dn, exc, L_subfr);
  L_temp = L_sub (corr, max);
  if (L_temp > 0) {
    max = corr;
    *pit_frac = -1;
    Copy (exc, exc_tmp, L_subfr);
  }

  /* Fraction +1/3 */

  Pred_lt_3 (exc, t0, 1, L_subfr);
  corr = Dot_Product (Dn, exc, L_subfr);
  L_temp = L_sub (corr, max);
  if (L_temp > 0) {
    max = corr;
    *pit_frac = 1;
  }
  else
    Copy (exc_tmp, exc, L_subfr);

  return t0;
}


/*---------------------------------------------------------------------*
 * Function  G_pitch:                                                  *
 *           ~~~~~~~~                                                  *
 *---------------------------------------------------------------------*
 * Compute correlations <xn,y1> and <y1,y1> to use in gains quantizer. *
 * Also compute the gain of pitch. Result in Q14                       *
 *  if (gain < 0)  gain =0                                             *
 *  if (gain >1.2) gain =1.2                                           *
 *---------------------------------------------------------------------*/


Word16
G_pitch (			/* (o) Q14 : Gain of pitch lag saturated to 1.2       */
	  Word16 xn[],		/* (i)     : Pitch target.                            */
	  Word16 y1[],		/* (i)     : Filtered adaptive codebook.              */
	  Word16 g_coeff[],	/* (i)     : Correlations need for gain quantization. */
	  Word16 L_subfr	/* (i)     : Length of subframe.                      */
  )
{
  Flag Overflow;
  Word16 i;
  Word16 xy, yy, exp_xy, exp_yy, gain;
  Word32 s;

  Word16 scaled_y1[L_SUBFR];

  /* divide "y1[]" by 4 to avoid overflow */

  for (i = 0; i < L_subfr; i++)
    scaled_y1[i] = shr (y1[i], 2);

  /* Compute scalar product <y1[],y1[]> */

  Overflow = 0;
  s = 1;			/* Avoid case of all zeros */
  for (i = 0; i < L_subfr; i++)
    s = L_mac_o (s, y1[i], y1[i], &Overflow);

  if (Overflow == 0) {
    exp_yy = norm_l (s);
    yy = wround (L_shl (s, exp_yy));
  }
  else {
    s = 1;			/* Avoid case of all zeros */
    for (i = 0; i < L_subfr; i++)
      s = L_mac (s, scaled_y1[i], scaled_y1[i]);
    exp_yy = norm_l (s);
    yy = wround (L_shl (s, exp_yy));
    exp_yy = sub (exp_yy, 4);
  }

  /* Compute scalar product <xn[],y1[]> */

  Overflow = 0;
  s = 0;
  for (i = 0; i < L_subfr; i++)
    s = L_mac_o (s, xn[i], y1[i], &Overflow);

  if (Overflow == 0) {
    exp_xy = norm_l (s);
    xy = wround (L_shl (s, exp_xy));
  }
  else {
    s = 0;
    for (i = 0; i < L_subfr; i++)
      s = L_mac (s, xn[i], scaled_y1[i]);
    exp_xy = norm_l (s);
    xy = wround (L_shl (s, exp_xy));
    exp_xy = sub (exp_xy, 2);
  }

  g_coeff[0] = yy;
  g_coeff[1] = sub (15, exp_yy);
  g_coeff[2] = xy;
  g_coeff[3] = sub (15, exp_xy);

  /* If (xy <= 0) gain = 0 */


  if (xy <= 0) {
    g_coeff[3] = -15;		/* Force exp_xy to -15 = (15-30) */
    return ((Word16) 0);
  }

  /* compute gain = xy/yy */

  xy = shr (xy, 1);		/* Be sure xy < yy */
  gain = div_s (xy, yy);

  i = sub (exp_xy, exp_yy);
  gain = shr (gain, i);		/* saturation if > 1.99 in Q14 */

  /* if(gain >1.2) gain = 1.2  in Q14 */

  if (sub (gain, 19661) > 0) {
    gain = 19661;
  }


  return (gain);
}



/*----------------------------------------------------------------------*
 *    Function Enc_lag3                                                 *
 *             ~~~~~~~~                                                 *
 *   Encoding of fractional pitch lag with 1/3 resolution.              *
 *----------------------------------------------------------------------*
 * The pitch range for the first subframe is divided as follows:        *
 *   19 1/3  to   84 2/3   resolution 1/3                               *
 *   85      to   143      resolution 1                                 *
 *                                                                      *
 * The period in the first subframe is encoded with 8 bits.             *
 * For the range with fractions:                                        *
 *   index = (T-19)*3 + frac - 1;   where T=[19..85] and frac=[-1,0,1]  *
 * and for the integer only range                                       *
 *   index = (T - 85) + 197;        where T=[86..143]                   *
 *----------------------------------------------------------------------*
 * For the second subframe a resolution of 1/3 is always used, and the  *
 * search range is relative to the lag in the first subframe.           *
 * If t0 is the lag in the first subframe then                          *
 *  t_min=t0-5   and  t_max=t0+4   and  the range is given by           *
 *       t_min - 2/3   to  t_max + 2/3                                  *
 *                                                                      *
 * The period in the 2nd subframe is encoded with 5 bits:               *
 *   index = (T-(t_min-1))*3 + frac - 1;    where T[t_min-1 .. t_max+1] *
 *----------------------------------------------------------------------*/


Word16
Enc_lag3 (			/* output: Return index of encoding */
	   Word16 T0,		/* input : Pitch delay              */
	   Word16 T0_frac,	/* input : Fractional pitch delay   */
	   Word16 * T0_min,	/* in/out: Minimum search delay     */
	   Word16 * T0_max,	/* in/out: Maximum search delay     */
	   Word16 pit_min,	/* input : Minimum pitch delay      */
	   Word16 pit_max,	/* input : Maximum pitch delay      */
	   Word16 pit_flag	/* input : Flag for 1st subframe    */
  )
{
  Word16 index, i;

  if (pit_flag == 0) {		/* if 1st subframe */
    /* encode pitch delay (with fraction) */

    if (sub (T0, 85) <= 0) {
      /* index = t0*3 - 58 + t0_frac   */
      i = add (add (T0, T0), T0);
      index = add (sub (i, 58), T0_frac);
    }
    else {
      index = add (T0, 112);
    }

    /* find T0_min and T0_max for second subframe */

    *T0_min = sub (T0, 5);
    if (sub (*T0_min, pit_min) < 0) {
      *T0_min = pit_min;
    }

    *T0_max = add (*T0_min, 9);
    if (sub (*T0_max, pit_max) > 0) {
      *T0_max = pit_max;
      *T0_min = sub (*T0_max, 9);
    }
  }
  else {			/* if second subframe */


    /* i = t0 - t0_min;               */
    /* index = i*3 + 2 + t0_frac;     */
    i = sub (T0, *T0_min);
    i = add (add (i, i), i);
    index = add (add (i, 2), T0_frac);
  }


  return index;
}
