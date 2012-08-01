/*
 * dossock.cxx
 *
 * MS-DOS imeplmentation of sockets.
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
 * $Log: dossock.cxx,v $
 * Revision 1.3  1998/09/24 03:30:42  robertj
 * Added open software license.
 *
 * Revision 1.2  1995/02/05 00:53:15  robertj
 * Commonised out of band stuff.
 *
 */

#include <ptlib.h>
#include <sockets.h>


//////////////////////////////////////////////////////////////////////////////
// PSocket

PSocket::PSocket()
{
}


BOOL PSocket::Read(void * buf, PINDEX len)
{
  return TRUE;
}


BOOL PSocket::Write(const void * buf, PINDEX len)
{
  return TRUE;
}


BOOL PSocket::Close()
{
  if (IsOpen())
    return FALSE;
//  return ConvertOSError(closesocket(os_handle));
  return FALSE;
}


BOOL PSocket::ConvertOSError(int error)
{
  if (error >= 0) {
    lastError = NoError;
    osError = 0;
    return TRUE;
  }

//  osError = WSAGetLastError();
  switch (osError) {
    case 0 :
      lastError = NoError;
      return TRUE;
    default :
      lastError = Miscellaneous;
      osError |= 0x20000000;
  }
  return FALSE;
}


// End Of File ///////////////////////////////////////////////////////////////
