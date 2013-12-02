/*
   ITU-T G.729A Speech Coder    ANSI-C Source Code
   Version 1.1    Last modified: September 1996

   Copyright (c) 1996,
   AT&T, France Telecom, NTT, Universite de Sherbrooke
   All rights reserved.
*/

void Lsp_get_quant (Word16 lspcb1[][M],	/* Q13 */
		    Word16 lspcb2[][M],	/* Q13 */
		    Word16 code0, Word16 code1, Word16 code2, Word16 fg[][M],	/* Q15 */
		    Word16 freq_prev[][M],	/* Q13 */
		    Word16 lspq[],	/* Q13 */
		    Word16 fg_sum[]	/* Q15 */
  );

void Lsp_expand_1 (Word16 buf[],	/* Q13 */
		   Word16 gap	/* Q13 */
  );

void Lsp_expand_2 (Word16 buf[],	/* Q13 */
		   Word16 gap	/* Q13 */
  );

void Lsp_expand_1_2 (Word16 buf[],	/* Q13 */
		     Word16 gap	/* Q13 */
  );

void Lsp_stability (Word16 buf[]	/* Q13 */
  );

void Lsp_prev_compose (Word16 lsp_ele[],	/* Q13 */
		       Word16 lsp[],	/* Q13 */
		       Word16 fg[][M],	/* Q15 */
		       Word16 freq_prev[][M],	/* Q13 */
		       Word16 fg_sum[]	/* Q15 */
  );

void Lsp_prev_extract (Word16 lsp[M],	/* Q13 */
		       Word16 lsp_ele[M],	/* Q13 */
		       Word16 fg[MA_NP][M],	/* Q15 */
		       Word16 freq_prev[MA_NP][M],	/* Q13 */
		       Word16 fg_sum_inv[M]	/* Q12 */
  );

void Lsp_prev_update (Word16 lsp_ele[M],	/* Q13 */
		      Word16 freq_prev[MA_NP][M]	/* Q13 */
  );

