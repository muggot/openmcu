/*
 * x224.cxx
 *
 * X.224 protocol handler
 *
 * Open H323 Library
 *
 * Copyright (c) 1998-2000 Equivalence Pty. Ltd.
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
 * The Original Code is Open H323 Library.
 *
 * The Initial Developer of the Original Code is Equivalence Pty. Ltd.
 *
 * Contributor(s): ______________________________________.
 *
 * $Log: x224.cxx,v $
 * Revision 1.1  2007/08/06 20:51:08  shorne
 * First commit of h323plus
 *
 * Revision 1.12  2002/09/03 06:21:01  robertj
 * Cosmetic change to formatting.
 *
 * Revision 1.11  2002/08/05 10:03:48  robertj
 * Cosmetic changes to normalise the usage of pragma interface/implementation.
 *
 * Revision 1.10  2002/06/27 03:11:12  robertj
 * Fixed encoding bugs, thanks Greg Adams
 *
 * Revision 1.9  2001/02/09 05:13:56  craigs
 * Added pragma implementation to (hopefully) reduce the executable image size
 * under Linux
 *
 * Revision 1.8  2000/05/02 04:32:28  robertj
 * Fixed copyright notice comment.
 *
 * Revision 1.7  2000/04/18 11:36:40  robertj
 * Fixed bug in setting data compont size in X224 packet, thanks Wolfgang Platzer
 *
 * Revision 1.6  1999/11/19 01:00:07  robertj
 * Fixed bug that disallowed zero length data PDU's, thanks Dave Kristol.
 *
 * Revision 1.5  1999/11/15 14:11:29  robertj
 * Fixed trace output stream being put back after setting hex/fillchar modes.
 *
 * Revision 1.4  1999/09/03 14:03:54  robertj
 * Fixed warning under GNU compiler.
 *
 * Revision 1.3  1999/08/31 13:30:20  robertj
 * Added gatekeeper support.
 *
 * Revision 1.2  1999/06/09 05:26:20  robertj
 * Major restructuring of classes.
 *
 * Revision 1.1  1998/12/14 09:13:48  robertj
 * Initial revision
 *
 */

#include <ptlib.h>

#ifdef __GNUC__
#pragma implementation "x224.h"
#endif

#include "x224.h"

#include <ptlib/sockets.h>


///////////////////////////////////////////////////////////////////////////////

X224::X224()
{
}


void X224::PrintOn(ostream & strm) const
{
  int indent = 2;
  strm << setprecision(indent) << "{\n"
       << setw(indent) << ' ' << "code=";
  switch (GetCode()) {
    case ConnectRequest :
      strm << "ConnectRequest";
      break;
    case ConnectConfirm :
      strm << "ConnectConfirm";
      break;
    case DataPDU :
      strm << "DataPDU";
  }

  char fillchar = strm.fill();

  strm << '\n'
       << setw(indent) << ' ' << "data: " << data.GetSize() << " bytes\n"
       << hex;

  PINDEX i = 0;
  while (i < data.GetSize()) {
    strm << setfill(' ') << setw(indent) << ' ' << setfill('0');
    PINDEX j;
    for (j = 0; j < 16; j++)
      if (i+j < data.GetSize())
        strm << setw(2) << (unsigned)data[i+j] << ' ';
      else
        strm << "   ";
    strm << "  ";
    for (j = 0; j < 16; j++) {
      if (i+j < data.GetSize()) {
        if (isprint(data[i+j]))
          strm << data[i+j];
        else
          strm << ' ';
      }
    }
    strm << '\n';
    i += 16;
  }
  strm << dec << setfill(fillchar)
       << setw(indent-1) << '}'
       << setprecision(indent-2);
}


BOOL X224::Decode(const PBYTEArray & rawData)
{
  PINDEX packetLength = rawData.GetSize();

  PINDEX headerLength = rawData[0];
  if (packetLength < headerLength + 1) // Not enough bytes
    return FALSE;

  header.SetSize(headerLength);
  memcpy(header.GetPointer(), (const BYTE *)rawData+1, headerLength);

  packetLength -= headerLength + 1;
  data.SetSize(packetLength);
  if (packetLength > 0)
    memcpy(data.GetPointer(), (const BYTE *)rawData+headerLength+1, packetLength);

  return TRUE;
}


BOOL X224::Encode(PBYTEArray & rawData) const
{
  PINDEX headerLength = header.GetSize();
  PINDEX dataLength = data.GetSize();

  if (!rawData.SetSize(headerLength + dataLength + 1))
    return FALSE;

  rawData[0] = (BYTE)headerLength;
  memcpy(rawData.GetPointer() + 1, header, headerLength);

  if (dataLength > 0)
    memcpy(rawData.GetPointer()+headerLength+1, data, dataLength);

  return TRUE;
}


void X224::BuildConnectRequest()
{
  data.SetSize(0);
  header.SetSize(6);
  header[0] = ConnectRequest;
  header[1] = 0;
  header[2] = 0x7b;
  header[3] = 2;
  header[4] = 0;
  header[5] = 0;
}


void X224::BuildConnectConfirm()
{
  data.SetSize(0);
  header.SetSize(6);
  header[0] = ConnectConfirm;
  header[1] = 0;
  header[2] = 0x7b;
  header[3] = 2;
  header[4] = 0;
  header[5] = 0;
}


void X224::BuildData(const PBYTEArray & d)
{
  header.SetSize(2);
  header[0] = DataPDU;
  header[1] = 0x80;
  data = d;
}



/////////////////////////////////////////////////////////////////////////////
