/*
 * timing.cxx
 *
 * Sample program to test PWLib PAdaptiveDelay.
 *
 * Portable Windows Library
 *
 * Copyright (c) 2003 Roger Hardiman
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
 * The Initial Developer of the Original Code is Roger Hardiman
 *
 * $Log: timing.cxx,v $
 * Revision 1.5  2006/06/21 03:28:43  csoutheren
 * Various cleanups thanks for Frederic Heem
 *
 * Revision 1.4  2005/11/15 22:27:29  dereksmithies
 * Modify output format so it better shows the delay characteristics of PAdaptiveDelay
 *
 * Revision 1.3  2004/05/18 12:47:38  rjongbloed
 * Added time, time interval and timer test/demonstration code
 *
 * Revision 1.2  2004/05/18 12:25:18  rjongbloed
 * Added time, time interval and timer test/demonstration code
 *
 * Revision 1.1  2003/02/19 14:10:11  rogerh
 * Add a program to test PAdaptiveDelay
 *
 *
 *
 */

#include <ptlib.h>
#include <ptlib/pprocess.h>
#include <ptclib/delaychan.h>

/*
 * The main program class
 */
class TimingTest : public PProcess
{
  PCLASSINFO(TimingTest, PProcess)
  public:
    void Main();
};

PCREATE_PROCESS(TimingTest);

#define TEST_TIME(t) cout << t << " => " << PTime(t) << '\n'

// The main program
void TimingTest::Main()
{
  cout << "Timing Test Program\n" << endl;

  PTime now;
  cout << "Time is now " << now.AsString("h:m:s.u d/M/y") << "\n"
          "Time is now " << now.AsString("yyyy/MM/dd h:m:s.uuuu") << "\n"
          "Time is now " << now.AsString("MMM/d/yyyyy w h:m:sa") << "\n"
          "Time is now " << now.AsString("wwww d/M/yyy h:m:s.uu") << "\n"
          "Time is now " << now.AsString("www d/MMMM/yy h:m:s.uuu") << endl;

  cout << "\nTesting time string conversion" << endl;
  TEST_TIME("20010203T1234Z");
  TEST_TIME("20010203T1234");
  TEST_TIME("20010203T0034");
  TEST_TIME("20010203T10034");
  TEST_TIME("20010203T123456+1100");
  TEST_TIME("20010203T000056");
  TEST_TIME("20010203T123456");
  TEST_TIME("2001-02-03 T 12:34:56");
  TEST_TIME("5/03/1999 12:34:56");
  TEST_TIME("15/06/1999 12:34:56");
  TEST_TIME("15/06/01 12:34:56 PST");
  TEST_TIME("5/06/02 12:34:56");
  TEST_TIME("5/23/1999 12:34am");
  TEST_TIME("5/23/00 12:34am");
  TEST_TIME("1999/23/04 12:34:56");
  TEST_TIME("Mar 3, 1999 12:34pm");
  TEST_TIME("3 Jul 2004 12:34pm");
  TEST_TIME("12:34:56 5 December 1999");
  TEST_TIME("10 minutes ago");
  TEST_TIME("2 weeks");

  cout << "\nTesting time interval arithmetic" << endl;
  PTime then("1 month ago");
  PTimeInterval elapsed = now - then;
  cout << "Now=" << now << "\n"
          "Then=" << then << "\n"
          "Elapsed=" << elapsed << "\n"
          "Milliseconds=" << elapsed.GetMilliSeconds() << "\n"
          "Seconds=" << elapsed.GetSeconds() << "\n"
          "Minutes=" << elapsed.GetMinutes() << "\n"
          "Hours=" << elapsed.GetHours() << "\n"
          "Days=" << elapsed.GetDays() << endl;

  then += PTimeInterval(0,0,0,0,30);
  cout << "Then plus 30 days=" << then << endl;

  cout << "\nTesting timer resolution, reported as " << PTimer::Resolution() << "ms" << endl;
  time_t oldSec = time(NULL);   // Wait for second boundary
  while (oldSec == time(NULL))
    ;

  oldSec++;
  PTimeInterval newTick = PTimer::Tick();
  PTimeInterval oldTick = newTick;
  unsigned count = 0;

  while (oldSec == time(NULL)) {  // For one full second
    while (newTick == oldTick)
      newTick = PTimer::Tick();
    oldTick = newTick;
    count++;                      // Count the changes in tick
  } ;

  cout << "Actual resolution is " << 1000000/count << "us" << endl;

  oldTick = 123456;
  cout << "TimeInterval output: \"" << setw(15) << newTick << '"' << endl;
  cout << "TimeInterval output: \"" << setw(15) << oldTick << '"' << endl;
  int p;
  for (p = 3; p < 10; p++)
    cout << "TimeInterval output: " << p << " \""
           << setiosflags(ios::scientific)
           << setw(p) << setprecision(2) << oldTick
           << resetiosflags(ios::scientific) << '"' << endl;
  for (p = 3; p < 20; p++)
    cout << "TimeInterval output: " << p << " \""
           << setw(p) << setprecision(2) << oldTick << '"' << endl;

  cout << "\nTesting sleep function" << endl;
  PTime start_time1;
  PINDEX loop;
  for(loop = 0; loop < 10; loop++) {
    Sleep(10 * 1000);     //10 seconds, or 10 * 1000ms
    PTime now1;
    cout << now1-start_time1 << endl;
  }
  PTime end_time1;

  cout << "The first loop took "<< end_time1-start_time1 << " milliseconds." << endl;


  cout << "\nTesting adaptive delay function" << endl;
  PAdaptiveDelay delay;

  PTime start_time2;
  cout << "Start at " << start_time2.AsString("hh.mm:ss.uuu") << endl;
  for(loop = 0; loop < 10; loop++) {
    delay.Delay(150);
    PTime now2;
    cout << "#" << setw(2) << (loop + 1) <<" ";
    cout << "After " << setw(4) << ((loop + 1)* 150) << "ms, time is " << now2.AsString("hh.mm:ss.uuu");
    PTimeInterval gap = now2-start_time2;
    cout << "  Elapsed time since start is " << setfill('0') << setw(2) << gap.GetSeconds() << ":" << setw(3) << (gap.GetMilliSeconds() % 1000) << endl;
  }
  PTime end_time2;

  cout << "The second loop took "<< end_time2-start_time2 << " milliseconds." << endl;
}

