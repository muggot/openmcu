/*
   ITU-T G.729A Speech Coder    ANSI-C Source Code
   Version 1.1    Last modified: September 1996

   Copyright (c) 1996,
   AT&T, France Telecom, NTT, Universite de Sherbrooke
   All rights reserved.
*/

/* Functions Corr_xy2() and Cor_h_x()   */

#include "typedef.h"
#include "basic_op.h"
#include "ld8a.h"

/*---------------------------------------------------------------------------*
 * Function corr_xy2()                                                       *
 * ~~~~~~~~~~~~~~~~~~~                                                       *
 * Find the correlations between the target xn[], the filtered adaptive      *
 * codebook excitation y1[], and the filtered 1st codebook innovation y2[].  *
 *   g_coeff[2]:exp_g_coeff[2] = <y2,y2>                                     *
 *   g_coeff[3]:exp_g_coeff[3] = -2<xn,y2>                                   *
 *   g_coeff[4]:exp_g_coeff[4] = 2<y1,y2>                                    *
 *---------------------------------------------------------------------------*/

void
Corr_xy2 (Word16 xn[],		/* (i) Q0  :Target vector.                  */
	  Word16 y1[],		/* (i) Q0  :Adaptive codebook.              */
	  Word16 y2[],		/* (i) Q12 :Filtered innovative vector.     */
	  Word16 g_coeff[],	/* (o) Q[exp]:Correlations between xn,y1,y2 */
	  Word16 exp_g_coeff[]	/* (o)       :Q-format of g_coeff[]         */
  )
{
  Word16 i, exp;
  Word16 exp_y2y2, exp_xny2, exp_y1y2;
  Word16 y2y2, xny2, y1y2;
  Word32 L_acc;
  Word16 scaled_y2[L_SUBFR];	/* Q9 */

      /*------------------------------------------------------------------*
       * Scale down y2[] from Q12 to Q9 to avoid overflow                 *
       *------------------------------------------------------------------*/
  for (i = 0; i < L_SUBFR; i++) {
    scaled_y2[i] = shr (y2[i], 3);
  }

  /* Compute scalar product <y2[],y2[]> */
  L_acc = 1;			/* Avoid case of all zeros */
  for (i = 0; i < L_SUBFR; i++)
    L_acc = L_mac (L_acc, scaled_y2[i], scaled_y2[i]);	/* L_acc:Q19 */

  exp = norm_l (L_acc);
  y2y2 = wround (L_shl (L_acc, exp));
  exp_y2y2 = add (exp, 19 - 16);	/* Q[19+exp-16] */

  g_coeff[2] = y2y2;
  exp_g_coeff[2] = exp_y2y2;

  /* Compute scalar product <xn[],y2[]> */
  L_acc = 1;			/* Avoid case of all zeros */
  for (i = 0; i < L_SUBFR; i++)
    L_acc = L_mac (L_acc, xn[i], scaled_y2[i]);	/* L_acc:Q10 */

  exp = norm_l (L_acc);
  xny2 = wround (L_shl (L_acc, exp));
  exp_xny2 = add (exp, 10 - 16);	/* Q[10+exp-16] */

  g_coeff[3] = negate (xny2);
  exp_g_coeff[3] = sub (exp_xny2, 1);	/* -2<xn,y2> */

  /* Compute scalar product <y1[],y2[]> */
  L_acc = 1;			/* Avoid case of all zeros */
  for (i = 0; i < L_SUBFR; i++)
    L_acc = L_mac (L_acc, y1[i], scaled_y2[i]);	/* L_acc:Q10 */

  exp = norm_l (L_acc);
  y1y2 = wround (L_shl (L_acc, exp));
  exp_y1y2 = add (exp, 10 - 16);	/* Q[10+exp-16] */

  g_coeff[4] = y1y2;
  exp_g_coeff[4] = sub (exp_y1y2, 1);;	/* 2<y1,y2> */

  return;
}


/*--------------------------------------------------------------------------*
 *  Function  Cor_h_X()                                                     *
 *  ~~~~~~~~~~~~~~~~~~~                                                     *
 * Compute correlations of input response h[] with the target vector X[].   *
 *--------------------------------------------------------------------------*/

void
Cor_h_X (Word16 h[],		/* (i) Q12 :Impulse response of filters      */
	 Word16 X[],		/* (i)     :Target vector                    */
	 Word16 D[]
	 /* (o)     :Correlations between h[] and D[] */
	 /*          Normalized to 13 bits            */
  )
{
  Word16 i, j;
  Word32 s, max, L_temp;
  Word32 y32[L_SUBFR];

  /* first keep the result on 32 bits and find absolute maximum */

  max = 0;

  for (i = 0; i < L_SUBFR; i++) {
    s = 0;
    for (j = i; j < L_SUBFR; j++)
      s = L_mac (s, X[j], h[j - i]);

    y32[i] = s;

    s = L_abs (s);
    L_temp = L_sub (s, max);
    if (L_temp > 0L) {
      max = s;
    }
  }

  /* Find the number of right shifts to do on y32[]  */
  /* so that maximum is on 13 bits                   */

  j = norm_l (max);
  if (sub (j, 16) > 0) {
    j = 16;
  }

  j = sub (18, j);

  for (i = 0; i < L_SUBFR; i++) {
    D[i] = extract_l (L_shr (y32[i], j));
  }

  return;

}
