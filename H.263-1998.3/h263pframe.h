/*
 * H.264 Plugin codec for OpenH323/OPAL
 *
 * Copyright (C) Matthias Schneider, All Rights Reserved
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
 * Contributor(s): Matthias Schneider (ma30002000@yahoo.de)
 *
 *
 */

#ifndef __H263PFrame_H__
#define __H263PFrame_H__ 1

#include <vector>
#include <string.h>
#include "rtpframe.h"

#include "ffmpeg.h"

extern "C" {
#include LIBAVCODEC_HEADER
};

enum codecInFlags {
  silenceFrame      = 1,
  forceIFrame       = 2
};

enum codecOutFlags {
  isLastFrame     = 1,
  isIFrame        = 2,
  requestIFrame   = 4
};

typedef struct data_t
{
  uint8_t* ptr;
  uint32_t pos;
  uint32_t len;
} data_t;

typedef struct header_data_t
{
  uint8_t* ptr;
  uint32_t len;
  uint32_t pebits;
} header_data_t;

class Bitstream
{
public:
  Bitstream ();
  void SetBytes (uint8_t* data, uint32_t dataLen, uint8_t sbits, uint8_t ebits);
  void GetBytes (uint8_t** data, uint32_t * dataLen);
  uint32_t GetBits (uint32_t numBits);
  uint32_t PeekBits (uint32_t numBits);
  void PutBits(uint32_t posBits, uint32_t numBits, uint32_t value);
  void SetPos(uint32_t pos);
  uint32_t GetPos();
private:
  data_t _data;
  uint8_t _sbits;
  uint8_t _ebits;
};

static const char formats[8][64] = { "forbidden",
                                     "sub-QCIF",
                                     "QCIF",
                                     "CIF",
                                     "4CIF",
                                     "16CIF",
                                     "reserved",
                                     "extended PTYPE" };

static const char picTypeCodes[8][64] = { "I-Picture",
                                          "P-Picture",
                                          "improved PB-frame",
                                          "B-Picture",
                                          "EI-Picture",
                                          "EP-Picture",
                                          "reserved (110)",
                                          "reserved (111)" };

static const char plusFormats[8][64] = { "forbidden",
                                         "sub-QCIF",
                                         "QCIF",
                                         "CIF",
                                         "4CIF",
                                         "16CIF",
                                         "custom format",
                                         "reserved" };

static const char PARs[16][64]   = { "forbidden",
                                     "1:1 (Square)",
                                     "12:11 (CIF for 4:3 picture)",
                                     "10:11 (525-type for 4:3 picture)",
                                     "16:11 (CIF stretched for 16:9 picture)",
                                     "40:33 (525-type stretched for 16:9 picture)",
                                     "reserved (0110)",
                                     "reserved (0111)",
                                     "reserved (1000)",
                                     "reserved (1001)",
                                     "reserved (1010)",
                                     "reserved (1011)",
                                     "reserved (1100)",
                                     "reserved (1101)",
                                     "reserved (1110)",
                                     "Extended PAR" };
class H263PFrame
{
public:
  H263PFrame(uint32_t maxFrameSize);
  ~H263PFrame();

  void BeginNewFrame();
  void GetRTPFrame (RTPFrame & frame, unsigned int & flags);
  bool SetFromRTPFrame (RTPFrame & frame, unsigned int & flags);

  bool HasRTPFrames ()
  {
    return (_encodedFrame.pos < _encodedFrame.len);
  }

  uint8_t* GetFramePtr ()
  {
    memset (_encodedFrame.ptr + _encodedFrame.pos,0 , FF_INPUT_BUFFER_PADDING_SIZE);
    return (_encodedFrame.ptr);
  }

  void SetFrameSize (uint32_t size) {
    _encodedFrame.len = size;
  }

  uint32_t GetFrameSize () {
    return (_encodedFrame.len);
  }

  void SetMaxPayloadSize (uint16_t maxPayloadSize) 
  {
    _maxPayloadSize = maxPayloadSize;
  }

  void SetTimestamp (uint64_t timestamp) 
  {
    _timestamp = timestamp;
  }
  
  bool hasPicHeader ();
  bool IsIFrame ();
private:
  uint32_t parseHeader(uint8_t* headerPtr, uint32_t headerMaxLen);

  uint64_t _timestamp;
  uint16_t _maxPayloadSize;
  uint16_t _minPayloadSize;
  uint32_t _maxFrameSize;
  bool     _customClock;
  data_t   _encodedFrame;
  header_data_t _picHeader;
  std::vector<uint32_t> _startCodes;
};

#endif /* __H263PFrame_H__ */
