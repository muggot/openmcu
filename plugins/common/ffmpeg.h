/*
 * Common ffmpeg code for OPAL
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
 * Copyright (C) 2011 Vox Lucida Pty. Ltd.
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
 *                 Robert Jongbloed (robertj@voxlucida.com.au)
 */

#ifndef __FFMPEG_H__
#define __FFMPEG_H__ 1

extern "C"
{
  #include "libavcodec/avcodec.h"
  #include "libavutil/mem.h"
}

#include "platform.h"
#include "opalplugin.hpp"

/////////////////////////////////////////////////////////////////

class FFMPEGCodec
{
  public:
    class EncodedFrame
    {
      protected:
        size_t    m_length;
        size_t    m_maxSize;
        uint8_t * m_buffer;
        size_t    m_maxPayloadSize;

      public:
        EncodedFrame();
        virtual ~EncodedFrame();

        virtual const char * GetName() const { return ""; }

        uint8_t * GetBuffer() const { return m_buffer; }
        size_t GetMaxSize() const { return m_maxSize; }
        size_t GetLength() const { return m_length; }
        void SetMaxPayloadSize(size_t size) { m_maxPayloadSize = size; }

        virtual bool SetResolution(unsigned width, unsigned height);
        virtual bool SetMaxSize(size_t newSize);
        virtual bool Reset(size_t len = 0);

        virtual bool GetPacket(PluginCodec_RTP & rtp, unsigned & flags) = 0;
        virtual bool AddPacket(const PluginCodec_RTP & rtp, unsigned & flags) = 0;

        virtual bool IsIntraFrame() const = 0;

        virtual void RTPCallBack(void * data, int size, int mbCount);

      protected:
        virtual bool Append(const uint8_t * data, size_t len);
    };

  protected:
    const char     * m_prefix;
    AVCodec        * m_codec;
    AVCodecContext * m_context;
    AVFrame        * m_picture;
    AVPacket         m_packet;
    uint8_t        * m_alignedInputYUV;
    size_t           m_alignedInputSize;
    EncodedFrame   * m_fullFrame;
    int              m_errorCount;

  public:
    FFMPEGCodec(const char * prefix, EncodedFrame * fullFrame);
    ~FFMPEGCodec();

    virtual bool InitEncoder(const char *codecId);
    virtual bool InitDecoder(const char *codecId);

    bool SetResolution(unsigned width, unsigned height);
    void SetEncoderOptions(
      unsigned frameTime,
      unsigned maxBitRate,
      unsigned maxRTPSize,
      unsigned tsto,
      unsigned keyFramePeriod
    );

    virtual bool OpenCodec();
    virtual void CloseCodec();

    virtual bool EncodeVideoPacket(const PluginCodec_RTP & in, PluginCodec_RTP & out, unsigned & flags);
    virtual bool DecodeVideoPacket(const PluginCodec_RTP & in, unsigned & flags);

    virtual int EncodeVideoFrame(uint8_t * frame, size_t length, unsigned & flags);
    virtual bool DecodeVideoFrame(const uint8_t * frame, size_t length, unsigned & flags);

    EncodedFrame * GetEncodedFrame() const { return m_fullFrame; }

    virtual void ErrorCallback(unsigned level, const char * msg);

  protected:
    bool InitContext();
};


#endif // __FFMPEG_H__
