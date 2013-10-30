/*transmitter.cc       (c) 1999-2000 Derek J Smithies (dereks@ibm.net)
 *                           Indranet Technologies ltd (lara@indranet.co.nz)
 *
 * This file is derived from vic, http://www-nrg.ee.lbl.gov/vic/
 * Their copyright notice is below.
 */
/*-
 * Copyright (c) 1993-1994 The Regents of the University of California.
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
 */

/************ Change log
 *
 * $Log: transmitter.cxx,v $
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
 * Revision 1.8  2003/03/14 07:25:55  robertj
 * Removed $header keyword so is not different on alternate repositories
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

#include "transmitter.h"


Transmitter::Transmitter() :
	head_(0),
	tail_(0)
{
  freehdrs_ = NULL;
  freebufs_ = NULL;
  seqno_ = 1;
}

Transmitter::~Transmitter() 
{
  PurgeBufferQueue(freehdrs_);
  PurgeBufferQueue(head_);
}
  
void Transmitter::PurgeBufferQueue(pktbuf *queue)
{
  pktbuf *pb = queue;

  while (pb) {
    pktbuf *tpb=pb->next;
    if(pb->buf) 
       delete(pb->buf);

    delete pb;
    pb=tpb;
  }
}

Transmitter::pktbuf* Transmitter::alloch()
{
  pktbuf* pb = freehdrs_;
  if (pb == 0) 
    pb = new pktbuf;
  else
    freehdrs_ = pb->next;		
  pb->buf = 0;
  
  return (pb);
}

Transmitter::pktbuf* Transmitter::alloc()
{
  pktbuf* pb = alloch();
  buffer* p = freebufs_;
  if (p == 0) 
    p = new buffer;
  else
    freebufs_ = p->next;
  
  pb->buf = p;
  return (pb);
}


void Transmitter::ReleaseOnePacket(pktbuf* pb)
{
  head_=head_->next;

  pb->next = freehdrs_;
  freehdrs_ = pb;
  buffer* p = pb->buf;
  if (p != 0) {
    p->next = freebufs_;
    freebufs_ = p;
  }
}


//Add packet to pending queue.
void Transmitter::StoreOnePacket(pktbuf* pb)
{
  if (head_ != 0) {
  	tail_->next = pb;
  	tail_ = pb;
  } else
  	tail_ = head_ = pb;
  pb->next = 0;
}

int Transmitter::PacketsOutStanding()
{
  return(head_!=NULL);
}

int Transmitter::GetCountPacketsOutStanding()
{
  pktbuf *pkt_ptr = head_;
  int    count = 0;

  while(pkt_ptr != NULL) {
    pkt_ptr = pkt_ptr->next;
    count++;
  }
  
  return count;
}

void Transmitter::GetNextPacket(u_char ** hptr,u_char ** bptr, u_int & hlen, u_int & blen)
{
  if( head_ != 0 ) {
    *hptr=(u_char*)head_->hdr;        //there is a packet to read.
    *bptr=(u_char*)head_->buf->data;
    hlen=head_->lenHdr;
    blen=head_->lenBuf;
    ReleaseOnePacket(head_);
  } else {
    hlen = 0;   //There is no packet to read.
    blen = 0;   
  }

}









