/*
 * h323filetransfer.h
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
 * $Log: h323filetransfer.h,v $
 * Revision 1.1  2008/01/01 00:16:12  shorne
 * Added GnuGknat and FileTransfer support
 *
 *
 *
 */


#ifdef H323_FILE

#include <ptclib/delaychan.h>
#include <list>

//////////////////////////////////////////////////////////////////////////////////

class H323File
{
  public:
	PString    m_Filename;
	PString    m_Directory;
	long       m_Filesize;
};

class H323FileTransferList : public list<H323File>
{ 
  public:
    H323FileTransferList();
	PINDEX GetSize();
	H323File * GetAt(PINDEX i);
    void Add(const PString & filename, const PDirectory & directory, long filesize);
    void SetSaveDirectory(const PString directory);
	const PDirectory & GetSaveDirectory();
 
	void SetDirection(H323Channel::Directions _direction);
	H323Channel::Directions GetDirection();

  protected:
	H323Channel::Directions direction;
	PDirectory saveDirectory;
};


//////////////////////////////////////////////////////////////////////////////////
/**This class describes the File Transfer logical channel.
 */
class H323FileTransferHandler;
class H323FileTransferChannel : public H323Channel
{
    PCLASSINFO(H323FileTransferChannel, H323Channel);
  public:
  /**@name Construction */
  //@{
    /**Create a new channel.
     */
    H323FileTransferChannel(
      H323Connection & connection,          ///<  Connection to endpoint for channel
      const H323Capability & capability,    ///<  Capability channel is using
      Directions theDirection,              ///<  Direction of channel
	  RTP_UDP & rtp,                        ///<  RTP Session for channel
      unsigned theSessionID                 ///<  Session ID for channel
    );

    ~H323FileTransferChannel();
  //@}

  /**@name Overrides from class H323Channel */
  //@{

   /**Indicate the direction of the channel.
       Return if the channel is bidirectional, or unidirectional, and which
       direction for the latter case.
     */
    virtual H323Channel::Directions GetDirection() const;

    /**Set the initial bandwidth for the channel.
       This calculates the initial bandwidth required by the channel and
       returns TRUE if the connection can support this bandwidth.

       The default behaviour does nothing.
     */
    virtual BOOL SetInitialBandwidth();

    /**Fill out the OpenLogicalChannel PDU for the particular channel type.
     */
    virtual BOOL OnSendingPDU(
      H245_OpenLogicalChannel & openPDU  ///<  Open PDU to send. 
    ) const;

    virtual BOOL OnSendingPDU(H245_H2250LogicalChannelParameters & param) const;

    virtual void OnSendOpenAck(const H245_OpenLogicalChannel & openPDU, 
							 H245_OpenLogicalChannelAck & ack) const;

    virtual void OnSendOpenAck(H245_H2250LogicalChannelAckParameters & param) const;


    /**This is called after a request to create a channel occurs from the
       local machine via the H245LogicalChannelDict::Open() function, and
       the request has been acknowledged by the remote endpoint.

       The default makes sure the parameters are compatible and passes on
       the PDU to the rtp session.
     */
    virtual BOOL OnReceivedPDU(
      const H245_OpenLogicalChannel & pdu,    ///<  Open PDU
      unsigned & errorCode                    ///<  Error code on failure
    );

    virtual BOOL OnReceivedPDU(const H245_H2250LogicalChannelParameters & param,
							 unsigned & errorCode);

    virtual BOOL OnReceivedAckPDU(const H245_OpenLogicalChannelAck & pdu);

    virtual BOOL OnReceivedAckPDU(const H245_H2250LogicalChannelAckParameters & param);


    /**Handle channel data reception.

       This is called by the thread started by the Start() function and is
       a loop reading from the transport and calling HandlePacket() for each
       PDU read.
      */
    virtual void Receive();

    /**Handle channel data transmission.

       This is called by the thread started by the Start() function and is
       typically a loop reading from the codec and writing to the transport
       (eg an RTP_session).
      */
    virtual void Transmit();


    virtual BOOL Open();

    virtual BOOL Start();

    virtual void Close();

    virtual BOOL SetDynamicRTPPayloadType(int newType);
    RTP_DataFrame::PayloadTypes GetDynamicRTPPayloadType() const { return rtpPayloadType; }
	
    H323FileTransferHandler * GetHandler() const { return fileHandler; }

  //@}

  protected:
    BOOL ExtractTransport(const H245_TransportAddress & pdu,
			     BOOL isDataPort,
			     unsigned & errorCode
						 );

    BOOL RetreiveFileInfo(const H245_GenericInformation & info, 
			     H323FileTransferList & filelist
						 );

    void SetFileList(H245_OpenLogicalChannel & open, H323FileTransferList flist) const;
    BOOL GetFileList(const H245_OpenLogicalChannel & open);

    unsigned sessionID;
    Directions direction;
    RTP_UDP & rtpSession;
    H323_RTP_Session & rtpCallbacks;
    H323FileTransferHandler *fileHandler;
	H323FileTransferList filelist;
    RTP_DataFrame::PayloadTypes rtpPayloadType;

};

////////////////////////////////////////////////////////////////////

class H323FileTransferCapability : public H323DataCapability
{
  PCLASSINFO(H323FileTransferCapability, H323DataCapability);

  public:
  /**@name Construction */
  //@{
    /**Create a new Extended Video capability.
      */
    H323FileTransferCapability(unsigned maxBitRate = 132000);
  //@}

  /**@name Overrides from class PObject */
  //@{
    /**Compare two capability instances. This compares the main and sub-types
       of the capability.
     */
    Comparison Compare(const PObject & obj) const;
  //@}

  /**@name Identification functions */
  //@{

    virtual PString GetFormatName() const
    { return "TFTP Data"; }

    /**Get the sub-type of the capability. This is a code dependent on the
       main type of the capability.

       This returns one of the four possible combinations of mode and speed
       using the enum values of the protocol ASN H245_AudioCapability class.
     */
	virtual unsigned GetSubType() const;
  //@}
    
  /**@name Protocol manipulation */
  //@{
    /**This function is called whenever and outgoing TerminalCapabilitySet
       or OpenLogicalChannel PDU is being constructed for the control channel.
       It allows the capability to set the PDU fields from information in
       members specific to the class.

       The default behaviour sets the pdu and calls OnSendingPDU with a
       H245_DataProtocolCapability parameter.
     */
    virtual BOOL OnSendingPDU(
      H245_DataApplicationCapability & pdu
    ) const;

    /**This function is called whenever and incoming TerminalCapabilitySet
       or OpenLogicalChannel PDU has been used to construct the control
       channel. It allows the capability to set from the PDU fields,
       information in members specific to the class.

       The default behaviour does nothing.
     */

    virtual BOOL OnReceivedPDU(
      const H245_DataApplicationCapability & cap  ///< PDU to get information from
    );

    /**This function is called whenever and incoming TerminalCapabilitySet
       or OpenLogicalChannel PDU has been used to construct the control
       channel. It allows the capability to set from the PDU fields,
       information in members specific to the class.

       The default behaviour does nothing.
     */
    BOOL OnReceivedPDU(const H245_GenericCapability & pdu);

    /**This function is called whenever and outgoing TerminalCapabilitySet
       PDU is being constructed for the control channel. It allows the
       capability to set the PDU fields from information in members specific
       to the class.

       The default behaviour calls the OnSendingPDU() function with a more
       specific PDU type.
     */
    BOOL OnSendingPDU(H245_GenericCapability & pdu) const;

	//@}

	enum blockSizes {
		e_RFC1350   = 512,
		e_1024      = 1024,
		e_1428      = 1428,
		e_2048      = 2048,
		e_4096      = 4096,
		e_8192      = 8192
	};

  /**@name Operations */
  //@{
    /**Create the channel instance, allocating resources as required.
     */
    virtual H323Channel * CreateChannel(
      H323Connection & connection,          ///< Owner connection for channel
      H323Channel::Directions direction,    ///< Direction of channel
      unsigned sessionID,                   ///< Session ID for RTP channel
      const H245_H2250LogicalChannelParameters * param
                                            ///< Parameters for channel
    ) const;
  //@}

  /**@name Utilities */
  //@{
    /**Get the BlockSize of data to be sent.
     */
	unsigned GetBlockSize() const  { return m_blockSize; }

    /**Get the BlockSize of data to be sent (as Capability Parameter).
     */
	unsigned GetBlockRate() const  { return (maxBitRate/8) / m_blockOctets; }

    /**Get the BlockSize of data to be sent (as Octets size).
     */
	unsigned GetOctetSize() const  { return m_blockOctets; }

    /**Get the Transfer Mode.
     */
	unsigned GetTransferMode() const  { return m_transferMode; }
  //@}

  protected:
    unsigned m_blockSize;          ///< Size indicator in capability negotiation
	unsigned m_blockOctets;        ///< Block Octet size
	unsigned m_transferMode;       ///< Mode of transfer Raw Tftp or RTP encaptulated

};

class H323FilePacket : public PBYTEArray
{
  PCLASSINFO(H323FilePacket, PBYTEArray);

public:
  enum opcodes {
	  e_PROB,
	  e_RRQ,
	  e_WRQ,
	  e_DATA,
	  e_ACK,
	  e_ERROR
  };

  enum errCodes {
          ErrNotDefined,
	  ErrFileNotFound,
	  ErrAccessViolation,
	  ErrDiskFull,
	  ErrIllegalOperation,
	  ErrUnknownID,
	  ErrFileExists,
	  ErrNoSuchUser
  }; 

  void BuildPROB();
  void BuildRequest(opcodes code, const PString & filename, int filesize, int blocksize);
  void BuildData(int blockid,PBYTEArray data);
  void BuildACK(int blockid);
  void BuildError(int errorcode,PString errmsg);

  H323FilePacket::opcodes GetPacketType();

  // for RRQ/WRQ Only
  PString GetFileName();
  unsigned GetFileSize();
  unsigned GetBlockSize();
  int GetBlockNo();

  // for DATA only
  unsigned GetDataSize();
  BYTE * GetDataPtr();

  // For ACK
  int GetACKBlockNo();

  // for ERROR messages
  void GetErrorInformation(int & ErrCode, PString & ErrStr);

};

class H323FileIOChannel : public PIndirectChannel
{
  public:

	  enum fileError {
		  e_OK,
		  e_NotFound,
		  e_AccessDenied,
		  e_FileExists = 6
	  };

    H323FileIOChannel(PFilePath _file, BOOL read);
	~H323FileIOChannel();

	BOOL IsError(fileError err);

	BOOL Open();
	BOOL Close();

    BOOL Read(void * buffer, PINDEX & amount);
    BOOL Write(const void * buf, PINDEX amount);

  protected:
	BOOL CheckFile(PFilePath _file, BOOL read, fileError & errCode);

    PMutex chanMutex;
	BOOL fileopen;
	fileError IOError;
};


//////////////////////////////////////////////////////////////////////////////
// FileTransferHandler
// Derive you class from this to set and collect events associated with
// the file transfer
//
class H323FileTransferHandler : public PObject
{
  PCLASSINFO(H323FileTransferHandler, PObject);
	
public:
	
  H323FileTransferHandler(H323Connection & connection, 
	                    unsigned sessionID,
			    H323Channel::Directions dir,
			    H323FileTransferList & filelist
			  );

  ~H323FileTransferHandler();

  enum transferState {
	  e_probing,         ///< Probing for connectivity
	  e_connect,         ///< Received a probe packet and confirm
	  e_waiting,         ///< Waiting for RRQ/WRQ command
	  e_sending,         ///< Sending Data
	  e_receiving,       ///< Receiving Data
	  e_completed,       ///< Transfer Completed
	  e_error            ///< Error Received
  };

  enum receiveStates {
	  recOK,             ///< received block OK
	  recPartial,        ///< Only partial block received
	  recComplete,       ///< File was fully received
	  recIncomplete,     ///< Incomplete receival of File.
	  recTimeOut,        ///< TimeOut waiting for packet.
  };

//////////////////////////
// User override to custom set blocksize and transmission rate

  virtual void SetBlockSize(H323FileTransferCapability::blockSizes size);
  virtual void SetMaxBlockRate(unsigned rate);

// User override to get events

  virtual void OnStateChange(transferState newState) {};
  virtual void OnFileStart(const PString & filename, unsigned filesize, BOOL transmit) {};
  virtual void OnFileOpenError(const PString & filename,H323FileIOChannel::fileError _err) {};
  virtual void OnError(const PString ErrMsg) {};
  virtual void OnFileComplete() {};
  virtual void OnFileProgress(int Blockno,unsigned OctetsSent, BOOL transmit) {};
  virtual void OnFileError(int Blockno, BOOL transmit) {};
  virtual void OnTransferComplete() {};

//////////////////////////


  H323FileTransferCapability::blockSizes GetBlockSize()  
        { return (H323FileTransferCapability::blockSizes)blockSize; }
  unsigned GetBlockRate()  
        { return blockRate; }

  BOOL Start(H323Channel::Directions direction);
  BOOL Stop(H323Channel::Directions direction);

  void SetPayloadType(RTP_DataFrame::PayloadTypes _type);

protected:

  BOOL TransmitFrame(H323FilePacket & buffer, BOOL final);
  BOOL ReceiveFrame(H323FilePacket & buffer, BOOL & final);

  void ChangeState(transferState newState);

  H323FileTransferList filelist;

  PThread * TransmitThread;
  PThread * ReceiveThread;

  PDECLARE_NOTIFIER(PThread, H323FileTransferHandler, Transmit);
  PDECLARE_NOTIFIER(PThread, H323FileTransferHandler, Receive);

  RTP_DataFrame transmitFrame;   ///< Template RTP Frame for sending/receiving

  RTP_Session * session;         ///< RTP Session

  PTimedMutex probMutex;    ///< probing Mutex
  PMutex transferMutex;     ///< Mutex for read/write operation
  PSyncPoint nextFrame;     ///< Wait for confirmation before sending the next block
  PINDEX responseTimeOut;   ///< Time to wait for a response (set at 1.5 sec)

  PTime *StartTime;
  BOOL IsStarter;           ///< Was Initiator of the channel
  BOOL shutdown;

private:
  RTP_DataFrame::PayloadTypes rtpPayloadType;  ///< Payload Type should be dynamically allocated
  int msBetweenBlocks;                         ///< milliseconds between sending next block of data
  PAdaptiveDelay sendwait;                     ///< Wait before sending the next block of data
  unsigned timestamp;                          ///< TimeStamp of last packet received
  unsigned blockSize;                          ///< Size of the Blocks to transmit
  unsigned blockRate;                          ///< Rate of Transmission in bits/sec
  
  // Transfer states
  H323FileIOChannel * curFile;                 ///< Current File being sent
  H323FileIOChannel::fileError ioerr;          ///< Last IO error
  transferState currentState;                  ///< Current state of Transmission
  receiveStates blockState;                    ///< State of Received block          
  int lastBlockNo;                             ///< Last Block No sent
  unsigned curFileSize;                        ///< Current File being Transmitted size
  unsigned curProgSize;						   ///< Current amount of data sent/received
};

#endif
