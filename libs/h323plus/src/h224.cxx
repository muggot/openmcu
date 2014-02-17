/*
 * h224.cxx
 *
 * H.224 implementation for the OpenH323 Project.
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
 * $Log: h224.cxx,v $
 * Revision 1.2  2007/11/01 14:35:52  willamowius
 * add newline at end of file
 *
 * Revision 1.1  2007/08/06 20:51:05  shorne
 * First commit of h323plus
 *
 * Revision 1.1  2006/06/22 11:07:23  shorne
 * Backport of FECC (H.224) from Opal
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
#include <h323.h>

#ifdef H323_H224

#ifdef __GNUC__
#pragma implementation "h224.h"
#pragma implementation "h224handler.h"
#endif

#ifdef _MSC_VER
#pragma warning(disable : 4244)
#endif

#include <h224.h>
#include <h224handler.h>
#include <h323con.h>
#include <h245.h>


H224_Frame::H224_Frame(PINDEX size)
: Q922_Frame(H224_HEADER_SIZE + size)
{
  SetHighPriority(FALSE);
	
  SetControlFieldOctet(0x03);
	
  BYTE *data = GetInformationFieldPtr();
	
  // setting destination & source terminal address to BROADCAST
  data[0] = 0;
  data[1] = 0;
  data[2] = 0;
  data[3] = 0;
	
  // setting Client ID to CME
  data[4] = 0;
	
  // setting ES / BS / C1 / C0 / Segment number to zero
  data[5] = 0;
}

H224_Frame::~H224_Frame()
{
}

void H224_Frame::SetHighPriority(BOOL flag)
{
  SetHighOrderAddressOctet(0x00);
	
  if(flag) {
    SetLowOrderAddressOctet(0x71);
  } else {
    SetLowOrderAddressOctet(0x061);
  }
}

WORD H224_Frame::GetDestinationTerminalAddress() const
{
  BYTE *data = GetInformationFieldPtr();
  return (WORD)((data[0] << 8) | data[1]);
}

void H224_Frame::SetDestinationTerminalAddress(WORD address)
{
  BYTE *data = GetInformationFieldPtr();
  data[0] = (BYTE)(address >> 8);
  data[1] = (BYTE) address;
}

WORD H224_Frame::GetSourceTerminalAddress() const
{
  BYTE *data = GetInformationFieldPtr();
  return (WORD)((data[2] << 8) | data[3]);
}

void H224_Frame::SetSourceTerminalAddress(WORD address)
{
  BYTE *data = GetInformationFieldPtr();
  data[2] = (BYTE)(address >> 8);
  data[3] = (BYTE) address;
}

BYTE H224_Frame::GetClientID() const
{
  BYTE *data = GetInformationFieldPtr();
	
  return data[4] & 0x7f;
}

void H224_Frame::SetClientID(BYTE clientID)
{
  // At the moment, only H.281 (client ID 0x01)
  // is supported
  PAssert(clientID <= 0x01, "Invalid client ID");
	
  BYTE *data = GetInformationFieldPtr();
	
  data[4] = clientID;
}

BOOL H224_Frame::GetBS() const
{
  BYTE *data = GetInformationFieldPtr();
	
  return (data[5] & 0x80) != 0;
}

void H224_Frame::SetBS(BOOL flag)
{
  BYTE *data = GetInformationFieldPtr();
	
  if(flag) {
    data[5] |= 0x80;
  }	else {
    data[5] &= 0x7f;
  }
}

BOOL H224_Frame::GetES() const
{
  BYTE *data = GetInformationFieldPtr();
	
  return (data[5] & 0x40) != 0;
}

void H224_Frame::SetES(BOOL flag)
{
  BYTE *data = GetInformationFieldPtr();
	
  if(flag) {
    data[5] |= 0x40;
  } else {
    data[5] &= 0xbf;
  }
}

BOOL H224_Frame::GetC1() const
{
  BYTE *data = GetInformationFieldPtr();
	
  return (data[5] & 0x20) != 0;
}

void H224_Frame::SetC1(BOOL flag)
{
  BYTE *data = GetInformationFieldPtr();
	
  if(flag) {
    data[5] |= 0x20;
  } else {
    data[5] &= 0xdf;
  }
}

BOOL H224_Frame::GetC0() const
{
  BYTE *data = GetInformationFieldPtr();
	
  return (data[5] & 0x10) != 0;
}

void H224_Frame::SetC0(BOOL flag)
{
  BYTE *data = GetInformationFieldPtr();
	
  if(flag) {
    data[5] |= 0x10;
  }	else {
    data[5] &= 0xef;
  }
}

BYTE H224_Frame::GetSegmentNumber() const
{
  BYTE *data = GetInformationFieldPtr();
	
  return (data[5] & 0x0f);
}

void H224_Frame::SetSegmentNumber(BYTE segmentNumber)
{
  BYTE *data = GetInformationFieldPtr();
	
  data[5] &= 0xf0;
  data[5] |= (segmentNumber & 0x0f);
}

BOOL H224_Frame::Decode(const BYTE *data, 
						PINDEX size)
{
  BOOL result = Q922_Frame::Decode(data, size);
	
  if(result == FALSE) {
	return FALSE;
  }
	
  // doing some validity check for H.224 frames
  BYTE highOrderAddressOctet = GetHighOrderAddressOctet();
  BYTE lowOrderAddressOctet = GetLowOrderAddressOctet();
  BYTE controlFieldOctet = GetControlFieldOctet();
	
  if((highOrderAddressOctet != 0x00) ||
     (!(lowOrderAddressOctet == 0x61 || lowOrderAddressOctet == 0x71)) ||
     (controlFieldOctet != 0x03) ||
     (GetClientID() > 0x02))
  {		
	  return FALSE;
  }
	
  return TRUE;
}

////////////////////////////////////

OpalH224Handler::OpalH224Handler(H323Connection & connection,
								 unsigned sessionID)
: transmitMutex()
{
  // Really need to check this?

  RTP_Session *session;
  H245_TransportAddress addr;
  connection.GetControlChannel().SetUpTransportPDU(addr, H323Transport::UseLocalTSAP);
  session = connection.UseSession(sessionID,addr,H323Channel::IsBidirectional);

  h281Handler = connection.CreateH281ProtocolHandler(*this);
  receiverThread = NULL;
	
}

OpalH224Handler::~OpalH224Handler()
{
  delete h281Handler;
}

void OpalH224Handler::StartTransmit()
{
  PWaitAndSignal m(transmitMutex);
	
  if(canTransmit == TRUE) {
    return;
  }
	
  canTransmit = TRUE;
	
  transmitFrame = new RTP_DataFrame(300);
  
  // Use payload code 100 as this seems to be common to other implementations
  transmitFrame->SetPayloadType((RTP_DataFrame::PayloadTypes)100);
  transmitBitIndex = 7;
  transmitStartTime = new PTime();
	
  SendClientList();
  SendExtraCapabilities();
}

void OpalH224Handler::StopTransmit()
{
  PWaitAndSignal m(transmitMutex);
	
  delete transmitStartTime;
  transmitStartTime = NULL;
	
  canTransmit = FALSE;
}

void OpalH224Handler::StartReceive()
{
  if(receiverThread != NULL) {
    PTRACE(5, "H.224 handler is already receiving");
    return;
  }
	
  receiverThread = CreateH224ReceiverThread();
  receiverThread->Resume();
}

void OpalH224Handler::StopReceive()
{
  if(receiverThread != NULL) {
    receiverThread->Close();
  }
}

BOOL OpalH224Handler::SendClientList()
{
  PWaitAndSignal m(transmitMutex);
	
  if(canTransmit == FALSE) {
    return FALSE;
  }
	
  H224_Frame h224Frame = H224_Frame(4);
  h224Frame.SetHighPriority(TRUE);
  h224Frame.SetDestinationTerminalAddress(H224_BROADCAST);
  h224Frame.SetSourceTerminalAddress(H224_BROADCAST);
	
  // CME frame
  h224Frame.SetClientID(0x00);
	
  // Begin and end of sequence
  h224Frame.SetBS(TRUE);
  h224Frame.SetES(TRUE);
  h224Frame.SetC1(FALSE);
  h224Frame.SetC0(FALSE);
  h224Frame.SetSegmentNumber(0);
	
  BYTE *ptr = h224Frame.GetClientDataPtr();
	
  ptr[0] = 0x01; // Client list code
  ptr[1] = 0x00; // Message code
  ptr[2] = 0x01; // one client
  ptr[3] = (0x80 | H281_CLIENT_ID); // H.281 with etra capabilities
	
  TransmitFrame(h224Frame);
	
  return TRUE;
}

BOOL OpalH224Handler::SendExtraCapabilities()
{
  PWaitAndSignal m(transmitMutex);
	
  if(canTransmit == FALSE) {
    return FALSE;
  }
	
  h281Handler->SendExtraCapabilities();
	
  return TRUE;
}

BOOL OpalH224Handler::SendClientListCommand()
{
  PWaitAndSignal m(transmitMutex);
	
  if(canTransmit == FALSE) {
    return FALSE;
  }
	
  H224_Frame h224Frame = H224_Frame(2);
  h224Frame.SetHighPriority(TRUE);
  h224Frame.SetDestinationTerminalAddress(H224_BROADCAST);
  h224Frame.SetSourceTerminalAddress(H224_BROADCAST);
	
  // CME frame
  h224Frame.SetClientID(0x00);
	
  // Begin and end of sequence
  h224Frame.SetBS(TRUE);
  h224Frame.SetES(TRUE);
  h224Frame.SetC1(FALSE);
  h224Frame.SetC0(FALSE);
  h224Frame.SetSegmentNumber(0);
	
  BYTE *ptr = h224Frame.GetClientDataPtr();
	
  ptr[0] = 0x01; // Client list code
  ptr[1] = 0xff; // Command code
	
  TransmitFrame(h224Frame);
	
  return TRUE;
}

BOOL OpalH224Handler::SendExtraCapabilitiesCommand(BYTE clientID)
{
  PWaitAndSignal m(transmitMutex);
	
  if(canTransmit == FALSE) {
    return FALSE;
  }
	
  if(clientID != H281_CLIENT_ID) {
    return FALSE;
  }
	
  H224_Frame h224Frame = H224_Frame(4);
  h224Frame.SetHighPriority(TRUE);
  h224Frame.SetDestinationTerminalAddress(H224_BROADCAST);
  h224Frame.SetSourceTerminalAddress(H224_BROADCAST);
	
  // CME frame
  h224Frame.SetClientID(0x00);
	
  // Begin and end of sequence
  h224Frame.SetBS(TRUE);
  h224Frame.SetES(TRUE);
  h224Frame.SetC1(FALSE);
  h224Frame.SetC0(FALSE);
  h224Frame.SetSegmentNumber(0);
	
  BYTE *ptr = h224Frame.GetClientDataPtr();
	
  ptr[0] = 0x01; // Client list code
  ptr[1] = 0xFF; // Response code
  ptr[2] = (0x80 | clientID); // clientID with extra capabilities
	
  TransmitFrame(h224Frame);
	
  return TRUE;
}

BOOL OpalH224Handler::SendExtraCapabilitiesMessage(BYTE clientID, 
												   BYTE *data, PINDEX length)
{	
  PWaitAndSignal m(transmitMutex);
	
  // only H.281 supported at the moment
  if(clientID != H281_CLIENT_ID) {
	
    return FALSE;
  }
	
  if(canTransmit == FALSE) {
    return FALSE;
  }
	
  H224_Frame h224Frame = H224_Frame(length+3);
  h224Frame.SetHighPriority(TRUE);
  h224Frame.SetDestinationTerminalAddress(H224_BROADCAST);
  h224Frame.SetSourceTerminalAddress(H224_BROADCAST);
	
  // use clientID zero to indicate a CME frame
  h224Frame.SetClientID(0x00);
	
  // Begin and end of sequence, rest is zero
  h224Frame.SetBS(TRUE);
  h224Frame.SetES(TRUE);
  h224Frame.SetC1(FALSE);
  h224Frame.SetC0(FALSE);
  h224Frame.SetSegmentNumber(0);
	
  BYTE *ptr = h224Frame.GetClientDataPtr();
	
  ptr[0] = 0x02; // Extra Capabilities code
  ptr[1] = 0x00; // Response Code
  ptr[2] = (0x80 | clientID); // EX CAPS and ClientID
	
  memcpy(ptr+3, data, length);
	
  TransmitFrame(h224Frame);
	
  return TRUE;	
}

BOOL OpalH224Handler::TransmitClientFrame(BYTE clientID, H224_Frame & frame)
{
  PWaitAndSignal m(transmitMutex);
	
  // only H.281 is supported at the moment
  if(clientID != H281_CLIENT_ID) {
    return FALSE;
  }
	
  frame.SetClientID(clientID);
	
  TransmitFrame(frame);
	
  return TRUE;
}

BOOL OpalH224Handler::OnReceivedFrame(H224_Frame & frame)
{
  if(frame.GetDestinationTerminalAddress() != H224_BROADCAST) {
    // only broadcast frames are handled at the moment
	PTRACE(3, "Received H.224 frame with non-broadcast address");
    return TRUE;
  }
  BYTE clientID = frame.GetClientID();
	
  if(clientID == 0x00) {
    return OnReceivedCMEMessage(frame);
  }
	
  if(clientID == H281_CLIENT_ID)	{
    h281Handler->OnReceivedMessage((const H281_Frame &)frame);
  }
	
  return TRUE;
}

BOOL OpalH224Handler::OnReceivedCMEMessage(H224_Frame & frame)
{
  BYTE *data = frame.GetClientDataPtr();
	
  if(data[0] == 0x01) { // Client list code
	
    if(data[1] == 0x00) { // Message
      return OnReceivedClientList(frame);
		
    } else if(data[1] == 0xff) { // Command
      return OnReceivedClientListCommand();
    }
	  
  } else if(data[0] == 0x02) { // Extra Capabilities code
	  
    if(data[1] == 0x00) { // Message
      return OnReceivedExtraCapabilities(frame);
		
    } else if(data[1] == 0xff) {// Command
      return OnReceivedExtraCapabilitiesCommand();
    }
  }
	
  // incorrect frames are simply ignored
  return TRUE;
}

BOOL OpalH224Handler::OnReceivedClientList(H224_Frame & frame)
{
  BYTE *data = frame.GetClientDataPtr();
	
  BYTE numberOfClients = data[2];
	
  PINDEX i = 3;
	
  BOOL remoteHasH281 = FALSE;
	
  while(numberOfClients > 0) {
	  
	BYTE clientID = (data[i] & 0x7f);
		
	if(clientID == H281_CLIENT_ID) {
	  remoteHasH281 = TRUE;
	  i++;
	} else if(clientID == 0x7e) { // extended client ID
      i += 2;
    } else if(clientID == 0x7f) { // non-standard client ID
      i += 6;
    } else { // other standard client ID such as T.140
      i++;
    }
    numberOfClients--;
  }
	
  h281Handler->SetRemoteHasH281(remoteHasH281);
	
  return TRUE;
}

BOOL OpalH224Handler::OnReceivedClientListCommand()
{
  SendClientList();
  return TRUE;
}

BOOL OpalH224Handler::OnReceivedExtraCapabilities(H224_Frame & frame)
{
  BYTE *data = frame.GetClientDataPtr();
	
  BYTE clientID = (data[2] & 0x7f);
	
  if(clientID == H281_CLIENT_ID) {
    PINDEX size = frame.GetClientDataSize() - 3;
    h281Handler->OnReceivedExtraCapabilities((data + 3), size);
  }
	
  return TRUE;
}

BOOL OpalH224Handler::OnReceivedExtraCapabilitiesCommand()
{
  SendExtraCapabilities();
  return TRUE;
}

OpalH224ReceiverThread * OpalH224Handler::CreateH224ReceiverThread()
{
  return new OpalH224ReceiverThread(this, *session);
}

void OpalH224Handler::TransmitFrame(H224_Frame & frame)
{	
  PINDEX size = frame.GetEncodedSize();
	
  if(!frame.Encode(transmitFrame->GetPayloadPtr(), size, transmitBitIndex)) {
    PTRACE(3, "Failed to encode H.224 frame");
    return;
  }
	
  // determining correct timestamp
  PTime currentTime = PTime();
  PTimeInterval timePassed = currentTime - *transmitStartTime;
  transmitFrame->SetTimestamp((DWORD)timePassed.GetMilliSeconds() * 8);
  
  transmitFrame->SetPayloadSize(size);
  transmitFrame->SetMarker(TRUE);
	
  if(!session->WriteData(*transmitFrame)) {
    PTRACE(3, "Failed to write encoded H.224 frame");
  }
}

////////////////////////////////////

OpalH224ReceiverThread::OpalH224ReceiverThread(OpalH224Handler *theH224Handler, RTP_Session & session)
: PThread(10000, NoAutoDeleteThread, HighestPriority, "H.224 Receiver Thread"),
  rtpSession(session)
{
  h224Handler = theH224Handler;
  timestamp = 0;
  terminate = FALSE;
}

OpalH224ReceiverThread::~OpalH224ReceiverThread()
{
}

void OpalH224ReceiverThread::Main()
{	
  RTP_DataFrame packet = RTP_DataFrame(300);
  H224_Frame h224Frame = H224_Frame();
	
  for (;;) {
	  
    inUse.Wait();
		
    if(!rtpSession.ReadBufferedData(timestamp, packet)) {
      inUse.Signal();
      return;
    }
	
    timestamp = packet.GetTimestamp();
		
    if(h224Frame.Decode(packet.GetPayloadPtr(), packet.GetPayloadSize())) {
      BOOL result = h224Handler->OnReceivedFrame(h224Frame);

      if(result == FALSE) {
        // FALSE indicates a serious problem, therefore the thread is closed
        return;
      }
    } else {
	  PTRACE(3, "Decoding of H.224 frame failed");
    }
		
    inUse.Signal();
		
    if(terminate == TRUE) {
      return;
    }
  }
}

void OpalH224ReceiverThread::Close()
{
  rtpSession.Close(TRUE);
	
  inUse.Wait();
	
  terminate = TRUE;
	
  inUse.Signal();
	
  PAssert(WaitForTermination(10000), "H224 receiver thread not terminated");
}

#endif // H323_H224

