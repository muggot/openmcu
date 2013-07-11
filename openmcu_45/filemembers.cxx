
#include <ptlib.h>

#ifndef _WIN32
#include <unistd.h>
#endif

//#include "conference.h"
#include "mcu.h"
//#include "filemembers.h"

#define AUDIO_EXPORT_PCM_BUFFER_SIZE_MS    30

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
    ReadAudio(pcmData.GetPointer(), pcmData.GetSize(), 8000, 1);

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

ConferenceFileMember::ConferenceFileMember(Conference * _conference, const OpalMediaFormat & _fmt, PFile::OpenMode _mode, unsigned _videoMixerNumber)
#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable:4355)
#endif
  : ConferenceMember(_conference, (void *)this), mode(_mode)
#ifdef _WIN32
#pragma warning(pop)
#endif
{
  videoMixerNumber=_videoMixerNumber;
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

const unsigned char wavHeader[44] =
  { 0x52,0x49,0x46,0x46,0xe7,0xff,0xff,0x7f,
    0x57,0x41,0x56,0x45,0x66,0x6d,0x74,0x20,
    0x10,0x00,0x00,0x00,0x01,0x00,0x01,0x00,
    0x40,0x1f,0x00,0x00,0x80,0x3e,0x00,0x00,
    0x02,0x00,0x10,0x00,0x64,0x61,0x74,0x61,
    0xef,0xff,0xff,0x7f};

void ConferenceFileMember::WriteThread(PThread &, INT)
{
  unsigned sampleRate = OpenMCU::Current().vr_sampleRate;
  if(sampleRate < 2000 || sampleRate > 1000000) sampleRate = 16000;
  PINDEX amountBytes = 2 * sampleRate * AUDIO_EXPORT_PCM_BUFFER_SIZE_MS / 1000;
  unsigned modulo = 0;
  unsigned d0 = (1000 * (amountBytes >> 1));
  unsigned d = d0 / sampleRate;
  unsigned m0 = d0 % sampleRate;
#ifdef _WIN32
  PString cstr="\\\\.\\pipe\\sound_"+conference->GetNumber();
  LPCSTR cname = cstr;
  HANDLE pipe = CreateNamedPipe(cname, PIPE_ACCESS_OUTBOUND,
    PIPE_TYPE_BYTE|PIPE_WAIT,
    1,   //DWORD nMaxInstances
    amountBytes, //DWORD nOutBufferSize
    0,   //DWORD nInBufferSize
    0,   //DWORD nDefaultTimeOut
    NULL //LPSECURITY_ATTRIBUTES lpSecurityAttributes
  );
  if (pipe == NULL || pipe == INVALID_HANDLE_VALUE) {
    PTRACE(3,"WriteThread\tFailed to create outbound pipe instance for audio: " << cstr);
    ConferenceManager & mgr = conference->GetManager();
    mgr.RemoveMember(conference->GetID(), this);
    return;
  }
  if (!ConnectNamedPipe(pipe, NULL)) {
    PTRACE(3,"WriteThread\tCould not connect to audio named pipe: " << cstr);
    ConferenceManager & mgr = conference->GetManager();
    mgr.RemoveMember(conference->GetID(), this);
    return;
  }
  PTRACE(3,"WriteThread\tAudio pipe created: " << cstr);
#else
#ifdef SYS_PIPE_DIR
  PString cstr = PString(SYS_PIPE_DIR) + "/sound." + conference->GetNumber();
#else
  PString cstr = "sound." + conference->GetNumber();
#endif
  const char *cname = cstr;
  cout << "cname= " << cname << "\n";
  mkfifo(cname,S_IRUSR | S_IRGRP | S_IROTH | S_IWUSR);
  int SS=open(cname,O_WRONLY);
#endif
  PBYTEArray pcmData(amountBytes);
  PAdaptiveDelay audioDelay;
  int success=0;

//  write(SS, wavHeader, 44);

  while (running) {

    modulo += m0;
    if(audioDelay.Delay(d + (modulo / sampleRate)))
    { PTRACE(6,"AudioExportThread_Delay\tPAdaptiveDelay.Delay() called Too late. Sample rate = " << sampleRate << " Hz, delay = " << AUDIO_EXPORT_PCM_BUFFER_SIZE_MS << " ms, amount = " << amountBytes << " bytes.");
    }
    modulo %= sampleRate;

    // read a block of data
    ReadAudio(pcmData.GetPointer(), amountBytes, sampleRate, 1);

    // write to the file
#ifdef _WIN32
    DWORD lpNumberOfBytesWritten;
    BOOL result=WriteFile(pipe, (const void *)pcmData.GetPointer(), amountBytes, &lpNumberOfBytesWritten, NULL);
    if(!result)result=(GetLastError()==ERROR_IO_PENDING);
    if(result) {
      if(success==0) { success++; audioDelay.Restart(); }
    } else {
      CloseHandle(pipe);
      pipe = CreateNamedPipe(cname, PIPE_ACCESS_OUTBOUND,
        PIPE_TYPE_BYTE|PIPE_WAIT,
        1,   //DWORD nMaxInstances
        amountBytes, //DWORD nOutBufferSize
        0,   //DWORD nInBufferSize
        0,   //DWORD nDefaultTimeOut
        NULL //LPSECURITY_ATTRIBUTES lpSecurityAttributes
      );
      if (pipe == NULL || pipe == INVALID_HANDLE_VALUE) {
        PTRACE(3,"WriteThread\tWriting audio to pipe failed; could not re-create outbound pipe instance: " << cstr);
        ConferenceManager & mgr = conference->GetManager();
        mgr.RemoveMember(conference->GetID(), this);
        return;
      }
      if (!ConnectNamedPipe(pipe, NULL)) {
        PTRACE(3,"WriteThread\tCould not connect to audio named pipe: " << cstr);
        ConferenceManager & mgr = conference->GetManager();
        mgr.RemoveMember(conference->GetID(), this);
        return;
      }
      PTRACE(3,"WriteThread\tAudio pipe re-created: " << cstr);
      success=0; audioDelay.Restart();
    }
#else
    if (write(SS,(const void *)pcmData.GetPointer(), amountBytes)<0) 
     { close(SS); SS=open(cname,O_WRONLY); success=0; audioDelay.Restart(); }
    else if(success==0) { success++; audioDelay.Restart(); } 
//    cout << "Write ";
#endif
    // and delay
//    audioDelay.Delay(pcmData.GetSize() / 32);


  }

#ifdef _WIN32
  CloseHandle(pipe);
#else
  close(SS);
#endif
  ConferenceManager & mgr = conference->GetManager();
  mgr.RemoveMember(conference->GetID(), this);
}

void ConferenceFileMember::WriteThreadV(PThread &, INT)
{
  int width=OpenMCU::Current().vr_framewidth;
  int height=OpenMCU::Current().vr_frameheight;
  int framerate=OpenMCU::Current().vr_framerate;

  if(width<176 || width>1920) { width=704; PTRACE(1,"WriteThreadV\tWrong frame width value changed to 4CIF width"); }
  if(height<144 || height>1152) { height=576; PTRACE(1,"WriteThreadV\tWrong frame height value changed to 4CIF height"); }
  if(framerate<1 || framerate>100) { framerate=10; PTRACE(1,"WriteThreadV\tWrong frame rate value changed to 10 FPS"); }

  int amount = width*height*3/2;
  int delay = 1000/framerate;
#ifdef _WIN32
  PString cstr="\\\\.\\pipe\\video_" + conference->GetNumber();
  LPCSTR cname = cstr;
  HANDLE pipe = CreateNamedPipe(cname, PIPE_ACCESS_OUTBOUND,
    PIPE_TYPE_BYTE|PIPE_WAIT,
    1,      //DWORD nMaxInstances
    amount, //DWORD nOutBufferSize
    0,      //DWORD nInBufferSize
    0,      //DWORD nDefaultTimeOut
    NULL    //LPSECURITY_ATTRIBUTES lpSecurityAttributes
  );
  if (pipe == NULL || pipe == INVALID_HANDLE_VALUE) {
    PTRACE(3,"WriteThreadV\tFailed to create outbound pipe instance for video: " << cstr);
    return;
  }
  if (!ConnectNamedPipe(pipe, NULL)) {
    PTRACE(3,"WriteThread\tCould not connect to video named pipe: " << cstr);
    return;
  }
  PTRACE(3,"WriteThreadV\tVideo pipe created: " << cstr);
#else
#ifdef SYS_PIPE_DIR
  PString cstr = PString(SYS_PIPE_DIR) + "/video." + conference->GetNumber();
#else
  PString cstr = "video." + conference->GetNumber();
#endif
  const char *cname = cstr;
  cout << "cname= " << cname << "\n";
  mkfifo(cname,S_IRUSR | S_IRGRP | S_IROTH | S_IWUSR);
  int SV=open(cname,O_WRONLY);
#endif
  PBYTEArray videoData(amount);
  PAdaptiveDelay videoDelay;
  int success=0;
  
  while (running) {

    // read a block of data
    if(videoMixer!=NULL) videoMixer->ReadFrame(*this,videoData.GetPointer(),width,height,amount);
    else conference->ReadMemberVideo(this,videoData.GetPointer(),width,height,amount);

    // write to the file
#ifdef _WIN32
    DWORD lpNumberOfBytesWritten=0;
    BOOL result=WriteFile(pipe, (const void *)videoData.GetPointer(), amount, &lpNumberOfBytesWritten, NULL);
    if(!result)result=(GetLastError()==ERROR_IO_PENDING);
    if(result) {
      if(success==0) { success++; videoDelay.Restart(); }
    } else {
      CloseHandle(pipe);
      pipe = CreateNamedPipe(cname, PIPE_ACCESS_OUTBOUND,
        PIPE_TYPE_BYTE|PIPE_WAIT,
        1,      //DWORD nMaxInstances
        amount, //DWORD nOutBufferSize
        0,      //DWORD nInBufferSize
        0,      //DWORD nDefaultTimeOut
        NULL    //LPSECURITY_ATTRIBUTES lpSecurityAttributes
      );
      if (pipe == NULL || pipe == INVALID_HANDLE_VALUE) {
        PTRACE(3,"WriteThreadV\tWriting video to pipe failed; could not re-create outbound pipe instance " << cstr);
        return;
      }
      if (!ConnectNamedPipe(pipe, NULL)) {
        PTRACE(3,"WriteThread\tCould not connect to video named pipe: " << cstr);
        return;
      }
      PTRACE(3,"WriteThreadV\tVideo pipe re-created: " << cstr);
      success=0; videoDelay.Restart();
    }
#else
    if (write(SV,(const void *)videoData.GetPointer(), amount)<0) 
     { close(SV); SV=open(cname,O_WRONLY); success=0; videoDelay.Restart();}
    else if(success==0) { success++; videoDelay.Restart(); }
#endif

    // and delay
    videoDelay.Delay(delay);
  }

#ifdef _WIN32
  CloseHandle(pipe);
  PTRACE(3,"WriteThreadV\tVideo pipe closed: " << cstr);
#else
  close(SV);
#endif
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
    WriteAudio(pcmData.GetPointer(), pcmData.GetSize(), 8000, 1);

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
   status = 1;
   cout << "Starting cache thread\n";
    codec = (H323VideoCodec *) cap->CreateCodec(H323Codec::Encoder);
    codec->cacheMode = 1; // caching codec
    con = new OpenMCUH323Connection(ep,0,NULL);
    con->videoMixerNumber=videoMixerNumber;
    con->SetupCacheConnection(vformat,conference,this);
    con->OpenVideoChannel(TRUE,*codec);
    
    if(codec->CheckCacheRTP())
    {
     delete(con); con=NULL;
     delete(codec); codec=NULL;
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
       { status = 0; cout << "Down to sleep " << codec->formatString << "\n"; totalVideoFramesSent=0; }
      PThread::Sleep(1000); 
     }
     if(status == 0 )  
     { 
      status = 1; 
      cout << "Wake up " << codec->formatString << "\n"; 
      con->RestartGrabber();
      firstFrameSendTime=PTime();
     }
     codec->Read(NULL,length,frame);
    }
    // must destroy videograbber and videochanell here? fix it
    delete(con); con=NULL;
    delete(codec); codec=NULL;
    caps.RemoveAll();
    ConferenceManager & mgr = conference->GetManager();
    mgr.RemoveMember(conference->GetID(), this);
  }
}
