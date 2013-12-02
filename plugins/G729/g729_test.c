#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <fcntl.h>

#include "typedef.h"
#include "basic_op.h"
#include "ld8a.h"
#include "tab_ld8a.h"
#include "util.h"
#include "pre_proc.h"

#define TEST_ENC

#define SAMPLES_PER_FRAME   L_FRAME
#define L_FRAME_COMPRESSED  10
#define BYTES_PER_FRAME     10
#define NANOSECONDSPERFRAME 10000
#define BITS_PER_SECOND     8000

#ifdef TEST_ENC

static int codec_encoder(CodState *coder, const void *from,
			 unsigned *fromLen,
			 void *to, unsigned *toLen, unsigned int *flag)
{
	Word16 i;
	Word16 parm[PRM_SIZE];

	if (*fromLen < SAMPLES_PER_FRAME * 2 || *toLen < L_FRAME_COMPRESSED)
		return 0;

	Copy ((Word16 *)from, coder->new_speech, SAMPLES_PER_FRAME);
	Pre_Process(coder, coder->new_speech, L_FRAME);
	Coder_ld8a(coder, parm);
	Store_Params(parm, to);
	
	for (i=0; i<PRM_SIZE; i++) 
	{
	    printf ("%d ", parm[i]);
	}
	printf ("\n");

	*fromLen = SAMPLES_PER_FRAME * 2;
	*toLen = L_FRAME_COMPRESSED;

	return 1;
}

#endif

#ifdef TEST_DEC

static int codec_decoder(DecState *decoder, const void *from,
			 unsigned *fromLen,
			 void *to, unsigned *toLen, unsigned int *flag)
{
	Word16 i;
	Word16 *synth;
	Word16 parm[PRM_SIZE + 1];

	if (*fromLen < BYTES_PER_FRAME || *toLen < SAMPLES_PER_FRAME * 2)
		return 0;

	Restore_Params(from, &parm[1]);

	synth = decoder->synth_buf + M;

	parm[0] = 1;
	for (i = 0; i < PRM_SIZE; i++) {
		if (parm[i + 1] != 0) {
			parm[0] = 0;
			break;
		}
	}

	parm[4] = Check_Parity_Pitch(parm[3], parm[4]);	

	Decod_ld8a(decoder, parm, synth, decoder->Az_dec, decoder->T2, &decoder->bad_lsf);
	Post_Filter(decoder, synth, decoder->Az_dec, decoder->T2);
	Post_Process(decoder, synth, L_FRAME);

	for (i = 0; i < SAMPLES_PER_FRAME; i++)
		((Word16 *) to)[i] = synth[i];

	*fromLen = BYTES_PER_FRAME;
	*toLen = SAMPLES_PER_FRAME * 2;

	return 1;
}

#endif

int main (int argc, char *argv[])
{
    int in_fd, out_fd;

#ifdef TEST_ENC
    CodState *coder;
    char in_data [SAMPLES_PER_FRAME * 2];
    char out_data [L_FRAME_COMPRESSED];   
    unsigned int in_len = SAMPLES_PER_FRAME * 2;
    unsigned int out_len = L_FRAME_COMPRESSED;
#endif    

#ifdef TEST_DEC
    DecState *decoder;
    char in_data [L_FRAME_COMPRESSED];   
    char out_data [SAMPLES_PER_FRAME * 2];
    unsigned int in_len = L_FRAME_COMPRESSED;
    unsigned int out_len = SAMPLES_PER_FRAME * 2;
#endif    

    if (argc != 3) {
	    fprintf (stderr, "Usage g729_test infile outfile\n");
	    exit (1);
    }
    
    in_fd = open (argv[1], O_RDONLY);
    out_fd = open (argv[2], O_CREAT | O_WRONLY, 0644);

#ifdef TEST_ENC
    coder = Init_Coder_ld8a();
    Init_Pre_Process(coder);
    
    while (read (in_fd, in_data, SAMPLES_PER_FRAME * 2) > 0) {
	   codec_encoder (coder, (void *)in_data, &in_len, 
			  (void *)out_data, &out_len, 0);
	   write (out_fd, out_data, L_FRAME_COMPRESSED);
    }
#endif

#ifdef TEST_DEC
    decoder = Init_Decod_ld8a();
    Init_Post_Filter(decoder);
    Init_Post_Process(decoder);

    while (read (in_fd, in_data, L_FRAME_COMPRESSED) > 0) {
	   codec_decoder (decoder, (void *)in_data, &in_len, 
			  (void *)out_data, &out_len, 0);
	   write (out_fd, out_data, SAMPLES_PER_FRAME * 2);
    }
#endif
    
    close (in_fd);
    close (out_fd);
    return 0;
}

