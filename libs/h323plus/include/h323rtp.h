/*
 * h323rtp.h
 *
 * H.323 RTP protocol handler
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
 * $Log: h323rtp.h,v $
 * Revision 1.1  2007/08/06 20:50:49  shorne
 * First commit of h323plus
 *
 * Revision 1.13.2.2  2007/05/23 06:58:02  shorne
 * Nat Support for EP's nested behind same NAT
 *
 * Revision 1.13.2.1  2006/12/23 19:08:02  shorne
 * Plugin video codecs & sundry
 *
 * Revision 1.13  2006/01/26 03:44:53  shorne
 * added Transport Capability exchange
 *
 * Revision 1.12  2005/11/30 13:05:01  csoutheren
 * Changed tags for Doxygen
 *
 * Revision 1.11  2003/10/27 06:03:39  csoutheren
 * Added support for QoS
 *   Thanks to Henry Harrison of AliceStreet
 *
 * Revision 1.10  2003/02/07 00:27:59  robertj
 * Changed function to virtual to help in using external multiicast RTP stacks.
 *
 * Revision 1.9  2002/09/16 01:14:15  robertj
 * Added #define so can select if #pragma interface/implementation is used on
 *   platform basis (eg MacOS) rather than compiler, thanks Robert Monaghan.
 *
 * Revision 1.8  2002/09/03 06:19:37  robertj
 * Normalised the multi-include header prevention ifdef/define symbol.
 *
 * Revision 1.7  2002/08/05 10:03:47  robertj
 * Cosmetic changes to normalise the usage of pragma interface/implementation.
 *
 * Revision 1.6  2001/02/09 05:16:24  robertj
 * Added #pragma interface for GNU C++.
 *
 * Revision 1.5  2000/08/31 08:15:32  robertj
 * Added support for dynamic RTP payload types in H.245 OpenLogicalChannel negotiations.
 *
 * Revision 1.4  2000/05/18 11:53:34  robertj
 * Changes to support doc++ documentation generation.
 *
 * Revision 1.3  2000/05/02 04:32:24  robertj
 * Fixed copyright notice comment.
 *
 * Revision 1.2  2000/04/05 03:17:31  robertj
 * Added more RTP statistics gathering and H.245 round trip delay calculation.
 *
 * Revision 1.1  1999/12/23 23:02:35  robertj
 * File reorganision for separating RTP from H.323 and creation of LID for VPB support.
 *
 */

#ifndef __OPAL_H323RTP_H
#define __OPAL_H323RTP_H

#ifdef P_USE_PRAGMA
#pragma interface
#endif


#include "rtp.h"

// #undef P_HAS_QOS

class H225_RTPSession;

class H245_TransportAddress;
class H245_H2250LogicalChannelParameters;
class H245_H2250LogicalChannelAckParameters;
class H245_ArrayOf_GenericInformation;

class H323Connection;
class H323_RTPChannel;

class H245_TransportCapability;


///////////////////////////////////////////////////////////////////////////////

/**This class is for encpsulating the IETF Real Time Protocol interface.
 */
class H323_RTP_Session : public RTP_UserData
{
  PCLASSINFO(H323_RTP_Session, RTP_UserData);

  /**@name Overrides from RTP_UserData */
  //@{
    /**Callback from the RTP session for transmit statistics monitoring.
       This is called every RTP_Session::senderReportInterval packets on the
       transmitter indicating that the statistics have been updated.

       The default behaviour calls H323Connection::OnRTPStatistics().
      */
    virtual void OnTxStatistics(
      const RTP_Session & session   ///< Session with statistics
    ) const;

    /**Callback from the RTP session for receive statistics monitoring.
       This is called every RTP_Session::receiverReportInterval packets on the
       receiver indicating that the statistics have been updated.

       The default behaviour calls H323Connection::OnRTPStatistics().
      */
    virtual void OnRxStatistics(
      const RTP_Session & session   ///< Session with statistics
    ) const;

    /**Callback from the RTP session for statistics monitoring.
       This is called at the end of a call to indicating 
	   that the statistics of the call.

       The default behaviour calls H323Connection::OnFinalStatistics().
      */
    virtual void OnFinalStatistics(
      const RTP_Session & session   ///< Session with statistics
    ) const;
  //@}

  /**@name Operations */
  //@{
    /**Fill out the OpenLogicalChannel PDU for the particular channel type.
     */
    virtual BOOL OnSendingPDU(
      const H323_RTPChannel & channel,            ///< Channel using this session.
      H245_H2250LogicalChannelParameters & param  ///< Open PDU to send.
    ) const = 0;

	/**Sending alternate RTP ports if behind same NAT
	  */
	virtual BOOL OnSendingAltPDU(
    const H323_RTPChannel & channel,               ///< Channel using this session.
	  H245_ArrayOf_GenericInformation & alternate  ///< Alternate RTP ports
	) const = 0;

    /**This is called when request to create a channel is received from a
       remote machine and is about to be acknowledged.
     */
    virtual void OnSendingAckPDU(
      const H323_RTPChannel & channel,              ///< Channel using this session.
      H245_H2250LogicalChannelAckParameters & param ///< Acknowledgement PDU
    ) const = 0;

	/**This is called if the call parties are detected behind the same NAT
	   Use this to set alternate internal LAN ports
	  */
	virtual void OnSendOpenAckAlt(
    const H323_RTPChannel & channel,              ///< Channel using this session.
	  H245_ArrayOf_GenericInformation & alternate  ///< Alternate RTP ports
	) const = 0;

    /**This is called after a request to create a channel occurs from the
       local machine via the H245LogicalChannelDict::Open() function, and
       the request has been acknowledged by the remote endpoint.
     */
    virtual BOOL OnReceivedPDU(
      H323_RTPChannel & channel,                  ///< Channel using this session.
      const H245_H2250LogicalChannelParameters & param, ///< Acknowledgement PDU
      unsigned & errorCode                              ///< Error on failure
    ) = 0;

	/**Alternate RTP port information for Same NAT
	  */
	virtual BOOL OnReceivedAltPDU(
	   H323_RTPChannel & channel,                  ///< Channel using this session.
	  const H245_ArrayOf_GenericInformation & alternate  ///< Alternate RTP ports
	) = 0;

    /**This is called after a request to create a channel occurs from the
       local machine via the H245LogicalChannelDict::Open() function, and
       the request has been acknowledged by the remote endpoint.
     */
    virtual BOOL OnReceivedAckPDU(
      H323_RTPChannel & channel,                  ///< Channel using this session.
      const H245_H2250LogicalChannelAckParameters & param ///< Acknowledgement PDU
    ) = 0;

	/**Alternate RTP port information for Same NAT
	  */
	virtual BOOL OnReceivedAckAltPDU(
      H323_RTPChannel & channel,                         ///< Channel using this session.
	  const H245_ArrayOf_GenericInformation & alternate  ///< Alternate RTP ports
	) = 0;

    /**This is called when a gatekeeper wants to get status information from
       the endpoint.

       The default behaviour fills in the session ID's and SSRC parameters
       but does not do anything with the transport fields.
     */
    virtual void OnSendRasInfo(
      H225_RTPSession & info  ///< RTP session info PDU
    ) = 0;
  //@}


  protected:
  /**@name Construction */
  //@{
    /**Create a new channel.
     */
    H323_RTP_Session(
      const H323Connection & connection  ///< Owner of the RTP session
    );
  //@}

    const H323Connection & connection; ///< Owner of the RTP session
};


/**This class is for the IETF Real Time Protocol interface on UDP/IP.
 */
class H323_RTP_UDP : public H323_RTP_Session
{
  PCLASSINFO(H323_RTP_UDP, H323_RTP_Session);

  public:
  /**@name Construction */
  //@{
    /**Create a new RTP session H323 info.
     */
    H323_RTP_UDP(
      const H323Connection & connection, ///< Owner of the RTP session
      RTP_UDP & rtp,                     ///< RTP session
      RTP_QOS * rtpqos = NULL            ///< QoS spec if available
    );
  //@}

  /**@name Operations */
  //@{
    /**Fill out the OpenLogicalChannel PDU for the particular channel type.
     */
    virtual BOOL OnSendingPDU(
      const H323_RTPChannel & channel,            ///< Channel using this session.
      H245_H2250LogicalChannelParameters & param  ///< Open PDU to send.
    ) const;

	/**Sending alternate RTP ports if behind same NAT
	  */
	virtual BOOL OnSendingAltPDU(
    const H323_RTPChannel & channel,               ///< Channel using this session.
	  H245_ArrayOf_GenericInformation & alternate  ///< Alternate RTP ports
	) const;

    /**This is called when request to create a channel is received from a
       remote machine and is about to be acknowledged.
     */
    virtual void OnSendingAckPDU(
      const H323_RTPChannel & channel,              ///< Channel using this session.
      H245_H2250LogicalChannelAckParameters & param ///< Acknowledgement PDU
    ) const;

	/**This is called if the call parties are detected behind the same NAT
	   Use this to set alternate internal LAN ports
	  */
	virtual void OnSendOpenAckAlt(
    const H323_RTPChannel & channel,              ///< Channel using this session.
	  H245_ArrayOf_GenericInformation & alternate  ///< Alternate RTP ports
	) const;

    /**This is called after a request to create a channel occurs from the
       local machine via the H245LogicalChannelDict::Open() function, and
       the request has been acknowledged by the remote endpoint.

       The default behaviour sets the remote ports to send UDP packets to.
     */
    virtual BOOL OnReceivedPDU(
      H323_RTPChannel & channel,                  ///< Channel using this session.
      const H245_H2250LogicalChannelParameters & param, ///< Acknowledgement PDU
      unsigned & errorCode                              ///< Error on failure
    );

	/**Alternate RTP port information for Same NAT
	  */
	virtual BOOL OnReceivedAltPDU(
	   H323_RTPChannel & channel,                  ///< Channel using this session.
	  const H245_ArrayOf_GenericInformation & alternate  ///< Alternate RTP ports
	);

    /**This is called after a request to create a channel occurs from the
       local machine via the H245LogicalChannelDict::Open() function, and
       the request has been acknowledged by the remote endpoint.

       The default behaviour sets the remote ports to send UDP packets to.
     */
    virtual BOOL OnReceivedAckPDU(
      H323_RTPChannel & channel,                  ///< Channel using this session.
      const H245_H2250LogicalChannelAckParameters & param ///< Acknowledgement PDU
    );

	/**Alternate RTP port information for Same NAT
	  */
	virtual BOOL OnReceivedAckAltPDU(
      H323_RTPChannel & channel,                         ///< Channel using this session.
	  const H245_ArrayOf_GenericInformation & alternate  ///< Alternate RTP ports
	);


    /**This is called when a gatekeeper wants to get status information from
       the endpoint.

       The default behaviour calls the ancestor functon and then fills in the 
       transport fields.
     */
    virtual void OnSendRasInfo(
      H225_RTPSession & info  ///< RTP session info PDU
    );
  //@}

#if P_HAS_QOS
  /**@name GQoS Support */
  //@{
    /**Write the Transport Capability PDU to Include GQoS Support.
     */
    virtual BOOL WriteTransportCapPDU(
       H245_TransportCapability & cap,	  ///* Transport Capability PDU
       const H323_RTPChannel & channel    ///* Channel using this session.
       ) const;

    /**Read the Transport Capability PDU to detect GQoS Support.
     */
    virtual void ReadTransportCapPDU(
	const H245_TransportCapability & cap,	///* Transport Capability PDU
	H323_RTPChannel & channel		///* Channel using this session.
        );
  //@}
#endif
  
  protected:
    virtual BOOL ExtractTransport(
      const H245_TransportAddress & pdu,
      BOOL isDataPort,
      unsigned & errorCode
    );

    RTP_UDP & rtp;
};


#endif // __OPAL_H323RTP_H


/////////////////////////////////////////////////////////////////////////////
