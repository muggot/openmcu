/*
 * main.cxx
 *
 * PWLib application source file for StringTest
 *
 * Main program entry point.
 *
 * Copyright (c) 2006 Indranet Technologies Ltd.
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
 * The Initial Developer of the Original Code is Derek J Smithies
 *
 * Contributor(s): ______________________________________.
 *
 * $Log: main.cxx,v $
 * Revision 1.1  2006/06/26 02:16:09  dereksmithies
 * Initial release of a program to test the speed and performance of the PString class.
 *
 *
 */
#include  <ptlib.h>
#include <ptlib/pprocess.h>

#include <string>

#include "main.h"
#include "version.h"


PCREATE_PROCESS(StringTest);




StringTest::StringTest()
  : PProcess("Derek Smithies code factory", "StringTest", MAJOR_VERSION, MINOR_VERSION, BUILD_TYPE, BUILD_NUMBER)
{
}


void StringTest::Main()
{
  PArgList & args = GetArguments();

  args.Parse(
             "h-help."  
	     "v-version."

	     "a-assign."
	     "c-copy."
	     "e-everything."
	     "j-join."
	     "l-length."

	     "i-iterations:"
	     "s-standard."
#if PTRACING  
             "o-output:"
             "t-trace." 
#endif
	     );

#if PTRACING
  PTrace::Initialise(args.GetOptionCount('t'),
                     args.HasOption('o') ? (const char *)args.GetOptionString('o') : NULL,
         PTrace::Blocks | PTrace::Timestamp | PTrace::Thread | PTrace::FileAndLine);
#endif

  if (args.HasOption('h')) {
    PError << "Available options are: " << endl
	   << endl
	   << "-a or --assign         : construct a  strng with an assigned value\n"
	   << "-c or --copy           : copy one string to another\n"
	   << "-e or --everything     : everything together, in such a way to avoid compiler optimisations\n"
	   << "-j or --join           : test the joining of two strings \n"
	   << "-l or --length         : calculate the length of a string\n"
	   << "-i or --interations    : the number of (x1E6) iterations to repeat the test\n"       
	   << "-h or --help           : print this help message.\n"
	   << "-v or --version        : report program version\n"
	   << "-s or --standard       : test std::string, and not the pwlib string\n"
#if PTRACING
	   << "-o or --output file    : file name for output of log messages\n"       
	   << "-t or --trace          : degree of verbosity in error log (more times for more detail)\n"     
#endif
	   << endl
	   << " e.g. stringtest -i 100 -e  " << endl << endl;
    return;
  }

  if (args.HasOption('v')) {
        cout << "Product Name: " << GetName() << endl
         << "Manufacturer: " << GetManufacturer() << endl
         << "Version     : " << GetVersion(TRUE) << endl
         << "System      : " << GetOSName() << '-'
         << GetOSHardware() << ' '
         << GetOSVersion() << endl;
    return;
  }

  PINDEX iterations = 100;
  if (args.HasOption('i'))
    iterations = args.GetOptionString('i').AsInteger(10);
  iterations = PMAX(1, PMIN(iterations, 100000));
  PError << "Will run the test for 1 million x " << iterations << " loops" << endl;

  BOOL testPwlib = ! args.HasOption('s');

  if (args.HasOption('s'))
    cerr << "examine std::string" << endl;
  else
    cerr << "examine pwlib string" << endl;

  TestToDo test = None;
  if (args.HasOption('a')) {
    cerr << "Test Assigning" << endl;
    test = Assign;
  }

  if (args.HasOption('c')) {
    cerr << "Test copying the middle of one string to another" << endl;
    test = Copy;
  }

  if (args.HasOption('e')) {
    cerr << "Every possible combination the author could think of" << endl;
    test = Everything;
  }

  if (args.HasOption('j')) {
    cerr << "test joining two strings together " << endl;
    test = Join;
  }

  if (args.HasOption('l')) {
    cerr << "test determining the length of a string " << endl;
    test = Length;
  }


  startTime = PTime();

  for (PINDEX i = 0; i < iterations; i++)
    switch(test) {

    case Assign:;
      if (testPwlib) 
	TestPwlibAssign();
      else
	TestStandardAssign();
      break;

    case Copy :;
      if (testPwlib) 
	TestPwlibCopy();
      else
	TestStandardCopy();
      break;

    case Everything: ;
      if (testPwlib) 
	TestPwlibEverything();
      else
	TestStandardEverything();
      break;

    case Join: ;
      if (testPwlib) 
	TestPwlibJoin();
      else
	TestStandardJoin();
      break;

    case Length: ;
      if (testPwlib) 
	TestPwlibLength();
      else
	TestStandardLength();
      break;

    case None: ;
      if (testPwlib)
	TestPwlibNone();
      else
	TestStandardNone();
    }

  PTime endTime;
  cerr << "Elapsed time is " << (endTime - startTime) << endl;
}
   
void StringTest::TestPwlibAssign()
{
  for (PINDEX i = 0; i < 1000000; i++) {
    PString src("abcdefg");
    PString dst;
    dst = src;
  }
}
    
void StringTest::TestStandardAssign()
{
  for (PINDEX i = 0; i < 1000000; i++) {
    std::string src("abcdefg");
    std::string dst;
    dst = src;
  }
}


void StringTest::TestPwlibCopy()
{
  PString src("abcdefghijkl");
  PString dsta;
  PString dstb;
  for (PINDEX i = 0; i < 1000000; i++) {
    switch(i % 3) {
    case 0:
      dsta = src.Mid(3, 6);
      break;
    case 1:
      dstb = dsta.Mid(1, 1);
      break;
    case 2: 
      dsta = src + dsta;
      break;
    }
  }
}
    
void StringTest::TestStandardCopy()
{
  std::string src("abcdefghijkl");
  std::string dsta;
  std::string dstb;
  for (PINDEX i = 0; i < 1000000; i++) {
    switch(i % 3) {
    case 0:
      dsta = src.substr(3,6);
      break;
    case 1:
      dstb = dsta.substr(1,1);
      break;
    case 2: 
      dsta = src + dsta;
      break;
    }
  }
}

  
void StringTest::TestPwlibEverything()
{
  PString answer;
  PString src("abcdefghijkl");
  PString dst;
  for (PINDEX i = 0; i < 1000000; i++) {
    dst = src + src + src + src + src;
    src = dst.Mid(1, 5);

    dst = src + PString(i);

    dst = dst + src.Mid(1, 2) + PString(src.GetLength());

    // src = dst;
    answer =  dst.Mid(dst.GetLength() - 1, 1);
  }    
  cout << answer << endl;
}


void StringTest::TestStandardEverything()
{
  stringstream temp;
  std::string answer;
  std::string src("abcdefghijkl");
  std::string dst;
  for (PINDEX i = 0; i < 1000000; i++) {
    dst = src + src + src + src + src;
    src = dst.substr(1, 5);

    temp << i; 
    dst = src + temp.str();
    temp.str("");
    
    temp << src.length();
    dst = dst + src.substr(1,2) + temp.str();
    temp.str("");


    answer = dst.substr(dst.length() - 1, 1);
  }    
  cout << answer << endl;
}



void StringTest::TestPwlibJoin()
{
  for (PINDEX i = 0; i < 1000000; i++) {
    PString src("abcdefg");
    PString dst("123");
    dst = dst + src;;
  }
}
    
void StringTest::TestStandardJoin()
{
  for (PINDEX i = 0; i < 1000000; i++) {
    std::string src("abcdefg");
    std::string dst("123");
    dst = dst + src;
  }
}

void StringTest::TestPwlibLength()
{
  for (PINDEX i = 0; i < 1000000; i++) {
    PString src("abcdefg");
    int i = src.GetLength();
    i++;
  }
}
    
void StringTest::TestStandardLength()
{
  for (PINDEX i = 0; i < 1000000; i++) {
    std::string src("abcdefg");
    int i = src.length();
    i++;
  }
}


void StringTest::TestPwlibNone()
{
  for (PINDEX i = 0; i < 1000000; i++) {
    PString src;    
  }
}
    
void StringTest::TestStandardNone()
{
  for (PINDEX i = 0; i < 1000000; i++) {
    std::string src;
  }
}  


// End of File ///////////////////////////////////////////////////////////////
