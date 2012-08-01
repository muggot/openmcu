
#include "abstract.h"

#include <ptlib/pprocess.h>

#include <ptclib/http.h>
#include <ptclib/ptts.h>
#include <ptclib/pwavfile.h>



class Factory : public PProcess
{
  public:
    Factory()
    : PProcess() { }
    void Main();
};

PCREATE_PROCESS(Factory)


template <class BaseClass, class TypeClass = PString>
class Display
{
public:
  static void ConcreteTypes(const char * title)
  {
    cout << "\nConcrete types for " << title << ':' << endl;
    typename PFactory<BaseClass, TypeClass>::KeyList_T keys = PFactory<BaseClass, TypeClass>::GetKeyList();
    typename PFactory<BaseClass, TypeClass>::KeyList_T::const_iterator r;
    for (r = keys.begin(); r != keys.end(); ++r)
      cout << "  " << *r << endl;
  
    cout << endl;
  }

  static void TestFactory()
  {
    typename PFactory<BaseClass, TypeClass>::KeyList_T keyList = PFactory<BaseClass, TypeClass>::GetKeyList();
    unsigned i;
    for (i = 0; i < keyList.size(); i++) {
      for (int j = 0; j < 3; j++)
      {
        MyAbstractClass * c = PFactory<BaseClass, TypeClass>::CreateInstance(keyList[i]);
        if (c == NULL) 
          cout << "Cannot instantiate class " << keyList[i] << endl;
        else
          cout << keyList[i] << "::Function returned \"" << c->Function() << "\", instance " << (void *)c << endl;
      }
    }
  }
};


void Factory::Main()
{
  Display<MyAbstractClass>::ConcreteTypes("MyAbstractClass by PString");
  Display<MyAbstractClass>::TestFactory();

  Display<MyAbstractClass, unsigned>::ConcreteTypes("MyAbstractClass by unsigned");
  Display<MyAbstractClass, unsigned>::TestFactory();

  Display<PURLScheme>::ConcreteTypes("PURLScheme");
  Display<PTextToSpeech>::ConcreteTypes("PTextToSpeech");
  Display<PPluginModuleManager>::ConcreteTypes("PPluginModuleManager");
}
