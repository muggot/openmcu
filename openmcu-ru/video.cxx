
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
  cout << "SetFrameSize " << width << " " << height << "\n";
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
    FrameStore *vf = *it;
    if(frameStoreList.Erase(it))
      delete vf;
  }
}

VideoFrameStoreList::shared_iterator VideoFrameStoreList::GetFrameStore(int width, int height) 
{
  shared_iterator it = frameStoreList.Find(WidthHeightToKey(width, height));
  if(it != frameStoreList.end())
    return it;
  FrameStore * vf = new FrameStore(width, height);
  return frameStoreList.Insert(vf, WidthHeightToKey(width, height));
}

VideoFrameStoreList::FrameStore::FrameStore(int _w, int _h)
  : width(_w), height(_h), frame_size(_w*_h*3/2)
{
  PAssert(_w != 0 && _h != 0, "Cannot create zero size framestore");
  bgframe.SetSize(frame_size);
  unsigned bgw, bgh;
  void *bg = OpenMCU::Current().GetBackgroundPointer(bgw, bgh);
  if(bg)
    MCUVideoMixer::ResizeYUV420P(bg, bgframe.GetPointer(), bgw, bgh, width, height);
  else
    MCUVideoMixer::FillYUVFrame(bgframe.GetPointer(), 0, 0, 0, width, height);
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


static inline int ABS(int v)
{  return (v >= 0) ? v : -v; }

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
}

VideoMixPosition::~VideoMixPosition()
{
#if USE_FREETYPE
  for(MCUSubtitlesList::shared_iterator it = subtitlesList.begin(); it != subtitlesList.end(); ++it)
  {
    MCUSubtitles *sub = *it;
    if(subtitlesList.Erase(it))
      delete sub;
  }
#endif
}

#if USE_FREETYPE
unsigned MCUSimpleVideoMixer::printsubs_calc(unsigned v, char s[10])
{ int slashpos=-1; char s2[10];
  for(int i=0;i<10;i++){ s2[i]=s[i]; if(s[i]==0) break; if(s[i]=='/') slashpos=i; }
  if (slashpos==-1) return atoi(s);
  s2[slashpos]=0; unsigned mul=atoi(s2);
  for(int i=slashpos+1;i<10;i++) s2[i-slashpos-1]=s[i];
  s2[9-slashpos]=0; unsigned div=atoi(s2);
  if(div>0) return v*mul/div;
  PTRACE(1,"FreeType\tprintsubs_calc() DIVISION BY ZERO: " << v << "*" << mul << "/" << div);
  return 1;
}

void MCUSimpleVideoMixer::RemoveSubtitles(VideoMixPosition & vmp)
{
  for(VideoMixPosition::MCUSubtitlesList::shared_iterator it = vmp.subtitlesList.begin(); it != vmp.subtitlesList.end(); ++it)
  {
    MCUSubtitles *sub = *it;
    if(vmp.subtitlesList.Erase(it))
      delete sub;
  }
}

void MCUSimpleVideoMixer::DeleteSubtitlesByFS(unsigned w, unsigned h)
{
  for(MCUVMPList::shared_iterator it = vmpList.begin(); it != vmpList.end(); ++it)
  {
    VideoMixPosition *vmp = *it;
    VMPCfgOptions & vmpcfg=OpenMCU::vmcfg.vmconf[specialLayout].vmpcfg[vmp->n];
    unsigned pw = vmpcfg.width *w/CIF4_WIDTH;
    unsigned ph = vmpcfg.height*h/CIF4_HEIGHT;
    unsigned key=(ph<<16) | pw;
    VideoMixPosition::MCUSubtitlesList::shared_iterator q = vmp->subtitlesList.Find(key);
    if(q != vmp->subtitlesList.end())
    {
      MCUSubtitles *sub = *q;
      if(vmp->subtitlesList.Erase(q))
        delete sub;
    }
  }
}

void MCUSimpleVideoMixer::PrintSubtitles(VideoMixPosition & vmp, void * buffer, unsigned int fw, unsigned int fh, unsigned int ft_properties)
{
  MCUSubtitles * st;

  unsigned key=(fh << 16) | fw;
  VideoMixPosition::MCUSubtitlesList::shared_iterator q = vmp.subtitlesList.Find(key);

  if(q == vmp.subtitlesList.end())
  {
    st = RenderSubtitles(key, vmp, buffer, fw, fh, ft_properties);
    if(st!=NULL) q = vmp.subtitlesList.Insert(st, key);
  }
  else
  {
    st = *q;
  }

  if(st == NULL) return;

  if(!(st->w)) return;

  VMPCfgOptions & vmpcfg = OpenMCU::vmcfg.vmconf[specialLayout].vmpcfg[vmp.n];

  if(ft_properties & FT_P_SUBTITLES) MixRectIntoFrameSubsMode(st->b,(BYTE *)buffer,st->x,st->y,st->w,st->h,fw,fh,0);

  if(ft_properties & FT_P_TRANSPARENT) MixRectIntoFrameGrayscale(st->b,(BYTE *)buffer,st->x,st->y,st->w,st->h,fw,fh,1);

  if(ft_properties & FT_P_TRANSPARENT)
  {
//    MixRectIntoFrameGrayscale(st->b.GetPointer(),(BYTE *)buffer,st->x,st->y,st->w,st->h,fw,fh,1);
    ReplaceUV_Rect((BYTE *)buffer,fw,fh,vmpcfg.label_bgcolor>>8,vmpcfg.label_bgcolor&0xFF,0,st->y,fw,st->h);
  }

//  if(ft_properties & FT_P_SUBTITLES) MixRectIntoFrameSubsMode(st->b.GetPointer(),(BYTE *)buffer,st->x,st->y,st->w,st->h,fw,fh,0);

  if(!(ft_properties & (FT_P_SUBTITLES + FT_P_TRANSPARENT))) CopyRectIntoFrame(st->b,(BYTE *)buffer,st->x,st->y,st->w,st->h,fw,fh);

}

void MCUSimpleVideoMixer::InitializeSubtitles()
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
  else
  {
    PTRACE(3,"FreeType\tCould not load truetype font: " << OpenMCU::vmcfg.fontfile);
  }
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


MCUSubtitles * MCUSimpleVideoMixer::RenderSubtitles(unsigned key, VideoMixPosition & vmp, void * buffer, unsigned fw, unsigned fh, unsigned ft_properties)
{

  MCUSubtitles * st = new MCUSubtitles;
  st->w = 0;
  st->b = NULL;

  VMPCfgSplitOptions & split = OpenMCU::vmcfg.vmconf[specialLayout].splitcfg;
  VMPCfgOptions & vmpcfg = OpenMCU::vmcfg.vmconf[specialLayout].vmpcfg[vmp.n];

  if((fw < 2) || (fh < 2)) return st;

  if(fw < printsubs_calc(fw * OpenMCU::vmcfg.bfw / vmpcfg.width, split.minimum_width_for_label)) return st;

  if(ft_error==FT_INITIAL_ERROR) InitializeSubtitles();
  if(ft_error) return st; // Stop using freetype on fail

  unsigned bl = printsubs_calc (fw, vmpcfg.border_left  ),
           br = printsubs_calc (fw, vmpcfg.border_right ),
           bt = printsubs_calc (fh, vmpcfg.border_top   ),
           bb = printsubs_calc (fh, vmpcfg.border_bottom),
           hp = printsubs_calc (fw, vmpcfg.h_pad        ),
           vp = printsubs_calc (fh, vmpcfg.v_pad        ),
          dsl = printsubs_calc (fw, vmpcfg.dropshadow_l ),
          dsr = printsubs_calc (fw, vmpcfg.dropshadow_r ),
          dst = printsubs_calc (fh, vmpcfg.dropshadow_t ),
          dsb = printsubs_calc (fh, vmpcfg.dropshadow_b ),
  fontsizepix = printsubs_calc (fh, vmpcfg.fontsize)     ;

  if(fontsizepix < 4) return st;

  int wi = (int)fw-(hp<<1)-bl-br;
  int hi = (int)fh-(vp<<1)-bt-bb;
  if((wi<2)||(hi<2)) return st;
  unsigned w = (unsigned)wi;
  unsigned h = (unsigned)hi;

  if((ft_error = FT_Set_Pixel_Sizes(ft_face,0,fontsizepix))) return st;

  PString & s = vmp.endpointName;
  PINDEX len = s.GetLength();
  if(len==0) return st;

  struct MyBMP{ PBYTEArray *bmp; int l, t, w, h, x; };
  MyBMP *bmps=NULL;
  PINDEX slotCounter=0;

  unsigned pen_x = 0, pen_y=0;
  unsigned pen_x_max = pen_x, c, c2, ft_previous = 0;
  unsigned hMax=0;
  for(PINDEX i=0;i<len;++i)
  {
    c = (BYTE)s[i]; if(i<len-1)c2=(BYTE)s[i+1]; else c2=0;
    if(vmpcfg.cut_before_bracket) if((c==' ') && ((c2=='[')||(c2=='('))) break;
    if     (!(c&128))                 {/* 0xxxxxxx */ } // utf-8 -> unicode
    else if(((c&224)==192)&&(i+1<len)){/* 110__ 10__ */ c = ((c&31)<<6) + (c2&63); i++; }
    else if(((c&240)==224)&&(i+2<len)){/* 1110__ 10__ 10__ */ c = ((c&15)<<12) + ((c2&63)<<6) + ((BYTE)s[i+2]&63); i+=2; }
    else if(((c&248)==240)&&(i+3<len)){/* 11110__ 10__ 10__ 10__ */ c = ((c&7)<<18) + ((c2&63)<<12) + (((unsigned)((BYTE)s[i+2]&63))<<6) + ((BYTE)s[i+3]&63); i+=3; }

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
 if(x < 0 || y < 0) continue;     // ‚ÓÚ ÓÌÓ
 if(x + bmps[i].w > (int)lw) continue; // ˝ÚÓ ˇ Ú‡Í, ‰ÓÎÊÌÓ ·˚Ú¸ ÏÂÌ¸¯Â ???

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

void MCUVideoMixer::ConvertRGBToYUV(BYTE R, BYTE G, BYTE B, BYTE & Y, BYTE & U, BYTE & V)
{
  Y = (BYTE)PMIN(ABS(R *  2104 + G *  4130 + B *  802 + 4096 +  131072) / 8192, 235);
  U = (BYTE)PMIN(ABS(R * -1214 + G * -2384 + B * 3598 + 4096 + 1048576) / 8192, 240);
  V = (BYTE)PMIN(ABS(R *  3598 + G * -3013 + B * -585 + 4096 + 1048576) / 8192, 240);
}

void MCUVideoMixer::FillYUVFrame(void * buffer, BYTE R, BYTE G, BYTE B, int w, int h)
{
  BYTE Y, U, V;
  ConvertRGBToYUV(R, G, B, Y, U, V);

  const int ysize = w*h;
  const int usize = (w>>1)*(h>>1);
  const int vsize = usize;

  memset((BYTE *)buffer + 0,             Y, ysize);
  memset((BYTE *)buffer + ysize,         U, usize);
  memset((BYTE *)buffer + ysize + usize, V, vsize);
}

void MCUVideoMixer::FillYUVFrame_YUV(void * buffer, BYTE Y, BYTE U, BYTE V, int w, int h)
{
  const int ysize = w*h;
  const int usize = (w/2)*(h/2);
  const int vsize = usize;

  memset((BYTE *)buffer + 0,             Y, ysize);
  memset((BYTE *)buffer + ysize,         U, usize);
  memset((BYTE *)buffer + ysize + usize, V, vsize);
}

void MCUVideoMixer::FillCIFYUVFrame(void * buffer, BYTE R, BYTE G, BYTE B)
{
  FillYUVFrame(buffer, R, G, B, CIF_WIDTH, CIF_HEIGHT);
}

void MCUVideoMixer::FillCIF4YUVFrame(void * buffer, BYTE R, BYTE G, BYTE B)
{
  FillYUVFrame(buffer, R, G, B, CIF4_WIDTH, CIF4_HEIGHT);
}

void MCUVideoMixer::FillCIF16YUVFrame(void * buffer, BYTE R, BYTE G, BYTE B)
{
  FillYUVFrame(buffer, R, G, B, CIF16_WIDTH, CIF16_HEIGHT);
}

void MCUVideoMixer::FillQCIFYUVFrame(void * buffer, BYTE R, BYTE G, BYTE B)
{
  FillYUVFrame(buffer, R, G, B, QCIF_WIDTH, QCIF_HEIGHT);
}

void MCUVideoMixer::FillCIFYUVRect(void * frame, BYTE R, BYTE G, BYTE B, int xPos, int yPos, int rectWidth, int rectHeight)
{
  FillYUVRect(frame, CIF_WIDTH, CIF_HEIGHT, R, G, B, xPos, yPos, rectWidth, rectHeight);
}

void MCUVideoMixer::FillCIF4YUVRect(void * frame, BYTE R, BYTE G, BYTE B, int xPos, int yPos, int rectWidth, int rectHeight)
{
  FillYUVRect(frame, CIF4_WIDTH, CIF4_HEIGHT, R, G, B, xPos, yPos, rectWidth, rectHeight);
}

void MCUVideoMixer::FillCIF16YUVRect(void * frame, BYTE R, BYTE G, BYTE B, int xPos, int yPos, int rectWidth, int rectHeight)
{
  FillYUVRect(frame, CIF16_WIDTH, CIF16_HEIGHT, R, G, B, xPos, yPos, rectWidth, rectHeight);
}

void MCUVideoMixer::FillYUVRect(void * frame, int frameWidth, int frameHeight, BYTE R, BYTE G, BYTE B, int xPos, int yPos, int rectWidth, int rectHeight)
{
  //This routine fills a region of the video image with data. It is used as the central
  //point because one only has to add other image formats here.

  int offset       = ( yPos * frameWidth ) + xPos;
  int colourOffset = ((yPos>>1) * (frameWidth>>1)) + (xPos >> 1);

  BYTE Y, U, V;
  ConvertRGBToYUV(R, G, B, Y, U, V);

  BYTE * Yptr = (BYTE*)frame + offset;
  BYTE * UPtr = (BYTE*)frame + (frameWidth * frameHeight) + colourOffset;
  BYTE * VPtr = (BYTE*)frame + (frameWidth * frameHeight) + ((frameWidth/2) * (frameHeight/2))  + colourOffset;

  int rr ;
  int halfRectWidth = rectWidth >> 1;
  int halfWidth     = frameWidth >> 1;
  
  for (rr = 0; rr < rectHeight;rr+=2) {
    memset(Yptr, Y, rectWidth);
    Yptr += frameWidth;
    memset(Yptr, Y, rectWidth);
    Yptr += frameWidth;

    memset(UPtr, U, halfRectWidth);
    memset(VPtr, V, halfRectWidth);

    UPtr += halfWidth;
    VPtr += halfWidth;
  }
}

void MCUVideoMixer::ReplaceUV_Rect(void * frame, int frameWidth, int frameHeight, BYTE U, BYTE V, int xPos, int yPos, int rectWidth, int rectHeight)
{
  unsigned int cw=frameWidth>>1;
  unsigned int ch=frameHeight>>1;
  unsigned int rcw=rectWidth>>1;
  unsigned int rch=rectHeight>>1;
  unsigned int offsetUV=(yPos>>1)*cw+(xPos>>1);
  unsigned int offsetU=frameWidth*frameHeight+offsetUV;
  unsigned int offsetV=cw*ch+offsetU;
  BYTE * UPtr = (BYTE*)frame + offsetU;
  BYTE * VPtr = (BYTE*)frame + offsetV;
  for (unsigned int rr=0;rr<rch;rr++) {
    memset(UPtr, U, rcw);
    memset(VPtr, V, rcw);
    UPtr += cw;
    VPtr += cw;
  }
}

void MCUVideoMixer::CopyRectIntoQCIF(const void * _src, void * _dst, int xpos, int ypos, int width, int height)
{
  BYTE * src = (BYTE *)_src;
  BYTE * dst = (BYTE *)_dst + (ypos * QCIF_WIDTH) + xpos;

  BYTE * dstEnd = dst + QCIF_SIZE;
  int y;

  // copy Y
  for (y = 0; y < height; ++y) {
    PAssert(dst + width < dstEnd, "Y write overflow");
    memcpy(dst, src, width);
    src += width;
    dst += QCIF_WIDTH;
  }

  // copy U
  dst = (BYTE *)_dst + (QCIF_WIDTH * QCIF_HEIGHT) + (ypos * CIF_WIDTH/4) + xpos / 2;
  for (y = 0; y < height/2; ++y) {
    PAssert(dst + width/2 <= dstEnd, "U write overflow");
    memcpy(dst, src, width/2);
    src += width/2;
    dst += QCIF_WIDTH/2;
  }

  // copy V
  dst = (BYTE *)_dst + (QCIF_WIDTH * QCIF_HEIGHT) + (QCIF_WIDTH * QCIF_HEIGHT) / 4 + (ypos * QCIF_WIDTH/4) + xpos / 2;
  for (y = 0; y < height/2; ++y) {
    PAssert(dst + width/2 <= dstEnd, "V write overflow");
    memcpy(dst, src, width/2);
    src += width/2;
    dst += QCIF_WIDTH/2;
  }
}

void MCUVideoMixer::CopyRectIntoCIF(const void * _src, void * _dst, int xpos, int ypos, int width, int height)
{
  BYTE * src = (BYTE *)_src;
  BYTE * dst = (BYTE *)_dst + (ypos * CIF_WIDTH) + xpos;

  BYTE * dstEnd = dst + CIF_SIZE;
  int y;

  // copy Y
  for (y = 0; y < height; ++y) {
    PAssert(dst + width < dstEnd, "Y write overflow");
    memcpy(dst, src, width);
    src += width;
    dst += CIF_WIDTH;
  }

  // copy U
  dst = (BYTE *)_dst + (CIF_WIDTH * CIF_HEIGHT) + (ypos * CIF_WIDTH/4) + xpos / 2;
  for (y = 0; y < height/2; ++y) {
    PAssert(dst + width/2 <= dstEnd, "U write overflow");
    memcpy(dst, src, width/2);
    src += width/2;
    dst += CIF_WIDTH/2;
  }

  // copy V
  dst = (BYTE *)_dst + (CIF_WIDTH * CIF_HEIGHT) + (CIF_WIDTH * CIF_HEIGHT) / 4 + (ypos * CIF_WIDTH/4) + xpos / 2;
  for (y = 0; y < height/2; ++y) {
    PAssert(dst + width/2 <= dstEnd, "V write overflow");
    memcpy(dst, src, width/2);
    src += width/2;
    dst += CIF_WIDTH/2;
  }
}

void MCUVideoMixer::CopyRectIntoCIF4(const void * _src, void * _dst, int xpos, int ypos, int width, int height)
{
  BYTE * src = (BYTE *)_src;
  BYTE * dst = (BYTE *)_dst + (ypos * CIF4_WIDTH) + xpos;

  BYTE * dstEnd = dst + CIF4_SIZE;
  int y;

  // copy Y
  for (y = 0; y < height; ++y) {
    PAssert(dst + width < dstEnd, "Y write overflow");
    memcpy(dst, src, width);
    src += width;
    dst += CIF4_WIDTH;
  }

  // copy U
  dst = (BYTE *)_dst + (CIF4_WIDTH * CIF4_HEIGHT) + (ypos * CIF4_WIDTH/4) + xpos / 2;
  for (y = 0; y < height/2; ++y) {
    PAssert(dst + width/2 <= dstEnd, "U write overflow");
    memcpy(dst, src, width/2);
    src += width/2;
    dst += CIF4_WIDTH/2;
  }

  // copy V
  dst = (BYTE *)_dst + (CIF4_WIDTH * CIF4_HEIGHT) + (CIF4_WIDTH * CIF4_HEIGHT) / 4 + (ypos * CIF4_WIDTH/4) + xpos / 2;
  for (y = 0; y < height/2; ++y) {
    PAssert(dst + width/2 <= dstEnd, "V write overflow");
    memcpy(dst, src, width/2);
    src += width/2;
    dst += CIF4_WIDTH/2;
  }
}

void MCUVideoMixer::CopyGrayscaleIntoCIF(const void * _src, void * _dst, int xpos, int ypos, int width, int height)
{
  BYTE * src = (BYTE *)_src;
  BYTE * dst = (BYTE *)_dst + (ypos * CIF_WIDTH) + xpos;
  int y;

  for (y=0;y<height;++y){
   memcpy(dst,src,width);
   src+=width;
   dst+=CIF_WIDTH;
  }
}

void MCUVideoMixer::CopyGrayscaleIntoCIF4(const void * _src, void * _dst, int xpos, int ypos, int width, int height)
{
  BYTE * src = (BYTE *)_src;
  BYTE * dst = (BYTE *)_dst + (ypos * CIF4_WIDTH) + xpos;
  int y;

  for (y=0;y<height;++y){
   memcpy(dst,src,width);
   src+=width;
   dst+=CIF4_WIDTH;
  }
}

void MCUVideoMixer::CopyGrayscaleIntoCIF16(const void * _src, void * _dst, int xpos, int ypos, int width, int height)
{
  BYTE * src = (BYTE *)_src;
  BYTE * dst = (BYTE *)_dst + (ypos * CIF16_WIDTH) + xpos;
  int y;

  for (y=0;y<height;++y){
   memcpy(dst,src,width);
   src+=width;
   dst+=CIF16_WIDTH;
  }
}

void MCUVideoMixer::CopyGrayscaleIntoFrame(const void * _src, void * _dst, int xpos, int ypos, int width, int height, int fw, int fh)
{
  BYTE * src = (BYTE *)_src;
  BYTE * dst = (BYTE *)_dst + (ypos * fw) + xpos;
  int y;

  for (y=0;y<height;++y){
   memcpy(dst,src,width);
   src+=width;
   dst+=fw;
  }
}

void MCUVideoMixer::CopyRectIntoCIF16(const void * _src, void * _dst, int xpos, int ypos, int width, int height)
{
  BYTE * src = (BYTE *)_src;
  BYTE * dst = (BYTE *)_dst + (ypos * CIF16_WIDTH) + xpos;

  BYTE * dstEnd = dst + CIF16_SIZE;
  int y;

  // copy Y
  for (y = 0; y < height; ++y) {
    PAssert(dst + width < dstEnd, "Y write overflow");
    memcpy(dst, src, width);
    src += width;
    dst += CIF16_WIDTH;
  }

  // copy U
  dst = (BYTE *)_dst + (CIF16_WIDTH * CIF16_HEIGHT) + (ypos * CIF16_WIDTH/4) + xpos / 2;
  for (y = 0; y < height/2; ++y) {
    PAssert(dst + width/2 <= dstEnd, "U write overflow");
    memcpy(dst, src, width/2);
    src += width/2;
    dst += CIF16_WIDTH/2;
  }

  // copy V
  dst = (BYTE *)_dst + (CIF16_WIDTH * CIF16_HEIGHT) + (CIF16_WIDTH * CIF16_HEIGHT) / 4 + (ypos * CIF16_WIDTH/4) + xpos / 2;
  for (y = 0; y < height/2; ++y) {
    PAssert(dst + width/2 <= dstEnd, "V write overflow");
    memcpy(dst, src, width/2);
    src += width/2;
    dst += CIF16_WIDTH/2;
  }
}


void MCUVideoMixer::CopyRFromRIntoR(const void *_s, void * _d, int xp, int yp, int w, int h, int rx_abs, int ry_abs, int rw, int rh, int fw, int fh, int lim_w, int lim_h)
{
 int rx=rx_abs-xp;
 int ry=ry_abs-yp;
 int w0=w/2;
 int ry0=ry/2;
 int rx0=rx/2;
 int fw0=fw/2;
 int rh0=rh/2;
 int rw0=rw/2;
 BYTE * s = (BYTE *)_s + w*ry + rx;
 BYTE * d = (BYTE *)_d + (yp+ry)*fw + xp + rx;
 BYTE * sU = (BYTE *)_s + w*h + ry0*w0 + rx0;
 BYTE * dU = (BYTE *)_d + fw*fh + (yp/2+ry0)*fw0 + xp/2 + rx0;
 BYTE * sV = sU + w0*(h/2);
 BYTE * dV = dU + fw0*(fh/2);

 if(rx+rw>lim_w)rw=lim_w-rx;
 if(rx0+rw0>lim_w/2)rw0=lim_w/2-rx0;
 if(ry+rh>lim_h)rh=lim_h-ry;
 if(ry0+rh0>lim_h/2)rh0=lim_h/2-ry0;

 if(rx&1){ dU++; sU++; dV++; sV++; }
 for(int i=ry;i<ry+rh;i++){
   memcpy(d,s,rw); s+=w; d+=fw;
   if(!(i&1)){
     memcpy(dU,sU,rw0); sU+=w0; dU+=fw0;
     memcpy(dV,sV,rw0); sV+=w0; dV+=fw0;
   }
 }
}

void MCUVideoMixer::CopyRectIntoFrame(const void * _src, void * _dst, int xpos, int ypos, int width, int height, int fw, int fh)
{
 if(xpos+width > fw || ypos+height > fh) return;
 
 BYTE * src = (BYTE *)_src;
 BYTE * dst = (BYTE *)_dst + (ypos * fw) + xpos;

 int y;

  // copy Y
  for (y = 0; y < height; ++y) 
   { memcpy(dst, src, width); src += width; dst += fw; }

  // copy U
  dst = (BYTE *)_dst + (fw * fh) + ((ypos>>1) * (fw>>1)) + (xpos >> 1);
  for (y = 0; y < height/2; ++y) 
   { memcpy(dst, src, width/2); src += width/2; dst += fw/2; }

  // copy V
  dst = (BYTE *)_dst + (fw * fh) + ((fw>>1) * (fh>>1)) + ((ypos>>1) * (fw>>1)) + (xpos >> 1);
  for (y = 0; y < height/2; ++y) 
   { memcpy(dst, src, width/2); src += width/2; dst += fw/2; }
}

void MCUVideoMixer::MixRectIntoFrameGrayscale(const void * _src, void * _dst, int xpos, int ypos, int width, int height, int fw, int fh, BYTE wide)
{
 if(xpos+width > fw || ypos+height > fh) return;
 BYTE * src = (BYTE *)_src;
 BYTE * dst = (BYTE *)_dst + (ypos * fw) + xpos*(1-wide);
 int y,x;
 for(y=0;y<height;y++)
 {
  if(wide)for(x=0;x<xpos;x++){ *dst>>=1; dst++; }
  for(x=0;x<width;x++) {
   if(*src>=*dst)*dst=*src; else
   *dst>>=1;
   src++; dst++;
  }
  if(wide)for(x=0;x<fw-width-xpos;x++){ *dst>>=1; dst++; }
  else dst+=(fw-width);
 }
}

#if USE_FREETYPE
void MCUVideoMixer::MixRectIntoFrameSubsMode(const void * _src, void * _dst, int xpos, int ypos, int width, int height, int fw, int fh, BYTE wide)
{
  if(xpos+width > fw || ypos+height > fh) return;
  BYTE * src = (BYTE *)_src;
  BYTE * dst = (BYTE *)_dst + (ypos * fw) + xpos;
  int y,x;
  for(y=0;y<height;y++)
  {
    for(x=0;x<width;x++)
    {
      if(*src!=0)*dst=*src;
      src++; dst++;
    }
    dst+=(fw-width);
  }
}
#endif

void MCUVideoMixer::CopyRectIntoRect(const void * _src, void * _dst, int xpos, int ypos, int width, int height, int fw, int fh)
{
 if(xpos+width > fw || ypos+height > fh) return;
 
 BYTE * src = (BYTE *)_src + (ypos * fw) + xpos;
 BYTE * dst = (BYTE *)_dst + (ypos * fw) + xpos;

 int y;

  // copy Y
  for (y = 0; y < height; ++y) 
   { memcpy(dst, src, width); src += fw; dst += fw; }

  // copy U
  src = (BYTE *)_src + (fw * fh) + (ypos * fw >> 2) + (xpos >> 1);
  dst = (BYTE *)_dst + (fw * fh) + (ypos * fw >> 2) + (xpos >> 1);
  for (y = 0; y < height/2; ++y) 
   { memcpy(dst, src, width/2); src += fw/2; dst += fw/2; }

  // copy V
  src = (BYTE *)_src + (fw * fh) + (fw * fh >> 2) + (ypos * fw >> 2) + (xpos >> 1);
  dst = (BYTE *)_dst + (fw * fh) + (fw * fh >> 2) + (ypos * fw >> 2) + (xpos >> 1);
  for (y = 0; y < height/2; ++y) 
   { memcpy(dst, src, width/2); src += fw/2; dst += fw/2; }
}

void MCUVideoMixer::CopyRectFromFrame(const void * _src, void * _dst, int xpos, int ypos, int width, int height, int fw, int fh)
{
 if(xpos+width > fw || ypos+height > fh) return;
 
 BYTE * dst = (BYTE *)_dst;
 BYTE * src = (BYTE *)_src + (ypos * fw) + xpos;

 int y;

  // copy Y
  for (y = 0; y < height; ++y) 
   { memcpy(dst, src, width); dst += width; src += fw; }

  // copy U
//  src = (BYTE *)_src + (fw * fh) + (ypos * fw >> 2) + (xpos >> 1);
  src = (BYTE *)_src + (fw * fh) + ((ypos>>1) * (fw >> 1)) + (xpos >> 1);
  for (y = 0; y < height/2; ++y) 
   { memcpy(dst, src, width/2); dst += width/2; src += fw/2; }

  // copy V
//  src = (BYTE *)_src + (fw * fh) + (fw * fh >> 2) + (ypos * fw >> 2) + (xpos >> 1);
  src = (BYTE *)_src + (fw * fh) + ((fw>>1) * (fh>>1)) + ((ypos>>1) * (fw>>1)) + (xpos >> 1);
  for (y = 0; y < height/2; ++y) 
   { memcpy(dst, src, width/2); dst += width/2; src += fw/2; }
}

#ifdef _WIN32
extern __inline uint64_t rdtsc()
{
  return __rdtsc();
}
#else
extern __inline__ uint64_t rdtsc()
{
  // https://www.kernel.org/doc/Documentation/prctl/disable-tsc-test.c
  uint32_t lo, hi;
  // We cannot use "=A", since this would use %rax on x86_64
  __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
  return (uint64_t)hi << 32 | lo;
}
#endif

void MCUVideoMixer::ResizeYUV420P(const void * _src, void * _dst, unsigned int sw, unsigned int sh, unsigned int dw, unsigned int dh)
{
  uint64_t TSC0=rdtsc();
  int scaleFilterType = OpenMCU::Current().GetScaleFilterType();

  if(sw==dw && sh==dh) // same size
    memcpy(_dst,_src,dw*dh*3/2);
#if USE_LIBYUV
  else if(scaleFilterType >= 1 && scaleFilterType <= 3)
  {
    libyuv::I420Scale(
    /* src_y */     (const uint8*)_src,                         /* src_stride_y */ sw,
    /* src_u */     (const uint8*)((long)_src+sw*sh),           /* src_stride_u */ (int)(sw >> 1),
    /* src_v */     (const uint8*)((long)_src+sw*sh*5/4),       /* src_stride_v */ (int)(sw >> 1),
    /* src_width */ (int)sw,                                    /* src_height */   (int)sh,
    /* dst_y */     (uint8*)_dst,                               /* dst_stride_y */ (int)dw,
    /* dst_u */     (uint8*)((long)_dst+dw*dh),                 /* dst_stride_u */ (int)(dw >> 1),
    /* dst_v */     (uint8*)((long)_dst+dw*dh+(dw>>1)*(dh>>1)), /* dst_stride_v */ (int)(dw >> 1),
    /* dst_width */ (int)dw,                                    /* dst_height */   (int)dh,
    /* filtering */ (libyuv::FilterMode)OpenMCU::GetScaleFilter(scaleFilterType)
    );
  }
#endif
#if USE_SWSCALE
  else if(scaleFilterType >= 4 && scaleFilterType <= 14)
  {
    struct SwsContext *sws_ctx = sws_getContext(sw, sh, AV_PIX_FMT_YUV420P,
                                                dw, dh, AV_PIX_FMT_YUV420P,
                                                OpenMCU::GetScaleFilter(scaleFilterType), NULL, NULL, NULL);
    if(sws_ctx == NULL)
    {
      MCUTRACE(1, "MCUVideoMixer\tImpossible to create scale context for the conversion "
                  << sw << "x" << sh << "->" << dw << "x" << dh);
      return;
    }

    // initialize linesize
    AVPicture src_picture;
    avpicture_fill(&src_picture, (uint8_t *)_src, AV_PIX_FMT_YUV420P, sw, sh);
    AVPicture dst_picture;
    avpicture_fill(&dst_picture, (uint8_t *)_dst, AV_PIX_FMT_YUV420P, dw, dh);

    sws_scale(sws_ctx, src_picture.data, src_picture.linesize, 0, sh,
                       dst_picture.data, dst_picture.linesize);

    sws_freeContext(sws_ctx);
  }
#endif
  else if(sw==CIF16_WIDTH && sh==CIF16_HEIGHT && dw==TCIF_WIDTH    && dh==TCIF_HEIGHT)   // CIF16 -> TCIF
    ConvertCIF16ToTCIF(_src,_dst);
  else if(sw==CIF16_WIDTH && sh==CIF16_HEIGHT && dw==Q3CIF16_WIDTH && dh==Q3CIF16_HEIGHT)// CIF16 -> Q3CIF16
    ConvertCIF16ToQ3CIF16(_src,_dst);
  else if(sw==CIF16_WIDTH && sh==CIF16_HEIGHT && dw==CIF4_WIDTH    && dh==CIF4_HEIGHT)   // CIF16 -> CIF4
    ConvertCIF16ToCIF4(_src,_dst);
  else if(sw==CIF16_WIDTH && sh==CIF16_HEIGHT && dw==Q3CIF4_WIDTH  && dh==Q3CIF4_HEIGHT) // CIF16 -> Q3CIF4
    ConvertCIF16ToQ3CIF4(_src,_dst);
  else if(sw==CIF16_WIDTH && sh==CIF16_HEIGHT && dw==CIF_WIDTH     && dh==CIF_HEIGHT)    // CIF16 -> CIF
    ConvertCIF16ToCIF(_src,_dst);

  else if(sw==CIF4_WIDTH && sh==CIF4_HEIGHT && dw==CIF16_WIDTH  && dh==CIF16_HEIGHT)  // CIF4 -> CIF16
    ConvertCIF4ToCIF16(_src,_dst);
  else if(sw==CIF4_WIDTH && sh==CIF4_HEIGHT && dw==TCIF_WIDTH   && dh==TCIF_HEIGHT)   // CIF4 -> TCIF
    ConvertCIF4ToTCIF(_src,_dst);
  else if(sw==CIF4_WIDTH && sh==CIF4_HEIGHT && dw==TQCIF_WIDTH  && dh==TQCIF_HEIGHT)  // CIF4 -> TQCIF
    ConvertCIF4ToTQCIF(_src,_dst);
  else if(sw==CIF4_WIDTH && sh==CIF4_HEIGHT && dw==CIF_WIDTH    && dh==CIF_HEIGHT)    // CIF4 -> CIF
    ConvertCIF4ToCIF(_src,_dst);
  else if(sw==CIF4_WIDTH && sh==CIF4_HEIGHT && dw==Q3CIF4_WIDTH && dh==Q3CIF4_HEIGHT) // CIF4 -> Q3CIF4
    ConvertCIF4ToQ3CIF4(_src,_dst);
  else if(sw==CIF4_WIDTH && sh==CIF4_HEIGHT && dw==QCIF_WIDTH   && dh==QCIF_HEIGHT)   // CIF4 -> QCIF
    ConvertCIF4ToQCIF(_src,_dst);
  else if(sw==CIF4_WIDTH && sh==CIF4_HEIGHT && dw==Q3CIF_WIDTH  && dh==Q3CIF_HEIGHT)  // CIF4 -> CIF16
    ConvertCIF4ToQ3CIF(_src,_dst);

  else if(sw==CIF_WIDTH && sh==CIF_HEIGHT && dw==CIF4_WIDTH   && dh==CIF4_HEIGHT)   // CIF -> CIF4
    ConvertCIFToCIF4(_src,_dst);
  else if(sw==CIF_WIDTH && sh==CIF_HEIGHT && dw==TQCIF_WIDTH  && dh==TQCIF_HEIGHT)  // CIF -> TQCIF
    ConvertCIFToTQCIF(_src,_dst);
  else if(sw==CIF_WIDTH && sh==CIF_HEIGHT && dw==TQCIF_WIDTH  && dh==TQCIF_HEIGHT)  // CIF -> TSQCIF
    ConvertCIFToTSQCIF(_src,_dst);
  else if(sw==CIF_WIDTH && sh==CIF_HEIGHT && dw==Q3CIF_WIDTH  && dh==Q3CIF_HEIGHT)  // CIF -> Q3CIF
    ConvertCIFToQ3CIF(_src,_dst);
  else if(sw==CIF_WIDTH && sh==CIF_HEIGHT && dw==QCIF_WIDTH   && dh==QCIF_HEIGHT)   // CIF -> QCIF
    ConvertCIFToQCIF(_src,_dst);
  else if(sw==CIF_WIDTH && sh==CIF_HEIGHT && dw==SQ3CIF_WIDTH && dh==SQ3CIF_HEIGHT) // CIF -> SQ3CIF
    ConvertCIFToSQ3CIF(_src,_dst);
  else if(sw==CIF_WIDTH && sh==CIF_HEIGHT && dw==SQCIF_WIDTH  && dh==SQCIF_HEIGHT)  // CIF -> SQCIF
    ConvertCIFToSQCIF(_src,_dst);

  else if(sw==QCIF_WIDTH && sh==QCIF_HEIGHT && dw==CIF4_WIDTH && dh==CIF4_HEIGHT) // QCIF -> CIF4
    ConvertQCIFToCIF4(_src,_dst);
  else if(sw==QCIF_WIDTH && sh==QCIF_HEIGHT && dw==CIF_WIDTH && dh==CIF_HEIGHT)   // QCIF -> CIF
    ConvertQCIFToCIF(_src,_dst);

  else if((sw<<1)==dw && (sh<<1)==dh) // needs 2x zoom
    Convert1To2(_src, _dst, sw, sh);
  else if((dw<<1)==sw && (dh<<1)==sh) // needs 2x reduce
    Convert2To1(_src, _dst, sw, sh);

  else ConvertFRAMEToCUSTOM_FRAME(_src,_dst,sw,sh,dw,dh);

  {
    PWaitAndSignal m(OpenMCU::Current().videoResizeDeltaTSCMutex);
    OpenMCU::Current().videoResizeDeltaTSCSum += (unsigned long)(rdtsc()-TSC0);
    OpenMCU::Current().videoResizeDeltaTSCCounter++;
    time_t t0 = time(NULL);
    if(t0 - OpenMCU::Current().videoResizeDeltaTSCReportTime >= 3)
    {
      PStringStream msg;
      msg << "resize_timing("
        << std::dec << (OpenMCU::Current().videoResizeDeltaTSCSum / OpenMCU::Current().videoResizeDeltaTSCCounter)
        << ")";
      OpenMCU::Current().HttpWriteCmd(msg);
      OpenMCU::Current().videoResizeDeltaTSCSum = 0;
      OpenMCU::Current().videoResizeDeltaTSCCounter = 0;
      OpenMCU::Current().videoResizeDeltaTSCReportTime = t0;
    }
  }

}

//#if !USE_LIBYUV && !USE_SWSCALE
void MCUVideoMixer::ConvertCIF4ToCIF(const void * _src, void * _dst)
{
  unsigned char * src = (unsigned char *)_src;
  unsigned char * dst = (unsigned char *)_dst;

  unsigned int y, x, val;
  unsigned char * srcRow0;
  unsigned char * srcRow1;

  // copy Y
  for (y = CIF_HEIGHT; y > 0; y--) {
    srcRow0 = src;
    srcRow1 = src + CIF4_WIDTH;
    for (x = CIF_WIDTH; x > 0; x--) {
      val = (*srcRow0+*(srcRow0+1)+*srcRow1+*(srcRow1+1))>>2;
      dst[0] = val;
      srcRow0 += 2; srcRow1 +=2;
      dst++;
    }
    src = srcRow1;
  }

  // copy U
  for (y = CIF_HEIGHT/2; y > 0; y--) {
    srcRow0 = src;
    srcRow1 = src + CIF_WIDTH;
    for (x = CIF_WIDTH/2; x > 0; x--) {
      val = (*srcRow0+*(srcRow0+1)+*srcRow1+*(srcRow1+1))>>2;
      dst[0] = val;
      srcRow0 += 2; srcRow1 +=2;
      dst++;
    }
    src = srcRow1;
  }

  // copy V
  for (y = CIF_HEIGHT/2; y > 0; y--) {
    srcRow0 = src;
    srcRow1 = src + CIF_WIDTH;
    for (x = CIF_WIDTH/2; x > 0; x--) {
      val = (*srcRow0+*(srcRow0+1)+*srcRow1+*(srcRow1+1))>>2;
      dst[0] = val;
      srcRow0 += 2; srcRow1 +=2;
      dst++;
    }
    src = srcRow1;
  }
}

void MCUVideoMixer::ConvertCIF16ToCIF4(const void * _src, void * _dst)
{
  unsigned char * src = (unsigned char *)_src;
  unsigned char * dst = (unsigned char *)_dst;

  unsigned int y, x, val;
  unsigned char * srcRow0;
  unsigned char * srcRow1;

  // copy Y
  for (y = CIF4_HEIGHT; y > 0; y--) {
    srcRow0 = src;
    srcRow1 = src + CIF16_WIDTH;
    for (x = CIF4_WIDTH; x > 0; x--) {
      val = (*srcRow0+*(srcRow0+1)+*srcRow1+*(srcRow1+1))>>2;
      dst[0] = val;
      srcRow0 += 2; srcRow1 +=2;
      dst++;
    }
    src = srcRow1;
  }

  // copy U
  for (y = CIF4_HEIGHT/2; y > 0; y--) {
    srcRow0 = src;
    srcRow1 = src + CIF4_WIDTH;
    for (x = CIF4_WIDTH/2; x > 0; x--) {
      val = (*srcRow0+*(srcRow0+1)+*srcRow1+*(srcRow1+1))>>2;
      dst[0] = val;
      srcRow0 += 2; srcRow1 +=2;
      dst++;
    }
    src = srcRow1;
  }

  // copy V
  for (y = CIF4_HEIGHT/2; y > 0; y--) {
    srcRow0 = src;
    srcRow1 = src + CIF4_WIDTH;
    for (x = CIF4_WIDTH/2; x > 0; x--) {
      val = (*srcRow0+*(srcRow0+1)+*srcRow1+*(srcRow1+1))>>2;
      dst[0] = val;
      srcRow0 += 2; srcRow1 +=2;
      dst++;
    }
    src = srcRow1;
  }
}

void MCUVideoMixer::ConvertCIFToQCIF(const void * _src, void * _dst)
{
  unsigned char * src = (unsigned char *)_src;
  unsigned char * dst = (unsigned char *)_dst;

  unsigned int y, x, val;
  unsigned char * srcRow0;
  unsigned char * srcRow1;

  // copy Y
  for (y = QCIF_HEIGHT; y > 0; y--) {
    srcRow0 = src;
    srcRow1 = src + CIF_WIDTH;
    for (x = QCIF_WIDTH; x > 0; x--) {
      val = (*srcRow0+*(srcRow0+1)+*srcRow1+*(srcRow1+1))>>2;
      dst[0] = val;
      srcRow0 += 2; srcRow1 +=2;
      dst++;
    }
    src = srcRow1;
  }

  // copy U
  for (y = QCIF_HEIGHT/2; y > 0; y--) {
    srcRow0 = src;
    srcRow1 = src + QCIF_WIDTH;
    for (x = QCIF_WIDTH/2; x > 0; x--) {
      val = (*srcRow0+*(srcRow0+1)+*srcRow1+*(srcRow1+1))>>2;
      dst[0] = val;
      srcRow0 += 2; srcRow1 +=2;
      dst++;
    }
    src = srcRow1;
  }

  // copy V
  for (y = QCIF_HEIGHT/2; y > 0; y--) {
    srcRow0 = src;
    srcRow1 = src + QCIF_WIDTH;
    for (x = QCIF_WIDTH/2; x > 0; x--) {
      val = (*srcRow0+*(srcRow0+1)+*srcRow1+*(srcRow1+1))>>2;
      dst[0] = val;
      srcRow0 += 2; srcRow1 +=2;
      dst++;
    }
    src = srcRow1;
  }
}

void MCUVideoMixer::ConvertCIFToQ3CIF(const void * _src, void * _dst)
{
  unsigned char * src = (unsigned char *)_src;
  unsigned char * dst = (unsigned char *)_dst;

  unsigned int y, x, val;
  unsigned char * srcRow0;
  unsigned char * srcRow1;
  unsigned char * srcRow2;

  // copy Y
  for (y = Q3CIF_HEIGHT; y > 1; y-=2) {
    srcRow0 = src;
    srcRow1 = src + CIF_WIDTH;
    srcRow2 = src + CIF_WIDTH*2;
    for (x = Q3CIF_WIDTH; x > 1; x-=2) {
      val = (*srcRow0*4+*(srcRow0+1)*2+*srcRow1*2+*(srcRow1+1))*0.111111111;
      dst[0] = val;
      val = (*(srcRow0+1)*2+*(srcRow0+2)*4+*(srcRow1+1)+*(srcRow1+2)*2)*0.111111111;
      dst[1] = val;
      val = (*srcRow1*2+*(srcRow1+1)+*srcRow2*4+*(srcRow2+1)*2)*0.111111111;
      dst[Q3CIF_WIDTH] = val;
      val = (*(srcRow1+1)+*(srcRow1+2)*2+*(srcRow2+1)*2+*(srcRow2+2)*4)*0.111111111;
      dst[Q3CIF_WIDTH+1] = val;
      srcRow0 +=3; srcRow1 +=3; srcRow2 +=3;
      dst+=2;
    }
//    dst[0]=*srcRow0; dst[Q3CIF_WIDTH]=*srcRow2; dst++;
    dst += Q3CIF_WIDTH;
    src += CIF_WIDTH*3;
  }
  src=(unsigned char *)_src+CIF_WIDTH*CIF_HEIGHT;

  // copy U
  for (y = Q3CIF_HEIGHT/2; y > 1; y-=2) {
    srcRow0 = src;
    srcRow1 = src + QCIF_WIDTH;
    srcRow2 = src + QCIF_WIDTH*2;
    for (x = Q3CIF_WIDTH/2; x > 1; x-=2) {
      val = (*srcRow0*4+*(srcRow0+1)*2+*srcRow1*2+*(srcRow1+1))*0.111111111;
      dst[0] = val;
      val = (*(srcRow0+1)*2+*(srcRow0+2)*4+*(srcRow1+1)+*(srcRow1+2)*2)*0.111111111;
      dst[1] = val;
      val = (*srcRow1*2+*(srcRow1+1)+*srcRow2*4+*(srcRow2+1)*2)*0.111111111;
      dst[Q3CIF_WIDTH/2] = val;
      val = (*(srcRow1+1)+*(srcRow1+2)*2+*(srcRow2+1)*2+*(srcRow2+2)*4)*0.111111111;
      dst[Q3CIF_WIDTH/2+1] = val;
      srcRow0 +=3; srcRow1 +=3; srcRow2 +=3;
      dst+=2;
    }
    if(x!=0) { dst[0]=*srcRow0; dst[Q3CIF_WIDTH/2]=*srcRow2; dst++; }
    dst += Q3CIF_WIDTH/2;
    src += QCIF_WIDTH*3;
  }
  src=(unsigned char *)_src+CIF_WIDTH*CIF_HEIGHT+QCIF_WIDTH*QCIF_HEIGHT;

  // copy V
  for (y = Q3CIF_HEIGHT/2; y > 1; y-=2) {
    srcRow0 = src;
    srcRow1 = src + QCIF_WIDTH;
    srcRow2 = src + QCIF_WIDTH*2;
    for (x = Q3CIF_WIDTH/2; x > 1; x-=2) {
      val = (*srcRow0*4+*(srcRow0+1)*2+*srcRow1*2+*(srcRow1+1))*0.111111111;
      dst[0] = val;
      val = (*(srcRow0+1)*2+*(srcRow0+2)*4+*(srcRow1+1)+*(srcRow1+2)*2)*0.111111111;
      dst[1] = val;
      val = (*srcRow1*2+*(srcRow1+1)+*srcRow2*4+*(srcRow2+1)*2)*0.111111111;
      dst[Q3CIF_WIDTH/2] = val;
      val = (*(srcRow1+1)+*(srcRow1+2)*2+*(srcRow2+1)*2+*(srcRow2+2)*4)*0.111111111;
      dst[Q3CIF_WIDTH/2+1] = val;
      srcRow0 +=3; srcRow1 +=3; srcRow2 +=3;
      dst+=2;
    }
    if(x!=0) { dst[0]=*srcRow0; dst[Q3CIF_WIDTH/2]=*srcRow2; dst++; }
    dst += Q3CIF_WIDTH/2;
    src += QCIF_WIDTH*3;
  }
}

void MCUVideoMixer::ConvertCIF4ToQ3CIF4(const void * _src, void * _dst)
{
  unsigned char * src = (unsigned char *)_src;
  unsigned char * dst = (unsigned char *)_dst;

  unsigned int y, x, val;
  unsigned char * srcRow0;
  unsigned char * srcRow1;
  unsigned char * srcRow2;

  // copy Y
  for (y = Q3CIF4_HEIGHT; y > 1; y-=2) {
    srcRow0 = src;
    srcRow1 = src + CIF4_WIDTH;
    srcRow2 = src + CIF4_WIDTH*2;
    for (x = Q3CIF4_WIDTH; x > 1; x-=2) {
      val = (*srcRow0*4+*(srcRow0+1)*2+*srcRow1*2+*(srcRow1+1))*0.111111111;
      dst[0] = val;
      val = (*(srcRow0+1)*2+*(srcRow0+2)*4+*(srcRow1+1)+*(srcRow1+2)*2)*0.111111111;
      dst[1] = val;
      val = (*srcRow1*2+*(srcRow1+1)+*srcRow2*4+*(srcRow2+1)*2)*0.111111111;
      dst[Q3CIF4_WIDTH] = val;
      val = (*(srcRow1+1)+*(srcRow1+2)*2+*(srcRow2+1)*2+*(srcRow2+2)*4)*0.111111111;
      dst[Q3CIF4_WIDTH+1] = val;
      srcRow0 +=3; srcRow1 +=3; srcRow2 +=3;
      dst+=2;
    }
//    dst[0]=*srcRow0; dst[Q3CIF_WIDTH]=*srcRow2; dst++;
    dst += Q3CIF4_WIDTH;
    src += CIF4_WIDTH*3;
  }
  src=(unsigned char *)_src+CIF4_WIDTH*CIF4_HEIGHT;

  // copy U
  for (y = Q3CIF4_HEIGHT/2; y > 1; y-=2) {
    srcRow0 = src;
    srcRow1 = src + CIF_WIDTH;
    srcRow2 = src + CIF_WIDTH*2;
    for (x = Q3CIF4_WIDTH/2; x > 1; x-=2) {
      val = (*srcRow0*4+*(srcRow0+1)*2+*srcRow1*2+*(srcRow1+1))*0.111111111;
      dst[0] = val;
      val = (*(srcRow0+1)*2+*(srcRow0+2)*4+*(srcRow1+1)+*(srcRow1+2)*2)*0.111111111;
      dst[1] = val;
      val = (*srcRow1*2+*(srcRow1+1)+*srcRow2*4+*(srcRow2+1)*2)*0.111111111;
      dst[Q3CIF4_WIDTH/2] = val;
      val = (*(srcRow1+1)+*(srcRow1+2)*2+*(srcRow2+1)*2+*(srcRow2+2)*4)*0.111111111;
      dst[Q3CIF4_WIDTH/2+1] = val;
      srcRow0 +=3; srcRow1 +=3; srcRow2 +=3;
      dst+=2;
    }
    if(x!=0) { dst[0]=*srcRow0; dst[Q3CIF4_WIDTH/2]=*srcRow2; dst++; }
    dst += Q3CIF4_WIDTH/2;
    src += CIF_WIDTH*3;
  }
  src=(unsigned char *)_src+CIF4_WIDTH*CIF4_HEIGHT+CIF_WIDTH*CIF_HEIGHT;

  // copy V
  for (y = Q3CIF4_HEIGHT/2; y > 1; y-=2) {
    srcRow0 = src;
    srcRow1 = src + CIF_WIDTH;
    srcRow2 = src + CIF_WIDTH*2;
    for (x = Q3CIF4_WIDTH/2; x > 1; x-=2) {
      val = (*srcRow0*4+*(srcRow0+1)*2+*srcRow1*2+*(srcRow1+1))*0.111111111;
      dst[0] = val;
      val = (*(srcRow0+1)*2+*(srcRow0+2)*4+*(srcRow1+1)+*(srcRow1+2)*2)*0.111111111;
      dst[1] = val;
      val = (*srcRow1*2+*(srcRow1+1)+*srcRow2*4+*(srcRow2+1)*2)*0.111111111;
      dst[Q3CIF4_WIDTH/2] = val;
      val = (*(srcRow1+1)+*(srcRow1+2)*2+*(srcRow2+1)*2+*(srcRow2+2)*4)*0.111111111;
      dst[Q3CIF4_WIDTH/2+1] = val;
      srcRow0 +=3; srcRow1 +=3; srcRow2 +=3;
      dst+=2;
    }
    if(x!=0) { dst[0]=*srcRow0; dst[Q3CIF4_WIDTH/2]=*srcRow2; dst++; }
    dst += Q3CIF4_WIDTH/2;
    src += CIF_WIDTH*3;
  }
}

void MCUVideoMixer::ConvertCIF16ToQ3CIF16(const void * _src, void * _dst)
{
  unsigned char * src = (unsigned char *)_src;
  unsigned char * dst = (unsigned char *)_dst;

  unsigned int y, x, val;
  unsigned char * srcRow0;
  unsigned char * srcRow1;
  unsigned char * srcRow2;

  // copy Y
  for (y = Q3CIF16_HEIGHT; y > 1; y-=2) {
    srcRow0 = src;
    srcRow1 = src + CIF16_WIDTH;
    srcRow2 = src + CIF16_WIDTH*2;
    for (x = Q3CIF16_WIDTH; x > 1; x-=2) {
      val = (*srcRow0*4+*(srcRow0+1)*2+*srcRow1*2+*(srcRow1+1))*0.111111111;
      dst[0] = val;
      val = (*(srcRow0+1)*2+*(srcRow0+2)*4+*(srcRow1+1)+*(srcRow1+2)*2)*0.111111111;
      dst[1] = val;
      val = (*srcRow1*2+*(srcRow1+1)+*srcRow2*4+*(srcRow2+1)*2)*0.111111111;
      dst[Q3CIF16_WIDTH] = val;
      val = (*(srcRow1+1)+*(srcRow1+2)*2+*(srcRow2+1)*2+*(srcRow2+2)*4)*0.111111111;
      dst[Q3CIF16_WIDTH+1] = val;
      srcRow0 +=3; srcRow1 +=3; srcRow2 +=3;
      dst+=2;
    }
//    dst[0]=*srcRow0; dst[Q3CIF_WIDTH]=*srcRow2; dst++;
    dst += Q3CIF16_WIDTH;
    src += CIF16_WIDTH*3;
  }
  src=(unsigned char *)_src+CIF16_WIDTH*CIF16_HEIGHT;

  // copy U
  for (y = Q3CIF16_HEIGHT/2; y > 1; y-=2) {
    srcRow0 = src;
    srcRow1 = src + CIF4_WIDTH;
    srcRow2 = src + CIF4_WIDTH*2;
    for (x = Q3CIF16_WIDTH/2; x > 1; x-=2) {
      val = (*srcRow0*4+*(srcRow0+1)*2+*srcRow1*2+*(srcRow1+1))*0.111111111;
      dst[0] = val;
      val = (*(srcRow0+1)*2+*(srcRow0+2)*4+*(srcRow1+1)+*(srcRow1+2)*2)*0.111111111;
      dst[1] = val;
      val = (*srcRow1*2+*(srcRow1+1)+*srcRow2*4+*(srcRow2+1)*2)*0.111111111;
      dst[Q3CIF16_WIDTH/2] = val;
      val = (*(srcRow1+1)+*(srcRow1+2)*2+*(srcRow2+1)*2+*(srcRow2+2)*4)*0.111111111;
      dst[Q3CIF16_WIDTH/2+1] = val;
      srcRow0 +=3; srcRow1 +=3; srcRow2 +=3;
      dst+=2;
    }
    if(x!=0) { dst[0]=*srcRow0; dst[Q3CIF16_WIDTH/2]=*srcRow2; dst++; }
    dst += Q3CIF16_WIDTH/2;
    src += CIF4_WIDTH*3;
  }
  src=(unsigned char *)_src+CIF16_WIDTH*CIF16_HEIGHT+CIF4_WIDTH*CIF4_HEIGHT;

  // copy V
  for (y = Q3CIF16_HEIGHT/2; y > 1; y-=2) {
    srcRow0 = src;
    srcRow1 = src + CIF4_WIDTH;
    srcRow2 = src + CIF4_WIDTH*2;
    for (x = Q3CIF16_WIDTH/2; x > 1; x-=2) {
      val = (*srcRow0*4+*(srcRow0+1)*2+*srcRow1*2+*(srcRow1+1))*0.111111111;
      dst[0] = val;
      val = (*(srcRow0+1)*2+*(srcRow0+2)*4+*(srcRow1+1)+*(srcRow1+2)*2)*0.111111111;
      dst[1] = val;
      val = (*srcRow1*2+*(srcRow1+1)+*srcRow2*4+*(srcRow2+1)*2)*0.111111111;
      dst[Q3CIF16_WIDTH/2] = val;
      val = (*(srcRow1+1)+*(srcRow1+2)*2+*(srcRow2+1)*2+*(srcRow2+2)*4)*0.111111111;
      dst[Q3CIF16_WIDTH/2+1] = val;
      srcRow0 +=3; srcRow1 +=3; srcRow2 +=3;
      dst+=2;
    }
    if(x!=0) { dst[0]=*srcRow0; dst[Q3CIF16_WIDTH/2]=*srcRow2; dst++; }
    dst += Q3CIF16_WIDTH/2;
    src += CIF4_WIDTH*3;
  }
}

void MCUVideoMixer::ConvertCIF16ToTCIF(const void * _src, void * _dst)
{
  unsigned char * src = (unsigned char *)_src;
  unsigned char * dst = (unsigned char *)_dst;

  unsigned int y, x, val;
  unsigned char * srcRow0;
  unsigned char * srcRow1;
  unsigned char * srcRow2;
  unsigned char * srcRow3;

  // copy Y
  for (y = TCIF_HEIGHT; y > 2; y-=3) {
    srcRow0 = src;
    srcRow1 = src + CIF16_WIDTH;
    srcRow2 = src + CIF16_WIDTH*2;
    srcRow3 = src + CIF16_WIDTH*3;
    for (x = TCIF_WIDTH; x > 2; x-=3) {
      val = (*srcRow0*9+*(srcRow0+1)*3+*srcRow1*3+*(srcRow1+1))>>4;
      dst[0] = val;
      val = (*(srcRow0+1)*6+*(srcRow0+2)*6+*(srcRow1+1)*2+*(srcRow1+2)*2)>>4;
      dst[1] = val;
      val = (*(srcRow0+2)*3+*(srcRow0+3)*9+*(srcRow1+2)+*(srcRow1+3)*3)>>4;
      dst[2] = val;
      val = (*srcRow1*6+*(srcRow1+1)*2+*srcRow2*6+*(srcRow2+1)*2)>>4;
      dst[TCIF_WIDTH] = val;
      val = (*(srcRow1+1)*4+*(srcRow1+2)*4+*(srcRow2+1)*4+*(srcRow2+2)*4)>>4;
      dst[TCIF_WIDTH+1] = val;
      val = (*(srcRow1+2)*2+*(srcRow1+3)*6+*(srcRow2+2)*2+*(srcRow2+3)*6)>>4;
      dst[TCIF_WIDTH+2] = val;
      val = (*srcRow2*3+*(srcRow2+1)*1+*srcRow3*9+*(srcRow3+1)*3)>>4;
      dst[TCIF_WIDTH*2] = val;
      val = (*(srcRow2+1)*2+*(srcRow2+2)*2+*(srcRow3+1)*6+*(srcRow3+2)*6)>>4;
      dst[TCIF_WIDTH*2+1] = val;
      val = (*(srcRow2+2)*1+*(srcRow2+3)*3+*(srcRow3+2)*3+*(srcRow3+3)*9)>>4;
      dst[TCIF_WIDTH*2+2] = val;
      srcRow0 +=4; srcRow1 +=4; srcRow2 +=4; srcRow3 +=4;
      dst+=3;
    }
//    dst[0]=*srcRow0; dst[Q3CIF_WIDTH]=*srcRow2; dst++;
    dst += TCIF_WIDTH*2;
    src += CIF16_WIDTH*4;
  }
  src=(unsigned char *)_src+CIF16_WIDTH*CIF16_HEIGHT;

  // copy U
  for (y = TCIF_HEIGHT/2; y > 2; y-=3) {
    srcRow0 = src;
    srcRow1 = src + CIF4_WIDTH;
    srcRow2 = src + CIF4_WIDTH*2;
    srcRow3 = src + CIF4_WIDTH*3;
    for (x = TCIF_WIDTH/2; x > 2; x-=3) {
      val = (*srcRow0*9+*(srcRow0+1)*3+*srcRow1*3+*(srcRow1+1))>>4;
      dst[0] = val;
      val = (*(srcRow0+1)*6+*(srcRow0+2)*6+*(srcRow1+1)*2+*(srcRow1+2)*2)>>4;
      dst[1] = val;
      val = (*(srcRow0+2)*3+*(srcRow0+3)*9+*(srcRow1+2)+*(srcRow1+3)*3)>>4;
      dst[2] = val;
      val = (*srcRow1*6+*(srcRow1+1)*2+*srcRow2*6+*(srcRow2+1)*2)>>4;
      dst[TCIF_WIDTH/2] = val;
      val = (*(srcRow1+1)*4+*(srcRow1+2)*4+*(srcRow2+1)*4+*(srcRow2+2)*4)>>4;
      dst[TCIF_WIDTH/2+1] = val;
      val = (*(srcRow1+2)*2+*(srcRow1+3)*6+*(srcRow2+2)*2+*(srcRow2+3)*6)>>4;
      dst[TCIF_WIDTH/2+2] = val;
      val = (*srcRow2*3+*(srcRow2+1)*1+*srcRow3*9+*(srcRow3+1)*3)>>4;
      dst[TCIF_WIDTH] = val;
      val = (*(srcRow2+1)*2+*(srcRow2+2)*2+*(srcRow3+1)*6+*(srcRow3+2)*6)>>4;
      dst[TCIF_WIDTH+1] = val;
      val = (*(srcRow2+2)*1+*(srcRow2+3)*3+*(srcRow3+2)*3+*(srcRow3+3)*9)>>4;
      dst[TCIF_WIDTH+2] = val;
      srcRow0 +=4; srcRow1 +=4; srcRow2 +=4; srcRow3 +=4;
      dst+=3;
    }
//    dst += x;
    dst += TCIF_WIDTH;
    src += CIF4_WIDTH*4;
  }
  src=(unsigned char *)_src+CIF16_WIDTH*CIF16_HEIGHT+CIF4_WIDTH*CIF4_HEIGHT;

  // copy V
  for (y = TCIF_HEIGHT/2; y > 2; y-=3) {
    srcRow0 = src;
    srcRow1 = src + CIF4_WIDTH;
    srcRow2 = src + CIF4_WIDTH*2;
    srcRow3 = src + CIF4_WIDTH*3;
    for (x = TCIF_WIDTH/2; x > 2; x-=3) {
      val = (*srcRow0*9+*(srcRow0+1)*3+*srcRow1*3+*(srcRow1+1))>>4;
      dst[0] = val;
      val = (*(srcRow0+1)*6+*(srcRow0+2)*6+*(srcRow1+1)*2+*(srcRow1+2)*2)>>4;
      dst[1] = val;
      val = (*(srcRow0+2)*3+*(srcRow0+3)*9+*(srcRow1+2)+*(srcRow1+3)*3)>>4;
      dst[2] = val;
      val = (*srcRow1*6+*(srcRow1+1)*2+*srcRow2*6+*(srcRow2+1)*2)>>4;
      dst[TCIF_WIDTH/2] = val;
      val = (*(srcRow1+1)*4+*(srcRow1+2)*4+*(srcRow2+1)*4+*(srcRow2+2)*4)>>4;
      dst[TCIF_WIDTH/2+1] = val;
      val = (*(srcRow1+2)*2+*(srcRow1+3)*6+*(srcRow2+2)*2+*(srcRow2+3)*6)>>4;
      dst[TCIF_WIDTH/2+2] = val;
      val = (*srcRow2*3+*(srcRow2+1)*1+*srcRow3*9+*(srcRow3+1)*3)>>4;
      dst[TCIF_WIDTH] = val;
      val = (*(srcRow2+1)*2+*(srcRow2+2)*2+*(srcRow3+1)*6+*(srcRow3+2)*6)>>4;
      dst[TCIF_WIDTH+1] = val;
      val = (*(srcRow2+2)*1+*(srcRow2+3)*3+*(srcRow3+2)*3+*(srcRow3+3)*9)>>4;
      dst[TCIF_WIDTH+2] = val;
      srcRow0 +=4; srcRow1 +=4; srcRow2 +=4; srcRow3 +=4;
      dst+=3;
    }
//    if(x!=0) { dst[0]=*srcRow0; dst[Q3CIF16_WIDTH/2]=*srcRow2; dst++; }
    dst += TCIF_WIDTH;
    src += CIF4_WIDTH*4;
  }
}

void MCUVideoMixer::ConvertCIF4ToTQCIF(const void * _src, void * _dst)
{
  unsigned char * src = (unsigned char *)_src;
  unsigned char * dst = (unsigned char *)_dst;

  unsigned int y, x, val;
  unsigned char * srcRow0;
  unsigned char * srcRow1;
  unsigned char * srcRow2;
  unsigned char * srcRow3;

  // copy Y
  for (y = TQCIF_HEIGHT; y > 2; y-=3) {
    srcRow0 = src;
    srcRow1 = src + CIF4_WIDTH;
    srcRow2 = src + CIF4_WIDTH*2;
    srcRow3 = src + CIF4_WIDTH*3;
    for (x = TQCIF_WIDTH; x > 2; x-=3) {
      val = (*srcRow0*9+*(srcRow0+1)*3+*srcRow1*3+*(srcRow1+1))>>4;
      dst[0] = val;
      val = (*(srcRow0+1)*6+*(srcRow0+2)*6+*(srcRow1+1)*2+*(srcRow1+2)*2)>>4;
      dst[1] = val;
      val = (*(srcRow0+2)*3+*(srcRow0+3)*9+*(srcRow1+2)+*(srcRow1+3)*3)>>4;
      dst[2] = val;
      val = (*srcRow1*6+*(srcRow1+1)*2+*srcRow2*6+*(srcRow2+1)*2)>>4;
      dst[TQCIF_WIDTH] = val;
      val = (*(srcRow1+1)*4+*(srcRow1+2)*4+*(srcRow2+1)*4+*(srcRow2+2)*4)>>4;
      dst[TQCIF_WIDTH+1] = val;
      val = (*(srcRow1+2)*2+*(srcRow1+3)*6+*(srcRow2+2)*2+*(srcRow2+3)*6)>>4;
      dst[TQCIF_WIDTH+2] = val;
      val = (*srcRow2*3+*(srcRow2+1)*1+*srcRow3*9+*(srcRow3+1)*3)>>4;
      dst[TQCIF_WIDTH*2] = val;
      val = (*(srcRow2+1)*2+*(srcRow2+2)*2+*(srcRow3+1)*6+*(srcRow3+2)*6)>>4;
      dst[TQCIF_WIDTH*2+1] = val;
      val = (*(srcRow2+2)*1+*(srcRow2+3)*3+*(srcRow3+2)*3+*(srcRow3+3)*9)>>4;
      dst[TQCIF_WIDTH*2+2] = val;
      srcRow0 +=4; srcRow1 +=4; srcRow2 +=4; srcRow3 +=4;
      dst+=3;
    }
//    dst[0]=*srcRow0; dst[Q3CIF_WIDTH]=*srcRow2; dst++;
    dst += TQCIF_WIDTH*2;
    src += CIF4_WIDTH*4;
  }
  src=(unsigned char *)_src+CIF4_WIDTH*CIF4_HEIGHT;

  // copy U
  for (y = TQCIF_HEIGHT/2; y > 2; y-=3) {
    srcRow0 = src;
    srcRow1 = src + CIF_WIDTH;
    srcRow2 = src + CIF_WIDTH*2;
    srcRow3 = src + CIF_WIDTH*3;
    for (x = TQCIF_WIDTH/2; x > 2; x-=3) {
      val = (*srcRow0*9+*(srcRow0+1)*3+*srcRow1*3+*(srcRow1+1))>>4;
      dst[0] = val;
      val = (*(srcRow0+1)*6+*(srcRow0+2)*6+*(srcRow1+1)*2+*(srcRow1+2)*2)>>4;
      dst[1] = val;
      val = (*(srcRow0+2)*3+*(srcRow0+3)*9+*(srcRow1+2)+*(srcRow1+3)*3)>>4;
      dst[2] = val;
      val = (*srcRow1*6+*(srcRow1+1)*2+*srcRow2*6+*(srcRow2+1)*2)>>4;
      dst[TQCIF_WIDTH/2] = val;
      val = (*(srcRow1+1)*4+*(srcRow1+2)*4+*(srcRow2+1)*4+*(srcRow2+2)*4)>>4;
      dst[TQCIF_WIDTH/2+1] = val;
      val = (*(srcRow1+2)*2+*(srcRow1+3)*6+*(srcRow2+2)*2+*(srcRow2+3)*6)>>4;
      dst[TQCIF_WIDTH/2+2] = val;
      val = (*srcRow2*3+*(srcRow2+1)*1+*srcRow3*9+*(srcRow3+1)*3)>>4;
      dst[TQCIF_WIDTH] = val;
      val = (*(srcRow2+1)*2+*(srcRow2+2)*2+*(srcRow3+1)*6+*(srcRow3+2)*6)>>4;
      dst[TQCIF_WIDTH+1] = val;
      val = (*(srcRow2+2)*1+*(srcRow2+3)*3+*(srcRow3+2)*3+*(srcRow3+3)*9)>>4;
      dst[TQCIF_WIDTH+2] = val;
      srcRow0 +=4; srcRow1 +=4; srcRow2 +=4; srcRow3 +=4;
      dst+=3;
    }
//    dst += x;
    dst += TQCIF_WIDTH;
    src += CIF_WIDTH*4;
  }
  src=(unsigned char *)_src+CIF4_WIDTH*CIF4_HEIGHT+CIF_WIDTH*CIF_HEIGHT;

  // copy V
  for (y = TQCIF_HEIGHT/2; y > 2; y-=3) {
    srcRow0 = src;
    srcRow1 = src + CIF_WIDTH;
    srcRow2 = src + CIF_WIDTH*2;
    srcRow3 = src + CIF_WIDTH*3;
    for (x = TQCIF_WIDTH/2; x > 2; x-=3) {
      val = (*srcRow0*9+*(srcRow0+1)*3+*srcRow1*3+*(srcRow1+1))>>4;
      dst[0] = val;
      val = (*(srcRow0+1)*6+*(srcRow0+2)*6+*(srcRow1+1)*2+*(srcRow1+2)*2)>>4;
      dst[1] = val;
      val = (*(srcRow0+2)*3+*(srcRow0+3)*9+*(srcRow1+2)+*(srcRow1+3)*3)>>4;
      dst[2] = val;
      val = (*srcRow1*6+*(srcRow1+1)*2+*srcRow2*6+*(srcRow2+1)*2)>>4;
      dst[TQCIF_WIDTH/2] = val;
      val = (*(srcRow1+1)*4+*(srcRow1+2)*4+*(srcRow2+1)*4+*(srcRow2+2)*4)>>4;
      dst[TQCIF_WIDTH/2+1] = val;
      val = (*(srcRow1+2)*2+*(srcRow1+3)*6+*(srcRow2+2)*2+*(srcRow2+3)*6)>>4;
      dst[TQCIF_WIDTH/2+2] = val;
      val = (*srcRow2*3+*(srcRow2+1)*1+*srcRow3*9+*(srcRow3+1)*3)>>4;
      dst[TQCIF_WIDTH] = val;
      val = (*(srcRow2+1)*2+*(srcRow2+2)*2+*(srcRow3+1)*6+*(srcRow3+2)*6)>>4;
      dst[TQCIF_WIDTH+1] = val;
      val = (*(srcRow2+2)*1+*(srcRow2+3)*3+*(srcRow3+2)*3+*(srcRow3+3)*9)>>4;
      dst[TQCIF_WIDTH+2] = val;
      srcRow0 +=4; srcRow1 +=4; srcRow2 +=4; srcRow3 +=4;
      dst+=3;
    }
//    if(x!=0) { dst[0]=*srcRow0; dst[Q3CIF16_WIDTH/2]=*srcRow2; dst++; }
    dst += TQCIF_WIDTH;
    src += CIF_WIDTH*4;
  }
}

void MCUVideoMixer::ConvertCIFToTSQCIF(const void * _src, void * _dst)
{
  unsigned char * src = (unsigned char *)_src;
  unsigned char * dst = (unsigned char *)_dst;

  unsigned int y, x, val;
  unsigned char * srcRow0;
  unsigned char * srcRow1;
  unsigned char * srcRow2;
  unsigned char * srcRow3;

  // copy Y
  for (y = TSQCIF_HEIGHT; y > 2; y-=3) {
    srcRow0 = src;
    srcRow1 = src + CIF_WIDTH;
    srcRow2 = src + CIF_WIDTH*2;
    srcRow3 = src + CIF_WIDTH*3;
    for (x = TSQCIF_WIDTH; x > 2; x-=3) {
      val = (*srcRow0*9+*(srcRow0+1)*3+*srcRow1*3+*(srcRow1+1))>>4;
      dst[0] = val;
      val = (*(srcRow0+1)*6+*(srcRow0+2)*6+*(srcRow1+1)*2+*(srcRow1+2)*2)>>4;
      dst[1] = val;
      val = (*(srcRow0+2)*3+*(srcRow0+3)*9+*(srcRow1+2)+*(srcRow1+3)*3)>>4;
      dst[2] = val;
      val = (*srcRow1*6+*(srcRow1+1)*2+*srcRow2*6+*(srcRow2+1)*2)>>4;
      dst[TSQCIF_WIDTH] = val;
      val = (*(srcRow1+1)*4+*(srcRow1+2)*4+*(srcRow2+1)*4+*(srcRow2+2)*4)>>4;
      dst[TSQCIF_WIDTH+1] = val;
      val = (*(srcRow1+2)*2+*(srcRow1+3)*6+*(srcRow2+2)*2+*(srcRow2+3)*6)>>4;
      dst[TSQCIF_WIDTH+2] = val;
      val = (*srcRow2*3+*(srcRow2+1)*1+*srcRow3*9+*(srcRow3+1)*3)>>4;
      dst[TSQCIF_WIDTH*2] = val;
      val = (*(srcRow2+1)*2+*(srcRow2+2)*2+*(srcRow3+1)*6+*(srcRow3+2)*6)>>4;
      dst[TSQCIF_WIDTH*2+1] = val;
      val = (*(srcRow2+2)*1+*(srcRow2+3)*3+*(srcRow3+2)*3+*(srcRow3+3)*9)>>4;
      dst[TSQCIF_WIDTH*2+2] = val;
      srcRow0 +=4; srcRow1 +=4; srcRow2 +=4; srcRow3 +=4;
      dst+=3;
    }
//    dst[0]=*srcRow0; dst[Q3CIF_WIDTH]=*srcRow2; dst++;
    dst += TSQCIF_WIDTH*2;
    src += CIF_WIDTH*4;
  }
  src=(unsigned char *)_src+CIF_WIDTH*CIF_HEIGHT;

  // copy U
  for (y = TSQCIF_HEIGHT/2; y > 2; y-=3) {
    srcRow0 = src;
    srcRow1 = src + QCIF_WIDTH;
    srcRow2 = src + QCIF_WIDTH*2;
    srcRow3 = src + QCIF_WIDTH*3;
    for (x = TSQCIF_WIDTH/2; x > 2; x-=3) {
      val = (*srcRow0*9+*(srcRow0+1)*3+*srcRow1*3+*(srcRow1+1))>>4;
      dst[0] = val;
      val = (*(srcRow0+1)*6+*(srcRow0+2)*6+*(srcRow1+1)*2+*(srcRow1+2)*2)>>4;
      dst[1] = val;
      val = (*(srcRow0+2)*3+*(srcRow0+3)*9+*(srcRow1+2)+*(srcRow1+3)*3)>>4;
      dst[2] = val;
      val = (*srcRow1*6+*(srcRow1+1)*2+*srcRow2*6+*(srcRow2+1)*2)>>4;
      dst[TSQCIF_WIDTH/2] = val;
      val = (*(srcRow1+1)*4+*(srcRow1+2)*4+*(srcRow2+1)*4+*(srcRow2+2)*4)>>4;
      dst[TSQCIF_WIDTH/2+1] = val;
      val = (*(srcRow1+2)*2+*(srcRow1+3)*6+*(srcRow2+2)*2+*(srcRow2+3)*6)>>4;
      dst[TSQCIF_WIDTH/2+2] = val;
      val = (*srcRow2*3+*(srcRow2+1)*1+*srcRow3*9+*(srcRow3+1)*3)>>4;
      dst[TSQCIF_WIDTH] = val;
      val = (*(srcRow2+1)*2+*(srcRow2+2)*2+*(srcRow3+1)*6+*(srcRow3+2)*6)>>4;
      dst[TSQCIF_WIDTH+1] = val;
      val = (*(srcRow2+2)*1+*(srcRow2+3)*3+*(srcRow3+2)*3+*(srcRow3+3)*9)>>4;
      dst[TSQCIF_WIDTH+2] = val;
      srcRow0 +=4; srcRow1 +=4; srcRow2 +=4; srcRow3 +=4;
      dst+=3;
    }
//    dst += x;
    dst += TSQCIF_WIDTH;
    src += QCIF_WIDTH*4;
  }
  src=(unsigned char *)_src+CIF_WIDTH*CIF_HEIGHT+QCIF_WIDTH*QCIF_HEIGHT;

  // copy V
  for (y = TSQCIF_HEIGHT/2; y > 2; y-=3) {
    srcRow0 = src;
    srcRow1 = src + QCIF_WIDTH;
    srcRow2 = src + QCIF_WIDTH*2;
    srcRow3 = src + QCIF_WIDTH*3;
    for (x = TSQCIF_WIDTH/2; x > 2; x-=3) {
      val = (*srcRow0*9+*(srcRow0+1)*3+*srcRow1*3+*(srcRow1+1))>>4;
      dst[0] = val;
      val = (*(srcRow0+1)*6+*(srcRow0+2)*6+*(srcRow1+1)*2+*(srcRow1+2)*2)>>4;
      dst[1] = val;
      val = (*(srcRow0+2)*3+*(srcRow0+3)*9+*(srcRow1+2)+*(srcRow1+3)*3)>>4;
      dst[2] = val;
      val = (*srcRow1*6+*(srcRow1+1)*2+*srcRow2*6+*(srcRow2+1)*2)>>4;
      dst[TSQCIF_WIDTH/2] = val;
      val = (*(srcRow1+1)*4+*(srcRow1+2)*4+*(srcRow2+1)*4+*(srcRow2+2)*4)>>4;
      dst[TSQCIF_WIDTH/2+1] = val;
      val = (*(srcRow1+2)*2+*(srcRow1+3)*6+*(srcRow2+2)*2+*(srcRow2+3)*6)>>4;
      dst[TSQCIF_WIDTH/2+2] = val;
      val = (*srcRow2*3+*(srcRow2+1)*1+*srcRow3*9+*(srcRow3+1)*3)>>4;
      dst[TSQCIF_WIDTH] = val;
      val = (*(srcRow2+1)*2+*(srcRow2+2)*2+*(srcRow3+1)*6+*(srcRow3+2)*6)>>4;
      dst[TSQCIF_WIDTH+1] = val;
      val = (*(srcRow2+2)*1+*(srcRow2+3)*3+*(srcRow3+2)*3+*(srcRow3+3)*9)>>4;
      dst[TSQCIF_WIDTH+2] = val;
      srcRow0 +=4; srcRow1 +=4; srcRow2 +=4; srcRow3 +=4;
      dst+=3;
    }
//    if(x!=0) { dst[0]=*srcRow0; dst[Q3CIF16_WIDTH/2]=*srcRow2; dst++; }
    dst += TSQCIF_WIDTH;
    src += QCIF_WIDTH*4;
  }
}


void MCUVideoMixer::Convert2To1(const void * _src, void * _dst, unsigned int w, unsigned int h)
{
 if(w==CIF16_WIDTH && h==CIF16_HEIGHT) { ConvertCIF16ToCIF4(_src,_dst); return; }
 if(w==CIF4_WIDTH && h==CIF4_HEIGHT) { ConvertCIF4ToCIF(_src,_dst); return; }
 if(w==CIF_WIDTH && h==CIF_HEIGHT) { ConvertCIFToQCIF(_src,_dst); return; }
// if(w==QCIF_WIDTH && h=QCIF_HEIGHT) { ConvertQCIFToSQCIF(_src,_dst); return; }

  unsigned char * src = (unsigned char *)_src;
  unsigned char * dst = (unsigned char *)_dst;

  unsigned int y, x, val;
  unsigned char * srcRow0;
  unsigned char * srcRow1;

  // copy Y
  for (y = h>>1; y > 0; y--) {
    srcRow0 = src;
    srcRow1 = src + w;
    for (x = w>>1; x > 0; x--) {
      val = (*srcRow0+*(srcRow0+1)+*srcRow1+*(srcRow1+1))>>2;
      dst[0] = val;
      srcRow0 += 2; srcRow1 +=2;
      dst++;
    }
    src = srcRow1;
  }

  // copy U
  for (y = h>>2; y > 0; y--) {
    srcRow0 = src;
    srcRow1 = src + (w>>1);
    for (x = w>>2; x > 0; x--) {
      val = (*srcRow0+*(srcRow0+1)+*srcRow1+*(srcRow1+1))>>2;
      dst[0] = val;
      srcRow0 += 2; srcRow1 +=2;
      dst++;
    }
    src = srcRow1;
  }

  // copy V
  for (y = h>>2; y > 0; y--) {
    srcRow0 = src;
    srcRow1 = src + (w>>1);
    for (x = w>>2; x > 0; x--) {
      val = (*srcRow0+*(srcRow0+1)+*srcRow1+*(srcRow1+1))>>2;
      dst[0] = val;
      srcRow0 += 2; srcRow1 +=2;
      dst++;
    }
    src = srcRow1;
  }
}

void MCUVideoMixer::ConvertCIFToSQ3CIF(const void * _src, void * _dst)
{
  unsigned char * src = (unsigned char *)_src;
  unsigned char * dst = (unsigned char *)_dst;

  unsigned int y, x, val;
  unsigned char * srcRow0;
  unsigned char * srcRow1;
  unsigned char * srcRow2;

  // copy Y
  for (y = SQ3CIF_HEIGHT; y > 0; y--) {
    srcRow0 = src;
    srcRow1 = src + CIF_WIDTH;
    srcRow2 = src + CIF_WIDTH*2;
    for (x = SQ3CIF_WIDTH; x > 0; x--) {
      val = (*srcRow0+*(srcRow0+1)+*(srcRow0+2)
    	    +*srcRow1+*(srcRow1+1)+*(srcRow1+2)
    	    +*srcRow2+*(srcRow2+1)+*(srcRow2+2))*0.11111111111;
      dst[0] = val;
      srcRow0 += 3; srcRow1 +=3; srcRow2 +=3;
      dst++;
    }
    src += CIF_WIDTH*3;
  }

  // copy U
  for (y = SQ3CIF_HEIGHT/2; y > 0; y--) {
    srcRow0 = src;
    srcRow1 = src + QCIF_WIDTH;
    srcRow2 = src + QCIF_WIDTH*2;
    for (x = SQ3CIF_WIDTH/2; x > 0; x--) {
      val = (*srcRow0+*(srcRow0+1)+*(srcRow0+2)
    	    +*srcRow1+*(srcRow1+1)+*(srcRow1+2)
    	    +*srcRow2+*(srcRow2+1)+*(srcRow2+2))*0.11111111111;
      dst[0] = val;
      srcRow0 += 3; srcRow1 +=3; srcRow2 +=3;
      dst++;
    }
    src += QCIF_WIDTH*3;
  }

  // copy V
  for (y = SQ3CIF_HEIGHT/2; y > 0; y--) {
    srcRow0 = src;
    srcRow1 = src + QCIF_WIDTH;
    srcRow2 = src + QCIF_WIDTH*2;
    for (x = SQ3CIF_WIDTH/2; x > 0; x--) {
      val = (*srcRow0+*(srcRow0+1)+*(srcRow0+2)
    	    +*srcRow1+*(srcRow1+1)+*(srcRow1+2)
    	    +*srcRow2+*(srcRow2+1)+*(srcRow2+2))*0.11111111111;
      dst[0] = val;
      srcRow0 += 3; srcRow1 +=3; srcRow2 +=3;
      dst++;
    }
    src += QCIF_WIDTH*3;
  }
}

void MCUVideoMixer::ConvertCIF4ToQ3CIF(const void * _src, void * _dst)
{
  unsigned char * src = (unsigned char *)_src;
  unsigned char * dst = (unsigned char *)_dst;

  unsigned int y, x, val;
  unsigned char * srcRow0;
  unsigned char * srcRow1;
  unsigned char * srcRow2;

  // copy Y
  for (y = Q3CIF_HEIGHT; y > 0; y--) {
    srcRow0 = src;
    srcRow1 = src + CIF4_WIDTH;
    srcRow2 = src + CIF4_WIDTH*2;
    for (x = Q3CIF_WIDTH; x > 0; x--) {
      val = (*srcRow0+*(srcRow0+1)+*(srcRow0+2)
    	    +*srcRow1+*(srcRow1+1)+*(srcRow1+2)
    	    +*srcRow2+*(srcRow2+1)+*(srcRow2+2))*0.11111111111;
      dst[0] = val;
      srcRow0 += 3; srcRow1 +=3; srcRow2 +=3;
      dst++;
    }
    src += CIF4_WIDTH*3;
  }

  // copy U
  for (y = Q3CIF_HEIGHT/2; y > 0; y--) {
    srcRow0 = src;
    srcRow1 = src + CIF_WIDTH;
    srcRow2 = src + CIF_WIDTH*2;
    for (x = Q3CIF_WIDTH/2; x > 0; x--) {
      val = (*srcRow0+*(srcRow0+1)+*(srcRow0+2)
    	    +*srcRow1+*(srcRow1+1)+*(srcRow1+2)
    	    +*srcRow2+*(srcRow2+1)+*(srcRow2+2))*0.11111111111;
      dst[0] = val;
      srcRow0 += 3; srcRow1 +=3; srcRow2 +=3;
      dst++;
    }
    src += CIF_WIDTH*3;
  }

  // copy V
  for (y = Q3CIF_HEIGHT/2; y > 0; y--) {
    srcRow0 = src;
    srcRow1 = src + CIF_WIDTH;
    srcRow2 = src + CIF_WIDTH*2;
    for (x = Q3CIF_WIDTH/2; x > 0; x--) {
      val = (*srcRow0+*(srcRow0+1)+*(srcRow0+2)
    	    +*srcRow1+*(srcRow1+1)+*(srcRow1+2)
    	    +*srcRow2+*(srcRow2+1)+*(srcRow2+2))*0.11111111111;
      dst[0] = val;
      srcRow0 += 3; srcRow1 +=3; srcRow2 +=3;
      dst++;
    }
    src += CIF_WIDTH*3;
  }
}

void MCUVideoMixer::ConvertCIF16ToQ3CIF4(const void * _src, void * _dst)
{
  unsigned char * src = (unsigned char *)_src;
  unsigned char * dst = (unsigned char *)_dst;

  unsigned int y, x, val;
  unsigned char * srcRow0;
  unsigned char * srcRow1;
  unsigned char * srcRow2;

  // copy Y
  for (y = Q3CIF4_HEIGHT; y > 0; y--) {
    srcRow0 = src;
    srcRow1 = src + CIF16_WIDTH;
    srcRow2 = src + CIF16_WIDTH*2;
    for (x = Q3CIF4_WIDTH; x > 0; x--) {
      val = (*srcRow0+*(srcRow0+1)+*(srcRow0+2)
    	    +*srcRow1+*(srcRow1+1)+*(srcRow1+2)
    	    +*srcRow2+*(srcRow2+1)+*(srcRow2+2))*0.11111111111;
      dst[0] = val;
      srcRow0 += 3; srcRow1 +=3; srcRow2 +=3;
      dst++;
    }
    src += CIF16_WIDTH*3;
  }

  // copy U
  for (y = Q3CIF4_HEIGHT/2; y > 0; y--) {
    srcRow0 = src;
    srcRow1 = src + CIF4_WIDTH;
    srcRow2 = src + CIF4_WIDTH*2;
    for (x = Q3CIF4_WIDTH/2; x > 0; x--) {
      val = (*srcRow0+*(srcRow0+1)+*(srcRow0+2)
    	    +*srcRow1+*(srcRow1+1)+*(srcRow1+2)
    	    +*srcRow2+*(srcRow2+1)+*(srcRow2+2))*0.11111111111;
      dst[0] = val;
      srcRow0 += 3; srcRow1 +=3; srcRow2 +=3;
      dst++;
    }
    src += CIF4_WIDTH*3;
  }

  // copy V
  for (y = Q3CIF4_HEIGHT/2; y > 0; y--) {
    srcRow0 = src;
    srcRow1 = src + CIF4_WIDTH;
    srcRow2 = src + CIF4_WIDTH*2;
    for (x = Q3CIF4_WIDTH/2; x > 0; x--) {
      val = (*srcRow0+*(srcRow0+1)+*(srcRow0+2)
    	    +*srcRow1+*(srcRow1+1)+*(srcRow1+2)
    	    +*srcRow2+*(srcRow2+1)+*(srcRow2+2))*0.11111111111;
      dst[0] = val;
      srcRow0 += 3; srcRow1 +=3; srcRow2 +=3;
      dst++;
    }
    src += CIF4_WIDTH*3;
  }
}

void MCUVideoMixer::ConvertCIFToSQCIF(const void * _src, void * _dst)
{
  unsigned char * src = (unsigned char *)_src;
  unsigned char * dst = (unsigned char *)_dst;

  unsigned int y, x, val;
  unsigned char * srcRow0;
  unsigned char * srcRow1;
  unsigned char * srcRow2;
  unsigned char * srcRow3;

  // copy Y
  for (y = SQCIF_HEIGHT; y > 0; y--) {
    srcRow0 = src;
    srcRow1 = src + CIF_WIDTH;
    srcRow2 = src + CIF_WIDTH*2;
    srcRow3 = src + CIF_WIDTH*3;
    for (x = SQCIF_WIDTH; x > 0; x--) {
      val = (*srcRow0+*(srcRow0+1)+*(srcRow0+2)+*(srcRow0+3)
    	    +*srcRow1+*(srcRow1+1)+*(srcRow1+2)+*(srcRow1+3)
    	    +*srcRow2+*(srcRow2+1)+*(srcRow2+2)+*(srcRow2+3)
    	    +*srcRow3+*(srcRow3+1)+*(srcRow3+2)+*(srcRow3+3))>>4;
      dst[0] = val;
      srcRow0 += 4; srcRow1 +=4; srcRow2 +=4; srcRow3 +=4;
      dst++;
    }
    src += CIF_WIDTH*4;
  }

  // copy U
  for (y = SQCIF_HEIGHT/2; y > 0; y--) {
    srcRow0 = src;
    srcRow1 = src + QCIF_WIDTH;
    srcRow2 = src + QCIF_WIDTH*2;
    srcRow3 = src + QCIF_WIDTH*3;
    for (x = SQCIF_WIDTH/2; x > 0; x--) {
      val = (*srcRow0+*(srcRow0+1)+*(srcRow0+2)+*(srcRow0+3)
    	    +*srcRow1+*(srcRow1+1)+*(srcRow1+2)+*(srcRow1+3)
    	    +*srcRow2+*(srcRow2+1)+*(srcRow2+2)+*(srcRow2+3)
    	    +*srcRow3+*(srcRow3+1)+*(srcRow3+2)+*(srcRow3+3))>>4;
      dst[0] = val;
      srcRow0 += 4; srcRow1 +=4; srcRow2 +=4; srcRow3 +=4;
      dst++;
    }
    src += QCIF_WIDTH*4;
  }

  // copy V
  for (y = SQCIF_HEIGHT/2; y > 0; y--) {
    srcRow0 = src;
    srcRow1 = src + QCIF_WIDTH;
    srcRow2 = src + QCIF_WIDTH*2;
    srcRow3 = src + QCIF_WIDTH*3;
    for (x = SQCIF_WIDTH/2; x > 0; x--) {
      val = (*srcRow0+*(srcRow0+1)+*(srcRow0+2)+*(srcRow0+3)
    	    +*srcRow1+*(srcRow1+1)+*(srcRow1+2)+*(srcRow1+3)
    	    +*srcRow2+*(srcRow2+1)+*(srcRow2+2)+*(srcRow2+3)
    	    +*srcRow3+*(srcRow3+1)+*(srcRow3+2)+*(srcRow3+3))>>4;
      dst[0] = val;
      srcRow0 += 4; srcRow1 +=4; srcRow2 +=4; srcRow3 +=4;
      dst++;
    }
    src += QCIF_WIDTH*4;
  }
}

void MCUVideoMixer::ConvertCIF4ToQCIF(const void * _src, void * _dst)
{
  unsigned char * src = (unsigned char *)_src;
  unsigned char * dst = (unsigned char *)_dst;

  unsigned int y, x, val;
  unsigned char * srcRow0;
  unsigned char * srcRow1;
  unsigned char * srcRow2;
  unsigned char * srcRow3;

  // copy Y
  for (y = QCIF_HEIGHT; y > 0; y--) {
    srcRow0 = src;
    srcRow1 = src + CIF4_WIDTH;
    srcRow2 = src + CIF4_WIDTH*2;
    srcRow3 = src + CIF4_WIDTH*3;
    for (x = QCIF_WIDTH; x > 0; x--) {
      val = (*srcRow0+*(srcRow0+1)+*(srcRow0+2)+*(srcRow0+3)
    	    +*srcRow1+*(srcRow1+1)+*(srcRow1+2)+*(srcRow1+3)
    	    +*srcRow2+*(srcRow2+1)+*(srcRow2+2)+*(srcRow2+3)
    	    +*srcRow3+*(srcRow3+1)+*(srcRow3+2)+*(srcRow3+3))>>4;
      dst[0] = val;
      srcRow0 += 4; srcRow1 +=4; srcRow2 +=4; srcRow3 +=4;
      dst++;
    }
    src += CIF4_WIDTH*4;
  }

  // copy U
  for (y = QCIF_HEIGHT/2; y > 0; y--) {
    srcRow0 = src;
    srcRow1 = src + CIF_WIDTH;
    srcRow2 = src + CIF_WIDTH*2;
    srcRow3 = src + CIF_WIDTH*3;
    for (x = QCIF_WIDTH/2; x > 0; x--) {
      val = (*srcRow0+*(srcRow0+1)+*(srcRow0+2)+*(srcRow0+3)
    	    +*srcRow1+*(srcRow1+1)+*(srcRow1+2)+*(srcRow1+3)
    	    +*srcRow2+*(srcRow2+1)+*(srcRow2+2)+*(srcRow2+3)
    	    +*srcRow3+*(srcRow3+1)+*(srcRow3+2)+*(srcRow3+3))>>4;
      dst[0] = val;
      srcRow0 += 4; srcRow1 +=4; srcRow2 +=4; srcRow3 +=4;
      dst++;
    }
    src += CIF_WIDTH*4;
  }

  // copy V
  for (y = QCIF_HEIGHT/2; y > 0; y--) {
    srcRow0 = src;
    srcRow1 = src + CIF_WIDTH;
    srcRow2 = src + CIF_WIDTH*2;
    srcRow3 = src + CIF_WIDTH*3;
    for (x = QCIF_WIDTH/2; x > 0; x--) {
      val = (*srcRow0+*(srcRow0+1)+*(srcRow0+2)+*(srcRow0+3)
    	    +*srcRow1+*(srcRow1+1)+*(srcRow1+2)+*(srcRow1+3)
    	    +*srcRow2+*(srcRow2+1)+*(srcRow2+2)+*(srcRow2+3)
    	    +*srcRow3+*(srcRow3+1)+*(srcRow3+2)+*(srcRow3+3))>>4;
      dst[0] = val;
      srcRow0 += 4; srcRow1 +=4; srcRow2 +=4; srcRow3 +=4;
      dst++;
    }
    src += CIF_WIDTH*4;
  }
}

void MCUVideoMixer::ConvertCIF16ToCIF(const void * _src, void * _dst)
{
  unsigned char * src = (unsigned char *)_src;
  unsigned char * dst = (unsigned char *)_dst;

  unsigned int y, x, val;
  unsigned char * srcRow0;
  unsigned char * srcRow1;
  unsigned char * srcRow2;
  unsigned char * srcRow3;

  // copy Y
  for (y = CIF_HEIGHT; y > 0; y--) {
    srcRow0 = src;
    srcRow1 = src + CIF16_WIDTH;
    srcRow2 = src + CIF16_WIDTH*2;
    srcRow3 = src + CIF16_WIDTH*3;
    for (x = CIF_WIDTH; x > 0; x--) {
      val = (*srcRow0+*(srcRow0+1)+*(srcRow0+2)+*(srcRow0+3)
    	    +*srcRow1+*(srcRow1+1)+*(srcRow1+2)+*(srcRow1+3)
    	    +*srcRow2+*(srcRow2+1)+*(srcRow2+2)+*(srcRow2+3)
    	    +*srcRow3+*(srcRow3+1)+*(srcRow3+2)+*(srcRow3+3))>>4;
      dst[0] = val;
      srcRow0 += 4; srcRow1 +=4; srcRow2 +=4; srcRow3 +=4;
      dst++;
    }
    src += CIF16_WIDTH*4;
  }

  // copy U
  for (y = CIF_HEIGHT/2; y > 0; y--) {
    srcRow0 = src;
    srcRow1 = src + CIF4_WIDTH;
    srcRow2 = src + CIF4_WIDTH*2;
    srcRow3 = src + CIF4_WIDTH*3;
    for (x = CIF_WIDTH/2; x > 0; x--) {
      val = (*srcRow0+*(srcRow0+1)+*(srcRow0+2)+*(srcRow0+3)
    	    +*srcRow1+*(srcRow1+1)+*(srcRow1+2)+*(srcRow1+3)
    	    +*srcRow2+*(srcRow2+1)+*(srcRow2+2)+*(srcRow2+3)
    	    +*srcRow3+*(srcRow3+1)+*(srcRow3+2)+*(srcRow3+3))>>4;
      dst[0] = val;
      srcRow0 += 4; srcRow1 +=4; srcRow2 +=4; srcRow3 +=4;
      dst++;
    }
    src += CIF4_WIDTH*4;
  }

  // copy V
  for (y = CIF_HEIGHT/2; y > 0; y--) {
    srcRow0 = src;
    srcRow1 = src + CIF4_WIDTH;
    srcRow2 = src + CIF4_WIDTH*2;
    srcRow3 = src + CIF4_WIDTH*3;
    for (x = CIF_WIDTH/2; x > 0; x--) {
      val = (*srcRow0+*(srcRow0+1)+*(srcRow0+2)+*(srcRow0+3)
    	    +*srcRow1+*(srcRow1+1)+*(srcRow1+2)+*(srcRow1+3)
    	    +*srcRow2+*(srcRow2+1)+*(srcRow2+2)+*(srcRow2+3)
    	    +*srcRow3+*(srcRow3+1)+*(srcRow3+2)+*(srcRow3+3))>>4;
      dst[0] = val;
      srcRow0 += 4; srcRow1 +=4; srcRow2 +=4; srcRow3 +=4;
      dst++;
    }
    src += CIF4_WIDTH*4;
  }
}

void MCUVideoMixer::ConvertFRAMEToCUSTOM_FRAME(const void * _src, void * _dst, unsigned int sw, unsigned int sh, unsigned int dw, unsigned int dh)
{
 BYTE * src = (BYTE *)_src;
 BYTE * dst = (BYTE *)_dst;

 //BYTE * dstEnd = dst + CIF_SIZE;
 int y, x, cx, cy;
 BYTE * srcRow;

  // copy Y
  cy=-dh;
  for (y = dh; y > 0; y--) {
    srcRow = src; cx=-dw;
    for (x = dw; x > 0; x--) {
      *dst = *srcRow;
      cx+=sw; while(cx>=0) { cx-=dw; srcRow++; }
      dst++;
    }
    cy+=sh; while(cy>=0) { cy-=dh; src+=sw; }
  }
  // copy U
  src=(BYTE *)_src+(sw*sh);
  cy=-dh;
  for (y = dh/2; y > 0; y--) {
    srcRow = src; cx=-dw;
    for (x = (dw>>1); x > 0; x--) {
      *dst = *srcRow;
      cx+=sw; while(cx>=0) { cx-=dw; srcRow++; }
      dst++;
    }
    cy+=sh; while(cy>=0) { cy-=dh; src+=(sw>>1); }
  }

  // copy V
  src=(BYTE *)_src+(sw*sh)+((sw/2)*(sh/2));
  cy=-dh;
  for (y = dh/2; y > 0; y--) {
    srcRow = src; cx=-dw;
    for (x = (dw>>1); x > 0; x--) {
      *dst = *srcRow;
      cx+=sw; while(cx>=0) { cx-=dw; srcRow++; }
      dst++;
    }
    cy+=sh; while(cy>=0) { cy-=dh; src+=(sw>>1); }
  }

}

void MCUVideoMixer::ConvertQCIFToCIF(const void * _src, void * _dst)
{
  BYTE * src = (BYTE *)_src;
  BYTE * dst = (BYTE *)_dst;

  //BYTE * dstEnd = dst + CIF_SIZE;
  int y, x;
  BYTE * srcRow;

  // copy Y
  srcRow = src;
  for (y = 1; y < QCIF_HEIGHT; y++) 
  {
    for (x = 1; x < QCIF_WIDTH; x++) 
    {
      dst[0] = srcRow[0];
      dst[1] = (srcRow[0]+srcRow[1])>>1;
      dst[QCIF_WIDTH*2] = (srcRow[0]+srcRow[QCIF_WIDTH])>>1;
      dst[QCIF_WIDTH*2+1] = (srcRow[0]+srcRow[1]+srcRow[QCIF_WIDTH]+srcRow[QCIF_WIDTH+1])>>2;
      dst+=2; srcRow++;
    }
    dst[0] = dst[1] = srcRow[0];
    dst[QCIF_WIDTH*2] = dst[QCIF_WIDTH*2+1] = (srcRow[0]+srcRow[QCIF_WIDTH])>>1;
    srcRow++; dst += 2; dst += QCIF_WIDTH*2;
  }
  for (x = 1; x < QCIF_WIDTH; x++) 
  {
   dst[0] = dst[QCIF_WIDTH*2] = srcRow[0];
   dst[1] = dst[QCIF_WIDTH*2+1] = (srcRow[0]+srcRow[1])>>1;
   dst+=2; srcRow++;
  }
  dst[0] = dst[1] = dst[QCIF_WIDTH*2] = dst[QCIF_WIDTH*2+1] = srcRow[0];
  srcRow++; dst += 2; dst += QCIF_WIDTH*2;

  for (y = 1; y < QCIF_HEIGHT/2; y++) 
  {
    for (x = 1; x < QCIF_WIDTH/2; x++) 
    {
      dst[0] = srcRow[0];
      dst[1] = (srcRow[0]+srcRow[1])>>1;
      dst[QCIF_WIDTH] = (srcRow[0]+srcRow[QCIF_WIDTH/2])>>1;
      dst[QCIF_WIDTH+1] = (srcRow[0]+srcRow[1]+srcRow[QCIF_WIDTH/2]+srcRow[QCIF_WIDTH/2+1])>>2;
      dst+=2; srcRow++;
    }
    dst[0] = dst[1] = srcRow[0];
    dst[QCIF_WIDTH] = dst[QCIF_WIDTH+1] = (srcRow[0]+srcRow[QCIF_WIDTH/2])>>1;
    srcRow++; dst += 2; dst += QCIF_WIDTH;
  }
  for (x = 1; x < QCIF_WIDTH/2; x++) 
  {
   dst[0] = dst[QCIF_WIDTH] = srcRow[0];
   dst[1] = dst[QCIF_WIDTH+1] = (srcRow[0]+srcRow[1])>>1;
   dst+=2; srcRow++;
  }
  dst[0] = dst[1] = dst[QCIF_WIDTH] = dst[QCIF_WIDTH+1] = srcRow[0];
  srcRow++; dst += 2; dst += QCIF_WIDTH;

  for (y = 1; y < QCIF_HEIGHT/2; y++) 
  {
    for (x = 1; x < QCIF_WIDTH/2; x++) 
    {
      dst[0] = srcRow[0];
      dst[1] = (srcRow[0]+srcRow[1])>>1;
      dst[QCIF_WIDTH] = (srcRow[0]+srcRow[QCIF_WIDTH/2])>>1;
      dst[QCIF_WIDTH+1] = (srcRow[0]+srcRow[1]+srcRow[QCIF_WIDTH/2]+srcRow[QCIF_WIDTH/2+1])>>2;
      dst+=2; srcRow++;
    }
    dst[0] = dst[1] = srcRow[0];
    dst[QCIF_WIDTH] = dst[QCIF_WIDTH+1] = (srcRow[0]+srcRow[QCIF_WIDTH/2])>>1;
    srcRow++; dst += 2; dst += QCIF_WIDTH;
  }
  for (x = 1; x < QCIF_WIDTH/2; x++) 
  {
   dst[0] = dst[QCIF_WIDTH] = srcRow[0];
   dst[1] = dst[QCIF_WIDTH+1] = (srcRow[0]+srcRow[1])>>1;
   dst+=2; srcRow++;
  }
  dst[0] = dst[1] = dst[QCIF_WIDTH] = dst[QCIF_WIDTH+1] = srcRow[0];
  srcRow++; dst += 2; dst += QCIF_WIDTH;
}


void MCUVideoMixer::ConvertCIFToCIF4(const void * _src, void * _dst)
{
  BYTE * src = (BYTE *)_src;
  BYTE * dst = (BYTE *)_dst;

  //BYTE * dstEnd = dst + CIF_SIZE;
  int y, x;
  BYTE * srcRow;

  // copy Y
  srcRow = src;
  for (y = 1; y < CIF_HEIGHT; y++) 
  {
    for (x = 1; x < CIF_WIDTH; x++) 
    {
      dst[0] = srcRow[0];
      dst[1] = (srcRow[0]+srcRow[1])>>1;
      dst[CIF_WIDTH*2] = (srcRow[0]+srcRow[CIF_WIDTH])>>1;
      dst[CIF_WIDTH*2+1] = (srcRow[0]+srcRow[1]+srcRow[CIF_WIDTH]+srcRow[CIF_WIDTH+1])>>2;
      dst+=2; srcRow++;
    }
    dst[0] = dst[1] = srcRow[0];
    dst[CIF_WIDTH*2] = dst[CIF_WIDTH*2+1] = (srcRow[0]+srcRow[CIF_WIDTH])>>1;
    srcRow++; dst += 2; dst += CIF_WIDTH*2;
  }
  for (x = 1; x < CIF_WIDTH; x++) 
  {
   dst[0] = dst[CIF_WIDTH*2] = srcRow[0];
   dst[1] = dst[CIF_WIDTH*2+1] = (srcRow[0]+srcRow[1])>>1;
   dst+=2; srcRow++;
  }
  dst[0] = dst[1] = dst[CIF_WIDTH*2] = dst[CIF_WIDTH*2+1] = srcRow[0];
  srcRow++; dst += 2; dst += CIF_WIDTH*2;

  for (y = 1; y < CIF_HEIGHT/2; y++) 
  {
    for (x = 1; x < CIF_WIDTH/2; x++) 
    {
      dst[0] = srcRow[0];
      dst[1] = (srcRow[0]+srcRow[1])>>1;
      dst[CIF_WIDTH] = (srcRow[0]+srcRow[CIF_WIDTH/2])>>1;
      dst[CIF_WIDTH+1] = (srcRow[0]+srcRow[1]+srcRow[CIF_WIDTH/2]+srcRow[CIF_WIDTH/2+1])>>2;
      dst+=2; srcRow++;
    }
    dst[0] = dst[1] = srcRow[0];
    dst[CIF_WIDTH] = dst[CIF_WIDTH+1] = (srcRow[0]+srcRow[CIF_WIDTH/2])>>1;
    srcRow++; dst += 2; dst += CIF_WIDTH;
  }
  for (x = 1; x < CIF_WIDTH/2; x++) 
  {
   dst[0] = dst[CIF_WIDTH] = srcRow[0];
   dst[1] = dst[CIF_WIDTH+1] = (srcRow[0]+srcRow[1])>>1;
   dst+=2; srcRow++;
  }
  dst[0] = dst[1] = dst[CIF_WIDTH] = dst[CIF_WIDTH+1] = srcRow[0];
  srcRow++; dst += 2; dst += CIF_WIDTH;

  for (y = 1; y < CIF_HEIGHT/2; y++) 
  {
    for (x = 1; x < CIF_WIDTH/2; x++) 
    {
      dst[0] = srcRow[0];
      dst[1] = (srcRow[0]+srcRow[1])>>1;
      dst[CIF_WIDTH] = (srcRow[0]+srcRow[CIF_WIDTH/2])>>1;
      dst[CIF_WIDTH+1] = (srcRow[0]+srcRow[1]+srcRow[CIF_WIDTH/2]+srcRow[CIF_WIDTH/2+1])>>2;
      dst+=2; srcRow++;
    }
    dst[0] = dst[1] = srcRow[0];
    dst[CIF_WIDTH] = dst[CIF_WIDTH+1] = (srcRow[0]+srcRow[CIF_WIDTH/2])>>1;
    srcRow++; dst += 2; dst += CIF_WIDTH;
  }
  for (x = 1; x < CIF_WIDTH/2; x++) 
  {
   dst[0] = dst[CIF_WIDTH] = srcRow[0];
   dst[1] = dst[CIF_WIDTH+1] = (srcRow[0]+srcRow[1])>>1;
   dst+=2; srcRow++;
  }
  dst[0] = dst[1] = dst[CIF_WIDTH] = dst[CIF_WIDTH+1] = srcRow[0];
  srcRow++; dst += 2; dst += CIF_WIDTH;
}

void MCUVideoMixer::ConvertCIF4ToCIF16(const void * _src, void * _dst)
{
  BYTE * src = (BYTE *)_src;
  BYTE * dst = (BYTE *)_dst;

  //BYTE * dstEnd = dst + CIF_SIZE;
  int y, x;
  BYTE * srcRow;

  // copy Y
  srcRow = src;
  for (y = 1; y < CIF4_HEIGHT; y++) 
  {
    for (x = 1; x < CIF4_WIDTH; x++) 
    {
      dst[0] = srcRow[0];
      dst[1] = (srcRow[0]+srcRow[1])>>1;
      dst[CIF4_WIDTH*2] = (srcRow[0]+srcRow[CIF4_WIDTH])>>1;
      dst[CIF4_WIDTH*2+1] = (srcRow[0]+srcRow[1]+srcRow[CIF4_WIDTH]+srcRow[CIF4_WIDTH+1])>>2;
      dst+=2; srcRow++;
    }
    dst[0] = dst[1] = srcRow[0];
    dst[CIF4_WIDTH*2] = dst[CIF4_WIDTH*2+1] = (srcRow[0]+srcRow[CIF4_WIDTH])>>1;
    srcRow++; dst += 2; dst += CIF4_WIDTH*2;
  }
  for (x = 1; x < CIF4_WIDTH; x++) 
  {
   dst[0] = dst[CIF4_WIDTH*2] = srcRow[0];
   dst[1] = dst[CIF4_WIDTH*2+1] = (srcRow[0]+srcRow[1])>>1;
   dst+=2; srcRow++;
  }
  dst[0] = dst[1] = dst[CIF4_WIDTH*2] = dst[CIF4_WIDTH*2+1] = srcRow[0];
  srcRow++; dst += 2; dst += CIF4_WIDTH*2;

  for (y = 1; y < CIF4_HEIGHT/2; y++) 
  {
    for (x = 1; x < CIF4_WIDTH/2; x++) 
    {
      dst[0] = srcRow[0];
      dst[1] = (srcRow[0]+srcRow[1])>>1;
      dst[CIF4_WIDTH] = (srcRow[0]+srcRow[CIF4_WIDTH/2])>>1;
      dst[CIF4_WIDTH+1] = (srcRow[0]+srcRow[1]+srcRow[CIF4_WIDTH/2]+srcRow[CIF4_WIDTH/2+1])>>2;
      dst+=2; srcRow++;
    }
    dst[0] = dst[1] = srcRow[0];
    dst[CIF4_WIDTH] = dst[CIF4_WIDTH+1] = (srcRow[0]+srcRow[CIF4_WIDTH/2])>>1;
    srcRow++; dst += 2; dst += CIF4_WIDTH;
  }
  for (x = 1; x < CIF4_WIDTH/2; x++) 
  {
   dst[0] = dst[CIF4_WIDTH] = srcRow[0];
   dst[1] = dst[CIF4_WIDTH+1] = (srcRow[0]+srcRow[1])>>1;
   dst+=2; srcRow++;
  }
  dst[0] = dst[1] = dst[CIF4_WIDTH] = dst[CIF4_WIDTH+1] = srcRow[0];
  srcRow++; dst += 2; dst += CIF4_WIDTH;

  for (y = 1; y < CIF4_HEIGHT/2; y++) 
  {
    for (x = 1; x < CIF4_WIDTH/2; x++) 
    {
      dst[0] = srcRow[0];
      dst[1] = (srcRow[0]+srcRow[1])>>1;
      dst[CIF4_WIDTH] = (srcRow[0]+srcRow[CIF4_WIDTH/2])>>1;
      dst[CIF4_WIDTH+1] = (srcRow[0]+srcRow[1]+srcRow[CIF4_WIDTH/2]+srcRow[CIF4_WIDTH/2+1])>>2;
      dst+=2; srcRow++;
    }
    dst[0] = dst[1] = srcRow[0];
    dst[CIF4_WIDTH] = dst[CIF4_WIDTH+1] = (srcRow[0]+srcRow[CIF4_WIDTH/2])>>1;
    srcRow++; dst += 2; dst += CIF4_WIDTH;
  }
  for (x = 1; x < CIF4_WIDTH/2; x++) 
  {
   dst[0] = dst[CIF4_WIDTH] = srcRow[0];
   dst[1] = dst[CIF4_WIDTH+1] = (srcRow[0]+srcRow[1])>>1;
   dst+=2; srcRow++;
  }
  dst[0] = dst[1] = dst[CIF4_WIDTH] = dst[CIF4_WIDTH+1] = srcRow[0];
  srcRow++; dst += 2; dst += CIF4_WIDTH;
}

void MCUVideoMixer::Convert1To2(const void * _src, void * _dst, unsigned int w, unsigned int h)
{
  BYTE * src = (BYTE *)_src;
  BYTE * dst = (BYTE *)_dst;

  if(w==QCIF_WIDTH && h==QCIF_HEIGHT) ConvertQCIFToCIF(_src,_dst);
  if(w==CIF_WIDTH && h==CIF_HEIGHT) ConvertCIFToCIF4(_src,_dst);
  if(w==CIF4_WIDTH && h==CIF4_HEIGHT) ConvertCIF4ToCIF16(_src,_dst);

  unsigned int y,x,w2=w*2;
  BYTE * srcRow;

  // copy Y
  srcRow = src;
  for (y = 1; y < h; y++) 
  {
    for (x = 1; x < w; x++) 
    {
      dst[0] = srcRow[0];
      dst[1] = (srcRow[0]+srcRow[1])>>1;
      dst[w2] = (srcRow[0]+srcRow[w])>>1;
      dst[w2+1] = (srcRow[0]+srcRow[1]+srcRow[w]+srcRow[w+1])>>2;
      dst+=2; srcRow++;
    }
    dst[0] = dst[1] = srcRow[0];
    dst[w2] = dst[w2+1] = (srcRow[0]+srcRow[w])>>1;
    srcRow++; dst += 2; dst += w2;
  }
  for (x = 1; x < w; x++) 
  {
   dst[0] = dst[w2] = srcRow[0];
   dst[1] = dst[w2+1] = (srcRow[0]+srcRow[1])>>1;
   dst+=2; srcRow++;
  }
  dst[0] = dst[1] = dst[w2] = dst[w2+1] = srcRow[0];
  srcRow++; dst += 2; dst += w2;

  w2=w>>1;
  for (y = 1; y < (h>>1); y++) 
  {
    for (x = 1; x < w2; x++) 
    {
      dst[0] = srcRow[0];
      dst[1] = (srcRow[0]+srcRow[1])>>1;
      dst[w] = (srcRow[0]+srcRow[w2])>>1;
      dst[w+1] = (srcRow[0]+srcRow[1]+srcRow[w2]+srcRow[w2+1])>>2;
      dst+=2; srcRow++;
    }
    dst[0] = dst[1] = srcRow[0];
    dst[w] = dst[w+1] = (srcRow[0]+srcRow[w2])>>1;
    srcRow++; dst += 2; dst += w;
  }
  for (x = 1; x < w2; x++) 
  {
   dst[0] = dst[w] = srcRow[0];
   dst[1] = dst[w+1] = (srcRow[0]+srcRow[1])>>1;
   dst+=2; srcRow++;
  }
  dst[0] = dst[1] = dst[w] = dst[w+1] = srcRow[0];
  srcRow++; dst += 2; dst += w;

  for (y = 1; y < (h>>1); y++) 
  {
    for (x = 1; x < w2; x++) 
    {
      dst[0] = srcRow[0];
      dst[1] = (srcRow[0]+srcRow[1])>>1;
      dst[w] = (srcRow[0]+srcRow[w2])>>1;
      dst[w+1] = (srcRow[0]+srcRow[1]+srcRow[w2]+srcRow[w2+1])>>2;
      dst+=2; srcRow++;
    }
    dst[0] = dst[1] = srcRow[0];
    dst[w] = dst[w+1] = (srcRow[0]+srcRow[w2])>>1;
    srcRow++; dst += 2; dst += w;
  }
  for (x = 1; x < w2; x++) 
  {
   dst[0] = dst[w] = srcRow[0];
   dst[1] = dst[w+1] = (srcRow[0]+srcRow[1])>>1;
   dst+=2; srcRow++;
  }
  dst[0] = dst[1] = dst[w] = dst[w+1] = srcRow[0];
  srcRow++; dst += 2; dst += w;
}


void MCUVideoMixer::ConvertCIFToTQCIF(const void * _src, void * _dst)
{
  BYTE * src = (BYTE *)_src;
  BYTE * dst = (BYTE *)_dst;

  //BYTE * dstEnd = dst + CIF_SIZE;
  int y, x;
  unsigned int sum;
  BYTE * srcRow;

  // copy Y
  srcRow = src;
  for (y = 0; y < CIF_HEIGHT/2; y++) {
    for (x = 0; x < CIF_WIDTH/2; x++) {
      sum = *srcRow; dst[0] = *srcRow++;
      sum+= *srcRow; dst[2] = *srcRow++;
      dst[1] = (sum >> 1);
      dst += 3;
    }
    dst += CIF_WIDTH*3/2;
    for (x = 0; x < CIF_WIDTH/2; x++) {
      sum = *srcRow; dst[0] = *srcRow++;
      sum+= *srcRow; dst[2] = *srcRow++;
      dst[1] = (sum >> 1);
      dst += 3;
    }
    dst -= CIF_WIDTH*9/2;
    for (x = 0; x < CIF_WIDTH*3/2; x++) {
      sum = dst[0]+dst[CIF_WIDTH*3];
      dst[CIF_WIDTH*3/2] = (sum >> 1);
      dst++;
    }
   dst+=CIF_WIDTH*3;
  }

  // copy U
  for (y = 0; y < CIF_HEIGHT/4; y++) {
    for (x = 0; x < CIF_WIDTH/4; x++) {
      sum = *srcRow; dst[0] = *srcRow++;
      sum+= *srcRow; dst[2] = *srcRow++;
      dst[1] = (sum >> 1);
      dst += 3;
    }
    dst += CIF_WIDTH*3/4;
    for (x = 0; x < CIF_WIDTH/4; x++) {
      sum = *srcRow; dst[0] = *srcRow++;
      sum+= *srcRow; dst[2] = *srcRow++;
      dst[1] = (sum >> 1);
      dst += 3;
    }
    dst -= CIF_WIDTH*9/4;
    for (x = 0; x < CIF_WIDTH*3/4; x++) {
      sum = dst[0]+dst[CIF_WIDTH*3/2];
      dst[CIF_WIDTH*3/4] = (sum >> 1);
      dst++;
    }
   dst+=CIF_WIDTH*3/2;
  }

  // copy V
  for (y = 0; y < CIF_HEIGHT/4; y++) {
    for (x = 0; x < CIF_WIDTH/4; x++) {
      sum = *srcRow; dst[0] = *srcRow++;
      sum+= *srcRow; dst[2] = *srcRow++;
      dst[1] = (sum >> 1);
      dst += 3;
    }
    dst += CIF_WIDTH*3/4;
    for (x = 0; x < CIF_WIDTH/4; x++) {
      sum = *srcRow; dst[0] = *srcRow++;
      sum+= *srcRow; dst[2] = *srcRow++;
      dst[1] = (sum >> 1);
      dst += 3;
    }
    dst -= CIF_WIDTH*9/4;
    for (x = 0; x < CIF_WIDTH*3/4; x++) {
      sum = dst[0]+dst[CIF_WIDTH*3/2];
      dst[CIF_WIDTH*3/4] = (sum >> 1);
      dst++;
    }
   dst+=CIF_WIDTH*3/2;
  }
}

void MCUVideoMixer::ConvertCIF4ToTCIF(const void * _src, void * _dst)
{
  BYTE * src = (BYTE *)_src;
  BYTE * dst = (BYTE *)_dst;

  //BYTE * dstEnd = dst + CIF_SIZE;
  int y, x;
  unsigned int sum;
  BYTE * srcRow;

  // copy Y
  srcRow = src;
  for (y = 0; y < CIF4_HEIGHT/2; y++) {
    for (x = 0; x < CIF4_WIDTH/2; x++) {
      sum = *srcRow; dst[0] = *srcRow++;
      sum+= *srcRow; dst[2] = *srcRow++;
      dst[1] = (sum >> 1);
      dst += 3;
    }
    dst += CIF4_WIDTH*3/2;
    for (x = 0; x < CIF4_WIDTH/2; x++) {
      sum = *srcRow; dst[0] = *srcRow++;
      sum+= *srcRow; dst[2] = *srcRow++;
      dst[1] = (sum >> 1);
      dst += 3;
    }
    dst -= CIF4_WIDTH*9/2;
    for (x = 0; x < CIF4_WIDTH*3/2; x++) {
      sum = dst[0]+dst[CIF4_WIDTH*3];
      dst[CIF4_WIDTH*3/2] = (sum >> 1);
      dst++;
    }
   dst+=CIF4_WIDTH*3;
  }

  // copy U
  for (y = 0; y < CIF4_HEIGHT/4; y++) {
    for (x = 0; x < CIF4_WIDTH/4; x++) {
      sum = *srcRow; dst[0] = *srcRow++;
      sum+= *srcRow; dst[2] = *srcRow++;
      dst[1] = (sum >> 1);
      dst += 3;
    }
    dst += CIF4_WIDTH*3/4;
    for (x = 0; x < CIF4_WIDTH/4; x++) {
      sum = *srcRow; dst[0] = *srcRow++;
      sum+= *srcRow; dst[2] = *srcRow++;
      dst[1] = (sum >> 1);
      dst += 3;
    }
    dst -= CIF4_WIDTH*9/4;
    for (x = 0; x < CIF4_WIDTH*3/4; x++) {
      sum = dst[0]+dst[CIF4_WIDTH*3/2];
      dst[CIF4_WIDTH*3/4] = (sum >> 1);
      dst++;
    }
   dst+=CIF4_WIDTH*3/2;
  }

  // copy V
  for (y = 0; y < CIF4_HEIGHT/4; y++) {
    for (x = 0; x < CIF4_WIDTH/4; x++) {
      sum = *srcRow; dst[0] = *srcRow++;
      sum+= *srcRow; dst[2] = *srcRow++;
      dst[1] = (sum >> 1);
      dst += 3;
    }
    dst += CIF4_WIDTH*3/4;
    for (x = 0; x < CIF4_WIDTH/4; x++) {
      sum = *srcRow; dst[0] = *srcRow++;
      sum+= *srcRow; dst[2] = *srcRow++;
      dst[1] = (sum >> 1);
      dst += 3;
    }
    dst -= CIF4_WIDTH*9/4;
    for (x = 0; x < CIF4_WIDTH*3/4; x++) {
      sum = dst[0]+dst[CIF4_WIDTH*3/2];
      dst[CIF4_WIDTH*3/4] = (sum >> 1);
      dst++;
    }
   dst+=CIF4_WIDTH*3/2;
  }
}

void MCUVideoMixer::ConvertQCIFToCIF4(const void * _src, void * _dst)
{
  BYTE * src = (BYTE *)_src;
  BYTE * dst = (BYTE *)_dst;

  //BYTE * dstEnd = dst + CIF_SIZE;
  int y, x;
  BYTE * srcRow;

  // copy Y
  for (y = 0; y < QCIF_HEIGHT; y++) {
    srcRow = src;
    for (x = 0; x < QCIF_WIDTH; x++) {
      dst[0] = dst[1] = dst[2] = dst[3] = *srcRow++;
      dst += 4;
    }
    srcRow = src;
    for (x = 0; x < QCIF_WIDTH; x++) {
      dst[0] = dst[1] = dst[2] = dst[3] = *srcRow++;
      dst += 4;
    }
    srcRow = src;
    for (x = 0; x < QCIF_WIDTH; x++) {
      dst[0] = dst[1] = dst[2] = dst[3] = *srcRow++;
      dst += 4;
    }
    srcRow = src;
    for (x = 0; x < QCIF_WIDTH; x++) {
      dst[0] = dst[1] = dst[2] = dst[3] = *srcRow++;
      dst += 4;
    }
    src += QCIF_WIDTH;
  }

  // copy U
  for (y = 0; y < QCIF_HEIGHT/2; y++) {
    srcRow = src;
    for (x = 0; x < QCIF_WIDTH/2; x++) {
      dst[0] = dst[1] = dst[2] = dst[3] = *srcRow++;
      dst += 4;
    }
    srcRow = src;
    for (x = 0; x < QCIF_WIDTH/2; x++) {
      dst[0] = dst[1] = dst[2] = dst[3] = *srcRow++;
      dst += 4;
    }
    srcRow = src;
    for (x = 0; x < QCIF_WIDTH/2; x++) {
      dst[0] = dst[1] = dst[2] = dst[3] = *srcRow++;
      dst += 4;
    }
    srcRow = src;
    for (x = 0; x < QCIF_WIDTH/2; x++) {
      dst[0] = dst[1] = dst[2] = dst[3] = *srcRow++;
      dst += 4;
    }
    src += QCIF_WIDTH/2;
  }

  // copy V
  for (y = 0; y < QCIF_HEIGHT/2; y++) {
    srcRow = src;
    for (x = 0; x < QCIF_WIDTH/2; x++) {
      dst[0] = dst[1] = dst[2] = dst[3] = *srcRow++;
      dst += 4;
    }
    srcRow = src;
    for (x = 0; x < QCIF_WIDTH/2; x++) {
      dst[0] = dst[1] = dst[2] = dst[3] = *srcRow++;
      dst += 4;
    }
    srcRow = src;
    for (x = 0; x < QCIF_WIDTH/2; x++) {
      dst[0] = dst[1] = dst[2] = dst[3] = *srcRow++;
      dst += 4;
    }
    srcRow = src;
    for (x = 0; x < QCIF_WIDTH/2; x++) {
      dst[0] = dst[1] = dst[2] = dst[3] = *srcRow++;
      dst += 4;
    }
    src += QCIF_WIDTH/2;
  }
}
//#endif // #if !USE_LIBYUV && !USE_SWSCALE

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
  SplitLine(d, w-1, y, 1, h, fw, fh);
}
void MCUVideoMixer::SplitLineTop(BYTE *d, unsigned x, unsigned y, unsigned w, unsigned h, unsigned fw, unsigned fh)
{
  SplitLine(d, x, y, w, 1, fw, fh);
}
void MCUVideoMixer::SplitLineBottom(BYTE *d, unsigned x, unsigned y, unsigned w, unsigned h, unsigned fw, unsigned fh)
{
  SplitLine(d, x, h-1, w, 1, fw, fh);
}

///////////////////////////////////////////////////////////////////////////////////////

MCUSimpleVideoMixer::MCUSimpleVideoMixer(BOOL _forceScreenSplit)
//  : forceScreenSplit(_forceScreenSplit)
{
  PTRACE(1,"MixerCtrl\tNew MCUSimpleVideoMixer created");
  forceScreenSplit = _forceScreenSplit;
  VMPListInit();
  imageStore_size=0;
  imageStore1_size=0;
  imageStore2_size=0;

//  converter = PColourConverter::Create("YUV420P", "YUV420P", CIF16_WIDTH, CIF16_HEIGHT);
  specialLayout = 0;
}

MCUSimpleVideoMixer::~MCUSimpleVideoMixer()
{
}

BOOL MCUSimpleVideoMixer::ReadFrame(ConferenceMember &, void * buffer, int width, int height, PINDEX & amount)
{
  return ReadMixedFrame(frameStores, buffer, width, height, amount);
}

BOOL MCUSimpleVideoMixer::ReadMixedFrame(void * buffer, int width, int height, PINDEX & amount)
{
  return ReadMixedFrame(frameStores, buffer, width, height, amount);
}

BOOL MCUSimpleVideoMixer::ReadMixedFrame(VideoFrameStoreList & srcFrameStores, void * buffer, int width, int height, PINDEX & amount)
{
  VideoFrameStoreList::shared_iterator fsit = srcFrameStores.GetFrameStore(width, height);
  VideoFrameStoreList::FrameStore & fs = **fsit;

  // background
  if(fs.bgframe.GetSize() != 0)
    memcpy(buffer, fs.bgframe.GetPointer(), fs.frame_size);

  for(unsigned i = 0; i < OpenMCU::vmcfg.vmconf[specialLayout].splitcfg.vidnum; i++)
  {
    VMPCfgOptions & vmpcfg = OpenMCU::vmcfg.vmconf[specialLayout].vmpcfg[i];
    int px = vmpcfg.posx  *width/CIF4_WIDTH; // pixel x&y of vmp-->fs
    int py = vmpcfg.posy  *height/CIF4_HEIGHT;
    int pw = vmpcfg.width *width/CIF4_WIDTH; // pixel w&h of vmp-->fs
    int ph = vmpcfg.height*height/CIF4_HEIGHT;
    if(pw<2 || ph<2) continue;

    MCUVMPList::shared_iterator vmp_it = VMPFind((int)i);
    if(vmp_it != vmpList.end())
    {
      VideoMixPosition *vmp = *vmp_it;
      MCUSharedList<MCUBufferArray>::shared_iterator vmpbuf_it = vmp->bufferList.Find((long)&fs);
      if(vmpbuf_it != vmp->bufferList.end())
      {
        int vmpbuf_index = vmp->vmpbuf_index;
        if(vmpbuf_index >= 0)
        {
          MCUBuffer *vmpbuf = (**vmpbuf_it)[vmpbuf_index];
          if(vmpbuf->GetSize() > 0)
          {
            for(unsigned i = 0; i < vmpcfg.blks; i++)
              CopyRFromRIntoR(vmpbuf->GetPointer(), buffer, px, py, pw, ph,
                AlignUp2(vmpcfg.blk[i].posx*width/CIF4_WIDTH), AlignUp2(vmpcfg.blk[i].posy*height/CIF4_HEIGHT),
                AlignUp2(vmpcfg.blk[i].width*width/CIF4_WIDTH), AlignUp2(vmpcfg.blk[i].height*height/CIF4_HEIGHT),
                width, height, pw, ph );
          }
        }
      }
    }

    // grid
    if(OpenMCU::vmcfg.vmconf[specialLayout].vmpcfg[i].border)
    {
      if(px != 0)
        SplitLineLeft((BYTE *)buffer, px, py, pw, ph, width, height);
      if(py != 0)
        SplitLineTop((BYTE *)buffer, px, py, pw, ph, width, height); // top
    }
  }

  // grid
  SplitLineLeft((BYTE *)buffer, 0, 0, width, height, width, height);
  SplitLineRight((BYTE *)buffer, 0, 0, width, height, width, height);
  SplitLineTop((BYTE *)buffer, 0, 0, width, height, width, height);
  SplitLineBottom((BYTE *)buffer, 0, 0, width, height, width, height);

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
  time_t removalDeadline = now - FRAMESTORE_TIMEOUT;

  if(options & WSF_VMP_SET_TIME)
    vmp.lastWrite=now;
  unsigned rule;
  if(options & WSF_VMP_FORCE_CUT) rule=0; else rule=vmp.rule;

  int vmpbuf_index = vmp.vmpbuf_index + 1;
  if(vmpbuf_index == 3)
    vmpbuf_index = 0;

  for(VideoFrameStoreList::shared_iterator it = frameStores.frameStoreList.begin(); it != frameStores.frameStoreList.end(); ++it)
  {
    VideoFrameStoreList::FrameStore & vf = **it;
    if(vf.lastRead<removalDeadline)
    {
#if USE_FREETYPE
      DeleteSubtitlesByFS(vf.width,vf.height);
#endif
      if(frameStores.frameStoreList.Erase(it))
        delete &vf;
      continue;
    }
    if(vf.width<2 || vf.height<2) continue; // minimum size 2*2

    // pixel w&h of vmp-->fs:
    int pw = vmpcfg.width *vf.width /CIF4_WIDTH;
    int ph = vmpcfg.height*vf.height/CIF4_HEIGHT;
    if(pw<2 || ph<2) continue;

    MCUSharedList<MCUBufferArray>::shared_iterator vmpbuf_it = vmp.bufferList.Find((long)&vf);
    if(vmpbuf_it == vmp.bufferList.end())
    {
      PWaitAndSignal m(vmp.bufferListMutex);
      vmpbuf_it = vmp.bufferList.Find((long)&vf);
      if(vmpbuf_it == vmp.bufferList.end())
        vmpbuf_it = vmp.bufferList.Insert(new MCUBufferArray(3, 0), (long)&vf);
    }
    MCUBuffer *vmpbuf = (**vmpbuf_it)[vmpbuf_index];

    if(pw==width && ph==height) //same size
    {
      vmpbuf->SetSize(pw*ph*3*2);
      MCUVideoMixer::FillYUVFrame(vmpbuf->GetPointer(), 0, 0, 0, pw, ph);
      memcpy(vmpbuf->GetPointer(), buffer, pw*ph*3/2); //making copy for subtitles & border
    }
    else if(pw*height<ph*width) //broader:  +---------+     pw     rule 0 => cut width
    {                           //          |  width  |    +--+
      if(rule==0)               //    height|         | -> |  |ph   rule 1 => add stripes
      {                         //          +---------+    +--+                  to top and bottom
        int dstWidth = ph*width/height; //bigger than we need
        vmpbuf->SetSize(dstWidth*ph*3*2);
        MCUVideoMixer::FillYUVFrame(vmpbuf->GetPointer(), 0, 0, 0, dstWidth, ph);
        ResizeYUV420P((const BYTE *)buffer, vmpbuf->GetPointer(), width, height, dstWidth, ph);
        CopyRectFromFrame(vmpbuf->GetPointer(), vmpbuf->GetPointer(), (dstWidth-pw)/2, 0, pw, ph, dstWidth, ph);
      }
      else if(rule==1)
      {
        int dstHeight = pw*height/width; //smaller than we need
        vmpbuf->SetSize(pw*ph*3*2);
        MCUVideoMixer::FillYUVFrame(vmpbuf->GetPointer(), 0, 0, 0, pw, ph);
        MCUBuffer buf(pw*ph*3*2);
        ResizeYUV420P((const BYTE *)buffer, buf.GetPointer(), width, height, pw, dstHeight);
        //FillYUVRect(buf.GetPointer(),pw,ph,127,127,127, 0,0, pw,(ph-dstHeight)/2);
        //FillYUVRect(buf.GetPointer(),pw,ph,127,127,127, 0,ph-(ph-dstHeight)/2, pw,(ph-dstHeight)/2);
        CopyRectIntoFrame(buf.GetPointer(), vmpbuf->GetPointer(), 0, (ph-dstHeight)/2, pw, dstHeight, pw, ph);
      }
    }                           //                   width
    else if(pw*height>ph*width) //narrower (higher): +-+      pw      rule 0 => cut height
    {                           //                   | |    +----+
      if(rule==0)               //             height| | -> |    | ph  rule 1 => add stripes
      {                         //                   +-+    +----+                to left and right
        int dstHeight = pw*height/width; //bigger than we need
        vmpbuf->SetSize(pw*dstHeight*3*2);
        MCUVideoMixer::FillYUVFrame(vmpbuf->GetPointer(), 0, 0, 0, pw, dstHeight);
        ResizeYUV420P((const BYTE *)buffer, vmpbuf->GetPointer(), width, height, pw, dstHeight);
        CopyRectFromFrame(vmpbuf->GetPointer(), vmpbuf->GetPointer(), 0, (dstHeight-ph)/2, pw, ph, pw, dstHeight);
      }
      else if(rule==1)
      {
        int dstWidth = ph*width/height; //smaller than we need
        vmpbuf->SetSize(pw*ph*3*2);
        MCUVideoMixer::FillYUVFrame(vmpbuf->GetPointer(), 0, 0, 0, pw, ph);
        MCUBuffer buf(pw*ph*3*2);
        ResizeYUV420P((const BYTE *)buffer, buf.GetPointer(), width, height, dstWidth, ph);
        //FillYUVRect(buf.GetPointer(),pw,ph,127,127,127, 0,0, (pw-dstWidth)/2, ph);
        //FillYUVRect(buf.GetPointer(),pw,ph,127,127,127, pw-(pw-dstWidth)/2,0, (pw-dstWidth)/2,ph);
        CopyRectIntoFrame(buf.GetPointer(), vmpbuf->GetPointer(), (pw-dstWidth)/2, 0, dstWidth, ph, pw, ph);
      }
    }
    else
    { // fit. scale
      vmpbuf->SetSize(pw*ph*3*2);
      MCUVideoMixer::FillYUVFrame(vmpbuf->GetPointer(), 0, 0, 0, pw, ph);
      ResizeYUV420P((const BYTE *)buffer, vmpbuf->GetPointer() , width, height, pw, ph);
    }

#if USE_FREETYPE
    if(options & WSF_VMP_SUBTITLES)
      if(!(vmpcfg.label_mask&FT_P_DISABLED))
        PrintSubtitles(vmp, (void *)vmpbuf->GetPointer(),pw,ph,vmpcfg.label_mask);
#endif

  }

  vmp.vmpbuf_index = vmpbuf_index;
  return TRUE;
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
    WriteOfflineFrame(*vmp);
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
    WriteNoVideoFrame(*vmp);
    VMPInsert(vmp);
  }
  return TRUE;
}

void MCUSimpleVideoMixer::WriteEmptyFrame(VideoMixPosition & vmp)
{
  unsigned width, height, options;
  void * buffer = OpenMCU::Current().GetEmptyFramePointer(width, height);
  if(width==16 && height==16) options=WSF_VMP_BORDER|WSF_VMP_FORCE_CUT;
  else options=WSF_VMP_BORDER;
  if(buffer) WriteSubFrame(vmp, buffer, width, height, options);
}

void MCUSimpleVideoMixer::WriteOfflineFrame(VideoMixPosition & vmp)
{
  unsigned width, height, options;
  void * buffer = OpenMCU::Current().GetOfflineFramePointer(width, height);
  if(width==16 && height==16) options=WSF_VMP_COMMON|WSF_VMP_FORCE_CUT;
  else options=WSF_VMP_COMMON;
  if(buffer) WriteSubFrame(vmp, buffer, width, height, options);
}

void MCUSimpleVideoMixer::WriteNoVideoFrame(VideoMixPosition & vmp)
{
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

void MCUSimpleVideoMixer::VMPSetConfig(VideoMixPosition *vmp)
{
# if USE_FREETYPE
  RemoveSubtitles(*vmp);
# endif
  VMPTouch(*vmp);
}

void MCUSimpleVideoMixer::VMPCopy(VideoMixPosition & vmp1, VideoMixPosition & vmp2)
{
# if USE_FREETYPE
  RemoveSubtitles(vmp2);
# endif
  vmp2.id             = vmp1.id;
  vmp2.endpointName   = vmp1.endpointName;
  vmp2.lastWrite      = vmp1.lastWrite;
  vmp2.silenceCounter = vmp1.silenceCounter;
  vmp2.rule           = vmp1.rule;
  vmp2.chosenVan      = vmp1.chosenVan;
  vmp2.offline        = vmp1.offline;
  vmp2.chosenVan      = vmp1.chosenVan;
  if((vmp2.type<1)||(vmp2.type>3)) vmp2.type = vmp1.type;
}

void MCUSimpleVideoMixer::VMPSwapAndTouch(VideoMixPosition & vmp1, VideoMixPosition & vmp2)
{
  VideoMixPosition vmp(0);
  VMPCopy(vmp2, vmp);
  VMPCopy(vmp1, vmp2);
  VMPCopy(vmp, vmp1);
  VMPTouch(vmp1);
  VMPTouch(vmp2);
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
      VMPSetConfig(vmp);
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
    tempNameList[memberCount]=v->endpointName;
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
        vmp->endpointName = tempNameList[i];
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
        vmp->endpointName = tempNameList[i];
        vmp->lastWrite = 0;
        vmp->silenceCounter = 0;
        vmp->offline = tempOfflineList[i];
#   if USE_FREETYPE
        RemoveSubtitles(*vmp);
#   endif
        VMPTouch(*vmp);
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
      VMPSetConfig(v);
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
      VMPSetConfig(v);
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
  if(n<0) return it;
  if((unsigned)n >= OpenMCU::vmcfg.vmconf[specialLayout].splitcfg.vidnum) return it;

  it = VMPFind(n);
  VideoMixPosition *newPosition = *it;

  if(it == vmpList.end())
  {
    if(member != NULL) newPosition = CreateVideoMixPosition(member->GetID());
    else newPosition = CreateVideoMixPosition(n);
    if(newPosition != NULL) newPosition->n = n;
  }
  if(newPosition == NULL)
    return it;

  newPosition->type = type;
  if(member != NULL)
  {
    newPosition->rule = member->resizerRule;
    newPosition->endpointName = member->GetName();
    newPosition->chosenVan = member->chosenVan;
  }
  else
    newPosition->endpointName = "VAD" + PString(type-1) + "/" + PString(n);
# if USE_FREETYPE
  RemoveSubtitles(*newPosition);
# endif
  VMPTouch(*newPosition);

  return VMPInsert(newPosition);
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
  PTRACE_IF(!result, 2, "AddVideoSource " << id << " could not find empty video position");
  return result;
}

void MCUSimpleVideoMixer::RemoveVideoSource(ConferenceMemberId id, ConferenceMember & mbr)
{
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

  VideoMixPosition *vmp = *vit;
  if(vmpList.Erase(vit))
  {
    vmp->id=member->GetID();
    vmp->silenceCounter=0;
    vmp->chosenVan=member->chosenVan;
    vmp->endpointName=member->GetName();
#if USE_FREETYPE
    RemoveSubtitles(*vmp);
#endif
    VMPInsert(vmp);
  }

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
  return TRUE;
}

BOOL MCUSimpleVideoMixer::SetVAD2Position(ConferenceMember *member)
{
  PWaitAndSignal m(vmpListMutex);

  if(member==NULL) return FALSE;
  ConferenceMemberId id=member->GetID();
  int maxStatus=-1;

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
      if(silenceCounter > maxStatus) { maxStatus=silenceCounter; vit = it; }
    }
  }

  if(vit == vmpList.end()) return FALSE;
  if(maxStatus < 3000) return FALSE;
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
      VMPSwapAndTouch(*old_vmp, *vmp);
      VMPInsert(vmp);
    }
    VMPInsert(old_vmp);
  }

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
    VMPSetConfig(vmp);
    VMPInsert(vmp);
  }
}

void MCUSimpleVideoMixer::PositionSetup(int pos, int type, ConferenceMember * member) //types 1000, 1001, 1002, 1003 means type will not changed
{
  PWaitAndSignal m(vmpListMutex);

  if((unsigned)pos>=OpenMCU::vmcfg.vmconf[specialLayout].splitcfg.vidnum)
    return; // n out of range

  MCUVMPList::shared_iterator old_it;
  ConferenceMemberId id;
  if(member!=NULL)
  {
    id = member->GetID();
    old_it = VMPFind(member->GetID());
  }
  else
    id = 0;

  if(old_it != vmpList.end())
  {
    if(old_it->n == pos)
    {
      if(type<1000)
      {
        old_it->type=type;
      }
      return;
    }
    int n = old_it->n;
    int type = old_it->type;
    old_it.Release();
    MyRemoveVideoSource(n, (type == 1));
  }

  for(MCUVMPList::shared_iterator it = vmpList.begin(); it != vmpList.end(); ++it)
  {
    VideoMixPosition *v = *it;
    if(v->n==pos) // we found it
    {
      if(type<1000) v->type=type;

      if((v->type==1) && (!id)) // special case: VMP needs to be removed
      {
        if(vmpList.Erase(it))
          delete v;
        return;
      }

      if((v->type==2)||(v->type==3))
      {
        if(v->chosenVan) return;
        BOOL disableVAD=FALSE;
        if(id)disableVAD=member->disableVAD;
        if(id && (!disableVAD))
        {
          v->id=id;
          v->silenceCounter=0;
          v->chosenVan=member->chosenVan;
          v->endpointName=member->GetName();
        }
        else v->id = v->n;
#if USE_FREETYPE
        RemoveSubtitles(*v);
#endif
        if(id)v->endpointName = member->GetName();
        else v->endpointName="Voice-activated " + PString(type-1);
        return;
      }

      if(v->id == id) return;

      v->id=id;
#if USE_FREETYPE
      RemoveSubtitles(*v);
#endif
      v->endpointName=member->GetName();
      return;
    }
  }

// creating new video mix position:

  if(type>1000) type-=1000;
  if((type==1) && (!id)) return;
  PString name;
  if(!id) id=pos; //vad
  else name=member->GetName();

  VideoMixPosition * newPosition = CreateVideoMixPosition(id);
  newPosition->type=type;
  newPosition->n=pos;
  newPosition->endpointName = name;
#if USE_FREETYPE
  RemoveSubtitles(*newPosition);
#endif
  VMPInsert(newPosition);
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
      VMPSetConfig(v1);
      VMPInsert(v1);
    }
    return;
  }

  if(vmpList.Erase(it1))
  {
    if(vmpList.Erase(it2))
    {
      VMPSwapAndTouch(*v1, *v2);
      VMPInsert(v2);
    }
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
  MCUVMPList::shared_iterator it = VMPFind(pos);
  if(it != vmpList.end())
    VMPDelete(it);
}

void MCUSimpleVideoMixer::MyRemoveVideoSourceById(ConferenceMemberId id, BOOL flag)
{
  MCUVMPList::shared_iterator it = VMPFind(id);
  if(it != vmpList.end())
    VMPDelete(it);
}


void MCUSimpleVideoMixer::MyRemoveAllVideoSource()
{
  VMPListClear();
}

PString MCUSimpleVideoMixer::GetFrameStoreMonitorList()
{
  PStringStream s;
  for(VideoFrameStoreList::shared_iterator it = frameStores.frameStoreList.begin(); it != frameStores.frameStoreList.end(); ++it)
  {
    VideoFrameStoreList::FrameStore *fs = *it;
    s << "  Frame store [" << fs->width << "x" << fs->height << "] "
      << "last read time: " << fs->lastRead << "\n";
  }
  return s;
}

inline void MCUSimpleVideoMixer::CheckOperationalSize(long w, long h, BYTE mask)
{
# define MY_IS_CHECK_SET_SIZE(store)\
    if(s > store##_size)\
    {\
      /* Width/height might be not multiple of 2, increase buffer for swscale +1 */ \
      /* –®–∏—Ä–∏–Ω–∞/–≤—ã—Å–æ—Ç–∞ –º–æ–∂–µ—Ç –±—ã—Ç—å –Ω–µ –∫—Ä–∞—Ç–Ω–∞ 2, –¥–ª—è swscale —É–≤–µ–ª–∏—á–∏—Ç—å –±—É—Ñ–µ—Ä +1 */ \
      store.SetSize(s+w+1);\
      store##_size=s;\
    }

  long s=w*h*3/2;
  if(mask & _IMGST ) MY_IS_CHECK_SET_SIZE(imageStore);
  if(mask & _IMGST1) MY_IS_CHECK_SET_SIZE(imageStore1);
  if(mask & _IMGST2) MY_IS_CHECK_SET_SIZE(imageStore2);

# undef MY_IS_CHECK_SET_SIZE
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
  imageStore_size=0;
  imageStore1_size=0;
  imageStore2_size=0;

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
    PStringStream s; s << "Mix Position " << i; newPosition->endpointName=s;
#if USE_FREETYPE
    RemoveSubtitles(*newPosition);
#endif
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
    PStringStream s; s << "Mix Position " << i; newPosition->endpointName=s;
#if USE_FREETYPE
    RemoveSubtitles(*newPosition);
#endif
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
  imageStore_size=0;
  imageStore1_size=0;
  imageStore2_size=0;

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
  vmp->endpointName = mbr.GetName();
#if USE_FREETYPE
  RemoveSubtitles(*vmp);
#endif
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

