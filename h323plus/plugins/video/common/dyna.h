/*
 * Common Plugin code for OpenH323/OPAL
 *
 * This code is based on the following files from the OPAL project which
 * have been removed from the current build and distributions but are still
 * available in the CVS "attic"
 * 
 *    src/codecs/h263codec.cxx 
 *    include/codecs/h263codec.h 

 * The original files, and this version of the original code, are released under the same 
 * MPL 1.0 license. Substantial portions of the original code were contributed
 * by Salyens and March Networks and their right to be identified as copyright holders
 * of the original code portions and any parts now included in this new copy is asserted through 
 * their inclusion in the copyright notices below.
 *
 * Copyright (C) 2006 Post Increment
 * Copyright (C) 2005 Salyens
 * Copyright (C) 2001 March Networks Corporation
 * Copyright (C) 1999-2000 Equivalence Pty. Ltd.
 *
 * The contents of this file are subject to the Mozilla Public License
 * Version 1.0 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See
 * the License for the specific language governing rights and limitations
 * under the License.
 *
 * The Original Code is Open H323 Library.
 *
 * The Initial Developer of the Original Code is Equivalence Pty. Ltd.
 *
 * Contributor(s): Guilhem Tardy (gtardy@salyens.com)
 *                 Craig Southeren (craigs@postincrement.com)
 *                 Matthias Schneider (ma30002000@yahoo.de)
 */

#ifndef __DYNA_H__
#define __DYNA_H__ 1
#include <codec/opalplugin.h>

#include <stdlib.h>
#ifdef _WIN32
#include <windows.h>
#include <malloc.h>
#define STRCMPI  _strcmpi
#else
#include <semaphore.h>
#include <dlfcn.h>
#define STRCMPI  strcasecmp
typedef unsigned char BYTE;

#endif

#include <string.h>
#include "critsect.h"
#include "trace.h"

extern "C" {
#include "ffmpeg/avcodec.h"
};

#include <vector>

// Compile time version checking
#if LIBAVCODEC_VERSION_INT < ((51<<16)+(11<<8)+0)
#error Libavcodec too old.
#endif


#ifdef WITH_STACKALIGN_HACK
/*
* Some combination of gcc 3.3.5, glibc 2.3.5 and PWLib 1.11.3 is throwing
* off stack alignment for ffmpeg when it is dynamically loaded by PWLib.
* Wrapping all ffmpeg calls in this macro should ensure the stack is aligned
* when it reaches ffmpeg. ffmpeg still needs to be compiled with a more
* recent gcc (we used 4.1.1) to ensure it preserves stack boundaries
* internally.
*
* This macro comes from FFTW 3.1.2, kernel/ifft.h. See:
*     http://www.fftw.org/fftw3_doc/Stack-alignment-on-x86.html
* Used with permission.
*/
#define WITH_ALIGNED_STACK(what)                               \
{                                                              \
    (void)__builtin_alloca(16);                                \
     __asm__ __volatile__ ("andl $-16, %esp");                 \
							       \
    what						       \
}
#else
#define WITH_ALIGNED_STACK(what) (what)
#endif

#ifdef  _WIN32
# define DIR_SEPARATOR "\\"
# define DIR_TOKENISER ";"
#else
# define DIR_SEPARATOR "/"
# define DIR_TOKENISER ":"
#endif
	  
	  
/////////////////////////////////////////////////////////////////
//
// define a class to simplify handling a DLL library
// based on PDynaLink from PWLib

class DynaLink
{
  public:
    typedef void (*Function)();

    DynaLink()
    { _hDLL = NULL; }

    ~DynaLink()
    { Close(); }

    virtual bool IsLoaded() const
    { return _hDLL != NULL; }

    virtual bool Open(const char *name);
    bool InternalOpen(const char * dir, const char *name);
    virtual void Close();
    bool GetFunction(const char * name, Function & func);
    
  protected:
#if defined(_WIN32)
    HINSTANCE _hDLL;
#else
    void * _hDLL;
#endif /* _WIN32 */
    CodecID _codec;
    char _codecString [32];
};

/////////////////////////////////////////////////////////////////
//
// define a class to interface to the FFMpeg library


class FFMPEGLibrary : public DynaLink
{
  public:
    FFMPEGLibrary(CodecID codec);
    ~FFMPEGLibrary();

    bool Load();

    AVCodec *AvcodecFindEncoder(enum CodecID id);
    AVCodec *AvcodecFindDecoder(enum CodecID id);
    AVCodecContext *AvcodecAllocContext(void);
    AVFrame *AvcodecAllocFrame(void);
    int AvcodecOpen(AVCodecContext *ctx, AVCodec *codec);
    int AvcodecClose(AVCodecContext *ctx);
    int AvcodecEncodeVideo(AVCodecContext *ctx, BYTE *buf, int buf_size, const AVFrame *pict);
    int AvcodecDecodeVideo(AVCodecContext *ctx, AVFrame *pict, int *got_picture_ptr, BYTE *buf, int buf_size);
    void AvcodecFree(void * ptr);

    void AvLogSetLevel(int level);
    void AvLogSetCallback(void (*callback)(void*, int, const char*, va_list));
    int FFCheckAlignment(void);

    bool IsLoaded();
    CriticalSection processLock;

  protected:
    void (*Favcodec_init)(void);
    AVCodec *Favcodec_h263_encoder;
    AVCodec *Favcodec_h263p_encoder;
    AVCodec *Favcodec_h263_decoder;
    AVCodec *Favcodec_h264_decoder;
    AVCodec *mpeg4_encoder;
    AVCodec *mpeg4_decoder;

    void (*Favcodec_register)(AVCodec *format);
    AVCodec *(*Favcodec_find_encoder)(enum CodecID id);
    AVCodec *(*Favcodec_find_decoder)(enum CodecID id);
    AVCodecContext *(*Favcodec_alloc_context)(void);
    void (*Favcodec_free)(void *);
    AVFrame *(*Favcodec_alloc_frame)(void);
    int (*Favcodec_open)(AVCodecContext *ctx, AVCodec *codec);
    int (*Favcodec_close)(AVCodecContext *ctx);
    int (*Favcodec_encode_video)(AVCodecContext *ctx, BYTE *buf, int buf_size, const AVFrame *pict);
    int (*Favcodec_decode_video)(AVCodecContext *ctx, AVFrame *pict, int *got_picture_ptr, BYTE *buf, int buf_size);
    unsigned (*Favcodec_version)(void);
    unsigned (*Favcodec_build)(void);

    void (*FAv_log_set_level)(int level);
    void (*FAv_log_set_callback)(void (*callback)(void*, int, const char*, va_list));
    int (*Fff_check_alignment)(void);

    bool isLoadedOK;
};

//////////////////////////////////////////////////////////////////////////////

#endif /* __DYNA_H__ */
