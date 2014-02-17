/*
 * modem.cxx
 *
 * Asynchronous Serial I/O channel class.
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
 * $Log: modem.cxx,v $
 * Revision 1.6  2002/11/06 22:47:25  robertj
 * Fixed header comment (copyright etc)
 *
 * Revision 1.5  2000/01/21 17:42:53  craigs
 * Added pragma to avoid problems when linking as Linux shared library
 *
 * Revision 1.4  1998/11/30 12:33:06  robertj
 * Split serial channel and modem, modem to components library.
 *
 * Revision 1.3  1998/09/23 06:22:38  robertj
 * Added open source copyright license.
 *
 * Revision 1.2  1996/04/15 10:57:59  robertj
 * Moved some functions from INL to serial.cxx so unix linker can make smaller executables.
 *
 * Revision 1.1  1996/04/14 02:54:14  robertj
 * Initial revision
 *
 */

#ifdef __GNUC__
#pragma implementation "modem.h"
#endif

#include <ptlib.h>
#include <ptclib/modem.h>

#include <ctype.h>


///////////////////////////////////////////////////////////////////////////////
// PModem

PModem::PModem()
{
  status = Unopened;
}


PModem::PModem(const PString & port, DWORD speed, BYTE data,
      Parity parity, BYTE stop, FlowControl inputFlow, FlowControl outputFlow)
  : PSerialChannel(port, speed, data, parity, stop, inputFlow, outputFlow)
{
  status = IsOpen() ? Uninitialised : Unopened;
}


PModem::PModem(PConfig & cfg)
{
  status = Open(cfg) ? Uninitialised : Unopened;
}


void PModem::SetInitString(const PString & str)
{
  initCmd = str;
}

PString PModem::GetInitString() const
{
  return initCmd;
}

void PModem::SetDeinitString(const PString & str)
{
  deinitCmd = str;
}

PString PModem::GetDeinitString() const
{
  return deinitCmd;
}

void PModem::SetPreDialString(const PString & str)
{
  preDialCmd = str;
}

PString PModem::GetPreDialString() const
{
  return preDialCmd;
}

void PModem::SetPostDialString(const PString & str)
{
  postDialCmd = str;
}

PString PModem::GetPostDialString() const
{
  return postDialCmd;
}

void PModem::SetBusyString(const PString & str)
{
  busyReply = str;
}

PString PModem::GetBusyString() const
{
  return busyReply;
}

void PModem::SetNoCarrierString(const PString & str)
{
  noCarrierReply = str;
}

PString PModem::GetNoCarrierString() const
{ 
  return noCarrierReply;
}

void PModem::SetConnectString(const PString & str)
{
  connectReply = str;
}

PString PModem::GetConnectString() const
{
  return connectReply;
}

void PModem::SetHangUpString(const PString & str)
{
  hangUpCmd = str;
}

PString PModem::GetHangUpString() const
{
  return hangUpCmd;
}

PModem::Status PModem::GetStatus() const
{
  return status;
}


BOOL PModem::Close()
{
  status = Unopened;
  return PSerialChannel::Close();
}


BOOL PModem::Open(const PString & port, DWORD speed, BYTE data,
      Parity parity, BYTE stop, FlowControl inputFlow, FlowControl outputFlow)
{
  if (!PSerialChannel::Open(port,
                            speed, data, parity, stop, inputFlow, outputFlow))
    return FALSE;

  status = Uninitialised;
  return TRUE;
}


static const char ModemInit[] = "ModemInit";
static const char ModemDeinit[] = "ModemDeinit";
static const char ModemPreDial[] = "ModemPreDial";
static const char ModemPostDial[] = "ModemPostDial";
static const char ModemBusy[] = "ModemBusy";
static const char ModemNoCarrier[] = "ModemNoCarrier";
static const char ModemConnect[] = "ModemConnect";
static const char ModemHangUp[] = "ModemHangUp";

BOOL PModem::Open(PConfig & cfg)
{
  initCmd = cfg.GetString(ModemInit, "ATZ\\r\\w2sOK\\w100m");
  deinitCmd = cfg.GetString(ModemDeinit, "\\d2s+++\\d2sATH0\\r");
  preDialCmd = cfg.GetString(ModemPreDial, "ATDT");
  postDialCmd = cfg.GetString(ModemPostDial, "\\r");
  busyReply = cfg.GetString(ModemBusy, "BUSY");
  noCarrierReply = cfg.GetString(ModemNoCarrier, "NO CARRIER");
  connectReply = cfg.GetString(ModemConnect, "CONNECT");
  hangUpCmd = cfg.GetString(ModemHangUp, "\\d2s+++\\d2sATH0\\r");

  if (!PSerialChannel::Open(cfg))
    return FALSE;

  status = Uninitialised;
  return TRUE;
}


void PModem::SaveSettings(PConfig & cfg)
{
  PSerialChannel::SaveSettings(cfg);
  cfg.SetString(ModemInit, initCmd);
  cfg.SetString(ModemDeinit, deinitCmd);
  cfg.SetString(ModemPreDial, preDialCmd);
  cfg.SetString(ModemPostDial, postDialCmd);
  cfg.SetString(ModemBusy, busyReply);
  cfg.SetString(ModemNoCarrier, noCarrierReply);
  cfg.SetString(ModemConnect, connectReply);
  cfg.SetString(ModemHangUp, hangUpCmd);
}


BOOL PModem::CanInitialise() const
{
  switch (status) {
    case Unopened :
    case Initialising :
    case Dialling :
    case AwaitingResponse :
    case HangingUp :
    case Deinitialising :
    case SendingUserCommand :
      return FALSE;

    default :
      return TRUE;
  }
}


BOOL PModem::Initialise()
{
  if (CanInitialise()) {
    status = Initialising;
    if (SendCommandString(initCmd)) {
      status = Initialised;
      return TRUE;
    }
    status = InitialiseFailed;
  }
  return FALSE;
}


BOOL PModem::CanDeinitialise() const
{
  switch (status) {
    case Unopened :
    case Initialising :
    case Dialling :
    case AwaitingResponse :
    case Connected :
    case HangingUp :
    case Deinitialising :
    case SendingUserCommand :
      return FALSE;

    default :
      return TRUE;
  }
}


BOOL PModem::Deinitialise()
{
  if (CanDeinitialise()) {
    status = Deinitialising;
    if (SendCommandString(deinitCmd)) {
      status = Uninitialised;
      return TRUE;
    }
    status = DeinitialiseFailed;
  }
  return FALSE;
}


BOOL PModem::CanDial() const
{
  switch (status) {
    case Unopened :
    case Uninitialised :
    case Initialising :
    case InitialiseFailed :
    case Dialling :
    case AwaitingResponse :
    case Connected :
    case HangingUp :
    case Deinitialising :
    case DeinitialiseFailed :
    case SendingUserCommand :
      return FALSE;

    default :
      return TRUE;
  }
}


BOOL PModem::Dial(const PString & number)
{
  if (!CanDial())
    return FALSE;

  status = Dialling;
  if (!SendCommandString(preDialCmd + "\\s" + number + postDialCmd)) {
    status = DialFailed;
    return FALSE;
  }

  status = AwaitingResponse;

  PTimer timeout = 120000;
  PINDEX connectPosition = 0;
  PINDEX busyPosition = 0;
  PINDEX noCarrierPosition = 0;

  for (;;) {
    int nextChar;
    if ((nextChar = ReadCharWithTimeout(timeout)) < 0)
      return FALSE;

    if (ReceiveCommandString(nextChar, connectReply, connectPosition, 0))
      break;

    if (ReceiveCommandString(nextChar, busyReply, busyPosition, 0)) {
      status = LineBusy;
      return FALSE;
    }

    if (ReceiveCommandString(nextChar, noCarrierReply, noCarrierPosition, 0)) {
      status = NoCarrier;
      return FALSE;
    }
  }

  status = Connected;
  return TRUE;
}


BOOL PModem::CanHangUp() const
{
  switch (status) {
    case Unopened :
    case Uninitialised :
    case Initialising :
    case InitialiseFailed :
    case Dialling :
    case AwaitingResponse :
    case HangingUp :
    case Deinitialising :
    case SendingUserCommand :
      return FALSE;

    default :
      return TRUE;
  }
}


BOOL PModem::HangUp()
{
  if (CanHangUp()) {
    status = HangingUp;
    if (SendCommandString(hangUpCmd)) {
      status = Initialised;
      return TRUE;
    }
    status = HangUpFailed;
  }
  return FALSE;
}


BOOL PModem::CanSendUser() const
{
  switch (status) {
    case Unopened :
    case Uninitialised :
    case Initialising :
    case InitialiseFailed :
    case Dialling :
    case AwaitingResponse :
    case HangingUp :
    case Deinitialising :
    case SendingUserCommand :
      return FALSE;

    default :
      return TRUE;
  }
}


BOOL PModem::SendUser(const PString & str)
{
  if (CanSendUser()) {
    Status oldStatus = status;
    status = SendingUserCommand;
    if (SendCommandString(str)) {
      status = oldStatus;
      return TRUE;
    }
    status = oldStatus;
  }
  return FALSE;
}


void PModem::Abort()
{
  switch (status) {
    case Initialising :
      status = InitialiseFailed;
      break;
    case Dialling :
    case AwaitingResponse :
      status = DialFailed;
      break;
    case HangingUp :
      status = HangUpFailed;
      break;
    case Deinitialising :
      status = DeinitialiseFailed;
      break;
    default :
      break;
  }
}


BOOL PModem::CanRead() const
{
  switch (status) {
    case Unopened :
    case Initialising :
    case Dialling :
    case AwaitingResponse :
    case HangingUp :
    case Deinitialising :
    case SendingUserCommand :
      return FALSE;

    default :
      return TRUE;
  }
}


// End Of File ///////////////////////////////////////////////////////////////
