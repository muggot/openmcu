/*
 * main.cxx
 *
 * PWLib application source file for vxmltest
 *
 * Main program entry point.
 *
 * Copyright 2002 Equivalence
 *
 * $Log: main.cxx,v $
 * Revision 1.5  2004/07/01 13:05:58  rjongbloed
 * Rewrite of plug in system to use single global variable for all factories to avoid all sorts
 *   of issues with startup orders and Windows DLL multiple instances.
 *
 * Revision 1.4  2004/06/19 09:00:35  csoutheren
 * Updated for TTS changes
 *
 * Revision 1.3  2004/06/02 08:30:22  csoutheren
 * Tweaks to avoid some problems with reading single bytes from a PCM stream
 *
 * Revision 1.2  2003/09/26 13:41:31  rjongbloed
 * Added special test to give more indicative error if try to compile without Expat support.
 *
 * Revision 1.1  2002/08/06 05:26:33  craigs
 * Initial version
 *
 */

#include <ptlib.h>
#include <ptlib/sound.h>
#include <ptclib/vxml.h>

#if !P_EXPAT
#error Must have Expat XML support for this application
#endif


#include "main.h"


PCREATE_PROCESS(Vxmltest);

#define BUFFER_SIZE 1024

class ChannelCopyThread : public PThread
{
  PCLASSINFO(ChannelCopyThread, PThread);
  public:
    ChannelCopyThread(PChannel & _from, PChannel & _to)
      : PThread(1000, NoAutoDeleteThread), from(_from), to(_to)
    { Resume(); }

    void Main();

  protected:
    PChannel & from;
    PChannel & to;
};

void ChannelCopyThread::Main()
{
  for (;;) {

    from.SetReadTimeout(P_MAX_INDEX);
    PBYTEArray readData;
    if (!from.Read(readData.GetPointer(BUFFER_SIZE), 2)) {
      PTRACE(2, "Read error 1");
      break;
    }
    from.SetReadTimeout(0);
    if (!from.Read(readData.GetPointer()+2, BUFFER_SIZE-2)) {
      if (from.GetErrorCode(PChannel::LastReadError) != PChannel::Timeout) {
        PTRACE(2, "Read error 2");
        break;
      }
    }
    readData.SetSize(from.GetLastReadCount()+2);

    if (readData.GetSize() > 0) {
      if (!to.Write((const BYTE *)readData, readData.GetSize())) {
        PTRACE(2, "Write error");
        break;
      }
    }
  }
}

Vxmltest::Vxmltest()
  : PProcess("Equivalence", "vxmltest", 1, 0, AlphaCode, 1)
{
}


void Vxmltest::Main()
{
  PArgList & args = GetArguments();
  args.Parse(
             "t.-trace."
             "o:output:"
             "-tts:"
             );

#if PTRACING
  PTrace::Initialise(args.GetOptionCount('t'),
                     args.HasOption('o') ? (const char *)args.GetOptionString('o') : NULL,
         PTrace::Blocks | PTrace::Timestamp | PTrace::Thread | PTrace::FileAndLine);
#endif

  if (args.GetCount() < 1) {
    PError << "usage: vxmltest [opts] doc\n";
    return;
  }

  PTextToSpeech * tts = NULL;
  PFactory<PTextToSpeech>::KeyList_T engines = PFactory<PTextToSpeech>::GetKeyList();
  if (engines.size() != 0)
    tts = PFactory<PTextToSpeech>::CreateInstance(engines[0]);
  if (tts == NULL) {
    PError << "error: cannot select default text to speech engine" << endl;
    return;
  }

  vxml = new PVXMLSession(tts);
  PString device = PSoundChannel::GetDefaultDevice(PSoundChannel::Player);
  PSoundChannel player;
  if (!player.Open(device, PSoundChannel::Player)) {
    PError << "error: cannot open sound device \"" << device << "\"" << endl;
    return;
  }
  cout << "Using audio device \"" << device << "\"" << endl;

  if (!vxml->Load(args[0])) {
    PError << "error: cannot loading VXML document \"" << args[0] << "\" - " << vxml->GetXMLError() << endl;
    return;
  }

  if (!vxml->Open(TRUE)) {
    PError << "error: cannot open VXML device in PCM mode" << endl;
    return;
  }

  cout << "Starting media" << endl;
  PThread * thread1 = new ChannelCopyThread(*vxml, player);

  inputRunning = TRUE;
  PThread * inputThread = PThread::Create(PCREATE_NOTIFIER(InputThread), 0, NoAutoDeleteThread);

  thread1->WaitForTermination();

  inputRunning = FALSE;
  cout << "Press a key to continue" << endl;
  inputThread->WaitForTermination();

  cout << "Media finished" << endl;
}

void Vxmltest::InputThread(PThread &, INT)
{
  PConsoleChannel console(PConsoleChannel::StandardInput);

  while (inputRunning) {
    console.SetReadTimeout(100);
    int ch = console.ReadChar();
    if (ch > 0)
      vxml->OnUserInput(PString((char)ch));
  }
}


// End of File ///////////////////////////////////////////////////////////////
