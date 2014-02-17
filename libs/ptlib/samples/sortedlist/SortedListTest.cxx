
#include <ptlib.h>
#include "SortedListTest.h"
#include <ptclib/random.h>


PCREATE_PROCESS(SortedListTest);

PMutex coutMutex;

SortedListTest::SortedListTest():PProcess("Reitek S.p.A.", "SortedListTest", 0, 0, BetaCode, 0)
{
}

void SortedListTest::Main() {

  PINDEX i;
  for (i = 0; i < 10; i++) {
    new DoSomeThing(i);
  }

  for (PINDEX j = i; j < i + 5; j++) {
    new DoSomeThing2(j);
  }

  Suspend();
}


DoSomeThing::DoSomeThing(PINDEX _index)
:PThread(1000, AutoDeleteThread, NormalPriority, psprintf("DoSomeThing %u", _index)), index(_index) {
  Resume();
}


void DoSomeThing::Main() {

  list.AllowDeleteObjects();

  PRandom rand(PRandom::Number());

  PINDEX i;

  for (i = 0; i < 5000; i++) {
    PString * p = new PString(rand.Generate());
    list.Append(p);
//    coutMutex.Wait();
//    cout << GetThreadName() << ": Added " << *p << " element to sorted list" << endl;
//    coutMutex.Signal();
  }

  for (;;) {

    PINDEX remove = rand.Generate() % (list.GetSize() + 1);
    for (i = 0; i < remove; i++) {
      PINDEX index = rand.Generate() % list.GetSize();
      coutMutex.Wait();
      cout << GetThreadName() << ": Removing element " << list[index] << " at index position " << index << endl;
      coutMutex.Signal();
      if (index%2)
        list.Remove(&list[index]);
      else
        list.RemoveAt(index);
    }

    PINDEX add = rand.Generate() % 1000 + 300;
    for (i = 0; i < add; i++) {
      PString * p = new PString(rand.Generate());
      coutMutex.Wait();
      cout << GetThreadName() << ": Adding element " << *p << "to sorted list" << endl;
      coutMutex.Signal();
      list.Append(p);
    }
  }
}


PSafeString::PSafeString(const PString & _string):string(_string) {
}


void PSafeString::PrintOn(ostream &strm) const {
  strm << string;
}


DoSomeThing2::DoSomeThing2(PINDEX _index)
:PThread(1000, AutoDeleteThread, NormalPriority, psprintf("DoSomeThing2 %u", _index)), index(_index) {
  Resume();
}


void DoSomeThing2::Main() {

  PRandom rand(PRandom::Number());

  PINDEX i;

  for (i = 0; i < 5000; i++) {
    PSafeString * p = new PSafeString(rand.Generate());
    list.Append(p);
//    coutMutex.Wait();
//    cout << GetThreadName() << ": Added " << *p << " element to sorted list" << endl;
//    coutMutex.Signal();
  }

  for (;;) {

    PINDEX remove = rand.Generate() % (list.GetSize() + 1);
    for (i = 0; i < remove; i++) {
      PINDEX index = rand.Generate() % list.GetSize();
      coutMutex.Wait();
      PSafePtr<PSafeString> str = list.GetAt(index, PSafeReference);
      cout << GetThreadName() << ": Removing element " << *str << " at index position " << index << endl;
      coutMutex.Signal();
      list.Remove(&(*str));
    }

    PINDEX add = rand.Generate() % 1000 + 300;
    for (i = 0; i < add; i++) {
      PSafeString * p = new PSafeString(rand.Generate());
      coutMutex.Wait();
      cout << GetThreadName() << ": Adding element " << *p << "to sorted list" << endl;
      coutMutex.Signal();
      list.Append(p);
    }

    list.DeleteObjectsToBeRemoved();
  }
}


