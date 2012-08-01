/*
 * q922.h
 *
 * Q.922 PDU implementation for the OpenH323 Project.
 *
 * Copyright (c) 2006 Network for Educational Technology, ETH Zurich.
 * Written by Hannes Friederich.
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
 * Contributor(s): ______________________________________.
 *
 * $Log: q922.cxx,v $
 * Revision 1.1  2007/08/06 20:51:07  shorne
 * First commit of h323plus
 *
 * Revision 1.1  2006/06/22 11:07:23  shorne
 * Backport of FECC (H.224) from Opal
 *
 * Revision 1.4  2006/05/03 17:45:51  hfriederich
 * reverting incorrect byte orderings in fcs from previous patch
 *
 * Revision 1.3  2006/05/01 10:29:50  csoutheren
 * Added pragams for gcc < 4
 *
 * Revision 1.2  2006/04/24 12:53:50  rjongbloed
 * Port of H.224 Far End Camera Control to DevStudio/Windows
 *
 * Revision 1.1  2006/04/20 16:48:17  hfriederich
 * Initial version of H.224/H.281 implementation.
 *
 */

#include <ptlib.h>

#ifdef __GNUC__
#pragma implementation "q922.h"
#endif

#ifdef _MSC_VER
#pragma warning(disable : 4244)
#endif

#include <q922.h>

#define Q922_FCS_SIZE 2

#define Q922_OK 0x00
#define Q922_FLAG 0x7e
#define Q922_ERROR 0x7f

/*
 * FCS lookup table.
 * Code based on implementation in RFC1549
 */
static WORD fcstable[256] = {
	0x0000, 0x1189, 0x2312, 0x329b, 0x4624, 0x57ad, 0x6536, 0x74bf,
	0x8c48, 0x9dc1, 0xaf5a, 0xbed3, 0xca6c, 0xdbe5, 0xe97e, 0xf8f7,
	0x1081, 0x0108, 0x3393, 0x221a, 0x56a5, 0x472c, 0x75b7, 0x643e,
	0x9cc9, 0x8d40, 0xbfdb, 0xae52, 0xdaed, 0xcb64, 0xf9ff, 0xe876,
	0x2102, 0x308b, 0x0210, 0x1399, 0x6726, 0x76af, 0x4434, 0x55bd,
	0xad4a, 0xbcc3, 0x8e58, 0x9fd1, 0xeb6e, 0xfae7, 0xc87c, 0xd9f5,
	0x3183, 0x200a, 0x1291, 0x0318, 0x77a7, 0x662e, 0x54b5, 0x453c,
	0xbdcb, 0xac42, 0x9ed9, 0x8f50, 0xfbef, 0xea66, 0xd8fd, 0xc974,
	0x4204, 0x538d, 0x6116, 0x709f, 0x0420, 0x15a9, 0x2732, 0x36bb,
	0xce4c, 0xdfc5, 0xed5e, 0xfcd7, 0x8868, 0x99e1, 0xab7a, 0xbaf3,
	0x5285, 0x430c, 0x7197, 0x601e, 0x14a1, 0x0528, 0x37b3, 0x263a,
	0xdecd, 0xcf44, 0xfddf, 0xec56, 0x98e9, 0x8960, 0xbbfb, 0xaa72,
	0x6306, 0x728f, 0x4014, 0x519d, 0x2522, 0x34ab, 0x0630, 0x17b9,
	0xef4e, 0xfec7, 0xcc5c, 0xddd5, 0xa96a, 0xb8e3, 0x8a78, 0x9bf1,
	0x7387, 0x620e, 0x5095, 0x411c, 0x35a3, 0x242a, 0x16b1, 0x0738,
	0xffcf, 0xee46, 0xdcdd, 0xcd54, 0xb9eb, 0xa862, 0x9af9, 0x8b70,
	0x8408, 0x9581, 0xa71a, 0xb693, 0xc22c, 0xd3a5, 0xe13e, 0xf0b7,
	0x0840, 0x19c9, 0x2b52, 0x3adb, 0x4e64, 0x5fed, 0x6d76, 0x7cff,
	0x9489, 0x8500, 0xb79b, 0xa612, 0xd2ad, 0xc324, 0xf1bf, 0xe036,
	0x18c1, 0x0948, 0x3bd3, 0x2a5a, 0x5ee5, 0x4f6c, 0x7df7, 0x6c7e,
	0xa50a, 0xb483, 0x8618, 0x9791, 0xe32e, 0xf2a7, 0xc03c, 0xd1b5,
	0x2942, 0x38cb, 0x0a50, 0x1bd9, 0x6f66, 0x7eef, 0x4c74, 0x5dfd,
	0xb58b, 0xa402, 0x9699, 0x8710, 0xf3af, 0xe226, 0xd0bd, 0xc134,
	0x39c3, 0x284a, 0x1ad1, 0x0b58, 0x7fe7, 0x6e6e, 0x5cf5, 0x4d7c,
	0xc60c, 0xd785, 0xe51e, 0xf497, 0x8028, 0x91a1, 0xa33a, 0xb2b3,
	0x4a44, 0x5bcd, 0x6956, 0x78df, 0x0c60, 0x1de9, 0x2f72, 0x3efb,
	0xd68d, 0xc704, 0xf59f, 0xe416, 0x90a9, 0x8120, 0xb3bb, 0xa232,
	0x5ac5, 0x4b4c, 0x79d7, 0x685e, 0x1ce1, 0x0d68, 0x3ff3, 0x2e7a,
	0xe70e, 0xf687, 0xc41c, 0xd595, 0xa12a, 0xb0a3, 0x8238, 0x93b1,
	0x6b46, 0x7acf, 0x4854, 0x59dd, 0x2d62, 0x3ceb, 0x0e70, 0x1ff9,
	0xf78f, 0xe606, 0xd49d, 0xc514, 0xb1ab, 0xa022, 0x92b9, 0x8330,
	0x7bc7, 0x6a4e, 0x58d5, 0x495c, 0x3de3, 0x2c6a, 0x1ef1, 0x0f78
};	

Q922_Frame::Q922_Frame(PINDEX size)
: PBYTEArray(Q922_HEADER_SIZE + size)
{
  informationFieldSize = size;
}


Q922_Frame::~Q922_Frame()
{
}


void Q922_Frame::SetInformationFieldSize(PINDEX size)
{
  informationFieldSize = size;
  SetMinSize(Q922_HEADER_SIZE+informationFieldSize);
}


BOOL Q922_Frame::Decode(const BYTE *data, PINDEX size)
{	
  // a valid frame must contain at least 2xFLAG, 3 octets Q922 header,
  // 2 octets FCS and at least 1 octet information
  if(size < 2+3+2+1)
    return FALSE;
	
  PINDEX octetIndex = 0;
  BYTE bitIndex = 7;
  BYTE onesCounter = 0;
	
  if(!FindFlagEnd(data, size, octetIndex, bitIndex))
	return FALSE;
	
  BYTE firstOctet;
  BYTE secondOctet;
	
  // read the two first octets
  if(octetIndex >= size || DecodeByte(data, &firstOctet, octetIndex, bitIndex, onesCounter) != Q922_OK)
    return FALSE;
  
  if(octetIndex >= size || DecodeByte(data, &secondOctet, octetIndex, bitIndex, onesCounter) != Q922_OK)
	return FALSE;
	
  PINDEX arrayIndex = 0;
  while(octetIndex < size) {
	
    BYTE decodedByte;
    BYTE result = DecodeByte(data, &decodedByte, octetIndex, bitIndex, onesCounter);
	
	if(result == Q922_ERROR) {
	  return FALSE;

	} else if(result == Q922_FLAG) {
		
      // Found end flag
      // FCS is contained in firstOctet and secondOctet.
      WORD fcs = (secondOctet << 8) | firstOctet;
			
      // Calculate FCS from data to check
      WORD calculatedFCS = CalculateFCS((const BYTE *)theArray, arrayIndex);
			
	  if(fcs != calculatedFCS) {
	    PTRACE(3, "Q.922 frame has incorrect checksum");
		return FALSE;
      }
			
      if(arrayIndex > Q922_HEADER_SIZE) {
        SetInformationFieldSize(arrayIndex - Q922_HEADER_SIZE);
        return TRUE;
      }
			
	  return FALSE;
	}
		
    theArray[arrayIndex] = firstOctet;
    arrayIndex++;
		
    firstOctet = secondOctet;
    secondOctet = decodedByte;
		
	// Q922-frames must not exceed an information field size of 260 octets
    if(arrayIndex >= 260+Q922_HEADER_SIZE) {
      return FALSE;
	}
  }
	
  return FALSE;
}

PINDEX Q922_Frame::GetEncodedSize() const
{
  if(informationFieldSize == 0) {
	return 0; // cannot encode
  }
	
  // dataSize is High Order Address, Low Order Address, Control, Information and
  // Frame Check Sequence (FCS, 2 Bytes)
  PINDEX dataSize = Q922_HEADER_SIZE + informationFieldSize + Q922_FCS_SIZE;
	
  // Due to possible need for bit insertion, the size may grow.
  // For simplicity, we assume twice the data size, which will certainly be enough.
  // So, encoded size is 3*FLAG + 2*dataSize + 3*FLAG;
  return 3+2*dataSize+3;
}

BOOL Q922_Frame::Encode(BYTE *buffer, PINDEX & size) const
{
  BYTE bitIndex = 7;
  return Encode(buffer, size, bitIndex);
}

BOOL Q922_Frame::Encode(BYTE *buffer, PINDEX & size, BYTE & theBitIndex) const
{
  if(informationFieldSize == 0)	{
    return FALSE;
  }
	
  PINDEX octetIndex = 0;
  BYTE bitIndex = theBitIndex;
  BYTE onesCounter = 0;
	
  // storing three FLAG sequencs.
  // since the FLAG sequences may be not byte-aligned, the first FLAG sequence is encoded
  // into a dummy buffer and extracted from there
  buffer[0] = 0;
  BYTE dummy[3];
  EncodeOctetNoEscape(Q922_FLAG, dummy, octetIndex, bitIndex);
  EncodeOctetNoEscape(Q922_FLAG, dummy, octetIndex, bitIndex);
  buffer[0] = dummy[1];
  buffer[1] = dummy[1];
  octetIndex = 1;
  EncodeOctetNoEscape(Q922_FLAG, buffer, octetIndex, bitIndex);
  EncodeOctetNoEscape(Q922_FLAG, buffer, octetIndex, bitIndex);
	
  // calculating the FCS
  PINDEX dataSize = GetInformationFieldSize() + Q922_HEADER_SIZE;
  WORD fcs = CalculateFCS((const BYTE *)theArray, dataSize);
	
  // Encoding the data byte-by-byte
  PINDEX i;
  PINDEX count = Q922_HEADER_SIZE + informationFieldSize;
  for(i = 0; i < count; i++) {
    EncodeOctet(theArray[i], buffer, octetIndex, bitIndex, onesCounter);
  }
	
  // Encoding the FCS
  EncodeOctet((BYTE)fcs, buffer, octetIndex, bitIndex, onesCounter);
  EncodeOctet((BYTE)(fcs >> 8), buffer, octetIndex, bitIndex, onesCounter);
	
  // Appending three FLAG sequences to the buffer
  // the buffer is not necessary byte aligned!
  EncodeOctetNoEscape(Q922_FLAG, buffer, octetIndex, bitIndex);
  EncodeOctetNoEscape(Q922_FLAG, buffer, octetIndex, bitIndex);
  EncodeOctetNoEscape(Q922_FLAG, buffer, octetIndex, bitIndex);
	
  // determining correct number of octets
  if(bitIndex == 7) {
	octetIndex--;
  }
	
  size = octetIndex;
  theBitIndex = bitIndex;
	
  return TRUE;
  
}

BOOL Q922_Frame::FindFlagEnd(const BYTE *buffer, 
							 PINDEX bufferSize, 
							 PINDEX & octetIndex, BYTE & bitIndex)
{
  BYTE positionsCorrect = 0;
	
  while(octetIndex < bufferSize) {
		
    BYTE bit = DecodeBit(buffer, octetIndex, bitIndex);
		
    switch(positionsCorrect) {
      case 0:
		if(bit == 0) {
		  positionsCorrect = 1;
		}
		break;
      case 1:
      case 2:
      case 3:
      case 4:
      case 5:
      case 6:
        if(bit == 1) {
          positionsCorrect++;
		} else {
          positionsCorrect = 1;
        }
        break;
      case 7:
        if(bit == 0) {
          positionsCorrect = 0xff;
        } else {
          // got 0x7f, ABORT sequence
          return FALSE;
        }
        break;
      default:
        return FALSE;
    }
		
    if(positionsCorrect == 0xff) {
      break;
    }
  }
	
  if(positionsCorrect != 0xff) {
    return FALSE;
  }
	
  // First FLAG sequence found, bit index determined.
  // now check for additinal FLAG sequences
  BYTE octet = Q922_FLAG;
	
  while(octet == Q922_FLAG && octetIndex < bufferSize) {
    
    PINDEX startOctetIndex = octetIndex;
    BYTE startBitIndex = bitIndex;
		
    BYTE positionsCorrect = 0;
		
    unsigned i;
    for(i = 0; i < 8; i++) {
	
      BYTE bit = DecodeBit(buffer, octetIndex, bitIndex);
			
      switch(positionsCorrect) {
	    case 0:
          if(bit == 1) {
            positionsCorrect = 0xf0;
          } else {
            positionsCorrect++;
		  }
          break;
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
          if(bit == 0) {
		    positionsCorrect = 0xf0;
          } else {
            positionsCorrect++;
          }
          break;
		case 7:
		  if(bit == 1) {
			// 0x7f read
		    return FALSE;
		  }
		  break;
        default:
          return FALSE;
      }
			
      if(positionsCorrect == 0xf0) {
        octetIndex = startOctetIndex;
        bitIndex = startBitIndex;
        return TRUE;
      }
    }
  }
	
  return FALSE;
}

BYTE Q922_Frame::DecodeByte(const BYTE *buffer, 
							BYTE *destination, 
							PINDEX & octetIndex,
							BYTE & bitIndex, 
							BYTE & onesCounter)
{
  // decoded byte is copied to destination.
  // returns Q922_OK if decoding succesful,
  // returns Q922_FLAG if ending FLAG detected,
  // returns Q922_ERROR if there was an error
	
  BYTE decodedByte = 0x00;
	
  PINDEX i;
  for(i = 0; i < 8; i++) {
	  
	BYTE bit = DecodeBit(buffer, octetIndex, bitIndex);
		
	if(bit) {
      onesCounter++;
			
      if(onesCounter == 6) {
		  
        // Either FLAG or ERROR
        bit = DecodeBit(buffer, octetIndex, bitIndex);
				
		if(i == 6 && !bit)  { // FLAG is byte aligned AND has only 6 consecutive ones
          return Q922_FLAG;
        } else {
          return Q922_ERROR;
        }
      }
	  
    } else {
	  if(onesCounter == 5) {
	    // discard bit, read again
		bit = DecodeBit(buffer, octetIndex, bitIndex);
	  }
			
      onesCounter = 0;
    }
		
    decodedByte |= (bit << i);
  }
	
  *destination = decodedByte;
  return Q922_OK;
}

BYTE Q922_Frame::DecodeBit(const BYTE *buffer,
						   PINDEX & octetIndex, 
						   BYTE & bitIndex)
{
  BYTE bit = (buffer[octetIndex] >> bitIndex) & 0x01;
	
  if(bitIndex == 0) {
    octetIndex++;
    bitIndex = 8;
  }
  bitIndex--;
	
  return bit;
}

void Q922_Frame::EncodeOctet(BYTE octet, BYTE *buffer,
							 PINDEX & octetIndex,
							 BYTE & bitIndex, 
							 BYTE & onesCounter) const
{
  // data is sent out with LSB first, so we need
  // to reverse the bit direction.
  // In addition, it is required to insert a zero
  // bit after 5 consecutive ones to avoid FLAG emulation
	
  PINDEX i;
  for(i = 0; i < 8; i++) {
    // reading one bit from the octet and write it to the buffer
    BYTE bit = (BYTE)((octet >> i) & 0x01);
		
    EncodeBit(bit, buffer, octetIndex, bitIndex);
		
    if(bit) {
      onesCounter++;
			
      if(onesCounter == 5) {
        // insert a zero bit
        EncodeBit(0, buffer, octetIndex, bitIndex);
        onesCounter = 0;
      }
    } else {
      onesCounter = 0;
    }
  }
}

void Q922_Frame::EncodeOctetNoEscape(BYTE octet, 
									 BYTE *buffer, 
									 PINDEX & octetIndex, 
									 BYTE & bitIndex) const
{
  // data is sent out with LSB first, so we need
  // to reverse the bit direction.
	
  PINDEX i;
  for(i = 0; i < 8; i++) {
    // reating one bit from the octet and write it to the buffer
    BYTE bit = (BYTE)((octet >> i) & 0x01);
		
    EncodeBit(bit, buffer, octetIndex, bitIndex);
  }
}

void Q922_Frame::EncodeBit(BYTE bit, 
						   BYTE *buffer, 
						   PINDEX & octetIndex, 
						   BYTE & bitIndex) const
{
  if(bitIndex == 7) {
    buffer[octetIndex] = 0;
  }
	
  buffer[octetIndex] |= ((bit & 0x01) << bitIndex);
	
  // adjusting bit/byte index
  if(bitIndex == 0) {
    octetIndex++;
    bitIndex = 8;
  }

  bitIndex--;
}

WORD Q922_Frame::CalculateFCS(const BYTE *data, PINDEX length) const
{
  // initial value of FCS is all ones.
  WORD fcs = 0xffff;
	
  while(length--) {
    fcs = (fcs >> 8) ^ fcstable[(fcs ^ *data++) & 0xff];
  }
	
  // take one's complement
  fcs = ~fcs;
	
  return fcs;
}
