//////////////////////////////////////////////////

#include <ptlib.h>
#include <ptlib/pprocess.h>
#include <ptclib/psockbun.h>


class SockBundleProcess : public PProcess
{
  PCLASSINFO(SockBundleProcess, PProcess)

  public:
    SockBundleProcess();
    void Main();
};

PCREATE_PROCESS(SockBundleProcess);


SockBundleProcess::SockBundleProcess()
{
}


void SockBundleProcess::Main()
{
  PMonitoredSocketBundle bundle;
  if (!bundle.Open(5080)) {
    cout << "Cannot open monitored socket bundle" << endl;
    return;
  }

  PSingleMonitoredSocket single(bundle.GetInterfaces()[0]);
  if (!single.Open(1719)) {
    cout << "Cannot open single monitored socket" << endl;
    return;
  }

  cout << "Initial interfaces:" << endl;
  for (;;) {
    PStringArray interfaces = bundle.GetInterfaces();
    PINDEX i;
    for (i = 0; i < interfaces.GetSize(); ++i) {
      cout << "  #" << i+1 << ' ';

      PIPSocket::InterfaceEntry entry;
      if (!bundle.GetInterfaceInfo(interfaces[i], entry)) 
        cout << interfaces[i] << " is no longer active" << endl;
      else
        cout << entry << endl;
    }

    cout << endl << "\nWaiting for interface changes" << endl;

    Sleep(10000);

    cout << "\nCurrent interfaces:" << endl;
  }
}
