/*
   ITU-T G.729A Speech Coder    ANSI-C Source Code
   Version 1.1    Last modified: September 1996

   Copyright (c) 1996,
   AT&T, France Telecom, NTT, Universite de Sherbrooke
   All rights reserved.
*/

/*--------------------------------------------------------------------------*
 * Prototypes for auxiliary functions.                                      *
 *--------------------------------------------------------------------------*/

void Copy (Word16 x[], Word16 y[], Word16 L);

void Set_zero (Word16 x[], Word16 L);

Word16 Random (Word16 *seed);

void Store_Params(Word16 * parm, void *to);

void Restore_Params(const void *from, Word16 * parm);
