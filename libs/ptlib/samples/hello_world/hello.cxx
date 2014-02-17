//
// hello.cxx
//
// Equivalence Pty. Ltd.
//

#include <ptlib.h>
#include <ptlib/pprocess.h>

class Hello : public PProcess
{
  PCLASSINFO(Hello, PProcess)
  public:
    void Main();
};

PCREATE_PROCESS(Hello)

void Hello::Main()
{
  cout << "Hello world!\n\n"
          "From " << GetOSClass() << ' ' << GetOSName() << " (" << GetOSVersion() << ")"
          " on " << GetOSHardware() << endl;
}

// End of hello.cxx
