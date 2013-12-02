/*
   ITU-T G.729A Speech Coder    ANSI-C Source Code
   Version 1.1    Last modified: September 1996

   Copyright (c) 1996,
   AT&T, France Telecom, NTT, Universite de Sherbrooke
   All rights reserved.
*/

/*-------------------------------------------------------------------*
 * Function  Set zero()                                              *
 *           ~~~~~~~~~~                                              *
 * Set vector x[] to zero                                            *
 *-------------------------------------------------------------------*/

#include "typedef.h"
#include "basic_op.h"
#include "ld8a.h"
#include "tab_ld8a.h"

void
Set_zero (Word16 x[], Word16 L)
{
  Word16 i;

  for (i = 0; i < L; i++)
    x[i] = 0;

  return;
}

/*-------------------------------------------------------------------*
 * Function  Copy:                                                   *
 *           ~~~~~                                                   *
 * Copy vector x[] to y[]                                            *
 *-------------------------------------------------------------------*/

void
Copy (Word16 x[], Word16 y[], Word16 L)
{
  Word16 i;

  for (i = 0; i < L; i++)
    y[i] = x[i];

  return;
}

/* Random generator  */

Word16
Random (Word16* seed)
{
  /* seed = seed*31821 + 13849; */
  *seed = extract_l (L_add (L_shr (L_mult (*seed, 31821), 1), 13849L));

  return (*seed);
}


/*----------------------------------------------------------------------------
 * Store_Param - converts encoder parameter vector into frame
 * Restore_Params - converts serial received frame to  encoder parameter vector
 *
 * The transmitted parameters are:
 *
 *     LPC:     1st codebook           7+1 bit
 *              2nd codebook           5+5 bit
 *
 *     1st subframe:
 *          pitch period                 8 bit
 *          parity check on 1st period   1 bit
 *          codebook index1 (positions) 13 bit
 *          codebook index2 (signs)      4 bit
 *          pitch and codebook gains   4+3 bit
 *
 *     2nd subframe:
 *          pitch period (relative)      5 bit
 *          codebook index1 (positions) 13 bit
 *          codebook index2 (signs)      4 bit
 *          pitch and codebook gains   4+3 bit
 *----------------------------------------------------------------------------
 */


void 
Store_Params(Word16 * parm, void *to)
{
	int i, j;
	unsigned char mask, *to_b;
	Word16 value, val_mask;

	to_b = (unsigned char *)to;
	mask = 0x80;
	for (i = 0; i < PRM_SIZE; i++) {
		value = parm[i];
		val_mask = 1 << (bitsno[i] - 1);
		for (j = 0; j < bitsno[i]; j++) {

			if (value & val_mask)
				*to_b |= mask;
			else 		
				*to_b &= ~mask;

			value = value << 1;
			mask = mask >> 1;

			if (mask == 0) {
				mask = 0x80;
				to_b++;
			}
		}
	}
	return;
}

void Restore_Params(const void *from, Word16 * parm)
{
	int i, j;
	unsigned char mask, *from_b;
	Word16 value;

	mask = 0x80;
	from_b = (unsigned char *)from;

	for (i = 0; i < PRM_SIZE; i++) {
		value = 0;
		for (j = 0; j < bitsno[i]; j++) {

			value = value << 1;

			if (mask & (*from_b))
				value += 1;

			mask = mask >> 1;
			if (mask == 0) {
				mask = 0x80;
				from_b++;
			}
		}
		parm[i] = value;
	}
	return;
}

