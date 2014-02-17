/*
 * psnmp.h
 *
 * Simple Network Management Protocol classes.
 *
 * Portable Windows Library
 *
 * Copyright (c) 1993-2002 Equivalence Pty. Ltd.
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
 * The Original Code is Portable Windows Library.
 *
 * The Initial Developer of the Original Code is Equivalence Pty. Ltd.
 *
 * Contributor(s): ______________________________________.
 *
 * $Log: psnmp.h,v $
 * Revision 1.10  2007/08/02 18:48:35  shorne
 * Added SNMP Server support
 *
 * Revision 1.9  2002/11/06 22:47:24  robertj
 * Fixed header comment (copyright etc)
 *
 * Revision 1.8  2002/09/16 01:08:59  robertj
 * Added #define so can select if #pragma interface/implementation is used on
 *   platform basis (eg MacOS) rather than compiler, thanks Robert Monaghan.
 *
 * Revision 1.7  1999/03/09 08:01:46  robertj
 * Changed comments for doc++ support (more to come).
 *
 * Revision 1.6  1999/02/16 08:07:10  robertj
 * MSVC 6.0 compatibility changes.
 *
 * Revision 1.5  1998/11/30 02:50:55  robertj
 * New directory structure
 *
 * Revision 1.4  1998/09/23 06:27:38  robertj
 * Added open source copyright license.
 *
 * Revision 1.3  1996/11/04 03:56:16  robertj
 * Added selectable read buffer size.
 *
 * Revision 1.2  1996/09/20 12:19:36  robertj
 * Used read timeout instead of member variable.
 *
 * Revision 1.1  1996/09/14 12:58:57  robertj
 * Initial revision
 *
 * Revision 1.6  1996/05/09 13:23:49  craigs
 * Added trap functions
 *
 * Revision 1.5  1996/04/23 12:12:46  craigs
 * Changed to use GetErrorText function
 *
 * Revision 1.4  1996/04/16 13:20:43  craigs
 * Final version prior to beta1 release
 *
 * Revision 1.3  1996/04/15 09:05:30  craigs
 * Latest version prior to integration with Robert's changes
 *
 * Revision 1.2  1996/04/01 12:36:12  craigs
 * Fixed RCS header, added IPAddress functions
 *
 * Revision 1.1  1996/03/02 06:49:51  craigs
 * Initial revision
 *
 */

#ifndef _PSNMP_H
#define _PSNMP_H

#ifdef P_USE_PRAGMA
#pragma interface
#endif

#ifdef P_SNMP

#include <ptlib/sockets.h>
#include <ptclib/snmp.h>
#include <ptclib/pasn.h>

#include <list>
#include <vector>

//////////////////////////////////////////////////////////////////////////

/** A list of object IDs and their values
 */
class PSNMPVarBindingList : public PObject
{
  PCLASSINFO(PSNMPVarBindingList, PObject)
  public:

    void Append(const PString & objectID);
    void Append(const PString & objectID, PASNObject * obj);
    void AppendString(const PString & objectID, const PString & str);

    void RemoveAll();

    PINDEX GetSize() const;

    PString GetObjectID(PINDEX idx) const;
    PASNObject & operator[](PINDEX idx) const;

    void PrintOn(ostream & strm) const;

  protected:
    PStringList     objectIds;
    PASNObjectList  values;
};

//////////////////////////////////////////////////////////////////////////

/** A descendant of PUDPSocket which can perform SNMP calls
 */
class PSNMP : public PIndirectChannel
{
  PCLASSINFO(PSNMP, PIndirectChannel)
  public:
    enum ErrorType {
       // Standard RFC1157 errors
       NoError        = 0,
       TooBig         = 1,
       NoSuchName     = 2,
       BadValue       = 3,
       ReadOnly       = 4,
       GenErr         = 5,

       // Additional errors
       NoResponse,
       MalformedResponse,
       SendFailed,
       RxBufferTooSmall,
       TxDataTooBig,
       NumErrors
    };

    enum RequestType {
       GetRequest     = 0,
       GetNextRequest = 1,
       GetResponse    = 2,
       SetRequest     = 3,
       Trap           = 4,
    };

    enum { TrapPort = 162 };

    enum TrapType {
      ColdStart             = 0,
      WarmStart             = 1,
      LinkDown              = 2,
      LinkUp                = 3,
      AuthenticationFailure = 4,
      EGPNeighbourLoss      = 5,
      EnterpriseSpecific    = 6,
      NumTrapTypes
    };

    static PString GetErrorText(ErrorType err);

    static PString GetTrapTypeText(PINDEX code);

    static void SendEnterpriseTrap (
                 const PIPSocket::Address & addr,
                            const PString & community,
                            const PString & enterprise,
                                     PINDEX specificTrap,
                               PASNUnsigned timeTicks,
                                       WORD sendPort = TrapPort);

    static void SendEnterpriseTrap (
                 const PIPSocket::Address & addr,
                            const PString & community,
                            const PString & enterprise,
                                     PINDEX specificTrap,
                               PASNUnsigned timeTicks,
                const PSNMPVarBindingList & vars,
                                       WORD sendPort = TrapPort);

    static void SendTrap (
                       const PIPSocket::Address & addr,
                                  PSNMP::TrapType trapType,
                                  const PString & community,
                                  const PString & enterprise,
                                           PINDEX specificTrap,
                                     PASNUnsigned timeTicks,
                      const PSNMPVarBindingList & vars,
                                             WORD sendPort = TrapPort);

    static void SendTrap (
                      const PIPSocket::Address & addr,
                                  PSNMP::TrapType trapType,
                                  const PString & community,
                                  const PString & enterprise,
                                           PINDEX specificTrap,
                                     PASNUnsigned timeTicks,
                      const PSNMPVarBindingList & vars,
                       const PIPSocket::Address & agentAddress,
                                             WORD sendPort = TrapPort);
                            
    static void WriteTrap (           PChannel & channel,
                                  PSNMP::TrapType trapType,
                                  const PString & community,
                                  const PString & enterprise,
                                           PINDEX specificTrap,
                                     PASNUnsigned timeTicks,
                      const PSNMPVarBindingList & vars,
                       const PIPSocket::Address & agentAddress);

    static BOOL DecodeTrap(const PBYTEArray & readBuffer,
                                       PINDEX & version,
                                      PString & community,
                                      PString & enterprise,
                           PIPSocket::Address & address,
                                       PINDEX & genericTrapType,
                                      PINDEX  & specificTrapType,
                                 PASNUnsigned & timeTicks,
                          PSNMPVarBindingList & varsOut);

	typedef list<pair<PString,PRFC1155_ObjectSyntax> > BindingList;
};


//////////////////////////////////////////////////////////////////////////

/** Class which gets SNMP data
 */
class PSNMPClient : public PSNMP
{
  PCLASSINFO(PSNMPClient, PSNMP)
  public:
    PSNMPClient(const PString & host,
                PINDEX retryMax = 5,
                PINDEX timeoutMax = 5,
                PINDEX rxBufferSize = 1500,
                PINDEX txSize = 484);

    PSNMPClient(PINDEX retryMax = 5,
                PINDEX timeoutMax = 5,
                PINDEX rxBufferSize = 1500,
                PINDEX txSize = 484);

    void SetVersion(PASNInt version);
    PASNInt GetVersion() const;

    void SetCommunity(const PString & str);
    PString GetCommunity() const;

    void SetRequestID(PASNInt requestID);
    PASNInt GetRequestID() const;

    BOOL WriteGetRequest (PSNMPVarBindingList & varsIn,
                          PSNMPVarBindingList & varsOut);

    BOOL WriteGetNextRequest (PSNMPVarBindingList & varsIn,
                              PSNMPVarBindingList & varsOut);

    BOOL WriteSetRequest (PSNMPVarBindingList & varsIn,
                          PSNMPVarBindingList & varsOut);

    ErrorType GetLastErrorCode() const;
    PINDEX    GetLastErrorIndex() const;
    PString   GetLastErrorText() const;

  protected:
    BOOL WriteRequest (PASNInt requestCode,
                       PSNMPVarBindingList & varsIn,
                       PSNMPVarBindingList & varsOut);


    BOOL ReadRequest(PBYTEArray & readBuffer);

    PString   hostName;
    PString   community;
    PASNInt   requestId;
    PASNInt   version;
    PINDEX    retryMax;
    PINDEX    lastErrorIndex;
    ErrorType lastErrorCode;
    PBYTEArray readBuffer;
    PINDEX     maxRxSize;
    PINDEX     maxTxSize;
};


//////////////////////////////////////////////////////////////////////////

/** Class which supplies SNMP data
 */
class PSNMPServer : public PSNMP, PThread
{
  PCLASSINFO(PSNMPServer, PSNMP)
  public:

    PSNMPServer(PIPSocket::Address binding = PIPSocket::GetDefaultIpAny(), 
		        WORD localPort = 161,   
				PINDEX timeout = 5000, 
				PINDEX rxSize = 10000, 
				PINDEX txSize = 10000);

	~PSNMPServer();

	void Main();

	void SetVersion(PASNInt newVersion);
	BOOL HandleChannel();
	int ProcessPDU(const PBYTEArray & readBuffer, PBYTEArray & writeBuffer);

	virtual BOOL Authorise(const PIPSocket::Address & received);

	virtual BOOL OnGetRequest     (PINDEX reqID, PSNMP::BindingList & vars, PSNMP::ErrorType & errCode);
	virtual BOOL OnGetNextRequest (PINDEX reqID, PSNMP::BindingList & vars, PSNMP::ErrorType & errCode);
	virtual BOOL OnSetRequest     (PINDEX reqID, PSNMP::BindingList & vars, PSNMP::ErrorType & errCode);

    BOOL SendGetResponse          (PSNMPVarBindingList & vars);
  
  protected:
    PString   community;
    PASNInt   version;
    PINDEX    lastErrorIndex;
    ErrorType lastErrorCode;
    PBYTEArray readBuffer;
    PINDEX     maxRxSize;
    PINDEX     maxTxSize;
	PUDPSocket * baseSocket;
};

#endif // P_SNMP

#endif


// End of File.
