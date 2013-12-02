/*
   ITU-T G.729A Speech Coder    ANSI-C Source Code
   Version 1.1    Last modified: September 1996

   Copyright (c) 1996,
   AT&T, France Telecom, NTT, Universite de Sherbrooke
   All rights reserved.
*/

void Lsf_lsp2 (Word16 lsf[],	/* (i) Q13 : lsf[m] (range: 0.0<=val<PI) */
	       Word16 lsp[],	/* (o) Q15 : lsp[m] (range: -1<=val<1)   */
	       Word16 m		/* (i)     : LPC order                   */
  );

void Lsp_lsf2 (Word16 lsp[],	/* (i) Q15 : lsp[m] (range: -1<=val<1)   */
	       Word16 lsf[],	/* (o) Q13 : lsf[m] (range: 0.0<=val<PI) */
	       Word16 m		/* (i)     : LPC order                   */
  );
