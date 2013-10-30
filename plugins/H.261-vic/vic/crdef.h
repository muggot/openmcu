/* crdef.h      (c) 1999-2000 Derek J Smithies (dereks@ibm.net)
 *                           Indranet Technologies ltd (lara@indranet.co.nz)
 *
 * This file is derived from vic, http://www-nrg.ee.lbl.gov/vic/
 * Their copyright notice is below.
 */
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
 *
 */

/*
 * The bit definitions for the values stored in the conditional
 * replenishment vector.  We use a variant of the algorithm used
 * in nv (which is very similar to the algorithms in many hardware
 * codecs) where we only send a block if it changes beyond some threshold
 * due to scene motion.  In this case, we send it at a "low-quality"
 * to trade of quality for frame rate in areas of high motion.
 * We then age the block and after it hasn't changed for a few frames
 * we send a higher quality version.  Finally, the process that scans
 * the background filling in blocks sends the highest quality version.
 *
 * A finite state machine defines the algorithm.  When motion is detected,
 * the block is reset to state MOTION.  On no motion, it is aged to AGE1,
 * AGE2, ... etc., until it reaches state AGEn (the age threshold), and
 * then it transitions to IDLE.  If the background process finds a block
 * in the IDLE state, it can promote it to the BG state.  On the next
 * frame, it reverts to the IDLE state.
 *
 * Blocks are transmitted only in the MOTION, AGEn, and BG state,
 * at low, medium, and high quality, respectively.  The high bit
 * is reserved to indicate the block is in one of these states
 * and should be sent.
 */

/************ Change log
 *
 * $Log: crdef.h,v $
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
 * Revision 1.3  2003/03/14 07:25:55  robertj
 * Removed $header keyword so is not different on alternate repositories
 *
 * Revision 1.2  2000/08/25 03:18:49  dereks
 * Add change log facility (Thanks Robert for the info on implementation)
 *
 *
 *
 ********/



#define CR_MOTION	0
#define CR_AGETHRESH	31
#define CR_IDLE		0x40
#define CR_BG		0x41
#define CR_SEND		0x80
#define CR_STATE(s)	((s) & 0x7f)
