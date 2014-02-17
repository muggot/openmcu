/*
 * gnugknat.h
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
 * $Log: gnugknat.h,v $
 * Revision 1.4  2008/02/01 07:50:17  shorne
 * added shutdown mutex to fix occasion shutdown timing errors.
 *
 * Revision 1.3  2008/01/18 01:36:42  shorne
 * Fix blocking and timeout on call ending
 *
 * Revision 1.2  2008/01/02 18:35:40  willamowius
 * make SetAvailable() return void
 *
 * Revision 1.1  2008/01/01 00:16:12  shorne
 * Added GnuGknat and FileTransfer support
 *
 *
 *
 */

#include <ptlib.h>
#include <h323.h>

#ifdef H323_GNUGK

#ifndef GNUGK_NAT
#define GNUGK_NAT

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class GNUGK_Feature;
class GNUGKTransport  : public H323TransportTCP
{
  PCLASSINFO(GNUGKTransport, H323TransportTCP);

  public:

	enum PDUType {
		e_raw,
	};

    /**Create a new transport channel.
     */
    GNUGKTransport(
      H323EndPoint & endpoint,        /// H323 End Point object
	  GNUGK_Feature * feat,			  /// Feature
	  PString & gkid                  /// Gatekeeper ID
    );

	~GNUGKTransport();

	/**Handle the GNUGK Signalling
	  */
	BOOL HandleGNUGKSignallingChannelPDU();

	/**Handle the GNUGK Signalling
	  */
	BOOL HandleGNUGKSignallingSocket(H323SignalPDU & pdu);

    /**Write a protocol data unit from the transport.
       This will write using the transports mechanism for PDU boundaries, for
       example UDP is a single Write() call, while for TCP there is a TPKT
       header that indicates the size of the PDU.
      */
    virtual BOOL WritePDU(
      const PBYTEArray & pdu  /// PDU to write
    );

    /**Read a protocol data unit from the transport.
       This will read using the transports mechanism for PDU boundaries, for
       example UDP is a single Read() call, while for TCP there is a TPKT
       header that indicates the size of the PDU.
      */
	virtual BOOL ReadPDU(
         PBYTEArray & pdu  /// PDU to Read
	);

	BOOL CreateNewTransport();

	BOOL InitialPDU();

	BOOL SetGKID(const PString & newid);

	BOOL isCall() { return isConnected; };

	void ConnectionLost(BOOL established);

	BOOL IsConnectionLost();


// Overrides
    /**Connect to the remote party.
      */
    virtual BOOL Connect();

    /**Close the channel.(Don't do anything)
      */
    virtual BOOL Close();

    virtual BOOL IsListening() const;

	virtual BOOL IsOpen () const;

	BOOL CloseTransport() { return closeTransport; };

  protected:
	 PString GKid;

	 PMutex connectionsMutex;
	 PMutex WriteMutex;
	 PMutex IntMutex;
	 PMutex shutdownMutex;
	 PTimeInterval ReadTimeOut;
	 PSyncPoint ReadMutex;

	 GNUGK_Feature * Feature;

	 BOOL   isConnected;
	 BOOL   remoteShutDown;
	 BOOL	closeTransport;
	 
};



class GNUGK_Feature : public PObject  
{

	PCLASSINFO(GNUGK_Feature, PObject);

public:
	GNUGK_Feature(H323EndPoint & ep, 
		H323TransportAddress & remoteAddress, 
		PString gkid,
		WORD KeepAlive = 10
		);

	~GNUGK_Feature();

	BOOL CreateNewTransport();

	BOOL ReRegister(const PString & newid);

	BOOL IsOpen() { return open; };

	static WORD keepalive;
	static GNUGKTransport * curtransport;
	static BOOL connectionlost;
		
protected:	

	H323EndPoint & ep;
	H323TransportAddress address;
	PString GKid;
	BOOL open;

};

class PNatMethod_GnuGk  : public PNatMethod
{
	PCLASSINFO(PNatMethod_GnuGk,PNatMethod);

public:

  /**@name Construction */
  //@{
	/** Default Contructor
	*/
	PNatMethod_GnuGk();

	/** Deconstructor
	*/
	~PNatMethod_GnuGk();
  //@}

  /**@name General Functions */
  //@{
   void AttachEndPoint(H323EndPoint * ep);

   virtual BOOL GetExternalAddress(
      PIPSocket::Address & externalAddress, /// External address of router
      const PTimeInterval & maxAge = 1000   /// Maximum age for caching
	  );

  /**  CreateSocketPair
		Create the UDP Socket pair
  */
    virtual BOOL CreateSocketPair(
      PUDPSocket * & socket1,
      PUDPSocket * & socket2,
      const PIPSocket::Address & binding = PIPSocket::GetDefaultIpAny()
    );

  /**  isAvailable.
		Returns whether the Nat Method is ready and available in
		assisting in NAT Traversal. The principal is function is
		to allow the EP to detect various methods and if a method
		is detected then this method is available for NAT traversal
		The Order of adding to the PNstStrategy determines which method
		is used
  */
   virtual BOOL IsAvailable() { return available; };

   void SetAvailable() { available = TRUE; };

   BOOL OpenSocket(PUDPSocket & socket, PortInfo & portInfo) const;

#if PTLIB_VER > PTLIB_VERSION_INT(2,0,1)
   static PString GetNatMethodName() { return PString("GNUGK"); };
   virtual PString GetName() const { return GetNatMethodName(); }
#else
   static PStringList GetNatMethodName() { return PStringList("GNUGK"); };
   virtual PStringList GetName() const { return GetNatMethodName(); }
#endif

#if PTLIB_VER > PTLIB_VERSION_INT(2,0,1) && PTLIB_VER < PTLIB_VERSION_INT(2,11,0)
   virtual bool GetServerAddress(PIPSocket::Address & address, WORD & port) const { return false; };
   virtual bool GetInterfaceAddress(PIPSocket::Address & internalAddress) const { return false; };
   virtual PBoolean CreateSocket(PUDPSocket * & socket, const PIPSocket::Address & binding = PIPSocket::GetDefaultIpAny(), WORD localPort = 0) { return false; };
   virtual bool IsAvailable(const PIPSocket::Address&) { return (available && active); }
   virtual RTPSupportTypes GetRTPSupport(PBoolean force = PFalse)  { return RTPSupported; }
protected:
   PBoolean active;
#elif PTLIB_VER >= PTLIB_VERSION_INT(2,11,0)
    virtual PString GetServer() const { return PString(); }
    virtual bool GetServerAddress(PIPSocketAddressAndPort & ) const { return false; }
    virtual NatTypes GetNatType(bool) { return UnknownNat; }
    virtual NatTypes GetNatType(const PTimeInterval &) { return UnknownNat; }
    virtual bool SetServer(const PString &) { return false; }
    virtual bool Open(const PIPSocket::Address &) { return false; }
    virtual bool CreateSocket(BYTE component,PUDPSocket * & socket,
            const PIPSocket::Address & binding = PIPSocket::GetDefaultIpAny(),WORD localPort = 0)  { return false; }
    virtual void SetCredentials(const PString &, const PString &, const PString &) {}
protected:
    virtual NatTypes InternalGetNatType(bool, const PTimeInterval &) { return UnknownNat; }
#endif

protected:
	BOOL available;
};

#if PTLIB_VER > PTLIB_VERSION_INT(2,8,0)
PPLUGIN_STATIC_LOAD(GnuGk, PNatMethod);
#else
PWLIB_STATIC_LOAD_PLUGIN(GnuGk, PNatMethod);
#endif

class GNUGKUDPSocket : public PUDPSocket
{
  PCLASSINFO(GNUGKUDPSocket, PUDPSocket);
  public:
  /**@name Construction/Deconstructor */
  //@{
	/** create a UDP Socket Fully Nat Supported
		ready for H323plus to Call.
	*/
    GNUGKUDPSocket();

	/** Deconstructor to reallocate Socket and remove any exiting
		allocated NAT ports, 
	*/
	~GNUGKUDPSocket();

	virtual void SetSendAddress(
      const Address & address,    /// IP address to send packets.
      WORD port                   /// Port to send packets.
    );
   //@}

  protected:

	PIPSocket::Address Remote;

};

#endif // GNUGK_NAT

#endif // H323_GNUGK



