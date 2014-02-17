/*
 * main.cxx
 *
 * PWLib application source file for stunclient
 *
 * Main program entry point.
 *
 * Copyright (c) 2003 Equivalence Pty. Ltd.
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
 * $Log: main.cxx,v $
 * Revision 1.7  2007/09/17 11:14:45  rjongbloed
 * Added "No Trace" build configuration.
 *
 * Revision 1.6  2004/10/26 06:00:45  csoutheren
 * Added -t and -o options
 *
 * Revision 1.5  2004/02/24 11:15:48  rjongbloed
 * Added function to get external router address, also did a bunch of documentation.
 *
 * Revision 1.4  2003/02/05 06:26:49  robertj
 * More work in making the STUN usable for Symmetric NAT systems.
 *
 * Revision 1.3  2003/02/04 07:02:55  robertj
 * Removed ports in test, added delete of created udp socket.
 *
 * Revision 1.2  2003/02/04 05:23:59  craigs
 * Added new functions
 *
 * Revision 1.1  2003/02/04 03:31:04  robertj
 * Added STUN
 *
 */

#include <ptlib.h>
#include "main.h"
#include "version.h"

#include <ptclib/pstun.h>


PCREATE_PROCESS(StunClient);



StunClient::StunClient()
  : PProcess("Equivalence", "stunclient", MAJOR_VERSION, MINOR_VERSION, BUILD_TYPE, BUILD_NUMBER)
{
}


void StunClient::Main()
{
  PArgList & args = GetArguments();
#if PTRACING
  args.Parse("t-trace."       "-no-trace."
             "o-output:"      "-no-output.");

  PTrace::Initialise(args.GetOptionCount('t'),
                   args.HasOption('o') ? (const char *)args.GetOptionString('o') : NULL,
                   PTrace::Blocks | PTrace::Timestamp | PTrace::Thread | PTrace::FileAndLine);
#endif

  WORD portbase, portmax;

  switch (args.GetCount()) {
    case 0 :
      cout << "usage: stunclient stunserver [ portbase [ portmax ]]\n";
      return;
    case 1 :
      portbase = 0;
      portmax = 0;
      break;
    case 2 :
      portbase = (WORD)args[1].AsUnsigned();
      portmax = (WORD)(portbase+9);
      break;
    default :
      portbase = (WORD)args[1].AsUnsigned();
      portmax = (WORD)args[2].AsUnsigned();
  }

  PSTUNClient stun(args[0], portbase, portmax, portbase, portmax);
  cout << "NAT type: " << stun.GetNatTypeName() << endl;

  PIPSocket::Address router;
  if (!stun.GetExternalAddress(router)) {
    cout << "Could not get router address!" << endl;
    return;
  }
  cout << "Router address: " << router << endl;

  PUDPSocket * udp;
  if (!stun.CreateSocket(udp)) {
    cout << "Cannot create a socket!" << endl;
    return;
  }

  PIPSocket::Address addr;
  WORD port;
  udp->GetLocalAddress(addr, port);
  cout << "Socket local address reported as " << addr << ":" << port << endl;

  delete udp;

  PUDPSocket * udp1, * udp2;
  if (!stun.CreateSocketPair(udp1, udp2)) {
    cout << "Cannot create socket pair" << endl;
    return;
  }

  udp1->GetLocalAddress(addr, port);
  cout << "Socket 1 local address reported as " << addr << ":" << port << endl;
  udp2->GetLocalAddress(addr, port);
  cout << "Socket 2 local address reported as " << addr << ":" << port << endl;

  delete udp1;
  delete udp2;
}


// End of File ///////////////////////////////////////////////////////////////
