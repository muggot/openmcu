/*  
 * Copyright (c) 1993-1994 Regents of the University of California.
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
 * $Log: vid_coder.cxx,v $
 * Revision 1.2  2012/09/13 09:20:40  willamowius
 * minor cleanup
 *
 * Revision 1.1  2010/02/24 02:19:22  shorne
 * First commit of h323plus mirror
 *
 * Revision 1.3  2006/12/19 03:11:55  dereksmithies
 * Add excellent fixes from Ben Weekes to suppress valgrind error messages.
 * This will help memory management - many thanks.
 *
 * Revision 1.2  2006/07/31 09:09:22  csoutheren
 * Checkin of validated codec used during development
 *
 * Revision 1.1.2.1  2006/04/06 01:17:17  csoutheren
 * Initial version of H.261 video codec plugin for OPAL
 *
 * Revision 2.2  2005/08/31 13:16:02  rjongbloed
 * Ported video fast update from OpenH323
 *
 * Revision 2.1  2003/03/15 23:43:00  robertj
 * Update to OpenH323 v1.11.7
 *
 * Revision 1.6  2003/04/03 23:54:15  robertj
 * Added fast update to H.261 codec, thanks Gustavo García Bernardo
 *
 * Revision 1.5  2000/09/22 02:40:13  dereks
 * Tidied code for generating test images.
 * Added mechanism to ensure the entire frame is sent on startup.
 *
 * Revision 1.4  2000/08/25 03:18:50  dereks
 * Add change log facility (Thanks Robert for the info on implementation)
 *
 *
 *
 ********/


#include "vid_coder.h"

void Pre_Vid_Coder::SetSize(int _width,int _height)
{
  if((_width!=width)||(_height!=height)) {
     Free_Memory();
     width    =_width;
     height   =_height;
     outw     =_width;
     //   idle_high= 60;
     idle_low = 2;
     framesize=width*height;
     allocref();
     crinit();
     rover=0;
  }
}

Pre_Vid_Coder::Pre_Vid_Coder():Encoder(NULL)
{ 
 rover = 0;
 frameCount = 0;

 ref  =NULL;
 crvec=NULL;

 fastUpdCount = 0;
}

Pre_Vid_Coder::~Pre_Vid_Coder()
{
  Free_Memory();
}

void Pre_Vid_Coder::Free_Memory()
{
    if(crvec)
	delete [] crvec;
    crvec= NULL;
    if(ref)
	delete [] ref;
    ref= NULL;
}

void Pre_Vid_Coder::crinit()
{
	blkw = width >> 4;
	blkh = height >> 4;
	scan = 0;
	nblk = blkw * blkh;	
        if (crvec)
           delete [] crvec;
	crvec = new u_char[nblk];
	for (int i = 0; i < nblk; ++i)
		crvec[i] = CR_MOTION|CR_SEND;
}

/* must call after set_size_xxx */
void Pre_Vid_Coder::allocref()
{
	if(ref)
           delete [] ref;
	ref = new u_char[framesize];
	memset((char*)ref, 0, framesize);
}

/*
 * define these for REPLENISH macro used below
 */
#define ABS(v) if (v < 0) v = -v;

#define DIFF4(in, frm, v) \
	v += (in)[0] - (frm)[0]; \
	v += (in)[1] - (frm)[1]; \
	v += (in)[2] - (frm)[2]; \
	v += (in)[3] - (frm)[3];

#define DIFFLINE(in, frm, left, center, right) \
	DIFF4(in, frm, left); \
	DIFF4(in + 1*4, frm + 1*4, center); \
	DIFF4(in + 2*4, frm + 2*4, center); \
	DIFF4(in + 3*4, frm + 3*4, right); \
	ABS(right); \
	ABS(left); \
	ABS(center);

void Pre_Vid_Coder::suppress(const u_char* devbuf)
{
	REPLENISH(devbuf, ref, outw, 1, 0, blkw, 0, blkh);
}

inline void Pre_Vid_Coder::save(u_char* lum, u_char* cache, int stride)
{
	for (int i = 16; --i >= 0; ) {
		((u_int*)cache)[0] = ((u_int*)lum)[0];
		((u_int*)cache)[1] = ((u_int*)lum)[1];
		((u_int*)cache)[2] = ((u_int*)lum)[2];
		((u_int*)cache)[3] = ((u_int*)lum)[3];
		cache += stride;
		lum += stride;
	}
}

void Pre_Vid_Coder::ProcessFrame(VideoFrame *vf)
{
  if (!SameSize(vf))
    SetSize(vf->width,vf->height);
  frametime= vf->ts;  //Need frametime for deciding what blocks are old.
  suppress(vf->frameptr);
  saveblks(vf->frameptr);
  vf->crvec= crvec;
}
  

/*
 * Default save routine -- stuff new luma blocks into cache.
 */
void Pre_Vid_Coder::saveblks(u_char* lum)
{
	u_char* crv = crvec;
	u_char* cache = ref;
	int stride = outw;
	stride = (stride << 4) - stride;
	for (int y = 0; y < blkh; y++) {
		for (int x = 0; x < blkw; x++) {
			if ((*crv++ & CR_SEND) != 0)
				save(lum, cache, outw);
			cache += 16;
			lum += 16;
		}
		lum += stride;
		cache += stride;
	}
}

void Pre_Vid_Coder::age_blocks()
{
  frameCount++;
  fastUpdCount++;
  if( (frameCount<3) || (fastUpdCount< 3) ) {
	for (int i = 0; i < nblk; ++i)
		crvec[i] = CR_MOTION|CR_SEND;
        return;
  }
  /* The lines at the head of this routine are a hack.
   * They ensure that for the first two frames of the current 
   * connection, all blocks in the frame are sent.
   * Without these lines, netmeeting fills in the image over a
   * series of frames. These lines ensure that netmeeting starts
   * up with a full image.
   */
	for (int i = 0; i < nblk; ++i) {
		int s = CR_STATE(crvec[i]);
		/*
		 * Age this block.
		 * Once we hit the age threshold, we
		 * set CR_SEND as a hint to send a
		 * higher-quality version of the block.
		 * After this the block will stop aging,
		 * until there is motion.  In the meantime,
		 * we might send it as background fill
		 * using the highest quality.
		 */
		if (s <= CR_AGETHRESH) {
			if (s == CR_AGETHRESH)
				s = CR_IDLE;
			else {  s++;
				if  (s == CR_AGETHRESH)
					s |= CR_SEND;
			}
			crvec[i] = s;
		} else if (s == CR_BG)
			/*
			 * reset the block to IDLE if it was sent
			 * as a BG block in the last frame.
			 */
			crvec[i] = CR_IDLE;
	}
	/*
	 * Now go through and look for some idle blocks to send
	 * as background fill.
	 */
    //TODO: HORRIBLE, need better algorithm for determining n.

	int n = (frametime >= 1) ? idle_high : idle_low;

	while (n > 0) {
		int s = CR_STATE(crvec[rover]);
		if (s == CR_IDLE) {
			crvec[rover] = CR_SEND|CR_BG;
			--n;
		}
                rover++;
		if (rover >= nblk) {
			rover = 0;
			/* guarantee loop termination */
			break;
		}
	}

	/*
	 * Bump the CR scan pointer.  This variable controls which
	 * scan line of a block we use to make the replenishment
	 * decision.  We skip 3 lines at a time to quickly precess
	 * over the block.  Since 3 and 8 are coprime, we will
	 * sweep out every line.
	 */
	scan = (scan + 3) & 7;
}


void Pre_Vid_Coder::FastUpdatePicture()  //ADDED
{
  fastUpdCount = 0;
}


