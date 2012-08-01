/*****************************************************************************/
/* The contents of this file are subject to the Mozilla Public License       */
/* Version 1.0 (the "License"); you may not use this file except in          */
/* compliance with the License.  You may obtain a copy of the License at     */
/* http://www.mozilla.org/MPL/                                               */
/*                                                                           */
/* Software distributed under the License is distributed on an "AS IS"       */
/* basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.  See the  */
/* License for the specific language governing rights and limitations under  */
/* the License.                                                              */
/*                                                                           */
/* The Original Code is the Open H323 Library.                               */
/*                                                                           */
/* The Initial Developer of the Original Code is Matthias Schneider          */
/* Copyright (C) 2007 Matthias Schneider, All Rights Reserved.               */
/*                                                                           */
/* Contributor(s): Matthias Schneider (ma30002000@yahoo.de)                  */
/*                                                                           */
/* Alternatively, the contents of this file may be used under the terms of   */
/* the GNU General Public License Version 2 or later (the "GPL"), in which   */
/* case the provisions of the GPL are applicable instead of those above.  If */
/* you wish to allow use of your version of this file only under the terms   */
/* of the GPL and not to allow others to use your version of this file under */
/* the MPL, indicate your decision by deleting the provisions above and      */
/* replace them with the notice and other provisions required by the GPL.    */
/* If you do not delete the provisions above, a recipient may use your       */
/* version of this file under either the MPL or the GPL.                     */
/*                                                                           */
/* The Original Code was written by Matthias Schneider <ma30002000@yahoo.de> */
/*****************************************************************************/

#ifndef __RTPFRAME_H__
#define __RTPFRAME_H__ 1

#ifdef _MSC_VER
#pragma warning(disable:4800)  // disable performance warning
#endif

class RTPFrame {
public:
  RTPFrame(const unsigned char * frame, int frameLen) {
    _frame = (unsigned char*) frame;
    _frameLen = frameLen;
  };

  RTPFrame(unsigned char * frame, int frameLen, unsigned char payloadType) {
    _frame = frame;
    _frameLen = frameLen;
    if (_frameLen > 0)
      _frame [0] = 0x80;
    SetPayloadType(payloadType);
  }

  unsigned GetPayloadSize() const {
    return (_frameLen - GetHeaderSize());
  }

  void SetPayloadSize(int size) {
    _frameLen = size + GetHeaderSize();
  }

  int GetFrameLen () const {
    return (_frameLen);
  }

  unsigned char * GetPayloadPtr() const {
    return (_frame + GetHeaderSize());
  }

  int GetHeaderSize() const {
    int size;
    size = 12;
    if (_frameLen < 12) 
      return 0;
    size += (_frame[0] & 0x0f) * 4;
    if (!(_frame[0] & 0x10))
      return size;
    if ((size + 4) < _frameLen) 
      return (size + 4 + (_frame[size + 2] << 8) + _frame[size + 3]);
    return 0;
  }

  bool GetMarker() const {
    if (_frameLen < 2) 
      return false;
    return (_frame[1] & 0x80);
  }

  unsigned GetSequenceNumber() const {
    if (_frameLen < 4)
      return 0;
    return (_frame[2] << 8) + _frame[3];
  }

  void SetMarker(bool set) {
    if (_frameLen < 2) 
      return;
    _frame[1] = _frame[1] & 0x7f;
    if (set) _frame[1] = _frame[1] | 0x80;
  }

  void SetPayloadType(unsigned char type) {
    if (_frameLen < 2) 
      return;
    _frame[1] = _frame [1] & 0x80;
    _frame[1] = _frame [1] | (type & 0x7f);
  }

  unsigned char GetPayloadType() const
  {
    if (_frameLen < 1)
      return 0xff;
    return _frame[1] & 0x7f;
  }

  unsigned long GetTimestamp() const {
    if (_frameLen < 8)
      return 0;
    return ((_frame[4] << 24) + (_frame[5] << 16) + (_frame[6] << 8) + _frame[7]);
  }

  void SetTimestamp(unsigned long timestamp) {
     if (_frameLen < 8)
       return;
     _frame[4] = (unsigned char) ((timestamp >> 24) & 0xff);
     _frame[5] = (unsigned char) ((timestamp >> 16) & 0xff);
     _frame[6] = (unsigned char) ((timestamp >> 8) & 0xff);
     _frame[7] = (unsigned char) (timestamp & 0xff);
  };

protected:
  unsigned char* _frame;
  int _frameLen;
};

struct frameHeader {
  unsigned int  x;
  unsigned int  y;
  unsigned int  width;
  unsigned int  height;
};
	
#endif /* __RTPFRAME_H__ */
