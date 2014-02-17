#ifdef __GNUC__
#pragma implementation "sound.h"
#endif

#include <fstream>
#include <ptlib.h>

extern char soundbite[];
extern unsigned long soundbitesize;

PSoundChannel::PSoundChannel()
{
  Construct();
}


void PSoundChannel::Construct()
{
}

PSoundChannel::~PSoundChannel()
{
}

PStringArray PSoundChannel::GetDeviceNames(Directions /*dir*/)
{
  PStringArray array;
  return array;
}


PString PSoundChannel::GetDefaultDevice(Directions dir)
{
  switch(dir)
  {
      case Recorder:
	      return "/dev/audiorecord";
	  case Player:
          return "/dev/audioplay";
      default:
          return "/dev/audio";
  }
}

PString PSoundChannel::GetName() const
{
  switch(m_Direction)
  {
      case Recorder:
	      return "/dev/audiorecord";
	  case Player:
          return "/dev/audioplay";
      default:
          return "/dev/audio";
  }
}

BOOL PSoundChannel::Open(const PString & device,
                         Directions dir,
                         unsigned numChannels,
                         unsigned sampleRate,
                         unsigned bitsPerSample)
{
    PTRACE(2, "PSoundChannel::Open("<<(dir==Recorder?"Recording":"Playing")<<","<<numChannels<<","<<sampleRate<<","<<bitsPerSample<<")");
    m_device = device;
    if(dir==Recorder)
    {
    } else {
       m_soundbitereadptr = 0L;
    }
    m_Direction = dir;
    m_numChannels = numChannels;
    m_sampleRate = sampleRate;
    m_bitsPerSample = bitsPerSample;

    return TRUE;
}

BOOL PSoundChannel::Read(void *buf, PINDEX len)
{
// len should be 8 bytes. It if isnt, yell.
   if(len!=8) PTRACE(0, "Asked for "<<len<<" bytes.");
//    PTRACE(9, "PSoundChannel::Read");
    if(m_Direction==Player)
    {
//        PTRACE(0, "Tried to PSoundChannel::Read from a Player channel.");
        return FALSE;
    }
    if(soundbitesize%8) soundbitesize = (soundbitesize/8)*8;
    unsigned int *dest, *src;
    dest = (unsigned int *)buf; src = (unsigned int *)&soundbite[m_soundbitereadptr];
    *dest++ = *src++;
    *dest++ = *src++;
    m_soundbitereadptr+=8;
    if(m_soundbitereadptr>= soundbitesize) m_soundbitereadptr=0L;
#if 0
    unsigned long available, tocopy, temp;
    available = soundbitesize - m_soundbitereadptr;
    tocopy = available<len?available:len;
    memcpy((char *)buf, (char *)&soundbite[m_soundbitereadptr],
            tocopy);
    m_soundbitereadptr+=tocopy;
    if(m_soundbitereadptr>= soundbitesize) m_soundbitereadptr=0L;
    if(tocopy < len)
    {
       temp = tocopy;
       tocopy = len-tocopy;
       memcpy((char *)&((char *)buf)[temp],
             (char *)&soundbite[m_soundbitereadptr], tocopy);
       m_soundbitereadptr += tocopy;
    }

#endif
    return TRUE;
//    PTRACE(1, "PSoundChannel::Read(bye)");*/
}

BOOL PSoundChannel::Write(const void *buf, PINDEX len)
{
    if(!len) return TRUE;
//    PTRACE(9, "PSoundChannel::Write");
    if(m_Direction==Recorder)
    {
//        PTRACE(0, "Tried to PSoundChannel::Write from a Recorder channel.");
        return FALSE;
    }
    PThread::Current()->Sleep(30);
    return TRUE;
}

BOOL PSoundChannel::Close()
{
    PTRACE(2, "PSoundChannel::Close");
    if(m_Direction==Recorder)
    {
    } else {
    }
    return TRUE;
}

BOOL PSoundChannel::Abort()
{
    return FALSE;
}

BOOL PSoundChannel::SetBuffers(PINDEX size, PINDEX count)
{
//    PTRACE(1, "PSoundChannel::SetBuffers("<<size<<","<<count<<")");
    return TRUE;
}

BOOL PSoundChannel::GetBuffers(PINDEX &size, PINDEX &count)
{
//    PTRACE(1, "PSoundChannel::GetBuffers");
    return TRUE;
}

BOOL PSoundChannel::SetVolume(int newVal)
{
  cerr << __FILE__ << "PSoundChannel :: SetVolume called in error. Please fix"<<endl;
  return FALSE;
}

BOOL  PSoundChannel::GetVolume(int &devVol)
{
 cerr << __FILE__ << "PSoundChannel :: GetVolume called in error. Please fix"<<endl;
  return FALSE;
}

