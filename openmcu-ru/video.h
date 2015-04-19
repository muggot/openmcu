
#include "precompile.h"

#ifndef _MCU_VIDEO_H
#define _MCU_VIDEO_H

#include "utils.h"

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


#define _IMGST 1
#define _IMGST1 2
#define _IMGST2 4

#define WSF_VMP_SET_TIME  1
#define WSF_VMP_SUBTITLES 2
#define WSF_VMP_BORDER    4
#define WSF_VMP_FORCE_CUT 8
#define WSF_VMP_COMMON    WSF_VMP_SET_TIME | WSF_VMP_SUBTITLES | WSF_VMP_BORDER

#define MAX_SUBFRAMES        100
#define FRAMESTORE_TIMEOUT   60 /* s */

////////////////////////////////////////////////////////////////////////////////////////////////////

#if USE_FREETYPE
  struct MCUSubtitles
  {
    unsigned x, y, w, h;
    void* b;
  };
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////

class VideoFrameStoreList {
  public:
    class FrameStore {
      public:
        FrameStore(int _w, int _h)
          : width(_w), height(_h)
        { PAssert(_w != 0 && _h != 0, "Cannot create zero size framestore"); }

      int width;
      int height;
      time_t lastRead;
    };
    typedef MCUSharedList<FrameStore> MCUFrameStoreList;
    MCUFrameStoreList frameStoreList;
    typedef MCUFrameStoreList::shared_iterator shared_iterator;

    inline unsigned WidthHeightToKey(int width, int height)
    { return width << 16 | height; }

    inline void KeyToWidthHeight(unsigned key, int & width, int & height)
    { width = (key >> 16) & 0xffff; height = (key & 0xffff); }

    ~VideoFrameStoreList();
    shared_iterator GetFrameStore(int width, int height);
};

////////////////////////////////////////////////////////////////////////////////////////////////////

#define VMPC_CONFIGURATION_NAME                 "layouts.conf"
#define VMPC_DEFAULT_ID                         "undefined"
#define VMPC_DEFAULT_FW                         704
#define VMPC_DEFAULT_FH                         576
#define VMPC_DEFAULT_POSX                       0
#define VMPC_DEFAULT_POSY                       0
#define VMPC_DEFAULT_WIDTH                      VMPC_DEFAULT_FW/2
#define VMPC_DEFAULT_HEIGHT                     VMPC_DEFAULT_FH/2
#define VMPC_DEFAULT_MODE_MASK                  0
#define VMPC_DEFAULT_BORDER                     1
#define VMPC_DEFAULT_VIDNUM                     0
#define VMPC_DEFALUT_SCALE_MODE                 1
#define VMPC_DEFAULT_REALLOCATE_ON_DISCONNECT   1
#define VMPC_DEFAULT_NEW_FROM_BEGIN             1
#define VMPC_DEFAULT_MOCKUP_WIDTH               388
#define VMPC_DEFAULT_MOCKUP_HEIGHT              218

#ifdef USE_FREETYPE
#  define VMPC_DEFAULT_LABEL_MASK               89
//#  define VMPC_DEFAULT_LABEL_COLOR              0x0ffffff
#  define VMPC_DEFAULT_LABEL_BGCOLOR            0xA85D //RGB 0x115599
#  define VMPC_DEFAULT_FONTFILE                 "Russo_One.ttf"
#  define VMPC_DEFAULT_FONTSIZE                 "1/16"
#  define VMPC_DEFAULT_BORDER_LEFT              "5/80"
#  define VMPC_DEFAULT_BORDER_RIGHT             "5/80"
#  define VMPC_DEFAULT_BORDER_TOP               "1/200"
#  define VMPC_DEFAULT_BORDER_BOTTOM            "1/100"
#  define VMPC_DEFAULT_H_PAD                    "1/16"
#  define VMPC_DEFAULT_V_PAD                    "1/24"
#  define VMPC_DEFAULT_SHADOW_L                 "1/200"
#  define VMPC_DEFAULT_SHADOW_R                 "1/80"
#  define VMPC_DEFAULT_SHADOW_T                 "1/150"
#  define VMPC_DEFAULT_SHADOW_B                 "1/65"
#  define VMPC_DEFAULT_CUT_BEFORE_BRACKET       1
#  define VMPC_DEFAULT_MINIMUM_WIDTH_FOR_LABEL  "1/5"
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////

struct VMPBlock {
 unsigned posx,posy,width,height;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

struct VMPCfgOptions {
 unsigned posx, posy, width, height, border, label_mask, /* label_color, */ label_bgcolor, scale_mode, blks;
 bool cut_before_bracket;
 char
   fontsize[10],
   border_left[10], border_right[10], border_top[10], border_bottom[10],
   h_pad[10], v_pad[10],
   dropshadow_l[10], dropshadow_r[10], dropshadow_t[10], dropshadow_b[10];
 VMPBlock *blk;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

struct VMPCfgSplitOptions
{
  unsigned vidnum, mode_mask, reallocate_on_disconnect, new_from_begin, mockup_width, mockup_height;
  char Id[32], minimum_width_for_label[10];
};

////////////////////////////////////////////////////////////////////////////////////////////////////

struct VMPCfgLayout {
  VMPCfgSplitOptions splitcfg;
  VMPCfgOptions *vmpcfg;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class VideoMixConfigurator {
  public:
    VideoMixConfigurator(long _w = CIF4_WIDTH, long _h = CIF4_HEIGHT);
    ~VideoMixConfigurator();
    VMPCfgLayout *vmconf; // *configuration*
    unsigned vmconfs; // count of vmconf
    char fontfile[256];
    unsigned bfw,bfh; // base frame values for "resize" frames
    virtual void go(unsigned frame_width, unsigned frame_height);
  protected:
    long lid; // layout number
    long pos_n; // positions found for current lid
    unsigned char ldm; // layout descriptor mode flag (1/0)
    bool geometry_changed;
    VMPCfgOptions opts[2]; // local & global VMP options
    VMPCfgSplitOptions sopts[2]; // local & global layout options
    unsigned fw[2],fh[2]; // local & global base frame values
    virtual void parser(char* &f_buff,long f_size);
    virtual void block_insert(VMPBlock * & b,long b_n,unsigned x,unsigned y,unsigned w,unsigned h);
    virtual void block_erase(VMPBlock * & b,long b_i,long b_n);
    virtual unsigned frame_splitter(VMPBlock * & b,long b_i,long b_n,unsigned x0,unsigned y0,unsigned w0,unsigned h0,unsigned x1,unsigned y1,unsigned w1,unsigned h1);
    virtual void geometry();
    virtual void handle_line(char* &f_buff,long pos,long pos1,long line);
    virtual void handle_atom(char* &f_buff,long pos,long pos1,long line,long lo);
    virtual void handle_layout_descriptor(char* &f_buff,long pos,long pos1,long line,long lo);
    virtual void handle_position_descriptor(char* &f_buff,long pos,long pos1,long line,long lo);
    virtual void handle_parameter(char* &f_buff,long pos,long pos1,long line,long lo);
    virtual void option_set(const char* p, const char* v, char* &f_buff, long line, long lo, long pos, long pos1);
    virtual bool option_cmp(const char* p,const char* str);
    virtual void warning(char* &f_buff,long line,long lo,const char warn[64],long pos,long pos1);
    virtual void initialize_layout_desc(char* &f_buff,long pos,long pos1,long line,long lo);
    virtual void finalize_layout_desc();
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class VideoMixPosition {
  public:
    VideoMixPosition(ConferenceMemberId _id, int _x = 0, int _y = 0, int _w = 0, int _h = 0);
    virtual ~VideoMixPosition();
    ConferenceMemberId id;
    int n;
    int xpos, ypos, width, height;
    int silenceCounter; // static | vad visibility
    volatile int type; // static, vad, vad2, vad3
    int chosenVan; // always visible vad members (can switched between vad and vad2)
    PString endpointName;
#if USE_FREETYPE
    typedef MCUSharedList<MCUSubtitles> MCUSubtitlesList;
    MCUSubtitlesList subtitlesList; // one per framestore
    unsigned minWidthForLabel;
#endif
    BOOL border;
    time_t lastWrite;
    int rule;
    BOOL offline;

    MCUSharedList<MCUBufferArray> bufferList;
    int vmpbuf_index;
    PMutex bufferListMutex;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class MCUVideoMixer
{
  public:
    MCUVideoMixer()
      : subImageWidth(0), subImageHeight(0)
    {
      conference = NULL;
      jpegTime=0; jpegSize=0;
    }

    virtual ~MCUVideoMixer()
    { VMPListClear(); }

    virtual void Unlock();

    virtual long GetID()
    { return listID; }

    virtual void SetID(long id)
    { listID = id; }

    virtual void SetConference(Conference * _conference)
    { conference = _conference; }

    void VMPListInit()
    { }

    MCUVMPList::shared_iterator InsertVMP(VideoMixPosition *vmp)
    {
      if(vmpList.GetSize() == MAX_SUBFRAMES)
      {
        PTRACE(1, "VMP insert " << vmp->id << ", maximum subframes exceeded " << MAX_SUBFRAMES);
        return vmpList.end();
      }
      if(vmpList.GetSize() == vmpList.GetMaxSize())
      {
        PTRACE(1, "VMP insert " << vmp->id << ", maximum list size exceeded " << vmpList.GetMaxSize());
        return vmpList.end();
      }
      if(vmpList.Find(vmp) != vmpList.end())
      {
        PTRACE(1, "VMP insert " << vmp << ", already in list");
        return vmpList.end();
      }
      if(FindVMP(vmp->id) != vmpList.end())
      {
        PTRACE(1, "VMP insert " << vmp->id << ", duplicate key error: " << vmp->id);
        return vmpList.end();
      }
      return vmpList.Insert(vmp, vmp->id);
    }

    PString VMPListScanJS() // returns associative javascript "{0:id1, 1:-1, 2:id2, ...}"
    {
      PStringStream r, q;
      r << "{";
      q=r;
      for(MCUVMPList::shared_iterator it = vmpList.begin(); it != vmpList.end(); ++it)
      {
        VideoMixPosition *vmp = *it;
        r << vmp->n << ":" << vmp->id;
        q << vmp->n << ":" << vmp->type;
        if(vmp!=NULL) { r << ","; q << ","; }
      }
      r << "}," << q << "}";
      return r;
    }

    void VMPListDelVMP(VideoMixPosition *vmp)
    {
      MCUVMPList::shared_iterator it = vmpList.Find(vmp->id);
      if(it != vmpList.end())
        vmpList.Erase(it);
    }

    void VMPListClear()
    {
      for(MCUVMPList::shared_iterator it = vmpList.begin(); it != vmpList.end(); ++it)
      {
        VideoMixPosition *vmp = *it;
        if(vmpList.Erase(it))
          delete vmp;
      }
    }

    MCUVMPList::shared_iterator FindVMP(ConferenceMemberId id)
    {
      for(MCUVMPList::shared_iterator it = vmpList.begin(); it != vmpList.end(); ++it)
      {
        if(it->id == id)
          return it;
      }
      return vmpList.end();
    }

    MCUVMPList::shared_iterator FindVMP(int pos)
    {
      for(MCUVMPList::shared_iterator it = vmpList.begin(); it != vmpList.end(); ++it)
      {
        if(it->n == pos)
          return it;
      }
      return vmpList.end();
    }

    int rows;
    int cols;
    int subImageWidth;
    int subImageHeight;

    virtual MCUVideoMixer * Clone() const = 0;
    virtual BOOL ReadFrame(ConferenceMember & mbr, void * buffer, int width, int height, PINDEX & amount) = 0;
    virtual BOOL WriteFrame(ConferenceMemberId id, const void * buffer, int width, int height) = 0;
    virtual BOOL WriteSubFrame(VideoMixPosition & vmp, const void * buffer, int width, int height, int options) = 0;

    virtual PString GetFrameStoreMonitorList() = 0;

    virtual void Shuffle() = 0;
    virtual void Scroll(BOOL reverse) = 0;
    virtual void Revert() = 0;
    virtual BOOL AddVideoSource(ConferenceMemberId id, ConferenceMember & mbr) = 0;
    virtual BOOL AddVideoSourceToLayout(ConferenceMemberId id, ConferenceMember & mbr) = 0;
    virtual void RemoveVideoSource(ConferenceMemberId id, ConferenceMember & mbr) = 0;
    virtual void MyChangeLayout(unsigned newLayout) = 0;
    virtual void PositionSetup(int pos, int type, ConferenceMember * member) =0;
//    virtual BOOL MyAddVideoSource(int num, ConferenceMemberId *idp) = 0;

    virtual void MyRemoveVideoSource(int pos, BOOL flag) = 0;
    virtual void MyRemoveVideoSourceById(ConferenceMemberId id, BOOL flag) = 0;
    virtual void MyRemoveAllVideoSource() = 0;
    virtual int GetPositionSet() = 0;
    virtual int GetPositionNum(ConferenceMemberId id) = 0;
    virtual int GetSilenceCounter(ConferenceMemberId id) = 0;
    virtual void ResetSilenceCounter(ConferenceMemberId id) = 0;
    virtual void IncreaseSilenceCounter(ConferenceMemberId, int) = 0;
    virtual int GetPositionType(ConferenceMemberId id) = 0;
    virtual void SetPositionType(int pos, int type) = 0;
    virtual ConferenceMemberId GetPositionId(int pos) = 0;
    virtual void Exchange(int pos1, int pos2) = 0;
    virtual BOOL TryOnVADPosition(ConferenceMember * member) = 0;
    virtual BOOL SetVADPosition(ConferenceMember * member, int chosenVan, unsigned short timeout) = 0;
    virtual BOOL SetVAD2Position(ConferenceMember * member) = 0;

    virtual VideoMixPosition * CreateVideoMixPosition(ConferenceMemberId _id, 
                                                         int _x, 
                                                         int _y,
                                                         int _w, 
                                                         int _h)
    { return new VideoMixPosition(_id, _x, _y, _w, _h); }

    static void ConvertRGBToYUV(BYTE R, BYTE G, BYTE B, BYTE & Y, BYTE & U, BYTE & V);
    static void FillYUVFrame(void * buffer, BYTE R, BYTE G, BYTE B, int w, int h);
    static void FillYUVFrame_YUV(void * buffer, BYTE Y, BYTE U, BYTE V, int w, int h);
    static void FillCIFYUVFrame(void * buffer, BYTE R, BYTE G, BYTE B);
    static void FillQCIFYUVFrame(void * buffer, BYTE R, BYTE G, BYTE B);
    static void FillCIF4YUVFrame(void * buffer, BYTE R, BYTE G, BYTE B);
    static void FillCIF16YUVFrame(void * buffer, BYTE R, BYTE G, BYTE B);
    static void FillCIFYUVRect(void * frame, BYTE R, BYTE G, BYTE B, int xPos, int yPos, int rectWidth, int rectHeight);
    static void FillCIF4YUVRect(void * frame, BYTE R, BYTE G, BYTE B, int xPos, int yPos, int rectWidth, int rectHeight);
    static void FillCIF16YUVRect(void * frame, BYTE R, BYTE G, BYTE B, int xPos, int yPos, int rectWidth, int rectHeight);
    static void FillYUVRect(void * frame, int frameWidth, int frameHeight, BYTE R, BYTE G, BYTE B, int xPos, int yPos, int rectWidth, int rectHeight);
    static void ReplaceUV_Rect(void * frame, int frameWidth, int frameHeight, BYTE U, BYTE V, int xPos, int yPos, int rectWidth, int rectHeight);
    static void CopyRectIntoQCIF(const void * _src, void * _dst, int xpos, int ypos, int width, int height);
    static void CopyRectIntoCIF(const void * _src, void * _dst, int xpos, int ypos, int width, int height);
    static void CopyRectIntoCIF4(const void * _src, void * _dst, int xpos, int ypos, int width, int height);
    static void CopyGrayscaleIntoCIF(const void * _src, void * _dst, int xpos, int ypos, int width, int height);
    static void CopyGrayscaleIntoCIF4(const void * _src, void * _dst, int xpos, int ypos, int width, int height);
    static void CopyGrayscaleIntoCIF16(const void * _src, void * _dst, int xpos, int ypos, int width, int height);
    static void CopyGrayscaleIntoFrame(const void * _src, void * _dst, int xpos, int ypos, int width, int height, int fw, int fh);
    static void CopyRectIntoCIF16(const void * _src, void * _dst, int xpos, int ypos, int width, int height);
    static void CopyRFromRIntoR(const void *_s, void * _d, int xp, int yp, int w, int h, int rx_abs, int ry_abs, int rw, int rh, int fw, int fh, int lim_w, int lim_h);
    static void CopyRectIntoFrame(const void * _src, void * _dst, int xpos, int ypos, int width, int height, int fw, int fh);
    static void MixRectIntoFrameGrayscale(const void * _src, void * _dst, int xpos, int ypos, int width, int height, int fw, int fh, BYTE wide);
#if USE_FREETYPE
    static void MixRectIntoFrameSubsMode(const void * _src, void * _dst, int xpos, int ypos, int width, int height, int fw, int fh, BYTE wide);
#endif
    static void CopyRectIntoRect(const void * _src, void * _dst, int xpos, int ypos, int width, int height, int fw, int fh);
    static void CopyRectFromFrame(const void * _src, void * _dst, int xpos, int ypos, int width, int height, int fw, int fh);
    static void ResizeYUV420P(const void * _src, void * _dst, unsigned int sw, unsigned int sh, unsigned int dw, unsigned int dh);
//#if !USE_LIBYUV && !USE_SWSCALE
    static void ConvertQCIFToCIF(const void * _src, void * _dst);
    static void ConvertCIFToCIF4(const void * _src, void * _dst);
    static void ConvertCIF4ToCIF16(const void * _src, void * _dst);
//    static void ConvertFRAMEToFRAME4(const void * _src, void * _dst, unsigned w, unsigned h);
    static void ConvertFRAMEToCUSTOM_FRAME(const void * _src, void * _dst, unsigned int sw, unsigned int sh, unsigned int dw, unsigned int dh);
    static void ConvertCIFToTQCIF(const void * _src, void * _dst);
    static void ConvertCIF4ToTCIF(const void * _src, void * _dst);
    static void ConvertCIF16ToTCIF(const void * _src, void * _dst);
    static void ConvertCIF4ToTQCIF(const void * _src, void * _dst);
    static void ConvertCIFToTSQCIF(const void * _src, void * _dst);
    static void ConvertQCIFToCIF4(const void * _src, void * _dst);
    static void ConvertCIF4ToCIF(const void * _src, void * _dst);
    static void ConvertCIF16ToCIF4(const void * _src, void * _dst);
    static void ConvertCIFToQCIF(const void * _src, void * _dst);
    static void Convert2To1(const void * _src, void * _dst, unsigned int w, unsigned int h);
    static void Convert1To2(const void * _src, void * _dst, unsigned int w, unsigned int h);
//    static void ConvertCIFToQCIF3(const void * _src, void * _dst);
    static void ConvertCIFToQ3CIF(const void * _src, void * _dst);
    static void ConvertCIF4ToQ3CIF4(const void * _src, void * _dst);
    static void ConvertCIF16ToQ3CIF16(const void * _src, void * _dst);
    static void ConvertCIFToSQ3CIF(const void*, void*);
    static void ConvertCIF4ToQ3CIF(const void*, void*);
    static void ConvertCIF16ToQ3CIF4(const void*, void*);
    static void ConvertCIF16ToCIF(const void * _src, void * _dst);
    static void ConvertCIF4ToQCIF(const void * _src, void * _dst);
    static void ConvertCIFToSQCIF(const void * _src, void * _dst);
//#endif

    PBYTEArray myjpeg;
    PINDEX jpegSize;
    unsigned long jpegTime;

    MCUVMPList vmpList;
    PMutex vmpListMutex;

    static void VideoSplitLines(BYTE *dst, unsigned fw, unsigned fh);
    static void LeftSplitLine(BYTE *dst, unsigned px, unsigned py, unsigned pw, unsigned ph, unsigned width, unsigned height);
    static void RightSplitLine(BYTE *dst, unsigned px, unsigned py, unsigned pw, unsigned ph, unsigned width, unsigned height);
    static void TopSplitLine(BYTE *dst, unsigned px, unsigned py, unsigned pw, unsigned ph, unsigned width, unsigned height);
    static void BottomSplitLine(BYTE *dst, unsigned px, unsigned py, unsigned pw, unsigned ph, unsigned width, unsigned height);

    virtual void SetForceScreenSplit(BOOL newForceScreenSplit){ forceScreenSplit=newForceScreenSplit; }

  protected:
    Conference * conference;
    long listID;

    BOOL forceScreenSplit;
};


////////////////////////////////////////////////////////////////////////////////////////////////////

class MCUSimpleVideoMixer : public MCUVideoMixer
{
  public:
    MCUSimpleVideoMixer(BOOL forceScreenSplit = FALSE);
    virtual MCUVideoMixer * Clone() const
    { return new MCUSimpleVideoMixer(*this); }

    ~MCUSimpleVideoMixer()
    { }

    virtual BOOL ReadFrame(ConferenceMember &, void * buffer, int width, int height, PINDEX & amount);
    virtual BOOL WriteFrame(ConferenceMemberId id, const void * buffer, int width, int height);
    virtual BOOL SetOffline(ConferenceMemberId id);
    virtual BOOL SetOnline(ConferenceMemberId id);

#if USE_FREETYPE
    virtual unsigned printsubs_calc(unsigned v, char s[10]);
    virtual void DeleteSubtitlesByFS(unsigned w, unsigned h);
    virtual void PrintSubtitles(VideoMixPosition & vmp, void * buffer, unsigned fw, unsigned fh, unsigned ft_properties);
    virtual void RemoveSubtitles(VideoMixPosition & vmp);
    virtual void InitializeSubtitles();
    virtual MCUSubtitles * RenderSubtitles(unsigned key, VideoMixPosition & vmp, void * buffer, unsigned fw, unsigned fh, unsigned ft_properties);
#endif
    virtual BOOL WriteSubFrame(VideoMixPosition & vmp, const void * buffer, int width, int height, int options);

    virtual void Shuffle();
    virtual void Scroll(BOOL reverse);
    virtual void Revert();
    virtual BOOL AddVideoSource(ConferenceMemberId id, ConferenceMember & mbr);
    virtual BOOL AddVideoSourceToLayout(ConferenceMemberId id, ConferenceMember & mbr);
    virtual void RemoveVideoSource(ConferenceMemberId id, ConferenceMember & mbr);
    virtual void MyChangeLayout(unsigned newLayout);
    virtual void PositionSetup(int pos, int type, ConferenceMember * member);
//    virtual BOOL MyAddVideoSource(int num, ConferenceMemberId *idp);

    virtual void MyRemoveVideoSource(int pos, BOOL flag);
    virtual void MyRemoveVideoSourceById(ConferenceMemberId id, BOOL flag);
    virtual void MyRemoveAllVideoSource();
    virtual int GetPositionSet();
    virtual int GetPositionNum(ConferenceMemberId id);
    virtual int GetSilenceCounter(ConferenceMemberId id);
    virtual void ResetSilenceCounter(ConferenceMemberId id);
    virtual void IncreaseSilenceCounter(ConferenceMemberId, int);
    virtual int GetPositionType(ConferenceMemberId id);
    virtual void SetPositionType(int pos, int type);
    virtual ConferenceMemberId GetPositionId(int pos);
    virtual void Exchange(int pos1, int pos2);
    virtual BOOL TryOnVADPosition(ConferenceMember * member);
    virtual BOOL SetVADPosition(ConferenceMember * member, int chosenVan, unsigned short timeout);
    virtual BOOL SetVAD2Position(ConferenceMember * member);
    inline void CheckOperationalSize(long w, long h, BYTE mask);
    virtual BOOL ReadMixedFrame(void * buffer, int width, int height, PINDEX & amount);
    virtual PString GetFrameStoreMonitorList();
    virtual int GetMostAppropriateLayout(unsigned n);
    virtual void WriteEmptyFrame(VideoMixPosition&);
    virtual void WriteOfflineFrame(VideoMixPosition&);
    virtual void WriteNoVideoFrame(VideoMixPosition&);

    virtual void VMPDelete(MCUVMPList::shared_iterator &it);
    virtual void VMPSetConfig(VideoMixPosition *vmp);
    virtual void VMPCopy(VideoMixPosition&, VideoMixPosition &);
    virtual void VMPTouch(VideoMixPosition&);
    virtual void VMPSwapAndTouch(VideoMixPosition&, VideoMixPosition &);

    virtual int VMPListFindEmptyIndex();
    virtual MCUVMPList::shared_iterator VMPCreator(ConferenceMember * m, int n, int type);
    virtual BOOL VMPExists(ConferenceMemberId);

  protected:
    virtual void ReallocatePositions();
    BOOL ReadMixedFrame(VideoFrameStoreList & srcFrameStores, void * buffer, int width, int height, PINDEX & amount);

    VideoFrameStoreList frameStores;  // list of framestores for data

    PBYTEArray imageStore;        // temporary conversion store
    PBYTEArray imageStore1;        // temporary conversion store
    PBYTEArray imageStore2;        // temporary conversion store
    long imageStore_size, imageStore1_size, imageStore2_size;
    int specialLayout;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

#if ENABLE_TEST_ROOMS
class TestVideoMixer : public MCUSimpleVideoMixer
{
  public:
    TestVideoMixer(unsigned frames);
    BOOL AddVideoSource(ConferenceMemberId id, ConferenceMember & mbr);
    BOOL WriteFrame(ConferenceMemberId id, const void * buffer, int width, int height);
    void RemoveVideoSource(ConferenceMemberId id, ConferenceMember & mbr);
    virtual void MyChangeLayout(unsigned newLayout);
    virtual void Shuffle() {};
    virtual void Scroll(BOOL reverse) {};
    virtual void Revert() {};
    virtual BOOL AddVideoSourceToLayout(ConferenceMemberId id, ConferenceMember & mbr) { return FALSE; };
    virtual void PositionSetup(int pos, int type, ConferenceMember * member) {};
//    virtual BOOL MyAddVideoSource(int num, ConferenceMemberId *idp) { return FALSE; };
    virtual void MyRemoveVideoSource(int pos, BOOL flag) {};
    virtual void MyRemoveVideoSourceById(ConferenceMemberId id, BOOL flag) {};
    virtual void MyRemoveAllVideoSource() {};
    virtual void SetPositionType(int pos, int type) {};
    virtual void Exchange(int pos1, int pos2) {};
    virtual BOOL SetVADPosition(ConferenceMember * member, int chosenVan, unsigned short timeout) { return 0; };
    virtual BOOL SetVAD2Position(ConferenceMember * member) { return FALSE; };

  protected:
    unsigned frames;
    BOOL allocated;
};
#endif // ENABLE_TEST_ROOMS

////////////////////////////////////////////////////////////////////////////////////////////////////

#if ENABLE_ECHO_MIXER
class EchoVideoMixer : public MCUSimpleVideoMixer
{
  public:
    EchoVideoMixer();
    BOOL AddVideoSource(ConferenceMemberId id, ConferenceMember & mbr);
    BOOL WriteFrame(ConferenceMemberId id, const void * buffer, int width, int height);
};
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////

#endif  // _MCU_VIDEO_H

