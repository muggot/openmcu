#include <ptlib.h>
#include <unistd.h>
//#include "conference.h"
#include "mcu.h"
//#include "filemembers.h"


ConferenceSoundCardMember::ConferenceSoundCardMember(Conference * _conference)
#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable:4355)
#endif
  : ConferenceMember(_conference, this)
#ifdef _WIN32
#pragma warning(pop)
#endif
{
  // open the default audio device
  PString deviceName = PSoundChannel::GetDefaultDevice(PSoundChannel::Player);
  soundDevice.Open(deviceName, PSoundChannel::Player, 1, 8000, 16);

  // start a thread to read from the conference and write to default audio device
  if (!soundDevice.IsOpen()) {
    thread = NULL;
    return;
  }

  running = TRUE;
  thread = PThread::Create(PCREATE_NOTIFIER(Thread), 0, PThread::AutoDeleteThread);

  conference->AddMember(this);
}

ConferenceSoundCardMember::~ConferenceSoundCardMember()
{
  Unlisten();
}

void ConferenceSoundCardMember::Unlisten()
{
  if (conference->RemoveMember(this))
    conference->GetManager().RemoveConference(conference->GetID());

  if ((thread != NULL) && running) {
    running = FALSE;
    thread->WaitForTermination();
    thread = NULL;
  }
}

void ConferenceSoundCardMember::Thread(PThread &, INT)
{
  PAdaptiveDelay audioDelay;
  PBYTEArray pcmData(480);
  soundDevice.SetBuffers(480, 3);

  while (running) {

    // read a block of data
    ReadAudio(pcmData.GetPointer(), pcmData.GetSize());

    // write the data to the sound card
    if (soundDevice.IsOpen())
      soundDevice.Write(pcmData.GetPointer(), pcmData.GetSize());

    audioDelay.Delay(pcmData.GetSize() / 16);
  }

  ConferenceManager & mgr = conference->GetManager();
  mgr.RemoveMember(conference->GetID(), this);
}


///////////////////////////////////////////////////////////////////////////

ConferenceFileMember::ConferenceFileMember(Conference * _conference, const PFilePath & _fn, PFile::OpenMode _mode)
#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable:4355)
#endif
  : ConferenceMember(_conference, (void *)this), mode(_mode)
#ifdef _WIN32
#pragma warning(pop)
#endif
{
  filenames.push_back(_fn);
  Construct();
}

ConferenceFileMember::ConferenceFileMember(Conference * _conference, const FilenameList & _fns, PFile::OpenMode _mode)
#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable:4355)
#endif
  : ConferenceMember(_conference, (void *)this), mode(_mode)
#ifdef _WIN32
#pragma warning(pop)
#endif
{
  filenames = _fns;
  Construct();
}

ConferenceFileMember::ConferenceFileMember(Conference * _conference, const OpalMediaFormat & _fmt, PFile::OpenMode _mode)
#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable:4355)
#endif
  : ConferenceMember(_conference, (void *)this), mode(_mode)
#ifdef _WIN32
#pragma warning(pop)
#endif
{
  vformat = _fmt;
  Construct();
}

ConferenceFileMember::ConferenceFileMember(Conference * _conference, const PString & _fmt, PFile::OpenMode _mode)
#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable:4355)
#endif
  : ConferenceMember(_conference, (void *)this), mode(_mode)
#ifdef _WIN32
#pragma warning(pop)
#endif
{
  format = _fmt;
  Construct();
}

void ConferenceFileMember::Construct()
{
  thread = NULL;
  running = FALSE;

  // open the first file
  if (mode != PFile::WriteOnly && !QueueNext())
    return;

  running = TRUE;
  conference->AddMember(this);

  if (mode == PFile::WriteOnly)
  {
   if(format=="recorder")
   {
    thread = PThread::Create(PCREATE_NOTIFIER(WriteThread), 0, PThread::AutoDeleteThread);
    vthread = PThread::Create(PCREATE_NOTIFIER(WriteThreadV), 0, PThread::AutoDeleteThread);
   }
   else // cache
   {
    thread = PThread::Create(PCREATE_NOTIFIER(VideoEncoderCashThread), 0, PThread::AutoDeleteThread);
   }
  }    
  else
    thread = PThread::Create(PCREATE_NOTIFIER(ReadThread), 0, PThread::AutoDeleteThread);
}

ConferenceFileMember::~ConferenceFileMember()
{
  Unlisten();
}

BOOL ConferenceFileMember::QueueNext()
{
  if (filenames.size() == 0)
    return FALSE;

  currentFilename = filenames.front();
  filenames.pop_front();
  if (!file.Open(currentFilename, mode))  {
    PTRACE(1, "Cannot open WAV file " << currentFilename);
    return FALSE;
  }

  PTRACE(1, "Playing WAV file " << currentFilename);

  return TRUE;
}

void ConferenceFileMember::Unlisten()
{
  if (conference->RemoveMember(this))
    conference->GetManager().RemoveConference(conference->GetID());

  if ((thread != NULL) && running) {
    running = FALSE;
    thread->WaitForTermination();
    thread = NULL;
  }
}

const char wavHeader[44] = {0x52,0x49,0x46,0x46,0xe7,0xff,0xff,0x7f,
                      0x57,0x41,0x56,0x45,0x66,0x6d,0x74,0x20,
                      0x10,0x00,0x00,0x00,0x01,0x00,0x01,0x00,
                      0x40,0x1f,0x00,0x00,0x80,0x3e,0x00,0x00,
                      0x02,0x00,0x10,0x00,0x64,0x61,0x74,0x61,
                      0xef,0xff,0xff,0x7f};

void ConferenceFileMember::WriteThread(PThread &, INT)
{
  PString cstr = "sound." + conference->GetNumber();
  const char *cname = cstr;
  cout << "cname= " << cname << "\n";
  mkfifo(cname,S_IRUSR | S_IRGRP | S_IROTH | S_IWUSR);
  int SS=open(cname,O_WRONLY);
  PBYTEArray pcmData(960);
  PAdaptiveDelay audioDelay;
  int success=0;

//  write(SS, wavHeader, 44);

  while (running) {

    // read a block of data
    ReadAudio(pcmData.GetPointer(), pcmData.GetSize());

    // write to the file
    if (write(SS,(const void *)pcmData.GetPointer(), pcmData.GetSize())<0) 
     { close(SS); SS=open(cname,O_WRONLY); success=0; audioDelay.Restart(); }
    else if(success==0) { success++; audioDelay.Restart(); } 
//    cout << "Write ";
    // and delay
    audioDelay.Delay(pcmData.GetSize() / 32);
  }

  close(SS);
  ConferenceManager & mgr = conference->GetManager();
  mgr.RemoveMember(conference->GetID(), this);
}

void ConferenceFileMember::WriteThreadV(PThread &, INT)
{
  PString cstr = "video." + conference->GetNumber();
  const char *cname = cstr;
  cout << "cname= " << cname << "\n";
  mkfifo(cname,S_IRUSR | S_IRGRP | S_IROTH | S_IWUSR);
  int SV=open(cname,O_WRONLY);
  int amount = CIF4_WIDTH*CIF4_HEIGHT*3/2;
  PBYTEArray videoData(amount);
  PAdaptiveDelay videoDelay;
  int success=0;
  
  while (running) {

    // read a block of data
    conference->ReadMemberVideo(this,videoData.GetPointer(),CIF4_WIDTH,CIF4_HEIGHT,amount);

    // write to the file
    if (write(SV,(const void *)videoData.GetPointer(), amount)<0) 
     { close(SV); SV=open(cname,O_WRONLY); success=0; videoDelay.Restart();}
    else if(success==0) { success++; videoDelay.Restart(); }

    // and delay
    videoDelay.Delay(100);
  }

  close(SV);
}

void ConferenceFileMember::ReadThread(PThread &, INT)
{
  PBYTEArray pcmData(480);
  PAdaptiveDelay audioDelay;
  
  while (running) {

    if (!file.IsOpen())
      break;

    // read a block of data from the file
    if (!file.Read(pcmData.GetPointer(), pcmData.GetSize())) {
      if (!QueueNext())
        break;
      else
        continue;
    }

    // read a block of data
    WriteAudio(pcmData.GetPointer(), pcmData.GetSize());

    // and delay
    audioDelay.Delay(pcmData.GetSize() / 16);
  }

  ConferenceManager & mgr = conference->GetManager();
  mgr.RemoveMember(conference->GetID(), this);
}

void ConferenceFileMember::VideoEncoderCashThread(PThread &, INT)
{
  OpenMCUH323EndPoint & ep = OpenMCU::Current().GetEndpoint();
//  const H323Capabilities &caps = ep.GetCapabilities();
  H323Capabilities caps;
  caps.AddCapabilities(0, 0, (const char **) ep.tvCaps);
  H323Capability * cap = caps.FindCapability(vformat);
  OpalMediaFormat & wf = cap->GetWritableMediaFormat(); 
  wf = vformat;

  if(cap!=NULL)
  {
   int status = 1;
   cout << "Starting cache thread\n";
    H323VideoCodec * codec = (H323VideoCodec *) cap->CreateCodec(H323Codec::Encoder);
    codec->cacheMode = 1; // caching codec
    OpenMCUH323Connection * con = new OpenMCUH323Connection(ep,0,NULL);
    con->SetupCacheConnection(vformat,conference,this);
    con->OpenVideoChannel(TRUE,*codec);
    
    if(codec->CheckCacheRTP())
    {
     delete(con);
     delete(codec);
     caps.RemoveAll();
     ConferenceManager & mgr = conference->GetManager();
     mgr.RemoveMember(conference->GetID(), this);
     return;
    }
    
    codec->NewCacheRTP();
    RTP_DataFrame frame;
    unsigned length = 0;
    // from here we are ready to call codec->Read in cicle
    while (running) 
    {
     while(codec->GetCacheUsersNumber()==0) 
     {
      if(status == 1 )  
       { status = 0; cout << "Down to sleep " << codec->formatString << "\n"; }
      PThread::Sleep(1000); 
     }
     if(status == 0 )  
     { 
      status = 1; 
      cout << "Wake up " << codec->formatString << "\n"; 
      con->RestartGrabber();
     }
     codec->Read(NULL,length,frame);
    }
    // must destroy videograbber and videochanell here? fix it
    delete(con);
    delete(codec);
    caps.RemoveAll();
    ConferenceManager & mgr = conference->GetManager();
    mgr.RemoveMember(conference->GetID(), this);
  }
}
