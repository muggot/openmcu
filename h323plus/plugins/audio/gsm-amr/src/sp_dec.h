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
#ifndef _SP_DEC_H_
#define _SP_DEC_H_
/*
 * definition of enumerated types
 */

/*
 * definition of modes for decoder
 */
enum Mode { MR475 = 0,
            MR515,
            MR59,
            MR67,
            MR74,
            MR795,
            MR102,
            MR122,
            MRDTX,
            N_MODES     /* number of (SPC) modes */
};

/* Declaration recieved frame types */
enum RXFrameType { RX_SPEECH_GOOD = 0,
                   RX_SPEECH_DEGRADED,
                   RX_ONSET,
                   RX_SPEECH_BAD,
                   RX_SID_FIRST,
                   RX_SID_UPDATE,
                   RX_SID_BAD,
                   RX_NO_DATA,
                   RX_N_FRAMETYPES     /* number of frame types */
};

#ifdef  __cplusplus
extern "C" {
#endif
/*
 * Function prototypes
 */
/*
 * initialize one instance of the speech decoder
 */
extern void* Speech_Decode_Frame_init ();

/*
 * free status struct
 */
extern void Speech_Decode_Frame_exit (void **st);

/*
 * Decodes one frame from encoded parameters
 */
extern void Speech_Decode_Frame (void *st, enum Mode mode, short *serial,
                   enum RXFrameType frame_type, short *synth);

/*
 * reset speech decoder
 */
extern int Speech_Decode_Frame_reset (void **st);
#ifdef  __cplusplus
}
#endif

#endif // !_SP_DEC_H_

