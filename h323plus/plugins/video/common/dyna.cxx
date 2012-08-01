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
#include "dyna.h"

#ifdef _MSC_VER
#define snprintf _snprintf
#endif

bool DynaLink::Open(const char *name)
{
  // At first we try without a path
  if (InternalOpen("", name))
    return true;

  // As a last resort, try the current directory
  return InternalOpen(".", name);
}

bool DynaLink::InternalOpen(const char * dir, const char *name)
{
  char path[1024];
  memset(path, 0, sizeof(path));

  // Copy the directory to "path" and add a separator if necessary
  if (strlen(path) > 0) {
    strcpy(path, dir);
    if (path[strlen(path)-1] != DIR_SEPARATOR[0]) 
    strcat(path, DIR_SEPARATOR);
  }
  strcat(path, name);

  // Load the Libary
  WITH_ALIGNED_STACK({  // must be called before using avcodec lib
#ifdef _WIN32
# ifdef UNICODE
     USES_CONVERSION;
    _hDLL = LoadLibrary(A2T(path));
# else
    _hDLL = LoadLibrary(name);
# endif /* UNICODE */
#else
    _hDLL = dlopen((const char *)path, RTLD_NOW);
#endif /* WIN32 */
  });
  // Check for errors
  if (_hDLL == NULL) {
#ifndef _WIN32
  char * err = dlerror();
  if (err != NULL)
    TRACE(1, _codecString << "\tDYNA\tError loading " << path << " - " << err)
    else
#endif /* WIN32 */
    TRACE(1, _codecString << "\tDYNA\tError loading " << path);
    return false;
  } 
  else {
    TRACE(1, _codecString << "\tDYNA\tSuccessfully loaded " << path);
    return true;
  }
}

void DynaLink::Close()
{
  if (_hDLL != NULL) {
#ifdef _WIN32
    FreeLibrary(_hDLL);
#else
    dlclose(_hDLL);
#endif /* WIN32 */
    _hDLL = NULL;
  }
}

bool DynaLink::GetFunction(const char * name, Function & func)
{
  if (_hDLL == NULL)
    return false;
#ifdef _WIN32

# ifdef UNICODE
  USES_CONVERSION;
  FARPROC p = GetProcAddress(_hDLL, A2T(name));
# else
  FARPROC p = GetProcAddress(_hDLL, name);
# endif /* UNICODE */
  if (p == NULL)
    return false;

  func = (Function)p;
  return true;
#else
  void * p = dlsym(_hDLL, (const char *)name);
  if (p == NULL)
    return false;
  func = (Function &)p;
  return true;
#endif /* _WIN32 */
}

FFMPEGLibrary::FFMPEGLibrary(CodecID codec)
{
  _codec = codec;
  if (_codec==CODEC_ID_H264)
      snprintf( _codecString, sizeof(_codecString), "H264");
  if (_codec==CODEC_ID_H263P)
      snprintf( _codecString, sizeof(_codecString), "H263+");
  if (_codec==CODEC_ID_MPEG4)
      snprintf( _codecString, sizeof(_codecString), "MPEG4");
  isLoadedOK = false;
}

bool FFMPEGLibrary::Load()
{
  WaitAndSignal m(processLock);      
  if (IsLoaded())
    return true;
  if (
#if defined(WIN32)
      !DynaLink::Open("libavcodec")
#else
#ifdef LIBAVCODEC_LIB_NAME
      !DynaLink::Open(LIBAVCODEC_LIB_NAME) &&
#endif
      !DynaLink::Open("libavcodec.so")
#endif
      && !DynaLink::Open("avcodec"))
    {
    TRACE (1, _codecString << "\tDYNA\tFailed to load libavcodec library");
    return false;
  }

  if (!GetFunction("avcodec_init", (Function &)Favcodec_init)) {
    TRACE (1, _codecString << "\tDYNA\tFailed to load avcodec_init");
    return false;
  }

  if (_codec==CODEC_ID_H264) {
    if (!GetFunction("h264_decoder", (Function &)Favcodec_h264_decoder)) {
      TRACE (1, _codecString << "\tDYNA\tFailed to load h264_decoder");
      return false;
    }
  }
  
  if (_codec==CODEC_ID_H263P) {
    if (!GetFunction("h263_encoder", (Function &)Favcodec_h263_encoder)) {
      TRACE (1, _codecString << "\tDYNA\tFailed to load h263_encoder" );
      return false;
    }
  
    if (!GetFunction("h263p_encoder", (Function &)Favcodec_h263p_encoder)) {
      TRACE (1, _codecString << "\tDYNA\tFailed to load h263p_encoder" );
      return false;
    }

    if (!GetFunction("h263_decoder", (Function &)Favcodec_h263_decoder)) {
      TRACE (1, _codecString << "\tDYNA\tFailed to load h263_decoder" );
      return false;
    }
  }

  if (_codec==CODEC_ID_MPEG4) {
    if (!GetFunction("mpeg4_encoder", (Function &)mpeg4_encoder)) {
      TRACE (1, _codecString << "\tDYNA\tFailed to load mpeg4_encoder");
      return false;
    }

    if (!GetFunction("mpeg4_decoder", (Function &)mpeg4_decoder)) {
      TRACE (1, _codecString << "\tDYNA\tFailed to load mpeg4_decoder");
      return false;
    }
  }

    if (!GetFunction("register_avcodec", (Function &)Favcodec_register)) {
      TRACE (1, _codecString << "\tDYNA\tFailed to load register_avcodec");
      return false;
    }
  
  if (!GetFunction("avcodec_find_encoder", (Function &)Favcodec_find_encoder)) {
    TRACE (1, _codecString << "\tDYNA\tFailed to load avcodec_find_encoder");
    return false;
  }

  if (!GetFunction("avcodec_find_decoder", (Function &)Favcodec_find_decoder)) {
    TRACE (1, _codecString << "\tDYNA\tFailed to load avcodec_find_decoder");
    return false;
  }

  if (!GetFunction("avcodec_alloc_context", (Function &)Favcodec_alloc_context)) {
    TRACE (1, _codecString << "\tDYNA\tFailed to load avcodec_alloc_context");
    return false;
  }

  if (!GetFunction("avcodec_alloc_frame", (Function &)Favcodec_alloc_frame)) {
    TRACE (1, _codecString << "\tDYNA\tFailed to load avcodec_alloc_frame");
    return false;
  }

  if (!GetFunction("avcodec_open", (Function &)Favcodec_open)) {
    TRACE (1, _codecString << "\tDYNA\tFailed to load avcodec_open");
    return false;
  }

  if (!GetFunction("avcodec_close", (Function &)Favcodec_close)) {
    TRACE (1, _codecString << "\tDYNA\tFailed to load avcodec_close");
    return false;
  }

  if (!GetFunction("avcodec_encode_video", (Function &)Favcodec_encode_video)) {
    TRACE (1, _codecString << "\tDYNA\tFailed to load avcodec_encode_video" );
    return false;
  }

  if (!GetFunction("avcodec_decode_video", (Function &)Favcodec_decode_video)) {
    TRACE (1, _codecString << "\tDYNA\tFailed to load avcodec_decode_video");
    return false;
  }

  if (!GetFunction("av_free", (Function &)Favcodec_free)) {
    TRACE (1, _codecString << "\tDYNA\tFailed to load av_free");
    return false;
  }

  if (!GetFunction("ff_check_alignment", (Function &) Fff_check_alignment)) {
    TRACE (1, _codecString << "\tDYNA\tFailed to load ff_check_alignment - alignment checks will be skipped");
    Fff_check_alignment = NULL;
  }

  if(!GetFunction("avcodec_version", (Function &)Favcodec_version)){
    TRACE (1, _codecString << "DYYNA\tFailed to load avcodec_version");
    return false;
  }
  
  if(!GetFunction("avcodec_build", (Function &)Favcodec_build)){
    TRACE (1, _codecString << "\tDYNA\tFailed to load avcodec_build");
    return false;
  }


  if (!GetFunction("av_log_set_level", (Function &)FAv_log_set_level)) {
    TRACE (1, _codecString << "\tDYNA\tFailed to load av_log_set_level");
    return false;
  }

  if (!GetFunction("av_log_set_callback", (Function &)FAv_log_set_callback)) {
    TRACE (1, _codecString << "\tDYNA\tFailed to load av_log_set_callback");
    return false;
  }

  WITH_ALIGNED_STACK({  // must be called before using avcodec lib

    unsigned libVer = Favcodec_version();
    unsigned libBuild = Favcodec_build();
    if (libVer != LIBAVCODEC_VERSION_INT || libBuild != LIBAVCODEC_BUILD) {
      TRACE (1, _codecString << "\tDYNA\tWarning: compiled against libavcodec headers from ver/build "
             << std::hex << LIBAVCODEC_VERSION_INT << "/"
             << std::dec << LIBAVCODEC_BUILD
             << ", loaded " 
             << std::hex << libVer << "/"
             << std::dec << libBuild);
    }

    Favcodec_init();

    // register only the codecs needed (to have smaller code)
    if (_codec==CODEC_ID_H264) 
      Favcodec_register(Favcodec_h264_decoder);

    if (_codec==CODEC_ID_H263P) {
      Favcodec_register(Favcodec_h263_encoder);
      Favcodec_register(Favcodec_h263p_encoder);
      Favcodec_register(Favcodec_h263_decoder);
    }

    if (_codec==CODEC_ID_MPEG4) {
      Favcodec_register(mpeg4_encoder);
      Favcodec_register(mpeg4_decoder);
    }

    if (FFCheckAlignment() != 0) {
      TRACE(1, _codecString << "\tDYNA\tff_check_alignment() reports failure - stack alignment is not correct");
    }	    
  });

  isLoadedOK = true;
  TRACE (4, _codecString << "\tDYNA\tSuccessfully loaded libavcodec library and verified functions");

  return true;
}

FFMPEGLibrary::~FFMPEGLibrary()
{
  DynaLink::Close();
}

AVCodec *FFMPEGLibrary::AvcodecFindEncoder(enum CodecID id)
{
  WITH_ALIGNED_STACK({
    AVCodec *res = Favcodec_find_encoder(id);
    return res;
  });
}

AVCodec *FFMPEGLibrary::AvcodecFindDecoder(enum CodecID id)
{
  WITH_ALIGNED_STACK({
    AVCodec *res = Favcodec_find_decoder(id);
    return res;
  });
}

AVCodecContext *FFMPEGLibrary::AvcodecAllocContext(void)
{
  WITH_ALIGNED_STACK({
    AVCodecContext *res = Favcodec_alloc_context();
    return res;
  });
}

AVFrame *FFMPEGLibrary::AvcodecAllocFrame(void)
{
  WITH_ALIGNED_STACK({
    AVFrame *res = Favcodec_alloc_frame();
    return res;
  });
}

int FFMPEGLibrary::AvcodecOpen(AVCodecContext *ctx, AVCodec *codec)
{
  WaitAndSignal m(processLock);

  WITH_ALIGNED_STACK({
    return Favcodec_open(ctx, codec);
  });
}

int FFMPEGLibrary::AvcodecClose(AVCodecContext *ctx)
{
  WITH_ALIGNED_STACK({
    return Favcodec_close(ctx);
  });
}

int FFMPEGLibrary::AvcodecEncodeVideo(AVCodecContext *ctx, BYTE *buf, int buf_size, const AVFrame *pict)
{
  WaitAndSignal m(processLock);

  WITH_ALIGNED_STACK({
    int res = Favcodec_encode_video(ctx, buf, buf_size, pict);

    TRACE(4, _codecString << "\tDYNA\tEncoded video into " << res << " bytes");
    return res;
  });
}

int FFMPEGLibrary::AvcodecDecodeVideo(AVCodecContext *ctx, AVFrame *pict, int *got_picture_ptr, BYTE *buf, int buf_size)
{
  WaitAndSignal m(processLock);

  WITH_ALIGNED_STACK({
    int res = Favcodec_decode_video(ctx, pict, got_picture_ptr, buf, buf_size);

    TRACE(4, _codecString << "\tDYNA\tDecoded video of " << res << " bytes, got_picture=" << *got_picture_ptr);
    return res;
  });
}

void FFMPEGLibrary::AvcodecFree(void * ptr)
{
  WITH_ALIGNED_STACK({
    Favcodec_free(ptr);
  });
}

void FFMPEGLibrary::AvLogSetLevel(int level)
{
  WITH_ALIGNED_STACK({
    FAv_log_set_level(level);
  });
}

void FFMPEGLibrary::AvLogSetCallback(void (*callback)(void*, int, const char*, va_list))
{
  WITH_ALIGNED_STACK({
    FAv_log_set_callback(callback);
  });
}

int FFMPEGLibrary::FFCheckAlignment(void)
{
  if (Fff_check_alignment == NULL) {
    TRACE(1, _codecString << "\tDYNA\tff_check_alignment is not supported by libavcodec.so - skipping check");
    return 0;
  }
  else {
    return (Fff_check_alignment());
  }
}

bool FFMPEGLibrary::IsLoaded()
{
  return isLoadedOK;
}
