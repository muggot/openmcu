/*--------------------------------------------------------------------------------*
 *                                                                                *
 * This material is trade secret owned by imtelephone.com                         *
 * and is strictly confidential and shall remain as such.                         *
 *                                                                                *
 * Copyright © 2003-2006 imtelephone.com. All Rights Reserved. No part of         *
 * this material may be reproduced, stored in a retrieval system, or transmitted, *
 * in any form or by any means, including, but not limited to, photocopying,      *
 *  electronic, mechanical, recording, or otherwise, without the prior written    *
 * permission of imtelephone.com.                                                 *
 *                                                                                *
 * This material is subject to continuous developments and improvements. All      *
 * warranties implied or expressed, including but not limited to implied          *
 * warranties of merchantability, or fitness for purpose, are excluded.           *
 *                                                                                *
 *--------------------------------------------------------------------------------*
 *                                                                                *
 * support@imtelephone.com                                                        *
 *                                                                                *
 *--------------------------------------------------------------------------------*
 *
 *--------------------------------------------------------------------------------*
 *                            EasyG722.h	                    		  *
 *                         ~~~~~~~~~~~~~~~~~~                                     *
 *--------------------------------------------------------------------------------*/


/* EasyG722 API functions prototypes and constants */

#define  L_G722_FRAME_COMPRESSED    80
#define  L_G722_FRAME               160

/* This codec only support G722 64K */

#define  CODER_HANDLE  unsigned long

extern CODER_HANDLE EasyG722_init_encoder( );
extern bool   EasyG722_encoder(CODER_HANDLE hEncoder, short *speech, unsigned char *bitstream);
extern bool   EasyG722_release_encoder(CODER_HANDLE hEncoder);

extern CODER_HANDLE EasyG722_init_decoder( );
extern bool   EasyG722_decoder(CODER_HANDLE hDecoder, unsigned char *bitstream, short *synth_short);
extern bool   EasyG722_release_decoder(CODER_HANDLE hDecoder);

