/*
 * ethsock.cxx
 *
 * Direct Ethernet socket implementation.
 *
 * Portable Windows Library
 *
 * Copyright (c) 1993-1998 Equivalence Pty. Ltd.
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
 * Portions are Copyright (C) 1993 Free Software Foundation, Inc.
 * All Rights Reserved.
 *
 * Contributor(s): ______________________________________.
 *
 * $Log: ethsock.cxx,v $
 * Revision 1.52  2007/10/03 01:18:47  rjongbloed
 * Fixed build for Windows Mobile 5 and added Windows Mobile 6
 *
 * Revision 1.51  2007/09/08 11:34:29  rjongbloed
 * Improved memory checking (leaks etc), especially when using MSVC debug library.
 *
 * Revision 1.50  2007/07/03 08:42:27  rjongbloed
 * Fixed strange issue where interface list return is sometimes empty if polled
 *   while an interface  is going up or down.
 *
 * Revision 1.49  2007/06/27 03:15:21  rjongbloed
 * Added ability to select filtering of down network interfaces.
 *
 * Revision 1.48  2006/04/09 11:03:59  csoutheren
 * Remove warnings on VS.net 2005
 *
 * Revision 1.47  2005/11/30 12:47:42  csoutheren
 * Removed tabs, reformatted some code, and changed tags for Doxygen
 *
 * Revision 1.46  2005/09/18 13:01:43  dominance
 * fixed pragma warnings when building with gcc.
 *
 * Revision 1.45  2005/07/13 11:48:54  csoutheren
 * Backported QOS changes from isvo branch
 *
 * Revision 1.44.4.1  2005/04/25 13:33:56  shorne
 * Extra support for DHCP Environment (Win32)
 *
 * Revision 1.44  2004/10/23 10:52:59  ykiryanov
 * Added ifdef _WIN32_WCE for PocketPC 2003 SDK port
 *
 * Revision 1.43  2004/06/30 12:17:06  rjongbloed
 * Rewrite of plug in system to use single global variable for all factories to avoid all sorts
 *   of issues with startup orders and Windows DLL multiple instances.
 *
 * Revision 1.42  2004/06/08 01:19:22  csoutheren
 * Fixed problem with SNMP library not loading under Windows in some cases
 *
 * Revision 1.41  2004/06/01 05:24:12  csoutheren
 * Changed loading of inetmib1.dll to use PProcessStartup to avoid crashes when it is unloaded before ~H323Endpoint is called
 *
 * Revision 1.40  2004/01/30 02:06:06  csoutheren
 * Added mutex to avoid threading problems on Windows
 * Thanks to Hans Verbeek
 *
 * Revision 1.39  2003/11/05 22:51:22  csoutheren
 * Added pragma to automatically included required libs
 *
 * Revision 1.38  2003/09/17 05:45:10  csoutheren
 * Removed recursive includes
 *
 * Revision 1.37  2003/04/01 06:01:48  robertj
 * Fixed problem with returning correct route table device name if have
 *   2 NIC's under Windows 2000, thanks faa06@tid.es
 *
 * Revision 1.36  2003/01/11 05:10:51  robertj
 * Fixed Win CE compatibility issues, thanks Joerg Schoemer
 *
 * Revision 1.35  2002/11/12 02:22:16  robertj
 * Fixed problem where if SNMP not correctly installed on some flavours of
 *   windows (eg ME) the system cannot find any interfaces. Added fail safe
 *   that if could not determine one, it uses the ip address of gethostname()
 *   which should be one of the interfaces on the system.
 *
 * Revision 1.34  2002/11/08 06:45:23  robertj
 * Fixed problem with very long interface names, pointed out by Kees Klop.
 *
 * Revision 1.33  2002/10/08 12:41:52  robertj
 * Changed for IPv6 support, thanks Sébastien Josset.
 *
 * Revision 1.32  2002/02/25 09:57:29  robertj
 * Fixed possible NULL pointer use and  memory leak, thanks Klaus König
 *
 * Revision 1.31  2002/02/15 03:56:46  yurik
 * Warnings removed during compilation, patch courtesy of Jehan Bing, jehan@bravobrava.com
 *
 * Revision 1.30  2001/10/12 19:04:24  yurik
 * New more robust ip collection routine
 *
 * Revision 1.29  2001/10/04 05:59:41  robertj
 * Plugged numerous memory leaks.
 *
 * Revision 1.28  2001/10/03 03:12:21  robertj
 * Changed to use only a single instance of SNMP library to avoid memory leak.
 *
 * Revision 1.27  2001/09/10 02:51:23  robertj
 * Major change to fix problem with error codes being corrupted in a
 *   PChannel when have simultaneous reads and writes in threads.
 *
 * Revision 1.26  2001/09/09 02:03:49  yurik
 * no message
 *
 * Revision 1.25  2001/08/28 03:22:01  yurik
 * Fixed crash on snmp init bug
 *
 * Revision 1.24  2001/08/16 20:12:25  yurik
 * Fixed duplicate ordinal - ifdef'd ce code
 *
 * Revision 1.23  2001/08/15 22:15:27  yurik
 * First cut of Windows CE port  to support gatekeeper
 *
 * Revision 1.22  2001/03/05 04:18:27  robertj
 * Added net mask to interface info returned by GetInterfaceTable()
 *
 * Revision 1.21  2000/03/06 03:59:22  robertj
 * Fixed warning about handle types, thanks Steve Bennett
 *
 * Revision 1.20  1999/10/29 03:34:19  robertj
 * Fixed possible crash accessing IP addresses from SNMP tables.
 *
 * Revision 1.19  1999/10/14 01:34:55  robertj
 * Fixed backward compatibility problem with old SNMP header file.
 *
 * Revision 1.18  1999/09/10 04:35:42  robertj
 * Added Windows version of PIPSocket::GetInterfaceTable() function.
 *
 * Revision 1.17  1999/04/18 12:58:39  robertj
 * MSVC 5 backward compatibility
 *
 * Revision 1.16  1999/02/16 08:08:06  robertj
 * MSVC 6.0 compatibility changes.
 *
 * Revision 1.15  1998/11/30 04:48:38  robertj
 * New directory structure
 *
 * Revision 1.14  1998/11/22 11:30:10  robertj
 * Check route table function to get a list
 *
 * Revision 1.13  1998/11/20 03:17:43  robertj
 * Split rad and write buffers to separate pools.
 *
 * Revision 1.12  1998/11/19 05:18:48  robertj
 * Added route table manipulation functions to PIPSocket class.
 *
 * Revision 1.11  1998/11/14 06:31:41  robertj
 * Changed semantics of os_sendto to return TRUE if ANY bytes are sent.
 * Added support for MSDUN1.3 DHCP registry entries.
 *
 * Revision 1.10  1998/10/23 04:09:08  robertj
 * Fixes for NT support.
 * Allowed both old and new driver by compilation option.
 *
 * Revision 1.9  1998/10/15 05:41:48  robertj
 * New memory leak check code.
 *
 * Revision 1.8  1998/10/12 09:34:42  robertj
 * New method for getting IP addresses of interfaces.
 *
 * Revision 1.7  1998/10/06 10:24:41  robertj
 * Fixed hang when using reset command, removed the command!
 *
 * Revision 1.6  1998/09/24 03:30:45  robertj
 * Added open software license.
 *
 * Revision 1.5  1998/09/15 08:25:36  robertj
 * Fixed a number of warnings at maximum optimisation.
 *
 * Revision 1.4  1998/09/08 15:14:36  robertj
 * Fixed packet type based filtering in Read() function.
 *
 * Revision 1.3  1998/08/25 11:03:15  robertj
 * Fixed proble with NT get of OID.
 * Fixed bug with not setting channel name when interface opened.
 *
 * Revision 1.2  1998/08/21 05:27:13  robertj
 * Fine tuning of interface.
 *
 * Revision 1.1  1998/08/20 06:04:52  robertj
 * Initial revision
 *
 */

#include <ptlib.h>
#include <ptlib/sockets.h>

#ifdef _WIN32_WCE
  #include <ptlib/wm/snmp.h>
#else
  #include <snmp.h>
  #ifdef _MSC_VER
    #pragma comment(lib, "snmpapi.lib")
  #endif
#endif

///////////////////////////////////////////////////////////////////////////////
// Stuff from snmp.h

#ifndef RFC1157VarBindList
typedef RFC1157VarBind SnmpVarBind;
typedef RFC1157VarBindList SnmpVarBindList;
typedef LONG AsnInteger32;
#define SNMP_PDU_GET ASN_RFC1157_GETREQUEST
#define SNMP_PDU_GETNEXT ASN_RFC1157_GETNEXTREQUEST
#define ASN_IPADDRESS    ASN_RFC1155_IPADDRESS // Prevents GetInterfaceTable failure
#pragma message("Later version of snmp.h required!")
#endif

///////////////////////////////////////////////////////////////////////////////
// Stuff from ndis.h

#define OID_802_3_PERMANENT_ADDRESS         0x01010101
#define OID_802_3_CURRENT_ADDRESS           0x01010102

#define OID_GEN_DRIVER_VERSION              0x00010110
#define OID_GEN_CURRENT_PACKET_FILTER       0x0001010E
#define OID_GEN_MEDIA_SUPPORTED             0x00010103

#define NDIS_PACKET_TYPE_DIRECTED           0x0001
#define NDIS_PACKET_TYPE_MULTICAST          0x0002
#define NDIS_PACKET_TYPE_ALL_MULTICAST      0x0004
#define NDIS_PACKET_TYPE_BROADCAST          0x0008
#define NDIS_PACKET_TYPE_PROMISCUOUS        0x0020

typedef enum _NDIS_MEDIUM {
    NdisMedium802_3,
    NdisMedium802_5,
    NdisMediumFddi,
    NdisMediumWan,
    NdisMediumLocalTalk,
    NdisMediumDix,              // defined for convenience, not a real medium
    NdisMediumArcnetRaw,
    NdisMediumArcnet878_2
} NDIS_MEDIUM, *PNDIS_MEDIUM;                    

///////////////////////////////////////////////////////////////////////////////


#define USE_VPACKET
#include <ptlib/msos/ptlib/epacket.h>

#ifdef USE_VPACKET
#define PACKET_SERVICE_NAME "Packet"
#define PACKET_VXD_NAME     "VPacket"
#else
#define PACKET_SERVICE_NAME "EPacket"
#define PACKET_VXD_NAME     "EPacket"
#define GetQueryOidCommand(oid) IOCTL_EPACKET_QUERY_OID
#endif

#define SERVICES_REGISTRY_KEY "HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Services\\"


///////////////////////////////////////////////////////////////////////////////

class PWin32AsnAny : public AsnAny
{
  public:
    PWin32AsnAny();
    ~PWin32AsnAny() { MemFree(); }
    BOOL GetInteger(AsnInteger & i);
    BOOL GetIpAddress(PIPSocket::Address & addr);
    void MemFree();
  private:
    PWin32AsnAny(const PWin32AsnAny &) { }
    PWin32AsnAny & operator=(const PWin32AsnAny &) { return *this; }
};


///////////////////////////////////////////////////////////////////////////////

class PWin32AsnOid : public AsnObjectIdentifier
{
  public:
    PWin32AsnOid();
    PWin32AsnOid(const char * str);
    PWin32AsnOid(const PWin32AsnOid & oid)              { SnmpUtilOidCpy(this, (AsnObjectIdentifier *)&oid); }
    ~PWin32AsnOid()                                     { SnmpUtilOidFree(this); }
    PWin32AsnOid & operator=(const AsnObjectIdentifier&);
    PWin32AsnOid & operator=(const PWin32AsnOid & oid)  { SnmpUtilOidFree(this); SnmpUtilOidCpy(this, (AsnObjectIdentifier *)&oid); return *this; }
    PWin32AsnOid & operator+=(const PWin32AsnOid & oid) { SnmpUtilOidAppend(this, (AsnObjectIdentifier *)&oid); return *this; }
    UINT & operator[](int idx)                          { return ids[idx]; }
    UINT   operator[](int idx) const                    { return ids[idx]; }
    bool operator==(const PWin32AsnOid & oid)           { return SnmpUtilOidCmp(this, (AsnObjectIdentifier *)&oid) == 0; }
    bool operator!=(const PWin32AsnOid & oid)           { return SnmpUtilOidCmp(this, (AsnObjectIdentifier *)&oid) != 0; }
    bool operator< (const PWin32AsnOid & oid)           { return SnmpUtilOidCmp(this, (AsnObjectIdentifier *)&oid) <  0; }
    bool operator<=(const PWin32AsnOid & oid)           { return SnmpUtilOidCmp(this, (AsnObjectIdentifier *)&oid) <= 0; }
    bool operator> (const PWin32AsnOid & oid)           { return SnmpUtilOidCmp(this, (AsnObjectIdentifier *)&oid) >  0; }
    bool operator>=(const PWin32AsnOid & oid)           { return SnmpUtilOidCmp(this, (AsnObjectIdentifier *)&oid) >= 0; }
    bool operator*=(const PWin32AsnOid & oid)           { return SnmpUtilOidNCmp(this, (AsnObjectIdentifier *)&oid, idLength) == 0; }
};


/////////////////////////////////////////////////////////////////////////////

class PWin32SnmpLibrary
#ifndef _WIN32_WCE
                        : public PDynaLink
{
    PCLASSINFO(PWin32SnmpLibrary, PDynaLink)
  public:
#else
{
  public:
    void Close();
    BOOL IsLoaded() { return TRUE; }
#endif
public:
    PWin32SnmpLibrary();

    BOOL GetOid(AsnObjectIdentifier & oid, AsnInteger & value);
    BOOL GetOid(AsnObjectIdentifier & oid, PIPSocket::Address & ip_address);
    BOOL GetOid(AsnObjectIdentifier & oid, PString & str);
    BOOL GetOid(AsnObjectIdentifier & oid, void * value, UINT valSize, UINT * len = NULL);
    BOOL GetOid(AsnObjectIdentifier & oid, PWin32AsnAny & value)     { return QueryOid(SNMP_PDU_GET, oid, value); }

    BOOL GetNextOid(AsnObjectIdentifier & oid, PWin32AsnAny & value) { return QueryOid(SNMP_PDU_GETNEXT, oid, value); }

    PString GetInterfaceName(int ifNum);
    PString GetInterfaceName(PIPSocket::Address ipAddr);
    PIPSocket::Address GetInterfaceAddress(int ifNum);

    static PWin32SnmpLibrary & Current();
    static PMutex & GetMutex();

  private:
    PMutex mutex;

    BOOL (WINAPI *_Init)(DWORD,HANDLE*,AsnObjectIdentifier*);
    BOOL (WINAPI *_Query)(BYTE,SnmpVarBindList*,AsnInteger32*,AsnInteger32*);

    BOOL Init(DWORD upTime, HANDLE * trapEvent, AsnObjectIdentifier * firstSupportedRegion)
    { return (*_Init)(upTime, trapEvent, firstSupportedRegion); }

    BOOL Query(BYTE pduType, SnmpVarBindList * pVarBindList, AsnInteger32 * pErrorStatus, AsnInteger32 * pErrorIndex)
    { return _Query(pduType, pVarBindList, pErrorStatus, pErrorIndex); }

    BOOL QueryOid(BYTE cmd, AsnObjectIdentifier & oid, PWin32AsnAny & value);
};

class WinSNMPLoader : public PProcessStartup
{
  PCLASSINFO(WinSNMPLoader, PProcessStartup);
  public:
    void OnStartup()
    { }

    PWin32SnmpLibrary & Current()
    {
      PWaitAndSignal m(mutex);
      if (snmpLibrary == NULL) {
        snmpLibrary = PNEW PWin32SnmpLibrary;
      }
      return *snmpLibrary;
    }

    void OnShutdown()
    { 
      PWaitAndSignal m(mutex); 
      delete snmpLibrary; 
      snmpLibrary = NULL;
    }

  protected:
    PMutex mutex;
    static PWin32SnmpLibrary * snmpLibrary;
};

PWin32SnmpLibrary * WinSNMPLoader::snmpLibrary = NULL;

static PFactory<PProcessStartup>::Worker<WinSNMPLoader> winSNMPLoadedStartupFactory("WinSNMPLoader", true);

PWin32SnmpLibrary & PWin32SnmpLibrary::Current()
{ 
  return ((WinSNMPLoader *)PFactory<PProcessStartup>::CreateInstance("WinSNMPLoader"))->Current();
}

PMutex & PWin32SnmpLibrary::GetMutex()
{
  return Current().mutex;
}

#define GetSNMPMutex    PWin32SnmpLibrary::GetMutex

///////////////////////////////////////////////////////////////////////////////

class PWin32OidBuffer
{
  public:
    PWin32OidBuffer(UINT oid, UINT len, const BYTE * data = NULL);
    ~PWin32OidBuffer() { delete buffer; }

    operator void *()         { return buffer; }
    operator DWORD ()         { return size; }
    DWORD operator [](int i)  { return buffer[i]; }

    void Move(BYTE * data, DWORD received);

  private:
    DWORD * buffer;
    UINT size;
};


///////////////////////////////////////////////////////////////////////////////

class PWin32PacketDriver
{
  public:
    static PWin32PacketDriver * Create();

    virtual ~PWin32PacketDriver();

    BOOL IsOpen() const;
    void Close();
    DWORD GetLastError() const;

    virtual BOOL EnumInterfaces(PINDEX idx, PString & name) = 0;
    virtual BOOL BindInterface(const PString & interfaceName) = 0;

    virtual BOOL EnumIpAddress(PINDEX idx, PIPSocket::Address & addr, PIPSocket::Address & net_mask) = 0;

    virtual BOOL BeginRead(void * buf, DWORD size, DWORD & received, PWin32Overlapped & overlap) = 0;
    virtual BOOL BeginWrite(const void * buf, DWORD len, PWin32Overlapped & overlap) = 0;
    BOOL CompleteIO(DWORD & received, PWin32Overlapped & overlap);

    BOOL IoControl(UINT func,
                   const void * input, DWORD inSize,
                   void * output, DWORD outSize,
                   DWORD & received);

    BOOL QueryOid(UINT oid, DWORD & data);
    BOOL QueryOid(UINT oid, UINT len, BYTE * data);
    BOOL SetOid(UINT oid, DWORD data);
    BOOL SetOid(UINT oid, UINT len, const BYTE * data);
#ifdef USE_VPACKET
    virtual UINT GetQueryOidCommand(DWORD oid) const = 0;
#endif

  protected:
    PWin32PacketDriver();

    DWORD dwError;
    HANDLE hDriver;
};

///////////////////////////////////////////////////////////////////////////////

class PWin32PacketVxD : public PWin32PacketDriver
{
  public:
    virtual BOOL EnumInterfaces(PINDEX idx, PString & name);
    virtual BOOL BindInterface(const PString & interfaceName);

    virtual BOOL EnumIpAddress(PINDEX idx, PIPSocket::Address & addr, PIPSocket::Address & net_mask);

    virtual BOOL BeginRead(void * buf, DWORD size, DWORD & received, PWin32Overlapped & overlap);
    virtual BOOL BeginWrite(const void * buf, DWORD len, PWin32Overlapped & overlap);

#ifdef USE_VPACKET
    virtual UINT GetQueryOidCommand(DWORD oid) const
      { return oid >= OID_802_3_PERMANENT_ADDRESS ? IOCTL_EPACKET_QUERY_OID : IOCTL_EPACKET_STATISTICS; }
#endif

  protected:
    PStringList transportBinding;
};


///////////////////////////////////////////////////////////////////////////////

class PWin32PacketSYS : public PWin32PacketDriver
{
  public:
    PWin32PacketSYS();

    virtual BOOL EnumInterfaces(PINDEX idx, PString & name);
    virtual BOOL BindInterface(const PString & interfaceName);

    virtual BOOL EnumIpAddress(PINDEX idx, PIPSocket::Address & addr, PIPSocket::Address & net_mask);

    virtual BOOL BeginRead(void * buf, DWORD size, DWORD & received, PWin32Overlapped & overlap);
    virtual BOOL BeginWrite(const void * buf, DWORD len, PWin32Overlapped & overlap);

#ifdef USE_VPACKET
    virtual UINT GetQueryOidCommand(DWORD) const
      { return IOCTL_EPACKET_QUERY_OID; }
#endif

  protected:
    PString registryKey;
};

///////////////////////////////////////////////////////////////////////////////

#ifdef _WIN32_WCE

class PWin32PacketCe : public PWin32PacketDriver
{
  public:
    PWin32PacketCe();

    virtual BOOL EnumInterfaces(PINDEX idx, PString & name);
    virtual BOOL BindInterface(const PString & interfaceName);

    virtual BOOL EnumIpAddress(PINDEX idx, PIPSocket::Address & addr, PIPSocket::Address & net_mask);

    virtual BOOL BeginRead(void * buf, DWORD size, DWORD & received, PWin32Overlapped & overlap);
    virtual BOOL BeginWrite(const void * buf, DWORD len, PWin32Overlapped & overlap);

#ifdef USE_VPACKET
    virtual UINT GetQueryOidCommand(DWORD) const
      { return IOCTL_EPACKET_QUERY_OID; }
#endif
  protected:
    PStringArray ipAddresses;
    PStringArray netMasks;
    PStringArray interfaces;
};

#endif // _WIN32_WCE

/////////////////////////////////////////////////////////////////////////////

class PWin32PacketBuffer : public PBYTEArray
{
  PCLASSINFO(PWin32PacketBuffer, PBYTEArray)
  public:
    enum Statuses {
      Uninitialised,
      Progressing,
      Completed
    };

    PWin32PacketBuffer(PINDEX sz);

    PINDEX GetData(void * buf, PINDEX size);
    PINDEX PutData(const void * buf, PINDEX length);
    HANDLE GetEvent() const { return overlap.hEvent; }

    BOOL ReadAsync(PWin32PacketDriver & pkt);
    BOOL ReadComplete(PWin32PacketDriver & pkt);
    BOOL WriteAsync(PWin32PacketDriver & pkt);
    BOOL WriteComplete(PWin32PacketDriver & pkt);

    BOOL InProgress() const { return status == Progressing; }
    BOOL IsCompleted() const { return status == Completed; }
    BOOL IsType(WORD type) const;

  protected:
    Statuses         status;
    PWin32Overlapped overlap;
    DWORD            count;
};


#define new PNEW


/////////////////////////////////////////////////////////////////////////////

PWin32AsnAny::PWin32AsnAny()
{
  asnType = ASN_INTEGER;
  asnValue.number = 0;
}


void PWin32AsnAny::MemFree()
{
  switch (asnType) {
    case ASN_OCTETSTRING :
      SnmpUtilMemFree(asnValue.string.stream);
      break;
#ifdef ASN_BITS
    case ASN_BITS :
      SnmpUtilMemFree(asnValue.bits.stream);
      break;
#endif
    case ASN_OBJECTIDENTIFIER :
      SnmpUtilMemFree(asnValue.object.ids);
      break;
    case ASN_SEQUENCE :
      SnmpUtilMemFree(asnValue.sequence.stream);
      break;
    case ASN_IPADDRESS :
      SnmpUtilMemFree(asnValue.address.stream);
      break;
#ifdef ASN_OPAQUE
    case ASN_OPAQUE :
      SnmpUtilMemFree(asnValue.arbitrary.stream);
      break;
#endif
  }

  asnType = ASN_INTEGER;
}


BOOL PWin32AsnAny::GetInteger(AsnInteger & i)
{
  if (asnType != ASN_INTEGER)
    return FALSE;

  i = asnValue.number;
  return TRUE;
}


BOOL PWin32AsnAny::GetIpAddress(PIPSocket::Address & addr)
{
  if (asnType != ASN_IPADDRESS || asnValue.address.stream == NULL)
    return FALSE;

  addr = PIPSocket::Address(asnValue.address.length, asnValue.address.stream);
  return TRUE;
}


///////////////////////////////////////////////////////////////////////////////

PWin32AsnOid::PWin32AsnOid()
{
  ids = NULL;
  idLength = 0;
}


PWin32AsnOid::PWin32AsnOid(const char * str)
{
  idLength = 0;
  ids = NULL;

  AsnObjectIdentifier oid;
  oid.idLength = 0;
  const char * dot = strchr(str, '.');
  while (dot != NULL) {
    oid.idLength++;
    dot = strchr(dot+1, '.');
  }

  if (oid.idLength > 0) {
    oid.ids = new UINT[++oid.idLength];
    char * next = (char *)str;
    for (UINT i = 0; i < oid.idLength; i++) {
      oid.ids[i] = strtoul(next, &next, 10);
      if (*next != '.')
        break;
      next++;
    }

    if (*next == '\0')
      SnmpUtilOidCpy(this, &oid);

    delete [] oid.ids;
  }
}


PWin32AsnOid & PWin32AsnOid::operator=(const AsnObjectIdentifier & oid)
{
  ids = oid.ids;
  idLength = oid.idLength;
  return *this;
}


///////////////////////////////////////////////////////////////////////////////


PWin32SnmpLibrary::PWin32SnmpLibrary()
#ifndef _WIN32_WCE
  : PDynaLink("inetmib1.dll")
#endif
{
#ifndef _WIN32_WCE
  HANDLE hEvent;
  AsnObjectIdentifier baseOid;
  if (!GetFunction("SnmpExtensionInit", (Function &)_Init) ||
      !GetFunction("SnmpExtensionQuery", (Function &)_Query)) {
    Close();
    PTRACE(1, "PWlib\tInvalid DLL: inetmib1.dll");
  }
  else if (!Init(0, &hEvent, &baseOid)) {
    PTRACE(1, "PWlib\tCould not initialise SNMP DLL: error=" << ::GetLastError());
    Close();
  }

#else
  _Init = SnmpExtensionInit; // do not call Init as we dont'have Close 
  _Query = SnmpExtensionQuery;
#endif
}

BOOL PWin32SnmpLibrary::GetOid(AsnObjectIdentifier & oid, AsnInteger & value)
{
  //if (!IsLoaded())
  //  return FALSE;

  PWin32AsnAny any;
  if (!GetOid(oid, any))
    return FALSE;

  return any.GetInteger(value);
}


BOOL PWin32SnmpLibrary::GetOid(AsnObjectIdentifier & oid, PIPSocket::Address & value)
{
  //if (!IsLoaded())
  //  return FALSE;

  PWin32AsnAny any;
  if (!GetOid(oid, any))
    return FALSE;

  return any.GetIpAddress(value);
}


BOOL PWin32SnmpLibrary::GetOid(AsnObjectIdentifier & oid, PString & str)
{
  //if (!IsLoaded())
  //  return FALSE;

  PWin32AsnAny any;
  if (!GetOid(oid, any))
    return FALSE;

  if (any.asnType != ASN_OCTETSTRING)
    return FALSE;

  str = PString((char *)any.asnValue.string.stream, any.asnValue.string.length);
  return TRUE;
}


BOOL PWin32SnmpLibrary::GetOid(AsnObjectIdentifier & oid, void * value, UINT valSize, UINT * len)
{
  //if (!IsLoaded())
  //  return FALSE;

  PWin32AsnAny any;
  if (!GetOid(oid, any))
    return FALSE;

  if (any.asnType != ASN_OCTETSTRING)
    return FALSE;

  if (len != NULL)
    *len = any.asnValue.string.length;

  if (any.asnValue.string.length > valSize)
    return FALSE;

  memcpy(value, any.asnValue.string.stream, any.asnValue.string.length);
  if (valSize > any.asnValue.string.length)
    ((char *)value)[any.asnValue.string.length] = '\0';
  return TRUE;
}


BOOL PWin32SnmpLibrary::QueryOid(BYTE cmd, AsnObjectIdentifier & oid, PWin32AsnAny & value)
{
  //if (!IsLoaded())
  //  return FALSE;

  value.MemFree();

  SnmpVarBindList vars;
  vars.len = 1;
  vars.list = (SnmpVarBind*)SnmpUtilMemAlloc(sizeof(SnmpVarBind));
  if (vars.list == NULL)
    return FALSE;

  vars.list->name = oid;
  vars.list->value = value;

  AsnInteger status, error;
  if (Query(cmd, &vars, &status, &error) && status == SNMP_ERRORSTATUS_NOERROR) {
    (AsnAny&)value = vars.list->value; // Use cast so does simple copy
    oid = vars.list->name;
  }

  SnmpUtilMemFree(vars.list);

  return status == SNMP_ERRORSTATUS_NOERROR;
}


PString PWin32SnmpLibrary::GetInterfaceName(int ifNum)
{
  PIPSocket::Address gwAddr = 0;

  gwAddr = GetInterfaceAddress(ifNum);
  if (gwAddr == 0)
    return PString::Empty();

  PString name = GetInterfaceName(gwAddr);
  if (name.IsEmpty()) {
    PWin32AsnOid nameOid = "1.3.6.1.2.1.2.2.1.2.0";
    nameOid[10] = ifNum;
    if (GetOid(nameOid, name.GetPointer(100), 100))
      name.MakeMinimumSize();
  }

  return name;
}


PString PWin32SnmpLibrary::GetInterfaceName(PIPSocket::Address ipAddr)
{
  PString gatewayInterface, anInterface;

  PWin32PacketDriver * tempDriver = PWin32PacketDriver::Create();

  PINDEX ifIdx = 0;
  while (gatewayInterface.IsEmpty() && tempDriver->EnumInterfaces(ifIdx++, anInterface)) {
    if (tempDriver->BindInterface(anInterface)) {
      PIPSocket::Address ifAddr, ifMask;
      PINDEX ipIdx = 0;
      if (tempDriver->EnumIpAddress(ipIdx++, ifAddr, ifMask) && ifAddr == ipAddr) {
        gatewayInterface = anInterface;
        break;
      }
    }
  }

  delete tempDriver;

  return gatewayInterface;
}

PIPSocket::Address PWin32SnmpLibrary::GetInterfaceAddress(int ifNum)
{
  PIPSocket::Address gwAddr = 0;
  PWin32AsnOid baseOid = "1.3.6.1.2.1.4.20.1";
  PWin32AsnOid oid = baseOid;
  PWin32AsnAny value;
  while (GetNextOid(oid, value)) {
    if (!(baseOid *= oid))
      break;
    if (value.asnType != ASN_IPADDRESS)
      break;

    oid[9] = 2;
    AsnInteger ifIndex = -1;
    if (!GetOid(oid, ifIndex) || ifIndex < 0)
      break;

    if (ifIndex == ifNum) {
      value.GetIpAddress(gwAddr);
      break;
    }

    oid[9] = 1;
  }

  return gwAddr;
}

/*
PWin32SnmpLibrary & PWin32SnmpLibrary::Current()
{
  static PWin32SnmpLibrary instance;
  return instance;
}
*/

///////////////////////////////////////////////////////////////////////////////

PWin32OidBuffer::PWin32OidBuffer(UINT oid, UINT len, const BYTE * data)
{
  size = sizeof(DWORD)*2 + len;
  buffer = new DWORD[(size+sizeof(DWORD)-1)/sizeof(DWORD)];

  buffer[0] = oid;
  buffer[1] = len;
  if (data != NULL)
    memcpy(&buffer[2], data, len);
}


void PWin32OidBuffer::Move(BYTE * data, DWORD received)
{
  memcpy(data, &buffer[2], received-sizeof(DWORD)*2);
}


///////////////////////////////////////////////////////////////////////////////

PWin32PacketDriver * PWin32PacketDriver::Create()
{
  OSVERSIONINFO info;
  info.dwOSVersionInfoSize = sizeof(info);
  GetVersionEx(&info);
#ifndef _WIN32_WCE
  if (info.dwPlatformId == VER_PLATFORM_WIN32_NT)
    return new PWin32PacketSYS;
  else
    return new PWin32PacketVxD;
#else
    return new PWin32PacketCe;
#endif
}


PWin32PacketDriver::PWin32PacketDriver()
{
  hDriver = INVALID_HANDLE_VALUE;
  dwError = ERROR_OPEN_FAILED;
}


PWin32PacketDriver::~PWin32PacketDriver()
{
  Close();
}


void PWin32PacketDriver::Close()
{
  if (hDriver != INVALID_HANDLE_VALUE) {
    CloseHandle(hDriver);
    hDriver = INVALID_HANDLE_VALUE;
  }
}


BOOL PWin32PacketDriver::IsOpen() const
{
  return hDriver != INVALID_HANDLE_VALUE;
}


DWORD PWin32PacketDriver::GetLastError() const
{
  return dwError;
}


BOOL PWin32PacketDriver::IoControl(UINT func,
                              const void * input, DWORD inSize,
                              void * output, DWORD outSize, DWORD & received)
{
  PWin32Overlapped overlap;

  if (DeviceIoControl(hDriver, func,
                      (LPVOID)input, inSize, output, outSize,
                      &received, &overlap)) {
    dwError = ERROR_SUCCESS;
    return TRUE;
  }

  dwError = ::GetLastError();
  if (dwError != ERROR_IO_PENDING)
    return FALSE;

  return CompleteIO(received, overlap);
}

#ifdef _WIN32_WCE
BOOL PWin32PacketDriver::CompleteIO(DWORD &, PWin32Overlapped &)
{
  return TRUE;
}
#else
BOOL PWin32PacketDriver::CompleteIO(DWORD & received, PWin32Overlapped & overlap)
{
  received = 0;
  if (GetOverlappedResult(hDriver, &overlap, &received, TRUE)) {
    dwError = ERROR_SUCCESS;
    return TRUE;
  }

  dwError = ::GetLastError();
  return FALSE;
}
#endif

BOOL PWin32PacketDriver::QueryOid(UINT oid, UINT len, BYTE * data)
{
  PWin32OidBuffer buf(oid, len);
  DWORD rxsize = 0;
  if (!IoControl(GetQueryOidCommand(oid), buf, buf, buf, buf, rxsize))
    return FALSE;

  if (rxsize == 0)
    return FALSE;

  buf.Move(data, rxsize);
  return TRUE;
}


BOOL PWin32PacketDriver::QueryOid(UINT oid, DWORD & data)
{
  DWORD oidData[3];
  oidData[0] = oid;
  oidData[1] = sizeof(data);
  oidData[2] = 0x12345678;

  DWORD rxsize = 0;
  if (!IoControl(GetQueryOidCommand(oid),
                 oidData, sizeof(oidData),
                 oidData, sizeof(oidData),
                 rxsize))
    return FALSE;

  if (rxsize == 0)
    return FALSE;

  data = oidData[2];
  return TRUE;
}


BOOL PWin32PacketDriver::SetOid(UINT oid, UINT len, const BYTE * data)
{
  DWORD rxsize = 0;
  PWin32OidBuffer buf(oid, len, data);
  return IoControl(IOCTL_EPACKET_SET_OID, buf, buf, buf, buf, rxsize);
}


BOOL PWin32PacketDriver::SetOid(UINT oid, DWORD data)
{
  DWORD oidData[3];
  oidData[0] = oid;
  oidData[1] = sizeof(data);
  oidData[2] = data;
  DWORD rxsize;
  return IoControl(IOCTL_EPACKET_SET_OID,
                   oidData, sizeof(oidData), oidData, sizeof(oidData), rxsize);
}

static BOOL RegistryQueryMultiSz(RegistryKey & registry,
                                 const PString & variable,
                                 PINDEX idx,
                                 PString & value)
{
  PString allValues;
  if (!registry.QueryValue(variable, allValues))
    return FALSE;

  const char * ptr = allValues;
  while (*ptr != '\0' && idx-- > 0)
    ptr += strlen(ptr)+1;

  if (*ptr == '\0')
    return FALSE;

  value = ptr;
  return TRUE;
}


///////////////////////////////////////////////////////////////////////////////

BOOL PWin32PacketVxD::EnumInterfaces(PINDEX idx, PString & name)
{
  static const PString RegBase = SERVICES_REGISTRY_KEY "Class\\Net";

  PString keyName;
  RegistryKey registry(RegBase, RegistryKey::ReadOnly);
  if (!registry.EnumKey(idx, keyName))
    return FALSE;

  PString description;
  RegistryKey subkey(RegBase + "\\" + keyName, RegistryKey::ReadOnly);
  if (subkey.QueryValue("DriverDesc", description))
    name = keyName + ": " + description;
  else
    name = keyName;

  return TRUE;
}


static PString SearchRegistryKeys(const PString & key,
                                  const PString & variable,
                                  const PString & value)
{
  RegistryKey registry(key, RegistryKey::ReadOnly);

  PString str;
  if (registry.QueryValue(variable, str) && (str *= value))
    return key;

  for (PINDEX idx = 0; registry.EnumKey(idx, str); idx++) {
    PString result = SearchRegistryKeys(key + str + '\\', variable, value);
    if (!result)
      return result;
  }

  return PString::Empty();
}


#ifdef _WIN32_WCE
BOOL PWin32PacketVxD::BindInterface(const PString &)
{
  return FALSE;
}
#else
BOOL PWin32PacketVxD::BindInterface(const PString & interfaceName)
{
  BYTE buf[20];
  DWORD rxsize;

  if (hDriver == INVALID_HANDLE_VALUE) {
    hDriver = CreateFile("\\\\.\\" PACKET_VXD_NAME ".VXD",
                         GENERIC_READ | GENERIC_WRITE,
                         0,
                         NULL,
                         OPEN_EXISTING,
                         FILE_ATTRIBUTE_NORMAL |
                             FILE_FLAG_OVERLAPPED |
                             FILE_FLAG_DELETE_ON_CLOSE,
                         NULL);
    if (hDriver == INVALID_HANDLE_VALUE) {
      dwError = ::GetLastError();
      return FALSE;
    }

#ifndef USE_VPACKET
    rxsize = 0;
    if (!IoControl(IOCTL_EPACKET_VERSION, NULL, 0, buf, sizeof(buf), rxsize)) {
      dwError = ::GetLastError();
      return FALSE;
    }

    if (rxsize != 2 || buf[0] < 1 || buf[1] < 1) {  // Require driver version 1.1
      Close();
      dwError = ERROR_BAD_DRIVER;
      return FALSE;
    }
#endif
  }

  PString devName;
  PINDEX colon = interfaceName.Find(':');
  if (colon != P_MAX_INDEX)
    devName = interfaceName.Left(colon);
  else
    devName = interfaceName;
  
  rxsize = 0;
  if (!IoControl(IOCTL_EPACKET_BIND,
                 (const char *)devName, devName.GetLength()+1,
                 buf, sizeof(buf), rxsize) || rxsize == 0) {
    dwError = ::GetLastError();
    if (dwError == 0)
      dwError = ERROR_BAD_DRIVER;
    return FALSE;
  }

  // Get a random OID to verify that the driver did actually open
  if (!QueryOid(OID_GEN_DRIVER_VERSION, 2, buf))
    return FALSE;

  dwError = ERROR_SUCCESS;    // Successful, even if may not be bound.

  PString devKey = SearchRegistryKeys("HKEY_LOCAL_MACHINE\\Enum\\", "Driver", "Net\\" + devName);
  if (devKey.IsEmpty())
    return TRUE;

  RegistryKey bindRegistry(devKey + "Bindings", RegistryKey::ReadOnly);
  PString binding;
  PINDEX idx = 0;
  while (bindRegistry.EnumValue(idx++, binding)) {
    if (binding.Left(6) *= "MSTCP\\") {
      RegistryKey mstcpRegistry("HKEY_LOCAL_MACHINE\\Enum\\Network\\" + binding, RegistryKey::ReadOnly);
      PString str;
      if (mstcpRegistry.QueryValue("Driver", str))
        transportBinding.AppendString(SERVICES_REGISTRY_KEY "Class\\" + str);
    }
  }
  return TRUE;
}
#endif // !_WIN32_WCE


BOOL PWin32PacketVxD::EnumIpAddress(PINDEX idx,
                                    PIPSocket::Address & addr,
                                    PIPSocket::Address & net_mask)
{
  if (idx >= transportBinding.GetSize())
    return FALSE;

  RegistryKey transportRegistry(transportBinding[idx], RegistryKey::ReadOnly);
  PString str;
  if (transportRegistry.QueryValue("IPAddress", str))
    addr = str;
  else
    addr = 0;

  if (addr != 0) {
    if (addr.GetVersion() == 6) {
      net_mask = 0;
      // Seb: Something to do ?
    } else {
      if (transportRegistry.QueryValue("IPMask", str))
        net_mask = str;
      else {
        if (IN_CLASSA(addr))
          net_mask = "255.0.0.0";
        else if (IN_CLASSB(addr))
          net_mask = "255.255.0.0";
        else if (IN_CLASSC(addr))
          net_mask = "255.255.255.0";
        else
          net_mask = 0;
      }
    }
    return TRUE;
  }

  PEthSocket::Address macAddress;
  if (!QueryOid(OID_802_3_CURRENT_ADDRESS, sizeof(macAddress), macAddress.b))
    return FALSE;

  PINDEX dhcpCount;
  for (dhcpCount = 0; dhcpCount < 8; dhcpCount++) {
    RegistryKey dhcpRegistry(psprintf(SERVICES_REGISTRY_KEY "VxD\\DHCP\\DhcpInfo%02u", dhcpCount),
                             RegistryKey::ReadOnly);
    if (dhcpRegistry.QueryValue("DhcpInfo", str)) {
      struct DhcpInfo {
        DWORD index;
        PIPSocket::Address ipAddress;
        PIPSocket::Address mask;
        PIPSocket::Address server;
        PIPSocket::Address anotherAddress;
        DWORD unknown1;
        DWORD unknown2;
        DWORD unknown3;
        DWORD unknown4;
        DWORD unknown5;
        DWORD unknown6;
        BYTE  unknown7;
        PEthSocket::Address macAddress;
      } * dhcpInfo = (DhcpInfo *)(const char *)str;
      if (dhcpInfo->macAddress == macAddress) {
        addr = dhcpInfo->ipAddress;
        net_mask = dhcpInfo->mask;
        return TRUE;
      }
    }
    else if (dhcpRegistry.QueryValue("HardwareAddress", str) &&
             str.GetSize() >= sizeof(PEthSocket::Address)) {
      PEthSocket::Address hardwareAddress;
      memcpy(&hardwareAddress, (const char *)str, sizeof(hardwareAddress));
      if (hardwareAddress == macAddress) {
        if (dhcpRegistry.QueryValue("DhcpIPAddress", str) &&
            str.GetSize() >= sizeof(addr)) {
          memcpy(&addr, (const char *)str, sizeof(addr));
          if (dhcpRegistry.QueryValue("DhcpSubnetMask", str) &&
              str.GetSize() >= sizeof(net_mask)) {
            memcpy(&net_mask, (const char *)str, sizeof(net_mask));
            return TRUE;
          }
        }
      }
    }
  }

  return FALSE;
}


BOOL PWin32PacketVxD::BeginRead(void * buf, DWORD size, DWORD & received, PWin32Overlapped & overlap)
{
  received = 0;
  if (DeviceIoControl(hDriver, IOCTL_EPACKET_READ,
                      buf, size, buf, size, &received, &overlap)) {
    dwError = ERROR_SUCCESS;
    return TRUE;
  }

  dwError = ::GetLastError();
  return dwError == ERROR_IO_PENDING;
}


BOOL PWin32PacketVxD::BeginWrite(const void * buf, DWORD len, PWin32Overlapped & overlap)
{
  DWORD rxsize = 0;
  BYTE dummy[2];
  if (DeviceIoControl(hDriver, IOCTL_EPACKET_WRITE,
                      (void *)buf, len, dummy, sizeof(dummy), &rxsize, &overlap)) {
    dwError = ERROR_SUCCESS;
    return TRUE;
  }

  dwError = ::GetLastError();
  return dwError == ERROR_IO_PENDING;
}


///////////////////////////////////////////////////////////////////////////////

PWin32PacketSYS::PWin32PacketSYS()
{
#ifndef _WIN32_WCE
  // Start the packet driver service
  SC_HANDLE hManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
  if (hManager != NULL) {
    SC_HANDLE hService = OpenService(hManager, PACKET_SERVICE_NAME, SERVICE_START);
    if (hService != NULL) {
      StartService(hService, 0, NULL);
      dwError = ::GetLastError();
      CloseServiceHandle(hService);
    }
    CloseServiceHandle(hManager);
  }
#endif // !_WIN32_WCE
}


static const char PacketDeviceStr[] = "\\Device\\" PACKET_SERVICE_NAME "_";

#ifdef _WIN32_WCE
BOOL PWin32PacketSYS::EnumInterfaces(PINDEX, PString &)
{
  return FALSE;
}
#else
BOOL PWin32PacketSYS::EnumInterfaces(PINDEX idx, PString & name)
{
  RegistryKey registry(SERVICES_REGISTRY_KEY PACKET_SERVICE_NAME "\\Linkage",
                       RegistryKey::ReadOnly);
  if (!RegistryQueryMultiSz(registry, "Export", idx, name)) {
    dwError = ERROR_NO_MORE_ITEMS;
    return FALSE;
  }

  if (strncasecmp(name, PacketDeviceStr, sizeof(PacketDeviceStr)-1) == 0)
    name.Delete(0, sizeof(PacketDeviceStr)-1);

  return TRUE;
}
#endif // !_WIN32_WCE


#ifdef _WIN32_WCE
BOOL PWin32PacketSYS::BindInterface(const PString &)
{
  return FALSE;
}
#else
BOOL PWin32PacketSYS::BindInterface(const PString & interfaceName)
{
  Close();

  if (!DefineDosDevice(DDD_RAW_TARGET_PATH,
                       PACKET_SERVICE_NAME "_" + interfaceName,
                       PacketDeviceStr + interfaceName)) {
    dwError = ::GetLastError();
    return FALSE;
  }

  ::SetLastError(0);
  hDriver = CreateFile("\\\\.\\" PACKET_SERVICE_NAME "_" + interfaceName,
                       GENERIC_READ | GENERIC_WRITE,
                       0,
                       NULL,
                       CREATE_ALWAYS,
                       FILE_FLAG_OVERLAPPED,
                       NULL);
  if (hDriver == INVALID_HANDLE_VALUE) {
    dwError = ::GetLastError();
    return FALSE;
  }

  registryKey = SERVICES_REGISTRY_KEY + interfaceName + "\\Parameters\\Tcpip";
  dwError = ERROR_SUCCESS;

  return TRUE;
}
#endif // !_WIN32_WCE


BOOL PWin32PacketSYS::EnumIpAddress(PINDEX idx,
                                    PIPSocket::Address & addr,
                                    PIPSocket::Address & net_mask)
{
  PString str;
  RegistryKey registry(registryKey, RegistryKey::ReadOnly);

  if (!RegistryQueryMultiSz(registry, "IPAddress", idx, str)) {
    dwError = ERROR_NO_MORE_ITEMS;
    return FALSE;
  }
  addr = str;

  if (!RegistryQueryMultiSz(registry, "SubnetMask", idx, str)) {
    dwError = ERROR_NO_MORE_ITEMS;
    return FALSE;
  }
  net_mask = str;

  return TRUE;
}


BOOL PWin32PacketSYS::BeginRead(void * buf, DWORD size, DWORD & received, PWin32Overlapped & overlap)
{
  overlap.Reset();
  received = 0;

  if (ReadFile(hDriver, buf, size, &received, &overlap)) {
    dwError = ERROR_SUCCESS;
    return TRUE;
  }

  return (dwError = ::GetLastError()) == ERROR_IO_PENDING;
}


BOOL PWin32PacketSYS::BeginWrite(const void * buf, DWORD len, PWin32Overlapped & overlap)
{
  overlap.Reset();
  DWORD sent = 0;
  if (WriteFile(hDriver, buf, len, &sent, &overlap)) {
    dwError = ERROR_SUCCESS;
    return TRUE;
  }

  dwError = ::GetLastError();
  return dwError == ERROR_IO_PENDING;
}

///////////////////////////////////////////////////////////////////////////////

#ifdef _WIN32_WCE
PWin32PacketCe::PWin32PacketCe()
{
  PString str, driver, nameStr, keyStr, driverStr, miniportStr, linkageStr, routeStr, tcpipStr;

  static const PString ActiveDrivers = "HKEY_LOCAL_MACHINE\\Drivers\\Active";
  static const PString CommBase = "HKEY_LOCAL_MACHINE\\Comm";

  // Collecting active drivers
  RegistryKey registry(ActiveDrivers, RegistryKey::ReadOnly);
  for (PINDEX idx = 0; registry.EnumKey(idx, str); idx++) 
  {
    driver = ActiveDrivers + "\\" + str;
    RegistryKey driverKey( driver, RegistryKey::ReadOnly );

    // Filter out non - NDS drivers
    if (!driverKey.QueryValue( "Name", nameStr ) || nameStr.Find("NDS") == P_MAX_INDEX )
      continue;

    // Active network driver found
    // 
    // e.g. built-in driver has "Key" = Drivers\BuiltIn\NDIS
    if( driverKey.QueryValue( "Key", keyStr ) )
    {
      if( P_MAX_INDEX != keyStr.Find("BuiltIn") )
      {
        // Built-in driver case
        continue;
      }
      else
      {
        driverStr = "HKEY_LOCAL_MACHINE\\"+ keyStr;
        RegistryKey ActiveDriverKey( driverStr, RegistryKey::ReadOnly );

        // Get miniport value
        if( ActiveDriverKey.QueryValue( "Miniport", miniportStr ) )
        {
          // Get miniport linkage
          //
          // e.g. [HKEY_LOCAL_MACHINE\Comm\SOCKETLPE\Linkage]
          linkageStr = CommBase + "\\" + miniportStr + "\\Linkage";

          RegistryKey LinkageKey( linkageStr, RegistryKey::ReadOnly );

          // Get route to real driver
          if( LinkageKey.QueryValue( "Route", routeStr ) )
          {
            tcpipStr = CommBase + "\\" + routeStr + "\\Parms\\TcpIp";

            RegistryKey TcpIpKey( tcpipStr, RegistryKey::ReadOnly );

            DWORD dwDHCPEnabled = FALSE;
            TcpIpKey.QueryValue( "EnableDHCP", dwDHCPEnabled, TRUE );

            /// Collect IP addresses and net masks
            PString ipAddress, netMask;
            if ( !dwDHCPEnabled )
            {
              if  (TcpIpKey.QueryValue( "IpAddress", ipAddress ) 
                  && (ipAddress != "0.0.0.0") )
              {
                interfaces[interfaces.GetSize()] = tcpipStr; // Registry key for the driver
                ipAddresses[ipAddresses.GetSize()] = ipAddress; // It's IP
                if( driverKey.QueryValue( "Subnetmask", netMask ) )
                  netMasks[netMasks.GetSize()] = netMask; // It's mask
                else
                  netMasks[netMasks.GetSize()] = "255.255.255.0";
              }
            }
            else // DHCP enabled
            if( TcpIpKey.QueryValue( "DhcpIpAddress", ipAddress ) 
              && (ipAddress != "0.0.0.0") )
            {
              interfaces[interfaces.GetSize()] = str;
              ipAddresses[ipAddresses.GetSize()] = ipAddress;
              if( driverKey.QueryValue( "DhcpSubnetMask", netMask ) )
                netMasks[netMasks.GetSize()] = netMask;
              else
                netMasks[netMasks.GetSize()] = "255.255.255.0";
            }
          }
        }
      }      
    }
  }
}

BOOL PWin32PacketCe::EnumInterfaces(PINDEX idx, PString & name)
{
  if( idx >= interfaces.GetSize() )
    return FALSE;
  
  name = interfaces[idx];
  return TRUE;
}


BOOL PWin32PacketCe::BindInterface(const PString &)
{
  return TRUE;
}


BOOL PWin32PacketCe::EnumIpAddress(PINDEX idx,
                                    PIPSocket::Address & addr,
                                    PIPSocket::Address & net_mask)
{
  if( idx >= interfaces.GetSize() )
    return FALSE;

  addr = ipAddresses[idx];
  net_mask = netMasks[idx];
  return TRUE;
}


BOOL PWin32PacketCe::BeginRead(void *, DWORD, DWORD & , PWin32Overlapped &)
{
  return TRUE;
}


BOOL PWin32PacketCe::BeginWrite(const void *, DWORD, PWin32Overlapped &)
{
  return TRUE;
}

#endif // _WIN32_WCE

///////////////////////////////////////////////////////////////////////////////

PEthSocket::PEthSocket(PINDEX nReadBuffers, PINDEX nWriteBuffers, PINDEX size)
  : readBuffers(min(nReadBuffers, MAXIMUM_WAIT_OBJECTS)),
    writeBuffers(min(nWriteBuffers, MAXIMUM_WAIT_OBJECTS))
{
  driver = PWin32PacketDriver::Create();
  PINDEX i;
  for (i = 0; i < nReadBuffers; i++)
    readBuffers.SetAt(i, new PWin32PacketBuffer(size));
  for (i = 0; i < nWriteBuffers; i++)
    writeBuffers.SetAt(i, new PWin32PacketBuffer(size));

  filterType = TypeAll;
}


PEthSocket::~PEthSocket()
{
  Close();

  delete driver;
}


BOOL PEthSocket::OpenSocket()
{
  PAssertAlways(PUnimplementedFunction);
  return FALSE;
}


BOOL PEthSocket::Close()
{
  driver->Close();
  os_handle = -1;
  return TRUE;
}


PString PEthSocket::GetName() const
{
  return interfaceName;
}


BOOL PEthSocket::Connect(const PString & newName)
{
  Close();

  if (!driver->BindInterface(newName))
    return SetErrorValues(Miscellaneous, driver->GetLastError()|PWIN32ErrorFlag);

  interfaceName = newName;
  os_handle = 1;
  return TRUE;
}


BOOL PEthSocket::EnumInterfaces(PINDEX idx, PString & name)
{
  return driver->EnumInterfaces(idx, name);
}


BOOL PEthSocket::GetAddress(Address & addr)
{
  if (driver->QueryOid(OID_802_3_CURRENT_ADDRESS, sizeof(addr), addr.b))
    return TRUE;

  return SetErrorValues(Miscellaneous, driver->GetLastError()|PWIN32ErrorFlag);
}


BOOL PEthSocket::EnumIpAddress(PINDEX idx,
                               PIPSocket::Address & addr,
                               PIPSocket::Address & net_mask)
{
  if (IsOpen()) {
    if (driver->EnumIpAddress(idx, addr, net_mask))
      return TRUE;

    return SetErrorValues(NotFound, ENOENT);
  }

  return SetErrorValues(NotOpen, EBADF);
}


static const struct {
  unsigned pwlib;
  DWORD    ndis;
} FilterMasks[] = {
  { PEthSocket::FilterDirected,     NDIS_PACKET_TYPE_DIRECTED },
  { PEthSocket::FilterMulticast,    NDIS_PACKET_TYPE_MULTICAST },
  { PEthSocket::FilterAllMulticast, NDIS_PACKET_TYPE_ALL_MULTICAST },
  { PEthSocket::FilterBroadcast,    NDIS_PACKET_TYPE_BROADCAST },
  { PEthSocket::FilterPromiscuous,  NDIS_PACKET_TYPE_PROMISCUOUS }
};


BOOL PEthSocket::GetFilter(unsigned & mask, WORD & type)
{
  if (!IsOpen())
    return SetErrorValues(NotOpen, EBADF);

  DWORD filter = 0;
  if (!driver->QueryOid(OID_GEN_CURRENT_PACKET_FILTER, filter))
    return SetErrorValues(Miscellaneous, driver->GetLastError()|PWIN32ErrorFlag);

  if (filter == 0)
    return PEthSocket::FilterDirected;

  mask = 0;
  for (PINDEX i = 0; i < PARRAYSIZE(FilterMasks); i++) {
    if ((filter&FilterMasks[i].ndis) != 0)
      mask |= FilterMasks[i].pwlib;
  }

  type = (WORD)filterType;
  return TRUE;
}


BOOL PEthSocket::SetFilter(unsigned filter, WORD type)
{
  if (!IsOpen())
    return SetErrorValues(NotOpen, EBADF);

  DWORD bits = 0;
  for (PINDEX i = 0; i < PARRAYSIZE(FilterMasks); i++) {
    if ((filter&FilterMasks[i].pwlib) != 0)
      bits |= FilterMasks[i].ndis;
  }

  if (!driver->SetOid(OID_GEN_CURRENT_PACKET_FILTER, bits))
    return SetErrorValues(Miscellaneous, driver->GetLastError()|PWIN32ErrorFlag);

  filterType = type;
  return TRUE;
}


PEthSocket::MediumTypes PEthSocket::GetMedium()
{
  if (!IsOpen()) {
    SetErrorValues(NotOpen, EBADF);
    return NumMediumTypes;
  }

  DWORD medium = 0xffffffff;
  if (!driver->QueryOid(OID_GEN_MEDIA_SUPPORTED, medium) || medium == 0xffffffff) {
    SetErrorValues(Miscellaneous, driver->GetLastError()|PWIN32ErrorFlag);
    return NumMediumTypes;
  }

  static const DWORD MediumValues[NumMediumTypes] = {
    0xffffffff, NdisMedium802_3, NdisMediumWan, 0xffffffff
  };

  for (int type = Medium802_3; type < NumMediumTypes; type++) {
    if (MediumValues[type] == medium)
      return (MediumTypes)type;
  }

  return MediumUnknown;
}


BOOL PEthSocket::Read(void * data, PINDEX length)
{
  if (!IsOpen())
    return SetErrorValues(NotOpen, EBADF, LastReadError);

  PINDEX idx;
  PINDEX numBuffers = readBuffers.GetSize();

  do {
    HANDLE handles[MAXIMUM_WAIT_OBJECTS];

    for (idx = 0; idx < numBuffers; idx++) {
      PWin32PacketBuffer & buffer = readBuffers[idx];
      if (buffer.InProgress()) {
        if (WaitForSingleObject(buffer.GetEvent(), 0) == WAIT_OBJECT_0)
          if (!buffer.ReadComplete(*driver))
            return ConvertOSError(-1, LastReadError);
      }
      else {
        if (!buffer.ReadAsync(*driver))
          return ConvertOSError(-1, LastReadError);
      }

      if (buffer.IsCompleted() && buffer.IsType(filterType)) {
        lastReadCount = buffer.GetData(data, length);
        return TRUE;
      }

      handles[idx] = buffer.GetEvent();
    }

    DWORD result;
    PINDEX retries = 100;
    for (;;) {
      result = WaitForMultipleObjects(numBuffers, handles, FALSE, INFINITE);
      if (result >= WAIT_OBJECT_0 && result < WAIT_OBJECT_0 + (DWORD)numBuffers)
        break;

      if (::GetLastError() != ERROR_INVALID_HANDLE || retries == 0)
        return ConvertOSError(-1, LastReadError);

      retries--;
    }

    idx = result - WAIT_OBJECT_0;
    if (!readBuffers[idx].ReadComplete(*driver))
      return ConvertOSError(-1, LastReadError);

  } while (!readBuffers[idx].IsType(filterType));

  lastReadCount = readBuffers[idx].GetData(data, length);
  return TRUE;
}


BOOL PEthSocket::Write(const void * data, PINDEX length)
{
  if (!IsOpen())
    return SetErrorValues(NotOpen, EBADF, LastWriteError);

  HANDLE handles[MAXIMUM_WAIT_OBJECTS];
  PINDEX numBuffers = writeBuffers.GetSize();

  PINDEX idx;
  for (idx = 0; idx < numBuffers; idx++) {
    PWin32PacketBuffer & buffer = writeBuffers[idx];
    if (buffer.InProgress()) {
      if (WaitForSingleObject(buffer.GetEvent(), 0) == WAIT_OBJECT_0)
        if (!buffer.WriteComplete(*driver))
          return ConvertOSError(-1, LastWriteError);
    }

    if (!buffer.InProgress()) {
      lastWriteCount = buffer.PutData(data, length);
      return ConvertOSError(buffer.WriteAsync(*driver) ? 0 : -1, LastWriteError);
    }

    handles[idx] = buffer.GetEvent();
  }

  DWORD result = WaitForMultipleObjects(numBuffers, handles, FALSE, INFINITE);
  if (result < WAIT_OBJECT_0 || result >= WAIT_OBJECT_0 + (DWORD) numBuffers)
    return ConvertOSError(-1, LastWriteError);

  idx = result - WAIT_OBJECT_0;
  if (!writeBuffers[idx].WriteComplete(*driver))
    return ConvertOSError(-1, LastWriteError);

  lastWriteCount = writeBuffers[idx].PutData(data, length);
  return ConvertOSError(writeBuffers[idx].WriteAsync(*driver) ? 0 : -1, LastWriteError);
}


///////////////////////////////////////////////////////////////////////////////

PWin32PacketBuffer::PWin32PacketBuffer(PINDEX sz)
  : PBYTEArray(sz)
{
  status = Uninitialised;
  count = 0;
}


PINDEX PWin32PacketBuffer::GetData(void * buf, PINDEX size)
{
  if (count > (DWORD)size)
    count = size;

  memcpy(buf, theArray, count);

  return count;
}


PINDEX PWin32PacketBuffer::PutData(const void * buf, PINDEX length)
{
  count = min(GetSize(), length);

  memcpy(theArray, buf, count);

  return count;
}


BOOL PWin32PacketBuffer::ReadAsync(PWin32PacketDriver & pkt)
{
  if (status == Progressing)
    return FALSE;

  status = Uninitialised;
  if (!pkt.BeginRead(theArray, GetSize(), count, overlap))
    return FALSE;

  if (pkt.GetLastError() == ERROR_SUCCESS)
    status = Completed;
  else
    status = Progressing;
  return TRUE;
}


BOOL PWin32PacketBuffer::ReadComplete(PWin32PacketDriver & pkt)
{
  if (status != Progressing)
    return status == Completed;

  if (!pkt.CompleteIO(count, overlap)) {
    status = Uninitialised;
    return FALSE;
  }

  status = Completed;
  return TRUE;
}


BOOL PWin32PacketBuffer::WriteAsync(PWin32PacketDriver & pkt)
{
  if (status == Progressing)
    return FALSE;

  status = Uninitialised;
  if (!pkt.BeginWrite(theArray, count, overlap))
    return FALSE;

  if (pkt.GetLastError() == ERROR_SUCCESS)
    status = Completed;
  else
    status = Progressing;
  return TRUE;
}


BOOL PWin32PacketBuffer::WriteComplete(PWin32PacketDriver & pkt)
{
  if (status != Progressing)
    return status == Completed;

  DWORD dummy;
  if (pkt.CompleteIO(dummy, overlap)) {
    status = Completed;
    return TRUE;
  }

  status = Uninitialised;
  return FALSE;
}


BOOL PWin32PacketBuffer::IsType(WORD filterType) const
{
  if (filterType == PEthSocket::TypeAll)
    return TRUE;

  const PEthSocket::Frame * frame = (const PEthSocket::Frame *)theArray;

  WORD len_or_type = ntohs(frame->snap.length);
  if (len_or_type > sizeof(*frame))
    return len_or_type == filterType;

  if (frame->snap.dsap == 0xaa && frame->snap.ssap == 0xaa)
    return ntohs(frame->snap.type) == filterType;   // SNAP header

  if (frame->snap.dsap == 0xff && frame->snap.ssap == 0xff)
    return PEthSocket::TypeIPX == filterType;   // Special case for Novell netware's stuffed up 802.3

  if (frame->snap.dsap == 0xe0 && frame->snap.ssap == 0xe0)
    return PEthSocket::TypeIPX == filterType;   // Special case for Novell netware's 802.2

  return frame->snap.dsap == filterType;    // A pure 802.2 protocol id
}

///////////////////////////////////////////////////////////////////////////////

/*
static PMutex & GetSNMPMutex()
{
  static PMutex snmpmutex;
  return snmpmutex;
}
*/

BOOL PIPSocket::GetGatewayAddress(Address & addr)
{
  PWaitAndSignal m(GetSNMPMutex());
  PWin32SnmpLibrary & snmp = PWin32SnmpLibrary::Current();

  PWin32AsnOid gatewayOid = "1.3.6.1.2.1.4.21.1.7.0.0.0.0";
  return snmp.GetOid(gatewayOid, addr);
}


PString PIPSocket::GetGatewayInterface()
{
  PWaitAndSignal m(GetSNMPMutex());

  PWin32SnmpLibrary & snmp = PWin32SnmpLibrary::Current();

  AsnInteger ifNum = -1;
  PWin32AsnOid gatewayOid = "1.3.6.1.2.1.4.21.1.2.0.0.0.0";
  if (!snmp.GetOid(gatewayOid, ifNum) && ifNum >= 0)
    return PString::Empty();

  return snmp.GetInterfaceName(ifNum);
}

PIPSocket::Address PIPSocket::GetGatewayInterfaceAddress()
{
  PWaitAndSignal m(GetSNMPMutex());

  PWin32SnmpLibrary & snmp = PWin32SnmpLibrary::Current();

  AsnInteger ifNum = -1;
  PWin32AsnOid gatewayOid = "1.3.6.1.2.1.4.21.1.2.0.0.0.0";
  if (!snmp.GetOid(gatewayOid, ifNum) && ifNum >= 0)
    return PString::Empty();

  return snmp.GetInterfaceAddress(ifNum);
}

BOOL PIPSocket::GetRouteTable(RouteTable & table)
{
  PWaitAndSignal m(GetSNMPMutex());

  PWin32SnmpLibrary & snmp = snmp.Current();
  table.RemoveAll();

  PWin32AsnOid baseOid = "1.3.6.1.2.1.4.21.1";
  PWin32AsnOid oid = baseOid;

  DWORD lastVariable = 1;
  PWin32AsnAny value;
  PLongArray ifNum;
  PINDEX idx = 0;

  while (snmp.GetNextOid(oid, value) && (baseOid *= oid)) {
    if (lastVariable != oid[9]) {
      lastVariable = oid[9];
      if (lastVariable == 2)
        ifNum.SetSize(table.GetSize());
      idx = 0;
    }

    switch (lastVariable) {
      case 1 : // network address
        {
          Address addr;
          if (!value.GetIpAddress(addr))
            return FALSE;  // Very confused route table

          table.Append(new RouteEntry(addr));
          break;
        }

      case 2 : // device interface
        if (!value.GetInteger(ifNum[idx]))
          return FALSE;
        break;

      case 3 : // metric
        if (!value.GetInteger(table[idx].metric))
          return FALSE;
        break;

      case 7 : // Get destination (next hop)
        if (!value.GetIpAddress(table[idx].destination))
          return FALSE;
        break;

      case 11 : // Get mask
        if (!value.GetIpAddress(table[idx].net_mask))
          return FALSE;
        break;
    }

    idx++;
  }

  for (idx = 0; idx < table.GetSize(); idx++)
    table[idx].interfaceName = snmp.GetInterfaceName(ifNum[idx]);

  return TRUE;
}

unsigned PIPSocket::AsNumeric(PIPSocket::Address addr)    
{ 
  return ((addr.Byte1() << 24) | (addr.Byte2()  << 16) |
           (addr.Byte3()  << 8) | addr.Byte4()); 
}

PIPSocket::Address PIPSocket::GetRouteAddress(PIPSocket::Address RemoteAddress)
{

Address localaddr;

  if (!RemoteAddress.IsRFC1918()) {         // Remote Address is not Local
    if (!GetNetworkInterface(localaddr)) {       // User not connected directly to Internet
      localaddr = GetGatewayInterfaceAddress(); // Get the default Gateway NIC address
       if ( localaddr != 0 )            // No connection to the Internet?    
         return localaddr;
    }
  } else {
    PIPSocket::InterfaceTable interfaceTable;
    if (PIPSocket::GetInterfaceTable(interfaceTable)) {
      PINDEX i;
      for (i = 0; i < interfaceTable.GetSize(); ++i) {
        localaddr = interfaceTable[i].GetAddress();
        if (!localaddr.IsLoopback() && localaddr.IsRFC1918()) {
          if (IsAddressReachable(localaddr,
              interfaceTable[i].GetNetMask(),RemoteAddress))
                return localaddr;
        }
      }
    }
  }
  return 0;
}

BOOL PIPSocket::IsAddressReachable(PIPSocket::Address LocalIP,
                   PIPSocket::Address LocalMask, 
                   PIPSocket::Address RemoteIP)
{

  BYTE t = 255;
  int t1=t,t2=t,t3 =t,t4=t;
  int b1=0,b2=0,b3=0,b4=0;

  if ((int)LocalMask.Byte1() > 0)
  {  t1 = LocalIP.Byte1() + (t-LocalMask.Byte1()); b1 = LocalIP.Byte1();}
  
  if ((int)LocalMask.Byte2() > 0)
  {  t2 = LocalIP.Byte2() + (t-LocalMask.Byte2()); b2 = LocalIP.Byte2();}

  if ((int)LocalMask.Byte3() > 0)
  {  t3 = LocalIP.Byte3() + (t-LocalMask.Byte3()); b3 = LocalIP.Byte3();}

  if ((int)LocalMask.Byte4() > 0)
  {  t4 = LocalIP.Byte4() + (t-LocalMask.Byte4()); b4 = LocalIP.Byte4();}


  Address lt = Address((BYTE)t1,(BYTE)t2,(BYTE)t3,(BYTE)t4);
  Address lb = Address((BYTE)b1,(BYTE)b2,(BYTE)b3,(BYTE)b4);  

  if (AsNumeric(RemoteIP) > AsNumeric(lb) && 
        AsNumeric(lt) > AsNumeric(RemoteIP))
          return TRUE;

  return FALSE;
}

PString PIPSocket::GetInterface(PIPSocket::Address addr)
{
  PIPSocket::InterfaceTable if_table;

  if (PIPSocket::GetInterfaceTable( if_table ) ) {
    for (PINDEX i=0; i < if_table.GetSize(); i++) {
    PIPSocket::InterfaceEntry if_entry = if_table[i];
       if (if_entry.GetAddress() == addr) 
         return if_entry.GetName();
    }        
  }

  return PString();
}

BOOL PIPSocket::GetInterfaceTable(InterfaceTable & table, BOOL includeDown)
{
  PWin32SnmpLibrary & snmp = snmp.Current();

  PWaitAndSignal m(GetSNMPMutex());

  table.RemoveAll();

  if (!snmp.IsLoaded()) {
    // Error loading the SNMP library, fail safe to using whatever the
    // address of the local host is.
    Address ipAddr;
    if (!GetHostAddress(ipAddr))
      return FALSE;
    Address netMask(255,255,255,255);
    table.Append(new InterfaceEntry("FailSafe Interface", ipAddr, netMask, PString::Empty()));
    return TRUE;
  }

  // Sometime this returns no interfaces when one is going up or down
  // so we need to retry if it fails to find any.
  for (unsigned retry = 0; retry < 5; retry++) {
    PWin32AsnOid baseOid = "1.3.6.1.2.1.4.20.1";
    PWin32AsnOid oid = baseOid;
    PWin32AsnAny value;
    bool foundOne = false;
    while (snmp.GetNextOid(oid, value)) {
      if (!(baseOid *= oid))
        break;
      if (value.asnType != ASN_IPADDRESS)
        break;

      Address ipAddr;
      value.GetIpAddress(ipAddr);

      oid[9] = 3;
      Address netMask;
      if (!snmp.GetOid(oid, netMask))
        break;

      oid[9] = 2;
      AsnInteger ifIndex = -1;
      if (!snmp.GetOid(oid, ifIndex))
        break;

      PString macAddr;
      PEthSocket::Address ifPhysAddress("");
      PWin32AsnOid ifOid = "1.3.6.1.2.1.2.2.1.6.0";
      ifOid[10] = ifIndex;
      UINT len;
      if (snmp.GetOid(ifOid, &ifPhysAddress, sizeof(ifPhysAddress), &len) && len > 0)
        macAddr = ifPhysAddress;
    
      PString name = snmp.GetInterfaceName(ipAddr);
      if (name.IsEmpty()) {
        PWin32AsnOid nameOid = "1.3.6.1.2.1.2.2.1.2.0";
        nameOid[10] = ifIndex;
        if (!snmp.GetOid(nameOid, name))
          break;
        name.MakeMinimumSize();
      }

      if (!name.IsEmpty() && (includeDown || !(ipAddr.IsAny() || netMask.IsAny())))
        table.Append(new InterfaceEntry(name, ipAddr, netMask, macAddr));

      foundOne = true;
      oid[9] = 1;
    }

    if (foundOne)
      return TRUE;

    Sleep(50);
  }

  return TRUE;
}


///////////////////////////////////////////////////////////////////////////////
