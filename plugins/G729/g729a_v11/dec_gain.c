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

/*---------------------------------------------------------------------------*
 * Function  Dec_gain                                                        *
 * ~~~~~~~~~~~~~~~~~~                                                        *
 * Decode the pitch and codebook gains                                       *
 *                                                                           *
 *---------------------------------------------------------------------------*
 * input arguments:                                                          *
 *                                                                           *
 *   index      :Quantization index                                          *
 *   code[]     :Innovative code vector                                      *
 *   L_subfr    :Subframe size                                               *
 *   bfi        :Bad frame indicator                                         *
 *                                                                           *
 * output arguments:                                                         *
 *                                                                           *
 *   gain_pit   :Quantized pitch gain                                        *
 *   gain_cod   :Quantized codebook gain                                     *
 *                                                                           *
 *---------------------------------------------------------------------------*/
void
Dec_gain (DecState *decoder,
	  Word16 index,		/* (i)     :Index of quantization.         */
	  Word16 code[],	/* (i) Q13 :Innovative vector.             */
	  Word16 L_subfr,	/* (i)     :Subframe length.               */
	  Word16 bfi,		/* (i)     :Bad frame indicator            */
	  Word16 * gain_pit,	/* (o) Q14 :Pitch gain.                    */
	  Word16 * gain_cod	/* (o) Q1  :Code gain.                     */
  )
{
  Word16 index1, index2, tmp;
  Word16 gcode0, exp_gcode0;
  Word32 L_gbk12, L_acc, L_accb;
  void Gain_predict (Word16 past_qua_en[], Word16 code[], Word16 L_subfr,
		     Word16 * gcode0, Word16 * exp_gcode0);
  void Gain_update (Word16 past_qua_en[], Word32 L_gbk12);
  void Gain_update_erasure (Word16 past_qua_en[]);

  /* Gain predictor, Past quantized energies = -14.0 in Q10 */



   /*-------------- Case of erasure. ---------------*/

  if (bfi != 0) {
    *gain_pit = mult (*gain_pit, 29491);	/* *0.9 in Q15 */
    if (sub (*gain_pit, 29491) > 0)
      *gain_pit = 29491;
    *gain_cod = mult (*gain_cod, 32111);	/* *0.98 in Q15 */

     /*----------------------------------------------*
      * update table of past quantized energies      *
      *                              (frame erasure) *
      *----------------------------------------------*/
    Gain_update_erasure (decoder->past_qua_en);

    return;
  }

   /*-------------- Decode pitch gain ---------------*/

  index1 = imap1[shr (index, NCODE2_B)];
  index2 = imap2[index & (NCODE2 - 1)];
  *gain_pit = add (gbk1[index1][0], gbk2[index2][0]);

   /*-------------- Decode codebook gain ---------------*/

  /*---------------------------------------------------*
   *-  energy due to innovation                       -*
   *-  predicted energy                               -*
   *-  predicted codebook gain => gcode0[exp_gcode0]  -*
   *---------------------------------------------------*/

  Gain_predict (decoder->past_qua_en, code, L_subfr, &gcode0, &exp_gcode0);

  /*-----------------------------------------------------------------*
   * *gain_code = (gbk1[indice1][1]+gbk2[indice2][1]) * gcode0;      *
   *-----------------------------------------------------------------*/

  L_acc = L_deposit_l (gbk1[index1][1]);
  L_accb = L_deposit_l (gbk2[index2][1]);
  L_gbk12 = L_add (L_acc, L_accb);	/* Q13 */
  tmp = extract_l (L_shr (L_gbk12, 1));	/* Q12 */
  L_acc = L_mult (tmp, gcode0);	/* Q[exp_gcode0+12+1] */

  L_acc = L_shl (L_acc, add (negate (exp_gcode0), (-12 - 1 + 1 + 16)));
  *gain_cod = extract_h (L_acc);	/* Q1 */

  /*----------------------------------------------*
   * update table of past quantized energies      *
   *----------------------------------------------*/
  Gain_update (decoder->past_qua_en, L_gbk12);

  return;

}
