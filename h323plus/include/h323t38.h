/*
 * h323t38.h
 *
 * H.323 T.38 logical channel establishment
 *
 * Open H323 Library
 *
 * Copyright (c) 2001 Equivalence Pty. Ltd.
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
 * Contributor(s): ______________________________________.
 *
 * $Log: h323t38.h,v $
 * Revision 1.1  2007/08/06 20:50:49  shorne
 * First commit of h323plus
 *
 * Revision 1.15  2005/11/30 13:05:01  csoutheren
 * Changed tags for Doxygen
 *
 * Revision 1.14  2002/09/16 01:14:15  robertj
 * Added #define so can select if #pragma interface/implementation is used on
 *   platform basis (eg MacOS) rather than compiler, thanks Robert Monaghan.
 *
 * Revision 1.13  2002/09/03 06:19:37  robertj
 * Normalised the multi-include header prevention ifdef/define symbol.
 *
 * Revision 1.12  2002/08/05 10:03:47  robertj
 * Cosmetic changes to normalise the usage of pragma interface/implementation.
 *
 * Revision 1.11  2002/05/15 23:30:01  robertj
 * Backed out delete of t38 handler, causes race conditions.
 *
 * Revision 1.10  2002/05/15 01:30:49  robertj
 * Added missing delete of t38 handler, thanks thsuk@digitalsis.com.
 * Changed to allow the T.35 information to be adjusted so it will work for
 *    various vendors version of the non-standard capability.
 *
 * Revision 1.9  2002/05/10 05:49:22  robertj
 * Added the max bit rate field to the data channel capability class.
 * Added session ID to the data logical channel class.
 * Added capability for old pre-v3 non-standard T.38.
 *
 * Revision 1.8  2002/01/09 00:21:36  robertj
 * Changes to support outgoing H.245 RequstModeChange.
 *
 * Revision 1.7  2002/01/01 23:27:50  craigs
 * Added CleanupOnTermination functions
 * Thanks to Vyacheslav Frolov
 *
 * Revision 1.6  2001/12/22 01:55:20  robertj
 * Removed vast quatities of redundent code that is done by ancestor class.
 *
 * Revision 1.5  2001/12/14 08:36:36  robertj
 * More implementation of T.38, thanks Adam Lazur
 *
 * Revision 1.4  2001/11/20 03:04:30  robertj
 * Added ability to reuse t38 channels with same session ID.
 *
 * Revision 1.3  2001/11/09 05:39:54  craigs
 * Added initial T.38 support thanks to Adam Lazur
 *
 * Revision 1.2  2001/07/24 02:25:57  robertj
 * Added UDP, dual TCP and single TCP modes to T.38 capability.
 *
 * Revision 1.1  2001/07/17 04:44:29  robertj
 * Partial implementation of T.120 and T.38 logical channels.
 *
 */

#ifndef __OPAL_H323T38_H
#define __OPAL_H323T38_H

#ifdef P_USE_PRAGMA
#pragma interface
#endif


#include "h323caps.h"


class H245_T38FaxProfile;

class OpalT38Protocol;


///////////////////////////////////////////////////////////////////////////////

/**This class describes the T.38 standard capability.
 */
class H323_T38Capability : public H323DataCapability
{
    PCLASSINFO(H323_T38Capability, H323DataCapability);
  public:
  /**@name Construction */
  //@{
    enum TransportMode {
      e_UDP,
      e_DualTCP,
      e_SingleTCP,
      NumTransportModes
    };

    /**Create a new capability.
     */
    H323_T38Capability(
      TransportMode mode
    );
  //@}

  /**@name Overrides from class PObject */
  //@{
    /**Compare two capability instances. This compares the main and sub-types
       of the capability.
     */
    Comparison Compare(const PObject & obj) const;

    /**Create a copy of the object.
      */
    virtual PObject * Clone() const;
  //@}

  /**@name Identification functions */
  //@{
    /**Get the sub-type of the capability. This is a code dependent on the
       main type of the capability.

       This returns the e_t38fax enum value from the protocol ASN
       H245_DataApplicationCapability_application class.
     */
    virtual unsigned GetSubType() const;

    /**Get the name of the media data format this class represents.
     */
    virtual PString GetFormatName() const;
  //@}

  /**@name Operations */
  //@{
    /**Create the channel instance, allocating resources as required.
     */
    virtual H323Channel * CreateChannel(
      H323Connection & connection,    ///<  Owner connection for channel
      H323Channel::Directions dir,    ///<  Direction of channel
      unsigned sessionID,             ///<  Session ID for RTP channel
      const H245_H2250LogicalChannelParameters * param
                                      ///<  Parameters for channel
    ) const;
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

    /**This function is called whenever and outgoing RequestMode
       PDU is being constructed for the control channel. It allows the
       capability to set the PDU fields from information in members specific
       to the class.

       The default behaviour sets the pdu and calls OnSendingPDU with a
       H245_DataProtocolCapability parameter.
     */
    virtual BOOL OnSendingPDU(
      H245_DataMode & pdu  ///<  PDU to set information on
    ) const;

    /**This function is called whenever and outgoing PDU is being constructed
       for the control channel. It allows the capability to set the PDU fields
       from information in members specific to the class.

       The default behaviour sets tcp or udp as required.
     */
    virtual BOOL OnSendingPDU(
      H245_DataProtocolCapability & proto,  ///<  PDU to set information on
      H245_T38FaxProfile & profile          ///<  PDU to set information on
    ) const;

    /**This function is called whenever and incoming TerminalCapabilitySet
       or OpenLogicalChannel PDU has been used to construct the control
       channel. It allows the capability to set from the PDU fields,
       information in members specific to the class.

       The default behaviour gets the data rate field from the PDU.
     */
    virtual BOOL OnReceivedPDU(
      const H245_DataApplicationCapability & pdu  ///<  PDU to set information on
    );
  //@}

    TransportMode GetTransportMode() const { return mode; }

  protected:
    TransportMode mode;
};


/**This class describes the T.38 non-standard capability.
 */
class H323_T38NonStandardCapability : public H323NonStandardDataCapability
{
    PCLASSINFO(H323_T38NonStandardCapability, H323NonStandardDataCapability);
  public:
  /**@name Construction */
  //@{
    /**Create a new capability.
     */
    H323_T38NonStandardCapability(
      BYTE country = 181,            ///<  t35 information
      BYTE extension = 0,            ///<  t35 information
      WORD maufacturer = 18          ///<  t35 information
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
    /**Get the name of the media data format this class represents.
     */
    virtual PString GetFormatName() const;
  //@}

  /**@name Operations */
  //@{
    /**Create the channel instance, allocating resources as required.
     */
    virtual H323Channel * CreateChannel(
      H323Connection & connection,    ///<  Owner connection for channel
      H323Channel::Directions dir,    ///<  Direction of channel
      unsigned sessionID,             ///<  Session ID for RTP channel
      const H245_H2250LogicalChannelParameters * param
                                      ///<  Parameters for channel
    ) const;
  //@}
};


/**This class describes the T.38 logical channel.
 */
class H323_T38Channel : public H323DataChannel
{
    PCLASSINFO(H323_T38Channel, H323DataChannel);
  public:
  /**@name Construction */
  //@{
    /**Create a new channel.
     */
    H323_T38Channel(
      H323Connection & connection,       ///<  Connection to endpoint for channel
      const H323Capability & capability, ///<  Capability channel is using
      Directions direction,              ///<  Direction of channel
      unsigned sessionID,                ///<  Session ID for channel
      H323_T38Capability::TransportMode mode
    );
    ~H323_T38Channel();
  //@}

  /**@name Overrides from class H323Channel */
  //@{
    /**Fill out the OpenLogicalChannel PDU for the particular channel type.
     */
    virtual BOOL OnSendingPDU(
      H245_OpenLogicalChannel & openPDU  ///<  Open PDU to send. 
    ) const;

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

    /**This is called to clean up any threads on connection termination.
     */
    virtual void CleanUpOnTermination();

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

    /**Create the H323Listener class to be used.
       This is called on receipt of an OpenLogicalChannel request.

       The default behaviour creates a compatible listener using the
       connections control channel as a basis and returns TRUE if successful.
      */
    virtual BOOL CreateListener();

    /**Create the H323Transport class to be used.
       This is called on receipt of an OpenLogicalChannelAck response. It
       should not return TRUE unless the transport member variable is set.

       The default behaviour uses the connection signalling channel to create
       the transport and returns TRUE if successful.
      */
    virtual BOOL CreateTransport();
  //@}

    OpalT38Protocol * GetHandler() const { return t38handler; }

  protected:
    BOOL              usesTCP;
    OpalT38Protocol * t38handler;
};


#endif // __OPAL_H323T38_H


/////////////////////////////////////////////////////////////////////////////
