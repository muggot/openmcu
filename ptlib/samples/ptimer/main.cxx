/*
 * main.cxx
 *
 * PWLib application source file for ptimer test program
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
 * The Initial Developer of the Original Code is Indranet Technologies Ltd.
 *
 * Contributor(s): ______________________________________.
 *
 * $Log: main.cxx,v $
 * Revision 1.5  2007/05/10 00:37:35  rjongbloed
 * Fixed compiler warnings.
 *
 * Revision 1.4  2007/05/01 03:15:26  dereksmithies
 * Add a second test, to test the repeated initialisation of some PTimer instances.
 *
 * Revision 1.3  2006/07/22 07:27:26  rjongbloed
 * Fixed various compilation issues
 *
 * Revision 1.2  2006/05/24 02:28:18  dereksmithies
 * add separate thread to get the timer to start.
 * Add option to check if the timer has started.
 * Fix use of the parameters.
 *
 * Revision 1.1  2006/05/23 04:36:49  dereksmithies
 * Initial release of a test program to examine the operation of PTimer.
 *
 *
 *
 */

#include "precompile.h"
#include "main.h"
#include "version.h"


PCREATE_PROCESS(PTimerTest);

#include  <ptclib/dtmf.h>
#include  <ptclib/random.h>

void PTimerTest::TooSoon(PTimeInterval & elapsed)
{
  cerr << "Timer retuned too soon, interval was " << elapsed << endl;
}

PTimerTest::PTimerTest()
  : PProcess("Derek Smithies code factory", "ptimer test", MAJOR_VERSION, MINOR_VERSION, BUILD_TYPE, BUILD_NUMBER)
{
}

void PTimerTest::Main()
{
  PArgList & args = GetArguments();

  args.Parse(
             "h-help."        
	     "c-check."
             "d-delay:"       
	     "i-interval:"
	     "s-reset."
#if PTRACING
             "o-output:"      
             "t-trace."       
#endif	     
             "v-version."
  );

#if PTRACING
  PTrace::Initialise(args.GetOptionCount('t'),
                     args.HasOption('o') ? (const char *)args.GetOptionString('o') : NULL,
         PTrace::Blocks | PTrace::Timestamp | PTrace::Thread | PTrace::FileAndLine);
#endif

  if (args.HasOption('v')) {
    cout << "Product Name: " << GetName() << endl
         << "Manufacturer: " << GetManufacturer() << endl
         << "Version     : " << GetVersion(TRUE) << endl
         << "System      : " << GetOSName() << '-'
         << GetOSHardware() << ' '
         << GetOSVersion() << endl;
    return;
  }

  if (args.HasOption('h')) {
    PError << "Available options are: " << endl         
           << "-h  or --help         print this help" << endl
	   << "-c  or --check        check the timer is running when it should be running" << endl
           << "-v  or --version      print version info" << endl
           << "-d  or --delay ##     duration (ms) the timer waits for" << endl
	   << "-i  or --interval ##  interval (ms) between timer tests" << endl
	   << "-s  or --second       A second test, which repeatedly resets two internal timers." << endl
#if PTRACING
           << "o-output              output file name for trace" << endl
           << "t-trace.              trace level to use." << endl
#endif
           << endl
           << endl << endl;
    return;
  }

  if (args.HasOption('s')) {
      RunSecondTest();
      return;
  }

  checkTimer = args.HasOption('c');

  delay = 200;
  if (args.HasOption('d'))
    delay = args.GetOptionString('d').AsInteger();

  delay = PMIN(1000000, PMAX(0, delay));
  cout << "Created ptimer will wait for " << delay 
       << " milliseconds before ending" << endl;
 
  interval = 50;
  if (args.HasOption('i'))
    interval = args.GetOptionString('i').AsInteger();

  interval = PMIN(1000000, PMAX(0, interval));
  cout << "Separate each instance of PTimer by " << interval 
       << " milliseconds " << endl;
 


  UserInterfaceThread ui;
  ui.Resume();
  ui.WaitForTermination();
}
/////////////////////////////////////////////////////////////////////////////
void PTimerTest::RunSecondTest()
{
    cerr << "Will run the second test, which goes forever (if pwlib works correctly)" << endl
	 << "or stops, on detecting an error" << endl
	 << " " << endl
	 << "This test runs two threads, which continually restart two timer instances " << endl
	 << " " << endl
	 <<"---There is no output, until an error is detected. All going well, you will have" << endl
	 << "to stop this program with Ctrl-C" << endl;

    firstTimer.SetNotifier(PCREATE_NOTIFIER(OnFirstTimerExpired));
    secondTimer.SetNotifier(PCREATE_NOTIFIER(OnSecondTimerExpired));

    PThread::Create(PCREATE_NOTIFIER(RestartFirstTimerMain), 30000,
				    PThread::NoAutoDeleteThread,
				    PThread::NormalPriority);

    PThread::Create(PCREATE_NOTIFIER(RestartSecondTimerMain), 30000,
				    PThread::NoAutoDeleteThread,
				    PThread::NormalPriority);


    PTime restartActive;
    PTimeInterval quietPeriod(4000);

    for (;;) {
	    if (restartActivity > 0) {
	        restartActive = PTime();
	        restartActivity.SetValue(0);
	    }
	    if ((restartActive + quietPeriod) < PTime()) {
	        cerr << "No activity for four seconds. Timers Locked up. PWlib Error" << endl;
	        exit(0);
	    }
	    PThread::Sleep(100);
    }
}

void PTimerTest::OnFirstTimerExpired(PTimer &, INT)
{
    cerr << "The first timer has expired " << endl;
}

void PTimerTest::OnSecondTimerExpired(PTimer &, INT)
{
    cerr << "The second timer has expired " << endl;
}

void PTimerTest::RestartFirstTimerMain(PThread &, INT)
{
    for (;;) {
	    firstTimer = PTimeInterval(1900);
	    restartActivity.SetValue(1);
	    PThread::Sleep(400);
    }
}

void PTimerTest::RestartSecondTimerMain(PThread &, INT)
{
    for (;;) {
	    secondTimer = PTimeInterval(2000);
	    restartActivity.SetValue(1);
	    PThread::Sleep(300);
    }
}

/////////////////////////////////////////////////////////////////////////////
MyTimer::MyTimer()
{
  exitFlag = NULL;
}

void MyTimer::StartRunning(PSyncPoint * _exitFlag, PINDEX delayMs)
{
  exitFlag = _exitFlag;

  Stop();
  
  SetInterval(delayMs);
  delayPeriod = PTimeInterval(delayMs);
  startTime = PTime();

  PThread * startIt = new TimerOnThread(*this);
  startIt->Resume();
}

void MyTimer::OnTimeout()
{
  PTimeInterval error(3); //provide a 3ms error in return time, to avoid
  //any ounding issue in timers.

  PTimeInterval elapsed = error + (PTime() - startTime);
  
  if (elapsed < delayPeriod)
    PTimerTest::Current().TooSoon(elapsed);    

  exitFlag->Signal();
};

/////////////////////////////////////////////////////////////////////////////

DelayThread::DelayThread(PINDEX _delay, BOOL _checkTimer)
  : PThread(10000, AutoDeleteThread), delay(_delay), checkTimer(_checkTimer)
{
  PTRACE(5, "Constructor for a auto deleted PTimer test thread");
}    

DelayThread::~DelayThread()
{
  PTRACE(5, "Destructor for a delay thread");
  //This thread must not have a PTRACE statement in the debugger, if it is an autodeleted thread.
  //If a PTRACE statement is here, the PTRACE will fail as the PThread::Current() returns empty.
}

void DelayThread::Main()  
{
  PTRACE(5, "DelayThread\t start now");
  localPTimer.StartRunning(&endMe, delay);

  if (checkTimer) {
    if (!localPTimer.IsRunning())
      cerr << "PTimer has been detected as finishing too soon" << endl;
  }

  endMe.Wait();
  PTRACE(5, "DelayThread\t all finished");
}

/////////////////////////////////////////////////////////////////////////////
TimerOnThread::TimerOnThread(PTimer & _timer)
  : PThread(10000, AutoDeleteThread), timer(_timer)
{
  PTRACE(5, "Constructor for a auto deleted Ptimer On thread.");
}    

void TimerOnThread::Main()  
{
  PTRACE(5, "DelayThread\t start now");
  timer.Resume();
}

///////////////////////////////////////////////////////////////////////////

void LauncherThread::Main()
{
  PINDEX delay      = PTimerTest::Current().Delay();
  PINDEX interval   = PTimerTest::Current().Interval();
  BOOL   checkTimer = PTimerTest::Current().CheckTimer();

  while (keepGoing) {
	PThread * thread = new DelayThread(delay, checkTimer);
	thread->Resume();
	PThread::Sleep(interval);
	iteration++;
      }

  return;
}

/////////////////////////////////////////////////////////////////////////////

void UserInterfaceThread::Main()
{
  PConsoleChannel console(PConsoleChannel::StandardInput);
  cout << endl;
  cout << "This program repeatedly create and destroys a PTimer" << endl;
  cout << "Warnings are printed if the PTimer runs too quick" << endl;
  cout<< "This program will end when the user enters \"q\"  " << endl;

  PStringStream help;
  help << "Press : " << endl
       << "         D      average interval between instances " << endl
       << "         H or ? help"                                << endl
       << "         R      report count of threads done"        << endl
       << "         T      time elapsed"                        << endl
       << "         X or Q exit "                               << endl;
 
  cout << endl << help;

  LauncherThread launch;
  launch.Resume();

  console.SetReadTimeout(P_MAX_INDEX);
  for (;;) {
    int ch = console.ReadChar();

    switch (tolower(ch)) {
    case 'd' :
      {
        int i = launch.GetIteration();
        if (i == 0) {
          cout << "Have not completed an iteration yet, so time per iteration is unavailable" << endl;
        } else {
          cout << "Average time per iteration is " << (launch.GetElapsedTime().GetMilliSeconds()/((double) i)) 
               << " milliseconds" << endl;
        }
        cout << "Command ? " << flush;
        break;
      }
    case 'r' :
      cout << "\nHave completed " << launch.GetIteration() << " iterations" << endl;
      cout << "Command ? " << flush;
      break;
    case 't' :
      cout << "\nElapsed time is " << launch.GetElapsedTime() << " (Hours:mins:seconds.millseconds)" << endl;
      cout << "Command ? " << flush;
      break;

    case 'x' :
    case 'q' :
      cout << "Exiting." << endl;
      launch.Terminate();
      launch.WaitForTermination();
      return;
      break;
    case '?' :
    case 'h' :
      cout << help << endl;
      cout << "Command ? " << flush;
    default:
      break;

    } // end switch
  } // end for
}


// End of File ///////////////////////////////////////////////////////////////
