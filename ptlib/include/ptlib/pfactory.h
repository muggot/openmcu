/*
 * factory.h
 *
 * Abstract Factory Classes
 *
 * Portable Windows Library
 *
 * Copyright (C) 2004 Post Increment
 *
 * The contents of this file are subject to the Mozilla Public License
 * Version 1.0 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See
 * the License for the specific language governing rights and limitations
 * under the License.
 *
 * The Original Code is Portable Windows Library.
 *
 * The Initial Developer of the Original Code is Post Increment
 *
 * Contributor(s): ______________________________________.
 *
 * $Log: pfactory.h,v $
 * Revision 1.30  2007/09/09 09:42:09  rjongbloed
 * Prevent internal factory allocations from being included in memory leaks
 *   as they are never deallocated during the life of the program.
 *
 * Revision 1.29  2007/09/08 11:34:28  rjongbloed
 * Improved memory checking (leaks etc), especially when using MSVC debug library.
 *
 * Revision 1.28  2007/08/07 01:37:05  csoutheren
 * Add RegisterAs function to allow registering a factory worker using another key
 *
 * Revision 1.27  2007/04/18 23:49:50  csoutheren
 * Add usage of precompiled headers
 *
 * Revision 1.26  2007/04/13 07:19:23  rjongbloed
 * Removed separate Win32 solution for "plug in static loading" issue,
 *   and used the PLOAD_FACTORY() mechanism for everything.
 * Slight clean up of the PLOAD_FACTORY macro.
 *
 * Revision 1.25  2006/11/20 03:18:39  csoutheren
 * Using std::string instead of PString avoids problems with key comparisons. Not sure why.....
 *
 * Revision 1.24  2006/08/11 04:45:36  csoutheren
 * Explicitly specify the default key type for PFactory
 *
 * Revision 1.23  2006/02/20 06:16:38  csoutheren
 * Extended factory macros
 *
 * Revision 1.22  2005/09/18 13:01:40  dominance
 * fixed pragma warnings when building with gcc.
 *
 * Revision 1.21  2005/05/03 11:58:45  csoutheren
 * Fixed various problems reported by valgrind
 * Thanks to Paul Cadach
 *
 * Revision 1.20  2005/01/04 07:44:02  csoutheren
 * More changes to implement the new configuration methodology, and also to
 * attack the global static problem
 *
 * Revision 1.19  2004/08/16 06:40:59  csoutheren
 * Added adapters template to make device plugins available via the abstract factory interface
 *
 * Revision 1.18  2004/07/12 09:17:20  csoutheren
 * Fixed warnings and errors under Linux
 *
 * Revision 1.17  2004/07/06 10:12:52  csoutheren
 * Added static integer o factory template to assist in ensuring factories are instantiated
 *
 * Revision 1.16  2004/07/06 04:26:44  csoutheren
 * Fixed problem when using factory maps with non-standard keys
 *
 * Revision 1.15  2004/07/02 03:14:47  csoutheren
 * Made factories non-singleton, by default
 * Added more docs
 *
 * Revision 1.14  2004/07/01 11:41:28  csoutheren
 * Fixed compile and run problems on Linux
 *
 * Revision 1.13  2004/07/01 04:33:57  csoutheren
 * Updated documentation on PFactory classes
 *
 * Revision 1.12  2004/06/30 12:17:04  rjongbloed
 * Rewrite of plug in system to use single global variable for all factories to avoid all sorts
 *   of issues with startup orders and Windows DLL multiple instances.
 *
 * Revision 1.11  2004/06/17 06:35:12  csoutheren
 * Use attribute (( constructor )) to guarantee that factories are
 * instantiated when loaded from a shared library
 *
 * Revision 1.10  2004/06/03 13:30:57  csoutheren
 * Renamed INSTANTIATE_FACTORY to avoid potential namespace collisions
 * Added documentaton on new PINSTANTIATE_FACTORY macro
 * Added generic form of PINSTANTIATE_FACTORY
 *
 * Revision 1.9  2004/06/03 12:47:58  csoutheren
 * Decomposed PFactory declarations to hopefully avoid problems with Windows DLLs
 *
 * Revision 1.8  2004/06/01 05:44:12  csoutheren
 * Added typedefs to allow access to types
 * Changed singleton class to use new so as to allow full cleanup
 *
 * Revision 1.7  2004/05/23 12:33:56  rjongbloed
 * Made some subtle changes to the way the static variables are instantiated in
 *   the factoris to fix problems with DLL's under windows. May not be final solution.
 *
 * Revision 1.6  2004/05/19 06:48:39  csoutheren
 * Added new functions to allow handling of singletons without concrete classes
 *
 * Revision 1.5  2004/05/18 06:01:06  csoutheren
 * Deferred plugin loading until after main has executed by using abstract factory classes
 *
 * Revision 1.4  2004/05/18 02:32:08  csoutheren
 * Fixed linking problems with PGenericFactory classes
 *
 * Revision 1.3  2004/05/13 15:10:51  csoutheren
 * Removed warnings under Windows
 *
 * Revision 1.2  2004/05/13 14:59:00  csoutheren
 * Removed warning under gcc
 *
 * Revision 1.1  2004/05/13 14:53:35  csoutheren
 * Add "abstract factory" template classes
 *
 */

#ifndef _PFACTORY_H
#define _PFACTORY_H

#ifdef P_USE_PRAGMA
#pragma interface
#endif

#ifndef _PTLIB_H
#include <ptlib.h>
#endif

#include <string>
#include <map>
#include <vector>

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif

/**
 *
 * These templates implement an Abstract Factory that allows
 * creation of a class "factory" that can be used to create
 * "concrete" instance that are descended from a abstract base class
 *
 * Given an abstract class A with a descendant concrete class B, the 
 * concrete class is registered by instantiating the PFactory template
 * as follows:
 *
 *       PFactory<A>::Worker<B> aFactory("B");
 *
 * To instantiate an object of type B, use the following:
 *
 *       A * b = PFactory<A>::CreateInstance("B");
 *
 * A vector containing the names of all of the concrete classes for an
 * abstract type can be obtained as follows:
 *
 *       PFactory<A>::KeyList_T list = PFactory<A>::GetKeyList()
 *
 * Note that these example assumes that the "key" type for the factory
 * registration is of the default type PString. If a different key type
 * is needed, then it is necessary to specify the key type:
 *
 *       PFactory<C, unsigned>::Worker<D> aFactory(42);
 *       C * d = PFactory<C, unsigned>::CreateInstance(42);
 *       PFactory<C, unsigned>::KeyList_T list = PFactory<C, unsigned>::GetKeyList()
 *
 * The factory functions also allow the creation of "singleton" factories that return a 
 * single instance for all calls to CreateInstance. This can be done by passing a "true"
 * as a second paramater to the factory registration as shown below, which will cause a single
 * instance to be minted upon the first call to CreateInstance, and then returned for all
 * subsequent calls. 
 *
 *      PFactory<A>::Worker<E> eFactory("E", true);
 *
 * It is also possible to manually set the instance in cases where the object needs to be created non-trivially.
 *
 * The following types are defined as part of the PFactory template class:
 *
 *     KeyList_T    a vector<> of the key type (usually std::string)
 *     Worker       an abstract factory for a specified concrete type
 *     KeyMap_T     a map<> that converts from the key type to the Worker instance
 *                  for each concrete type registered for a specific abstract type
 *
 * As a side issue, note that the factory lists are all thread safe for addition,
 * creation, and obtaining the key lists.
 *
 */

// this define the default class to be used for keys into PFactories
//typedef PString PDefaultPFactoryKey;
typedef std::string PDefaultPFactoryKey;


/** Base class for generic factories.
    This classes reason for existance and the FactoryMap contained within it
    is to resolve issues with static global construction order and Windows DLL
    multiple instances issues. THis mechanism guarantees that the one and one
    only global variable (inside the GetFactories() function) is initialised
    before any other factory related instances of classes.
  */
class PFactoryBase
{
  protected:
    PFactoryBase()
    { }
  public:
    virtual ~PFactoryBase()
    { }

    class FactoryMap : public std::map<std::string, PFactoryBase *>
    {
      public:
        FactoryMap() { }
        ~FactoryMap();
    };

    static FactoryMap & GetFactories();
    static PMutex & GetFactoriesMutex();

    PMutex mutex;

  private:
    PFactoryBase(const PFactoryBase &) {}
    void operator=(const PFactoryBase &) {}
};


/** Template class for generic factories of an abstract class.
  */
template <class _Abstract_T, typename _Key_T = PDefaultPFactoryKey>
class PFactory : PFactoryBase
{
  public:
    typedef _Key_T      Key_T;
    typedef _Abstract_T Abstract_T;

    class WorkerBase
    {
      protected:
        WorkerBase(bool singleton = false)
          : isDynamic(false),
            isSingleton(singleton),
            singletonInstance(NULL),
            deleteSingleton(false)
        { }
        WorkerBase(Abstract_T * instance)
          : isDynamic(true),
            isSingleton(true),
            singletonInstance(instance),
            deleteSingleton(true)
        { }

        virtual ~WorkerBase()
        {
          if (deleteSingleton)
            delete singletonInstance;
        }

        Abstract_T * CreateInstance(const Key_T & key)
        {
          if (!isSingleton)
            return Create(key);

          if (singletonInstance == NULL)
            singletonInstance = Create(key);
          return singletonInstance;
        }

        virtual Abstract_T * Create(const Key_T & /*key*/) const { return singletonInstance; }

        bool         isDynamic;
        bool         isSingleton;
        Abstract_T * singletonInstance;
        bool         deleteSingleton;

      friend class PFactory<_Abstract_T, _Key_T>;
    };

    template <class _Concrete_T>
    class Worker : WorkerBase
    {
      public:
        Worker(const Key_T & key, bool singleton = false)
          : WorkerBase(singleton)
        {
          PMEMORY_IGNORE_ALLOCATIONS_FOR_SCOPE;
          PFactory<_Abstract_T, _Key_T>::Register(key, this);   // here
        }

      protected:
        virtual Abstract_T * Create(const Key_T & /*key*/) const
        {
#if PMEMORY_HEAP
          // Singletons are never deallocated, so make sure they arenot reported as a leak
          BOOL previousIgnoreAllocations = PMemoryHeap::SetIgnoreAllocations(isSingleton);
#endif
          Abstract_T * instance = new _Concrete_T;
#if PMEMORY_HEAP
          PMemoryHeap::SetIgnoreAllocations(previousIgnoreAllocations);
#endif
          return instance;
        }
    };

    typedef std::map<_Key_T, WorkerBase *> KeyMap_T;
    typedef std::vector<_Key_T> KeyList_T;

    static void Register(const _Key_T & key, WorkerBase * worker)
    {
      GetInstance().Register_Internal(key, worker);
    }

    static void Register(const _Key_T & key, Abstract_T * instance)
    {
      GetInstance().Register_Internal(key, PNEW WorkerBase(instance));
    }

    static BOOL RegisterAs(const _Key_T & newKey, const _Key_T & oldKey)
    {
      return GetInstance().RegisterAs_Internal(newKey, oldKey);
    }

    static void Unregister(const _Key_T & key)
    {
      GetInstance().Unregister_Internal(key);
    }

    static void UnregisterAll()
    {
      GetInstance().UnregisterAll_Internal();
    }

    static bool IsRegistered(const _Key_T & key)
    {
      return GetInstance().IsRegistered_Internal(key);
    }

    static _Abstract_T * CreateInstance(const _Key_T & key)
    {
      return GetInstance().CreateInstance_Internal(key);
    }

    static BOOL IsSingleton(const _Key_T & key)
    {
      return GetInstance().IsSingleton_Internal(key);
    }

    static KeyList_T GetKeyList()
    { 
      return GetInstance().GetKeyList_Internal();
    }

    static KeyMap_T & GetKeyMap()
    { 
      return GetInstance().keyMap;
    }

    static PMutex & GetMutex()
    {
      return GetInstance().mutex;
    }

  protected:
    PFactory()
    { }

    ~PFactory()
    {
      typename KeyMap_T::const_iterator entry;
      for (entry = keyMap.begin(); entry != keyMap.end(); ++entry) {
        if (entry->second->isDynamic)
          delete entry->second;
      }
    }

    static PFactory & GetInstance()
    {
      std::string className = typeid(PFactory).name();
      PWaitAndSignal m(GetFactoriesMutex());
      FactoryMap & factories = GetFactories();
      FactoryMap::const_iterator entry = factories.find(className);
      if (entry != factories.end()) {
        PAssert(entry->second != NULL, "Factory map returned NULL for existing key");
        PFactoryBase * b = entry->second;
        // don't use the following dynamic cast, because gcc does not like it
        //PFactory * f = dynamic_cast<PFactory*>(b);
        return *(PFactory *)b;
      }

      PMEMORY_IGNORE_ALLOCATIONS_FOR_SCOPE;
      PFactory * factory = new PFactory;
      factories[className] = factory;
      return *factory;
    }


    void Register_Internal(const _Key_T & key, WorkerBase * worker)
    {
      PWaitAndSignal m(mutex);
      if (keyMap.find(key) == keyMap.end())
        keyMap[key] = worker;
    }

    BOOL RegisterAs_Internal(const _Key_T & newKey, const _Key_T & oldKey)
    {
      PWaitAndSignal m(mutex);
      if (keyMap.find(oldKey) == keyMap.end())
        return FALSE;
      keyMap[newKey] = keyMap[oldKey];
      return TRUE;
    }

    void Unregister_Internal(const _Key_T & key)
    {
      PWaitAndSignal m(mutex);
      keyMap.erase(key);
    }

    void UnregisterAll_Internal()
    {
      PWaitAndSignal m(mutex);
      keyMap.erase(keyMap.begin(), keyMap.end());
    }

    bool IsRegistered_Internal(const _Key_T & key)
    {
      PWaitAndSignal m(mutex);
      return keyMap.find(key) != keyMap.end();
    }

    _Abstract_T * CreateInstance_Internal(const _Key_T & key)
    {
      PWaitAndSignal m(mutex);
      typename KeyMap_T::const_iterator entry = keyMap.find(key);
      if (entry != keyMap.end())
        return entry->second->CreateInstance(key);
      return NULL;
    }

    bool IsSingleton_Internal(const _Key_T & key)
    {
      PWaitAndSignal m(mutex);
      if (keyMap.find(key) == keyMap.end())
        return false;
      return keyMap[key]->isSingleton;
    }

    KeyList_T GetKeyList_Internal()
    { 
      PWaitAndSignal m(mutex);
      KeyList_T list;
      typename KeyMap_T::const_iterator entry;
      for (entry = keyMap.begin(); entry != keyMap.end(); ++entry)
        list.push_back(entry->first);
      return list;
    }

    KeyMap_T keyMap;

  private:
    PFactory(const PFactory &) {}
    void operator=(const PFactory &) {}
};

//
//  this macro is used to initialise the static member variable used to force factories to instantiate
//
#define PLOAD_FACTORY(AbstractType, KeyType) \
  namespace PWLibFactoryLoader { \
    extern int AbstractType##_##KeyType##_loader; \
    static int AbstractType##_##KeyType##_loader_instance = AbstractType##_##KeyType##_loader; \
  };


//
//  this macro is used to instantiate a static variable that accesses the static member variable 
//  in a factory forcing it to load
//
#define PINSTANTIATE_FACTORY(AbstractType, KeyType) \
  namespace PWLibFactoryLoader { int AbstractType##_##KeyType##_loader; }


#endif // _PFACTORY_H
