/*
 * Copyright (c) 1993-1995 Regents of the University of California.
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
 *	Group at Lawrence Berkeley National Laboratory.
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
/*
 * This code is derived from the P64 software implementation by the
 * Stanford PVRG group:
 * 
 * Copyright (C) 1990, 1991, 1993 Andy C. Hung, all rights reserved.
 * PUBLIC DOMAIN LICENSE: Stanford University Portable Video Research
 * Group. If you use this software, you agree to the following: This
 * program package is purely experimental, and is licensed "as is".
 * Permission is granted to use, modify, and distribute this program
 * without charge for any purpose, provided this license/ disclaimer
 * notice appears in the copies.  No warranty or maintenance is given,
 * either expressed or implied.  In no event shall the author(s) be
 * liable to you or a third party for any special, incidental,
 * consequential, or other damages, arising out of the use or inability
 * to use the program for any purpose (or the loss of data), even if we
 * have been advised of such possibilities.  Any public reference or
 * advertisement of this source code should refer to it as the Portable
 * Video Research Group (PVRG) code, and not by any author(s) (or
 * Stanford University) name.
 */

/************ Change log
 *
 * $Log: p64.cxx,v $
 * Revision 1.1  2010/02/24 02:19:22  shorne
 * First commit of h323plus mirror
 *
 * Revision 1.8  2007/07/06 12:54:44  dsandras
 * Added guard against possibly NULL pointer (Ekiga report 367981).
 *
 * Revision 1.7  2006/12/19 03:11:55  dereksmithies
 * Add excellent fixes from Ben Weekes to suppress valgrind error messages.
 * This will help memory management - many thanks.
 *
 * Revision 1.6  2006/11/12 03:34:07  rjongbloed
 * Fixed compilation on DevStudio 2003
 *
 * Revision 1.5  2006/11/01 20:02:33  dsandras
 * Fixed MINGW32 compilation.
 *
 * Revision 1.4  2006/08/12 10:59:14  rjongbloed
 * Added Linux build for H.261 plug-in.
 *
 * Revision 1.3  2006/08/01 13:02:40  rjongbloed
 * Merged changes from OpenH323 on removing need to winsock (ntohl function reference)
 *
 * Revision 1.2  2006/07/31 09:09:22  csoutheren
 * Checkin of validated codec used during development
 *
 * Revision 1.1.2.1  2006/04/06 01:17:17  csoutheren
 * Initial version of H.261 video codec plugin for OPAL
 *
 * Revision 2.7  2006/01/12 17:56:37  dsandras
 * Added initialization as in Mimas branch of openh323.
 *
 * Revision 2.6  2005/11/25 00:14:29  csoutheren
 * Applied patch #1303543 from Hannes Friederich
 * Added guard to weird conditions in H261 codec
 *
 * Revision 2.5  2005/10/12 21:20:25  dsandras
 * Added check for buffLen to prevent potential crash thanks to Hannes Friederich.
 *
 * Revision 2.4  2005/09/15 18:00:38  dsandras
 * Make sure qv_ is initialized when being used.
 *
 * Revision 2.3  2005/08/31 13:16:02  rjongbloed
 * Ported video fast update from OpenH323
 *
 * Revision 2.2  2005/08/15 01:47:27  csoutheren
 * Fixed compile problem on 64 bit
 *
 * Revision 2.1  2003/03/15 23:43:00  robertj
 * Update to OpenH323 v1.11.7
 *
 * Revision 1.16  2006/07/22 13:15:31  rjongbloed
 * Eliminate need for linking winsock just for ntohl() function.
 *
 * Revision 1.15  2005/12/04 22:33:35  csoutheren
 * Removed warning
 *
 * Revision 1.14  2005/11/25 00:10:23  csoutheren
 * Applied patch #1303543 from Hannes Friederich
 * Added guard to weird conditions in H261 codec
 *
 * Revision 1.13  2005/02/25 01:21:12  dominance
 * fixed building on amd64 with gcc-4.0 according to
 * http://bugs.debian.org/cgi-bin/bugreport.cgi?bug=287864
 *
 * Thanks fly to Andreas Jochens <aj@andaco.de>.
 *
 * Revision 1.12  2003/03/14 07:25:55  robertj
 * Removed $header keyword so is not different on alternate repositories
 *
 * Revision 1.11  2002/10/10 05:40:29  robertj
 * VxWorks port, thanks Martijn Roest
 *
 * Revision 1.10  2002/04/26 04:57:41  dereks
 * Add Walter Whitlocks fixes, based on Victor Ivashim's suggestions to
 * improve the quality with Netmeeting. Thanks guys!!!!
 *
 * Revision 1.9  2001/05/10 05:25:44  robertj
 * Removed need for VIC code to use ptlib.
 *
 * Revision 1.8  2001/05/08 14:53:29  yurik
 * Fixed misshap with removing non Wince def!
 *
 * Revision 1.7  2001/05/08 05:26:24  yurik
 * No ifdef _WIN32_WCE anymore - 3+ version of  SDK allows it
 *
 * Revision 1.6  2001/01/30 06:39:44  yurik
 * Modification submitted by Dave Cassel, dcassel@cyberfone.com.
 *
 * Revision 1.5  2000/12/19 22:22:34  dereks
 * Remove connection to grabber-OS.cxx files. grabber-OS.cxx files no longer used.
 * Video data is now read from a video channel, using the pwlib classes.
 *
 * Revision 1.4  2000/08/25 03:18:50  dereks
 * Add change log facility (Thanks Robert for the info on implementation)
 *
 *
 *
 ********/


#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#ifndef _MSC_VER
#include <stdint.h>
#endif

#include "config.h"

#ifndef _MSC_VER
  #include <stdint.h>
#endif

#include "p64.h"
#include "p64-huff.h"
#include "dct.h"
#include "bsd-endian.h"


#define SPLICE(v, p, n) (v) |= (p) << SHIFT(n)

void P64Decoder::err(const char* msg ...) const
{
#ifdef DEVELOPMENT_VERSION
	va_list ap;
	va_start(ap, msg);
	vfprintf(stderr, msg, ap);
	fprintf(stderr, " @g%d m%d %d/%d of %d/%d: %04x %04x %04x %04x|%04x\n",
		gob_, mba_,
		(int)((u_char*)bs_ - (u_char*)ps_), nbb_,
		(int)((u_char*)es_ - (u_char*)ps_), pebit_,
	       bs_[-4], bs_[-3], bs_[-2], bs_[-1], bs_[0]);
#endif
}

P64Decoder::P64Decoder()
	: fs_(0), front_(0), back_(0), qt_(0),
          ngob_(0), maxgob_(0), ndblk_(0),
          gobquant_(0),  mt_(0), gob_(0), 
          mba_(0), mvdh_(0),  mvdv_(0),
	  marks_(0),  mark_(0), 
	  bad_psc_(0), bad_bits_(0), bad_GOBno_(0), bad_fmt_(0) 
{
	fmt_ = IT_CIF;/*XXX*/
	inithuff();
	initquant();
}

P64Decoder::~P64Decoder()
{
	delete [] fs_;
}

void P64Decoder::init()
{
	if (fmt_ == IT_CIF) {
		ngob_ = 12;
		width_ = 352;
		height_ = 288;
	} else {
		ngob_ = 3;
		width_ = 176;
		height_ = 144;
	}
	size_ = width_ * height_;
	memset(mb_state_, MBST_OLD, sizeof(mb_state_));

	for (u_int gob = 0; gob < 12; ++gob) {
		u_short* p = &base_[gob << 6];
		for (int mba = 0; mba < MBPERGOB; ++mba) {
			u_int x = 2 * (mba % 11);
			u_int y;
			if (fmt_ == IT_CIF) {
				y = 2 * (3 * (gob >> 1) + mba / 11);
				if (gob & 1)
					x += 22;
			} else
				y = 2 * (3 * gob + mba / 11);
			
			p[mba] = (x << 8) | y;
		}
	}
	minx_ = width_;
	miny_ = height_;
	maxx_ = 0;
	maxy_ = 0;

	allocate();

	// invalidate the just-changed-block table (marks_ buffer)
	// to avoid buffer overruns
	marks_ = 0;
}

#if BYTE_ORDER == LITTLE_ENDIAN
#define HUFFRQ(bs, bb) \
 { \
	register int t = *bs++; \
	bb <<= 16; \
	bb |= (t & 0xff) << 8; \
	bb |= t >> 8; \
}
#else
#define HUFFRQ(bs, bb) \
 { \
	bb <<= 16; \
	bb |= *bs++; \
}
#endif

#define MASK(s) ((1 << (s)) - 1)

#define HUFF_DECODE(bs, ht, nbb, bb, result) { \
	register int s__, v__; \
 \
	if (nbb < 16) { \
		HUFFRQ(bs, bb); \
		nbb += 16; \
	} \
	s__ = ht.maxlen; \
	v__ = (bb >> (nbb - s__)) & MASK(s__); \
	s__ = (ht.prefix)[v__]; \
	nbb -= (s__ & 0x1f); \
	result = s__ >> 5; \
 }

#define GET_BITS(bs, n, nbb, bb, result) \
{ \
	nbb -= n; \
	if (nbb < 0)  { \
		HUFFRQ(bs, bb); \
		nbb += 16; \
	} \
	(result) = ((bb >> nbb) & MASK(n)); \
}

#define SKIP_BITS(bs, n, nbb, bb) \
{ \
	nbb -= n; \
	if (nbb < 0)  { \
		HUFFRQ(bs, bb); \
		nbb += 16; \
	} \
}

/*
 * Set up the huffman tables.
 */
void P64Decoder::inithuff()
{
	ht_mtype_.prefix = htd_mtype;
	ht_mtype_.maxlen = htd_mtype_width;
	ht_mba_.prefix = htd_mba;
	ht_mba_.maxlen = htd_mba_width;
	ht_mvd_.prefix = htd_dvm;
	ht_mvd_.maxlen = htd_dvm_width;
	ht_cbp_.prefix = htd_cbp;
	ht_cbp_.maxlen = htd_cbp_width;
	ht_tcoeff_.prefix = htd_tcoeff;
	ht_tcoeff_.maxlen = htd_tcoeff_width;
}

int P64Decoder::quantize(int v, int q)
{
	if (v > 0) 
		return (((v << 1) + 1) * q) - (~q & 1);
	else 
		return (((v << 1) - 1) * q) + (~q & 1);
}

/*
 * Build quantization lookup table.
 * One for each possible MQUANT paramenter.
 */
void P64Decoder::initquant()
{
	for (int mq = 0; mq < 32; ++mq) {
		short* qt = &quant_[mq << 8];
		for (int v = 0; v < 256; ++v) {
			int s = (v << 24) >> 24;
			qt[v] = quantize(s, mq);
		}
	}
}

/*
 * Decode the next block of transform coefficients
 * from the input stream.
 * Return number of non-zero ac coefficients.
 */
#ifdef INT_64
int P64Decoder::parse_block(short* blk, INT_64* mask)
#else
int P64Decoder::parse_block(short* blk, u_int* mask)
#endif
{
#ifdef INT_64
	INT_64 m0 = 0;
#else
	u_int m1 = 0, m0 = 0;
#endif
	/*
	 * Cache bit buffer in registers.
	 */
	register int nbb = nbb_;
	register int bb = bb_;
	register short* qt = qt_;

	int k;
	if ((mt_ & MT_CBP) == 0) {
		int v;
		GET_BITS(bs_, 8, nbb, bb, v);
		if (v == 255)
			v = 128;
		if (mt_ & MT_INTRA)
			v <<= 3;
		else
			v = qt[v];
		blk[0] = v;
		k = 1;
		m0 |= 1;
	} else if ((bb >> (nbb - 1)) & 1) {
		/*
		 * In CBP blocks, the first block present must be
		 * non-empty (otherwise it's mask bit wouldn't
		 * be set), so the first code cannot be an EOB.
		 * CCITT optimizes this case by using a huffman
		 * table equivalent to ht_tcoeff_ but without EOB,
		 * in which 1 is coded as "1" instead of "11".
		 * We grab two bits, the first bit is the code
		 * and the second is the sign.
		 */
		int v;
		GET_BITS(bs_, 2, nbb, bb, v);
		/*XXX quantize?*/
                if (qt)
                  blk[0] = qt[(v & 1) ? 0xff : 1];
                else
                  blk[0] = 0;
		k = 1;
		m0 |= 1;
	} else {
		k = 0;
#ifndef INT_64
		blk[0] = 0;/*XXX need this because the way we set bits below*/
#endif
	}
	int nc = 0;
	for (;;) {
		int r, v;
		HUFF_DECODE(bs_, ht_tcoeff_, nbb, bb, r);
		if (r <= 0) {
			/* SYM_EOB, SYM_ILLEGAL, or SYM_ESCAPE */
			if (r == SYM_ESCAPE) {
				GET_BITS(bs_, 14, nbb, bb, r);
				v = r & 0xff;
				r >>= 8;
			} else {
				if (r == SYM_ILLEGAL) {
					bb_ = bb;
					nbb_ = nbb;
					err("illegal symbol in block");
				}
				/* EOB */
				break;
			}
		} else {
			v = (r << 22) >> 27;
			r = r & 0x1f;
		}
		k += r;
		if (k >= 64) {
			bb_ = bb;
			nbb_ = nbb;
			err("bad run length %d (r %d, v %d)", k, r, v);
			break;
		}
		r = COLZAG[k++];
		blk[r] = (qt?qt[v & 0xff]:0);
		++nc;
#ifdef INT_64
		m0 |= (INT_64)1 << r;
#else
		if (r < 32)
			m0 |= 1 << r;
		else
			m1 |= 1 << (r - 32);
#endif
	}
	/*
	 * Done reading input.  Update bit buffer.
	 */
	bb_ = bb;
	nbb_ = nbb;

	*mask = m0;
#ifndef INT_64
	mask[1] = m1;
#endif
	return (nc);
}

/*
 * Mix in a motion-compensated, filtered block.  Note that
 * the input block may be misaligned so we cannot try fancy,
 * word-at-a-time accesses without being careful.  The output
 * block is, of course, aligned.
 * 
 * The 2-D loop filter is separable into 1-D FIR (0.25 0.5 0.25)
 * horizontal and vertical passes.  At the block edge, the filter
 * taps are (0 1 0).  Full arithmetic precision must be maintained,
 * until the output stage, where values are rounded (up).
 *
 * The code below tries to be efficient by caching the input
 * rows in registers, and running the filter on 3x3 chunks.
 * Multiple columns can be computed in parallel by using 
 * two 16-bit adds in a 32-bit register, or four 16-bit adds
 * in a 64-bit register.
 */
void P64Decoder::filter(u_char* in, u_char* out, u_int stride)
{
	/* Corner pixel has filter coef 1 */
	u_int s = in[0];
	u_int o = 0;
	SPLICE(o, s, 24);

	u_int r00 = s << 24 | in[1] << 16 | in[2] << 8 | in[3];
	u_int r01 = in[4] << 24 | in[5] << 16 | in[6] << 8 | in[7];
	in += stride;

	/*
	 * First row.
	 */
	s += (r00 >> 15) & 0x1fe;
	s += (r00 >> 8) & 0xff;
	/* round */
	s += 2;
	s >>= 2;
	SPLICE(o, s, 16);

	s = (r00 >> 16) & 0xff;
	s += (r00 >> 7) & 0x1fe;
	s += r00 & 0xff;
	/* round */
	s += 2;
	s >>= 2;
	SPLICE(o, s, 8);
	
	s = (r00 >> 8) & 0xff;
	s += (r00 & 0xff) << 1;
	s += r01 >> 24;
	/* round */
	s += 2;
	s >>= 2;
	SPLICE(o, s, 0);
	*(u_int*)out = o;
	
	s = r00 & 0xff;
	s += (r01 >> 23) & 0x1fe;
	s += (r01 >> 16) & 0xff;
	/* round */
	s += 2;
	s >>= 2;
	o = 0;
	SPLICE(o, s, 24);
	
	s = r01 >> 24;
	s += (r01 >> 15) & 0x1fe;
	s += (r01 >> 8) & 0xff;
	/* round */
	s += 2;
	s >>= 2;
	SPLICE(o, s, 16);
	
	s = (r01 >> 16) & 0xff;
	s += (r01 >> 7) & 0x1fe;
	s += r01 & 0xff;
	/* round */
	s += 2;
	s >>= 2;
	SPLICE(o, s, 8);

	/* corner has filter coef 1 */
	s = r01 & 0xff;
	SPLICE(o, s, 0);
	*(u_int*)(out + 4) = o;
	out += stride;

	/* load next rows into cache */
	u_int r10 = in[0] << 24 | in[1] << 16 | in[2] << 8 | in[3];
	u_int r11 = in[4] << 24 | in[5] << 16 | in[6] << 8 | in[7];
	in += stride;

	u_int r20 = 0, r21 = 0;
	u_int mask = 0xff00ff;
	for (int k = 6; --k >= 0; ) {
		/* load next row */
		r20 = in[0] << 24 | in[1] << 16 | in[2] << 8 | in[3];
		r21 = in[4] << 24 | in[5] << 16 | in[6] << 8 | in[7];
		in += stride;

		/* columns 0,2 */
		u_int v = (r00 >> 8) & mask;
		v += ((r10 >> 8) & mask) << 1;
		v += (r20 >> 8) & mask;

		/* first pixel */
		s = v >> 16;
		/* round */
		s += 2;
		s >>= 2;
		o = 0;
		SPLICE(o, s, 24);
		
		/* columns 1,3 */
		u_int w = r00 & mask;
		w += (r10 & mask) << 1;
		w += r20 & mask;

		/* row */
		s = v >> 16;
		s += v & 0xffff;
		s += w >> (16-1);
		/* round */
		s += 8;
		s >>= 4;
		SPLICE(o, s, 16);

		s = w >> 16;
		s += w & 0xffff;
		s += (v & 0xffff) << 1;
		/* round */
		s += 8;
		s >>= 4;
		SPLICE(o, s, 8);

		/* start next row */
		s = v & 0xffff;
		s += (w & 0xffff) << 1;
		/* but first do columns 4,6 */
		v = (r01 >> 8) & mask;
		v += ((r11 >> 8) & mask) << 1;
		v += (r21 >> 8) & mask;
		/* finish row */
		s += v >> 16;
		/* round */
		s += 8;
		s >>= 4;
		SPLICE(o, s, 0);
		*(u_int*)out = o;

		/* start next row */
		s = w & 0xffff;
		s += (v >> 16) << 1;
		/* but first do columns 5,7 */
		w = r01 & mask;
		w += (r11 & mask) << 1;
		w += r21 & mask;
		/* finish row */
		s += w >> 16;
		/* round */
		s += 8;
		s >>= 4;
		o = 0;
		SPLICE(o, s, 24);

		s = v >> 16;
		s += v & 0xffff;
		s += w >> (16-1);
		/* round */
		s += 8;
		s >>= 4;
		SPLICE(o, s, 16);

		s = w >> 16;
		s += w & 0xffff;
		s += (v & 0xffff) << 1;
		/* round */
		s += 8;
		s >>= 4;
		SPLICE(o, s, 8);

		s = w & 0xffff;
		/* round */
		s += 2;
		s >>= 2;
		SPLICE(o, s, 0);
		*(u_int*)(out + 4) = o;

		out += stride;

		/* roll lines up cache */
		r00 = r10;
		r01 = r11;
		r10 = r20;
		r11 = r21;
	}
	/*
	 * last row
	 */
	s = r20 >> 24;
	o = 0;
	SPLICE(o, s, 24);

	s += (r20 >> 15) & 0x1fe;
	s += (r20 >> 8) & 0xff;
	/* round */
	s += 2;
	s >>= 2;
	SPLICE(o, s, 16);

	s = (r20 >> 16) & 0xff;
	s += (r20 >> 7) & 0x1fe;
	s += r20 & 0xff;
	/* round */
	s += 2;
	s >>= 2;
	SPLICE(o, s, 8);
	
	s = (r20 >> 8) & 0xff;
	s += (r20 & 0xff) << 1;
	s += r21 >> 24;
	/* round */
	s += 2;
	s >>= 2;
	SPLICE(o, s, 0);
	*(u_int*)out = o;
	
	s = r20 & 0xff;
	s += (r21 >> 23) & 0x1fe;
	s += (r21 >> 16) & 0xff;
	/* round */
	s += 2;
	s >>= 2;
	o = 0;
	SPLICE(o, s, 24);
	
	s = r21 >> 24;
	s += (r21 >> 15) & 0x1fe;
	s += (r21 >> 8) & 0xff;
	/* round */
	s += 2;
	s >>= 2;
	SPLICE(o, s, 16);
	
	s = (r21 >> 16) & 0xff;
	s += (r21 >> 7) & 0x1fe;
	s += r21 & 0xff;
	/* round */
	s += 2;
	s >>= 2;
	SPLICE(o, s, 8);

	/* corner has filter coef 1 */
	s = r21 & 0xff;
	SPLICE(o, s, 0);
	*(u_int*)(out + 4) = o;
}

void P64Decoder::mvblka(u_char* in, u_char* out, u_int stride)
{
#ifdef INT_64
	*(INT_64*)out = *(INT_64*)in;
	out += stride; in += stride;
	*(INT_64*)out = *(INT_64*)in;
	out += stride; in += stride;
	*(INT_64*)out = *(INT_64*)in;
	out += stride; in += stride;
	*(INT_64*)out = *(INT_64*)in;
	out += stride; in += stride;
	*(INT_64*)out = *(INT_64*)in;
	out += stride; in += stride;
	*(INT_64*)out = *(INT_64*)in;
	out += stride; in += stride;
	*(INT_64*)out = *(INT_64*)in;
	out += stride; in += stride;
	*(INT_64*)out = *(INT_64*)in;
#else
	for (int k = 8; --k >= 0; ) {
		*(u_int*)out = *(u_int*)in;
		*(u_int*)(out + 4) = *(u_int*)(in + 4);
		in += stride;
		out += stride;
	}
#endif
}

void P64Decoder::mvblk(u_char* in, u_char* out, u_int stride)
{
#ifdef INT_64
	if (((u_long)in & 7) == 0) {
		mvblka(in, out, stride);
		return;
	}
#else
	if (((u_long)in & 3) == 0) {
		mvblka(in, out, stride);
		return;
	}
#endif
	for (int k = 8; --k >= 0;) {
		u_int* o = (u_int*)out;
#if BYTE_ORDER == LITTLE_ENDIAN
		o[0] = in[3] << 24 | in[2] << 16 | in[1] << 8 | in[0];
		o[1] = in[7] << 24 | in[6] << 16 | in[5] << 8 | in[4];
#else
		o[0] = in[0] << 24 | in[1] << 16 | in[2] << 8 | in[3];
		o[1] = in[4] << 24 | in[5] << 16 | in[6] << 8 | in[7];
#endif
		in += stride;
		out += stride;
	}
}

/*
 * Parse a picture header.  We assume that the
 * start code has already been snarfed.
 */
int P64Decoder::parse_picture_hdr()
{
	/* throw away the temporal reference */
	SKIP_BITS(bs_, 5, nbb_, bb_);
	int pt;
	GET_BITS(bs_, 6, nbb_, bb_, pt);
	int fmt = (pt >> 2) & 1;
	if (fmt_ != fmt) {
		/* change formats */
		fmt_ = fmt;
		init();
	}
	int v;
	GET_BITS(bs_, 1, nbb_, bb_, v);
	while (v != 0) {
		GET_BITS(bs_, 9, nbb_, bb_, v);
		/*
		 * XXX from pvrg code: 0x8c in PSPARE means ntsc.
		 * this is a hack.  we don't support it.
		 */
		int pspare = v >> 1;
		if (pspare == 0x8c && (pt & 0x04) != 0) {
			static int first = 1;
			if (first) {
				err("pvrg ntsc not supported");
				first = 0;
			}
		}
		v &= 1;
	}
	return (0);
}

inline int P64Decoder::parse_sc()
{
	int v;
	GET_BITS(bs_, 16, nbb_, bb_, v);
	if (v != 0x0001) {
		err("bad start code %04x", v);
		++bad_psc_;
		return (-1);
	}
	return (0);
}

/*
 * Parse a GOB header, which consists of the GOB quantiation
 * factor (GQUANT) and spare bytes that we ignore.
 */
int P64Decoder::parse_gob_hdr(int ebit)
{
	mba_ = -1;
	mvdh_ = 0;
	mvdv_ = 0;

	/*
	 * Get the next GOB number (or 0 for a picture header).
	 * The invariant at the top of this loop is that the
	 * bit stream is positioned immediately past the last
	 * start code.
	 */
	int gob;
	for (;;) {
		GET_BITS(bs_, 4, nbb_, bb_, gob);
		if (gob != 0)
			break;
		/*
		 * should happen only on first iteration
		 * (if at all).  pictures always start on
		 * packet boundaries per section 5 of the
		 * Internet Draft.
		 */
		if (parse_picture_hdr() < 0) {
			++bad_fmt_;
			return (-1);
		}
		/*
		 * Check to see that the next 16 bits
		 * are a start code and throw them away.
		 * But first check that we have the bits.
		 */
		int nbit = ((es_ - bs_) << 4) + nbb_ - ebit;
		if (nbit < 20)
			return (0);

		if (parse_sc() < 0)
			return (-1);
	}
	gob -= 1;
	if (fmt_ == IT_QCIF)
		/*
		 * Number QCIF GOBs 0,1,2 instead of 0,2,4.
		 */
		gob >>= 1;

	if (gob >= ngob_) {
		err("gob number too big (%d>%d)", gob, ngob_);
		return (-1);
	}

	int mq;
	GET_BITS(bs_, 5, nbb_, bb_, mq);
	gobquant_ = mq;
	qt_ = &quant_[mq << 8];

	int v;
	GET_BITS(bs_, 1, nbb_, bb_, v);
	while (v != 0) {
		GET_BITS(bs_, 9, nbb_, bb_, v);
		v &= 1;
	}
	gob_ = gob;
	if (gob > maxgob_)
		maxgob_ = gob;

	return (gob);
}

/*
 * Parse a macroblock header.  If there is no mb header because
 * we hit the next start code, return -1, otherwise 0.
 */
int P64Decoder::parse_mb_hdr(u_int& cbp)
{
	/*
	 * Read the macroblock address (MBA)
	 */
	int v;
	HUFF_DECODE(bs_, ht_mba_, nbb_, bb_, v);
	if (v <= 0) {
		/*
		 * (probably) hit a start code; either the
		 * next GOB or the next picture header.
		 * If we got MBA stuffing (0) we need to return
		 * so the outer loop can check if we're at the
		 * end of the buffer (lots of codecs put stuffing
		 * at the end of a picture to byte align the psc).
		 */
		return (v);
	}

	/*
	 * MBA is differentially encoded.
	 */
	mba_ += v;
	if (mba_ >= MBPERGOB) {
		err("mba too big %d", mba_);
		return (SYM_ILLEGAL);
	}

	u_int omt = mt_;
	HUFF_DECODE(bs_, ht_mtype_, nbb_, bb_, mt_);
	if (mt_ & MT_MQUANT) {
		int mq;
		GET_BITS(bs_, 5, nbb_, bb_, mq);
		qt_ = &quant_[mq << 8];
	}
	if (mt_ & MT_MVD) {
		/*
		 * Read motion vector.
		 */
		int dh;
		int dv;
		HUFF_DECODE(bs_, ht_mvd_, nbb_, bb_, dh);
		HUFF_DECODE(bs_, ht_mvd_, nbb_, bb_, dv);
		/*
		 * Section 4.2.3.4
		 * The vector is differentially coded unless any of:
		 *   - the current mba delta isn't 1
		 *   - the current mba is 1, 12, or 23 (mba mod 11 = 1)
		 *   - the last block didn't have motion vectors.
		 *
		 * This arithmetic is twos-complement restricted
		 * to 5 bits.
		 */
		if ((omt & MT_MVD) != 0 && v == 1 &&
		    mba_ != 0 && mba_ != 11 && mba_ != 22) {
			dh += mvdh_;
			dv += mvdv_;
		}
		mvdh_ = (dh << 27) >> 27;
		mvdv_ = (dv << 27) >> 27;
	}
	/*
	 * Coded block pattern.
	 */
	if (mt_ & MT_CBP) {
		HUFF_DECODE(bs_, ht_cbp_, nbb_, bb_, cbp);
		if (cbp > 63) {
			err("cbp invalid %x", cbp);
			return (SYM_ILLEGAL);
		}
	} else
		cbp = 0x3f;

	return (1);
}

/*
 * Handle the next block in the current macroblock.
 * If tc is non-zero, then coefficients are present
 * in the input stream and they are parsed.  Otherwise,
 * coefficients are not present, but we take action
 * according to the type macroblock that we have.
 */
void P64Decoder::decode_block(u_int tc, u_int x, u_int y, u_int stride,
			      u_char* front, u_char* back, int sf)
{
	short blk[64];
#ifdef INT_64
	INT_64 mask;
#define MASK_VAL	mask
#define MASK_REF	&mask
#else
	u_int mask[2];
#define MASK_VAL	mask[0], mask[1]
#define MASK_REF	mask
#endif
	int nc=0;
	if (tc != 0)
		nc = parse_block(blk, MASK_REF);

	int off = y * stride + x;
	u_char* out = front + off;

	if (mt_ & MT_INTRA) {
		if (tc != 0) {
			if (nc == 0)
				dcfill((blk[0] + 4) >> 3, out, stride);
#ifdef notdef
			else if (nc == 1) {
#ifdef INT_64
				u_int dc = (mask & 1) ? (blk[0] + 4) >> 3 : 0;
				for (int k = 1; k < 64; ++k) {
					if (mask & ((INT_64)1 << k)) {
						bv_rdct1(dc, blk, k,
							 out, stride);
						return;
					}
				}
#else
				u_int m0 = mask[0];
				u_int m1 = mask[1];
				u_int dc = (m0 & 1) ? (blk[0] + 4) >> 3 : 0;
				for (int k = 1; k < 64; ++k) {
					m0 >>= 1;
					m0 |= m1 << 31;
					m1 >>= 1;
					if (m0 & 1) {
						bv_rdct1(dc, blk, k,
							 out, stride);
						return;
					}
				}
#endif
#endif
			 else
				rdct(blk, MASK_VAL, out, stride, (u_char*)0);
		} else {
			u_char* in = back + off;
			mvblka(in, out, stride);
		}
		return;
	}
	if ((mt_ & MT_MVD) == 0) {
		u_char* in = back + off;
		if (tc != 0) {	
			if (nc == 0) {
				dcsum((blk[0] + 4) >> 3, in, out, stride);
			} else
				rdct(blk, MASK_VAL, out, stride, in);
		} else
			mvblka(in, out, stride);
		return;
	}
	int sx = x + (mvdh_ / sf);
	int sy = y + (mvdv_ / sf);
	u_char* in = (u_char*)((intptr_t)back + sy * stride + sx);
	if (mt_ & MT_FILTER) {
		filter(in, out, stride);
		if (tc != 0) {
			if (nc == 0)
				dcsum2((blk[0] + 4) >> 3, out, out, stride);
			else
				rdct(blk, MASK_VAL, out, stride, out);
		}
	} else {
		if (tc != 0) {
			if (nc == 0)
				dcsum2((blk[0] + 4) >> 3, in, out, stride);
			else
				rdct(blk, MASK_VAL, out, stride, in);
		} else
			mvblk(in, out, stride);
	}
}

/*
 * Decompress the next macroblock.  Return 0 if the macroblock
 * was present (with no errors).  Return SYM_STARTCODE (-1),
 * if there was no macroblock but instead the start of the
 * next GOB or picture (in which case the start code has
 * been consumed).  Return SYM_ILLEGAL (-2) if there was an error.
 */
int P64Decoder::decode_mb()
{
	u_int cbp;
	register int v;

	if ((v = parse_mb_hdr(cbp)) <= 0)
		return (v);

	/*
	 * Lookup the base coordinate for this MBA.
	 * Convert from a block to a pixel coord.
	 */
	register u_int x, y;
	x = coord_[mba_];
	y = (x & 0xff) << 3;
	x >>= 8;
	x <<= 3;

	/* Update bounding box */
	if (x < minx_)
		minx_ = x;
	if (x > maxx_)
		maxx_ = x;
	if (y < miny_)
		miny_ = y;
	if (y > maxy_)
		maxy_ = y;

	/*
	 * Decode the six blocks in the MB (4Y:1U:1V).
	 * (This code assumes MT_TCOEFF is 1.)
	 */
	register u_int tc = mt_ & MT_TCOEFF;
	register u_int s = width_;
	decode_block(tc & (cbp >> 5), x, y, s, front_, back_, 1);
	decode_block(tc & (cbp >> 4), x + 8, y, s, front_, back_, 1);
	decode_block(tc & (cbp >> 3), x, y + 8, s, front_, back_, 1);
	decode_block(tc & (cbp >> 2), x + 8, y + 8, s, front_, back_, 1);
	s >>= 1;
	int off = size_;
	decode_block(tc & (cbp >> 1), x >> 1, y >> 1, s,
		     front_ + off, back_ + off, 2);
	off += size_ >> 2;
	decode_block(tc & (cbp >> 0), x >> 1, y >> 1, s,
		     front_ + off, back_ + off, 2);

	mbst_[mba_] = MBST_NEW;
	
	/*
	 * If a marking table was attached, take note.
	 * This allows us to dither only the blocks that have changed,
	 * rather than the entire image on each frame.
	 */
	if (marks_) {
		/* convert to 8x8 block offset */
		off = (x >> 3) + (y >> 3) * (width_ >> 3);
		int m = mark_;
		marks_[off] = m;
		marks_[off + 1] = m;
		off += width_ >> 3;
		marks_[off] = m;
		marks_[off + 1] = m;
	}
	return (0);
}

/*
 * Decode H.261 stream.  Decoding can begin on either
 * a GOB or macroblock header.  All the macroblocks of
 * a given frame can be decoded in any order, but chunks
 * cannot be reordered across frame boundaries.  Since data
 * can be decoded in any order, this entry point can't tell
 * when a frame is fully decoded (actually, we could count
 * macroblocks but if there is loss, we would not know when
 * to sync).  Instead, the callee should sync the decoder
 * by calling the sync() method after the entire frame 
 * has been decoded (modulo loss).
 *
 * This routine should not be called with more than
 * one frame present since there is no callback mechanism
 * for renderering frames (i.e., don't call this routine
 * with a buffer that has a picture header that's not
 * at the front).
 */
bool P64Decoder::decode(const unsigned char *hdrPtr, int buffLen,
         bool lostPreviousPacket)
{
  const u_char *bp;
  int cc, sbit, ebit, gob;
  h261hdr_t h261hdr;
  
  // preventing accidential crashes
  if (buffLen == 0)
    return false;

  // get 32 bit H261 header
  SWAP32(&h261hdr, hdrPtr);
  // decode values we need from the RTP H261 header
  sbit  = (h261hdr >> 29) & 0x07;
  ebit  = (h261hdr >> 26) & 0x07;
  gob   = (h261hdr >> 20) & 0xf;
  // get remaining values from H261 header only when previous packet was lost
  if (lostPreviousPacket) {
    //PTRACE(3, "H261\tLost or out of order packet, using values from H261 header");
    mba_ = (h261hdr >> 15) & 0x1f;
    int quant = (h261hdr >> 10) & 0x1f;
    qt_ = &quant_[quant << 8];
    mvdh_ = (h261hdr >> 5) & 0x1f; // sometimes NetMeeting sends bad values for these fields
    mvdv_ = h261hdr & 0x1f; // use them only when the previous packet was lost
  }

  // adjust for length of H.261 header
  bp = hdrPtr + sizeof(h261hdr_t);
  ps_ = (u_short*)bp; // this does not seem to be used anywhere
  cc = buffLen - sizeof(h261hdr_t);
	/*
	 * If cc is odd, ignore 8 extra bits in last short.
	 */
	int odd = cc & 1;
	ebit += odd << 3;
	pebit_ = ebit;
	es_ = (u_short*)(bp + ((cc - 1) &~ 1));

	/*
	 * If input buffer not aligned, prime bit-buffer
	 * with 8 bits; otherwise, prime it with a 16.
	 */
	if ((long)bp & 1) {
		bs_ = (u_short*)(bp + 1);
		bb_ = *bp;
		nbb_ = 8 - sbit;
	} else {
		bs_ = (u_short*)bp;
		HUFFRQ(bs_, bb_);
		nbb_ = 16 - sbit;
	}

  if (gob > 12) {
    //count(STAT_BAD_HEADER);
    return false; // bad header value, what to do?
  }
  /*XXX don't rely on this*/
  if (gob != 0) {
    gob -= 1;
    if (fmt_ == IT_QCIF) gob >>= 1;
	}

  while (bs_ < es_ || (bs_ == es_ && nbb_ > ebit)) {
		mbst_ = &mb_state_[gob << 6];
		coord_ = &base_[gob << 6];

		ndblk_++;
		int v = decode_mb();
		if (v == 0)
			continue;

		if (v != SYM_STARTCODE) {
			err("expected GOB startcode");
			++bad_bits_;
			return (false);
		}
		gob = parse_gob_hdr(ebit);
		if (gob < 0) {
			/*XXX*/
			++bad_bits_;
			return (false);
		}
	}
	return (true);
}

FullP64Decoder::FullP64Decoder()
{
	init();
}

void FullP64Decoder::allocate()
{
	delete [] fs_;
	int n = size_ + (size_ >> 1);
	fs_ = new u_char[2 * n];
	/* initialize to gray */
	memset(fs_, 0x80, 2 * n);
	front_ = fs_;
	back_ = front_ + n;
}

/*
 * Swap the `front' and `back' frame buffers.  While decoding a
 * frame, the front buffer is the image being constructed while
 * the back buffer is the reference image.  Rather than copy
 * the whole image each time, we just swap pointers here.
 * We defer this copying until we find out that we're skipping
 * over a macroblock, or even a whole gob.  In this case, we
 * go ahead and copy it, but take note in the mb_skip_ array.
 * Next time we need to copy it, we skip it if the skip array
 * says it's okay (e.g., there is no reason to copy a given block
 * back and forth between buffers if it never changes).  When we
 * modify a macroblock, we clear out it's entry in mb_skip_.
 */
void FullP64Decoder::swap()
{
	u_char* p = front_;
	front_ = back_;
	back_ = p;
}

/*
 * Copy a macroblock from the saved frame (back buffer)
 * to the current frame (front buffer). coord_ determines
 * which GOB we're in.
 */
void FullP64Decoder::mbcopy(u_int mba)
{
	u_int x, y;
	x = coord_[mba];
	y = (x & 0xff) << 3;
	x >>= 8;
	x <<= 3;

	u_int stride = width_;
	u_int off = y * stride + x;
	u_char* in = back_ + off;
	u_char* out = front_ + off;

	mvblka(in, out, stride);
	mvblka(in + 8, out + 8, stride);
	in += stride << 3;
	out += stride << 3;
	mvblka(in, out, stride);
	mvblka(in + 8, out + 8, stride);
	x >>= 1;
	y >>= 1;
	stride >>= 1;
	off = y * stride + x;
	off += size_;
	in = back_ + off;
	out = front_ + off;
	mvblka(in, out, stride);
	off += size_ >> 2;
	in = back_ + off;
	out = front_ + off;
	mvblka(in, out, stride);
}

void P64Decoder::sync()
{
	bbx_ = minx_;
	bby_ = miny_;
	bbw_ = maxx_ - minx_ + 16;
	bbh_ = maxy_ - miny_ + 16;

	minx_ = width_;
	miny_ = height_;
	maxx_ = 0;
	maxy_ = 0;

	maxgob_ = 0;
}

void FullP64Decoder::sync()
{
	for (int k = 0; k < ngob_; ++k) {
		coord_ = &base_[k << 6];
		u_char* mbst = &mb_state_[k << 6];
		for (int mba = 0; mba < MBPERGOB; ++mba) {
			int s = mbst[mba];
			if (s == MBST_FRESH) {
				mbcopy(mba);
				mbst[mba] = MBST_OLD;
			} else if (s == MBST_NEW)
				mbst[mba] = MBST_FRESH;
		}
	}
	swap();
	P64Decoder::sync();
}

IntraP64Decoder::IntraP64Decoder()
{
	init();
}

void IntraP64Decoder::allocate()
{
	delete [] fs_;
	int n = size_ + (size_ >> 1);
	fs_ = new u_char[n];
	/* initialize to gray */
	memset(fs_, 0x80, n);
	front_ = back_ = fs_;
}
