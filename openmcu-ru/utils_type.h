
#include "precompile.h"

#ifndef _MCU_UTILS_TYPE_H
#define _MCU_UTILS_TYPE_H

////////////////////////////////////////////////////////////////////////////////////////////////////

class OpenMCU;
class OpenMCUMonitor;

class MCUH323EndPoint;
class MCUSipEndPoint;

class MCUH323Connection;
class MCUSipConnnection;
class SipRTP_UDP;

class CacheRTP;

class MCUSimpleVideoMixer;
class VideoMixPosition;
class VideoFrameStore;
class MCUBufferYUVArray;
struct MCUSubtitles;

class ConferenceAudioConnection;
class ConferenceProfile;
class ConferenceMember;
class ConferenceRecorder;
class Conference;
class ConferenceManager;

class MCUConnection_ConferenceMember;
class MCUPVideoInputDevice;
class MCUPVideoOutputDevice;

class GatekeeperMonitor;
class ConnectionMonitor;
class ConnectionMonitorInfo;
class ConferenceMonitorInfo;

class RegistrarAccount;
class RegistrarConnection;
class RegistrarSubscription;
class AbookAccount;

class MCU_RTPChannel;
class MCUSIP_RTPChannel;
class MCUH323_RTPChannel;
class MCU_RTP_UDP;
class MCUSIP_RTP_UDP;

class MCUSocket;
class MCUListener;
class MCUTelnetSession;
class MCUJSON;

////////////////////////////////////////////////////////////////////////////////////////////////////

typedef long ConferenceMemberId;

////////////////////////////////////////////////////////////////////////////////////////////////////

enum Directions
{
  DIRECTION_INBOUND = 0,
  DIRECTION_OUTBOUND = 1
};

////////////////////////////////////////////////////////////////////////////////////////////////////

enum MCUConnectionTypes
{
  CONNECTION_TYPE_NONE,
  CONNECTION_TYPE_H323,
  CONNECTION_TYPE_SIP,
  CONNECTION_TYPE_RTSP
};

#define MCU_STRINGIFY(s) MCU_TOSTRING(s)
#define MCU_TOSTRING(s)  #s

#ifdef _WIN32
  #define setenv(n,v,f) _putenv(n "=" v)
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef _WIN32
#define sync_bool long
#define sync_val_compare_and_swap(ptr, oldval, newval) InterlockedCompareExchange(ptr, newval, oldval)
inline bool sync_bool_compare_and_swap(volatile long *ptr, long oldval, long newval)
{
  return sync_val_compare_and_swap(ptr, oldval, newval) == oldval;
}
#define sync_fetch_and_add(value, addvalue) InterlockedExchangeAdd(value, addvalue)
#define sync_fetch_and_sub(value, subvalue) InterlockedExchangeAdd(value, subvalue*(-1))
#define sync_increment(value) InterlockedIncrement(value)
#define sync_decrement(value) InterlockedDecrement(value)
#else
#define sync_bool bool
// returns the contents of *ptr before the operation
#define sync_val_compare_and_swap(ptr, oldval, newval) __sync_val_compare_and_swap(ptr, oldval, newval)
// returns true if the comparison is successful and newval was written
#define sync_bool_compare_and_swap(ptr, oldval, newval) __sync_bool_compare_and_swap(ptr, oldval, newval)
#define sync_fetch_and_add(value, addvalue) __sync_fetch_and_add(value, addvalue);
#define sync_fetch_and_sub(value, subvalue) __sync_fetch_and_sub(value, subvalue)
#define sync_increment(value) __sync_fetch_and_add(value, 1)
#define sync_decrement(value) __sync_fetch_and_sub(value, 1)
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////

#define MCUTRACE(level, args) \
  if(PTrace::CanTrace(level)) \
  { \
    if(level > 0) PTrace::Begin(level, __FILE__, __LINE__) << args << PTrace::End; \
    cout << setw(8) << PTime() - PProcess::Current().GetStartTime() << " " << args << endl; \
  }

#define MCUTRACE_IF(level, cond, args) \
  if(PTrace::CanTrace(level) && (cond)) MCUTRACE(level, args);

////////////////////////////////////////////////////////////////////////////////////////////////////

class MCUBuffer
{
  public:
    MCUBuffer(int newsize = 2048, bool _aligned = true)
    {
      aligned = _aligned;

      if(newsize < 0)
        newsize = 0;

      size = newsize;
      buffer = (uint8_t *)aligned_malloc(size);
    }

    ~MCUBuffer()
    {
      aligned_free(buffer);
    }

    int GetSize()
    {
      return size;
    }

    void SetSize(int newsize)
    {
      if(newsize <= size) // quick check before lock
        return;
      size = newsize;
      aligned_free(buffer);
      buffer = (uint8_t *)aligned_malloc(size);
    }

    uint8_t * GetPointer()
    {
      return buffer;
    }

    static void * aligned_malloc(int size)
    {
      if(size <= 0)
        return NULL;

      void *ptr = NULL;
#if HAVE_POSIX_MEMALIGN
      if(posix_memalign(&ptr, 64, size))
        ptr = NULL;
#else
      ptr = malloc(size);
#endif
      return ptr;
    }

    static void aligned_free(void *ptr)
    {
      free(ptr);
    }

  protected:
    int size;
    bool aligned;
    uint8_t *buffer;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class MCUTime
{
  public:
    MCUTime()
    {
      timestamp = GetRealTimestampUsec();
    }

    MCUTime(const uint64_t & _timestamp)
    {
      timestamp = _timestamp;
    }

    const uint64_t GetTimestamp() const
    {
      return timestamp;
    }

    const uint64_t GetMilliSeconds() const
    {
      return timestamp/1000;
    }

    const uint32_t GetSeconds() const
    {
#     ifdef _WIN32
        return (const uint32_t)(timestamp/1000000);
#     else
        return timestamp/1000000;
#     endif
    }

    operator uint64_t()
    {
      return timestamp;
    }

    static void Sleep(uint32_t interval_msec)
    {
#ifdef _WIN32
      ::Sleep(interval_msec);
#else
      SleepUsec(interval_msec*1000);
#endif
    }

    static void SleepUsec(uint32_t interval_usec)
    {
#     ifdef _WIN32
        uint32_t ms=interval_usec / 1000;
        uint32_t us=interval_usec % 1000;
        if(ms) ::Sleep(ms);
        if(us)
        {
          __int64 time1=0, time2=0, freq=0;
          QueryPerformanceCounter((LARGE_INTEGER*)&time1);
          QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
          do { QueryPerformanceCounter((LARGE_INTEGER*)&time2); } while((time2-time1) < us);
        }
#     else
        struct timespec req;
        req.tv_sec = interval_usec/1000000;
        req.tv_nsec = (interval_usec % 1000000) * 1000;
        while(nanosleep(&req, &req) == -1 && errno == EINTR)
          ;
#     endif
    }

    static uint64_t GetRealTimestampUsec()
    {
#ifdef _WIN32
      return PTime().GetTimestamp();
#else
      struct timespec ts;
      clock_gettime(CLOCK_REALTIME, &ts);
      return ts.tv_sec*1000000ULL + ts.tv_nsec/1000;
#endif
    }

    static uint64_t GetMonoTimestampUsec()
    {
#ifdef _WIN32
      return PTime().GetTimestamp();
#else
      struct timespec ts;
      clock_gettime(CLOCK_MONOTONIC, &ts);
      return ts.tv_sec*1000000ULL + ts.tv_nsec/1000;
#endif
    }

    static uint64_t GetProcTimestampUsec()
    {
#ifdef _WIN32
      return PTime().GetTimestamp();
#else
      struct timespec ts;
      clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts);
      return ts.tv_sec*1000000ULL + ts.tv_nsec/1000;
#endif
    }

  protected:
    uint64_t timestamp;

};

////////////////////////////////////////////////////////////////////////////////////////////////////

class MCUDelay
{
  public:
    MCUDelay()
    {
      Restart();
    }

    void Restart()
    {
      delay_time = MCUTime::GetMonoTimestampUsec();
      PTRACE(6, "MCUDelay " << this << " now " << delay_time);
    }

    void Delay(uint32_t delay_msec)
    {
      DelayUsec(delay_msec * 1000);
    }

    void DelayUsec(uint32_t delay_usec)
    {
      delay_time += delay_usec;
      now = MCUTime::GetMonoTimestampUsec();
      if(now < delay_time)
      {
        uint32_t interval = (uint32_t)(delay_time - now);
        MCUTime::SleepUsec(interval);
      }
      //else // restart
      //  delay_time = now;
    }

    // Для канала чтения RTP, последний timestamp или перезапуск
    const uint64_t GetDelayTimestampUsec(uint32_t delay_usec, uint32_t jitter_usec = 0)
    {
      now = MCUTime::GetMonoTimestampUsec();
      if(now > delay_time + delay_usec + jitter_usec)
      {
        PTRACE(6, "MCUDelay " << this << " now " << now << " before " << delay_time << " , jitter " << jitter_usec);
        delay_time = now;
      }
      return delay_time;
    }

    // Для канала записи RTP, последний timestamp
    const uint64_t GetDelayTimestampUsec()
    { return delay_time; }

  protected:
    uint64_t delay_time;
    uint64_t now;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class MCUReadWriteMutex : public PObject
{
  public:
    MCUReadWriteMutex(unsigned max = 64)
      : reader(max, max) { }
    void ReadWait()
    {
      reader.Wait();
    }
    void ReadSignal()
    {
      reader.Signal();
    }
    void WriteWait()
    {
      writeMutex.Wait();
#ifdef _WIN32
      for(unsigned i = 0; i < reader.GetMaxCountVal(); ++i)
#else
      for(unsigned i = 0; i < reader.GetMaxCount(); ++i)
#endif
        reader.Wait();
    }
    void WriteSignal()
    {
#ifdef _WIN32
      for(unsigned i = 0; i < reader.GetMaxCountVal(); ++i)
#else
      for(unsigned i = 0; i < reader.GetMaxCount(); ++i)
#endif
        reader.Signal();
      writeMutex.Signal();
    }
  protected:
    PSemaphore reader;
    PMutex writeMutex;
};
class MCUReadWaitAndSignal
{
  public:
    MCUReadWaitAndSignal(MCUReadWriteMutex & _mutex)
      : mutex(_mutex)
    { mutex.ReadWait(); }
    ~MCUReadWaitAndSignal()
    { mutex.ReadSignal(); }
  protected:
    MCUReadWriteMutex & mutex;
};
class MCUWriteWaitAndSignal
{
  public:
    MCUWriteWaitAndSignal(MCUReadWriteMutex & _mutex)
      : mutex(_mutex)
    { mutex.WriteWait(); }
    ~MCUWriteWaitAndSignal()
    { mutex.WriteSignal(); }
  protected:
    MCUReadWriteMutex & mutex;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class MCUConfig: public PConfig
{
 public:
   MCUConfig()
    : PConfig(CONFIG_PATH, "Options") { };
   MCUConfig(const PString & section)
    : PConfig(CONFIG_PATH, section) { };

   static PStringList GetSectionsPrefix(PString prefix)
   {
     MCUConfig cfg;
     PStringList sect = cfg.GetSections();
     for(PINDEX i = 0; i < sect.GetSize(); )
     {
       if(sect[i].Left(prefix.GetLength()) != prefix)
         sect.RemoveAt(i);
       else
        i++;
     }
     return sect;
   }
   static BOOL HasSection(PString section)
   {
     MCUConfig cfg;
     PStringList sect = cfg.GetSections();
     if(sect.GetStringsIndex(section) != P_MAX_INDEX)
       return TRUE;
     return FALSE;
   }
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class MCUURL : public PURL
{
  public:
    MCUURL() { }
    MCUURL(const PString & str);

    BOOL Parse(const PString & str);

    const PString & GetUserName() const
    { return username; }

    const PString & GetHostName() const
    { return hostname; }

    void SetDisplayName(const PString & name)
    { display_name = name; }

    const PString & GetDisplayName() const
    { return display_name; }

    const PString GetPort() const
    { return PString(port); }

    void SetTransport(const PString & _transport)
    { transport = _transport; }

    const PString & GetTransport() const
    { return transport; }

    const PString GetMemberName()
    {
      PString memberName;
      if(display_name != "")
        memberName += display_name+" ";
      memberName += "["+GetUrl()+"]";
      memberName += memberNameSuffix;
      return memberName;
    }

    const PString GetMemberNameId()
    {
      if(url_scheme == "rtsp")
        return GetUrl();
      PString id = url_scheme+":";
      if(username != "") id += username;
      else               id += hostname;
      return id;
    }

    const PString & GetUrl()
    {
      if(url_scheme == "h323" || url_scheme == "sip")
      {
        url_party = url_scheme+":"+username+"@"+hostname;
        if(port != 0)
          url_party += ":"+PString(port);
        if(transport != "" && transport != "*")
          url_party += ";transport="+transport;
      }
      else if(url_scheme == "rtsp")
      {
        url_party = url_scheme+"://";
        if(username != "")
        {
          url_party += username;
          if(password != "")
            url_party += ":"+password;
          url_party += "@";
        }
        url_party += hostname;
        if(port != 0)
          url_party += ":"+PString(port);
        url_party += pathStr;
      }
      return url_party;
    }

    const PString & AsString()
    { return GetUrl(); }

  protected:
    PString display_name;
    PString url_scheme;
    PString url_party;
    PString transport;
    PString memberNameSuffix;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // _MCU_UTILS_TYPE_H
