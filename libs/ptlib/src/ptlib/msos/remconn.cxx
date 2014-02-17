/*
 * remconn.cxx
 *
 * Remote Networking Connection class implmentation for Win32 RAS.
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
 * $Log: remconn.cxx,v $
 * Revision 1.28  2004/04/03 08:22:22  csoutheren
 * Remove pseudo-RTTI and replaced with real RTTI
 *
 * Revision 1.27  2000/03/07 01:43:29  robertj
 * Removed double include of ras.h as no longer works with later versions.
 *
 * Revision 1.26  1999/06/27 04:21:59  robertj
 * Added more debugging to GetStatus() error reporting.
 *
 * Revision 1.25  1999/02/16 08:08:07  robertj
 * MSVC 6.0 compatibility changes.
 *
 * Revision 1.24  1998/11/30 04:50:16  robertj
 * New directory structure
 *
 * Revision 1.23  1998/10/15 05:41:51  robertj
 * New memory leak check code.
 *
 * Revision 1.22  1998/09/24 03:30:55  robertj
 * Added open software license.
 *
 * Revision 1.21  1998/07/24 06:51:56  robertj
 * Added ability to get IP number of RAS connection.
 *
 * Revision 1.20  1998/03/05 12:49:54  robertj
 * MemCheck fixes.
 *
 * Revision 1.19  1998/02/03 06:16:44  robertj
 * Added more error codes.
 *
 * Revision 1.18  1998/01/26 00:54:58  robertj
 * Added function to PRemoteConnection to get at OS error code.
 *
 * Revision 1.17  1997/04/06 07:45:28  robertj
 * Fixed bug in dialling already connected remotes.
 *
 * Revision 1.16  1997/04/01 06:00:06  robertj
 * Added Remove Configuration.
 *
 * Revision 1.15  1997/02/05 11:47:25  robertj
 * Fixed NT 3.51 support, again! (PAP compatibility)
 *
 * Revision 1.14  1997/01/25 02:22:47  robertj
 * Fixed backward compatibilty with NT3.51 and Win'95
 *
 * Revision 1.13  1997/01/12 04:14:39  robertj
 * Added ability to add/change new connections.
 *
 * Revision 1.12  1996/12/01 07:01:28  robertj
 * Changed debugging asserts to simple PError output.
 *
 * Revision 1.11  1996/11/16 10:53:17  robertj
 * Added missing SetlastError() so assert has correct error value.
 *
 * Revision 1.10  1996/11/10 21:02:46  robertj
 * Try doing double sample before flagging a RAS connection disconnected.
 *
 * Revision 1.9  1996/11/04 09:45:26  robertj
 * Yet more debugging.
 *
 * Revision 1.8  1996/11/04 03:37:23  robertj
 * Added more debugging for remote drop outs.
 *
 * Revision 1.7  1996/10/31 12:39:53  robertj
 * Added RCS keywords.
 *
 */

#include <ptlib.h>
#include <ptlib/remconn.h>

#define SizeWin400_RAS_MaxEntryName 256
#define SizeWin400_RAS_MaxPhoneNumber 128
#define SizeWin400_RAS_MaxCallbackNumber SizeWin400_RAS_MaxPhoneNumber
#define SizeWin400_RAS_MaxDeviceType 16
#define SizeWin400_RAS_MaxDeviceName 128

#define SizeWin400_RASCONN (sizeof(DWORD) + \
                            sizeof(HRASCONN) + \
                            SizeWin400_RAS_MaxEntryName + 1)

#define SizeWin400_RASDIALPARAMS (sizeof(DWORD) + \
                                  SizeWin400_RAS_MaxEntryName + 1 + \
                                  SizeWin400_RAS_MaxPhoneNumber + 1 + \
                                  SizeWin400_RAS_MaxCallbackNumber + 1 + \
                                  UNLEN + 1 + \
                                  PWLEN + 1 + \
                                  DNLEN + 1)

#define SizeWin400_RASCONNSTATUS (sizeof(DWORD) + \
                                  sizeof(RASCONNSTATE) + \
                                  sizeof(DWORD) + \
                                  SizeWin400_RAS_MaxDeviceType + 1 + \
                                  SizeWin400_RAS_MaxDeviceName + 1)



class PRASDLL : public PDynaLink
{
  PCLASSINFO(PRASDLL, PDynaLink)
  public:
    PRASDLL();

  DWORD (FAR PASCAL *Dial)(LPRASDIALEXTENSIONS,LPTSTR,LPRASDIALPARAMS,DWORD,LPVOID,LPHRASCONN);
  DWORD (FAR PASCAL *HangUp)(HRASCONN);
  DWORD (FAR PASCAL *GetConnectStatus)(HRASCONN,LPRASCONNSTATUS);
  DWORD (FAR PASCAL *EnumConnections)(LPRASCONN,LPDWORD,LPDWORD);
  DWORD (FAR PASCAL *EnumEntries)(LPTSTR,LPTSTR,LPRASENTRYNAME,LPDWORD,LPDWORD);
  DWORD (FAR PASCAL *GetEntryProperties)(LPTSTR, LPTSTR, LPRASENTRY, LPDWORD, LPBYTE, LPDWORD);
  DWORD (FAR PASCAL *SetEntryProperties)(LPTSTR, LPTSTR, LPRASENTRY, DWORD, LPBYTE, DWORD);
  DWORD (FAR PASCAL *DeleteEntry)(LPTSTR, LPTSTR);
  DWORD (FAR PASCAL *ValidateEntryName)(LPTSTR, LPTSTR);
  DWORD (FAR PASCAL *GetProjectionInfo)(HRASCONN, RASPROJECTION, LPVOID, LPDWORD);
} Ras;


PRASDLL::PRASDLL()
#ifdef _WIN32
  : PDynaLink("RASAPI32.DLL")
#else
  : PDynaLink("RASAPI16.DLL")
#endif
{
  if (!GetFunction("RasDialA", (Function &)Dial) ||
      !GetFunction("RasHangUpA", (Function &)HangUp) ||
      !GetFunction("RasGetConnectStatusA", (Function &)GetConnectStatus) ||
      !GetFunction("RasEnumConnectionsA", (Function &)EnumConnections) ||
      !GetFunction("RasEnumEntriesA", (Function &)EnumEntries))
    Close();

  GetFunction("RasGetEntryPropertiesA", (Function &)GetEntryProperties);
  GetFunction("RasSetEntryPropertiesA", (Function &)SetEntryProperties);
  GetFunction("RasDeleteEntryA", (Function &)DeleteEntry);
  GetFunction("RasValidateEntryNameA", (Function &)ValidateEntryName);
  GetFunction("RasGetProjectionInfoA", (Function &)GetProjectionInfo);
}


#define new PNEW


static BOOL IsWinVer401()
{
  OSVERSIONINFO verinf;
  verinf.dwOSVersionInfoSize = sizeof(verinf);
  GetVersionEx(&verinf);

  if (verinf.dwPlatformId != VER_PLATFORM_WIN32_NT)
    return FALSE;

  if (verinf.dwMajorVersion < 4)
    return FALSE;

  return TRUE;
}


PRemoteConnection::PRemoteConnection()
{
  Construct();
}


PRemoteConnection::PRemoteConnection(const PString & name)
  : remoteName(name)
{
  Construct();
}


PRemoteConnection::~PRemoteConnection()
{
  Close();
}


BOOL PRemoteConnection::Open(const PString & name,
                             const PString & user,
                             const PString & pass,
                             BOOL existing)
{
  if (name != remoteName) {
    Close();
    remoteName = name;
  }
  userName = user;
  password = pass;
  return Open(existing);
}


BOOL PRemoteConnection::Open(const PString & name, BOOL existing)
{
  if (name != remoteName) {
    Close();
    remoteName = name;
  }
  return Open(existing);
}


PObject::Comparison PRemoteConnection::Compare(const PObject & obj) const
{
  PAssert(PIsDescendant(&obj, PRemoteConnection), PInvalidCast);
  return remoteName.Compare(((const PRemoteConnection &)obj).remoteName);
}


PINDEX PRemoteConnection::HashFunction() const
{
  return remoteName.HashFunction();
}


void PRemoteConnection::Construct()
{
  rasConnection = NULL;
  osError = SUCCESS;
}


BOOL PRemoteConnection::Open(BOOL existing)
{
  Close();
  if (!Ras.IsLoaded())
    return FALSE;

  BOOL isVer401 = IsWinVer401();

  RASCONN connection;
  connection.dwSize = isVer401 ? sizeof(RASCONN) : SizeWin400_RASCONN;

  LPRASCONN connections = &connection;
  DWORD size = sizeof(connection);
  DWORD numConnections;

  osError = Ras.EnumConnections(connections, &size, &numConnections);
  if (osError == ERROR_BUFFER_TOO_SMALL) {
    connections = new RASCONN[size/connection.dwSize];
    connections[0].dwSize = connection.dwSize;
    osError = Ras.EnumConnections(connections, &size, &numConnections);
  }

  if (osError == 0) {
    for (DWORD i = 0; i < numConnections; i++) {
      if (remoteName == connections[i].szEntryName) {
        rasConnection = connections[i].hrasconn;
        break;
      }
    }
  }

  if (connections != &connection)
    delete [] connections;

  if (rasConnection != NULL && GetStatus() == Connected) {
    osError = 0;
    return TRUE;
  }
  rasConnection = NULL;

  if (existing)
    return FALSE;

  RASDIALPARAMS params;
  memset(&params, 0, sizeof(params));
  params.dwSize = isVer401 ? sizeof(params) : SizeWin400_RASDIALPARAMS;

  if (remoteName[0] != '.') {
    PAssert(remoteName.GetLength() < sizeof(params.szEntryName)-1, PInvalidParameter);
    strcpy(params.szEntryName, remoteName);
  }
  else {
    PAssert(remoteName.GetLength() < sizeof(params.szPhoneNumber), PInvalidParameter);
    strcpy(params.szPhoneNumber, remoteName(1, P_MAX_INDEX));
  }
  strcpy(params.szUserName, userName);
  strcpy(params.szPassword, password);

  osError = Ras.Dial(NULL, NULL, &params, 0, NULL, &rasConnection);
  if (osError == 0)
    return TRUE;

  if (rasConnection != NULL) {
    Ras.HangUp(rasConnection);
    rasConnection = NULL;
  }

  SetLastError(osError);
  return FALSE;
}


void PRemoteConnection::Close()
{
  if (rasConnection != NULL) {
    Ras.HangUp(rasConnection);
    rasConnection = NULL;
  }
}


static int GetRasStatus(HRASCONN rasConnection, DWORD & rasError)
{
  RASCONNSTATUS status;
  status.dwSize = IsWinVer401() ? sizeof(status) : SizeWin400_RASCONNSTATUS;

  rasError = Ras.GetConnectStatus(rasConnection, &status);
  if (rasError == ERROR_INVALID_HANDLE) {
    PError << "RAS Connection Status invalid handle, retrying.";
    rasError = Ras.GetConnectStatus(rasConnection, &status);
  }

  if (rasError == 0) {
    rasError = status.dwError;
    SetLastError(rasError);
    return status.rasconnstate;
  }

  PError << "RAS Connection Status failed (" << rasError << "), retrying.";
  rasError = Ras.GetConnectStatus(rasConnection, &status);
  if (rasError == 0)
    rasError = status.dwError;
  SetLastError(rasError);

  return -1;
}


PRemoteConnection::Status PRemoteConnection::GetStatus() const
{
  if (!Ras.IsLoaded())
    return NotInstalled;

  if (rasConnection == NULL) {
    switch (osError) {
      case SUCCESS :
        return Idle;
      case ERROR_CANNOT_FIND_PHONEBOOK_ENTRY :
        return NoNameOrNumber;
      case ERROR_LINE_BUSY :
        return LineBusy;
      case ERROR_NO_DIALTONE :
        return NoDialTone;
      case ERROR_NO_ANSWER :
      case ERROR_NO_CARRIER :
        return NoAnswer;
      case ERROR_PORT_ALREADY_OPEN :
      case ERROR_PORT_NOT_AVAILABLE :
        return PortInUse;
      case ERROR_ACCESS_DENIED :
      case ERROR_NO_DIALIN_PERMISSION :
      case ERROR_AUTHENTICATION_FAILURE :
        return AccessDenied;
      case ERROR_HARDWARE_FAILURE :
      case ERROR_PORT_OR_DEVICE :
        return HardwareFailure;
    }
    return GeneralFailure;
  }

  switch (GetRasStatus(rasConnection, ((PRemoteConnection*)this)->osError)) {
    case RASCS_Connected :
      return Connected;
    case RASCS_Disconnected :
      break;
    case -1 :
      return ConnectionLost;
    default :
      return InProgress;
  }

  PError << "RAS Connection Status disconnected, retrying.";
  switch (GetRasStatus(rasConnection, ((PRemoteConnection*)this)->osError)) {
    case RASCS_Connected :
      return Connected;
    case RASCS_Disconnected :
      return Idle;
    case -1 :
      return ConnectionLost;
  }
  return InProgress;
}


PString PRemoteConnection::GetAddress()
{
  if (Ras.GetProjectionInfo == NULL) {
    osError = ERROR_CALL_NOT_IMPLEMENTED;
    return PString();
  }

  if (rasConnection == NULL) {
    osError = ERROR_INVALID_HANDLE;
    return PString();
  }

  RASPPPIP ip;
  ip.dwSize = sizeof(ip);
  DWORD size = sizeof(ip);
  osError = Ras.GetProjectionInfo(rasConnection, RASP_PppIp, &ip, &size);
  if (osError != ERROR_SUCCESS)
    return PString();

  osError = ip.dwError;
  if (osError != ERROR_SUCCESS)
    return PString();

  return ip.szIpAddress;
}


PStringArray PRemoteConnection::GetAvailableNames()
{
  PStringArray array;
  if (!Ras.IsLoaded())
    return array;

  RASENTRYNAME entry;
  entry.dwSize = sizeof(RASENTRYNAME);

  LPRASENTRYNAME entries = &entry;
  DWORD size = sizeof(entry);
  DWORD numEntries;

  DWORD rasError = Ras.EnumEntries(NULL, NULL, entries, &size, &numEntries);

  if (rasError == ERROR_BUFFER_TOO_SMALL) {
    entries = new RASENTRYNAME[size/sizeof(RASENTRYNAME)];
    entries[0].dwSize = sizeof(RASENTRYNAME);
    rasError = Ras.EnumEntries(NULL, NULL, entries, &size, &numEntries);
  }

  if (rasError == 0) {
    array.SetSize(numEntries);
    for (DWORD i = 0; i < numEntries; i++)
      array[i] = entries[i].szEntryName;
  }

  if (entries != &entry)
    delete [] entries;

  return array;
}


PRemoteConnection::Status
      PRemoteConnection::GetConfiguration(Configuration & config)
{
  return GetConfiguration(remoteName, config);
}


static DWORD MyRasGetEntryProperties(const char * name, PBYTEArray & entrybuf)
{
  LPRASENTRY entry = (LPRASENTRY)entrybuf.GetPointer(sizeof(RASENTRY));
  entry->dwSize = sizeof(RASENTRY);

  DWORD entrySize = sizeof(RASENTRY);
  DWORD error = Ras.GetEntryProperties(NULL, (char *)name, entry, &entrySize, NULL, 0);
  if (error == ERROR_BUFFER_TOO_SMALL) {
    entry = (LPRASENTRY)entrybuf.GetPointer(entrySize);
    error = Ras.GetEntryProperties(NULL, (char *)name, entry, &entrySize, NULL, 0);
  }

  return error;
}


PRemoteConnection::Status
      PRemoteConnection::GetConfiguration(const PString & name,
                                          Configuration & config)
{
  if (!Ras.IsLoaded() || Ras.GetEntryProperties == NULL)
    return NotInstalled;

  PBYTEArray entrybuf;
  switch (MyRasGetEntryProperties(name, entrybuf)) {
    case 0 :
      break;

    case ERROR_CANNOT_FIND_PHONEBOOK_ENTRY :
      return NoNameOrNumber;

    default :
      return GeneralFailure;
  }

  LPRASENTRY entry = (LPRASENTRY)(const BYTE *)entrybuf;

  config.device = entry->szDeviceType + PString("/") + entry->szDeviceName;

  if ((entry->dwfOptions&RASEO_UseCountryAndAreaCodes) == 0)
    config.phoneNumber = entry->szLocalPhoneNumber;
  else
    config.phoneNumber = psprintf("+%u %s %s",
                       entry->dwCountryCode, entry->szAreaCode, entry->szLocalPhoneNumber);

  if ((entry->dwfOptions&RASEO_SpecificIpAddr) == 0)
    config.ipAddress = "";
  else
    config.ipAddress = psprintf("%u.%u.%u.%u",
                                entry->ipaddr.a, entry->ipaddr.b,
                                entry->ipaddr.c, entry->ipaddr.d);
  if ((entry->dwfOptions&RASEO_SpecificNameServers) == 0)
    config.dnsAddress = "";
  else
    config.dnsAddress = psprintf("%u.%u.%u.%u",
                                 entry->ipaddrDns.a, entry->ipaddrDns.b,
                                 entry->ipaddrDns.c, entry->ipaddrDns.d);

  config.script = entry->szScript;
  
  config.subEntries = entry->dwSubEntries;
  config.dialAllSubEntries = entry->dwDialMode == RASEDM_DialAll;

  return Connected;
}


PRemoteConnection::Status
      PRemoteConnection::SetConfiguration(const Configuration & config, BOOL create)
{
  return SetConfiguration(remoteName, config, create);
}


PRemoteConnection::Status
      PRemoteConnection::SetConfiguration(const PString & name,
                                          const Configuration & config,
                                          BOOL create)
{
  if (!Ras.IsLoaded() || Ras.SetEntryProperties == NULL || Ras.ValidateEntryName == NULL)
    return NotInstalled;

  PBYTEArray entrybuf;
  switch (MyRasGetEntryProperties(name, entrybuf)) {
    case 0 :
      break;

    case ERROR_CANNOT_FIND_PHONEBOOK_ENTRY :
      if (!create)
        return NoNameOrNumber;
      if (Ras.ValidateEntryName(NULL, (char *)(const char *)name) != 0)
        return GeneralFailure;
      break;

    default :
      return GeneralFailure;
  }

  LPRASENTRY entry = (LPRASENTRY)(const BYTE *)entrybuf;

  PINDEX barpos = config.device.Find('/');
  if (barpos == P_MAX_INDEX)
    strncpy(entry->szDeviceName, config.device, sizeof(entry->szDeviceName)-1);
  else {
    strncpy(entry->szDeviceType, config.device.Left(barpos),  sizeof(entry->szDeviceType)-1);
    strncpy(entry->szDeviceName, config.device.Mid(barpos+1), sizeof(entry->szDeviceName)-1);
  }

  strncpy(entry->szLocalPhoneNumber, config.phoneNumber, sizeof(entry->szLocalPhoneNumber)-1);

  PStringArray dots = config.ipAddress.Tokenise('.');
  if (dots.GetSize() != 4)
    entry->dwfOptions &= ~RASEO_SpecificIpAddr;
  else {
    entry->dwfOptions |= RASEO_SpecificIpAddr;
    entry->ipaddr.a = (BYTE)dots[0].AsInteger();
    entry->ipaddr.b = (BYTE)dots[1].AsInteger();
    entry->ipaddr.c = (BYTE)dots[2].AsInteger();
    entry->ipaddr.d = (BYTE)dots[3].AsInteger();
  }
  dots = config.dnsAddress.Tokenise('.');
  if (dots.GetSize() != 4)
    entry->dwfOptions &= ~RASEO_SpecificNameServers;
  else {
    entry->dwfOptions |= RASEO_SpecificNameServers;
    entry->ipaddrDns.a = (BYTE)dots[0].AsInteger();
    entry->ipaddrDns.b = (BYTE)dots[1].AsInteger();
    entry->ipaddrDns.c = (BYTE)dots[2].AsInteger();
    entry->ipaddrDns.d = (BYTE)dots[3].AsInteger();
  }

  strncpy(entry->szScript, config.script, sizeof(entry->szScript-1));

  entry->dwDialMode = config.dialAllSubEntries ? RASEDM_DialAll : RASEDM_DialAsNeeded;

  if (Ras.SetEntryProperties(NULL, (char *)(const char *)name,
                             entry, entrybuf.GetSize(), NULL, 0) != 0)
    return GeneralFailure;

  return Connected;
}


PRemoteConnection::Status
                  PRemoteConnection::RemoveConfiguration(const PString & name)
{
  if (!Ras.IsLoaded() || Ras.SetEntryProperties == NULL || Ras.ValidateEntryName == NULL)
    return NotInstalled;

  switch (Ras.DeleteEntry(NULL, (char *)(const char *)name)) {
    case 0 :
      return Connected;

    case ERROR_INVALID_NAME :
      return NoNameOrNumber;
  }

  return GeneralFailure;
}


// End of File ////////////////////////////////////////////////////////////////
