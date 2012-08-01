/*
 * serchan.cxx
 *
 * Asynchronous serial I/O channel class implementation.
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
 * $Log: serchan.cxx,v $
 * Revision 1.32  2005/11/30 12:47:42  csoutheren
 * Removed tabs, reformatted some code, and changed tags for Doxygen
 *
 * Revision 1.31  2005/03/10 03:27:30  dereksmithies
 * Fix an address typo.
 *
 * Revision 1.30  2005/01/03 02:52:52  csoutheren
 * Fixed problem with default speed of serial ports
 * Fixed problem with using obsolete lock directory for serial ports
 *
 * Revision 1.29  2004/07/11 07:56:36  csoutheren
 * Applied jumbo VxWorks patch, thanks to Eize Slange
 *
 * Revision 1.28  2004/02/22 04:06:47  ykiryanov
 * ifdef'd all functions because BeOS don't support it
 *
 * Revision 1.27  2002/11/02 00:32:21  robertj
 * Further fixes to VxWorks (Tornado) port, thanks Andreas Sikkema.
 *
 * Revision 1.26  2002/10/17 13:44:27  robertj
 * Port to RTEMS, thanks Vladimir Nesic.
 *
 * Revision 1.25  2002/10/10 04:43:44  robertj
 * VxWorks port, thanks Martijn Roest
 *
 * Revision 1.24  2002/03/27 06:42:16  robertj
 * Implemented the DTR etc functions and ttya/ttyb strings for sunos,
 *    thanks tommi.korhonen@insta.fi & Raimo Ruokonen <rruokonen@koti.soon.fi>
 *
 * Revision 1.23  2001/09/10 03:03:36  robertj
 * Major change to fix problem with error codes being corrupted in a
 *   PChannel when have simultaneous reads and writes in threads.
 *
 * Revision 1.22  2001/08/11 15:38:43  rogerh
 * Add Mac OS Carbon changes from John Woods <jfw@jfwhome.funhouse.com>
 *
 * Revision 1.21  2001/01/04 17:57:41  rogerh
 * Fix a cut and past error in my previous commit
 *
 * Revision 1.20  2001/01/04 10:28:07  rogerh
 * FreeBSD does not set the Baud Rate with c_cflags. Add the 'BSD' way
 *
 * Revision 1.19  2001/01/03 10:56:01  rogerh
 * CBAUD is not defined on FreeBSD.
 *
 * Revision 1.18  2000/12/29 07:36:18  craigs
 * Finally got working correctly!
 *
 * Revision 1.17  2000/11/14 14:56:24  rogerh
 * Fix #define parameters (fd should be just f)
 *
 * Revision 1.16  2000/11/14 14:52:32  rogerh
 * Fix SET/GET typo error
 *
 * Revision 1.15  2000/11/12 23:30:41  craigs
 * Fixed problems with serial port configuration
 *
 * Revision 1.14  2000/06/21 01:01:22  robertj
 * AIX port, thanks Wolfgang Platzer (wolfgang.platzer@infonova.at).
 *
 * Revision 1.13  2000/04/09 18:19:23  rogerh
 * Add my changes for NetBSD support.
 *
 * Revision 1.12  2000/04/06 12:11:32  rogerh
 * MacOS X support submitted by Kevin Packard
 *
 * Revision 1.11  2000/03/08 12:17:09  rogerh
 * Add OpenBSD support
 *
 * Revision 1.10  1998/12/21 06:08:08  robertj
 * Fixed warning on solaris x86 GNU system.
 *
 * Revision 1.9  1998/11/30 21:51:54  robertj
 * New directory structure.
 *
 * Revision 1.8  1998/11/24 09:39:14  robertj
 * FreeBSD port.
 *
 * Revision 1.7  1998/09/24 04:12:17  robertj
 * Added open software license.
 *
 */
 
#pragma implementation "serchan.h"
#pragma implementation "modem.h"

#include <ptlib.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/ioctl.h>

#if defined(P_LINUX)
#define  TCSETATTR(f,t)  tcsetattr(f,TCSANOW,t)
#define  TCGETATTR(f,t)  tcgetattr(f,t)

#elif defined(P_FREEBSD) || defined(P_OPENBSD) || defined (P_NETBSD) || defined(P_MACOSX) || defined(P_MACOS) || defined(P_RTEMS)
#include <sys/ttycom.h>
#define TCGETA TIOCGETA
#define TCSETAW TIOCSETAW

#elif defined(P_SUN4)
#include <sys/termio.h>
extern "C" int ioctl(int, int, void *);

#elif defined (P_AIX)
#include <sys/termio.h>
#endif


#ifndef  TCSETATTR
#define  TCSETATTR(f,t)  ::ioctl(f,TCSETAW,t)
#endif

#ifndef  TCGETATTR
#define  TCGETATTR(f,t)  ::ioctl(f,TCGETA,t)
#endif

//#define BINARY_LOCK  1
//#define  LOCK_PREFIX  "/var/spool/uucp/LCK.."
#define  LOCK_PREFIX  "/var/lock/LCK.."
#define  DEV_PREFIX  "/dev/"

#define  PORTLISTENV  "PWLIB_SERIALPORTS"
#define  DEV_PREFIX  "/dev/"

#include "../common/serial.cxx"

////////////////////////////////////////////////////////////////
//
//  PSerialChannel
//

void PSerialChannel::Construct()
{
  // set control modes: 9600, N, 8, 1, local line
  baudRate   = 9600;
  dataBits   = 8;
  parityBits = NoParity;
  stopBits   = 1;

#if defined(P_VXWORKS) || defined (__BEOS__)
  PAssertAlways(PUnimplementedFunction);
#else

  // set input mode: ignore breaks, ignore parity errors, do not strip chars,
  // no CR/NL conversion, no case conversion, no XON/XOFF control,
  // no start/stop
  Termio.c_iflag = IGNBRK | IGNPAR;
  Termio.c_cflag = CS8 | CSTOPB | CREAD | CLOCAL;

#if defined(P_FREEBSD) || defined(P_OPENBSD) || defined (P_NETBSD) || defined(P_MACOSX) || defined(P_MACOS)
  Termio.c_ispeed = Termio.c_ospeed = B9600;
#else
  Termio.c_cflag |= B9600;
#endif

  // set output mode: no post process output, 
  Termio.c_oflag = 0;

  // set line discipline
  Termio.c_lflag = 0;

#endif // P_VXWORKS
}

BOOL PSerialChannel::Close()
{
#if defined(P_VXWORKS) || defined (__BEOS__)
  PAssertAlways(PUnimplementedFunction);
  return FALSE;
#else
  if (os_handle >= 0) {

    // delete the lockfile
    PFile::Remove(PString(LOCK_PREFIX) + channelName);

    // restore the original terminal settings
    TCSETATTR(os_handle, &oldTermio);
  }

  return PChannel::Close();
#endif // P_VXWORKS
}


BOOL PSerialChannel::Open(const PString & port, 
                                    DWORD speed,
                                     BYTE data,
                                   Parity parity,
                                     BYTE stop,
                              FlowControl inputFlow,
                              FlowControl outputFlow)
{
  // if the port is already open, close it
  if (IsOpen())
    Close();

//  // check prefix of name
//  if (port.Left(PORT_PREFIX_LEN) != PORT_PREFIX) {
//    lastError = BadParameter;
//    return FALSE;
//  }

//  // check suffix
//  int portnum = (port.Right(port.GetLength()-PORT_PREFIX_LEN)).AsInteger();
//  if ((portnum < PORT_START) || (portnum >= (PORT_START + PORT_COUNT))) {
//    lastError = BadParameter;
//    return FALSE;
//  }

  // save the port name
  channelName = port;

#if defined(P_VXWORKS) || defined (__BEOS__)
  PAssertAlways(PUnimplementedFunction);
  return FALSE;
#else

  // construct lock filename 
  PString lockfilename = PString(LOCK_PREFIX) + port;

  // if the file exists, probe the process to see if it is still running
  if (PFile::Exists(lockfilename)) {
    PFile lockfile(lockfilename, PFile::ReadOnly);

    int lock_pid;
#ifdef BINARY_LOCK
    lockfile.Read(&lock_pid, sizeof(lock_pid));
#else
    char lock_pid_str[20];
    lockfile.Read(lock_pid_str, 20);
    lock_pid = atoi(lock_pid_str);
#endif
    
    // if kill returns 0, then the port is in use
    if (kill(lock_pid, 0) == 0)
      return SetErrorValues(DeviceInUse, EBUSY);

    // remove the lock file
    lockfile.Remove();
  }

  // create new lockfile with our PID
  PFile lockfile(lockfilename, PFile::WriteOnly, PFile::Create);
  int pid = getpid();
#ifdef BINARY_LOCK
  lockfile.Write(&pid, sizeof(pid));
#else
  lockfile << pid;
#endif
  lockfile.Close();

  // attempt to open the device
  PString device_name = PString(DEV_PREFIX) + port;
  if ((os_handle = ::open((const char *)device_name, O_RDWR|O_NONBLOCK|O_NOCTTY)) < 0) {
    ConvertOSError(os_handle);
    Close();
    return FALSE;
  }

  // save the channel name
  channelName = port;

  // save the current port setup
  TCGETATTR(os_handle, &oldTermio);

  // set the default paramaters
  TCSETATTR(os_handle, &Termio);

  // now set the mode that was passed in
  if (!SetSpeed(speed) ||
      !SetDataBits(data) ||
      !SetParity(parity) ||
      !SetStopBits(stop) ||
      !SetInputFlowControl(inputFlow) ||
      !SetOutputFlowControl(outputFlow)) {
    errno = EINVAL;
    ConvertOSError(-1);
    return FALSE;
  }

  ::fcntl(os_handle, F_SETFD, 1);

#endif // P_VXWORKS

  return TRUE;
}

BOOL PSerialChannel::SetSpeed(DWORD newBaudRate)
{
  if (newBaudRate == baudRate)
    return TRUE;

  if (os_handle < 0)
    return TRUE;

#if defined(P_VXWORKS) || defined (__BEOS__)
  PAssertAlways(PUnimplementedFunction);
  return FALSE;
#else

  int baud;

  switch(newBaudRate) {
#ifdef B50
    case 50:
      baud = B50;
      break;
#endif
#ifdef B75
    case 75:
      baud = B75;
      break;
#endif
#ifdef B110
    case 110:
      baud = B110;
      break;
#endif
#ifdef B134
    case 134:
      baud = B134;
      break;
#endif
#ifdef B150
    case 150:
      baud = B150;
      break;
#endif
#ifdef B200
    case 200:
      baud = B200;
      break;
#endif
#ifdef B300
    case 300:
      baud = B300;
      break;
#endif
#ifdef B600
    case 600:
      baud = B600;
      break;
#endif
#ifdef B1200
    case 1200:
      baud = B1200;
      break;
#endif
#ifdef B1800
    case 1800:
      baud = B1800;
      break;
#endif
#ifdef B2400
    case 2400:
      baud = B2400;
      break;
#endif
#ifdef B4800
    case 4800:
      baud = B4800;
      break;
#endif
#ifdef B9600
    case 9600:
    case 0:          // default
      baud = B9600;
      break;
#endif
#ifdef B19200
    case 19200:
      baud = B19200;  
      break;
#endif
#ifdef B38400
    case 38400:
      baud = B38400;  
      break;
#endif
#ifdef B57600
    case 57600:
      baud = B57600;
      break;
#endif
#ifdef B115200
    case 115200:
      baud = B115200;
      break;
#endif
#ifdef B230400
    case 230400:
      baud = B230400;
      break;
#endif
    default:
      baud = -1;
  };
 
  if (baud == -1) {
    errno = EINVAL;
    ConvertOSError(-1);
    return FALSE;
  }

  // save new baud rate
  baudRate = newBaudRate;

#if defined(P_FREEBSD) || defined(P_OPENBSD) || defined (P_NETBSD) || defined(P_MACOSX) || defined(P_MACOS)
  // The BSD way
  Termio.c_ispeed = baud; 
  Termio.c_ospeed = baud;
#else
  // The Linux way
  Termio.c_cflag &= ~CBAUD;
  Termio.c_cflag |= baud;
#endif

  if (os_handle < 0)
    return TRUE;

  // initialise the port
  return ConvertOSError(TCSETATTR(os_handle, &Termio));

#endif // P_VXWORKS
}


BOOL PSerialChannel::SetDataBits(BYTE data)
{
  if (data == dataBits)
    return TRUE;

#if defined(P_VXWORKS) || defined (__BEOS__)
  PAssertAlways(PUnimplementedFunction);
  return FALSE;
#else

  int flags;

  switch (data) {
#ifdef CS5
    case 5:
      flags = CS5;
      break;
#endif
#ifdef CS6
    case 6:
      flags = CS6;
      break;
#endif
#ifdef CS7
    case 7:
      flags = CS7;
      break;
#endif
#ifdef CS8
    case 8:
    case 0:  // default 
      flags = CS8;
      break;
#endif
    default:
      flags = -1;
      break;
  }

  if (flags == 0) {
    errno = EINVAL;
    ConvertOSError(-1);
    return FALSE;
  }

  // set the new number of data bits
  dataBits = data;
  Termio.c_cflag &= ~CSIZE;
  Termio.c_cflag |= flags;

  if (os_handle < 0)
    return TRUE;

  return ConvertOSError(TCSETATTR(os_handle, &Termio));

#endif // P_VXWORKS
}

BOOL PSerialChannel::SetParity(Parity parity)
{
  if (parity == parityBits)
    return TRUE;

#if defined(P_VXWORKS) || defined (__BEOS__)
  PAssertAlways(PUnimplementedFunction);
  return FALSE;
#else

  int flags;

  switch (parity) {
    case OddParity:
      flags = PARODD | PARENB;
      break;
    case EvenParity:
      flags = PARENB;
    case NoParity:
    case DefaultParity:
      flags = IGNPAR;
      break;

    case MarkParity:
    case SpaceParity:
    default:
      flags = -1;
  }

  if (flags < 0) {
    errno = EINVAL;
    ConvertOSError(-1);
    return FALSE;
  }

  if (os_handle < 0)
    return TRUE;

  // set the new parity
  parityBits = parity;
  Termio.c_cflag &= ~(PARENB|PARODD);
  Termio.c_cflag |= flags;

  return ConvertOSError(TCSETATTR(os_handle, &Termio));

#endif // P_VXWORKS
}

BOOL PSerialChannel::SetStopBits(BYTE stop)
{
  if (stop == stopBits)
    return TRUE;

#if defined(P_VXWORKS) || defined (__BEOS__)
  PAssertAlways(PUnimplementedFunction);
  return FALSE;
#else

  int flags;

  switch (stop) {
    case 2:
      flags = CSTOPB;
      break;
    default:
    case 1:
      flags = 0;
      break;
  }

  if (flags < 0) {
    errno = EINVAL;
    ConvertOSError(-1);
    return FALSE;
  }

  if (os_handle < 0)
    return TRUE;

  // set the new number of stop bits
  stopBits = stop;
  Termio.c_cflag &= ~CSTOPB;
  Termio.c_cflag |= flags;

  return ConvertOSError(TCSETATTR(os_handle, &Termio));

#endif // P_VXWORKS
}

DWORD PSerialChannel::GetSpeed() const
{
  return baudRate;
}

BYTE PSerialChannel::GetStopBits() const
{
  return stopBits;
}

BYTE PSerialChannel::GetDataBits() const
{
  return dataBits;
}

PSerialChannel::Parity PSerialChannel::GetParity() const
{
  return parityBits;
}

BOOL PSerialChannel::SetInputFlowControl(FlowControl)
{
  return TRUE;
}


PSerialChannel::FlowControl PSerialChannel::GetInputFlowControl() const
{
  return NoFlowControl;
}


BOOL PSerialChannel::SetOutputFlowControl(FlowControl)
{
  return TRUE;
}


PSerialChannel::FlowControl PSerialChannel::GetOutputFlowControl() const
{
  return NoFlowControl;
}


void PSerialChannel::SetDTR(BOOL mode)
{
#if defined(P_VXWORKS) || defined (__BEOS__)
  PAssertAlways(PUnimplementedFunction);
#else

  int flags = 0;
  ioctl(os_handle,TIOCMGET,&flags);  // get the bits
  flags &= ~TIOCM_DTR;
  if ( mode == TRUE )
    flags |= TIOCM_DTR;
  ioctl(os_handle,TIOCMSET,&flags);  // set back

  /* 
  ALTERNATE IMPLEMENTATION?
  Uses "Local Mode" bits?
  if ( mode TRUE )
    ioctl(os_handle, TIOCSDTR, 0);
  else 
    ioctl(os_handle, TIOCCDTR, 0);
  */

#endif // P_VXWORKS
}


void PSerialChannel::SetRTS(BOOL mode)
{
#if defined(P_VXWORKS) || defined (__BEOS__)
  PAssertAlways(PUnimplementedFunction);
#else

  int flags = 0;
  ioctl(os_handle,TIOCMGET,&flags);  // get the bits
  flags &= ~TIOCM_RTS;
  if ( mode == TRUE )
    flags |= TIOCM_RTS;
  ioctl(os_handle,TIOCMSET,&flags);  // set back

#endif // P_VXWORKS
}


void PSerialChannel::SetBreak(BOOL mode)
{
#if defined(P_VXWORKS) || defined (__BEOS__)
  PAssertAlways(PUnimplementedFunction);
#else

  if (mode)
    ioctl(os_handle, TIOCSBRK, 0);
  else 
    ioctl(os_handle, TIOCCBRK, 0);

#endif // P_VXWORKS
}


BOOL PSerialChannel::GetCTS()
{
#if defined(P_VXWORKS) || defined (__BEOS__)
  PAssertAlways(PUnimplementedFunction);
  return FALSE;
#else

  int flags = 0;
  ioctl(os_handle,TIOCMGET,&flags);  // get the bits
  return (flags&TIOCM_CTS)?TRUE:FALSE;

#endif // P_VXWORKS
}


BOOL PSerialChannel::GetDSR()
{
#if defined(P_VXWORKS) || defined (__BEOS__)
  PAssertAlways(PUnimplementedFunction);
  return FALSE;
#else

  int flags = 0;

  ioctl(os_handle,TIOCMGET,&flags);  // get the bits
  return (flags&TIOCM_DSR)?TRUE:FALSE;

#endif // P_VXWORKS
}


BOOL PSerialChannel::GetDCD()
{
#if defined(P_VXWORKS) || defined (__BEOS__)
  PAssertAlways(PUnimplementedFunction);
  return FALSE;
#else

  int flags = 0;

  ioctl(os_handle,TIOCMGET,&flags);  // get the bits
  return (flags&TIOCM_CD)?TRUE:FALSE;

#endif // P_VXWORKS
}


BOOL PSerialChannel::GetRing()
{
#if defined(P_VXWORKS) || defined (__BEOS__)
  PAssertAlways(PUnimplementedFunction);
  return FALSE;
#else

  int flags = 0;
  
  ioctl(os_handle,TIOCMGET,&flags);  // get the bits
  return (flags&TIOCM_RNG)?TRUE:FALSE;

#endif // P_VXWORKS
}


PStringList PSerialChannel::GetPortNames()
{
  PStringList ports;

  char * env = getenv(PORTLISTENV);
  if (env != NULL) {
    PString str(env);
    PStringArray tokens = str.Tokenise(" ,\t", FALSE);
    PINDEX i;
    for (i = 0; i < tokens.GetSize(); i++) 
      ports.AppendString(tokens[i]);
  } else {
#if defined(__sun) && defined (__sparc)
    ports.AppendString(PString("ttya"));
    ports.AppendString(PString("ttyb"));
#else
    ports.AppendString(PString("ttyS0"));
    ports.AppendString(PString("ttyS1"));
    ports.AppendString(PString("ttyS2"));
    ports.AppendString(PString("ttyS3"));
#endif
  }

  return ports;
}


// End of File ///////////////////////////////////////////////////////////////
