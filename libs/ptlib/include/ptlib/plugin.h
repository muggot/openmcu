/*
 * plugin.h
 *
 * Plugin Class Declarations
 *
 * Portable Windows Library
 *
 * Contributor(s): Snark at GnomeMeeting
 *
 * $Log: plugin.h,v $
 * Revision 1.16  2007/09/04 02:15:53  rjongbloed
 * Allow for API versions other than zero.
 *
 * Revision 1.15  2006/01/08 14:49:08  dsandras
 * Several fixes to allow compilation on Open Solaris thanks to Brian Lu <brian.lu _AT_____ sun.com>. Many thanks!
 *
 * Revision 1.14  2005/08/09 09:08:09  rjongbloed
 * Merged new video code from branch back to the trunk.
 *
 * Revision 1.13.2.1  2005/07/17 09:27:04  rjongbloed
 * Major revisions of the PWLib video subsystem including:
 *   removal of F suffix on colour formats for vertical flipping, all done with existing bool
 *   working through use of RGB and BGR formats so now consistent
 *   cleaning up the plug in system to use virtuals instead of pointers to functions.
 *   rewrite of SDL to be a plug in compatible video output device.
 *   extensive enhancement of video test program
 *
 * Revision 1.13  2005/06/07 00:42:55  csoutheren
 * Apply patch 1214249 to fix crash with Suse 9.3. Thanks to Stefan Bruns
 *
 * Revision 1.12  2005/01/04 07:44:03  csoutheren
 * More changes to implement the new configuration methodology, and also to
 * attack the global static problem
 *
 * Revision 1.11  2004/08/16 11:57:47  csoutheren
 * More changes for VS.net
 *
 * Revision 1.10  2004/08/16 10:55:09  csoutheren
 * Fixed problems compiling under Linux
 *
 * Revision 1.9  2004/08/16 06:40:59  csoutheren
 * Added adapters template to make device plugins available via the abstract factory interface
 *
 * Revision 1.8  2004/06/21 10:40:02  csoutheren
 * Fixed problem with dynamic plugins
 *
 * Revision 1.7  2004/06/21 00:57:40  csoutheren
 * Changed service plugin static registration to use attribute (( constructor ))
 *
 * Revision 1.6  2003/12/19 00:34:27  csoutheren
 * Ensured that older compilers do not get confused about functions wth empty
 * parameter lists. Thanks to Kilian Krause
 *
 * Revision 1.5  2003/11/19 09:29:19  csoutheren
 * Added super hack to avoid problems with multiple plugins in a single file
 *
 * Revision 1.4  2003/11/12 10:24:35  csoutheren
 * Changes to allow operation of static plugins under Windows
 *
 * Revision 1.3  2003/11/12 06:58:21  csoutheren
 * Changes to help in making static plugins autoregister under Windows
 *
 * Revision 1.2  2003/11/12 03:26:17  csoutheren
 * Initial version of plugin code from Snark of GnomeMeeting with changes
 *    by Craig Southeren os Post Increment
 *
 *
 */

#ifndef _PLUGIN_H
#define _PLUGIN_H

//////////////////////////////////////////////////////
//
//  these templates implement an adapter to make the old style device plugins appear in the new factory system
//

#include <ptlib/pfactory.h>

template <class _Abstract_T, typename _Key_T = PString>
class PDevicePluginFactory : public PFactory<_Abstract_T, _Key_T>
{
  public:
    class Worker : public PFactory<_Abstract_T, _Key_T>::WorkerBase 
    {
      public:
        Worker(const _Key_T & key, bool singleton = false)
          : PFactory<_Abstract_T, _Key_T>::WorkerBase(singleton)
        {
          PFactory<_Abstract_T, _Key_T>::Register(key, this);
        }

        ~Worker()
        {
          typedef typename PFactory<_Abstract_T, _Key_T>::WorkerBase WorkerBase_T;
          typedef std::map<_Key_T, WorkerBase_T *> KeyMap_T;
          _Key_T key;

          KeyMap_T km = PFactory<_Abstract_T, _Key_T>::GetKeyMap();

          typename KeyMap_T::const_iterator entry;
          for (entry = km.begin(); entry != km.end(); ++entry) {
            if (entry->second == this) {
              key = entry->first;
              break;
            }
          }
          if (key != NULL)
            PFactory<_Abstract_T, _Key_T>::Unregister(key);
        }

      protected:
        virtual _Abstract_T * Create(const _Key_T & key) const;
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
        new Worker_T(device, FALSE);
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

    virtual PObject *   CreateInstance(int userData) const = 0;
    virtual PStringList GetDeviceNames(int userData) const = 0;
    virtual bool        ValidateDeviceName(const PString & deviceName, int userData) const;
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
    PPluginService(const PString & _serviceName,
                   const PString & _serviceType,
                   PPluginServiceDescriptor *_descriptor)
    {
      serviceName = _serviceName;
      serviceType = _serviceType;
      descriptor  = _descriptor;
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

#define PWLIB_STATIC_LOAD_PLUGIN(serviceName, serviceType) \
  class PPlugin_##serviceType##_##serviceName##_Registration; \
  extern PPlugin_##serviceType##_##serviceName##_Registration PPlugin_##serviceType##_##serviceName##_Registration_Instance; \
  static PPlugin_##serviceType##_##serviceName##_Registration * PPlugin_##serviceType##_##serviceName##_Registration_Static_Library_Loader = &PPlugin_##serviceType##_##serviceName##_Registration_Instance;

// Win32 onl;y has static plugins at present, maybe one day ...
#define P_FORCE_STATIC_PLUGIN

#else

#ifdef USE_GCC
#define PCREATE_PLUGIN_STATIC(serviceName, serviceType, descriptor) \
static void __attribute__ (( constructor )) PWLIB_StaticLoader_##serviceName##_##serviceType() \
{ PPluginManager::GetPluginManager().RegisterService(#serviceName, #serviceType, descriptor); } \

#else
#define PCREATE_PLUGIN_STATIC(serviceName, serviceType, descriptor) \
extern int PWLIB_gStaticLoader__##serviceName##_##serviceType; \
static int PWLIB_StaticLoader_##serviceName##_##serviceType() \
{ PPluginManager::GetPluginManager().RegisterService(#serviceName, #serviceType, descriptor); return 1; } \
int PWLIB_gStaticLoader__##serviceName##_##serviceType =  PWLIB_StaticLoader_##serviceName##_##serviceType(); 
#endif
#define PWLIB_STATIC_LOAD_PLUGIN(serviceName, serviceType) 

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

#endif
