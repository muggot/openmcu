/*
 * mswin.cxx
 *
 * General class implementations for 16 bit Windows.
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
 * $Log: mswin.cxx,v $
 * Revision 1.18  2001/08/07 03:20:39  robertj
 * Fixed close of DLL so flagged as closed, thanks Stefan Ditscheid.
 *
 * Revision 1.17  1998/09/24 03:30:51  robertj
 * Added open software license.
 *
 * Revision 1.16  1996/02/15 14:55:02  robertj
 * Win16 compatibility
 *
 * Revision 1.15  1996/01/28 02:55:33  robertj
 * WIN16 support.
 *
 * Revision 1.14  1995/12/10 11:58:37  robertj
 * Added WIN32 registry support for PConfig objects.
 *
 * Revision 1.13  1995/08/24 12:40:52  robertj
 * Changed PChannel so not a PContainer.
 *
 * Revision 1.12  1995/07/02 01:24:45  robertj
 * Added running of hidden VM for DOS program in PPipeChannel.
 *
 * Revision 1.11  1995/06/17 00:59:23  robertj
 * Moved PPipeChannel::Execute from common dos/windows to individual files.
 *
 * Revision 1.10  1995/03/12 05:00:06  robertj
 * Re-organisation of DOS/WIN16 and WIN32 platforms to maximise common code.
 * Used built-in equate for WIN32 API (_WIN32).
 *
 * Revision 1.9  1995/01/09  12:28:00  robertj
 * Added implementation for PConfig::Environment
 *
 * Revision 1.8  1994/10/23  05:41:29  robertj
 * Fixed config file bugs.
 *
 * Revision 1.7  1994/08/22  00:18:02  robertj
 * Fixed bug in serial comms timers.
 *
 * Revision 1.6  1994/08/04  13:24:27  robertj
 * Added DCB so can set paraemters on closed channel.
 *
 * Revision 1.5  1994/07/27  06:00:10  robertj
 * Backup
 *
 * Revision 1.4  1994/07/21  12:35:18  robertj
 * *** empty log message ***
 *
 * Revision 1.3  1994/07/17  11:01:04  robertj
 * Ehancements, implementation, bug fixes etc.
 *
 * Revision 1.2  1994/07/02  03:18:09  robertj
 * Multi-threading implementation.
 *
 * Revision 1.1  1994/06/25  12:13:01  robertj
 * Initial revision
 *
// Revision 1.1  1994/04/01  14:39:35  robertj
// Initial revision
//
 */

#include "ptlib.h"
#include <errno.h>
#include <fcntl.h>

#include <stdresid.h>


extern "C" HINSTANCE _hInstance;


///////////////////////////////////////////////////////////////////////////////
// PTime

PString PTime::GetTimeSeparator()
{
  PString str;
  GetProfileString("intl", "sTime", ":", str.GetPointer(100), 99);
  str.MakeMinimumSize();
  return str;
}


BOOL PTime::GetTimeAMPM()
{
  return GetProfileInt("intl", "iTime", 0) != 0;
}


PString PTime::GetTimeAM()
{
  PString str;
  GetProfileString("intl", "s1159", "am", str.GetPointer(100), 99);
  str.MakeMinimumSize();
  return str;
}


PString PTime::GetTimePM()
{
  PString str;
  GetProfileString("intl", "s2359", "pm", str.GetPointer(100), 99);
  str.MakeMinimumSize();
  return str;
}


PString PTime::GetDayName(Weekdays dayOfWeek, NameType type)
{
  static const char * const weekdays[] = {
    "Sunday", "Monday", "Tuesday", "Wednesday",
    "Thursday", "Friday", "Saturday"
  };
  static const char * const abbrev_weekdays[] = {
    "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
  };
  PString str;
  if (LoadString(_hInstance, dayOfWeek+
        (type != FullName ? PSTD_ID_STR_ABBREV_WEEKDAYS : PSTD_ID_STR_WEEKDAYS),
        str.GetPointer(100), 99) == 0)
    return (type != FullName ? abbrev_weekdays : weekdays)[dayOfWeek];
  str.MakeMinimumSize();
  return str;
}


PString PTime::GetDateSeparator()
{
  PString str;
  GetProfileString("intl", "sDate", "-", str.GetPointer(100), 99);
  str.MakeMinimumSize();
  return str;
}


PString PTime::GetMonthName(Months month, NameType type)
{
  static const char * const months[] = { "",
    "January", "February", "March", "April", "May", "June",
    "July", "August", "September", "October", "November", "December"
  };
  static const char * const abbrev_months[] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
  };
  PString str;
  if (LoadString(_hInstance, month+
       (UINT)(type != FullName ? PSTD_ID_STR_ABBREV_MONTHS : PSTD_ID_STR_MONTHS),
       str.GetPointer(100), 99) == 0)
    return (type != FullName ? abbrev_months : months)[month];
  str.MakeMinimumSize();
  return str;
}


PTime::DateOrder PTime::GetDateOrder()
{
  return (DateOrder)GetProfileInt("intl", "iDate", 0);
}


BOOL PTime::IsDaylightSavings()
{
  return FALSE;
}


int PTime::GetTimeZone(TimeZoneType type)
{
  return 0;
}


PString PTime::GetTimeZoneString(TimeZoneType type)
{
  return "";
}


///////////////////////////////////////////////////////////////////////////////
// PSerialChannel

void PSerialChannel::Construct()
{
  char str[50];
  strcpy(str, "com1");
  GetProfileString("ports", str, "9600,n,8,1,x", &str[5], sizeof(str)-6);
  str[4] = ':';
  if (!BuildCommDCB(str, &deviceControlBlock)) {
    osError = EINVAL;
    lastError = BadParameter;
  }
}


PString PSerialChannel::GetName() const
{
  if (IsOpen())
    return psprintf("COM%i", os_handle+1);

  return PString();
}


BOOL PSerialChannel::IsReadBlocked(PObject * obj)
{
  PSerialChannel & chan = *(PSerialChannel *)PAssertNULL(obj);
  COMSTAT stat;
  GetCommError(chan.os_handle, &stat);
  return stat.cbInQue <= 0 &&
         (chan.readTimeout == PMaxTimeInterval || chan.readTimer.IsRunning());
}


BOOL PSerialChannel::Read(void * buf, PINDEX len)
{
  lastReadCount = 0;

  if (!IsOpen()) {
    PThread::Yield();
    osError = EBADF;
    lastError = NotOpen;
    return FALSE;
  }

  if (readTimeout != PMaxTimeInterval)
    readTimer = readTimeout;
  if (IsReadBlocked(this))
    PThread::Current()->Block(&PSerialChannel::IsReadBlocked, this);

  lastReadCount = ReadComm(os_handle, buf, len);
  if (lastReadCount > 0)
    return TRUE;

  COMSTAT stat;
  GetCommError(os_handle, &stat);
  osError = EFAULT;
  lastReadCount = -lastReadCount;
  return lastReadCount > 0;
}


BOOL PSerialChannel::IsWriteBlocked(PObject * obj)
{
  PSerialChannel & chan = *(PSerialChannel *)PAssertNULL(obj);
  COMSTAT stat;
  GetCommError(chan.os_handle, &stat);
  return stat.cbOutQue >= OutputQueueSize &&
       (chan.writeTimeout == PMaxTimeInterval || chan.writeTimer.IsRunning());
}


BOOL PSerialChannel::Write(const void * buf, PINDEX len)
{
  lastWriteCount = 0;

  if (!IsOpen()) {
    osError = EBADF;
    lastError = NotOpen;
    return FALSE;
  }

  if (writeTimeout != PMaxTimeInterval)
    writeTimer = writeTimeout;
  if (IsWriteBlocked(this))
    PThread::Current()->Block(&PSerialChannel::IsWriteBlocked, this);

  lastWriteCount = WriteComm(os_handle, buf, len);
  if (lastWriteCount <= 0) {
    COMSTAT stat;
    GetCommError(os_handle, &stat);
    osError = EFAULT;
    lastWriteCount = -lastWriteCount;
  }
  return lastWriteCount >= len;
}


BOOL PSerialChannel::Close()
{
  if (!IsOpen()) {
    osError = EBADF;
    lastError = NotOpen;
    return FALSE;
  }

  BOOL retVal = CloseComm(os_handle) == 0;
  os_handle = -1;
  return retVal;
}


BOOL PSerialChannel::SetCommsParam(DWORD speed, BYTE data, Parity parity,
                     BYTE stop, FlowControl inputFlow, FlowControl outputFlow)
{
  if (IsOpen())
    PAssert(GetCommState(os_handle, &deviceControlBlock) == 0,
                                                        POperatingSystemError);

  switch (speed) {
    case 0 :
      break;
    case 14400 :
      deviceControlBlock.BaudRate = CBR_14400;
      break;
    case 19200 :
      deviceControlBlock.BaudRate = CBR_19200;
      break;
    case 38400 :
      deviceControlBlock.BaudRate = CBR_38400;
      break;
    case 56000 :
      deviceControlBlock.BaudRate = CBR_56000;
      break;
    case 128000 :
      deviceControlBlock.BaudRate = CBR_128000;
      break;
    case 256000 :
      deviceControlBlock.BaudRate = CBR_256000;
      break;
    default :
      if (speed > 9600) {
        osError = EINVAL;
        return FALSE;
      }
      deviceControlBlock.BaudRate = (UINT)speed;
  }

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
      deviceControlBlock.fRtsflow = FALSE;
      deviceControlBlock.fInX = FALSE;
      break;
    case XonXoff :
      deviceControlBlock.fRtsflow = FALSE;
      deviceControlBlock.fInX = TRUE;
      break;
    case RtsCts :
      deviceControlBlock.fRtsflow = TRUE;
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

  if (!IsOpen()) {
    osError = EBADF;
    lastError = NotOpen;
    lastError = BadParameter;
    return FALSE;
  }

  if (SetCommState(&deviceControlBlock) < 0) {
    osError = EINVAL;
    return FALSE;
  }

  PAssert(GetCommState(os_handle, &deviceControlBlock) == 0,
                                                        POperatingSystemError);
  return TRUE;
}


BOOL PSerialChannel::Open(const PString & port, DWORD speed, BYTE data,
       Parity parity, BYTE stop, FlowControl inputFlow, FlowControl outputFlow)
{
  Close();

  os_handle = OpenComm(port, InputQueueSize, OutputQueueSize);
  if (os_handle < 0) {
    switch (os_handle) {
      case IE_BADID :
      case IE_HARDWARE :
        osError = ENOENT;
        lastError = NotFound;
        break;
      case IE_OPEN :
        osError = EBUSY;
        lastError = DeviceInUse;
        break;
      case IE_MEMORY :
        osError = ENOMEM;
        lastError = NoMemory;
        break;
      case IE_BAUDRATE :
      case IE_BYTESIZE :
        osError = EINVAL;
        lastError = BadParameter;
        break;
      default :
        osError = EFAULT;
        lastError = Miscellaneous;
    }
    os_handle = -1;
    return FALSE;
  }

  deviceControlBlock.Id = (BYTE)os_handle;
  SetCommState(&deviceControlBlock);

  if (!SetCommsParam(speed, data, parity, stop, inputFlow, outputFlow)) {
    CloseComm(os_handle);
    return FALSE;
  }

  SetCommEventMask(os_handle, EV_CTSS|EV_DSR|EV_RING|EV_RLSDS);
  return TRUE;
}


BOOL PSerialChannel::SetSpeed(DWORD speed)
{
  return SetCommsParam(speed,
                 0, DefaultParity, 0, DefaultFlowControl, DefaultFlowControl);
}


DWORD PSerialChannel::GetSpeed() const
{
  switch (deviceControlBlock.BaudRate) {
    case CBR_110 :
      return 110;
    case CBR_300 :
      return 300;
    case CBR_600 :
      return 600;
    case CBR_1200 :
      return 1200;
    case CBR_2400 :
      return 2400;
    case CBR_4800 :
      return 4800;
    case CBR_9600 :
      return 9600;
    case CBR_14400 :
      return 14400;
    case CBR_19200 :
      return 19200;
    case CBR_38400 :
      return 38400;
    case CBR_56000 :
      return 56000;
    case CBR_128000 :
      return 128000;
    case CBR_256000 :
      return 256000;
  }
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
  return SetCommsParam(0,0, parity, 0, DefaultFlowControl, DefaultFlowControl);
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
  return SetCommsParam(0,0, DefaultParity, 0, flowControl, DefaultFlowControl);
}


PSerialChannel::FlowControl PSerialChannel::GetInputFlowControl() const
{
  if (deviceControlBlock.fRtsflow)
    return RtsCts;
  if (deviceControlBlock.fInX != 0)
    return XonXoff;
  return NoFlowControl;
}


BOOL PSerialChannel::SetOutputFlowControl(FlowControl flowControl)
{
  return SetCommsParam(0,0, DefaultParity, 0, DefaultFlowControl, flowControl);
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
  if (!IsOpen()) {
    osError = EBADF;
    lastError = NotOpen;
    return;
  }

  PAssert(EscapeCommFunction(os_handle, state ? SETDTR : CLRDTR) == 0,
                                                      POperatingSystemError);
}


void PSerialChannel::SetRTS(BOOL state)
{
  if (!IsOpen()) {
    osError = EBADF;
    lastError = NotOpen;
    return;
  }

  PAssert(EscapeCommFunction(os_handle, state ? SETRTS : CLRRTS) == 0,
                                                      POperatingSystemError);
}


void PSerialChannel::SetBreak(BOOL state)
{
  if (!IsOpen()) {
    osError = EBADF;
    lastError = NotOpen;
    return;
  }

  if (state)
    PAssert(SetCommBreak(os_handle), POperatingSystemError);
  else
    PAssert(ClearCommBreak(os_handle), POperatingSystemError);
}


BOOL PSerialChannel::GetCTS()
{
  if (!IsOpen()) {
    osError = EBADF;
    lastError = NotOpen;
    return FALSE;
  }

  return (GetCommEventMask(os_handle, 0)&EV_CTSS) != 0;
}


BOOL PSerialChannel::GetDSR()
{
  if (!IsOpen()) {
    osError = EBADF;
    lastError = NotOpen;
    return FALSE;
  }

  return (GetCommEventMask(os_handle, 0)&EV_DSR) != 0;
}


BOOL PSerialChannel::GetDCD()
{
  if (!IsOpen()) {
    osError = EBADF;
    lastError = NotOpen;
    return FALSE;
  }

  return (GetCommEventMask(os_handle, 0)&EV_RLSDS) != 0;
}


BOOL PSerialChannel::GetRing()
{
  if (!IsOpen()) {
    osError = EBADF;
    lastError = NotOpen;
    return FALSE;
  }

  return (GetCommEventMask(os_handle, 0)&EV_RING) != 0;
}


PStringList PSerialChannel::GetPortNames()
{
  static char buf[] = "COM ";
  PStringList ports;
  for (char p = '1'; p <= '4'; p++) {
    buf[3] = p;
    ports.Append(new PString(buf));
  }
  return ports;
}


///////////////////////////////////////////////////////////////////////////////
// PPipeChannel

BOOL PPipeChannel::Execute()
{
  if (hasRun)
    return FALSE;

  flush();
  if (os_handle >= 0) {
    _close(os_handle);
    os_handle = -1;
  }

  static struct {
    DWORD pifFlags;
    DWORD displayFlags;
    struct {
      DWORD offset;
      WORD  selector;
    } exePath, programArguments, workingDirectory;
    WORD  desiredV86Pages;
    WORD  minimumV86Pages;
    WORD  foregroundPriority;
    WORD  backgroundPriority;
    WORD  maximumEMS;
    WORD  minimumEMS;
    WORD  maximumXMS;
    WORD  minimumXMS;
    DWORD unknown;
    char  windowTitle[128];
  } seb = {
    0x40000006,  // Runs in background, runs in window, close on exit
    0x0000001f,  // Emulate text mode, no monitor ports
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    0xffff, // desired memory
    0xffff, // minimum memory
    100,    // foreground priority
    50,     // background priority
    0x0400, // maximum EMS
    0,      // minimum EMS
    0x4000, // maximum XMS
    0,      // minimum XMS
    0,      // unknown
    "PWLib Pipe Channel Process"
  };

  char * commandDotCom = getenv("COMSPEC");
  if (commandDotCom == NULL)
    commandDotCom = "C:\\COMMAND.COM";
  seb.exePath.selector = SELECTOROF(commandDotCom);
  seb.exePath.offset   = OFFSETOF  (commandDotCom);

  PString commandArguments = " /c " + subProgName;
  const char * argumentPointer = commandArguments;
  seb.programArguments.selector = SELECTOROF(argumentPointer);
  seb.programArguments.offset   = OFFSETOF  (argumentPointer);
  
  static char * currentDirectory = ".";
  seb.workingDirectory.selector = SELECTOROF(currentDirectory);
  seb.workingDirectory.offset   = OFFSETOF  (currentDirectory);

  void (FAR * shellEntry)();
  _asm mov  ax,1684h;  //Get Shell VXDs protected mode entry point
  _asm mov  bx,0017h;
  _asm int  2fh;
  _asm mov  word ptr [shellEntry], di;
  _asm mov  word ptr [shellEntry+2], es;
  if (shellEntry == NULL)
    return FALSE;

  _asm lea  di, word ptr seb;
  _asm mov  dx, 3;
  _asm push es;
  _asm push ss;
  _asm pop  es;
  shellEntry();
  _asm pop  es;

  DWORD hVirtualMachine;
#if defined(_MSC_VER)
  _asm _emit 66h;
#else
  _asm db 66h;
#endif
  _asm mov  word ptr hVirtualMachine, ax; // Really EAX
  if (hVirtualMachine == 0)
    return FALSE;

  if (fromChild.IsEmpty())
    return TRUE;

  // Wait for child to complete
  

  os_handle = _open(fromChild, _O_RDONLY);
  return ConvertOSError(os_handle);
}


///////////////////////////////////////////////////////////////////////////////
// Configuration files

void PConfig::Construct(Source src)
{
  switch (src) {
    case System :
      location = "WIN.INI";
      break;

    case Application :
      PFilePath appFile = PProcess::Current()->GetFile();
      location = appFile.GetDirectory() + appFile.GetTitle() + ".INI";
      break;
  }

  source = src;
}


void PConfig::Construct(const PFilePath & filename)
{
  location = filename;
  source = NumSources;
}


PStringList PConfig::GetSections()
{
  PStringList sections;

  if (source != Environment) {
    PString buf;
    char * ptr = buf.GetPointer(10000);
    GetPrivateProfileString(NULL, NULL, "", ptr, 9999, location);
    while (*ptr != '\0') {
      sections.AppendString(ptr);
      ptr += strlen(ptr)+1;
    }
  }

  return sections;
}


PStringList PConfig::GetKeys(const PString & section) const
{
  PStringList keys;

  if (source == Environment) {
    char ** ptr = _environ;
    while (*ptr != NULL) {
      PString buf = *ptr++;
      keys.AppendString(buf.Left(buf.Find('=')));
    }
  }
  else {
    PString buf;
    char * ptr = buf.GetPointer(10000);
    GetPrivateProfileString(section, NULL, "", ptr, 9999, location);
    while (*ptr != '\0') {
      keys.AppendString(ptr);
      ptr += strlen(ptr)+1;
    }
  }

  return keys;
}


void PConfig::DeleteSection(const PString & section)
{
  if (source == Environment)
    return;

  PAssert(!section.IsEmpty(), PInvalidParameter);
  PAssertOS(WritePrivateProfileString(section, NULL, NULL, location));
}


void PConfig::DeleteKey(const PString & section, const PString & key)
{
  PAssert(!key.IsEmpty(), PInvalidParameter);

  if (source == Environment) {
    PString str = key;
    PAssert(str.Find('=') == P_MAX_INDEX, PInvalidParameter);
    _putenv(str + "=");
  }
  else {
    PAssert(!section.IsEmpty(), PInvalidParameter);
    PAssertOS(WritePrivateProfileString(section, key, NULL, location));
  }
}


PString PConfig::GetString(const PString & section,
                                    const PString & key, const PString & dflt)
{
  PString str;

  PAssert(!key.IsEmpty(), PInvalidParameter);

  if (source == Environment) {
    PAssert(key.Find('=') == P_MAX_INDEX, PInvalidParameter);
    char * env = getenv(key);
    if (env != NULL)
      str = env;
    else
      str = dflt;
  }
  else {
    PAssert(!section.IsEmpty(), PInvalidParameter);
    GetPrivateProfileString(section, key, dflt,
                                        str.GetPointer(1000), 999, location);
    str.MakeMinimumSize();
  }

  return str;
}


void PConfig::SetString(const PString & section,
                                   const PString & key, const PString & value)
{
  PAssert(!key.IsEmpty(), PInvalidParameter);

  if (source == Environment) {
    PString str = key;
    PAssert(str.Find('=') == P_MAX_INDEX, PInvalidParameter);
    _putenv(str + "=" + value);
  }
  else {
    PAssert(!section.IsEmpty(), PInvalidParameter);
    PAssertOS(WritePrivateProfileString(section, key, value, location));
  }
}



///////////////////////////////////////////////////////////////////////////////
// Threads

static char NEAR * NEAR * const StackBase = (char NEAR * NEAR *)0xa;
static char NEAR * NEAR * const StackUsed = (char NEAR * NEAR *)0xc;
static char NEAR * NEAR * const StackTop  = (char NEAR * NEAR *)0xe;

void PThread::SwitchContext(PThread * from)
{
  if (from == this) // Switching to itself, ie is only thread
    return;

  if (setjmp(from->context) != 0) // Are being reactivated from previous yield
    return;

  // Save some magic global variables in MS-Windows DGROUP segment
  from->stackBase = *StackBase;
  from->stackTop  = *StackTop;
  from->stackUsed = *StackTop - *StackUsed;

  if (status == Starting) {
    if (setjmp(context) != 0)
      BeginThread();
    context[3] = (int)stackTop-16;  // Change the stack pointer in jmp_buf
  }

  // Restore those MS-Windows magic global for the next context
  *StackBase = stackBase;
  *StackTop = stackTop;
  *StackUsed = stackTop - stackUsed;
  
  longjmp(context, TRUE);
  PAssertAlways("longjmp failed"); // Should never get here
}


///////////////////////////////////////////////////////////////////////////////
// PDynaLink

PDynaLink::PDynaLink()
{
  _hDLL = NULL;
}


PDynaLink::PDynaLink(const PString & name)
{
  Open(name);
}


PDynaLink::~PDynaLink()
{
  Close();
}


BOOL PDynaLink::Open(const PString & name)
{
  if ((_hDLL = LoadLibrary(name)) < HINSTANCE_ERROR)
    _hDLL = NULL;
  return _hDLL != NULL;
}


void PDynaLink::Close()
{
  if (_hDLL != NULL) {
    FreeLibrary(_hDLL);
    _hDLL = NULL;
  }
}


BOOL PDynaLink::IsLoaded() const
{
  return _hDLL != NULL;
}


BOOL PDynaLink::GetFunction(PINDEX index, Function & func)
{
  if (_hDLL == NULL)
    return FALSE;

  FARPROC p = GetProcAddress(_hDLL, (LPSTR)(DWORD)LOWORD(index));
  if (p == NULL)
    return FALSE;

  func = (Function)p;
  return TRUE;
}


BOOL PDynaLink::GetFunction(const PString & name, Function & func)
{
  if (_hDLL == NULL)
    return FALSE;

  FARPROC p = GetProcAddress(_hDLL, name);
  if (p == NULL)
    return FALSE;

  func = (Function)p;
  return TRUE;
}



// End Of File ///////////////////////////////////////////////////////////////
