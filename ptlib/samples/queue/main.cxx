/*
 * main.cxx
 *
 * Queue - a tester of the PQueueChannel class in pwlib
 *
 * Copyright (c) 2006 Derek J Smithies
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
 * The Original Code is Jester
 *
 * The Initial Developer of the Original Code is Derek J Smithies
 *
 * Contributor(s): ______________________________________.
 *
 * $Log: main.cxx,v $
 * Revision 1.1  2006/12/07 08:46:17  dereksmithies
 * Initial cut of code to test the operation of PQueueChannel class.
 *
 *
 */

#include <ptlib.h>

#include "main.h"
#include "version.h"


#define new PNEW


PCREATE_PROCESS(QueueProcess);

///////////////////////////////////////////////////////////////
   
/////////////////////////////////////////////////////////////////////////////

QueueProcess::QueueProcess()
  : PProcess("Derek Smithies Code Factory", "Queue",
             1, 1, ReleaseCode, 0)
{
}


void QueueProcess::Main()
{
  // Get and parse all of the command line arguments.
  PArgList & args = GetArguments();
  args.Parse(
             "h-help."
	     "d-durationread:"
	     "D-durationwrite:"
	     "s-sizeread:"
	     "S-sizewrite:"
	     "i-iterations:"
#if PTRACING
             "o-output:"
             "t-trace."
#endif
	     "v-version."
          , FALSE);


  if (args.HasOption('h') ) {
      cout << "Usage : " << GetName() << " [options] \n"
	  
	  "General options:\n"
	  "  -d --durationread    : interval, in ms between reads   (30ms)\n"
	  "  -D --durationwrite   : interval, in ms between writes  (20ms)\n"
	  "  -s --sizeread        : number of bytes read at each read iteration (480)\n"
	  "  -S --sizewrite       : number of bytes written at each write iteration (320)\n"
	  "  -i --iterations #    : number of iteration of the write loop (100) \n"
#if PTRACING
	  "  -t --trace           : Enable trace, use multiple times for more detail.\n"
	  "  -o --output          : File for trace output, default is stderr.\n"
#endif

	  "  -h --help            : This help message.\n"
	  "  -v --version         : report version and program info.\n"
	  "\n"
	  "\n";
      return;
  }

  if (args.HasOption('v')) {
      cout << GetName()  << endl
	   << " Version " << GetVersion(TRUE) << endl
	   << " by " << GetManufacturer() << endl
	   << " on " << GetOSClass() << ' ' << GetOSName() << endl
	   << " (" << GetOSVersion() << '-' << GetOSHardware() << ")\n\n";
      return;
  }

#if PTRACING
  PTrace::Initialise(args.GetOptionCount('t'),
                     args.HasOption('o') ? (const char *)args.GetOptionString('o') : NULL,
                     PTrace::Timestamp|PTrace::Thread|PTrace::FileAndLine);
#endif

  iterations = 100;
  writeDelay  = 20;
  readDelay   = 30;
  writeSize   = 320;
  readSize    = 480;

  if (args.HasOption('i'))
      iterations = args.GetOptionString('i').AsInteger();
  if (args.HasOption('d'))
      writeDelay = args.GetOptionString('d').AsInteger();
  if (args.HasOption('D'))
      readDelay = args.GetOptionString('D').AsInteger();
  if (args.HasOption('s'))
      writeSize = args.GetOptionString('s').AsInteger();
  if (args.HasOption('S'))
      readSize = args.GetOptionString('S').AsInteger();

  PStringStream values;
  values << " processs " << iterations << " iterations of the write loop" << endl;
  values << " Read  " << readSize  << " bytes per loop,  delay " << readDelay  << " ms" << endl;
  values << " Write " << writeSize << " bytes per loop,  delay " << writeDelay << " ms" << endl;
  cout << values;
  PTRACE(3, values);

  queue.Open(10000);

  PThread * writer = PThread::Create(PCREATE_NOTIFIER(GenerateBlockData), 0,
				     PThread::NoAutoDeleteThread,
				     PThread::NormalPriority,
				     "generate");

  PThread * reader = PThread::Create(PCREATE_NOTIFIER(ConsumeBlockData), 0,
				     PThread::NoAutoDeleteThread,
				     PThread::NormalPriority,
				     "consume");



  writer->WaitForTermination();

  reader->WaitForTermination();

  delete writer;
  delete reader;
  
}


void QueueProcess::GenerateBlockData(PThread &, INT )
{
    PAdaptiveDelay delay;
    BYTE buffer[writeSize];
    PINDEX count = 0;

    for (PINDEX i = 0; i < iterations; i++) {
	count++;
	delay.Delay(writeDelay);
	queue.Write(buffer, writeSize);
	PTRACE(3, "Write " << writeSize << " bytes to the queue     ("
		   << queue.GetLength() << " bytes)"
		   << "     " << (count * writeSize));
    }
    PTRACE(3, "End of generate data blocks ");
    queue.Close();
}


void QueueProcess::ConsumeBlockData(PThread &, INT)
{
    PAdaptiveDelay delay;
    BYTE buffer[readSize];
    PINDEX count = 0;
    PINDEX readCount = 0;
    while (queue.GetLength() < (readSize * 2)) {
	PTRACE(3, "Wait for queue to grow, is only " << queue.GetLength() << " bytes).");
	PThread::Sleep(10);
    }
    delay.Restart();

    while (queue.IsOpen()) {
	count++;
	delay.Delay(readDelay);      
	if (!queue.Read(buffer, readSize)) {
	    PTRACE(2, "Failed in read from the queue. Read only " 
		   << queue.GetLastReadCount() << " bytes.");
	} else {
	    readCount += queue.GetLastReadCount();
	    PTRACE(3, "Read " << queue.GetLastReadCount() << " bytes from the queue    ("
		   << queue.GetLength() << " bytes)"
		   << "               " << (count * readSize)
		   << " (" << readCount << ")");
	}
    }
    
  PTRACE(3, "End of read data blocks from the queue");
}


// End of File ///////////////////////////////////////////////////////////////
