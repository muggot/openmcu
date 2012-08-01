/*
 * remconn.CXX
 *
 * Remote network connection (ppp) class implementation
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
 * Revision 1.20  2004/02/22 03:36:41  ykiryanov
 * Added inclusion of signal.h on BeOS to define SIGINT
 *
 * Revision 1.19  2003/12/02 10:46:15  csoutheren
 * Added patch for Solaris, thanks to Michal Zygmuntowicz
 *
 * Revision 1.18  2002/10/10 04:43:44  robertj
 * VxWorks port, thanks Martijn Roest
 *
 * Revision 1.17  1998/11/30 21:51:49  robertj
 * New directory structure.
 *
 * Revision 1.16  1998/09/24 04:12:15  robertj
 * Added open software license.
 *
 */

#pragma implementation "remconn.h"

#include <ptlib.h>
#include <ptlib/pipechan.h>
#include <ptlib/remconn.h>

#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#ifdef P_VXWORKS
#include <socklib.h>
#endif // P_VXWORKS

#ifdef P_SOLARIS
#include <signal.h>
#endif

#ifdef __BEOS__
#include <signal.h>
#endif

#include "uerror.h"

static const PString RasStr      = "ras";
static const PString NumberStr   = "Number";
static const PCaselessString UsernameStr = "$USERID";
static const PCaselessString PasswordStr = "$PASSWORD";
static const PString AddressStr = "Address";
static const PString NameServerStr = "NameServer";

static const PString OptionsStr = "Options";

static const PString DeviceStr     = "Device";
static const PString DefaultDevice = "ppp0";

static const PString PPPDStr     = "PPPD";
static const PString DefaultPPPD = "pppd";

static const PString ChatStr     = "Chat";
static const PString DefaultChat = "chat";

static const PString PortStr     = "Port";
static const PString DefaultPort = "/dev/modem";

static const PString DialPrefixStr     = "DialPrefix";
static const PString DefaultDialPrefix = "ATDT";

static const PString LoginStr     = "Login";
static const PString DefaultLogin = "'' sername: $USERID assword: $PASSWORD";

static const PString TimeoutStr     = "TimeoutStr";
static const PString DefaultTimeout = "90";

static const PString PPPDOptsStr     = "PPPDOpts";
static const PString PPPDOpts        = "-detach";
static const PString DefaultPPPDOpts = "crtscts modem defaultroute lock";

static const PString BaudRateStr     = "BaudRate";
static const PString DefaultBaudRate = "57600";

static const PString ErrorsStr     = "Errors";
static const PString DefaultErrors = "ABORT 'NO CARRIER' ABORT BUSY ABORT 'NO DIALTONE'";

static const PString InitStr     = "Init";
static const PString DefaultInit = "'' ATE1Q0Z OK";


static const PXErrorStruct ErrorTable[] =
{
  // Remote connection errors
  { 1000, "Attempt to open remote connection with empty system name" },
  { 1001, "Attempt to open connection to unknown remote system"},
  { 1002, "pppd could not connect to remote system"},
};

static int PPPDeviceStatus(const char * devName);

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


BOOL PRemoteConnection::Open(const PString & name, BOOL existing)
{
  return Open(name, "", "", existing);
}

BOOL PRemoteConnection::Open(const PString & name,
                             const PString & user,
                             const PString & pword,
                             BOOL existing)
{
  userName = user;
  password = pword;

  // cannot open remote connection with an empty name
  if (name.IsEmpty()) {
    status = NoNameOrNumber;
    PProcess::PXShowSystemWarning(1000, ErrorTable[0].str);
    return FALSE;
  }

  // cannot open remote connection not in config file
  PConfig config(0, RasStr);
  PString phoneNumber;
  if ((phoneNumber = config.GetString(name, NumberStr, "")).IsEmpty()) {
    status = NoNameOrNumber;
    PProcess::PXShowSystemWarning(1001, ErrorTable[1].str);
    return FALSE;
  }

  // if there is a connection active, check to see if it has the same name
  if (pipeChannel != NULL &&
      pipeChannel->IsRunning() &&
      name == remoteName &&
      PPPDeviceStatus(deviceStr) > 0) {
    osError = errno;
    status = Connected;
    return TRUE;
  }
  osError = errno;

  if (existing)
    return FALSE;

  Close();

  // name        = name of configuration
  // sectionName = name of config section
  remoteName = name;

  ///////////////////////////////////////////
  //
  // get global options
  //
  config.SetDefaultSection(OptionsStr);
  deviceStr          = config.GetString(DeviceStr,     DefaultDevice);
  PString pppdStr    = config.GetString(PPPDStr,       DefaultPPPD);
  PString chatStr    = config.GetString(ChatStr,       DefaultChat);
  PString baudRate   = config.GetString(BaudRateStr,   DefaultBaudRate);
  PString chatErrs   = config.GetString(ErrorsStr,     DefaultErrors);
  PString modemInit  = config.GetString(InitStr,       DefaultInit);
  PString dialPrefix = config.GetString(DialPrefixStr, DefaultDialPrefix);
  PString pppdOpts   = config.GetString(PPPDOptsStr,   DefaultPPPDOpts);

  ///////////////////////////////////////////
  //
  // get remote system parameters
  //
  config.SetDefaultSection(remoteName);
  PString portName   = config.GetString(PortStr,
				config.GetString(OptionsStr, PortStr, DefaultPort));
  PString loginStr   = config.GetString(LoginStr,    DefaultLogin);
  PString timeoutStr = config.GetString(TimeoutStr,  DefaultTimeout);
  PINDEX timeout = timeoutStr.AsInteger();
  PString addressStr = config.GetString(AddressStr, "");
  PString nameServerStr = config.GetString(NameServerStr, "");


  ///////////////////////////////////////////
  //
  // start constructing the command argument array
  //
  PStringArray argArray;
  PINDEX argCount = 0;
  argArray[argCount++] = portName;
  argArray[argCount++] = baudRate;

  PStringArray tokens = PPPDOpts.Tokenise(' ');
  PINDEX i;
  for (i = 0; i < tokens.GetSize(); i++)
    argArray[argCount++] = tokens[i];

  tokens = pppdOpts.Tokenise(' ');
  for (i = 0; i < tokens.GetSize(); i++)
    argArray[argCount++] = tokens[i];

  if (!nameServerStr.IsEmpty()) {
    argArray[argCount++] = "ipparam";
    argArray[argCount++] = nameServerStr;
  }

  ///////////////////////////////////////////
  //
  // replace metastrings in the login string
  //
  loginStr.Replace(UsernameStr, user);
  loginStr.Replace(PasswordStr, pword);

  ///////////////////////////////////////////
  //
  // setup the chat command
  //
  PString chatCmd = chatErrs & modemInit & dialPrefix + phoneNumber & loginStr;
  if (!chatCmd.IsEmpty()) {
    argArray[argCount++] = "connect";
    argArray[argCount++] = chatStr & "-t" + timeoutStr & chatCmd;
  }

  if (!addressStr)
    argArray[argCount++] = addressStr + ":";

  ///////////////////////////////////////////
  //
  // instigate a dial using pppd
  //
  pipeChannel  = PNEW PPipeChannel(pppdStr, argArray);
  osError = errno;

  ///////////////////////////////////////////
  //
  //  wait until the dial succeeds, or times out
  //
  PTimer timer(timeout*1000);
  for (;;) {
    if (pipeChannel == NULL || !pipeChannel->IsRunning()) 
      break;

    if (PPPDeviceStatus(deviceStr) > 0) {
      osError = errno;
      return TRUE;
    }

    if (!timer.IsRunning())
      break;

    PThread::Current()->Sleep(1000);
  }
  osError = errno;

  ///////////////////////////////////////////
  //
  //  dial failed
  //
  Close();

  return FALSE;
}


PObject::Comparison PRemoteConnection::Compare(const PObject & obj) const
{
  return remoteName.Compare(((const PRemoteConnection &)obj).remoteName);
}


PINDEX PRemoteConnection::HashFunction() const
{
  return remoteName.HashFunction();
}


void PRemoteConnection::Construct()
{
  pipeChannel  = NULL;
  status       = Idle;
  osError      = 0;
}


BOOL PRemoteConnection::Open(BOOL existing)
{
  return Open(remoteName, existing);
}


void PRemoteConnection::Close()
{
  if (pipeChannel != NULL) {

    // give pppd a chance to clean up
    pipeChannel->Kill(SIGINT);

    PTimer timer(10*1000);
    for (;;) {
      if (!pipeChannel->IsRunning() ||
          (PPPDeviceStatus(deviceStr) <= 0) ||
          !timer.IsRunning())
        break;
      PThread::Current()->Sleep(1000);
    }

    // kill the connection for real
    delete pipeChannel;
    pipeChannel = NULL;
  }
}

PRemoteConnection::Status PRemoteConnection::GetStatus() const
{
  if (pipeChannel != NULL &&
      pipeChannel->IsRunning() &&
      PPPDeviceStatus(deviceStr) > 0) 
    return Connected;

  return Idle;
}

PStringArray PRemoteConnection::GetAvailableNames() 
{
  PStringArray names;

  // get the list of remote system names from the system config file
  PConfig config(0, RasStr);

  // remotes have section names of the form "Remote-x" where X is some
  // unique identifier, usually a number or the system name
  PStringList sections = config.GetSections();
  for (PINDEX i = 0; i < sections.GetSize(); i++) {
    PString sectionName = sections[i];
    if (sectionName != OptionsStr)
      names[names.GetSize()] = sectionName;
  }

  return names;
}

//
//  <0 = does not exist
//   0 = exists, but is down
//  >0 = exists, is up
//
static int PPPDeviceStatus(const char * devName)
{
#if defined(HAS_IFREQ)
  int skfd;
  struct ifreq ifr;

  // Create a channel to the NET kernel. 
  if ((skfd = socket(AF_INET, SOCK_DGRAM,0)) < 0) 
    return -1;

  // attempt to get the status of the ppp connection
  int stat;
  strcpy(ifr.ifr_name, devName);
  if (ioctl(skfd, SIOCGIFFLAGS, &ifr) < 0)
    stat = -1;
  else 
    stat = (ifr.ifr_flags & IFF_UP) ? 1 : 0;

  // attempt to get the status of the ppp connection
  close(skfd);
  return stat;
#else
#warning "No PPPDeviceExists implementation defined"
  return FALSE;
#endif
}


PRemoteConnection::Status PRemoteConnection::GetConfiguration(
                 Configuration & config  // Configuration of remote connection
               )
{
  return GetConfiguration(remoteName, config);
}


PRemoteConnection::Status PRemoteConnection::GetConfiguration(
                 const PString & name,   // Remote connection name to get configuration
                 Configuration & config  // Configuration of remote connection
               )
{
  if (name.IsEmpty())
    return NoNameOrNumber;

  PConfig cfg(0, RasStr);
  if (cfg.GetString(name, NumberStr, "").IsEmpty())
    return NoNameOrNumber;

  cfg.SetDefaultSection(name);

  config.device = cfg.GetString(OptionsStr, PortStr, DefaultPort);
  config.phoneNumber = cfg.GetString(NumberStr);
  config.ipAddress = cfg.GetString(AddressStr);
  config.dnsAddress = cfg.GetString(NameServerStr);
  config.script = cfg.GetString(LoginStr, DefaultLogin);
  config.subEntries = 0;
  config.dialAllSubEntries = FALSE;

  return Connected;
}


PRemoteConnection::Status PRemoteConnection::SetConfiguration(
                 const Configuration & config,  // Configuration of remote connection
                 BOOL create            // Flag to create connection if not present
               )
{
  return SetConfiguration(remoteName, config, create);
}


PRemoteConnection::Status PRemoteConnection::SetConfiguration(
                 const PString & name,          // Remote connection name to configure
                 const Configuration & config,  // Configuration of remote connection
                 BOOL create            // Flag to create connection if not present
               )
{
  if (config.phoneNumber.IsEmpty())
    return GeneralFailure;

  PConfig cfg(0, RasStr);

  if (!create && cfg.GetString(name, NumberStr, "").IsEmpty())
    return NoNameOrNumber;

  cfg.SetDefaultSection(name);

  if (config.device.IsEmpty())
    cfg.DeleteKey(PortStr);
  else
    cfg.SetString(PortStr, config.device);

  cfg.SetString(NumberStr, config.phoneNumber);

  if (config.ipAddress.IsEmpty())
    cfg.DeleteKey(AddressStr);
  else
    cfg.SetString(AddressStr, config.ipAddress);

  if (config.dnsAddress.IsEmpty())
    cfg.DeleteKey(NameServerStr);
  else
    cfg.SetString(NameServerStr, config.dnsAddress);

  if (config.script.IsEmpty())
    cfg.DeleteKey(LoginStr);
  else
    cfg.SetString(LoginStr, config.script);

  return Connected;
}


PRemoteConnection::Status PRemoteConnection::RemoveConfiguration(
		const PString & name  // Remote connection to remove
		)
{
  PConfig cfg(0, RasStr);

  if (cfg.GetString(name, NumberStr, "").IsEmpty())
    return NoNameOrNumber;

  cfg.DeleteSection(name);
  return Connected;
}



// End of File ////////////////////////////////////////////////////////////////
