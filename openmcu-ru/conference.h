
#ifndef _MCU_CONFERENCE_H
#define _MCU_CONFERENCE_H

#ifdef _WIN32
#pragma warning(disable:4786)
#pragma warning(disable:4100)
#endif

#include <ptlib/sound.h>
#include <ptlib/video.h>
#include <ptlib/vconvert.h>
#include <ptclib/delaychan.h>

#include <set>
#include <map>

#include "config.h"
#include "util.h"

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

#if USE_LIBYUV
#include <libyuv/scale.h>
#endif

typedef void * ConferenceMemberId;

class ConferenceMember;
class ConferenceRecorder;
class Conference;
class ConferenceManager;

////////////////////////////////////////////////////////////////////////////////////////////////////

enum MemberTypes
{
  MEMBER_TYPE_NONE       = 0,
  MEMBER_TYPE_MCU        = 1,
  MEMBER_TYPE_PIPE       = 2,
  MEMBER_TYPE_CACHE      = 3,
  MEMBER_TYPE_RECORDER   = 4,
  MEMBER_TYPE_STREAM     = 5,
  MEMBER_TYPE_GSYSTEM    = (MEMBER_TYPE_PIPE|MEMBER_TYPE_CACHE|MEMBER_TYPE_RECORDER|MEMBER_TYPE_STREAM)
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class MCULock : public PObject
{
  PCLASSINFO(MCULock, PObject);
  public:
    MCULock();
    BOOL Wait(BOOL hard = FALSE);
    void Signal(BOOL hard = FALSE);
    void WaitForClose();
  protected:
    PMutex mutex;
    BOOL closing;
    int count;
    PSyncPoint closeSync;
};

#if MCU_VIDEO

////////////////////////////////////////////////////

#if USE_FREETYPE
  struct MCUSubtitles
  {
    unsigned x, y, w, h;
    void* b;
  };
  typedef std::map<unsigned, MCUSubtitles*> MCUSubtitlesMapType;
#endif

class VideoFrameStoreList {
  public:
    class FrameStore {
      public:
        FrameStore(int _w, int _h)
          : valid(FALSE), width(_w), height(_h)
        { valid = FALSE; PAssert(_w != 0 && _h != 0, "Cannot create zero size framestore"); data.SetSize(_w * _h * 3 / 2); }

      BOOL valid;
      int width;
      int height;
      time_t lastRead;
      PBYTEArray data;
    };

    inline unsigned WidthHeightToKey(int width, int height)
    { return width << 16 | height; }

    inline void KeyToWidthHeight(unsigned key, int & width, int & height)
    { width = (key >> 16) & 0xffff; height = (key & 0xffff); }

    ~VideoFrameStoreList();
    FrameStore & AddFrameStore(int width, int height);
    FrameStore & GetFrameStore(int width, int height);
    FrameStore & GetNearestFrameStore(int width, int height, BOOL & found);
    void InvalidateExcept(int w, int h);

    typedef std::map<unsigned, FrameStore *> VideoFrameStoreListMapType;
    VideoFrameStoreListMapType videoFrameStoreList;
};

////////////////////////////////////////////////////

/// Video Mixer Configurator - Begin ///
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

struct VMPBlock {
 unsigned posx,posy,width,height;
};

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

struct VMPCfgSplitOptions { unsigned vidnum, mode_mask, reallocate_on_disconnect, new_from_begin, mockup_width, mockup_height; char Id[32], minimum_width_for_label[10]; };

struct VMPCfgLayout {
  VMPCfgSplitOptions splitcfg;
  VMPCfgOptions *vmpcfg;
};

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
/// Video Mixer Configurator - End ///

class MCUVideoMixer
{
  public:
    class VideoMixPosition {
      public:
        VideoMixPosition(ConferenceMemberId _id, int _x = 0, int _y = 0, int _w = 0, int _h = 0);
        virtual ~VideoMixPosition();
        VideoMixPosition *next;
        VideoMixPosition *prev;
        ConferenceMemberId id;
	int n;
        int xpos, ypos, width, height;
        int status; // static | vad visibility
        volatile int type; // static, vad, vad2, vad3
        int chosenVan; // always visible vad members (can switched between vad and vad2)
	PString endpointName;
#if USE_FREETYPE
	MCUSubtitlesMapType subtitlesList; // one per framestore
        unsigned minWidthForLabel;
#endif
        BOOL border;
    };

    virtual ~MCUVideoMixer()
    { VMPListClear(); }

    virtual void Unlock();

    virtual long GetID()
    { return id; }

    virtual void SetID(long newid)
    { id = newid; }

    virtual void SetConference(Conference * _conference)
    { conference = _conference; }

    VideoMixPosition *vmpList;
    unsigned vmpNum;

    void VMPListInit() 
    { 
     vmpList = new VideoMixPosition(0); 
     vmpNum = 0; 
    }

    void VMPListInsVMP(VideoMixPosition *pVMP)
    {
      VideoMixPosition *vmpListMember = vmpList->next;
      while(vmpListMember!=NULL)
      {
        if(vmpListMember == pVMP) 
        {
          PTRACE(3, "VMP " << pVMP->n << " already in list"); 
          return;
        }
       vmpListMember = vmpListMember->next;
     }

     vmpListMember = vmpList->next;
     while(vmpListMember!=NULL)
     {
       if(vmpListMember->id == pVMP->id) 
       {
         PTRACE(1, "VMP " << pVMP->n << " duplicate key error: " << pVMP->id); 
         return;
       }
       vmpListMember = vmpListMember->next;
     }

     pVMP->prev = vmpList; pVMP->next = vmpList->next;
     if(vmpList->next != NULL) pVMP->next->prev = pVMP;
     vmpList->next = pVMP;
     vmpNum++;
    }

    PString VMPListScanJS() // returns associative javascript "{0:id1, 1:-1, 2:id2, ...}"
    {
      PStringStream r, q;
      r << "{";
      q=r;
      VideoMixPosition *vmp = vmpList->next;
      while(vmp!=NULL)
      {
        r << vmp->n << ":" << (long)(vmp->id);
        q << vmp->n << ":" << vmp->type;
        vmp = vmp->next;
        if(vmp!=NULL) { r << ","; q << ","; }
      }
      r << "}," << q << "}";
      return r;
    }

    void VMPListAddVMP(VideoMixPosition *pVMP)
    {
      VideoMixPosition *vmpListMember;
      VideoMixPosition *vmpListLastMember=NULL;
      vmpListMember = vmpList->next;
      while(vmpListMember!=NULL)
      {
        if(vmpListMember == pVMP)
        {
          PTRACE(3, "VMP " << pVMP->n << " already in list"); 
          return;
        }
        vmpListMember = vmpListMember->next;
      }
      vmpListMember = vmpList->next;
      while(vmpListMember!=NULL)
      {
        if(vmpListMember->id == pVMP->id)
        {
          PTRACE(1, "VMP " << pVMP->n << " duplicate key error: " << pVMP->id); 
          return;
        }
        vmpListLastMember=vmpListMember;
        vmpListMember = vmpListMember->next; 
      }
      if(vmpListLastMember!=NULL)
      {
        vmpListLastMember->next=pVMP;
        pVMP->prev = vmpListLastMember;
      }
      if(vmpList->next == NULL)
      {
        vmpList->next=pVMP;
        pVMP->prev = vmpList;
      }
      pVMP->next = NULL;
      vmpNum++;
    }

    void VMPListDelVMP(VideoMixPosition *pVMP)
    {
     if(pVMP->next!=NULL) pVMP->next->prev=pVMP->prev;
     pVMP->prev->next=pVMP->next;
     vmpNum--;
    }

    void VMPListClear()
    {
     VideoMixPosition *vmpListMember;
     while(vmpList->next!=NULL)
     {
      vmpListMember = vmpList->next;
      vmpList->next=vmpListMember->next;
      delete vmpListMember;
     }
     vmpNum = 0;
    }

    VideoMixPosition * VMPListFindVMP(ConferenceMemberId id)
    {
     VideoMixPosition *vmpListMember = vmpList->next;
     while(vmpListMember!=NULL)
     {
      if(vmpListMember->id == id) return vmpListMember;
      vmpListMember = vmpListMember->next;
     }
     return NULL;
    }

    VideoMixPosition * VMPListFindVMP(int pos)
    {
     VideoMixPosition *vmpListMember = vmpList->next;
     while(vmpListMember!=NULL)
     {
      if(vmpListMember->n == pos) return vmpListMember;
      vmpListMember = vmpListMember->next;
     }
     return NULL;
    }

    int rows;
    int cols;
    int subImageWidth;
    int subImageHeight;

    MCUVideoMixer()
      : subImageWidth(0), subImageHeight(0)
    {
#if USE_LIBJPEG
      jpegTime=0; jpegSize=0;
#endif
    }

    virtual MCUVideoMixer * Clone() const = 0;
    virtual BOOL ReadFrame(ConferenceMember & mbr, void * buffer, int width, int height, PINDEX & amount) = 0;
    virtual BOOL WriteFrame(ConferenceMemberId id, const void * buffer, int width, int height, PINDEX amount) = 0;

    virtual BOOL WriteSubFrame(VideoMixPosition & vmp, const void * buffer, int width, int height, PINDEX amount) = 0;
    virtual PString GetFrameStoreMonitorList() = 0;
//    virtual void WriteCIFSubFrame(VideoMixPosition & vmp, const void * buffer, PINDEX amount) = 0;
//    virtual void WriteCIF4SubFrame(VideoMixPosition & vmp, const void * buffer, PINDEX amount) = 0;
//    virtual void WriteCIF16SubFrame(VideoMixPosition & vmp, const void * buffer, PINDEX amount) = 0;
    virtual void NullRectangle(int x, int y, int w, int h, BOOL border) = 0;
    virtual void NullAllFrameStores() = 0;

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
    virtual int GetPositionStatus(ConferenceMemberId id) = 0;
    virtual int GetPositionType(ConferenceMemberId id) = 0;
    virtual void SetPositionType(int pos, int type) = 0;
    virtual void InsertVideoSource(ConferenceMember * member, int pos) = 0;
    virtual void SetPositionStatus(ConferenceMemberId id,int newStatus) = 0;
    virtual ConferenceMemberId GetPositionId(int pos) = 0;
    virtual ConferenceMemberId GetHonestId(int pos) = 0;
    virtual void Exchange(int pos1, int pos2) = 0;
    virtual ConferenceMemberId SetVADPosition(ConferenceMember * member, int chosenVan, unsigned short timeout) = 0;
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
#if USE_LIBYUV==0
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
#endif
#if USE_LIBJPEG
    PBYTEArray myjpeg;
    PINDEX jpegSize;
    unsigned long jpegTime;
#endif
    static void VideoSplitLines(void * dst, unsigned fw, unsigned fh);
    virtual void SetForceScreenSplit(BOOL newForceScreenSplit){ forceScreenSplit=newForceScreenSplit; }

  protected:
    Conference * conference;
    long id;

    PMutex mutex;
    BOOL forceScreenSplit;
};

class MCUSimpleVideoMixer : public MCUVideoMixer
{
  public:
    MCUSimpleVideoMixer(BOOL forceScreenSplit = FALSE);
    virtual MCUVideoMixer * Clone() const
    { return new MCUSimpleVideoMixer(*this); }

    ~MCUSimpleVideoMixer()
    { }

    virtual BOOL ReadFrame(ConferenceMember &, void * buffer, int width, int height, PINDEX & amount);
    virtual BOOL WriteFrame(ConferenceMemberId id, const void * buffer, int width, int height, PINDEX amount);

#if USE_FREETYPE
    virtual unsigned printsubs_calc(unsigned v, char s[10]);
    virtual void DeleteSubtitlesByFS(unsigned w, unsigned h);
    virtual void PrintSubtitles(VideoMixPosition & vmp, void * buffer, unsigned fw, unsigned fh, unsigned ft_properties);
    virtual void RemoveSubtitles(VideoMixPosition & vmp);
    virtual void InitializeSubtitles();
    virtual MCUSubtitles * RenderSubtitles(unsigned key, VideoMixPosition & vmp, void * buffer, unsigned fw, unsigned fh, unsigned ft_properties);
#endif
    virtual BOOL WriteSubFrame(VideoMixPosition & vmp, const void * buffer, int width, int height, PINDEX amount);
/*
    virtual void WriteCIFSubFrame(VideoMixPosition & vmp, const void * buffer, PINDEX amount);
    virtual void WriteCIF4SubFrame(VideoMixPosition & vmp, const void * buffer, PINDEX amount);
    virtual void WriteCIF16SubFrame(VideoMixPosition & vmp, const void * buffer, PINDEX amount);
*/
    virtual void NullRectangle(int x, int y, int w, int h, BOOL border);
    virtual void NullAllFrameStores();

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
    virtual int GetPositionStatus(ConferenceMemberId id);
    virtual int GetPositionType(ConferenceMemberId id);
    virtual void SetPositionType(int pos, int type);
    virtual void InsertVideoSource(ConferenceMember * member, int pos);
    virtual void SetPositionStatus(ConferenceMemberId id,int newStatus);
    virtual ConferenceMemberId GetPositionId(int pos);
    virtual ConferenceMemberId GetHonestId(int pos);
    virtual void Exchange(int pos1, int pos2);
    virtual ConferenceMemberId SetVADPosition(ConferenceMember * member, int chosenVan, unsigned short timeout);
    virtual BOOL SetVAD2Position(ConferenceMember * member);
//    virtual void SetForceScreenSplit(BOOL newForceScreenSplit){ forceScreenSplit=newForceScreenSplit; }
    virtual void imageStores_operational_size(long w, long h, BYTE mask){
      long s=w*h*3/2;
      if(mask&_IMGST)if(s>imageStore_size){imageStore.SetSize(s);imageStore_size=s;}
      if(mask&_IMGST1)if(s>imageStore1_size){imageStore1.SetSize(s);imageStore1_size=s;}
      if(mask&_IMGST2)if(s>imageStore2_size){imageStore2.SetSize(s);imageStore2_size=s;}
     }
    virtual BOOL ReadMixedFrame(void * buffer, int width, int height, PINDEX & amount);
    virtual PString GetFrameStoreMonitorList();

  protected:
    virtual void CalcVideoSplitSize(unsigned int imageCount, int & subImageWidth, int & subImageHeight, int & cols, int & rows);
    virtual void ReallocatePositions();
    BOOL ReadSrcFrame(VideoFrameStoreList & srcFrameStores, void * buffer, int width, int height, PINDEX & amount);

    VideoFrameStoreList frameStores;  // list of framestores for data

    PBYTEArray imageStore;        // temporary conversion store
    PBYTEArray imageStore1;        // temporary conversion store
    PBYTEArray imageStore2;        // temporary conversion store
    long imageStore_size, imageStore1_size, imageStore2_size;
//    PColourConverter * converter; // CIF to QCIF converter
    int specialLayout;
};

#if ENABLE_TEST_ROOMS
class TestVideoMixer : public MCUSimpleVideoMixer
{
  public:
    TestVideoMixer(unsigned frames);
    BOOL AddVideoSource(ConferenceMemberId id, ConferenceMember & mbr);
    BOOL WriteFrame(ConferenceMemberId id, const void * buffer, int width, int height, PINDEX amount);
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
    virtual void InsertVideoSource(ConferenceMember * member, int pos) {};
    virtual void SetPositionStatus(ConferenceMemberId id,int newStatus) {};
    virtual void Exchange(int pos1, int pos2) {};
    virtual ConferenceMemberId SetVADPosition(ConferenceMember * member, int chosenVan, unsigned short timeout) { return NULL; };
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
    BOOL WriteFrame(ConferenceMemberId id, const void * buffer, int width, int height, PINDEX amount);
};
#endif

#endif  // MCU_VIDEO

////////////////////////////////////////////////////////////////////////////////////////////////////

class AudioResampler
{
  public:
    AudioResampler()
    {
#if USE_SWRESAMPLE || USE_AVRESAMPLE || USE_LIBSAMPLERATE
      swrc = NULL;
#endif
    }
    ~AudioResampler()
    {
#if USE_SWRESAMPLE
      if(swrc) swr_free(&swrc);
#elif USE_AVRESAMPLE
      if(swrc) avresample_free(&swrc);
#elif USE_LIBSAMPLERATE
      if(swrc) src_delete(swrc);
#endif
    }

    PTime readTime;
    PBYTEArray data;
    int srcSampleRate;
    int srcChannels;
    int dstSampleRate;
    int dstChannels;
#if USE_SWRESAMPLE
    struct SwrContext * swrc;
#elif USE_AVRESAMPLE
    struct AVAudioResampleContext * swrc;
#elif USE_LIBSAMPLERATE
    struct SRC_STATE_tag * swrc;
#endif
};
typedef std::map<long, AudioResampler *> AudioResamplerListType;

////////////////////////////////////////////////////////////////////////////////////////////////////

class AudioReader
{
  public:
    AudioReader() { }
    ~AudioReader() { }
    int timeIndex;
    PTime readTime;
    PBYTEArray buffer;
};
typedef std::map<ConferenceMemberId, AudioReader *> AudioReaderListType;

////////////////////////////////////////////////////////////////////////////////////////////////////

class ConferenceConnection : public PObject {
  PCLASSINFO(ConferenceConnection, PObject);
  public:
    ConferenceConnection(ConferenceMemberId _id)
      : id(_id)
    { }

    ~ConferenceConnection()
    { }

    ConferenceMemberId GetID() const
    { return id; }

#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable:4311)
#endif
    virtual PString GetName() const
#ifdef P_64BIT
    { return psprintf("%lli", id); }
#else
    { return PString(PString::Unsigned, (unsigned)id); }
#endif
#ifdef _WIN32
#pragma warning(pop)
#endif

    virtual void OnUserInputIndication(const PString &)
    { }

  protected:
    ConferenceMemberId id;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class ConferenceAudioConnection : public ConferenceConnection
{
  PCLASSINFO(ConferenceAudioConnection, ConferenceConnection);

  public:
    ConferenceAudioConnection(ConferenceMemberId _id, int _sampleRate = 8000, int _channels = 1);
    ~ConferenceAudioConnection();

    virtual void WriteAudio(const BYTE * data, int amount);
    virtual void ReadAudio(ConferenceMember *member, BYTE * data, int amount, int dstSampleRate, int dstChannels);

    int GetSampleRate() const
    { return sampleRate; }

    int GetChannels() const
    { return channels; }

    void Mix(const BYTE * src, BYTE * dst, int count);
    void Resample(BYTE * src, int srcBytes, int srcSampleRate, int srcChannels, AudioResampler *resampler, int dstBytes, int dstSampleRate, int dstChannels);
    AudioResampler * CreateResampler(int srcSampleRate, int srcChannels, int dstSampleRate, int dstChannels);

  protected:
    int sampleRate;
    int channels;
    int timeSize;           // 1ms size
    int timeIndex;          // current position ms
    int timeBufferSize;     // size ms
    int byteBufferSize;     // size bytes
    PBYTEArray buffer;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class ConferenceProfile : public PObject
{
  PCLASSINFO(ConferenceProfile, PObject);

  public:
    ConferenceProfile(Conference * _conference, PString _name);

    ~ConferenceProfile()
    { }

    PMutex & GetMutex()
    { return mutex; }

    void Lock()
    { mutex.Wait(); }

    void Unlock()
    { mutex.Signal(); }

    void SetName(PString _name);

    const PString & GetName() const
    { return name; }

    const PString & GetNameID() const
    { return nameID; }

    PString GetNameHTML() const
    {
      PString _name = name;
      _name.Replace("&","&amp;",TRUE,0);
      _name.Replace("\"","&quot;",TRUE,0);
      return _name;
    }

    ConferenceMemberId GetID() const
    { return (ConferenceMemberId)this; }

    ConferenceMember * GetMember()
    { return member; }

    void SetMember(ConferenceMember *_member);

  protected:
    PMutex mutex;

    PString name;
    PString nameID;

    Conference *conference;
    ConferenceMember *member;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class ConferenceMember : public PObject
{
  PCLASSINFO(ConferenceMember, PObject);
  public:

    /**
      * create a new conference member. The single parameter is an "id" (usually a pointer) 
      * that can used to identify this member unambiguously
      */
    ConferenceMember(Conference * conference, ConferenceMemberId id);

    /**
      * destroy the conference member
      */
    ~ConferenceMember();

    void ChannelBrowserStateUpdate(BYTE bitMask, BOOL bitState);

    PMutex & GetMutex()
    { return destructorMutex; }

    void Lock()
    { destructorMutex.Wait(); }

    void Unlock()
    { destructorMutex.Signal(); }

    /**
      * used to pre-emptively close a members connection
      */
    virtual void Close()
    { }

    /**
      * If this returns TRUE, the conference member will be visible in all publically displayed
      * conference lists. It will always be visible in the console displays
      */
    virtual BOOL IsVisible() const
    { return TRUE; }

    /**
      * return the conference member ID
      */
    ConferenceMemberId GetID() const
    { return id; }

    virtual PTime GetStartTime() const
    { return startTime; }
     
#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable:4311)
#endif

    /**
      * return the title that should be used for the conference member
      */
    virtual PString GetTitle() const
#ifdef P_64BIT
    { return psprintf("%lli", id); }
#else
    { return PString(PString::Unsigned, (unsigned)id); }
#endif

#ifdef _WIN32
#pragma warning(pop)
#endif

    /**
      * return the conference this member belongs to
      */
    Conference * GetConference()
    { return conference; }

    virtual void RemoveAllConnections();

    /**
     * This is called when the conference member want to send a user input indication to the the conference.
     * By default, this routines calls OnReceivedUserInputIndication for all of the other conference members
     */
    virtual void SendUserInputIndication(const PString & str) {}

    /**
     * this virtual function is called when the conference sends a user input indication to the endpoint
     * the conference
     */
    virtual void OnReceivedUserInputIndication(const PString & str)
    { }

    /**
      *  Called when the conference member want to send audio data to the cofnerence
      */
    virtual void WriteAudio(const void * buffer, PINDEX amount, unsigned sampleRate, unsigned channels);

    /**
      *  Called when the conference member wants to read a block of audio from the conference
      *  By default, this calls ReadMemberAudio on the conference
      */
    virtual void ReadAudio(void * buffer, PINDEX amount, unsigned sampleRate, unsigned channels);

    virtual void ReadAudioOutputGain(void * buffer, int amount);

#if MCU_VIDEO
    /**
      *  Called when the conference member wants to send video data to the conference
      */
    virtual void WriteVideo(const void * buffer, int width, int height, PINDEX amount);

    /**
      *  Called when a conference member wants to read a block of video from the conference
      *  By default, this calls ReadMemberVideo on the conference
      */
    virtual void ReadVideo(void * buffer, int width, int height, PINDEX & amount);

    /**
      * Called when another conference member wants to read video from the endpoint
      * UnlockExternalVideo must be called after video has been used
      */
    virtual void * OnExternalReadVideo(ConferenceMemberId /*id*/, int width, int height, PINDEX & /*amount*/);

    virtual void UnlockExternalVideo();

    /**
      * called when another conference member wants to write a video frame to this endpoint
      * this will only be called when the conference is not "use same video for all members"
      */
    virtual void OnExternalSendVideo(ConferenceMemberId id, const void * buffer, int width, int height, PINDEX amount);

    /**
      * called to when a new video source added
      */
    virtual BOOL AddVideoSource(ConferenceMemberId id, ConferenceMember & mbr);

    /**
      * called to when a new video source removed
      */
    virtual void RemoveVideoSource(ConferenceMemberId id, ConferenceMember & mbr);

    virtual BOOL OnIncomingVideo(const void * buffer, int width, int height, PINDEX amount);
    virtual BOOL OnOutgoingVideo(void * buffer, int width, int height, PINDEX & amount);

    double GetVideoTxFrameRate() const
    { 
      if (totalVideoFramesSent == 0) 
        return 0.0; 
      else 
        return totalVideoFramesSent * 1000.0 / ((PTime() - firstFrameSendTime).GetMilliSeconds()); }

    PString GetVideoRxFrameSize() const
    {
      PStringStream res;
      res << rxFrameWidth << "x" << rxFrameHeight;
      return res;
    }

    double GetVideoRxFrameRate() const
    { 
      if (totalVideoFramesReceived == 0)
        return 0.0; 
      else 
        return totalVideoFramesReceived * 1000.0 / ((PTime() - firstFrameReceiveTime).GetMilliSeconds()); 
    }

    unsigned GetVideoMixerNumber()
    {
      return videoMixerNumber;
    }
    void SetVideoMixerNumber(unsigned newVideoMixerNumber)
    {
      videoMixerNumber=newVideoMixerNumber;
    }
#endif

    /*
     *  Used to create a conference connection for this member
     */
    virtual ConferenceConnection * CreateConnection() = 0;

    void WaitForClose()
    { lock.WaitForClose(); }

    /*
     * used to output monitor information for the member
     */
    virtual PString GetMonitorInfo(const PString & hdr);

    virtual int GetTerminalNumber() const             { return terminalNumber; }
    virtual void SetTerminalNumber(int n)             { terminalNumber = n; }

    void SetJoined(BOOL isJoinedNow)
    { memberIsJoined = isJoinedNow; }

    BOOL IsJoined() const
    { return memberIsJoined; }

    virtual void SetName() {}
    virtual void SetName(PString newName) {}

    virtual PString GetName() const
    { return name; }

    virtual PString GetNameID() const
    { return nameID; }

    PString GetNameHTML() const
    {
      PString _name = name;
      _name.Replace("&","&amp;",TRUE,0);
      _name.Replace("\"","&quot;",TRUE,0);
      return _name;
    }

    virtual PString GetCallToken() const
    { return callToken; }

    virtual MemberTypes GetType()
    { return memberType; }

    virtual void SetFreezeVideo(BOOL) const
    { }

    virtual unsigned GetAudioLevel() const
    { return audioLevel;  }

    AudioResamplerListType & GetAudioResamplerList()
    { return audioResamplerList; }

    AudioReaderListType & GetAudioReaderList()
    { return audioReaderList; }

    void ClearAudioReaderList(BOOL force = FALSE);

    BOOL autoDial;
    unsigned muteMask;
    BOOL disableVAD;
    BOOL chosenVan; // allways visible, but can change place on frame, used in 5+1 layout
    int vad;
    unsigned long audioCounter;
    unsigned audioLevelIndicator;
    unsigned previousAudioLevel;
    BYTE channelCheck;

#if MCU_VIDEO
    MCUVideoMixer * videoMixer;
#endif

    float kManualGain, kOutputGain;
    int kManualGainDB, kOutputGainDB;

    // functions H323Connection_ConferenceMember
    virtual void SetChannelPauses(unsigned mask) {};
    virtual void UnsetChannelPauses(unsigned mask) {};


  protected:
    unsigned videoMixerNumber;
    Conference * conference;
    ConferenceMemberId id;
    BOOL memberIsJoined;
    MCULock lock;
    PTime startTime;
    unsigned audioLevel;
    int terminalNumber;
    PString callToken;
    MemberTypes memberType;
    PString name;
    PString nameID;
    float currVolCoef;

    PMutex destructorMutex;

    AudioReaderListType audioReaderList;
    AudioResamplerListType audioResamplerList;

#if MCU_VIDEO
    //PMutex videoMutex;

    VideoFrameStoreList memberFrameStores;
    PMutex memberFrameStoreMutex;
//    PColourConverter * fsConverter; 

    PTime firstFrameSendTime;
    PINDEX totalVideoFramesSent;

    PTime firstFrameReceiveTime;
    PINDEX totalVideoFramesReceived;
    
    int rxFrameWidth, rxFrameHeight;
#endif
};


////////////////////////////////////////////////////

template <class KeyType>
class MCUNumberMapType : public std::map<int, KeyType>
{
  public:
    typedef std::map<int, KeyType> Ancestor;
    int GetNumber(const KeyType & id)
    {
      PWaitAndSignal m(mutex);
      int mcuNumber = 1;
      if (Ancestor::size() != 0) {
        mcuNumber = 1 + Ancestor::begin()->first;
        while (Ancestor::find(mcuNumber) != Ancestor::end())
          ++mcuNumber;
      }
      Ancestor::insert(std::pair<int, KeyType>(mcuNumber, id));
      return mcuNumber;
    }

    void RemoveNumber(int mcuNumber)
    { PWaitAndSignal m(mutex); Ancestor::erase(mcuNumber); }

  protected:
    PMutex mutex;
};

////////////////////////////////////////////////////

class ConferenceMonitorInfo;

/**
  * this class describes a conference or "room"
  */

class Conference : public PObject
{
  PCLASSINFO(Conference, PObject);
  public:

    typedef std::map<void *, ConferenceMember *> MemberList;
    typedef std::map<ConferenceMemberId, ConferenceProfile *> ProfileList;

    Conference(ConferenceManager & manager,
                                  long _id,
        const OpalGloballyUniqueID & _guid, // H.323 m_conferenceID ???
                   const PString & _number,
                     const PString & _name,
                             int _mcuNumber
#if MCU_VIDEO
                  ,MCUSimpleVideoMixer * _videoMixer = NULL
#endif
                   );

    ~Conference();

    PMutex & GetMemberListMutex()
    { return memberListMutex; }

    PMutex & GetProfileListMutex()
    { return profileListMutex; }

    void Unlock();

    ConferenceManager & GetManager()
    { return manager; }

    void RefreshAddressBook();

    /**
      * add the specified member to the conference
      */
    BOOL AddMember(ConferenceMember * member);

    /**
     * remove the specifed member from the conference.
     * Note that this function does not actually delete the conference member
     * as sometimes a conference member needs to remove itself from a conference
     * 
     * @return if TRUE, the conference is now empty
     */
    BOOL RemoveMember(ConferenceMember * member);

    MemberList & GetMemberList()
    { return memberList; }

    ProfileList & GetProfileList()
    { return profileList; }

    void AddMemberToList(const PString & name, ConferenceMember *member);
    void RemoveMemberFromList(const PString & name, ConferenceMember *member);

    int GetMemberCount() const
    { PWaitAndSignal m(memberListMutex); return (int)memberList.size(); }

    int GetVisibleMemberCount() const
    { return visibleMemberCount; }

    virtual PString GetName() const
    { return name; }

    virtual PString GetNumber() const
    { return number; }

    OpalGloballyUniqueID GetGUID() const
    { return guid; }

    long GetID() const
    { return id; }

    virtual BOOL IsVisible() const
    { return TRUE; }

    virtual BOOL IsMuteUnvisible() const
    { return muteUnvisible; }

    virtual void SetMuteUnvisible(BOOL set)
    { muteUnvisible = set; }

    virtual PString IsModerated() const
    {
      if(forceScreenSplit == FALSE)
        return "0";
     PString yes="+";
     PString no="-";
     if(!moderated) return no; else return yes;
    }

    virtual void SetModerated(BOOL set)
    { moderated = set; }

    PTime GetStartTime() const
    { return startTime; }

    PINDEX GetMaxMemberCount() const
    { return maxMemberCount; }

    int GetMCUNumber() const
    { return mcuNumber; }

    MCUVideoMixerList & GetVideoMixerList()
    { return videoMixerList; }

    virtual BOOL BeforeMemberJoining(ConferenceMember *);

    virtual void OnMemberJoining(ConferenceMember *);

    virtual void OnMemberLeaving(ConferenceMember *);

    virtual void ReadMemberAudio(ConferenceMember * member, void * buffer, int amount, int sampleRate, int channels);

    virtual void WriteMemberAudio(ConferenceMember * member, const void * buffer, int amount, int sampleRate, int channels);

    virtual void WriteMemberAudioLevel(ConferenceMember * member, int audioLevel, int tint);

#if MCU_VIDEO
    virtual void ReadMemberVideo(ConferenceMember * member, void * buffer, int width, int height, PINDEX & amount);

    virtual BOOL WriteMemberVideo(ConferenceMember * member, const void * buffer, int width, int height, PINDEX amount);

    virtual BOOL UseSameVideoForAllMembers()
    { return videoMixerList.GetCurrentSize() > 0; }

    virtual void FreezeVideo(ConferenceMemberId id);
    virtual BOOL PutChosenVan();
#endif

    void AddMonitorEvent(ConferenceMonitorInfo * info);

    void HandleFeatureAccessCode(ConferenceMember & member, PString fac);

    unsigned short int VAdelay;
    unsigned short int VAtimeout;
    unsigned short int VAlevel;
    unsigned short int echoLevel;

    PString membersConf;
    PString confTpl;
    virtual PString SaveTemplate(PString tplName);
    virtual void LoadTemplate(PString tpl);
    virtual PString GetTemplateList();
    virtual PString GetSelectedTemplateName();
    virtual PString ExtractTemplate(PString tplName);
    virtual void PullMemberOptionsFromTemplate(ConferenceMember * member, PString tpl);
    virtual void TemplateInsertAndRewrite(PString tplName, PString tpl);
    virtual void SetLastUsedTemplate(PString tplName);
    virtual void DeleteTemplate(PString tplName);
    virtual BOOL RewriteMembersConf();
    virtual void OnConnectionClean(const PString & remotePartyName, const PString & remotePartyAddress);

    ConferenceRecorder * conferenceRecorder;

    void SetForceScreenSplit(BOOL _forceScreenSplit) { forceScreenSplit = _forceScreenSplit; }
    BOOL GetForceScreenSplit() { return forceScreenSplit; }

    BOOL RecorderCheckSpace();
    BOOL StartRecorder();
    BOOL StopRecorder();

    BOOL stopping;
    BOOL lockedTemplate;

    ConferenceMember * pipeMember;

  protected:
    ConferenceManager & manager;

    PMutex memberListMutex;
    PMutex profileListMutex;

    MemberList memberList;
    ProfileList profileList;

    ConferenceAudioConnection * AddAudioConnection(ConferenceMember * member, unsigned sampleRate = 8000, unsigned channels = 1);
    void RemoveAudioConnection(ConferenceMember * member);
    MCUAudioConnectionList audioConnectionList;

    MCUVideoMixerList videoMixerList;

    PINDEX visibleMemberCount;
    PINDEX maxMemberCount;

    long id;
    OpalGloballyUniqueID guid;

    PString number;
    PString name;
    int mcuNumber;
    PTime startTime;
    MCUNumberMapType<ConferenceMemberId> terminalNumberMap;
    BOOL mcuMonitorRunning;
    BOOL moderated;
    BOOL muteUnvisible;
    int vidmembernum;
    PMutex membersConfMutex;
    BOOL forceScreenSplit;
};

////////////////////////////////////////////////////

class ConferenceMonitorInfo : public PObject
{
  PCLASSINFO(ConferenceMonitorInfo, PObject);
  public:
    ConferenceMonitorInfo(const long & _id, const PTime & endTime)
      : id(_id), timeToPerform(endTime) { }

    long id;
    PTime timeToPerform;

    virtual int Perform(Conference &) = 0;
};

class ConferenceTimeLimitInfo : public ConferenceMonitorInfo
{
  public:
    ConferenceTimeLimitInfo(const long & _id, const PTime & endTime)
      : ConferenceMonitorInfo(_id, endTime)
    { }

    int Perform(Conference & conference);
};

class ConferenceRepeatingInfo : public ConferenceMonitorInfo
{
  public:
    ConferenceRepeatingInfo(const long & _id, const PTimeInterval & _repeatTime)
      : ConferenceMonitorInfo(_id, PTime() + _repeatTime), repeatTime(_repeatTime)
    { }

    int Perform(Conference & conference);

  protected:
    PTimeInterval repeatTime;
};

class ConferenceStatusInfo : public ConferenceRepeatingInfo
{
  public:
    ConferenceStatusInfo(const long & _id)
      : ConferenceRepeatingInfo(_id, 1000)
    { }

    int Perform(Conference & conference);
};

class ConferenceRecorderInfo : public ConferenceRepeatingInfo
{
  public:
    ConferenceRecorderInfo(const long & _id)
      : ConferenceRepeatingInfo(_id, 1000)
    { }

    int Perform(Conference & conference);
};

class ConferenceMCUCheckInfo : public ConferenceRepeatingInfo
{
  public:
    ConferenceMCUCheckInfo(const long & _id, const PTimeInterval & _repeatTime)
      : ConferenceRepeatingInfo(_id, _repeatTime)
    { }

    int Perform(Conference & conference);
};

class ConferenceMonitor : public PThread
{
  PCLASSINFO(ConferenceMonitor, PThread);
  public:
    ConferenceMonitor(ConferenceManager & _manager)
      : PThread(10000, NoAutoDeleteThread), manager(_manager)
    { Resume(); }

    void Main();
    void AddMonitorEvent(ConferenceMonitorInfo * info);
    void RemoveForConference(const long & _id);

    typedef std::vector<ConferenceMonitorInfo *> MonitorInfoList;
    BOOL running;

  protected:
    ConferenceManager & manager;
    PMutex mutex;
    MonitorInfoList monitorList;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class MemberDeleteThread : public PThread
{
  public:
    MemberDeleteThread(Conference * _conference, ConferenceMember * _member)
      : PThread(10000, AutoDeleteThread), conference(_conference), member(_member)
    {
      Resume();
    }

    void Main()
    {
      if(member)
      {
        member->WaitForClose();
        if(conference)
          conference->RemoveMember(member);
        delete member;
      }
    }

  protected:
    Conference * conference;
    ConferenceMember * member;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class ConferenceManager : public PObject
{
  PCLASSINFO(ConferenceManager, PObject);
  public:
    ConferenceManager();
    ~ConferenceManager();

    Conference * FindConferenceWithLock(Conference * c);
    Conference * FindConferenceWithLock(const OpalGloballyUniqueID & conferenceID);
    Conference * FindConferenceWithLock(long id);
    Conference * FindConferenceWithLock(const PString & room);

    Conference * MakeConferenceWithLock(const PString & number, PString name = "");

    ConferenceProfile * FindProfileWithLock(const PString & roomName, const PString & memberName);
    ConferenceProfile * FindProfileWithLock(Conference * conference, const PString & memberName);
    ConferenceProfile * FindProfileWithoutLock(Conference * conference, const PString & memberName);

    ConferenceMember * FindMemberWithLock(const PString & roomName, const PString & memberName);
    ConferenceMember * FindMemberWithLock(Conference * conference, const PString & memberName);
    ConferenceMember * FindMemberWithoutLock(Conference * conference, const PString & memberName);

    ConferenceMember * FindMemberNameIDWithLock(const PString & roomName, const PString & memberName);
    ConferenceMember * FindMemberNameIDWithLock(Conference * conference, const PString & memberName);
    ConferenceMember * FindMemberNameIDWithoutLock(Conference * conference, const PString & memberName);

    ConferenceMember * FindMemberWithLock(const PString & roomName, long id);
    ConferenceMember * FindMemberWithLock(Conference * conference, long id);
    ConferenceMember * FindMemberWithoutLock(Conference * conference, long id);

    MCUSimpleVideoMixer * FindVideoMixerWithLock(const PString & room, int number);
    MCUSimpleVideoMixer * FindVideoMixerWithLock(Conference * conference, int number);
    MCUSimpleVideoMixer * GetVideoMixerWithLock(const PString & room);
    MCUSimpleVideoMixer * GetVideoMixerWithLock(Conference * conference);
    int AddVideoMixer(Conference * conference);
    int DeleteVideoMixer(Conference * conference, int number);

    /**
      * return true if a conference with the specified ID exists
      */
    BOOL HasConference(const OpalGloballyUniqueID & conferenceID, PString & room);
    BOOL HasConference(const OpalGloballyUniqueID & conferenceID)
    { PString r; return HasConference(conferenceID, r); }

    /**
      * return true if a conference with the specified number exists
      */
    BOOL HasConference(const PString & number, OpalGloballyUniqueID & conferenceID);
    BOOL HasConference(const PString & number)
    { OpalGloballyUniqueID i; return HasConference(number, i); }

    //
    void RemoveConference(const PString & room);

    MCUConferenceList & GetConferenceList()
    { return conferenceList; }

    virtual void OnCreateConference(Conference *);

    virtual void OnDestroyConference(Conference *);

    virtual BOOL BeforeMemberJoining(Conference *, ConferenceMember *)
    { return TRUE; }

    virtual void OnMemberJoining(Conference *, ConferenceMember *)
    { }

    virtual void OnMemberLeaving(Conference *, ConferenceMember *)
    { }

    PINDEX GetMaxConferenceCount() const
    { return maxConferenceCount; }

    void AddMonitorEvent(ConferenceMonitorInfo * info);
    void ClearMonitorEvents();
    void ClearConferenceList();

  protected:
    virtual Conference * CreateConference(long _id, const OpalGloballyUniqueID & _guid, const PString & _number, const PString & _name, int mcuNumber);

    MCUConferenceList conferenceList;

    PINDEX maxConferenceCount;
    MCUNumberMapType<OpalGloballyUniqueID> mcuNumberMap;
    ConferenceMonitor * monitor;
};

#endif  // _MCU_CONFERENCE_H

