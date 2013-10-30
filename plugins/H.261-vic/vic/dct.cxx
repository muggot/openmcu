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
 *	This product includes software developed by the Network Research
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
 */

/************ Change log
 *
 * $Log: dct.cxx,v $
 * Revision 1.1  2010/02/24 02:19:22  shorne
 * First commit of h323plus mirror
 *
 * Revision 1.3  2006/08/10 07:05:46  csoutheren
 * Fixed compile warnings on VC 2005
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
 * Revision 1.14  2003/03/14 07:25:55  robertj
 * Removed $header keyword so is not different on alternate repositories
 *
 * Revision 1.13  2002/10/24 21:05:26  dereks
 * Fix compile time warning.
 *
 * Revision 1.12  2002/05/17 01:47:33  dereks
 * backout the integer maths in the h261 codec.
 *
 * Revision 1.11  2002/02/15 03:54:31  yurik
 * Warnings removed during compilation, patch courtesy of Jehan Bing, jehan@bravobrava.com
 *
 * Revision 1.10  2001/10/24 20:24:32  dereks
 * Remove green stripes under windows for INT_64. Thanks to Robert Lupa.
 *
 * Revision 1.9  2001/10/17 03:52:39  robertj
 * Fixed MSVC compatibility
 *
 * Revision 1.8  2001/10/17 01:54:36  yurik
 * Fixed clash with CE includes for INT32 type
 *
 * Revision 1.7  2001/10/16 23:51:42  dereks
 * Change vic's fdct() from floating-point to fix-point. Improves performance
 * for h261 video significantly on some machines. Thanks to Cosmos Jiang
 *
 * Revision 1.6  2001/10/16 21:20:07  yurik
 * Removed warnings on Windows CE. Submitted by Jehan Bing, jehan@bravobrava.com
 *
 * Revision 1.3  2000/12/19 22:22:34  dereks
 * Remove connection to grabber-OS.cxx files. grabber-OS.cxx files no longer used.
 * Video data is now read from a video channel, using the pwlib classes.
 *
 * Revision 1.2  2000/08/25 03:18:49  dereks
 * Add change log facility (Thanks Robert for the info on implementation)
 *
 *
 *
 ********/

#include "bsd-endian.h"
#include "dct.h"

/*
 * Macros for fix-point (integer) arithmetic.  FP_NBITS gives the number
 * of binary digits past the decimal point.  FP_MUL computes the product
 * of two fixed point numbers.  A fixed point number and an integer
 * can be directly multiplied to give a fixed point number.  FP_SCALE
 * converts a floating point number to fixed point (and is used only
 * at startup, not by the dct engine).  FP_NORM converts a fixed
 * point number to scalar by rounding to the closest integer.
 * FP_JNORM is similar except it folds the jpeg bias of 128 into the
 * rounding addition.
 */
#define FP_NBITS 15
#define FP_MUL(a, b)	((((a) >> 5) * ((b) >> 5)) >> (FP_NBITS - 10))
#define FP_SCALE(v)	(int)((double)(v) * double(1 << FP_NBITS) + 0.5)
#define FP_NORM(v)	(((v) + (1 << (FP_NBITS-1))) >> FP_NBITS)
#define FP_JNORM(v)	(((v) + (257 << (FP_NBITS-1))) >> FP_NBITS)

#define M(n) ((m0 >> (n)) & 1)

/*
 * This macro stolen from nv.
 */
/* Sick little macro which will limit x to [0..255] with logical ops */
#define LIMIT8(x, t) ((t = (x)), (t &= ~(t>>31)), (t | ~((t-256) >> 31)))
#define LIMIT(x, t) (LIMIT8((x), t) & 0xff)

/* row order */
const u_char ROWZAG[] = {
	0,  1,  8, 16,  9,  2,  3, 10,
	17, 24, 32, 25, 18, 11,  4,  5,
	12, 19, 26, 33, 40, 48, 41, 34,
	27, 20, 13,  6,  7, 14, 21, 28,
	35, 42, 49, 56, 57, 50, 43, 36,
	29, 22, 15, 23, 30, 37, 44, 51,
	58, 59, 52, 45, 38, 31, 39, 46,
	53, 60, 61, 54, 47, 55, 62, 63,
	0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0
};
/* column order */
const u_char COLZAG[] = {
	0, 8, 1, 2, 9, 16, 24, 17,
	10, 3, 4, 11, 18, 25, 32, 40,
	33, 26, 19, 12, 5, 6, 13, 20,
	27, 34, 41, 48, 56, 49, 42, 35,
	28, 21, 14, 7, 15, 22, 29, 36,
	43, 50, 57, 58, 51, 44, 37, 30,
	23, 31, 38, 45, 52, 59, 60, 53,
	46, 39, 47, 54, 61, 62, 55, 63,
	0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0
};

#define A1 FP_SCALE(0.7071068)
#define A2 FP_SCALE(0.5411961)
#define A3 A1
#define A4 FP_SCALE(1.3065630)
#define A5 FP_SCALE(0.3826834)

#define FA1 (0.707106781f)
#define FA2 (0.541196100f)
#define FA3 FA1
#define FA4 (1.306562965f)
#define FA5 (0.382683433f)

#ifdef B0
#undef B0
#endif
/*
 * these magic numbers are scaling factors for each coef of the 1-d
 * AA&N DCT.  The scale factor for coef 0 is 1 and coef 1<=n<=7 is
 * cos(n*PI/16)*sqrt(2).  There is also a normalization of sqrt(8).
 * Formally you divide by the scale factor but we multiply by the
 * inverse because it's faster.  So the numbers below are the inverse
 * of what was just described.
 */
#define B0 0.35355339059327376220
#define B1 0.25489778955207958447
#define B2 0.27059805007309849220
#define B3 0.30067244346752264027
#define B4 0.35355339059327376220
#define B5 0.44998811156820785231
#define B6 0.65328148243818826392
#define B7 1.28145772387075308943

/*
 * Output multipliers for AA&N DCT
 * (i.e., first stage multipliers for inverse DCT).
 */
static const double first_stage[8] = { B0, B1, B2, B3, B4, B5, B6, B7, };

/*
 * The first_stage array crossed with itself.  This allows us
 * to embed the first stage multipliers of the row pass by
 * computing scaled versions of the columns.
 */
static const int cross_stage[64] = {
	FP_SCALE(B0 * B0),
	FP_SCALE(B0 * B1),
	FP_SCALE(B0 * B2),
	FP_SCALE(B0 * B3),
	FP_SCALE(B0 * B4),
	FP_SCALE(B0 * B5),
	FP_SCALE(B0 * B6),
	FP_SCALE(B0 * B7),

	FP_SCALE(B1 * B0),
	FP_SCALE(B1 * B1),
	FP_SCALE(B1 * B2),
	FP_SCALE(B1 * B3),
	FP_SCALE(B1 * B4),
	FP_SCALE(B1 * B5),
	FP_SCALE(B1 * B6),
	FP_SCALE(B1 * B7),

	FP_SCALE(B2 * B0),
	FP_SCALE(B2 * B1),
	FP_SCALE(B2 * B2),
	FP_SCALE(B2 * B3),
	FP_SCALE(B2 * B4),
	FP_SCALE(B2 * B5),
	FP_SCALE(B2 * B6),
	FP_SCALE(B2 * B7),

	FP_SCALE(B3 * B0),
	FP_SCALE(B3 * B1),
	FP_SCALE(B3 * B2),
	FP_SCALE(B3 * B3),
	FP_SCALE(B3 * B4),
	FP_SCALE(B3 * B5),
	FP_SCALE(B3 * B6),
	FP_SCALE(B3 * B7),

	FP_SCALE(B4 * B0),
	FP_SCALE(B4 * B1),
	FP_SCALE(B4 * B2),
	FP_SCALE(B4 * B3),
	FP_SCALE(B4 * B4),
	FP_SCALE(B4 * B5),
	FP_SCALE(B4 * B6),
	FP_SCALE(B4 * B7),

	FP_SCALE(B5 * B0),
	FP_SCALE(B5 * B1),
	FP_SCALE(B5 * B2),
	FP_SCALE(B5 * B3),
	FP_SCALE(B5 * B4),
	FP_SCALE(B5 * B5),
	FP_SCALE(B5 * B6),
	FP_SCALE(B5 * B7),

	FP_SCALE(B6 * B0),
	FP_SCALE(B6 * B1),
	FP_SCALE(B6 * B2),
	FP_SCALE(B6 * B3),
	FP_SCALE(B6 * B4),
	FP_SCALE(B6 * B5),
	FP_SCALE(B6 * B6),
	FP_SCALE(B6 * B7),

	FP_SCALE(B7 * B0),
	FP_SCALE(B7 * B1),
	FP_SCALE(B7 * B2),
	FP_SCALE(B7 * B3),
	FP_SCALE(B7 * B4),
	FP_SCALE(B7 * B5),
	FP_SCALE(B7 * B6),
	FP_SCALE(B7 * B7),
};
static const float f_cross_stage[64] = {
	(float)(B0 * B0),
	(float)(B0 * B1),
	(float)(B0 * B2),
	(float)(B0 * B3),
	(float)(B0 * B4),
	(float)(B0 * B5),
	(float)(B0 * B6),
	(float)(B0 * B7),

	(float)(B1 * B0),
	(float)(B1 * B1),
	(float)(B1 * B2),
	(float)(B1 * B3),
	(float)(B1 * B4),
	(float)(B1 * B5),
	(float)(B1 * B6),
	(float)(B1 * B7),

	(float)(B2 * B0),
	(float)(B2 * B1),
	(float)(B2 * B2),
	(float)(B2 * B3),
	(float)(B2 * B4),
	(float)(B2 * B5),
	(float)(B2 * B6),
	(float)(B2 * B7),

	(float)(B3 * B0),
	(float)(B3 * B1),
	(float)(B3 * B2),
	(float)(B3 * B3),
	(float)(B3 * B4),
	(float)(B3 * B5),
	(float)(B3 * B6),
	(float)(B3 * B7),

	(float)(B4 * B0),
	(float)(B4 * B1),
	(float)(B4 * B2),
	(float)(B4 * B3),
	(float)(B4 * B4),
	(float)(B4 * B5),
	(float)(B4 * B6),
	(float)(B4 * B7),

	(float)(B5 * B0),
	(float)(B5 * B1),
	(float)(B5 * B2),
	(float)(B5 * B3),
	(float)(B5 * B4),
	(float)(B5 * B5),
	(float)(B5 * B6),
	(float)(B5 * B7),

	(float)(B6 * B0),
	(float)(B6 * B1),
	(float)(B6 * B2),
	(float)(B6 * B3),
	(float)(B6 * B4),
	(float)(B6 * B5),
	(float)(B6 * B6),
	(float)(B6 * B7),

	(float)(B7 * B0),
	(float)(B7 * B1),
	(float)(B7 * B2),
	(float)(B7 * B3),
	(float)(B7 * B4),
	(float)(B7 * B5),
	(float)(B7 * B6),
	(float)(B7 * B7),
};

/*
 * Map a quantization table in natural, row-order,
 * into the qt input expected by rdct().
 */
void
rdct_fold_q(const int* in, int* out)
{
	for (int i = 0; i < 64; ++i) {
		/*
		 * Fold column and row passes of the dct.
		 * By scaling each column DCT independently,
		 * we pre-bias all the row DCT's so the
		 * first multiplier is already embedded
		 * in the temporary result.  Thanks to
		 * Martin Vetterli for explaining how
		 * to do this.
		 */
		double v = double(in[i]);
		v *= first_stage[i & 7];
		v *= first_stage[i >> 3];
		out[i] = FP_SCALE(v);
	}
}

/*
 * Just like rdct_fold_q() but we divide by the quantizer.
 */
void fdct_fold_q(const int* in, float* out)
{
	for (int i = 0; i < 64; ++i) {
		double v = first_stage[i >> 3];
		v *= first_stage[i & 7];
		double q = double(in[i]);
		out[i] = (float)(v / q);
	}
}

void dcsum(int dc, u_char* in, u_char* out, int stride)
{
	for (int k = 8; --k >= 0; ) {
		int t;
#ifdef INT_64
		/*XXX assume little-endian */
		INT_64 i = *(INT_64*)in;
		INT_64 o = (INT_64)LIMIT(dc + (int)(i >> 56 & 0xff), t) << 56;
		o |=  (INT_64)LIMIT(dc + (int)(i >> 48 & 0xff), t) << 48;
		o |=  (INT_64)LIMIT(dc + (int)(i >> 40 & 0xff), t) << 40;
		o |=  (INT_64)LIMIT(dc + (int)(i >> 32 & 0xff), t) << 32;
		o |=  (INT_64)LIMIT(dc + (int)(i >> 24 & 0xff), t) << 24;
		o |=  (INT_64)LIMIT(dc + (int)(i >> 16 & 0xff), t) << 16;
		o |=  (INT_64)LIMIT(dc + (int)(i >> 8 & 0xff), t) << 8;
		o |=  (INT_64)LIMIT(dc + (int)(i & 0xff), t);
		*(INT_64*)out = o;
#else
		u_int o = 0;
		u_int i = *(u_int*)in;
		SPLICE(o, LIMIT(dc + EXTRACT(i, 24), t), 24);
		SPLICE(o, LIMIT(dc + EXTRACT(i, 16), t), 16);
		SPLICE(o, LIMIT(dc + EXTRACT(i, 8), t), 8);
		SPLICE(o, LIMIT(dc + EXTRACT(i, 0), t), 0);
		*(u_int*)out = o;

		o = 0;
		i = *(u_int*)(in + 4);
		SPLICE(o, LIMIT(dc + EXTRACT(i, 24),  t), 24);
		SPLICE(o, LIMIT(dc + EXTRACT(i, 16), t), 16);
		SPLICE(o, LIMIT(dc + EXTRACT(i, 8), t), 8);
		SPLICE(o, LIMIT(dc + EXTRACT(i, 0), t), 0);
		*(u_int*)(out + 4) = o;
#endif
		in += stride;
		out += stride;
	}
}

void dcsum2(int dc, u_char* in, u_char* out, int stride)
{
	for (int k = 8; --k >= 0; ) {
		int t;
		u_int o = 0;
		SPLICE(o, LIMIT(dc + in[0], t), 24);
		SPLICE(o, LIMIT(dc + in[1], t), 16);
		SPLICE(o, LIMIT(dc + in[2], t), 8);
		SPLICE(o, LIMIT(dc + in[3], t), 0);
		*(u_int*)out = o;

		o = 0;
		SPLICE(o, LIMIT(dc + in[4], t), 24);
		SPLICE(o, LIMIT(dc + in[5], t), 16);
		SPLICE(o, LIMIT(dc + in[6], t), 8);
		SPLICE(o, LIMIT(dc + in[7], t), 0);
		*(u_int*)(out + 4) = o;

		in += stride;
		out += stride;
	}
}

void dcfill(int DC, u_char* out, int stride)
{
	int t;
	u_int dc = DC;
	dc = LIMIT(dc, t);
	dc |= dc << 8;
	dc |= dc << 16;
#ifdef INT_64
	INT_64 xdc = dc;
	xdc |= xdc << 32;
	*(INT_64 *)out = xdc;
	out += stride;
	*(INT_64 *)out = xdc;
	out += stride;
	*(INT_64 *)out = xdc;
	out += stride;
	*(INT_64 *)out = xdc;
	out += stride;
	*(INT_64 *)out = xdc;
	out += stride;
	*(INT_64 *)out = xdc;
	out += stride;
	*(INT_64 *)out = xdc;
	out += stride;
	*(INT_64 *)out = xdc;
#else
	*(u_int*)out = dc;
	*(u_int*)(out + 4) = dc;
	out += stride;
	*(u_int*)out = dc;
	*(u_int*)(out + 4) = dc;
	out += stride;
	*(u_int*)out = dc;
	*(u_int*)(out + 4) = dc;
	out += stride;
	*(u_int*)out = dc;
	*(u_int*)(out + 4) = dc;
	out += stride;
	*(u_int*)out = dc;
	*(u_int*)(out + 4) = dc;
	out += stride;
	*(u_int*)out = dc;
	*(u_int*)(out + 4) = dc;
	out += stride;
	*(u_int*)out = dc;
	*(u_int*)(out + 4) = dc;
	out += stride;
	*(u_int*)out = dc;
	*(u_int*)(out + 4) = dc;
#endif
}

/*
 * This routine mixes the DC & AC components of an 8x8 block of
 * pixels.  This routine is called for every block decoded so it
 * needs to be efficient.  It tries to do as many pixels in parallel
 * as will fit in a word.  The one complication is that it has to
 * deal with overflow (sum > 255) and underflow (sum < 0).  Underflow
 * & overflow are only possible if both terms have the same sign and
 * are indicated by the result having a different sign than the terms.
 * Note that underflow is more worrisome than overflow since it results
 * in bright white dots in a black field.
 * The DC term and sum are biased by 128 so a negative number has the
 * 2^7 bit = 0.  The AC term is not biased so a negative number has
 * the 2^7 bit = 1.  So underflow is indicated by (DC & AC & sum) != 0;
 */
#define MIX_LOGIC(sum, a, b, omask, uflo) \
{ \
	sum = a + b; \
	uflo = (a ^ b) & (a ^ sum) & omask; \
	if (uflo) { \
		if ((b = uflo & a) != 0) { \
			/* integer overflows */ \
			b |= b >> 1; \
			b |= b >> 2; \
			b |= b >> 4; \
			sum |= b; \
		} \
		if ((uflo &=~ b) != 0) { \
			/* integer underflow(s) */ \
			uflo |= uflo >> 1; \
			uflo |= uflo >> 2; \
			uflo |= uflo >> 4; \
			sum &= ~uflo; \
		} \
	} \
}
/*
 * Table of products of 8-bit scaled coefficients
 * and idct coefficients (there are only 33 unique
 * coefficients so we index via a compact ID).
 */
extern "C" u_char multab[];
/*
 * Array of coefficient ID's used to index multab.
 */
extern "C" u_int dct_basis[64][64 / sizeof(u_int)];

/*XXX*/
#define LIMIT_512(s) ((s) > 511 ? 511 : (s) < -512 ? -512 : (s))

void
bv_rdct1(int dc, short* bp, int acx, u_char* out, int stride)
{
	u_int omask = 0x80808080;
	u_int uflo;
	u_int* vp = dct_basis[acx];
	int s = LIMIT_512(bp[acx]);
	s = (s >> 2) & 0xff;
	/* 66 unique coefficients require 7 bits */
	char* mt = (char*)&multab[s << 7];

	dc |= dc << 8;
	dc |= dc << 16;
	for (int k = 8; --k >= 0; ) {
		u_int v = *vp++;
		u_int m = mt[v >> 24] << SHIFT(24) |
			mt[v >> 16 & 0xff] << SHIFT(16) |
			mt[v >> 8 & 0xff] << SHIFT(8) |
			mt[v & 0xff] << SHIFT(0);
		MIX_LOGIC(v, dc, m, omask, uflo);
		*(u_int*)out = v;
		v = *vp++;
		m = mt[v >> 24] << SHIFT(24) |
			mt[v >> 16 & 0xff] << SHIFT(16) |
			mt[v >> 8 & 0xff] << SHIFT(8) |
			mt[v & 0xff] << SHIFT(0);
		MIX_LOGIC(v, dc, m, omask, uflo);
		*(u_int*)(out + 4) = v;
		out += stride;
	}
}

/* XXX this version has to be exact */
void
bv_rdct2(int dc, short* bp, int ac0, u_char* in, u_char* out, int stride)
{
	int s0 = LIMIT_512(bp[ac0]);
	s0 = (s0 >> 2) & 0xff;
	/* 66 unique coefficients require 7 bits */
	const char* mt = (const char*)&multab[s0 << 7];
	const u_int* vp0 = dct_basis[ac0];

	dc |= dc << 8;
	dc |= dc << 16;
	u_int omask = 0x80808080;
	u_int uflo;
	for (int k = 8; --k >= 0; ) {
		u_int v, m, i;
		v = *vp0++;
		m = mt[v >> 24] << SHIFT(24) | mt[v >> 16 & 0xff] << SHIFT(16) |
		    mt[v >> 8 & 0xff] << SHIFT(8) | mt[v & 0xff] << SHIFT(0);
		MIX_LOGIC(v, dc, m, omask, uflo);
		i = in[0] << SHIFT(24) | in[1] << SHIFT(16) |
		    in[2] << SHIFT(8) | in[3] << SHIFT(0);
		MIX_LOGIC(m, i, v, omask, uflo);
		*(u_int*)out = m;

		v = *vp0++;
		m = mt[v >> 24] << SHIFT(24) | mt[v >> 16 & 0xff] << SHIFT(16) |
		    mt[v >> 8 & 0xff] << SHIFT(8) | mt[v & 0xff] << SHIFT(0);
		MIX_LOGIC(v, dc, m, omask, uflo);
		i = in[4] << SHIFT(24) | in[5] << SHIFT(16) |
		    in[6] << SHIFT(8) | in[7] << SHIFT(0);
		MIX_LOGIC(m, i, v, omask, uflo);
		*(u_int*)(out + 4) = m;
		out += stride;
		in += stride;
	}
}

/* XXX this version has to be exact */
void
bv_rdct3(int dc, short* bp, int ac0, int ac1, u_char* in, u_char* out, int stride)
{
	int s0 = LIMIT_512(bp[ac0]);
	s0 = (s0 >> 2) & 0xff;
	/* 66 unique coefficients require 7 bits */
	char* mt0 = (char*)&multab[s0 << 7];

	int s1 = LIMIT_512(bp[ac1]);
	s1 = (s1 >> 2) & 0xff;
	char* mt1 = (char*)&multab[s1 << 7];

	u_int* vp0 = dct_basis[ac0];
	u_int* vp1 = dct_basis[ac1];

	for (int k = 8; --k >= 0; ) {
		int t;
		u_int v0 = *vp0++;
		u_int v1 = *vp1++;
		s0 = mt0[v0 >> 24] + mt1[v1 >> 24] + in[0] + dc;
		u_int m = LIMIT(s0, t) << SHIFT(24);
		s0 = mt0[v0 >> 16 & 0xff] + mt1[v1 >> 16 & 0xff] + in[1] + dc;
		m |= LIMIT(s0, t) << SHIFT(16);
		s0 = mt0[v0 >> 8 & 0xff] + mt1[v1 >> 8 & 0xff] + in[2] + dc;
		m |= LIMIT(s0, t) << SHIFT(8);
		s0 = mt0[v0 & 0xff] + mt1[v1 & 0xff] + in[3] + dc;
		m |= LIMIT(s0, t) << SHIFT(0);
		*(u_int*)out = m;

		v0 = *vp0++;
		v1 = *vp1++;
		s0 = mt0[v0 >> 24] + mt1[v1 >> 24] + in[4] + dc;
		m = 0;
		m |= LIMIT(s0, t) << SHIFT(24);
		s0 = mt0[v0 >> 16 & 0xff] + mt1[v1 >> 16 & 0xff] + in[5] + dc;
		m |= LIMIT(s0, t) << SHIFT(16);
		s0 = mt0[v0 >> 8 & 0xff] + mt1[v1 >> 8 & 0xff] + in[6] + dc;
		m |= LIMIT(s0, t) << SHIFT(8);
		s0 = mt0[v0 & 0xff] + mt1[v1 & 0xff] + in[7] + dc;
		m |= LIMIT(s0, t) << SHIFT(0);
		*(u_int*)(out + 4) = m;

		out += stride;
		in += stride;
	}
}

#ifdef INT_64
/*XXX assume little-endian */
#define PSPLICE(v, n) pix |= (INT_64)(v) << ((n)*8)
#define DID4PIX 
#define PSTORE ((INT_64*)p)[0] = pix
#define PIXDEF INT_64 pix = 0; int v, oflo = 0
#else
#define PSPLICE(v, n) SPLICE(pix, (v), (3 - ((n)&3)) * 8)
#define DID4PIX pix0 = pix; pix = 0
#define PSTORE ((u_int*)p)[0] = pix0; ((u_int*)p)[1] = pix
#define PIXDEF	u_int pix0, pix = 0; int v, oflo = 0
#endif
#define DOJPIX(val, n) v = FP_JNORM(val); oflo |= v; PSPLICE(v, n)
#define DOJPIXLIMIT(val, n) PSPLICE(LIMIT(FP_JNORM(val),t), n)
#define DOPIX(val, n) v = FP_NORM(val); oflo |= v; PSPLICE(v, n)
#define DOPIXLIMIT(val, n) PSPLICE(LIMIT(FP_NORM(val),t), n)
#define DOPIXIN(val, n) v = FP_NORM(val) + in[n]; oflo |= v; PSPLICE(v, n)
#define DOPIXINLIMIT(val, n) PSPLICE(LIMIT(FP_NORM(val) + in[n], t), n)

/*
 * A 2D Inverse DCT based on a column-row decomposition using
 * Arai, Agui, and Nakajmia's 8pt 1D Inverse DCT, from Fig. 4-8
 * Pennebaker & Mitchell (i.e., the pink JPEG book).  This figure
 * is the forward transform; reverse the flowgraph for the inverse
 * (you need to draw a picture).  The outputs are DFT coefficients
 * and need to be scaled according to Eq [4-3].
 *
 * The input coefficients are, counter to tradition, in column-order.
 * The bit mask indicates which coefficients are non-zero.  If the
 * corresponding bit is zero, then the coefficient is assumed zero
 * and the input coefficient is not referenced and need not be defined.
 * The 8-bit outputs are computed in row order and placed in the
 * output array pointed to by p, with each of the eight 8-byte lines
 * offset by "stride" bytes.
 *
 * qt is the inverse quantization table in column order.  These
 * coefficients are the product of the inverse quantization factor,
 * specified by the jpeg quantization table, and the first multiplier
 * in the inverse DCT flow graph.  This first multiplier is actually
 * biased by the row term so that the columns are pre-scaled to
 * eliminate the first row multiplication stage.
 *
 * The output is biased by 128, i.e., [-128,127] is mapped to [0,255],
 * which is relevant to jpeg.
 */
void
#ifdef INT_64
rdct(register short *bp, INT_64 m0, u_char* p, int stride, const int* qt)
#else
rdct(register short *bp, u_int m0, u_int m1, u_char* p,
     int stride, const int* qt)
#endif
{
	/*
	 * First pass is 1D transform over the columns.  Note that
	 * the coefficients are stored in column order, so even
	 * though we're accessing the columns, we access them
	 * in a row-like fashion.  We use a column-row decomposition
	 * instead of a row-column decomposition so we can splice
	 * pixels in an efficient, row-wise order in the second
	 * stage.
	 *
	 * The inverse quantization is folded together with the
	 * first stage multiplier.  This reduces the number of
	 * multiplies (per 8-pt transform) from 11 to 5 (ignoring
	 * the inverse quantization which must be done anyway).
	 *
	 * Because we compute appropriately scaled column DCTs,
	 * the row DCTs require only 5 multiplies per row total.
	 * So the total number of multiplies for the 8x8 DCT is 80
	 * (ignoring the checks for zero coefficients).
	 */
	int tmp[64];
	int* tp = tmp;
	int i;
	for (i = 8; --i >= 0; ) {
		if ((m0 & 0xfe) == 0) {
			/* AC terms all zero */
			int v = M(0) ? qt[0] * bp[0] : 0;
			tp[0] = v;
			tp[1] = v;
			tp[2] = v;
			tp[3] = v;
			tp[4] = v;
			tp[5] = v;
			tp[6] = v;
			tp[7] = v;
		} else {
			int t0, t1, t2, t3, t4, t5, t6, t7;

			if ((m0 & 0xaa) == 0)
				t4 = t5 = t6 = t7 = 0;
			else {
				t0 = M(5) ? qt[5] * bp[5] : 0;
				t2 = M(1) ? qt[1] * bp[1] : 0;
				t6 = M(7) ? qt[7] * bp[7] : 0;
				t7 = M(3) ? qt[3] * bp[3] : 0;

				t4 = t0 - t7;
				t1 = t2 + t6;
				t6 = t2 - t6;
				t7 += t0;

				t5 = t1 - t7;
				t7 += t1;

				t2 = FP_MUL(-A5, t4 + t6);
				t4 = FP_MUL(-A2, t4);
				t0 = t4 + t2;
				t1 = FP_MUL(A3, t5);
				t2 += FP_MUL(A4, t6);
			
				t4 = -t0;
				t5 = t1 - t0;
				t6 = t1 + t2;
				t7 += t2;
			}

#ifdef notdef
			if ((m0 & 0x55) == 0)
				t0 = t1 = t2 = t3 = 0;
			else {
#endif
				t0 = M(0) ? qt[0] * bp[0] : 0;
				t1 = M(4) ? qt[4] * bp[4] : 0;
				int x0 = t0 + t1;
				int x1 = t0 - t1;

				t0 = M(2) ? qt[2] * bp[2] : 0;
				t3 = M(6) ? qt[6] * bp[6] : 0;
				t2 = t0 - t3;
				t3 += t0;

				t2 = FP_MUL(A1, t2);
				t3 += t2;

				t0 = x0 + t3;
				t1 = x1 + t2;
				t2 = x1 - t2;
				t3 = x0 - t3;
#ifdef notdef
			}
#endif

			tp[0] = t0 + t7;
			tp[7] = t0 - t7;
			tp[1] = t1 + t6;
			tp[6] = t1 - t6;
			tp[2] = t2 + t5;
			tp[5] = t2 - t5;
			tp[3] = t3 + t4;
			tp[4] = t3 - t4;
		}
		tp += 8;
		bp += 8;
		qt += 8;

		m0 >>= 8;
#ifndef INT_64
		m0 |= m1 << 24;
		m1 >>= 8;
#endif
	}
	tp -= 64;
	/*
	 * Second pass is 1D transform over the rows.  Note that
	 * the coefficients are stored in column order, so even
	 * though we're accessing the rows, we access them
	 * in a column-like fashion.
	 *
	 * The pass above already computed the first multiplier
	 * in the flow graph.
	 */
	for (i = 8; --i >= 0; ) {
		int t0, t1, t2, t3, t4, t5, t6, t7;

		t0 = tp[8*5];
		t2 = tp[8*1];
		t6 = tp[8*7];
		t7 = tp[8*3];

#ifdef notdef
		if ((t0 | t2 | t6 | t7) == 0) {
			t4 = t5 = 0;
		} else 
#endif
{
			t4 = t0 - t7;
			t1 = t2 + t6;
			t6 = t2 - t6;
			t7 += t0;

			t5 = t1 - t7;
			t7 += t1;

			t2 = FP_MUL(-A5, t4 + t6);
			t4 = FP_MUL(-A2, t4);
			t0 = t4 + t2;
			t1 = FP_MUL(A3, t5);
			t2 += FP_MUL(A4, t6);
			
			t4 = -t0;
			t5 = t1 - t0;
			t6 = t1 + t2;
			t7 += t2;
		}

		t0 = tp[8*0];
		t1 = tp[8*4];
		int x0 = t0 + t1;
		int x1 = t0 - t1;

		t0 = tp[8*2];
		t3 = tp[8*6];
		t2 = t0 - t3;
		t3 += t0;

		t2 = FP_MUL(A1, t2);
		t3 += t2;

		t0 = x0 + t3;
		t1 = x1 + t2;
		t2 = x1 - t2;
		t3 = x0 - t3;

		PIXDEF;
		DOJPIX(t0 + t7, 0);
		DOJPIX(t1 + t6, 1);
		DOJPIX(t2 + t5, 2);
		DOJPIX(t3 + t4, 3);
		DID4PIX;
		DOJPIX(t3 - t4, 4);
		DOJPIX(t2 - t5, 5);
		DOJPIX(t1 - t6, 6);
		DOJPIX(t0 - t7, 7);
		if (oflo & ~0xff) {
			int t;
			pix = 0;
			DOJPIXLIMIT(t0 + t7, 0);
			DOJPIXLIMIT(t1 + t6, 1);
			DOJPIXLIMIT(t2 + t5, 2);
			DOJPIXLIMIT(t3 + t4, 3);
			DID4PIX;
			DOJPIXLIMIT(t3 - t4, 4);
			DOJPIXLIMIT(t2 - t5, 5);
			DOJPIXLIMIT(t1 - t6, 6);
			DOJPIXLIMIT(t0 - t7, 7);
		}
		PSTORE;

		++tp;
		p += stride;
	}
}

/*
 * Inverse 2-D transform, similar to routine above (see comment above),
 * but more appropriate for H.261 instead of JPEG.  This routine does
 * not bias the output by 128, and has an additional argument which is
 * an input array which gets summed together with the inverse-transform.
 * For example, this allows motion-compensation to be folded in here,
 * saving an extra traversal of the block.  The input pointer can be
 * null, if motion-compensation is not needed.
 *
 * This routine does not take a quantization table, since the H.261
 * inverse quantizer is easily implemented via table lookup in the decoder.
 */
void
#ifdef INT_64
rdct(register short *bp, INT_64 m0, u_char* p, int stride, const u_char* in)
#else
rdct(register short *bp, u_int m0, u_int m1, u_char* p, int stride, const u_char *in)
#endif
{
	int tmp[64];
	int* tp = tmp;
	const int* qt = cross_stage;
	/*
	 * First pass is 1D transform over the rows of the input array.
	 */
	int i;
	for (i = 8; --i >= 0; ) {
		if ((m0 & 0xfe) == 0) {
			/*
			 * All ac terms are zero.
			 */
			int v = 0;
			if (M(0))
				v = qt[0] * bp[0];
			tp[0] = v;
			tp[1] = v;
			tp[2] = v;
			tp[3] = v;
			tp[4] = v;
			tp[5] = v;
			tp[6] = v;
			tp[7] = v;
		} else {
			int t4 = 0, t5 = 0, t6 = 0, t7 = 0;
			if (m0 & 0xaa) {
				/* odd part */
				if (M(1))
					t4 = qt[1] * bp[1];
				if (M(3))
					t5 = qt[3] * bp[3];
				if (M(5))
					t6 = qt[5] * bp[5];
				if (M(7))
					t7 = qt[7] * bp[7];

				int x0 = t6 - t5;
				t6 += t5;
				int x1 = t4 - t7;
				t7 += t4;

				t5 = FP_MUL(t7 - t6, A3);
				t7 += t6;

				t4 = FP_MUL(x1 + x0, A5);
				t6 = FP_MUL(x1, A4) - t4;
				t4 += FP_MUL(x0, A2);

				t7 += t6;
				t6 += t5;
				t5 += t4;
			}
			int t0 = 0, t1 = 0, t2 = 0, t3 = 0;
			if (m0 & 0x55) {
				/* even part */
				if (M(0))
					t0 = qt[0] * bp[0];
				if (M(2))
					t1 = qt[2] * bp[2];
				if (M(4))
					t2 = qt[4] * bp[4];
				if (M(6))
					t3 = qt[6] * bp[6];

				int x0 = FP_MUL(t1 - t3, A1);
				t3 += t1;
				t1 = t0 - t2;
				t0 += t2;
				t2 = t3 + x0;
				t3 = t0 - t2;
				t0 += t2;
				t2 = t1 - x0;
				t1 += x0;
			}
			tp[0] = t0 + t7;
			tp[1] = t1 + t6;
			tp[2] = t2 + t5;
			tp[3] = t3 + t4;
			tp[4] = t3 - t4;
			tp[5] = t2 - t5;
			tp[6] = t1 - t6;
			tp[7] = t0 - t7;
		}
		qt += 8;
		tp += 8;
		bp += 8;
		m0 >>= 8;
#ifndef INT_64
		m0 |= m1 << 24;
		m1 >>= 8;
#endif
	}
	tp -= 64;
	/*
	 * Second pass is 1D transform over the rows of the temp array.
	 */
	for (i = 8; --i >= 0; ) {
		int t4 = tp[8*1];
		int t5 = tp[8*3];
		int t6 = tp[8*5];
		int t7 = tp[8*7];
		if ((t4|t5|t6|t7) != 0) {
			/* odd part */
			int x0 = t6 - t5;
			t6 += t5;
			int x1 = t4 - t7;
			t7 += t4;

			t5 = FP_MUL(t7 - t6, A3);
			t7 += t6;

			t4 = FP_MUL(x1 + x0, A5);
			t6 = FP_MUL(x1, A4) - t4;
			t4 += FP_MUL(x0, A2);

			t7 += t6;
			t6 += t5;
			t5 += t4;
		}
		int t0 = tp[8*0];
		int t1 = tp[8*2];
		int t2 = tp[8*4];
		int t3 = tp[8*6];
		if ((t0|t1|t2|t3) != 0) {
			/* even part */
			int x0 = FP_MUL(t1 - t3, A1);
			t3 += t1;
			t1 = t0 - t2;
			t0 += t2;
			t2 = t3 + x0;
			t3 = t0 - t2;
			t0 += t2;
			t2 = t1 - x0;
			t1 += x0;
		}
		if (in != 0) {
			PIXDEF;
			DOPIXIN(t0 + t7, 0);
			DOPIXIN(t1 + t6, 1);
			DOPIXIN(t2 + t5, 2);
			DOPIXIN(t3 + t4, 3);
			DID4PIX;
			DOPIXIN(t3 - t4, 4);
			DOPIXIN(t2 - t5, 5);
			DOPIXIN(t1 - t6, 6);
			DOPIXIN(t0 - t7, 7);
			if (oflo & ~0xff) {
				int t;
				pix = 0;
				DOPIXINLIMIT(t0 + t7, 0);
				DOPIXINLIMIT(t1 + t6, 1);
				DOPIXINLIMIT(t2 + t5, 2);
				DOPIXINLIMIT(t3 + t4, 3);
				DID4PIX;
				DOPIXINLIMIT(t3 - t4, 4);
				DOPIXINLIMIT(t2 - t5, 5);
				DOPIXINLIMIT(t1 - t6, 6);
				DOPIXINLIMIT(t0 - t7, 7);
			}
			PSTORE;
			in += stride;
		} else {
			PIXDEF;
			DOPIX(t0 + t7, 0);
			DOPIX(t1 + t6, 1);
			DOPIX(t2 + t5, 2);
			DOPIX(t3 + t4, 3);
			DID4PIX;
			DOPIX(t3 - t4, 4);
			DOPIX(t2 - t5, 5);
			DOPIX(t1 - t6, 6);
			DOPIX(t0 - t7, 7);
			if (oflo & ~0xff) {
				int t;
				pix = 0;
				DOPIXLIMIT(t0 + t7, 0);
				DOPIXLIMIT(t1 + t6, 1);
				DOPIXLIMIT(t2 + t5, 2);
				DOPIXLIMIT(t3 + t4, 3);
				DID4PIX;
				DOPIXLIMIT(t3 - t4, 4);
				DOPIXLIMIT(t2 - t5, 5);
				DOPIXLIMIT(t1 - t6, 6);
				DOPIXLIMIT(t0 - t7, 7);
			}
			PSTORE;
		}
		tp += 1;
		p += stride;
	}
}

/*
 * This macro does the combined descale-and-quantize
 * multiply.  It truncates rather than rounds to give
 * the behavior required for the h.261 deadband quantizer.
 */
 
#define FWD_DandQ(v, iq) short((v) * qt[iq])

void fdct(const u_char* in, int stride, short* out, const float* qt)
{
	float tmp[64];
	float* tp = tmp;

	int i;
	for (i = 8; --i >= 0; ) {
               float x0, x1, x2, x3, t0, t1, t2, t3, t4, t5, t6, t7;
               t0 = float(in[0] + in[7]);
               t7 = float(in[0] - in[7]);
               t1 = float(in[1] + in[6]);
               t6 = float(in[1] - in[6]);
               t2 = float(in[2] + in[5]);
               t5 = float(in[2] - in[5]);
               t3 = float(in[3] + in[4]);
               t4 = float(in[3] - in[4]);


		/* even part */
		x0 = t0 + t3;
		x2 = t1 + t2;
		tp[8*0] = x0 + x2;
		tp[8*4] = x0 - x2;
    
		x1 = t0 - t3;
		x3 = t1 - t2;
		t0 = (x1 + x3) * FA1;
		tp[8*2] = x1 + t0;
		tp[8*6] = x1 - t0;

		/* odd part */
		x0 = t4 + t5;
		x1 = t5 + t6;
		x2 = t6 + t7;

		t3 = x1 * FA1;
		t4 = t7 - t3;

		t0 = (x0 - x2) * FA5;
		t1 = x0 * FA2 + t0;
		tp[8*3] = t4 - t1;
		tp[8*5] = t4 + t1;

		t7 += t3;
		t2 = x2 * FA4 + t0;
		tp[8*1] = t7 + t2;
		tp[8*7] = t7 - t2;
		
		in += stride;
		tp += 1;
	}
	tp -= 8;

	for (i = 8; --i >= 0; ) {
		float x0, x1, x2, x3, t0, t1, t2, t3, t4, t5, t6, t7;
		t0 = tp[0] + tp[7];
		t7 = tp[0] - tp[7];
		t1 = tp[1] + tp[6];
		t6 = tp[1] - tp[6];
		t2 = tp[2] + tp[5];
		t5 = tp[2] - tp[5];
		t3 = tp[3] + tp[4];
		t4 = tp[3] - tp[4];

		/* even part */
		x0 = t0 + t3;
		x2 = t1 + t2;
		out[0] = FWD_DandQ(x0 + x2, 0);
		out[4] = FWD_DandQ(x0 - x2, 4);
    
		x1 = t0 - t3;
		x3 = t1 - t2;
		t0 = (x1 + x3) * FA1;
		out[2] = FWD_DandQ(x1 + t0, 2);
		out[6] = FWD_DandQ(x1 - t0, 6);

		/* odd part */
		x0 = t4 + t5;
		x1 = t5 + t6;
		x2 = t6 + t7;

		t3 = x1 * FA1;
		t4 = t7 - t3;

		t0 = (x0 - x2) * FA5;
		t1 =  x0 * FA2 + t0;
		out[3] = FWD_DandQ(t4 - t1, 3);
		out[5] = FWD_DandQ(t4 + t1, 5);

		t7 += t3;
		t2 = x2 * FA4 + t0;
		out[1] = FWD_DandQ(t7 + t2, 1);
		out[7] = FWD_DandQ(t7 - t2, 7);

		out += 8;
		tp += 8;
		qt += 8;
	}
}

/*
 * decimate the *rows* of the two input 8x8 DCT matrices into
 * a single output matrix.  we decimate rows rather than
 * columns even though we want column decimation because
 * the DCTs are stored in column order.
 */
void
dct_decimate(const short* in0, const short* in1, short* o)
{
	for (int k = 0; k < 8; ++k) {
		int x00 = in0[0];
		int x01 = in0[1];
		int x02 = in0[2];
		int x03 = in0[3];
		int x10 = in1[0];
		int x11 = in1[1];
		int x12 = in1[2];
		int x13 = in1[3];
#define X_N 4
#define X_5(v)  ((v) << (X_N - 1))
#define X_25(v)  ((v) << (X_N - 2))
#define X_125(v)  ((v) << (X_N - 3))
#define X_0625(v)  ((v) << (X_N - 4))
#define X_375(v) (X_25(v) + X_125(v))
#define X_625(v) (X_5(v) + X_125(v))
#define X_75(v) (X_5(v) + X_25(v))
#define X_6875(v) (X_5(v) + X_125(v) + X_0625(v))
#define X_1875(v) (X_125(v) + X_0625(v))
#define X_NORM(v) ((v) >> X_N)

		/*
		 * 0.50000000  0.09011998  0.00000000 0.10630376 
		 * 	0.50000000  0.09011998  0.00000000  0.10630376
		 * 0.45306372  0.28832037  0.03732892 0.08667963
		 * 	-0.45306372  0.11942621  0.10630376 -0.06764951
		 * 0.00000000  0.49039264  0.17677670 0.00000000
		 * 	0.00000000 -0.49039264 -0.17677670  0.00000000 
		 * -0.15909482  0.34009707  0.38408888 0.05735049
		 *	0.15909482  0.43576792 -0.09011998 -0.13845632
		 * 0.00000000 -0.03732892  0.46193977 0.25663998
		 * 	0.00000000 -0.03732892  0.46193977  0.25663998
		 * 0.10630376 -0.18235049  0.25663998 0.42361940 
		 *	-0.10630376 -0.16332037 -0.45306372 -0.01587282
		 * 0.00000000  0.00000000 -0.07322330 0.41573481
		 * 	0.00000000  0.00000000  0.07322330 -0.41573481
		 * -0.09011998  0.13399123 -0.18766514 0.24442621
		 *	0.09011998  0.13845632  0.15909482  0.47539609
		 */

		o[0] = X_NORM(X_5(x00 + x10) + X_0625(x01 + x11) +
			      X_125(x03 + x13));
		o[1] = X_NORM(X_5(x00 - x10) + X_25(x01) + X_0625(x03) +
			      X_125(x11 + x12));
		o[2] = X_NORM(X_5(x01 - x11) + X_1875(x02 + x12));
		o[3] = X_NORM(X_1875(x10 - x00) + X_375(x01 + x02) + 
			      X_5(x11) - X_125(x13));
		o[4] = X_NORM(X_5(x02 + x12) + X_25(x03 + x13));
		o[5] = X_NORM(X_125(x00 - x10) - X_1875(x01 + x11) +
			      X_25(x02) + X_5(x03 - x12));
		o[6] = X_NORM(X_625(x12 - x02) + X_375(x03 + x13));
		o[7] = X_NORM(X_125(x01 - x00 + x11 + x10 + x12) +
			      X_1875(x02) + X_25(x03) + X_5(x13));

		o += 8;
		in0 += 8;
		in1 += 8;
	}
}
