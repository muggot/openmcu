/*
   ITU-T G.729A Speech Coder    ANSI-C Source Code
   Version 1.1    Last modified: September 1996

   Copyright (c) 1996,
   AT&T, France Telecom, NTT, Universite de Sherbrooke
   All rights reserved.
*/

/*------------------------------------------------------*
 * Parity_pitch - compute parity bit for first 6 MSBs   *
 *------------------------------------------------------*/

#include "typedef.h"
#include "basic_op.h"
#include "ld8a.h"

Word16
Parity_Pitch (			/* output: parity bit (XOR of 6 MSB bits)    */
	       Word16 pitch_index	/* input : index for which parity to compute */
  )
{
  Word16 temp, sum, i, bit;

  temp = shr (pitch_index, 1);

  sum = 1;
  for (i = 0; i <= 5; i++) {
    temp = shr (temp, 1);
    bit = temp & (Word16) 1;
    sum = add (sum, bit);
  }
  sum = sum & (Word16) 1;


  return sum;
}

/*--------------------------------------------------------------------*
 * check_parity_pitch - check parity of index with transmitted parity *
 *--------------------------------------------------------------------*/

Word16
Check_Parity_Pitch (		/* output: 0 = no error, 1= error */
		     Word16 pitch_index,	/* input : index of parameter     */
		     Word16 parity	/* input : parity bit             */
  )
{
  Word16 temp, sum, i, bit;

  temp = shr (pitch_index, 1);

  sum = 1;
  for (i = 0; i <= 5; i++) {
    temp = shr (temp, 1);
    bit = temp & (Word16) 1;
    sum = add (sum, bit);
  }
  sum = add (sum, parity);
  sum = sum & (Word16) 1;

  return sum;
}
