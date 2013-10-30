/*encoder-h261.h       (c) 1999-2000 Derek J Smithies (dereks@ibm.net)
 *                           Indranet Technologies ltd (lara@indranet.co.nz)
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
 * $Log: encoder-h261.h,v $
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
 * Revision 1.12  2002/05/17 01:47:33  dereks
 * backout the integer maths in the h261 codec.
 *
 * Revision 1.11  2002/04/26 04:57:41  dereks
 * Add Walter Whitlocks fixes, based on Victor Ivashim's suggestions to
 * improve the quality with Netmeeting. Thanks guys!!!!
 *
 * Revision 1.10  2002/04/05 00:53:19  dereks
 * Modify video frame encoding so that frame is encoded on an incremental basis.
 * Thanks to Walter Whitlock - good work.
 *
 * Revision 1.9  2001/10/16 23:51:42  dereks
 * Change vic's fdct() from floating-point to fix-point. Improves performance
 * for h261 video significantly on some machines. Thanks to Cosmos Jiang
 *
 * Revision 1.8  2001/10/16 21:20:07  yurik
 * Removed warnings on Windows CE. Submitted by Jehan Bing, jehan@bravobrava.com
 *
 * Revision 1.6  2001/05/10 05:25:44  robertj
 * Removed need for VIC code to use ptlib.
 *
 * Revision 1.5  2000/08/25 03:18:49  dereks
 * Add change log facility (Thanks Robert for the info on implementation)
 *
 *
 *
 ********/

#include "bsd-endian.h" 
#include "dct.h"
#include "p64-huff.h"
#include "crdef.h"
#include "transmitter.h"
#include "encoder.h"

/*
 * H.261 encapsulation.in payload of h261 packet.
 * See Internet draft.
 *
 *  0                   1                   2                   3
 *  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |SBIT |EBIT |I|V| GOBN  |  MBAP   |  QUANT  |  HMVD   |  VMVD   |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 */

// define H261 header related values
// (currently these are defined in both p64.h and encoder-h261.h for use
//  in decode and encode.  Should there be one common include file?)
typedef unsigned h261hdr_t;

#define HDRSIZE         4
#define	CIF_WIDTH	352
#define	CIF_HEIGHT	288
#define	QCIF_WIDTH	176
#define	QCIF_HEIGHT	144
#define	BMB	6	/* # blocks in a MB */
#define MBPERGOB	33	/* # of Macroblocks per GOB */

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

/** append the "n" LSbits of "bits" to the place pointed to by bc
    post-increment "bc" as needed.  "bb" is used for intermediate storage
    "nbb" keeps track of bits in "bb"
    note that ALL bits in "n" are OR'ed into the result
 */
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


class H261Encoder :public  Encoder { 
    public:
	void setq(int q);
    protected:
	int encode(const VideoFrame*, const u_char *crvec);
	H261Encoder(Transmitter *T);
        virtual ~H261Encoder(void)=0;
	void encode_blk(const short* blk, const char* lm);
	int flush(Transmitter::pktbuf* pb, int nbit, Transmitter::pktbuf* npb);
	char* make_level_map(int q, u_int fthresh);
	void setquantizers(int lq, int mq, int hq);

	virtual void SetSize(int /*w*/, int /*h*/){};
	virtual void encode_mb(u_int /*mba*/, const u_char* /*frm*/,
			       u_int /*loff*/, u_int /*coff*/, int /*how*/){};

	/* bit buffer */
	BB_INT bb_; // intermediate working space for encoding bits
	u_int nbb_; // # of valid bits in bb_

	u_char* bs_; // used as pointer to start of buffer
	u_char* bc_; // where to put encoded bits in buffer
	int sbit_; // SBIT value obtained from previous packet for use on next packet

	u_char lq_;		/* low quality quantizer */
	u_char mq_;		/* medium quality quantizer */
	u_char hq_;		/* high quality quantizer */
	u_char mquant_;		/* the last quantizer we sent to other side */
	int quant_required_;	/* 1 if not quant'd in dct */
	u_int ngob_; // number of GOBs for this frame size, (3 or 12)
	u_int mba_;  // address of just encoded macroblock

	u_int cif_;		/* 1 for CIF, 0 for QCIF */
	u_int bstride_;
	u_int lstride_;
	u_int cstride_;

	u_int loffsize_;	/* amount of 1 luma block */
	u_int coffsize_;	/* amount of 1 chroma block */
	u_int bloffsize_;	/* amount of 1 block advance */

	const char* llm_[32];	/* luma dct val -> level maps */
	const char* clm_[32];	/* chroma dct val -> level maps */

	float lqt_[64];		/* low quality quantizer */
	float mqt_[64];		/* medium quality quantizer */
	float hqt_[64];		/* high quality quantizer */

	u_int coff_[12];	/* where to find U given gob# */
	u_int loff_[12];	/* where to find Y given gob# */
	u_int blkno_[12];	/* for CR */

  const VideoFrame *gVf; // current video frame YUV data
  bool gPicture; // if PTrue, send picture layer header
  int gHdrGOBN; // next GOB number for last encoded MB
  int gNxtGOB; // GOB number for next marcroblock to be considered for encoding
  int gGobMax; // maximum GOB number in frame
  bool gGOBhdrNxt; // set PTrue when GOB header is next data
  bool gSendGOBhdr; // set PTrue when GOB header must be encoded before next MB
  int gHdrMBAP; // address of last macroblock encoded in previous packet 1..33
  int gNxtMBA; // address of next macroblock to be considered for encoding
  int gHdrQUANT; // QUANT in effect for next MB in buffer
  int gStep; // Macro Block step size
  bool gDone; // Set PTrue when encoding of frame is done
  int gDbase;  //offset from gData where valid data starts
  int gNbytes; // number of bytes in gData buffer from previous packet
  u_int gloff;
  u_int gcoff;
  u_int gblkno;
  int gline;		
};

class H261DCTEncoder:  public H261Encoder 
{
  public:
    H261DCTEncoder(Transmitter *T);
    int consume(const VideoFrame*);
    void SetSize(int w, int h);
  protected:
    void encode_mb(u_int mba, const u_char* frm,
		       u_int loff, u_int coff, int how);
};

class H261PixelEncoder : public H261Encoder 
{
  public:
    H261PixelEncoder(Transmitter *T);
    int consume(const VideoFrame*);
    void SetSize(int w, int h);
    int PreIncEncodeSetup(const VideoFrame*);
    void IncEncodeAndGetPacket(u_char * buffer, unsigned & length );
    /** When incrementally encoding, return true if there is still 
        more encoding waiting to be done for the current video frame.
    */ 
    int MoreToIncEncode() 
      { return !gDone; }

  protected:
    void encode_mb(u_int mba, const u_char* frm,
		       u_int loff, u_int coff, int how);
    //
    u_char gData[2 * RTP_MTU];
};


