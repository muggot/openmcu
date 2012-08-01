/*
 * RFC 2190 packetiser and unpacketiser 
 *
 * Copyright (C) 2008 Post Increment
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
 * The Original Code is Opal
 *
 * Contributor(s): Craig Southeren <craigs@postincrement.com>
 *
 */

#include <iostream>
#include <string.h>
#include <stdlib.h>
using namespace std;

#include "rfc2190.h"

#define MAX_PACKET_LEN 1200

const unsigned char PSC[3]      = { 0x00, 0x00, 0x80 };
const unsigned char PSC_Mask[3] = { 0xff, 0xff, 0xfc };

static int MacroblocksPerGOBTable[] = {
    -1,  // forbidden
    -1,  // sub-QCIF
     (144 / 16) *  (176 / 16),  //  QCIF = 99
     (288 / 16) *  (352 / 16),  //  CIF  = 396
     (576 / 32) *  (704 / 32),  //  4CIF = 396
    (1408 / 64) * (1152 / 64),  //  16CIF = 396
    -1,  // Reserved
    -1   // extended
};


static int FindByteAlignedCode(const unsigned char * base, int len, const unsigned char * code, const unsigned char * mask, int codeLen)
{
  const unsigned char * ptr = base;
  while (len > codeLen) {
    int i;
    for (i = 0; i < codeLen; ++i) {
      if ((ptr[i] & mask[i]) != code[i])
        break;
    }
    if (i == codeLen)
      return (int)(ptr - base);
    ++ptr;
    --len;
  }
  return -1;
}


static int FindPSC(const unsigned char * base, int len)
{ return FindByteAlignedCode(base, len, PSC, PSC_Mask, sizeof(PSC)); }

#if 0

static int FindGBSC(const unsigned char * base, int len, int & sbit)
{
  // a GBSC is the following bit sequence:
  //
  //               0000 0000 0000 0000 1
  //
  // as it may not be byte aligned, we look for byte aligned zero byte and then examine the bytes around it
  // to see if it is a GOB header
  // first check to see if we are already pointing to a GBSC
  if ((base[0] == 0x00) && (base[1] == 0x00) && ((base[2] & 0x80) != 0x80))
    return 0;

  const unsigned char * ptr = base + 1;
  while (len > 5) {
    if (ptr[0] == 0x00) {
      int offs = (int)(ptr - base - 1);
      sbit = 0;
      if (                                   (ptr[1]         == 0x00) && ((ptr[2] & 0x80) == 0x80) ) return offs+1;
      ++sbit;
      if (    ((ptr[-1] & 0x01) == 0x00) &&  (ptr[1]         == 0x01)                              ) return offs;
      ++sbit;
      if (    ((ptr[-1] & 0x03) == 0x00) && ((ptr[1] & 0xfe) == 0x02)                              ) return offs;
      ++sbit;
      if (    ((ptr[-1] & 0x07) == 0x00) && ((ptr[1] & 0xfc) == 0x04)                              ) return offs;
      ++sbit;
      if (    ((ptr[-1] & 0x0f) == 0x00) && ((ptr[1] & 0xf8) == 0x08)                              ) return offs;
      ++sbit;
      if (    ((ptr[-1] & 0x1f) == 0x00) && ((ptr[1] & 0xf0) == 0x10)                              ) return offs;
      ++sbit;
      if (    ((ptr[-1] & 0x3f) == 0x00) && ((ptr[1] & 0xe0) == 0x20)                              ) return offs;
      ++sbit;
      if (    ((ptr[-1] & 0x7f) == 0x00) && ((ptr[1] & 0xc0) == 0x40)                              ) return offs;
    }
    ++ptr;
    --len;
  }

  return -1;
}

#endif

///////////////////////////////////////////////////////////////////////////////////////3Y

RFC2190Packetizer::RFC2190Packetizer()
{
  m_buffer = NULL;
}

RFC2190Packetizer::~RFC2190Packetizer()
{
  free(m_buffer);
}

int RFC2190Packetizer::Open(unsigned long _timestamp, unsigned long /*maxLen*/)
{
  // do a sanity check on the fragment data - must be equal to maxLen
  {
    unsigned long len = 0;
    FragmentListType::iterator r;
    for (r = fragments.begin(); r != fragments.end(); ++r) 
      len += r->length;

//    if (len != maxLen) 
//      cout << "rfc2190: mismatch between encoder length and fragment length - " << len << "/" << maxLen << endl;
  }
  
  // save timestamp
  timestamp = _timestamp;

  const unsigned char * data = m_buffer;
  size_t dataLen = m_bufferSize;

  // make sure data is at least long enough to contain PSC, TR & minimum PTYPE, PQUANT and CPM
  if (dataLen < 7)
    return -1;

  // ensure data starts with PSC
  //     0         1         2
  // 0000 0000 0000 0000 1000 00..
  if (FindPSC(data, dataLen) != 0)
    return -2;

  // get TR
  //     2         3    
  // .... ..XX XXXX XX..
  TR = ((data[2] << 6) & 0xfc) | (data[3] >> 2);
      
  // make sure mandatory part of PTYPE is present
  //     3    
  // .... ..10
  if ((data[3] & 0x03) != 2)
    return -3;

  // we don't do split screen, document indicator, full picture freeze
  //     4    
  // XXX. ....
  if ((data[4] & 0xe0) != 0)
    return -4;

  // get image size
  //     4
  // ...X XX..
  frameSize = (data[4] >> 2) & 0x7;
  macroblocksPerGOB = MacroblocksPerGOBTable[frameSize];
  if (macroblocksPerGOB == -1)
    return -6;

  // get I-frame flag
  //     4
  // .... ..X.
  iFrame = (data[4] & 2) == 0;

  // get annex bits:
  //   Annex D - unrestricted motion vector mode
  //   Annex E - syntax-based arithmetic coding mode
  //   Annex F - advanced prediction mode
  //   Annex G - PB-frames mode
  //
  //     4         5
  // .... ...X XXX. ....
  annexD = data[4] & 0x01;
  annexE = data[5] & 0x80;
  annexF = data[5] & 0x40;
  annexG = data[5] & 0x20;

  // annex G not supported 
  if (annexG)
    return -5;

  // get PQUANT
  //     5
  // ...X XXXX
  pQuant = data[5] & 0x1f;

  // get CPM
  //     6
  // X... ....
  cpm = (data[6] & 0x80) != 0;

  // ensure PEI is always 0
  //     6
  // .X.. ....
  if ((data[6] & 0x40) != 0) 
    return -6;

#if 0
  cout << "TR=" << TR 
       << ",size=" << frameSize 
       << ",I=" << iFrame 
       << ",D=" << annexD
       << ",E=" << annexE
       << ",F=" << annexF
       << ",G=" << annexG
       << ",PQUANT=" << pQuant
       << endl;

#endif

  // split fragments longer than the maximum
  FragmentListType::iterator r;
  for (r = fragments.begin(); r != fragments.end(); ++r) {
    while (r->length > MAX_PACKET_LEN) {
      int oldLen = r->length;
      int newLen = MAX_PACKET_LEN;
      if ((oldLen - newLen) < MAX_PACKET_LEN)
        newLen = oldLen / 2;
      fragment oldFrag = *r;
      r = fragments.erase(r);

      fragment frag;
      frag.length = newLen;
      frag.mbNum  = oldFrag.mbNum;
      r = fragments.insert(r, frag);

      frag.length = oldLen - newLen;
      frag.mbNum  = oldFrag.mbNum;
      r = fragments.insert(r, frag);
      // for loop will move r to next entry
    }
  }

  // reset pointers to start of fragments
  currFrag = fragments.begin();
  fragPtr = m_buffer;

  return 0;
}

int RFC2190Packetizer::GetPacket(RTPFrame & outputFrame, unsigned int & flags)
{
  while ((fragments.size() != 0) && (currFrag != fragments.end())) {
      
    // set the timestamp
    outputFrame.SetTimestamp(timestamp);
    fragment frag = *currFrag++;


    // if this fragment starts with a GBSC, then output as Mode A else output as Mode B
    bool modeA = ((frag.length >= 3) &&
                  (fragPtr[0] == 0x00) &&
                  (fragPtr[1] == 0x00) &&
                  ((fragPtr[2] & 0x80) == 0x80));

    size_t payloadRemaining = outputFrame.GetFrameLen() - outputFrame.GetHeaderSize();

    // offset of the data
    size_t offs = modeA ? 4 : 8;

    // make sure RTP storage is sufficient
    if ((frag.length + offs) > payloadRemaining) {
      //std::cout << "no room for Mode " << (modeA ? 'A' : 'B') << " frame - " << (frag.length+offs) << " > " << payloadRemaining << std::endl;
      continue;
    }

    // set size of final frame
    outputFrame.SetPayloadSize(offs + frag.length);

    // get ptr to payload that is about to be created
    unsigned char * ptr = outputFrame.GetPayloadPtr();

    if (modeA) {
      int sBit = 0;
      int eBit = 0;
      ptr[0] = (unsigned char)(((sBit & 7) << 3) | (eBit & 7));
      ptr[1] = (unsigned char)((frameSize << 5) | (iFrame ? 0 : 0x10) | (annexD ? 0x08 : 0) | (annexE ? 0x04 : 0) | (annexF ? 0x02 : 0));
      ptr[2] = ptr[3] = 0;
    }
    else
    {
      // create the Mode B header
      int sBit = 0;
      int eBit = 0;
      int gobn = frag.mbNum / macroblocksPerGOB;
      int mba  = frag.mbNum % macroblocksPerGOB;
      ptr[0] = (unsigned char)(0x80 | ((sBit & 7) << 3) | (eBit & 7));
      ptr[1] = (unsigned char)(frameSize << 5);
      ptr[2] = (unsigned char)(((gobn << 3) & 0xf8) | ((mba >> 6) & 0x7));
      ptr[3] = (unsigned char)((mba << 2) & 0xfc);
      ptr[4] = (iFrame ? 0 : 0x80) | (annexD ? 0x40 : 0) | (annexE ? 0x20 : 0) | (annexF ? 0x010: 0);
      ptr[5] = ptr[6] = ptr[7] = 0;
    }

    // copy the data
    memcpy(ptr + offs, fragPtr, frag.length);

    fragPtr += frag.length;

    // set marker bit
    flags = 0;
    if (currFrag == fragments.end()) {
      flags |= 1;
      outputFrame.SetMarker(1);
    }
    else outputFrame.SetMarker(0);
    
    if (iFrame)
      flags |= 2;

    return 1;
  }

  return 0;
}

///////////////////////////////////////////////////////////////////////////////////////3Y

RFC2190Depacketizer::RFC2190Depacketizer()
{
  NewFrame();
}

void RFC2190Depacketizer::NewFrame()
{
  frame.resize(0);
  first               = true;
  skipUntilEndOfFrame = false;
  lastEbit            = 8;
}

int RFC2190Depacketizer::LostSync(bool & requestIframe, const char * /*reason*/)
{
  skipUntilEndOfFrame = true;
  requestIframe = true;
  return 0;
}

int RFC2190Depacketizer::SetPacket(const RTPFrame & inputFrame, bool & requestIFrame, bool & isIFrame)
{
  requestIFrame = false;
  isIFrame      = false;

  // ignore packets if required
  if (skipUntilEndOfFrame) {
    if (inputFrame.GetMarker()) 
      NewFrame();
    return 0;
  }

  // check if packet is in sequence. If not, skip til end of frame 
  if (first) {
    NewFrame();    // make sure this is called before "first = false"
    first = false;
    lastSequence = inputFrame.GetSequenceNumber();
  }
  else {
    ++lastSequence;
    if (inputFrame.GetSequenceNumber() != lastSequence) {
      return LostSync(requestIFrame, "missed frame");
    }
  }

  unsigned payloadLen = inputFrame.GetPayloadSize();

  // payload must be at least as long as mode A header + 1 byte
  if (payloadLen < 5) 
    return LostSync(requestIFrame, "payload too small");

  unsigned char * payload = inputFrame.GetPayloadPtr();
  unsigned int sbit = (payload[0] >> 3) & 0x07;
  unsigned hdrLen;

  char mode;

  // handle mode A frames
  if ((payload[0] & 0x80) == 0) {
    isIFrame = (payload[1] & 0x10) == 0;
    hdrLen = 4;
    mode = 'A';

#if 0
    // sanity check data
    if (payloadLen < (hdrLen+3) ||
        (payload[hdrLen+0] != 0x00) ||
        (payload[hdrLen+1] != 0x00) ||
        ((payload[hdrLen+2] & 0x80) != 0x80)
       ) {
      return LostSync(requestIFrame, "Mode A packet not starting with GBSC");
    }
#endif
  }

  // handle mode B frames
  else if ((payload[0] & 0x40) == 0) {
    if (payloadLen < 9)
      return LostSync(requestIFrame, "mode B payload too small");
    isIFrame = (payload[4] & 0x80) == 0;
    hdrLen = 8;
    mode = 'B';
  }

  // handle mode C frames
  else {
    if (payloadLen < 13)
      return LostSync(requestIFrame, "mode C payload too small");
    isIFrame = (payload[4] & 0x80) == 0;
    hdrLen = 12;
    mode = 'C';
  }

  // if ebit and sbit do not add up, then we have lost sync
  if (((sbit + lastEbit) & 0x7) != 0) {
    return LostSync(requestIFrame, "mismatched ebit and sbit");
  }

  unsigned char * src = payload + hdrLen;
  size_t cpyLen = payloadLen - hdrLen;

  // handle first partial byte
  if ((sbit != 0) && (frame.size() > 0)) {
    
    static unsigned char smasks[7] = { 0x7f, 0x3f, 0x1f, 0x0f, 0x07, 0x03, 0x01 };
    unsigned smask = smasks[sbit-1];
    frame[frame.size()-1] |= (*src & smask);
    --cpyLen;
    ++src;
  }

  // copy whole bytes
  if (cpyLen > 0) {
    size_t frameSize = frame.size();
    frame.resize(frameSize + cpyLen);
    memcpy(&frame[0] + frameSize, src, cpyLen);
  }

  // keep ebit for next time
  lastEbit = payload[0] & 0x07;

  // return 0 if no frame yet, return 1 if frame is available
  if (!inputFrame.GetMarker()) 
    return 0;

  return 1;
}

