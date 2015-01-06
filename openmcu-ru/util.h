
#ifndef _MCU_UTIL_H
#define _MCU_UTIL_H

#include "config.h"

#include <algorithm>
#include <typeinfo>
#include <sofia-sip/msg_types.h>

extern "C"
{
  #include "libavcodec/avcodec.h"
  #include "libavformat/avformat.h"
  #include "libavutil/avutil.h"
  #include "libavutil/opt.h"
  #include "libavutil/mem.h"
  #include "libavutil/mathematics.h"
  #include "libavutil/audioconvert.h"
#if USE_SWRESAMPLE
  #include <libswresample/swresample.h>
#elif USE_AVRESAMPLE
  #include <libavresample/avresample.h>
  #include <libavutil/samplefmt.h>
#elif USE_LIBSAMPLERATE
  #include <samplerate.h>
#endif
}

#ifdef _WIN32
#pragma warning(disable:4786)
#pragma warning(disable:4100)
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////

class OpenMCU;
class OpenMCUMonitor;

class MCUH323EndPoint;
class MCUSipEndPoint;

class MCUH323Connection;
class MCUSipConnnection;
class SipRTP_UDP;

class MCUSimpleVideoMixer;

class ConferenceAudioConnection;
class ConferenceProfile;
class ConferenceMember;
class ConferenceRecorder;
class Conference;
class ConferenceManager;

class H323Connection_ConferenceMember;
class MCUPVideoInputDevice;
class MCUPVideoOutputDevice;

class GatekeeperMonitor;
class ConnectionMonitor;
class ConnectionMonitorInfo;
class ConferenceMonitorInfo;

class RegistrarAccount;
class RegistrarConnection;
class RegistrarSubscription;

class MCU_RTPChannel;
class MCUSIP_RTPChannel;
class MCUH323_RTPChannel;
class MCU_RTP_UDP;
class MCUSIP_RTP_UDP;

////////////////////////////////////////////////////////////////////////////////////////////////////

typedef void * ConferenceMemberId;

////////////////////////////////////////////////////////////////////////////////////////////////////

// avcodecMutex - используется при создании/удалении кодеков
// insufficient thread locking around avcodec_open/close()
extern PMutex avcodecMutex;

// cacheRTPListMutex - используется при создании кэшей
// предотвращает создание в списке двух одноименных кэшей
extern PMutex cacheRTPListMutex;

////////////////////////////////////////////////////////////////////////////////////////////////////

const uint64_t MCU_AV_CH_Layout_Selector[] =
{
  0
  ,AV_CH_LAYOUT_MONO
  ,AV_CH_LAYOUT_STEREO
  ,AV_CH_LAYOUT_2_1
  ,AV_CH_LAYOUT_3POINT1
  ,AV_CH_LAYOUT_5POINT0
  ,AV_CH_LAYOUT_5POINT1
  ,AV_CH_LAYOUT_7POINT0
  ,AV_CH_LAYOUT_7POINT1
};

////////////////////////////////////////////////////////////////////////////////////////////////////

enum MCUConnectionTypes
{
  CONNECTION_TYPE_NONE,
  CONNECTION_TYPE_H323,
  CONNECTION_TYPE_SIP,
  CONNECTION_TYPE_RTSP
};

////////////////////////////////////////////////////////////////////////////////////////////////////

#define MCUTRACE(level, args) \
  if(level > 0) PTrace::Begin(level, __FILE__, __LINE__) << args << PTrace::End; \
  if(PTrace::CanTrace(level)) cout << setw(8) << PTime() - PProcess::Current().GetStartTime() << " " << args << endl

////////////////////////////////////////////////////////////////////////////////////////////////////

PString convert_cp1251_to_utf8(PString str);
PString convert_ucs2_to_utf8(PString str);

PString GetSectionParamFromUrl(PString param, PString url, bool asterisk = true);
PString GetSectionParamFromUrl(PString param, PString url, PString defaultValue, bool asterisk = true);

PString GetConferenceParam(PString room, PString param, PString defaultValue);
int GetConferenceParam(PString room, PString param, int defaultValue);

char * PStringToChar(PString str);

int PTimedMutexTryLock(PTimedMutex & mutex, const PTimeInterval & timeout, PString info="");

//BOOL CreateCustomVideoCache(PString requestedRoom, H323Capability *cap);

BOOL CheckCapability(const PString & formatName);
BOOL SkipCapability(const PString & formatName, MCUConnectionTypes connectionType = CONNECTION_TYPE_NONE);

PString GetPluginName(const PString & format);

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
      return timestamp/1000LL;
    }

    const uint32_t GetSeconds() const
    {
      return timestamp/1000000LL;
    }

    operator uint64_t()
    {
      return timestamp;
    }

    static uint64_t GetRealTimestampUsec()
    {
#ifdef _WIN32
      PTimeInterval interval = PTimer::Tick();
      return interval.GetMilliSeconds()*1000LL;
#else
      struct timespec ts;
      clock_gettime(CLOCK_REALTIME, &ts);
      return ts.tv_sec*1000000L + ts.tv_nsec/1000L;
#endif
    }

    static uint64_t GetMonoTimestampUsec()
    {
#ifdef _WIN32
      PTimeInterval interval = PTimer::Tick();
      return interval.GetMilliSeconds()*1000LL;
#else
      struct timespec ts;
      clock_gettime(CLOCK_MONOTONIC, &ts);
      return ts.tv_sec*1000000L + ts.tv_nsec/1000L;
#endif
    }

    static uint64_t GetProcTimestampUsec()
    {
#ifdef _WIN32
      PTimeInterval interval = PTimer::Tick();
      return interval.GetMilliSeconds()*1000LL;
#else
      struct timespec ts;
      clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts);
      return ts.tv_sec*1000000L + ts.tv_nsec/1000L;
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
    }

    void DelayMsec(uint32_t delay_msec)
    {
      DelayUsec(delay_msec * 1000L);
    }

    void DelayUsec(uint32_t delay_usec)
    {
      delay_time += delay_usec;
      now = MCUTime::GetMonoTimestampUsec();
      if(now < delay_time)
      {
        uint32_t interval = delay_time - now;
        struct timespec req;
        req.tv_sec = interval/1000000L;
        req.tv_nsec = (interval % 1000000L) * 1000L;
        while(nanosleep(&req, &req) == -1 && errno == EINTR)
          ;
      }
      //else // restart
      //  delay_time = now;
    }

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
    void ReadWait();
    void ReadSignal();
    void WriteWait();
    void WriteSignal();

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
    : PConfig() { };
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
    MCUURL();
    MCUURL(PString str);

    void SetDisplayName(PString name) { display_name = name; }

    virtual const PString & GetDisplayName() const { return display_name; }
    virtual const PString & GetUrl() const { return url_party; }
    virtual const PString GetPort() const { return PString(port); }
    virtual const PString & GetTransport() const { return transport; }
    virtual const PString GetMemberName() const { return display_name+" ["+url_party+"]"; }
    virtual const PString GetMemberNameId() const
    {
      PString id = url_scheme+":";
      if(username != "") id += username;
      else               id += hostname;
      return id;
    }

  protected:
    PString display_name;
    PString url_scheme;
    PString url_party;
    PString transport;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class MCUStringDictionary
{
  public:
    MCUStringDictionary(const PString & str="");
    MCUStringDictionary(const PString & str, const PString & _delim1, const PString & _delim2);

    void Parse(const PString & str);
    void SetKeyDelim(const PString & _delim1)
    { delim1 = _delim1; }
    void SetValueDelim(const PString & _delim2)
    { delim2 = _delim2; }

    PINDEX GetSize()
    { return keys.GetSize(); }

    void Append(PString name, const PString & value);
    void Remove(const PString & name);

    void SetValueAt(PINDEX index, const PString & value);
    PString GetKeyAt(PINDEX index);
    PString GetValueAt(PINDEX index);

    PString AsString(const PString & _delim1, const PString & _delim2);
    PString AsString()
    { return AsString(delim1, delim2); }

    PString operator()(const PString & key, const char *defvalue="") const;

  protected:
    PString delim1, delim2;
    PStringArray keys;
    PStringArray values;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

unsigned GetVideoMacroBlocks(unsigned width, unsigned height);
BOOL GetParamsH263(PString & mpiname, unsigned & width, unsigned & height);
BOOL GetParamsH264(unsigned & level, unsigned & level_h241, unsigned & max_fs);
BOOL GetParamsH264(unsigned & level, unsigned & level_h241, unsigned & max_fs, unsigned & max_mbps, unsigned & max_br);
BOOL GetParamsH264(unsigned & level, unsigned & level_h241, unsigned & max_fs, unsigned & max_mbps, unsigned & max_br, unsigned & width, unsigned & height);
BOOL GetParamsMpeg4(unsigned & profile_level, unsigned & profile, unsigned & level, unsigned & max_fs);
BOOL GetParamsMpeg4(unsigned & profile_level, unsigned & profile, unsigned & level, unsigned & max_fs, unsigned & width, unsigned & height);

void SetFormatParams(OpalMediaFormat & wf, unsigned width, unsigned height);
void SetFormatParams(OpalMediaFormat & wf, unsigned width, unsigned height, unsigned frame_rate, unsigned bandwidth);
void SetFormatParamsH261(OpalMediaFormat & wf, unsigned width, unsigned height);
void SetFormatParamsH263(OpalMediaFormat & wf, unsigned width, unsigned height);
void SetFormatParamsH264(OpalMediaFormat & wf, unsigned width, unsigned height);
void SetFormatParamsMPEG4(OpalMediaFormat & wf, unsigned width, unsigned height);
void SetFormatParamsVP8(OpalMediaFormat & wf, unsigned width, unsigned height);

static struct vp8_resolution {
  const char *capname;
  unsigned width;
  unsigned height;
} const vp8_resolutions[] = {
  { "VP8-QCIF",  176,  144  },
  { NULL,        320,  240  },
  { "VP8-CIF",   352,  288  },
  { "VP8-360P",  640,  360  },
  { NULL,        640,  480  },
  { "VP8-4CIF",  704,  576  },
  { NULL,        800,  600  },
  { "VP8-480P",  852,  480  },
  { NULL,        1024, 768  },
  { "VP8-720P",  1280, 720  },
  { NULL,        1280, 1024 },
  { "VP8-768P",  1364, 768  },
  { "VP8-1080P", 1920, 1080 },
  { NULL }
};

static struct h263_resolution {
  const char *mpiname;
  unsigned width;
  unsigned height;
} const h263_resolutions[] = {
  { "SQCIF", 128,  96   },
  { "QCIF",  176,  144  },
  { "CIF",   352,  288  },
  { "CIF4",  704,  576  },
  { "CIF16", 1408, 1152 },
  { NULL }
};

static struct mpeg4_profile_level {
  unsigned profile_level;
  const char* profile_name;
  unsigned profile;
  unsigned level;
  unsigned max_fs;
  unsigned max_br;
  unsigned width;
  unsigned height;
} const mpeg4_profile_levels[] = {
  {   1, "Simple",                     1, 1, 99,    64000,    176,  144  },
  {   2, "Simple",                     1, 2, 792,   128000,   352,  288  },
  {   3, "Simple",                     1, 3, 792,   384000,   352,  288  },
  {   4, "Simple",                     1, 4, 1200,  4000000,  352,  288  },
  {   5, "Simple",                     1, 5, 1620,  8000000,  704,  576  },
  {   8, "Simple",                     1, 0, 99,    64000,    176,  144  },
  {   9, "Simple",                     1, 0, 99,    128000,   176,  144  },
//  {  17, "Simple Scalable",            2, 1, 495,   128000,   0,    0    },
//  {  18, "Simple Scalable",            2, 2, 792,   256000,   0,    0    },
//  {  33, "Core",                       3, 1, 198,   384000,   0,    0    },
//  {  34, "Core",                       3, 2, 792,   2000000,  0,    0    },
//  {  50, "Main",                       4, 2, 1188,  2000000,  0,    0    },
//  {  51, "Main",                       4, 3, 3240,  15000000, 0,    0    },
//  {  52, "Main",                       4, 4, 16320, 38400000, 0,    0    },
//  {  66, "N-Bit",                      5, 2, 792,   2000000,  0,    0    },
  { 145, "Advanced Real Time Simple",  6, 1, 99,    64000,    176,  144  },
  { 146, "Advanced Real Time Simple",  6, 2, 396,   128000,   352,  288  },
  { 147, "Advanced Real Time Simple",  6, 3, 396,   384000,   352,  288  },
  { 148, "Advanced Real Time Simple",  6, 4, 396,   2000000,  352,  288  },
//  { 161, "Core Scalable",              7, 1, 792,   768000,   0,    0    },
//  { 162, "Core Scalable",              7, 2, 990,   1500000,  0,    0    },
//  { 163, "Core Scalable",              7, 3, 4032,  4000000,  0,    0    },
//  { 177, "Advanced Coding Efficiency", 8, 1, 792,   384000,   0,    0    },
//  { 178, "Advanced Coding Efficiency", 8, 2, 1188,  2000000,  0,    0    },
//  { 179, "Advanced Coding Efficiency", 8, 3, 3240,  15000000, 0,    0    },
//  { 180, "Advanced Coding Efficiency", 8, 4, 16320, 38400000, 0,    0    },
//  { 193, "Advanced Core",              9, 1, 198,   384000,   0,    0    },
//  { 194, "Advanced Core",              9, 2, 792,   2000000,  0,    0    },
  { 240, "Advanced Simple",           10, 0, 99,    128000,   176,  144  },
  { 241, "Advanced Simple",           10, 1, 99,    128000,   176,  144  },
  { 242, "Advanced Simple",           10, 2, 396,   384000,   352,  288  },
  { 243, "Advanced Simple",           10, 3, 396,   768000,   352,  288  },
  { 244, "Advanced Simple",           10, 4, 792,   3000000,  352,  288  },
  { 245, "Advanced Simple",           10, 5, 1620,  8000000,  704,  576  },
  { 0 }
};

static struct h264_profile_level {
  const char *capname;
  unsigned level;
  unsigned level_h241;
  unsigned max_fs;
  unsigned max_mbps;
  unsigned max_br;
} const h264_profile_levels[] = {
  { "H.264-SQCIF",  10, 15,  99,    1485,   64000     },
  { "H.264-QCIF1",  11, 19,  99,    1485,   128000    },
  { "H.264-QCIF2",  11, 22,  396,   3000,   192000    },
  { "H.264-CIF1",   12, 29,  396,   6000,   384000    },
  { "H.264-CIF2",   13, 36,  396,   11880,  768000    },
  { "H.264-CIF3",   20, 43,  396,   11880,  2000000   },
  { "H.264-4CIF1",  21, 50,  792,   19800,  4000000   },
  { "H.264-4CIF2",  22, 57,  1620,  20250,  4000000   },
  { "H.264-4CIF3",  30, 64,  1620,  40500,  10000000  },
  { "H.264-SD",     31, 71,  3600,  108000, 14000000  },
  { "H.264-HD",     32, 78,  5120,  216000, 20000000  },
  { "H.264-FHD",    40, 85,  8192,  245760, 25000000  },
//  { NULL,           41, 92,  8192,  245760, 62500000  },
//  { NULL,           42, 99,  8704,  522340, 62500000  },
//  { NULL,           50, 106, 22080, 589824, 135000000 },
//  { NULL,           51, 113, 36864, 983040, 240000000 },
  { NULL }
};

const static struct h264_resolution {
    unsigned width;
    unsigned height;
    unsigned macroblocks;
} h264_resolutions[] = {
//  { 7680, 4320,129600}, // 16:9 Super Hi-Vision 33,17 mp
//  { 2560, 1440,14400 }, // 16:9 WQXGA            3,68 mp
//  { 2048, 1152, 9216 }, // 16:9 QWXGA            2,36 mp
//  { 1920, 1088, 8160 }, // ???                   2,08 mp
  { 1920, 1080, 8100 }, // 16:9 Full HD 1080P    2,07 mp
//  { 1600, 1200, 7500 }, //  4:3 UXGA             1,92 mp
  { 1408, 1152, 6336 }, // 11:9 CIF16            1,62 mp
//  { 1600,  900, 5625 }, // 16:9 WXGA++           1,44 mp
//  { 1280, 1024, 5120 }, //  5:4 SXGA             1,31 mp
//  { 1232, 1008, 4851 }, // 11:9                  1,24 mp
  { 1366,  768, 4098 }, // 16:9 WXGA             1,04 mp
  { 1280,  720, 3600 }, // 11:9 HD 720P          0,92 mp
//  { 1056,  864, 3564 }, // 11:9 TCIF             0,91 mp
//  { 1024,  768, 3072 }, //  4:3 XGA              0,78 mp
  {  960,  540, 2025 }, // 16:9 qHD              0,51 mp
//  {  800,  600, 1900 }, //  4:3 SVGA             0,48 mp
//  {  854,  480, 1601 }, // 16:9 FWVGA            0,40 mp
  {  704,  576, 1584 }, // 11:9 CIF4             0,40 mp
//  {  640,  480, 1200 }, //  4:3 VGA              0,30 mp
//  {  640,  360,  900 }, // 16:9 nHD              0,23 mp
  {  352,  288,  396 }, // 11:9 CIF              0,10 mp
//  {  320,  240,  300 }, //  4:3 QVGA             0,07 mp
  {  176,  144,   99 }, // 11:9 QCIF             0,02 mp
  {  128,   96,   48 }, //  4:3                  0,01 mp
  { 0 }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef _WIN32
#define sync_val_compare_and_swap(ptr, oldval, newval) InterlockedCompareExchange(ptr, newval, oldval)
//inline bool sync_bool_compare_and_swap(bool *ptr, bool oldval, bool newval)
//{
//  if(InterlockedCompareExchange(ptr, newval, oldval) == oldval)
//    return true;
//  else
//    return false;
//}
#define sync_fetch_and_add(value, addvalue) InterlockedExchangeAdd(value, addvalue)
#define sync_fetch_and_sub(value, subvalue) InterlockedExchangeAdd(value, subvalue*(-1))
#define sync_increment(value) InterlockedIncrement(value)
#define sync_decrement(value) InterlockedDecrement(value)
#else
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
//
// "Умный" итератор
//  - захватывает объект в конструкторе
//  - освобождает текущий объект и захватывает новый при изменении операторами
//  - освобождает в деструкторе
//
template <class _T1> class MCUSharedList;

template <class T1, class T2>
class MCUSharedListSharedIterator
{
    template <class _T1> friend class MCUSharedList;
    typedef MCUSharedListSharedIterator<T1, T2> shared_iterator;

    // Только для внутреннего пользования
    MCUSharedListSharedIterator(T1 * _list, int _index, bool _captured)
      : list(_list), index(_index), captured(_captured)
    {
      if(captured == false)
        Capture();
    }

  public:

    // Пустой итератор
    MCUSharedListSharedIterator()
      : list(NULL), index(-1), captured(false)
    { }

    // Итератор n-го доступного объекта в списке
    MCUSharedListSharedIterator(T1 * _list, int _number)
      : list(_list), index(-1), captured(false)
    {
      IteratorIncrement(_number);
    }

    MCUSharedListSharedIterator(const shared_iterator & it)
      : list(it.list), index(it.index), captured(false)
    {
      Capture();
    }

    ~MCUSharedListSharedIterator()
    {
      Release();
    }

    int GetIndex()
    { return index; }

    long GetID()
    {
      if(captured)
        return list->ids[index];
      return -1;
    }

    // Возвращает текущий объект итератора или NULL
    T2 * GetObject()
    {
      if(captured)
        return list->objs[index];
      return NULL;
    }

    // Возвращает повторно захваченный объект.
    // Объект останется захваченным после изменения/уничтожения итератора,
    // использовать например в функциях поиска объекта FindWithLock()
    T2 * GetCapturedObject()
    {
      if(captured)
      {
        list->CaptureInternal(index);
        return list->objs[index];
      }
      return NULL;
    }

    T2 * operator -> ()
    { return GetObject(); }

    T2 * operator * ()
    { return GetObject(); }

    MCUSharedListSharedIterator & operator = (const shared_iterator & it)
    {
      Release();
      list = it.list;
      index = it.index;
      Capture();
      return *this;
    }

    MCUSharedListSharedIterator & operator ++ ()
    {
      Release();
      IteratorIncrement();
      return *this;
    }

    bool operator == (const shared_iterator & it)
    { return (index == it.index); }

    bool operator != (const shared_iterator & it)
    { return (index != it.index); }

    // После Release() объект недоступен в итераторе
    void Release()
    {
      if(captured)
      {
        captured = false;
        list->ReleaseInternal(index);
      }
    }

  protected:

    // Только для внутреннего пользования
    void Capture()
    {
      if(!captured && list && index >= 0 && index < list->size)
      {
        captured = true;
        list->CaptureInternal(index);
      }
    }

    // Только для внутреннего пользования
    bool IteratorIncrement(int _number = 0)
    {
      if(list == NULL)
        return false;
      ++index;
      if(index < 0 || index >= list->size)
      {
        index = list->size;
        return false;
      }
      int i = -1;
      for(bool *it = find(list->states + index, list->states_end, true); it != list->states_end; it = find(++it, list->states_end, true))
      {
        if(++i < _number)
          continue;
        index = it - list->states;
        Capture();
        // повторная проверка после захвата
        if(list->states[index] == true)
          return true;
        // освободить если нет объекта
        Release();
      }
      index = list->size;
      return false;
    }

    T1 * list;
    int index;
    bool captured;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
class MCUSharedList
{
    template<class T1, class T2> friend class MCUSharedListSharedIterator;

  public:
    MCUSharedList(int _size = 256);
    ~MCUSharedList();

    // В новом итераторе объект захвачен
    // Каждый новый клон итератора делает еще один захват
    typedef MCUSharedListSharedIterator<MCUSharedList, T> shared_iterator;

    // begin() - первый доступный объект
    shared_iterator begin() { return shared_iterator(this, 0); }
    // end() - постоянный index = size
    const shared_iterator & end() const { return iterator_end; }

    // Максимальный размер списка
    int GetSize()
    { return size; }

    // Количество доступных объектов
    int GetCurrentSize()
    { return (int)current_size; }

    // Уникальный идентификатор для объектов
    long GetNextID()
    { return sync_increment(&idcounter); }

    // Insert вохвращает false если нет свободного места
    // Insert() - после добавления объект захвачен
    bool Insert(long id, T * obj, PString name = "");

    // Erase возвращает false если объекта нет в списке или Erase выполняется другим потоком
    // Обязательно проверять результат выполнения перед удалением объекта!
    // Erase(id) - перед Erase освободить(если захвачен)
    bool Erase(long id);
    // Erase(iterator)
    bool Erase(shared_iterator & it);

    // Release() - освободить объект
    void Release(long id);

    // Find() возвращают захваченный объект
    // Освобождать функцией iterator.Release()
    shared_iterator Find(long id);
    shared_iterator Find(const PString & name);
    shared_iterator Find(T * obj);

    // Операторы возвращают захваченный объект
    // Освобождать функцией list.Release(id)
    T * operator[] (int index);
    T * operator() (long id);
    T * operator() (const PString & name);
    T * operator() (T * obj);

  protected:
    void CaptureInternal(int index);
    void ReleaseInternal(int index);
    void ReleaseWait(long * _captures, int threshold);

    int size;
    long current_size;
    long idcounter;
    bool * volatile states;
    bool * states_end;
    long * volatile ids;
    long * ids_end;
    PString * names;
    PString * names_end;
    T ** volatile objs;
    T ** objs_end;
    long * volatile captures;
    bool * volatile locks;
    shared_iterator iterator_end;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
MCUSharedList<T>::MCUSharedList(int _size)
  : iterator_end(NULL, _size, false)
{
  size = _size;
  current_size = 0;
  idcounter = 0;
  states = new bool [size];
  states_end = states + size;
  ids = new long [size];
  ids_end = ids + size;
  names = new PString [size];
  names_end = names + size;
  objs = new T * [size];
  objs_end = objs + size;
  captures = new long [size];
  locks = new bool [size];
  for(int i = 0; i < size; ++i)
  {
    states[i] = false;
    ids[i] = -1;
    objs[i] = NULL;
    captures[i] = 0;
    locks[i] = false;
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
MCUSharedList<T>::~MCUSharedList()
{
  delete [] states;
  states = NULL;

  delete [] ids;
  ids = NULL;

  delete [] names;
  names = NULL;

  for(int i = 0; i < size; ++i)
    objs[i] = NULL;
  delete [] objs;
  objs = NULL;

  delete [] captures;
  captures = NULL;

  delete [] locks;
  locks = NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
bool MCUSharedList<T>::Insert(long id, T * obj, PString name)
{
  bool insert = false;
  for(bool *it = find(states, states_end, false); it != states_end; it = find(++it, states_end, false))
  {
    int index = it - states;
    // блокировка записи
    if(sync_bool_compare_and_swap(&locks[index], false, true) == true)
    {
      // повторная проверка после блокировки
      if(states[index] == false)
      {
        // захват
        CaptureInternal(index);
        // запись объекта
        objs[index] = obj;
        ids[index] = id;
        names[index] = name;
        // разрешить получение объекта
        states[index] = true;
        sync_increment(&current_size);
        insert = true;
      }
      // разблокировка записи
      sync_bool_compare_and_swap(&locks[index], true, false);
      if(insert)
        return true;
    }
  }
  return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
bool MCUSharedList<T>::Erase(long id)
{
  bool erase = false;
  long *it = find(ids, ids_end, id);
  if(it != ids_end)
  {
    int index = it - ids;
    if(ids[index] == id && states[index] == true )
    {
      // блокировка записи
      if(sync_bool_compare_and_swap(&locks[index], false, true) == true)
      {
        // повторная проверка после блокировки
        if(ids[index] == id && states[index] == true)
        {
          // запретить получение объекта
          states[index] = false;
          sync_decrement(&current_size);
          // ждать освобождения объекта
          ReleaseWait(&captures[index], 0);
          // запись объекта
          ids[index] = -1;
          names[index] = "";
          objs[index] = NULL;
          erase = true;
        }
        // разблокировка записи
        sync_bool_compare_and_swap(&locks[index], true, false);
      }
    }
  }
  return erase;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
bool MCUSharedList<T>::Erase(shared_iterator & it)
{
  // итератор должен быть захвачен
  if(it.captured == false)
    return false;

  int index = it.GetIndex();
  // пустой итератор
  if(index < 0 || index >= size)
    return false;

  bool erase = false;
  if(states[index] == true )
  {
    // блокировка записи
    if(sync_bool_compare_and_swap(&locks[index], false, true) == true)
    {
      // повторная проверка после блокировки
      if(states[index] == true)
      {
        // запретить получение объекта
        states[index] = false;
        sync_decrement(&current_size);
        // ждать освобождения объекта
        // 1 захват в итераторе
        ReleaseWait(&captures[index], 1);
        // запись объекта
        ids[index] = -1;
        names[index] = "";
        objs[index] = NULL;
        erase = true;
      }
      // разблокировка записи
      sync_bool_compare_and_swap(&locks[index], true, false);
    }
  }

  // освободить объект и
  // запретить получение объекта из итератора
  it.Release();

  return erase;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
void MCUSharedList<T>::ReleaseWait(long * _captures, int treshold)
{
  for(int i = 0; *_captures != treshold; ++i)
  {
    if(i < 100)
      __asm__ __volatile__("pause":::"memory");
    else if(i < 1000)
    {
      struct timespec req = {0};
      req.tv_nsec = 1000; // 1μs
      nanosleep(&req, NULL);
    }
    else
    {
      struct timespec req = {0};
      req.tv_nsec = 1000000; // 1ms
      nanosleep(&req, NULL);
    }
    if(i % 5000 == 0)
    {
      PTRACE(1, "ReleaseWait list: " << typeid(this).name() << ", captures: " << *_captures);
      cout << "ReleaseWait list: " << typeid(this).name() << ", captures: " << *_captures << "\n";
    }
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
void MCUSharedList<T>::Release(long id)
{
  long *it = find(ids, ids_end, id);
  if(it != ids_end)
  {
    int index = it - ids;
    ReleaseInternal(index);
  }
};

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
void MCUSharedList<T>::ReleaseInternal(int index)
{
  //PTRACE(6, "release index=" << index << " captures=" << captures[index] << " id=" << ids[index] << " obj=" << (objs[index] == NULL ? 0 : objs[index]) << " thread=" << PThread::Current() << " " << PThread::Current()->GetThreadName()<< "\ttype=" << typeid(objs[index]).name());
  sync_decrement(&captures[index]);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
void MCUSharedList<T>::CaptureInternal(int index)
{
  //PTRACE(6, "capture index=" << index << " captures=" << captures[index] << " id=" << ids[index] << " obj=" << (objs[index] == NULL ? 0 : objs[index]) << " thread=" << PThread::Current() << " " << PThread::Current()->GetThreadName()<< "\ttype=" << typeid(objs[index]).name());
  sync_increment(&captures[index]);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
MCUSharedListSharedIterator<MCUSharedList<T>, T> MCUSharedList<T>::Find(long id)
{
  long *it = find(ids, ids_end, id);
  if(it != ids_end)
  {
    int index = it - ids;
    CaptureInternal(index);
    // повторная проверка после захвата
    if(ids[index] == id && states[index] == true)
      return shared_iterator(this, index, true);
    // освободить если нет объекта
    ReleaseInternal(index);
  }
  return iterator_end;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
MCUSharedListSharedIterator<MCUSharedList<T>, T> MCUSharedList<T>::Find(const PString & name)
{
  PString *it = find(names, names_end, name);
  if(it != names_end)
  {
    int index = it - names;
    CaptureInternal(index);
    // повторная проверка после захвата
    if(names[index] == name && states[index] == true)
      return shared_iterator(this, index, true);
    // освободить если нет объекта
    ReleaseInternal(index);
  }
  return iterator_end;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
MCUSharedListSharedIterator<MCUSharedList<T>, T> MCUSharedList<T>::Find(T * obj)
{
  T **it = find(objs, objs_end, obj);
  if(it != objs_end)
  {
    int index = it - objs;
    CaptureInternal(index);
    // повторная проверка после захвата
    if(objs[index] == obj && states[index] == true)
      return shared_iterator(this, index, true);
    // освободить если нет объекта
    ReleaseInternal(index);
  }
  return iterator_end;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
T * MCUSharedList<T>::operator[] (int index)
{
  if(index < 0 || index >= size)
    return NULL;
  if(states[index] == true)
  {
    CaptureInternal(index);
    // повторная проверка после захвата
    if(states[index] == true)
      return objs[index];
    // освободить если нет объекта
    ReleaseInternal(index);
  }
  return NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
T * MCUSharedList<T>::operator() (long id)
{
  return Find(id).GetCapturedObject();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
T * MCUSharedList<T>::operator() (const PString & name)
{
  return Find(name).GetCapturedObject();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
T * MCUSharedList<T>::operator() (T * obj)
{
  return Find(obj).GetCapturedObject();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

typedef MCUSharedList<Conference> MCUConferenceList;
typedef MCUSharedList<ConferenceAudioConnection> MCUAudioConnectionList;
typedef MCUSharedList<MCUSimpleVideoMixer> MCUVideoMixerList;
typedef MCUSharedList<ConferenceMember> MCUMemberList;
typedef MCUSharedList<ConferenceProfile> MCUProfileList;

typedef MCUSharedList<RegistrarAccount> MCURegistrarAccountList;
typedef MCUSharedList<RegistrarConnection> MCURegistrarConnectionList;
typedef MCUSharedList<RegistrarSubscription> MCURegistrarSubscriptionList;

////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
class MCUQueueT
{
  public:
    MCUQueueT(int _size = 256)
      : queue(_size)
    {
      stopped = false;
    }
    ~MCUQueueT() { }

    virtual bool Push(T *obj)
    {
      if(stopped)
        return false;
      while(!queue.Insert((long)obj, obj))
        PThread::Sleep(20);
      queue.Release((long)obj);
      return true;
    }
    virtual T * Pop()
    {
      for(int i = 0; i < queue.GetSize(); ++i)
      {
        T *obj = queue[i];
        if(obj == NULL)
          continue;
        queue.Release((long)obj);
        queue.Erase((long)obj);
        return obj;
      }
      return NULL;
    }
    virtual void Stop()
    {
      stopped = true;
      PThread::Sleep(2);
    }

  protected:
    bool stopped;
    MCUSharedList<T> queue;
};

typedef MCUQueueT<PString> MCUQueuePString;
typedef MCUQueueT<msg_t> MCUQueueMsg;


////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // _MCU_UTIL_H
