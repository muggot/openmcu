#include "abstract.h"

class MyConcreteClass : public MyAbstractClass
{
  public:
    PString Function()
    { return "Concrete"; }
};

PFactory<MyAbstractClass>::Worker<MyConcreteClass> concreteFactory("concrete", false);
PFactory<MyAbstractClass, unsigned>::Worker<MyConcreteClass> intConcreteFactory(1, false);

class MyConcrete2Class : public MyAbstractClass
{
  public:
    PString Function()
    { return "Concrete2"; }
};

PFactory<MyAbstractClass>::Worker<MyConcrete2Class> concrete2Factory("concrete2", false);
PFactory<MyAbstractClass, unsigned>::Worker<MyConcrete2Class> intConcrete2Factory(2, false);
