
#include <ptlib.h>
#include <ptlib/pprocess.h>
#include <ptlib/safecoll.h>

class SortedListTest:public PProcess {
  PCLASSINFO(SortedListTest, PProcess);
public:
  SortedListTest();
  void Main();
};


class DoSomeThing:public PThread {
  PCLASSINFO(DoSomeThing, PThread);
public:
  DoSomeThing(PINDEX _index);
  void Main();
private:
  PINDEX index;
  PSortedList<PString> list;
};

class PSafeString:public PSafeObject {
  PCLASSINFO(PSafeString, PSafeObject);
  PSafeString(const PString & _string);
  void PrintOn(ostream &strm) const;
private:
  PString string;
};

class DoSomeThing2:public PThread {
  PCLASSINFO(DoSomeThing2, PThread);
public:
  DoSomeThing2(PINDEX _index);
  void Main();
private:
  PINDEX index;
  PSafeSortedList<PSafeString> list;
};


