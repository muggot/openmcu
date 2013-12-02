/*
   ITU-T G.729A Speech Coder    ANSI-C Source Code
   Version 1.1    Last modified: September 1996

   Copyright (c) 1996,
   AT&T, France Telecom, NTT, Universite de Sherbrooke
   All rights reserved.
*/


void Dec_gain (DecState *decoder,
	       Word16 index,	/* (i)     : Index of quantization.                     */
	       Word16 code[],	/* (i) Q13 : Innovative vector.                         */
	       Word16 L_subfr,	/* (i)     : Subframe length.                           */
	       Word16 bfi,	/* (i)     : Bad frame indicator                        */
	       Word16 * gain_pit,	/* (o) Q14 : Pitch gain.                                */
	       Word16 * gain_cod	/* (o) Q1  : Code gain.                                 */
  );
