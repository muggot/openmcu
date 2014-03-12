
#include <ptlib.h>

#ifndef _WIN32
#include <unistd.h>
#endif

#include "mcu.h"

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
  if (!soundDevice.IsOpen()) return;

  running = TRUE;
  PThread::Create(PCREATE_NOTIFIER(Thread), 0, PThread::AutoDeleteThread);

  conference->AddMember(this);
}

ConferenceSoundCardMember::~ConferenceSoundCardMember()
{
  Unlisten();
}

void ConferenceSoundCardMember::Unlisten()
{
  conference->RemoveMember(this);
  running=FALSE;
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
  PTRACE(1,"FileMember\tConstruct");
  running = FALSE;

  // open the first file
  if (mode != PFile::WriteOnly && !QueueNext()) return;

  if(conference==NULL) return;
  roomName=conference->GetNumber();

  running = TRUE; a_ended = FALSE; v_ended = FALSE;
  conference->AddMember(this);

  if (mode == PFile::WriteOnly)
  {
    if(format=="recorder")
    {
      PThread::Create(PCREATE_NOTIFIER(WriteThread), 0, PThread::AutoDeleteThread);
      PThread::Create(PCREATE_NOTIFIER(WriteThreadV), 0, PThread::AutoDeleteThread);
    }
    else // cache
    {
      PThread::Create(PCREATE_NOTIFIER(VideoEncoderCacheThread), 0, PThread::AutoDeleteThread);
    }
  }    
  else
    PThread::Create(PCREATE_NOTIFIER(ReadThread), 0, PThread::AutoDeleteThread);
}

ConferenceFileMember::~ConferenceFileMember()
{
  PTRACE(1,"ConferenceFileMember\tDestructor for " << GetName());
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
  if(!running) return;
  running = FALSE;

  if (mode == PFile::WriteOnly) if(format=="recorder")
  {
#ifdef _WIN32
    DeleteFile(PString("\\\\.\\pipe\\sound_") + roomName);
    DeleteFile(PString("\\\\.\\pipe\\video_") + roomName);
#else
    if((!audioPipeName.IsEmpty())&&(!a_ended))
    {
      int SS = open(audioPipeName,O_RDONLY | O_NONBLOCK);
      PBYTEArray buffer;
      buffer.SetSize(100);
      while(!a_ended) (void)read(SS, buffer.GetPointer(), buffer.GetSize());
      close(SS);
      unlink(audioPipeName);
    }
    if((!videoPipeName.IsEmpty())&&(!v_ended))
    {
      int SS = open(videoPipeName,O_RDONLY | O_NONBLOCK);
      PBYTEArray buffer;
      buffer.SetSize(100);
      while(!v_ended) (void)read(SS, buffer.GetPointer(), buffer.GetSize());
      close(SS);
      unlink(videoPipeName);
    }
#endif
  }

  PTRACE(5,"ConferenceFileMember\tWaiting for termination");
  while(!(a_ended && v_ended))
  { PThread::Sleep(10);
    PTRACE(5,"ConferenceFileMember still waiting when audio & video threads will stopped: audio=" << a_ended << " video=" << v_ended);
  }
  PTRACE(5,"ConferenceFileMember\tTerminated");

}

const unsigned char wavHeader[44] =
  { 0x52,0x49,0x46,0x46,0xe7,0xff,0xff,0x7f,
    0x57,0x41,0x56,0x45,0x66,0x6d,0x74,0x20,
    0x10,0x00,0x00,0x00,0x01,0x00,0x01,0x00,
    0x40,0x1f,0x00,0x00,0x80,0x3e,0x00,0x00,
    0x02,0x00,0x10,0x00,0x64,0x61,0x74,0x61,
    0xef,0xff,0xff,0x7f};

#ifdef _WIN32

#  define MY_NAMED_PIPE_OPEN(_name,_bufOut,_bufIn) \
     PString cstr = PString("\\\\.\\pipe\\") + _name + "_" + conference->GetNumber(); \
     \
     LPCSTR cname = cstr; \
     \
     HANDLE pipe = CreateNamedPipe( \
       cname, \
       PIPE_ACCESS_OUTBOUND | FILE_FLAG_OVERLAPPED, \
       PIPE_TYPE_BYTE|PIPE_WAIT, \
       PIPE_UNLIMITED_INSTANCES, /* DWORD nMaxInstances */ \
       _bufOut,                  /* DWORD nOutBufferSize */ \
       _bufIn,                   /* DWORD nInBufferSize */ \
       0,                        /* DWORD nDefaultTimeOut */ \
       NULL                      /* /LPSECURITY_ATTRIBUTES lpSecurityAttributes */ \
     ); \
     \
     if (pipe == NULL || pipe == INVALID_HANDLE_VALUE) \
     { \
       PTRACE(3,"WriteThread\tFailed to create outbound pipe instance " << cstr); \
       return; \
     } \
     \
     if(!ConnectNamedPipe(pipe, NULL)) { \
       PTRACE(3,"WriteThread\tCould not connect to pipe: " << cstr); \
       return; \
     } \
     \
     PTRACE(3,"WriteThread\tNamed pipe created: " << cstr);

#  define MY_NAMED_PIPE_CLOSE \
   { \
     DWORD one = 1; \
     LPDWORD lpone = &one; \
     if (!SetNamedPipeHandleState(pipe, lpone, lpone, lpone)) \
     { \
       PTRACE(2,"WriteThread\tSetNamedPipeHandleState failed"); \
     } \
     FlushFileBuffers(pipe); \
     DisconnectNamedPipe(pipe); \
     CloseHandle(pipe); \
   }

#endif

void ConferenceFileMember::WriteThread(PThread &, INT)
{
  unsigned sampleRate = OpenMCU::Current().vr_sampleRate;
  if(sampleRate < 2000 || sampleRate > 1000000) sampleRate = 16000;

  unsigned channels = OpenMCU::Current().vr_audioChans;
  if(channels < 1 || channels > 8) channels = 1;

  PINDEX amountBytes = channels * 2 * sampleRate * AUDIO_EXPORT_PCM_BUFFER_SIZE_MS / 1000;
  unsigned modulo = 0;
  unsigned d0 = (1000 * (amountBytes >> 1));
  unsigned d = d0 / (sampleRate * channels);
  unsigned m0 = d0 % (sampleRate * channels);
#ifdef _WIN32
  MY_NAMED_PIPE_OPEN("sound",amountBytes,0);
#else
#  ifdef SYS_PIPE_DIR
  PString cstr = PString(SYS_PIPE_DIR) + "/sound." + conference->GetNumber();
#  else
  PString cstr = "sound." + conference->GetNumber();
#  endif
  audioPipeName = cstr;
  const char *cname = cstr;
  PTRACE(2,"AudioExportThread\tcname=" << cname);
  mkfifo(cname,S_IRUSR | S_IRGRP | S_IROTH | S_IWUSR);
  int SS=open(cname,O_WRONLY);
#endif
  PBYTEArray pcmData(amountBytes);
  PAdaptiveDelay audioDelay;
  int success=0;
  unsigned wrn=63;

//  write(SS, wavHeader, 44);

  a_ended=FALSE;
  while (running)
  {
    // read a block of data
    ReadAudio(pcmData.GetPointer(), amountBytes, sampleRate, channels);

    // write to the file
#ifdef _WIN32
    DWORD lpNumberOfBytesWritten;
    BOOL result=WriteFile(pipe, (const void *)pcmData.GetPointer(), amountBytes, &lpNumberOfBytesWritten, NULL);
    if(!result)result=(GetLastError()==ERROR_IO_PENDING);
    if(result) {
      if(success==0) { success++; audioDelay.Restart(); }
    }
    else
    { MY_NAMED_PIPE_CLOSE; PThread::Sleep(120); if(!running) break; MY_NAMED_PIPE_OPEN("sound",amountBytes,0); success=0; audioDelay.Restart();
      wrn=(wrn+1)&63; if(!wrn) OpenMCU::Current().HttpWriteEventRoom("Video recorder audio pipe problem",conference->GetNumber());
    }
#else
    if (write(SS,(const void *)pcmData.GetPointer(), amountBytes)<0) 
    { close(SS); PThread::Sleep(120); if(!running) break; SS=open(cname,O_WRONLY); success=0; audioDelay.Restart();
      wrn=(wrn+1)&63; if(!wrn) OpenMCU::Current().HttpWriteEventRoom("Video recorder audio pipe problem",conference->GetNumber());
    }
    else if(success==0) { success++; audioDelay.Restart(); } 
#endif

    // and delay
    modulo += m0;
    if(audioDelay.Delay(d + (modulo / (channels * sampleRate))))
    {
      PTRACE(6,"WriteThread\tAdaptive delay for " << AUDIO_EXPORT_PCM_BUFFER_SIZE_MS << "ms called too late, sampleRate=" << sampleRate << " amountBytes=" << amountBytes);
    }
    modulo %= (channels * sampleRate);
  }

#ifdef _WIN32
  MY_NAMED_PIPE_CLOSE;
#else
  close(SS);
#endif
  a_ended=TRUE;
}

void ConferenceFileMember::WriteThreadV(PThread &, INT)
{
  int width=OpenMCU::Current().vr_framewidth;
  int height=OpenMCU::Current().vr_frameheight;
  int framerate=OpenMCU::Current().vr_framerate;

  if(width<176 || width>1920) { width=704; PTRACE(1,"WriteThreadV\tWrong frame width value changed to 4CIF width"); }
  if(height<144 || height>1152) { height=576; PTRACE(1,"WriteThreadV\tWrong frame height value changed to 4CIF height"); }
  if(framerate<1 || framerate>100) { framerate=10; PTRACE(1,"WriteThreadV\tWrong frame rate value changed to 10 FPS"); }

  PINDEX amount = width*height*3/2;
  int delay = 1000/framerate;
#ifdef _WIN32
  MY_NAMED_PIPE_OPEN("video",amount,0);
#else
#ifdef SYS_PIPE_DIR
  PString cstr = PString(SYS_PIPE_DIR) + "/video." + conference->GetNumber();
#else
  PString cstr = "video." + conference->GetNumber();
#endif
  videoPipeName = cstr;
  const char *cname = cstr;
  PTRACE(2,"VideoExportThread\tcname=" << cname);
  mkfifo(cname,S_IRUSR | S_IRGRP | S_IROTH | S_IWUSR);
  int SV=open(cname,O_WRONLY);
#endif
  PBYTEArray videoData(amount);
  PAdaptiveDelay videoDelay;
  int success=0;
  unsigned wrn=63;

  v_ended=FALSE;
  while (running)
  {
    // read a block of data
    if(videoMixer!=NULL) videoMixer->ReadFrame(*this,videoData.GetPointer(),width,height,amount);
    else conference->ReadMemberVideo(this,videoData.GetPointer(),width,height,amount);

    // write to the file
#ifdef _WIN32
    DWORD lpNumberOfBytesWritten=0;
    BOOL result=WriteFile(pipe, (const void *)videoData.GetPointer(), amount, &lpNumberOfBytesWritten, NULL);
    if(!running) break;
    if(!result)result=(GetLastError()==ERROR_IO_PENDING);
    if(result) {
      if(success==0) { success++; videoDelay.Restart(); }
    }
    else
    { MY_NAMED_PIPE_CLOSE; PThread::Sleep(120); if(!running) break; MY_NAMED_PIPE_OPEN("video",amount,0); success=0; videoDelay.Restart();
      wrn=(wrn+1)&63; if(!wrn) OpenMCU::Current().HttpWriteEventRoom("Video recorder video pipe problem",conference->GetNumber());
    }
#else
    if (write(SV,(const void *)videoData.GetPointer(), amount)<0) 
    { close(SV); PThread::Sleep(120); if(!running) break; SV=open(cname,O_WRONLY); success=0; videoDelay.Restart();
      wrn=(wrn+1)&63; if(!wrn) OpenMCU::Current().HttpWriteEventRoom("Video recorder video pipe problem",conference->GetNumber());
    }
    else if(success==0) { success++; videoDelay.Restart(); }
#endif

    // and delay
    if(videoDelay.Delay(delay))
    {
      PTRACE(6,"WriteThreadV\tAdaptive delay for " << delay << "ms called too late, framerate=" << framerate << " amount=" << amount);
    }
  }

#ifdef _WIN32
  MY_NAMED_PIPE_CLOSE;
#else
  close(SV);
#endif
  v_ended=TRUE;
}

void ConferenceFileMember::ReadThread(PThread &, INT)
{
  PBYTEArray pcmData(480);
  PAdaptiveDelay audioDelay;
  
  v_ended=TRUE; a_ended=FALSE;
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
  a_ended=TRUE;
}

void ConferenceFileMember::VideoEncoderCacheThread(PThread &, INT)
{
  OpenMCUH323EndPoint & ep = OpenMCU::Current().GetEndpoint();
//  const H323Capabilities &caps = ep.GetCapabilities();
  H323Capabilities caps;
  caps.AddCapabilities(0, 0, (const char **) ep.tvCaps);
  H323Capability * cap = caps.FindCapability(vformat);
  OpalMediaFormat & wf = cap->GetWritableMediaFormat(); 
  wf = vformat;

  a_ended=TRUE; v_ended=FALSE;
  if(cap!=NULL)
  {
    status = 1;
    cout << "Starting cache thread\n";
    PTRACE(3,"Cache\tStarting cache thread " << vformat);
    codec = (H323VideoCodec *) cap->CreateCodec(H323Codec::Encoder);
    codec->cacheMode = 1; // caching codec
    con = new OpenMCUH323Connection(ep,0,NULL);
    con->videoMixerNumber=videoMixerNumber;
    con->SetupCacheConnection(vformat,conference,this);
    con->OpenVideoChannel(TRUE,*codec);
    
    if(codec->CheckCacheRTP())
    {
      PTRACE(3,"Cache\t" << vformat << " already exists, nothing to do, stopping thread");
      if(conference!=NULL) conference->RemoveMember(this);
      delete(con); con=NULL;
      delete(codec); codec=NULL;
      caps.RemoveAll();
      v_ended=TRUE;
      return;
    }

    codec->NewCacheRTP();
    RTP_DataFrame frame;
    unsigned length = 0;
    // from here we are ready to call codec->Read in cicle
    while (running) 
    {
      while((codec->GetCacheUsersNumber()==0) && running)
      {
        if(status == 1 )  
        {
          status = 0;
          totalVideoFramesSent=0;
          PTRACE(3,"MCU\tDown to sleep " << codec->formatString);
          cout << "Down to sleep " << codec->formatString << "\n";
        }
        PThread::Sleep(1000); 
      }
      if((status == 0) && running)
      { 
        status = 1; 
        PTRACE(3,"MCU\tWake up " << codec->formatString); 
        cout << "Wake up " << codec->formatString << "\n"; 
        con->RestartGrabber();
        firstFrameSendTime=PTime();
      }
      if(running) codec->Read(NULL,length,frame);
    }
    // must destroy videograbber and videochanell here? fix it
    delete(con); con=NULL;
    codec->CodecDeleteCacheRTP();
    delete(codec); codec=NULL;
    caps.RemoveAll();
  }
  v_ended=TRUE;
}
