/*
   ITU-T G.729A Speech Coder    ANSI-C Source Code
   Version 1.1    Last modified: September 1996

   Copyright (c) 1996,
   AT&T, France Telecom, NTT, Universite de Sherbrooke
   All rights reserved.
*/

#include "typedef.h"
#include "ld8a.h"
#include "basic_op.h"
#include "tab_ld8a.h"
#include "util.h"
#include "lspgetq.h"
#include "lpcfunc.h"


/*----------------------------------------------------------------------------
 * Lsp_decw_reset -   set the previous LSP vectors
 *----------------------------------------------------------------------------
 */
void
Lsp_decw_reset (DecState *decoder)
{
  Word16 i;

  for (i = 0; i < MA_NP; i++)
    Copy (&freq_prev_reset[0], &decoder->freq_prev[i][0], M);

  decoder->prev_ma = 0;

  Copy (freq_prev_reset, decoder->prev_lsp, M);
}



/*----------------------------------------------------------------------------
 * Lsp_iqua_cs -  LSP main quantization routine
 *----------------------------------------------------------------------------
 */
void
Lsp_iqua_cs (DecState *decoder,
	     Word16 prm[],	/* (i)     : indexes of the selected LSP */
	     Word16 lsp_q[],	/* (o) Q13 : Quantized LSP parameters    */
	     Word16 erase	/* (i)     : frame erase information     */
  )
{
  Word16 mode_index;
  Word16 code0;
  Word16 code1;
  Word16 code2;
  Word16 buf[M];		/* Q13 */

  if (erase == 0) {		/* Not frame erasure */
    mode_index = shr (prm[0], NC0_B) & (Word16) 1;
    code0 = prm[0] & (Word16) (NC0 - 1);
    code1 = shr (prm[1], NC1_B) & (Word16) (NC1 - 1);
    code2 = prm[1] & (Word16) (NC1 - 1);

    /* compose quantized LSP (lsp_q) from indexes */

    Lsp_get_quant (lspcb1, lspcb2, code0, code1, code2,
		   fg[mode_index], decoder->freq_prev, lsp_q, fg_sum[mode_index]);

    /* save parameters to use in case of the frame erased situation */

    Copy (lsp_q, decoder->prev_lsp, M);
    decoder->prev_ma = mode_index;
  }
  else {			/* Frame erased */
    /* use revious LSP */

    Copy (decoder->prev_lsp, lsp_q, M);

    /* update freq_prev */

    Lsp_prev_extract (decoder->prev_lsp, buf,
		      fg[decoder->prev_ma], decoder->freq_prev, fg_sum_inv[decoder->prev_ma]);
    Lsp_prev_update (buf, decoder->freq_prev);
  }

  return;
}



/*-------------------------------------------------------------------*
 * Function  D_lsp:                                                  *
 *           ~~~~~~                                                  *
 *-------------------------------------------------------------------*/

void
D_lsp (DecState *decoder,
       Word16 prm[],		/* (i)     : indexes of the selected LSP */
       Word16 lsp_q[],		/* (o) Q15 : Quantized LSP parameters    */
       Word16 erase		/* (i)     : frame erase information     */
  )
{
  Word16 lsf_q[M];		/* domain 0.0<= lsf_q <PI in Q13 */


  Lsp_iqua_cs (decoder, prm, lsf_q, erase);

  /* Convert LSFs to LSPs */

  Lsf_lsp2 (lsf_q, lsp_q, M);

  return;
}
