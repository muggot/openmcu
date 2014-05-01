/*
 * pluginmgr.h
 *
 * Plugin Manager Class Declarations
 *
 * Portable Windows Library
 *
 * Contributor(s): Snark at GnomeMeeting
 *
 * $Log: pluginmgr.h,v $
 * Revision 1.24  2007/08/09 10:31:33  csoutheren
 * Restored template for OpenH323 compatibility
 *
 * Revision 1.23  2007/08/08 08:58:39  csoutheren
 * More plugin manager changes, as the last approach dead-ended :(
 *
 * Revision 1.22  2007/08/08 07:12:03  csoutheren
 * More re-arrangement of plugin suffixes
 *
 * Revision 1.21  2007/08/07 07:59:11  csoutheren
 * Allow plugin suffix to be determined via virtual
 *
 * Revision 1.20  2007/04/02 05:29:54  rjongbloed
 * Tidied some trace logs to assure all have a category (bit before a tab character) set.
 *
 * Revision 1.19  2006/10/25 12:51:05  shorne
 * fix for devices having same name for different drivers.
 *
 * Revision 1.18  2006/01/21 13:43:05  dsandras
 * Allow the plugin manager to look for plugins in symlinked directories.
 *
 * Revision 1.17  2005/08/09 09:08:09  rjongbloed
 * Merged new video code from branch back to the trunk.
 *
 * Revision 1.16.6.1  2005/07/17 09:27:04  rjongbloed
 * Major revisions of the PWLib video subsystem including:
 *   removal of F suffix on colour formats for vertical flipping, all done with existing bool
 *   working through use of RGB and BGR formats so now consistent
 *   cleaning up the plug in system to use virtuals instead of pointers to functions.
 *   rewrite of SDL to be a plug in compatible video output device.
 *   extensive enhancement of video test program
 *
 * Revision 1.16  2004/08/05 03:45:35  csoutheren
 * Fixed problems with plugin suffix not being propagated to sudirectories
 *
 * Revision 1.15  2004/06/24 23:10:27  csoutheren
 * Require plugins to have _pwplugin suffix
 *
 * Revision 1.14  2004/06/01 05:44:57  csoutheren
 * Added OnShutdown to allow cleanup on exit
 *
 * Revision 1.13  2004/05/19 06:54:11  csoutheren
 * Removed unused code
 *
 * Revision 1.12  2004/05/18 06:01:06  csoutheren
 * Deferred plugin loading until after main has executed by using abstract factory classes
 *
 * Revision 1.11  2004/05/17 06:05:20  csoutheren
 * Changed "make docs" to use doxygen
 * Added new config file and main page
 *
 * Revision 1.10  2004/04/22 11:43:47  csoutheren
 * Factored out functions useful for loading dynamic libraries
 *
 * Revision 1.9  2004/04/22 07:55:30  csoutheren
 * Fix problem with generic plugin manager having pure virtual. Thanks to Ben Lear
 *
 * Revision 1.8  2004/04/14 11:14:10  csoutheren
 * Final fix for generic plugin manager
 *
 * Revision 1.7  2004/04/14 10:57:38  csoutheren
 * Removed multiple definition of statc function in generic plugin functions
 *
 * Revision 1.6  2004/04/14 10:01:54  csoutheren
 * Fixed compile problem on Windows
 *
 * Revision 1.5  2004/04/14 08:12:02  csoutheren
 * Added support for generic plugin managers
 *
 * Revision 1.4  2004/03/23 04:43:42  csoutheren
 * Modified plugin manager to allow code modules to be notified when plugins
 * are loaded or unloaded
 *
 * Revision 1.3  2003/11/12 10:24:35  csoutheren
 * Changes to allow operation of static plugins under Windows
 *
 * Revision 1.2  2003/11/12 03:26:17  csoutheren
 * Initial version of plugin code from Snark of GnomeMeeting with changes
 *    by Craig Southeren os Post Increment
 *
 *
 */

#ifndef _PLUGINMGR_H
#define _PLUGINMGR_H

#define DEFAULT_PLUGINDIR "/usr/lib/pwlib"

#include <ptlib/plugin.h>

class PPluginSuffix {
  private:
    int dummy;
};

template <class C>
void PLoadPluginDirectory(C & obj, const PDirectory & directory, const char * suffix = NULL)
{
  PDirectory dir = directory;
  if (!dir.Open()) {
    PTRACE(4, "Cannot open plugin directory " << dir);
    return;
  }
  PTRACE(4, "Enumerating plugin directory " << dir);
  do {
    PString entry = dir + dir.GetEntryName();
    PDirectory subdir = entry;
    if (subdir.Open())
      PLoadPluginDirectory<C>(obj, entry, suffix);
    else {
      PFilePath fn(entry);
      if (
           (fn.GetType() *= PDynaLink::GetExtension()) &&
           (
             (suffix == NULL) || (fn.GetTitle().Right(strlen(suffix)) *= suffix)
           )
         )
        obj.LoadPlugin(entry);
    }
  } while (dir.Next());
}

//////////////////////////////////////////////////////
//
//  Manager for plugins
//

class PPluginManager : public PObject
{
  PCLASSINFO(PPluginManager, PObject);

  public:
    // functions to load/unload a dynamic plugin 
    BOOL LoadPlugin (const PString & fileName);
    void LoadPluginDirectory (const PDirectory & dir);
  
    // functions to access the plugins' services 
    PStringList GetPluginTypes() const;
    PStringList GetPluginsProviding(const PString & serviceType) const;
    PPluginServiceDescriptor * GetServiceDescriptor(const PString & serviceName, const PString & serviceType) const;
    PObject * CreatePluginsDevice(const PString & serviceName, const PString & serviceType, int userData = 0) const;
    PObject * CreatePluginsDeviceByName(const PString & deviceName, const PString & serviceType, int userData = 0, const PString & serviceName = PString::Empty()) const;
    PStringList GetPluginsDeviceNames(const PString & serviceName, const PString & serviceType, int userData = 0) const;
	BOOL GetPluginsDeviceCapabilities(const PString & serviceType,const PString & serviceName,const PString & deviceName,void * capabilities) const;

    // function to register a service (used by the plugins themselves)
    BOOL RegisterService (const PString & serviceName, const PString & serviceType, PPluginServiceDescriptor * descriptor);

    // Get the list of plugin directories
    static PStringArray GetPluginDirs();

    // static functions for accessing global instances of plugin managers
    static PPluginManager & GetPluginManager();

    /**Add a notifier to the plugin manager.
       The call back function is executed just after loading, or 
       just after unloading, a plugin. 

       To use define:
         PDECLARE_NOTIFIER(PDynaLink, YourClass, YourFunction);
       and
         void YourClass::YourFunction(PDynaLink & dll, INT code)
         {
           // code == 0 means loading
           // code == 1 means unloading
         }
       and to connect to the plugin manager:
         PPluginManager & mgr = PPluginManager::GetPluginManager();
         mgr->AddNotifier((PCREATE_NOTIFIER(YourFunction));
      */

    void AddNotifier(
      const PNotifier & filterFunction,
      BOOL existing = FALSE
    );

    void RemoveNotifier(
      const PNotifier & filterFunction
    );

  protected:
    void LoadPluginDirectory (const PDirectory & directory, const PStringList & suffixes);
    void CallNotifier(PDynaLink & dll, INT code);

    PMutex pluginListMutex;
    PList<PDynaLink> pluginList;
    
    PMutex serviceListMutex;
    PList<PPluginService> serviceList;

    PMutex notifierMutex;
    PList<PNotifier> notifierList;
};

//////////////////////////////////////////////////////
//
//  Manager for plugin modules
//

class PPluginModuleManager : public PObject
{
  public:
    typedef PDictionary<PString, PDynaLink> PluginListType;

    PPluginModuleManager(const char * _signatureFunctionName, PPluginManager * pluginMgr = NULL);

    BOOL LoadPlugin(const PString & fileName)
    { if (pluginMgr == NULL) return FALSE; else return pluginMgr->LoadPlugin(fileName); }

    void LoadPluginDirectory(const PDirectory &directory)
    { if (pluginMgr != NULL) pluginMgr->LoadPluginDirectory(directory); }

    virtual void OnLoadPlugin(PDynaLink & /*dll*/, INT /*code*/)
    { }

    virtual PluginListType GetPluginList() const
    { return pluginList; }

    virtual void OnShutdown()
    { }

  protected:
    PluginListType pluginList;
    PDECLARE_NOTIFIER(PDynaLink, PPluginModuleManager, OnLoadModule);

  protected:
    const char * signatureFunctionName;
    PPluginManager * pluginMgr;
};

#endif // ifndef _PLUGINMGR_H
