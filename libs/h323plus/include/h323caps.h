/*
 * h323caps.h
 *
 * H.323 protocol handler
 *
 * Open H323 Library
 *
 * Copyright (c) 1998-2000 Equivalence Pty. Ltd.
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
 * Portions of this code were written with the assisance of funding from
 * Vovida Networks, Inc. http://www.vovida.com.
 *
 * Contributor(s): ______________________________________.
 *
 * $Log: h323caps.h,v $
 * Revision 1.8  2007/11/14 08:55:22  shorne
 * Added ability to set DSCP values for audio/video
 *
 * Revision 1.7  2007/11/06 17:43:36  shorne
 * added i480 standard framesize
 *
 * Revision 1.6  2007/11/01 20:17:30  shorne
 * updates for H.239 support
 *
 * Revision 1.5  2007/10/30 04:23:45  shorne
 * Corrections and Improvements for H.239 support
 *
 * Revision 1.4  2007/10/25 21:08:03  shorne
 * Added support for HD Video devices
 *
 * Revision 1.3  2007/10/16 17:00:28  shorne
 * Added H.230 Support
 *
 * Revision 1.2  2007/08/20 19:13:27  shorne
 * Added Generic Capability support. Fixed Linux compile errors
 *
 * Revision 1.1  2007/08/06 20:50:49  shorne
 * First commit of h323plus
 *
 * Revision 1.53.2.5  2007/03/24 23:39:42  shorne
 * More H.239 work
 *
 * Revision 1.53.2.4  2007/02/19 20:11:05  shorne
 * Added Baseline H.239 Support
 *
 * Revision 1.53.2.3  2007/02/18 17:11:22  shorne
 * Added H.249 Extended UserInput Support
 *
 * Revision 1.53.2.2  2007/02/02 22:12:43  shorne
 * Added ability to set FrameSize for video plugins
 *
 * Revision 1.53.2.1  2006/12/23 19:08:02  shorne
 * Plugin video codecs & sundry
 *
 * Revision 1.53  2006/09/05 23:56:57  csoutheren
 * Convert media format and capability factories to use std::string
 *
 * Revision 1.52  2006/05/16 11:36:01  shorne
 * added AEC support and  H323VdieoCapability default constructor
 *
 * Revision 1.51  2005/11/30 13:05:01  csoutheren
 * Changed tags for Doxygen
 *
 * Revision 1.50  2005/06/07 07:09:13  csoutheren
 * Removed compiler warnings on Windows
 *
 * Revision 1.49  2005/06/07 03:22:22  csoutheren
 * Added patch 1198741 with support for plugin codecs with generic capabilities
 * Added patch 1198754 with support for setting quality level on audio codecs
 * Added patch 1198760 with GSM-AMR codec support
 * Many thanks to Richard van der Hoff for his work
 *
 * Revision 1.48  2005/01/03 06:25:52  csoutheren
 * Added extensive support for disabling code modules at compile time
 *
 * Revision 1.47  2004/08/24 14:23:09  csoutheren
 * Fixed problem with plugin codecs using capability compare functions
 *
 * Revision 1.46  2004/06/30 12:31:09  rjongbloed
 * Rewrite of plug in system to use single global variable for all factories to avoid all sorts
 *   of issues with startup orders and Windows DLL multiple instances.
 *
 * Revision 1.45  2004/06/08 01:20:28  csoutheren
 * Provide backwards compatibility for applications using old capability API
 *
 * Revision 1.44  2004/06/01 05:47:58  csoutheren
 * Changed capability table to use abstract factory routines rather than internal linked list
 *
 * Revision 1.43  2004/05/13 12:49:32  rjongbloed
 * Fixed usage of the compare function in non-standard capabilities.
 *
 * Revision 1.42  2004/04/03 10:38:24  csoutheren
 * Added in initial cut at codec plugin code. Branches are for wimps :)
 *
 * Revision 1.41.2.1  2004/03/31 11:11:58  csoutheren
 * Initial public release of plugin codec code
 *
 * Revision 1.41  2004/03/02 11:48:14  rjongbloed
 * Fixed correct capability table matching when FindCapability with a mode change,
 *   uses different enum so needs translation table.
 *
 * Revision 1.40  2003/10/27 06:03:39  csoutheren
 * Added support for QoS
 *   Thanks to Henry Harrison of AliceStreet
 *
 * Revision 1.39  2003/06/06 02:13:10  rjongbloed
 * Changed non-standard capability semantics so can use C style strings as
 *   the embedded data block (ie automatically call strlen)
 *
 * Revision 1.38  2003/04/28 07:00:00  robertj
 * Fixed problem with compiler(s) not correctly initialising static globals
 *
 * Revision 1.37  2003/04/27 23:49:21  craigs
 * Fixed some comments and made list of registered codecs
 * available outside h323caps.cxx
 *
 * Revision 1.36  2002/11/09 04:24:01  robertj
 * Fixed minor documentation errors.
 *
 * Revision 1.35  2002/09/16 01:14:15  robertj
 * Added #define so can select if #pragma interface/implementation is used on
 *   platform basis (eg MacOS) rather than compiler, thanks Robert Monaghan.
 *
 * Revision 1.34  2002/09/03 06:19:36  robertj
 * Normalised the multi-include header prevention ifdef/define symbol.
 *
 * Revision 1.33  2002/08/05 10:03:47  robertj
 * Cosmetic changes to normalise the usage of pragma interface/implementation.
 *
 * Revision 1.32  2002/05/29 03:55:17  robertj
 * Added protocol version number checking infrastructure, primarily to improve
 *   interoperability with stacks that are unforgiving of new features.
 *
 * Revision 1.31  2002/05/10 05:44:50  robertj
 * Added the max bit rate field to the data channel capability class.
 *
 * Revision 1.30  2002/01/22 06:25:02  robertj
 * Moved payload type to ancestor so any capability can adjust it on logical channel.
 *
 * Revision 1.29  2002/01/17 07:04:57  robertj
 * Added support for RFC2833 embedded DTMF in the RTP stream.
 *
 * Revision 1.28  2002/01/16 05:37:41  robertj
 * Added missing mode change functions on non standard capabilities.
 *
 * Revision 1.27  2002/01/09 00:21:36  robertj
 * Changes to support outgoing H.245 RequstModeChange.
 *
 * Revision 1.26  2001/12/22 01:44:05  robertj
 * Added more support for H.245 RequestMode operation.
 *
 * Revision 1.25  2001/10/24 01:20:34  robertj
 * Added code to help with static linking of H323Capability names database.
 *
 * Revision 1.24  2001/09/21 02:48:51  robertj
 * Added default implementation for PDU encode/decode for codecs
 *   that have simple integer as frames per packet.
 *
 * Revision 1.23  2001/09/11 10:21:40  robertj
 * Added direction field to capabilities, thanks Nick Hoath.
 *
 * Revision 1.22  2001/07/19 09:50:40  robertj
 * Added code for default session ID on data channel being three.
 *
 * Revision 1.21  2001/07/17 04:44:29  robertj
 * Partial implementation of T.120 and T.38 logical channels.
 *
 * Revision 1.20  2001/05/21 07:20:47  robertj
 * Removed redundent class name in declaration.
 *
 * Revision 1.19  2001/05/14 05:56:26  robertj
 * Added H323 capability registration system so can add capabilities by
 *   string name instead of having to instantiate explicit classes.
 *
 * Revision 1.18  2001/05/02 16:22:21  rogerh
 * Add IsAllow() for a single capability to check if it is in the
 * capabilities set. This fixes the bug where OpenH323 would accept
 * incoming H261 video even when told not to accept it.
 *
 * Revision 1.17  2001/03/16 23:00:21  robertj
 * Improved validation of codec selection against capability set, thanks Chris Purvis.
 *
 * Revision 1.16  2001/02/09 05:16:24  robertj
 * Added #pragma interface for GNU C++.
 *
 * Revision 1.15  2001/01/25 07:27:14  robertj
 * Major changes to add more flexible OpalMediaFormat class to normalise
 *   all information about media types, especially codecs.
 *
 * Revision 1.14  2001/01/09 23:05:22  robertj
 * Fixed inability to have 2 non standard codecs in capability table.
 *
 * Revision 1.13  2000/10/16 08:49:30  robertj
 * Added single function to add all UserInput capability types.
 *
 * Revision 1.12  2000/08/23 14:23:11  craigs
 * Added prototype support for Microsoft GSM codec
 *
 * Revision 1.11  2000/07/13 12:25:47  robertj
 * Fixed problems with fast start frames per packet adjustment.
 *
 * Revision 1.10  2000/07/10 16:01:50  robertj
 * Started fixing capability set merging, still more to do.
 *
 * Revision 1.9  2000/07/04 01:16:49  robertj
 * Added check for capability allowed in "combinations" set, still needs more done yet.
 *
 * Revision 1.8  2000/06/03 03:16:47  robertj
 * Fixed using the wrong capability table (should be connections) for some operations.
 *
 * Revision 1.7  2000/05/23 11:32:27  robertj
 * Rewrite of capability table to combine 2 structures into one and move functionality into that class
 *    allowing some normalisation of usage across several applications.
 * Changed H323Connection so gets a copy of capabilities instead of using endponts, allows adjustments
 *    to be done depending on the remote client application.
 *
 * Revision 1.6  2000/05/18 11:53:34  robertj
 * Changes to support doc++ documentation generation.
 *
 * Revision 1.5  2000/05/10 04:05:26  robertj
 * Changed capabilities so has a function to get name of codec, instead of relying on PrintOn.
 *
 * Revision 1.4  2000/05/02 04:32:24  robertj
 * Fixed copyright notice comment.
 *
 * Revision 1.3  2000/04/05 19:01:12  robertj
 * Added function so can change desired transmit packet size.
 *
 * Revision 1.2  2000/03/21 03:06:47  robertj
 * Changes to make RTP TX of exact numbers of frames in some codecs.
 *
 * Revision 1.1  1999/12/23 23:02:35  robertj
 * File reorganision for separating RTP from H.323 and creation of LID for VPB support.
 *
 */

#ifndef __OPAL_H323CAPS_H
#define __OPAL_H323CAPS_H

#ifdef P_USE_PRAGMA
#pragma interface
#endif


#include "codecs.h"
#include "channels.h"
#include "mediafmt.h"


/* The following classes have forward references to avoid including the VERY
   large header files for H225 and H245. If an application requires access
   to the protocol classes they can include them, but for simple usage their
   inclusion can be avoided.
 */
class PASN_Choice;
class H245_Capability;
class H245_DataType;
class H245_ModeElement;
class H245_AudioCapability;
class H245_AudioMode;
class H245_VideoCapability;
class H245_VideoMode;
class H245_DataApplicationCapability;
class H245_DataMode;
class H245_DataProtocolCapability;
class H245_GenericCapability;
class H245_CapabilityIdentifier;
class H245_GenericParameter;
class H245_H2250LogicalChannelParameters;
class H245_H223LogicalChannelParameters;
class H245_TerminalCapabilitySet;
class H245_NonStandardParameter;
class H323Connection;
class H323Capabilities;



///////////////////////////////////////////////////////////////////////////////

/**This class describes the interface to a capability of the endpoint, usually
   a codec, used to transfer data via the logical channels opened and managed
   by the H323 control channel.

   Note that this is not an instance of the codec itself. Merely the
   description of that codec. There is typically only one instance of this
   class contained in the capability tables of the endpoint. There may be
   several instances of the actualy codec managing the conversion of an
   individual stream of data.

   An application may create a descendent off this class and override
   functions as required for describing a codec that it implements.
 */
class H323Capability : public PObject
{
  PCLASSINFO(H323Capability, PObject);

  public:
  /**@name Construction */
  //@{
    /**Create a new capability specification.
     */
    H323Capability();
  //@}

  /**@name Overrides from class PObject */
  //@{
    /**Compare two capability instances. This compares the main and sub-types
       of the capability.
     */
    Comparison Compare(const PObject & obj) const;

    /**Print out the object to the stream, virtual version of << operator.
     */
    void PrintOn(ostream & strm) const;
  //@}

  /**@name Identification functions */
  //@{
    enum MainTypes {
      /// Audio codec capability
      e_Audio,
      /// Video codec capability
      e_Video,
      /// Arbitrary data capability
      e_Data,
      /// User Input capability
      e_UserInput,
      /// Video Extention
	  e_ExtendVideo,
	  /// Generic Control
	  e_GenericControl,
      /// Conference Control
	  e_ConferenceControl,
      /// Count of main types
      e_NumMainTypes
    };

    /**Get the main type of the capability.

       This function is overridden by one of the three main sub-classes off
       which real capabilities would be descendend.
     */
    virtual MainTypes GetMainType() const = 0;

    /**Get the sub-type of the capability. This is a code dependent on the
       main type of the capability.
     */
    virtual unsigned  GetSubType()  const = 0;

    /**Get the name of the media data format this class represents.
     */
    virtual PString GetFormatName() const = 0;
  //@}

  /**@name Operations */
  //@{
    /**Create an H323Capability descendant given a string name.
       This uses the registration system to create the capability.
      */
    static H323Capability * Create(
      const PString & name    ///< Name of capability
    );

    /**
      * provided for backwards compatibility
      */
    static H323Capability * Create(
      H323EndPoint &,         ///< endpoint (not used)
      const PString & name    ///< Name of capability
    )
    { return Create(name); }

    /**Get the default RTP session.
       This function gets the default RTP session ID for the capability
       type. For example audio capabilities return the value
       RTP_Session::DefaultAudioSessionID etc.

       The default behaviour returns zero, indicating it is not an RTP
       based capability.
      */
    virtual unsigned GetDefaultSessionID() const;

    /**Set the maximum size (in frames) of data that will be transmitted in a
       single PDU.

       This will also be the desired number that will be sent by most codec
       implemetations.

       The default behaviour does nothing.
     */
    virtual void SetTxFramesInPacket(
      unsigned frames   ///< Number of frames per packet
    );

    /**Get the maximum size (in frames) of data that will be transmitted in a single PDU.

       The default behaviour returns the value 1.
     */
    virtual unsigned GetTxFramesInPacket() const;

    /**Get the maximum size (in frames) of data that can be received in a single PDU.

       The default behaviour returns the value 1.
     */
    virtual unsigned GetRxFramesInPacket() const;

 	 enum CapabilityFrameSize {
		 sqcifMPI,
         qcifMPI,
		 cifMPI,
		 cif4MPI,
		 cif16MPI,
		 i480MPI,
		 p720MPI,
		 i1080MPI
	 };

    /**Set the Maximum Frame Size of the Video Capability.
	   This is used for set Capabilities to only utilise the specified Max FrameSizes.
       The default behaviour returns False.
     */
	 virtual BOOL SetMaxFrameSize(
		   CapabilityFrameSize /*framesize*/, 
		   int /*frameUnits*/) 
	 { return FALSE; };

    /**Create the channel instance, allocating resources as required.
       This creates a logical channel object appropriate for the parameters
       provided. Not if param is NULL, sessionID must be provided, otherwise
       this is taken from the fields in param.
     */
    virtual H323Channel * CreateChannel(
      H323Connection & connection,    ///< Owner connection for channel
      H323Channel::Directions dir,    ///< Direction of channel
      unsigned sessionID,             ///< Session ID for RTP channel
      const H245_H2250LogicalChannelParameters * param
                                      ///< Parameters for channel
    ) const = 0;

    /**Create the codec instance, allocating resources as required.
     */
    virtual H323Codec * CreateCodec(
      H323Codec::Direction direction  ///< Direction in which this instance runs
    ) const = 0;
  //@}

  /**@name Protocol manipulation */
  //@{
    enum CommandType {
      e_TCS,
      e_OLC,
      e_ReqMode
    };

    /**This function is called whenever and outgoing TerminalCapabilitySet
       PDU is being constructed for the control channel. It allows the
       capability to set the PDU fields from information in members specific
       to the class.

       The default behaviour is pure.
     */
    virtual BOOL OnSendingPDU(
      H245_Capability & pdu  ///< PDU to set information on
    ) const = 0;

    /**This function is called whenever and outgoing OpenLogicalChannel
       PDU is being constructed for the control channel. It allows the
       capability to set the PDU fields from information in members specific
       to the class.

       The default behaviour is pure.
     */
    virtual BOOL OnSendingPDU(
      H245_DataType & pdu  ///< PDU to set information on
    ) const = 0;

    /**This function is called whenever and outgoing RequestMode
       PDU is being constructed for the control channel. It allows the
       capability to set the PDU fields from information in members specific
       to the class.

       The default behaviour is pure.
     */
    virtual BOOL OnSendingPDU(
      H245_ModeElement & pdu  ///< PDU to set information on
    ) const = 0;

    /**This function is called whenever and incoming TerminalCapabilitySet
       PDU is received on the control channel, and a new H323Capability
       descendent was created. This completes reading fields from the PDU
       into the classes members.

       If the function returns FALSE then the received PDU codec description
       is not supported, so will be ignored.
       
         The default behaviour sets the capabilityDirection member variable
         from the PDU and then returns TRUE. Note that this means it is very
         important to call the ancestor function when overriding.
     */
    virtual BOOL OnReceivedPDU(
      const H245_Capability & pdu ///< PDU to get information from
    );

    /**This function is called whenever and incoming OpenLogicalChannel
       PDU has been used to construct the control channel. It allows the
       capability to set from the PDU fields, information in members specific
       to the class.

       The default behaviour is pure.
     */
    virtual BOOL OnReceivedPDU(
      const H245_DataType & pdu,  ///< PDU to get information from
      BOOL receiver               ///< Is receiver OLC
    ) = 0;

    /**Compare the PDU part of the capability.
      */
    virtual BOOL IsMatch(
      const PASN_Choice & subTypePDU  ///<  sub-type PDU of H323Capability
    ) const;

    /**Validate that the capability is usable given the connection.
       This checks agains the negotiated protocol version number and remote
       application to determine if this capability should be used in TCS or
       OLC pdus.

       The default behaviour returns TRUE.
      */
    virtual BOOL IsUsable(
      const H323Connection & connection
    ) const;
  //@}

  /**@name Member variable access */
  //@{
    enum CapabilityDirection {
      e_Unknown,
      e_Receive,
      e_Transmit,
      e_ReceiveAndTransmit,
      e_NoDirection,
      NumCapabilityDirections
    };

    /**Get the direction for this capability.
      */ 
    CapabilityDirection GetCapabilityDirection() const { return capabilityDirection; }

    /**Set the direction for this capability.
      */
    void SetCapabilityDirection(
      CapabilityDirection dir   ///< New direction code
    ) { capabilityDirection = dir; }

    void SetMediaFormatOptionInteger(const PString & Name, int Val);
    /// Get unique capability number.
    virtual unsigned GetCapabilityNumber() const { return assignedCapabilityNumber; }

    /// Set unique capability number.
    virtual void SetCapabilityNumber(unsigned num) { assignedCapabilityNumber = num; }

    /**Get media format of the media data this class represents.
      */
    const OpalMediaFormat & GetMediaFormat() const;
    OpalMediaFormat & GetWritableMediaFormat();

    /// Get the payload type for the capaibility
    RTP_DataFrame::PayloadTypes GetPayloadType() const { return rtpPayloadType; }

    /// Attach a QoS specification to this channel
    virtual void AttachQoS(RTP_QOS *)
    {}
  //@}

#if PTRACING
    friend ostream & operator<<(ostream & o , MainTypes t);
    friend ostream & operator<<(ostream & o , CapabilityDirection d);
#endif

  protected:
    unsigned assignedCapabilityNumber;  /// Unique ID assigned to capability
    CapabilityDirection capabilityDirection;
    RTP_DataFrame::PayloadTypes rtpPayloadType;
    
  private:
    OpalMediaFormat mediaFormat;
};



/**This class describes the interface to a non-standard codec used to
   transfer data via the logical channels opened and managed by the H323
   control channel.

   It is expected that an application makes a descendent off
   H323NonStandardAudioCapability or H323NonStandardVideoCapability which
   multiply inherit from this class.
 */

class H323NonStandardCapabilityInfo
{
  public:
    typedef PObject::Comparison (*CompareFuncType)(struct PluginCodec_H323NonStandardCodecData *);

    /**Create a new set of information about a non-standard codec.
      */
    H323NonStandardCapabilityInfo(
      CompareFuncType compareFunc,
      const BYTE * dataBlock,         ///< Non-Standard data for codec type
      PINDEX dataSize                 ///< Size of dataBlock. If 0 and dataBlock != NULL use strlen(dataBlock)
    );

    H323NonStandardCapabilityInfo(
      const BYTE * dataBlock,         ///< Non-Standard data for codec type
      PINDEX dataSize,                ///< Size of dataBlock. If 0 and dataBlock != NULL use strlen(dataBlock)
      PINDEX comparisonOffset = 0,    ///< Offset into dataBlock to compare
      PINDEX comparisonLength = P_MAX_INDEX    ///< Length of bytes to compare
    );

    /**Create a new set of information about a non-standard codec.
      */
    H323NonStandardCapabilityInfo(
      const PString & oid,
      const BYTE * dataBlock,         ///< Non-Standard data for codec type
      PINDEX dataSize,                ///< Size of dataBlock. If 0 and dataBlock != NULL use strlen(dataBlock)
      PINDEX comparisonOffset = 0,
      PINDEX comparisonLength = P_MAX_INDEX
    );

    /**Create a new set of information about a non-standard codec.
      */
    H323NonStandardCapabilityInfo(
      BYTE country,                  ///< t35 information
      BYTE extension,                ///< t35 information
      WORD maufacturer,              ///< t35 information
      const BYTE * dataBlock,        ///< Non-Standard data for codec type
      PINDEX dataSize,               ///< Size of dataBlock. If 0 and dataBlock != NULL use strlen(dataBlock)
      PINDEX comparisonOffset = 0,   ///< Offset into dataBlock to compare
      PINDEX comparisonLength = P_MAX_INDEX  ///< Length of bytes to compare
    );

    /**Destroy the capability information
     */
    virtual ~H323NonStandardCapabilityInfo();

    /**This function gets the non-standard data field.

       The default behaviour sets data to fixedData.
      */
    virtual BOOL OnSendingPDU(
      PBYTEArray & data  ///< Data field in PDU to send
    ) const;

    /**This function validates and uses the non-standard data field.

       The default behaviour returns TRUE if data is equal to fixedData.
      */
    virtual BOOL OnReceivedPDU(
      const PBYTEArray & data  ///< Data field in PDU received
    );

    BOOL IsMatch(const H245_NonStandardParameter & param) const;

  protected:
    BOOL OnSendingNonStandardPDU(
      PASN_Choice & pdu,
      unsigned nonStandardTag
    ) const;
    BOOL OnReceivedNonStandardPDU(
      const PASN_Choice & pdu,
      unsigned nonStandardTag
    );

    PObject::Comparison CompareParam(
      const H245_NonStandardParameter & param
    ) const;
    PObject::Comparison CompareInfo(
      const H323NonStandardCapabilityInfo & obj
    ) const;
    PObject::Comparison CompareData(
      const PBYTEArray & data  ///< Data field in PDU received
    ) const;

    PString    oid;
    BYTE       t35CountryCode;
    BYTE       t35Extension;
    WORD       manufacturerCode;
    PBYTEArray nonStandardData;
    PINDEX     comparisonOffset;
    PINDEX     comparisonLength;
    CompareFuncType compareFunc;
};


/**This class describes the interface to a generic codec used to
   transfer data via the logical channels opened and managed by the H323
   control channel.

   It is expected that an application makes a descendent off
   H323GenericAudioCapability or H323GenericVideoCapability which
   multiply inherit from this class.
 */

class H323GenericCapabilityInfo
{
  public:
    H323GenericCapabilityInfo(
        const PString &capabilityId,	///< generic codec identifier
        PINDEX maxBitRate = 0	      ///< maxBitRate parameter for the GenericCapability
    );
    H323GenericCapabilityInfo(const H323GenericCapabilityInfo &obj);
    virtual ~H323GenericCapabilityInfo();

    BOOL IsMatch(
      const H245_GenericCapability & param  ///< Non standard field in PDU received
    ) const;
    
    void SetMaxBitRate(unsigned bitrate)
    {
     maxBitRate = bitrate;
    }
    
  protected:
    virtual BOOL OnSendingGenericPDU(
      H245_GenericCapability & pdu,
      const OpalMediaFormat & mediaFormat,
      H323Capability::CommandType type
    ) const;
    virtual BOOL OnReceivedGenericPDU(
      OpalMediaFormat & mediaFormat,
      const H245_GenericCapability & pdu,
      H323Capability::CommandType type
    );

    PObject::Comparison CompareInfo(
	const H323GenericCapabilityInfo & obj
	) const;


    H245_CapabilityIdentifier * identifier;
    unsigned                    maxBitRate;
};

    
/**This class describes the interface to a codec that has channels based on
   the RTP protocol.

   An application may create a descendent off this class and override
   functions as required for descibing the codec.
 */
class PAec;
class H323RealTimeCapability : public H323Capability
{
  PCLASSINFO(H323RealTimeCapability, H323Capability);

  public:
  /**@name Operations */
  //@{
    /**Create the channel instance, allocating resources as required.
     */
    virtual H323Channel * CreateChannel(
      H323Connection & connection,    ///< Owner connection for channel
      H323Channel::Directions dir,    ///< Direction of channel
      unsigned sessionID,             ///< Session ID for RTP channel
      const H245_H2250LogicalChannelParameters * param
                                      ///< Parameters for channel
    ) const;

    H323RealTimeCapability();
    H323RealTimeCapability(const H323RealTimeCapability &rtc);
    virtual ~H323RealTimeCapability();
    virtual void AttachQoS(RTP_QOS * _rtpqos);

  protected:
    RTP_QOS * rtpqos;


  //@}
};

#ifndef NO_H323_AUDIO_CODECS

/**This class describes the interface to an audio codec used to transfer data
   via the logical channels opened and managed by the H323 control channel.

   An application may create a descendent off this class and override
   functions as required for descibing the codec.
 */
class H323AudioCapability : public H323RealTimeCapability
{
  PCLASSINFO(H323AudioCapability, H323RealTimeCapability);

  public:
  /**@name Construction */
  //@{
    /**Create an audio based capability.
      */
    H323AudioCapability(
      unsigned rxPacketSize, ///< Maximum size of an audio packet in frames
      unsigned txPacketSize  ///< Desired transmit size of an audio packet frames
    );
  //@}

  /**@name Identification functions */
  //@{
    /**Get the main type of the capability.
       Always returns e_Audio.
     */
    virtual MainTypes GetMainType() const;
  //@}

  /**@name Operations */
  //@{
    /**Get the default RTP session.
       This function gets the default RTP session ID for the capability
       type. For example audio capabilities return the value
       RTP_Session::DefaultAudioSessionID etc.

       The default behaviour returns zero, indicating it is not an RTP
       based capability.
      */
    virtual unsigned GetDefaultSessionID() const;

    /**Set the maximum size (in frames) of data that will be transmitted in a
       single PDU.

       This will also be the desired number that will be sent by most codec
       implemetations.

       The default behaviour sets the txFramesInPacket variable.
     */
    virtual void SetTxFramesInPacket(
      unsigned frames   ///< Number of frames per packet
    );

    /**Get the maximum size (in frames) of data that will be transmitted in a
       single PDU.

       The default behaviour sends the txFramesInPacket variable.
     */
    virtual unsigned GetTxFramesInPacket() const;

    /**Get the maximum size (in frames) of data that can be received in a
       single PDU.

       The default behaviour sends the rxFramesInPacket variable.
     */
    virtual unsigned GetRxFramesInPacket() const;

	/** Set Audio DiffServ Value
	    Use This to override the default DSCP value
	  */
	static void SetDSCPvalue(int newValue);

	/** Get the current Audio DSCP value
	  */
	static int GetDSCPvalue(); 

  /**@name Protocol manipulation */
  //@{
    /**This function is called whenever and outgoing TerminalCapabilitySet
       PDU is being constructed for the control channel. It allows the
       capability to set the PDU fields from information in members specific
       to the class.

       The default behaviour calls the OnSendingPDU() function with a more
       specific PDU type.
     */
    virtual BOOL OnSendingPDU(
      H245_Capability & pdu  ///< PDU to set information on
    ) const;

    /**This function is called whenever and outgoing OpenLogicalChannel
       PDU is being constructed for the control channel. It allows the
       capability to set the PDU fields from information in members specific
       to the class.

       The default behaviour calls the OnSendingPDU() function with a more
       specific PDU type.
     */
    virtual BOOL OnSendingPDU(
      H245_DataType & pdu  ///< PDU to set information on
    ) const;

    /**This function is called whenever and outgoing RequestMode
       PDU is being constructed for the control channel. It allows the
       capability to set the PDU fields from information in members specific
       to the class.

       The default behaviour calls the OnSendingPDU() function with a more
       specific PDU type.
     */
    virtual BOOL OnSendingPDU(
      H245_ModeElement & pdu  ///< PDU to set information on
    ) const;

    /**This function is called whenever and outgoing TerminalCapabilitySet
       or OpenLogicalChannel PDU is being constructed for the control channel.
       It allows the capability to set the PDU fields from information in
       members specific to the class.

       The default behaviour assumes the pdu is an integer number of frames
       per packet.
     */
    virtual BOOL OnSendingPDU(
      H245_AudioCapability & pdu,  ///< PDU to set information on
      unsigned packetSize          ///< Packet size to use in capability
    ) const;
    virtual BOOL OnSendingPDU(
      H245_AudioCapability & pdu,  ///<  PDU to set information on
      unsigned packetSize,         ///<  Packet size to use in capability
      CommandType type             ///<  Type of PDU to send in
    ) const;

    /**This function is called whenever and outgoing RequestMode
       PDU is being constructed for the control channel. It allows the
       capability to set the PDU fields from information in members specific
       to the class.

       The default behaviour sets the PDUs tag according to the GetSubType()
       function (translated to different enum).
     */
    virtual BOOL OnSendingPDU(
      H245_AudioMode & pdu  ///< PDU to set information on
    ) const;

    /**This function is called whenever and incoming TerminalCapabilitySet
       PDU is received on the control channel, and a new H323Capability
       descendent was created. This completes reading fields from the PDU
       into the classes members.

       If the function returns FALSE then the received PDU codec description
       is not supported, so will be ignored.
       
       The default behaviour calls the OnReceivedPDU() that takes a
       H245_AudioCapability and clamps the txFramesInPacket.
     */
    virtual BOOL OnReceivedPDU(
      const H245_Capability & pdu  ///< PDU to get information from
    );

    /**This function is called whenever and incoming OpenLogicalChannel
       PDU has been used to construct the control channel. It allows the
       capability to set from the PDU fields, information in members specific
       to the class.
       
       The default behaviour calls the OnReceivedPDU() that takes a
       H245_AudioCapability and clamps the txFramesInPacket or
       rxFramesInPacket.
     */
    virtual BOOL OnReceivedPDU(
      const H245_DataType & pdu,  ///< PDU to get information from
      BOOL receiver               ///< Is receiver OLC
    );

    /**This function is called whenever and incoming TerminalCapabilitySet
       or OpenLogicalChannel PDU has been used to construct the control
       channel. It allows the capability to set from the PDU fields,
       information in members specific to the class.

       The default behaviour assumes the pdu is an integer number of frames
       per packet.
     */
    virtual BOOL OnReceivedPDU(
      const H245_AudioCapability & pdu,  ///< PDU to get information from
      unsigned & packetSize              ///< Packet size to use in capability
    );
    virtual BOOL OnReceivedPDU(
      const H245_AudioCapability & pdu,  ///< PDU to get information from
      unsigned & packetSize,             ///< Packet size to use in capability
      CommandType type                   ///<  Type of PDU to send in
    );
  //@}

  protected:
    unsigned rxFramesInPacket;
    unsigned txFramesInPacket;
	static int DSCPvalue;          ///< DiffServ Value
};


/**This class describes the interface to a non-standard audio codec used to
   transfer data via the logical channels opened and managed by the H323
   control channel.

   An application may create a descendent off this class and override
   functions as required for descibing the codec.
 */
class H323NonStandardAudioCapability : public H323AudioCapability,
                                       public H323NonStandardCapabilityInfo
{
  PCLASSINFO(H323NonStandardAudioCapability, H323AudioCapability);

  public:
  /**@name Construction */
  //@{
    /**Create a new set of information about a non-standard codec.
      */
    H323NonStandardAudioCapability(
      unsigned maxPacketSize,         ///< Maximum size of an audio packet in frames
      unsigned desiredPacketSize,     ///< Desired transmit size of an audio packet in frames
      H323NonStandardCapabilityInfo::CompareFuncType compareFunc,
      const BYTE * dataBlock,         ///< Non-Standard data for codec type
      PINDEX dataSize                 ///< Size of dataBlock. If 0 and dataBlock != NULL use strlen(dataBlock)
    );
    H323NonStandardAudioCapability(
      unsigned maxPacketSize,         ///< Maximum size of an audio packet in frames
      unsigned desiredPacketSize,     ///< Desired transmit size of an audio packet in frames
      const BYTE * dataBlock,         ///< Non-Standard data for codec type
      PINDEX dataSize,                ///< Size of dataBlock. If 0 and dataBlock != NULL use strlen(dataBlock)
      PINDEX comparisonOffset = 0,    ///< Offset into dataBlock to compare
      PINDEX comparisonLength = P_MAX_INDEX     ///< Length of bytes to compare
     );
    /**
      * provided for backwards compatibility
      */
    H323NonStandardAudioCapability(
      unsigned maxPacketSize,         ///< Maximum size of an audio packet in frames
      unsigned desiredPacketSize,     ///< Desired transmit size of an audio packet in frames
      H323EndPoint &,
      H323NonStandardCapabilityInfo::CompareFuncType compareFunc,
      const BYTE * dataBlock,         ///< Non-Standard data for codec type
      PINDEX dataSize                 ///< Size of dataBlock. If 0 and dataBlock != NULL use strlen(dataBlock)
    );
    H323NonStandardAudioCapability(
      unsigned maxPacketSize,         ///< Maximum size of an audio packet in frames
      unsigned desiredPacketSize,     ///< Desired transmit size of an audio packet in frames
      H323EndPoint &,
      const BYTE * dataBlock,         ///< Non-Standard data for codec type
      PINDEX dataSize,                ///< Size of dataBlock. If 0 and dataBlock != NULL use strlen(dataBlock)
      PINDEX comparisonOffset = 0,    ///< Offset into dataBlock to compare
      PINDEX comparisonLength = P_MAX_INDEX         ///< Length of bytes to compare
     );

    /**Create a new set of information about a non-standard codec.
      */
    H323NonStandardAudioCapability(
      unsigned maxPacketSize,         ///< Maximum size of an audio packet in frames
      unsigned desiredPacketSize,     ///< Desired transmit size of an audio packet in frames
      const PString & oid,            ///< OID for indentification of codec
      const BYTE * dataBlock,         ///< Non-Standard data for codec type
      PINDEX dataSize,                ///< Size of dataBlock. If 0 and dataBlock != NULL use strlen(dataBlock)
      PINDEX comparisonOffset = 0,    ///< Offset into dataBlock to compare
      PINDEX comparisonLength = P_MAX_INDEX  ///< Length of bytes to compare
    );

    /**Create a new set of information about a non-standard codec.
      */
    H323NonStandardAudioCapability(
      unsigned maxPacketSize,         ///< Maximum size of an audio packet in frames
      unsigned desiredPacketSize,     ///< Desired transmit size of an audio packet in frames
      BYTE country,                   ///< t35 information
      BYTE extension,                 ///< t35 information
      WORD maufacturer,               ///< t35 information
      const BYTE * dataBlock,         ///< Non-Standard data for codec type
      PINDEX dataSize,                ///< Size of dataBlock. If 0 and dataBlock != NULL use strlen(dataBlock)
      PINDEX comparisonOffset = 0,        ///< Offset into dataBlock to compare
      PINDEX comparisonLength = P_MAX_INDEX        ///< Length of bytes to compare
    );
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
    /**Get the sub-type of the capability. This is a code dependent on the
       main type of the capability.

       This returns H245_AudioCapability::e_nonStandard.
     */
    virtual unsigned GetSubType() const;
  //@}

  /**@name Protocol manipulation */
  //@{
    /**This function is called whenever and outgoing TerminalCapabilitySet
       or OpenLogicalChannel PDU is being constructed for the control channel.
       It allows the capability to set the PDU fields from information in
       members specific to the class.

       The default behaviour calls H323NonStandardCapabilityinfo::OnSendingPDU()
       to handle the PDU.
     */
    virtual BOOL OnSendingPDU(
      H245_AudioCapability & pdu,  ///< PDU to set information on
      unsigned packetSize          ///< Packet size to use in capability
    ) const;

    /**This function is called whenever and outgoing RequestMode
       PDU is being constructed for the control channel. It allows the
       capability to set the PDU fields from information in members specific
       to the class.

       The default behaviour calls H323NonStandardCapabilityinfo::OnSendingPDU()
       to handle the PDU.
     */
    virtual BOOL OnSendingPDU(
      H245_AudioMode & pdu  ///< PDU to set information on
    ) const;

    /**This function is called whenever and incoming TerminalCapabilitySet
       or OpenLogicalChannel PDU has been used to construct the control
       channel. It allows the capability to set from the PDU fields,
       information in members specific to the class.

       The default behaviour calls H323NonStandardCapabilityinfo::OnReceivedPDU()
       to handle the provided PDU.
     */
    virtual BOOL OnReceivedPDU(
      const H245_AudioCapability & pdu,  ///< PDU to get information from
      unsigned & packetSize              ///< Packet size to use in capability
    );

    /**Compare the nonStandardData part of the capability, if applicable.
      */
    virtual BOOL IsMatch(
      const PASN_Choice & subTypePDU  ///<  sub-type PDU of H323Capability
    ) const;
  //@}
};

/**This class describes the interface to a generic audio codec used to
   transfer data via the logical channels opened and managed by the H323
   control channel.

   An application may create a descendent off this class and override
   functions as required for descibing the codec.
 */
class H323GenericAudioCapability : public H323AudioCapability,
				   public H323GenericCapabilityInfo
{
  PCLASSINFO(H323NonStandardAudioCapability, H323AudioCapability);

  public:
  /**@name Construction */
  //@{
    /**Create a new set of information about a non-standard codec.
      */
    H323GenericAudioCapability(
      unsigned maxPacketSize,         ///< Maximum size of an audio packet in frames
      unsigned desiredPacketSize,     ///< Desired transmit size of an audio packet in frames
      const PString &capabilityId,    ///< generic codec identifier
      PINDEX maxBitRate = 0	      ///< maxBitRate parameter for the GenericCapability
      );

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
    /**Get the sub-type of the capability. This is a code dependent on the
       main type of the capability.

       This returns H245_AudioCapability::e_genericCapability.
     */
    virtual unsigned GetSubType() const;
  //@}

  /**@name Protocol manipulation */
  //@{
    /**This function is called whenever and outgoing TerminalCapabilitySet
       or OpenLogicalChannel PDU is being constructed for the control channel.
       It allows the capability to set the PDU fields from information in
       members specific to the class.

       The default behaviour calls H323GenericCapabilityinfo::OnSendingPDU()
       to handle the PDU.
     */
    virtual BOOL OnSendingPDU(
      H245_AudioCapability & pdu,  ///< PDU to set information on
      unsigned packetSize,         ///<  Packet size to use in capability
      CommandType type             ///<  Type of PDU to send in
    ) const;

    /**This function is called whenever and outgoing RequestMode
       PDU is being constructed for the control channel. It allows the
       capability to set the PDU fields from information in members specific
       to the class.

       The default behaviour sets the PDUs tag according to the GetSubType()
       function (translated to different enum).
     */
    virtual BOOL OnSendingPDU(
      H245_AudioMode & pdu  ///<  PDU to set information on
    ) const;

    /**This function is called whenever and incoming TerminalCapabilitySet
       or OpenLogicalChannel PDU has been used to construct the control
       channel. It allows the capability to set from the PDU fields,
       information in members specific to the class.

       The default behaviour calls H323GenericCapabilityinfo::OnReceivedPDU()
       to handle the provided PDU.
     */
    virtual BOOL OnReceivedPDU(
      const H245_AudioCapability & pdu,  ///< PDU to get information from
      unsigned & packetSize,             ///< Packet size to use in capability
      CommandType type                   ///<  Type of PDU to send in
    );

    /**Compare the generic part of the capability, if applicable.
     */
    virtual BOOL IsMatch(
      const PASN_Choice & subTypePDU  ///<  sub-type PDU of H323Capability
    ) const;
  //@}
};

#endif

#ifndef NO_H323_VIDEO

/**This class describes the interface to a video codec used to transfer data
   via the logical channels opened and managed by the H323 control channel.

   An application may create a descendent off this class and override
   functions as required for descibing a codec.
 */
class H323VideoCapability : public H323RealTimeCapability
{
  PCLASSINFO(H323VideoCapability, H323RealTimeCapability);

  public:
  /**@name Construction */
  //@{
    /**Create an Video based capability.
      */
    H323VideoCapability();
  //@}
  /**@name Identification functions */
  //@{
    /**Get the main type of the capability.
       Always returns e_Video.
     */
    virtual MainTypes GetMainType() const;
  //@}

  /**@name Operations */
  //@{
    /**Get the default RTP session.
       This function gets the default RTP session ID for the capability
       type. For example audio capabilities return the value
       RTP_Session::DefaultAudioSessionID etc.

       The default behaviour returns zero, indicating it is not an RTP
       based capability.
      */
    virtual unsigned GetDefaultSessionID() const;

	/** Set Video DiffServ Value
	    Use This to override the default DSCP value
	  */
	static void SetDSCPvalue(int newValue);

	/** Get the current Video DSCP value
	  */
	static int GetDSCPvalue(); 
  //@}

  /**@name Protocol manipulation */
  //@{
    /**This function is called whenever and outgoing TerminalCapabilitySet
       PDU is being constructed for the control channel. It allows the
       capability to set the PDU fields from information in members specific
       to the class.

       The default behaviour calls the OnSendingPDU() function with a more
       specific PDU type.
     */
    virtual BOOL OnSendingPDU(
      H245_Capability & pdu  ///< PDU to set information on
    ) const;

    /**This function is called whenever and outgoing OpenLogicalChannel
       PDU is being constructed for the control channel. It allows the
       capability to set the PDU fields from information in members specific
       to the class.

       The default behaviour calls the OnSendingPDU() function with a more
       specific PDU type.
     */
    virtual BOOL OnSendingPDU(
      H245_DataType & pdu  ///< PDU to set information on
    ) const;

    /**This function is called whenever and outgoing RequestMode
       PDU is being constructed for the control channel. It allows the
       capability to set the PDU fields from information in members specific
       to the class.

       The default behaviour calls the OnSendingPDU() function with a more
       specific PDU type.
     */
    virtual BOOL OnSendingPDU(
      H245_ModeElement & pdu  ///< PDU to set information on
    ) const;

    /**This function is called whenever and outgoing TerminalCapabilitySet
       or OpenLogicalChannel PDU is being constructed for the control channel.
       It allows the capability to set the PDU fields from information in
       members specific to the class.

       The default behaviour is pure.
     */
    virtual BOOL OnSendingPDU(
      H245_VideoCapability & pdu  ///< PDU to set information on
    ) const;
    virtual BOOL OnSendingPDU(
      H245_VideoCapability & pdu,  ///<  PDU to set information on
      CommandType type             ///<  Type of PDU to send in
    ) const;

    /**This function is called whenever and outgoing RequestMode
       PDU is being constructed for the control channel. It allows the
       capability to set the PDU fields from information in members specific
       to the class.

       The default behaviour sets the PDUs tag according to the GetSubType()
       function (translated to different enum).
     */
    virtual BOOL OnSendingPDU(
      H245_VideoMode & pdu  ///< PDU to set information on
    ) const = 0;

    /**This function is called whenever and incoming TerminalCapabilitySet
       PDU is received on the control channel, and a new H323Capability
       descendent was created. This completes reading fields from the PDU
       into the classes members.

       If the function returns FALSE then the received PDU codec description
       is not supported, so will be ignored. The default behaviour simply
       returns TRUE.
     */
    virtual BOOL OnReceivedPDU(
      const H245_Capability & pdu  ///< PDU to get information from
    );

    /**This function is called whenever and incoming OpenLogicalChannel
       PDU has been used to construct the control channel. It allows the
       capability to set from the PDU fields, information in members specific
       to the class.

       The default behaviour is pure.
     */
    virtual BOOL OnReceivedPDU(
      const H245_DataType & pdu,  ///< PDU to get information from
      BOOL receiver               ///< Is receiver OLC
    );

    /**This function is called whenever and incoming TerminalCapabilitySet
       or OpenLogicalChannel PDU has been used to construct the control
       channel. It allows the capability to set from the PDU fields,
       information in members specific to the class.

       The default behaviour is pure.
     */
    virtual BOOL OnReceivedPDU(
      const H245_VideoCapability & pdu  ///< PDU to set information on
    );
    virtual BOOL OnReceivedPDU(
      const H245_VideoCapability & pdu,  ///< PDU to get information from
      CommandType type                   ///<  Type of PDU to send in
    );
  //@}

  protected:
	  static int DSCPvalue;             ///< Video DSCP Value
};


/**This class describes the interface to a non-standard video codec used to
   transfer data via the logical channels opened and managed by the H323
   control channel.

   An application may create a descendent off this class and override
   functions as required for descibing the codec.
 */
class H323NonStandardVideoCapability : public H323VideoCapability,
                                       public H323NonStandardCapabilityInfo
{
  PCLASSINFO(H323NonStandardVideoCapability, H323VideoCapability);

  public:
  /**@name Construction */
  //@{
    /**Create a new set of information about a non-standard codec.
      */
    H323NonStandardVideoCapability(
      const BYTE * dataBlock,         ///< Non-Standard data for codec type
      PINDEX dataSize,                ///< Size of dataBlock. If 0 and dataBlock != NULL use strlen(dataBlock)
      PINDEX comparisonOffset = 0,    ///< Offset into dataBlock to compare
      PINDEX comparisonLength = P_MAX_INDEX  ///< Length of bytes to compare
    );

    /**
      * provided for backwards compatibility
      */
    H323NonStandardVideoCapability(
      H323EndPoint &,
      const BYTE * dataBlock,         ///< Non-Standard data for codec type
      PINDEX dataSize,                ///< Size of dataBlock. If 0 and dataBlock != NULL use strlen(dataBlock)
      PINDEX comparisonOffset = 0,    ///< Offset into dataBlock to compare
      PINDEX comparisonLength = P_MAX_INDEX  ///< Length of bytes to compare
    );

    /**Create a new set of information about a non-standard codec.
      */
    H323NonStandardVideoCapability(
      const PString & oid,            ///< OID for indentification of codec
      const BYTE * dataBlock,         ///< Non-Standard data for codec type
      PINDEX dataSize,                ///< Size of dataBlock. If 0 and dataBlock != NULL use strlen(dataBlock)
      PINDEX comparisonOffset = 0,    ///< Offset into dataBlock to compare
      PINDEX comparisonLength = P_MAX_INDEX  ///< Length of bytes to compare
    );

    /**Create a new set of information about a non-standard codec.
      */
    H323NonStandardVideoCapability(
      BYTE country,                   ///< t35 information
      BYTE extension,                 ///< t35 information
      WORD maufacturer,               ///< t35 information
      const BYTE * dataBlock,         ///< Non-Standard data for codec type
      PINDEX dataSize,                ///< Size of dataBlock. If 0 and dataBlock != NULL use strlen(dataBlock)
      PINDEX comparisonOffset = 0,    ///< Offset into dataBlock to compare
      PINDEX comparisonLength = P_MAX_INDEX  ///< Length of bytes to compare
    );
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

       The default behaviour calls H323NonStandardCapabilityinfo::OnSendingPDU()
       to handle the PDU.
     */
    virtual BOOL OnSendingPDU(
      H245_VideoCapability & pdu  ///< PDU to set information on
    ) const;

    /**This function is called whenever and outgoing RequestMode
       PDU is being constructed for the control channel. It allows the
       capability to set the PDU fields from information in members specific
       to the class.

       The default behaviour calls H323NonStandardCapabilityinfo::OnSendingPDU()
       to handle the PDU.
     */
    virtual BOOL OnSendingPDU(
      H245_VideoMode & pdu  ///< PDU to set information on
    ) const;

    /**This function is called whenever and incoming TerminalCapabilitySet
       or OpenLogicalChannel PDU has been used to construct the control
       channel. It allows the capability to set from the PDU fields,
       information in members specific to the class.

       The default behaviour calls H323NonStandardCapabilityinfo::OnReceivedPDU()
       to handle the provided PDU.
     */
    virtual BOOL OnReceivedPDU(
      const H245_VideoCapability & pdu  ///< PDU to set information on
    );

    /**Compare the nonStandardData part of the capability, if applicable.
      */
    virtual BOOL IsMatch(
      const PASN_Choice & subTypePDU  ///<  sub-type PDU of H323Capability
    ) const;
  //@}
};

/**This class describes the interface to an H245 "generic" video codec used to
   transfer data via the logical channels opened and managed by the H323
   control channel.

   An application may create a descendent off this class and override
   functions as required for descibing the codec.
 */
class H323GenericVideoCapability : public H323VideoCapability,
                                   public H323GenericCapabilityInfo
{
  PCLASSINFO(H323GenericVideoCapability, H323VideoCapability);

  public:
  /**@name Construction */
  //@{
    /**Create a new set of information about a generic codec.
      */
    H323GenericVideoCapability(
        const PString &capabilityId,    ///< codec identifier, from H245
        PINDEX maxBitRate = 0		///< maxBitRate parameter for the GenericCapability
    );
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
    /**Get the sub-type of the capability. This is a code dependent on the
       main type of the capability.

       This returns H245_VideoCapability::e_genericCapability.
     */
    virtual unsigned GetSubType() const;
  //@}

  /**@name Protocol manipulation */
  //@{
    /**This function is called whenever and outgoing TerminalCapabilitySet
       or OpenLogicalChannel PDU is being constructed for the control channel.
       It allows the capability to set the PDU fields from information in
       members specific to the class.

       The default behaviour calls H323GenericCapabilityinfo::OnSendingPDU()
       to handle the PDU.
     */
    virtual BOOL OnSendingPDU(
      H245_VideoCapability & pdu,  ///<  PDU to set information on
      CommandType type             ///<  Type of PDU to send in
    ) const;

    virtual BOOL OnSendingPDU(
      H245_VideoCapability & pdu  ///<  PDU to set information on
    ) const;

    /**This function is called whenever and outgoing RequestMode
       PDU is being constructed for the control channel. It allows the
       capability to set the PDU fields from information in members specific
       to the class.

       The default behaviour sets the PDUs tag according to the GetSubType()
       function (translated to different enum).
     */
    virtual BOOL OnSendingPDU(
      H245_VideoMode & pdu  ///< PDU to set information on
    ) const;

    /**This function is called whenever and incoming TerminalCapabilitySet
       or OpenLogicalChannel PDU has been used to construct the control
       channel. It allows the capability to set from the PDU fields,
       information in members specific to the class.

       The default behaviour calls H323GenericCapabilityinfo::OnReceivedPDU()
       to handle the provided PDU.
     */
    virtual BOOL OnReceivedPDU(
      const H245_VideoCapability & pdu,  ///< PDU to get information from
      CommandType type                   ///<  Type of PDU to send in
    );
  //@}

    /**Compare the generic part of the capability, if applicable.
      */
    virtual BOOL IsMatch(
      const PASN_Choice & subTypePDU  ///<  sub-type PDU of H323Capability
    ) const;
};

#endif


/**This class describes the interface to a data channel used to transfer data
   via the logical channels opened and managed by the H323 control channel.

   An application may create a descendent off this class and override
   functions as required for descibing a codec.
 */
class H323DataCapability : public H323Capability
{
  PCLASSINFO(H323DataCapability, H323Capability);

  public:
  /**@name Construction */
  //@{
    /**Create a new data capability.
      */
    H323DataCapability(
      unsigned maxBitRate = 0  ///< Maximum bit rate for data in 100's b/s
    );
  //@}

  /**@name Identification functions */
  //@{
    /**Get the main type of the capability.
       Always returns e_Data.
     */
    virtual MainTypes GetMainType() const;
  //@}

  /**@name Operations */
  //@{
    /**Get the default RTP session.
       This function gets the default RTP session ID for the capability
       type. For example audio capabilities return the value
       RTP_Session::DefaultAudioSessionID etc.

       The default behaviour returns 3, indicating a data session.
      */
    virtual unsigned GetDefaultSessionID() const;

    /**Create the codec instance, allocating resources as required.
       As a data channel has no codec, this always returns NULL.
     */
    virtual H323Codec * CreateCodec(
      H323Codec::Direction direction  ///< Direction in which this instance runs
    ) const;
  //@}

  /**@name Protocol manipulation */
  //@{
    /**This function is called whenever and outgoing TerminalCapabilitySet
       PDU is being constructed for the control channel. It allows the
       capability to set the PDU fields from information in members specific
       to the class.

       The default behaviour calls the OnSendingPDU() function with a more
       specific PDU type.
     */
    virtual BOOL OnSendingPDU(
      H245_Capability & pdu  ///< PDU to set information on
    ) const;

    /**This function is called whenever and outgoing OpenLogicalChannel
       PDU is being constructed for the control channel. It allows the
       capability to set the PDU fields from information in members specific
       to the class.

       The default behaviour calls the OnSendingPDU() function with a more
       specific PDU type.
     */
    virtual BOOL OnSendingPDU(
      H245_DataType & pdu  ///< PDU to set information on
    ) const;

    /**This function is called whenever and outgoing RequestMode
       PDU is being constructed for the control channel. It allows the
       capability to set the PDU fields from information in members specific
       to the class.

       The default behaviour calls the OnSendingPDU() function with a more
       specific PDU type.
     */
    virtual BOOL OnSendingPDU(
      H245_ModeElement & pdu  ///< PDU to set information on
    ) const;

    /**This function is called whenever and outgoing TerminalCapabilitySet
       or OpenLogicalChannel PDU is being constructed for the control channel.
       It allows the capability to set the PDU fields from information in
       members specific to the class.

       The default behaviour is pure.
     */
    virtual BOOL OnSendingPDU(
      H245_DataApplicationCapability & pdu  ///< PDU to set information on
    ) const;
    virtual BOOL OnSendingPDU(
      H245_DataApplicationCapability & pdu, ///<  PDU to set information on
      CommandType type                      ///<  Type of PDU to send in
    ) const;

    /**This function is called whenever and outgoing RequestMode
       PDU is being constructed for the control channel. It allows the
       capability to set the PDU fields from information in members specific
       to the class.

       The default behaviour sets the PDUs tag according to the GetSubType()
       function (translated to different enum).
     */
    virtual BOOL OnSendingPDU(
      H245_DataMode & pdu  ///< PDU to set information on
    ) const = 0;

    /**This function is called whenever and incoming TerminalCapabilitySet
       PDU is received on the control channel, and a new H323Capability
       descendent was created. This completes reading fields from the PDU
       into the classes members.

       If the function returns FALSE then the received PDU codec description
       is not supported, so will be ignored. The default behaviour simply
       returns TRUE.
     */
    virtual BOOL OnReceivedPDU(
      const H245_Capability & pdu  ///< PDU to get information from
    );

    /**This function is called whenever and incoming OpenLogicalChannel
       PDU has been used to construct the control channel. It allows the
       capability to set from the PDU fields, information in members specific
       to the class.

       The default behaviour is pure.
     */
    virtual BOOL OnReceivedPDU(
      const H245_DataType & pdu,  ///< PDU to get information from
      BOOL receiver               ///< Is receiver OLC
    );

    /**This function is called whenever and incoming TerminalCapabilitySet
       or OpenLogicalChannel PDU has been used to construct the control
       channel. It allows the capability to set from the PDU fields,
       information in members specific to the class.

       The default behaviour is pure.
     */
    virtual BOOL OnReceivedPDU(
      const H245_DataApplicationCapability & pdu  ///< PDU to set information on
    );
    virtual BOOL OnReceivedPDU(
      const H245_DataApplicationCapability & pdu, ///<  PDU to set information on
      CommandType type                            ///<  Type of PDU to send in
    );
  //@}

  protected:
    unsigned maxBitRate;
};


/**This class describes the interface to a non-standard data codec used to
   transfer data via the logical channels opened and managed by the H323
   control channel.

   An application may create a descendent off this class and override
   functions as required for descibing the codec.
 */
class H323NonStandardDataCapability : public H323DataCapability,
                                      public H323NonStandardCapabilityInfo
{
  PCLASSINFO(H323NonStandardDataCapability, H323DataCapability);

  public:
  /**@name Construction */
  //@{
    /**Create a new set of information about a non-standard codec.
      */
    H323NonStandardDataCapability(
      unsigned maxBitRate,            ///< Maximum bit rate for data in 100's b/s
      const BYTE * dataBlock,         ///< Non-Standard data for codec type
      PINDEX dataSize,                ///< Size of dataBlock. If 0 and dataBlock != NULL use strlen(dataBlock)
      PINDEX comparisonOffset = 0,    ///< Offset into dataBlock to compare
      PINDEX comparisonLength = P_MAX_INDEX  ///< Length of bytes to compare
    );

    /**Create a new set of information about a non-standard codec.
      */
    H323NonStandardDataCapability(
      unsigned maxBitRate,            ///< Maximum bit rate for data in 100's b/s
      const PString & oid,            ///< OID for indentification of codec
      const BYTE * dataBlock,         ///< Non-Standard data for codec type
      PINDEX dataSize,                ///< Size of dataBlock. If 0 and dataBlock != NULL use strlen(dataBlock)
      PINDEX comparisonOffset = 0,    ///< Offset into dataBlock to compare
      PINDEX comparisonLength = P_MAX_INDEX  ///< Length of bytes to compare
    );

    /**Create a new set of information about a non-standard codec.
      */
    H323NonStandardDataCapability(
      unsigned maxBitRate,            ///< Maximum bit rate for data in 100's b/s
      BYTE country,                   ///< t35 information
      BYTE extension,                 ///< t35 information
      WORD maufacturer,               ///< t35 information
      const BYTE * dataBlock,         ///< Non-Standard data for codec type
      PINDEX dataSize,                ///< Size of dataBlock. If 0 and dataBlock != NULL use strlen(dataBlock)
      PINDEX comparisonOffset = 0,    ///< Offset into dataBlock to compare
      PINDEX comparisonLength = P_MAX_INDEX  ///< Length of bytes to compare
    );
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

       The default behaviour calls H323NonStandardCapabilityinfo::OnSendingPDU()
       to handle the PDU.
     */
    virtual BOOL OnSendingPDU(
      H245_DataApplicationCapability & pdu  ///< PDU to set information on
    ) const;

    /**This function is called whenever and outgoing RequestMode
       PDU is being constructed for the control channel. It allows the
       capability to set the PDU fields from information in members specific
       to the class.

       The default behaviour calls H323NonStandardCapabilityinfo::OnSendingPDU()
       to handle the PDU.
     */
    virtual BOOL OnSendingPDU(
      H245_DataMode & pdu  ///< PDU to set information on
    ) const;

    /**This function is called whenever and incoming TerminalCapabilitySet
       or OpenLogicalChannel PDU has been used to construct the control
       channel. It allows the capability to set from the PDU fields,
       information in members specific to the class.

       The default behaviour calls H323NonStandardCapabilityinfo::OnReceivedPDU()
       to handle the provided PDU.
     */
    virtual BOOL OnReceivedPDU(
      const H245_DataApplicationCapability & pdu  ///< PDU to set information on
    );

    /**Compare the nonStandardData part of the capability, if applicable.
      */
    virtual BOOL IsMatch(
      const PASN_Choice & subTypePDU  ///<  sub-type PDU of H323Capability
    ) const;
  //@}
};

#ifndef NO_H323_AUDIO_CODECS

///////////////////////////////////////////////////////////////////////////////
// The simplest codec is the G.711 PCM codec.

/**This class describes the G.711 codec capability.
 */
class H323_G711Capability : public H323AudioCapability
{
  PCLASSINFO(H323_G711Capability, H323AudioCapability)

  public:
    /// Specific G.711 encoding algorithm.
    enum Mode {
      /// European standard
      ALaw,
      /// American standard
      muLaw
    };
    /// Specific G.711 encoding bit rates.
    enum Speed {
      /// European standard
      At64k,
      /// American standard
      At56k
    };

  /**@name Construction */
  //@{
    /**Create a new G.711 capability.
     */
    H323_G711Capability(
      Mode mode = muLaw,    ///< Type of encoding.
      Speed speed = At64k   ///< Encoding bit rate.
    );
  //@}

  /**@name Overrides from class PObject */
  //@{
    /**Create a copy of the object.
      */
    virtual PObject * Clone() const;
  //@}

  /**@name Identification functions */
  //@{
    /**Get the sub-type of the capability. This is a code dependent on the
       main type of the capability.

       This returns one of the four possible combinations of mode and speed
       using the enum values of the protocol ASN H245_AudioCapability class.
     */
    virtual unsigned GetSubType() const;

    /**Get the name of the media data format this class represents.
     */
    virtual PString GetFormatName() const;
  //@}

  /**@name Operations */
  //@{
    /**Create the codec instance, allocating resources as required.
     */
    virtual H323Codec * CreateCodec(
      H323Codec::Direction direction  ///< Direction in which this instance runs
    ) const;
  //@}

  protected:
    Mode     mode;
    Speed    speed;
};

#endif // NO_H323_AUDIO_CODECS


///////////////////////////////////////////////////////////////////////////////

/**This class describes the UserInput psuedo-channel.
 */
class H245_GenericInformation;
class H323_UserInputCapability : public H323Capability
{
  PCLASSINFO(H323_UserInputCapability, H323Capability);

  public:
  /**@name Construction */
  //@{
    enum SubTypes {
      BasicString,
      IA5String,
      GeneralString,
      SignalToneH245,
      HookFlashH245,
      SignalToneRFC2833,
	  H249A_Navigation,
	  H249B_Softkey,
	  H249C_PointDevice,
	  H249D_Modal,
      NumSubTypes
    };
    static const char * const SubTypeNames[NumSubTypes];

#ifdef H323_H249
	static const char * const SubTypeOID[4];

	enum NavigateKeyID {
		NavigateRight =1,
		NavigateLeft,
		NavigateUp,
		NavigateDown,
		NavigateActivate
	};
#endif

    /**Create the capability for User Input.
       The subType parameter is a value from the enum
       H245_UserInputCapability::Choices.
      */
    H323_UserInputCapability(
      SubTypes subType
    );
  //@}

  /**@name Overrides from class PObject */
  //@{
    /**Create a copy of the object.
      */
    virtual PObject * Clone() const;
  //@}

  /**@name Identification functions */
  //@{
    /**Get the main type of the capability.

       This function is overridden by one of the three main sub-classes off
       which real capabilities would be descendend.
     */
    virtual MainTypes GetMainType() const;

    /**Get the sub-type of the capability. This is a code dependent on the
       main type of the capability.
     */
    virtual unsigned  GetSubType()  const;

    /**Get the name of the media data format this class represents.
     */
    virtual PString GetFormatName() const;
  //@}

  /**@name Operations */
  //@{
    /**Create the channel instance, allocating resources as required.
       This creates a logical channel object appropriate for the parameters
       provided. Not if param is NULL, sessionID must be provided, otherwise
       this is taken from the fields in param.
     */
    virtual H323Channel * CreateChannel(
      H323Connection & connection,    ///< Owner connection for channel
      H323Channel::Directions dir,    ///< Direction of channel
      unsigned sessionID,             ///< Session ID for RTP channel
      const H245_H2250LogicalChannelParameters * param
                                      ///< Parameters for channel
    ) const;

    /**Create the codec instance, allocating resources as required.
     */
    virtual H323Codec * CreateCodec(
      H323Codec::Direction direction  ///< Direction in which this instance runs
    ) const;

#ifdef H323_H249
	/** Build Generic UserInputIndication
	 */
	static H245_GenericInformation * BuildGenericIndication(
		const char * oid  ///< OID of parameter
	);

	static H245_GenericParameter * BuildGenericParameter(
		unsigned id,              ///< Standard Identifier of the parameter
		unsigned type,            ///< H245_GenericParameter Type
		const PString & value     ///< Value of the parameter
	);
#endif

  //@}

  /**@name Protocol manipulation */
  //@{
    /**This function is called whenever and outgoing TerminalCapabilitySet
       PDU is being constructed for the control channel. It allows the
       capability to set the PDU fields from information in members specific
       to the class.

       The default behaviour is pure.
     */
    virtual BOOL OnSendingPDU(
      H245_Capability & pdu  ///< PDU to set information on
    ) const;

    /**This function is called whenever and outgoing OpenLogicalChannel
       PDU is being constructed for the control channel. It allows the
       capability to set the PDU fields from information in members specific
       to the class.

       The default behaviour is pure.
     */
    virtual BOOL OnSendingPDU(
      H245_DataType & pdu  ///< PDU to set information on
    ) const;

    /**This function is called whenever and outgoing RequestMode
       PDU is being constructed for the control channel. It allows the
       capability to set the PDU fields from information in members specific
       to the class.

       The default behaviour calls the OnSendingPDU() function with a more
       specific PDU type.
     */
    virtual BOOL OnSendingPDU(
      H245_ModeElement & pdu  ///< PDU to set information on
    ) const;

    /**This function is called whenever and incoming TerminalCapabilitySet
       PDU is received on the control channel, and a new H323Capability
       descendent was created. This completes reading fields from the PDU
       into the classes members.

       If the function returns FALSE then the received PDU codec description
       is not supported, so will be ignored. The default behaviour simply
       returns TRUE.
     */
    virtual BOOL OnReceivedPDU(
      const H245_Capability & pdu  ///< PDU to get information from
    );

    /**This function is called whenever and incoming OpenLogicalChannel
       PDU has been used to construct the control channel. It allows the
       capability to set from the PDU fields, information in members specific
       to the class.

       The default behaviour is pure.
     */
    virtual BOOL OnReceivedPDU(
      const H245_DataType & pdu,  ///< PDU to get information from
      BOOL receiver               ///< Is receiver OLC
    );

    /**Validate that the capability is usable given the connection.
       This checks agains the negotiated protocol version number and remote
       application to determine if this capability should be used in TCS or
       OLC pdus.

       The default behaviour will check for early versions and return FALSE
       for RFC2833 mode.
      */
    virtual BOOL IsUsable(
      const H323Connection & connection
    ) const;
  //@}

    static void AddAllCapabilities(
      H323Capabilities & capabilities,        ///< Table to add capabilities to
      PINDEX descriptorNum,   ///< The member of the capabilityDescriptor to add
      PINDEX simultaneous     ///< The member of the SimultaneousCapabilitySet to add
    );

  protected:
    SubTypes subType;
    
#ifdef H323_H249
	PString subTypeOID;
#endif

};



///////////////////////////////////////////////////////////////////////////////

PLIST(H323CapabilitiesList, H323Capability);

PARRAY(H323CapabilitiesListArray, H323CapabilitiesList);

class H323SimultaneousCapabilities : public H323CapabilitiesListArray
{
  PCLASSINFO(H323SimultaneousCapabilities, H323CapabilitiesListArray);
  public:
    BOOL SetSize(PINDEX newSize);
};


PARRAY(H323CapabilitiesSetArray, H323SimultaneousCapabilities);


class H323CapabilitiesSet : public H323CapabilitiesSetArray
{
  PCLASSINFO(H323CapabilitiesSet, H323CapabilitiesSetArray);
  public:
    /// Set the new size of the table, internal use only.
    BOOL SetSize(PINDEX newSize);
};


/**This class contains all of the capabilities and their combinations.
  */
class H323Capabilities : public PObject
{
    PCLASSINFO(H323Capabilities, PObject);
  public:
  /**@name Construction */
  //@{
    /**Construct an empty capability set.
      */
    H323Capabilities();

    /**Construct a capability set from the H.245 PDU provided.
      */
    H323Capabilities(
      const H323Connection & connection,      ///< Connection for capabilities
      const H245_TerminalCapabilitySet & pdu  ///< PDU to convert to a capability set.
    );

    /**Construct a copy of a capability set.
       Note this will completely duplicate the set by making clones of every
       capability in the original set.
      */
    H323Capabilities(
      const H323Capabilities & original ///< Original capabilities to duplicate
    );

    /**Assign a copy of a capability set.
       Note this will completely duplicate the set by making clones of every
       capability in the original set.
      */
    H323Capabilities & operator=(
      const H323Capabilities & original ///< Original capabilities to duplicate
    );
  //@}

  /**@name Overrides from class PObject */
  //@{
    /**Print out the object to the stream, virtual version of << operator.
     */
    void PrintOn(
      ostream & strm    ///< Stream to print out to.
    ) const;
  //@}

  /**@name Operations */
  //@{
    /**Get the number of capabilities in the set.
      */
    PINDEX GetSize() const { return table.GetSize(); }

    /**Get the capability at the specified index.
      */
    H323Capability & operator[](PINDEX i) const { return table[i]; }

    /**Set the capability descriptor lists. This is three tier set of
       codecs. The top most level is a list of particular capabilities. Each
       of these consists of a list of alternatives that can operate
       simultaneously. The lowest level is a list of codecs that cannot
       operate together. See H323 section 6.2.8.1 and H245 section 7.2 for
       details.

       If descriptorNum is P_MAX_INDEX, the the next available index in the
       array of descriptors is used. Similarly if simultaneous is P_MAX_INDEX
       the the next available SimultaneousCapabilitySet is used. The return
       value is the index used for the new entry. Note if both are P_MAX_INDEX
       then the return value is the descriptor index as the simultaneous index
       must be zero.

       Note that the capability specified here is automatically added to the
       capability table using the AddCapability() function. A specific
       instance of a capability is only ever added once, so multiple
       SetCapability() calls with the same H323Capability pointer will only
       add that capability once.
     */
    PINDEX SetCapability(
      PINDEX descriptorNum, ///< The member of the capabilityDescriptor to add
      PINDEX simultaneous,  ///< The member of the SimultaneousCapabilitySet to add
      H323Capability * cap  ///< New capability specification
    );

    /**Add all matching capabilities to descriptor lists.
       All capabilities that match the specified name are added as in the other
       form of the SetCapability() function.
      */
    PINDEX AddAllCapabilities(
      PINDEX descriptorNum, ///< The member of the capabilityDescriptor to add
      PINDEX simultaneous,  ///< The member of the SimultaneousCapabilitySet to add
      const PString & name  ///< New capabilities name, if using "known" one.
    );
    PINDEX AddAllCapabilities(
      H323EndPoint &,
      PINDEX descriptorNum, ///< The member of the capabilityDescriptor to add
      PINDEX simultaneous,  ///< The member of the SimultaneousCapabilitySet to add
      const PString & name  ///< New capabilities name, if using "known" one.
    )
    { return AddAllCapabilities(descriptorNum, simultaneous, name); }

    /**Add a codec to the capabilities table. This will assure that the
       assignedCapabilityNumber field in the capability is unique for all
       capabilities installed on this set.

       If the specific instance of the capability is already in the table, it
       is not added again. Ther can be multiple instances of the same
       capability class however.
     */
    void Add(
      H323Capability * capability   ///< New capability specification
    );

    /**Copy a codec to the capabilities table. This will make a clone of the
       capability and assure that the assignedCapabilityNumber field in the
       capability is unique for all capabilities installed on this set.

       Returns the copy that is put in the table.
     */
    H323Capability * Copy(
      const H323Capability & capability   ///< New capability specification
    );

    /**Remove a capability from the table. Note that the the parameter must be
       the actual instance of the capability in the table. The instance is
       deleted when removed from the table.
      */
    void Remove(
      H323Capability * capability   ///< Existing capability specification
    );

    /**Remove all capabilities matching the string. This uses FindCapability()
       to locate the first capability whose format name does a partial match
       for the argument.
      */
    void Remove(
      const PString & formatName   ///< Format name to search for.
    );

    /**Remove all capabilities matching any of the strings provided. This
       simply calls Remove() for each string in the list.
      */
    void Remove(
      const PStringArray & formatNames  ///< Array of format names to remove
    );

    /**Remove all of the capabilities.
      */
    void RemoveAll();

	/**Manually remove Capability type. This removes the specified Capability type out of the 
	   default capability list.
	  */
	BOOL RemoveCapability(H323Capability::MainTypes capabilityType);

#ifdef H323_VIDEO
	/**Set the Video Frame Size. This is used for capabilities
	   that use 1 definition for all Video Frame Sizes. This will remove all capabilities
	   not matching the specified Frame Size and send a message to the remaining video capabilities 
	   to set the maximum framesize allowed to the specified value
	  */
	BOOL SetVideoFrameSize(H323Capability::CapabilityFrameSize frameSize, 
		                  int frameUnits
	);
#endif

    /**Find the capability given the capability number. This number is
       guarenteed to be unique for a give capability table. Note that is may
       not be the same as the index into the table.

       Returns:
       NULL if no capability meeting the criteria was found
      */
    H323Capability * FindCapability(
      unsigned capabilityNumber
    ) const;

    /**Find the capability given the capability format name string. This does
       a partial match for the supplied argument. If the argument matches a
       substring of the actual capabilities name, then it is returned. For
       example "GSM" or "0610" will match "GSM 0610". Note case is not
       significant.

       The user should be carefull of using short strings such as "G"!

       The direction parameter can further refine the search for specific
       receive or transmit capabilities. The default value of e_Unknown will
       wildcard that field.

       Returns:
       NULL if no capability meeting the criteria was found
      */
    H323Capability * FindCapability(
      const PString & formatName, ///< Wildcard format name to search for
      H323Capability::CapabilityDirection direction = H323Capability::e_Unknown
            ///< Optional direction to include into search criteria
    ) const;

    /**Find the first capability in the table of the specified direction.

       Returns:
       NULL if no capability meeting the criteria was found
      */
    H323Capability * FindCapability(
      H323Capability::CapabilityDirection direction ///< Direction to search for
    ) const;

    /**Find the capability given the capability. This does a value compare of
       the two capabilities. Usually this means the mainType and subType are
       the same.

       Returns:
       NULL if no capability meeting the criteria was found
      */
    H323Capability * FindCapability(
      const H323Capability & capability ///< Capability to search for
    ) const;

    /**Find the capability given the H.245 capability PDU.

       Returns:
       NULL if no capability meeting the criteria was found
      */
    H323Capability * FindCapability(
      const H245_Capability & cap  ///< H245 capability table entry
    ) const;

    /**Find the capability given the H.245 data type PDU.

       Returns:
       NULL if no capability meeting the criteria was found
      */
    H323Capability * FindCapability(
      const H245_DataType & dataType  ///< H245 data type of codec
    ) const;

    /**Find the capability given the H.245 data type PDU.

       Returns:
       NULL if no capability meeting the criteria was found
      */
    H323Capability * FindCapability(
      const H245_ModeElement & modeElement  ///< H245 data type of codec
    ) const;

    /**Find the capability given the sub-type info.

       Returns:
       NULL if no capability meeting the criteria was found
      */
    H323Capability * FindCapability(
      H323Capability::MainTypes mainType, ///< Main type to find
      const PASN_Choice & subTypePDU,     ///< Sub-type info
      const unsigned * translationTable   ///< Translation table sub-type tags
    ) const;

    /**Find the capability given the type codecs.

       Returns:
       NULL if no capability meeting the criteria was found
      */
    H323Capability * FindCapability(
      H323Capability::MainTypes mainType, ///< Main type to find
      unsigned subType = UINT_MAX         ///< Sub-type to find (UINT_MAX=ignore)
    ) const;

    H323Capability * FindCapability(const H245_VideoCapability & video) const;
    BOOL FindCapability(const H323Capability & capability, unsigned ID) const;

    /**Build a H.245 PDU from the information in the capability set.
      */
    void BuildPDU(
      const H323Connection & connection,  ///< Connection building PDU for
      H245_TerminalCapabilitySet & pdu    ///< PDU to build
    ) const;

    /**Merge the capabilities into this set.
      */
    BOOL Merge(
      const H323Capabilities & newCaps
    );

    /**Change the order of capabilities in the table to the order specified.
       Note that this does not change the unique capability numbers assigned
       when the capability is first added to the set.

       The string matching rules are as for the FindCapability() function.
      */
    void Reorder(
      const PStringArray & preferenceOrder  ///< New order
    );

    /**Test if the capability is allowed.
      */
    BOOL IsAllowed(
      const H323Capability & capability
    );

    /**Test if the capability is allowed.
      */
    BOOL IsAllowed(
      unsigned capabilityNumber
    );

    /**Test if the capabilities are an allowed combination.
      */
    BOOL IsAllowed(
      const H323Capability & capability1,
      const H323Capability & capability2
    );

    /**Test if the capabilities are an allowed combination.
      */
    BOOL IsAllowed(
      unsigned capabilityNumber1,
      unsigned capabilityNumber2
    );
  //@}

  protected:
    H323CapabilitiesList table;
    H323CapabilitiesSet  set;
};

///////////////////////////////////////////////////////////////////////////////

#ifdef H323_VIDEO
#ifdef H323_H239

class H323ExtendedVideoCapability : public H323Capability,
	                                public H323GenericCapabilityInfo
{
  PCLASSINFO(H323ExtendedVideoCapability, H323Capability);

  public:
  /**@name Construction */
  //@{
    /**Create a new Extended Video capability.
      */
    H323ExtendedVideoCapability(
      const PString &capabilityId   ///< Extended Capability OID
	);
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

    /**Get the main type of the capability.
       Always returns e_ExtendedVideo.
     */
    virtual H323Capability::MainTypes GetMainType() const;

    /**Get the sub-type of the capability. This is a code dependent on the
       main type of the capability.

       This returns one of the four possible combinations of mode and speed
       using the enum values of the protocol ASN H245_AudioCapability class.
     */
	virtual unsigned GetSubType() const;

    /**Get the default RTP session.
       This function gets the default RTP session ID for the capability
       type. 
       returns H323Capability::DefaultExtVideoSessionID .
      */
    virtual unsigned GetDefaultSessionID() const;

    /**Print Extended Capabilities list
	 */
    virtual void PrintOn(ostream & strm) const;

	/** Create Codec Not Required
	 */
    virtual H323Codec * CreateCodec(H323Codec::Direction /*direction*/) const
    { return NULL; }
  //@}
    

  /**@name Protocol manipulation */
  //@{
    /**This function is called whenever and outgoing TerminalCapabilitySet
       PDU is being constructed for the control channel. It allows the
       capability to set the PDU fields from information in members specific
       to the class.

       The default behaviour calls the OnSendingPDU() function with a more
       specific PDU type.
     */
    virtual BOOL OnSendingPDU(
      H245_Capability & pdu  ///< PDU to set information on
    ) const;

    /**This function is called whenever and incoming TerminalCapabilitySet
       PDU is received on the control channel, and a new H323Capability
       descendent was created. This completes reading fields from the PDU
       into the classes members.

       If the function returns FALSE then the received PDU codec description
       is not supported, so will be ignored. The default behaviour simply
       returns TRUE.
     */
    virtual BOOL OnReceivedPDU(
      const H245_Capability & pdu  ///< PDU to get information from
    );

    /**This function is called whenever and incoming TerminalCapabilitySet
       or OpenLogicalChannel PDU has been used to construct the control
       channel. It allows the capability to set from the PDU fields,
       information in members specific to the class.

       The default behaviour does nothing.
     */

    virtual BOOL OnReceivedPDU(
      const H245_GenericCapability & cap,  ///< PDU to get information from
      CommandType type                     ///<  Type of PDU to send in
    );

    /**This function is called whenever and outgoing TerminalCapabilitySet
       or OpenLogicalChannel PDU is being constructed for the control channel.
       It allows the capability to set the PDU fields from information in
       members specific to the class.

       The default behaviour does nothing.
     */
    virtual BOOL OnSendingPDU(
      H245_GenericCapability & cap,  ///< PDU to get information from
      CommandType type               ///<  Type of PDU to send in
    ) const;

    /**This function is called whenever and outgoing OpenLogicalChannel
       PDU is being constructed for the control channel. It allows the
       capability to set the PDU fields from information in members specific
       to the class.

       The default behaviour is pure.
     */
    virtual BOOL OnSendingPDU(
      H245_DataType & pdu  ///< PDU to set information on
    ) const;

    /**This function is called whenever and outgoing RequestMode
       PDU is being constructed for the control channel. It allows the
       capability to set the PDU fields from information in members specific
       to the class.

       The default behaviour is pure.
     */
    virtual BOOL OnSendingPDU(
      H245_ModeElement & pdu  ///< PDU to set information on
    ) const;

    /**This function is called whenever and incoming OpenLogicalChannel
       PDU has been used to construct the control channel. It allows the
       capability to set from the PDU fields, information in members specific
       to the class.

       The default behaviour is pure.
     */
    virtual BOOL OnReceivedPDU(
      const H245_DataType & pdu,  ///< PDU to get information from
      BOOL receiver               ///< Is receiver OLC
    );

    /**Compare the generic part of the capability, if applicable.
      */
    virtual BOOL IsMatch(
      const PASN_Choice & subTypePDU  ///<  sub-type PDU of H323Capability
    ) const;
  //@}

  /**@name Operations */
  //@{
    /**Create the channel instance, allocating resources as required.
     */
    virtual H323Channel * CreateChannel(
      H323Connection & connection,    ///< Owner connection for channel
      H323Channel::Directions dir,    ///< Direction of channel
      unsigned sessionID,             ///< Session ID for RTP channel
      const H245_H2250LogicalChannelParameters * param
                                      ///< Parameters for channel
    ) const;

	/** Get matching Capability from List */
    H323Capability & operator[](PINDEX i);

	/** Get Number of matching Capabilities */
	PINDEX GetSize() { return table.GetSize(); }

	/**Add All Capabilities
	  */
    static void AddAllCapabilities(
      H323Capabilities &  basecapabilities,  ///< Base Table to add capability to
      PINDEX descriptorNum,   ///< The member of the capabilityDescriptor to add
      PINDEX simultaneous     ///< The member of the SimultaneousCapabilitySet to add
    );
  //@}

protected:
	H323Capabilities extCapabilities;     ///< local Capability List
	H323CapabilitiesList table;           ///< common Capability List

};

//////////////////////////////////////////////////////////////////////////////
//
// Class for Handling extended video control

class H323ControlExtendedVideoCapability : public H323ExtendedVideoCapability
{
  PCLASSINFO(H323ControlExtendedVideoCapability, H323ExtendedVideoCapability);
  public:
	H323ControlExtendedVideoCapability();

   virtual PString GetFormatName() const
    { return "H.239 Control"; }

   virtual PObject * Clone() const
    {
      return new H323ControlExtendedVideoCapability(*this);
    }
};


//////////////////////////////////////////////////////////////////////////////
//
// Class for handling extended video capabilities ie H.239
//

class H323CodecExtendedVideoCapability : public H323ExtendedVideoCapability
{
  PCLASSINFO(H323CodecExtendedVideoCapability, H323ExtendedVideoCapability);
  public:
    H323CodecExtendedVideoCapability();
    ~H323CodecExtendedVideoCapability();

    virtual H323Capability::MainTypes GetMainType() const;

	virtual unsigned GetSubType() const;

    virtual PObject * Clone() const
    { return new H323CodecExtendedVideoCapability(*this); }

	virtual void AddCapability(H323Capability * capability);
//	virtual void AddCapability(const PString & cap);

    virtual BOOL OnReceivedGenericPDU(
		const H245_GenericCapability &pdu
	);

	virtual PString GetFormatName() const
    { return "H.239 Capabilities";}

    virtual BOOL IsMatch(const PASN_Choice & subTypePDU) const;

    virtual BOOL OnSendingPDU(
		H245_Capability & cap
	) const;

    virtual BOOL OnReceivedPDU(
	  const H245_Capability & cap
	);

    virtual BOOL OnSendingPDU(
		H245_VideoCapability & cap
	) const;

    virtual BOOL OnReceivedPDU(
      const H245_VideoCapability & pdu  /// PDU to get information from
	);

};

typedef PFactory<H323VideoCapability, std::string> H323ExtendedVideoFactory;

#endif  // H323_H239
#endif  // H323_VIDEO

/////////////////////////////////////////////////////////////////////////////

#ifdef H323_H230
class H323_ConferenceControlCapability : public H323Capability
{
  PCLASSINFO(H323_ConferenceControlCapability, H323Capability);

  public:
  /**@name Construction */

    /**Create the Conference capability
      */
    H323_ConferenceControlCapability();

	H323_ConferenceControlCapability( 
			       BOOL chairControls,
	               BOOL T124Extension
				   );
  //@}

  /**@name Overrides from class PObject */
  //@{
    /**Create a copy of the object.
      */
    virtual PObject * Clone() const;
  //@}

  /**@name Identification functions */
  //@{
    /**Get the main type of the capability.
     */
    virtual MainTypes GetMainType() const;

    /**Get the sub-type of the capability. This is a code dependent on the
       main type of the capability.
     */
    virtual unsigned GetSubType()  const;

    /**Get the name of the media data format this class represents.
     */
    virtual PString GetFormatName() const;
  //@}

  /**@name Operations */
  //@{
    /**Create the channel instance, allocating resources as required.
       This creates a logical channel object appropriate for the parameters
       provided. Not if param is NULL, sessionID must be provided, otherwise
       this is taken from the fields in param.
     */
    virtual H323Channel * CreateChannel(
      H323Connection & connection,    ///< Owner connection for channel
      H323Channel::Directions dir,    ///< Direction of channel
      unsigned sessionID,             ///< Session ID for RTP channel
      const H245_H2250LogicalChannelParameters * param
                                      ///< Parameters for channel
    ) const;

    /**Create the codec instance, allocating resources as required.
     */
    virtual H323Codec * CreateCodec(
      H323Codec::Direction direction  ///< Direction in which this instance runs
    ) const;

  //@}

  /**@name Protocol manipulation */
  //@{
    /**This function is called whenever and outgoing TerminalCapabilitySet
       PDU is being constructed for the control channel. It allows the
       capability to set the PDU fields from information in members specific
       to the class.

       The default behaviour is pure.
     */
    virtual BOOL OnSendingPDU(
      H245_Capability & pdu  ///< PDU to set information on
    ) const;

    /**This function is called whenever and outgoing OpenLogicalChannel
       PDU is being constructed for the control channel. It allows the
       capability to set the PDU fields from information in members specific
       to the class.

       The default behaviour is pure.
     */
    virtual BOOL OnSendingPDU(
      H245_DataType & pdu  ///< PDU to set information on
    ) const;

    /**This function is called whenever and outgoing RequestMode
       PDU is being constructed for the control channel. It allows the
       capability to set the PDU fields from information in members specific
       to the class.

       The default behaviour calls the OnSendingPDU() function with a more
       specific PDU type.
     */
    virtual BOOL OnSendingPDU(
      H245_ModeElement & pdu  ///< PDU to set information on
    ) const;

    /**This function is called whenever and incoming TerminalCapabilitySet
       PDU is received on the control channel, and a new H323Capability
       descendent was created. This completes reading fields from the PDU
       into the classes members.

       If the function returns FALSE then the received PDU codec description
       is not supported, so will be ignored. The default behaviour simply
       returns TRUE.
     */
    virtual BOOL OnReceivedPDU(
      const H245_Capability & pdu  ///< PDU to get information from
    );

    /**This function is called whenever and incoming OpenLogicalChannel
       PDU has been used to construct the control channel. It allows the
       capability to set from the PDU fields, information in members specific
       to the class.

       The default behaviour is pure.
     */
    virtual BOOL OnReceivedPDU(
      const H245_DataType & pdu,  ///< PDU to get information from
      BOOL receiver               ///< Is receiver OLC
    );
  //@}

  /**@name Option */
  //@{
	/**This function indicates whether the remote supports chair controls
	 */
	BOOL SupportChairControls() {  return chairControlCapability;  }

	/**This function indicates whether the remote supports extended controls
	   such as T.124 Tunnelling
	 */
	BOOL SupportExtControls() {  return nonStandardExtension;  }
  //@}

  protected:
	  BOOL chairControlCapability;
	  BOOL nonStandardExtension;

};
#endif // H323_H230


///////////////////////////////////////////////////////////////////////////////

typedef PFactory<H323Capability, std::string> H323CapabilityFactory;

#define H323_REGISTER_CAPABILITY(cls, capName)   static H323CapabilityFactory::Worker<cls> cls##Factory(capName, true); \

#define H323_DEFINE_CAPABILITY(cls, capName, fmtName) \
class cls : public H323Capability { \
  public: \
    cls() : H323Capability() { } \
    PString GetFormatName() const \
    { return fmtName; } \
}; \
H323_REGISTER_CAPABILITY(cls, capName) \

#define H323_DEFINE_CAPABILITY_FROM(cls, ancestor, capName, fmtName) \
class cls : public ancestor { \
  public: \
    cls() : ancestor() { } \
    PString GetFormatName() const \
    { return fmtName; } \
}; \
H323_REGISTER_CAPABILITY(cls, capName) \

#endif // __OPAL_H323CAPS_H


/////////////////////////////////////////////////////////////////////////////
