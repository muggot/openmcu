/*
 * main.cxx
 *
 * PWLib application source file for dtmftest
 *
 * Main program entry point.
 *
 * Copyright (C) 2004 Post Increment
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
 * $Log: main.cxx,v $
 * Revision 1.11  2007/04/17 23:40:54  csoutheren
 * Add ptbuildopts.h
 *
 * Revision 1.10  2006/02/10 22:38:03  csoutheren
 * Added ability to do DNS lookups and force IPV4 mode
 *
 * Revision 1.9  2005/11/30 12:47:40  csoutheren
 * Removed tabs, reformatted some code, and changed tags for Doxygen
 *
 * Revision 1.8  2005/02/13 23:04:18  csoutheren
 * Put isrfc1918 option handling before first test (doh!)
 *
 * Revision 1.7  2005/02/13 22:33:09  csoutheren
 * Added ability to test an IP address for RFC1918
 *
 * Revision 1.6  2005/02/07 12:12:34  csoutheren
 * Expanded interface list routines to include IPV6 addresses
 * Added IPV6 to GetLocalAddress
 *
 * Revision 1.5  2005/02/03 12:38:00  csoutheren
 * Added tests for URL parsing
 *
 * Revision 1.4  2005/01/15 19:27:07  csoutheren
 * Added test for GetInterfaceTable
 * Thanks to Jan Willamowius
 *
 * Revision 1.3  2004/12/14 14:24:20  csoutheren
 * Added PIPSocket::Address::operator*= to compare IPV4 addresses
 * to IPV4-compatible IPV6 addresses. More documentation needed
 * once this is tested as working
 *
 * Revision 1.2  2004/12/14 07:49:49  csoutheren
 * added some tests
 *
 * Revision 1.1  2004/12/14 06:50:59  csoutheren
 * Initial version
 *
 *
 */

#include "precompile.h"
#include "main.h"
#include "version.h"

#include <ptlib/sockets.h>
#include <ptclib/url.h>
#include <ptbuildopts.h>

PCREATE_PROCESS(IPV6Test);


IPV6Test::IPV6Test()
  : PProcess("Post Increment", "dtmftest", MAJOR_VERSION, MINOR_VERSION, BUILD_TYPE, BUILD_NUMBER)
{
}


void IPV6Test::Main()
{
  PArgList & args = GetArguments();

  args.Parse(
             "-isrfc1918."
             "4-ipv4." 
             "d-dns:"                
             "h-help."
             "v-version."

#if PTRACING
             "o-output:"             "-no-output."
             "t-trace."              "-no-trace."
#endif
       );

#if PTRACING
  PTrace::Initialise(args.GetOptionCount('t'),
                     args.HasOption('o') ? (const char *)args.GetOptionString('o') : NULL,
         PTrace::Blocks | PTrace::Timestamp | PTrace::Thread | PTrace::FileAndLine);
#endif

  if (args.HasOption('h')) {
    cout << "usage: ipv6test -v                display version information\n"
            "       ipv6test --isrfc1918 addr  check if specified address is RFC1918\n"
            "       ipv6test [-4] -d name      perform DNS lookup of hostname (with optional IPV4 force)\n"
            "       ipv6test                   perform a variety of IPV6 tests\n";
    return;
  }

  if (args.HasOption('v')) {
    cout << "Product Name: " << GetName() << endl
         << "Manufacturer: " << GetManufacturer() << endl
         << "Version     : " << GetVersion(TRUE) << endl
         << "System      : " << GetOSName() << '-'
         << GetOSHardware() << ' '
         << GetOSVersion() << endl;
    return;
  }

#if ! P_HAS_IPV6
  cout << "error: IPV6 not included in PWLib" << endl;
#else

  if (args.HasOption('4')) {
    cout << "forcing IPV4 mode" << endl;
    PIPSocket::SetDefaultIpAddressFamilyV4();
  }

  if (args.HasOption('d')) {
    PString name = args.GetOptionString('d');
    PIPSocket::Address addr;
    if (!PIPSocket::GetHostAddress(name, addr)) 
      PError << "error: hostname \"" << name << "\" not found" << endl;
    else
      cout << addr << endl;
    return;
  }

  if (args.HasOption("isrfc1918")) {
    if (args.GetCount() == 0) 
      PError << "error: must supply IP address as argument" << endl;
    else {
      PIPSocket::Address addr(args[0]);
      cout << addr << " is " << (addr.IsRFC1918() ? "" : "not ") << "an RFC1918 address" << endl;
    }
    return;
  }

  {
    // test #1 - check PIPSocket::IsIpAddressFamilyV6Supported
    cout << "test #1: PIPSocket::IsIpAddressFamilyV6Supported ";
    if (PIPSocket::IsIpAddressFamilyV6Supported())
        cout << "OK";
    else
        cout << "failed";
    cout << endl;
  }


  // this is an IPV6 compatibility address
  const BYTE ipv6Data[] = { 0x00, 0x00, 0x00, 0x00,
                            0x00, 0x00, 0x00, 0x00,
                            0x00, 0x00, 0xff, 0xff,
                            220,  244,  81,   10 };
  const PIPSocket::Address ipv6Addr1(sizeof(ipv6Data), ipv6Data);
  {
    // test #2 - check V6 constructor
    cout << "test #2: PIPSocket::Address(int, const BYTE *) ";
    if (ipv6Addr1.GetVersion() == 6) 
        cout << "OK";
    else
        cout << "failed";
    cout << endl;
  }

  PIPSocket::Address ipv6Addr2("::ffff:220.244.81.10");
  {
    // test #3 - check V6 constructor
    cout << "test #3: PIPSocket::Address(const PString & str) ";
    if (ipv6Addr1.GetVersion() == 6) 
        cout << "OK";
    else
        cout << "failed - " << ipv6Addr1 << " <-> " << ipv6Addr2;
    cout << endl;
  }
  
  {
    // test #4 - check comparison for equality
    cout << "test #4: PIPSocket::operator == ";
    if (ipv6Addr1 == ipv6Addr2)
        cout << "OK";
    else
        cout << "failed - " << ipv6Addr1 << " <-> " << ipv6Addr2;
    cout << endl;
  }
  
  PIPSocket::Address ipv6Addr3("::ffff:220.244.81.09");
  {
    // test #5 - check comparison for non-equality
    cout << "test #5: PIPSocket::operator != ";
    if (ipv6Addr1 != ipv6Addr3)
        cout << "OK";
    else
        cout << "failed";
    cout << endl;
  }

  PIPSocket::Address ipv4Addr("220.244.81.10");
  {
    // test #6 - check IPV6 comparison to IPV4
    cout << "test #6: PIPSocket::operator == with IPV4 (should fail) ";
    if (ipv6Addr1 == ipv4Addr)
        cout << "OK";
    else
        cout << "failed";
    cout << endl;
  }
  {
    // test #6a - check IPV6 comparison to IPV4
    cout << "test #6a: PIPSocket::operator == with IPV4 (should fail) ";
    if (ipv6Addr3 == ipv4Addr)
        cout << "OK";
    else
        cout << "failed";
    cout << endl;
  }

  {
    // test #7 - check IPV6 comparison to IPV4
    cout << "test #7: PIPSocket::operator *= with IPV4 ";
    if (ipv6Addr1 *= ipv4Addr)
        cout << "OK";
    else
        cout << "failed";
    cout << endl;
  }
  {
    // test #7a - check IPV6 comparison to IPV4
    cout << "test #7a: PIPSocket::operator *= with IPV4 (should fail) ";
    if (ipv6Addr3 *= ipv4Addr)
        cout << "OK";
    else
        cout << "failed";
    cout << endl;
  }
  {
    // test #8 - check if interface table contains IPV6 addresses
    cout << "test #8: check if interface table contains IPV6 addresses";

    PIPSocket::InterfaceTable if_table;
    PIPSocket::GetInterfaceTable( if_table );

    // Display the interface table
    cout << endl;
    cout << "The interface table has " << if_table.GetSize()
         <<" entries" << endl;

    for (PINDEX i=0; i < if_table.GetSize(); i++) {
      PIPSocket::InterfaceEntry if_entry = if_table[i];
      cout << i << " " << if_entry << endl;
    }
    cout << "manual check";
    cout << endl;
  }
  {
    // test #9 - see if URLs decode correctly
    cout << "test #9: check if parsing IPV6 URLs works" << endl;

    PURL url("h323:@[::ffff:220.244.81.10]:1234");
    PString addrStr = url.GetHostName();
    PIPSocket::Address addr;
    PIPSocket::GetHostAddress(addrStr, addr);
    WORD port = url.GetPort();
    cout << "  host string = " << addrStr << " (should be [::ffff:220.244.81.10])\n"
         << "  address     = " << addr    << " (should be ::ffff:220.244.81.10)\n"
         << "  port        = " << port    << " (should be 1234)\n";
  }
#endif
}

// End of File ///////////////////////////////////////////////////////////////
