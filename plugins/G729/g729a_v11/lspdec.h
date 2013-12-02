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

void D_lsp (DecState *decoder, 
	    Word16 prm[],	/* (i)     : indexes of the selected LSP */
	    Word16 lsp_q[],	/* (o) Q15 : Quantized LSP parameters    */
	    Word16 erase	/* (i)     : frame erase information     */
  );

void Lsp_decw_reset (DecState *decoder);

void Lsp_iqua_cs (DecState *decoder,
		  Word16 prm[],	/* input : codes of the selected LSP */
		  Word16 lsp_q[],	/* output: Quantized LSP parameters */
		  Word16 erase	/* input : frame erase information */
  );

