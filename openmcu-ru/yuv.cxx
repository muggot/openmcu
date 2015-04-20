#include "precompile.h"

#if MCU_VIDEO

#include "mcu.h"

void ConvertRGBToYUV(BYTE R, BYTE G, BYTE B, BYTE & Y, BYTE & U, BYTE & V)
{
  Y = (BYTE)PMIN(ABS(R *  2104 + G *  4130 + B *  802 + 4096 +  131072) / 8192, 235);
  U = (BYTE)PMIN(ABS(R * -1214 + G * -2384 + B * 3598 + 4096 + 1048576) / 8192, 240);
  V = (BYTE)PMIN(ABS(R *  3598 + G * -3013 + B * -585 + 4096 + 1048576) / 8192, 240);
}

void FillYUVFrame(void * buffer, BYTE R, BYTE G, BYTE B, int w, int h)
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

void FillYUVFrame_YUV(void * buffer, BYTE Y, BYTE U, BYTE V, int w, int h)
{
  const int ysize = w*h;
  const int usize = (w/2)*(h/2);
  const int vsize = usize;

  memset((BYTE *)buffer + 0,             Y, ysize);
  memset((BYTE *)buffer + ysize,         U, usize);
  memset((BYTE *)buffer + ysize + usize, V, vsize);
}

void FillCIFYUVFrame(void * buffer, BYTE R, BYTE G, BYTE B)
{
  FillYUVFrame(buffer, R, G, B, CIF_WIDTH, CIF_HEIGHT);
}

void FillCIF4YUVFrame(void * buffer, BYTE R, BYTE G, BYTE B)
{
  FillYUVFrame(buffer, R, G, B, CIF4_WIDTH, CIF4_HEIGHT);
}

void FillCIF16YUVFrame(void * buffer, BYTE R, BYTE G, BYTE B)
{
  FillYUVFrame(buffer, R, G, B, CIF16_WIDTH, CIF16_HEIGHT);
}

void FillQCIFYUVFrame(void * buffer, BYTE R, BYTE G, BYTE B)
{
  FillYUVFrame(buffer, R, G, B, QCIF_WIDTH, QCIF_HEIGHT);
}

void FillCIFYUVRect(void * frame, BYTE R, BYTE G, BYTE B, int xPos, int yPos, int rectWidth, int rectHeight)
{
  FillYUVRect(frame, CIF_WIDTH, CIF_HEIGHT, R, G, B, xPos, yPos, rectWidth, rectHeight);
}

void FillCIF4YUVRect(void * frame, BYTE R, BYTE G, BYTE B, int xPos, int yPos, int rectWidth, int rectHeight)
{
  FillYUVRect(frame, CIF4_WIDTH, CIF4_HEIGHT, R, G, B, xPos, yPos, rectWidth, rectHeight);
}

void FillCIF16YUVRect(void * frame, BYTE R, BYTE G, BYTE B, int xPos, int yPos, int rectWidth, int rectHeight)
{
  FillYUVRect(frame, CIF16_WIDTH, CIF16_HEIGHT, R, G, B, xPos, yPos, rectWidth, rectHeight);
}

void FillYUVRect(void * frame, int frameWidth, int frameHeight, BYTE R, BYTE G, BYTE B, int xPos, int yPos, int rectWidth, int rectHeight)
{
  //This routine fills a region of the video image with data. It is used as the central
  //point because one only has to add other image formats here.

  if((frameWidth<2)||(frameHeight<2)) return;
  if(xPos<0) xPos=0;
  if(yPos<0) yPos=0;
  if(xPos+rectWidth >frameWidth ) rectWidth =frameWidth -xPos;
  if(yPos+rectHeight>frameHeight) rectHeight=frameHeight-yPos;
  if((rectWidth<2)||(rectHeight<2)) return;

  int halfRectWidth   = rectWidth   >> 1;
  int halfRectHeight  = rectHeight  >> 1;
  int halfFrameWidth  = frameWidth  >> 1;
  int halfFrameHeight = frameHeight >> 1;
  int halfXPos        = (xPos)      >> 1;
  int halfYPos        = (yPos)      >> 1;
  halfRectWidth  += (rectWidth  & xPos & 1);
  halfRectHeight += (rectHeight & yPos & 1);

  int grayscaleOffset = (    yPos *     frameWidth) +     xPos;
  int colourOffset =    (halfYPos * halfFrameWidth) + halfXPos;

  BYTE Y, U, V;
  ConvertRGBToYUV(R, G, B, Y, U, V);

  BYTE * Yptr = (BYTE*)frame + grayscaleOffset;
  BYTE * UPtr = (BYTE*)frame + (frameWidth * frameHeight) + colourOffset;
  BYTE * VPtr = (BYTE*)frame + (frameWidth * frameHeight) + colourOffset + (halfFrameWidth * halfFrameHeight);

  int rr;

  for (rr = 0; rr < rectHeight;rr++)
  {
    memset(Yptr, Y, rectWidth);
    Yptr += frameWidth;
  }

  for (rr = 0; rr < halfRectHeight;rr++)
  {
    memset(UPtr, U, halfRectWidth);
    memset(VPtr, V, halfRectWidth);
    UPtr += halfFrameWidth;
    VPtr += halfFrameWidth;
  }
}

void ReplaceUV_Rect(void * frame, int frameWidth, int frameHeight, BYTE U, BYTE V, int xPos, int yPos, int rectWidth, int rectHeight)
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

void CopyRectIntoQCIF(const void * _src, void * _dst, int xpos, int ypos, int width, int height)
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

void CopyRectIntoCIF(const void * _src, void * _dst, int xpos, int ypos, int width, int height)
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

void CopyRectIntoCIF4(const void * _src, void * _dst, int xpos, int ypos, int width, int height)
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

void CopyGrayscaleIntoCIF(const void * _src, void * _dst, int xpos, int ypos, int width, int height)
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

void CopyGrayscaleIntoCIF4(const void * _src, void * _dst, int xpos, int ypos, int width, int height)
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

void CopyGrayscaleIntoCIF16(const void * _src, void * _dst, int xpos, int ypos, int width, int height)
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

void CopyGrayscaleIntoFrame(const void * _src, void * _dst, int xpos, int ypos, int width, int height, int fw, int fh)
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

void CopyRectIntoCIF16(const void * _src, void * _dst, int xpos, int ypos, int width, int height)
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


void CopyRFromRIntoR(const void *_s, void * _d, int xp, int yp, int w, int h, int rx_abs, int ry_abs, int rw, int rh, int fw, int fh, int lim_w, int lim_h)
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

void CopyRectIntoFrame(const void * _src, void * _dst, int xpos, int ypos, int width, int height, int fw, int fh)
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

void MixRectIntoFrameGrayscale(const void * _src, void * _dst, int xpos, int ypos, int width, int height, int fw, int fh, BYTE wide)
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
void MixRectIntoFrameSubsMode(const void * _src, void * _dst, int xpos, int ypos, int width, int height, int fw, int fh, BYTE wide)
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

void CopyRectIntoRect(const void * _src, void * _dst, int xpos, int ypos, int width, int height, int fw, int fh)
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

void CopyRectFromFrame(const void * _src, void * _dst, int xpos, int ypos, int width, int height, int fw, int fh)
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

void ResizeYUV420P(const void * _src, void * _dst, unsigned int sw, unsigned int sh, unsigned int dw, unsigned int dh)
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
void ConvertCIF4ToCIF(const void * _src, void * _dst)
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

void ConvertCIF16ToCIF4(const void * _src, void * _dst)
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

void ConvertCIFToQCIF(const void * _src, void * _dst)
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

void ConvertCIFToQ3CIF(const void * _src, void * _dst)
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

void ConvertCIF4ToQ3CIF4(const void * _src, void * _dst)
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

void ConvertCIF16ToQ3CIF16(const void * _src, void * _dst)
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

void ConvertCIF16ToTCIF(const void * _src, void * _dst)
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

void ConvertCIF4ToTQCIF(const void * _src, void * _dst)
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

void ConvertCIFToTSQCIF(const void * _src, void * _dst)
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


void Convert2To1(const void * _src, void * _dst, unsigned int w, unsigned int h)
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

void ConvertCIFToSQ3CIF(const void * _src, void * _dst)
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

void ConvertCIF4ToQ3CIF(const void * _src, void * _dst)
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

void ConvertCIF16ToQ3CIF4(const void * _src, void * _dst)
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

void ConvertCIFToSQCIF(const void * _src, void * _dst)
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

void ConvertCIF4ToQCIF(const void * _src, void * _dst)
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

void ConvertCIF16ToCIF(const void * _src, void * _dst)
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

void ConvertFRAMEToCUSTOM_FRAME(const void * _src, void * _dst, unsigned int sw, unsigned int sh, unsigned int dw, unsigned int dh)
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

void ConvertQCIFToCIF(const void * _src, void * _dst)
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


void ConvertCIFToCIF4(const void * _src, void * _dst)
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

void ConvertCIF4ToCIF16(const void * _src, void * _dst)
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

void Convert1To2(const void * _src, void * _dst, unsigned int w, unsigned int h)
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


void ConvertCIFToTQCIF(const void * _src, void * _dst)
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

void ConvertCIF4ToTCIF(const void * _src, void * _dst)
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

void ConvertQCIFToCIF4(const void * _src, void * _dst)
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

#endif // if MCU_VIDEO
