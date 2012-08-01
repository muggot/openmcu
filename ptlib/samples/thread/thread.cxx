/*
 * thread.cxx
 *
 * Sample program to test PWLib threads.
 *
 * Portable Windows Library
 *
 * Copyright (c) 2001,2002 Roger Hardiman
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
 * $Log: thread.cxx,v $
 * Revision 1.7  2006/06/21 03:28:43  csoutheren
 * Various cleanups thanks for Frederic Heem
 *
 * Revision 1.6  2003/01/07 10:04:13  rogerh
 * Revert to 2 seconds per phase
 *
 * Revision 1.5  2002/11/04 22:46:23  rogerh
 * Implement a Stop() method to make the threads terminate.
 *
 * Revision 1.4  2002/11/04 18:11:22  rogerh
 * Terminate the threads prior to deletion.
 *
 * Revision 1.3  2002/11/04 16:24:21  rogerh
 * Delete the threads, fixing a memory leak.
 *
 * Revision 1.2  2001/09/27 08:22:48  rogerh
 * Doing a flush on cout does not work on Mac OS X. So you do not see any
 * results until the program stops. So replace the printing of the numbers with
 * good old printf and fflush.
 *
 * Revision 1.1  2001/09/21 09:18:28  rogerh
 * Add a thread test program which demonstrates thread, suspend and resume.
 *
 *
 */

/*
 * This sample program tests threads is PWLib. It creates two threads,
 * one which display the number '1' and one which displays the number '2'.
 * It also demonstrates starting a thread with Resume(), using
 * Suspend() and Resume() to suspend a running thread and two different
 * ways to make a thread terminate.
 */

#include <ptlib.h>
#include <ptlib/pprocess.h>

/*
 * Thread #1 displays the number 1 every 10ms.
 * When it is created, Main() starts executing immediatly.
 * The thread is terminated by calling Stop() which uses a PSyncPoint with a
 * 10ms timeout.
 */
class MyThread1 : public PThread
{
  PCLASSINFO(MyThread1, PThread);
  public:
    MyThread1() : PThread(1000,NoAutoDeleteThread)
    {
      Resume(); // start running this thread when it is created.
    }

    void Main() {
      while (!shutdown.Wait(10)) { // 10ms delay
        printf("1 ");
        fflush(stdout);
	Sleep(10);
      }
    }

    void Stop() {
      // signal the shutdown PSyncPoint. On the next iteration, the thread's
      // Main() function will exit cleanly.
      shutdown.Signal();
    }

    protected:
      PSyncPoint shutdown;
};


/*
 * Thread #2 displays the number 2 every 10 ms.
 * This thread will not start automatically. We must call
 * Resume() after creating the thread.
 * The thread is terminated by calling Stop() which sets a local variable.
 */
class MyThread2 : public PThread
{
  PCLASSINFO(MyThread2, PThread);
  public:
    MyThread2() : PThread(1000,NoAutoDeleteThread) {
      exitFlag = FALSE;
    }

    void Main() {
      while (1) {
        // Check if we need to exit
        exitMutex.Wait();
        if (exitFlag == TRUE) {
          exitMutex.Signal();
          break;
        }
        exitMutex.Signal();

        // Display the number 2, then sleep for a short time
        printf("2 "); fflush(stdout);
	Sleep(10); // sleep 10ms
      }
    }

    void Stop() {
      // set the exit flag. On the next iteration, the thread's
      // Main() function will exit cleanly.
      exitMutex.Wait();
      exitFlag = TRUE;
      exitMutex.Signal();
    }

    protected:
      PMutex exitMutex;
      BOOL exitFlag;
};


/*
 * The main program class
 */
class ThreadTest : public PProcess
{
  PCLASSINFO(ThreadTest, PProcess)
  public:
    void Main();
};

PCREATE_PROCESS(ThreadTest);

// The main program
void ThreadTest::Main()
{
  cout << "Thread Test Program" << endl;
  cout << "This program will display the following:" << endl;
  cout << "             2 seconds of 1 1 1 1 1..." << endl;
  cout << " followed by 2 seconds of 1 2 1 2 1 2 1 2 1 2..." << endl;
  cout << " followed by 2 seconds of 2 2 2 2 2..." << endl;
  cout << " followed by 2 seconds of 1 2 1 2 1 2 1 2 1 2..." << endl;
  cout << endl;
  cout << "It tests thread creation, suspend and resume functions." << endl;
  cout << endl;

  // Create the threads
  MyThread1 * mythread1;
  MyThread2 * mythread2;

  mythread1 = new MyThread1();
  mythread2 = new MyThread2();


  // Thread 1 should now be running, as there is a Resume() function
  // in the thread constructor.
  // Thread 2 should be suspended.
  // Sleep for three seconds. Only thread 1 will be running.
  // Display will show "1 1 1 1 1 1 1..."
  sleep(2);


  // Start the second thread.
  // Both threads should be running
  // Sleep for 3 seconds, allowing the threads to run.
  // Display will show "1 2 1 2 1 2 1 2 1 2..."
  mythread2->Resume();
  sleep(2);


  // Suspend thread 1.
  // Sleep for 3 seconds. Only thread 2 should be running.
  // Display will show "2 2 2 2 2 2 2..."
  mythread1->Suspend();
  sleep(2);


  // Resume thread 1.
  // Sleep for 3 seconds. Both threads should be running.
  // Display will show "1 2 1 2 1 2 1 2 1 2..."
  mythread1->Resume();
  sleep(2);


  // Clean up
  mythread1->Stop();
  mythread1->WaitForTermination();
  cout << "Thread 1 terminated" << endl;

  mythread2->Stop();
  mythread2->WaitForTermination();
  cout << "Thread 2 terminated" << endl;

  delete mythread1;
  delete mythread2;

}

