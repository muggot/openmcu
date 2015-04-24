#include "precompile.h"

#ifndef _MCU_YUV_H
#define _MCU_YUV_H

void ConvertRGBToYUV(BYTE R, BYTE G, BYTE B, BYTE & Y, BYTE & U, BYTE & V);
void FillYUVFrame(void * buffer, BYTE R, BYTE G, BYTE B, int w, int h);
void FillYUVFrame_YUV(void * buffer, BYTE Y, BYTE U, BYTE V, int w, int h);
void FillCIFYUVFrame(void * buffer, BYTE R, BYTE G, BYTE B);
void FillQCIFYUVFrame(void * buffer, BYTE R, BYTE G, BYTE B);
void FillCIF4YUVFrame(void * buffer, BYTE R, BYTE G, BYTE B);
void FillCIF16YUVFrame(void * buffer, BYTE R, BYTE G, BYTE B);
void FillCIFYUVRect(void * frame, BYTE R, BYTE G, BYTE B, int xPos, int yPos, int rectWidth, int rectHeight);
void FillCIF4YUVRect(void * frame, BYTE R, BYTE G, BYTE B, int xPos, int yPos, int rectWidth, int rectHeight);
void FillCIF16YUVRect(void * frame, BYTE R, BYTE G, BYTE B, int xPos, int yPos, int rectWidth, int rectHeight);
void FillYUVRect(void * frame, int frameWidth, int frameHeight, BYTE R, BYTE G, BYTE B, int xPos, int yPos, int rectWidth, int rectHeight);
void ReplaceUV_Rect(void * frame, int frameWidth, int frameHeight, BYTE U, BYTE V, int xPos, int yPos, int rectWidth, int rectHeight);
void CopyRectIntoQCIF(const void * _src, void * _dst, int xpos, int ypos, int width, int height);
void CopyRectIntoCIF(const void * _src, void * _dst, int xpos, int ypos, int width, int height);
void CopyRectIntoCIF4(const void * _src, void * _dst, int xpos, int ypos, int width, int height);
void CopyGrayscaleIntoCIF(const void * _src, void * _dst, int xpos, int ypos, int width, int height);
void CopyGrayscaleIntoCIF4(const void * _src, void * _dst, int xpos, int ypos, int width, int height);
void CopyGrayscaleIntoCIF16(const void * _src, void * _dst, int xpos, int ypos, int width, int height);
void CopyGrayscaleIntoFrame(const void * _src, void * _dst, int xpos, int ypos, int width, int height, int fw, int fh);
void CopyRectIntoCIF16(const void * _src, void * _dst, int xpos, int ypos, int width, int height);
void CopyRFromRIntoR(const void *_s, void * _d, int xp, int yp, int w, int h, int rx_abs, int ry_abs, int rw, int rh, int fw, int fh, int lim_w, int lim_h);
void CopyRectIntoFrame(const void * _src, void * _dst, int xpos, int ypos, int width, int height, int fw, int fh);
void MixRectIntoFrameGrayscale(const void * _src, void * _dst, int xpos, int ypos, int width, int height, int fw, int fh, BYTE wide);
#if USE_FREETYPE
void MixRectIntoFrameSubsMode(const void * _src, void * _dst, int xpos, int ypos, int width, int height, int fw, int fh, BYTE wide);
#endif
void CopyRectIntoRect(const void * _src, void * _dst, int xpos, int ypos, int width, int height, int fw, int fh);
void CopyRectFromFrame(const void * _src, void * _dst, int xpos, int ypos, int width, int height, int fw, int fh);
void ResizeYUV420P(const void * _src, void * _dst, unsigned int sw, unsigned int sh, unsigned int dw, unsigned int dh);
void ConvertQCIFToCIF(const void * _src, void * _dst);
void ConvertCIFToCIF4(const void * _src, void * _dst);
void ConvertCIF4ToCIF16(const void * _src, void * _dst);
void ConvertFRAMEToCUSTOM_FRAME(const void * _src, void * _dst, unsigned int sw, unsigned int sh, unsigned int dw, unsigned int dh);
void ConvertCIFToTQCIF(const void * _src, void * _dst);
void ConvertCIF4ToTCIF(const void * _src, void * _dst);
void ConvertCIF16ToTCIF(const void * _src, void * _dst);
void ConvertCIF4ToTQCIF(const void * _src, void * _dst);
void ConvertCIFToTSQCIF(const void * _src, void * _dst);
void ConvertQCIFToCIF4(const void * _src, void * _dst);
void ConvertCIF4ToCIF(const void * _src, void * _dst);
void ConvertCIF16ToCIF4(const void * _src, void * _dst);
void ConvertCIFToQCIF(const void * _src, void * _dst);
void Convert2To1(const void * _src, void * _dst, unsigned int w, unsigned int h);
void Convert1To2(const void * _src, void * _dst, unsigned int w, unsigned int h);
void ConvertCIFToQ3CIF(const void * _src, void * _dst);
void ConvertCIF4ToQ3CIF4(const void * _src, void * _dst);
void ConvertCIF16ToQ3CIF16(const void * _src, void * _dst);
void ConvertCIFToSQ3CIF(const void*, void*);
void ConvertCIF4ToQ3CIF(const void*, void*);
void ConvertCIF16ToQ3CIF4(const void*, void*);
void ConvertCIF16ToCIF(const void * _src, void * _dst);
void ConvertCIF4ToQCIF(const void * _src, void * _dst);
void ConvertCIFToSQCIF(const void * _src, void * _dst);

////////////////////////////////////////////////////////////////////////////////////////////////////

class MCUBufferYUV : public MCUBuffer
{
  public:
    MCUBufferYUV(int w = 0, int h = 0)
      : MCUBuffer(AlignUp2(w)*AlignUp2(h)*3/2), width(w), height(h)
    {
      if(size == 0)
      {
        width = 0;
        height = 0;
      }
    }

    void SetFrameSize(int w, int h)
    {
      SetSize(AlignUp2(w)*AlignUp2(h)*3/2); // +1 for swscale
      if(size != 0)
      {
        width = w;
        height = h;
      }
    }

    const int GetWidth() const
    { return width; }

    const int GetHeight() const
    { return height; }

  protected:
    int width;
    int height;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class MCUBufferYUVArray
{
  public:
    MCUBufferYUVArray(int _array_size, int w, int h)
    {
      array_size = _array_size;
      if(array_size < 1)
        array_size = 1;

      buffers = new MCUBufferYUV * [array_size];
      for(int i = 0; i < array_size; i++)
        buffers[i] = new MCUBufferYUV(w, h);
    }
    ~MCUBufferYUVArray()
    {
      for(int i = 0; i < array_size; i++)
      {
        delete buffers[i];
        buffers[i] = NULL;
      }
      delete [] buffers;
      buffers = NULL;
    }

    MCUBufferYUV * operator[] (int index)
    {
      if(index < 0 || index >= array_size)
        return NULL;
      return buffers[index];
    }

  protected:
    int array_size;
    MCUBufferYUV **buffers;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

#endif //ifndef _MCU_YUV_H
