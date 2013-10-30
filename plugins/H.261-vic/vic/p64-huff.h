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
 *
 */

/************ Change log
 *
 * $Log: p64-huff.h,v $
 * Revision 1.1  2010/02/24 02:19:22  shorne
 * First commit of h323plus mirror
 *
 * Revision 1.2  2006/07/31 09:09:22  csoutheren
 * Checkin of validated codec used during development
 *
 * Revision 1.1.2.1  2006/04/06 01:17:17  csoutheren
 * Initial version of H.261 video codec plugin for OPAL
 *
 * Revision 2.1  2003/03/15 23:42:59  robertj
 * Update to OpenH323 v1.11.7
 *
 * Revision 1.5  2003/03/14 07:25:55  robertj
 * Removed $header keyword so is not different on alternate repositories
 *
 * Revision 1.4  2002/10/10 05:38:30  robertj
 * Removed non-ansi-C comments to maximise portability, thanks Martijn Roest
 *
 * Revision 1.3  2002/04/05 00:53:19  dereks
 * Modify video frame encoding so that frame is encoded on an incremental basis.
 * Thanks to Walter Whitlock - good work.
 *
 * Revision 1.2  2000/08/25 03:18:50  dereks
 * Add change log facility (Thanks Robert for the info on implementation)
 *
 *
 *
 ********/


#ifdef __cplusplus
extern "C" {
#endif

#define SYM_ESCAPE	0
#define SYM_EOB		-1
#define SYM_STUFFBITS	0
#define SYM_STARTCODE	-1
#define SYM_ILLEGAL	-2
#define SYM_EOMB	-3

/*
 * Flags that indicate which types of encoding apply
 * to a given macroblock type code.
 *
 * MT_QUANT	new quantization factor present
 * MT_CBP	bit vector of which blocks present
 * MT_INTRA	this block is intra-coded (e.g., not differenced)
 * MT_MFM	motion vectors present (e.g., displacement of difference)
 * MT_FILTER	old block should be filtered before summing with xmitted blk
 * MT_TCOEFF	block present
 */
#define MT_TCOEFF	0x01
#define MT_CBP		0x02
#define MT_MVD		0x04
#define MT_MQUANT	0x08
#define MT_FILTER	0x10
#define MT_INTRA	0x20

#ifndef HUFFSTRINGS

struct huffent {
	int val; /* Huffman code value */
	int nb;  /* number of bits */
};

extern const unsigned char skiptab[];

/*
 * Lookup tables that map an encoded prefix of the bit string,
 * into the next symbol (for decoding).
 */
extern const short htd_mtype[];
extern const short htd_mba[];
extern const short htd_cbp[];
extern const short htd_dvm[];
extern const short htd_tcoeff[];
extern const short htd_tcoeff_noeob[];

extern const int htd_mtype_width;
extern const int htd_mba_width;
extern const int htd_cbp_width;
extern const int htd_dvm_width;
extern const int htd_tcoeff_width;
extern const int htd_tcoeff_noeob_width;

/*
 * Look up tables that produce a huffman encoding string
 * from a symbol or group of symbols (for encoding).
 */
extern struct huffent hte_mba[];
extern struct huffent hte_tc[];

#else
struct huffcode {
	int val;
	char* str;
};

static struct huffcode hc_mtype[] = {
	{ MT_CBP|MT_TCOEFF, "1" },
	{ MT_FILTER|MT_MVD|MT_CBP|MT_TCOEFF, "01" },
	{ MT_FILTER|MT_MVD, "001" },
	{ MT_INTRA|MT_TCOEFF, "0001" },
	{ MT_MQUANT|MT_CBP|MT_TCOEFF, "00001" },
	{ MT_MQUANT|MT_FILTER|MT_MVD|MT_CBP|MT_TCOEFF, "000001" },
	{ MT_INTRA|MT_MQUANT|MT_TCOEFF, "0000001" },
	{ MT_MVD|MT_CBP|MT_TCOEFF, "00000001" },
	{ MT_MVD, "000000001" },
	{ MT_MQUANT|MT_CBP|MT_MVD|MT_TCOEFF, "0000000001" },
	{ 0, 0 }
};

static struct huffcode hc_mba[] = {
	{ SYM_STUFFBITS, "00000001111" },
	{ SYM_STARTCODE, "0000000000000001" },
	{ 1, "1" },
	{ 2, "011" }, 
	{ 3, "010" },
	{ 4, "0011" },
	{ 5, "0010" }, 
	{ 6, "00011" },
	{ 7, "00010" },
	{ 8, "0000111" }, 
	{ 9, "0000110" },
	{ 10, "00001011" },
	{ 11, "00001010" }, 
	{ 12, "00001001" },
	{ 13, "00001000" },
	{ 14, "00000111" }, 
	{ 15, "00000110" },
	{ 16, "0000010111" },
	{ 17, "0000010110" }, 
	{ 18, "0000010101" },
	{ 19, "0000010100" },
	{ 20, "0000010011" }, 
	{ 21, "0000010010" },
	{ 22, "00000100011" },
	{ 23, "00000100010" }, 
	{ 24, "00000100001" },
	{ 25, "00000100000" },
	{ 26, "00000011111" }, 
	{ 27, "00000011110" },
	{ 28, "00000011101" },
	{ 29, "00000011100" }, 
	{ 30, "00000011011" },
	{ 31, "00000011010" },
	{ 32, "00000011001" }, 
	{ 33, "00000011000" },
	{ 0, 0 }
};

static huffcode hc_cbp[] = {
	{ 1, "01011" },
	{ 2, "01001" },
	{ 3, "001101" }, 
	{ 4, "1101" },
	{ 5, "0010111" },
	{ 6, "0010011" }, 
	{ 7, "00011111" },
	{ 8, "1100" },
	{ 9, "0010110" }, 
	{ 10, "0010010" },
	{ 11, "00011110" },
	{ 12, "10011" }, 
	{ 13, "00011011" },
	{ 14, "00010111" },
	{ 15, "00010011" }, 
	{ 16, "1011" },
	{ 17, "0010101" },
	{ 18, "0010001" }, 
	{ 19, "00011101" },
	{ 20, "10001" },
	{ 21, "00011001" }, 
	{ 22, "00010101" },
	{ 23, "00010001" },
	{ 24, "001111" }, 
	{ 25, "00001111" },
	{ 26, "00001101" },
	{ 27, "000000011" }, 
	{ 28, "01111" },
	{ 29, "00001011" },
	{ 30, "00000111" }, 
	{ 31, "000000111" },
	{ 32, "1010" },
	{ 33, "0010100" }, 
	{ 34, "0010000" },
	{ 35, "00011100" },
	{ 36, "001110" }, 
	{ 37, "00001110" },
	{ 38, "00001100" },
	{ 39, "000000010" }, 
	{ 40, "10000" },
	{ 41, "00011000" },
	{ 42, "00010100" }, 
	{ 43, "00010000" },
	{ 44, "01110" },
	{ 45, "00001010" }, 
	{ 46, "00000110" },
	{ 47, "000000110" },
	{ 48, "10010" }, 
	{ 49, "00011010" },
	{ 50, "00010110" },
	{ 51, "00010010" }, 
	{ 52, "01101" },
	{ 53, "00001001" },
	{ 54, "00000101" }, 
	{ 55, "000000101" },
	{ 56, "01100" },
	{ 57, "00001000" }, 
	{ 58, "00000100" },
	{ 59, "000000100" },
	{ 60, "111" }, 
	{ 61, "01010" },
	{ 62, "01000" },
	{ 63, "001100" },
	{ 0, 0 }
};

static struct huffcode hc_dvm[] = {
	{ -16, "00000011001" },
	{ -15, "00000011011" },
	{ -14, "00000011101" }, 
	{ -13, "00000011111" },
	{ -12, "00000100001" },
	{ -11, "00000100011" }, 
	{ -10, "0000010011" },
	{ -9, "0000010101" },
	{ -8, "0000010111" }, 
	{ -7, "00000111" },
	{ -6, "00001001" },
	{ -5, "00001011" }, 
	{ -4, "0000111" },
	{ -3, "00011" },
	{ -2, "0011" }, 
	{ -1, "011" },
	{ 0, "1" },
	{ 1, "010" }, 
	{ 2, "0010" },
	{ 3, "00010" },
	{ 4, "0000110" }, 
	{ 5, "00001010" },
	{ 6, "00001000" },
	{ 7, "00000110" }, 
	{ 8, "0000010110" },
	{ 9, "0000010100" },
	{ 10, "0000010010" }, 
	{ 11, "00000100010" },
	{ 12, "00000100000" },
	{ 13, "00000011110" }, 
	{ 14, "00000011100" },
	{ 15, "00000011010" },
	{ 0, 0 }
};

/*
 * The signs of the values below are determined by the last
 * bit in the huffman code.  Rather than special case them
 * in the decoding process, we make this table twice as big.
 * (Note that we have to be careful that SYM_ESCAPE = -1
 * or SYM_ILLEGAL = -2 don't crop up here -- they don't.)
 */
static struct huffcode hc_tcoeff[] = {
	{ SYM_EOB, "10" },
	{ SYM_ESCAPE, "000001" },
#define TC_RUN(x) (x)
#define TC_LEVEL(x) (((x) & 0x1f) << 5)
	{ TC_RUN(0)|TC_LEVEL(1),  "110" }, 
	{ TC_RUN(0)|TC_LEVEL(-1), "111" }, 
	{ TC_RUN(0)|TC_LEVEL(2),  "01000" },
	{ TC_RUN(0)|TC_LEVEL(-2), "01001" },
	{ TC_RUN(0)|TC_LEVEL(3),  "001010" },
	{ TC_RUN(0)|TC_LEVEL(-3), "001011" },
	{ TC_RUN(0)|TC_LEVEL(4),  "00001100" },
	{ TC_RUN(0)|TC_LEVEL(-4), "00001101" },
	{ TC_RUN(0)|TC_LEVEL(5),  "001001100" },
	{ TC_RUN(0)|TC_LEVEL(-5), "001001101" },
	{ TC_RUN(0)|TC_LEVEL(6),  "001000010" },
	{ TC_RUN(0)|TC_LEVEL(-6), "001000011" },
	{ TC_RUN(0)|TC_LEVEL(7),  "00000010100" },
	{ TC_RUN(0)|TC_LEVEL(-7), "00000010101" },
	{ TC_RUN(0)|TC_LEVEL(8),  "0000000111010" },
	{ TC_RUN(0)|TC_LEVEL(-8), "0000000111011" },
	{ TC_RUN(0)|TC_LEVEL(9),  "0000000110000" },
	{ TC_RUN(0)|TC_LEVEL(-9), "0000000110001" },
	{ TC_RUN(0)|TC_LEVEL(10),  "0000000100110" },
	{ TC_RUN(0)|TC_LEVEL(-10), "0000000100111" },
	{ TC_RUN(0)|TC_LEVEL(11),  "0000000100000" },
	{ TC_RUN(0)|TC_LEVEL(-11), "0000000100001" },
	{ TC_RUN(0)|TC_LEVEL(12),  "00000000110100" },
	{ TC_RUN(0)|TC_LEVEL(-12), "00000000110101" },
	{ TC_RUN(0)|TC_LEVEL(13),  "00000000110010" },
	{ TC_RUN(0)|TC_LEVEL(-13), "00000000110011" },
	{ TC_RUN(0)|TC_LEVEL(14),  "00000000110000" },
	{ TC_RUN(0)|TC_LEVEL(-14), "00000000110001" },
	{ TC_RUN(0)|TC_LEVEL(15),  "00000000101110" },
	{ TC_RUN(0)|TC_LEVEL(-15), "00000000101111" },
	{ TC_RUN(1)|TC_LEVEL(1),  "0110" },
	{ TC_RUN(1)|TC_LEVEL(-1), "0111" },
	{ TC_RUN(1)|TC_LEVEL(2),  "0001100" },
	{ TC_RUN(1)|TC_LEVEL(-2), "0001101" },
	{ TC_RUN(1)|TC_LEVEL(3),  "001001010" },
	{ TC_RUN(1)|TC_LEVEL(-3), "001001011" },
	{ TC_RUN(1)|TC_LEVEL(4),  "00000011000" },
	{ TC_RUN(1)|TC_LEVEL(-4), "00000011001" },
	{ TC_RUN(1)|TC_LEVEL(5),  "0000000110110" },
	{ TC_RUN(1)|TC_LEVEL(-5), "0000000110111" },
	{ TC_RUN(1)|TC_LEVEL(6),  "00000000101100" },
	{ TC_RUN(1)|TC_LEVEL(-6), "00000000101101" },
	{ TC_RUN(1)|TC_LEVEL(7),  "00000000101010" },
	{ TC_RUN(1)|TC_LEVEL(-7), "00000000101011" },
	{ TC_RUN(2)|TC_LEVEL(1),  "01010" },
	{ TC_RUN(2)|TC_LEVEL(-1), "01011" },
	{ TC_RUN(2)|TC_LEVEL(2),  "00001000" },
	{ TC_RUN(2)|TC_LEVEL(-2), "00001001" },
	{ TC_RUN(2)|TC_LEVEL(3),  "00000010110" },
	{ TC_RUN(2)|TC_LEVEL(-3), "00000010111" },
	{ TC_RUN(2)|TC_LEVEL(4),  "0000000101000" },
	{ TC_RUN(2)|TC_LEVEL(-4), "0000000101001" },
	{ TC_RUN(2)|TC_LEVEL(5),  "00000000101000" },
	{ TC_RUN(2)|TC_LEVEL(-5), "00000000101001" },
	{ TC_RUN(3)|TC_LEVEL(1),  "001110" },
	{ TC_RUN(3)|TC_LEVEL(-1), "001111" },
	{ TC_RUN(3)|TC_LEVEL(2),  "001001000" },
	{ TC_RUN(3)|TC_LEVEL(-2), "001001001" },
	{ TC_RUN(3)|TC_LEVEL(3),  "0000000111000" },
	{ TC_RUN(3)|TC_LEVEL(-3), "0000000111001" },
	{ TC_RUN(3)|TC_LEVEL(4),  "00000000100110" },
	{ TC_RUN(3)|TC_LEVEL(-4), "00000000100111" },
	{ TC_RUN(4)|TC_LEVEL(1),  "001100" },
	{ TC_RUN(4)|TC_LEVEL(-1), "001101" },
	{ TC_RUN(4)|TC_LEVEL(2),  "00000011110" },
	{ TC_RUN(4)|TC_LEVEL(-2), "00000011111" },
	{ TC_RUN(4)|TC_LEVEL(3),  "0000000100100" },
	{ TC_RUN(4)|TC_LEVEL(-3), "0000000100101" },
	{ TC_RUN(5)|TC_LEVEL(1),  "0001110" },
	{ TC_RUN(5)|TC_LEVEL(-1), "0001111" },
	{ TC_RUN(5)|TC_LEVEL(2),  "00000010010" },
	{ TC_RUN(5)|TC_LEVEL(-2), "00000010011" },
	{ TC_RUN(5)|TC_LEVEL(3),  "00000000100100" },
	{ TC_RUN(5)|TC_LEVEL(-3), "00000000100101" },
	{ TC_RUN(6)|TC_LEVEL(1),  "0001010" },
	{ TC_RUN(6)|TC_LEVEL(-1), "0001011" },
	{ TC_RUN(6)|TC_LEVEL(2),  "0000000111100" },
	{ TC_RUN(6)|TC_LEVEL(-2), "0000000111101" },
	{ TC_RUN(7)|TC_LEVEL(1),  "0001000" },
	{ TC_RUN(7)|TC_LEVEL(-1), "0001001" },
	{ TC_RUN(7)|TC_LEVEL(2),  "0000000101010" },
	{ TC_RUN(7)|TC_LEVEL(-2), "0000000101011" },
	{ TC_RUN(8)|TC_LEVEL(1),  "00001110" },
	{ TC_RUN(8)|TC_LEVEL(-1), "00001111" },
	{ TC_RUN(8)|TC_LEVEL(2),  "0000000100010" },
	{ TC_RUN(8)|TC_LEVEL(-2), "0000000100011" },
	{ TC_RUN(9)|TC_LEVEL(1),  "00001010" },
	{ TC_RUN(9)|TC_LEVEL(-1), "00001011" },
	{ TC_RUN(9)|TC_LEVEL(2),  "00000000100010" },
	{ TC_RUN(9)|TC_LEVEL(-2), "00000000100011" },
	{ TC_RUN(10)|TC_LEVEL(1),  "001001110" },
	{ TC_RUN(10)|TC_LEVEL(-1), "001001111" },
	{ TC_RUN(10)|TC_LEVEL(2),  "00000000100000" },
	{ TC_RUN(10)|TC_LEVEL(-2), "00000000100001" },
	{ TC_RUN(11)|TC_LEVEL(1),  "001000110" },
	{ TC_RUN(11)|TC_LEVEL(-1), "001000111" },
	{ TC_RUN(12)|TC_LEVEL(1),  "001000100" },
	{ TC_RUN(12)|TC_LEVEL(-1), "001000101" },
	{ TC_RUN(13)|TC_LEVEL(1),  "001000000" },
	{ TC_RUN(13)|TC_LEVEL(-1), "001000001" },
	{ TC_RUN(14)|TC_LEVEL(1),  "00000011100" },
	{ TC_RUN(14)|TC_LEVEL(-1), "00000011101" },
	{ TC_RUN(15)|TC_LEVEL(1),  "00000011010" },
	{ TC_RUN(15)|TC_LEVEL(-1), "00000011011" },
	{ TC_RUN(16)|TC_LEVEL(1),  "00000010000" },
	{ TC_RUN(16)|TC_LEVEL(-1), "00000010001" },
	{ TC_RUN(17)|TC_LEVEL(1),  "0000000111110" },
	{ TC_RUN(17)|TC_LEVEL(-1), "0000000111111" },
	{ TC_RUN(18)|TC_LEVEL(1),  "0000000110100" },
	{ TC_RUN(18)|TC_LEVEL(-1), "0000000110101" },
	{ TC_RUN(19)|TC_LEVEL(1),  "0000000110010" },
	{ TC_RUN(19)|TC_LEVEL(-1), "0000000110011" },
	{ TC_RUN(20)|TC_LEVEL(1),  "0000000101110" },
	{ TC_RUN(20)|TC_LEVEL(-1), "0000000101111" },
	{ TC_RUN(21)|TC_LEVEL(1),  "0000000101100" },
	{ TC_RUN(21)|TC_LEVEL(-1), "0000000101101" },
	{ TC_RUN(22)|TC_LEVEL(1),  "00000000111110" },
	{ TC_RUN(22)|TC_LEVEL(-1), "00000000111111" },
	{ TC_RUN(23)|TC_LEVEL(1),  "00000000111100" },
	{ TC_RUN(23)|TC_LEVEL(-1), "00000000111101" },
	{ TC_RUN(24)|TC_LEVEL(1),  "00000000111010" },
	{ TC_RUN(24)|TC_LEVEL(-1), "00000000111011" },
	{ TC_RUN(25)|TC_LEVEL(1),  "00000000111000" },
	{ TC_RUN(25)|TC_LEVEL(-1), "00000000111001" },
	{ TC_RUN(26)|TC_LEVEL(1),  "00000000110110" },
	{ TC_RUN(26)|TC_LEVEL(-1), "00000000110111" },
	{ 0, 0 }
};
#endif
#ifdef __cplusplus
}
#endif
