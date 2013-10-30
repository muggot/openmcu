/*transmitter.h       (c) 1999-2000 Derek J Smithies (dereks@ibm.net)
 *                           Indranet Technologies ltd (lara@indranet.co.nz)
 *
 * This file is derived from vic, http://www-nrg.ee.lbl.gov/vic/
 * Their copyright notice is below.
 */
/*
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
 * $Log: transmitter.h,v $
 * Revision 1.1  2010/02/24 02:19:22  shorne
 * First commit of h323plus mirror
 *
 * Revision 1.2  2006/07/31 09:09:22  csoutheren
 * Checkin of validated codec used during development
 *
 * Revision 1.1.2.1  2006/04/06 01:17:17  csoutheren
 * Initial version of H.261 video codec plugin for OPAL
 *
 * Revision 2.1  2003/03/15 23:43:00  robertj
 * Update to OpenH323 v1.11.7
 *
 * Revision 1.9  2003/03/14 07:25:55  robertj
 * Removed $header keyword so is not different on alternate repositories
 *
 * Revision 1.8  2002/04/05 00:53:19  dereks
 * Modify video frame encoding so that frame is encoded on an incremental basis.
 * Thanks to Walter Whitlock - good work.
 *
 * Revision 1.7  2002/01/04 02:48:05  dereks
 * Fix previous commit so it compiles OK.
 *
 * Revision 1.6  2002/01/03 23:05:50  dereks
 * Add methods to count number of H261 packets waiting to be sent.
 *
 * Revision 1.5  2001/05/10 05:25:44  robertj
 * Removed need for VIC code to use ptlib.
 *
 * Revision 1.4  2000/12/19 22:22:34  dereks
 * Remove connection to grabber-OS.cxx files. grabber-OS.cxx files no longer used.
 * Video data is now read from a video channel, using the pwlib classes.
 *
 * Revision 1.3  2000/08/25 03:18:50  dereks
 * Add change log facility (Thanks Robert for the info on implementation)
 *
 *
 *
 ********/


#ifndef vic_transmitter_h
#define vic_transmitter_h

#include "config.h"

#define MAXHDR 24


#include "dct.h"
/*
 * The base object for performing the outbound path of
 * the application level protocol.
 */
class Transmitter {
    public:
	Transmitter();
        virtual ~Transmitter();

	struct buffer {
		struct buffer* next;
		/*
		 * make buffer twice as big as necessary so we can
		 * run off end while doing in-place encoding.
		 */
		u_char data[2 * RTP_MTU];
	};
	struct pktbuf {
		struct pktbuf* next;
    	        int lenHdr,lenBuf;
		u_char hdr[MAXHDR];
		buffer* buf;
	};
        virtual int  mtu() { return 1024; }  //Maximum transfer unit.
	void StoreOnePacket(pktbuf*);
        
        //Returns a pointer to buffer in the next packet, + length of buffer;
        //Frees the packet from the queue.

        void GetNextPacket(u_char ** hptr,u_char ** bptr, u_int & hlen, u_int & blen);

        //Returns true if there are packets still in the queue.
        int PacketsOutStanding();

	int GetCountPacketsOutStanding();

	pktbuf* alloch();
	pktbuf* alloc();

    protected:
        void PurgeBufferQueue(pktbuf *queue);

	/*
	 * Buffer allocation hooks.
	 */
	void ReleaseOnePacket(pktbuf*);

	/* packet transmission queue */
	pktbuf* head_;
	pktbuf* tail_;

	 u_int   seqno_;

  private:
	 pktbuf* freehdrs_;
	 buffer* freebufs_;
};



#endif




