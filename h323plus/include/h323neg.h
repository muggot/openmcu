/*
 * h323neg.h
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
 * $Log: h323neg.h,v $
 * Revision 1.1  2007/08/06 20:50:49  shorne
 * First commit of h323plus
 *
 * Revision 1.34.14.1  2007/03/24 23:39:42  shorne
 * More H.239 work
 *
 * Revision 1.34  2002/09/16 01:14:15  robertj
 * Added #define so can select if #pragma interface/implementation is used on
 *   platform basis (eg MacOS) rather than compiler, thanks Robert Monaghan.
 *
 * Revision 1.33  2002/09/03 06:19:36  robertj
 * Normalised the multi-include header prevention ifdef/define symbol.
 *
 * Revision 1.32  2002/08/05 10:03:47  robertj
 * Cosmetic changes to normalise the usage of pragma interface/implementation.
 *
 * Revision 1.31  2002/06/26 08:51:16  robertj
 * Fixed deadlock if logical channel is closed via H.245 at exactly same
 *   time as being closed locally due to a channel I/O error.
 *
 * Revision 1.30  2002/05/03 03:08:35  robertj
 * Added replacementFor field in OLC when resolving conflicting channels.
 *
 * Revision 1.29  2002/01/09 00:21:36  robertj
 * Changes to support outgoing H.245 RequstModeChange.
 *
 * Revision 1.28  2002/01/01 23:32:30  craigs
 * Added HandleAck and StartRequest implementations for T.38
 * thanks to Vyacheslav Frolov
 *
 * Revision 1.27  2002/01/01 23:21:30  craigs
 * Added virtual keyword to many functions
 *
 * Revision 1.26  2001/09/12 01:54:45  robertj
 * Added virtual keyword to function in logical channel management.
 *
 * Revision 1.25  2001/08/06 03:08:11  robertj
 * Fission of h323.h to h323ep.h & h323con.h, h323.h now just includes files.
 *
 * Revision 1.24  2001/05/30 23:34:54  robertj
 * Added functions to send TCS=0 for transmitter side pause.
 *
 * Revision 1.23  2001/03/16 07:11:38  robertj
 * Added logical channel open function version without locking.
 *
 * Revision 1.22  2001/03/14 22:05:24  robertj
 * Changed H245NegLogicalChannel::Release() to be virtual protected rather than private.
 *
 * Revision 1.21  2001/03/14 03:20:25  robertj
 * Fixed possible nested mutex deadlock in logical channel negotiator.
 *
 * Revision 1.20  2001/03/06 04:44:46  robertj
 * Fixed problem where could send capability set twice. This should not be
 *   a problem except when talking to another broken stack, eg Cisco routers.
 *
 * Revision 1.19  2001/02/09 05:16:24  robertj
 * Added #pragma interface for GNU C++.
 *
 * Revision 1.18  2000/08/21 12:37:14  robertj
 * Fixed race condition if close call just as slow start media channels are opening, part 2.
 *
 * Revision 1.17  2000/07/14 08:59:56  robertj
 * Fixed race condition in closing connection and explicit close logical channel.
 *
 * Revision 1.16  2000/07/10 16:00:14  robertj
 * Added TCS=0 support.
 * Fixed bug where negotiations hang if not fast start and tunnelled but remot does not tunnel.
 *
 * Revision 1.15  2000/05/22 07:32:51  craigs
 * Fixed problem with ohphone silence detection hanging
 *
 * Revision 1.14  2000/05/16 08:13:32  robertj
 * Added function to find channel by session ID, supporting H323Connection::FindChannel() with mutex.
 *
 * Revision 1.13  2000/05/11 04:16:35  robertj
 * Fixed missing timeout (and typo!) in bidirectional open logical channel.
 *
 * Revision 1.12  2000/05/02 04:32:24  robertj
 * Fixed copyright notice comment.
 *
 * Revision 1.11  2000/04/10 17:50:53  robertj
 * Fixed yet another race condition needing mutex in logical channels management class.
 *
 * Revision 1.10  2000/04/05 03:17:30  robertj
 * Added more RTP statistics gathering and H.245 round trip delay calculation.
 *
 * Revision 1.9  2000/03/25 02:19:50  robertj
 * Fixed missing mutex call in some logical channels structure access.
 *
 * Revision 1.8  1999/11/06 11:58:38  robertj
 * Changed clean up to delete logical channels before channel destructor is called.
 *
 * Revision 1.7  1999/11/06 11:00:08  robertj
 * Fixed race condition in explicit channel close and connection close.
 *
 * Revision 1.6  1999/11/06 05:37:44  robertj
 * Complete rewrite of termination of connection to avoid numerous race conditions.
 *
 * Revision 1.5  1999/10/14 12:05:03  robertj
 * Fixed deadlock possibilities in clearing calls.
 *
 * Revision 1.4  1999/09/21 14:03:03  robertj
 * Fixed incorrect PTRACING test
 *
 * Revision 1.3  1999/09/08 04:05:48  robertj
 * Added support for video capabilities & codec, still needs the actual codec itself!
 *
 * Revision 1.2  1999/08/31 12:34:18  robertj
 * Added gatekeeper support.
 *
 * Revision 1.1  1999/08/25 05:07:49  robertj
 * File fission (critical mass reached).
 *
 */

#ifndef __OPAL_H323NEG_H
#define __OPAL_H323NEG_H

#ifdef P_USE_PRAGMA
#pragma interface
#endif


#include "h323pdu.h"
#include "channels.h"



///////////////////////////////////////////////////////////////////////////////

/**Base class for doing H245 negotiations
 */
class H245Negotiator : public PObject
{
  PCLASSINFO(H245Negotiator, PObject);

  public:
    H245Negotiator(H323EndPoint & endpoint, H323Connection & connection);

  protected:
    PDECLARE_NOTIFIER(PTimer, H245Negotiator, HandleTimeout);

    H323EndPoint   & endpoint;
    H323Connection & connection;
    PTimer           replyTimer;
    PMutex           mutex;
};


/**Determine the master and slave on a H245 connection as per H245 section 8.2
 */
class H245NegMasterSlaveDetermination : public H245Negotiator
{
  PCLASSINFO(H245NegMasterSlaveDetermination, H245Negotiator);

  public:
    H245NegMasterSlaveDetermination(H323EndPoint & endpoint, H323Connection & connection);

    BOOL Start(BOOL renegotiate);
    void Stop();
    BOOL HandleIncoming(const H245_MasterSlaveDetermination & pdu);
    BOOL HandleAck(const H245_MasterSlaveDeterminationAck & pdu);
    BOOL HandleReject(const H245_MasterSlaveDeterminationReject & pdu);
    BOOL HandleRelease(const H245_MasterSlaveDeterminationRelease & pdu);
    void HandleTimeout(PTimer &, INT);

    BOOL IsMaster() const     { return status == e_DeterminedMaster; }
    BOOL IsDetermined() const { return state == e_Idle && status != e_Indeterminate; }

  protected:
    BOOL Restart();

    enum States {
      e_Idle, e_Outgoing, e_Incoming,
      e_NumStates
    } state;
#if PTRACING
    static const char * const StateNames[e_NumStates];
    friend ostream & operator<<(ostream & o, States s) { return o << StateNames[s]; }
#endif

    DWORD    determinationNumber;
    unsigned retryCount;

    enum MasterSlaveStatus {
      e_Indeterminate, e_DeterminedMaster, e_DeterminedSlave,
      e_NumStatuses
    } status;
#if PTRACING
    static const char * const StatusNames[e_NumStatuses];
    friend ostream & operator<<(ostream & o , MasterSlaveStatus s) { return o << StatusNames[s]; }
#endif
};


/**Exchange capabilities on a H245 connection as per H245 section 8.3
 */
class H245NegTerminalCapabilitySet : public H245Negotiator
{
  PCLASSINFO(H245NegTerminalCapabilitySet, H245Negotiator);

  public:
    H245NegTerminalCapabilitySet(H323EndPoint & endpoint, H323Connection & connection);

    BOOL Start(BOOL renegotiate, BOOL empty = FALSE);
    void Stop();
    BOOL HandleIncoming(const H245_TerminalCapabilitySet & pdu);
    BOOL HandleAck(const H245_TerminalCapabilitySetAck & pdu);
    BOOL HandleReject(const H245_TerminalCapabilitySetReject & pdu);
    BOOL HandleRelease(const H245_TerminalCapabilitySetRelease & pdu);
    void HandleTimeout(PTimer &, INT);

    BOOL HasSentCapabilities() const { return state == e_Sent; }
    BOOL HasReceivedCapabilities() const { return receivedCapabilites; }

  protected:
    enum States {
      e_Idle, e_InProgress, e_Sent,
      e_NumStates
    } state;
#if PTRACING
    static const char * const StateNames[e_NumStates];
    friend ostream & operator<<(ostream & o, States s) { return o << StateNames[s]; }
#endif

    unsigned inSequenceNumber;
    unsigned outSequenceNumber;

    BOOL receivedCapabilites;
};


/**Logical Channel signalling on a H245 connection as per H245 section 8.4
 */
class H245NegLogicalChannel : public H245Negotiator
{
  PCLASSINFO(H245NegLogicalChannel, H245Negotiator);

  public:
    H245NegLogicalChannel(H323EndPoint & endpoint,
                          H323Connection & connection,
                          const H323ChannelNumber & channelNumber);
    H245NegLogicalChannel(H323EndPoint & endpoint,
                          H323Connection & connection,
                          H323Channel & channel);
    ~H245NegLogicalChannel();

    virtual BOOL Open(
      const H323Capability & capability,
      unsigned sessionID,
      unsigned replacementFor = 0,
	  unsigned roleLabel = 0
    );
    virtual BOOL Close();
    virtual BOOL HandleOpen(const H245_OpenLogicalChannel & pdu);
    virtual BOOL HandleOpenAck(const H245_OpenLogicalChannelAck & pdu);
    virtual BOOL HandleOpenConfirm(const H245_OpenLogicalChannelConfirm & pdu);
    virtual BOOL HandleReject(const H245_OpenLogicalChannelReject & pdu);
    virtual BOOL HandleClose(const H245_CloseLogicalChannel & pdu);
    virtual BOOL HandleCloseAck(const H245_CloseLogicalChannelAck & pdu);
    virtual BOOL HandleRequestClose(const H245_RequestChannelClose & pdu);
    virtual BOOL HandleRequestCloseAck(const H245_RequestChannelCloseAck & pdu);
    virtual BOOL HandleRequestCloseReject(const H245_RequestChannelCloseReject & pdu);
    virtual BOOL HandleRequestCloseRelease(const H245_RequestChannelCloseRelease & pdu);
    virtual void HandleTimeout(PTimer &, INT);

    H323Channel * GetChannel();


  protected:
    virtual BOOL OpenWhileLocked(
      const H323Capability & capability,
      unsigned sessionID,
      unsigned replacementFor = 0,
	  unsigned roleLabel = 0
    );
    virtual BOOL CloseWhileLocked();
    virtual void Release();


    H323Channel * channel;

    H323ChannelNumber channelNumber;

    enum States {
      e_Released,
      e_AwaitingEstablishment,
      e_Established,
      e_AwaitingRelease,
      e_AwaitingConfirmation,
      e_AwaitingResponse,
      e_NumStates
    } state;
#if PTRACING
    static const char * const StateNames[e_NumStates];
    friend ostream & operator<<(ostream & o, States s) { return o << StateNames[s]; }
#endif


  friend class H245NegLogicalChannels;
};


PDICTIONARY(H245LogicalChannelDict, H323ChannelNumber, H245NegLogicalChannel);

/**Dictionary of all Logical Channels
 */
class H245NegLogicalChannels : public H245Negotiator
{
  PCLASSINFO(H245NegLogicalChannels, H245Negotiator);

  public:
    H245NegLogicalChannels(H323EndPoint & endpoint, H323Connection & connection);

    virtual void Add(H323Channel & channel);

    virtual BOOL Open(
      const H323Capability & capability,
      unsigned sessionID,
      unsigned replacementFor = 0
    );

    virtual BOOL Open(
      const H323Capability & capability,
      unsigned sessionID,
	  H323ChannelNumber & channelnumber,
      unsigned replacementFor = 0,
	  unsigned roleLabel = 0
    );

    virtual BOOL Close(unsigned channelNumber, BOOL fromRemote);
    virtual BOOL HandleOpen(const H245_OpenLogicalChannel & pdu);
    virtual BOOL HandleOpenAck(const H245_OpenLogicalChannelAck & pdu);
    virtual BOOL HandleOpenConfirm(const H245_OpenLogicalChannelConfirm & pdu);
    virtual BOOL HandleReject(const H245_OpenLogicalChannelReject & pdu);
    virtual BOOL HandleClose(const H245_CloseLogicalChannel & pdu);
    virtual BOOL HandleCloseAck(const H245_CloseLogicalChannelAck & pdu);
    virtual BOOL HandleRequestClose(const H245_RequestChannelClose & pdu);
    virtual BOOL HandleRequestCloseAck(const H245_RequestChannelCloseAck & pdu);
    virtual BOOL HandleRequestCloseReject(const H245_RequestChannelCloseReject & pdu);
    virtual BOOL HandleRequestCloseRelease(const H245_RequestChannelCloseRelease & pdu);

    H323ChannelNumber GetNextChannelNumber();
    PINDEX GetSize() const { return channels.GetSize(); }
    H323Channel * GetChannelAt(PINDEX i);
    H323Channel * FindChannel(unsigned channelNumber, BOOL fromRemote);
    H245NegLogicalChannel & GetNegLogicalChannelAt(PINDEX i);
    H245NegLogicalChannel * FindNegLogicalChannel(unsigned channelNumber, BOOL fromRemote);
    H323Channel * FindChannelBySession(unsigned rtpSessionId, BOOL fromRemote);
    void RemoveAll();

  protected:
    H323ChannelNumber      lastChannelNumber;
    H245LogicalChannelDict channels;
};


/**Request mode change as per H245 section 8.9
 */
class H245NegRequestMode : public H245Negotiator
{
  PCLASSINFO(H245NegRequestMode, H245Negotiator);

  public:
    H245NegRequestMode(H323EndPoint & endpoint, H323Connection & connection);

    virtual BOOL StartRequest(const PString & newModes);
    virtual BOOL StartRequest(const H245_ArrayOf_ModeDescription & newModes);
    virtual BOOL HandleRequest(const H245_RequestMode & pdu);
    virtual BOOL HandleAck(const H245_RequestModeAck & pdu);
    virtual BOOL HandleReject(const H245_RequestModeReject & pdu);
    virtual BOOL HandleRelease(const H245_RequestModeRelease & pdu);
    virtual void HandleTimeout(PTimer &, INT);

  protected:
    BOOL awaitingResponse;
    unsigned inSequenceNumber;
    unsigned outSequenceNumber;
};


/**Request mode change as per H245 section 8.9
 */
class H245NegRoundTripDelay : public H245Negotiator
{
  PCLASSINFO(H245NegRoundTripDelay, H245Negotiator);

  public:
    H245NegRoundTripDelay(H323EndPoint & endpoint, H323Connection & connection);

    BOOL StartRequest();
    BOOL HandleRequest(const H245_RoundTripDelayRequest & pdu);
    BOOL HandleResponse(const H245_RoundTripDelayResponse & pdu);
    void HandleTimeout(PTimer &, INT);

    PTimeInterval GetRoundTripDelay() const { return roundTripTime; }
    BOOL IsRemoteOffline() const { return retryCount == 0; }

  protected:
    BOOL          awaitingResponse;
    unsigned      sequenceNumber;
    PTimeInterval tripStartTime;
    PTimeInterval roundTripTime;
    unsigned      retryCount;
};


#endif // __OPAL_H323NEG_H


/////////////////////////////////////////////////////////////////////////////
