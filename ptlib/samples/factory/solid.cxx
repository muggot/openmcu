#include "abstract.h"

class MySolidClass : public MyAbstractClass
{
  public:
    PString Function()
    { return "Solid"; }
};

PFactory<MyAbstractClass>::Worker<MySolidClass> solidFactory("solid", true);

class MySolid2Class : public MyAbstractClass
{
  public:
    PString Function()
    { return "Solid2"; }
};

PFactory<MyAbstractClass>::Worker<MySolid2Class> solid2Factory("solid2", true);
