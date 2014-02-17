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
 * $Log: q922.h,v $
 * Revision 1.1  2007/08/06 20:50:50  shorne
 * First commit of h323plus
 *
 * Revision 1.1  2006/06/22 11:07:22  shorne
 * Backport of FECC (H.224) from Opal
 *
 * Revision 1.2  2006/04/23 18:52:19  dsandras
 * Removed warnings when compiling with gcc on Linux.
 *
 * Revision 1.1  2006/04/20 16:48:17  hfriederich
 * Initial version of H.224/H.281 implementation.
 *
 */

#ifndef __OPAL_Q922_H
#define __OPAL_Q922_H

#ifdef P_USE_PRAGMA
#pragma interface
#endif

#include <ptlib.h>

#define Q922_HEADER_SIZE 3

class Q922_Frame : public PBYTEArray
{
  PCLASSINFO(Q922_Frame, PBYTEArray);
	
public:
	
  Q922_Frame(PINDEX informationFieldSize = 260);
  ~Q922_Frame();
	
  BYTE GetHighOrderAddressOctet() const { return theArray[0]; }
  BYTE GetLowOrderAddressOctet() const { return theArray[1]; }
  void SetHighOrderAddressOctet(BYTE octet) { theArray[0] = octet; }
  void SetLowOrderAddressOctet(BYTE octet) { theArray[1] = octet; }
	
  BYTE GetControlFieldOctet() const { return theArray[2]; }
  void SetControlFieldOctet(BYTE octet) { theArray[2] = octet; }
	
  BYTE *GetInformationFieldPtr() const { return (BYTE *)(theArray + Q922_HEADER_SIZE); }
	
  PINDEX GetInformationFieldSize() const { return informationFieldSize; }
  void SetInformationFieldSize(PINDEX size);
	
  /** Decodes a Q.922 frame from a given buffer, returns the success of this operation
	*/
  BOOL Decode(const BYTE *data, PINDEX size);
	
 /** Returns an estimate of the encoded size.
	 The receiver will use at most the size when encoding. Returns zero if encoding will fail.
   */
  PINDEX GetEncodedSize() const;
  
  /** Encodes this Q.922 frame into the given buffer.
	  On return, size contains the number of octets occupied in the buffer.
	*/
  BOOL Encode(BYTE *buffer, PINDEX & size) const;

  /** Encodes this Q.922 frame into the given buffer.
	  On return, size contains the number of octets occupied in the buffer.
	  Use bitPosition to determine at which bit the Q.922 FLAG sequence should begin.
	  On return, bitPosition contains the bit at which the encoded stream ends.
	  bitPosition shall be in the range 0-7, whereas 7 means that the FLAG sequence
	  is encoded at byte boundaries
	*/
  BOOL Encode(BYTE *buffer, PINDEX & size, BYTE & bitPosition) const;
	
protected:
	
  PINDEX informationFieldSize;
	
private:

  inline BOOL FindFlagEnd(const BYTE *buffer, PINDEX bufferSize, PINDEX & octetIndex, BYTE & bitIndex);
  inline BYTE DecodeByte(const BYTE *buffer, BYTE *destination, PINDEX & octetIndex, BYTE & bitIndex, BYTE & onesCounter);
  inline BYTE DecodeBit(const BYTE *buffer, PINDEX & octetIndex, BYTE & bitIndex);
	
  inline void EncodeOctet(BYTE octet, BYTE *buffer, PINDEX & octetIndex, BYTE & bitIndex, BYTE & onesCounter) const;
  inline void EncodeOctetNoEscape(BYTE octet, BYTE *buffer, PINDEX & octetIndex, BYTE & bitIndex) const;
  inline void EncodeBit(BYTE bit, BYTE *buffer, PINDEX & octetIndex, BYTE & bitIndex) const;
	
  inline WORD CalculateFCS(const BYTE*data, PINDEX length) const;
};

#endif // __OPAL_Q922_H

