
#include "precompile.h"

#if MCU_VIDEO

#include "mcu.h"

#if USE_FREETYPE
FT_Library ft_library;
FT_Face ft_face;
PMutex ft_mutex;
FT_Bool ft_use_kerning;
FT_UInt ft_glyph_index,ft_previous;
BOOL ft_subtitles=FALSE;
// Fake error code mean that we need initialize labels from scratch:
#define FT_INITIAL_ERROR 555
#define FT_FINAL_ERROR 5555
// Number of frames skipped before render label:
#define FT_SKIPFRAMES    1
// Number of getting username attempts:
#define FT_ATTEMPTS      100
// Label's options:
#define FT_P_H_CENTER    0x0001
#define FT_P_V_CENTER    0x0002
#define FT_P_RIGHT       0x0004
#define FT_P_BOTTOM      0x0008
#define FT_P_TRANSPARENT 0x0010
#define FT_P_DISABLED    0x0020
#define FT_P_SUBTITLES   0x0040
int ft_error=FT_INITIAL_ERROR;
#endif // #if USE_FREETYPE

///////////////////////////////////////////////////////////////////////////////////////
//
//  declare a video capture (input) device for use with MCU
//

MCUPVideoInputDevice::MCUPVideoInputDevice(MCUH323Connection & _mcuConnection)
  : mcuConnection(_mcuConnection)
{
  SetColourFormat("YUV420P");
  channelNumber = 0; 
  grabCount = 0;
  SetFrameRate(25);
}


BOOL MCUPVideoInputDevice::Open(const PString & devName, BOOL /*startImmediate*/)
{
  //file.SetWidth(frameWidth);
  //file.SetHeight(frameHeight);

  deviceName = devName;

  return TRUE;    
}


BOOL MCUPVideoInputDevice::IsOpen() 
{
  return TRUE;
}


BOOL MCUPVideoInputDevice::Close()
{
  return TRUE;
}


BOOL MCUPVideoInputDevice::Start()
{
  return TRUE;
}


BOOL MCUPVideoInputDevice::Stop()
{
  return TRUE;
}

BOOL MCUPVideoInputDevice::IsCapturing()
{
  return IsOpen();
}


PStringList MCUPVideoInputDevice::GetInputDeviceNames()
{
  PStringList list;
  list.AppendString("MCU");
  return list;
}


BOOL MCUPVideoInputDevice::SetVideoFormat(VideoFormat newFormat)
{
  return PVideoDevice::SetVideoFormat(newFormat);
}


int MCUPVideoInputDevice::GetNumChannels() 
{
  return 0;
}


BOOL MCUPVideoInputDevice::SetChannel(int newChannel)
{
  return PVideoDevice::SetChannel(newChannel);
}

BOOL MCUPVideoInputDevice::SetColourFormat(const PString & newFormat)
{
  if (!(newFormat *= "YUV420P"))
    return FALSE;

  if (!PVideoDevice::SetColourFormat(newFormat))
    return FALSE;

  return SetFrameSize(frameWidth, frameHeight);
}


BOOL MCUPVideoInputDevice::SetFrameRate(unsigned rate)
{
  if (rate < 1)
    rate = 1;
  else if (rate > 999)
    rate = 999;

  return PVideoDevice::SetFrameRate(rate);
}


BOOL MCUPVideoInputDevice::GetFrameSizeLimits(unsigned & minWidth,
                                           unsigned & minHeight,
                                           unsigned & maxWidth,
                                           unsigned & maxHeight) 
{
  maxWidth  = 2048;
  maxHeight = 2048;
  minWidth  = QCIF_WIDTH;
  minHeight = QCIF_HEIGHT;

  return TRUE;
}


BOOL MCUPVideoInputDevice::SetFrameSize(unsigned width, unsigned height)
{
  PTRACE(4,"SetFrameSize " << width << " " << height);
  if (!PVideoDevice::SetFrameSize(width, height))
    return FALSE;

  videoFrameSize = CalculateFrameBytes(frameWidth, frameHeight, colourFormat);
  scanLineWidth = videoFrameSize/frameHeight;
  return videoFrameSize > 0;
}


PINDEX MCUPVideoInputDevice::GetMaxFrameBytes()
{
  return GetMaxFrameBytesConverted(videoFrameSize);
}


BOOL MCUPVideoInputDevice::GetFrameData(BYTE * buffer, PINDEX * bytesReturned)
{    
  grabDelay.Delay(1000/GetFrameRate());
  return GetFrameDataNoDelay(buffer, bytesReturned);
}

 
BOOL MCUPVideoInputDevice::GetFrameDataNoDelay(BYTE *destFrame, PINDEX * bytesReturned)
{
  grabCount++;

  if (!mcuConnection.OnOutgoingVideo(destFrame, frameWidth, frameHeight, *bytesReturned))
    return FALSE;

  if (converter != NULL) {
    if (!converter->Convert(destFrame, destFrame, bytesReturned))
      return FALSE;
  }

  if (bytesReturned != NULL)
    *bytesReturned = videoFrameSize;

  return TRUE;
}

///////////////////////////////////////////////////////////////////////////////////////
//
//  declare a video display (output) device for use with MCU
//

MCUPVideoOutputDevice::MCUPVideoOutputDevice(MCUH323Connection & _mcuConnection)
  : mcuConnection(_mcuConnection)
{
}


BOOL MCUPVideoOutputDevice::Open(const PString & _deviceName, BOOL /*startImmediate*/)
{
  deviceName = _deviceName;
  return TRUE;
}

BOOL MCUPVideoOutputDevice::Close()
{
  return TRUE;
}

BOOL MCUPVideoOutputDevice::Start()
{
  return TRUE;
}

BOOL MCUPVideoOutputDevice::Stop()
{
  return TRUE;
}

BOOL MCUPVideoOutputDevice::IsOpen()
{
  return TRUE;
}


#if PTLIB_MAJOR == 2 && PTLIB_MINOR > 0
PStringArray MCUPVideoOutputDevice::GetOutputDeviceNames()
{
  PStringArray list;
#else
PStringList MCUPVideoOutputDevice::GetOutputDeviceNames()
{
  PStringList list;
#endif
  return list;
}


PINDEX MCUPVideoOutputDevice::GetMaxFrameBytes()
{
  return GetMaxFrameBytesConverted(CalculateFrameBytes(frameWidth, frameHeight, colourFormat));
}


BOOL MCUPVideoOutputDevice::SetFrameData(unsigned x, unsigned y,
                                              unsigned width, unsigned height,
                                              const BYTE * data,
                                              BOOL /*endFrame*/)
{
  if (x != 0 || y != 0 || width != frameWidth || height != frameHeight)
  {
    PTRACE(1, "OpenMCU only supports full frame writes");
    return FALSE;
  }

  return mcuConnection.OnIncomingVideo(data, width, height);
}


BOOL MCUPVideoOutputDevice::EndFrame()
{
  return TRUE;
}


///////////////////////////////////////////////////////////////////////////////////////

VideoFrameStoreList::~VideoFrameStoreList()
{
  for(shared_iterator it = frameStoreList.begin(); it != frameStoreList.end(); ++it)
  {
    VideoFrameStore *vf = *it;
    if(frameStoreList.Erase(it))
      delete vf;
  }
}

VideoFrameStoreList::shared_iterator VideoFrameStoreList::GetFrameStore(int width, int height) 
{
  shared_iterator it = frameStoreList.Find(WidthHeightToKey(width, height));
  if(it == frameStoreList.end())
  {
    PWaitAndSignal m(frameStoreListMutex);
    it = frameStoreList.Find(WidthHeightToKey(width, height));
    if(it == frameStoreList.end())
    {
      VideoFrameStore * vf = new VideoFrameStore(width, height);
      it = frameStoreList.Insert(vf, WidthHeightToKey(width, height));
    }
  }
  return it;
}

VideoFrameStore::VideoFrameStore(int _w, int _h)
  : width(_w), height(_h), frame_size(_w*_h*3/2)
{
  PAssert(_w != 0 && _h != 0, "Cannot create zero size framestore");
  lastRead = time(NULL);

  bg_frame.SetSize(frame_size);
  unsigned bgw, bgh;
  void *bg = OpenMCU::Current().GetBackgroundPointer(bgw, bgh);
  if(bg)
    ResizeYUV420P(bg, bg_frame.GetPointer(), bgw, bgh, width, height);
  else
    FillYUVFrame(bg_frame.GetPointer(), 0, 0, 0, width, height);

  logo_frame.SetSize(frame_size);
  unsigned logo_width, logo_height;
  void *logo = OpenMCU::Current().GetLogoFramePointer(logo_width, logo_height);
  if(logo)
    ResizeYUV420P(logo, logo_frame.GetPointer(), logo_width, logo_height, width, height);
  else
    FillYUVFrame(logo_frame.GetPointer(), 0, 0, 0, width, height);
}

///////////////////////////////////////////////////////////////////////////////////////

void MCUVideoMixer::Unlock()
{
  if(conference)
  {
    MCUVideoMixerList & videoMixerList = conference->GetVideoMixerList();
    videoMixerList.Release(listID);
  }
}


VideoMixPosition::VideoMixPosition(ConferenceMemberId _id)
  : id(_id)
{
  silenceCounter = 0;
  type = 0;
  chosenVan = 0;
  rule = 1;
  offline = FALSE; //dont show offline banner for 1st time
  lastWrite = 0;
  vmpbuf_index = -1;
  shows_logo = FALSE;
}

VideoMixPosition::~VideoMixPosition()
{
  PTRACE(5,"VideoMixer\tVMP " << n << " destructor: " << endpointName);
#if USE_FREETYPE
  MCURemoveSubtitles(*this);
#endif
  for(MCUBufferYUVArrayList::shared_iterator it = bufferList.begin(); it != bufferList.end(); ++it)
  {
    MCUBufferYUVArray *buffer = *it;
    if(bufferList.Erase(it))
      delete buffer;
  }
}

#if USE_FREETYPE
unsigned MCUSubsCalc(const unsigned v, const PString s)
{
  if(s.Find("/")==P_MAX_INDEX) return s.AsInteger();
  PStringArray fraction=s.Tokenise("/");
  if(fraction.GetSize() != 2)
  {
    PTRACE(1, "FreeType\tMCUSubsCals bad value: " << s);
    return 1;
  }
  unsigned divisor = fraction[1].AsInteger();
  if(divisor==0)
  {
    PTRACE(1, "FreeType\tMCUSubsCals division by zero (" << s << "): " << v << "*" << fraction[0] << "/" << fraction[1]);
    return 1;
  }
  return v * fraction[0].AsInteger() / divisor;
}

void MCURemoveSubtitles(VideoMixPosition & vmp)
{
  for(MCUSubtitlesList::shared_iterator it = vmp.subtitlesList.begin(); it != vmp.subtitlesList.end(); ++it)
  {
    MCUSubtitles *sub = *it;
    if(vmp.subtitlesList.Erase(it))
    {
      if(sub->b) free(sub->b);
      delete sub;
    }
  }
}

void MCUPrintSubtitles(VideoMixPosition & vmp, void * buffer, unsigned int fw, unsigned int fh, unsigned int ft_properties, unsigned layout)
{
  unsigned key=(fh << 16) | fw;
  MCUSubtitlesList::shared_iterator q = vmp.subtitlesList.Find(key);
  if(q == vmp.subtitlesList.end())
    return;
  MCUSubtitles * st = *q;

  if(st == NULL) return;

  if(!(st->w)) return;

  unsigned bgcolor = OpenMCU::vmcfg.vmconf[layout].vmpcfg[vmp.n].label_bgcolor;

  if(ft_properties & FT_P_SUBTITLES) MixRectIntoFrameSubsMode(st->b,(BYTE *)buffer,st->x,st->y,st->w,st->h,fw,fh,0);

  if(ft_properties & FT_P_TRANSPARENT) MixRectIntoFrameGrayscale(st->b,(BYTE *)buffer,st->x,st->y,st->w,st->h,fw,fh,1);

  if(ft_properties & FT_P_TRANSPARENT)
  {
//    MixRectIntoFrameGrayscale(st->b.GetPointer(),(BYTE *)buffer,st->x,st->y,st->w,st->h,fw,fh,1);
    ReplaceUV_Rect((BYTE *)buffer,fw,fh,bgcolor>>8,bgcolor,0,st->y,fw,st->h);
  }

//  if(ft_properties & FT_P_SUBTITLES) MixRectIntoFrameSubsMode(st->b.GetPointer(),(BYTE *)buffer,st->x,st->y,st->w,st->h,fw,fh,0);

  if(!(ft_properties & (FT_P_SUBTITLES + FT_P_TRANSPARENT))) CopyRectIntoFrame(st->b,(BYTE *)buffer,st->x,st->y,st->w,st->h,fw,fh);

}

void InitializeSubtitles()
{
  PWaitAndSignal m(ft_mutex);
  if(ft_error != FT_INITIAL_ERROR) return;

  PTRACE(3,"FreeType\tInitialization");
  if ((ft_error = FT_Init_FreeType(&ft_library))) return;
# ifdef TRUETYPE_FONT_DIR
    ft_error = FT_New_Face(ft_library,PString(TRUETYPE_FONT_DIR)+PATH_SEPARATOR+OpenMCU::vmcfg.fontfile,0,&ft_face);
# else
    ft_error = FT_New_Face(ft_library,OpenMCU::vmcfg.fontfile,0,&ft_face);
# endif
  if (!ft_error)
  {
    ft_use_kerning=FT_HAS_KERNING(ft_face);
    PTRACE(3,"FreeType\tTruetype font " << OpenMCU::vmcfg.fontfile << " loaded with" << (ft_use_kerning?"":"out") << " kerning");
  }
  PTRACE_IF(1,ft_error,"FreeType\tCould not load truetype font: " << OpenMCU::vmcfg.fontfile);
}

void SubtitlesDropShadow(void * s, unsigned w, unsigned h, unsigned l, unsigned t, unsigned r, unsigned b)
{
  int x, y;
  for (y=0; y<(int)h; y++)
  {
    long ob=y*w;
    for(x=0; x<(int)w; x++)
    {
      BYTE c=*((BYTE*)s+ob+x);
      if(c>50)
      {
        int x0, y0;
        for (y0=(int)(y-t); y0<=(int)(y+b); y0++)
        {
          if((y0<0)||(y0>=(int)h)) continue;
          long ob0=y0*w;
          for(x0=x-l; x0<=(int)(x+r); x0++)
          {
            if((x0<0)||(x0>=(int)w)) continue;
            BYTE c0=*((BYTE*)s+ob0+x0);
            if(c0==0) *((BYTE*)s+ob0+x0)=1;
          }
        }
      }
    }
  }
}

MCUSubtitles * MCURenderSubtitles(VideoMixPosition & vmp, unsigned fw, unsigned fh, unsigned ft_properties, unsigned layout)
{
  MCUSubtitles * st = new MCUSubtitles;
  st->w = 0;
  st->b = NULL;
  st->text = vmp.GetEndpointName();

  VMPCfgSplitOptions & split = OpenMCU::vmcfg.vmconf[layout].splitcfg;
  VMPCfgOptions & vmpcfg = OpenMCU::vmcfg.vmconf[layout].vmpcfg[vmp.n];

  if((fw < 2) || (fh < 2)) return st;

  if(fw < MCUSubsCalc(fw * OpenMCU::vmcfg.bfw / vmpcfg.width, split.minimum_width_for_label)) return st;

  if(ft_error==FT_INITIAL_ERROR) InitializeSubtitles();

  // Stop using freetype on fail
  if(ft_error==FT_FINAL_ERROR) return st;
  if(ft_error)
  {
    PTRACE(1,"FreeType\tError " << ft_error <<", bye-bye");
    ft_error=FT_FINAL_ERROR;
    return st;
  }

  unsigned bl = MCUSubsCalc (fw, vmpcfg.border_left  ),
           br = MCUSubsCalc (fw, vmpcfg.border_right ),
           bt = MCUSubsCalc (fh, vmpcfg.border_top   ),
           bb = MCUSubsCalc (fh, vmpcfg.border_bottom),
           hp = MCUSubsCalc (fw, vmpcfg.h_pad        ),
           vp = MCUSubsCalc (fh, vmpcfg.v_pad        ),
          dsl = MCUSubsCalc (fw, vmpcfg.dropshadow_l ),
          dsr = MCUSubsCalc (fw, vmpcfg.dropshadow_r ),
          dst = MCUSubsCalc (fh, vmpcfg.dropshadow_t ),
          dsb = MCUSubsCalc (fh, vmpcfg.dropshadow_b ),
  fontsizepix = MCUSubsCalc (fh, vmpcfg.fontsize)     ;

  if(fontsizepix < 4) return st;

  int wi = (int)fw-(hp<<1)-bl-br;
  int hi = (int)fh-(vp<<1)-bt-bb;
  if((wi<2)||(hi<2)) return st;
  unsigned w = (unsigned)wi;
  unsigned h = (unsigned)hi;

  PWaitAndSignal m(ft_mutex);

  if((ft_error = FT_Set_Pixel_Sizes(ft_face,0,fontsizepix))) return st;

  PINDEX len = st->text.GetLength();
  if(len==0) return st;

  struct MyBMP{ PBYTEArray *bmp; int l, t, w, h, x; };
  MyBMP *bmps=NULL;
  PINDEX slotCounter=0;

  unsigned pen_x = 0, pen_y=0;
  unsigned pen_x_max = pen_x, c, c2, ft_previous = 0;
  unsigned hMax=0;
  for(PINDEX i=0;i<len;++i)
  {
    c = (BYTE)st->text[i]; if(i<len-1)c2=(BYTE)st->text[i+1]; else c2=0;
    if(vmpcfg.cut_before_bracket) if((c==' ') && ((c2=='[')||(c2=='('))) break;
    if     (!(c&128))                 {/* 0xxxxxxx */ } // utf-8 -> unicode
    else if(((c&224)==192)&&(i+1<len)){/* 110__ 10__ */ c = ((c&31)<<6) + (c2&63); i++; }
    else if(((c&240)==224)&&(i+2<len)){/* 1110__ 10__ 10__ */ c = ((c&15)<<12) + ((c2&63)<<6) + ((BYTE)st->text[i+2]&63); i+=2; }
    else if(((c&248)==240)&&(i+3<len)){/* 11110__ 10__ 10__ 10__ */ c = ((c&7)<<18) + ((c2&63)<<12) + (((unsigned)((BYTE)st->text[i+2]&63))<<6) + ((BYTE)st->text[i+3]&63); i+=3; }

    bmps = (MyBMP*)realloc((void *)bmps, (slotCounter + 1) * sizeof(MyBMP));
    FT_GlyphSlot ft_slot = ft_face->glyph;
    ft_glyph_index = FT_Get_Char_Index(ft_face, c);

    if(ft_use_kerning && ft_previous && ft_glyph_index)
    {
      FT_Vector delta;
      FT_Get_Kerning(ft_face, ft_previous, ft_glyph_index, FT_KERNING_DEFAULT, &delta);
      pen_x += delta.x>>6;
    }

    if( (ft_error = FT_Load_Glyph(ft_face, ft_glyph_index, FT_LOAD_RENDER)) ) break;

    MyBMP & bmp = bmps[slotCounter];
    if(pen_x + (ft_slot->advance.x>>6) >= w)
    { // horizontal overflow: make new line
      if(pen_x_max < pen_x) pen_x_max = pen_x; // store max. h. pos in pen_x_max
      pen_x = 0; // CR
      pen_y += fontsizepix+1; // LF
      if(pen_y + fontsizepix+1 >= h) break; // vertical overflow: no more place, stopping
    }

    bmp.x          = pen_x;
    bmp.l          = ft_slot->bitmap_left;
    bmp.t          = ft_slot->bitmap_top;
    bmp.w          = (&ft_slot->bitmap)->width;
    bmp.h          = (&ft_slot->bitmap)->rows;
    if(bmp.h>(int)hMax) hMax=(unsigned)bmp.h;
    size_t bmpSize = bmp.w * bmp.h;
    bmp.bmp        = new PBYTEArray(bmpSize);
    memcpy(bmp.bmp->GetPointer(bmpSize), (&ft_slot->bitmap)->buffer, bmpSize);

    pen_x += ft_slot->advance.x>>6;
    ft_previous = ft_glyph_index;
    slotCounter++;
  }

  if(hMax>fontsizepix+1)
  {
    PTRACE(3,"FreeType\tHeight of some characters in greater than height of a line");
  }

  if(pen_x_max < pen_x) pen_x_max = pen_x;
  unsigned lw = (bl+pen_x_max+br+1)&~1; st->w = lw;
  unsigned lh = (bt+pen_y+fontsizepix+2+bb)&~1; st->h = lh;

  if(ft_properties & FT_P_RIGHT) st->x = fw-hp-lw;
  else if(ft_properties & FT_P_H_CENTER) st->x = (fw-lw)>>1;
  else st->x = hp;
  if(ft_properties & FT_P_BOTTOM) st->y = fh-vp-lh-1;
  else if(ft_properties & FT_P_V_CENTER) st->y = (fh-lh)>>1;
  else st->y = vp;
  st->x&=~1; st->y&=~1;

  size_t bufferSize = ((lw*lh*3) >> 1);
  st->b=malloc(bufferSize);
  if(st->b)
  {
    PTRACE(3,"FreeType\tRendering to st->b[" << bufferSize << "]");
    FillYUVFrame_YUV(st->b,0,vmpcfg.label_bgcolor>>8,vmpcfg.label_bgcolor&0xFF,lw,lh);
//    pen_y=lh-bb-2;
    pen_y=bt;
    for(PINDEX i=0;i<slotCounter;i++)
    {
      if(i>0) if(bmps[i].x < bmps[i-1].x) pen_y+=(fontsizepix+1); //lf
//      int y=pen_y+fontsizepix-bmps[i].t-1;
      int y=pen_y+fontsizepix-bmps[i].t;
      int h=bmps[i].h;
      if((int)(y+h) > (int)(lh+(bb>>1))) h=fontsizepix+1+(bb>>1)-y; //allow to use 1/2 of bottom border

 int x = bmps[i].l + bmps[i].x + bl;
 if(x < 0 || y < 0) continue;     // вот оно
 if(x + bmps[i].w > (int)lw) continue; // это я так, должно быть меньше ???

      CopyGrayscaleIntoFrame( bmps[i].bmp->GetPointer(), st->b,
       bmps[i].l + bmps[i].x + bl, y,
       bmps[i].w, h, lw, lh );
    }

    if(ft_properties & FT_P_SUBTITLES) SubtitlesDropShadow(st->b, lw, lh, dsl, dst, dsr, dsb);
  }

  for(PINDEX i=slotCounter-1;i>=0;i--)
  {
    delete bmps[i].bmp;
  }
  free(bmps);

  return st;
}
#endif


void MCUVideoMixer::VideoSplitLines(BYTE *d, unsigned fw, unsigned fh){
 unsigned int i;
 for(i=1;i<fh-1;i++){
  if(d[i*fw]>127)d[i*fw]=255;else if(d[i*fw]<63)d[i*fw]=64; else d[i*fw]<<=1;
  d[i*fw+fw-1]>>=1;
 }
 for(i=1;i<fw-1;i++){
  if(d[i]>127)d[i]=255;else if(d[i]<63)d[i]=64; else d[i]<<=1;
  d[(fh-1)*fw+i]>>=1;
 }
 if(d[0]>127)d[0]=255;else if(d[0]<63)d[0]=64; else d[0]<<=1;
 d[fw-1]>>=2;
 d[(fh-1)*fw]>>=2;
 d[(fh-1)*fw+fw-1]>>=1;
 return;
}

void MCUVideoMixer::SplitLine(BYTE *d, unsigned x, unsigned y, unsigned w, unsigned h, unsigned fw, unsigned fh)
{
  d = d + (y * fw) + x;
  for(unsigned i = 0; i < h; ++i)
  {
    for(unsigned j = 0; j < w; ++j)
    {
      if(d[j] == 16)
        d[j] = 63;
      else if(d[j] != 63)
        d[j] = 0;
    }
    d += fw;
  }
}
void MCUVideoMixer::SplitLineLeft(BYTE *d, unsigned x, unsigned y, unsigned w, unsigned h, unsigned fw, unsigned fh)
{
  SplitLine(d, x, y, 1, h, fw, fh);
}
void MCUVideoMixer::SplitLineRight(BYTE *d, unsigned x, unsigned y, unsigned w, unsigned h, unsigned fw, unsigned fh)
{
  SplitLine(d, x+w-1, y, 1, h, fw, fh);
}
void MCUVideoMixer::SplitLineTop(BYTE *d, unsigned x, unsigned y, unsigned w, unsigned h, unsigned fw, unsigned fh)
{
  SplitLine(d, x, y, w, 1, fw, fh);
}
void MCUVideoMixer::SplitLineBottom(BYTE *d, unsigned x, unsigned y, unsigned w, unsigned h, unsigned fw, unsigned fh)
{
  SplitLine(d, x, y+h-1, w, 1, fw, fh);
}

///////////////////////////////////////////////////////////////////////////////////////

MCUSimpleVideoMixer::MCUSimpleVideoMixer(BOOL _forceScreenSplit)
{
  PTRACE(2,"VideoMixer\tMCUSimpleVideoMixer(" << _forceScreenSplit << ") created");
  forceScreenSplit = _forceScreenSplit;
  VMPListInit();
  specialLayout = 0;
}

MCUSimpleVideoMixer::~MCUSimpleVideoMixer()
{
}

BOOL MCUSimpleVideoMixer::ReadFrame(ConferenceMember & member, void * buffer, int width, int height, PINDEX & amount)
{
  if(member.GetType() != MEMBER_TYPE_CACHE && PTime()-member.firstFrameSendTime < 3000)
  {
    VideoFrameStoreList::shared_iterator fsit = frameStores.GetFrameStore(width, height);
    VideoFrameStore & fs = **fsit;
    if(fs.logo_frame.GetSize() != 0)
      memcpy(buffer, fs.logo_frame.GetPointer(), fs.frame_size);
    return TRUE;
  }
  return ReadMixedFrame(frameStores, buffer, width, height, amount);
}

BOOL MCUSimpleVideoMixer::ReadMixedFrame(void * buffer, int width, int height, PINDEX & amount)
{
  return ReadMixedFrame(frameStores, buffer, width, height, amount);
}

BOOL MCUSimpleVideoMixer::ReadMixedFrame(VideoFrameStoreList & srcFrameStores, void * buffer, int width, int height, PINDEX & amount)
{
  VideoFrameStoreList::shared_iterator fsit = srcFrameStores.GetFrameStore(width, height);
  VideoFrameStore & fs = **fsit;

  // background
  if(fs.bg_frame.GetSize() != 0)
    memcpy(buffer, fs.bg_frame.GetPointer(), fs.frame_size);

  for(unsigned i = 0; i < OpenMCU::vmcfg.vmconf[specialLayout].splitcfg.vidnum; i++)
  {
    VMPCfgOptions & vmpcfg = OpenMCU::vmcfg.vmconf[specialLayout].vmpcfg[i];
    int px = (float)vmpcfg.posx  *width/CIF4_WIDTH; // pixel x&y of vmp-->fs
    int py = (float)vmpcfg.posy  *height/CIF4_HEIGHT;
    int pw = (float)vmpcfg.width *width/CIF4_WIDTH; // pixel w&h of vmp-->fs
    int ph = (float)vmpcfg.height*height/CIF4_HEIGHT;
    if(pw<2 || ph<2) continue;

    MCUVMPList::shared_iterator vmp_it = VMPFind((int)i);
    if(vmp_it != vmpList.end())
    {
      VideoMixPosition *vmp = *vmp_it;
      if(vmp->vmpbuf_index >= 0)
      {
        MCUBufferYUVArrayList::shared_iterator vmpbuf_it = vmp->bufferList.Find((long)&fs);
        if(vmpbuf_it != vmp->bufferList.end())
        {
          MCUBufferYUV *vmpbuf = (**vmpbuf_it)[vmp->vmpbuf_index];
          if(vmpbuf->GetWidth() == pw && vmpbuf->GetHeight() == ph)
          {
            if(vmpcfg.blks == 1)
              CopyRectIntoFrame(vmpbuf->GetPointer(), buffer, px, py, pw, ph, width, height);
            else
              for(unsigned i = 0; i < vmpcfg.blks; i++)
                CopyRFromRIntoR(vmpbuf->GetPointer(), buffer, px, py, pw, ph,
                  AlignUp2(vmpcfg.blk[i].posx*width/CIF4_WIDTH), AlignUp2(vmpcfg.blk[i].posy*height/CIF4_HEIGHT),
                  AlignUp2(vmpcfg.blk[i].width*width/CIF4_WIDTH), AlignUp2(vmpcfg.blk[i].height*height/CIF4_HEIGHT),
                  width, height, pw, ph );
          }
          else
            MCUTRACE(6, "VideoMixer: VMP read error0: n=" << vmp->n << " fs=" << width << "x" << height << " pos=" << pw << "x" << ph << " buf=" << vmpbuf->GetWidth() << "x" << vmpbuf->GetHeight());
        }
      }
    }

    // grid
    if(OpenMCU::vmcfg.vmconf[specialLayout].vmpcfg[i].border)
    {
      if(px != 0)
        SplitLineLeft((BYTE *)buffer, px, py, pw, ph, width, height);
      if(py != 0)
        SplitLineTop((BYTE *)buffer, px, py, pw, ph, width, height);
      //if(px+pw != width)
        //SplitLineRight((BYTE *)buffer, px, py, pw, ph, width, height);
      //if(py+ph != height)
        //SplitLineBottom((BYTE *)buffer, px, py, pw, ph, width, height);
    }
  }

  fs.lastRead = time(NULL);
  return TRUE;
}


BOOL MCUSimpleVideoMixer::WriteFrame(ConferenceMemberId id, const void * buffer, int width, int height)
{
  MCUVMPList::shared_iterator it = VMPFind(id);
  if(it == vmpList.end())
    return FALSE;
  VideoMixPosition *vmp = *it;
  vmp->offline = FALSE;
  return WriteSubFrame(*vmp, buffer, width, height, WSF_VMP_COMMON);
}

BOOL MCUSimpleVideoMixer::WriteSubFrame(VideoMixPosition & vmp, const void * buffer, int width, int height, int options)
{
  VMPCfgOptions & vmpcfg=OpenMCU::vmcfg.vmconf[specialLayout].vmpcfg[vmp.n];
  time_t now = time(NULL);

  if(options & WSF_VMP_SET_TIME)
    vmp.lastWrite=now;
  unsigned rule;
  if(options & WSF_VMP_FORCE_CUT) rule=0; else rule=vmp.rule;

  int vmpbuf_index = vmp.vmpbuf_index + 1;
  if(vmpbuf_index == 3)
    vmpbuf_index = 0;

  for(VideoFrameStoreList::shared_iterator it = frameStores.frameStoreList.begin(); it != frameStores.frameStoreList.end(); ++it)
  {
    VideoFrameStore & vf = **it;
    if(vf.width<2 || vf.height<2) continue; // minimum size 2*2

    // pixel w&h of vmp-->fs:
    int pw = (float)vmpcfg.width *vf.width /CIF4_WIDTH;
    int ph = (float)vmpcfg.height*vf.height/CIF4_HEIGHT;
    if(pw<2 || ph<2) continue;

    MCUBufferYUVArrayList::shared_iterator vmpbuf_it = vmp.bufferList.Find((long)&vf);
    if(vmpbuf_it == vmp.bufferList.end())
      vmpbuf_it = vmp.bufferList.Insert(new MCUBufferYUVArray(3, 0, 0), (long)&vf);
    MCUBufferYUV *vmpbuf = (**vmpbuf_it)[vmpbuf_index];
    vmpbuf->SetFrameSize(pw, ph);

    float src_aspect_ratio = (float)width/height;
    float dst_aspect_ratio = (float)pw/ph;

    if(pw==width && ph==height) //same size
    {
      memcpy(vmpbuf->GetPointer(), buffer, pw*ph*3/2); //making copy for subtitles & border
    }
    else if(src_aspect_ratio > dst_aspect_ratio+0.05)
    {
      //broader:  +---------+     pw      rule 0 => cut width
      //          |  width  |    +--+     rule 1 => add stripes to top and bottom
      //    height|         | -> |  |ph
      //          +---------+    +--+
      if(rule==0)
      {
        int dstWidth = (float)ph*width/height; //bigger than we need
        vmp.tmpbuf.SetFrameSize(dstWidth, ph);
        ResizeYUV420P((const BYTE *)buffer, vmp.tmpbuf.GetPointer(), width, height, dstWidth, ph);
        CopyRectFromFrame(vmp.tmpbuf.GetPointer(), vmpbuf->GetPointer(), (dstWidth-pw)/2, 0, pw, ph, dstWidth, ph);
      }
      else if(rule==1)
      {
        int dstHeight = (float)pw*height/width; //smaller than we need
        vmp.tmpbuf.SetFrameSize(pw, dstHeight);
        ResizeYUV420P((const BYTE *)buffer, vmp.tmpbuf.GetPointer(), width, height, pw, dstHeight);
        FillYUVRect(vmpbuf->GetPointer(),pw,ph,127,127,127, 0,0, pw,(ph-dstHeight)/2);
        FillYUVRect(vmpbuf->GetPointer(),pw,ph,127,127,127, 0,ph-(ph-dstHeight)/2, pw,(ph-dstHeight)/2);
        CopyRectIntoFrame(vmp.tmpbuf.GetPointer(), vmpbuf->GetPointer(), 0, (ph-dstHeight)/2, pw, dstHeight, pw, ph);
      }
    }
    else if(src_aspect_ratio < dst_aspect_ratio-0.05)
    {
      //narrower (higher): +-+      pw      rule 0 => cut height
      //                   | |    +----+    rule 1 => add stripes to left and right
      //             height| | -> |    | ph
      //                   +-+    +----+
      if(rule==0)
      {
        int dstHeight = (float)pw*height/width; //bigger than we need
        vmp.tmpbuf.SetFrameSize(pw, dstHeight);
        ResizeYUV420P((const BYTE *)buffer, vmp.tmpbuf.GetPointer(), width, height, pw, dstHeight);
        CopyRectFromFrame(vmp.tmpbuf.GetPointer(), vmpbuf->GetPointer(), 0, (dstHeight-ph)/2, pw, ph, pw, dstHeight);
      }
      else if(rule==1)
      {
        int dstWidth = (float)ph*width/height; //smaller than we need
        vmp.tmpbuf.SetFrameSize(dstWidth, ph);
        ResizeYUV420P((const BYTE *)buffer, vmp.tmpbuf.GetPointer(), width, height, dstWidth, ph);
        FillYUVRect(vmpbuf->GetPointer(),pw,ph,127,127,127, 0,0, (pw-dstWidth)/2, ph);
        FillYUVRect(vmpbuf->GetPointer(),pw,ph,127,127,127, pw-(pw-dstWidth)/2,0, (pw-dstWidth)/2,ph);
        CopyRectIntoFrame(vmp.tmpbuf.GetPointer(), vmpbuf->GetPointer(), (pw-dstWidth)/2, 0, dstWidth, ph, pw, ph);
      }
    }
    else
    { // fit. scale
      ResizeYUV420P((const BYTE *)buffer, vmpbuf->GetPointer() , width, height, pw, ph);
    }

#if USE_FREETYPE
    if(options & WSF_VMP_SUBTITLES)
      if(!(vmpcfg.label_mask&FT_P_DISABLED))
        MCUPrintSubtitles(vmp, (void *)vmpbuf->GetPointer(),pw,ph,vmpcfg.label_mask,specialLayout);
#endif

  }

  vmp.vmpbuf_index = vmpbuf_index;
  return TRUE;
}

void MCUSimpleVideoMixer::RemoveFrameStore(VideoFrameStoreList::shared_iterator & it)
{
  VideoFrameStore *fs = *it;
  if(frameStores.frameStoreList.Erase(it))
  {
    for(MCUVMPList::shared_iterator it = vmpList.begin(); it != vmpList.end(); ++it)
    {
      VideoMixPosition *vmp = *it;
#if USE_FREETYPE
      VMPCfgOptions & vmpcfg = OpenMCU::vmcfg.vmconf[specialLayout].vmpcfg[vmp->n];
      unsigned pw = vmpcfg.width *fs->width/CIF4_WIDTH;
      unsigned ph = vmpcfg.height*fs->height/CIF4_HEIGHT;
      unsigned key= (ph<<16) | pw;
      MCUSubtitlesList::shared_iterator q = vmp->subtitlesList.Find(key);
      if(q != vmp->subtitlesList.end())
      {
        MCUSubtitles *sub = *q;
        if(vmp->subtitlesList.Erase(q))
        {
          if(sub->b) free(sub->b);
          delete sub;
        }
      }
#endif
      MCUBufferYUVArrayList::shared_iterator vmpbuf_it = vmp->bufferList.Find((long)fs);
      if(vmpbuf_it != vmp->bufferList.end())
      {
        MCUBufferYUVArray *buffer = *vmpbuf_it;
        if(vmp->bufferList.Erase(vmpbuf_it))
          delete buffer;
      }
    }
    MCUTRACE(1, "VideoMixer: remove VideoFrameStore " << fs->width << "x" << fs->height);
    delete fs;
  }
}

void MCUSimpleVideoMixer::Monitor(Conference *conference)
{
  PWaitAndSignal m(vmpListMutex);

  for(VideoFrameStoreList::shared_iterator fs_it = frameStores.frameStoreList.begin(); fs_it != frameStores.frameStoreList.end(); ++fs_it)
  {
    VideoFrameStore *fs = *fs_it;
    // remove FrameStore
    if(fs->lastRead < (time(NULL) - FRAMESTORE_TIMEOUT))
    {
      RemoveFrameStore(fs_it);
      continue;
    }
  }

  for(MCUVMPList::shared_iterator vmp_it = vmpList.begin(); vmp_it != vmpList.end(); ++vmp_it)
  {
    VideoMixPosition *vmp = *vmp_it;
    VMPCfgOptions & vmpcfg = OpenMCU::vmcfg.vmconf[specialLayout].vmpcfg[vmp->n];
    // remove subtitles
    for(MCUSubtitlesList::shared_iterator sub_it = vmp->subtitlesList.begin(); sub_it != vmp->subtitlesList.end(); ++sub_it)
    {
      MCUSubtitles *sub = *sub_it;
      VideoFrameStoreList::shared_iterator fs_it;
      for(fs_it = frameStores.frameStoreList.begin(); fs_it != frameStores.frameStoreList.end(); ++fs_it)
      {
        VideoFrameStore *fs = *fs_it;
        int pw = vmpcfg.width *fs->width /CIF4_WIDTH;
        int ph = vmpcfg.height*fs->height/CIF4_HEIGHT;
        if(sub_it.GetID() == ((ph << 16) | pw))
          break;
      }
      if(fs_it == frameStores.frameStoreList.end() && vmp->subtitlesList.Erase(sub_it))
      {
        MCUTRACE(6, "VideoMixer: remove subtitles n=" << vmp->n << " key=" << sub_it.GetID());
        if(sub->b) free(sub->b);
        delete sub;
      }
    }
    //
    for(VideoFrameStoreList::shared_iterator fs_it = frameStores.frameStoreList.begin(); fs_it != frameStores.frameStoreList.end(); ++fs_it)
    {
      VideoFrameStore *fs = *fs_it;
      // render subtitles
      int pw = vmpcfg.width *fs->width /CIF4_WIDTH;
      int ph = vmpcfg.height*fs->height/CIF4_HEIGHT;
      long sub_key = (ph << 16) | pw;
      MCUSubtitlesList::shared_iterator sub_it = vmp->subtitlesList.Find(sub_key);
      if(sub_it == vmp->subtitlesList.end())
      {
        MCUTRACE(6, "VideoMixer: render subtitles n=" << vmp->n << " fs=" << fs->width << "x" << fs->height << " pos=" << pw << "x" << ph << " key=" << sub_key);
        MCUSubtitles *st = MCURenderSubtitles(*vmp, pw, ph, vmpcfg.label_mask, specialLayout);
        if(st) vmp->subtitlesList.Insert(st, sub_key);
      }
      // create buffer
      if(vmp->bufferList.Find((long)fs) == vmp->bufferList.end())
      {
        if(vmpList.Erase(vmp_it))
        {
          VMPTouch(*vmp);
          VMPInsert(vmp);
        }
      }
    }
    // touch
    /*
    if(vmp->vmpbuf_index == -1 || vmp->shows_logo)
    {
      if(vmpList.Erase(vmp_it))
      {
        if(vmp->vmpbuf_index == -1)
        {
          vmp->shows_logo = TRUE;
          unsigned width, height;
          void *buffer = OpenMCU::Current().GetPreMediaFrame(width, height);
          WriteSubFrame(*vmp, buffer, width, height, WSF_VMP_BORDER|WSF_VMP_FORCE_CUT);
        }
        else if(vmp->shows_logo)
        {
          vmp->shows_logo = FALSE;
          VMPTouch(*vmp);
        }
        VMPInsert(vmp);
      }
      continue;
    }
    */
    if(vmp->vmpbuf_index == -1)
    {
      if(vmpList.Erase(vmp_it))
      {
        VMPTouch(*vmp);
        VMPInsert(vmp);
      }
      continue;
    }
  }
}

BOOL MCUSimpleVideoMixer::SetOffline(ConferenceMemberId id)
{
  PWaitAndSignal m(vmpListMutex);

  MCUVMPList::shared_iterator it = VMPFind(id);
  if(it == vmpList.end())
    return FALSE;
  VideoMixPosition *vmp = *it;
  if(vmpList.Erase(it))
  {
    vmp->offline = TRUE;
    vmp->vmpbuf_index = -1;
    VMPInsert(vmp);
  }
  return TRUE;
}

BOOL MCUSimpleVideoMixer::SetOnline(ConferenceMemberId id)
{
  PWaitAndSignal m(vmpListMutex);

  MCUVMPList::shared_iterator it = VMPFind(id);
  if(it == vmpList.end())
    return FALSE;
  VideoMixPosition *vmp = *it;
  if(vmpList.Erase(it))
  {
    vmp->offline = FALSE;
    vmp->vmpbuf_index = -1;
    VMPInsert(vmp);
  }
  return TRUE;
}

void MCUSimpleVideoMixer::WriteEmptyFrame(VideoMixPosition & vmp)
{
  MCUTRACE(6, "VideoMixer: WriteEmptyFrame n=" << vmp.n << " id=" << vmp.id << " offline=" << vmp.offline << " write=" << (time(NULL)-vmp.lastWrite));
  unsigned width, height, options;
  void * buffer = OpenMCU::Current().GetEmptyFramePointer(width, height);
  if(width==16 && height==16) options=WSF_VMP_BORDER|WSF_VMP_FORCE_CUT;
  else options=WSF_VMP_BORDER;
  if(buffer) WriteSubFrame(vmp, buffer, width, height, options);
}

void MCUSimpleVideoMixer::WriteOfflineFrame(VideoMixPosition & vmp)
{
  MCUTRACE(6, "VideoMixer: WriteOfflineFrame n=" << vmp.n << " id=" << vmp.id << " offline=" << vmp.offline << " write=" << (time(NULL)-vmp.lastWrite));
  unsigned width, height, options;
  void * buffer = OpenMCU::Current().GetOfflineFramePointer(width, height);
  if(width==16 && height==16) options=WSF_VMP_COMMON|WSF_VMP_FORCE_CUT;
  else options=WSF_VMP_COMMON;
  if(buffer) WriteSubFrame(vmp, buffer, width, height, options);
}

void MCUSimpleVideoMixer::WriteNoVideoFrame(VideoMixPosition & vmp)
{
  MCUTRACE(6, "VideoMixer: WriteNoVideoFrame n=" << vmp.n << " id=" << vmp.id << " offline=" << vmp.offline << " write=" << (time(NULL)-vmp.lastWrite));
  unsigned width, height, options;
  void * buffer = OpenMCU::Current().GetNoVideoFramePointer(width, height);
  if(width==16 && height==16) options=WSF_VMP_SUBTITLES|WSF_VMP_BORDER|WSF_VMP_FORCE_CUT;
  else options=WSF_VMP_SUBTITLES|WSF_VMP_BORDER;
  if(buffer) WriteSubFrame(vmp, buffer, width, height, options);
}

void MCUSimpleVideoMixer::VMPTouch(VideoMixPosition & vmp)
{
  if(!vmp.id) WriteEmptyFrame(vmp);
  else if(vmp.offline) WriteOfflineFrame(vmp);
  else if((time(NULL)-vmp.lastWrite) > 1) WriteNoVideoFrame(vmp);
}

void MCUSimpleVideoMixer::VMPCopy(VideoMixPosition & vmp1, VideoMixPosition & vmp2)
{
  vmp2.id             = vmp1.id;
  vmp2.SetEndpointName(vmp1.GetEndpointName());
  vmp2.lastWrite      = vmp1.lastWrite;
  vmp2.silenceCounter = vmp1.silenceCounter;
  vmp2.rule           = vmp1.rule;
  vmp2.chosenVan      = vmp1.chosenVan;
  vmp2.offline        = vmp1.offline;
  vmp2.chosenVan      = vmp1.chosenVan;
  if((vmp2.type<1)||(vmp2.type>3)) vmp2.type = vmp1.type;
}

void MCUSimpleVideoMixer::VMPSwap(VideoMixPosition & vmp1, VideoMixPosition & vmp2)
{
  VideoMixPosition vmp(0);
  VMPCopy(vmp2, vmp);
  VMPCopy(vmp1, vmp2);
  VMPCopy(vmp, vmp1);
}

void MCUSimpleVideoMixer::ReallocatePositions()
{
  PWaitAndSignal m(vmpListMutex);

  int i = 0;
  for(MCUVMPList::shared_iterator it = vmpList.begin(); it != vmpList.end(); ++it, ++i)
  {
    VideoMixPosition *vmp = *it;
    if(vmpList.Erase(it))
    {
      vmp->n = i;
      vmp->vmpbuf_index = -1;
      VMPInsert(vmp);
    }
  }
}

void MCUSimpleVideoMixer::Shuffle()
{
  PWaitAndSignal m(vmpListMutex);

  if(vmpList.GetSize() == 0) return;
  unsigned & vidnum = OpenMCU::vmcfg.vmconf[specialLayout].splitcfg.vidnum;
  ConferenceMemberId * tempMemberList = new ConferenceMemberId [vidnum];
  PString * tempNameList = new PString [vidnum];
  BOOL * tempOfflineList = new BOOL [vidnum];
  unsigned memberCount=0;

  for(MCUVMPList::shared_iterator it = vmpList.begin(); it != vmpList.end(); ++it)
  {
    VideoMixPosition *v = *it;
    tempNameList[memberCount]=v->GetEndpointName();
    tempOfflineList[memberCount]=v->offline;
    tempMemberList[memberCount++]=v->id;
    VMPDelete(it);
  }
  for(unsigned i=0;i<memberCount;i++)
  {
    unsigned r=rand()%vidnum, a=1;
    if(r&1) a=vidnum-1;
    ConferenceMemberId id;
    while((unsigned long)(id=GetPositionId(r))>99) r=(r+a)%vidnum;
    if(id==0)
    {
      MCUVMPList::shared_iterator it = VMPCreator(NULL, r, 2);
      if(it != vmpList.end())
      {
        VideoMixPosition *vmp = *it;
        vmp->id = tempMemberList[i];
        vmp->SetEndpointName(tempNameList[i]);
        vmp->offline = tempOfflineList[i];
        vmp->type = 1;
      }
    }
    else
    {
      MCUVMPList::shared_iterator it = VMPFind(id);
      if(it != vmpList.end())
      {
        VideoMixPosition *vmp = *it;
        vmp->id = tempMemberList[i];
        vmp->SetEndpointName(tempNameList[i]);
        vmp->lastWrite = 0;
        vmp->silenceCounter = 0;
        vmp->offline = tempOfflineList[i];
        vmp->vmpbuf_index = -1;
      }
    }
  }
  delete[]tempNameList;
  delete[]tempMemberList;
  delete[]tempOfflineList;
}

void MCUSimpleVideoMixer::Scroll(BOOL reverse)
{
  PWaitAndSignal m(vmpListMutex);

  if(vmpList.GetSize() == 0) return;
  unsigned & vidnum=OpenMCU::vmcfg.vmconf[specialLayout].splitcfg.vidnum;
  if(vidnum < 2) return;

  for(MCUVMPList::shared_iterator it = vmpList.begin(); it != vmpList.end(); ++it)
  {
    VideoMixPosition *v = *it;
    if(vmpList.Erase(it))
    {
      if(reverse) v->n = (v->n + vidnum - 1) % vidnum;
      else        v->n = (v->n +          1) % vidnum;
      v->vmpbuf_index = -1;
      VMPInsert(v);
    }
  }
}

void MCUSimpleVideoMixer::Revert()
{
  PWaitAndSignal m(vmpListMutex);

  if(vmpList.GetSize() == 0) return;
  unsigned & vidnum = OpenMCU::vmcfg.vmconf[specialLayout].splitcfg.vidnum;
  if(vidnum < 2) return;

  for(MCUVMPList::shared_iterator it = vmpList.begin(); it != vmpList.end(); ++it)
  {
    VideoMixPosition *v = *it;
    if(vmpList.Erase(it))
    {
      v->n = vidnum-1-v->n;
      v->vmpbuf_index = -1;
      VMPInsert(v);
    }
  }
}

int MCUSimpleVideoMixer::VMPListFindEmptyIndex()
{
  MCUVMPList::shared_iterator it;
  for(int i=0; (unsigned)i<OpenMCU::vmcfg.vmconf[specialLayout].splitcfg.vidnum; i++)
  {
    for(it = vmpList.begin(); it != vmpList.end(); ++it)
    {
      if(it->n == i)
        break;
    }
    if(it == vmpList.end()) return i;
  }
  return -1;
}

MCUVMPList::shared_iterator MCUSimpleVideoMixer::VMPCreator(ConferenceMember * member, int n, int type)
{
  PWaitAndSignal m(vmpListMutex);

  MCUVMPList::shared_iterator it;
  if(member==NULL && type==1) return it;
  if((unsigned)n >= OpenMCU::vmcfg.vmconf[specialLayout].splitcfg.vidnum) return it;

  if(member!=NULL)
  {
    MCUVMPList::shared_iterator old_it = VMPFind(member->GetID());
    if(old_it != vmpList.end()) if(old_it->n != n)  // check whether source already exists in layout
    {
      old_it.Release();
      VMPDelete(old_it);
    }
  }

  it = VMPFind(n);
  VideoMixPosition *newPosition = NULL;
  BOOL create = it == vmpList.end();
  if(create)
  {
    if(member != NULL) newPosition = CreateVideoMixPosition(member->GetID());
    else newPosition = CreateVideoMixPosition(n);
    if(newPosition != NULL) newPosition->n = n;
  }
  else
  {
    newPosition = *it;
    if(member!=NULL) newPosition->id=member->GetID();
    else newPosition->id=(ConferenceMemberId)n;
    newPosition->vmpbuf_index = -1;
  }

  if(newPosition == NULL) return it;

  newPosition->type = type;
  if(member != NULL)
  {
    newPosition->rule = member->resizerRule;
    newPosition->SetEndpointName(member->GetName());
    newPosition->chosenVan = member->chosenVan;
    newPosition->offline = !member->IsOnline();
  }
  else
  {
    newPosition->SetEndpointName("VAD" + PString(type-1) + "/" + PString(n));
    newPosition->chosenVan = 0;
    newPosition->offline = FALSE;
  }
  if(create) it = VMPInsert(newPosition);
  return it;
}

void MCUSimpleVideoMixer::Update(ConferenceMember * member)
{
  if(member==NULL) return;
  PWaitAndSignal m(vmpListMutex);
  MCUVMPList::shared_iterator it = VMPFind(member->GetID());
  if(it == vmpList.end()) return;
  VideoMixPosition *vmp = *it;

  vmp->rule = member->resizerRule;
}

BOOL MCUSimpleVideoMixer::AddVideoSourceToLayout(ConferenceMemberId id, ConferenceMember & mbr)
{
  PWaitAndSignal m(vmpListMutex);

  int i = VMPListFindEmptyIndex();
  if(i < 0)
    return FALSE;

  MCUVMPList::shared_iterator it = VMPCreator(&mbr, i, 1);
  if(it != vmpList.end())
    return TRUE;

  return FALSE;
}

BOOL MCUSimpleVideoMixer::AddVideoSource(ConferenceMemberId id, ConferenceMember & mbr)
{
  PWaitAndSignal m(vmpListMutex);

  MCUVMPList::shared_iterator it;
  int newsL=GetMostAppropriateLayout(vmpList.GetSize()+1);
  if(newsL != specialLayout || vmpList.GetSize() == 0) // split changed or first vmp
  {
    specialLayout=newsL;
    it = VMPCreator(&mbr, vmpList.GetSize(), 1);
    if(it != vmpList.end())
    {
      it.Release();
      ReallocatePositions();
      it = VMPFind(id);
    }
  }
  else  // otherwise find an empty position
  {
    int i = VMPListFindEmptyIndex();
    if(i >= 0)
      it = VMPCreator(&mbr, i, 1);
  }

  BOOL result = (it != vmpList.end());
  PTRACE_IF(2, !result, "VideoMixer\tAddVideoSource " << id << " could not find empty video position");
  return result;
}

void MCUSimpleVideoMixer::RemoveVideoSource(ConferenceMemberId id, ConferenceMember & mbr)
{
  PWaitAndSignal m(vmpListMutex);

  MCUVMPList::shared_iterator it = VMPFind(id);
  if(it == vmpList.end())
    return;
  VMPDelete(it);

  int newsL=GetMostAppropriateLayout(vmpList.GetSize());
  if (newsL!=specialLayout || OpenMCU::vmcfg.vmconf[newsL].splitcfg.reallocate_on_disconnect)
  {
    specialLayout=newsL;
    ReallocatePositions();
  }
}

int MCUSimpleVideoMixer::GetPositionSet()
{
  return specialLayout;
}

int MCUSimpleVideoMixer::GetPositionNum(ConferenceMemberId id)
{
  MCUVMPList::shared_iterator it = VMPFind(id);
  if(it == vmpList.end())
    return -1;
  return it->n;
}

void MCUSimpleVideoMixer::SetPositionType(int pos, int type)
{
  MCUVMPList::shared_iterator it = VMPFind(pos);
  if(it != vmpList.end())
  {
    it->type=type;
    return;
  }

  if((unsigned)pos>=OpenMCU::vmcfg.vmconf[specialLayout].splitcfg.vidnum) return;
  if(type==1) return;

  VMPCreator(NULL, pos, type);
}

BOOL MCUSimpleVideoMixer::VMPExists(ConferenceMemberId id)
{
  return (VMPFind(id) != vmpList.end());
}

int MCUSimpleVideoMixer::GetSilenceCounter(ConferenceMemberId id)
{
  MCUVMPList::shared_iterator it = VMPFind(id);
  if(it == vmpList.end())
    return -1;
  return it->silenceCounter;
}

void MCUSimpleVideoMixer::ResetSilenceCounter(ConferenceMemberId id)
{
  MCUVMPList::shared_iterator it = VMPFind(id);
  if(it == vmpList.end())
    return;
  it->silenceCounter = 0;
}

void MCUSimpleVideoMixer::IncreaseSilenceCounter(ConferenceMemberId id, int milliseconds)
{
  MCUVMPList::shared_iterator it = VMPFind(id);
  if(it == vmpList.end())
    return;
  it->silenceCounter+=milliseconds;
}

int MCUSimpleVideoMixer::GetPositionType(ConferenceMemberId id)
{
  MCUVMPList::shared_iterator it = VMPFind(id);
  if(it == vmpList.end())
    return -1;
  return it->type;
}

int MCUSimpleVideoMixer::GetPositionType(int pos)
{
  for(MCUVMPList::shared_iterator it = vmpList.begin(); it != vmpList.end(); ++it)
  {
    if(it->n == pos)
      return it->type;
  }
  return 0;
}

ConferenceMemberId MCUSimpleVideoMixer::GetPositionId(int pos)
{
  for(MCUVMPList::shared_iterator it = vmpList.begin(); it != vmpList.end(); ++it)
  {
    if(it->n == pos)
      return it->id;
  }
  return 0;
}

BOOL MCUSimpleVideoMixer::TryOnVADPosition(ConferenceMember * member)
{
  PWaitAndSignal m(vmpListMutex);

  MCUVMPList::shared_iterator vit;
  long orderKey=0;

  for(MCUVMPList::shared_iterator it = vmpList.begin(); it != vmpList.end(); ++it)
  {
    VideoMixPosition *vmp = *it;
    if((vmp->type!=2)&&(vmp->type!=3)) { continue;}
    if((vmp->id<0)||(vmp->id>=100)) { continue;}
    long currentOrderKey=(vmp->type<<16)-vmp->id; //at least 131072-x where x=0..99
    if(currentOrderKey>orderKey)
    {
      orderKey=currentOrderKey;
      vit = it;
    }
  }
  if(!(vit != vmpList.end() && orderKey)) return FALSE;

  VMPCreator(member, (*vit)->n, (*vit)->type);

  return TRUE;
}

BOOL MCUSimpleVideoMixer::SetVADPosition(ConferenceMember * member, int chosenVan, unsigned short timeout)
{
  PWaitAndSignal m(vmpListMutex);

  if(member==NULL) return FALSE;
  int maxSilence = -1;
  MCUVMPList::shared_iterator vit;

  for(MCUVMPList::shared_iterator it = vmpList.begin(); it != vmpList.end(); ++it)
  {
    VideoMixPosition *vmp = *it;
    if((vmp->type&~1) != 2) { continue;} //only look for VAD & VAD2
    if(vmp->chosenVan) { continue; } // skip chosenVan

    if(vmp->id<1000) // free VAD position
    {
      int silence = 0x3fffffff-vmp->n;
      if(vmp->type==2) silence += 0x40000000; // VAD1 better than VAD2
      if(silence>maxSilence) {maxSilence=silence; vit=it;}
      continue;
    }
    // wrap-around for offline endpoints:
    if(vmp->offline) vmp->silenceCounter = timeout;
    // busy VAD position:
    if(vmp->type==2 && vmp->silenceCounter > maxSilence) { maxSilence = vmp->silenceCounter; vit = it;  }
    // busy VAD2 position:
    if(vmp->type==3 && chosenVan && vmp->silenceCounter>maxSilence) { maxSilence = vmp->silenceCounter; vit = it; }
  }

  if(maxSilence == -1) return FALSE;
  if(vit == vmpList.end()) return FALSE;
  if(maxSilence < timeout && !chosenVan) return FALSE;
  if(vit->id == member->GetID()) { vit->silenceCounter=0; return TRUE; }

  vit = VMPCreator(member, vit->n, vit->type);
  if(vit == vmpList.end()) return FALSE;
  vit->silenceCounter=0;

  PTRACE(3,"VideoMixer\tSetVADPosition(" << member->GetName() << ", " << chosenVan << ", " << timeout
    << "): maxSilence=" << maxSilence << ", n=" << vit->n);

  return TRUE;
}

BOOL MCUSimpleVideoMixer::SetVAD2Position(ConferenceMember *member)
{
  int timeout = 3000;

  PWaitAndSignal m(vmpListMutex);

  if(member==NULL) return FALSE;
  ConferenceMemberId id=member->GetID();
  int maxSilence=-1;

  if(GetPositionType(id)!=2) return FALSE; // must be VAD1 to be switched to VAD2

  MCUVMPList::shared_iterator vit;
  for(MCUVMPList::shared_iterator it = vmpList.begin(); it != vmpList.end(); ++it)
  {
    VideoMixPosition *vmp = *it;
    if(vmp->type==3) //only look for VAD2-type positions
    {
      int silenceCounter;
      if(vmp->id < 1000) silenceCounter = 0x7fffffff-vmp->n;
      else silenceCounter = vmp->silenceCounter;
      if(vmp->offline) silenceCounter = timeout;
      if(silenceCounter > maxSilence) { maxSilence=silenceCounter; vit = it; }
    }
  }

  if(vit == vmpList.end()) return FALSE;
  if(maxSilence < timeout) return FALSE;
  if(vit->id == id) { vit->silenceCounter = 0; return TRUE; }

  MCUVMPList::shared_iterator old_vit = VMPFind(id);
  if(old_vit == vmpList.end()) return FALSE;
  old_vit->silenceCounter = 0; vit->silenceCounter = 0;

  VideoMixPosition *vmp = *vit;
  VideoMixPosition *old_vmp = *old_vit;
  if(vmpList.Erase(old_vit))
  {
    if(vmpList.Erase(vit))
    {
      VMPSwap(*old_vmp, *vmp);
      vmp->vmpbuf_index = -1;
      VMPInsert(vmp);
    }
    old_vmp->vmpbuf_index = -1;
    VMPInsert(old_vmp);
  }

  PTRACE(3,"VideoMixer\tSetVAD2Position(" << member->GetName()
    << "): maxSilence=" << maxSilence << ", n=" << vmp->n);

  return TRUE;
}

void MCUSimpleVideoMixer::MyChangeLayout(unsigned newLayout)
{
  PWaitAndSignal m(vmpListMutex);

  if(newLayout >= OpenMCU::vmcfg.vmconfs) return;
  int newCount = OpenMCU::vmcfg.vmconf[newLayout].splitcfg.vidnum;

  MCUVMPList list; // temporary
  for(MCUVMPList::shared_iterator it = vmpList.begin(); it != vmpList.end(); ++it)
  {
    VideoMixPosition *vmp = *it;
    if(vmpList.Erase(it))
    {
      if(vmp->n < newCount)
        list.Insert(vmp, vmp->id);
      else
        delete vmp;
    }
  }

  specialLayout = newLayout; // change mixer layout
  for(MCUVMPList::shared_iterator it = list.begin(); it != list.end(); ++it)
  {
    VideoMixPosition *vmp = *it;
    vmp->vmpbuf_index = -1;
    VMPInsert(vmp);
  }
}

void MCUSimpleVideoMixer::PositionSetup(int pos, int type, ConferenceMember * member) //types 1000, 1001, 1002, 1003 means type will not changed
{
  int old_type = 0;
  PWaitAndSignal m(vmpListMutex);
  if(type>=1000)
  {
    MCUVMPList::shared_iterator it = VMPFind(pos);
    if(it != vmpList.end()) old_type=(*it)->type;
  }
  MCUVMPList::shared_iterator it = VMPCreator(member, pos, type % 1000);
  if(it!=vmpList.end()) if(old_type) (*it)->type = old_type;
}

void MCUSimpleVideoMixer::Exchange(int pos1, int pos2)
{
  PWaitAndSignal m(vmpListMutex);

  unsigned layoutCapacity = OpenMCU::vmcfg.vmconf[specialLayout].splitcfg.vidnum;
  if(((unsigned)pos1>=layoutCapacity)||((unsigned)pos2>=layoutCapacity)) return;

  MCUVMPList::shared_iterator it1 = VMPFind(pos1);
  MCUVMPList::shared_iterator it2 = VMPFind(pos2);
  if(it1 == vmpList.end() && it2 == vmpList.end())
    return;

  VideoMixPosition *v1 = *it1;
  VideoMixPosition *v2 = *it2;

  if(it2 == vmpList.end()) // lazy to type the following twice
  {
    if(vmpList.Erase(it1))
    {
      v1->n = pos2;
      v1->vmpbuf_index = -1;
      VMPInsert(v1);
    }
    return;
  }

  if(vmpList.Erase(it1))
  {
    if(vmpList.Erase(it2))
    {
      VMPSwap(*v1, *v2);
      v2->vmpbuf_index = -1;
      VMPInsert(v2);
    }
    v1->vmpbuf_index = -1;
    VMPInsert(v1);
  }
}

void MCUSimpleVideoMixer::VMPDelete(MCUVMPList::shared_iterator & it)
{
  VideoMixPosition *vmp = *it;
  if(vmpList.Erase(it))
  {
    if(vmp->type==1)
      delete vmp;
    else
    {
      vmp->silenceCounter = 0;
      vmp->id = vmp->n;
      VMPInsert(vmp);
    }
  }
}

void MCUSimpleVideoMixer::MyRemoveVideoSource(int pos, BOOL flag)
{
  PWaitAndSignal m(vmpListMutex);
  MCUVMPList::shared_iterator it = VMPFind(pos);
  if(it != vmpList.end())
  {
    if(flag) (*it)->type=1;
    VMPDelete(it);
  }
}

void MCUSimpleVideoMixer::MyRemoveVideoSourceById(ConferenceMemberId id, BOOL flag)
{
  PWaitAndSignal m(vmpListMutex);
  MCUVMPList::shared_iterator it = VMPFind(id);
  if(it != vmpList.end())
  {
    if(flag) (*it)->type=1;
    VMPDelete(it);
  }
}


void MCUSimpleVideoMixer::MyRemoveAllVideoSource()
{
  PWaitAndSignal m(vmpListMutex);
  VMPListClear();
}

PString MCUSimpleVideoMixer::GetFrameStoreMonitorList()
{
  PStringStream s;
  for(VideoFrameStoreList::shared_iterator it = frameStores.frameStoreList.begin(); it != frameStores.frameStoreList.end(); ++it)
  {
    VideoFrameStore *fs = *it;
    s << "  Frame store [" << fs->width << "x" << fs->height << "] "
      << "last read time: " << fs->lastRead << "\n";
  }
  return s;
}

int MCUSimpleVideoMixer::GetMostAppropriateLayout(unsigned n)
{
  int newLayout=-1, maxL=-1;
  unsigned maxV=99999;
  for(unsigned i=0;i<OpenMCU::vmcfg.vmconfs;i++)
  {
    unsigned & vidnum = OpenMCU::vmcfg.vmconf[i].splitcfg.vidnum;
    if(OpenMCU::vmcfg.vmconf[i].splitcfg.mode_mask & 1)
    {
      if(vidnum == n) { newLayout=i; break; }
      else if((vidnum > n)&&(vidnum < maxV))
      {
        maxV = vidnum;
        maxL = (int)i;
      }
    }
  }
  if(newLayout==-1)
  {
    if(maxL!=-1) newLayout=maxL;
    else newLayout=specialLayout;
  }
  return newLayout;
}

///////////////////////////////////////////////////////////////////////////////////////

#if ENABLE_TEST_ROOMS

TestVideoMixer::TestVideoMixer(unsigned _frames)
  : frames(_frames), allocated(FALSE)
{
  forceScreenSplit=TRUE;
  VMPListInit();

  specialLayout=-1;
  int specialLayoutMaxFrames=-1; int maxV=-1;
  int specialLayoutMaxOfMatched=-1; unsigned matchV=99999;
  for(unsigned i=0;i<OpenMCU::vmcfg.vmconfs;i++) if(OpenMCU::vmcfg.vmconf[i].splitcfg.mode_mask&1)
  { if(OpenMCU::vmcfg.vmconf[i].splitcfg.vidnum==_frames) { specialLayout=i; break; }
    else if((OpenMCU::vmcfg.vmconf[i].splitcfg.vidnum>_frames)&&(OpenMCU::vmcfg.vmconf[i].splitcfg.vidnum<matchV))
    { matchV=OpenMCU::vmcfg.vmconf[i].splitcfg.vidnum;  specialLayoutMaxOfMatched=(int)i; }
    else if((int)OpenMCU::vmcfg.vmconf[i].splitcfg.vidnum>maxV)
    { maxV=OpenMCU::vmcfg.vmconf[i].splitcfg.vidnum;  specialLayoutMaxFrames=(int)i; }
  }
  if(specialLayout==-1)
  { if(specialLayoutMaxOfMatched!=-1) specialLayout=specialLayoutMaxOfMatched;
    else if(specialLayoutMaxFrames!=-1) specialLayout=specialLayoutMaxFrames;
    else { PTRACE(1,"TestVideoMixer\tError: could not find the suitable layout"); specialLayout=0; }
  }
  PTRACE(2,"TestVideoMixer\tConstructed, layout id " << specialLayout << ", frames " << frames);
}

BOOL TestVideoMixer::AddVideoSource(ConferenceMemberId id, ConferenceMember & mbr)
{
  PWaitAndSignal m(vmpListMutex);

  if (allocated) return TRUE;
  allocated=TRUE;
  VMPListClear();

  VMPCfgOptions * o;

  unsigned _f = frames; if (_f==0) _f=OpenMCU::vmcfg.vmconf[specialLayout].splitcfg.vidnum;

  for (unsigned i = 0; i < _f; ++i) {
    o = &(OpenMCU::vmcfg.vmconf[specialLayout].vmpcfg[i]);

    VideoMixPosition * newPosition;
    if(i==0)
    { newPosition = CreateVideoMixPosition(id);
      newPosition->type=1;
    }
    else
    { newPosition = CreateVideoMixPosition(i);
      newPosition->type=2;
    }
    PStringStream s; s << "Mix Position " << i; newPosition->SetEndpointName(s);
    newPosition->n=i; 
    VMPInsert(newPosition);
  }

  return TRUE;
}

void TestVideoMixer::MyChangeLayout(unsigned newLayout)
{
  PWaitAndSignal m(vmpListMutex);

  specialLayout=newLayout;

  ConferenceMemberId id = 0;
  MCUVMPList::shared_iterator it = vmpList.begin();
  if(it != vmpList.end())
  {
    id = it->id;
    it.Release();
  }

  VMPListClear();

  VMPCfgOptions * o;

  unsigned _f = frames; if (_f==0) _f=OpenMCU::vmcfg.vmconf[specialLayout].splitcfg.vidnum;

  for (unsigned i = 0; i < _f; ++i)
  {
    o = &(OpenMCU::vmcfg.vmconf[specialLayout].vmpcfg[i]);

    VideoMixPosition * newPosition;
    if(i==0)
    { newPosition = CreateVideoMixPosition(id);
      newPosition->type=1;
    }
    else
    { newPosition = CreateVideoMixPosition(i);
      newPosition->type=2;
    }
    PStringStream s; s << "Mix Position " << i; newPosition->SetEndpointName(s);
    newPosition->n=i; 
    VMPInsert(newPosition);
  }
}

void TestVideoMixer::RemoveVideoSource(ConferenceMemberId id, ConferenceMember & mbr)
{
  MCUVMPList::shared_iterator it = vmpList.begin();
  if(it != vmpList.end() && it->id == id)
    allocated = FALSE;
}

BOOL TestVideoMixer::WriteFrame(ConferenceMemberId id, const void * buffer, int width, int height)
{
  if(vmpList.GetSize() == 0) return FALSE;

  MCUVMPList::shared_iterator it = vmpList.begin();
  if(it->id != id)
  {
    if(allocated) return FALSE;
    else { it->id=id; allocated=TRUE; }
  }

  for(MCUVMPList::shared_iterator it = vmpList.begin(); it != vmpList.end(); ++it)
  {
    VideoMixPosition * vmp = *it;
    WriteSubFrame(*vmp, buffer, width, height, WSF_VMP_COMMON);
  }

  return TRUE;
}

#endif // ENABLE_TEST_ROOMS


#if ENABLE_ECHO_MIXER

EchoVideoMixer::EchoVideoMixer()
  : MCUSimpleVideoMixer()
{
  forceScreenSplit=TRUE;
  VMPListInit();

  specialLayout=-1;
  int specialLayoutMaxFrames=-1; int maxV=-1;
  int specialLayoutMaxOfMatched=-1; unsigned matchV=99999;
  for(unsigned i=0;i<OpenMCU::vmcfg.vmconfs;i++) if(OpenMCU::vmcfg.vmconf[i].splitcfg.mode_mask&1)
  { if(OpenMCU::vmcfg.vmconf[i].splitcfg.vidnum==1) { specialLayout=i; break; }
    else if((OpenMCU::vmcfg.vmconf[i].splitcfg.vidnum>1)&&(OpenMCU::vmcfg.vmconf[i].splitcfg.vidnum<matchV))
    { matchV=OpenMCU::vmcfg.vmconf[i].splitcfg.vidnum;  specialLayoutMaxOfMatched=(int)i; }
    else if((int)OpenMCU::vmcfg.vmconf[i].splitcfg.vidnum>maxV)
    { maxV=OpenMCU::vmcfg.vmconf[i].splitcfg.vidnum;  specialLayoutMaxFrames=(int)i; }
  }
  if(specialLayout==-1)
  { if(specialLayoutMaxOfMatched!=-1) specialLayout=specialLayoutMaxOfMatched;
    else if(specialLayoutMaxFrames!=-1) specialLayout=specialLayoutMaxFrames;
    else { PTRACE(1,"EchoVideoMixer\tError: could not find the suitable layout"); specialLayout=0; }
  }
  PTRACE(6,"EchoVideoMixer\tConstructed, layout id " << specialLayout);
}

BOOL EchoVideoMixer::AddVideoSource(ConferenceMemberId id, ConferenceMember & mbr)
{
  PTRACE(6,"EchoVideoMixer\tAddVideoSource" << flush);
  if(specialLayout<0) return FALSE;
  MCUVMPList::shared_iterator it = vmpList.begin();
  if(it == vmpList.end())
  {
    VideoMixPosition * newPosition;
    newPosition = CreateVideoMixPosition(id);
    it = VMPInsert(newPosition);
  }
  VideoMixPosition *vmp = *it;
  vmp->n=0;
  vmp->id=id;
  vmp->SetEndpointName(mbr.GetName());
  return TRUE;
}

BOOL EchoVideoMixer::WriteFrame(ConferenceMemberId id, const void * buffer, int width, int height)
{
  if(specialLayout<0) return FALSE;
  MCUVMPList::shared_iterator it = vmpList.begin();
  if(it == vmpList.end())
    return FALSE;
  VideoMixPosition *vmp = *it;
  if(vmp->id != id)
    return FALSE;
  WriteSubFrame(*vmp, buffer, width, height, WSF_VMP_COMMON);
  return TRUE;
}

#endif // ENABLE_ECHO_MIXER


VideoMixConfigurator::VideoMixConfigurator(long _w, long _h)
{
  bfw=_w; bfh=_h;
}

VideoMixConfigurator::~VideoMixConfigurator(){
 for(unsigned ii=0;ii<vmconfs;ii++) { //attempt to delete all
  vmconf[ii].vmpcfg=(VMPCfgOptions *)realloc((void *)(vmconf[ii].vmpcfg),0);
  vmconf[ii].vmpcfg=NULL;
 }
 vmconfs=0;
 vmconf=(VMPCfgLayout *)realloc((void *)vmconf,0);
 vmconf=NULL;
}

void VideoMixConfigurator::go(unsigned frame_width, unsigned frame_height)
{
  bfw=frame_width; bfh=frame_height;
  FILE *fs; unsigned long f_size; char * f_buff;
#ifdef SYS_CONFIG_DIR
  fs = fopen(PString(SYS_CONFIG_DIR) + PATH_SEPARATOR + VMPC_CONFIGURATION_NAME,"r");
#else
  fs = fopen(VMPC_CONFIGURATION_NAME,"r");
#endif
  if(!fs) { cout << "Video Mixer Configurator: ERROR! Can't read file \"" << VMPC_CONFIGURATION_NAME << "\"\n"; return; }
  fseek(fs,0L,SEEK_END); f_size=ftell(fs); rewind(fs);
  f_buff=new char[f_size+1]; 
  if(f_size != fread(f_buff,1,f_size,fs)) { cout << "Video Mixer Configurator: ERROR! Can't read file \"" << VMPC_CONFIGURATION_NAME << "\"\n"; return; }
  f_buff[f_size]=0;
  fclose(fs);
  strcpy(fontfile,VMPC_DEFAULT_FONTFILE);
  for(long i=0;i<2;i++){
   fw[i]=bfw; if(fw[i]==0) fw[i]=2;
   fh[i]=bfh; if(fh[i]==0) fh[i]=2;
   strcpy(sopts[i].Id,VMPC_DEFAULT_ID);
   sopts[i].vidnum=VMPC_DEFAULT_VIDNUM;
   sopts[i].mode_mask=VMPC_DEFAULT_MODE_MASK;
   sopts[i].new_from_begin=VMPC_DEFAULT_NEW_FROM_BEGIN;
   sopts[i].reallocate_on_disconnect=VMPC_DEFAULT_REALLOCATE_ON_DISCONNECT;
   sopts[i].mockup_width=VMPC_DEFAULT_MOCKUP_WIDTH;
   sopts[i].mockup_height=VMPC_DEFAULT_MOCKUP_HEIGHT;
   opts[i].posx=VMPC_DEFAULT_POSX;
   opts[i].posy=VMPC_DEFAULT_POSY;
   opts[i].width=VMPC_DEFAULT_WIDTH;
   opts[i].height=VMPC_DEFAULT_HEIGHT;
   opts[i].border=VMPC_DEFAULT_BORDER;
#if USE_FREETYPE
   strcpy(sopts[i].minimum_width_for_label,VMPC_DEFAULT_MINIMUM_WIDTH_FOR_LABEL);
   opts[i].label_mask=VMPC_DEFAULT_LABEL_MASK;
//   strcpy(opts[i].label_text,VMPC_DEFAULT_LABEL_TEXT);
//   opts[i].label_color=VMPC_DEFAULT_LABEL_COLOR;
   opts[i].label_bgcolor=VMPC_DEFAULT_LABEL_BGCOLOR;
   strcpy(opts[i].fontsize,VMPC_DEFAULT_FONTSIZE);
   strcpy(opts[i].border_left,VMPC_DEFAULT_BORDER_LEFT);
   strcpy(opts[i].border_right,VMPC_DEFAULT_BORDER_RIGHT);
   strcpy(opts[i].border_top,VMPC_DEFAULT_BORDER_TOP);
   strcpy(opts[i].border_bottom,VMPC_DEFAULT_BORDER_BOTTOM);
   strcpy(opts[i].h_pad,VMPC_DEFAULT_H_PAD);
   strcpy(opts[i].v_pad,VMPC_DEFAULT_V_PAD);
   strcpy(opts[i].dropshadow_l,VMPC_DEFAULT_SHADOW_L);
   strcpy(opts[i].dropshadow_r,VMPC_DEFAULT_SHADOW_R);
   strcpy(opts[i].dropshadow_t,VMPC_DEFAULT_SHADOW_T);
   strcpy(opts[i].dropshadow_b,VMPC_DEFAULT_SHADOW_B);
   opts[i].cut_before_bracket=VMPC_DEFAULT_CUT_BEFORE_BRACKET;
#endif
  }
  parser(f_buff,f_size);
  delete[] f_buff;
  cout << "VideoMixConfigurator: " << VMPC_CONFIGURATION_NAME << " processed: " << vmconfs << " layout(s) loaded.\n";
}

void VideoMixConfigurator::parser(char* &f_buff,long f_size)
{
  lid=0; ldm=0;
  long pos1,pos=0;
  long line=0;
  bool escape=false;
  while(pos<f_size){
   pos1=pos;
   while(((f_buff[pos1]!='\n')||escape)&&(pos1<f_size)){
    if(escape&&(f_buff[pos1]=='\n')) line++;
    escape=((f_buff[pos1]=='\\')&&(!escape));
    pos1++;
   }
   line++;
   if(pos!=pos1)handle_line(f_buff,pos,pos1,line);
   pos=pos1+1;
  }
  if(ldm==1)finalize_layout_desc();
  geometry();
}

void VideoMixConfigurator::block_insert(VMPBlock * & b,long b_n,unsigned x,unsigned y,unsigned w,unsigned h){
 if(b_n==0) b=(VMPBlock *)malloc(sizeof(VMPBlock));
 else b=(VMPBlock *)realloc((void *)b,sizeof(VMPBlock)*(b_n+1));
 b[b_n].posx=x;
 b[b_n].posy=y;
 b[b_n].width=w;
 b[b_n].height=h;
// cout << "b.ins[" << x << "," << y << "," << w << "," << h << "]\n";
}

void VideoMixConfigurator::block_erase(VMPBlock * & b,long b_i,long b_n){
// cout << "e.bl[" << b_i << "," << b_n << "]\n";
 if(b_n<1) return;
 for(long i=b_i;i<b_n-1;i++)b[i]=b[i+1];
 if(b_n==1) {
  free((void *)b);
  b=NULL;
 }
 else b=(VMPBlock *)realloc((void *)b,sizeof(VMPBlock)*(b_n-1));
}

unsigned VideoMixConfigurator::frame_splitter(VMPBlock * & b,long b_i,long b_n,unsigned x0,unsigned y0,unsigned w0,unsigned h0,unsigned x1,unsigned y1,unsigned w1,unsigned h1){
// cout << "f.spl[" << b_i << "," << b_n << "," << x0 << "," << y0 << "," << w0 << "," << h0 << "," << x1 << "," << y1 << "," << w1 << "," << h1 << "]\n";
 unsigned x00=x0+w0; unsigned y00=y0+h0; unsigned x11=x1+w1; unsigned y11=y1+h1;
 geometry_changed=true;
 if((x1<=x0)&&(y1<=y0)&&(x11>=x00)&&(y11>=y00)){ // no visible blocks
//cout << "[16]";
  block_erase(b,b_i,b_n);
  return (b_n-1);
 }
 if((x1>x0)&&(y1>y0)&&(x11<x00)&&(y11<y00)){ // somewhere inside completely
//cout << "[1]";
  block_erase(b,b_i,b_n);                                     //////
  block_insert(b,b_n-1,x0   ,y0   ,w0         ,y1-y0      );  //  //
  block_insert(b,b_n  ,x0   ,y1   ,x1-x0      ,h1         );  //////
  block_insert(b,b_n+1,x11  ,y1   ,x00-x11    ,h1         );
  block_insert(b,b_n+2,x0   ,y11  ,w0         ,y00-y11    );
  return b_n+3;
 }

 if((x1>x0)&&(x11<x00)&&(y1<=y0)&&(y11>y0)&&(y11<y00)){ // top middle intersection
//cout << "[2]";
  block_erase(b,b_i,b_n);                                     //  //
  block_insert(b,b_n-1,x0   ,y0   ,x1-x0      ,y11-y0     );  //////
  block_insert(b,b_n  ,x1+w1,y0   ,x00-x11    ,y11-y0     );  //////
  block_insert(b,b_n+1,x0   ,y11  ,w0         ,y00-y11    );
  return b_n+2;
 }
 if((x1>x0)&&(x11<x00)&&(y1>y0)&&(y1<y00)&&(y11>=y00)){ // bottom middle intersection
//cout << "[3]";
  block_erase(b,b_i,b_n);                                     //////
  block_insert(b,b_n-1,x0   ,y0   ,w0         ,y1-y0      );  //////
  block_insert(b,b_n  ,x0   ,y1   ,x1-x0      ,y00-y1     );  //  //
  block_insert(b,b_n+1,x1+w1,y1   ,x00-x11    ,y00-y1     );
  return b_n+2;
 }
 if((x1<=x0)&&(x11>x0)&&(x11<x00)&&(y1>y0)&&(y11<y00)){ // middle left intersection
//cout << "[4]";
  block_erase(b,b_i,b_n);                                     //////
  block_insert(b,b_n-1,x0   ,y0   ,w0         ,y1-y0      );    ////
  block_insert(b,b_n  ,x11  ,y1   ,x00-x11    ,h1         );  //////
  block_insert(b,b_n+1,x0   ,y11  ,w0         ,y00-y11    );
  return b_n+2;
 }
 if((x11>=x00)&&(x1<x00)&&(x1>x0)&&(y1>y0)&&(y11<y00)){ // middle right intersection
//cout << "[5]";
  block_erase(b,b_i,b_n);                                     //////
  block_insert(b,b_n-1,x0   ,y0   ,w0         ,y1-y0      );  ////
  block_insert(b,b_n  ,x0   ,y1   ,x1-x0      ,h1         );  //////
  block_insert(b,b_n+1,x0   ,y11  ,w0         ,y00-y11    );
  return b_n+2;
 }
 if((x1<=x0)&&(x11>x0)&&(x11<x00)&&(y1<=y0)&&(y11>y0)&&(y11<y00)){ // top left intersection
//cout << "[6]";
  block_erase(b,b_i,b_n);                                       ////
  block_insert(b,b_n-1,x11  ,y0   ,x00-x11    ,y11-y0     );  //////
  block_insert(b,b_n  ,x0   ,y11  ,w0         ,y00-y11    );  //////
  return b_n+1;
 }
 if((x11>=x00)&&(x1<x00)&&(x1>x0)&&(y1<=y0)&&(y11>y0)&&(y11<y00)){ // top right intersection
//cout << "[7]";
  block_erase(b,b_i,b_n);                                     ////
  block_insert(b,b_n-1,x0   ,y0   ,x1-x0      ,y11-y0     );  //////
  block_insert(b,b_n  ,x0   ,y11  ,w0         ,y00-y11    );  //////
  return b_n+1;
 }
 if((x1<=x0)&&(x11>x0)&&(x11<x00)&&(y11>=y00)&&(y1<y00)&&(y1>y0)){ // bottom left intersection
//cout << "[8]";
  block_erase(b,b_i,b_n);                                     //////
  block_insert(b,b_n-1,x0   ,y0   ,w0         ,y1-y0      );  //////
  block_insert(b,b_n  ,x11  ,y1   ,x00-x11    ,y00-y1     );    ////
  return b_n+1;
 }
 if((x11>=x00)&&(x1<x00)&&(x1>x0)&&(y11>=y00)&&(y1<y00)&&(y1>y0)){ // bottom right intersection
//cout << "[9]";
  block_erase(b,b_i,b_n);                                     //////
  block_insert(b,b_n-1,x0   ,y0   ,w0         ,y1-y0      );  //////
  block_insert(b,b_n  ,x0   ,y1   ,x1-x0      ,y00-y1     );  ////
  return b_n+1;
 }
 if((x1<=x0)&&(x11>=x00)&&(y1<=y0)&&(y11>y0)&&(y11<y00)){ // all-over top intersection
//cout << "[10]";
  block_erase(b,b_i,b_n);
  block_insert(b,b_n-1,x0   ,y11  ,w0         ,y00-y11    );  //////
  return b_n;                                                 //////
 }
 if((x1<=x0)&&(x11>=x00)&&(y11>=y00)&&(y1<y00)&&(y1>y0)){ // all-over bottom intersection
//cout << "[11]";
  block_erase(b,b_i,b_n);                                     //////
  block_insert(b,b_n-1,x0   ,y0   ,w0         ,y1-y0      );  //////
  return b_n;
 }
 if((x1<=x0)&&(x11>x0)&&(x11<x00)&&(y1<=y0)&&(y11>=y00)){ // all-over left intersection
//cout << "[12]";
  block_erase(b,b_i,b_n);                                       ////
  block_insert(b,b_n-1,x11  ,y0   ,x00-x11    ,h0         );    ////
  return b_n;                                                   ////
 }
 if((x11>=x00)&&(x1<x00)&&(x1>x0)&&(y1<=y0)&&(y11>=y00)){ // all-over left intersection
//cout << "[13]";
  block_erase(b,b_i,b_n);                                     ////
  block_insert(b,b_n-1,x0   ,y0   ,x1-x0      ,h0         );  ////
  return b_n;                                                 ////
 }
 if((x1<=x0)&&(x11>=x00)&&(y1>y0)&&(y11<y00)){ // all-over left-right intersection
//cout << "[14]";
  block_erase(b,b_i,b_n);                                     //////
  block_insert(b,b_n-1,x0   ,y0   ,w0         ,y1-y0      );        
  block_insert(b,b_n  ,x0   ,y11  ,w0         ,y00-y11    );  //////
  return b_n+1;
 }
 if((y1<=y0)&&(y11>=y00)&&(x1>x0)&&(x11<x00)){ // all-over top-bottom intersection
//cout << "[15]";
  block_erase(b,b_i,b_n);                                     //  //
  block_insert(b,b_n-1,x0   ,y0   ,x1-x0      ,h0         );  //  //
  block_insert(b,b_n  ,x11  ,y0   ,x00-x11    ,h0         );  //  //
  return b_n+1;
 }
// cout << "[0]";
 geometry_changed=false;
 return b_n;
}

void VideoMixConfigurator::geometry(){ // find and store visible blocks of frames
 for(unsigned i=0;i<vmconfs;i++)
 {
   for(unsigned j=0;j<vmconf[i].splitcfg.vidnum;j++)
   { // Create single block 0 for each position first:
     block_insert(vmconf[i].vmpcfg[j].blk,0,vmconf[i].vmpcfg[j].posx,vmconf[i].vmpcfg[j].posy,vmconf[i].vmpcfg[j].width,vmconf[i].vmpcfg[j].height);
     vmconf[i].vmpcfg[j].blks=1;
//     cout << "*ctrl/i: i=" << i << " j=" << j << " posx=" << vmconf[i].vmpcfg[j].blk[0].posx << "\n";
   }
   for(unsigned j=0;j<vmconf[i].splitcfg.vidnum-1;j++) for (unsigned k=j+1; k<vmconf[i].splitcfg.vidnum;k++)
   {
     unsigned bn=vmconf[i].vmpcfg[j].blks; //remember initial value of blocks
     unsigned b0=0; // block index
     while ((b0<bn)&&(b0<vmconf[i].vmpcfg[j].blks)) {
       unsigned b1=frame_splitter(
         vmconf[i].vmpcfg[j].blk,b0,
         vmconf[i].vmpcfg[j].blks,
         vmconf[i].vmpcfg[j].blk[b0].posx,
         vmconf[i].vmpcfg[j].blk[b0].posy,
         vmconf[i].vmpcfg[j].blk[b0].width,
         vmconf[i].vmpcfg[j].blk[b0].height,
         vmconf[i].vmpcfg[k].blk[0].posx,
         vmconf[i].vmpcfg[k].blk[0].posy,
         vmconf[i].vmpcfg[k].blk[0].width,
         vmconf[i].vmpcfg[k].blk[0].height
       );
//       if(b1==vmconf[i].vmpcfg[j].blks)b0++;
       if(!geometry_changed)b0++;
       else vmconf[i].vmpcfg[j].blks=b1;
     }
   }
 }

}

void VideoMixConfigurator::handle_line(char* &f_buff,long pos,long pos1,long line){
   long i,pos0=pos;
   bool escape=false;
   for(i=pos;i<pos1;i++) {
    escape=((f_buff[i]=='\\')&&(!escape));
    if(!escape){
     if(f_buff[i]=='#') { // comment
      if(pos!=i)handle_atom(f_buff,pos,i,line,pos-pos0);
	  return;
	 }
	 if(f_buff[i]=='/') if(i<pos1-1) if(f_buff[i+1]=='/') { // comment
      if(pos!=i)handle_atom(f_buff,pos,i,line,pos-pos0);
	  return;
	 }
     if(f_buff[i]==';') {
      if(i!=pos)handle_atom(f_buff,pos,i,line,pos-pos0);
      pos=i+1;
     }
    }
   }
   if(pos!=pos1)handle_atom(f_buff,pos,pos1,line,pos-pos0);
  }

void VideoMixConfigurator::handle_atom(char* &f_buff,long pos,long pos1,long line,long lo){
   while((f_buff[pos]<33)&&(pos<pos1)) {pos++; lo++;}
   if(pos==pos1) return; //empty atom
   while(f_buff[pos1-1]<33) pos1--; // atom is now trimed: [pos..pos1-1]
   if(f_buff[pos]=='[') handle_layout_descriptor(f_buff,pos,pos1,line,lo);
   else if(f_buff[pos]=='(') handle_position_descriptor(f_buff,pos,pos1,line,lo);
   else handle_parameter(f_buff,pos,pos1,line,lo);
  }

void VideoMixConfigurator::handle_layout_descriptor(char* &f_buff,long pos,long pos1,long line,long lo){
   if(f_buff[pos1-1]==']'){
    pos++;
    pos1--;
    if(pos1-pos>32) {
     warning(f_buff,line,lo,"too long layout id, truncated",pos,pos+32);
     pos1=pos+32;
    };
    if(ldm==1)finalize_layout_desc();
    if(pos1<=pos) return; //empty layout Id
    initialize_layout_desc(f_buff,pos,pos1,line,lo);
   } else warning(f_buff,line,lo,"bad token",pos,pos1);
//   cout << line << "/" << lo << "\tLAYOUT\t"; for (long i=pos;i<pos1;i++) cout << (char)f_buff[i]; cout << "\n";
  }

void VideoMixConfigurator::handle_position_descriptor(char* &f_buff,long pos,long pos1,long line,long lo){
   if(f_buff[pos1-1]==')'){
    pos++; pos1--;
    while((f_buff[pos]<33)&&(pos<pos1))pos++;
    if(pos==pos1) { warning(f_buff,line,lo,"incomplete position descriptor",1,0); return; }
    if((f_buff[pos]>'9')||(f_buff[pos]<'0')){ warning(f_buff,line,lo,"error in position description",pos,pos1); return; }
    long pos0=pos;
    while((pos<pos1)&&(f_buff[pos]>='0')&&(f_buff[pos]<='9'))pos++;
    long pos2=pos;
    while((pos<pos1)&&(f_buff[pos]<33))pos++;
    if(f_buff[pos]!=','){ warning(f_buff,line,lo,"unknown character in position descriptor",pos,pos1); return; }
    char t=f_buff[pos2];
    f_buff[pos2]=0;
    opts[1].posx=(atoi((const char*)(f_buff+pos0))*bfw)/fw[1];
//    opts[1].posx=((atoi((const char*)(f_buff+pos0))*bfw)/fw[1]+1)&0xFFFFFE;
    f_buff[pos2]=t;
    pos++;
    if(pos>=pos1){ warning(f_buff,line,lo,"Y-coordinate does not set",1,0); return; }
    while((f_buff[pos]<33)&&(pos<pos1))pos++;
    if(pos==pos1) { warning(f_buff,line,lo,"Y not set",1,0); return; }
    if((f_buff[pos]>'9')||(f_buff[pos]<'0')){ warning(f_buff,line,lo,"error in Y crd. description",pos,pos1); return; }
    pos0=pos;
    while((pos<pos1)&&(f_buff[pos]>='0')&&(f_buff[pos]<='9'))pos++;
    pos2=pos;
    while((pos<pos1)&&(f_buff[pos]<33))pos++;
    if(pos!=pos1){ warning(f_buff,line,lo,"unknown chars in position descriptor",pos,pos1); return; }
    t=f_buff[pos2];
    f_buff[pos2]=0;
    opts[1].posy=(atoi((const char*)(f_buff+pos0))*bfh)/fh[1];
//    opts[1].posy=((atoi((const char*)(f_buff+pos0))*bfh)/fh[1]+1)&0xFFFFFE;
    f_buff[pos2]=t;
   } else warning(f_buff,line,lo,"bad position descriptor",pos,pos1);
   if(pos_n==0)vmconf[lid].vmpcfg=(VMPCfgOptions *)malloc(sizeof(VMPCfgOptions));
   else vmconf[lid].vmpcfg=(VMPCfgOptions *)realloc((void *)(vmconf[lid].vmpcfg),sizeof(VMPCfgOptions)*(pos_n+1));
   vmconf[lid].vmpcfg[pos_n]=opts[1];
//   cout << "Position " << pos_n << ": " << opts[true].posx << "," << opts[true].posy << "\n";
   pos_n++;
  }

void VideoMixConfigurator::handle_parameter(char* &f_buff,long pos,long pos1,long line,long lo){
   char p[64], v[256];
   bool escape=false;
   long pos0=pos;
   long pos00=pos;
   while((pos<pos1)&&(((f_buff[pos]>32)&&(f_buff[pos]!='='))||escape)&&(pos-pos0<63)){
    p[pos-pos0]=f_buff[pos];
    pos++;
   }
   p[pos-pos0]=0;
   if(pos-pos0>63){ warning(f_buff,line,lo,"parameter name is too long",pos0,pos1); return; }
   while((pos<pos1)&&(f_buff[pos]<33))pos++;
   if(pos==pos1){ warning(f_buff,line,lo,"unknown text",pos0,pos1); return; }
   if(f_buff[pos]!='='){ warning(f_buff,line,lo,"missing \"=\"",pos0,pos1); return; }
   pos++;
   while((pos<pos1)&&(f_buff[pos]<33))pos++;
   escape=false;
   pos0=pos;
   while((pos<pos1)&&((f_buff[pos]>31)||escape)&&(pos-pos0<255)){
    v[pos-pos0]=f_buff[pos];
    pos++;
   }
   v[pos-pos0]=0;
   if(pos-pos0>255){ warning(f_buff,line,lo,"parameter value is too long",pos0,pos1); return; }
   while((pos<pos1)&&(f_buff[pos]<33))pos++;
   if(pos!=pos1) warning(f_buff,line,lo,"unknown characters",pos,pos1);
   option_set((const char *)p,(const char *)v, f_buff, line, lo, pos00, pos0);
  }

void VideoMixConfigurator::option_set(const char* p, const char* v, char* &f_buff, long line, long lo, long pos, long pos1){
   if(option_cmp((const char *)p,(const char *)"frame_width")){fw[ldm]=atoi(v);if(fw[ldm]<1)fw[ldm]=1;}
   else if(option_cmp((const char *)p,(const char *)"frame_height")){fh[ldm]=atoi(v);if(fh[ldm]<1)fh[ldm]=1;}
   else if(option_cmp((const char *)p,(const char *)"border"))opts[ldm].border=atoi(v);
   else if(option_cmp((const char *)p,(const char *)"mode_mask"))sopts[ldm].mode_mask=atoi(v);
//   else if(option_cmp((const char *)p,(const char *)"tags")){
//     if(strlen(v)<=128)strcpy(sopts[ldm].tags,v); else 
//     warning(f_buff,line,lo,"tags value too long (max 128 chars allowed)",pos,pos1);
//   }
   else if(option_cmp((const char *)p,(const char *)"position_width")) opts[ldm].width=(atoi(v)*bfw)/fw[ldm];
//   else if(option_cmp((const char *)p,(const char *)"position_width")) opts[ldm].width=((atoi(v)*bfw)/fw[ldm]+1)&0xFFFFFE;
   else if(option_cmp((const char *)p,(const char *)"position_height")) opts[ldm].height=(atoi(v)*bfh)/fh[ldm];
//   else if(option_cmp((const char *)p,(const char *)"position_height")) opts[ldm].height=((atoi(v)*bfh)/fh[ldm]+1)&0xFFFFFE;
#if USE_FREETYPE
   else if(option_cmp((const char *)p,(const char *)"label_mask")) opts[ldm].label_mask=atoi(v);
/*   else if(option_cmp((const char *)p,(const char *)"label_color")) {
    int tempc=0xFFFFFF; sscanf(v,"%x",&tempc); int R=(tempc>>16)&255; int G=(tempc>>8)&255; int B=tempc&255;
    int U = (BYTE)PMIN(ABS(R * -1214 + G * -2384 + B * 3598 + 4096 + 1048576) / 8192, 240);
    int V = (BYTE)PMIN(ABS(R *  3598 + G * -3013 + B * -585 + 4096 + 1048576) / 8192, 240);
    opts[ldm].label_color=(U<<8)+V;
   }
*/
   else if(option_cmp((const char *)p,(const char *)"label_bgcolor")) {
    int tempc=0xFFFFFF; sscanf(v,"%x",&tempc); int R=(tempc>>16)&255; int G=(tempc>>8)&255; int B=tempc&255;
    int U = (BYTE)PMIN(ABS(R * -1214 + G * -2384 + B * 3598 + 4096 + 1048576) / 8192, 240);
    int V = (BYTE)PMIN(ABS(R *  3598 + G * -3013 + B * -585 + 4096 + 1048576) / 8192, 240);
    opts[ldm].label_bgcolor=(U<<8)+V;
   }
   else if(option_cmp((const char *)p,(const char *)"font")) {
    if(strlen(v)<256)strcpy(fontfile,v); else 
    warning(f_buff,line,lo,"fonts value too long (max 255 chars allowed)",pos,pos1);
   }
   else if(option_cmp((const char *)p,(const char *)"fontsize")) {
    if(strlen(v)<11)strcpy(opts[ldm].fontsize,v); else 
    warning(f_buff,line,lo,"fontsize value too long (max 10 chars allowed)",pos,pos1);
   }
   else if(option_cmp((const char *)p,(const char *)"border_left")) {
    if(strlen(v)<11)strcpy(opts[ldm].border_left,v); else 
    warning(f_buff,line,lo,"border_left value too long (max 10 chars allowed)",pos,pos1);
   }
   else if(option_cmp((const char *)p,(const char *)"border_right")) {
    if(strlen(v)<11)strcpy(opts[ldm].border_right,v); else 
    warning(f_buff,line,lo,"border_right value too long (max 10 chars allowed)",pos,pos1);
   }
   else if(option_cmp((const char *)p,(const char *)"border_top")) {
    if(strlen(v)<11)strcpy(opts[ldm].border_top,v); else 
    warning(f_buff,line,lo,"border_top value too long (max 10 chars allowed)",pos,pos1);
   }
   else if(option_cmp((const char *)p,(const char *)"border_bottom")) {
    if(strlen(v)<11)strcpy(opts[ldm].border_bottom,v); else 
    warning(f_buff,line,lo,"border_bottom value too long (max 10 chars allowed)",pos,pos1);
   }
   else if(option_cmp((const char *)p,(const char *)"h_pad")) {
    if(strlen(v)<11)strcpy(opts[ldm].h_pad,v); else 
    warning(f_buff,line,lo,"h_pad value too long (max 10 chars allowed)",pos,pos1);
   }
   else if(option_cmp((const char *)p,(const char *)"v_pad")) {
    if(strlen(v)<11)strcpy(opts[ldm].v_pad,v); else 
    warning(f_buff,line,lo,"v_pad value too long (max 10 chars allowed)",pos,pos1);
   }
   else if(option_cmp((const char *)p,(const char *)"dropshadow_l")) {
    if(strlen(v)<11)strcpy(opts[ldm].dropshadow_l,v); else 
    warning(f_buff,line,lo,"dropshadow_l value too long (max 10 chars allowed)",pos,pos1);
   }
   else if(option_cmp((const char *)p,(const char *)"dropshadow_r")) {
    if(strlen(v)<11)strcpy(opts[ldm].dropshadow_r,v); else 
    warning(f_buff,line,lo,"dropshadow_r value too long (max 10 chars allowed)",pos,pos1);
   }
   else if(option_cmp((const char *)p,(const char *)"dropshadow_t")) {
    if(strlen(v)<11)strcpy(opts[ldm].dropshadow_t,v); else 
    warning(f_buff,line,lo,"dropshadow_t value too long (max 10 chars allowed)",pos,pos1);
   }
   else if(option_cmp((const char *)p,(const char *)"dropshadow_b")) {
    if(strlen(v)<11)strcpy(opts[ldm].dropshadow_b,v); else 
    warning(f_buff,line,lo,"dropshadow_b value too long (max 10 chars allowed)",pos,pos1);
   }
   else if(option_cmp((const char *)p,(const char *)"cut_before_bracket")) opts[ldm].cut_before_bracket=atoi(v);
#endif
   else if(option_cmp((const char *)p,(const char *)"reallocate_on_disconnect")) sopts[ldm].reallocate_on_disconnect=atoi(v);
   else if(option_cmp((const char *)p,(const char *)"new_members_first")) sopts[ldm].new_from_begin=atoi(v);
   else if(option_cmp((const char *)p,(const char *)"mockup_width")) sopts[ldm].mockup_width=atoi(v);
   else if(option_cmp((const char *)p,(const char *)"mockup_height")) sopts[ldm].mockup_height=atoi(v);
   else if(option_cmp((const char *)p,(const char *)"minimum_width_for_label"))
   { if(strlen(v)<11) strcpy(sopts[ldm].minimum_width_for_label,v);
     else warning(f_buff,line,lo,"minimum_width_for_label value too long (max 10 chars allowed)",pos,pos1);
   }
   else warning(f_buff,line,lo,"unknown parameter",pos,pos1);
  }

bool VideoMixConfigurator::option_cmp(const char* p,const char* str){
   if(strlen(p)!=strlen(str))return false;
   for(unsigned i=0;i<strlen(str);i++)if(p[i]!=str[i]) return false;
   return true;
  }

void VideoMixConfigurator::warning(char* &f_buff,long line,long lo,const char warn[64],long pos,long pos1)
{
  PStringStream w;
  w << "Warning! " << VMPC_CONFIGURATION_NAME << ":" << line << ":" << lo << ": "<< warn;
  if(pos1>pos)
  {
    w << ": \"";
    for(long i=pos;i<pos1;i++) w << (char)f_buff[i];
    w << "\"";
  }
  cout << w << "\n";
  PTRACE(1, w);
}

void VideoMixConfigurator::initialize_layout_desc(char* &f_buff,long pos,long pos1,long line,long lo){
   ldm=1;
   pos_n=0;
   opts[1]=opts[0]; sopts[1]=sopts[0]; fw[1]=fw[0]; fh[1]=fh[0];
   f_buff[pos1]=0;
//cout << " memcpy " << (pos1-pos+1) << "\n" << sopts[true].Id << "\n" << f_buff[pos] << "\n";
   strcpy((char *)(sopts[1].Id),(char *)(f_buff+pos));
   f_buff[pos1]=']';
   if(lid==0)vmconf=(VMPCfgLayout *)malloc(sizeof(VMPCfgLayout));
   else vmconf=(VMPCfgLayout *)realloc((void *)vmconf,sizeof(VMPCfgLayout)*(lid+1));
   vmconf[lid].vmpcfg=NULL;
  }

void VideoMixConfigurator::finalize_layout_desc(){
   ldm=0;
   vmconf[lid].splitcfg=sopts[1];
   vmconf[lid].splitcfg.vidnum=pos_n;
   lid++;
   vmconfs=lid;
  }

#endif // MCU_VIDEO

