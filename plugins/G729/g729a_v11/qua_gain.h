/*
   ITU-T G.729A Speech Coder    ANSI-C Source Code
   Version 1.1    Last modified: September 1996

   Copyright (c) 1996,
   AT&T, France Telecom, NTT, Universite de Sherbrooke
   All rights reserved.
*/

/*--------------------------------------------------------------------------*
 * gain VQ functions.                                                       *
 *--------------------------------------------------------------------------*/

Word16 Qua_gain (CodState *coder, 
		 Word16 code[],	/* (i) Q13 : Innovative vector.                         */
		 Word16 g_coeff[],	/* (i)     : Correlations <xn y1> -2<y1 y1>             */
		 /*            <y2,y2>, -2<xn,y2>, 2<y1,y2>              */
		 Word16 exp_coeff[],	/* (i)    : Q-Format g_coeff[]                         */
		 Word16 L_subfr,	/* (i)     : Subframe length.                           */
		 Word16 * gain_pit,	/* (o) Q14 : Pitch gain.                                */
		 Word16 * gain_cod,	/* (o) Q1  : Code gain.                                 */
		 Word16 tameflag	/* (i)     : flag set to 1 if taming is needed          */
  );
