/*
 * h323neg.cxx
 *
 * H.323 PDU definitions
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
 * $Log: h323neg.cxx,v $
 * Revision 1.1  2007/08/06 20:51:07  shorne
 * First commit of h323plus
 *
 * Revision 1.70.4.1  2007/03/24 23:39:43  shorne
 * More H.239 work
 *
 * Revision 1.70  2005/05/03 12:23:05  csoutheren
 * Unlock connection list when creating connection
 * Remove chance of race condition with H.245 negotiation timer
 * Unregister OpalPluginMediaFormat when instance is destroyed
 * Thank to Paul Cadach
 *
 * Revision 1.69  2003/01/30 01:38:19  robertj
 * Fixed some trace log errors
 *
 * Revision 1.68  2002/11/05 05:33:56  robertj
 * Fixed problem where if capability is rejected by OnReceivedCapabilitySet()
 *   it still thinks it has received them.
 *
 * Revision 1.67  2002/08/05 10:03:47  robertj
 * Cosmetic changes to normalise the usage of pragma interface/implementation.
 *
 * Revision 1.66  2002/06/27 07:11:55  robertj
 * Fixed GNU warning
 *
 * Revision 1.65  2002/06/26 08:51:19  robertj
 * Fixed deadlock if logical channel is closed via H.245 at exactly same
 *   time as being closed locally due to a channel I/O error.
 *
 * Revision 1.64  2002/05/03 03:08:40  robertj
 * Added replacementFor field in OLC when resolving conflicting channels.
 *
 * Revision 1.63  2002/01/10 04:38:29  robertj
 * Fixed problem with having incorrect constraint if getting array from caller on
 *   setting up a requestMode.
 *
 * Revision 1.62  2002/01/10 04:02:08  robertj
 * Fixed correct filling of modeRequest structure.
 *
 * Revision 1.61  2002/01/09 00:21:40  robertj
 * Changes to support outgoing H.245 RequstModeChange.
 *
 * Revision 1.60  2002/01/08 01:26:32  robertj
 * Removed PTRACE in FindChannelsBySession as it can get called a LOT.
 *
 * Revision 1.59  2002/01/01 23:24:04  craigs
 * Removed reference to myPTRACE
 *
 * Revision 1.58  2002/01/01 23:20:22  craigs
 * Added HandleAck and StartRequest implementations for T.38
 * thanks to Vyacheslav Frolov
 *
 * Revision 1.57  2001/12/22 03:10:17  robertj
 * Changed OnRequstModeChange to return ack, then actually do the change.
 *
 * Revision 1.56  2001/12/22 01:46:33  robertj
 * Fixed bug in RequestMode negotiation, failure to set sequence number.
 *
 * Revision 1.55  2001/12/14 08:36:36  robertj
 * More implementation of T.38, thanks Adam Lazur
 *
 * Revision 1.54  2001/11/11 23:07:52  robertj
 * Some clean ups after T.38 commit, thanks Adam Lazur
 *
 * Revision 1.53  2001/11/09 05:39:54  craigs
 * Added initial T.38 support thanks to Adam Lazur
 *
 * Revision 1.52  2001/09/13 04:18:57  robertj
 * Added support for "reopen" function when closing logical channel.
 *
 * Revision 1.51  2001/08/06 03:08:56  robertj
 * Fission of h323.h to h323ep.h & h323con.h, h323.h now just includes files.
 *
 * Revision 1.50  2001/05/30 23:34:54  robertj
 * Added functions to send TCS=0 for transmitter side pause.
 *
 * Revision 1.49  2001/05/09 02:34:37  robertj
 * Fixed problem with being able to start two MSD's at the same time.
 * Fixed possible race condition in code to prevent two TCS's happening
 *   at the same time.
 *
 * Revision 1.48  2001/04/26 02:01:53  robertj
 * Added workaround for more strange Cisco behaviour, stops sending RTP
 *   data if you try and do a H.245 RoundTripDelay request!!
 *
 * Revision 1.47  2001/03/16 07:11:39  robertj
 * Added logical channel open function version without locking.
 *
 * Revision 1.46  2001/03/14 03:20:27  robertj
 * Fixed possible nested mutex deadlock in logical channel negotiator.
 *
 * Revision 1.45  2001/03/06 04:44:47  robertj
 * Fixed problem where could send capability set twice. This should not be
 *   a problem except when talking to another broken stack, eg Cisco routers.
 *
 * Revision 1.44  2001/02/09 05:13:56  craigs
 * Added pragma implementation to (hopefully) reduce the executable image size
 * under Linux
 *
 * Revision 1.43  2001/01/04 07:46:22  robertj
 * Added special test for broken NetMeeting in restablishing logical channels
 *   when have to close one due to device conflict (asymetric codecs).
 *
 * Revision 1.42  2001/01/02 08:03:28  robertj
 * Changed MSD determination number comparision to be clearly as per spec.
 *
 * Revision 1.41  2000/10/16 09:46:37  robertj
 * Fixed problem with opening codec before capability fields updated from
 *    OLC PDU, caused problems in G.711 tweaked psuedo-packet size.
 *
 * Revision 1.40  2000/09/22 01:35:50  robertj
 * Added support for handling LID's that only do symmetric codecs.
 *
 * Revision 1.39  2000/09/12 01:01:17  robertj
 * Fixed protocol issue, should send CloseLogicalChannel PDU's on accepting RequestClose.
 *
 * Revision 1.38  2000/08/31 10:49:38  robertj
 * Fixed unix deadlock when non-fastStart but with H245 tunneling.
 *
 * Revision 1.37  2000/08/21 12:37:14  robertj
 * Fixed race condition if close call just as slow start media channels are opening, part 2.
 *
 * Revision 1.36  2000/08/21 02:50:28  robertj
 * Fixed race condition if close call just as slow start media channels are opening.
 *
 * Revision 1.35  2000/07/14 09:00:48  robertj
 * Fixed race condition in closing connection and explicit close logical channel.
 *
 * Revision 1.34  2000/07/11 19:31:28  robertj
 * Fixed possible deadlock under unix when trying to restart running TCS.
 *
 * Revision 1.33  2000/07/10 16:04:01  robertj
 * Added TCS=0 support.
 * Fixed bug where negotiations hang if not fast start and tunnelled but remot does not tunnel.
 *
 * Revision 1.32  2000/07/09 14:57:54  robertj
 * Fixed bug where don't accept TCS if supplied sequence number is zero.
 *
 * Revision 1.31  2000/07/04 13:00:36  craigs
 * Fixed problem with selecting large and small video sizes
 *
 * Revision 1.30  2000/07/04 04:15:41  robertj
 * Fixed capability check of "combinations" for fast start cases.
 *
 * Revision 1.29  2000/07/03 00:38:40  robertj
 * Fixed deadlock caused by nested mutex waits in negotiated channel closing.
 *
 * Revision 1.28  2000/06/27 03:46:52  robertj
 * Fixed another possible race condition on logical channel closure.
 *
 * Revision 1.27  2000/06/03 03:16:39  robertj
 * Fixed using the wrong capability table (should be connections) for some operations.
 *
 * Revision 1.26  2000/05/23 11:32:37  robertj
 * Rewrite of capability table to combine 2 structures into one and move functionality into that class
 *    allowing some normalisation of usage across several applications.
 * Changed H323Connection so gets a copy of capabilities instead of using endponts, allows adjustments
 *    to be done depending on the remote client application.
 *
 * Revision 1.25  2000/05/22 07:32:51  craigs
 * Fixed problem with ohphone silence detection hanging
 *
 * Revision 1.24  2000/05/16 08:13:42  robertj
 * Added function to find channel by session ID, supporting H323Connection::FindChannel() with mutex.
 *
 * Revision 1.23  2000/05/11 04:19:45  robertj
 * Fixed typo
 *
 * Revision 1.22  2000/05/11 04:16:35  robertj
 * Fixed missing timeout (and typo!) in bidirectional open logical channel.
 *
 * Revision 1.21  2000/05/08 05:06:27  robertj
 * Fixed bug in H.245 close logical channel timeout, thanks XuPeili.
 *
 * Revision 1.20  2000/05/05 04:35:10  robertj
 * Changed where the system timer is sampled to minimise overhead in round trip delay calculation.
 *
 * Revision 1.19  2000/05/02 04:32:27  robertj
 * Fixed copyright notice comment.
 *
 * Revision 1.18  2000/04/24 23:47:01  robertj
 * Fixed bug in master/slave negotiator for if have "in sync" random number generators, thanks Shawn Sincoski.
 *
 * Revision 1.17  2000/04/10 17:50:28  robertj
 * Fixed yet another race condition needing mutex in logical channels management class.
 *
 * Revision 1.16  2000/04/05 03:17:31  robertj
 * Added more RTP statistics gathering and H.245 round trip delay calculation.
 *
 * Revision 1.15  2000/03/29 02:14:46  robertj
 * Changed TerminationReason to CallEndReason to use correct telephony nomenclature.
 * Added CallEndReason for capability exchange failure.
 *
 * Revision 1.14  2000/03/25 02:19:50  robertj
 * Fixed missing mutex call in some logical channels structure access.
 *
 * Revision 1.13  2000/02/17 12:07:43  robertj
 * Used ne wPWLib random number generator after finding major problem in MSVC rand().
 *
 * Revision 1.12  1999/11/26 01:25:01  craigs
 * Removed deadlock when remote endpoint requests unsupported channel type
 *
 * Revision 1.11  1999/11/19 09:07:27  robertj
 * Fixed bug allocating incorrect channel number to fast start transmit channel.
 *
 * Revision 1.10  1999/11/06 11:00:18  robertj
 * Fixed race condition in explicit channel close and connection close.
 *
 * Revision 1.9  1999/11/06 05:37:45  robertj
 * Complete rewrite of termination of connection to avoid numerous race conditions.
 *
 * Revision 1.8  1999/10/30 12:34:47  robertj
 * Added information callback for closed logical channel on H323EndPoint.
 *
 * Revision 1.7  1999/10/24 04:45:14  robertj
 * Removed requirement that must be sending capability set to receive capability set.
 *
 * Revision 1.6  1999/09/21 14:10:41  robertj
 * Fixed incorrect PTRACING test and removed uneeded include of videoio.h
 *
 * Revision 1.5  1999/09/15 01:26:27  robertj
 * Changed capability set call backs to have more specific class as parameter.
 *
 * Revision 1.4  1999/09/14 14:26:17  robertj
 * Added more debug tracing.
 *
 * Revision 1.3  1999/09/08 04:05:49  robertj
 * Added support for video capabilities & codec, still needs the actual codec itself!
 *
 * Revision 1.2  1999/08/31 12:34:19  robertj
 * Added gatekeeper support.
 *
 * Revision 1.1  1999/08/25 05:08:14  robertj
 * File fission (critical mass reached).
 *
 */

#include <ptlib.h>

#ifdef __GNUC__
#pragma implementation "h323neg.h"
#endif

#include "h323neg.h"

#include "h323ep.h"

#include <ptclib/random.h>


#define new PNEW


///////////////////////////////////////////////////////////////////////////////

H245Negotiator::H245Negotiator(H323EndPoint & end, H323Connection & conn)
  : endpoint(end),
    connection(conn)
{
  replyTimer.SetNotifier(PCREATE_NOTIFIER(HandleTimeout));
}


void H245Negotiator::HandleTimeout(PTimer &, INT)
{
}


/////////////////////////////////////////////////////////////////////////////

H245NegMasterSlaveDetermination::H245NegMasterSlaveDetermination(H323EndPoint & end,
                                                                 H323Connection & conn)
  : H245Negotiator(end, conn)
{
  retryCount = 1;
  state = e_Idle;
  status = e_Indeterminate;
}


BOOL H245NegMasterSlaveDetermination::Start(BOOL renegotiate)
{
  PWaitAndSignal wait(mutex);

  if (state != e_Idle) {
    PTRACE(3, "H245\tMasterSlaveDetermination already in progress");
    return TRUE;
  }

  if (!renegotiate && IsDetermined())
    return TRUE;

  retryCount = 1;
  return Restart();
}


BOOL H245NegMasterSlaveDetermination::Restart()
{
  PTRACE(3, "H245\tSending MasterSlaveDetermination");

  // Begin the Master/Slave determination procedure
  determinationNumber = PRandom::Number()%16777216;
  replyTimer = endpoint.GetMasterSlaveDeterminationTimeout();
  state = e_Outgoing;

  H323ControlPDU pdu;
  pdu.BuildMasterSlaveDetermination(endpoint.GetTerminalType(), determinationNumber);
  return connection.WriteControlPDU(pdu);
}


void H245NegMasterSlaveDetermination::Stop()
{
  PWaitAndSignal wait(mutex);

  PTRACE(3, "H245\tStopping MasterSlaveDetermination: state=" << state);

  if (state == e_Idle)
    return;

  replyTimer.Stop();
  state = e_Idle;
}


BOOL H245NegMasterSlaveDetermination::HandleIncoming(const H245_MasterSlaveDetermination & pdu)
{
  replyTimer.Stop();
  PWaitAndSignal wait(mutex);

  PTRACE(3, "H245\tReceived MasterSlaveDetermination: state=" << state);

  if (state == e_Incoming) {
    replyTimer.Stop();
    state = e_Idle;
    return connection.OnControlProtocolError(H323Connection::e_MasterSlaveDetermination,
                                             "Duplicate MasterSlaveDetermination");
  }

  replyTimer = endpoint.GetMasterSlaveDeterminationTimeout();

  // Determine the master and slave
  MasterSlaveStatus newStatus;
  if (pdu.m_terminalType < (unsigned)endpoint.GetTerminalType())
    newStatus = e_DeterminedMaster;
  else if (pdu.m_terminalType > (unsigned)endpoint.GetTerminalType())
    newStatus = e_DeterminedSlave;
  else {
    DWORD moduloDiff = (pdu.m_statusDeterminationNumber - determinationNumber)&0xffffff;
    if (moduloDiff == 0 || moduloDiff == 0x800000)
      newStatus = e_Indeterminate;
    else if (moduloDiff < 0x800000)
      newStatus = e_DeterminedMaster;
    else
      newStatus = e_DeterminedSlave;
  }

  H323ControlPDU reply;

  if (newStatus != e_Indeterminate) {
    PTRACE(2, "H245\tMasterSlaveDetermination: local is "
                  << (newStatus == e_DeterminedMaster ? "master" : "slave"));
    reply.BuildMasterSlaveDeterminationAck(newStatus == e_DeterminedMaster);
    state = e_Incoming;
    status = newStatus;
  }
  else if (state == e_Outgoing) {
    retryCount++;
    if (retryCount < endpoint.GetMasterSlaveDeterminationRetries())
      return Restart(); // Try again

    replyTimer.Stop();
    state = e_Idle;
    return connection.OnControlProtocolError(H323Connection::e_MasterSlaveDetermination,
                                             "Retries exceeded");
  }
  else {
    reply.BuildMasterSlaveDeterminationReject(H245_MasterSlaveDeterminationReject_cause::e_identicalNumbers);
  }

  return connection.WriteControlPDU(reply);
}


BOOL H245NegMasterSlaveDetermination::HandleAck(const H245_MasterSlaveDeterminationAck & pdu)
{
  replyTimer.Stop();
  PWaitAndSignal wait(mutex);

  PTRACE(3, "H245\tReceived MasterSlaveDeterminationAck: state=" << state);

  if (state == e_Idle)
    return TRUE;

  replyTimer = endpoint.GetMasterSlaveDeterminationTimeout();

  MasterSlaveStatus newStatus;
  if (pdu.m_decision.GetTag() == H245_MasterSlaveDeterminationAck_decision::e_master)
    newStatus = e_DeterminedMaster;
  else
    newStatus = e_DeterminedSlave;

  H323ControlPDU reply;
  if (state == e_Outgoing) {
    status = newStatus;
    PTRACE(2, "H245\tMasterSlaveDetermination: remote is "
                  << (newStatus == e_DeterminedSlave ? "master" : "slave"));
    reply.BuildMasterSlaveDeterminationAck(newStatus == e_DeterminedMaster);
    if (!connection.WriteControlPDU(reply))
      return FALSE;
  }

  replyTimer.Stop();
  state = e_Idle;

  if (status != newStatus)
    return connection.OnControlProtocolError(H323Connection::e_MasterSlaveDetermination,
                                             "Master/Slave mismatch");

  return TRUE;
}


BOOL H245NegMasterSlaveDetermination::HandleReject(const H245_MasterSlaveDeterminationReject & pdu)
{
  replyTimer.Stop();
  PWaitAndSignal wait(mutex);

  PTRACE(3, "H245\tReceived MasterSlaveDeterminationReject: state=" << state);

  switch (state) {
    case e_Idle :
      return TRUE;

    case e_Outgoing :
      if (pdu.m_cause.GetTag() == H245_MasterSlaveDeterminationReject_cause::e_identicalNumbers) {
        retryCount++;
        if (retryCount < endpoint.GetMasterSlaveDeterminationRetries())
          return Restart();
      }

    default :
      break;
  }

  replyTimer.Stop();
  state = e_Idle;

  return connection.OnControlProtocolError(H323Connection::e_MasterSlaveDetermination,
                                           "Retries exceeded");
}


BOOL H245NegMasterSlaveDetermination::HandleRelease(const H245_MasterSlaveDeterminationRelease & /*pdu*/)
{
  replyTimer.Stop();
  PWaitAndSignal wait(mutex);

  PTRACE(3, "H245\tReceived MasterSlaveDeterminationRelease: state=" << state);

  if (state == e_Idle)
    return TRUE;

  replyTimer.Stop();
  state = e_Idle;

  return connection.OnControlProtocolError(H323Connection::e_MasterSlaveDetermination,
                                           "Aborted");
}


void H245NegMasterSlaveDetermination::HandleTimeout(PTimer &, INT)
{
  PWaitAndSignal wait(mutex);

  PTRACE(3, "H245\tTimeout on MasterSlaveDetermination: state=" << state);

  if (state == e_Outgoing) {
    H323ControlPDU reply;
    reply.Build(H245_IndicationMessage::e_masterSlaveDeterminationRelease);
    connection.WriteControlPDU(reply);
  }

  state = e_Idle;

  connection.OnControlProtocolError(H323Connection::e_MasterSlaveDetermination,
                                    "Timeout");
}


#if PTRACING
const char * const H245NegMasterSlaveDetermination::StateNames[] = {
  "Idle", "Outgoing", "Incoming"
};


const char * const H245NegMasterSlaveDetermination::StatusNames[] = {
  "Indeterminate", "DeterminedMaster", "DeterminedSlave"
};
#endif


/////////////////////////////////////////////////////////////////////////////

H245NegTerminalCapabilitySet::H245NegTerminalCapabilitySet(H323EndPoint & end,
                                                           H323Connection & conn)
  : H245Negotiator(end, conn)
{
  inSequenceNumber = UINT_MAX;
  outSequenceNumber = 0;
  state = e_Idle;
  receivedCapabilites = FALSE;
}


BOOL H245NegTerminalCapabilitySet::Start(BOOL renegotiate, BOOL empty)
{
  replyTimer.Stop();
  PWaitAndSignal wait(mutex);

  if (state == e_InProgress) {
    PTRACE(3, "H245\tTerminalCapabilitySet already in progress: outSeq=" << outSequenceNumber);
    return TRUE;
  }

  if (!renegotiate && state == e_Sent) {
    PTRACE(3, "H245\tTerminalCapabilitySet already sent.");
    return TRUE;
  }

  // Begin the capability exchange procedure
  outSequenceNumber = (outSequenceNumber+1)%256;
  replyTimer = endpoint.GetCapabilityExchangeTimeout();
  state = e_InProgress;

  PTRACE(3, "H245\tSending TerminalCapabilitySet: outSeq=" << outSequenceNumber);

  H323ControlPDU pdu;
  connection.OnSendCapabilitySet(pdu.BuildTerminalCapabilitySet(connection, outSequenceNumber, empty));
  return connection.WriteControlPDU(pdu);
}


void H245NegTerminalCapabilitySet::Stop()
{
  PWaitAndSignal wait(mutex);

  PTRACE(3, "H245\tStopping TerminalCapabilitySet: state=" << state);

  if (state == e_Idle)
    return;

  replyTimer.Stop();
  state = e_Idle;
  receivedCapabilites = FALSE;
}


BOOL H245NegTerminalCapabilitySet::HandleIncoming(const H245_TerminalCapabilitySet & pdu)
{
  replyTimer.Stop();
  mutex.Wait();

  PTRACE(3, "H245\tReceived TerminalCapabilitySet:"
                     " state=" << state <<
                     " pduSeq=" << pdu.m_sequenceNumber <<
                     " inSeq=" << inSequenceNumber);

  if (pdu.m_sequenceNumber == inSequenceNumber) {
    mutex.Signal();
    PTRACE(3, "H245\tIgnoring TerminalCapabilitySet, already received sequence number");
    return TRUE;  // Already had this one
  }

  inSequenceNumber = pdu.m_sequenceNumber;

  mutex.Signal();

  PTRACE(3, "H245\tremoteCapabilities start\n");
  H323Capabilities remoteCapabilities(connection, pdu);
  PTRACE(3, "H245\tremoteCapabilities end\n");

  const H245_MultiplexCapability * muxCap = NULL;
  if (pdu.HasOptionalField(H245_TerminalCapabilitySet::e_multiplexCapability))
    muxCap = &pdu.m_multiplexCapability;

  H323ControlPDU reject;
  if (connection.OnReceivedCapabilitySet(remoteCapabilities, muxCap,
                    reject.BuildTerminalCapabilitySetReject(inSequenceNumber,
                            H245_TerminalCapabilitySetReject_cause::e_unspecified))) {
    receivedCapabilites = TRUE;
    H323ControlPDU ack;
    ack.BuildTerminalCapabilitySetAck(inSequenceNumber);
    return connection.WriteControlPDU(ack);
  }

  connection.WriteControlPDU(reject);
  connection.ClearCall(H323Connection::EndedByCapabilityExchange);
  return TRUE;
}


BOOL H245NegTerminalCapabilitySet::HandleAck(const H245_TerminalCapabilitySetAck & pdu)
{
  replyTimer.Stop();
  PWaitAndSignal wait(mutex);

  PTRACE(3, "H245\tReceived TerminalCapabilitySetAck:"
                     " state=" << state <<
                     " pduSeq=" << pdu.m_sequenceNumber <<
                     " outSeq=" << (unsigned)outSequenceNumber);

  if (state != e_InProgress)
    return TRUE;

  if (pdu.m_sequenceNumber != outSequenceNumber)
    return TRUE;

  replyTimer.Stop();
  state = e_Sent;
  PTRACE(2, "H245\tTerminalCapabilitySet Sent.");
  return TRUE;
}


BOOL H245NegTerminalCapabilitySet::HandleReject(const H245_TerminalCapabilitySetReject & pdu)
{
  replyTimer.Stop();
  PWaitAndSignal wait(mutex);

  PTRACE(3, "H245\tReceived TerminalCapabilitySetReject:"
                     " state=" << state <<
                     " pduSeq=" << pdu.m_sequenceNumber <<
                     " outSeq=" << (unsigned)outSequenceNumber);

  if (state != e_InProgress)
    return TRUE;

  if (pdu.m_sequenceNumber != outSequenceNumber)
    return TRUE;

  state = e_Idle;
  replyTimer.Stop();
  return connection.OnControlProtocolError(H323Connection::e_CapabilityExchange,
                                           "Rejected");
}


BOOL H245NegTerminalCapabilitySet::HandleRelease(const H245_TerminalCapabilitySetRelease & /*pdu*/)
{
  replyTimer.Stop();
  PWaitAndSignal wait(mutex);

  PTRACE(3, "H245\tReceived TerminalCapabilityRelease: state=" << state);

  receivedCapabilites = FALSE;
  return connection.OnControlProtocolError(H323Connection::e_CapabilityExchange,
                                           "Aborted");
}


void H245NegTerminalCapabilitySet::HandleTimeout(PTimer &, INT)
{
  replyTimer.Stop();
  PWaitAndSignal wait(mutex);

  PTRACE(3, "H245\tTimeout on TerminalCapabilitySet: state=" << state);

  H323ControlPDU reply;
  reply.Build(H245_IndicationMessage::e_terminalCapabilitySetRelease);
  connection.WriteControlPDU(reply);

  connection.OnControlProtocolError(H323Connection::e_CapabilityExchange, "Timeout");
}


#if PTRACING
const char * const H245NegTerminalCapabilitySet::StateNames[] = {
  "Idle", "InProgress", "Sent"
};
#endif


/////////////////////////////////////////////////////////////////////////////

H245NegLogicalChannel::H245NegLogicalChannel(H323EndPoint & end,
                                             H323Connection & conn,
                                             const H323ChannelNumber & chanNum)
  : H245Negotiator(end, conn),
    channelNumber(chanNum)
{
  channel = NULL;
  state = e_Released;
}


H245NegLogicalChannel::H245NegLogicalChannel(H323EndPoint & end,
                                             H323Connection & conn,
                                             H323Channel & chan)
  : H245Negotiator(end, conn),
    channelNumber(chan.GetNumber())
{
  channel = &chan;
  state = e_Established;
}


H245NegLogicalChannel::~H245NegLogicalChannel()
{
  replyTimer.Stop();
  PThread::Yield(); // Do this to avoid possible race condition with timer

  mutex.Wait();
  delete channel;
  mutex.Signal();
}


BOOL H245NegLogicalChannel::Open(const H323Capability & capability,
                                 unsigned sessionID,
                                 unsigned replacementFor,
								 unsigned roleLabel
								 )
{
  PWaitAndSignal wait(mutex);
  return OpenWhileLocked(capability, sessionID, replacementFor,roleLabel);
}


BOOL H245NegLogicalChannel::OpenWhileLocked(const H323Capability & capability,
                                            unsigned sessionID,
                                            unsigned replacementFor,
											unsigned roleLabel
											)
{
  if (state != e_Released && state != e_AwaitingRelease) {
    PTRACE(3, "H245\tOpen of channel currently in negotiations: " << channelNumber);
    return FALSE;
  }

  PTRACE(3, "H245\tOpening channel: " << channelNumber);

  if (channel != NULL) {
    channel->CleanUpOnTermination();
    delete channel;
    channel = NULL;
  }

  state = e_AwaitingEstablishment;

  H323ControlPDU pdu;
  H245_OpenLogicalChannel & open = pdu.BuildOpenLogicalChannel(channelNumber);

  if (!capability.OnSendingPDU(open.m_forwardLogicalChannelParameters.m_dataType)) {
    PTRACE(3, "H245\tOpening channel: " << channelNumber
           << ", capability.OnSendingPDU() failed");
    return FALSE;
  }

  channel = capability.CreateChannel(connection, H323Channel::IsTransmitter, sessionID, NULL);
  if (channel == NULL) {
    PTRACE(3, "H245\tOpening channel: " << channelNumber
           << ", capability.CreateChannel() failed");
    return FALSE;
  }

  channel->SetNumber(channelNumber);

  if (!channel->OnSendingPDU(open)) {
    PTRACE(3, "H245\tOpening channel: " << channelNumber
           << ", channel->OnSendingPDU() failed");
    return FALSE;
  }

  if (replacementFor > 0) {
    if (open.HasOptionalField(H245_OpenLogicalChannel::e_reverseLogicalChannelParameters)) {
      open.m_reverseLogicalChannelParameters.IncludeOptionalField(H245_OpenLogicalChannel_reverseLogicalChannelParameters::e_replacementFor);
      open.m_reverseLogicalChannelParameters.m_replacementFor = replacementFor;
    }
    else {
      open.m_forwardLogicalChannelParameters.IncludeOptionalField(H245_OpenLogicalChannel_forwardLogicalChannelParameters::e_replacementFor);
      open.m_forwardLogicalChannelParameters.m_replacementFor = replacementFor;
    }
  }

#ifdef H323_H239
  if (sessionID == OpalMediaFormat::DefaultExtVideoSessionID) {  // extended Video Session
	open.IncludeOptionalField(H245_OpenLogicalChannel::e_genericInformation);
    H245_ArrayOf_GenericInformation & cape = open.m_genericInformation;

	  H245_GenericMessage * gcap = new H245_GenericMessage();
	  gcap->m_messageIdentifier = *(new H245_CapabilityIdentifier(H245_CapabilityIdentifier::e_standard));
	  PASN_ObjectId &object_id = gcap->m_messageIdentifier;
      object_id = OpalPluginCodec_Identifer_H239_Video;   // Indicates H239 (Extended Video)

	  gcap->IncludeOptionalField(H245_GenericMessage::e_messageContent);
		H245_ArrayOf_GenericParameter & params = gcap->m_messageContent;
			H245_GenericParameter * content = new H245_GenericParameter();
			H245_ParameterIdentifier & paramid = content->m_parameterIdentifier;
				paramid.SetTag(H245_ParameterIdentifier::e_standard);
				PASN_Integer & pid = paramid;
				pid.SetValue(1);

				H245_ParameterValue & paramval = content->m_parameterValue;
				paramval.SetTag(H245_ParameterValue::e_booleanArray);
				PASN_Integer & val = paramval;
				val.SetValue(roleLabel);
		params.Append(content);
		params.SetSize(params.GetSize()+1);
      cape.Append(gcap);
      cape.SetSize(cape.GetSize()+1);
  }
#endif

  if (!channel->Open())
    return FALSE;

  if (!channel->SetInitialBandwidth()) {
    PTRACE(3, "H245\tOpening channel: " << channelNumber << ", Insufficient bandwidth");
    return FALSE;
  }

  replyTimer = endpoint.GetLogicalChannelTimeout();

  return connection.WriteControlPDU(pdu);
}


BOOL H245NegLogicalChannel::Close()
{
  PWaitAndSignal wait(mutex);
  return CloseWhileLocked();
}


BOOL H245NegLogicalChannel::CloseWhileLocked()
{
  PTRACE(3, "H245\tClosing channel: " << channelNumber << ", state=" << state);

  if (state != e_AwaitingEstablishment && state != e_Established)
    return TRUE;

  replyTimer = endpoint.GetLogicalChannelTimeout();

  H323ControlPDU reply;

  if (channelNumber.IsFromRemote()) {
    reply.BuildRequestChannelClose(channelNumber, H245_RequestChannelClose_reason::e_normal);
    state = e_AwaitingResponse;
  }
  else {
    reply.BuildCloseLogicalChannel(channelNumber);
    state = e_AwaitingRelease;
  }

  return connection.WriteControlPDU(reply);
}


BOOL H245NegLogicalChannel::HandleOpen(const H245_OpenLogicalChannel & pdu)
{
  PTRACE(3, "H245\tReceived open channel: " << channelNumber << ", state=" << state);

  if (channel != NULL) {
    channel->CleanUpOnTermination();
    delete channel;
    channel = NULL;
  }

  state = e_AwaitingEstablishment;

  H323ControlPDU reply;
  H245_OpenLogicalChannelAck & ack = reply.BuildOpenLogicalChannelAck(channelNumber);

  BOOL ok = FALSE;

  unsigned cause = H245_OpenLogicalChannelReject_cause::e_unspecified;
  if (connection.OnOpenLogicalChannel(pdu, ack, cause))
    channel = connection.CreateLogicalChannel(pdu, FALSE, cause);

  if (channel != NULL) {
    channel->SetNumber(channelNumber);
    channel->OnSendOpenAck(pdu, ack);
    if (channel->GetDirection() == H323Channel::IsBidirectional) {
      state = e_AwaitingConfirmation;
      replyTimer = endpoint.GetLogicalChannelTimeout(); // T103
      ok = TRUE;
    }
    else {
      ok = channel->Start();
      if (!ok) {
        // The correct protocol thing to do is reject the channel if we are
        // the master. However NetMeeting will not then reopen a channel, so
        // we act like we are a slave and close our end instead.
        if (connection.IsH245Master() &&
            connection.GetRemoteApplication().Find("NetMeeting") == P_MAX_INDEX)
          cause = H245_OpenLogicalChannelReject_cause::e_masterSlaveConflict;
        else {
          connection.OnConflictingLogicalChannel(*channel);
          ok = channel->Start();
        }
      }

      if (ok)
        state = e_Established;
    }
  }

  if (ok)
    mutex.Signal();
  else {
    reply.BuildOpenLogicalChannelReject(channelNumber, cause);
    Release();
  }

  return connection.WriteControlPDU(reply);
}


BOOL H245NegLogicalChannel::HandleOpenAck(const H245_OpenLogicalChannelAck & pdu)
{
  replyTimer.Stop();
  PWaitAndSignal wait(mutex);

  PTRACE(3, "H245\tReceived open channel ack: " << channelNumber << ", state=" << state);

  switch (state) {
    case e_Released :
      return connection.OnControlProtocolError(H323Connection::e_LogicalChannel,
                                               "Ack unknown channel");
    case e_AwaitingEstablishment :
      state = e_Established;
      replyTimer.Stop();

      if (!channel->OnReceivedAckPDU(pdu))
        return CloseWhileLocked();

      if (channel->GetDirection() == H323Channel::IsBidirectional) {
        H323ControlPDU reply;
        reply.BuildOpenLogicalChannelConfirm(channelNumber);
        if (!connection.WriteControlPDU(reply))
          return FALSE;
      }

      // Channel was already opened when OLC sent, if have error here it is
      // somthing other than an asymmetric codec conflict, so close it down.
      if (!channel->Start())
        return CloseWhileLocked();

    default :
      break;
  }

  return TRUE;
}


BOOL H245NegLogicalChannel::HandleOpenConfirm(const H245_OpenLogicalChannelConfirm & /*pdu*/)
{
  replyTimer.Stop();
  PWaitAndSignal wait(mutex);

  PTRACE(3, "H245\tReceived open channel confirm: " << channelNumber << ", state=" << state);

  switch (state) {
    case e_Released :
      return connection.OnControlProtocolError(H323Connection::e_LogicalChannel,
                                               "Confirm unknown channel");
    case e_AwaitingEstablishment :
      return connection.OnControlProtocolError(H323Connection::e_LogicalChannel,
                                               "Confirm established channel");
    case e_AwaitingConfirmation :
      replyTimer.Stop();
      state = e_Established;
      // Channel was already opened when OLC sent, if have error here it is
      // somthing other than an asymmetric codec conflict, so close it down.
      if (!channel->Start())
        return CloseWhileLocked();

    default :
      break;
  }

  return TRUE;
}


BOOL H245NegLogicalChannel::HandleReject(const H245_OpenLogicalChannelReject & pdu)
{
  replyTimer.Stop();
  mutex.Wait();

  PTRACE(3, "H245\tReceived open channel reject: " << channelNumber << ", state=" << state);

  switch (state) {
    case e_Released :
      mutex.Signal();
      return connection.OnControlProtocolError(H323Connection::e_LogicalChannel,
                                               "Reject unknown channel");
    case e_Established :
      Release();
      return connection.OnControlProtocolError(H323Connection::e_LogicalChannel,
                                               "Reject established channel");
    case e_AwaitingEstablishment :
      // Master rejected our attempt to open, so try something else.
      if (pdu.m_cause.GetTag() == H245_OpenLogicalChannelReject_cause::e_masterSlaveConflict)
        connection.OnConflictingLogicalChannel(*channel);
      // Do next case

    case e_AwaitingRelease :
      Release();
      break;

    default :
      mutex.Signal();
      break;
  }

  return TRUE;
}


BOOL H245NegLogicalChannel::HandleClose(const H245_CloseLogicalChannel & /*pdu*/)
{
  replyTimer.Stop();
  mutex.Wait();

  PTRACE(3, "H245\tReceived close channel: " << channelNumber << ", state=" << state);

  //if (pdu.m_source.GetTag() == H245_CloseLogicalChannel_source::e_user)

  H323ControlPDU reply;
  reply.BuildCloseLogicalChannelAck(channelNumber);

  Release();

  return connection.WriteControlPDU(reply);
}


BOOL H245NegLogicalChannel::HandleCloseAck(const H245_CloseLogicalChannelAck & /*pdu*/)
{
  replyTimer.Stop();
  mutex.Wait();

  PTRACE(3, "H245\tReceived close channel ack: " << channelNumber << ", state=" << state);

  switch (state) {
    case e_Established :
      Release();
      return connection.OnControlProtocolError(H323Connection::e_LogicalChannel,
                                               "Close ack open channel");
    case e_AwaitingRelease :
      Release();
      break;

    default :
      mutex.Signal();
      break;
  }

  return TRUE;
}


BOOL H245NegLogicalChannel::HandleRequestClose(const H245_RequestChannelClose & pdu)
{
  replyTimer.Stop();
  PWaitAndSignal wait(mutex);

  PTRACE(3, "H245\tReceived request close channel: " << channelNumber << ", state=" << state);

  if (state != e_Established)
    return TRUE;    // Already closed

  H323ControlPDU reply;
  if (connection.OnClosingLogicalChannel(*channel)) {
    reply.BuildRequestChannelCloseAck(channelNumber);
    if (!connection.WriteControlPDU(reply))
      return FALSE;

    // Do normal Close procedure
    replyTimer = endpoint.GetLogicalChannelTimeout();
    reply.BuildCloseLogicalChannel(channelNumber);
    state = e_AwaitingRelease;

    if (pdu.m_reason.GetTag() == H245_RequestChannelClose_reason::e_reopen) {
      PTRACE(2, "H245\tReopening channel: " << channelNumber);
      connection.OpenLogicalChannel(channel->GetCapability(),
                                    channel->GetSessionID(),
                                    channel->GetDirection());
    }
  }
  else
    reply.BuildRequestChannelCloseReject(channelNumber);

  return connection.WriteControlPDU(reply);
}


BOOL H245NegLogicalChannel::HandleRequestCloseAck(const H245_RequestChannelCloseAck & /*pdu*/)
{
  replyTimer.Stop();
  mutex.Wait();

  PTRACE(3, "H245\tReceived request close ack channel: " << channelNumber << ", state=" << state);

  if (state == e_AwaitingResponse)
    Release();  // Other end says close OK, so do so.
  else
    mutex.Signal();

  return TRUE;
}


BOOL H245NegLogicalChannel::HandleRequestCloseReject(const H245_RequestChannelCloseReject & /*pdu*/)
{
  replyTimer.Stop();
  PWaitAndSignal wait(mutex);

  PTRACE(3, "H245\tReceived request close reject channel: " << channelNumber << ", state=" << state);

  // Other end refused close, so go back to still having channel open
  if (state == e_AwaitingResponse)
    state = e_Established;

  return TRUE;
}


BOOL H245NegLogicalChannel::HandleRequestCloseRelease(const H245_RequestChannelCloseRelease & /*pdu*/)
{
  replyTimer.Stop();
  PWaitAndSignal wait(mutex);

  PTRACE(3, "H245\tReceived request close release channel: " << channelNumber << ", state=" << state);

  // Other end refused close, so go back to still having channel open
  state = e_Established;

  return TRUE;
}


void H245NegLogicalChannel::HandleTimeout(PTimer &, INT)
{
  mutex.Wait();

  PTRACE(3, "H245\tTimeout on open channel: " << channelNumber << ", state=" << state);

  H323ControlPDU reply;
  switch (state) {
    case e_AwaitingEstablishment :
      reply.BuildCloseLogicalChannel(channelNumber);
      connection.WriteControlPDU(reply);
      break;

    case e_AwaitingResponse :
      reply.BuildRequestChannelCloseRelease(channelNumber);
      connection.WriteControlPDU(reply);
      break;

    case e_Released :
      mutex.Signal();
      return;

    default :
      break;
  }

  Release();
  connection.OnControlProtocolError(H323Connection::e_LogicalChannel, "Timeout");
}


void H245NegLogicalChannel::Release()
{
  state = e_Released;
  H323Channel * chan = channel;
  channel = NULL;
  mutex.Signal();

  replyTimer.Stop();

  if (chan != NULL) {
    chan->CleanUpOnTermination();
    delete chan;
  }
}


H323Channel * H245NegLogicalChannel::GetChannel()
{
  return channel;
}


#if PTRACING
const char * const H245NegLogicalChannel::StateNames[] = {
  "Released",
  "AwaitingEstablishment",
  "Established",
  "AwaitingRelease",
  "AwatingConfirmation",
  "AwaitingResponse"
};
#endif


/////////////////////////////////////////////////////////////////////////////

H245NegLogicalChannels::H245NegLogicalChannels(H323EndPoint & end,
                                               H323Connection & conn)
  : H245Negotiator(end, conn),
    lastChannelNumber(100, FALSE)
{
}


void H245NegLogicalChannels::Add(H323Channel & channel)
{
  mutex.Wait();
  channels.SetAt(channel.GetNumber(), new H245NegLogicalChannel(endpoint, connection, channel));
  mutex.Signal();
}

BOOL H245NegLogicalChannels::Open(const H323Capability & capability,
                                  unsigned sessionID,
                                  unsigned replacementFor
								  )
{
     H323ChannelNumber channelnumber;
     return Open(capability,sessionID,channelnumber,replacementFor);
}


BOOL H245NegLogicalChannels::Open(const H323Capability & capability,
                                  unsigned sessionID,
                                  H323ChannelNumber & channelnumber,
                                  unsigned replacementFor,
								  unsigned roleLabel
								  )
{
  mutex.Wait();

  lastChannelNumber++;

  H245NegLogicalChannel * negChan = new H245NegLogicalChannel(endpoint, connection, lastChannelNumber);
  channels.SetAt(lastChannelNumber, negChan);

  channelnumber = lastChannelNumber;

  mutex.Signal();

  return negChan->Open(capability, sessionID, replacementFor,roleLabel);
}


BOOL H245NegLogicalChannels::Close(unsigned channelNumber, BOOL fromRemote)
{
  H245NegLogicalChannel * chan = FindNegLogicalChannel(channelNumber, fromRemote);
  if (chan != NULL)
    return chan->Close();

  return connection.OnControlProtocolError(H323Connection::e_LogicalChannel,
                                           "Close unknown");
}


BOOL H245NegLogicalChannels::HandleOpen(const H245_OpenLogicalChannel & pdu)
{
  H323ChannelNumber chanNum(pdu.m_forwardLogicalChannelNumber, TRUE);
  H245NegLogicalChannel * chan;

  mutex.Wait();

  if (channels.Contains(chanNum))
    chan = &channels[chanNum];
  else {
    chan = new H245NegLogicalChannel(endpoint, connection, chanNum);
    channels.SetAt(chanNum, chan);
  }

  chan->mutex.Wait();

  mutex.Signal();

  return chan->HandleOpen(pdu);
}


BOOL H245NegLogicalChannels::HandleOpenAck(const H245_OpenLogicalChannelAck & pdu)
{
  H245NegLogicalChannel * chan = FindNegLogicalChannel(pdu.m_forwardLogicalChannelNumber, FALSE);
  if (chan != NULL)
    return chan->HandleOpenAck(pdu);

  return connection.OnControlProtocolError(H323Connection::e_LogicalChannel,
                                           "Ack unknown");
}


BOOL H245NegLogicalChannels::HandleOpenConfirm(const H245_OpenLogicalChannelConfirm & pdu)
{
  H245NegLogicalChannel * chan = FindNegLogicalChannel(pdu.m_forwardLogicalChannelNumber, TRUE);
  if (chan != NULL)
    return chan->HandleOpenConfirm(pdu);

  return connection.OnControlProtocolError(H323Connection::e_LogicalChannel,
                                           "Confirm unknown");
}


BOOL H245NegLogicalChannels::HandleReject(const H245_OpenLogicalChannelReject & pdu)
{
  H245NegLogicalChannel * chan = FindNegLogicalChannel(pdu.m_forwardLogicalChannelNumber, FALSE);
  if (chan != NULL)
    return chan->HandleReject(pdu);

  return connection.OnControlProtocolError(H323Connection::e_LogicalChannel,
                                           "Reject unknown");
}


BOOL H245NegLogicalChannels::HandleClose(const H245_CloseLogicalChannel & pdu)
{
  H245NegLogicalChannel * chan = FindNegLogicalChannel(pdu.m_forwardLogicalChannelNumber, TRUE);
  if (chan != NULL)
    return chan->HandleClose(pdu);

  return connection.OnControlProtocolError(H323Connection::e_LogicalChannel,
                                           "Close unknown");
}


BOOL H245NegLogicalChannels::HandleCloseAck(const H245_CloseLogicalChannelAck & pdu)
{
  H245NegLogicalChannel * chan = FindNegLogicalChannel(pdu.m_forwardLogicalChannelNumber, FALSE);
  if (chan != NULL)
    return chan->HandleCloseAck(pdu);

  return connection.OnControlProtocolError(H323Connection::e_LogicalChannel,
                                           "Close Ack unknown");
}


BOOL H245NegLogicalChannels::HandleRequestClose(const H245_RequestChannelClose & pdu)
{
  H245NegLogicalChannel * chan = FindNegLogicalChannel(pdu.m_forwardLogicalChannelNumber, FALSE);
  if (chan != NULL)
    return chan->HandleRequestClose(pdu);

  return connection.OnControlProtocolError(H323Connection::e_LogicalChannel,
                                           "Request Close unknown");
}


BOOL H245NegLogicalChannels::HandleRequestCloseAck(const H245_RequestChannelCloseAck & pdu)
{
  H245NegLogicalChannel * chan = FindNegLogicalChannel(pdu.m_forwardLogicalChannelNumber, TRUE);
  if (chan != NULL)
    return chan->HandleRequestCloseAck(pdu);

  return connection.OnControlProtocolError(H323Connection::e_LogicalChannel,
                                           "Request Close Ack unknown");
}


BOOL H245NegLogicalChannels::HandleRequestCloseReject(const H245_RequestChannelCloseReject & pdu)
{
  H245NegLogicalChannel * chan = FindNegLogicalChannel(pdu.m_forwardLogicalChannelNumber, TRUE);
  if (chan != NULL)
    return chan->HandleRequestCloseReject(pdu);

  return connection.OnControlProtocolError(H323Connection::e_LogicalChannel,
                                           "Request Close Reject unknown");
}


BOOL H245NegLogicalChannels::HandleRequestCloseRelease(const H245_RequestChannelCloseRelease & pdu)
{
  H245NegLogicalChannel * chan = FindNegLogicalChannel(pdu.m_forwardLogicalChannelNumber, FALSE);
  if (chan != NULL)
    return chan->HandleRequestCloseRelease(pdu);

  return connection.OnControlProtocolError(H323Connection::e_LogicalChannel,
                                           "Request Close Release unknown");
}


H323ChannelNumber H245NegLogicalChannels::GetNextChannelNumber()
{
  PWaitAndSignal wait(mutex);
  lastChannelNumber++;
  return lastChannelNumber;
}


H323Channel * H245NegLogicalChannels::GetChannelAt(PINDEX i)
{
  mutex.Wait();
  H323Channel * chan =  channels.GetDataAt(i).GetChannel();
  mutex.Signal();
  return chan;
}


H323Channel * H245NegLogicalChannels::FindChannel(unsigned channelNumber,
                                                  BOOL fromRemote)
{
  PWaitAndSignal wait(mutex);

  H323ChannelNumber chanNum(channelNumber, fromRemote);

  if (channels.Contains(chanNum))
    return channels[chanNum].GetChannel();

  return NULL;
}


H245NegLogicalChannel & H245NegLogicalChannels::GetNegLogicalChannelAt(PINDEX i)
{
  PWaitAndSignal wait(mutex);
  return channels.GetDataAt(i);
}


H245NegLogicalChannel * H245NegLogicalChannels::FindNegLogicalChannel(unsigned channelNumber,
                                                                      BOOL fromRemote)
{
  H323ChannelNumber chanNum(channelNumber, fromRemote);

  mutex.Wait();
  H245NegLogicalChannel * channel = channels.GetAt(chanNum);
  mutex.Signal();

  return channel;
}


H323Channel * H245NegLogicalChannels::FindChannelBySession(unsigned rtpSessionId,
                                                           BOOL fromRemote)
{
  PWaitAndSignal wait(mutex);

  PINDEX i;
  H323Channel::Directions desiredDirection = fromRemote ? H323Channel::IsReceiver : H323Channel::IsTransmitter;
  for (i = 0; i < GetSize(); i++) {
    H323Channel * channel = channels.GetDataAt(i).GetChannel();
    if (channel != NULL && channel->GetSessionID() == rtpSessionId &&
                           channel->GetDirection() == desiredDirection)
      return channel;
  }

  return NULL;
}


void H245NegLogicalChannels::RemoveAll()
{
  PWaitAndSignal wait(mutex);

  for (PINDEX i = 0; i < channels.GetSize(); i++) {
    H245NegLogicalChannel & neg = channels.GetDataAt(i);
    neg.mutex.Wait();
    H323Channel * channel = neg.GetChannel();
    if (channel != NULL)
      channel->CleanUpOnTermination();
    neg.mutex.Signal();
  }

  channels.RemoveAll();
}


/////////////////////////////////////////////////////////////////////////////

H245NegRequestMode::H245NegRequestMode(H323EndPoint & end, H323Connection & conn)
  : H245Negotiator(end, conn)
{
  awaitingResponse = FALSE;
  inSequenceNumber = UINT_MAX;
  outSequenceNumber = 0;
}


BOOL H245NegRequestMode::StartRequest(const PString & newModes)
{
  PStringArray modes = newModes.Lines();
  if (modes.IsEmpty())
    return FALSE;

  H245_ArrayOf_ModeDescription descriptions;
  PINDEX modeCount = 0;

  const H323Capabilities & localCapabilities = connection.GetLocalCapabilities();
  
  PTRACE(1, "H245\tStartRequest begin");

  for (PINDEX i = 0; i < modes.GetSize(); i++) {
    H245_ModeDescription description;
    PINDEX count = 0;

    PStringArray caps = modes[i].Tokenise('\t');
    for (PINDEX j = 0; j < caps.GetSize(); j++) {
      H323Capability * capability = localCapabilities.FindCapability(caps[j]);
      if (capability != NULL) {
        description.SetSize(count+1);
        capability->OnSendingPDU(description[count]);
        count++;
      }
    }

    if (count > 0) {
      descriptions.SetSize(modeCount+1);
      descriptions[modeCount] = description;
      modeCount++;
    }
  }

  if (modeCount == 0)
    return FALSE;

  return StartRequest(descriptions);
}


BOOL H245NegRequestMode::StartRequest(const H245_ArrayOf_ModeDescription & newModes)
{
  PTRACE(1, "H245\tStarted request mode: outSeq=" << outSequenceNumber
         << (awaitingResponse ? " awaitingResponse" : " idle"));

  if (awaitingResponse)
    return FALSE;

  // Initiate a mode request
  outSequenceNumber = (outSequenceNumber+1)%256;
  replyTimer = endpoint.GetRequestModeTimeout();
  awaitingResponse = TRUE;

  H323ControlPDU pdu;
  H245_RequestMode & requestMode = pdu.BuildRequestMode(outSequenceNumber);
  requestMode.m_requestedModes = newModes;
  requestMode.m_requestedModes.SetConstraints(PASN_Object::FixedConstraint, 1, 256);

  return connection.WriteControlPDU(pdu);
}


BOOL H245NegRequestMode::HandleRequest(const H245_RequestMode & pdu)
{
  replyTimer.Stop();
  inSequenceNumber = pdu.m_sequenceNumber;

  PTRACE(3, "H245\tReceived request mode: inSeq=" << inSequenceNumber);

  H323ControlPDU reply_ack;
  H245_RequestModeAck & ack = reply_ack.BuildRequestModeAck(inSequenceNumber,
                  H245_RequestModeAck_response::e_willTransmitMostPreferredMode);

  H323ControlPDU reply_reject;
  H245_RequestModeReject & reject = reply_reject.BuildRequestModeReject(inSequenceNumber,
                                        H245_RequestModeReject_cause::e_modeUnavailable);

  PINDEX selectedMode = 0;
  if (!connection.OnRequestModeChange(pdu, ack, reject, selectedMode))
    return connection.WriteControlPDU(reply_reject);

  if (selectedMode != 0)
    ack.m_response.SetTag(H245_RequestModeAck_response::e_willTransmitLessPreferredMode);

  if (!connection.WriteControlPDU(reply_ack))
    return FALSE;

  connection.OnModeChanged(pdu.m_requestedModes[selectedMode]);
  return TRUE;
}


BOOL H245NegRequestMode::HandleAck(const H245_RequestModeAck & pdu)
{
  replyTimer.Stop();
  PTRACE(3, "H245\tReceived ack on request mode: outSeq=" << outSequenceNumber
         << (awaitingResponse ? " awaitingResponse" : " idle"));

  if (awaitingResponse && pdu.m_sequenceNumber == outSequenceNumber) {
    awaitingResponse = FALSE;
    replyTimer.Stop();
    connection.OnAcceptModeChange(pdu);
  }

  return TRUE;
}

BOOL H245NegRequestMode::HandleReject(const H245_RequestModeReject & pdu)
{
  replyTimer.Stop();
  PTRACE(3, "H245\tReceived reject on request mode: outSeq=" << outSequenceNumber
         << (awaitingResponse ? " awaitingResponse" : " idle"));

  if (awaitingResponse && pdu.m_sequenceNumber == outSequenceNumber) {
    awaitingResponse = FALSE;
    replyTimer.Stop();
    connection.OnRefusedModeChange(&pdu);
  }

  return TRUE;
}


BOOL H245NegRequestMode::HandleRelease(const H245_RequestModeRelease & /*pdu*/)
{
  replyTimer.Stop();
  PTRACE(3, "H245\tReceived release on request mode: inSeq=" << inSequenceNumber);
  return TRUE;
}


void H245NegRequestMode::HandleTimeout(PTimer &, INT)
{
  PTRACE(3, "H245\tTimeout on request mode: outSeq=" << outSequenceNumber
         << (awaitingResponse ? " awaitingResponse" : " idle"));

  if (awaitingResponse) {
    awaitingResponse = FALSE;
    H323ControlPDU pdu;
    pdu.Build(H245_IndicationMessage::e_requestModeRelease);
    connection.WriteControlPDU(pdu);
    connection.OnRefusedModeChange(NULL);
  }

  connection.OnControlProtocolError(H323Connection::e_ModeRequest, "Timeout");
}


/////////////////////////////////////////////////////////////////////////////

H245NegRoundTripDelay::H245NegRoundTripDelay(H323EndPoint & end, H323Connection & conn)
  : H245Negotiator(end, conn)
{
  awaitingResponse = FALSE;
  sequenceNumber = 0;

  // Temporary (ie quick) fix for strange Cisco behaviour. If keep trying to
  // do this it stops sending RTP audio data!!
  retryCount = 1;
}


BOOL H245NegRoundTripDelay::StartRequest()
{
  PWaitAndSignal wait(mutex);

  replyTimer = endpoint.GetRoundTripDelayTimeout();
  sequenceNumber = (sequenceNumber + 1)%256;
  awaitingResponse = TRUE;

  PTRACE(3, "H245\tStarted round trip delay: seq=" << sequenceNumber
         << (awaitingResponse ? " awaitingResponse" : " idle"));

  H323ControlPDU pdu;
  pdu.BuildRoundTripDelayRequest(sequenceNumber);
  if (!connection.WriteControlPDU(pdu))
    return FALSE;

  tripStartTime = PTimer::Tick();
  return TRUE;
}


BOOL H245NegRoundTripDelay::HandleRequest(const H245_RoundTripDelayRequest & pdu)
{
  PWaitAndSignal wait(mutex);

  PTRACE(3, "H245\tStarted round trip delay: seq=" << sequenceNumber
         << (awaitingResponse ? " awaitingResponse" : " idle"));

  H323ControlPDU reply;
  reply.BuildRoundTripDelayResponse(pdu.m_sequenceNumber);
  return connection.WriteControlPDU(reply);
}


BOOL H245NegRoundTripDelay::HandleResponse(const H245_RoundTripDelayResponse & pdu)
{
  replyTimer.Stop();
  PWaitAndSignal wait(mutex);

  PTimeInterval tripEndTime = PTimer::Tick();

  PTRACE(3, "H245\tHandling round trip delay: seq=" << sequenceNumber
         << (awaitingResponse ? " awaitingResponse" : " idle"));

  if (awaitingResponse && pdu.m_sequenceNumber == sequenceNumber) {
    replyTimer.Stop();
    awaitingResponse = FALSE;
    roundTripTime = tripEndTime - tripStartTime;
    retryCount = 3;
  }

  return TRUE;
}


void H245NegRoundTripDelay::HandleTimeout(PTimer &, INT)
{
  PWaitAndSignal wait(mutex);

  PTRACE(3, "H245\tTimeout on round trip delay: seq=" << sequenceNumber
         << (awaitingResponse ? " awaitingResponse" : " idle"));

  if (awaitingResponse && retryCount > 0)
    retryCount--;
  awaitingResponse = FALSE;

  connection.OnControlProtocolError(H323Connection::e_RoundTripDelay, "Timeout");
}


/////////////////////////////////////////////////////////////////////////////
