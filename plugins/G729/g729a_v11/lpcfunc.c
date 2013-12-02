/*
   ITU-T G.729A Speech Coder    ANSI-C Source Code
   Version 1.1    Last modified: September 1996

   Copyright (c) 1996,
   AT&T, France Telecom, NTT, Universite de Sherbrooke
   All rights reserved.
*/

/*-------------------------------------------------------------*
 *  Procedure Lsp_Az:                                          *
 *            ~~~~~~                                           *
 *   Compute the LPC coefficients from lsp (order=10)          *
 *-------------------------------------------------------------*/

#include "typedef.h"
#include "basic_op.h"
#include "oper_32b.h"

#include "ld8a.h"
#include "tab_ld8a.h"

/* local function */

void Get_lsp_pol (Word16 * lsp, Word32 * f);

void
Lsp_Az (Word16 lsp[],		/* (i) Q15 : line spectral frequencies            */
	Word16 a[]		/* (o) Q12 : predictor coefficients (order = 10)  */
  )
{
  Word16 i, j;
  Word32 f1[6], f2[6];
  Word32 t0;

  Get_lsp_pol (&lsp[0], f1);
  Get_lsp_pol (&lsp[1], f2);

  for (i = 5; i > 0; i--) {
    f1[i] = L_add (f1[i], f1[i - 1]);	/* f1[i] += f1[i-1]; */
    f2[i] = L_sub (f2[i], f2[i - 1]);	/* f2[i] -= f2[i-1]; */
  }

  a[0] = 4096;
  for (i = 1, j = 10; i <= 5; i++, j--) {
    t0 = L_add (f1[i], f2[i]);	/* f1[i] + f2[i]             */
    a[i] = extract_l (L_shr_r (t0, 13));	/* from Q24 to Q12 and * 0.5 */

    t0 = L_sub (f1[i], f2[i]);	/* f1[i] - f2[i]             */
    a[j] = extract_l (L_shr_r (t0, 13));	/* from Q24 to Q12 and * 0.5 */

  }

  return;
}

/*-----------------------------------------------------------*
 * procedure Get_lsp_pol:                                    *
 *           ~~~~~~~~~~~                                     *
 *   Find the polynomial F1(z) or F2(z) from the LSPs        *
 *-----------------------------------------------------------*
 *                                                           *
 * Parameters:                                               *
 *  lsp[]   : line spectral freq. (cosine domain)    in Q15  *
 *  f[]     : the coefficients of F1 or F2           in Q24  *
 *-----------------------------------------------------------*/

void
Get_lsp_pol (Word16 * lsp, Word32 * f)
{
  Word16 i, j, hi, lo;
  Word32 t0;

  /* All computation in Q24 */

  *f = L_mult (4096, 2048);	/* f[0] = 1.0;             in Q24  */
  f++;
  *f = L_msu ((Word32) 0, *lsp, 512);	/* f[1] =  -2.0 * lsp[0];  in Q24  */

  f++;
  lsp += 2;			/* Advance lsp pointer             */

  for (i = 2; i <= 5; i++) {
    *f = f[-2];

    for (j = 1; j < i; j++, f--) {
      L_Extract (f[-1], &hi, &lo);
      t0 = Mpy_32_16 (hi, lo, *lsp);	/* t0 = f[-1] * lsp    */
      t0 = L_shl (t0, 1);
      *f = L_add (*f, f[-2]);	/* *f += f[-2]         */
      *f = L_sub (*f, t0);	/* *f -= t0            */
    }
    *f = L_msu (*f, *lsp, 512);	/* *f -= lsp<<9        */
    f += i;			/* Advance f pointer   */
    lsp += 2;			/* Advance lsp pointer */
  }

  return;
}

/*___________________________________________________________________________
 |                                                                           |
 |   Functions : Lsp_lsf and Lsf_lsp                                         |
 |                                                                           |
 |      Lsp_lsf   Transformation lsp to lsf                                  |
 |      Lsf_lsp   Transformation lsf to lsp                                  |
 |---------------------------------------------------------------------------|
 |  Algorithm:                                                               |
 |                                                                           |
 |   The transformation from lsp[i] to lsf[i] and lsf[i] to lsp[i] are       |
 |   approximated by a look-up table and interpolation.                      |
 |___________________________________________________________________________|
*/


void
Lsf_lsp (Word16 lsf[],		/* (i) Q15 : lsf[m] normalized (range: 0.0<=val<=0.5) */
	 Word16 lsp[],		/* (o) Q15 : lsp[m] (range: -1<=val<1)                */
	 Word16 m		/* (i)     : LPC order                                */
  )
{
  Word16 i, ind, offset;
  Word32 L_tmp;

  for (i = 0; i < m; i++) {
    ind = shr (lsf[i], 8);	/* ind    = b8-b15 of lsf[i] */
    offset = lsf[i] & (Word16) 0x00ff;	/* offset = b0-b7  of lsf[i] */

    /* lsp[i] = table[ind]+ ((table[ind+1]-table[ind])*offset) / 256 */

    L_tmp = L_mult (sub (table[ind + 1], table[ind]), offset);
    lsp[i] = add (table[ind], extract_l (L_shr (L_tmp, 9)));
  }
  return;
}


void
Lsp_lsf (Word16 lsp[],		/* (i) Q15 : lsp[m] (range: -1<=val<1)                */
	 Word16 lsf[],		/* (o) Q15 : lsf[m] normalized (range: 0.0<=val<=0.5) */
	 Word16 m		/* (i)     : LPC order                                */
  )
{
  Word16 i, ind, tmp;
  Word32 L_tmp;

  ind = 63;			/* begin at end of table -1 */

  for (i = m - (Word16) 1; i >= 0; i--) {
    /* find value in table that is just greater than lsp[i] */
    while (sub (table[ind], lsp[i]) < 0) {
      ind = sub (ind, 1);
    }

    /* acos(lsp[i])= ind*256 + ( ( lsp[i]-table[ind] ) * slope[ind] )/4096 */

    L_tmp = L_mult (sub (lsp[i], table[ind]), slope[ind]);
    tmp = wround (L_shl (L_tmp, 3));	/*(lsp[i]-table[ind])*slope[ind])>>12 */
    lsf[i] = add (tmp, shl (ind, 8));
  }
  return;
}

/*___________________________________________________________________________
 |                                                                           |
 |   Functions : Lsp_lsf and Lsf_lsp                                         |
 |                                                                           |
 |      Lsp_lsf   Transformation lsp to lsf                                  |
 |      Lsf_lsp   Transformation lsf to lsp                                  |
 |---------------------------------------------------------------------------|
 |  Algorithm:                                                               |
 |                                                                           |
 |   The transformation from lsp[i] to lsf[i] and lsf[i] to lsp[i] are       |
 |   approximated by a look-up table and interpolation.                      |
 |___________________________________________________________________________|
*/

void
Lsf_lsp2 (Word16 lsf[],		/* (i) Q13 : lsf[m] (range: 0.0<=val<PI) */
	  Word16 lsp[],		/* (o) Q15 : lsp[m] (range: -1<=val<1)   */
	  Word16 m		/* (i)     : LPC order                   */
  )
{
  Word16 i, ind;
  Word16 offset;		/* in Q8 */
  Word16 freq;			/* normalized frequency in Q15 */
  Word32 L_tmp;

  for (i = 0; i < m; i++) {
/*    freq = abs_s(freq);*/
    freq = mult (lsf[i], 20861);	/* 20861: 1.0/(2.0*PI) in Q17 */
    ind = shr (freq, 8);	/* ind    = b8-b15 of freq */
    offset = freq & (Word16) 0x00ff;	/* offset = b0-b7  of freq */

    if (sub (ind, 63) > 0) {
      ind = 63;			/* 0 <= ind <= 63 */
    }

    /* lsp[i] = table2[ind]+ (slope_cos[ind]*offset >> 12) */

    L_tmp = L_mult (slope_cos[ind], offset);	/* L_tmp in Q28 */
    lsp[i] = add (table2[ind], extract_l (L_shr (L_tmp, 13)));

  }
  return;
}



void
Lsp_lsf2 (Word16 lsp[],		/* (i) Q15 : lsp[m] (range: -1<=val<1)   */
	  Word16 lsf[],		/* (o) Q13 : lsf[m] (range: 0.0<=val<PI) */
	  Word16 m		/* (i)     : LPC order                   */
  )
{
  Word16 i, ind;
  Word16 offset;		/* in Q15 */
  Word16 freq;			/* normalized frequency in Q16 */
  Word32 L_tmp;

  ind = 63;			/* begin at end of table2 -1 */

  for (i = m - (Word16) 1; i >= 0; i--) {
    /* find value in table2 that is just greater than lsp[i] */
    while (sub (table2[ind], lsp[i]) < 0) {
      ind = sub (ind, 1);
      if (ind <= 0)
	break;
    }

    offset = sub (lsp[i], table2[ind]);

    /* acos(lsp[i])= ind*512 + (slope_acos[ind]*offset >> 11) */

    L_tmp = L_mult (slope_acos[ind], offset);	/* L_tmp in Q28 */
    freq = add (shl (ind, 9), extract_l (L_shr (L_tmp, 12)));
    lsf[i] = mult (freq, 25736);	/* 25736: 2.0*PI in Q12 */

  }
  return;
}

/*-------------------------------------------------------------*
 *  procedure Weight_Az                                        *
 *            ~~~~~~~~~                                        *
 * Weighting of LPC coefficients.                              *
 *   ap[i]  =  a[i] * (gamma ** i)                             *
 *                                                             *
 *-------------------------------------------------------------*/


void
Weight_Az (Word16 a[],		/* (i) Q12 : a[m+1]  LPC coefficients             */
	   Word16 gamma,	/* (i) Q15 : Spectral expansion factor.           */
	   Word16 m,		/* (i)     : LPC order.                           */
	   Word16 ap[]		/* (o) Q12 : Spectral expanded LPC coefficients   */
  )
{
  Word16 i, fac;

  ap[0] = a[0];
  fac = gamma;
  for (i = 1; i < m; i++) {
    ap[i] = wround (L_mult (a[i], fac));
    fac = wround (L_mult (fac, gamma));
  }
  ap[m] = wround (L_mult (a[m], fac));

  return;
}

/*----------------------------------------------------------------------*
 * Function Int_qlpc()                                                  *
 * ~~~~~~~~~~~~~~~~~~~                                                  *
 * Interpolation of the LPC parameters.                                 *
 *----------------------------------------------------------------------*/

/* Interpolation of the quantized LSP's */

void
Int_qlpc (Word16 lsp_old[],	/* input : LSP vector of past frame              */
	  Word16 lsp_new[],	/* input : LSP vector of present frame           */
	  Word16 Az[]		/* output: interpolated Az() for the 2 subframes */
  )
{
  Word16 i;
  Word16 lsp[M];

  /*  lsp[i] = lsp_new[i] * 0.5 + lsp_old[i] * 0.5 */

  for (i = 0; i < M; i++) {
    lsp[i] = add (shr (lsp_new[i], 1), shr (lsp_old[i], 1));
  }

  Lsp_Az (lsp, Az);		/* Subframe 1 */

  Lsp_Az (lsp_new, &Az[MP1]);	/* Subframe 2 */

  return;
}
