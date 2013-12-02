/*
   ITU-T G.729A Speech Coder    ANSI-C Source Code
   Version 1.1    Last modified: September 1996

   Copyright (c) 1996,
   AT&T, France Telecom, NTT, Universite de Sherbrooke  
   All rights reserved.
*/

/*---------------------------------------------------------------------------*
 *  Gain_predict()        : make gcode0(exp_gcode0)                          *
 *  Gain_update()         : update table of past quantized energies.         *
 *  Gain_update_erasure() : update table of past quantized energies.         *
 *                                                        (frame erasure)    *
 *    This function is used both Coder and Decoder.                          *
 *---------------------------------------------------------------------------*/

#include "typedef.h"
#include "basic_op.h"
#include "ld8a.h"
#include "tab_ld8a.h"
#include "oper_32b.h"

/*---------------------------------------------------------------------------*
 * Function  Gain_predict                                                    *
 * ~~~~~~~~~~~~~~~~~~~~~~                                                    *
 * MA prediction is performed on the innovation energy (in dB with mean      *
 * removed).                                                                 *
 *---------------------------------------------------------------------------*/
void
Gain_predict (Word16 past_qua_en[],	/* (i) Q10 :Past quantized energies        */
	      Word16 code[],	/* (i) Q13 :Innovative vector.             */
	      Word16 L_subfr,	/* (i)     :Subframe length.               */
	      Word16 * gcode0,	/* (o) Qxx :Predicted codebook gain        */
	      Word16 * exp_gcode0	/* (o)     :Q-Format(gcode0)               */
  )
{
  Word16 i, exp, frac;
  Word32 L_tmp;

  /*-------------------------------*
   * Energy coming from code       *
   *-------------------------------*/

  L_tmp = 0;
  for (i = 0; i < L_subfr; i++)
    L_tmp = L_mac (L_tmp, code[i], code[i]);

  /*-----------------------------------------------------------------*
   *  Compute: means_ener - 10log10(ener_code/ L_sufr)               *
   *  Note: mean_ener change from 36 dB to 30 dB because input/2     *
   *                                                                 *
   * = 30.0 - 10 log10( ener_code / lcode)  + 10log10(2^27)          *
   *                                          !!ener_code in Q27!!   *
   * = 30.0 - 3.0103 * log2(ener_code) + 10log10(40) + 10log10(2^27) *
   * = 30.0 - 3.0103 * log2(ener_code) + 16.02  + 81.278             *
   * = 127.298 - 3.0103 * log2(ener_code)                            *
   *-----------------------------------------------------------------*/

  Log2 (L_tmp, &exp, &frac);	/* Q27->Q0 ^Q0 ^Q15       */
  L_tmp = Mpy_32_16 (exp, frac, -24660);	/* Q0 Q15 Q13 -> ^Q14     */
  /* hi:Q0+Q13+1            */
  /* lo:Q15+Q13-15+1        */
  /* -24660[Q13]=-3.0103    */
  L_tmp = L_mac (L_tmp, 32588, 32);	/* 32588*32[Q14]=127.298  */

  /*-----------------------------------------------------------------*
   * Compute gcode0.                                                 *
   *  = Sum(i=0,3) pred[i]*past_qua_en[i] - ener_code + mean_ener    *
   *-----------------------------------------------------------------*/

  L_tmp = L_shl (L_tmp, 10);	/* From Q14 to Q24 */
  for (i = 0; i < 4; i++)
    L_tmp = L_mac (L_tmp, pred[i], past_qua_en[i]);	/* Q13*Q10 ->Q24 */

  *gcode0 = extract_h (L_tmp);	/* From Q24 to Q8  */

  /*-----------------------------------------------------------------*
   * gcode0 = pow(10.0, gcode0/20)                                   *
   *        = pow(2, 3.3219*gcode0/20)                               *
   *        = pow(2, 0.166*gcode0)                                   *
   *-----------------------------------------------------------------*/

  L_tmp = L_mult (*gcode0, 5439);	/* *0.166 in Q15, result in Q24 */
  L_tmp = L_shr (L_tmp, 8);	/* From Q24 to Q16             */
  L_Extract (L_tmp, &exp, &frac);	/* Extract exponent of gcode0  */

  *gcode0 = extract_l (Pow2 (14, frac));	/* Put 14 as exponent so that  */
  /* output of Pow2() will be:   */
  /* 16768 < Pow2() <= 32767     */
  *exp_gcode0 = sub (14, exp);
}


/*---------------------------------------------------------------------------*
 * Function  Gain_update                                                     *
 * ~~~~~~~~~~~~~~~~~~~~~~                                                    *
 * update table of past quantized energies                                   *
 *---------------------------------------------------------------------------*/
void
Gain_update (Word16 past_qua_en[],	/* (io) Q10 :Past quantized energies           */
	     Word32 L_gbk12	/* (i) Q13 : gbk1[indice1][1]+gbk2[indice2][1] */
  )
{
  Word16 i, tmp;
  Word16 exp, frac;
  Word32 L_acc;

  for (i = 3; i > 0; i--) {
    past_qua_en[i] = past_qua_en[i - 1];	/* Q10 */
  }
  /*----------------------------------------------------------------------*
   * -- past_qua_en[0] = 20*log10(gbk1[index1][1]+gbk2[index2][1]); --    *
   *    2 * 10 log10( gbk1[index1][1]+gbk2[index2][1] )                   *
   *  = 2 * 3.0103 log2( gbk1[index1][1]+gbk2[index2][1] )                *
   *  = 2 * 3.0103 log2( gbk1[index1][1]+gbk2[index2][1] )                *
   *                                                 24660:Q12(6.0205)    *
   *----------------------------------------------------------------------*/

  Log2 (L_gbk12, &exp, &frac);	/* L_gbk12:Q13       */
  L_acc = L_Comp (sub (exp, 13), frac);	/* L_acc:Q16           */
  tmp = extract_h (L_shl (L_acc, 13));	/* tmp:Q13           */
  past_qua_en[0] = mult (tmp, 24660);	/* past_qua_en[]:Q10 */
}


/*---------------------------------------------------------------------------*
 * Function  Gain_update_erasure                                             *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~                                             *
 * update table of past quantized energies (frame erasure)                   *
 *---------------------------------------------------------------------------*
 *     av_pred_en = 0.0;                                                     *
 *     for (i = 0; i < 4; i++)                                               *
 *        av_pred_en += past_qua_en[i];                                      *
 *     av_pred_en = av_pred_en*0.25 - 4.0;                                   *
 *     if (av_pred_en < -14.0) av_pred_en = -14.0;                           *
 *---------------------------------------------------------------------------*/
void
Gain_update_erasure (Word16 past_qua_en[]	/* (i) Q10 :Past quantized energies        */
  )
{
  Word16 i, av_pred_en;
  Word32 L_tmp;

  L_tmp = 0;			/* Q10 */
  for (i = 0; i < 4; i++)
    L_tmp = L_add (L_tmp, L_deposit_l (past_qua_en[i]));
  av_pred_en = extract_l (L_shr (L_tmp, 2));
  av_pred_en = sub (av_pred_en, 4096);	/* Q10 */

  if (sub (av_pred_en, -14336) < 0) {
    av_pred_en = -14336;	/* 14336:14[Q10] */
  }

  for (i = 3; i > 0; i--) {
    past_qua_en[i] = past_qua_en[i - 1];
  }
  past_qua_en[0] = av_pred_en;
}
