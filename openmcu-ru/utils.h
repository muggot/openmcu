
#ifndef _MCU_UTILS_H
#define _MCU_UTILS_H

#include "config.h"
#include "utils_av.h"

#include <algorithm>
#include <typeinfo>
#include <sofia-sip/msg_types.h>

////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef _WIN32
#pragma warning(disable:4786)
#pragma warning(disable:4100)
#endif

#define MCU_STRINGIFY(s) MCU_TOSTRING(s)
#define MCU_TOSTRING(s)  #s

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

// avcodecMutex - используется при создании/удалении кодеков
// insufficient thread locking around avcodec_open/close()
extern PMutex avcodecMutex;

// cacheRTPListMutex - используется при создании кэшей
// предотвращает создание в списке двух одноименных кэшей
extern PMutex cacheRTPListMutex;

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
  if(level > 0 && PTrace::CanTrace(level)) PTrace::Begin(level, __FILE__, __LINE__) << args << PTrace::End; \
  if(PTrace::CanTrace(level)) cout << setw(8) << PTime() - PProcess::Current().GetStartTime() << " " << args << endl

////////////////////////////////////////////////////////////////////////////////////////////////////

PString convert_cp1251_to_utf8(PString str);
PString convert_ucs2_to_utf8(PString str);

PString GetSectionParamFromUrl(PString param, PString url, bool asterisk = true);
PString GetSectionParamFromUrl(PString param, PString url, PString defaultValue, bool asterisk = true);

PString GetConferenceParam(PString room, PString param, PString defaultValue);
int GetConferenceParam(PString room, PString param, int defaultValue);

char * PStringToChar(PString str);

//BOOL CreateCustomVideoCache(PString requestedRoom, H323Capability *cap);

BOOL CheckCapability(const PString & formatName);
BOOL SkipCapability(const PString & formatName, MCUConnectionTypes connectionType = CONNECTION_TYPE_NONE);

PString GetPluginName(const PString & format);

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

    MCUBuffer(PixelFormat pix_fmt, int width, int height)
    {
      aligned = true;
      size = avpicture_get_size(pix_fmt, width, height);
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
      if(newsize < 0)
        newsize = 0;

      if(newsize == size)
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
      return timestamp/1000000;
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
      // win32 что?
      struct timespec req;
      req.tv_sec = interval_usec/1000000;
      req.tv_nsec = (interval_usec % 1000000) * 1000;
      while(nanosleep(&req, &req) == -1 && errno == EINTR)
        ;
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
        uint32_t interval = delay_time - now;
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
template <class _T_obj> class MCUSharedList;

template <class T_list, class T_obj>
class MCUSharedListSharedIterator
{
    template <class _T_obj> friend class MCUSharedList;
    typedef MCUSharedListSharedIterator<T_list, T_obj> shared_iterator;

    // Только для внутреннего пользования
    MCUSharedListSharedIterator(T_list * _list, int _index, bool _captured)
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
    MCUSharedListSharedIterator(T_list * _list, int _number)
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

    PString GetName()
    {
      if(captured)
        return list->names[index];
      return "";
    }

    // Возвращает текущий объект итератора или NULL
    T_obj * GetObject()
    {
      if(captured)
        return list->objs[index];
      return NULL;
    }

    // Возвращает повторно захваченный объект.
    // Объект останется захваченным после изменения/уничтожения итератора,
    // использовать например в функциях поиска объекта FindWithLock()
    T_obj * GetCapturedObject()
    {
      if(captured)
      {
        list->CaptureInternal(index);
        return list->objs[index];
      }
      return NULL;
    }

    T_obj * operator -> ()
    { return GetObject(); }

    T_obj * operator * ()
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

    T_list * list;
    int index;
    bool captured;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T_obj>
class MCUSharedList
{
    template<class _T_list, class _T_obj> friend class MCUSharedListSharedIterator;

  public:
    MCUSharedList(int _size = 256);
    ~MCUSharedList();

    // В новом итераторе объект захвачен
    // Каждый новый клон итератора делает еще один захват
    typedef MCUSharedListSharedIterator<MCUSharedList, T_obj> shared_iterator;

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
    bool Insert(T_obj * obj, long id, PString name = "");

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
    shared_iterator Find(PString name);
    shared_iterator Find(T_obj * obj);

    // Операторы возвращают захваченный объект
    // Освобождать функцией list.Release(id)
    T_obj * operator[] (int index);
    T_obj * operator() (long id);
    T_obj * operator() (PString name);
    T_obj * operator() (T_obj * obj);

  protected:
    void CaptureInternal(int index);
    void ReleaseInternal(int index);
    void ReleaseWait(long * _captures, int threshold);

    int size;
    long current_size;
    long idcounter;
    bool * volatile states;
    bool * states_end;
    long * ids;
    long * ids_end;
    PString * names;
    PString * names_end;
    T_obj ** objs;
    T_obj ** objs_end;
    long * volatile captures;
    bool * volatile locks;
    shared_iterator iterator_end;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T_obj>
MCUSharedList<T_obj>::MCUSharedList(int _size)
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
  objs = new T_obj * [size];
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

template <class T_obj>
MCUSharedList<T_obj>::~MCUSharedList()
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

template <class T_obj>
bool MCUSharedList<T_obj>::Insert(T_obj * obj, long id, PString name)
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

template <class T_obj>
bool MCUSharedList<T_obj>::Erase(long id)
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

template <class T_obj>
bool MCUSharedList<T_obj>::Erase(shared_iterator & it)
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

template <class T_obj>
void MCUSharedList<T_obj>::ReleaseWait(long * _captures, int threshold)
{
  for(int i = 0; *_captures != threshold; ++i)
  {
    if(i < 100)
      __asm__ __volatile__("pause":::"memory");
    else if(i < 1000)
      MCUTime::SleepUsec(10);
    else
      MCUTime::SleepUsec(1000);

    if(i % 5000 == 0)
    {
      PTRACE(1, "ReleaseWait list: " << typeid(this).name() << ", captures: " << *_captures);
      cout << "ReleaseWait list: " << typeid(this).name() << ", captures: " << *_captures << "\n";
    }
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T_obj>
void MCUSharedList<T_obj>::Release(long id)
{
  long *it = find(ids, ids_end, id);
  if(it != ids_end)
  {
    int index = it - ids;
    ReleaseInternal(index);
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T_obj>
void MCUSharedList<T_obj>::ReleaseInternal(int index)
{
  //PTRACE(6, "release index=" << index << " captures=" << captures[index] << " id=" << ids[index] << " obj=" << (objs[index] == NULL ? 0 : objs[index]) << " thread=" << PThread::Current() << " " << PThread::Current()->GetThreadName()<< "\ttype=" << typeid(objs[index]).name());
  sync_decrement(&captures[index]);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T_obj>
void MCUSharedList<T_obj>::CaptureInternal(int index)
{
  //PTRACE(6, "capture index=" << index << " captures=" << captures[index] << " id=" << ids[index] << " obj=" << (objs[index] == NULL ? 0 : objs[index]) << " thread=" << PThread::Current() << " " << PThread::Current()->GetThreadName()<< "\ttype=" << typeid(objs[index]).name());
  sync_increment(&captures[index]);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T_obj>
MCUSharedListSharedIterator<MCUSharedList<T_obj>, T_obj> MCUSharedList<T_obj>::Find(long id)
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

template <class T_obj>
MCUSharedListSharedIterator<MCUSharedList<T_obj>, T_obj> MCUSharedList<T_obj>::Find(PString name)
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

template <class T_obj>
MCUSharedListSharedIterator<MCUSharedList<T_obj>, T_obj> MCUSharedList<T_obj>::Find(T_obj * obj)
{
  T_obj **it = find(objs, objs_end, obj);
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

template <class T_obj>
T_obj * MCUSharedList<T_obj>::operator[] (int index)
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

template <class T_obj>
T_obj * MCUSharedList<T_obj>::operator() (long id)
{
  return Find(id).GetCapturedObject();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T_obj>
T_obj * MCUSharedList<T_obj>::operator() (PString name)
{
  return Find(name).GetCapturedObject();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T_obj>
T_obj * MCUSharedList<T_obj>::operator() (T_obj * obj)
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

typedef MCUSharedList<MCUH323Connection> MCUConnectionList;

////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T_obj>
class MCUQueueT
{
  public:
    MCUQueueT(int _size = 256)
      : queue(_size)
    {
      stopped = false;
    }
    ~MCUQueueT() { }

    virtual bool Push(T_obj *obj)
    {
      if(stopped)
        return false;
      while(!queue.Insert(obj, (long)obj))
        MCUTime::Sleep(20);
      queue.Release((long)obj);
      return true;
    }
    virtual T_obj * Pop()
    {
      for(int i = 0; i < queue.GetSize(); ++i)
      {
        T_obj *obj = queue[i];
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
      MCUTime::Sleep(2);
    }

  protected:
    bool stopped;
    MCUSharedList<T_obj> queue;
};

typedef MCUQueueT<PString> MCUQueuePString;
typedef MCUQueueT<msg_t> MCUQueueMsg;

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // _MCU_UTILS_H
