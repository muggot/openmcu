/*vid_coder.h       (c)   Indranet Technologies ltd (lara@indranet.co.nz)
 *    Code mangled by  Derek J Smithies (derek@indranet.co.nz), so that
 *    it runs inside voxilla.
 *                         
 * This file is derived from vic, http://www-nrg.ee.lbl.gov/vic/
 * Their copyright notice is below.
 */
/*-
 * Copyright (c) 1993-1995 The Regents of the University of California.
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
 * $Log: vid_coder.h,v $
 * Revision 1.1  2010/02/24 02:19:22  shorne
 * First commit of h323plus mirror
 *
 * Revision 1.2  2006/07/31 09:09:22  csoutheren
 * Checkin of validated codec used during development
 *
 * Revision 1.1.2.1  2006/04/06 01:17:17  csoutheren
 * Initial version of H.261 video codec plugin for OPAL
 *
 * Revision 2.3  2006/01/14 10:43:06  dsandras
 * Applied patch from Brian Lu <Brian.Lu _AT_____ sun.com> to allow compilation
 * with OpenSolaris compiler. Many thanks !!!
 *
 * Revision 2.2  2005/08/31 13:16:02  rjongbloed
 * Ported video fast update from OpenH323
 *
 * Revision 2.1  2003/03/15 23:43:00  robertj
 * Update to OpenH323 v1.11.7
 *
 * Revision 1.10  2003/04/03 23:54:15  robertj
 * Added fast update to H.261 codec, thanks Gustavo García Bernardo
 *
 * Revision 1.9  2002/04/26 04:57:41  dereks
 * Add Walter Whitlocks fixes, based on Victor Ivashim's suggestions to
 * improve the quality with Netmeeting. Thanks guys!!!!
 *
 * Revision 1.8  2001/10/16 21:20:07  yurik
 * Removed warnings on Windows CE. Submitted by Jehan Bing, jehan@bravobrava.com
 *
 * Revision 1.7  2001/05/10 05:25:44  robertj
 * Removed need for VIC code to use ptlib.
 *
 * Revision 1.6  2000/09/22 02:40:14  dereks
 * Tidied code for generating test images.
 * Added mechanism to ensure the entire frame is sent on startup.
 *
 * Revision 1.5  2000/08/25 03:18:50  dereks
 * Add change log facility (Thanks Robert for the info on implementation)
 *
 *
 *
 ********/


#if 0
#if defined(__osf__) || defined(__ultrix__)
/*XXX they didn't get this one right */
extern "C" {
#include <sys/types.h>
#include <sys/uio.h>
}
#else
#include <sys/types.h>
#ifndef _WIN32
#include <sys/uio.h>
#endif
#endif

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#endif 

#include "crdef.h"
#include "transmitter.h"
#include "encoder.h"

#include "crdef.h"
#include "videoframe.h"

#define HDRSIZE         4
#define	CIF_WIDTH	352
#define	CIF_HEIGHT	288
#define	QCIF_WIDTH	176
#define	QCIF_HEIGHT	144
#define	BMB		6    /* # blocks in a Macroblock */
#define MBPERGOB	33   /* # of Macroblocks per GOB (3 x 11 in image) */

#ifdef INT_64
#define NBIT 64
#define BB_INT INT_64
#else
#define NBIT 32
#define BB_INT u_int
#endif




#if BYTE_ORDER == LITTLE_ENDIAN
#if NBIT == 64
#define STORE_BITS(bb, bc) \
	bc[0] = (u_char)(bb >> 56); \
	bc[1] = (u_char)(bb >> 48); \
	bc[2] = (u_char)(bb >> 40); \
	bc[3] = (u_char)(bb >> 32); \
	bc[4] = (u_char)(bb >> 24); \
	bc[5] = (u_char)(bb >> 16); \
	bc[6] = (u_char)(bb >> 8); \
	bc[7] = (u_char)(bb);
#define LOAD_BITS(bc) \
	((BB_INT)bc[0] << 56 | \
	 (BB_INT)bc[1] << 48 | \
	 (BB_INT)bc[2] << 40 | \
	 (BB_INT)bc[3] << 32 | \
	 (BB_INT)bc[4] << 24 | \
	 (BB_INT)bc[5] << 16 | \
	 (BB_INT)bc[6] << 8 | \
	 (BB_INT)bc[7])
#else
#define STORE_BITS(bb, bc) \
	bc[0] = (u_char)(bb >> 24); \
	bc[1] = (u_char)(bb >> 16); \
	bc[2] = (u_char)(bb >> 8); \
	bc[3] = (u_char)(bb);
#define LOAD_BITS(bc) (ntohl(*(BB_INT*)(bc)))
#endif
#else
#define STORE_BITS(bb, bc) *(BB_INT*)bc = (bb);
#define LOAD_BITS(bc) (*(BB_INT*)(bc))
#endif

#define PUT_BITS(bits, n, nbb, bb, bc) \
{ \
	nbb += (n); \
	if (nbb > NBIT)  { \
		u_int extra = (nbb) - NBIT; \
		bb |= (BB_INT)(bits) >> extra; \
		STORE_BITS(bb, bc) \
		bc += sizeof(BB_INT); \
		bb = (BB_INT)(bits) << (NBIT - extra); \
		nbb = extra; \
	} else \
		bb |= (BB_INT)(bits) << (NBIT - (nbb)); \
}




class Pre_Vid_Coder : public Encoder {
  public:
      /**Constructor
      */
      Pre_Vid_Coder();

      /**Destructor
       */
      virtual ~Pre_Vid_Coder();

      /**Take a YUV411 planar frame and access which blocks to send
       */
      void ProcessFrame(VideoFrame *vf);

      /**Set internal variable (idle_high) for the fill rate.
       */
      void SetBackgroundFill(int fillLevel)
        {idle_high= fillLevel; }  

      void FastUpdatePicture();

 protected:
      void SetSize(int _width,int _height);
      void allocref();
      void Free_Memory();
      void suppress(const u_char* devbuf);
      void save(u_char* lum, u_char* cache, int stride);
      void saveblks(u_char* lum);
      void age_blocks();

      /**Conditional replacement vector (crvec) creation&initialization 
       */
      void crinit();

      /** conditional replacement vector
       */
      u_char *crvec;
      u_char *ref;

      int inw;
      int inh;
      int outw;
      int outh;
      int frameCount;

      /**number of blocks across the width of a frame.
         blkw is equal to pixels wide/16 */
      int blkw;

      /**number of blocks in the height of a frame.
         blkh is equal to pixels high/16 */     
      int blkh;

      /**Number of background blocks (those that did not change) which are
         sent in addition to those that have changed*/
      int idle_high;
      int idle_low;
      int delta;

      int frametime;

      int scan;

      /** number of blocks in the frame. */
      int nblk;
      /** index used to find the unchanged blocks to be transmitted with those
          that have changed*/
      int rover;

      int fastUpdCount;
};





#define REPLENISH(devbuf, refbuf, ds, bpp, hstart, hstop, vstart, vstop) \
{ \
	/* \
	 * First age the blocks from the previous frame. \
	 */ \
	age_blocks(); \
\
	register int _ds = ds; \
	register int _rs = outw; \
	const u_char* rb = &(refbuf)[scan * _rs]; \
	const u_char* db = &(devbuf)[scan * _ds]; \
	int w = blkw; \
	u_char* crv = crvec; \
 \
	crv += (vstart) * w; \
	for (int y = vstart; y < vstop; ++y) { \
		const u_char* ndb = db; \
		const u_char* nrb = rb; \
		u_char* ncrv = crv; \
		crv += hstart; \
		for (int x = hstart; x < hstop; x++) { \
			int left = 0; \
			int right = 0; \
			int top = 0; \
			int bottom = 0; \
			DIFFLINE(db, rb, left, top, right); \
			db += _ds << 3; \
			rb += _rs << 3; \
			DIFFLINE(db, rb, left, bottom, right); \
			db -= _ds << 3; \
			rb -= _rs << 3; \
 \
			int center = 0; \
			if (left >= 48 && x > 0) { \
				crv[-1] = CR_MOTION|CR_SEND; \
				center = 1; \
			} \
			if (right >= 48 && x < w - 1) { \
				crv[1] = CR_MOTION|CR_SEND; \
				center = 1; \
			} \
			if (bottom >= 48 && y < blkh - 1) { \
				crv[w] = CR_MOTION|CR_SEND; \
				center = 1; \
			} \
			if (top >= 48 && y > 0) { \
				crv[-w] = CR_MOTION|CR_SEND; \
				center = 1; \
			} \
			if (center) \
				crv[0] = CR_MOTION|CR_SEND; \
 \
			db += 16 * (bpp); \
			rb += 16; \
			++crv; \
		} \
		db = ndb + (_ds << 4); \
		rb = nrb + (_rs << 4); \
		crv = ncrv + w; \
	} \
}












