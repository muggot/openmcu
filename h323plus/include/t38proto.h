/*
 * t38proto.h
 *
 * T.38 protocol handler
 *
 * Open Phone Abstraction Library
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
 * $Log: t38proto.h,v $
 * Revision 1.1  2007/08/06 20:50:50  shorne
 * First commit of h323plus
 *
 * Revision 1.9  2002/12/02 04:07:58  robertj
 * Turned T.38 Originate inside out, so now has WriteXXX() functions that can
 *   be call ed in different thread contexts.
 *
 * Revision 1.8  2002/12/02 00:37:15  robertj
 * More implementation of T38 base library code, some taken from the t38modem
 *   application by Vyacheslav Frolov, eg redundent frames.
 *
 * Revision 1.7  2002/09/16 01:14:15  robertj
 * Added #define so can select if #pragma interface/implementation is used on
 *   platform basis (eg MacOS) rather than compiler, thanks Robert Monaghan.
 *
 * Revision 1.6  2002/09/03 06:19:37  robertj
 * Normalised the multi-include header prevention ifdef/define symbol.
 *
 * Revision 1.5  2002/02/09 04:39:01  robertj
 * Changes to allow T.38 logical channels to use single transport which is
 *   now owned by the OpalT38Protocol object instead of H323Channel.
 *
 * Revision 1.4  2002/01/01 23:27:50  craigs
 * Added CleanupOnTermination functions
 * Thanks to Vyacheslav Frolov
 *
 * Revision 1.3  2001/12/22 01:57:04  robertj
 * Cleaned up code and allowed for repeated sequence numbers.
 *
 * Revision 1.2  2001/11/09 05:39:54  craigs
 * Added initial T.38 support thanks to Adam Lazur
 *
 * Revision 1.1  2001/07/17 04:44:29  robertj
 * Partial implementation of T.120 and T.38 logical channels.
 *
 */

#ifndef __OPAL_T38PROTO_H
#define __OPAL_T38PROTO_H

#ifdef P_USE_PRAGMA
#pragma interface
#endif


class H323Transport;
class T38_IFPPacket;
class PASN_OctetString;


///////////////////////////////////////////////////////////////////////////////

/**This class handles the processing of the T.38 protocol.
  */
class OpalT38Protocol : public PObject
{
    PCLASSINFO(OpalT38Protocol, PObject);
  public:
  /**@name Construction */
  //@{
    /**Create a new protocol handler.
     */
    OpalT38Protocol();

    /**Destroy the protocol handler.
     */
    ~OpalT38Protocol();
  //@}

  /**@name Operations */
  //@{
    /**This is called to clean up any threads on connection termination.
     */
    virtual void CleanUpOnTermination();

    /**Handle the origination of a T.38 connection.
       An application would normally override this. The default just sends
       "heartbeat" T.30 no signal indicator.
      */
    virtual BOOL Originate();

    /**Write packet to the T.38 connection.
      */
    virtual BOOL WritePacket(
      const T38_IFPPacket & pdu
    );

    /**Write T.30 indicator packet to the T.38 connection.
      */
    virtual BOOL WriteIndicator(
      unsigned indicator
    );

    /**Write data packet to the T.38 connection.
      */
    virtual BOOL WriteMultipleData(
      unsigned mode,
      PINDEX count,
      unsigned * type,
      const PBYTEArray * data
    );

    /**Write data packet to the T.38 connection.
      */
    virtual BOOL WriteData(
      unsigned mode,
      unsigned type,
      const PBYTEArray & data
    );

    /**Handle the origination of a T.38 connection.
      */
    virtual BOOL Answer();

    /**Handle incoming T.38 packet.

       If returns FALSE, then the reading loop should be terminated.
      */
    virtual BOOL HandlePacket(
      const T38_IFPPacket & pdu
    );

    /**Handle lost T.38 packets.

       If returns FALSE, then the reading loop should be terminated.
      */
    virtual BOOL HandlePacketLost(
      unsigned nLost
    );

    /**Handle incoming T.38 indicator packet.
       If returns FALSE, then the reading loop should be terminated.
      */
    virtual BOOL OnIndicator(
      unsigned indicator
    );

    /**Handle incoming T.38 CNG indicator.
       If returns FALSE, then the reading loop should be terminated.
      */
    virtual BOOL OnCNG();

    /**Handle incoming T.38 CED indicator.
       If returns FALSE, then the reading loop should be terminated.
      */
    virtual BOOL OnCED();

    /**Handle incoming T.38 V.21 preamble indicator.
       If returns FALSE, then the reading loop should be terminated.
      */
    virtual BOOL OnPreamble();

    /**Handle incoming T.38 data mode training indicator.
       If returns FALSE, then the reading loop should be terminated.
      */
    virtual BOOL OnTraining(
      unsigned indicator
    );

    /**Handle incoming T.38 data packet.

       If returns FALSE, then the reading loop should be terminated.
      */
    virtual BOOL OnData(
      unsigned mode,
      unsigned type,
      const PBYTEArray & data
    );
  //@}

    H323Transport * GetTransport() const { return transport; }
    void SetTransport(
      H323Transport * transport,
      BOOL autoDelete = TRUE
    );

  protected:
    BOOL HandleRawIFP(
      const PASN_OctetString & pdu
    );

    H323Transport * transport;
    BOOL            autoDeleteTransport;

    BOOL     corrigendumASN;
    unsigned indicatorRedundancy;
    unsigned lowSpeedRedundancy;
    unsigned highSpeedRedundancy;

    int               lastSentSequenceNumber;
    PList<PBYTEArray> redundantIFPs;
};


#endif // __OPAL_T38PROTO_H


/////////////////////////////////////////////////////////////////////////////
