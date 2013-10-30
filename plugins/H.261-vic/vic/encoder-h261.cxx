/*encoder-h261.cc  (c) 1999-2000 Derek J Smithies (dereks@ibm.net)
 *                           Indranet Technologies ltd (lara@indranet.co.nz)
 *
 * This file is derived from vic, http://www-nrg.ee.lbl.gov/vic/
 * Their copyright notice is below.
 */
/*
 * Copyright (c) 1994-1995 The Regents of the University of California.
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
 *      This product includes software developed by the University of
 *      California, Berkeley and the Network Research Group at
 *      Lawrence Berkeley Laboratory.
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
 * $Log: encoder-h261.cxx,v $
 * Revision 1.2  2011/01/10 21:42:24  willamowius
 * initialize sbit:, comment proposed removal of resetting sbit_
 *
 * Revision 1.1  2010/02/24 02:19:22  shorne
 * First commit of h323plus mirror
 *
 * Revision 1.6  2006/12/19 03:11:55  dereksmithies
 * Add excellent fixes from Ben Weekes to suppress valgrind error messages.
 * This will help memory management - many thanks.
 *
 * Revision 1.5  2006/08/12 10:59:14  rjongbloed
 * Added Linux build for H.261 plug-in.
 *
 * Revision 1.4  2006/08/10 07:05:46  csoutheren
 * Fixed compile warnings on VC 2005
 *
 * Revision 1.3  2006/08/01 13:02:40  rjongbloed
 * Merged changes from OpenH323 on removing need to winsock (ntohl function reference)
 *
 * Revision 1.2  2006/07/31 09:09:21  csoutheren
 * Checkin of validated codec used during development
 *
 * Revision 1.1.2.1  2006/04/06 01:17:17  csoutheren
 * Initial version of H.261 video codec plugin for OPAL
 *
 * Revision 2.2  2005/08/31 13:16:01  rjongbloed
 * Ported video fast update from OpenH323
 *
 * Revision 2.1  2003/03/15 23:42:59  robertj
 * Update to OpenH323 v1.11.7
 *
 * Revision 1.20  2004/04/25 01:52:47  rjongbloed
 * Fixed GCC 3.4 warnings
 *
 * Revision 1.19  2003/03/14 07:25:55  robertj
 * Removed $header keyword so is not different on alternate repositories
 *
 * Revision 1.18  2003/02/10 00:33:02  robertj
 * Removed code for redundent class and constructor.
 *
 * Revision 1.17  2002/05/17 01:47:33  dereks
 * backout the integer maths in the h261 codec.
 *
 * Revision 1.16  2002/04/05 00:53:19  dereks
 * Modify video frame encoding so that frame is encoded on an incremental basis.
 * Thanks to Walter Whitlock - good work.
 *
 * Revision 1.15  2001/10/14 21:48:45  dereks
 * Change vic's fdct() from floating-point to fix-point. Improves performance
 * for h261 video significantly on some machines. Thanks to Cosmos Jiang
 *
 * Revision 1.14  2001/05/10 05:25:44  robertj
 * Removed need for VIC code to use ptlib.
 *
 * Revision 1.13  2001/04/20 02:55:27  robertj
 * Fixed MSVC warning.
 *
 * Revision 1.12  2001/04/20 02:16:53  robertj
 * Removed GNU C++ warnings.
 *
 * Revision 1.11  2001/04/06 12:49:13  robertj
 * Fixed incorrect motion vector bit, thanks Liu Hao
 *
 * Revision 1.10  2001/04/04 02:05:48  robertj
 * Put back fix from revision 1.7, got lost in revision 1.8! Thanks Tom Dolsky.
 *
 * Revision 1.9  2000/12/19 22:22:34  dereks
 * Remove connection to grabber-OS.cxx files. grabber-OS.cxx files no longer used.
 * Video data is now read from a video channel, using the pwlib classes.
 *
 * Revision 1.8  2000/09/08 06:41:38  craigs
 * Added ability to set video device
 * Added ability to select test input frames
 *
 * Revision 1.7  2000/09/07 21:49:02  dereks
 * Implement fix from Vassili Leonov, to set PTYPE variable, so it now
 * follows the H261 spec as of 1993-03, page 7, paragraph 4.2.1.3:
 *
 * Revision 1.6  2000/08/25 03:18:49  dereks
 * Add change log facility (Thanks Robert for the info on implementation)
 *
 *
 *
 ********/


#include "encoder-h261.h"


H261Encoder::H261Encoder(Transmitter *T) : Encoder(T),
        bs_(0), bc_(0), ngob_(12), sbit_(0),
        gDone(true) // must initialize to true
{
 	for (int q = 0; q < 32; ++q) {
		llm_[q] = 0;
		clm_[q] = 0;
	}
}

H261Encoder::~H261Encoder(void)
{
    for (int q = 0; q < 32; ++q) {
	if(llm_[q])
	    delete [] (char *)llm_[q];
	if(clm_[q])
	    delete [] (char *)clm_[q];
    }
}


H261PixelEncoder:: H261PixelEncoder(Transmitter *T): H261Encoder(T)
{
	quant_required_ = 0;
}

H261DCTEncoder::H261DCTEncoder(Transmitter *T):H261Encoder(T)
{
	quant_required_ = 1;
}

/*
 * Set up the forward DCT quantization table for
 * INTRA mode operation.
 */
void
H261Encoder::setquantizers(int lq, int mq, int hq)
{
	int qt[64];
	if (lq > 31)
		lq = 31;
	if (lq < 1)
		lq = 1;
	lq_ = lq;

	if (mq > 31)
		mq = 31;
	if (mq < 1)
		mq = 1;
	mq_ = mq;

	if (hq > 31)
		hq = 31;
	if (hq <= 0)
		hq = 1;
	hq_ = hq;

	/*
	 * quant_required_ indicates quantization is not folded
	 * into fdct [because fdct is not performed]
	 */
	if (quant_required_ == 0) {
		/*
		 * Set the DC quantizer to 1, since we want to do this
		 * coefficient differently (i.e., the DC is rounded while
		 * the AC terms are truncated).
		 */
		qt[0] = 1;
		int i;
		for (i = 1; i < 64; ++i)
			qt[i] = lq_ << 1;
		fdct_fold_q(qt, lqt_);

		qt[0] = 1;
		for (i = 1; i < 64; ++i)
			qt[i] = mq_ << 1;
		fdct_fold_q(qt, mqt_);

		qt[0] = 1;
		for (i = 1; i < 64; ++i)
			qt[i] = hq_ << 1;
		fdct_fold_q(qt, hqt_);
	}
}

void
H261Encoder::setq(int q)
{
	setquantizers(q, q / 2, 1);
}

void
H261PixelEncoder::SetSize(int w, int h)
{
  if(width!=w){
	Encoder::SetSize(w, h);
	if (w == CIF_WIDTH && h == CIF_HEIGHT) {
		/* CIF */
		cif_ = 1;
		ngob_ = 12;
		bstride_ = 11;
		lstride_ = 16 * CIF_WIDTH - CIF_WIDTH / 2;
		cstride_ = 8 * 176 - 176 / 2;
		loffsize_ = 16;
		coffsize_ = 8;
		bloffsize_ = 1;
	} else if (w == QCIF_WIDTH && h == QCIF_HEIGHT) {
		/* QCIF */
		cif_ = 0;
		ngob_ = 6; /* not really number of GOBs, just loop limit */
		bstride_ = 0;
		lstride_ = 16 * QCIF_WIDTH - QCIF_WIDTH;
		cstride_ = 8 * 88 - 88;
		loffsize_ = 16;
		coffsize_ = 8;
		bloffsize_ = 1;
	} else {
	        //cerr << "H261PixelEncoder: H.261 bad geometry: " << w << 'x' << h << endl;
		return;
	}
	u_int loff = 0;
	u_int coff = 0;
	u_int blkno = 0;
	for (u_int gob = 0; gob < ngob_; gob += 2) {
		loff_[gob] = loff;
		coff_[gob] = coff;
		blkno_[gob] = blkno;
		/* width of a GOB (these aren't ref'd in QCIF case) */
		loff_[gob + 1] = loff + 11 * 16;
		coff_[gob + 1] = coff + 11 * 8;
		blkno_[gob + 1] = blkno + 11;

		/* advance to next GOB row */
		loff += (16 * 16 * MBPERGOB) << cif_;
		coff += (8 * 8 * MBPERGOB) << cif_;
		blkno += MBPERGOB << cif_;
	}
  }//if(width!=w)
}

void
H261DCTEncoder::SetSize(int w, int h)
{

	Encoder::SetSize(w, h);
	if (w == CIF_WIDTH && h == CIF_HEIGHT) {
		/* CIF */
		cif_ = 1;
		ngob_ = 12;
		bstride_ = 11;
		lstride_ = - (11 * (64*BMB)) + 2 * 11 * 64 * BMB;
		cstride_ = - (11 * (64*BMB)) + 2 * 11 * 64 * BMB;
		loffsize_ = 64 * BMB;
		coffsize_ = 64 * BMB;
		bloffsize_ = 1;
	} else if (w == QCIF_WIDTH && h == QCIF_HEIGHT) {
		/* QCIF */
		cif_ = 0;
		ngob_ = 6; /* not really number of GOBs, just loop limit */
		bstride_ = 0;
		lstride_ = 0;
		cstride_ = 0;
		loffsize_ = 64 * BMB;
		coffsize_ = 64 * BMB;
		bloffsize_ = 1;
	} else {
                //cerr << "H261DCTEncoder: H.261 bad geometry: " << w << 'x' << h << endl;
		return;
	}

	u_int gob;
	for (gob = 0; gob < ngob_; gob += 2) {

		if (gob != 0) {
			loff_[gob] = loff_[gob-2] +
				(MBPERGOB << cif_) * BMB * 64;
			coff_[gob] = coff_[gob-2] +
				(MBPERGOB << cif_) * BMB * 64;
			blkno_[gob] = blkno_[gob-2] +
				(MBPERGOB << cif_);
		} else {
			loff_[0] = 0;
			coff_[0] = loff_[0] + 4 * 64;	// 4 Y's
			blkno_[0] = 0;
		}

		loff_[gob + 1] = loff_[gob] + 11 * BMB * 64;
		coff_[gob + 1] = coff_[gob] + 11 * BMB * 64;
		blkno_[gob + 1] = blkno_[gob] + 11;
	}
}



/*
 * Make a map to go from a 12 bit dct value to an 8 bit quantized
 * 'level' number.  The 'map' includes both the quantizer (for the
 * dct encoder) and the perceptual filter 'threshhold' (for both
 * the pixel & dct encoders).  The first 4k of the map is for the
 * unfiltered coeff (the first 20 in zigzag order; roughly the
 * upper left quadrant) and the next 4k of the map are for the
 * filtered coef.
 */
char*
H261Encoder::make_level_map(int q, u_int fthresh)
{
	/* make the luminance map */
	char* lm = new char[0x2000];
  	char* flm = lm + 0x1000;
	int i;
	lm[0] = 0;
	flm[0] = 0;
	q = quant_required_? q << 1 : 0;
	for (i = 1; i < 0x800; ++i) {
		u_int l = i;
		if (q)
			l /= q;
		lm[i] = l;
		lm[-i & 0xfff] = -(int)l;

		if (l <= fthresh)
			l = 0;
		flm[i] = l;
		flm[-i & 0xfff] = -(int)l;
	}
	return (lm);
}

/*
 * encode_blk:
 *	encode a block of DCT coef's
 */
void
H261Encoder::encode_blk(const short* blk, const char* lm)
{
  int lm_incs=0;
	BB_INT bb = bb_;
	u_int nbb = nbb_;
	u_char* bc = bc_;

	/*
	 * Quantize DC.  Round instead of truncate.
	 */
	int dc = (blk[0] + 4) >> 3;

	if (dc <= 0)
		/* shouldn't happen with CCIR 601 black (level 16) */
		dc = 1;
	else if (dc > 254)
		dc = 254;
	else if (dc == 128)
		/* per Table 6/H.261 */
		dc = 255;
	/* Code DC */
	PUT_BITS(dc, 8, nbb, bb, bc);
	int run = 0;
	const u_char* colzag = &COLZAG[0];
	for (int zag; (zag = *++colzag) != 0; ) {
	  if (colzag == &COLZAG[20]) {
			lm += 0x1000;
      lm_incs++;
      if(lm_incs==2) {
			  //cerr<<"About to fart"<<endl; 
      }
    }
		int level = lm[((const u_short*)blk)[zag] & 0xfff];
		if (level != 0) {
			int val, nb;
			huffent* he; 
			if ((u_int)(level + 15) <= 30 &&
			    (nb = (he = &hte_tc[((level&0x1f) << 6)|run])->nb))
				/* we can use a VLC. */
				val = he->val;
			else {
				 /* Can't use a VLC.  Escape it. */
				val = (1 << 14) | (run << 8) | (level & 0xff);
				nb = 20;
			}
			PUT_BITS(val, nb, nbb, bb, bc);
			run = 0;
		} else
			++run;
	}
	/* EOB */
	PUT_BITS(2, 2, nbb, bb, bc);

	bb_ = bb;
	nbb_ = nbb;
	bc_ = bc;
}

/*
 * H261PixelEncoder::encode_mb
 *	encode a macroblock given a set of input YUV pixels
 */
void
H261PixelEncoder::encode_mb(u_int mba, // address of macroblock to encode
                    const u_char* frm, // address of YUV pixels
                           u_int loff, // Luminance offset from frm
                           u_int coff, // Chrominance offset from frm
                           int how)
{
	register int q;
	float* qt;
	if (how == CR_MOTION) {
		q = lq_;
		qt = lqt_;
	} else if (how == CR_BG) {
		q = hq_;
		qt = hqt_; 
	} else {
		/* must be at age threshold */
		q = mq_;
		qt = mqt_; 
	}

	/*
	 * encode all 6 blocks of the macro block to find the largest
	 * coef (so we can pick a new quantizer if gquant doesn't have
	 * enough range).
	 */
	/*XXX this can be u_char instead of short but need smarts in fdct */
	short blk[64 * 6];
	register int stride = width;
	/* luminance */
	const u_char* p = &frm[loff];
	fdct(p, stride, blk + 0, qt);
	fdct(p + 8, stride, blk + 64, qt);
	fdct(p + 8 * stride, stride, blk + 128, qt);
	fdct(p + (8 * stride + 8), stride, blk + 192, qt);
	/* chominance */
	int fs = framesize;
	p = &frm[fs + coff];
	stride >>= 1;
	fdct(p, stride, blk + 256, qt);
	fdct(p + (fs >> 2), stride, blk + 320, qt);

	/*
	 * if the default quantizer is too small to handle the coef.
	 * dynamic range, spin through the blocks and see if any
	 * coef. would significantly overflow.
	 */
	if (q < 8) {
		register int cmin = 0, cmax = 0;
		register short* bp = blk;
		for (register int i = 6; --i >= 0; ) {
			++bp;	// ignore dc coef
			for (register int j = 63; --j >= 0; ) {
				register int v = *bp++;
				if (v < cmin)
					cmin = v;
				else if (v > cmax)
					cmax = v;
			}
		}
		if (cmax < -cmin)
			cmax = -cmin;
		if (cmax >= 128) {
			/* need to re-quantize */
			register int s;
			for (s = 1; cmax >= (128 << s); ++s) {
			}
			q <<= s;
			register short* bp = blk;
			for (register int i = 6; --i >= 0; ) {
				++bp;	// ignore dc coef
				for (register int j = 63; --j >= 0; ) {
					register int v = *bp;
					*bp++ = v >> s;
				}
			}
		}
	}

	u_int m = mba - mba_; // set macroblock address difference 
	mba_ = mba; // set last absolute macroblock address transmitted
	huffent* he = &hte_mba[m - 1]; // Huffman coded macroblock address
	/* MBA */
	PUT_BITS(he->val, he->nb, nbb_, bb_, bc_);
	if (q != mquant_) {
		/* MTYPE = INTRA + TC + MQUANT */
		PUT_BITS(1, 7, nbb_, bb_, bc_);
		PUT_BITS(q, 5, nbb_, bb_, bc_);
		mquant_ = q;
	} else {
		/* MTYPE = INTRA + TC (no quantizer) */
		PUT_BITS(1, 4, nbb_, bb_, bc_);
	}

	/* luminance */
	const char* lm = llm_[q];
	if (lm == 0) {
		lm = make_level_map(q, 1);
		llm_[q] = lm;
		clm_[q] = make_level_map(q, 2);
	}
	encode_blk(blk + 0, lm);
	encode_blk(blk + 64, lm);
	encode_blk(blk + 128, lm);
	encode_blk(blk + 192, lm);
	/* chominance */
	lm = clm_[q];
	encode_blk(blk + 256, lm);
	encode_blk(blk + 320, lm);
}


/*
 * H261DCTEncoder::encode_mb
 *	encode a macroblock given a set of input DCT coefs
 *	each coef is stored as a short
 */
void
H261DCTEncoder::encode_mb(u_int mba, const u_char* frm,
			  u_int loff, u_int coff, int how)
{
	short *lblk = (short *)frm + loff;
	short *ublk = (short *)frm + coff;
	short *vblk = (short *)frm + coff + 64;

	register u_int q;
	if (how == CR_MOTION)
		q = lq_;
	else if (how == CR_BG)
		q = hq_;
	else
		/* must be at age threshold */
		q = mq_;

	/*
	 * if the default quantizer is too small to handle the coef.
	 * dynamic range, spin through the blocks and see if any
	 * coef. would significantly overflow.
	 */
	if (q < 8) {
		register int cmin = 0, cmax = 0;
		register short* bp = lblk;
		register int i, j;

		// Y U and V blocks
		for (i = 6; --i >= 0; ) {
			++bp;	// ignore dc coef
			for (j = 63; --j >= 0; ) {
				register int v = *bp++;
				if (v < cmin)
					cmin = v;
				else if (v > cmax)
					cmax = v;
			}
		}

		if (cmax < -cmin)
			cmax = -cmin;
		cmax /= (q << 1);
		if (cmax >= 128) {
			/* need to re-quantize */
			register int s;

			for (s = 1; cmax >= (128 << s); ++s) {
			}
			q <<= s;

		}
	}

	u_int m = mba - mba_;
	mba_ = mba;
	huffent* he = &hte_mba[m - 1];
	/* MBA */
	PUT_BITS(he->val, he->nb, nbb_, bb_, bc_);
	if (q != mquant_) {
		/* MTYPE = INTRA + TC + MQUANT */
		PUT_BITS(1, 7, nbb_, bb_, bc_);
		PUT_BITS(q, 5, nbb_, bb_, bc_);
		mquant_ = q;
	} else {
		/* MTYPE = INTRA + TC (no quantizer) */
		PUT_BITS(1, 4, nbb_, bb_, bc_);
	}

	/* luminance */
	const char* lm = llm_[q];
	if (lm == 0) {
		/*
		 * the filter thresh is 0 since we assume the jpeg percept.
		 * quantizer already did the filtering.
		 */
		lm = make_level_map(q, 0);
		llm_[q] = lm;
		clm_[q] = make_level_map(q, 0);
	}
	encode_blk(lblk + 0, lm);
	encode_blk(lblk + 64, lm);
	encode_blk(lblk + 128, lm);
	encode_blk(lblk + 192, lm);
	/* chominance */
	lm = clm_[q];
	encode_blk(ublk, lm);
	encode_blk(vblk, lm);
}

int
H261Encoder::flush(Transmitter::pktbuf* pb, int nbit,
		       Transmitter::pktbuf* npb)
{
	/* flush bit buffer */
	STORE_BITS(bb_, bc_);

	int cc = (nbit + 7) >> 3;
	int ebit = (cc << 3) - nbit;

	/*XXX*/
	if (cc == 0 && npb != 0)
		return 0;

	pb->lenHdr = HDRSIZE;
	pb->lenBuf = cc;
	u_int* rh = (u_int*)pb->hdr;

	*rh = (*rh) | ebit << 26 | sbit_ << 29;

	if (npb != 0) {
		u_char* nbs = (u_char*)npb->buf->data;
		u_int bc = (bc_ - bs_) << 3;
		int tbit = bc + nbb_;
		int extra = ((tbit + 7) >> 3) - (nbit >> 3);
		if (extra > 0)
			memcpy(nbs, bs_ + (nbit >> 3), extra);
		bs_ = nbs;
		sbit_ = nbit & 7;
		tbit -= nbit &~ 7;
		bc = tbit &~ (NBIT - 1);
		nbb_ = tbit - bc;
		bc_ = bs_ + (bc >> 3);
		/*
		 * Prime the bit buffer.  Be careful to set bits that
		 * are not yet in use to 0, since output bits are later
		 * or'd into the buffer.
		 */
		if (nbb_ > 0) {
			u_int n = NBIT - nbb_;
			bb_ = (LOAD_BITS(bc_) >> n) << n;
		} else
			bb_ = 0;
	}
	tx_->StoreOnePacket(pb);
	return (cc + HDRSIZE);
}

int H261DCTEncoder::consume(const VideoFrame *vf)
{
	if (!SameSize(vf))
		SetSize(vf->width, vf->height);
	return(encode(vf, vf->crvec));
}

int H261PixelEncoder::consume(const VideoFrame *vf)
{
	if (!SameSize(vf))
		SetSize(vf->width, vf->height);
	return(encode(vf, vf->crvec));
}

//////NOTE: HDRSIZE is the size of the H261 hdr in the rtp packet.== 4
int H261Encoder::encode(const VideoFrame* vf, const BYTE *crvec) {
  Transmitter::pktbuf* pb = tx_->alloc();
	bs_ = (u_char*)pb->buf->data; // pointer to start of buffer
	bc_ = bs_; // pointer to destination in buffer
	u_int ec = (tx_->mtu() - HDRSIZE) << 3; // bits available in H261 packet
	bb_ = 0; // intermediate working space
	nbb_ = 0; // cumulative put bit count
	sbit_ = 0;
	/* RTP/H.261 header */
	u_int* rh = (u_int*)pb->hdr; // H.261 header (32 bits)
	*rh = (1 << 24) | (lq_ << 10); // set motion vector flag V to 1 & set QUANT

	/* PSC Picture Start Code */
	PUT_BITS(0x0001, 16, nbb_, bb_, bc_);
	/* GOB 0 -> picture header Finishes PSC */
	PUT_BITS(0, 4, nbb_, bb_, bc_);
	/* TR Temporal Reference (XXX should do this right) */
	PUT_BITS(0, 5, nbb_, bb_, bc_);

	/* PTYPE = CIF */
	int pt = cif_ ? 7 : 3;
	PUT_BITS(pt, 6, nbb_, bb_, bc_);

	/* PEI Picture Extra Information = none */
	PUT_BITS(0, 1, nbb_, bb_, bc_);

	int step = cif_ ? 1 : 2;
	int cc = 0;

	BYTE* frm = vf->frameptr;
  for (u_int gobIndex = 0; gobIndex < ngob_; gobIndex += step) { // fill in GOB layer
		u_int loff = loff_[gobIndex];
		u_int coff = coff_[gobIndex];
		u_int blkno = blkno_[gobIndex];
		u_int nbit = ((bc_ - bs_) << 3) + nbb_; // # of bits already in buffer

		/* GSC/GN */
		PUT_BITS(0x10 | (gobIndex + 1), 20, nbb_, bb_, bc_);
		/* GQUANT/GEI */
		mquant_ = lq_;
		PUT_BITS(mquant_ << 1, 6, nbb_, bb_, bc_);

		mba_ = 0;
		int line = 11;
		for (u_int mba = 1; mba <= 33; ++mba) {
			/*
			 * If the conditional replenishment algorithm
			 * has decided to send any of the blocks of
			 * this macroblock, code it.
			 */
			u_int s = crvec[blkno];

			if ((s & CR_SEND) != 0) {
				u_int mbpred = mba_;
				encode_mb(mba, frm, loff, coff, CR_STATE(s)); // encode MB
				u_int cbits = ((bc_ - bs_) << 3) + nbb_;
				if (cbits > ec) { // make new packet
					Transmitter::pktbuf* npb;
					npb = tx_->alloc();
					cc += flush(pb, nbit, npb);
					cbits -= nbit;
					pb = npb;
					/* RTP/H.261 header */
					u_int m = mbpred;
					u_int g;
					if (m != 0) {
						g = gobIndex + 1;
						m -= 1;
					} else
						g = 0;

					rh = (u_int*)pb->hdr;
					*rh =	1 << 24 |  //set motion vector flag.
 					      m << 15 |  //macroblock address predictor.
					      g << 20 |  //Group of blocks number.
             mquant_ << 10;//quantizer value.
                                        
				}
				nbit = cbits;
			}

			loff += loffsize_;
			coff += coffsize_;
			blkno += bloffsize_;
			if (--line <= 0) {
				line = 11;
				blkno += bstride_;
				loff += lstride_;
				coff += cstride_;
			}

		}
	}
	cc += flush(pb, ((bc_ - bs_) << 3) + nbb_, 0); 
	return (cc);
}

int H261PixelEncoder::PreIncEncodeSetup(const VideoFrame *vf)
{
  if (!SameSize(vf))
    SetSize(vf->width, vf->height);
  gVf = vf;
  gPicture = true; //if true, send picture layer header
  gNbytes = 0;  //number of bytes in previous packet
  gDbase = 0; //offset from gData where valid data starts
  nbb_ = 0; //# of valid bits in bb_
  bb_ = 0; //intermediate working space
  bc_ = gData; //where to put encoded bits
  gStep = cif_ ? 1 : 2; //Macro Block step size
  gGobMax = cif_ ? 12 : 5; //how many GOB per frame
  sbit_ = 0;	// TODO: comment out for Polycom compatibility ???
  gSendGOBhdr = true; //must send GOB hdr before sending MB
  gGOBhdrNxt = true; //will start out with GOB header
  //because gGOBhdrNxt == true, no need to initialize the following 2 header variables:
  //gHdrGOBN; // next GOB number for last encoded MB
  //gHdrMBAP; // address of last macroblock encoded in previous packet 1..32
  //mba_= 0; // should not be necessary to initialize this
  gHdrQUANT = lq_; // QUANT in effect for next MB to be encoded
  gNxtMBA = 1; // address of next macroblock to be considered for encoding 1..33
  gNxtGOB = 1; // start encoding at GOB 1
  gDone = false;
  return 1;
} 

void H261PixelEncoder::IncEncodeAndGetPacket(
  u_char * buffer,    // returns buffer of encoded data
  unsigned & length ) // returns actual length of encoded data buffer
{
  u_int previousBitCount = 0, currentBitCount;
  h261hdr_t h261hdr;

  if (gDone) {
    length = 0;
    return;
  }

  // TESTING
  if (!gGOBhdrNxt && ((33 <= gHdrMBAP) || (0 >= gHdrMBAP))) {
    //cerr << __FILE__<< "(" << __LINE__ << ") " <<
    //  "illegal gHdrMBAP value when gGOBhdrNxt is false = " << gHdrMBAP << endl;
    gHdrMBAP = 1;
  }
  //  unsigned t1 = sbit_ << 29;
  //  unsigned t2 = (gGOBhdrNxt?0:gHdrGOBN) << 20;
  //  unsigned t3 = (gGOBhdrNxt||(0==gHdrMBAP)?0:gHdrMBAP-1) << 15;
  //  unsigned t4 = (gGOBhdrNxt?0:gHdrQUANT) << 10;

  // set the H.261 header (32 bits) bits that we know now
  h261hdr =               1 << 24 | // V = 1, I = 0, HMVD = 0, VMVD = 0
                      sbit_ << 29 | // SBIT
    (gGOBhdrNxt?0:gHdrGOBN) << 20 | // GOBN
  (gGOBhdrNxt?0:gHdrMBAP-1) << 15 | // MBAP 
   (gGOBhdrNxt?0:gHdrQUANT) << 10;  // QUANT
                  // gHdrQUANT << 10;  // QUANT set the old & incorrect way

  // if any, move unsent bits encoded during previous packet encode
  unsigned usedBB_INTs;
  u_char* msrc;
  unsigned m1;
  if (sbit_) gNbytes--; // last byte of previous packet has bits needed for this packet
  usedBB_INTs = (gNbytes + gDbase)/sizeof(BB_INT);
  msrc = gData + usedBB_INTs*sizeof(BB_INT);
  gDbase = (gDbase + gNbytes) % sizeof(BB_INT);
  m1 = bc_ - msrc;
  if (m1) memcpy(gData, msrc, m1);
  bc_ = gData + m1; // set starting bc_ address
  
  // encode H.261 stream
  if (gPicture) { // only ever sent at start of 1st packet 
    /* PSC Picture Start Code */
    PUT_BITS(0x0001, 16, nbb_, bb_, bc_);
    /* GOB 0 -> picture header Finishes PSC */
    PUT_BITS(0, 4, nbb_, bb_, bc_);
    /* TR Temporal Reference (XXX should do this right) */
    PUT_BITS(0, 5, nbb_, bb_, bc_);
    /* PTYPE = CIF */
    int pt = cif_ ? 7 : 3;
    PUT_BITS(pt, 6, nbb_, bb_, bc_);
    /* PEI Picture Extra Information = none */
    PUT_BITS(0, 1, nbb_, bb_, bc_);
    gloff = loff_[0];
    gcoff = coff_[0];
    gblkno = blkno_[0];
    gline = 11;
    gPicture = false;
  }
  unsigned bitLimit = 8*(RTP_MTU - sizeof(h261hdr));
  u_char* bbase = gData + gDbase;
  if (!(gNxtGOB > gGobMax)) {
    while ((currentBitCount = nbb_ + 8*(bc_ - bbase)) <= bitLimit) {
      // everything encoded up till now fits within the RTP_MTU buffer
      // test to see if the packet can be broken here
      if (gSendGOBhdr || (0 != mba_)){ // if packet can be broken,
        // record conditions that will be needed to construct the h261 header
        // for the next packet if the packet breaks here
        gHdrGOBN = gNxtGOB; // GOB number in effect for next MB to be encoded
        gHdrMBAP = mba_; // MBA of the last encoded MB & now in the buffer
        gHdrQUANT = mquant_; // QUANT in effect for next MB to be encoded
        gGOBhdrNxt = gSendGOBhdr; // is GOB header next?
        previousBitCount = currentBitCount; // encoded bits now in the buffer
      }
      if (gSendGOBhdr) { // need to send GOB header before can send MB ?
        /* GSC/GN */
        PUT_BITS(0x10 | gNxtGOB, 20, nbb_, bb_, bc_);
        /* GQUANT/GEI */
        mquant_ = lq_;
        PUT_BITS(mquant_ << 1, 6, nbb_, bb_, bc_);
        mba_= 0;
        gSendGOBhdr = false;
      }
      /** If the conditional replenishment algorithm
      * has decided to send any of the blocks of
      * this macroblock, encode it.
      */
      u_int s = gVf->crvec[gblkno];
      if ((s & CR_SEND) != 0) {
        //H261PixelEncoder::encode_mb(u_int mba, // address of macroblock to encode
        //  const u_char* frm, // address of YUV pixels
        //  u_int loff, // Luminance offset from frm
        //  u_int coff, // Chrominance offset from frm
        //  int how)
        encode_mb(gNxtMBA, gVf->frameptr, gloff, gcoff, CR_STATE(s)); // encode MB
        mba_= gNxtMBA;
      }
      gNxtMBA++;
      if (gNxtMBA > 33) {
        gNxtGOB += gStep;
        if (gNxtGOB > gGobMax) {
          gDone = true;
          break; // out of while(), done encoding frame
        }
        gNxtMBA = 1;
        gSendGOBhdr = true; // must send GOB hdr before sending MB
        gloff = loff_[gNxtGOB-1];
        gcoff = coff_[gNxtGOB-1];
        gblkno = blkno_[gNxtGOB-1];
        gline = 11;
      }
      else {
        gloff += loffsize_;
        gcoff += coffsize_;
        gblkno += bloffsize_;
        if (--gline <= 0) {
          gline = 11;
          gblkno += bstride_;
          gloff += lstride_;
          gcoff += cstride_;
        }
      }
    }
  }
  else 
    gDone = true;
  // have full packet now, finish & set up for next packet
  // break packet at end of previous MB unless this is end of frame
  // flush bits from bb_
  STORE_BITS(bb_, bc_); // necessary when gDbase != 0
  if (gDone) {
    unsigned totalBits = nbb_ + 8*(bc_ - bbase);
    if (totalBits <= bitLimit) { // would packet be too big?
      previousBitCount = totalBits;
    }
    else { // need another packet to finish frame
      gDone = false;
    }
  }
  gNbytes = previousBitCount / 8;
  sbit_ = previousBitCount % 8; // SBIT for next packet = good bits for this packet
  if (sbit_) gNbytes++; // include last bits for this frame
  
  // set the H.261 header (32 bits) bits that we know now
  h261hdr |= ((8 - sbit_) & 7) << 26; //EBIT = 8 - SBIT for next packet
  SWAP32(buffer, &h261hdr);
  memcpy(buffer+sizeof(h261hdr), gData+gDbase, gNbytes);
  length = gNbytes+sizeof(h261hdr);

  if (RTP_MTU < length) { // test for too large packet (I haven't seen one yet)
    //cerr << __FILE__<< "(" << __LINE__ << ") " <<
    //  "packet size of " << length << " > RTP_MTU(" << RTP_MTU << ")" << endl;
  }
}
