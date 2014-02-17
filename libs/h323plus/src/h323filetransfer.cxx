/*
 * h323filetransfer.cxx
 *
 * H323 File Transfer class.
 *
 * h323plus library
 *
 * Copyright (c) 2008 ISVO (Asia) Pte. Ltd.
 *
 * The contents of this file are subject to the Mozilla Public License
 * Version 1.1 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Alternatively, the contents of this file may be used under the terms
 * of the General Public License (the  "GNU License"), in which case the
 * provisions of GNU License are applicable instead of those
 * above. If you wish to allow use of your version of this file only
 * under the terms of the GNU License and not to allow others to use
 * your version of this file under the MPL, indicate your decision by
 * deleting the provisions above and replace them with the notice and
 * other provisions required by the GNU License. If you do not delete
 * the provisions above, a recipient may use your version of this file
 * under either the MPL or the GNU License."
 *
 * Software distributed under the License is distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See
 * the License for the specific language governing rights and limitations
 * under the License.
 *
 *
 * The Initial Developer of the Original Code is ISVO (Asia) Pte. Ltd.
 *
 * Contributor(s): ______________________________________.
 *
 * $Log: h323filetransfer.cxx,v $
 * Revision 1.2  2008/02/12 05:47:42  shorne
 * Fix compiling on older GCC versions
 *
 * Revision 1.1  2008/01/01 00:16:12  shorne
 * Added GnuGknat and FileTransfer support
 *
 *
 *
 */

#include <ptlib.h>
#include <h323.h>

#ifdef H323_FILE

#ifdef __GNUC__
#pragma implementation "h323filetransfer.h"
#endif

#include "h323filetransfer.h"

#include <h323pdu.h>


static const char * FileTransferOID = "1.3.6.1.4.1.17090.0.3";

static struct  {
   int blocksize;
   int identifier;
} paramBlockSize[8] = {
  {  512,    1 },
  { 1024,    2 },
  { 1428,    4 },
  { 2048,    8 },
  { 4096,   16 },
  { 8192,   32 },
  { 16384,  64 },
  { 32768, 128 },
};

static int SetParameterBlockSize(int size)
{
	for (PINDEX i = 0; i < 8 ; i++) {
		if (paramBlockSize[i].blocksize == size)
			return paramBlockSize[i].identifier;
	}
	return 16;
}

static int GetParameterBlockSize(int size)
{
	for (PINDEX i = 0; i < 8 ; i++) {
		if (paramBlockSize[i].identifier == size)
			return paramBlockSize[i].blocksize;
	}
	return 16;
}

H323FileTransferCapability::H323FileTransferCapability(unsigned maxBitRate)
: H323DataCapability(maxBitRate)
{
    m_blockSize = 16;      // Indicating blocks are 4096 octets
	m_blockOctets = 4096;  // blockOctets
	m_transferMode = 1;    // Transfer mode is RTP encaptulated
}

BOOL H323FileTransferCapability::OnReceivedPDU(const H245_DataApplicationCapability & pdu)
{
  if (pdu.m_application.GetTag() != H245_DataApplicationCapability_application::e_genericDataCapability)
        return FALSE;

  maxBitRate = pdu.m_maxBitRate;
  const H245_GenericCapability & genCapability = (const H245_GenericCapability &)pdu.m_application;
  return OnReceivedPDU(genCapability);
}

BOOL H323FileTransferCapability::OnSendingPDU(H245_DataApplicationCapability & pdu) const
{
  pdu.m_maxBitRate = maxBitRate;
  pdu.m_application.SetTag(H245_DataApplicationCapability_application::e_genericDataCapability);
	
  H245_GenericCapability & genCapability = (H245_GenericCapability &)pdu.m_application;
  return OnSendingPDU(genCapability);
}

PObject::Comparison H323FileTransferCapability::Compare(const PObject & obj) const
{
  if (!PIsDescendant(&obj, H323FileTransferCapability))
      return LessThan;

  const H323FileTransferCapability & other = (const H323FileTransferCapability &)obj;

  // We only support block sizes of 8196 octets in RTP encaptulation mode
  if ((m_blockSize == other.GetBlockSize()) &&
      (m_transferMode == other.GetTransferMode()))
                return EqualTo;

  return GreaterThan;
}

BOOL H323FileTransferCapability::OnReceivedPDU(const H245_GenericCapability & pdu)
{

   const H245_CapabilityIdentifier & capId = pdu.m_capabilityIdentifier; 

   if (capId.GetTag() != H245_CapabilityIdentifier::e_standard)
	   return FALSE;
      
   const PASN_ObjectId & id = capId;
   if (id.AsString() != FileTransferOID)
	   return FALSE;

   if (pdu.HasOptionalField(H245_GenericCapability::e_maxBitRate)) {
      const PASN_Integer & bitRate = pdu.m_maxBitRate;
      maxBitRate = bitRate;
   }

   if (!pdu.HasOptionalField(H245_GenericCapability::e_collapsing)) 
		return FALSE;
 
   const H245_ArrayOf_GenericParameter & params = pdu.m_collapsing;
   for (PINDEX j=0; j<params.GetSize(); j++) {
	 const H245_GenericParameter & content = params[j];
	 if (content.m_parameterIdentifier.GetTag() == H245_ParameterIdentifier::e_standard) {
       const PASN_Integer & id = (const PASN_Integer &)content.m_parameterIdentifier;
        if (content.m_parameterValue.GetTag() == H245_ParameterValue::e_booleanArray) {
		  const PASN_Integer & val = (const PASN_Integer &)content.m_parameterValue;
		     if (id == 1) {
			   m_blockSize = val;
			   m_blockOctets = GetParameterBlockSize(m_blockSize);
		    }
			if (id == 2)
			   m_transferMode = val;
		}
	  }
	}

  return TRUE;
}

BOOL H323FileTransferCapability::OnSendingPDU(H245_GenericCapability & pdu) const
{
   H245_CapabilityIdentifier & capId = pdu.m_capabilityIdentifier; 

   capId.SetTag(H245_CapabilityIdentifier::e_standard);
   PASN_ObjectId & id = capId;
   id.SetValue(FileTransferOID);

   pdu.IncludeOptionalField(H245_GenericCapability::e_maxBitRate);
   PASN_Integer & bitRate = pdu.m_maxBitRate;
   bitRate = maxBitRate;

   // Add the Block size parameter
   H245_GenericParameter * blockparam = new H245_GenericParameter;
   blockparam->m_parameterIdentifier.SetTag(H245_ParameterIdentifier::e_standard);
   (PASN_Integer &)blockparam->m_parameterIdentifier = 1;
   blockparam->m_parameterValue.SetTag(H245_ParameterValue::e_booleanArray);
   (PASN_Integer &)blockparam->m_parameterValue = SetParameterBlockSize(m_blockOctets);  
   
   // Add the Transfer Mode parameter
   H245_GenericParameter * modeparam = new H245_GenericParameter;
   modeparam->m_parameterIdentifier.SetTag(H245_ParameterIdentifier::e_standard);
   (PASN_Integer &)modeparam->m_parameterIdentifier = 2;
   modeparam->m_parameterValue.SetTag(H245_ParameterValue::e_booleanArray);
   (PASN_Integer &)modeparam->m_parameterValue = m_transferMode;  
   
   pdu.HasOptionalField(H245_GenericCapability::e_collapsing);
   pdu.m_collapsing.Append(blockparam);
   pdu.m_collapsing.Append(modeparam);

   return TRUE;
}

unsigned H323FileTransferCapability::GetSubType() const
{
	return H245_DataApplicationCapability_application::e_genericDataCapability;
}

H323Channel * H323FileTransferCapability::CreateChannel(H323Connection & connection,   
      H323Channel::Directions direction,unsigned sessionID,const H245_H2250LogicalChannelParameters * /*param*/
) const
{
	RTP_Session *session;
    H245_TransportAddress addr;
    connection.GetControlChannel().SetUpTransportPDU(addr, H323Transport::UseLocalTSAP);
    session = connection.UseSession(sessionID, addr, direction);
	
  if(session == NULL) {
    return NULL;
  } 
  
  return new H323FileTransferChannel(connection, *this, direction, (RTP_UDP &)*session, sessionID);
}

/////////////////////////////////////////////////////////////////////////////////

H323FileTransferChannel::H323FileTransferChannel(H323Connection & connection,
                                 const H323Capability & capability,
                                 H323Channel::Directions theDirection,
                                 RTP_UDP & rtp,
                                 unsigned theSessionID
                                 )
 : H323Channel(connection, capability),
  rtpSession(rtp),
  rtpCallbacks(*(H323_RTP_Session *)rtp.GetUserData())
{

  direction = theDirection;
  sessionID = theSessionID;
	
  rtpPayloadType = (RTP_DataFrame::PayloadTypes)101;

  // Call back to load the file list
  if (direction == H323Channel::IsTransmitter) {
    fileHandler = connection.CreateFileTransferHandler(sessionID,direction,filelist);
	if (fileHandler != NULL) {
	  if (fileHandler->GetBlockSize() == 0)
         fileHandler->SetBlockSize((H323FileTransferCapability::blockSizes)
		                               ((const H323FileTransferCapability &)capability).GetOctetSize());
	  if (fileHandler->GetBlockRate() == 0)
         fileHandler->SetMaxBlockRate((H323FileTransferCapability::blockSizes)
		                               ((const H323FileTransferCapability &)capability).GetBlockRate());
	}
  } else
      fileHandler = NULL;
}


H323FileTransferChannel::~H323FileTransferChannel()
{
}

H323Channel::Directions H323FileTransferChannel::GetDirection() const
{
  return direction;
}

BOOL H323FileTransferChannel::SetInitialBandwidth()
{
  return TRUE;
}

void H323FileTransferChannel::Receive()
{

}

void H323FileTransferChannel::Transmit()
{

}

BOOL H323FileTransferChannel::Open()
{
  return H323Channel::Open();
}

BOOL H323FileTransferChannel::Start()
{
  if (!Open())
    return FALSE;
	
  if(fileHandler == NULL) {
	 fileHandler = connection.CreateFileTransferHandler(sessionID,direction,filelist);
	if (fileHandler != NULL) {
     fileHandler->SetPayloadType(rtpPayloadType);
	  if (fileHandler->GetBlockSize() == 0)
         fileHandler->SetBlockSize((H323FileTransferCapability::blockSizes)
		                             ((H323FileTransferCapability *)capability)->GetOctetSize());
	  if (fileHandler->GetBlockRate() == 0)
         fileHandler->SetMaxBlockRate((H323FileTransferCapability::blockSizes)
		                               ((H323FileTransferCapability *)capability)->GetBlockRate());
	}
  }

  if (fileHandler == NULL)
     return FALSE;
 
  return fileHandler->Start(direction);
}

void H323FileTransferChannel::Close()
{
  if (terminating) 
      return;

  if (fileHandler != NULL)
      fileHandler->Stop(direction);
}

BOOL H323FileTransferChannel::OnSendingPDU(H245_OpenLogicalChannel & open) const
{
  open.m_forwardLogicalChannelNumber = (unsigned)number;

  if (direction == H323Channel::IsTransmitter) 
                SetFileList(open,filelist);
		
  if (open.HasOptionalField(H245_OpenLogicalChannel::e_reverseLogicalChannelParameters)) {
	  
	open.m_reverseLogicalChannelParameters.IncludeOptionalField(
		H245_OpenLogicalChannel_reverseLogicalChannelParameters::e_multiplexParameters);
			
    open.m_reverseLogicalChannelParameters.m_multiplexParameters.SetTag(
		H245_OpenLogicalChannel_reverseLogicalChannelParameters_multiplexParameters::e_h2250LogicalChannelParameters);
			
    return OnSendingPDU(open.m_reverseLogicalChannelParameters.m_multiplexParameters);
	
  }	else {
	  
    open.m_forwardLogicalChannelParameters.m_multiplexParameters.SetTag(
		H245_OpenLogicalChannel_forwardLogicalChannelParameters_multiplexParameters::e_h2250LogicalChannelParameters);
		
    return OnSendingPDU(open.m_forwardLogicalChannelParameters.m_multiplexParameters);
  }
}

void H323FileTransferChannel::OnSendOpenAck(const H245_OpenLogicalChannel & openPDU, 
										H245_OpenLogicalChannelAck & ack) const
{
  // set forwardMultiplexAckParameters option
  ack.IncludeOptionalField(H245_OpenLogicalChannelAck::e_forwardMultiplexAckParameters);
	
  // select H225 choice
  ack.m_forwardMultiplexAckParameters.SetTag(
    H245_OpenLogicalChannelAck_forwardMultiplexAckParameters::e_h2250LogicalChannelAckParameters);
	
  // get H225 params
  H245_H2250LogicalChannelAckParameters & param = ack.m_forwardMultiplexAckParameters;
	
  // set session ID
  param.IncludeOptionalField(H245_H2250LogicalChannelAckParameters::e_sessionID);
  const H245_H2250LogicalChannelParameters & openparam =
	  openPDU.m_forwardLogicalChannelParameters.m_multiplexParameters;
	
  unsigned sessionID = openparam.m_sessionID;
  param.m_sessionID = sessionID;
	
  OnSendOpenAck(param);
}

BOOL H323FileTransferChannel::OnReceivedPDU(const H245_OpenLogicalChannel & open,
									 unsigned & errorCode)
{
  if (direction == H323Channel::IsReceiver) {
    number = H323ChannelNumber(open.m_forwardLogicalChannelNumber, TRUE);
	GetFileList(open);
  }
	
  BOOL reverse = open.HasOptionalField(H245_OpenLogicalChannel::e_reverseLogicalChannelParameters);
  const H245_DataType & dataType = reverse ? open.m_reverseLogicalChannelParameters.m_dataType
										   : open.m_forwardLogicalChannelParameters.m_dataType;
	
  if (!capability->OnReceivedPDU(dataType, direction)) {
	  
    errorCode = H245_OpenLogicalChannelReject_cause::e_dataTypeNotSupported;
    return FALSE;
  }
	
  if (reverse) {
    if (open.m_reverseLogicalChannelParameters.m_multiplexParameters.GetTag() ==
			H245_OpenLogicalChannel_reverseLogicalChannelParameters_multiplexParameters::e_h2250LogicalChannelParameters) 
	{
      return OnReceivedPDU(open.m_reverseLogicalChannelParameters.m_multiplexParameters, errorCode);
    }
	  
  } else {
    if (open.m_forwardLogicalChannelParameters.m_multiplexParameters.GetTag() ==
			H245_OpenLogicalChannel_forwardLogicalChannelParameters_multiplexParameters::e_h2250LogicalChannelParameters)
    {
      return OnReceivedPDU(open.m_forwardLogicalChannelParameters.m_multiplexParameters, errorCode);
    }
  }

  errorCode = H245_OpenLogicalChannelReject_cause::e_unsuitableReverseParameters;
  return FALSE;
}

BOOL H323FileTransferChannel::OnReceivedAckPDU(const H245_OpenLogicalChannelAck & ack)
{
  if (!ack.HasOptionalField(H245_OpenLogicalChannelAck::e_forwardMultiplexAckParameters)) {
    return FALSE;
  }
	
  if (ack.m_forwardMultiplexAckParameters.GetTag() !=
	H245_OpenLogicalChannelAck_forwardMultiplexAckParameters::e_h2250LogicalChannelAckParameters)
  {
	return FALSE;
  }
	
  return OnReceivedAckPDU(ack.m_forwardMultiplexAckParameters);
}

BOOL H323FileTransferChannel::OnSendingPDU(H245_H2250LogicalChannelParameters & param) const
{
  param.m_sessionID = sessionID;
	
  param.IncludeOptionalField(H245_H2250LogicalChannelParameters::e_mediaGuaranteedDelivery);
  param.m_mediaGuaranteedDelivery = FALSE;
	
  // unicast must have mediaControlChannel
  H323TransportAddress mediaControlAddress(rtpSession.GetLocalAddress(), rtpSession.GetLocalControlPort());
  param.IncludeOptionalField(H245_H2250LogicalChannelParameters::e_mediaControlChannel);
  mediaControlAddress.SetPDU(param.m_mediaControlChannel);
	
  if (direction == H323Channel::IsReceiver) {
    // set mediaChannel
    H323TransportAddress mediaAddress(rtpSession.GetLocalAddress(), rtpSession.GetLocalDataPort());
    param.IncludeOptionalField(H245_H2250LogicalChannelAckParameters::e_mediaChannel);
    mediaAddress.SetPDU(param.m_mediaChannel);
	
  }	else{

  }
	
  // Set dynamic payload type, if is one
  int rtpPayloadType = GetDynamicRTPPayloadType();
  
  if (rtpPayloadType >= RTP_DataFrame::DynamicBase && rtpPayloadType < RTP_DataFrame::IllegalPayloadType) {
    param.IncludeOptionalField(H245_H2250LogicalChannelParameters::e_dynamicRTPPayloadType);
    param.m_dynamicRTPPayloadType = rtpPayloadType;
  }
	
  return TRUE;
}

void H323FileTransferChannel::OnSendOpenAck(H245_H2250LogicalChannelAckParameters & param) const
{
  // set mediaControlChannel
  H323TransportAddress mediaControlAddress(rtpSession.GetLocalAddress(), rtpSession.GetLocalControlPort());
  param.IncludeOptionalField(H245_H2250LogicalChannelAckParameters::e_mediaControlChannel);
  mediaControlAddress.SetPDU(param.m_mediaControlChannel);
	
  // set mediaChannel
  H323TransportAddress mediaAddress(rtpSession.GetLocalAddress(), rtpSession.GetLocalDataPort());
  param.IncludeOptionalField(H245_H2250LogicalChannelAckParameters::e_mediaChannel);
  mediaAddress.SetPDU(param.m_mediaChannel);
	
  // Set dynamic payload type, if is one
  int rtpPayloadType = GetDynamicRTPPayloadType();
  if (rtpPayloadType >= RTP_DataFrame::DynamicBase && rtpPayloadType < RTP_DataFrame::IllegalPayloadType) {
    param.IncludeOptionalField(H245_H2250LogicalChannelAckParameters::e_dynamicRTPPayloadType);
    param.m_dynamicRTPPayloadType = rtpPayloadType;
  }
}

BOOL H323FileTransferChannel::OnReceivedPDU(const H245_H2250LogicalChannelParameters & param,
						   unsigned & errorCode)
{
  if (param.m_sessionID != sessionID) {
	errorCode = H245_OpenLogicalChannelReject_cause::e_invalidSessionID;
	return FALSE;
  }
	
  BOOL ok = FALSE;
	
  if (param.HasOptionalField(H245_H2250LogicalChannelParameters::e_mediaControlChannel)) {
		
	if (!ExtractTransport(param.m_mediaControlChannel, FALSE, errorCode)) {
	  return FALSE;
	}
	
	ok = TRUE;
  }
	
  if (param.HasOptionalField(H245_H2250LogicalChannelParameters::e_mediaChannel)) {
	if (ok && direction == H323Channel::IsReceiver) {
		
	} else if (!ExtractTransport(param.m_mediaChannel, TRUE, errorCode)) {
      return FALSE;
    }
    
    ok = TRUE;
  }
	
  if (param.HasOptionalField(H245_H2250LogicalChannelParameters::e_dynamicRTPPayloadType)) {
    SetDynamicRTPPayloadType(param.m_dynamicRTPPayloadType);
  }
	
  if (ok) {
    return TRUE;
  }
	
  errorCode = H245_OpenLogicalChannelReject_cause::e_unspecified;
  return FALSE;
}

BOOL H323FileTransferChannel::OnReceivedAckPDU(const H245_H2250LogicalChannelAckParameters & param)
{
  if (!param.HasOptionalField(H245_H2250LogicalChannelAckParameters::e_sessionID)) {
  }
	
  if (!param.HasOptionalField(H245_H2250LogicalChannelAckParameters::e_mediaControlChannel)) {
    return FALSE;
  }
	
  unsigned errorCode;
  if (!ExtractTransport(param.m_mediaControlChannel, FALSE, errorCode)) {
    return FALSE;
  }
	
  if (!param.HasOptionalField(H245_H2250LogicalChannelAckParameters::e_mediaChannel)) {
    return FALSE;
  }
	
  if (!ExtractTransport(param.m_mediaChannel, TRUE, errorCode)) {
    return FALSE;
  }
	
  if (param.HasOptionalField(H245_H2250LogicalChannelAckParameters::e_dynamicRTPPayloadType)) {
    SetDynamicRTPPayloadType(param.m_dynamicRTPPayloadType);
  }
	
  return TRUE;
}

BOOL H323FileTransferChannel::SetDynamicRTPPayloadType(int newType)
{
  if (newType == -1) {
    return TRUE;
  }
	
  if (newType < RTP_DataFrame::DynamicBase || newType >= RTP_DataFrame::IllegalPayloadType) {
    return FALSE;
  }
	
  if (rtpPayloadType < RTP_DataFrame::DynamicBase) {
    return FALSE;
  }
	
  rtpPayloadType = (RTP_DataFrame::PayloadTypes)newType;
	
  return TRUE;
}

BOOL H323FileTransferChannel::ExtractTransport(const H245_TransportAddress & pdu,
										         BOOL isDataPort,
										        unsigned & errorCode)
{
  if (pdu.GetTag() != H245_TransportAddress::e_unicastAddress) {
    errorCode = H245_OpenLogicalChannelReject_cause::e_multicastChannelNotAllowed;
    return FALSE;
  }
	
  H323TransportAddress transAddr = pdu;
	
  PIPSocket::Address ip;
  WORD port;
  if (transAddr.GetIpAndPort(ip, port)) {
    return rtpSession.SetRemoteSocketInfo(ip, port, isDataPort);
  }
	
  return FALSE;
}

BOOL H323FileTransferChannel::RetreiveFileInfo(const H245_GenericInformation & info, H323FileTransferList & filelist)
{
	  if (info.m_messageIdentifier.GetTag() != H245_CapabilityIdentifier::e_standard)
		  return FALSE;

	  const PASN_ObjectId &object_id = info.m_messageIdentifier;
      if (object_id != FileTransferOID)   // Indicates File Information
		   return FALSE;
 
	  if (!info.HasOptionalField(H245_GenericInformation::e_messageContent))
		  return FALSE;
    
	   const H245_ArrayOf_GenericParameter & params = info.m_messageContent;

       int direction=0;
	   long size =0;
	   PString name = PString();
	   for (PINDEX i =0; i < params.GetSize(); i++)
	   {
		   PASN_Integer & pid = params[i].m_parameterIdentifier; 
		   H245_ParameterValue & paramval = params[i].m_parameterValue;
		   int val = pid.GetValue();

		   if (val == 1) {  // tftpDirection
				PASN_Integer & val = paramval;
                direction = val.GetValue();
		   } else if (val == 2) {  // tftpFilename
                PASN_OctetString & val = paramval;
				name = val.AsString();
		   } else if (val == 3) { // tftpFilesize
                PASN_Integer & val = paramval;
				size = val.GetValue();
		   }
	   }
	   filelist.Add(name,"",size);
	   if ((direction > 0) &&  (direction != filelist.GetDirection()))
		   filelist.SetDirection((H323Channel::Directions)direction);

    return TRUE;
}

BOOL H323FileTransferChannel::GetFileList(const H245_OpenLogicalChannel & open) 
{

  if (!open.HasOptionalField(H245_OpenLogicalChannel::e_genericInformation))
	  return FALSE;

  const H245_ArrayOf_GenericInformation & cape = open.m_genericInformation;

  for (PINDEX i=0; i<cape.GetSize(); i++) {
      RetreiveFileInfo(cape[i], filelist);
  }
  
  return TRUE;	
}

static H245_GenericParameter * BuildGenericParameter(unsigned id,unsigned type, const PString & value)
{  

 H245_GenericParameter * content = new H245_GenericParameter();
      H245_ParameterIdentifier & paramid = content->m_parameterIdentifier;
        paramid.SetTag(H245_ParameterIdentifier::e_standard);
        PASN_Integer & pid = paramid;
        pid.SetValue(id);
	  H245_ParameterValue & paramval = content->m_parameterValue;
	    paramval.SetTag(type);
		 if ((type == H245_ParameterValue::e_unsignedMin) ||
			(type == H245_ParameterValue::e_unsignedMax) ||
			(type == H245_ParameterValue::e_unsigned32Min) ||
			(type == H245_ParameterValue::e_unsigned32Max)) {
				PASN_Integer & val = paramval;
				val.SetValue(value.AsUnsigned());
		 } else if (type == H245_ParameterValue::e_octetString) {
				PASN_OctetString & val = paramval;
				val.SetValue(value);
		 }		
//			H245_ParameterValue::e_logical,
//			H245_ParameterValue::e_booleanArray,
//			H245_ParameterValue::e_genericParameter

     return content;
}

void H323FileTransferChannel::SetFileList(H245_OpenLogicalChannel & open, H323FileTransferList flist) const
{
  if (flist.GetSize() == 0)
	  return;

  PINDEX i = 0;

  open.IncludeOptionalField(H245_OpenLogicalChannel::e_genericInformation);
  H245_ArrayOf_GenericInformation & cape = open.m_genericInformation;

  for (H323FileTransferList::const_iterator r = filelist.begin(); r != filelist.end(); ++r) {
	  H245_GenericInformation * gcap = new H245_GenericInformation();
	  gcap->m_messageIdentifier = *(new H245_CapabilityIdentifier(H245_CapabilityIdentifier::e_standard));
	  PASN_ObjectId &object_id = gcap->m_messageIdentifier;
      object_id = FileTransferOID;   // Indicates File Information

  	  i++;
	  gcap->IncludeOptionalField(H245_GenericInformation::e_subMessageIdentifier);
	  PASN_Integer & sub_id = gcap->m_subMessageIdentifier;
      sub_id = i; 

	    gcap->IncludeOptionalField(H245_GenericInformation::e_messageContent);
		H245_ArrayOf_GenericParameter & params = gcap->m_messageContent;
            // tftpDirection
		    params.Append(BuildGenericParameter(1,H245_ParameterValue::e_unsignedMin,flist.GetDirection()));
			// tftpFilename
			params.Append(BuildGenericParameter(2,H245_ParameterValue::e_octetString,r->m_Filename));
			// tftpFilesize
			if (flist.GetDirection() == H323Channel::IsTransmitter)
              params.Append(BuildGenericParameter(3,H245_ParameterValue::e_unsigned32Max,r->m_Filesize));

		params.SetSize(params.GetSize()+1);
     cape.Append(gcap);
  }
  cape.SetSize(cape.GetSize()+1);
 
}

///////////////////////////////////////////////////////////////////////////////////

H323FileTransferList::H323FileTransferList()
{
	saveDirectory = PProcess::Current().GetFile().GetDirectory();
	direction = H323Channel::IsReceiver;
}

void H323FileTransferList::Add(const PString & filename, const PDirectory & directory, long filesize)
{
   H323File file;
   file.m_Filename = filename;
   file.m_Directory = directory;
   file.m_Filesize = filesize;
   push_back(file);
}
	 
void H323FileTransferList::SetSaveDirectory(const PString directory)
{
   saveDirectory = directory;
}

const PDirectory & H323FileTransferList::GetSaveDirectory()
{
   return saveDirectory; 
}

void H323FileTransferList::SetDirection(H323Channel::Directions _direction)
{
   direction = _direction;
}
	
H323Channel::Directions H323FileTransferList::GetDirection()
{
   return direction;
}

PINDEX H323FileTransferList::GetSize()
{
    return size();
}


H323File * H323FileTransferList::GetAt(PINDEX i)
{
	PINDEX c=0;
    for (H323FileTransferList::iterator r = begin(); r != end(); ++r) {
		c++;
		if (c == i)
			return &(*r);
	}
	return NULL;
}

///////////////////////////////////////////////////////////////////////////////////

static PString errString[] = {
	  "Not Defined.",
	  "File Not Found.",
	  "Access Violation.",
	  "Disk Full/Allocation exceeded.",
      "Illegal TFTP operation.",
	  "Unknown transfer ID.",
	  "File Already Exist.s",
	  "No such user."
};

H323FileTransferHandler::H323FileTransferHandler(H323Connection & connection, 
												 unsigned sessionID, 
												 H323Channel::Directions dir,
												 H323FileTransferList & _filelist
												 )
 :filelist(_filelist)
{
  RTP_Session *session;
  H245_TransportAddress addr;
  connection.GetControlChannel().SetUpTransportPDU(addr, H323Transport::UseLocalTSAP);
  session = connection.UseSession(sessionID,addr,H323Channel::IsBidirectional);

  TransmitThread = NULL;
  ReceiveThread = NULL;
  blockRate = 0;
  blockSize = 0;
  msBetweenBlocks = 0;

  // Transmission Settings
  curFile = NULL;
  timestamp = 0;
  lastBlockNo = 0;
  curFileSize = 0;                     
  curProgSize = 0;	
  rtpPayloadType = (RTP_DataFrame::PayloadTypes)101;
  responseTimeOut = 1500;

  currentState = e_probing;
  IsStarter = FALSE;

  shutdown = FALSE;
}

H323FileTransferHandler::~H323FileTransferHandler()
{
  shutdown = TRUE;
}

BOOL H323FileTransferHandler::Start(H323Channel::Directions direction)
{
  IsStarter = (direction == H323Channel::IsTransmitter);

  transmitFrame.SetPayloadType(rtpPayloadType);

  ReceiveThread = PThread::Create(PCREATE_NOTIFIER(Receive), 0, PThread::AutoDeleteThread);
  TransmitThread = PThread::Create(PCREATE_NOTIFIER(Transmit), 0, PThread::AutoDeleteThread);

  StartTime = new PTime();


  return TRUE;
}

BOOL H323FileTransferHandler::Stop(H323Channel::Directions direction)
{
  PWaitAndSignal m(transferMutex);
	
  delete StartTime;
  StartTime = NULL;
  
  // CloseDown the Transmit/Receive Threads
  shutdown = TRUE;
  nextFrame.Signal();

  return TRUE;
}

void H323FileTransferHandler::SetPayloadType(RTP_DataFrame::PayloadTypes _type)
{
	rtpPayloadType = _type;
}

void H323FileTransferHandler::SetBlockSize(H323FileTransferCapability::blockSizes size)
{
    blockSize = size;
}

void H323FileTransferHandler::SetMaxBlockRate(unsigned rate)
{
	blockRate = rate;
	msBetweenBlocks = (int)(1.000/((double)rate))*1000;
}

void H323FileTransferHandler::ChangeState(transferState newState)
{
   currentState = newState;
   OnStateChange(currentState);
}

BOOL H323FileTransferHandler::TransmitFrame(H323FilePacket & buffer, BOOL final)
{	

  // determining correct timestamp
  PTime currentTime = PTime();
  PTimeInterval timePassed = currentTime - *StartTime;
  transmitFrame.SetTimestamp((DWORD)timePassed.GetMilliSeconds() * 8);
  transmitFrame.SetMarker(final);
  
  transmitFrame.SetPayloadSize(buffer.GetSize());
  memmove(transmitFrame.GetPayloadPtr(),buffer.GetPointer(), buffer.GetSize());
	
  return session->WriteData(transmitFrame);
}

BOOL H323FileTransferHandler::ReceiveFrame(H323FilePacket & buffer, BOOL & final)
{	

   RTP_DataFrame packet = RTP_DataFrame(1024);

   if(!session->ReadBufferedData(timestamp, packet)) 
      return FALSE;

    timestamp = packet.GetTimestamp();
   
   final = packet.GetMarker();
   buffer.SetSize(0);
   buffer.Attach(packet.GetPayloadPtr(), packet.GetPayloadSize());
  return TRUE;
}

BOOL Segment(PBYTEArray & lastBlock, const int size, int & segment, PBYTEArray & thearray)
{
	int newsize;
	if (lastBlock.GetSize() < (segment + size)) {
       newsize = lastBlock.GetSize() - segment;
	} else {
	  if (segment == 0)
		newsize = 4 + size;
	  else
		newsize = size;
	}

	BYTE * seg = lastBlock.GetPointer();
    thearray.SetSize(newsize);
    memcpy(thearray.GetPointer(),seg+segment, newsize);
	segment =+ newsize;

	if (segment == lastBlock.GetSize()) {
		segment = 0;
		return TRUE;
	}
	return FALSE;
}

void H323FileTransferHandler::Transmit(PThread &, INT)
{
	BOOL success = TRUE;
	H323File * f = NULL;
	PFilePath p;

	BOOL read = FALSE;
	BOOL waitforResponse = FALSE;
	int fileid = 0;
	PBYTEArray readBlock(blockSize);
	H323FilePacket lastBlock;
	PBYTEArray lastSegment;
	int offset = 0;

	while (success && !shutdown) {
	    
		H323FilePacket packet;
		BOOL final = FALSE;
		switch (currentState) {
		   case e_probing:
			    probMutex.Wait(100);
			    packet.BuildPROB();
				final = TRUE;
			    break;
		   case e_connect:
			    packet.BuildACK(0);
				final = TRUE;
                ChangeState(e_waiting);
			    break;
		   case e_waiting:
			    // if we have something to send
			     if (IsStarter) {
					 if (waitforResponse) {
                        blockState = recTimeOut;
			            nextFrame.Wait(responseTimeOut);
						if (blockState != recTimeOut) {
							waitforResponse = FALSE;  
							if (read) 
								ChangeState(e_receiving);
							else
                                ChangeState(e_sending);
						    break;   // Move to next state!
						}
					 } else {
						fileid++;
						if (fileid > filelist.GetSize()) {
						     OnTransferComplete();
						     ChangeState(e_completed);
						     break;
						}
						delete f;
						f = filelist.GetAt(fileid);
						if (f == NULL) {
							OnFileOpenError("",H323FileIOChannel::e_NotFound);
							ChangeState(e_error);
							break;
						}

						p = f->m_Directory + PDIR_SEPARATOR + f->m_Filename;
					 
						read = (filelist.GetDirection() == H323Channel::IsTransmitter);
						if (read) {
							curFileSize = f->m_Filesize;
							delete curFile;
							curFile = new H323FileIOChannel(p,read);
							if (curFile->IsError(ioerr)) {
								OnFileOpenError(p,ioerr);
								ChangeState(e_error);
								break;
							}
							OnFileStart(p, curFileSize,read);  // Notify to start send
						} else {
						    OnFileStart(f->m_Filename, f->m_Filesize,read);  // Notify to start receive
						}
					 }   
				     if (!read) {
					   packet.BuildRequest(H323FilePacket::e_RRQ ,f->m_Filename, f->m_Filesize, blockSize);
					   final = TRUE;
					   waitforResponse = TRUE;
				     } else {
					   packet.BuildRequest(H323FilePacket::e_WRQ ,f->m_Filename, f->m_Filesize, blockSize);
					   final = TRUE;
					   waitforResponse = TRUE;
				     } 
			    }
			    break;
		   case e_sending:
			   if (blockState != recPartial) {
                 blockState = recTimeOut;
			     nextFrame.Wait(responseTimeOut);
					if (blockState == recOK) {
						sendwait.Delay(msBetweenBlocks);
						offset = 0;
						lastBlockNo++;
                        if (lastBlockNo > 99) lastBlockNo = 0;
						readBlock.SetSize(blockSize);
						PINDEX readsize = readBlock.GetSize();
						curFile->Read(readBlock.GetPointer(),readsize);
						lastBlock.BuildData(lastBlockNo,readBlock);
					}
			   }
                // Segment and mark as recPartial
			   if (blockSize > H323FileTransferCapability::e_1428)
			      final = Segment(readBlock, blockSize, offset, packet);
			    else {
			      packet.Attach(readBlock.GetPointer(),readBlock.GetSize());
				  final = TRUE;
			    }

				if (final) {
                  blockState = recComplete;
				} else
				  blockState = recPartial;
				break;
		   case e_receiving:
                blockState = recTimeOut;
			    nextFrame.Wait(responseTimeOut);
				if ((blockState == recOK) || (blockState == recComplete)) {
				    packet.BuildACK(lastBlockNo);
					if (blockState != recComplete) {
				      lastBlockNo++;
                      if (lastBlockNo > 99) lastBlockNo = 0;
					} else
                      lastBlockNo = 0;  // Indicating a new file
				} else if  (blockState == recIncomplete) {
                    OnFileError(lastBlockNo, FALSE);
                    packet.BuildError(0,"");   // Indicate to remote to resend the last block
				} else {
					// Do nothing
					continue;
				}

			    break;
		   case e_error:
                packet.BuildError(ioerr,errString[ioerr]);
				final = TRUE;
                ChangeState(e_completed);
			    break;
		   case e_completed:
		   default:
			    shutdown = TRUE;
			    break;
		}
          
		success = TransmitFrame(packet,final);
	}

	delete curFile;

}

void H323FileTransferHandler::Receive(PThread &, INT)
{

	BOOL success = TRUE;
	H323FilePacket packet;
	packet.SetSize(0);
    PFilePath p;

	while (success && !shutdown) {

	   BOOL final = FALSE;
	   H323FilePacket buffer;
       success = ReceiveFrame(buffer, final);

	   if (!success)
		   continue;

	   if (currentState == e_receiving) {
		     packet.Concatenate(buffer);
		      if (!final) 
		        continue;
	   } else {
		   packet = buffer;
	   }

	   int ptype = packet.GetPacketType();

	   if (ptype == H323FilePacket::e_ERROR) {
		   int errCode;
		   PString errString;
		   packet.GetErrorInformation(errCode,errString);
		   if (errCode > 0) {
			   OnError(errString);
			   currentState = e_error;
		   }
	   }

		switch (currentState) {
		   case e_probing:
			  if (ptype == H323FilePacket::e_ACK)
                   ChangeState(e_connect);
			 break;
		   case e_connect:
			   // Do nothing
			   break;
		   case e_waiting:
			   if (ptype == H323FilePacket::e_WRQ) {
				   p = filelist.GetSaveDirectory() + PDIR_SEPARATOR + packet.GetFileName();
				   delete curFile;
				   curFile = new H323FileIOChannel(p,FALSE);
				   if (curFile->IsError(ioerr)) {
						OnFileOpenError(p,ioerr);
						ChangeState(e_error);
						break;
				   }
				   packet.SetSize(0);
				   ChangeState(e_receiving);
				   OnFileStart(p, curFileSize,FALSE);  // Notify to start receive
			   } else if (ptype == H323FilePacket::e_RRQ) {
				   p = filelist.GetSaveDirectory() + PDIR_SEPARATOR + packet.GetFileName();
				   delete curFile;
				   curFile = new H323FileIOChannel(p,TRUE);
				   if (curFile->IsError(ioerr)) {
						OnFileOpenError(p,ioerr);
						ChangeState(e_error);
						break;
				   }
				   ChangeState(e_sending);
				   OnFileStart(p, curFileSize,TRUE);  // Notify to start send
                   
			   } else if (ptype != H323FilePacket::e_ACK) {
                   // Do nothing
				   break;
			   }
			    blockState = recOK;
				nextFrame.Signal();
			   break;
		   case e_receiving:
			   if (ptype == H323FilePacket::e_DATA) {
				   if ((packet.GetDataSize() == blockSize) ||  // We have complete block
					   (curFileSize == (curProgSize + packet.GetDataSize()))) {  // We have the last block
				        curProgSize =+ packet.GetDataSize(); 
						lastBlockNo++;
                        if (lastBlockNo > 99) lastBlockNo = 0;
						// Write away to file.
                        curFile->Write(packet.GetPointer(),packet.GetSize());

						if (curFileSize != curProgSize) {
							blockState = recOK;
							OnFileProgress(lastBlockNo,curProgSize, FALSE);
						} else { 
							blockState = recComplete;
                            OnFileComplete();
							ChangeState(e_waiting);
						}
				   } else {
					    blockState = recIncomplete;
						OnFileError(lastBlockNo, FALSE);
				   }
				 packet.SetSize(0);
				 nextFrame.Signal();
			   }
			  break;
 		   case e_sending:
			   if (ptype == H323FilePacket::e_ACK) {
					curProgSize =+ blockSize;
					OnFileProgress(lastBlockNo,curProgSize, TRUE);
				    blockState = recOK;
                    nextFrame.Signal();
			   } else if (ptype == H323FilePacket::e_ERROR) {
					OnFileError(lastBlockNo, TRUE);
                    blockState = recIncomplete;
					nextFrame.Signal();
			   }
			   break;
		   case e_error:
                // Do nothing
			    break;
		   case e_completed:
		   default:
			    shutdown = TRUE;
			    break;
		}
	}
}

///////////////////////////////////////////////////////////////////////////

static PString opStr[] = {
	  "00",
	  "01",
	  "02",
	  "03",
	  "04",
	  "05"
  };

void H323FilePacket::BuildPROB()
{
  PString header = opStr[e_PROB];
  Attach(header,header.GetSize());
}

void H323FilePacket::BuildRequest(opcodes code, const PString & filename, int filesize, int blocksize)
{
   PString header = opStr[code] + filename + "0octet0blksize0" + PString(blocksize) 
	                    + "0tsize0" + PString(filesize) + "0";
   Attach(header,header.GetSize());

}

void H323FilePacket::BuildData(int blockid,PBYTEArray data)
{
   PString blkstr;
   if (blockid < 10)
	   blkstr = "0" + blockid;
   else
       blkstr = blockid;

   PString header = opStr[e_DATA] + PString(blockid);
   Attach(header,header.GetSize());
   Concatenate(data);
}

void H323FilePacket::BuildACK(int blockid)
{
   PString blkstr;
   if (blockid < 10)
	   blkstr = "0" + blockid;
   else
       blkstr = blockid;

   PString header = opStr[e_ACK] + PString(blockid);
   Attach(header,header.GetSize());
}

void H323FilePacket::BuildError(int errorcode,PString errmsg)
{
   PString blkerr;
   if (errorcode < 10)
	   blkerr = "0" + PString(errorcode);
   else
       blkerr = PString(errorcode);

   PString header = opStr[e_ERROR] + blkerr + errmsg + "0";
   Attach(header,header.GetSize());
}

PString H323FilePacket::GetFileName()
{
  if ((GetPacketType() != e_RRQ) ||
	   (GetPacketType() != e_WRQ))
	      return PString();

  PString data((const char *)GetPointer(), GetSize());

  PStringArray array = (data.Mid(2)).Tokenise('0');

  return array[0];
}

unsigned H323FilePacket::GetFileSize()
{
  if ((GetPacketType() != e_RRQ) ||
	   (GetPacketType() != e_WRQ))
	      return 0;

  PString data((const char *)GetPointer(), GetSize());

  PStringArray array = (data.Mid(2)).Tokenise('0');

  for (PINDEX i=0; i<array.GetSize()-1; i++)
	  if (array[i] == "tsize")
		  return array[i+1].AsInteger();

  return 0;
}

unsigned H323FilePacket::GetBlockSize()
{
  if ((GetPacketType() != e_RRQ) ||
	   (GetPacketType() != e_WRQ))
	      return 0;

  PString data((const char *)GetPointer(), GetSize());

  PStringArray array = (data.Mid(2)).Tokenise('0');

  for (PINDEX i=0; i<array.GetSize()-1; i++)
	  if (array[i] == "blksize")
		  return array[i+1].AsInteger();

  return 0;
}

void H323FilePacket::GetErrorInformation(int & ErrCode, PString & ErrStr)
{
  if (GetPacketType() != e_ERROR) 
	      return;

  PString data((const char *)GetPointer(), GetSize());
  PString array = data.Mid(2);

  ErrCode = (array.Left(2)).AsInteger();
  ErrStr = array.Mid(3,array.GetLength()-3);
}
    
BYTE * H323FilePacket::GetDataPtr() 
{
	return (BYTE *)(theArray+4); 
}

unsigned H323FilePacket::GetDataSize() 
{
  if (GetPacketType() == e_DATA)
	return GetSize() - 4; 
  else
    return 0;
}

int H323FilePacket::GetACKBlockNo()
{
  if (GetPacketType() != e_ACK) 
	      return 0;

  PString data((const char *)GetPointer(), GetSize());
  return data.Mid(2).AsInteger();
}

H323FilePacket::opcodes H323FilePacket::GetPacketType()
{
   PString val = (const char *)(const BYTE *)theArray[1];
   return (opcodes)((short)val.AsInteger());
}

////////////////////////////////////////////////////////////////////

H323FileIOChannel::H323FileIOChannel(PFilePath _file, BOOL read)
{
	if (!CheckFile(_file,read,IOError))
		return;

    PFile::OpenMode mode;
	if (read) 
		mode = PFile::ReadOnly;
	else 
		mode = PFile::WriteOnly;

	PFile * file = new PFile(_file,mode);
	fileopen = file->IsOpen();

	if (!fileopen) {
		IOError = e_AccessDenied;
		delete file;
		file = NULL;
		return;
	}

   if (read) 
	   SetReadChannel(file, TRUE);
   else 
	   SetWriteChannel(file, TRUE);
}
	
H323FileIOChannel::~H323FileIOChannel()
{
}

BOOL H323FileIOChannel::IsError(fileError err)
{
	err = IOError;
	return (err > 0);
}

BOOL H323FileIOChannel::CheckFile(PFilePath _file, BOOL read, fileError & errCode)
{
	BOOL exists = PFile::Exists(_file);
	
	if (read && !exists) {
		errCode = e_NotFound;
		return FALSE;
	}

	if (!read && exists) {
		errCode = e_FileExists;
		return FALSE;
	}

	PFileInfo info;
	PFile::GetInfo(_file,info);

	if (read && (info.permissions < PFileInfo::UserRead)) {
		errCode = e_AccessDenied;
		return FALSE;
	}

	errCode = e_OK;
	return TRUE;
}

BOOL H323FileIOChannel::Open()
{
  PWaitAndSignal mutex(chanMutex);

	if (fileopen)
		return TRUE;

	return TRUE;
}
	
BOOL H323FileIOChannel::Close()
{
  PWaitAndSignal mutex(chanMutex);

  if (!fileopen)
		return TRUE;

  PIndirectChannel::Close();
  return TRUE;
}

BOOL H323FileIOChannel::Read(void * buffer, PINDEX & amount)
{
    PWaitAndSignal mutex(chanMutex);

	if (!fileopen)
		return FALSE;

	BOOL result = PIndirectChannel::Read(buffer, amount);
	amount = GetLastReadCount();

    return result;
}
   
BOOL H323FileIOChannel::Write(const void * buf, PINDEX amount)
{
    PWaitAndSignal mutex(chanMutex);

	if (!fileopen)
		return FALSE;

	return PIndirectChannel::Write(buf, amount);
}

#endif  // H323_FILE



