/*
 * Copyright (c) 1994 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *      This product includes software developed by the Network Research
 *	Group at Lawrence Berkeley Laboratory.
 * 4. Neither the name of the University nor of the Laboratory may be used
 *    to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

/************ Change log
 *
 * $Log: dct.h,v $
 * Revision 1.1  2010/02/24 02:19:22  shorne
 * First commit of h323plus mirror
 *
 * Revision 1.2  2006/07/31 09:09:21  csoutheren
 * Checkin of validated codec used during development
 *
 * Revision 1.1.2.1  2006/04/06 01:17:17  csoutheren
 * Initial version of H.261 video codec plugin for OPAL
 *
 * Revision 2.1  2003/03/15 23:42:59  robertj
 * Update to OpenH323 v1.11.7
 *
 * Revision 1.5  2003/03/14 07:25:55  robertj
 * Removed $header keyword so is not different on alternate repositories
 *
 * Revision 1.4  2002/05/17 01:47:33  dereks
 * backout the integer maths in the h261 codec.
 *
 * Revision 1.3  2001/10/14 21:48:45  dereks
 * Change vic's fdct() from floating-point to fix-point. Improves performance
 * for h261 video significantly on some machines. Thanks to Cosmos Jiang
 *
 * Revision 1.2  2000/08/25 03:18:49  dereks
 * Add change log facility (Thanks Robert for the info on implementation)
 *
 *
 *
 ********/



/*
 * Ck = cos(k pi / 16)
 * Sk = sin(k pi / 16)
 */
#define C1 0.98078528
#define C2 0.92387953
#define C3 0.83146961
#define C4 0.70710678
#define C5 0.55557023
#define C6 0.38268343
#define C7 0.19509032
#define S1 C7
#define S3 C5
#define S6 C2
#define C_1 C1
#define S_1 (-S1)
#define C_3 C3
#define S_3 (-S3)

void fdct(const u_char* in, int stride, short* out, const float* qt);
void bv_rdct1(int dc, short* blk, int ac0, u_char* out, int stride);
void bv_rdct2(int dc, short* blk, int ac0,
	      u_char* in, u_char* out, int stride);
void bv_rdct3(int dc, short* blk, int ac0, int ac1,
	      u_char* in, u_char* out, int stride);

#ifdef INT_64
void rdct(short* coef, INT_64 mask, u_char* out, int stride, const int* qt);
void rdct(short* coef, INT_64 mask, u_char* out, int stride, const u_char* in);
#else
void rdct(short* coef, u_int m0, u_int m1, u_char* out, 
	  int stride, const int* qt);
void rdct(short* coef, u_int m0, u_int m1, u_char* out,
	  int stride, const u_char* in);
#endif

void dcfill(int dc, u_char* out, int stride);
void dcsum(int dc, u_char* in, u_char* out, int stride);
void dcsum2(int dc, u_char* in, u_char* out, int stride);
void dct_decimate(const short* in0, const short* in1, short* out);

/*XXX*/
void rdct_fold_q(const int* in, int* qt);
void fdct_fold_q(const int* in, float* qt);

extern const u_char ROWZAG[];
extern const u_char COLZAG[];

/*XXX*/
extern void j_fwd_dct (short* data);
