
#ifndef _MCU_UTILS_LIST_H
#define _MCU_UTILS_LIST_H

#include "config.h"
#include "utils_type.h"

#include <algorithm>
#include <typeinfo>

#include <sofia-sip/sip_util.h>

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
      Capture();
    }

  public:

    // Пустой итератор
    MCUSharedListSharedIterator()
      : list(NULL), index(INT_MAX), captured(false)
    { }

    // Итератор n-го доступного объекта в списке
    MCUSharedListSharedIterator(T_list * _list, int _number)
      : list(_list), index(INT_MAX), captured(false)
    {
      Next(_number);
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
    {
      if(captured)
        return index;
      return INT_MAX;
    }

    long GetID()
    {
      if(captured)
        return list->ids[index];
      return LONG_MAX;
    }

    std::string GetName()
    {
      if(captured)
        return *list->names[index];
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
      Next();
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
      if(!captured && list && index < list->size)
      {
        captured = true;
        list->CaptureInternal(index);
      }
    }

    // Только для внутреннего пользования
    bool Next(int number = 0)
    {
      if(list == NULL)
        goto end;

      Release();
      if(index >= list->size)
        index = 0;
      else
        ++index;

      for(bool *it = find(list->states + index, list->states_end, true); it != list->states_end; it = find(++it, list->states_end, true), --number)
      {
        if(number > 0)
          continue;
        index = it - list->states;
        Capture();
        // повторная проверка после захвата
        if(list->states[index] == true)
          return true;
        // освободить если нет объекта
        Release();
      }

      end:
        index = INT_MAX;
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
    MCUSharedList(int init_size = 256);
    ~MCUSharedList();

    // В новом итераторе объект захвачен
    // Каждый новый клон итератора делает еще один захват
    typedef MCUSharedListSharedIterator<MCUSharedList, T_obj> shared_iterator;

    // begin() - первый доступный объект
    shared_iterator begin() { return shared_iterator(this, 0); }
    // end() - постоянный index = INT_MAX
    const shared_iterator & end() const { return iterator_end; }

    // Максимальный размер списка
    int GetMaxSize()
    { return size; }

    // Количество доступных объектов
    int GetSize()
    { return (int)current_size; }

    // Уникальный идентификатор для объектов
    long GetNextID()
    { return sync_increment(&id_counter); }

    // Insert добавляет в первую свободную позицию с начала списка
    // Возвращает false(end) если нет свободного места, после добавления объект захвачен(в итераторе)
    bool Insert(int index, T_obj * obj, long id, const std::string &name = "");
    shared_iterator Insert(T_obj * obj, long id, const std::string &name = "");

    // Pushback добавляет в конец списка или в первую свободную позицию
    // Возвращает end() если нет свободного места, после добавления объект захвачен в итераторе
    shared_iterator Pushback(T_obj * obj, long id, const std::string &name = "");

    // Erase возвращает false если объекта нет в списке или Erase выполняется другим потоком
    // Обязательно проверять результат выполнения перед удалением объекта!
    // Erase(id) - перед Erase освободить(если захвачен)
    bool Erase(long id);
    // Erase(iterator)
    bool Erase(shared_iterator & it);

    // Release() - освободить объект
    void Release(long id);
    void Release(shared_iterator & it);

    // Find() возвращают захваченный объект
    // Освобождать функцией iterator.Release()
    shared_iterator Find(long id);
    shared_iterator Find(std::string name);
    shared_iterator Find(const T_obj * obj);

    // Операторы возвращают захваченный объект
    // Освобождать функцией list.Release(id)
    T_obj * operator[] (int index);
    T_obj * operator() (long id);
    T_obj * operator() (std::string name);
    T_obj * operator() (const T_obj * obj);

  protected:
    void CaptureInternal(int index);
    void ReleaseInternal(int index);
    void ReleaseWait(int index, int threshold);

    // GetIndex() возвращает захваченный индекс
    int GetIndex(const long id);
    int GetIndex(const std::string &name);
    int GetIndex(const T_obj * obj);

    bool EraseInternal(int index);
    void UpdatePushbackIndex(int new_index);

    const int size;
    long current_size;
    long id_counter;
    int pushback_index;
    bool * volatile states;
    bool * states_end;
    long * ids;
    long * ids_end;
    std::string ** names;
    std::string ** names_end;
    T_obj ** objs;
    T_obj ** objs_end;
    long * volatile captures;
    bool * volatile locks;
    const shared_iterator iterator_end;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T_obj>
MCUSharedList<T_obj>::MCUSharedList(int _size)
  : size(_size), current_size(0), id_counter(0), pushback_index(0)
{
  states = new bool [size];
  states_end = states + size;
  ids = new long [size];
  ids_end = ids + size;
  names = new std::string * [size];
  names_end = names + size;
  objs = new T_obj * [size];
  objs_end = objs + size;
  captures = new long [size];
  locks = new bool [size];
  for(int i = 0; i < size; ++i)
  {
    states[i] = false;
    ids[i] = LONG_MAX;
    names[i] = NULL;
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

  for(int i = 0; i < size; ++i)
    delete names[i];
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
void MCUSharedList<T_obj>::UpdatePushbackIndex(int new_index)
{
  new_index++;
  if(new_index == size)
    new_index = 0;
  int old_index = pushback_index;
  if(new_index > pushback_index || old_index == size - 1)
#  ifdef _WIN32
    sync_val_compare_and_swap((volatile unsigned *)(&pushback_index), (unsigned)old_index, (unsigned)new_index);
#  else
    sync_val_compare_and_swap(&pushback_index, old_index, new_index);
#  endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T_obj>
bool MCUSharedList<T_obj>::Insert(int index, T_obj * obj, long id, const std::string &name)
{
  bool insert = false;
  // блокировка записи
  if(sync_bool_compare_and_swap(&locks[index], false, true) == true)
  {
    // повторная проверка после блокировки
    if(states[index] == false)
    {
      // последний индекс
      UpdatePushbackIndex(index);
      // захват
      CaptureInternal(index);
      // запись объекта
      objs[index] = obj;
      ids[index] = id;
      if(!name.empty())
      {
        if(names[index])
          *names[index] = name;
        else
          names[index] = new std::string(name);
      }
      // разрешить получение объекта
      states[index] = true;
      sync_increment(&current_size);
      insert = true;
    }
    // разблокировка записи
    sync_bool_compare_and_swap(&locks[index], true, false);
  }
  return insert;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T_obj>
MCUSharedListSharedIterator<MCUSharedList<T_obj>, T_obj> MCUSharedList<T_obj>::Insert(T_obj * obj, long id, const std::string &name)
{
  for(bool *it = find(states, states_end, false); it != states_end; it = find(++it, states_end, false))
  {
    int index = it - states;
    if(Insert(index, obj, id, name))
      return shared_iterator(this, index, true);
  }
  return iterator_end;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T_obj>
MCUSharedListSharedIterator<MCUSharedList<T_obj>, T_obj> MCUSharedList<T_obj>::Pushback(T_obj * obj, long id, const std::string &name)
{
  bool *pb_end = states_end;
  bool *pb_begin = states + pushback_index;
  bool *it = pb_begin - 1;
  while(true)
  {
    it = find(++it, pb_end, false);
    if(it == pb_end)
    {
      if(pb_begin == states)
        break;
      pb_end = pb_begin;
      pb_begin = states;
      it = pb_begin - 1;
      continue;
    }
    int index = it - states;
    if(Insert(index, obj, id, name))
      return shared_iterator(this, index, true);
  }
  return iterator_end;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T_obj>
bool MCUSharedList<T_obj>::Erase(long id)
{
  int index = GetIndex(id);
  if(index == INT_MAX)
    return false;
  bool erase = EraseInternal(index);
  ReleaseInternal(index);
  return erase;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T_obj>
bool MCUSharedList<T_obj>::Erase(shared_iterator & it)
{
  // итератор должен быть захвачен
  if(it.captured == false)
    return false;
  bool erase = EraseInternal(it.GetIndex());
  it.Release();
  return erase;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T_obj>
bool MCUSharedList<T_obj>::EraseInternal(int index)
{
  // блокировка записи
  if(sync_bool_compare_and_swap(&locks[index], false, true) == true)
  {
    // запретить получение объекта
    states[index] = false;
    sync_decrement(&current_size);
    // ждать освобождения объекта
    ReleaseWait(index, 1);
    // запись объекта
    ids[index] = LONG_MAX;
    if(names[index])
      names[index]->clear();
    objs[index] = NULL;
    // разблокировка записи
    sync_bool_compare_and_swap(&locks[index], true, false);
    return true;
  }
  return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T_obj>
void MCUSharedList<T_obj>::ReleaseWait(int index, int threshold)
{
  for(int i = 0; captures[index] != threshold; ++i)
  {
    if(i < 4000)
#   ifdef _WIN32
      YieldProcessor();
#   else
      __asm__ __volatile__("pause":::"memory");
#   endif
    else if(i < 9000) // +50msec
      MCUTime::SleepUsec(10);
    else
    {
      if(i % 5000 == 0)
        MCUTRACE(1, "ReleaseWait: " << typeid(*this).name() << " index=" << index <<
                    " obj=" << objs[index] << " id=" << ids[index] << " name=" << (names[index] ? *names[index] : "") <<
                    " captures=" << captures[index]);
      MCUTime::SleepUsec(1000);
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
void MCUSharedList<T_obj>::Release(shared_iterator & it)
{
  // освободить объект и
  // запретить получение объекта из итератора
  it.Release();
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
int MCUSharedList<T_obj>::GetIndex(const long id)
{
  long *it = find(ids, ids_end, id);
  if(it != ids_end)
  {
    int index = it - ids;
    CaptureInternal(index);
    // повторная проверка после захвата
    if(ids[index] == id && states[index] == true)
      return index;
    // освободить если нет объекта
    ReleaseInternal(index);
  }
  return INT_MAX;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

struct string_equal_pointers
{
  string_equal_pointers(const std::string *_s1) : s1(_s1) { }
  bool operator()(const std::string *s2)
  { return (s2 && *s2 == *s1); }
  const std::string *s1;
};

template <class T_obj>
int MCUSharedList<T_obj>::GetIndex(const std::string &name)
{
  std::string **it = find_if(names, names_end, string_equal_pointers(&name));
  if(it != names_end)
  {
    int index = it - names;
    CaptureInternal(index);
    // повторная проверка после захвата
    if(*names[index] == name && states[index] == true)
      return index;
    // освободить если нет объекта
    ReleaseInternal(index);
  }
  return INT_MAX;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T_obj>
int MCUSharedList<T_obj>::GetIndex(const T_obj * obj)
{
  T_obj **it = find(objs, objs_end, obj);
  if(it != objs_end)
  {
    int index = it - objs;
    CaptureInternal(index);
    // повторная проверка после захвата
    if(objs[index] == obj && states[index] == true)
      return index;
    // освободить если нет объекта
    ReleaseInternal(index);
  }
  return INT_MAX;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T_obj>
MCUSharedListSharedIterator<MCUSharedList<T_obj>, T_obj> MCUSharedList<T_obj>::Find(long id)
{
  int index = GetIndex(id);
  if(index == INT_MAX)
    return iterator_end;
  return shared_iterator(this, index, true);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T_obj>
MCUSharedListSharedIterator<MCUSharedList<T_obj>, T_obj> MCUSharedList<T_obj>::Find(std::string name)
{
  int index = GetIndex(name);
  if(index == INT_MAX)
    return iterator_end;
  return shared_iterator(this, index, true);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T_obj>
MCUSharedListSharedIterator<MCUSharedList<T_obj>, T_obj> MCUSharedList<T_obj>::Find(const T_obj * obj)
{
  int index = GetIndex(obj);
  if(index == INT_MAX)
    return iterator_end;
  return shared_iterator(this, index, true);
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
  int index = GetIndex(id);
  if(index == INT_MAX)
    return NULL;
  return objs[index];
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T_obj>
T_obj * MCUSharedList<T_obj>::operator() (std::string name)
{
  int index = GetIndex(name);
  if(index == INT_MAX)
    return NULL;
  return objs[index];
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <class T_obj>
T_obj * MCUSharedList<T_obj>::operator() (const T_obj * obj)
{
  int index = GetIndex(obj);
  if(index == INT_MAX)
    return NULL;
  return objs[index];
}

////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T_obj>
class MCUQueue
{
  public:
    MCUQueue(int _size = 1024)
      : list(_size)
    { }

    virtual bool Push(T_obj *obj)
    {
      for(int i = 0; i < 100; ++i)
      {
        MCUSharedListSharedIterator<MCUSharedList<T_obj>, T_obj> it = list.Insert(obj, (long)obj);
        if(it != list.end())
          return true;
        MCUTime::Sleep(10);
      }
      return false;
    }

    virtual T_obj * Pop()
    {
      MCUSharedListSharedIterator<MCUSharedList<T_obj>, T_obj> it = list.begin();
      T_obj *obj = *it;
      if(list.Erase(it))
        return obj;
      return NULL;
    }

  protected:
    MCUSharedList<T_obj> list;
};

class MCUQueuePString : public MCUQueue<PString>
{
  public:
    MCUQueuePString(int _size = 1024)
      : MCUQueue<PString>(_size)
    { }

    ~MCUQueuePString()
    {
      PString *str = NULL;
      while((str = Pop()))
        delete str;
    }

    virtual bool Push(PString *str)
    {
      if(MCUQueue<PString>::Push(str))
        return true;
      delete str;
      return false;
    }
};

class MCUQueueMsg : public MCUQueue<msg_t>
{
  public:
    MCUQueueMsg(int _size = 1024)
      : MCUQueue<msg_t>(_size)
    { }

    ~MCUQueueMsg()
    {
      msg_t *msg = NULL;
      while((msg = Pop()))
        msg_destroy(msg);
    }

    virtual bool Push(msg_t *msg)
    {
      if(MCUQueue<msg_t>::Push(msg))
        return true;
      msg_destroy(msg);
      return false;
    }
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

typedef MCUSharedList<Conference> MCUConferenceList;
typedef MCUSharedList<ConferenceAudioConnection> MCUAudioConnectionList;
typedef MCUSharedList<MCUSimpleVideoMixer> MCUVideoMixerList;
typedef MCUSharedList<ConferenceMember> MCUMemberList;

typedef MCUSharedList<RegistrarAccount> MCURegistrarAccountList;
typedef MCUSharedList<RegistrarConnection> MCURegistrarConnectionList;
typedef MCUSharedList<RegistrarSubscription> MCURegistrarSubscriptionList;

typedef MCUSharedList<MCUH323Connection> MCUConnectionList;

typedef MCUSharedList<AbookAccount> MCUAbookList;

////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // _MCU_UTILS_LIST_H
