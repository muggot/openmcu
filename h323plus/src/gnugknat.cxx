/*
 * gnugknat.cxx
 *
 * GnuGk NAT Traversal class.
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
 * $Log: gnugknat.cxx,v $
 * Revision 1.8  2008/02/01 09:34:20  shorne
 * Cleaner shutdown of GnuGk NAT support
 *
 * Revision 1.7  2008/02/01 07:50:17  shorne
 * added shutdown mutex to fix occasion shutdown timing errors.
 *
 * Revision 1.6  2008/01/30 02:53:42  shorne
 * code format tidy up
 *
 * Revision 1.5  2008/01/18 01:36:43  shorne
 * Fix blocking and timeout on call ending
 *
 * Revision 1.4  2008/01/15 02:25:34  shorne
 * readbuffer not being emptied after each read.
 *
 * Revision 1.3  2008/01/02 20:43:05  shorne
 * Fix compile warning on Linux
 *
 * Revision 1.2  2008/01/02 20:10:28  willamowius
 * fix initialization order for gcc
 *
 * Revision 1.1  2008/01/01 00:16:12  shorne
 * Added GnuGknat and FileTransfer support
 *
 *
 *
 */

#include <ptlib.h>
#include <h323pdu.h>

#ifdef H323_GNUGK

#include "gnugknat.h"

#if defined(_WIN32) && !defined(P_FORCE_STATIC_PLUGIN)
#error "gnugknat.cxx must be compiled without precompiled headers"
#endif

PCREATE_NAT_PLUGIN(GnuGk);


WORD GNUGK_Feature::keepalive = 10;

GNUGKTransport * GNUGK_Feature::curtransport = NULL;
BOOL GNUGK_Feature::connectionlost = FALSE;

///////////////////////////////////////////////////////////////////////////////////

// Listening/Keep Alive Thread

class GNUGKTransportThread : public PThread
{
   PCLASSINFO(GNUGKTransportThread, PThread)

   public:
	GNUGKTransportThread(H323EndPoint & endpoint, GNUGKTransport * transport, WORD KeepAlive =0);

   protected:
	void Main();
	PDECLARE_NOTIFIER(PTimer, GNUGKTransportThread, Ping);	/// Timer to notify to poll for External IP
	PTimer	Keep;						/// Polling Timer													
	BOOL    isConnected;
	GNUGKTransport * transport;
	WORD   keepAlive;

	PTime   lastupdate;

};

/////////////////////////////////////////////////////////////////////////////

GNUGKTransportThread::GNUGKTransportThread(H323EndPoint & ep, GNUGKTransport * t, WORD KeepAlive)
  : PThread(ep.GetSignallingThreadStackSize(),
            AutoDeleteThread,
            NormalPriority,
            "H225 Answer:%0x"),
			 transport(t)
{  

   isConnected = FALSE;
   keepAlive = KeepAlive;

   if (keepAlive > 0 ) {
      // Send the first PDU.
      transport->InitialPDU();

      Keep.SetNotifier(PCREATE_NOTIFIER(Ping));
      Keep.RunContinuous(keepAlive * 1000); 
   }

// Start the Thread
   Resume();
}

void GNUGKTransportThread::Ping(PTimer &, INT)
{ 

   // Fix for some PC's that fail on the runcontinuously time interval.
   PSyncPoint handlewait;

   PTime curTime = PTime();
   if ((curTime - lastupdate) < PTimeInterval(GNUGK_Feature::keepalive * 1000))
	handlewait.Wait(lastupdate + PTimeInterval(GNUGK_Feature::keepalive * 1000) - curTime);
	   

   if (transport->isCall() ||		/// We have call or we are closing down
       transport->CloseTransport()) 
	Keep.Stop();
   else					/// Stop what we are doing 
	transport->InitialPDU();

   lastupdate = PTime();
}

void GNUGKTransportThread::Main()
{
  PTRACE(3, "GNUGK\tStarted Listening-KeepAlive Thread");

  BOOL ret = TRUE;
  while ((transport->IsOpen()) &&		// Transport is Open
	(!isConnected) &&			// Does not have a call connection 
	(ret) &&	                        // is not a Failed connection
	(!transport->CloseTransport())) {	// not close due to shutdown
	  ret = transport->HandleGNUGKSignallingChannelPDU();

	  if (!ret && transport->CloseTransport()) {  // Closing down Instruction
            PTRACE(3, "GNUGK\tShutting down GnuGk Thread");
            GNUGK_Feature::curtransport = NULL;
            transport->ConnectionLost(TRUE);

	  } else if (!ret) {   // We have a socket failure wait 1 sec and try again.
             PTRACE(3, "GNUGK\tConnection Lost! Retrying Connection..");
	     transport->ConnectionLost(TRUE);
             while (!transport->CloseTransport() && 
                           !transport->Connect()) {
                PTRACE(3, "GNUGK\tReconnect Failed! Waiting 1 sec");
                PProcess::Sleep(1000);
             }

	    if (!transport->CloseTransport()) {
              PTRACE(3, "GNUGK\tConnection ReEstablished");
	      transport->ConnectionLost(FALSE);
              ret = TRUE;			// Signal that the connection has been ReEstablished.
	    }
	  } else {				// We are connected to a call on this thread 
              isConnected = TRUE;
	  } 
  }

  PTRACE(3, "GNUGK\tTransport Closed");
}

///////////////////////////////////////////////////////////////////////////////////////

GNUGKTransport::GNUGKTransport(H323EndPoint & endpoint,
				GNUGK_Feature * feat,
				PString & gkid  
				)	
   : H323TransportTCP(endpoint), GKid(gkid), Feature(feat)
{
	GNUGK_Feature::curtransport = this;
	ReadTimeOut = PMaxTimeInterval;
	isConnected = FALSE;
	closeTransport = FALSE;
	remoteShutDown = FALSE;
}

GNUGKTransport::~GNUGKTransport()
{
	Close();
}

BOOL GNUGKTransport::HandleGNUGKSignallingSocket(H323SignalPDU & pdu)
{
  for (;;) {

	  if (!IsOpen())
		  return FALSE;

	  H323SignalPDU rpdu;
	  if (!rpdu.Read(*this)) { 
            PTRACE(3, "GNUGK\tSocket Read Failure");
            if (GetErrorNumber(PChannel::LastReadError) == 0) {
              PTRACE(3, "GNUGK\tRemote SHUT DOWN or Intermediary Shutdown!");
              remoteShutDown = TRUE;
            }
            return FALSE;
	  } else if ((rpdu.GetQ931().GetMessageType() == Q931::InformationMsg) &&
                          (endpoint.HandleUnsolicitedInformation(rpdu))) {
              // Handle unsolicited Information Message
	  } else if (rpdu.GetQ931().GetMessageType() == Q931::SetupMsg) {
              pdu = rpdu;
              return TRUE;
	  } else {
	     PTRACE(3, "GNUGK\tUnknown PDU Received");
             return FALSE;
	  }

  }
}

BOOL GNUGKTransport::HandleGNUGKSignallingChannelPDU()
{

  H323SignalPDU pdu;
  if (!HandleGNUGKSignallingSocket(pdu)) {
    if (remoteShutDown) {   // Intentional Shutdown?
      GNUGK_Feature::curtransport = NULL;
      Close();
    }
    return FALSE;
  }

    // Create a new transport to the GK as this one will be closed at the end of the call.
      isConnected = TRUE;
      GNUGK_Feature::curtransport = NULL;
	  CreateNewTransport();

	// Process the Tokens
	  unsigned callReference = pdu.GetQ931().GetCallReference();
	  PString token = endpoint.BuildConnectionToken(*this, callReference, TRUE);

	  H323Connection * connection = endpoint.CreateConnection(callReference, NULL, this, &pdu);
		if (connection == NULL) {
			PTRACE(1, "GNUGK\tEndpoint could not create connection, " <<
					  "sending release complete PDU: callRef=" << callReference);
			Q931 pdu;
			pdu.BuildReleaseComplete(callReference, TRUE);
			PBYTEArray rawData;
			pdu.Encode(rawData);
			WritePDU(rawData);
			return TRUE;
		}

		PTRACE(3, "GNUGK\tCreated new connection: " << token);
		connectionsMutex.Wait();
		GetEndPoint().GetConnections().SetAt(token, connection);
		connectionsMutex.Signal();

		connection->AttachSignalChannel(token, this, TRUE);
 
		 PThread * thread = PThread::Current();
		 AttachThread(thread);
		 thread->SetNoAutoDelete();

		 if (connection->HandleSignalPDU(pdu)) {
			// All subsequent PDU's should wait forever
			SetReadTimeout(PMaxTimeInterval);
			connection->HandleSignallingChannel();
		 }
		 else {
			connection->ClearCall(H323Connection::EndedByTransportFail);
			PTRACE(1, "GNUGK\tSignal channel stopped on first PDU.");
		 }

  return TRUE;
}


BOOL GNUGKTransport::WritePDU( const PBYTEArray & pdu )
{
	PWaitAndSignal m(WriteMutex);
	return H323TransportTCP::WritePDU(pdu);

}
	
BOOL GNUGKTransport::ReadPDU(PBYTEArray & pdu)
{
	return H323TransportTCP::ReadPDU(pdu);
}

BOOL GNUGKTransport::Connect() 
{ 
        PTRACE(4, "GNUGK\tConnecting to GK"  );
	if (!H323TransportTCP::Connect())
		return FALSE;
	
	return InitialPDU();
}

void GNUGKTransport::ConnectionLost(BOOL established)
{
	PWaitAndSignal m(shutdownMutex);

	if (closeTransport)
		return;
         PTRACE(4,"GnuGK\tConnection lost " << established 
              << " have " << GNUGK_Feature::connectionlost);
	if (GNUGK_Feature::connectionlost != established) {
	   GetEndPoint().NATLostConnection(established);
	   GNUGK_Feature::connectionlost = established;
	}
}

BOOL GNUGKTransport::IsConnectionLost()  
{ 
	return GNUGK_Feature::connectionlost; 
}


BOOL GNUGKTransport::InitialPDU()
{
  PWaitAndSignal mutex(IntMutex);

  if (!IsOpen())
	  return FALSE;

 PBYTEArray bytes(GKid,GKid.GetLength(), false);

   Q931 qPDU;
   qPDU.BuildInformation(0,false);
   qPDU.SetCallState(Q931::CallState_IncomingCallProceeding);
   qPDU.SetIE(Q931::FacilityIE, bytes);
 

  PBYTEArray rawData;
  if (!qPDU.Encode(rawData)) {
	PTRACE(4, "GNUGK\tError Encoding PDU.");
    return FALSE;
  }

  if (!WritePDU(rawData)) {
	PTRACE(4, "GNUGK\tError Writing PDU.");
	 return FALSE;
  }

  PTRACE(6, "GNUGK\tSent KeepAlive PDU.");

 return TRUE;
}

BOOL GNUGKTransport::SetGKID(const PString & newid)
{
	if (GKid != newid) {
	   GKid = newid;
	   return TRUE;
	}
	return FALSE;
}

BOOL GNUGKTransport::CreateNewTransport()
{

	GNUGKTransport * transport = new GNUGKTransport(GetEndPoint(),Feature,GKid);
	H323TransportAddress remote = GetRemoteAddress();
	transport->SetRemoteAddress(remote);

	if (transport->Connect()) {
          PTRACE(3, "GNUGK\tConnected to " << transport->GetRemoteAddress());
	    new GNUGKTransportThread(transport->GetEndPoint(), transport,GNUGK_Feature::keepalive);
		if (transport->IsConnectionLost())
		     transport->ConnectionLost(FALSE);
		return TRUE;
	}
	return FALSE;
}

BOOL GNUGKTransport::Close() 
{ 
   PWaitAndSignal m(shutdownMutex);

   PTRACE(4, "GNUGK\tClosing GnuGK NAT channel.");	
   closeTransport = TRUE;
   return H323TransportTCP::Close(); 
}

BOOL GNUGKTransport::IsOpen () const
{
   return H323TransportTCP::IsOpen();
}

BOOL GNUGKTransport::IsListening() const
{	  
  if (isConnected)
    return FALSE;

  if (h245listener == NULL)
    return FALSE;

  if (GNUGK_Feature::connectionlost)
    return FALSE;

  return h245listener->IsOpen();
}

/////////////////////////////////////////////////////////////////////////////

GNUGK_Feature::GNUGK_Feature(H323EndPoint & EP, 
							 H323TransportAddress & remoteAddress, 
							 PString gkid,
							 WORD KeepAlive )
     :  ep(EP), address(remoteAddress), GKid(gkid)
{
	PTRACE(4, "GNUGK\tCreating GNUGK Feature.");	
	keepalive = KeepAlive;
	open = CreateNewTransport();
}

GNUGK_Feature::~GNUGK_Feature()
{
	if (curtransport != NULL)
		curtransport->Close();
}

BOOL GNUGK_Feature::CreateNewTransport()
{
	PTRACE(5, "GNUGK\tCreating Transport.");

	GNUGKTransport * transport = new GNUGKTransport(ep,this,GKid);
	transport->SetRemoteAddress(address);

	if (transport->Connect()) {
	 PTRACE(3, "GNUGK\tConnected to " << transport->GetRemoteAddress());
	    new GNUGKTransportThread(transport->GetEndPoint(), transport,keepalive);
		return TRUE;
	}

	 PTRACE(3, "GNUGK\tTransport Failure " << transport->GetRemoteAddress());
	return FALSE;
}

BOOL GNUGK_Feature::ReRegister(const PString & newid)
{
  // If there is a change in the gatekeeper id then notify the update socket
	if ((GNUGK_Feature::curtransport != NULL) && curtransport->SetGKID(newid))
	             return curtransport->InitialPDU();       // Send on existing Transport

   return FALSE;

}

///////////////////////////////////////////////////////////////////////////////////////////
	
PNatMethod_GnuGk::PNatMethod_GnuGk()
{

}

PNatMethod_GnuGk::~PNatMethod_GnuGk()
{

}


void PNatMethod_GnuGk::AttachEndPoint(H323EndPoint * ep)
{

   WORD portPairBase = ep->GetRtpIpPortBase();
   WORD portPairMax = ep->GetRtpIpPortMax();

// Initialise
//  ExternalAddress = 0;
  pairedPortInfo.basePort = 0;
  pairedPortInfo.maxPort = 0;
  pairedPortInfo.currentPort = 0;

// Set the Port Pair Information
  pairedPortInfo.mutex.Wait();

  pairedPortInfo.basePort = (WORD)((portPairBase+1)&0xfffe);
  if (portPairBase == 0) {
    pairedPortInfo.basePort = 0;
    pairedPortInfo.maxPort = 0;
  }
  else if (portPairMax == 0)
    pairedPortInfo.maxPort = (WORD)(pairedPortInfo.basePort+99);
  else if (portPairMax < portPairBase)
    pairedPortInfo.maxPort = portPairBase;
  else
    pairedPortInfo.maxPort = portPairMax;

  pairedPortInfo.currentPort = pairedPortInfo.basePort;

  pairedPortInfo.mutex.Signal();

	available = FALSE;
}

BOOL PNatMethod_GnuGk::GetExternalAddress(
      PIPSocket::Address & /*externalAddress*/, /// External address of router
      const PTimeInterval & /* maxAge */         /// Maximum age for caching
	  )
{
	return FALSE;
}


BOOL PNatMethod_GnuGk::CreateSocketPair(
							PUDPSocket * & socket1,
							PUDPSocket * & socket2,
							const PIPSocket::Address & /*binding*/
							)
{

	  if (pairedPortInfo.basePort == 0 || pairedPortInfo.basePort > pairedPortInfo.maxPort)
	  {
		PTRACE(1, "GNUGK\tInvalid local UDP port range "
			   << pairedPortInfo.currentPort << '-' << pairedPortInfo.maxPort);
		return FALSE;
	  }

    socket1 = new GNUGKUDPSocket();  /// Data 
    socket2 = new GNUGKUDPSocket();  /// Signal

/// Make sure we have sequential ports
	while ((!OpenSocket(*socket1, pairedPortInfo)) ||
		   (!OpenSocket(*socket2, pairedPortInfo)) ||
		   (socket2->GetPort() != socket1->GetPort() + 1) )
	{
			delete socket1;
			delete socket2;
			socket1 = new GNUGKUDPSocket();  /// Data 
			socket2 = new GNUGKUDPSocket();  /// Signal
	}

		PTRACE(5, "GNUGK\tUDP ports "
			   << socket1->GetPort() << '-' << socket2->GetPort());

    return TRUE;
}

BOOL PNatMethod_GnuGk::OpenSocket(PUDPSocket & socket, PortInfo & portInfo) const
{
  PWaitAndSignal mutex(portInfo.mutex);

  WORD startPort = portInfo.currentPort;

  do {
    portInfo.currentPort++;
    if (portInfo.currentPort > portInfo.maxPort)
      portInfo.currentPort = portInfo.basePort;

    if (socket.Listen(1, portInfo.currentPort)) {
      socket.SetReadTimeout(500);
      return TRUE;
    }

  } while (portInfo.currentPort != startPort);

  PTRACE(2, "GNUGK\tFailed to bind to local UDP port in range "
         << portInfo.currentPort << '-' << portInfo.maxPort);
  return FALSE;
}

/////////////////////////////////////////////////////////////////////////////////////////////

GNUGKUDPSocket::GNUGKUDPSocket()
{
}


GNUGKUDPSocket::~GNUGKUDPSocket()
{
}

void GNUGKUDPSocket::SetSendAddress(const Address & address,WORD port)
{

  sendAddress = address;
  sendPort    = port;
/*
	    PString ping = "ping";
		PBYTEArray bytes(ping,ping.GetLength(), false); 
		if (PIPDatagramSocket::WriteTo(bytes, ping.GetLength(), sendAddress, sendPort))
			PTRACE(4, "GNUGK\tUDP socket pinged " << sendAddress << '-' << sendPort << " from " << GetPort()); 
		else
			PTRACE(4, "GNUGK\tUDP socket no ping " << sendAddress << '-' << sendPort << " from " << GetPort()); 
*/

  ApplyQoS();
}

#endif  // H323_GNUGK




