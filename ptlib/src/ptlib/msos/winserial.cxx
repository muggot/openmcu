/*
 * winserial.cxx
 *
 * Miscellaneous implementation of classes for Win32
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
 * $Log: winserial.cxx,v $
 * Revision 1.7  2005/01/12 03:24:08  csoutheren
 * More cleanup of event handling
 *
 * Revision 1.6  2005/01/11 12:46:37  csoutheren
 * Removed handle leak on serial port caused by memset
 * Thanks to Dmitry Samokhin
 *
 * Revision 1.5  2004/12/27 22:38:27  csoutheren
 * Fixed problems with accessing serial port under Windows
 *
 * Revision 1.4  2001/09/10 02:51:23  robertj
 * Major change to fix problem with error codes being corrupted in a
 *   PChannel when have simultaneous reads and writes in threads.
 *
 * Revision 1.3  2000/03/20 17:55:05  robertj
 * Fixed prolem with XON/XOFF under NT, thanks Damien Slee.
 *
 * Revision 1.2  1998/11/30 12:32:47  robertj
 * Added missing copyright header.
 *
 */

#include <ptlib.h>
#include <ptlib/serchan.h>


#define QUEUE_SIZE 2048


///////////////////////////////////////////////////////////////////////////////
// PSerialChannel

void PSerialChannel::Construct()
{
  commsResource = INVALID_HANDLE_VALUE;

  char str[50];
  strcpy(str, "com1");
  GetProfileString("ports", str, "9600,n,8,1,x", &str[5], sizeof(str)-6);
  str[4] = ':';
  memset(&deviceControlBlock, 0, sizeof(deviceControlBlock));
  deviceControlBlock.DCBlength = sizeof(deviceControlBlock);
  BuildCommDCB(str, &deviceControlBlock);

  // These values are not set by BuildCommDCB
  deviceControlBlock.XoffChar = 19;
  deviceControlBlock.XonChar = 17;
  deviceControlBlock.XoffLim = (QUEUE_SIZE * 7)/8;  // upper limit before XOFF is sent to stop reception
  deviceControlBlock.XonLim = (QUEUE_SIZE * 3)/4;   // lower limit before XON is sent to re-enabled reception
}


PString PSerialChannel::GetName() const
{
  return portName;
}


BOOL PSerialChannel::Read(void * buf, PINDEX len)
{
  lastReadCount = 0;

  if (!IsOpen())
    return SetErrorValues(NotOpen, EBADF, LastReadError);

  COMMTIMEOUTS cto;
  PAssertOS(GetCommTimeouts(commsResource, &cto));
  cto.ReadIntervalTimeout = 0;
  cto.ReadTotalTimeoutMultiplier = 0;
  cto.ReadTotalTimeoutConstant = 0;
  cto.ReadIntervalTimeout = MAXDWORD; // Immediate timeout
  PAssertOS(SetCommTimeouts(commsResource, &cto));

  DWORD eventMask;
  PAssertOS(GetCommMask(commsResource, &eventMask));
  if (eventMask != (EV_RXCHAR|EV_TXEMPTY))
    PAssertOS(SetCommMask(commsResource, EV_RXCHAR|EV_TXEMPTY));

  DWORD timeToGo = readTimeout.GetInterval();
  DWORD bytesToGo = len;
  char * bufferPtr = (char *)buf;

  for (;;) {
    PWin32Overlapped overlap;
    DWORD readCount = 0;
    if (!ReadFile(commsResource, bufferPtr, bytesToGo, &readCount, &overlap)) {
      if (::GetLastError() != ERROR_IO_PENDING)
        return ConvertOSError(-2, LastReadError);
      if (!::GetOverlappedResult(commsResource, &overlap, &readCount, FALSE))
        return ConvertOSError(-2, LastReadError);
    }

    bytesToGo -= readCount;
    bufferPtr += readCount;
    lastReadCount += readCount;
    if (lastReadCount >= len || timeToGo == 0)
      return lastReadCount > 0;

    if (!::WaitCommEvent(commsResource, &eventMask, &overlap)) {
      if (::GetLastError()!= ERROR_IO_PENDING)
        return ConvertOSError(-2, LastReadError);
      DWORD err = ::WaitForSingleObject(overlap.hEvent, timeToGo);
      if (err == WAIT_TIMEOUT) {
        SetErrorValues(Timeout, EAGAIN, LastReadError);
        ::CancelIo(commsResource);
        return lastReadCount > 0;
      }
      else if (err == WAIT_FAILED)
        return ConvertOSError(-2, LastReadError);
    }
  }
}


BOOL PSerialChannel::Write(const void * buf, PINDEX len)
{
  lastWriteCount = 0;

  if (!IsOpen())
    return SetErrorValues(NotOpen, EBADF, LastWriteError);

  COMMTIMEOUTS cto;
  PAssertOS(GetCommTimeouts(commsResource, &cto));
  cto.WriteTotalTimeoutMultiplier = 0;
  if (writeTimeout == PMaxTimeInterval)
    cto.WriteTotalTimeoutConstant = 0;
  else if (writeTimeout <= PTimeInterval(0))
    cto.WriteTotalTimeoutConstant = 1;
  else
    cto.WriteTotalTimeoutConstant = writeTimeout.GetInterval();
  PAssertOS(SetCommTimeouts(commsResource, &cto));

  PWin32Overlapped overlap;
  if (WriteFile(commsResource, buf, len, (LPDWORD)&lastWriteCount, &overlap)) 
    return lastWriteCount == len;

  if (GetLastError() == ERROR_IO_PENDING)
    if (GetOverlappedResult(commsResource, &overlap, (LPDWORD)&lastWriteCount, TRUE)) {
      return lastWriteCount == len;
    }

  ConvertOSError(-2, LastWriteError);

  return FALSE;
}


BOOL PSerialChannel::Close()
{
  if (!IsOpen())
    return SetErrorValues(NotOpen, EBADF);

  CloseHandle(commsResource);
  commsResource = INVALID_HANDLE_VALUE;
  os_handle = -1;
  return ConvertOSError(-2);
}


BOOL PSerialChannel::SetCommsParam(DWORD speed, BYTE data, Parity parity,
                     BYTE stop, FlowControl inputFlow, FlowControl outputFlow)
{
  if (speed > 0)
    deviceControlBlock.BaudRate = speed;

  if (data > 0)
    deviceControlBlock.ByteSize = data;

  switch (parity) {
    case NoParity :
      deviceControlBlock.Parity = NOPARITY;
      break;
    case OddParity :
      deviceControlBlock.Parity = ODDPARITY;
      break;
    case EvenParity :
      deviceControlBlock.Parity = EVENPARITY;
      break;
    case MarkParity :
      deviceControlBlock.Parity = MARKPARITY;
      break;
    case SpaceParity :
      deviceControlBlock.Parity = SPACEPARITY;
      break;
  }

  switch (stop) {
    case 1 :
      deviceControlBlock.StopBits = ONESTOPBIT;
      break;
    case 2 :
      deviceControlBlock.StopBits = TWOSTOPBITS;
      break;
  }

  switch (inputFlow) {
    case NoFlowControl :
      deviceControlBlock.fRtsControl = RTS_CONTROL_DISABLE;
      deviceControlBlock.fInX = FALSE;
      break;
    case XonXoff :
      deviceControlBlock.fRtsControl = RTS_CONTROL_DISABLE;
      deviceControlBlock.fInX = TRUE;
      break;
    case RtsCts :
      deviceControlBlock.fRtsControl = RTS_CONTROL_HANDSHAKE;
      deviceControlBlock.fInX = FALSE;
      break;
  }

  switch (outputFlow) {
    case NoFlowControl :
      deviceControlBlock.fOutxCtsFlow = FALSE;
      deviceControlBlock.fOutxDsrFlow = FALSE;
      deviceControlBlock.fOutX = FALSE;
      break;
    case XonXoff :
      deviceControlBlock.fOutxCtsFlow = FALSE;
      deviceControlBlock.fOutxDsrFlow = FALSE;
      deviceControlBlock.fOutX = TRUE;
      break;
    case RtsCts :
      deviceControlBlock.fOutxCtsFlow = TRUE;
      deviceControlBlock.fOutxDsrFlow = FALSE;
      deviceControlBlock.fOutX = FALSE;
      break;
  }

  if (!IsOpen())
    return SetErrorValues(NotOpen, EBADF);

  return ConvertOSError(SetCommState(commsResource, &deviceControlBlock) ? 0 : -2);
}


BOOL PSerialChannel::Open(const PString & port, DWORD speed, BYTE data,
               Parity parity, BYTE stop, FlowControl inputFlow, FlowControl outputFlow)
{
  Close();

  portName = port;
  if (portName.Find(PDIR_SEPARATOR) == P_MAX_INDEX)
    portName = "\\\\.\\" + port;
  commsResource = CreateFile(portName,
                             GENERIC_READ|GENERIC_WRITE,
                             0,
                             NULL,
                             OPEN_EXISTING,
                             FILE_FLAG_OVERLAPPED,
                             NULL);
  if (commsResource == INVALID_HANDLE_VALUE)
    return ConvertOSError(-2);

  os_handle = 0;

  SetupComm(commsResource, QUEUE_SIZE, QUEUE_SIZE);

  if (SetCommsParam(speed, data, parity, stop, inputFlow, outputFlow))
    return TRUE;

  ConvertOSError(-2);
  CloseHandle(commsResource);
  os_handle = -1;
  return FALSE;
}


BOOL PSerialChannel::SetSpeed(DWORD speed)
{
  return SetCommsParam(speed,
                  0, DefaultParity, 0, DefaultFlowControl, DefaultFlowControl);
}


DWORD PSerialChannel::GetSpeed() const
{
  return deviceControlBlock.BaudRate;
}


BOOL PSerialChannel::SetDataBits(BYTE data)
{
  return SetCommsParam(0,
               data, DefaultParity, 0, DefaultFlowControl, DefaultFlowControl);
}


BYTE PSerialChannel::GetDataBits() const
{
  return deviceControlBlock.ByteSize;
}


BOOL PSerialChannel::SetParity(Parity parity)
{
  return SetCommsParam(0,0,parity,0,DefaultFlowControl,DefaultFlowControl);
}


PSerialChannel::Parity PSerialChannel::GetParity() const
{
  switch (deviceControlBlock.Parity) {
    case ODDPARITY :
      return OddParity;
    case EVENPARITY :
      return EvenParity;
    case MARKPARITY :
      return MarkParity;
    case SPACEPARITY :
      return SpaceParity;
  }
  return NoParity;
}


BOOL PSerialChannel::SetStopBits(BYTE stop)
{
  return SetCommsParam(0,
               0, DefaultParity, stop, DefaultFlowControl, DefaultFlowControl);
}


BYTE PSerialChannel::GetStopBits() const
{
  return (BYTE)(deviceControlBlock.StopBits == ONESTOPBIT ? 1 : 2);
}


BOOL PSerialChannel::SetInputFlowControl(FlowControl flowControl)
{
  return SetCommsParam(0,0,DefaultParity,0,flowControl,DefaultFlowControl);
}


PSerialChannel::FlowControl PSerialChannel::GetInputFlowControl() const
{
  if (deviceControlBlock.fRtsControl == RTS_CONTROL_HANDSHAKE)
    return RtsCts;
  if (deviceControlBlock.fInX != 0)
    return XonXoff;
  return NoFlowControl;
}


BOOL PSerialChannel::SetOutputFlowControl(FlowControl flowControl)
{
  return SetCommsParam(0,0,DefaultParity,0,DefaultFlowControl,flowControl);
}


PSerialChannel::FlowControl PSerialChannel::GetOutputFlowControl() const
{
  if (deviceControlBlock.fOutxCtsFlow != 0)
    return RtsCts;
  if (deviceControlBlock.fOutX != 0)
    return XonXoff;
  return NoFlowControl;
}


void PSerialChannel::SetDTR(BOOL state)
{
  if (IsOpen())
    PAssertOS(EscapeCommFunction(commsResource, state ? SETDTR : CLRDTR));
  else
    SetErrorValues(NotOpen, EBADF);
}


void PSerialChannel::SetRTS(BOOL state)
{
  if (IsOpen())
    PAssertOS(EscapeCommFunction(commsResource, state ? SETRTS : CLRRTS));
  else
    SetErrorValues(NotOpen, EBADF);
}


void PSerialChannel::SetBreak(BOOL state)
{
  if (IsOpen())
    if (state)
      PAssertOS(SetCommBreak(commsResource));
    else
      PAssertOS(ClearCommBreak(commsResource));
  else
    SetErrorValues(NotOpen, EBADF);
}


BOOL PSerialChannel::GetCTS()
{
  if (!IsOpen())
    return SetErrorValues(NotOpen, EBADF);

  DWORD stat;
  PAssertOS(GetCommModemStatus(commsResource, &stat));
  return (stat&MS_CTS_ON) != 0;
}


BOOL PSerialChannel::GetDSR()
{
  if (!IsOpen())
    return SetErrorValues(NotOpen, EBADF);

  DWORD stat;
  PAssertOS(GetCommModemStatus(commsResource, &stat));
  return (stat&MS_DSR_ON) != 0;
}


BOOL PSerialChannel::GetDCD()
{
  if (!IsOpen())
    return SetErrorValues(NotOpen, EBADF);

  DWORD stat;
  PAssertOS(GetCommModemStatus(commsResource, &stat));
  return (stat&MS_RLSD_ON) != 0;
}


BOOL PSerialChannel::GetRing()
{
  if (!IsOpen())
    return SetErrorValues(NotOpen, EBADF);

  DWORD stat;
  PAssertOS(GetCommModemStatus(commsResource, &stat));
  return (stat&MS_RING_ON) != 0;
}


PStringList PSerialChannel::GetPortNames()
{
  PStringList ports;
  for (char p = 1; p <= 9; p++)
    ports.AppendString(psprintf("\\\\.\\COM%u", p));
  return ports;
}
