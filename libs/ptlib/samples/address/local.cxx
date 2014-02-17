/*
 * local.cxx
 *
 * copyright 2005 Derek J Smithies
 *
 *
 * Simple program to report the host name of this machine
 *                          the address of one network interface
 *
 * $Log: local.cxx,v $
 * Revision 1.3  2005/11/30 12:47:39  csoutheren
 * Removed tabs, reformatted some code, and changed tags for Doxygen
 *
 * Revision 1.2  2005/03/10 09:27:42  dereksmithies
 * Improve text output.
 *
 * Revision 1.1  2005/03/10 09:19:01  dereksmithies
 * Initial release of code to illustrate the reading of the machines external ip address and hostname
 *
 *
 *
 */
#include <ptlib.h>
#include <ptlib/sockets.h>

class LocalAddress : public PProcess
{
    PCLASSINFO(LocalAddress, PProcess);
public:
    LocalAddress();
    
    void Main();
};

PCREATE_PROCESS(LocalAddress);

LocalAddress::LocalAddress()
    : PProcess("PwLib Example Factory", "local", 1, 0, ReleaseCode, 0)
{
}

void LocalAddress::Main()
{
    PStringStream progName;
    progName << "Product Name: " << GetName() << endl
             << "Manufacturer: " << GetManufacturer() << endl
             << "Version     : " << GetVersion(TRUE) << endl
             << "System      : " << GetOSName() << '-'
             << GetOSHardware() << ' '
             << GetOSVersion();
    cout << endl <<  progName << endl << endl;
    

    PUDPSocket localSocket;
    PIPSocket::Address addr;
    if(localSocket.GetNetworkInterface(addr)) {
        cout << "local address is    " << addr.AsString() << endl;
        if (addr == 0)
            cout << "sorry, that is a 0.0.0.0 address" << endl;
    } else
      cout << "Sorry, failed to get local address" << endl;
    
    cout << "local host name is  " << localSocket.GetHostName() << endl;
}

// End of local.cxx
