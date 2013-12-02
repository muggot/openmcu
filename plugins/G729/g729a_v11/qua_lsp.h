/*
   ITU-T G.729A Speech Coder    ANSI-C Source Code
   Version 1.1    Last modified: September 1996

   Copyright (c) 1996,
   AT&T, France Telecom, NTT, Universite de Sherbrooke
   All rights reserved.
*/

/*-------------------------------*
 * LSP VQ functions.             *
 *-------------------------------*/

void Lsp_qua_cs (CodState *coder,
		 Word16 flsp_in[M],	/* Q13 */
		 Word16 lspq_out[M],	/* Q13 */
		 Word16 * code);

void Lsp_get_tdist (Word16 wegt[],	/* normalized */
		    Word16 buf[],	/* Q13 */
		    Word32 * L_tdist,	/* Q27 */
		    Word16 rbuf[],	/* Q13 */
		    Word16 fg_sum[]	/* Q15 */
  );

void Lsp_last_select (Word32 L_tdist[],	/* Q27 */
		      Word16 * mode_index);

void Lsp_pre_select (Word16 rbuf[],	/* Q13 */
		     Word16 lspcb1[][M],	/* Q13 */
		     Word16 * cand);

void Lsp_select_1 (Word16 rbuf[],	/* Q13 */
		   Word16 lspcb1[],	/* Q13 */
		   Word16 wegt[],	/* normalized */
		   Word16 lspcb2[][M],	/* Q13 */
		   Word16 * index);

void Lsp_select_2 (Word16 rbuf[],	/* Q13 */
		   Word16 lspcb1[],	/* Q13 */
		   Word16 wegt[],	/* normalized */
		   Word16 lspcb2[][M],	/* Q13 */
		   Word16 * index);

void Relspwed (Word16 lsp[],	/* Q13 */
	       Word16 wegt[],	/* normalized */
	       Word16 lspq[],	/* Q13 */
	       Word16 lspcb1[][M],	/* Q13 */
	       Word16 lspcb2[][M],	/* Q13 */
	       Word16 fg[MODE][MA_NP][M],	/* Q15 */
	       Word16 freq_prev[MA_NP][M],	/* Q13 */
	       Word16 fg_sum[MODE][M],	/* Q15 */
	       Word16 fg_sum_inv[MODE][M],	/* Q12 */
	       Word16 code_ana[]
  );

void Qua_lsp (CodState *coder, 
	      Word16 lsp[],	/* (i) Q15 : Unquantized LSP            */
	      Word16 lsp_q[],	/* (o) Q15 : Quantized LSP              */
	      Word16 ana[]	/* (o)     : indexes                    */
  );

void Get_wegt (Word16 flsp[],	/* Q13 */
	       Word16 wegt[]	/* Q11 -> normalized */
  );

void Lsp_encw_reset (CodState *coder);
