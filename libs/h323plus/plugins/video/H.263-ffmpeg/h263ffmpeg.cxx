/*
 * H.263 Plugin codec for OpenH323/OPAL
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
 *
 * $Log: h263ffmpeg.cxx,v $
 * Revision 1.5  2007/12/20 15:41:09  willamowius
 * merge in bugfixes from OPAL repository
 *
 * Revision 1.4  2007/12/20 15:32:21  willamowius
 * Simon added adaptive packet delay / encoding Quality
 *
 * Revision 1.3  2007/11/05 11:45:16  shorne
 * updated to Opal Version 18813
 *
 * Revision 1.16  2007/08/31 07:01:06  rjongbloed
 * Fixed incorrect search directory for avcodec DLL
 *
 * Revision 1.15  2007/08/16 03:13:36  rjongbloed
 * Added H.323 Media Packetization OLC field, sourced from an OpalMediaOption
 *   so plug ins can provide it as required.
 *
 * Revision 1.14  2007/06/25 22:49:52  rjongbloed
 * Fixed compile after change to anonymous structure in header (for GCC).
 *
 * Revision 1.13  2007/06/22 05:41:47  rjongbloed
 * Major codec API update:
 *   Automatically map OpalMediaOptions to SIP/SDP FMTP parameters.
 *   Automatically map OpalMediaOptions to H.245 Generic Capability parameters.
 *   Largely removed need to distinguish between SIP and H.323 codecs.
 *   New mechanism for setting OpalMediaOptions from within a plug in.
 *
 * Revision 1.12  2007/06/02 12:30:33  dsandras
 * Added security checks against the FFMpeg version to prevent problems in case
 * they are not coherent as all our plugins require different versions of
 * FFMPEG. Thanks Michael Smith <msmith cbnco com>.
 *
 * Revision 1.11  2007/06/02 12:24:28  dsandras
 * Fixed H.263 plugin search directory thanks to Michael Smith
 * <msmith cbnco com>. Many thanks!
 *
 * Revision 1.10  2007/04/05 05:47:54  rjongbloed
 * Fixed crash in H.263 plug in if do not have PWLIBPLUGINDIR environment variable set.
 *
 * Revision 1.9  2006/11/01 06:57:23  csoutheren
 * Fixed usage of YUV frame header
 *
 * Revision 1.8  2006/10/31 05:53:30  csoutheren
 * More changes to copyright information
 *
 * Revision 1.7  2006/10/31 05:47:56  csoutheren
 * Clarified copyright and heritage of original codec source
 *
 * Revision 1.6  2006/10/26 08:35:59  rjongbloed
 * Fixed DevStudio 2005 warning
 *
 * Revision 1.5  2006/10/04 08:44:14  csoutheren
 * Fixed loading problems on Linux
 *
 * Revision 1.4  2006/09/06 23:09:21  csoutheren
 * Add Makefile and more implementation for Linux H.263
 *
 * Revision 1.3  2006/08/10 07:05:46  csoutheren
 * Fixed compile warnings on VC 2005
 *
 * Revision 1.2  2006/07/31 09:09:23  csoutheren
 * Checkin of validated codec used during development
 *
 * Revision 1.1.2.3  2006/04/26 08:03:58  csoutheren
 * H.263 encoding and decoding now working from plugin for both SIP and H.323
 *
 * Revision 1.1.2.2  2006/04/26 05:05:59  csoutheren
 * H.263 decoding working via codec plugin
 *
 * Revision 1.1.2.1  2006/04/24 09:07:34  csoutheren
 * Initial implementation of H.263 codec plugin using ffmpeg.
 * Not yet tested - decoder only implemented
 *
 */

/*
  Notes
  -----

  This codec implements a H.263 encoder and decoder with RTP packaging as per 
  RFC 2190 "RTP Payload Format for H.263 Video Streams". As per this specification,
  The RTP payload code is always set to 34

 */

#define _CRT_NONSTDC_NO_WARNINGS
#define _CRT_SECURE_NO_DEPRECATE
#include <codec/opalplugin.h>

#include <stdlib.h>
#ifdef _WIN32
#include <windows.h>
#include <malloc.h>
#define STRCMPI  _strcmpi
#else
#include <semaphore.h>
#include <dlfcn.h>
#include <sys/time.h>
#define STRCMPI  strcasecmp
typedef unsigned char BYTE;
typedef bool BOOL;
#define FALSE false
#define TRUE  true

#endif

#ifdef _MSC_VER
#pragma warning(disable:4800)
#endif

#include <string.h>

extern "C" {
#include "ffmpeg/avcodec.h"
};

#if LIBAVCODEC_VERSION_INT != 0x000406
#error Wrong libavcodec version for h.263.
#endif

#  ifdef  _WIN32
#    define P_DEFAULT_PLUGIN_DIR "C:\\PTLIB_PLUGINS;C:\\PWLIB_PLUGINS"
#    define DIR_SEPERATOR "\\"
#    define DIR_TOKENISER ";"
#  else
#    define P_DEFAULT_PLUGIN_DIR "/usr/lib/ptlib:/usr/lib/pwlib"
#    define DIR_SEPERATOR "/"
#    define DIR_TOKENISER ":"
#  endif

#include <vector>
#include <iostream>
#include <iomanip>
using namespace std;

// if defined, the FFMPEG code is access via another DLL
// otherwise, the FFMPEG code is assumed to be statically linked into this plugin

#define USE_DLL_AVCODEC   1

#define RTP_RFC2190_PAYLOAD  34
#define RTP_DYNAMIC_PAYLOAD  96

#define H263_CLOCKRATE    90000
#define H263_BITRATE      327600

#define CIF_WIDTH       352
#define CIF_HEIGHT      288

#define CIF4_WIDTH      (CIF_WIDTH*2)
#define CIF4_HEIGHT     (CIF_HEIGHT*2)

#define CIF16_WIDTH     (CIF_WIDTH*4)
#define CIF16_HEIGHT    (CIF_HEIGHT*4)

#define QCIF_WIDTH     (CIF_WIDTH/2)
#define QCIF_HEIGHT    (CIF_HEIGHT/2)

#define SQCIF_WIDTH     128
#define SQCIF_HEIGHT    96

#define MAX_H263_PACKET_SIZE     2048
#define MAX_YUV420P_PACKET_SIZE (((CIF16_WIDTH * CIF16_HEIGHT * 3) / 2) + FF_INPUT_BUFFER_PADDING_SIZE)

#define MIN(v1, v2) ((v1) < (v2) ? (v1) : (v2))
#define MAX(v1, v2) ((v1) > (v2) ? (v1) : (v2))

/////////////////////////////////////////////////////////////////
//
// define a class to implement a critical section mutex
// based on PCriticalSection from PWLib

class CriticalSection
{
  public:
    CriticalSection()
    { 
#ifdef _WIN32
      ::InitializeCriticalSection(&criticalSection); 
#else
      ::sem_init(&sem, 0, 1);
#endif
    }

    ~CriticalSection()
    { 
#ifdef _WIN32
      ::DeleteCriticalSection(&criticalSection); 
#else
      ::sem_destroy(&sem);
#endif
    }

    void Wait()
    { 
#ifdef _WIN32
      ::EnterCriticalSection(&criticalSection); 
#else
      ::sem_wait(&sem);
#endif
    }

    void Signal()
    { 
#ifdef _WIN32
      ::LeaveCriticalSection(&criticalSection); 
#else
      ::sem_post(&sem); 
#endif
    }

  private:
    CriticalSection(const CriticalSection &)
    { }
    CriticalSection & operator=(const CriticalSection &) { return *this; }
#ifdef _WIN32
    mutable CRITICAL_SECTION criticalSection; 
#else
    mutable sem_t sem;
#endif
};
    
class WaitAndSignal {
  public:
    inline WaitAndSignal(const CriticalSection & cs)
      : sync((CriticalSection &)cs)
    { sync.Wait(); }

    ~WaitAndSignal()
    { sync.Signal(); }

    WaitAndSignal & operator=(const WaitAndSignal &) 
    { return *this; }

  protected:
    CriticalSection & sync;
};

/////////////////////////////////////////////////////////////////
//
// define a class to simplify handling a DLL library
// based on PDynaLink from PWLib

#if USE_DLL_AVCODEC

class DynaLink
{
  public:
    typedef void (*Function)();

    DynaLink()
    { _hDLL = NULL; }

    ~DynaLink()
    { Close(); }

    virtual bool Open(const char *name)
    {
      char * env;
      if ((env = ::getenv("PTLIBPLUGINDIR")) == NULL &&
          (env = ::getenv("PWLIBPLUGINDIR")) == NULL) 
        env = strdup(P_DEFAULT_PLUGIN_DIR);

      const char * token = strtok(env, DIR_TOKENISER);
      while (token != NULL) {
        if (InternalOpen(token, name))
          return true;
        token = strtok(NULL, DIR_TOKENISER);
      }
      return false;
    }

  // split into directories on correct seperator

    bool InternalOpen(const char * dir, const char *name)
    {
      char path[1024];
      memset(path, 0, sizeof(path));
      if (dir != NULL) {
        strcpy(path, dir);
        if (path[strlen(path)-1] != DIR_SEPERATOR[0]) 
          strcat(path, DIR_SEPERATOR);
      }
      strcat(path, name);

#ifdef _WIN32
# ifdef UNICODE
      USES_CONVERSION;
      _hDLL = LoadLibrary(A2T(path));
# else
      _hDLL = LoadLibrary(path);
# endif // UNICODE
#else
      _hDLL = dlopen((const char *)path, RTLD_NOW);
      if (_hDLL == NULL) {
        fprintf(stderr, "error loading %s", path);
        char * err = dlerror();
        if (err != NULL)
          fprintf(stderr, " - %s", err);
        fprintf(stderr, "\n");
      }
#endif // _WIN32
      return _hDLL != NULL;
    }

    virtual void Close()
    {
      if (_hDLL != NULL) {
#ifdef _WIN32
        FreeLibrary(_hDLL);
#else
        dlclose(_hDLL);
#endif // _WIN32
        _hDLL = NULL;
      }
    }


    virtual bool IsLoaded() const
    { return _hDLL != NULL; }

    bool GetFunction(const char * name, Function & func)
    {
      if (_hDLL == NULL)
        return FALSE;
#ifdef _WIN32

# ifdef UNICODE
      USES_CONVERSION;
      FARPROC p = GetProcAddress(_hDLL, A2T(name));
# else
      FARPROC p = GetProcAddress(_hDLL, name);
# endif // UNICODE
      if (p == NULL)
        return FALSE;

      func = (Function)p;
      return TRUE;
#else
      void * p = dlsym(_hDLL, (const char *)name);
      if (p == NULL)
        return FALSE;
      func = (Function &)p;
      return TRUE;
#endif // _WIN32
    }

  protected:
#if defined(_WIN32)
    HINSTANCE _hDLL;
#else
    void * _hDLL;
#endif // _WIN32
};

#endif  // USE_DLL_AVCODEC 

/////////////////////////////////////////////////////////////////
//
// define a class to interface to the FFMpeg library


class FFMPEGLibrary

#if USE_DLL_AVCODEC
                   : public DynaLink
#endif // USE_DLL_AVCODEC
{
  public:
    FFMPEGLibrary();
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

    void AvcodecSetPrintFn(void (*print_fn)(char *));

    bool IsLoaded();
    CriticalSection processLock;

  protected:
    void (*Favcodec_init)(void);
    AVCodec *Favcodec_h263_encoder;
    AVCodec *Favcodec_h263p_encoder;
    AVCodec *Favcodec_h263_decoder;
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

    void (*Favcodec_set_print_fn)(void (*print_fn)(char *));
    unsigned (*Favcodec_version)(void);
    unsigned (*Favcodec_build)(void);

    bool isLoadedOK;
};

static FFMPEGLibrary FFMPEGLibraryInstance;

//////////////////////////////////////////////////////////////////////////////

#ifdef USE_DLL_AVCODEC

FFMPEGLibrary::FFMPEGLibrary()
{
  isLoadedOK = FALSE;
}

bool FFMPEGLibrary::Load()
{
  WaitAndSignal m(processLock);
  if (IsLoaded())
    return true;

  if (!DynaLink::Open("avcodec")
#if defined(WIN32)
      && !DynaLink::Open("libavcodec")
#else
      && !DynaLink::Open("libavcodec.so")
#endif
    ) {
    //cerr << "FFLINK\tFailed to load a library, some codecs won't operate correctly;" << endl;
#if !defined(WIN32)
    //cerr << "put libavcodec.so in the current directory (together with this program) and try again" << endl;
#else
    //cerr << "put avcodec.dll in the current directory (together with this program) and try again" << endl;
#endif
    return false;
  }

  if (!GetFunction("avcodec_init", (Function &)Favcodec_init)) {
    //cerr << "Failed to load avcodec_int" << endl;
    return false;
  }

  if (!GetFunction("h263_encoder", (Function &)Favcodec_h263_encoder)) {
    //cerr << "Failed to load h263_encoder" << endl;
    return false;
  }

  if (!GetFunction("h263p_encoder", (Function &)Favcodec_h263p_encoder)) {
    //cerr << "Failed to load h263p_encoder" << endl;
    return false;
  }

  if (!GetFunction("h263_decoder", (Function &)Favcodec_h263_decoder)) {
    //cerr << "Failed to load h263_decoder" << endl;
    return false;
  }

  if (!GetFunction("register_avcodec", (Function &)Favcodec_register)) {
    //cerr << "Failed to load register_avcodec" << endl;
    return false;
  }

  if (!GetFunction("avcodec_find_encoder", (Function &)Favcodec_find_encoder)) {
    //cerr << "Failed to load avcodec_find_encoder" << endl;
    return false;
  }

  if (!GetFunction("avcodec_find_decoder", (Function &)Favcodec_find_decoder)) {
    //cerr << "Failed to load avcodec_find_decoder" << endl;
    return false;
  }

  if (!GetFunction("avcodec_alloc_context", (Function &)Favcodec_alloc_context)) {
    //cerr << "Failed to load avcodec_alloc_context" << endl;
    return false;
  }

  if (!GetFunction("avcodec_alloc_frame", (Function &)Favcodec_alloc_frame)) {
    //cerr << "Failed to load avcodec_alloc_frame" << endl;
    return false;
  }

  if (!GetFunction("avcodec_open", (Function &)Favcodec_open)) {
    //cerr << "Failed to load avcodec_open" << endl;
    return false;
  }

  if (!GetFunction("avcodec_close", (Function &)Favcodec_close)) {
    //cerr << "Failed to load avcodec_close" << endl;
    return false;
  }

  if (!GetFunction("avcodec_encode_video", (Function &)Favcodec_encode_video)) {
    //cerr << "Failed to load avcodec_encode_video" << endl;
    return false;
  }

  if (!GetFunction("avcodec_decode_video", (Function &)Favcodec_decode_video)) {
    //cerr << "Failed to load avcodec_decode_video" << endl;
    return false;
  }

  if (!GetFunction("avcodec_set_print_fn", (Function &)Favcodec_set_print_fn)) {
    //cerr << "Failed to load avcodec_set_print_fn" << endl;
    return false;
  }
   
  if (!GetFunction("av_free", (Function &)Favcodec_free)) {
    //cerr << "Failed to load avcodec_close" << endl;
    return false;
  }

  if (!GetFunction("avcodec_version", (Function &)Favcodec_version)) {
    return false;
  }

  if (!GetFunction("avcodec_build", (Function &)Favcodec_build)) {
    return false;
  }

  unsigned libVer = Favcodec_version();
  unsigned libBuild = Favcodec_build();
  if (libVer != LIBAVCODEC_VERSION_INT) {
    fprintf(stderr, "h.263 ffmpeg version mismatch: compiled against headers "
                    "from ver/build 0x%x/%d, loaded library version "
                    "0x%x/%d.\n", LIBAVCODEC_VERSION_INT, LIBAVCODEC_BUILD,
                    libVer, libBuild);
    return false;
  }
  if (libBuild != LIBAVCODEC_BUILD) {
    fprintf(stderr, "Warning: potential h.263 ffmpeg build mismatch: "
                    "compiled against build %d, loaded library build %d.\n",
                    LIBAVCODEC_BUILD, libBuild);
  }

  // must be called before using avcodec lib
  Favcodec_init();

  // register only the codecs needed (to have smaller code)
  Favcodec_register(Favcodec_h263_encoder);
  Favcodec_register(Favcodec_h263p_encoder);
  Favcodec_register(Favcodec_h263_decoder);
  
  //Favcodec_set_print_fn(h263_ffmpeg_printon);

  isLoadedOK = TRUE;

  return true;
}

FFMPEGLibrary::~FFMPEGLibrary()
{
  DynaLink::Close();
}

AVCodec *FFMPEGLibrary::AvcodecFindEncoder(enum CodecID id)
{
  AVCodec *res = Favcodec_find_encoder(id);
  //PTRACE_IF(6, res, "FFLINK\tFound encoder " << res->name << " @ " << ::hex << (int)res << ::dec);
  return res;
}

AVCodec *FFMPEGLibrary::AvcodecFindDecoder(enum CodecID id)
{
  AVCodec *res = Favcodec_find_decoder(id);
  //PTRACE_IF(6, res, "FFLINK\tFound decoder " << res->name << " @ " << ::hex << (int)res << ::dec);
  return res;
}

AVCodecContext *FFMPEGLibrary::AvcodecAllocContext(void)
{
  AVCodecContext *res = Favcodec_alloc_context();
  //PTRACE_IF(6, res, "FFLINK\tAllocated context @ " << ::hex << (int)res << ::dec);
  return res;
}

AVFrame *FFMPEGLibrary::AvcodecAllocFrame(void)
{
  AVFrame *res = Favcodec_alloc_frame();
  //PTRACE_IF(6, res, "FFLINK\tAllocated frame @ " << ::hex << (int)res << ::dec);
  return res;
}

int FFMPEGLibrary::AvcodecOpen(AVCodecContext *ctx, AVCodec *codec)
{
  WaitAndSignal m(processLock);

  //PTRACE(6, "FFLINK\tNow open context @ " << ::hex << (int)ctx << ", codec @ " << (int)codec << ::dec);
  return Favcodec_open(ctx, codec);
}

int FFMPEGLibrary::AvcodecClose(AVCodecContext *ctx)
{
  //PTRACE(6, "FFLINK\tNow close context @ " << ::hex << (int)ctx << ::dec);
  return Favcodec_close(ctx);
}

int FFMPEGLibrary::AvcodecEncodeVideo(AVCodecContext *ctx, BYTE *buf, int buf_size, const AVFrame *pict)
{
  WaitAndSignal m(processLock);

  //PTRACE(6, "FFLINK\tNow encode video for ctxt @ " << ::hex << (int)ctx << ", pict @ " << (int)pict
	// << ", buf @ " << (int)buf << ::dec << " (" << buf_size << " bytes)");
  int res = Favcodec_encode_video(ctx, buf, buf_size, pict);

  //PTRACE(6, "FFLINK\tEncoded video into " << res << " bytes");
  return res;
}

int FFMPEGLibrary::AvcodecDecodeVideo(AVCodecContext *ctx, AVFrame *pict, int *got_picture_ptr, BYTE *buf, int buf_size)
{
  WaitAndSignal m(processLock);

  //PTRACE(6, "FFLINK\tNow decode video for ctxt @ " << ::hex << (int)ctx << ", pict @ " << (int)pict
	// << ", buf @ " << (int)buf << ::dec << " (" << buf_size << " bytes)");
  int res = Favcodec_decode_video(ctx, pict, got_picture_ptr, buf, buf_size);

  //PTRACE(6, "FFLINK\tDecoded video of " << res << " bytes, got_picture=" << *got_picture_ptr);
  return res;
}

void FFMPEGLibrary::AvcodecSetPrintFn(void (*print_fn)(char *))
{
  Favcodec_set_print_fn(print_fn);
}

void FFMPEGLibrary::AvcodecFree(void * ptr)
{
  Favcodec_free(ptr);
}

bool FFMPEGLibrary::IsLoaded()
{
  return isLoadedOK;
}

#else

#error "Not yet able to use statically linked libavcodec"

#endif // USE_DLL_AVCODEC

/////////////////////////////////////////////////////////////////////////////
//
// define some simple RTP packet routines
//

#define RTP_MIN_HEADER_SIZE 12

class RTPFrame
{
  public:
    RTPFrame(const unsigned char * _packet, int _maxPacketLen)
      : packet((unsigned char *)_packet), maxPacketLen(_maxPacketLen), packetLen(_maxPacketLen)
    {
    }

    RTPFrame(unsigned char * _packet, int _maxPacketLen, unsigned char payloadType)
      : packet(_packet), maxPacketLen(_maxPacketLen), packetLen(_maxPacketLen)
    { 
      if (packetLen > 0)
        packet[0] = 0x80;    // set version, no extensions, zero contrib count
      SetPayloadType(payloadType);
    }

    inline unsigned long GetLong(unsigned offs) const
    {
      if (offs + 4 > packetLen)
        return 0;
      return (packet[offs + 0] << 24) + (packet[offs+1] << 16) + (packet[offs+2] << 8) + packet[offs+3]; 
    }

    inline void SetLong(unsigned offs, unsigned long n)
    {
      if (offs + 4 <= packetLen) {
        packet[offs + 0] = (BYTE)((n >> 24) & 0xff);
        packet[offs + 1] = (BYTE)((n >> 16) & 0xff);
        packet[offs + 2] = (BYTE)((n >> 8) & 0xff);
        packet[offs + 3] = (BYTE)(n & 0xff);
      }
    }

    inline unsigned short GetShort(unsigned offs) const
    { 
      if (offs + 2 > packetLen)
        return 0;
      return (packet[offs + 0] << 8) + packet[offs + 1]; 
    }

    inline void SetShort(unsigned offs, unsigned short n) 
    { 
      if (offs + 2 <= packetLen) {
        packet[offs + 0] = (BYTE)((n >> 8) & 0xff);
        packet[offs + 1] = (BYTE)(n & 0xff);
      }
    }

    inline int GetPacketLen() const                    { return packetLen; }
    inline int GetMaxPacketLen() const                 { return maxPacketLen; }
    inline unsigned GetVersion() const                 { return (packetLen < 1) ? 0 : (packet[0]>>6)&3; }
    inline bool GetExtension() const                   { return (packetLen < 1) ? 0 : (packet[0]&0x10) != 0; }
    inline bool GetMarker()  const                     { return (packetLen < 2) ? FALSE : ((packet[1]&0x80) != 0); }
    inline unsigned char GetPayloadType() const        { return (packetLen < 2) ? FALSE : (packet[1] & 0x7f);  }
    inline unsigned short GetSequenceNumber() const    { return GetShort(2); }
    inline unsigned long GetTimestamp() const          { return GetLong(4); }
    inline unsigned long GetSyncSource() const         { return GetLong(8); }
    inline int GetContribSrcCount() const              { return (packetLen < 1) ? 0  : (packet[0]&0xf); }
    inline int GetExtensionSize() const                { return !GetExtension() ? 0  : GetShort(RTP_MIN_HEADER_SIZE + 4*GetContribSrcCount() + 2); }
    inline int GetExtensionType() const                { return !GetExtension() ? -1 : GetShort(RTP_MIN_HEADER_SIZE + 4*GetContribSrcCount()); }
    inline int GetPayloadSize() const                  { return packetLen - GetHeaderSize(); }
    inline unsigned char * GetPayloadPtr() const       { return packet + GetHeaderSize(); }

    inline unsigned int GetHeaderSize() const    
    { 
      unsigned int sz = RTP_MIN_HEADER_SIZE + 4*GetContribSrcCount();
      if (GetExtension())
        sz += 4 + GetExtensionSize();
      return sz;
    }

    inline void SetMarker(bool m)                    { if (packetLen >= 2) packet[1] = (packet[1] & 0x7f) | (m ? 0x80 : 0x00); }
    inline void SetPayloadType(unsigned char t)      { if (packetLen >= 2) packet[1] = (packet[1] & 0x80) | (t & 0x7f); }
    inline void SetSequenceNumber(unsigned short v)  { SetShort(2, v); }
    inline void SetTimestamp(unsigned long n)        { SetLong(4, n); }
    inline void SetSyncSource(unsigned long n)       { SetLong(8, n); }

    inline bool SetPayloadSize(int payloadSize)      
    { 
      if (GetHeaderSize() + payloadSize > maxPacketLen)
        return true; 
      packetLen = GetHeaderSize() + payloadSize;
      return true;
    }

  protected:
    unsigned char * packet;
    unsigned maxPacketLen;
    unsigned packetLen;
};

/////////////////////////////////////////////////////////////////////////////

class H263Packet
{
  public:
    H263Packet() { data_size = hdr_size = 0; hdr = data = NULL; };
    ~H263Packet() {};

    void Store(void * _data, int _data_size, void * _hdr, int _hdr_size)
    {
      data      = _data;
      data_size = _data_size;
      hdr       = _hdr;
      hdr_size  = _hdr_size;
    }

    bool Read(RTPFrame & frame)
    {
      if (!frame.SetPayloadSize(hdr_size + data_size)) {
        //PTRACE(1, "H263Pck\tNot enough memory for packet of " << length << " bytes");
        return false;
      }
      memcpy(frame.GetPayloadPtr(), hdr, hdr_size);
      memcpy(frame.GetPayloadPtr() + hdr_size, data, data_size);

      data = NULL;
      hdr = NULL;

      return true;
    }

  private:
    void *data;
    int data_size;
    void *hdr;
    int hdr_size;
};

class H263EncoderContext
{
  public:
    typedef std::vector<H263Packet *> H263PacketList;
    static void RtpCallback(void *data, int data_size,
                            void *hdr, int hdr_size, void *priv_data);

    H263EncoderContext();
    ~H263EncoderContext();
    int EncodeFrames(const BYTE * src, unsigned & srcLen, BYTE * dst, unsigned & dstLen, unsigned int & flags);

    BOOL OpenCodec();
    void CloseCodec();

    unsigned GetNextEncodedPacket(RTPFrame & dstRTP, unsigned char payloadCode, unsigned long lastTimeStamp, unsigned & flags);

    H263PacketList encodedPackets;
    H263PacketList unusedPackets;

    unsigned char encFrameBuffer[MAX_YUV420P_PACKET_SIZE];
    int encFrameLen;

    unsigned char rawFrameBuffer[MAX_YUV420P_PACKET_SIZE];
    int rawFrameLen;

    AVCodec        *avcodec;
    AVCodecContext *avcontext;
    AVFrame        *avpicture;

    int videoQMax, videoQMin; // dynamic video quality min/max limits, 1..31
    int videoQuality; // current video encode quality setting, 1..31
    int frameNum;
    unsigned frameWidth, frameHeight;
    unsigned long lastTimeStamp;
	int bitRate;
	// Adaptive Packet delay
	bool packetDelay;
	unsigned frameLength;
    long waitFactor;
    #ifdef _WIN32
      long newTime;
    #else
      timeval newTime;
    #endif

    enum StdSize { 
      SQCIF, 
      QCIF, 
      CIF, 
      CIF4, 
      CIF16, 
      i480,
	  p720,
      NumStdSizes,
      UnknownStdSize = NumStdSizes
    };

    static int GetStdSize(int width, int height)
    {
      static struct { 
        int width; 
        int height; 
      } StandardVideoSizes[NumStdSizes] = {
        {  128,   96}, // SQCIF
        {  176,  144}, // QCIF
        {  352,  288}, // CIF
        {  704,  576}, // 4CIF
        { 1408, 1152}, // 16CIF
        {  640,  480}, // i480
        {  960,  720}, // p720
      };

      int sizeIndex;
      for (sizeIndex = 0; sizeIndex < NumStdSizes; ++sizeIndex )
        if (StandardVideoSizes[sizeIndex].width == width && StandardVideoSizes[sizeIndex].height == height )
          return sizeIndex;
      return UnknownStdSize;
    }

    protected:
	   void adaptiveDelay(unsigned totalLength);
};

H263EncoderContext::H263EncoderContext() 
{ 
  if (!FFMPEGLibraryInstance.IsLoaded())
    return;

  if ((avcodec = FFMPEGLibraryInstance.AvcodecFindEncoder(CODEC_ID_H263)) == NULL) {
    //PTRACE(1, "H263\tCodec not found for encoder");
    return;
  }

  frameWidth  = CIF_WIDTH;
  frameHeight = CIF_HEIGHT;
  rawFrameLen = (CIF_HEIGHT * CIF_WIDTH * 3) / 2;

  avcontext = FFMPEGLibraryInstance.AvcodecAllocContext();
  if (avcontext == NULL) {
    //PTRACE(1, "H263\tFailed to allocate context for encoder");
    return;
  }

  avpicture = FFMPEGLibraryInstance.AvcodecAllocFrame();
  if (avpicture == NULL) {
    //PTRACE(1, "H263\tFailed to allocate frame for encoder");
    return;
  }

  avcontext->codec = NULL;

  // set some reasonable values for quality as default
  videoQuality = 10; 
  videoQMin = 4;
  videoQMax = 24;
  frameNum = 0;
  bitRate = 256000;

  packetDelay = false;
  frameLength = 0;
  waitFactor = 0;
  #ifdef _WIN32
    newTime = 0;
  #else
    newTime.tv_sec = 0;
    newTime.tv_usec = 0;
  #endif

  //PTRACE(3, "Codec\tH263 encoder created");
}

H263EncoderContext::~H263EncoderContext()
{
  if (FFMPEGLibraryInstance.IsLoaded()) {
    CloseCodec();

    FFMPEGLibraryInstance.AvcodecFree(avcontext);
    FFMPEGLibraryInstance.AvcodecFree(avpicture);

    while (encodedPackets.size() > 0) {
      delete *encodedPackets.begin();
      encodedPackets.erase(encodedPackets.begin());
    }
    while (unusedPackets.size() > 0) {
      delete *unusedPackets.begin();
      unusedPackets.erase(unusedPackets.begin());
    }
  }
}

BOOL H263EncoderContext::OpenCodec()
{
  // avoid copying input/output
  avcontext->flags |= CODEC_FLAG_INPUT_PRESERVED; // we guarantee to preserve input for max_b_frames+1 frames
  avcontext->flags |= CODEC_FLAG_EMU_EDGE; // don't draw edges

  avcontext->width  = frameWidth;
  avcontext->height = frameHeight;

  avpicture->linesize[0] = frameWidth;
  avpicture->linesize[1] = frameWidth / 2;
  avpicture->linesize[2] = frameWidth / 2;
  avpicture->quality = (float)videoQuality;

  avcontext->bit_rate = (bitRate * 3) >> 2; // average bit rate
  avcontext->bit_rate_tolerance = bitRate << 3;
  avcontext->rc_min_rate = 0; // minimum bitrate
  avcontext->rc_max_rate = bitRate; // maximum bitrate
  avcontext->mb_qmin = avcontext->qmin = videoQMin;
  avcontext->mb_qmax = avcontext->qmax = videoQMax;
  avcontext->max_qdiff = 3; // max q difference between frames
  avcontext->rc_qsquish = 0; // limit q by clipping
  avcontext->rc_eq= "tex^qComp"; // rate control equation
  avcontext->qcompress = 0.5; // qscale factor between easy & hard scenes (0.0-1.0)
  avcontext->i_quant_factor = (float)-0.6; // qscale factor between p and i frames
  avcontext->i_quant_offset = (float)0.0; // qscale offset between p and i frames
  // context->b_quant_factor = (float)1.25; // qscale factor between ip and b frames
  // context->b_quant_offset = (float)1.25; // qscale offset between ip and b frames

  avcontext->flags |= CODEC_FLAG_PASS1;

  avcontext->mb_decision = FF_MB_DECISION_SIMPLE; // choose only one MB type at a time
  avcontext->me_method = ME_EPZS;
  avcontext->me_subpel_quality = 8;

  avcontext->frame_rate_base = 1;
  avcontext->frame_rate = 15;

  avcontext->gop_size = 64;

  avcontext->flags &= ~CODEC_FLAG_H263P_UMV;
  avcontext->flags &= ~CODEC_FLAG_4MV;
  avcontext->max_b_frames = 0;
  avcontext->flags &= ~CODEC_FLAG_H263P_AIC; // advanced intra coding (not handled by H323_FFH263Capability)

  avcontext->flags |= CODEC_FLAG_RFC2190;

  avcontext->rtp_mode = 1;
  avcontext->rtp_payload_size = 750;
  avcontext->rtp_callback = &H263EncoderContext::RtpCallback;
  avcontext->opaque = this; // used to separate out packets from different encode threads

  return FFMPEGLibraryInstance.AvcodecOpen(avcontext, avcodec) == 0;
}

void H263EncoderContext::CloseCodec()
{
  if (avcontext != NULL) {
    if (avcontext->codec != NULL) {
      FFMPEGLibraryInstance.AvcodecClose(avcontext);
      //PTRACE(5, "H263\tClosed H.263 encoder" );
    }
  }
}

void H263EncoderContext::RtpCallback(void *data, int data_size, void *hdr, int hdr_size, void *priv_data)
{
  H263EncoderContext *c = (H263EncoderContext *) priv_data;
  H263Packet *p;
  if (c->unusedPackets.size() == 0)
    p = new H263Packet();
  else {
    p = *c->unusedPackets.begin();
    c->unusedPackets.erase(c->unusedPackets.begin());
  }
  p->Store(data, data_size, hdr, hdr_size);
  c->encodedPackets.push_back(p);
}

unsigned int H263EncoderContext::GetNextEncodedPacket(RTPFrame & dstRTP, unsigned char payloadCode, unsigned long lastTimeStamp, unsigned & flags)
{
  if (encodedPackets.size() == 0)
    return 0;

  // get the next packet from the unencoded list
  H263Packet *p = *encodedPackets.begin();
  encodedPackets.erase(encodedPackets.begin());

  // this packet will be shortly unused
  unusedPackets.push_back(p);

  // if the packet is too long, throw it away
  if (!p->Read(dstRTP))
    return 0;

  dstRTP.SetMarker(encodedPackets.size() == 0);
  dstRTP.SetPayloadType(payloadCode);
  dstRTP.SetTimestamp(lastTimeStamp);

  flags = 0;
  flags |= (encodedPackets.size() == 0) ? PluginCodec_ReturnCoderLastFrame : 0;  // marker bit on last frame of video

  frameLength += dstRTP.GetPacketLen();

  if (encodedPackets.size() == 0) {
    if (packetDelay) adaptiveDelay(frameLength);
    frameLength = 0;
  }

  return dstRTP.GetPacketLen();
}

int H263EncoderContext::EncodeFrames(const BYTE * src, unsigned & srcLen, BYTE * dst, unsigned & dstLen, unsigned int & flags)
{
  if (!FFMPEGLibraryInstance.IsLoaded())
    return 0;

  // create RTP frame from source buffer
  RTPFrame srcRTP(src, srcLen);

  // create RTP frame from destination buffer
  RTPFrame dstRTP(dst, dstLen, RTP_RFC2190_PAYLOAD);
  dstLen = 0;
//  flags = 0;

  //WaitAndSignal mutex(updateMutex);

  // if there are RTP packets to return, return them
  if (encodedPackets.size() > 0) {
    dstLen = GetNextEncodedPacket(dstRTP, RTP_RFC2190_PAYLOAD, lastTimeStamp, flags);
    return 1;
  }

  // from here, we are encoding a new frame
  lastTimeStamp = srcRTP.GetTimestamp();

  if (srcRTP.GetPayloadSize() < sizeof(PluginCodec_Video_FrameHeader)) {
    //PTRACE(1,"H263\tVideo grab too small, Close down video transmission thread.");
    return 0;
  }

  PluginCodec_Video_FrameHeader * header = (PluginCodec_Video_FrameHeader *)srcRTP.GetPayloadPtr();
  if (header->x != 0 || header->y != 0) {
    //PTRACE(1,"H263\tVideo grab of partial frame unsupported, Close down video transmission thread.");
    return FALSE;
  }

  // if this is the first frame, or the frame size has changed, deal wth it
  if (frameNum == 0 || 
      frameWidth != header->width || 
      frameHeight != header->height) {

#ifndef h323pluslib
    int sizeIndex = GetStdSize(header->width, header->height);
    if (sizeIndex == UnknownStdSize) {
      //PTRACE(3, "H263\tCannot resize to " << header->width << "x" << header->height << " (non-standard format), Close down video transmission thread.");
      return FALSE;
    }
#endif

    frameWidth  = header->width;
    frameHeight = header->height;

    rawFrameLen = (frameWidth * frameHeight * 12) / 8;
    memset(rawFrameBuffer + rawFrameLen, 0, FF_INPUT_BUFFER_PADDING_SIZE);

    encFrameLen = rawFrameLen; // this could be set to some lower value

    CloseCodec();
    if (!OpenCodec())
      return FALSE;
  }

  unsigned char * payload;

  // get payload and ensure correct padding
  if (srcRTP.GetHeaderSize() + (unsigned)(srcRTP.GetPayloadSize() + FF_INPUT_BUFFER_PADDING_SIZE <= srcRTP.GetMaxPacketLen()))
    payload = OPAL_VIDEO_FRAME_DATA_PTR(header);
  else {
    payload = rawFrameBuffer;
    memcpy(payload, OPAL_VIDEO_FRAME_DATA_PTR(header), rawFrameLen);
  }

  int size = frameWidth * frameHeight;
  avpicture->data[0] = payload;
  avpicture->data[1] = avpicture->data[0] + size;
  avpicture->data[2] = avpicture->data[1] + (size / 4);
//  avpicture->pict_type = (flags&PluginCodec_CoderForceIFrame) ? FF_I_TYPE : 0;

  FFMPEGLibraryInstance.AvcodecEncodeVideo(avcontext, encFrameBuffer, encFrameLen, avpicture);
  frameNum++; // increment the number of frames encoded

  if (encodedPackets.size() == 0) {
    //PTRACE(1, "H263\tEncoder internal error - there should be outstanding packets at this point");
    return 1;
  }

  dstLen = GetNextEncodedPacket(dstRTP, RTP_RFC2190_PAYLOAD, lastTimeStamp, flags);
  flags |= PluginCodec_ReturnCoderIFrame;                       // sadly, this encoder *always* returns I-frames :(

  //PTRACE(6, "H263\tEncoded " << src.GetPayloadSize() << " bytes of YUV420P raw data into " << dst.GetSize() << " RTP frame(s)");

  return 1;
}

void H263EncoderContext::adaptiveDelay(unsigned totalLength) {
  #ifdef _WIN32
    long waitBeforeSending =  0; 
    if (newTime!= 0) { // calculate delay and wait
      waitBeforeSending = newTime - GetTickCount();
      if (waitBeforeSending > 0)
        Sleep(waitBeforeSending);
    }
    if (waitFactor) {
      newTime = GetTickCount() +  8000 / waitFactor  * totalLength;
    }
    else {
      newTime = 0;
    }
  #else
    struct timeval currentTime;
    long waitBeforeSending;
    long waitAtNextFrame;
  
    if ((newTime.tv_sec != 0)  || (newTime.tv_usec != 0) ) { // calculate delay and wait
      gettimeofday(&currentTime, NULL); 
      waitBeforeSending = ((newTime.tv_sec - currentTime.tv_sec) * 1000000) + ((newTime.tv_usec - currentTime.tv_usec));  // in useconds
      if (waitBeforeSending > 0) 
        usleep(waitBeforeSending);
    }
    gettimeofday(&currentTime, NULL); 
    if (waitFactor) {
      waitAtNextFrame = waitFactor * totalLength ; // in us in ms
      newTime.tv_sec = currentTime.tv_sec + (int)((waitAtNextFrame  + currentTime.tv_usec) / 1000000);
      newTime.tv_usec = (int)((waitAtNextFrame + currentTime.tv_usec) % 1000000);
    }
    else {
      newTime.tv_sec = 0;
      newTime.tv_usec = 0;
    }
  #endif
}


static void * create_encoder(const struct PluginCodec_Definition * /*codec*/)
{
  return new H263EncoderContext;
}

static int encoder_set_options(const PluginCodec_Definition *, 
                               void * _context,
                               const char * , 
                               void * parm, 
                               unsigned * parmLen)
{
  H263EncoderContext * context = (H263EncoderContext *)_context;
  if (parmLen == NULL || *parmLen != sizeof(const char **) || parm == NULL)
    return 0;

  // get the "frame width" media format parameter to use as a hint for the encoder to start off
  for (const char * const * option = (const char * const *)parm; *option != NULL; option += 2) {
    if (STRCMPI(option[0], "Frame Width") == 0)
      context->frameWidth = atoi(option[1]);
    if (STRCMPI(option[0], "Frame Height") == 0)
      context->frameHeight = atoi(option[1]);
    if (STRCMPI(option[0], "Adaptive Packet Delay") == 0)
      context->packetDelay = atoi(option[1]);
	if (STRCMPI(option[0], "Encoding Quality") == 0) 
      context->videoQuality = MIN(context->videoQMax, MAX(atoi(option[1]), context->videoQMin));
	if (STRCMPI(option[0], "Max Bit Rate") == 0) 
	  context->bitRate = atoi(option[1]);
	if (STRCMPI(option[0], "set_min_quality") == 0) 
      context->videoQMin = atoi(option[1]);
	if (STRCMPI(option[0], "set_max_quality") == 0)
      context->videoQMax = atoi(option[1]);
  }

  return 1;
}

static void destroy_encoder(const struct PluginCodec_Definition * /*codec*/, void * _context)
{
  H263EncoderContext * context = (H263EncoderContext *)_context;
  delete context;
}

static int codec_encoder(const struct PluginCodec_Definition * , 
                                           void * _context,
                                     const void * from, 
                                       unsigned * fromLen,
                                           void * to,         
                                       unsigned * toLen,
                                   unsigned int * flag)
{
  H263EncoderContext * context = (H263EncoderContext *)_context;
  return context->EncodeFrames((const BYTE *)from, *fromLen, (BYTE *)to, *toLen, *flag);
}


/////////////////////////////////////////////////////////////////////////////

class H263DecoderContext
{
  public:
    H263DecoderContext();
    ~H263DecoderContext();

    bool DecodeFrames(const BYTE * src, unsigned & srcLen, BYTE * dst, unsigned & dstLen, unsigned int & flags);

  protected:
    bool OpenCodec();
    void CloseCodec();

    unsigned char encFrameBuffer[MAX_H263_PACKET_SIZE];

    AVCodec        *avcodec;
    AVCodecContext *avcontext;
    AVFrame        *picture;

    int frameNum;
    unsigned int frameWidth;
    unsigned int frameHeight;
    
    BOOL freezeVideo;
};

H263DecoderContext::H263DecoderContext()
{
  if (!FFMPEGLibraryInstance.IsLoaded())
    return;

  if ((avcodec = FFMPEGLibraryInstance.AvcodecFindDecoder(CODEC_ID_H263)) == NULL) {
    //PTRACE(1, "H263\tCodec not found for decoder");
    return;
  }

  frameWidth  = CIF_WIDTH;
  frameHeight = CIF_HEIGHT;

  avcontext = FFMPEGLibraryInstance.AvcodecAllocContext();
  if (avcontext == NULL) {
    //PTRACE(1, "H263\tFailed to allocate context for decoder");
    return;
  }

  picture = FFMPEGLibraryInstance.AvcodecAllocFrame();
  if (picture == NULL) {
    //PTRACE(1, "H263\tFailed to allocate frame for decoder");
    return;
  }

  if (!OpenCodec()) { // decoder will re-initialise context with correct frame size
    //PTRACE(1, "H263\tFailed to open codec for decoder");
    return;
  }

  frameNum = 0;
  freezeVideo = FALSE;

  //PTRACE(3, "Codec\tH263 decoder created");
}

H263DecoderContext::~H263DecoderContext()
{
  if (FFMPEGLibraryInstance.IsLoaded()) {
    CloseCodec();

    FFMPEGLibraryInstance.AvcodecFree(avcontext);
    FFMPEGLibraryInstance.AvcodecFree(picture);
  }
}

bool H263DecoderContext::OpenCodec()
{
  // avoid copying input/output
  avcontext->flags |= CODEC_FLAG_INPUT_PRESERVED; // we guarantee to preserve input for max_b_frames+1 frames
  avcontext->flags |= CODEC_FLAG_EMU_EDGE; // don't draw edges
//  avcontext->flags |= CODEC_FLAG_H263P_UMV;
  avcontext->width  = frameWidth;
  avcontext->height = frameHeight;

  avcontext->workaround_bugs = 0; // no workaround for buggy H.263 implementations
  avcontext->error_concealment = FF_EC_GUESS_MVS | FF_EC_DEBLOCK;
  avcontext->error_resilience = FF_ER_CAREFULL;

  if (FFMPEGLibraryInstance.AvcodecOpen(avcontext, avcodec) < 0) {
    //PTRACE(1, "H263\tFailed to open H.263 decoder");
    return FALSE;
  }

  return TRUE;
}

void H263DecoderContext::CloseCodec()
{
  if (avcontext != NULL) {
    if (avcontext->codec != NULL) {
      FFMPEGLibraryInstance.AvcodecClose(avcontext);
      //PTRACE(5, "H263\tClosed H.263 decoder" );
    }
  }
}

bool H263DecoderContext::DecodeFrames(const BYTE * src, unsigned & srcLen, BYTE * dst, unsigned & dstLen, unsigned int & flags)
{

  
  if (!FFMPEGLibraryInstance.IsLoaded())
    return 0;

  // create RTP frame from source buffer
  RTPFrame srcRTP(src, srcLen);

  // create RTP frame from destination buffer
  RTPFrame dstRTP(dst, dstLen, 0);
  dstLen = 0;

  if(flags!=freezeVideo)
  {
   if(flags)
   {
    if(srcRTP.GetMarker()) freezeVideo=flags; // last packet of current frame received, stop decoding
   }
   else
   {
    freezeVideo=flags; // start decoding
    flags = PluginCodec_ReturnCoderRequestIFrame;
    return 1;
   } 
  }
  else if(freezeVideo) { flags = 0; return 1; }

  flags = 0;
  
  int srcPayloadSize = srcRTP.GetPayloadSize();
  unsigned char * payload;

  // copy payload to a temporary buffer if there are not enough bytes after the end of the payload
  if (srcRTP.GetHeaderSize() + srcPayloadSize + FF_INPUT_BUFFER_PADDING_SIZE > srcLen) {
    if (srcPayloadSize + FF_INPUT_BUFFER_PADDING_SIZE > sizeof(encFrameBuffer))
      return 0; 

    memcpy(encFrameBuffer, srcRTP.GetPayloadPtr(), srcPayloadSize);
    payload = encFrameBuffer;
  }
  else
    payload = (unsigned char *) srcRTP.GetPayloadPtr();

  // ensure the first 24 bits past the end of the payload are all zero
  {
    unsigned char * padding = payload + srcPayloadSize;
    padding[0] = padding[1] = padding[2] = 0;
  }

  // only accept RFC 2190 for now
  switch (srcRTP.GetPayloadType()) {
    case RTP_RFC2190_PAYLOAD:
      avcontext->flags |= CODEC_FLAG_RFC2190;
      break;

    //case RTP_DYNAMIC_PAYLOAD:
    //  avcontext->flags |= RTPCODEC_FLAG_RFC2429
    //  break;
    default:
      return 1;
  }

  // decode the frame
  int got_picture;
  int len = FFMPEGLibraryInstance.AvcodecDecodeVideo(avcontext, picture, &got_picture, payload, srcPayloadSize);

  // if that was not the last packet for the frame, keep going
  if (!srcRTP.GetMarker()) {
    return 1;
  }

  // cause decoder to end the frame
  len = FFMPEGLibraryInstance.AvcodecDecodeVideo(avcontext, picture, &got_picture, NULL, -1);

  // if error occurred, tell the other end to send another I-frame and hopefully we can resync
  if (len < 0) {
    flags = PluginCodec_ReturnCoderRequestIFrame;
    cout << "Case1\n";
    return 1;
  }

  // no picture was decoded - shrug and do nothing
  if (!got_picture)
    return 1;

  // if decoded frame size is not legal, request an I-Frame
  if (avcontext->width == 0 || avcontext->height == 0) {
    flags = PluginCodec_ReturnCoderRequestIFrame;
    cout << "Case2\n";
    return 1;
  }

  // see if frame size has changed
  if (frameWidth != (unsigned)avcontext->width || frameHeight != (unsigned)avcontext->height) {
    frameWidth  = avcontext->width;
    frameHeight = avcontext->height;
  }

  int frameBytes = (frameWidth * frameHeight * 12) / 8;

  // if the frame decodes to more than we can handle, ignore the frame
  if ((sizeof(PluginCodec_Video_FrameHeader) + frameBytes) > dstRTP.GetPayloadSize())
    return 1;

  PluginCodec_Video_FrameHeader * header = (PluginCodec_Video_FrameHeader *)dstRTP.GetPayloadPtr();
  header->x = header->y = 0;
  header->width = frameWidth;
  header->height = frameHeight;
  int size = frameWidth * frameHeight;
  if (picture->data[1] == picture->data[0] + size
      && picture->data[2] == picture->data[1] + (size >> 2))
    memcpy(OPAL_VIDEO_FRAME_DATA_PTR(header), picture->data[0], frameBytes);
  else {
    unsigned char *dst = OPAL_VIDEO_FRAME_DATA_PTR(header);
    for (int i=0; i<3; i ++) {
      unsigned char *src = picture->data[i];
      int dst_stride = i ? frameWidth >> 1 : frameWidth;
      int src_stride = picture->linesize[i];
      int h = i ? frameHeight >> 1 : frameHeight;

      if (src_stride==dst_stride) {
        memcpy(dst, src, dst_stride*h);
        dst += dst_stride*h;
      } else {
        while (h--) {
          memcpy(dst, src, dst_stride);
          dst += dst_stride;
          src += src_stride;
        }
      }
    }
  }

  dstRTP.SetPayloadSize(sizeof(PluginCodec_Video_FrameHeader) + frameBytes);
  dstRTP.SetPayloadType(RTP_DYNAMIC_PAYLOAD);
  dstRTP.SetTimestamp(srcRTP.GetTimestamp());
  dstRTP.SetMarker(TRUE);

  dstLen = dstRTP.GetPacketLen();

  flags = PluginCodec_ReturnCoderLastFrame ;   // TODO: THIS NEEDS TO BE CHANGED TO DO CORRECT IFRAME DETECTION

  frameNum++;

  return 1;
}


static void * create_decoder(const struct PluginCodec_Definition *)
{
  return new H263DecoderContext;
}

static void destroy_decoder(const struct PluginCodec_Definition * /*codec*/, void * _context)
{
  H263DecoderContext * context = (H263DecoderContext *)_context;
  delete context;
}

static int codec_decoder(const struct PluginCodec_Definition *, 
                                           void * _context,
                                     const void * from, 
                                       unsigned * fromLen,
                                           void * to,         
                                       unsigned * toLen,
                                   unsigned int * flag)
{
  H263DecoderContext * context = (H263DecoderContext *)_context;
  return context->DecodeFrames((const BYTE *)from, *fromLen, (BYTE *)to, *toLen, *flag);
}

static int decoder_get_output_data_size(const PluginCodec_Definition * codec, void *, const char *, void *, unsigned *)
{
  // this is really frame height * frame width;
  return RTP_MIN_HEADER_SIZE + sizeof(PluginCodec_Video_FrameHeader) + ((codec->parm.video.maxFrameWidth * codec->parm.video.maxFrameHeight * 3) / 2);
}

static int get_codec_options(const struct PluginCodec_Definition * codec,
                             void *, 
                             const char *,
                             void * parm,
                             unsigned * parmLen)
{
  if (parmLen == NULL || parm == NULL || *parmLen != sizeof(struct PluginCodec_Option **))
    return 0;

  *(const void **)parm = codec->userData;
  return 1;
}


/////////////////////////////////////////////////////////////////////////////


static struct PluginCodec_information licenseInfo = {
  1145863600,                                                   // timestamp =  Mon 24 Apr 2006 07:26:40 AM UTC

  "Craig Southeren, Guilhem Tardy, Derek Smithies",             // source code author
  "1.0",                                                        // source code version
  "openh323@openh323.org",                                      // source code email
  "http://sourceforge.net/projects/openh323",                   // source code URL
  "Copyright (C) 2006 by Post Increment",                       // source code copyright
  ", Copyright (C) 2005 Salyens"
  ", Copyright (C) 2001 March Networks Corporation"
  ", Copyright (C) 1999-2000 Equivalence Pty. Ltd."
  "MPL 1.0",                                                    // source code license
  PluginCodec_License_MPL,                                      // source code license
  
  "FFMPEG",                                                     // codec description
  "Michael Niedermayer, Fabrice Bellard",                       // codec author
  "4.7.1",                                                      // codec version
  "ffmpeg-devel-request@ mplayerhq.hu",                         // codec email
  "http://sourceforge.net/projects/ffmpeg/",                    // codec URL
  "Copyright (c) 2000-2001 Fabrice Bellard"                     // codec copyright information
  ", Copyright (c) 2002-2003 Michael Niedermayer",
  "GNU LESSER GENERAL PUBLIC LICENSE, Version 2.1, February 1999", // codec license
  PluginCodec_License_LGPL                                         // codec license code
};

static const char YUV420PDesc[]  = { "YUV420P" };

static const char h263QCIFDesc[]  = { "H.263-QCIF" };
static const char h263CIFDesc[]   = { "H.263-CIF" };
static const char h2634CIFDesc[]  = { "H.263-4CIF" };
static const char h263Desc[]      = { "H.263" };

static const char sdpH263[]   = { "h263" };

static PluginCodec_ControlDefn EncoderControls[] = {
  { "get_codec_options", get_codec_options },
  { "set_codec_options", encoder_set_options },
  { NULL }
};

static PluginCodec_ControlDefn DecoderControls[] = {
  { "get_codec_options",    get_codec_options },
  { "get_output_data_size", decoder_get_output_data_size },
  { NULL }
};

static struct PluginCodec_Option const qcifMPI =
  { PluginCodec_IntegerOption, "QCIF MPI", false, PluginCodec_MaxMerge, "6", "QCIF", "0", 0, "0", "6" };

static struct PluginCodec_Option const cifMPI =
  { PluginCodec_IntegerOption, "CIF MPI",  false, PluginCodec_MaxMerge, "6", "CIF",  "0", 0, "0", "6" };

static struct PluginCodec_Option const sqcifMPI =
  { PluginCodec_IntegerOption, "SQCIF MPI", false, PluginCodec_MaxMerge, "6", "SQCIF", "0", 0, "0", "6" };

static struct PluginCodec_Option const cif4MPI =
  { PluginCodec_IntegerOption, "CIF4 MPI",  false, PluginCodec_MaxMerge, "6", "CIF4", "0", 0, "0", "6" };

static struct PluginCodec_Option const cif16MPI =
  { PluginCodec_IntegerOption, "CIF16 MPI", false, PluginCodec_MaxMerge, "1", "CIF16", "0", 0, "0", "4" };

// The following is optional for H.263, but not for H.263+ where the value should be RFC2429
static struct PluginCodec_Option const mediaPacketization =
  { PluginCodec_StringOption,  "Media Packetization",  0, PluginCodec_EqualMerge, "RFC2190" };

/* All of the annexes below are turned off and set to read/only because this
   implementation does not support them. Their presence here is so that if
   someone out there does a different implementation of the codec and copies
   this file as a template, they will get them and hopefully notice that they
   can just make them read/write and/or turned on.
 */
static struct PluginCodec_Option const annexF =
  { PluginCodec_BoolOption,    "Annex F",   true,  PluginCodec_AndMerge, "0", "F", "0" };

static struct PluginCodec_Option const annexI =
  { PluginCodec_BoolOption,    "Annex I",   true,  PluginCodec_AndMerge, "0", "I", "0" };

static struct PluginCodec_Option const annexJ =
  { PluginCodec_BoolOption,    "Annex J",   true,  PluginCodec_AndMerge, "0", "J", "0" };

static struct PluginCodec_Option const annexK =
  { PluginCodec_IntegerOption, "Annex K",   true,  PluginCodec_EqualMerge, "0", "K", "0", 0, "0", "4" };

static struct PluginCodec_Option const annexN =
  { PluginCodec_BoolOption,    "Annex N",   true,  PluginCodec_AndMerge, "0", "N", "0" };

static struct PluginCodec_Option const annexP =
  { PluginCodec_BoolOption,    "Annex P",   true,  PluginCodec_AndMerge, "0", "P", "0" };

static struct PluginCodec_Option const annexT =
  { PluginCodec_BoolOption,    "Annex T",   true,  PluginCodec_AndMerge, "0", "T", "0" };

static struct PluginCodec_Option const * const qcifOptionTable[] = {
//  &mediaPacketization,
  &qcifMPI,
  NULL
};

static struct PluginCodec_Option const * const cifOptionTable[] = {
//  &mediaPacketization,
  &cifMPI,
  NULL
};

static struct PluginCodec_Option const * const cif4OptionTable[] = {
//  &mediaPacketization,
  &cif4MPI,
  NULL
};

static struct PluginCodec_Option const * const cif16OptionTable[] = {
//  &mediaPacketization,
  &cif16MPI,
  NULL
};

/////////////////////////////////////////////////////////////////////////////

static struct PluginCodec_Definition h263CodecDefn[8] = {

{ 
  // 16CIF only encoder
  PLUGIN_CODEC_VERSION_OPTIONS,       // codec API version
  &licenseInfo,                       // license information

  PluginCodec_MediaTypeVideo |        // video codec
  PluginCodec_RTPTypeExplicit,        // specified RTP type

  "H.263-16CIF",                       // text decription
  YUV420PDesc,                        // source format
  "H.263-16CIF",                       // destination format

  cif16OptionTable,                    // user data 

  H263_CLOCKRATE,                     // samples per second
  H263_BITRATE,                       // raw bits per second
  20000,                              // nanoseconds per frame

  CIF16_WIDTH,                         // frame width
  CIF16_HEIGHT,                        // frame height
  10,                                 // recommended frame rate
  60,                                 // maximum frame rate
  RTP_RFC2190_PAYLOAD,                // IANA RTP payload code
  sdpH263,                            // RTP payload name

  create_encoder,                     // create codec function
  destroy_encoder,                    // destroy codec
  codec_encoder,                      // encode/decode
  EncoderControls,                    // codec controls

  PluginCodec_H323VideoCodec_h263,    // h323CapabilityType 
  NULL                                // h323CapabilityData
},
{ 
  // 4CIF only decoder
  PLUGIN_CODEC_VERSION_OPTIONS,       // codec API version
  &licenseInfo,                       // license information

  PluginCodec_MediaTypeVideo |        // video codec
  PluginCodec_RTPTypeExplicit,        // specified RTP type

  "H.263-16CIF",                       // text decription
  "H.263-16CIF",                       // source format
  YUV420PDesc,                        // destination format

  cif16OptionTable,                    // user data 

  H263_CLOCKRATE,                     // samples per second
  H263_BITRATE,                       // raw bits per second
  20000,                              // nanoseconds per frame

  CIF16_WIDTH,                         // frame width
  CIF16_HEIGHT,                        // frame height
  10,                                 // recommended frame rate
  60,                                 // maximum frame rate
  RTP_RFC2190_PAYLOAD,                // IANA RTP payload code
  sdpH263,                            // RTP payload name

  create_decoder,                     // create codec function
  destroy_decoder,                    // destroy codec
  codec_decoder,                      // encode/decode
  DecoderControls,                    // codec controls

  PluginCodec_H323VideoCodec_h263,    // h323CapabilityType 
  NULL                                // h323CapabilityData
},

{ 
  // 4CIF only encoder
  PLUGIN_CODEC_VERSION_OPTIONS,       // codec API version
  &licenseInfo,                       // license information

  PluginCodec_MediaTypeVideo |        // video codec
  PluginCodec_RTPTypeExplicit,        // specified RTP type

  h2634CIFDesc,                       // text decription
  YUV420PDesc,                        // source format
  h2634CIFDesc,                       // destination format

  cif4OptionTable,                    // user data 

  H263_CLOCKRATE,                     // samples per second
  H263_BITRATE,                       // raw bits per second
  20000,                              // nanoseconds per frame

  CIF4_WIDTH,                         // frame width
  CIF4_HEIGHT,                        // frame height
  10,                                 // recommended frame rate
  60,                                 // maximum frame rate
  RTP_RFC2190_PAYLOAD,                // IANA RTP payload code
  sdpH263,                            // RTP payload name

  create_encoder,                     // create codec function
  destroy_encoder,                    // destroy codec
  codec_encoder,                      // encode/decode
  EncoderControls,                    // codec controls

  PluginCodec_H323VideoCodec_h263,    // h323CapabilityType 
  NULL                                // h323CapabilityData
},
{ 
  // 4CIF only decoder
  PLUGIN_CODEC_VERSION_OPTIONS,       // codec API version
  &licenseInfo,                       // license information

  PluginCodec_MediaTypeVideo |        // video codec
  PluginCodec_RTPTypeExplicit,        // specified RTP type

  h2634CIFDesc,                       // text decription
  h2634CIFDesc,                       // source format
  YUV420PDesc,                        // destination format

  cif4OptionTable,                    // user data 

  H263_CLOCKRATE,                     // samples per second
  H263_BITRATE,                       // raw bits per second
  20000,                              // nanoseconds per frame

  CIF4_WIDTH,                         // frame width
  CIF4_HEIGHT,                        // frame height
  10,                                 // recommended frame rate
  60,                                 // maximum frame rate
  RTP_RFC2190_PAYLOAD,                // IANA RTP payload code
  sdpH263,                            // RTP payload name

  create_decoder,                     // create codec function
  destroy_decoder,                    // destroy codec
  codec_decoder,                      // encode/decode
  DecoderControls,                    // codec controls

  PluginCodec_H323VideoCodec_h263,    // h323CapabilityType 
  NULL                                // h323CapabilityData
},
{
  // CIF only encoder
  PLUGIN_CODEC_VERSION_OPTIONS,       // codec API version
  &licenseInfo,                       // license information

  PluginCodec_MediaTypeVideo |        // video codec
  PluginCodec_RTPTypeExplicit,        // specified RTP type

  h263CIFDesc,                        // text decription
  YUV420PDesc,                        // source format
  h263CIFDesc,                        // destination format

  cifOptionTable,                     // user data 

  H263_CLOCKRATE,                     // samples per second
  H263_BITRATE,                       // raw bits per second
  20000,                              // nanoseconds per frame

  CIF_WIDTH,                          // frame width
  CIF_HEIGHT,                         // frame height
  10,                                 // recommended frame rate
  60,                                 // maximum frame rate
  RTP_RFC2190_PAYLOAD,                // IANA RTP payload code
  sdpH263,                            // RTP payload name

  create_encoder,                     // create codec function
  destroy_encoder,                    // destroy codec
  codec_encoder,                      // encode/decode
  EncoderControls,                    // codec controls

  PluginCodec_H323VideoCodec_h263,    // h323CapabilityType 
  NULL                                // h323CapabilityData
},
{ 
  // CIF only decoder
  PLUGIN_CODEC_VERSION_OPTIONS,       // codec API version
  &licenseInfo,                       // license information

  PluginCodec_MediaTypeVideo |        // video codec
  PluginCodec_RTPTypeExplicit,        // specified RTP type

  h263CIFDesc,                        // text decription
  h263CIFDesc,                        // source format
  YUV420PDesc,                        // destination format

  cifOptionTable,                     // user data 

  H263_CLOCKRATE,                     // samples per second
  H263_BITRATE,                       // raw bits per second
  20000,                              // nanoseconds per frame

  CIF_WIDTH,                          // frame width
  CIF_HEIGHT,                         // frame height
  10,                                 // recommended frame rate
  60,                                 // maximum frame rate
  RTP_RFC2190_PAYLOAD,                // IANA RTP payload code
  sdpH263,                            // RTP payload name

  create_decoder,                     // create codec function
  destroy_decoder,                    // destroy codec
  codec_decoder,                      // encode/decode
  DecoderControls,                    // codec controls

  PluginCodec_H323VideoCodec_h263,    // h323CapabilityType 
  NULL                                // h323CapabilityData
},

{ 
  // QCIF only encoder
  PLUGIN_CODEC_VERSION_OPTIONS,       // codec API version
  &licenseInfo,                       // license information

  PluginCodec_MediaTypeVideo |        // audio codec
  PluginCodec_RTPTypeExplicit,        // specified RTP type

  h263QCIFDesc,                       // text decription
  YUV420PDesc,                        // source format
  h263QCIFDesc,                       // destination format

  qcifOptionTable,                    // user data 

  H263_CLOCKRATE,                     // samples per second
  H263_BITRATE,                       // raw bits per second
  20000,                              // nanoseconds per frame

  QCIF_WIDTH,                         // frame width
  QCIF_HEIGHT,                        // frame height
  10,                                 // recommended frame rate
  60,                                 // maximum frame rate
  RTP_RFC2190_PAYLOAD,                // IANA RTP payload code
  sdpH263,                            // RTP payload name

  create_encoder,                     // create codec function
  destroy_encoder,                    // destroy codec
  codec_encoder,                      // encode/decode
  EncoderControls,                    // codec controls

  PluginCodec_H323VideoCodec_h263,    // h323CapabilityType 
  NULL                                // h323CapabilityData
},
{ 
  // QCIF only decoder
  PLUGIN_CODEC_VERSION_OPTIONS,       // codec API version
  &licenseInfo,                       // license information

  PluginCodec_MediaTypeVideo |        // audio codec
  PluginCodec_RTPTypeExplicit,        // specified RTP type

  h263QCIFDesc,                       // text decription
  h263QCIFDesc,                       // source format
  YUV420PDesc,                        // destination format

  qcifOptionTable,                    // user data 

  H263_CLOCKRATE,                     // samples per second
  H263_BITRATE,                       // raw bits per second
  20000,                              // nanoseconds per frame

  QCIF_WIDTH,                         // frame width
  QCIF_HEIGHT,                        // frame height
  10,                                 // recommended frame rate
  60,                                 // maximum frame rate
  RTP_RFC2190_PAYLOAD,                // IANA RTP payload code
  sdpH263,                            // RTP payload name

  create_decoder,                     // create codec function
  destroy_decoder,                    // destroy codec
  codec_decoder,                      // encode/decode
  DecoderControls,                    // codec controls

  PluginCodec_H323VideoCodec_h263,    // h323CapabilityType 
  NULL                                // h323CapabilityData
},

};


/////////////////////////////////////////////////////////////////////////////

extern "C" {
  PLUGIN_CODEC_IMPLEMENT(FFMPEG_H263)

  PLUGIN_CODEC_DLL_API struct PluginCodec_Definition * PLUGIN_CODEC_GET_CODEC_FN(unsigned * count, unsigned version)
  {
    // check version numbers etc
    if (version < PLUGIN_CODEC_VERSION_OPTIONS || !FFMPEGLibraryInstance.Load()) {
      *count = 0;
      return NULL;
    }

    *count = sizeof(h263CodecDefn) / sizeof(struct PluginCodec_Definition);
    return h263CodecDefn;
  }

};
