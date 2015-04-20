
#include "precompile.h"

#ifndef _MCU_UTILS_AV_H
#define _MCU_UTILS_AV_H

#include "utils_type.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(54,25,0)
  #define AVCodecID               CodecID
  #define AV_CODEC_ID_NONE        CODEC_ID_NONE
  #define AV_CODEC_ID_PCM_S16LE   CODEC_ID_PCM_S16LE
  #define AV_CODEC_ID_AC3         CODEC_ID_AC3
  #define AV_CODEC_ID_H264        CODEC_ID_H264
  #define AV_CODEC_ID_MPEG4       CODEC_ID_MPEG4
  #define AV_CODEC_ID_MSMPEG4V3   CODEC_ID_MSMPEG4V3
  #define AV_CODEC_ID_VP8         CODEC_ID_VP8
  #define AV_CODEC_ID_MJPEG       CODEC_ID_MJPEG
#endif

#if LIBAVUTILS_VERSION_INT < AV_VERSION_INT(51,42,0)
  #define AV_PIX_FMT_YUV420P    PIX_FMT_YUV420P
  #define AV_PIX_FMT_YUVJ420P   PIX_FMT_YUVJ420P
  #define AV_PIX_FMT_YUVJ444P   PIX_FMT_YUVJ444P
#endif


#define AV_ALIGN 1

// avcodecMutex - используется при создании/удалении кодеков
// insufficient thread locking around avcodec_open/close()
extern PMutex avcodecMutex;

////////////////////////////////////////////////////////////////////////////////////////////////////

#define CIF_WIDTH     352
#define CIF_HEIGHT    288
#define CIF_SIZE      (CIF_WIDTH*CIF_HEIGHT*3/2)

#define QCIF_WIDTH    (CIF_WIDTH / 2)
#define QCIF_HEIGHT   (CIF_HEIGHT / 2)
#define QCIF_SIZE     (QCIF_WIDTH*QCIF_HEIGHT*3/2)

#define SQCIF_WIDTH    (QCIF_WIDTH / 2)
#define SQCIF_HEIGHT   (QCIF_HEIGHT / 2)
#define SQCIF_SIZE     (SQCIF_WIDTH*SQCIF_HEIGHT*3/2)

#define CIF4_WIDTH     (CIF_WIDTH * 2)
#define CIF4_HEIGHT    (CIF_HEIGHT * 2)
#define CIF4_SIZE      (CIF4_WIDTH*CIF4_HEIGHT*3/2)

#define CIF16_WIDTH     (CIF4_WIDTH * 2)
#define CIF16_HEIGHT    (CIF4_HEIGHT * 2)
#define CIF16_SIZE      (CIF16_WIDTH*CIF16_HEIGHT*3/2)

#define SQ3CIF_WIDTH    116
#define SQ3CIF_HEIGHT   96
#define SQ3CIF_SIZE     (SQ3CIF_WIDTH*SQ3CIF_HEIGHT*3/2)

#define Q3CIF_WIDTH    (2*SQ3CIF_WIDTH)
#define Q3CIF_HEIGHT   (2*SQ3CIF_HEIGHT)
#define Q3CIF_SIZE     (Q3CIF_WIDTH*Q3CIF_HEIGHT*3/2)

#define Q3CIF4_WIDTH    (4*SQ3CIF_WIDTH)
#define Q3CIF4_HEIGHT   (4*SQ3CIF_HEIGHT)
#define Q3CIF4_SIZE     (Q3CIF4_WIDTH*Q3CIF4_HEIGHT*3/2)

#define Q3CIF16_WIDTH    (8*SQ3CIF_WIDTH)
#define Q3CIF16_HEIGHT   (8*SQ3CIF_HEIGHT)
#define Q3CIF16_SIZE     (Q3CIF16_WIDTH*Q3CIF16_HEIGHT*3/2)

#define SQ5CIF_WIDTH    140
#define SQ5CIF_HEIGHT   112
#define SQ5CIF_SIZE     (SQ5CIF_WIDTH*SQ5CIF_HEIGHT*3/2)

#define Q5CIF_WIDTH    (2*SQ5CIF_WIDTH)
#define Q5CIF_HEIGHT   (2*SQ5CIF_HEIGHT)
#define Q5CIF_SIZE     (Q5CIF_WIDTH*Q5CIF_HEIGHT*3/2)

#define TCIF_WIDTH    (CIF_WIDTH*3)
#define TCIF_HEIGHT   (CIF_HEIGHT*3)
#define TCIF_SIZE     (TCIF_WIDTH*TCIF_HEIGHT*3/2)

#define TQCIF_WIDTH    (CIF_WIDTH*3 / 2)
#define TQCIF_HEIGHT   (CIF_HEIGHT*3 / 2)
#define TQCIF_SIZE     (TQCIF_WIDTH*TQCIF_HEIGHT*3/2)

#define TSQCIF_WIDTH    (CIF_WIDTH*3 / 4)
#define TSQCIF_HEIGHT   (CIF_HEIGHT*3 / 4)
#define TSQCIF_SIZE     (TSQCIF_WIDTH*TSQCIF_HEIGHT*3/2)

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL MCU_AVEncodeFrame(AVCodecID codec_id, const void * src, int src_size, void * dst, int & dst_size, int src_width, int src_height);
BOOL MCU_AVDecodeFrameFromFile(PString & filename, void *dst, int & dst_size, int & dst_width, int & dst_height);

unsigned GetVideoMacroBlocks(unsigned width, unsigned height);
BOOL GetParamsH263(PString & mpiname, unsigned & width, unsigned & height);
BOOL GetParamsH264(unsigned & level, unsigned & level_h241, unsigned & max_fs);
BOOL GetParamsH264(unsigned & level, unsigned & level_h241, unsigned & max_fs, unsigned & max_mbps, unsigned & max_br);
BOOL GetParamsH264(unsigned & level, unsigned & level_h241, unsigned & max_fs, unsigned & max_mbps, unsigned & max_br, unsigned & width, unsigned & height);
BOOL GetParamsMpeg4(unsigned & profile_level, unsigned & profile, unsigned & level, unsigned & max_fs);
BOOL GetParamsMpeg4(unsigned & profile_level, unsigned & profile, unsigned & level, unsigned & max_fs, unsigned & width, unsigned & height);

void SetFormatParams(OpalMediaFormat & wf, unsigned width, unsigned height);
void SetFormatParams(OpalMediaFormat & wf, unsigned width, unsigned height, unsigned frame_rate, unsigned bandwidth);
void SetFormatParamsH261(OpalMediaFormat & wf, unsigned width, unsigned height);
void SetFormatParamsH263(OpalMediaFormat & wf, unsigned width, unsigned height);
void SetFormatParamsH264(OpalMediaFormat & wf, unsigned width, unsigned height);
void SetFormatParamsMPEG4(OpalMediaFormat & wf, unsigned width, unsigned height);
void SetFormatParamsVP8(OpalMediaFormat & wf, unsigned width, unsigned height);

////////////////////////////////////////////////////////////////////////////////////////////////////

const uint64_t MCU_AV_CH_Layout_Selector[] =
{
  0
  ,AV_CH_LAYOUT_MONO
  ,AV_CH_LAYOUT_STEREO
  ,AV_CH_LAYOUT_2_1
  ,AV_CH_LAYOUT_3POINT1
  ,AV_CH_LAYOUT_5POINT0
  ,AV_CH_LAYOUT_5POINT1
  ,AV_CH_LAYOUT_7POINT0
  ,AV_CH_LAYOUT_7POINT1
};

////////////////////////////////////////////////////////////////////////////////////////////////////

static struct vp8_resolution {
  const char *capname;
  unsigned width;
  unsigned height;
} const vp8_resolutions[] = {
  { "VP8-QCIF",  176,  144  },
  { NULL,        320,  240  },
  { "VP8-CIF",   352,  288  },
  { "VP8-360P",  640,  360  },
  { NULL,        640,  480  },
  { "VP8-4CIF",  704,  576  },
  { NULL,        800,  600  },
  { "VP8-480P",  852,  480  },
  { NULL,        1024, 768  },
  { "VP8-720P",  1280, 720  },
  { NULL,        1280, 1024 },
  { "VP8-768P",  1364, 768  },
  { "VP8-1080P", 1920, 1080 },
  { NULL }
};

////////////////////////////////////////////////////////////////////////////////////////////////////

static struct h263_resolution {
  const char *mpiname;
  unsigned width;
  unsigned height;
} const h263_resolutions[] = {
  { "SQCIF", 128,  96   },
  { "QCIF",  176,  144  },
  { "CIF",   352,  288  },
  { "CIF4",  704,  576  },
  { "CIF16", 1408, 1152 },
  { NULL }
};

////////////////////////////////////////////////////////////////////////////////////////////////////

static struct mpeg4_profile_level {
  unsigned profile_level;
  const char* profile_name;
  unsigned profile;
  unsigned level;
  unsigned max_fs;
  unsigned max_br;
  unsigned width;
  unsigned height;
} const mpeg4_profile_levels[] = {
  {   1, "Simple",                     1, 1, 99,    64000,    176,  144  },
  {   2, "Simple",                     1, 2, 792,   128000,   352,  288  },
  {   3, "Simple",                     1, 3, 792,   384000,   352,  288  },
  {   4, "Simple",                     1, 4, 1200,  4000000,  352,  288  },
  {   5, "Simple",                     1, 5, 1620,  8000000,  704,  576  },
  {   8, "Simple",                     1, 0, 99,    64000,    176,  144  },
  {   9, "Simple",                     1, 0, 99,    128000,   176,  144  },
//  {  17, "Simple Scalable",            2, 1, 495,   128000,   0,    0    },
//  {  18, "Simple Scalable",            2, 2, 792,   256000,   0,    0    },
//  {  33, "Core",                       3, 1, 198,   384000,   0,    0    },
//  {  34, "Core",                       3, 2, 792,   2000000,  0,    0    },
//  {  50, "Main",                       4, 2, 1188,  2000000,  0,    0    },
//  {  51, "Main",                       4, 3, 3240,  15000000, 0,    0    },
//  {  52, "Main",                       4, 4, 16320, 38400000, 0,    0    },
//  {  66, "N-Bit",                      5, 2, 792,   2000000,  0,    0    },
  { 145, "Advanced Real Time Simple",  6, 1, 99,    64000,    176,  144  },
  { 146, "Advanced Real Time Simple",  6, 2, 396,   128000,   352,  288  },
  { 147, "Advanced Real Time Simple",  6, 3, 396,   384000,   352,  288  },
  { 148, "Advanced Real Time Simple",  6, 4, 396,   2000000,  352,  288  },
//  { 161, "Core Scalable",              7, 1, 792,   768000,   0,    0    },
//  { 162, "Core Scalable",              7, 2, 990,   1500000,  0,    0    },
//  { 163, "Core Scalable",              7, 3, 4032,  4000000,  0,    0    },
//  { 177, "Advanced Coding Efficiency", 8, 1, 792,   384000,   0,    0    },
//  { 178, "Advanced Coding Efficiency", 8, 2, 1188,  2000000,  0,    0    },
//  { 179, "Advanced Coding Efficiency", 8, 3, 3240,  15000000, 0,    0    },
//  { 180, "Advanced Coding Efficiency", 8, 4, 16320, 38400000, 0,    0    },
//  { 193, "Advanced Core",              9, 1, 198,   384000,   0,    0    },
//  { 194, "Advanced Core",              9, 2, 792,   2000000,  0,    0    },
  { 240, "Advanced Simple",           10, 0, 99,    128000,   176,  144  },
  { 241, "Advanced Simple",           10, 1, 99,    128000,   176,  144  },
  { 242, "Advanced Simple",           10, 2, 396,   384000,   352,  288  },
  { 243, "Advanced Simple",           10, 3, 396,   768000,   352,  288  },
  { 244, "Advanced Simple",           10, 4, 792,   3000000,  352,  288  },
  { 245, "Advanced Simple",           10, 5, 1620,  8000000,  704,  576  },
  { 0 }
};

////////////////////////////////////////////////////////////////////////////////////////////////////

static struct h264_profile_level {
  const char *capname;
  unsigned level;
  unsigned level_h241;
  unsigned max_fs;
  unsigned max_mbps;
  unsigned max_br;
} const h264_profile_levels[] = {
  { "H.264-SQCIF",  10, 15,  99,    1485,   64000     },
  { "H.264-QCIF1",  11, 19,  99,    1485,   128000    },
  { "H.264-QCIF2",  11, 22,  396,   3000,   192000    },
  { "H.264-CIF1",   12, 29,  396,   6000,   384000    },
  { "H.264-CIF2",   13, 36,  396,   11880,  768000    },
  { "H.264-CIF3",   20, 43,  396,   11880,  2000000   },
  { "H.264-4CIF1",  21, 50,  792,   19800,  4000000   },
  { "H.264-4CIF2",  22, 57,  1620,  20250,  4000000   },
  { "H.264-4CIF3",  30, 64,  1620,  40500,  10000000  },
  { "H.264-SD",     31, 71,  3600,  108000, 14000000  },
  { "H.264-HD",     32, 78,  5120,  216000, 20000000  },
  { "H.264-FHD",    40, 85,  8192,  245760, 25000000  },
//  { NULL,           41, 92,  8192,  245760, 62500000  },
//  { NULL,           42, 99,  8704,  522340, 62500000  },
//  { NULL,           50, 106, 22080, 589824, 135000000 },
//  { NULL,           51, 113, 36864, 983040, 240000000 },
  { NULL }
};

////////////////////////////////////////////////////////////////////////////////////////////////////

const static struct h264_resolution {
    unsigned width;
    unsigned height;
    unsigned macroblocks;
} h264_resolutions[] = {
//  { 7680, 4320,129600}, // 16:9 Super Hi-Vision 33,17 mp
//  { 2560, 1440,14400 }, // 16:9 WQXGA            3,68 mp
//  { 2048, 1152, 9216 }, // 16:9 QWXGA            2,36 mp
//  { 1920, 1088, 8160 }, // ???                   2,08 mp
  { 1920, 1080, 8100 }, // 16:9 Full HD 1080P    2,07 mp
//  { 1600, 1200, 7500 }, //  4:3 UXGA             1,92 mp
  { 1408, 1152, 6336 }, // 11:9 CIF16            1,62 mp
//  { 1600,  900, 5625 }, // 16:9 WXGA++           1,44 mp
//  { 1280, 1024, 5120 }, //  5:4 SXGA             1,31 mp
//  { 1232, 1008, 4851 }, // 11:9                  1,24 mp
  { 1366,  768, 4098 }, // 16:9 WXGA             1,04 mp
  { 1280,  720, 3600 }, // 11:9 HD 720P          0,92 mp
//  { 1056,  864, 3564 }, // 11:9 TCIF             0,91 mp
//  { 1024,  768, 3072 }, //  4:3 XGA              0,78 mp
  {  960,  540, 2025 }, // 16:9 qHD              0,51 mp
//  {  800,  600, 1900 }, //  4:3 SVGA             0,48 mp
//  {  854,  480, 1601 }, // 16:9 FWVGA            0,40 mp
  {  704,  576, 1584 }, // 11:9 CIF4             0,40 mp
//  {  640,  480, 1200 }, //  4:3 VGA              0,30 mp
//  {  640,  360,  900 }, // 16:9 nHD              0,23 mp
  {  352,  288,  396 }, // 11:9 CIF              0,10 mp
//  {  320,  240,  300 }, //  4:3 QVGA             0,07 mp
  {  176,  144,   99 }, // 11:9 QCIF             0,02 mp
  {  128,   96,   48 }, //  4:3                  0,01 mp
  { 0 }
};

////////////////////////////////////////////////////////////////////////////////////////////////////

inline PString AVErrorToString(int errnum)
{
  char errbuf[64];
  av_strerror(errnum, errbuf, 64);
  return PString(errbuf);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

inline PString AVCodecGetName(AVCodecID id)
{
  PString name = "id:" + PString(id);
#if LIBAVCODEC_VERSION_INT >= AV_VERSION_INT(54,26,1)
  const AVCodecDescriptor *cd = avcodec_descriptor_get(id);
  if(cd)
    name += " name:" + PString(cd->name);
#endif
  return name;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

inline AVFrame * AVFrameAlloc()
{
#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(55,28,0)
  return avcodec_alloc_frame();
#else
  return av_frame_alloc();
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////

inline void AVFrameFree(AVFrame **frame)
{
#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(54,28,0)
    av_freep(frame);
#elif LIBAVCODEC_VERSION_INT < AV_VERSION_INT(55,28,0)
    avcodec_free_frame(frame);
#else
    av_frame_free(frame);
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////

inline int AlignUp2(int size)
{
  return ((size + 1) & ~1);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

static inline int ABS(int v)
{  return (v >= 0) ? v : -v; }

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // _MCU_UTILS_H
