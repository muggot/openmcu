/*
 * netif.cxx
 *
 * Sample program to query the network interfaces.
 *
 * Portable Windows Library
 *
 * Copyright (c) 2001 Roger Hardiman
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
 * The Initial Developer of the Original Code is Roger Hardiman
 *
 * $Log: netif.cxx,v $
 * Revision 1.4  2006/06/21 03:28:42  csoutheren
 * Various cleanups thanks for Frederic Heem
 *
 * Revision 1.3  2002/09/05 11:56:49  rogerh
 * Fix some warnings by using PINDEX
 *
 * Revision 1.2  2002/01/09 14:08:57  rogerh
 * Fix incorrect error message.
 *
 * Revision 1.1  2001/10/03 10:31:27  rogerh
 * Add netif, a program to display detains of the network interfaces
 * and the routing table.
 *
 *
 *
 */

/*
 * The network interface test program displays information about the
 * network connections and setup.
 * It displays the results of
 * GetInterfaceTable()
 * and GetRouteTable()
 */


#include <ptlib.h>
#include <ptlib/pprocess.h>
#include <ptlib/sockets.h>

class NetTest : public PProcess
{
  PCLASSINFO(NetTest, PProcess)
  public:
    void Main();
};


PCREATE_PROCESS(NetTest)

void NetTest::Main()
{

  cout << "Net Test Utility" << endl;
  cout << endl;

  // Read the interface table
  PIPSocket::InterfaceTable if_table;

  // Read the Interface Table
  if ( !PIPSocket::GetInterfaceTable( if_table ) ) {
        cout << "GetInterfaceTable() failed. No interface table" << endl;
        exit(0);
  }

  // Display the interface table
  cout << "The interface table has " << if_table.GetSize()
       <<" entries" << endl;

  for (PINDEX i=0; i < if_table.GetSize(); i++) {
    PIPSocket::InterfaceEntry if_entry = if_table[i];
    cout << i << " " << if_entry.GetName() << " "
                     << if_entry.GetAddress() << " "
                     << if_entry.GetNetMask() << " "
                     << if_entry.GetMACAddress() << "."
                     << endl;
  }
  cout << endl;

  // Read the Route Table
  PIPSocket::RouteTable rt_table;

  if ( !PIPSocket::GetRouteTable( rt_table ) ) {
        cout << "GetRouteTable() failed. No routing table" << endl;
        exit(0);
  }

  // Display the route table
  cout << "The route table has " << rt_table.GetSize()
       <<" entries" << endl;

  for (PINDEX i=0; i < rt_table.GetSize(); i++) {
    PIPSocket::RouteEntry rt_entry = rt_table[i];
    cout << i << " " << rt_entry.GetNetwork() << " "
                     << rt_entry.GetNetMask() << " "
                     << rt_entry.GetDestination() << " "
                     << rt_entry.GetInterface() << " "
                     << rt_entry.GetMetric() << "."
                     << endl;
  }
  cout << endl;

  // Get the Default Gateway
  PIPSocket::Address addr;
  PIPSocket::GetGatewayAddress(addr);
  cout << "Default gateway is " << addr << endl;
  cout << endl;

  // Get the interface for the Default Gateway
  PString gw_if = PIPSocket::GetGatewayInterface();
  if ( gw_if.IsEmpty() )
    cout << "No default gateway interface" << endl;
  else
    cout << "Gateway is on " << gw_if << endl;
  cout << endl;


}
// End of netif.cxx

