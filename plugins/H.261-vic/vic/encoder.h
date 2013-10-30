/*encoder.h       (c) 1999-2000 Derek J Smithies (dereks@ibm.net)
 *                           Indranet Technologies ltd (lara@indranet.co.nz)
 *
 * This file is derived from vic, http://www-nrg.ee.lbl.gov/vic/
 * Their copyright notice is below.
 */
/*
 * Copyright (c) 1995 The Regents of the University of California.
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
 * 	This product includes software developed by the Network Research
 * 	Group at Lawrence Berkeley National Laboratory.
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
 * $Log: encoder.h,v $
 * Revision 1.1  2010/02/24 02:19:22  shorne
 * First commit of h323plus mirror
 *
 * Revision 1.2  2006/07/31 09:09:22  csoutheren
 * Checkin of validated codec used during development
 *
 * Revision 1.1.2.1  2006/04/06 01:17:17  csoutheren
 * Initial version of H.261 video codec plugin for OPAL
 *
 * Revision 2.3  2005/08/31 13:16:02  rjongbloed
 * Ported video fast update from OpenH323
 *
 * Revision 2.2  2005/03/19 04:08:10  csoutheren
 * Fixed warnings with gcc snapshot 4.1-20050313
 * Updated to configure 2.59
 *
 * Revision 2.1  2003/03/15 23:42:59  robertj
 * Update to OpenH323 v1.11.7
 *
 * Revision 1.7  2005/03/19 03:22:12  csoutheren
 * Removed warnings from gcc snapshot 4.1-20050313
 *
 * Revision 1.6  2003/04/10 22:39:32  dereks
 * Sort out copyright issues. Thanks Qhong Wang
 *
 * Revision 1.5  2003/03/14 07:25:55  robertj
 * Removed $header keyword so is not different on alternate repositories
 *
 * Revision 1.4  2003/02/10 00:32:47  robertj
 * Removed code for redundent class and constructor.
 *
 * Revision 1.3  2000/08/25 03:18:49  dereks
 * Add change log facility (Thanks Robert for the info on implementation)
 *
 *
 *
 ********/


#ifndef vic_encoder_h
#define vic_encoder_h
#include "config.h"

class Transmitter;

#include "videoframe.h"

class Encoder {
    public:
        virtual ~Encoder() { }
	virtual int consume(const VideoFrame*)
           {return 0; };

	void SetSize(int w, int h) {
		width = w;
		height = h;
		framesize = w * h;
	}
    protected:
	Encoder(Transmitter *T)
	  {width=0;  height=0; framesize=0;
           tx_ = T;}

	Transmitter* tx_;       //Given a transmitter object at creation.

	inline int SameSize(const VideoFrame* vf) {
		return ((vf->width == width) && (vf->height == height));
	}
	int width;
	int height;
	int framesize;
};


#endif
























































