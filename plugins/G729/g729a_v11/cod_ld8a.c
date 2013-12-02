/*
   ITU-T G.729A Speech Coder    ANSI-C Source Code
   Version 1.1    Last modified: September 1996

   Copyright (c) 1996,
   AT&T, France Telecom, NTT, Universite de Sherbrooke
   All rights reserved.
*/

/*-----------------------------------------------------------------*
 *   Functions Coder_ld8a and Init_Coder_ld8a                      *
 *             ~~~~~~~~~~     ~~~~~~~~~~~~~~~                      *
 *                                                                 *
 *  Init_Coder_ld8a(void);                                         *
 *                                                                 *
 *   ->Initialization of variables for the coder section.          *
 *                                                                 *
 *                                                                 *
 *  Coder_ld8a(Word16 ana[]);                                      *
 *                                                                 *
 *   ->Main coder function.                                        *
 *                                                                 *
 *                                                                 *
 *  Input:                                                         *
 *                                                                 *
 *    80 speech data should have beee copy to vector new_speech[]. *
 *    This vector is global and is declared in this function.      *
 *                                                                 *
 *  Ouputs:                                                        *
 *                                                                 *
 *    ana[]      ->analysis parameters.                            *
 *                                                                 *
 *-----------------------------------------------------------------*/

#include "typedef.h"
#include "basic_op.h"
#include "ld8a.h"
#include "tab_ld8a.h"
#include "util.h"
#include "qua_lsp.h"
#include "taming.h"
#include "qua_gain.h"

/*-----------------------------------------------------------*
 *    Coder constant parameters (defined in "ld8a.h")        *
 *-----------------------------------------------------------*
 *   L_WINDOW    : LPC analysis window size.                 *
 *   L_NEXT      : Samples of next frame needed for autocor. *
 *   L_FRAME     : Frame size.                               *
 *   L_SUBFR     : Sub-frame size.                           *
 *   M           : LPC order.                                *
 *   MP1         : LPC order+1                               *
 *   L_TOTAL     : Total size of speech buffer.              *
 *   PIT_MIN     : Minimum pitch lag.                        *
 *   PIT_MAX     : Maximum pitch lag.                        *
 *   L_INTERPOL  : Length of filter for interpolation        *
 *-----------------------------------------------------------*/

/*-----------------------------------------------------------------*
 *   Function  Init_Coder_ld8a                                     *
 *            ~~~~~~~~~~~~~~~                                      *
 *                                                                 *
 *  Init_Coder_ld8a(void);                                         *
 *                                                                 *
 *   ->Initialization of variables for the coder section.          *
 *       - initialize pointers to speech buffer                    *
 *       - initialize pointers                             *
 *       - set vectors to zero                              *
 *                                                                 *
 *-----------------------------------------------------------------*/

CodState*
Init_Coder_ld8a (void)
{
  CodState *coder;
  
  coder = (CodState *) malloc (sizeof (CodState));

  /*----------------------------------------------------------------------*
  *      Initialize pointers to speech vector.                            *
  *                                                                       *
  *                                                                       *
  *   |--------------------|-------------|-------------|------------|     *
  *     previous speech           sf1           sf2         L_NEXT        *
  *                                                                       *
  *   <----------------  Total speech vector (L_TOTAL)   ----------->     *
  *   <----------------  LPC analysis window (L_WINDOW)  ----------->     *
  *   |                   <-- present frame (L_FRAME) -->                 *
  * old_speech            |              <-- new speech (L_FRAME) -->     *
  * p_window              |              |                                *
  *                     speech           |                                *
  *                             new_speech                                *
  *-----------------------------------------------------------------------*/

  coder->new_speech = coder->old_speech + L_TOTAL - L_FRAME;	/* New speech     */
  coder->speech = coder->new_speech - L_NEXT;	/* Present frame  */
  coder->p_window = coder->old_speech + L_TOTAL - L_WINDOW;	/* For LPC window */

  /* Initialize pointers */

  coder->wsp = coder->old_wsp + PIT_MAX;
  coder->exc = coder->old_exc + PIT_MAX + L_INTERPOL;

  /* Static vectors to zero */

  Set_zero (coder->old_speech, L_TOTAL);
  Set_zero (coder->old_exc, PIT_MAX + L_INTERPOL);
  Set_zero (coder->old_wsp, PIT_MAX);
  Set_zero (coder->mem_w, M);
  Set_zero (coder->mem_w0, M);
  Set_zero (coder->mem_zero, M);
  coder->sharp = SHARPMIN;

  /* Initialize lsp_old_q[] */
  Copy (lsp_old_init, coder->lsp_old, M);
  Copy (lsp_old_init, coder->lsp_old_q, M);
  
  Lsp_encw_reset (coder);
  Init_exc_err (coder);
  
  Copy (past_qua_en_init, coder->past_qua_en, 4);

  Set_zero (coder->old_A, M + 1);
  coder->old_A [0] = 4096;
  Set_zero (coder->old_rc, 2);

  return coder;
}

/*-----------------------------------------------------------------*
 *   Functions Coder_ld8a                                          *
 *            ~~~~~~~~~~                                           *
 *  Coder_ld8a(Word16 ana[]);                                      *
 *                                                                 *
 *   ->Main coder function.                                        *
 *                                                                 *
 *                                                                 *
 *  Input:                                                         *
 *                                                                 *
 *    80 speech data should have beee copy to vector new_speech[]. *
 *    This vector is global and is declared in this function.      *
 *                                                                 *
 *  Ouputs:                                                        *
 *                                                                 *
 *    ana[]      ->analysis parameters.                            *
 *                                                                 *
 *-----------------------------------------------------------------*/

void
Coder_ld8a (CodState *coder, Word16 ana[])
{

  /* LPC analysis */

  Word16 Aq_t[(MP1) * 2];	/* A(z)   quantized for the 2 subframes */
  Word16 Ap_t[(MP1) * 2];	/* A(z/gamma)       for the 2 subframes */
  Word16 *Aq, *Ap;		/* Pointer on Aq_t and Ap_t             */

  /* Other vectors */

  Word16 h1[L_SUBFR];		/* Impulse response h1[]              */
  Word16 xn[L_SUBFR];		/* Target vector for pitch search     */
  Word16 xn2[L_SUBFR];		/* Target vector for codebook search  */
  Word16 code[L_SUBFR];		/* Fixed codebook excitation          */
  Word16 y1[L_SUBFR];		/* Filtered adaptive excitation       */
  Word16 y2[L_SUBFR];		/* Filtered fixed codebook excitation */
  Word16 g_coeff[4];		/* Correlations between xn & y1       */

  Word16 g_coeff_cs[5];
  Word16 exp_g_coeff_cs[5];	/* Correlations between xn, y1, & y2
				   <y1,y1>, -2<xn,y1>,
				   <y2,y2>, -2<xn,y2>, 2<y1,y2> */

  /* Scalars */

  Word16 i, j, k, i_subfr;
  Word16 T_op, T0, T0_min, T0_max, T0_frac;
  Word16 gain_pit, gain_code, index;
  Word16 temp, taming;
  Word32 L_temp;

/*------------------------------------------------------------------------*
 *  - Perform LPC analysis:                                               *
 *       * autocorrelation + lag windowing                                *
 *       * Levinson-durbin algorithm to find a[]                          *
 *       * convert a[] to lsp[]                                           *
 *       * quantize and code the LSPs                                     *
 *       * find the interpolated LSPs and convert to a[] for the 2        *
 *         subframes (both quantized and unquantized)                     *
 *------------------------------------------------------------------------*/
  {
    /* Temporary vectors */
    Word16 r_l[MP1], r_h[MP1];	/* Autocorrelations low and hi          */
    Word16 rc[M];		/* Reflection coefficients.             */
    Word16 lsp_new[M], lsp_new_q[M];	/* LSPs at 2th subframe                 */

    /* LP analysis */

    Autocorr (coder->p_window, M, r_h, r_l);	/* Autocorrelations */
    Lag_window (M, r_h, r_l);	/* Lag windowing    */
    Levinson (coder, r_h, r_l, Ap_t, rc);	/* Levinson Durbin  */
    Az_lsp (Ap_t, lsp_new, coder->lsp_old);	/* From A(z) to lsp */

    /* LSP quantization */

    Qua_lsp (coder, lsp_new, lsp_new_q, ana);
    ana += 2;			/* Advance analysis parameters pointer */

    /*--------------------------------------------------------------------*
     * Find interpolated LPC parameters in all subframes                  *
     * The interpolated parameters are in array Aq_t[].                   *
     *--------------------------------------------------------------------*/

    Int_qlpc (coder->lsp_old_q, lsp_new_q, Aq_t);

    /* Compute A(z/gamma) */

    Weight_Az (&Aq_t[0], GAMMA1, M, &Ap_t[0]);
    Weight_Az (&Aq_t[MP1], GAMMA1, M, &Ap_t[MP1]);

    /* update the LSPs for the next frame */

    Copy (lsp_new, coder->lsp_old, M);
    Copy (lsp_new_q, coder->lsp_old_q, M);
  }

 /*----------------------------------------------------------------------*
  * - Find the weighted input speech w_sp[] for the whole speech frame   *
  * - Find the open-loop pitch delay                                     *
  *----------------------------------------------------------------------*/

  Residu (&Aq_t[0], &coder->speech[0], &coder->exc[0], L_SUBFR);
  Residu (&Aq_t[MP1], &coder->speech[L_SUBFR], &coder->exc[L_SUBFR], L_SUBFR);

  {
    Word16 Ap1[MP1];

    Ap = Ap_t;
    Ap1[0] = 4096;
    for (i = 1; i <= M; i++)	/* Ap1[i] = Ap[i] - 0.7 * Ap[i-1]; */
      Ap1[i] = sub (Ap[i], mult (Ap[i - 1], 22938));
    Syn_filt (Ap1, &coder->exc[0], &coder->wsp[0], L_SUBFR, coder->mem_w, 1, NULL);

    Ap += MP1;
    for (i = 1; i <= M; i++)	/* Ap1[i] = Ap[i] - 0.7 * Ap[i-1]; */
      Ap1[i] = sub (Ap[i], mult (Ap[i - 1], 22938));
    Syn_filt (Ap1, &coder->exc[L_SUBFR], &coder->wsp[L_SUBFR], L_SUBFR, coder->mem_w, 1, NULL);
  }

  /* Find open loop pitch lag */

  T_op = Pitch_ol_fast (coder->wsp, PIT_MAX, L_FRAME);

  /* Range for closed loop pitch search in 1st subframe */

  T0_min = sub (T_op, 3);
  if (sub (T0_min, PIT_MIN) < 0) {
    T0_min = PIT_MIN;
  }

  T0_max = add (T0_min, 6);
  if (sub (T0_max, PIT_MAX) > 0) {
    T0_max = PIT_MAX;
    T0_min = sub (T0_max, 6);
  }


 /*------------------------------------------------------------------------*
  *          Loop for every subframe in the analysis frame                 *
  *------------------------------------------------------------------------*
  *  To find the pitch and innovation parameters. The subframe size is     *
  *  L_SUBFR and the loop is repeated 2 times.                             *
  *     - find the weighted LPC coefficients                               *
  *     - find the LPC residual signal res[]                               *
  *     - compute the target signal for pitch search                       *
  *     - compute impulse response of weighted synthesis filter (h1[])     *
  *     - find the closed-loop pitch parameters                            *
  *     - encode the pitch delay                                           *
  *     - find target vector for codebook search                           *
  *     - codebook search                                                  *
  *     - VQ of pitch and codebook gains                                   *
  *     - update states of weighting filter                                *
  *------------------------------------------------------------------------*/

  Aq = Aq_t;			/* pointer to interpolated quantized LPC parameters */
  Ap = Ap_t;			/* pointer to weighted LPC coefficients             */

  for (i_subfr = 0; i_subfr < L_FRAME; i_subfr += L_SUBFR) {

    /*---------------------------------------------------------------*
     * Compute impulse response, h1[], of weighted synthesis filter  *
     *---------------------------------------------------------------*/

    h1[0] = 4096;
    Set_zero (&h1[1], L_SUBFR - 1);
    Syn_filt (Ap, h1, h1, L_SUBFR, &h1[1], 0, NULL);

   /*----------------------------------------------------------------------*
    *  Find the target vector for pitch search:                            *
    *----------------------------------------------------------------------*/

    Syn_filt (Ap, &coder->exc[i_subfr], xn, L_SUBFR, coder->mem_w0, 0, NULL);

    /*---------------------------------------------------------------------*
     *                 Closed-loop fractional pitch search                 *
     *---------------------------------------------------------------------*/

    T0 = Pitch_fr3_fast (&coder->exc[i_subfr], xn, h1, L_SUBFR, T0_min, T0_max,
			 i_subfr, &T0_frac);

    index =
      Enc_lag3 (T0, T0_frac, &T0_min, &T0_max, PIT_MIN, PIT_MAX, i_subfr);

    *ana++ = index;

    if (i_subfr == 0) {
      *ana++ = Parity_Pitch (index);
    }

   /*-----------------------------------------------------------------*
    *   - find filtered pitch exc                                     *
    *   - compute pitch gain and limit between 0 and 1.2              *
    *   - update target vector for codebook search                    *
    *-----------------------------------------------------------------*/

    Syn_filt (Ap, &coder->exc[i_subfr], y1, L_SUBFR, coder->mem_zero, 0, NULL);

    gain_pit = G_pitch (xn, y1, g_coeff, L_SUBFR);

    /* clip pitch gain if taming is necessary */

    taming = test_err (coder, T0, T0_frac);

    if (taming == 1) {
      if (sub (gain_pit, GPCLIP) > 0) {
	gain_pit = GPCLIP;
      }
    }

    /* xn2[i]   = xn[i] - y1[i] * gain_pit  */

    for (i = 0; i < L_SUBFR; i++) {
      L_temp = L_mult (y1[i], gain_pit);
      L_temp = L_shl (L_temp, 1);	/* gain_pit in Q14 */
      xn2[i] = sub (xn[i], extract_h (L_temp));
    }


   /*-----------------------------------------------------*
    * - Innovative codebook search.                       *
    *-----------------------------------------------------*/

    index = ACELP_Code_A (xn2, h1, T0, coder->sharp, code, y2, &i);

    *ana++ = index;		/* Positions index */
    *ana++ = i;			/* Signs index     */


   /*-----------------------------------------------------*
    * - Quantization of gains.                            *
    *-----------------------------------------------------*/

    g_coeff_cs[0] = g_coeff[0];	/* <y1,y1> */
    exp_g_coeff_cs[0] = negate (g_coeff[1]);	/* Q-Format:XXX -> JPN */
    g_coeff_cs[1] = negate (g_coeff[2]);	/* (xn,y1) -> -2<xn,y1> */
    exp_g_coeff_cs[1] = negate (add (g_coeff[3], 1));	/* Q-Format:XXX -> JPN */

    Corr_xy2 (xn, y1, y2, g_coeff_cs, exp_g_coeff_cs);	/* Q0 Q0 Q12 ^Qx ^Q0 */
    /* g_coeff_cs[3]:exp_g_coeff_cs[3] = <y2,y2>   */
    /* g_coeff_cs[4]:exp_g_coeff_cs[4] = -2<xn,y2> */
    /* g_coeff_cs[5]:exp_g_coeff_cs[5] = 2<y1,y2>  */

    *ana++ = Qua_gain (coder, code, g_coeff_cs, exp_g_coeff_cs,
		       L_SUBFR, &gain_pit, &gain_code, taming);


   /*------------------------------------------------------------*
    * - Update pitch sharpening "sharp" with quantized gain_pit  *
    *------------------------------------------------------------*/

    coder->sharp = gain_pit;
    if (sub (coder->sharp, SHARPMAX) > 0) {
      coder->sharp = SHARPMAX;
    }
    if (sub (coder->sharp, SHARPMIN) < 0) {
      coder->sharp = SHARPMIN;
    }

   /*------------------------------------------------------*
    * - Find the total excitation                          *
    * - update filters memories for finding the target     *
    *   vector in the next subframe                        *
    *------------------------------------------------------*/

    for (i = 0; i < L_SUBFR; i++) {
      /* exc[i] = gain_pit*exc[i] + gain_code*code[i]; */
      /* exc[i]  in Q0   gain_pit in Q14               */
      /* code[i] in Q13  gain_cod in Q1                */

      L_temp = L_mult (coder->exc[i + i_subfr], gain_pit);
      L_temp = L_mac (L_temp, code[i], gain_code);
      L_temp = L_shl (L_temp, 1);
      coder->exc[i + i_subfr] = wround (L_temp);
    }

    update_exc_err (coder, gain_pit, T0);

    for (i = L_SUBFR - M, j = 0; i < L_SUBFR; i++, j++) {
      temp = extract_h (L_shl (L_mult (y1[i], gain_pit), 1));
      k = extract_h (L_shl (L_mult (y2[i], gain_code), 2));
      coder->mem_w0[j] = sub (xn[i], add (temp, k));
    }

    Aq += MP1;			/* interpolated LPC parameters for next subframe */
    Ap += MP1;

  }

 /*--------------------------------------------------*
  * Update signal for next frame.                    *
  * -> shift to the left by L_FRAME:                 *
  *     speech[], wsp[] and  exc[]                   *
  *--------------------------------------------------*/

  Copy (&coder->old_speech[L_FRAME], &coder->old_speech[0], L_TOTAL - L_FRAME);
  Copy (&coder->old_wsp[L_FRAME], &coder->old_wsp[0], PIT_MAX);
  Copy (&coder->old_exc[L_FRAME], &coder->old_exc[0], PIT_MAX + L_INTERPOL);

  return;
}
