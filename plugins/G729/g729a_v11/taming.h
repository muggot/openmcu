/*
   ITU-T G.729A Speech Coder    ANSI-C Source Code
   Version 1.1    Last modified: September 1996

   Copyright (c) 1996,
   AT&T, France Telecom, NTT, Universite de Sherbrooke
   All rights reserved.
*/

/*--------------------------------------------------------------------------*
 * Constants and prototypes for taming procedure.                           *
 *--------------------------------------------------------------------------*/

#define GPCLIP      15564	/* Maximum pitch gain if taming is needed Q14 */
#define GPCLIP2     481		/* Maximum pitch gain if taming is needed Q9 */
#define GP0999      16383	/* Maximum pitch gain if taming is needed    */
#define L_THRESH_ERR 983040000L	/* Error threshold taming 16384. * 60000.   */

void Init_exc_err (CodState *coder);
void update_exc_err (CodState *coder, Word16 gain_pit, Word16 t0);
Word16 test_err (CodState *coder, Word16 t0, Word16 t0_frac);

