/*
 * main.cxx
 *
 * PWLib application source file for ThreadSafe
 *
 * Main program entry point.
 *
 * Copyright 2002 Equivalence
 *
 * $Log: main.cxx,v $
 * Revision 1.10  2007/09/17 11:14:42  rjongbloed
 * Added "No Trace" build configuration.
 *
 * Revision 1.9  2005/11/30 12:47:39  csoutheren
 * Removed tabs, reformatted some code, and changed tags for Doxygen
 *
 * Revision 1.8  2004/10/14 12:31:46  rjongbloed
 * Added synchronous mode for safe collection RemoveAll() to wait until all objects
 *   have actually been deleted before returning.
 *
 * Revision 1.7  2004/09/07 11:32:02  rjongbloed
 * Changed function name in PSafeCollection to something more standard for collections
 *
 * Revision 1.6  2004/04/04 13:24:19  rjongbloed
 * Changes to support native C++ Run Time Type Information
 *
 * Revision 1.5  2003/10/27 22:12:56  dereksmithies
 * Add more good changes to get Compare method work. Thanks to Gene Small
 *
 * Revision 1.4  2003/10/13 23:38:31  dereksmithies
 * Add debugging statements, usage(), Fixed Compare method. Thanks Gene Small.
 *
 * Revision 1.3  2002/12/11 03:38:45  robertj
 * Added more tests
 *
 * Revision 1.2  2002/05/02 00:30:26  robertj
 * Added dump of thread times during start up.
 *
 * Revision 1.1  2002/05/01 04:16:44  robertj
 * Added thread safe collection classes.
 *
 */

#include <ptlib.h>
#include "main.h"

#include <ptclib/random.h>


PCREATE_PROCESS(ThreadSafe);


///////////////////////////////////////////////////////////////////////////////

TestObject::TestObject(ThreadSafe & proc, unsigned val)
  : process(proc)
{
  value = val;

  process.mutexObjects.Wait();
  process.totalObjects++;
  process.currentObjects++;
  process.mutexObjects.Signal();
}


TestObject::~TestObject()
{
  process.mutexObjects.Wait();
  process.currentObjects--;
  process.mutexObjects.Signal();
}


PObject::Comparison TestObject::Compare(const PObject & obj) const
{
  PAssert(PIsDescendant(&obj, TestObject), PInvalidCast);
  unsigned othervalue = ((const TestObject &)obj).value;
      
  if (value < othervalue)
    return LessThan;
  if (value > othervalue)
    return GreaterThan; 
  return EqualTo;
}


void TestObject::PrintOn(ostream & strm) const
{
  strm << value;
}


///////////////////////////////////////////////////////////////////////////////

ThreadSafe::ThreadSafe()
  : PProcess("Equivalence", "ThreadSafe", 1, 0, AlphaCode, 1)
{
  threadCount = 0;
  totalObjects = 0;
  currentObjects = 0;
}


ThreadSafe::~ThreadSafe()
{
  unsorted.RemoveAll(TRUE);
  sorted.RemoveAll(TRUE);
  sparse.RemoveAll(TRUE);
}

void ThreadSafe::Usage()
{
  cout << "Usage: threadsafe {options} [number]" << endl
       << "-t (more t's for more detail) logging on" << endl
       << "-o output file for logging" << endl
       << "-1 (or --test1) carry out test 1" << endl
       << "-2 (or --test2) carry out test 2" << endl
       << "-3 (or --test3) carry out test 3" << endl 
       << "The number field is optional, and specifies the number of threads for test 1" << endl
       << endl;
  return;
}



void ThreadSafe::Main()
{
  PArgList & args = GetArguments();

  args.Parse(
#if PTRACING
             "t-trace."       "-no-trace."
             "o-output:"      "-no-output."
#endif
             "1-test1."       "-no-test1."
             "2-test2."       "-no-test2."
             "3-test3."       "-no-test3.");

#if PTRACING
  PTrace::Initialise(args.GetOptionCount('t'),
                     args.HasOption('o') ? (const char *)args.GetOptionString('o') : NULL,
                     PTrace::Blocks | PTrace::Timestamp | PTrace::Thread | PTrace::FileAndLine);
#endif

  if (args.HasOption('1'))
    Test1(args);
  else if (args.HasOption('2'))
    Test2(args);
  else if (args.HasOption('3'))
    Test3(args);
  else
    Usage();
}


void ThreadSafe::Test1(PArgList & args)
{
  if (args.GetCount() > 0)
    threadCount = args[0].AsUnsigned();
  else
    threadCount = 99;

  cout << "Starting " << threadCount << " threads." << endl;

  for (PINDEX i = 0; i < threadCount; i++) {
    PTimeInterval duration = PRandom::Number()%540000 + 60000;
    cout << setw(4) << (i + 1) << '=' << duration;
    if (i%5 == 4)
      cout << '\n';
    PThread::Create(PCREATE_NOTIFIER(Test1Thread), (INT)duration.GetMilliSeconds());
  }
  cout << endl;

  startTick = PTimer::Tick();
  while (threadCount > 0) {
    Test1Output();
    Sleep(5000);
  }

  Test1OutputEnd();
  sorted.RemoveAll();
  unsorted.RemoveAll();
  sparse.RemoveAll();
  Test1OutputEnd();
}


void ThreadSafe::Test1Output()
{
  PSafePtr<TestObject> ptr;
  sorted.DeleteObjectsToBeRemoved();
  unsorted.DeleteObjectsToBeRemoved();
  sparse.DeleteObjectsToBeRemoved();

  cout << setprecision(0) << setw(5) << (PTimer::Tick() - startTick)
       << " Threads=" << threadCount
       << ", Unsorted=" << unsorted.GetSize()
       << ", Sorted=" << sorted.GetSize()
       << ", Dictionary=" << sparse.GetSize()
       << ", Objects:";


  mutexObjects.Wait();
  cout << currentObjects << '/' << totalObjects << endl;
  mutexObjects.Signal();
  
}

void ThreadSafe::Test1OutputEnd()
{
  PSafePtr<TestObject> ptr;

  Test1Output();

  cout << setprecision(0) << setw(5) << (PTimer::Tick() - startTick) << " Unsorted:" << endl;
  for (ptr = unsorted.GetAt(0, PSafeReference); ptr != NULL; ++ptr) {
    cout << *ptr << endl;
  }
    
  cout << setprecision(0) << setw(5) << (PTimer::Tick() - startTick) << " Sorted:" << endl;
  for (ptr = sorted.GetAt(0, PSafeReference); ptr != NULL; ++ptr) {
    cout << *ptr << endl;
  }

  cout << setprecision(0) << setw(5) << (PTimer::Tick() - startTick) << " Sparse:" << endl;
  for (ptr = sparse.GetAt(0, PSafeReference); ptr != NULL; ++ptr) {
    cout << *ptr << endl;
  }


}



void ThreadSafe::Test1Thread(PThread &, INT duration)
{
  PRandom random;
  PSafePtr<TestObject> ptr;

  PTimer timeout = duration;

  while (timeout.IsRunning()) {
    switch (random%17) {
      case 0 :
        if (random%(unsorted.GetSize()+1) == 0)
          unsorted.Append(new TestObject(*this, random));
        break;

      case 1 :
        if (random%(sorted.GetSize()+1) == 0) 
          sorted.Append(new TestObject(*this, random));
        break;

      case 2 :
        sparse.SetAt(random%20, new TestObject(*this, random));
        break;

      case 3 :
        for (ptr = unsorted.GetAt(0, PSafeReference); ptr != NULL; ++ptr) {
          if (random%50 == 0)
            unsorted.Remove(ptr);
        }
        break;

      case 4 :
        for (ptr = sorted.GetAt(0, PSafeReference); ptr != NULL; ++ptr) {
          if (random%50 == 0)
            sorted.Remove(ptr);
        }
        break;

      case 5 :
        sparse.RemoveAt(random%20);
        break;

      case 6 :
        for (ptr = unsorted; ptr != NULL; ++ptr)
          Sleep(random%50);
        break;

      case 7 :
        for (ptr = sorted; ptr != NULL; ++ptr)
          Sleep(random%50);
        break;

      case 8 :
        for (ptr = sparse; ptr != NULL; ++ptr)
          Sleep(random%50);
        break;

      case 9 :
        for (ptr = unsorted.GetAt(0, PSafeReadOnly); ptr != NULL; ++ptr)
          Sleep(random%50);
        break;

      case 10 :
        for (ptr = sorted.GetAt(0, PSafeReadOnly); ptr != NULL; ++ptr)
          Sleep(random%50);
        break;

      case 11 :
        for (ptr = sparse.GetAt(0, PSafeReadOnly); ptr != NULL; ++ptr)
          Sleep(random%50);
        break;

      case 12 :
        for (ptr = unsorted.GetAt(0, PSafeReference); ptr != NULL; ++ptr)
          Sleep(random%50);
        break;

      case 13 :
        for (ptr = sorted.GetAt(0, PSafeReference); ptr != NULL; ++ptr)
          Sleep(random%50);
        break;

      case 14 :
        for (ptr = sparse.GetAt(0, PSafeReference); ptr != NULL; ++ptr)
          Sleep(random%50);
        break;


      case 15 :
        if ( unsorted.GetSize() > 0 ) {
          PSafePtr<TestObject> ptr2 = unsorted.GetAt(unsorted.GetSize() - 1, PSafeReadOnly);
          if ( ptr2 != NULL )
            ptr2 = unsorted.FindWithLock(*ptr2, PSafeReadOnly);
        }
        break;

      case 16 :
        if ( sorted.GetSize() > 0 ) {
          PSafePtr<TestObject> ptr2 = unsorted.GetAt(sorted.GetSize() - 1, PSafeReference);
          if ( ptr2 != NULL )
            ptr2 = sorted.FindWithLock(*ptr2, PSafeReference);
        }
        break;
    }
    Sleep(random%500);
  }

  threadCount--;
}


void ThreadSafe::Test2(PArgList &)
{
  sparse.SetAt(0, new TestObject(*this, 0));

  threadCount = 2;
  PThread::Create(PCREATE_NOTIFIER(Test2Thread1));
  PThread::Create(PCREATE_NOTIFIER(Test2Thread2));

  while (threadCount > 0)
    Sleep(1000);
}


void ThreadSafe::Test2Thread1(PThread &, INT)
{
  cout << "Thread 1 before read only lock" << endl;
  PSafePtr<TestObject> ptr = sparse.FindWithLock(0, PSafeReadOnly);

  cout << "Thread 1 after read only lock, pausing ..." << endl;
  Sleep(3000);

  cout << "Thread 1 before read write lock" << endl;
  ptr = sparse.FindWithLock(0, PSafeReadWrite);

  cout << "Thread 1 after read write lock, exiting" << endl;

  threadCount--;
}


void ThreadSafe::Test2Thread2(PThread &, INT)
{
  Sleep(1000);

  cout << "Thread 2 before read write lock" << endl;
  PSafePtr<TestObject> ptr = sparse.FindWithLock(0, PSafeReadWrite);

  cout << "Thread 2 after read write lock, exiting" << endl;
  threadCount--;
}


void ThreadSafe::Test3(PArgList &)
{
  for (PINDEX i = 0; i < 10; i++)
    unsorted.Append(new TestObject(*this, i));

  threadCount = 2;
  PThread::Create(PCREATE_NOTIFIER(Test3Thread1));
  PThread::Create(PCREATE_NOTIFIER(Test3Thread2));

  while (threadCount > 0)
    Sleep(1000);
}


void ThreadSafe::Test3Thread1(PThread &, INT)
{
  {
    cout << "Thread 1 before read only lock" << endl;
    PSafePtr<TestObject> ptr = unsorted.GetAt(2, PSafeReadOnly);

    cout << "Thread 1 after read only lock, pausing ..." << endl;
    Sleep(2000);

    cout << "Thread 1 before read write lock" << endl;
    ptr.SetSafetyMode(PSafeReadWrite);

    cout << "Thread 1 after read write lock, before ptr going out of scope" << endl;
  }
  cout << "Thread 1 after ptr out of scope, exiting" << endl;

  threadCount--;
}


void ThreadSafe::Test3Thread2(PThread &, INT)
{
  Sleep(1000);

  cout << "Thread 2 before enumeration" << endl;
  PSafePtr<TestObject> ptr = unsorted.GetAt(0, PSafeReadOnly);
  while (ptr != NULL) {
    if (ptr->value == 2) {
      cout << "Thread 2 before read write lock" << endl;
      ptr->LockReadWrite();
      cout << "Thread 2 after read write lock" << endl;

      Sleep(2000);

      cout << "Thread 2 before read write unlock" << endl;
      ptr->UnlockReadWrite();
      cout << "Thread 2 after read write unlock" << endl;
    }
    ptr++;
  }

  cout << "Thread 2 after enumeration, exiting" << endl;
  threadCount--;
}


// End of File ///////////////////////////////////////////////////////////////
