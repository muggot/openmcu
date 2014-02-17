/*
 * ===================================================================
 *  TS 26.104
 *  R99   V3.3.0 2001-09
 *  REL-4 V4.2.0 2001-09
 *  3GPP AMR Floating-point Speech Codec
 * ===================================================================
 *
 */

/*
 * sp_enc.h
 *
 *
 * Project:
 *    AMR Floating-Point Codec
 *
 * Contains:
 *    Defines interface to AMR encoder
 *
 */
#ifndef _SP_ENC_H
#define _SP_ENC_H

/*
 * include files
 */
#include "typedef.h"

/*
 * definition of modes for encoder
 */
enum Mode { MR475 = 0,
            MR515,
            MR59,
            MR67,
            MR74,
            MR795,
            MR102,
            MR122,
            MRDTX
};
#ifdef  __cplusplus
extern "C" {
#endif
/*
 * Function prototypes
 */

/*
 * initialize one instance of the speech encoder
 * Stores pointer to filter status struct in *st. This pointer has to
 * be passed to Speech_Encode_Frame in each call.
 * returns 0 on success
 */
extern void *Speech_Encode_Frame_init (int dtx);
/*
 * reset speech encoder (i.e. set state memory to zero)
 * returns 0 on success
 */
extern int Speech_Encode_Frame_reset(void *st, int dtx);

/*
 * de-initialize speech encoder (i.e. free status struct)
 * stores NULL in *st
 */
extern void Speech_Encode_Frame_exit (void **st);

/*
 * Encodes one speech frame
 * Returns analysis parameters
 */
extern void Speech_Encode_Frame (void *st, enum Mode mode, const short *newSpeech,
                   short *prm, enum Mode *usedMode);

#ifdef  __cplusplus
}
#endif
#endif // !_SP_ENC_H
