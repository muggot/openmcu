/*
 * serial.cxx
 *
 * copyright 2005 Derek J Smithies
 *
 * Simple program to illustrate usage of the serial port.
 *
 * Get two computers. Connect com1 port of both computers by cross over serial cable.
 * run this program on both computers (without arguments).
 * type text in one, hit return.  See this text in the second computer.
 *
 *
 * $Log: serial.cxx,v $
 * Revision 1.6  2006/06/21 03:28:43  csoutheren
 * Various cleanups thanks for Frederic Heem
 *
 * Revision 1.5  2005/11/30 12:47:40  csoutheren
 * Removed tabs, reformatted some code, and changed tags for Doxygen
 *
 * Revision 1.4  2005/03/18 21:06:09  dereksmithies
 * Add help messages. Enable different flow control options.
 *
 * Revision 1.3  2005/03/14 07:33:51  dereksmithies
 * Fix console input handling.  Concatenate characters split by PSerialChannel reading.
 *
 * Revision 1.2  2005/03/12 06:44:22  dereksmithies
 * Fix typo in setting stopbits.  Program now reports all serial parameters that are used.
 *
 * Revision 1.1  2005/03/10 09:21:46  dereksmithies
 * Initial release of a program to illustrate the operation of the serial port.
 *
 *
 *
 *
 */

#include <ptlib.h>
#include <ptlib/pprocess.h>
#include <ptlib/serchan.h>
#include <ptlib/sockets.h>

class Serial : public PProcess
{
    PCLASSINFO(Serial, PProcess);
public:    
    Serial();

    void Main();
    
    BOOL Initialise(PConfigArgs & args);

    void HandleConsoleInput();

    void HandleSerialInput();

protected:
    PSerialChannel serial;

    PINDEX dataBits;

    PINDEX stopBits;

    PString flowControlString;

    PINDEX hardwarePort;

    PINDEX baud;
    
    PString parity;
    BOOL endNow;
};


class UserInterfaceThread : public PThread
{
    PCLASSINFO(UserInterfaceThread, PThread);
  public:
    UserInterfaceThread(Serial & _srl)
      : PThread(1000, NoAutoDeleteThread), srl(_srl) { Resume(); }

    void Main()
      { srl.HandleConsoleInput(); }

  protected:
    Serial & srl; 
};

class SerialInterfaceThread : public PThread
{
    PCLASSINFO(SerialInterfaceThread, PThread);
public:
    SerialInterfaceThread(Serial & _srl)
      : PThread(1000, NoAutoDeleteThread), srl(_srl) { Resume(); }

    void Main()
      { srl.HandleSerialInput(); }

  protected:
    Serial & srl; 
};
    

PCREATE_PROCESS(Serial)


Serial::Serial()
  : PProcess("PwLib Example Factory", "serial", 1, 0, ReleaseCode, 0)
{
    endNow = FALSE;
}


void Serial::Main()
{

 PConfigArgs args(GetArguments());

// Example command line is :
// serial --hardwareport 0 --baud 4800 --parity odd --stopbits 1 --databits 8 --flowcontrol XonXoff

  args.Parse(
#if PTRACING
             "t-trace."              "-no-trace."
             "o-output:"             "-no-output."
#endif
#ifdef PMEMORY_CHECK
             "-setallocationbreakpoint:"
#endif
             "-baud:"
             "-databits:"
             "-parity:"
             "-stopbits:"
             "-flowcontrol:"
             "-hardwareport:"
             "v-version."
             "h-help."
          , FALSE);

#if PMEMORY_CHECK
  if (args.HasOption("setallocationbreakpoint"))
    PMemoryHeap::SetAllocationBreakpoint(args.GetOptionString("setallocationbreakpoint").AsInteger());
#endif

  PStringStream progName;
  progName << "Product Name: " << GetName() << endl
           << "Manufacturer: " << GetManufacturer() << endl
           << "Version     : " << GetVersion(TRUE) << endl
           << "System      : " << GetOSName() << '-'
           << GetOSHardware() << ' '
           << GetOSVersion();
  cout << progName << endl;

  if (args.HasOption('v'))
    return;


#if PTRACING
  PTrace::Initialise(args.GetOptionCount('t'),
                     args.HasOption('o') ? (const char *)args.GetOptionString('o') : NULL,
         PTrace::Blocks | PTrace::Timestamp | PTrace::Thread | PTrace::FileAndLine);
#endif

  if (args.HasOption('h')) {
      cout << endl
#if PTRACING
           <<  "-t   --trace                     Debugging. Using more times for more detail" << endl
           <<  "-o   --output                    name of trace output file. If not specified, goes to stdout" << endl
#endif
#ifdef PMEMORY_CHECK
           <<  "     --setallocationbreakpoint   stop program on allocation of memory block number" << endl
#endif
           <<  "     --baud                      Set the data rate for serial comms" << endl
           <<  "     --databits                  Set the number of data bits (5, 6, 7, 8)" << endl
           <<  "     --parity                    Set parity, even, odd or none " << endl
           <<  "     --stopbits                  Set the number of stop bits (0, 1, 2) " << endl
           <<  "     --flowcontrol               Specifiy flow control, (none rtscts, xonxoff)" << endl
           <<  "     --hardwareport              Which serial port to use, 0, 1, 2..." << endl
           <<  "-v   --version                   Print version information and exit" << endl
           <<  "-h   --help                      Write this help out.                   " << endl
           << endl;
      return;
  }


  if (!Initialise(args)) {
      cout << "Failed to initialise the program with args of " << args << endl;
      PThread::Sleep(100);
      return;
  }

  UserInterfaceThread *ui = new UserInterfaceThread(*this);
  SerialInterfaceThread *si = new SerialInterfaceThread(*this);

  ui->WaitForTermination();

  serial.Close();

  si->WaitForTermination();

  delete ui;
  delete si;

  cout << endl << "End of program" << endl << endl;
}

void Serial::HandleSerialInput()
{
#define MAXM 1000
  char buffer[MAXM];
  PString str;
  BOOL found = FALSE;

  while(serial.IsOpen()) {
    memset(buffer, 0, MAXM);
    serial.Read(buffer, MAXM);

    if (endNow) {
	    PTRACE(3, "End of thread to handle serial input");
	    return;
	  }

    PINDEX len = serial.GetLastReadCount();
    if (len != 0) {
	    buffer[len] = 0;
	    PTRACE(1, "Read the string \"" << buffer << "\" from the serial port");
      str += PString(buffer);
	    if (str.Find("\n") != P_MAX_INDEX)
		    found = TRUE;
    }

    PINDEX err = serial.GetErrorCode();
    if ((err != PChannel::NoError) && (err != PChannel::Timeout)) {
      PTRACE(1, "get data from serial port, failed, error is " << serial.GetErrorText());
      cout << "get data from serial port, failed, error is " << serial.GetErrorText() << endl;
    }

    if (found) {
	    str.Replace("\n", "");	    
      PTRACE(1, "Read the message \"" << str << "\"");
      cout << "have read the message \"" << str << "\" from the serial port" << endl;
      str = "";
      found = FALSE;
    }
  }
}

BOOL Serial::Initialise(PConfigArgs & args)
{
  if (!args.HasOption("baud")) {
    baud = 4800;
	  cout << "Baud not specifed.          Using 4800" << endl;
  } else {
	  baud = args.GetOptionString("baud").AsInteger();
	  cout << "Baud specified.             Using " << baud << endl;
  }

  if (!args.HasOption("databits")) {
  	cout << "databits not specified.     Using 8" << endl;
	  dataBits = 1;
  } else {
	  dataBits = args.GetOptionString("databits").AsInteger();
	  cout << "databits specified.         Using " << dataBits << endl;
  }

  if (!args.HasOption("parity")) {
    cout << "parity not specified.       Using \"odd\"" << endl;
    parity = "odd";
  } else {
	  parity = args.GetOptionString("parity");
	  cout << "parity specified            Using \"" << parity << "\"" << endl;
  }

  if (!args.HasOption("stopbits")) {
	  cout << "stopbits not specified.     Using 1" << endl;
	  stopBits = 1;
  } else {
	  stopBits = args.GetOptionString("stopbits").AsInteger();
	  cout << "stopbits specified.         Using " << stopBits << endl;
  }

  PString flow;
  if (!args.HasOption("flowcontrol")) {
	  cout << "Flow control not specified. Flow control set to XonXoff" << endl;
	  flow = "XonXoff";
  } else {
	  flow = args.GetOptionString("flowcontrol");
	  cout << "Flow control is specified.  Flow control is set to " << flow << endl;
  }

  if ((flow *= "xonxoff") || (flow *= "rtscts") || (flow *= "none"))
	  flowControlString = flow;
  else {
	  cout << "Valid args to flowcontrol are \"XonXoff\" or \"RtsCts\" or \"none\"" << endl;
	  return FALSE;
  }
    
  if (!args.HasOption("hardwareport")) {
	  cout << "Hardware port is not set.   Using 0 - the first hardware port" << endl;
	  hardwarePort = 0;
  } else
	  hardwarePort = args.GetOptionString("hardwareport").AsInteger();
    
    PStringList names = serial.GetPortNames();
    PStringStream allNames;
    for(PINDEX i = 0; i < names.GetSize(); i++)
      allNames << names[i] << " ";
    PTRACE(1, "available serial ports are " << allNames);
    
    PString portName;
    if (hardwarePort >= names.GetSize()) {
      cout << "hardware port is too large, list is only " << names.GetSize() << " long" << endl;
	    return FALSE;
    }
    portName = names[hardwarePort];
    
    PSerialChannel::Parity pValue = PSerialChannel::DefaultParity;
    if (parity *= "none")
      pValue = PSerialChannel::NoParity;
    if (parity *= "even")
      pValue = PSerialChannel::EvenParity;
    if (parity *= "odd")
      pValue = PSerialChannel::OddParity;
    if (pValue == PSerialChannel::DefaultParity) {
      cout << "Parity value of " << parity << " could not be interpreted" << endl;
      return FALSE;
    }
    
    PSerialChannel::FlowControl flowControl = PSerialChannel::DefaultFlowControl;
    if (flowControlString *= "xonxoff"){
      flowControl = PSerialChannel::XonXoff;
      PTRACE(3, "Using xonxoff flow control");
    }

    if (flowControlString *= "rtscts") {
      flowControl = PSerialChannel::RtsCts;
      PTRACE(3, "Using rts cts flow conrol ");
    }

    if (flowControlString *= "none") {
      flowControl = PSerialChannel::NoFlowControl;
      PTRACE(3, "Not using any flow control of any sort");
    }
    
    if (!serial.Open(portName, baud, dataBits, pValue, stopBits, flowControl, flowControl)) {
      cout << "Failed to open serial port " << endl;
      cout << "Error code is " << serial.GetErrorText() << endl;
      cout << "Failed in attempt to open port  /dev/" << portName << endl;
      return FALSE;
    }
    
    
    return TRUE;
}

void Serial::HandleConsoleInput()
{
  PTRACE(2, "Serial\tUser interface thread started.");

  PStringStream help;
  help << "Select:\n";
  help << "  ?   : display this help\n";
  help << "  H   : display this help\n";
  help << "  X   : Exit program\n";
  help << "  Q   : Exit program\n";
  help << "      : anything else to send a message\n";

  PError << " " << endl << help << endl;

  for (;;) {

    // display the prompt
    PError << "Command ? " << flush;
    char oneLine[200];
    fgets(oneLine, 200, stdin);
    
    PString str(oneLine);    
    if (str.GetLength() < 1)
      continue;

    BOOL helped = FALSE;
    if (str.GetLength() == 2) {
      char ch = str.ToLower()[0];

      if ((ch == '?') || (ch == 'h')) {
        helped = TRUE;
        PError << help << endl;
	    }

      if ((ch == 'x') || (ch == 'q')) {
	      PTRACE(3, "\nEnd of thread to read from keyboard ");
        endNow = TRUE;
        return;
      }
    }

    if (!helped) {
      PTRACE(1, "Serial\t Write the message\"" << str << "\" to the serial port");
      serial.Write(str.GetPointer(), str.GetLength());
      continue;
    }
  }
}
 
// End of serial.cxx
