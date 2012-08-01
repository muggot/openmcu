/*
 * psockbun.cxx
 *
 * Socket and interface bundle code
 *
 * Portable Windows Library
 *
 * Copyright (C) 2007 Post Increment
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
 * The Initial Developer of the Original Code is Post Increment
 *
 * Contributor(s): ______________________________________.
 *
 * $Log: psockbun.cxx,v $
 * Revision 1.22  2007/10/12 11:11:58  csoutheren
 * Fix warning on gcc
 *
 * Revision 1.21  2007/10/12 03:52:15  rjongbloed
 * Fixed broken virtual by someone changing base class function signature,
 *   and the override is silently not called. pet hate #1 about C++!
 *
 * Revision 1.20  2007/10/12 00:27:22  rjongbloed
 * Added more logging
 *
 * Revision 1.19  2007/10/07 07:35:31  rjongbloed
 * Changed bundled sockets so does not return error if interface goes away it just
 *   blocks reads till the interface comes back, or is explicitly closed.
 * Also return error codes, rather than just a BOOL.
 *
 * Revision 1.18  2007/09/28 09:59:16  hfriederich
 * Allow to use PInterfaceMonitor without running monitor thread
 *
 * Revision 1.17  2007/09/25 14:27:51  hfriederich
 * Don't use STUN if interface filter is in use and STUN server is not
 * reachable through local binding. This avoids unnecessary timeouts.
 *
 * Revision 1.16  2007/09/22 04:32:03  rjongbloed
 * Fixed lock up on exit whena  gatekeeper is used.
 * Also fixed fatal "read error" (ECONNRESET) when send packet to a machine which
 *   is not listening on the specified port. No error is lgged but does not stop listener.
 *
 * Revision 1.15  2007/09/11 08:37:30  rjongbloed
 * Set thread name for Network Interface Monitor thread.
 *
 * Revision 1.14  2007/09/08 11:34:28  rjongbloed
 * Improved memory checking (leaks etc), especially when using MSVC debug library.
 *
 * Revision 1.13  2007/08/26 20:01:58  hfriederich
 * Allow to filter interfaces based on remote address
 *
 * Revision 1.12  2007/08/22 05:08:26  rjongbloed
 * Fixed issue where if a bundled socket using STUN to be on specific local address,
 *   eg sip an port 5060 can still accept calls from local network on that port.
 *
 * Revision 1.11  2007/07/22 04:03:32  rjongbloed
 * Fixed issues with STUN usage in socket bundling, now OpalTransport indicates
 *   if it wants local or NAT address/port for inclusion to outgoing PDUs.
 *
 * Revision 1.10  2007/07/03 08:55:18  rjongbloed
 * Fixed various issues with handling interfaces going up, eg not being added
 *   to currently active ReadFrom().
 * Added more logging.
 *
 * Revision 1.9  2007/07/01 15:23:00  dsandras
 * Removed accidental log message.
 *
 * Revision 1.7  2007/06/25 05:44:01  rjongbloed
 * Fixed numerous issues with "bound" managed socket, ie associating
 *   listeners to a specific named interface.
 *
 * Revision 1.6  2007/06/22 04:51:40  rjongbloed
 * Fixed missing mutex release in socket bundle interface monitor thread shut down.
 *
 * Revision 1.5  2007/06/17 03:17:52  rjongbloed
 * Added using empty interface string as "just use predefined fixed interface"
 *
 * Revision 1.4  2007/06/10 06:26:54  rjongbloed
 * Major enhancements to the "socket bundling" feature:
 *   singleton thread for monitoring network interfaces
 *   a generic API for anything to be informed of interface changes
 *   PChannel derived class for reading/writing to bundled sockets
 *   many new API functions
 *
 * Revision 1.3  2007/05/28 11:26:50  hfriederich
 * Fix compilation
 *
 * Revision 1.2  2007/05/22 11:50:57  csoutheren
 * Further implementation of socket bundle
 *
 * Revision 1.1  2007/05/21 06:07:17  csoutheren
 * Add new socket bundle code to be used to OpalUDPListener
 *
 */

//////////////////////////////////////////////////

#ifdef __GNUC__
#pragma implementation "psockbun.h"
#endif

#include <ptlib.h>
#include <ptclib/psockbun.h>
#include <ptclib/pstun.h>


#define new PNEW


//////////////////////////////////////////////////

PInterfaceMonitorClient::PInterfaceMonitorClient()
{
  PInterfaceMonitor::GetInstance().AddClient(this);
}


PInterfaceMonitorClient::~PInterfaceMonitorClient()
{
  PInterfaceMonitor::GetInstance().RemoveClient(this);
}


PStringArray PInterfaceMonitorClient::GetInterfaces(BOOL includeLoopBack, const PIPSocket::Address & destination)
{
  return PInterfaceMonitor::GetInstance().GetInterfaces(includeLoopBack, destination);
}


BOOL PInterfaceMonitorClient::GetInterfaceInfo(const PString & iface, InterfaceEntry & info)
{
  return PInterfaceMonitor::GetInstance().GetInterfaceInfo(iface, info);
}


//////////////////////////////////////////////////

static PMutex PInterfaceMonitorInstanceMutex;
static PInterfaceMonitor * PInterfaceMonitorInstance;

PInterfaceMonitor::PInterfaceMonitor(unsigned refresh, BOOL _runMonitorThread)
  : runMonitorThread(_runMonitorThread)
  , refreshInterval(refresh)
  , updateThread(NULL)
  , interfaceFilter(NULL)
{
  PInterfaceMonitorInstanceMutex.Wait();
  PAssert(PInterfaceMonitorInstance == NULL, PLogicError);
  PInterfaceMonitorInstance = this;
  PInterfaceMonitorInstanceMutex.Signal();
}


PInterfaceMonitor::~PInterfaceMonitor()
{
  Stop();
  
  delete interfaceFilter;
}


PInterfaceMonitor & PInterfaceMonitor::GetInstance()
{
  PInterfaceMonitorInstanceMutex.Wait();
  if (PInterfaceMonitorInstance == NULL) {
    static PInterfaceMonitor theInstance;
  }
  PInterfaceMonitorInstanceMutex.Signal();

  return *PInterfaceMonitorInstance;
}


BOOL PInterfaceMonitor::Start()
{
  PWaitAndSignal m(mutex);
  
  if (runMonitorThread && updateThread != NULL)
    return FALSE; // Already running

  PIPSocket::GetInterfaceTable(currentInterfaces);
  PTRACE(4, "IfaceMon\tInitial interface list:\n" << setfill('\n') << currentInterfaces << setfill(' '));

  if (runMonitorThread) {
    updateThread = new PThreadObj<PInterfaceMonitor>(*this, &PInterfaceMonitor::UpdateThreadMain);
    updateThread->SetThreadName("Network Interface Monitor");
  }
  
  return TRUE;
}


void PInterfaceMonitor::Stop()
{
  mutex.Wait();

  // shutdown the update thread
  if (updateThread != NULL) {
    threadRunning.Signal();

    mutex.Signal();
    updateThread->WaitForTermination();
    mutex.Wait();

    delete updateThread;
    updateThread = NULL;
  }

  mutex.Signal();
}


static BOOL IsInterfaceInList(const PIPSocket::InterfaceEntry & entry,
                              const PIPSocket::InterfaceTable & list)
{
  for (PINDEX i = 0; i < list.GetSize(); ++i) {
    PIPSocket::InterfaceEntry & listEntry = list[i];
    if ((entry.GetName() == listEntry.GetName()) && (entry.GetAddress() == listEntry.GetAddress()))
      return TRUE;
  }
  return FALSE;
}


static BOOL InterfaceListIsSubsetOf(const PIPSocket::InterfaceTable & subset,
                                    const PIPSocket::InterfaceTable & set)
{
  for (PINDEX i = 0; i < subset.GetSize(); ++i) {
    PIPSocket::InterfaceEntry & entry = subset[i];
    if (!IsInterfaceInList(entry, set))
      return FALSE;
  }

  return TRUE;
}


static BOOL CompareInterfaceLists(const PIPSocket::InterfaceTable & list1,
                                  const PIPSocket::InterfaceTable & list2)
{
  // if the sizes are different, then the list has changed. 
  if (list1.GetSize() != list2.GetSize())
    return FALSE;

  // ensure every element in list1 is in list2
  if (!InterfaceListIsSubsetOf(list1, list2))
    return FALSE;

  // ensure every element in list1 is in list2
  return InterfaceListIsSubsetOf(list2, list1);
}


void PInterfaceMonitor::RefreshInterfaceList()
{
  // get a new interface list
  PIPSocket::InterfaceTable newInterfaces;
  PIPSocket::GetInterfaceTable(newInterfaces);

  PWaitAndSignal m(mutex);

  // if changed, then update the internal list
  if (!CompareInterfaceLists(currentInterfaces, newInterfaces)) {

    PIPSocket::InterfaceTable oldInterfaces = currentInterfaces;
    currentInterfaces = newInterfaces;

    PTRACE(4, "IfaceMon\tInterface change detected, new list:\n" << setfill('\n') << currentInterfaces << setfill(' '));

    // look for interfaces to add that are in new list that are not in the old list
    PINDEX i;
    for (i = 0; i < newInterfaces.GetSize(); ++i) {
      PIPSocket::InterfaceEntry & newEntry = newInterfaces[i];
      if (!newEntry.GetAddress().IsLoopback() && !IsInterfaceInList(newEntry, oldInterfaces))
        OnAddInterface(newEntry);
    }

    // look for interfaces to remove that are in old list that are not in the new list
    for (i = 0; i < oldInterfaces.GetSize(); ++i) {
      PIPSocket::InterfaceEntry & oldEntry = oldInterfaces[i];
      if (!oldEntry.GetAddress().IsLoopback() && !IsInterfaceInList(oldEntry, newInterfaces))
        OnRemoveInterface(oldEntry);
    }
  }
}


void PInterfaceMonitor::UpdateThreadMain()
{
  PTRACE(4, "IfaceMon\tStarted interface monitor thread.");

  // check for interface changes periodically
  do {
    RefreshInterfaceList();
  } while (!threadRunning.Wait(refreshInterval));

  PTRACE(4, "IfaceMon\tFinished interface monitor thread.");
}


static PString MakeInterfaceDescription(const PIPSocket::InterfaceEntry & entry)
{
  return entry.GetAddress().AsString() + '%' + entry.GetName();
}


static BOOL SplitInterfaceDescription(const PString & iface,
                                      PIPSocket::Address & address,
                                      PString & name)
{
  if (iface.IsEmpty())
    return FALSE;

  PINDEX percent = iface.Find('%');
  switch (percent) {
    case 0 :
      address = PIPSocket::GetDefaultIpAny();
      name = iface.Mid(1);
      return !name.IsEmpty();

    case P_MAX_INDEX :
      address = iface;
      name = PString::Empty();
      return !address.IsAny();
  }

  if (iface[0] == '*')
    address = PIPSocket::GetDefaultIpAny();
  else
    address = iface.Left(percent);
  name = iface.Mid(percent+1);
  return !name.IsEmpty();
}


PStringArray PInterfaceMonitor::GetInterfaces(BOOL includeLoopBack, 
                                              const PIPSocket::Address & destination)
{
  PWaitAndSignal guard(mutex);
  
  PIPSocket::InterfaceTable ifaces = currentInterfaces;
  
  if (interfaceFilter != NULL && !destination.IsAny()) {
    ifaces = interfaceFilter->FilterInterfaces(destination, ifaces);
  }

  PStringArray names;

  names.SetSize(ifaces.GetSize());
  PINDEX count = 0;

  for (PINDEX i = 0; i < ifaces.GetSize(); ++i) {
    PIPSocket::InterfaceEntry & entry = ifaces[i];
    if (includeLoopBack || !entry.GetAddress().IsLoopback())
      names[count++] = MakeInterfaceDescription(entry);
  }

  names.SetSize(count);

  return names;
}


BOOL PInterfaceMonitor::IsValidBindingForDestination(const PIPSocket::Address & binding,
                                                     const PIPSocket::Address & destination)
{
  PWaitAndSignal guard(mutex);
  
  if (interfaceFilter == NULL)
    return TRUE;
  
  PIPSocket::InterfaceTable ifaces = currentInterfaces;
  ifaces = interfaceFilter->FilterInterfaces(destination, ifaces);
  for (PINDEX i = 0; i < ifaces.GetSize(); i++) {
    if (ifaces[i].GetAddress() == binding)
      return TRUE;
  }
  return FALSE;
}


BOOL PInterfaceMonitor::GetInterfaceInfo(const PString & iface, PIPSocket::InterfaceEntry & info)
{
  PIPSocket::Address addr;
  PString name;
  if (!SplitInterfaceDescription(iface, addr, name))
    return FALSE;

  PWaitAndSignal m(mutex);

  for (PINDEX i = 0; i < currentInterfaces.GetSize(); ++i) {
    PIPSocket::InterfaceEntry & entry = currentInterfaces[i];
    if ((addr.IsAny()   || entry.GetAddress() == addr) &&
        (name.IsEmpty() || entry.GetName().NumCompare(name) == EqualTo)) {
      info = entry;
      return TRUE;
    }
  }

  return FALSE;
}


void PInterfaceMonitor::SetInterfaceFilter(PInterfaceFilter * filter)
{
  PWaitAndSignal m(mutex);
  
  delete interfaceFilter;
  interfaceFilter = filter;
}


void PInterfaceMonitor::AddClient(PInterfaceMonitorClient * client)
{
  PWaitAndSignal m(mutex);

  if (currentClients.empty())
    Start();
  currentClients.push_back(client);
}


void PInterfaceMonitor::RemoveClient(PInterfaceMonitorClient * client)
{
  mutex.Wait();
  currentClients.remove(client);
  bool stop = currentClients.empty();
  mutex.Signal();
  if (stop)
    Stop();
}


void PInterfaceMonitor::OnAddInterface(const PIPSocket::InterfaceEntry & entry)
{
  PWaitAndSignal m(mutex);

  for (ClientList_T::iterator iter = currentClients.begin(); iter != currentClients.end(); ++iter) {
    PInterfaceMonitorClient * client = *iter;
    if (client->LockReadWrite()) {
      client->OnAddInterface(entry);
      client->UnlockReadWrite();
    }
  }
}


void PInterfaceMonitor::OnRemoveInterface(const PIPSocket::InterfaceEntry & entry)
{
  PWaitAndSignal m(mutex);

  for (ClientList_T::iterator iter = currentClients.begin(); iter != currentClients.end(); ++iter) {
    PInterfaceMonitorClient * client = *iter;
    if (client->LockReadWrite()) {
      client->OnRemoveInterface(entry);
      client->UnlockReadWrite();
    }
  }
}


//////////////////////////////////////////////////

PMonitoredSockets::PMonitoredSockets(BOOL reuseAddr, PSTUNClient * stunClient)
  : localPort(0)
  , reuseAddress(reuseAddr)
  , stun(stunClient)
  , opened(FALSE)
{
}


BOOL PMonitoredSockets::CreateSocket(SocketInfo & info, const PIPSocket::Address & binding)
{
  delete info.socket;
  info.socket = NULL;
  
  if (stun != NULL) {
    PIPSocket::Address address;
    WORD port;
    stun->GetServer(address, port);
    if (PInterfaceMonitor::GetInstance().IsValidBindingForDestination(binding, address)) {
      if (stun->CreateSocket(info.socket, binding, localPort)) {
        PTRACE(4, "MonSock\tCreated bundled UDP socket via STUN internal="
               << binding << ':' << info.socket->PUDPSocket::GetPort()
               << " external=" << info.socket->GetLocalAddress());
        return TRUE;
      }
    }
  }

  info.socket = new PUDPSocket;
  if (info.socket->Listen(binding, 0, localPort, reuseAddress?PIPSocket::CanReuseAddress:PIPSocket::AddressIsExclusive)) {
    PTRACE(4, "MonSock\tCreated bundled UDP socket " << binding << ':' << info.socket->GetPort());
    return true;
  }

  delete info.socket;
  info.socket = NULL;
  return false;
}


BOOL PMonitoredSockets::DestroySocket(SocketInfo & info)
{
  if (info.socket == NULL)
    return FALSE;

  BOOL result = info.socket->Close();
  PTRACE_IF(4, result, "MonSock\tClosed bundled UDP socket " << info.socket);

  // This is pretty ugly, but needed to make sure multi-threading doesn't crash
  unsigned failSafe = 100; // Approx. two seconds
  while (info.inUse) {
    UnlockReadWrite();
    PThread::Sleep(20);
    if (!LockReadWrite())
      return FALSE;
    if (--failSafe == 0) {
      PTRACE(1, "MonSock\tClose of bundled UDP socket " << info.socket << " taking too long.");
      break;
    }
  }

  delete info.socket;
  info.socket = NULL;

  return result;
}


BOOL PMonitoredSockets::GetSocketAddress(const SocketInfo & info,
                                         PIPSocket::Address & address,
                                         WORD & port,
                                         BOOL usingNAT) const
{
  if (info.socket == NULL)
    return FALSE;

  return usingNAT ? info.socket->GetLocalAddress(address, port)
                  : info.socket->PUDPSocket::GetLocalAddress(address, port);
}


PChannel::Errors PMonitoredSockets::WriteToSocket(const void * buf,
                                                  PINDEX len,
                                                  const PIPSocket::Address & addr,
                                                  WORD port,
                                                  const SocketInfo & info,
                                                  PINDEX & lastWriteCount)
{
  info.socket->WriteTo(buf, len, addr, port);
  lastWriteCount = info.socket->GetLastWriteCount();
  return info.socket->GetErrorCode(PChannel::LastWriteError);
}


PChannel::Errors PMonitoredSockets::ReadFromSocket(SocketInfo & info,
                                                   void * buf,
                                                   PINDEX len,
                                                   PIPSocket::Address & addr,
                                                   WORD & port,
                                                   PINDEX & lastReadCount,
                                                   const PTimeInterval & timeout)
{
  // Assume is already locked

  if (info.inUse) {
    PTRACE(2, "MonSock\tCannot read from multiple threads.");
    return PChannel::DeviceInUse;
  }

  info.inUse = true;

  while (opened) {
    PSocket::SelectList sockets;
    if (info.socket != NULL && info.socket->IsOpen())
      sockets += *info.socket;
    sockets += interfaceAddedSignal;

    UnlockReadWrite();

    PChannel::Errors errorCode = PSocket::Select(sockets, timeout);

    if (!LockReadWrite())
      return PChannel::NotOpen;

    switch (errorCode) {
      default :
        info.inUse = false;
        return errorCode;

      case PChannel::NotOpen : // Interface went down
        break;

      case PChannel::NoError :
        if (sockets.IsEmpty()) { // Timeout
          info.inUse = false;
          return PChannel::Timeout;
        }

        if (info.socket == NULL) // Interface went down
          break;

        if (info.socket->ReadFrom(buf, len, addr, port)) {
          lastReadCount = info.socket->GetLastReadCount();
          info.inUse = false;
          return PChannel::NoError;
        }

        switch (info.socket->GetErrorNumber(PChannel::LastReadError)) {
          case ECONNRESET :
          case ECONNREFUSED :
            PTRACE(2, "MonSock\tUDP Port on remote not ready.");
            break;

          case EMSGSIZE :
            PTRACE(2, "MonSock\tRead UDP packet too large for buffer of " << len << " bytes.");
            break;

          case EBADF : // Interface went down
          case EINTR :
          case EAGAIN : // Shouldn't happen, but it does.
            break;

          default:
            PTRACE(1, "MonSock\tSocket read UDP error ("
                   << info.socket->GetErrorNumber(PChannel::LastReadError) << "): "
                   << info.socket->GetErrorText(PChannel::LastReadError));

            info.inUse = false;
            return info.socket->GetErrorCode(PChannel::LastReadError);
        }
    }

    if (!interfaceAddedSignal.IsOpen())
      interfaceAddedSignal.Listen(); // Reset if this was used to break Select() block
  }

  info.inUse = false;
  return PChannel::NotOpen;
}


PMonitoredSockets * PMonitoredSockets::Create(const PString & iface, BOOL reuseAddr, PSTUNClient * stunClient)
{
  if (iface.IsEmpty() || iface == "*" || PIPSocket::Address(iface).IsAny())
    return new PMonitoredSocketBundle(reuseAddr, stunClient);
  else
    return new PSingleMonitoredSocket(iface, reuseAddr, stunClient);
}


//////////////////////////////////////////////////

PMonitoredSocketChannel::PMonitoredSocketChannel(const PMonitoredSocketsPtr & sock, BOOL shared)
  : socketBundle(sock)
  , sharedBundle(shared)
  , promiscuousReads(false)
  , closing(FALSE)
  , remotePort(0)
  , lastReceivedAddress(PIPSocket::GetDefaultIpAny())
  , lastReceivedPort(0)
{
}


BOOL PMonitoredSocketChannel::IsOpen() const
{
  return !closing && socketBundle != NULL && socketBundle->IsOpen();
}


BOOL PMonitoredSocketChannel::Close()
{
  closing = TRUE;
  return sharedBundle || socketBundle == NULL || socketBundle->Close();
}


BOOL PMonitoredSocketChannel::Read(void * buffer, PINDEX length)
{
  if (!IsOpen())
    return FALSE;

  do {
    PString iface = GetInterface();
    if (!SetErrorValues(socketBundle->ReadFromBundle(buffer, length,
                                                     lastReceivedAddress, lastReceivedPort,
                                                     iface, lastReadCount, readTimeout),
                        0, LastReadError))
      return FALSE;

    if (promiscuousReads)
      return TRUE;

    if (remoteAddress.IsAny())
      remoteAddress = lastReceivedAddress;
    if (remotePort == 0)
      remotePort = lastReceivedPort;

  } while (remoteAddress != lastReceivedAddress || remotePort != lastReceivedPort);
  return TRUE;
}


BOOL PMonitoredSocketChannel::Write(const void * buffer, PINDEX length)
{
  return IsOpen() &&
         SetErrorValues(socketBundle->WriteToBundle(buffer, length,
                                                    remoteAddress, remotePort,
                                                    GetInterface(), lastWriteCount),
                        0, LastWriteError);
}


void PMonitoredSocketChannel::SetInterface(const PString & iface)
{
  PIPSocket::InterfaceEntry info;
  if (socketBundle != NULL && socketBundle->GetInterfaceInfo(iface, info))
    currentInterface = MakeInterfaceDescription(info);
  else
    currentInterface = iface;
}


const PString & PMonitoredSocketChannel::GetInterface()
{
  if (currentInterface.Find('%') == P_MAX_INDEX)
    SetInterface(currentInterface);

  return currentInterface;
}


BOOL PMonitoredSocketChannel::GetLocal(PIPSocket::Address & address, WORD & port, BOOL usingNAT)
{
  return socketBundle->GetAddress(GetInterface(), address, port, usingNAT);
}


void PMonitoredSocketChannel::SetRemote(const PIPSocket::Address & address, WORD port)
{
  remoteAddress = address;
  remotePort = port;
}


void PMonitoredSocketChannel::SetRemote(const PString & hostAndPort)
{
  PINDEX colon = hostAndPort.Find(':');
  if (colon == P_MAX_INDEX)
    remoteAddress = hostAndPort;
  else {
    remoteAddress = hostAndPort.Left(colon);
    remotePort = PIPSocket::GetPortByService("udp", hostAndPort.Mid(colon+1));
  }
}


//////////////////////////////////////////////////

PMonitoredSocketBundle::PMonitoredSocketBundle(BOOL reuseAddr, PSTUNClient * stunClient)
  : PMonitoredSockets(reuseAddr, stunClient)
{
}


PMonitoredSocketBundle::~PMonitoredSocketBundle()
{
  Close();
}


BOOL PMonitoredSocketBundle::Open(WORD port)
{
  PSafeLockReadWrite guard(*this);

  if (IsOpen() && localPort != 0  && localPort == port)
    return TRUE;

  opened = TRUE;

  localPort = port;

  // Close and re-open all sockets
  while (!socketInfoMap.empty())
    CloseSocket(socketInfoMap.begin());

  PStringArray interfaces = GetInterfaces();
  for (PINDEX i = 0; i < interfaces.GetSize(); ++i)
    OpenSocket(interfaces[i]);

  return TRUE;
}


BOOL PMonitoredSocketBundle::Close()
{
  if (!LockReadWrite())
    return FALSE;

  opened = FALSE;

  while (!socketInfoMap.empty())
    CloseSocket(socketInfoMap.begin());
  interfaceAddedSignal.Close(); // Fail safe break out of Select()

  UnlockReadWrite();

  return TRUE;
}


BOOL PMonitoredSocketBundle::GetAddress(const PString & iface,
                                        PIPSocket::Address & address,
                                        WORD & port,
                                        BOOL usingNAT) const
{
  if (iface.IsEmpty()) {
    address = PIPSocket::GetDefaultIpAny();
    port = localPort;
    return TRUE;
  }

  PSafeLockReadOnly guard(*this);
  if (!guard.IsLocked())
    return FALSE;

  SocketInfoMap_T::const_iterator iter = socketInfoMap.find(iface);
  return iter != socketInfoMap.end() && GetSocketAddress(iter->second, address, port, usingNAT);
}


void PMonitoredSocketBundle::OpenSocket(const PString & iface)
{
  PIPSocket::Address binding;
  PString name;
  SplitInterfaceDescription(iface, binding, name);

  SocketInfo info;
  if (CreateSocket(info, binding)) {
    if (localPort == 0)
      localPort = info.socket->GetPort();
    socketInfoMap[iface] = info;
  }
}


void PMonitoredSocketBundle::CloseSocket(const SocketInfoMap_T::iterator & iterSocket)
{
  //Already locked by caller

  if (iterSocket == socketInfoMap.end())
    return;

  DestroySocket(iterSocket->second);
  socketInfoMap.erase(iterSocket);
}


PChannel::Errors PMonitoredSocketBundle::WriteToBundle(const void * buf,
                                                       PINDEX len,
                                                       const PIPSocket::Address & addr,
                                                       WORD port,
                                                       const PString & iface,
                                                       PINDEX & lastWriteCount)
{
  if (!LockReadWrite())
    return PChannel::NotOpen;

  PChannel::Errors errorCode = PChannel::NoError;

  if (iface.IsEmpty()) {
    for (SocketInfoMap_T::iterator iter = socketInfoMap.begin(); iter != socketInfoMap.end(); ++iter) {
      PChannel::Errors err = WriteToSocket(buf, len, addr, port, iter->second, lastWriteCount);
      if (err != PChannel::NoError)
        errorCode = err;
    }
  }
  else {
    SocketInfoMap_T::iterator iter = socketInfoMap.find(iface);
    if (iter != socketInfoMap.end())
      errorCode = WriteToSocket(buf, len, addr, port, iter->second, lastWriteCount);
    else
      errorCode = PChannel::NotFound;
  }

  UnlockReadWrite();

  return errorCode;
}


PChannel::Errors PMonitoredSocketBundle::ReadFromBundle(void * buf,
                                                        PINDEX len,
                                                        PIPSocket::Address & addr,
                                                        WORD & port,
                                                        PString & iface,
                                                        PINDEX & lastReadCount,
                                                        const PTimeInterval & timeout)
{
  PChannel::Errors errorCode = PChannel::NoError;

  if (!LockReadWrite())
    return PChannel::NotOpen;

  if (iface.IsEmpty()) {
    for (;;) {
      // If interface is empty, then grab the next datagram on any of the interfaces
      PSocket::SelectList readers;

      for (SocketInfoMap_T::iterator iter = socketInfoMap.begin(); iter != socketInfoMap.end(); ++iter) {
        if (iter->second.inUse) {
          PTRACE(2, "MonSock\tCannot read from multiple threads.");
          UnlockReadWrite();
          return PChannel::DeviceInUse;
        }
        if (iter->second.socket->IsOpen()) {
          readers += *iter->second.socket;
          iter->second.inUse = true;
        }
      }
      readers += interfaceAddedSignal;

      UnlockReadWrite();
      errorCode = PSocket::Select(readers, timeout);
      if (!LockReadWrite())
        return PChannel::NotOpen;

      PUDPSocket * socket = NULL;
      if (errorCode != PChannel::NoError) {
        PTRACE(2, "MonSock\tMulti-interface read select failure: " << errorCode);
      }
      else {
        socket = (PUDPSocket *)&readers[0];
        if (socket->ReadFrom(buf, len, addr, port))
          lastReadCount = socket->GetLastReadCount();
        else {
          errorCode = socket->GetErrorCode(PChannel::LastReadError);
          PTRACE(2, "MonSock\tUDP socket read failure: " << socket->GetErrorText(PChannel::LastReadError));
          if (errorCode == PChannel::NotOpen)
            socket->Close(); // If interface goes down, socket is not open to OS, but still is to us. Make them agree.
        }
      }

      for (SocketInfoMap_T::iterator iter = socketInfoMap.begin(); iter != socketInfoMap.end(); ++iter) {
        if (iter->second.socket == socket)
          iface = iter->first;
        iter->second.inUse = false;
      }

      if (interfaceAddedSignal.IsOpen())
        break;

      interfaceAddedSignal.Listen();
    }
  }
  else {
    // if interface is not empty, use that specific interface
    SocketInfoMap_T::iterator iter = socketInfoMap.find(iface);
    if (iter != socketInfoMap.end())
      errorCode = ReadFromSocket(iter->second, buf, len, addr, port, lastReadCount, timeout);
    else
      errorCode = PChannel::NotFound;
  }

  UnlockReadWrite();

  return errorCode;
}


void PMonitoredSocketBundle::OnAddInterface(const InterfaceEntry & entry)
{
  // Already locked
  if (opened) {
    OpenSocket(MakeInterfaceDescription(entry));
    PTRACE(3, "MonSock\tUDP socket bundle has added interface " << entry);
    interfaceAddedSignal.Close();
  }
}


void PMonitoredSocketBundle::OnRemoveInterface(const InterfaceEntry & entry)
{
  // Already locked
  if (opened) {
    CloseSocket(socketInfoMap.find(MakeInterfaceDescription(entry)));
    PTRACE(3, "MonSock\tUDP socket bundle has removed interface " << entry);
  }
}


//////////////////////////////////////////////////

PSingleMonitoredSocket::PSingleMonitoredSocket(const PString & _theInterface, BOOL reuseAddr, PSTUNClient * stunClient)
  : PMonitoredSocketBundle(reuseAddr, stunClient)
  , theInterface(_theInterface)
{
}


PSingleMonitoredSocket::~PSingleMonitoredSocket()
{
  DestroySocket(theInfo);
}


PStringArray PSingleMonitoredSocket::GetInterfaces(BOOL /*includeLoopBack*/, const PIPSocket::Address & /*destination*/)
{
  PSafeLockReadOnly guard(*this);

  PStringList names;
  if (!theEntry.GetAddress().IsAny())
    names.AppendString(MakeInterfaceDescription(theEntry));
  return names;
}


BOOL PSingleMonitoredSocket::Open(WORD port)
{
  PSafeLockReadWrite guard(*this);

  if (opened && theInfo.socket != NULL && theInfo.socket->IsOpen())
    return FALSE;

  opened = TRUE;

  localPort = port;

  if (theEntry.GetAddress().IsAny())
    GetInterfaceInfo(theInterface, theEntry);

  if (theEntry.GetAddress().IsAny())
    return TRUE;

  if (!CreateSocket(theInfo, theEntry.GetAddress()))
    return FALSE;

  localPort = theInfo.socket->PUDPSocket::GetPort();
  return TRUE;
}


BOOL PSingleMonitoredSocket::Close()
{
  PSafeLockReadWrite guard(*this);

  opened = FALSE;
  interfaceAddedSignal.Close(); // Fail safe break out of Select()
  return DestroySocket(theInfo);
}


BOOL PSingleMonitoredSocket::GetAddress(const PString & iface,
                                        PIPSocket::Address & address,
                                        WORD & port,
                                        BOOL usingNAT) const
{
  PSafeLockReadOnly guard(*this);

  return guard.IsLocked() && IsInterface(iface) && GetSocketAddress(theInfo, address, port, usingNAT);
}


PChannel::Errors PSingleMonitoredSocket::WriteToBundle(const void * buf,
                                                       PINDEX len,
                                                       const PIPSocket::Address & addr,
                                                       WORD port,
                                                       const PString & iface,
                                                       PINDEX & lastWriteCount)
{
  PSafeLockReadWrite guard(*this);

  if (guard.IsLocked() && theInfo.socket != NULL && IsInterface(iface))
    return WriteToSocket(buf, len, addr, port, theInfo, lastWriteCount);

  return PChannel::NotFound;
}


PChannel::Errors PSingleMonitoredSocket::ReadFromBundle(void * buf,
                                                        PINDEX len,
                                                        PIPSocket::Address & addr,
                                                        WORD & port,
                                                        PString & iface,
                                                        PINDEX & lastReadCount,
                                                        const PTimeInterval & timeout)
{
  if (!LockReadWrite())
    return PChannel::NotOpen;

  PChannel::Errors errorCode;
  if (IsInterface(iface))
    errorCode = ReadFromSocket(theInfo, buf, len, addr, port, lastReadCount, timeout);
  else
    errorCode = PChannel::NotFound;

  UnlockReadWrite();

  return errorCode;
}


void PSingleMonitoredSocket::OnAddInterface(const InterfaceEntry & entry)
{
  // Already locked

  PIPSocket::Address addr;
  PString name;
  if (!SplitInterfaceDescription(theInterface, addr, name))
    return;

  if (entry.GetAddress() == addr && entry.GetName().NumCompare(name) == EqualTo) {
    theEntry = entry;
    if (!Open(localPort))
      theEntry = InterfaceEntry();
    else {
      interfaceAddedSignal.Close();
      PTRACE(3, "MonSock\tBound UDP socket UP event on interface " << theEntry);
    }
  }
}


void PSingleMonitoredSocket::OnRemoveInterface(const InterfaceEntry & entry)
{
  // Already locked

  if (entry != theEntry)
    return;

  PTRACE(3, "MonSock\tBound UDP socket DOWN event on interface " << theEntry);
  theEntry = InterfaceEntry();
  DestroySocket(theInfo);
}


BOOL PSingleMonitoredSocket::IsInterface(const PString & iface) const
{
  if (iface.IsEmpty())
    return TRUE;

  PINDEX percent1 = iface.Find('%');
  PINDEX percent2 = theInterface.Find('%');

  if (percent1 != P_MAX_INDEX && percent2 != P_MAX_INDEX)
    return iface.Mid(percent1+1).NumCompare(theInterface.Mid(percent2+1)) == EqualTo;

  return PIPSocket::Address(iface.Left(percent1)) == PIPSocket::Address(theInterface.Left(percent2));
}
