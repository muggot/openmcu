/*
 * plugin.h
 *
 * Plugin Class Declarations
 *
 * Portable Windows Library
 *
 * Contributor(s): Snark at GnomeMeeting
 *
 * $Revision: 25329 $
 * $Author: ededu $
 * $Date: 2011-03-15 18:03:58 -0500 (Tue, 15 Mar 2011) $
 */

#ifndef PTLIB_PLUGIN_H
#define PTLIB_PLUGIN_H

//////////////////////////////////////////////////////
//
//  these templates implement an adapter to make the old style device plugins appear in the new factory system
//

#include <ptlib/pfactory.h>

template <class AbstractClass, typename KeyType = PString>
class PDevicePluginFactory : public PFactory<AbstractClass, KeyType>
{
  public:
    class Worker : public PFactory<AbstractClass, KeyType>::WorkerBase 
    {
      public:
        Worker(const KeyType & key, bool singleton = false)
          : PFactory<AbstractClass, KeyType>::WorkerBase(singleton)
        {
          PFactory<AbstractClass, KeyType>::Register(key, this);
        }

        ~Worker()
        {
          typedef typename PFactory<AbstractClass, KeyType>::WorkerBase WorkerBase_T;
          typedef std::map<KeyType, WorkerBase_T *> KeyMap_T;
          KeyType key;

          KeyMap_T km = PFactory<AbstractClass, KeyType>::GetKeyMap();

          typename KeyMap_T::const_iterator entry;
          for (entry = km.begin(); entry != km.end(); ++entry) {
            if (entry->second == this) {
              key = entry->first;
              break;
            }
          }
          if (key != NULL)
            PFactory<AbstractClass, KeyType>::Unregister(key);
        }

      protected:
        virtual AbstractClass * Create(const KeyType & key) const;
    };
};

class PDevicePluginAdapterBase
{
  public:
    PDevicePluginAdapterBase()
    { }
    virtual ~PDevicePluginAdapterBase()
    { }
    virtual void CreateFactory(const PString & device) = 0;
};

template <typename DeviceBase>
class PDevicePluginAdapter : public PDevicePluginAdapterBase
{
  public:
    typedef PDevicePluginFactory<DeviceBase> Factory_T;
    typedef typename Factory_T::Worker Worker_T;
    void CreateFactory(const PString & device)
    {
      if (!(Factory_T::IsRegistered(device)))
        new Worker_T(device, false);
    }
};


#ifndef PWLIB_PLUGIN_API_VERSION
#define PWLIB_PLUGIN_API_VERSION 0
#endif


//////////////////////////////////////////////////////
//
//  Ancestor Service descriptor for plugins
//

class PPluginServiceDescriptor 
{
  public:
    PPluginServiceDescriptor() { version = PWLIB_PLUGIN_API_VERSION; }
    virtual ~PPluginServiceDescriptor() { }

    virtual unsigned GetPluginAPIVersion() const { return version; }

  protected:
    unsigned version;
};


class PDevicePluginServiceDescriptor : public PPluginServiceDescriptor
{
  public:
    static const char SeparatorChar;

    virtual PObject *    CreateInstance(int userData) const = 0;
    virtual PStringList GetDeviceNames(int userData) const = 0;
    virtual bool         ValidateDeviceName(const PString & deviceName, int userData) const;
    virtual bool         GetDeviceCapabilities(const PString & deviceName, void * capabilities) const;
};


//////////////////////////////////////////////////////
//
// Define a service provided by a plugin, which consists of the following:
//
//    serviceType - the base class name of the service which is used to identify
//                  the service type, such as PSoundChannel, 
//
//    serviceName - the name of the service provided by the plugin. This will usually
//                  be related to the class implementing the service, such as:
//                       service name = OSS, class name = PSoundChannelOSS
//
//    descriptor  - a pointer to a class containing pointers to any static functions
//                  for this class
//   
//

class PPluginService: public PObject
{
  public:
    PPluginService(const PString & name,
                   const PString & type,
                   PPluginServiceDescriptor * desc)
      : serviceName(name)
      , serviceType(type)
      , descriptor(desc)
    {
    }

    PString serviceName;
    PString serviceType;
    PPluginServiceDescriptor * descriptor;
};


//////////////////////////////////////////////////////
//
//  These crazy macros are needed to cause automatic registration of 
//  static plugins. They are made more complex by the arcane behaviour
//  of the Windows link system that requires an external reference in the
//  object module before it will instantiate any globals in in it
//

#define PCREATE_PLUGIN_REGISTERER(serviceName, serviceType, descriptor) \
class PPlugin_##serviceType##_##serviceName##_Registration { \
  public: \
    PPlugin_##serviceType##_##serviceName##_Registration(PPluginManager * pluginMgr) \
    { \
      static PDevicePluginFactory<serviceType>::Worker factory(#serviceName); \
      pluginMgr->RegisterService(#serviceName, #serviceType, descriptor); \
    } \
    int kill_warning; \
}; \

#ifdef _WIN32

#define PCREATE_PLUGIN_STATIC(serviceName, serviceType, descriptor) \
PCREATE_PLUGIN_REGISTERER(serviceName, serviceType, descriptor) \
PPlugin_##serviceType##_##serviceName##_Registration \
  PPlugin_##serviceType##_##serviceName##_Registration_Instance(&PPluginManager::GetPluginManager()); \
  int PPlugin_##serviceType##_##serviceName##_link() { return 0; }

#define PPLUGIN_STATIC_LOAD(serviceName, serviceType) \
  extern int PPlugin_##serviceType##_##serviceName##_link(); \
  int const PPlugin_##serviceType##_##serviceName##_loader = PPlugin_##serviceType##_##serviceName##_link();

#define PWLIB_STATIC_LOAD_PLUGIN(serviceName, serviceType) \
  class PPlugin_##serviceType##_##serviceName##_Registration; \
  extern PPlugin_##serviceType##_##serviceName##_Registration PPlugin_##serviceType##_##serviceName##_Registration_Instance; \
  static PPlugin_##serviceType##_##serviceName##_Registration * PPlugin_##serviceType##_##serviceName##_Registration_Static_Library_Loader = &PPlugin_##serviceType##_##serviceName##_Registration_Instance;

// always define static plugins in Windows, since otherwise they seem not to work
#ifndef P_FORCE_STATIC_PLUGIN
  #define P_FORCE_STATIC_PLUGIN 1
#endif

#else

#ifdef USE_GCC
#define PCREATE_PLUGIN_STATIC(serviceName, serviceType, descriptor) \
static void __attribute__ (( constructor )) PWLIB_StaticLoader_##serviceName##_##serviceType() \
{ PPluginManager::GetPluginManager().RegisterService(#serviceName, #serviceType, descriptor); } \
  int PPlugin_##serviceType##_##serviceName##_link() { return 0; }

#else
#define PCREATE_PLUGIN_STATIC(serviceName, serviceType, descriptor) \
extern int PWLIB_gStaticLoader__##serviceName##_##serviceType; \
static int PWLIB_StaticLoader_##serviceName##_##serviceType() \
{ PPluginManager::GetPluginManager().RegisterService(#serviceName, #serviceType, descriptor); return 1; } \
  int PWLIB_gStaticLoader__##serviceName##_##serviceType =  PWLIB_StaticLoader_##serviceName##_##serviceType(); \
  int PPlugin_##serviceType##_##serviceName##_link() { return 0; }
#endif

#define PWLIB_STATIC_LOAD_PLUGIN(serviceName, serviceType) 

#define PPLUGIN_STATIC_LOAD(serviceName, serviceType) \
  extern int PPlugin_##serviceType##_##serviceName##_link(); \
  int const PPlugin_##serviceType##_##serviceName##_loader = PPlugin_##serviceType##_##serviceName##_link();

#ifndef P_SHAREDLIB
#ifndef P_FORCE_STATIC_PLUGIN
  #define P_FORCE_STATIC_PLUGIN 1
#endif
#endif

#endif


//////////////////////////////////////////////////////

#if defined(P_HAS_PLUGINS) && ! defined(P_FORCE_STATIC_PLUGIN)

#  define PCREATE_PLUGIN(serviceName, serviceType, descriptor) \
    PCREATE_PLUGIN_REGISTERER(serviceName, serviceType, descriptor) \
    extern "C" void PWLibPlugin_TriggerRegister (PPluginManager * pluginMgr) { \
    PPlugin_##serviceType##_##serviceName##_Registration \
        pplugin_##serviceType##_##serviceName##_Registration_Instance(pluginMgr); \
        pplugin_##serviceType##_##serviceName##_Registration_Instance.kill_warning = 0; \
    } \
    extern "C" unsigned PWLibPlugin_GetAPIVersion (void) \
    { return PWLIB_PLUGIN_API_VERSION; }

#else

#  define PCREATE_PLUGIN(serviceName, serviceType, descriptor) \
    PCREATE_PLUGIN_STATIC(serviceName, serviceType, descriptor)

#endif

//////////////////////////////////////////////////////


#endif // PTLIB_PLUGIN_H


// End Of File ///////////////////////////////////////////////////////////////
