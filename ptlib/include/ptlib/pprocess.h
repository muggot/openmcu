/*
 * pprocess.h
 *
 * Operating System Process (running program executable) class.
 *
 * Portable Windows Library
 *
 * Copyright (c) 1993-1998 Equivalence Pty. Ltd.
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
 * The Initial Developer of the Original Code is Equivalence Pty. Ltd.
 *
 * Portions are Copyright (C) 1993 Free Software Foundation, Inc.
 * All Rights Reserved.
 *
 * Contributor(s): ______________________________________.
 *
 * $Log: pprocess.h,v $
 * Revision 1.73  2007/10/03 01:18:45  rjongbloed
 * Fixed build for Windows Mobile 5 and added Windows Mobile 6
 *
 * Revision 1.72  2006/06/21 03:28:41  csoutheren
 * Various cleanups thanks for Frederic Heem
 *
 * Revision 1.71  2005/11/30 12:47:38  csoutheren
 * Removed tabs, reformatted some code, and changed tags for Doxygen
 *
 * Revision 1.70  2005/11/25 03:43:47  csoutheren
 * Fixed function argument comments to be compatible with Doxygen
 *
 * Revision 1.69  2005/01/26 05:37:54  csoutheren
 * Added ability to remove config file support
 *
 * Revision 1.68  2004/06/30 12:17:04  rjongbloed
 * Rewrite of plug in system to use single global variable for all factories to avoid all sorts
 *   of issues with startup orders and Windows DLL multiple instances.
 *
 * Revision 1.67  2004/05/27 04:46:42  csoutheren
 * Removed vestigal Macintosh code
 *
 * Revision 1.66  2004/05/21 00:28:39  csoutheren
 * Moved PProcessStartup creation to PProcess::Initialise
 * Added PreShutdown function and called it from ~PProcess to handle PProcessStartup removal
 *
 * Revision 1.65  2004/05/19 22:27:19  csoutheren
 * Added fix for gcc 2.95
 *
 * Revision 1.64  2004/05/18 21:49:25  csoutheren
 * Added ability to display trace output from program startup via environment
 * variable or by application creating a PProcessStartup descendant
 *
 * Revision 1.63  2004/05/18 06:01:06  csoutheren
 * Deferred plugin loading until after main has executed by using abstract factory classes
 *
 * Revision 1.62  2004/05/13 14:54:57  csoutheren
 * Implement PProcess startup and shutdown handling using abstract factory classes
 *
 * Revision 1.61  2003/11/25 08:28:13  rjongbloed
 * Removed ability to have platform without threads, win16 finally deprecated
 *
 * Revision 1.60  2003/09/17 05:41:59  csoutheren
 * Removed recursive includes
 *
 * Revision 1.59  2003/09/17 01:18:02  csoutheren
 * Removed recursive include file system and removed all references
 * to deprecated coooperative threading support
 *
 * Revision 1.58  2002/12/11 22:23:59  robertj
 * Added ability to set user identity temporarily and permanently.
 * Added get and set users group functions.
 *
 * Revision 1.57  2002/12/02 03:57:18  robertj
 * More RTEMS support patches, thank you Vladimir Nesic.
 *
 * Revision 1.56  2002/10/17 13:44:27  robertj
 * Port to RTEMS, thanks Vladimir Nesic.
 *
 * Revision 1.55  2002/10/17 07:17:42  robertj
 * Added ability to increase maximum file handles on a process.
 *
 * Revision 1.54  2002/10/10 04:43:43  robertj
 * VxWorks port, thanks Martijn Roest
 *
 * Revision 1.53  2002/09/16 01:08:59  robertj
 * Added #define so can select if #pragma interface/implementation is used on
 *   platform basis (eg MacOS) rather than compiler, thanks Robert Monaghan.
 *
 * Revision 1.52  2002/07/30 02:55:48  craigs
 * Added program start time to PProcess
 * Added virtual to GetVersion etc
 *
 * Revision 1.51  2002/02/14 05:13:33  robertj
 * Fixed possible deadlock if a timer is deleted (however indirectly) in the
 *   OnTimeout of another timer.
 *
 * Revision 1.50  2001/11/23 06:59:29  robertj
 * Added PProcess::SetUserName() function for effective user changes.
 *
 * Revision 1.49  2001/08/11 07:57:30  rogerh
 * Add Mac OS Carbon changes from John Woods <jfw@jfwhome.funhouse.com>
 *
 * Revision 1.48  2001/05/22 12:49:32  robertj
 * Did some seriously wierd rewrite of platform headers to eliminate the
 *   stupid GNU compiler warning about braces not matching.
 *
 * Revision 1.47  2001/03/09 05:50:48  robertj
 * Added ability to set default PConfig file or path to find it.
 *
 * Revision 1.46  2001/01/02 07:47:44  robertj
 * Fixed very narrow race condition in timers (destroyed while in OnTimeout()).
 *
 * Revision 1.45  2000/08/30 03:16:59  robertj
 * Improved multithreaded reliability of the timers under stress.
 *
 * Revision 1.44  2000/04/03 18:42:40  robertj
 * Added function to determine if PProcess instance is initialised.
 *
 * Revision 1.43  2000/02/29 12:26:14  robertj
 * Added named threads to tracing, thanks to Dave Harvey
 *
 * Revision 1.42  1999/03/09 02:59:50  robertj
 * Changed comments to doc++ compatible documentation.
 *
 * Revision 1.41  1999/02/16 08:11:09  robertj
 * MSVC 6.0 compatibility changes.
 *
 * Revision 1.40  1999/01/30 14:28:10  robertj
 * Added GetOSConfigDir() function.
 *
 * Revision 1.39  1999/01/11 11:27:11  robertj
 * Added function to get the hardware process is running on.
 *
 * Revision 1.38  1998/11/30 02:51:00  robertj
 * New directory structure
 *
 * Revision 1.37  1998/10/18 14:28:44  robertj
 * Renamed argv/argc to eliminate accidental usage.
 *
 * Revision 1.36  1998/10/13 14:06:13  robertj
 * Complete rewrite of memory leak detection code.
 *
 * Revision 1.35  1998/09/23 06:21:10  robertj
 * Added open source copyright license.
 *
 * Revision 1.34  1998/09/14 12:30:38  robertj
 * Fixed memory leak dump under windows to not include static globals.
 *
 * Revision 1.33  1998/04/07 13:33:53  robertj
 * Changed startup code to support PApplication class.
 *
 * Revision 1.32  1998/04/01 01:56:21  robertj
 * Fixed standard console mode app main() function generation.
 *
 * Revision 1.31  1998/03/29 06:16:44  robertj
 * Rearranged initialisation sequence so PProcess descendent constructors can do "things".
 *
 * Revision 1.30  1998/03/20 03:16:10  robertj
 * Added special classes for specific sepahores, PMutex and PSyncPoint.
 *
 * Revision 1.29  1997/07/08 13:13:46  robertj
 * DLL support.
 *
 * Revision 1.28  1997/04/27 05:50:13  robertj
 * DLL support.
 *
 * Revision 1.27  1997/02/05 11:51:56  robertj
 * Changed current process function to return reference and validate objects descendancy.
 *
 * Revision 1.26  1996/06/28 13:17:08  robertj
 * Fixed incorrect declaration of internal timer list.
 *
 * Revision 1.25  1996/06/13 13:30:49  robertj
 * Rewrite of auto-delete threads, fixes Windows95 total crash.
 *
 * Revision 1.24  1996/05/23 09:58:47  robertj
 * Changed process.h to pprocess.h to avoid name conflict.
 * Added mutex to timer list.
 *
 * Revision 1.23  1996/05/18 09:18:30  robertj
 * Added mutex to timer list.
 *
 * Revision 1.22  1996/04/29 12:18:48  robertj
 * Added function to return process ID.
 *
 * Revision 1.21  1996/03/12 11:30:21  robertj
 * Moved destructor to platform dependent code.
 *
 * Revision 1.20  1996/02/25 11:15:26  robertj
 * Added platform dependent Construct function to PProcess.
 *
 * Revision 1.19  1996/02/03 11:54:09  robertj
 * Added operating system identification functions.
 *
 * Revision 1.18  1996/01/02 11:57:17  robertj
 * Added thread for timers.
 *
 * Revision 1.17  1995/12/23 03:46:02  robertj
 * Changed version numbers.
 *
 * Revision 1.16  1995/12/10 11:33:36  robertj
 * Added extra user information to processes and applications.
 * Changes to main() startup mechanism to support Mac.
 *
 * Revision 1.15  1995/06/17 11:13:05  robertj
 * Documentation update.
 *
 * Revision 1.14  1995/06/17 00:43:10  robertj
 * Made PreInitialise virtual for NT service support
 *
 * Revision 1.13  1995/03/14 12:42:14  robertj
 * Updated documentation to use HTML codes.
 *
 * Revision 1.12  1995/03/12  04:43:26  robertj
 * Remvoed redundent destructor.
 *
 * Revision 1.11  1995/01/11  09:45:09  robertj
 * Documentation and normalisation.
 *
 * Revision 1.10  1994/08/23  11:32:52  robertj
 * Oops
 *
 * Revision 1.9  1994/08/22  00:46:48  robertj
 * Added pragma fro GNU C++ compiler.
 *
 * Revision 1.8  1994/08/21  23:43:02  robertj
 * Added function to get the user name of the owner of a process.
 *
 * Revision 1.7  1994/08/04  11:51:04  robertj
 * Moved OperatingSystemYield() to protected for Unix.
 *
 * Revision 1.6  1994/08/01  03:42:23  robertj
 * Destructor needed for heap debugging.
 *
 * Revision 1.5  1994/07/27  05:58:07  robertj
 * Synchronisation.
 *
 * Revision 1.4  1994/07/21  12:33:49  robertj
 * Moved cooperative threads to common.
 *
 * Revision 1.3  1994/06/25  11:55:15  robertj
 * Unix version synchronisation.
 *
 */

#ifndef _PPROCESS
#define _PPROCESS

#ifdef P_USE_PRAGMA
#pragma interface
#endif

#include <ptlib/mutex.h>
#include <ptlib/syncpoint.h>
#include <ptlib/thread.h>
#include <ptlib/pfactory.h>

/**Create a process.
   This macro is used to create the components necessary for a user PWLib
   process. For a PWLib program to work correctly on all platforms the
   #main()# function must be defined in the same module as the
   instance of the application.
 */
#ifdef P_VXWORKS
#define PCREATE_PROCESS(cls) \
  PProcess::PreInitialise(0, NULL, NULL); \
  cls instance; \
  instance._main();
#elif defined(P_RTEMS)
#define PCREATE_PROCESS(cls) \
extern "C" {\
   void* POSIX_Init( void* argument) \
     { PProcess::PreInitialise(0, 0, 0); \
       static cls instance; \
       exit( instance._main() ); \
     } \
}
#elif defined(_WIN32_WCE)
#define PCREATE_PROCESS(cls) \
  int WinMain(HINSTANCE, HINSTANCE, LPWSTR, int) \
    { cls *pInstance = new cls(); \
      int terminationValue = pInstance->_main(); \
      delete pInstance; \
      return terminationValue; \
    }
#else
#define PCREATE_PROCESS(cls) \
  int main(int argc, char ** argv, char ** envp) \
    { PProcess::PreInitialise(argc, argv, envp); \
      cls *pInstance = new cls(); \
      int terminationValue = pInstance->_main(); \
      delete pInstance; \
      return terminationValue; \
    }
#endif // P_VXWORKS

/*$MACRO PDECLARE_PROCESS(cls,ancestor,manuf,name,major,minor,status,build)
   This macro is used to declare the components necessary for a user PWLib
   process. This will declare the PProcess descendent class, eg PApplication,
   and create an instance of the class. See the #PCREATE_PROCESS# macro
   for more details.
 */
#define PDECLARE_PROCESS(cls,ancestor,manuf,name,major,minor,status,build) \
  class cls : public ancestor { \
    PCLASSINFO(cls, ancestor); \
    public: \
      cls() : ancestor(manuf, name, major, minor, status, build) { } \
    private: \
      virtual void Main(); \
  };


PLIST(PInternalTimerList, PTimer);

class PTimerList : PInternalTimerList // Want this to be private
/* This class defines a list of #PTimer# objects. It is primarily used
   internally by the library and the user should never create an instance of
   it. The #PProcess# instance for the application maintains an instance
   of all of the timers created so that it may decrements them at regular
   intervals.
 */
{
  PCLASSINFO(PTimerList, PInternalTimerList);

  public:
    PTimerList();
    // Create a new timer list

    PTimeInterval Process();
    /* Decrement all the created timers and dispatch to their callback
       functions if they have expired. The #PTimer::Tick()# function
       value is used to determine the time elapsed since the last call to
       Process().

       The return value is the number of milliseconds until the next timer
       needs to be despatched. The function need not be called again for this
       amount of time, though it can (and usually is).
       
       @return
       maximum time interval before function should be called again.
     */

  private:
    PMutex listMutex, processingMutex, inTimeoutMutex;
    // Mutual exclusion for multi tasking

    PTimeInterval lastSample;
    // The last system timer tick value that was used to process timers.

    PTimer * currentTimer;
    // The timer which is currently being handled

  friend class PTimer;
};


///////////////////////////////////////////////////////////////////////////////
// PProcess

/**This class represents an operating system process. This is a running
   "programme" in the  context of the operating system. Note that there can
   only be one instance of a PProcess class in a given programme.
   
   The instance of a PProcess or its GUI descendent #PApplication# is
   usually a static variable created by the application writer. This is the
   initial "anchor" point for all data structures in an application. As the
   application writer never needs to access the standard system
   #main()# function, it is in the library, the programmes
   execution begins with the virtual function #PThread::Main()# on a
   process.
 */
class PProcess : public PThread
{
  PCLASSINFO(PProcess, PThread);

  public:
  /**@name Construction */
  //@{
    /// Release status for the program.
    enum CodeStatus {
      /// Code is still very much under construction.
      AlphaCode,    
      /// Code is largely complete and is under test.
      BetaCode,     
      /// Code has all known bugs removed and is shipping.
      ReleaseCode,  
      NumCodeStatuses
    };

    /** Create a new process instance.
     */
    PProcess(
      const char * manuf = "",         ///< Name of manufacturer
      const char * name = "",          ///< Name of product
      WORD majorVersion = 1,           ///< Major version number of the product
      WORD minorVersion = 0,           ///< Minor version number of the product
      CodeStatus status = ReleaseCode, ///< Development status of the product
      WORD buildNumber = 1             ///< Build number of the product
    );
  //@}

  /**@name Overrides from class PObject */
  //@{
    /**Compare two process instances. This should almost never be called as
       a programme only has access to a single process, its own.

       @return
       #EqualTo# if the two process object have the same name.
     */
    Comparison Compare(
      const PObject & obj   ///< Other process to compare against.
    ) const;
  //@}

  /**@name Overrides from class PThread */
  //@{
    /**Terminate the process. Usually only used in abnormal abort situation.
     */
    virtual void Terminate();

    /** Get the name of the thread. Thread names are a optional debugging aid.

       @return
       current thread name.
     */
    virtual PString GetThreadName() const;

    /** Change the name of the thread. Thread names are a optional debugging aid.

       @return
       current thread name.
     */
    virtual void SetThreadName(
      const PString & name        ///< New name for the thread.
    );
  //@}

  /**@name Process information functions */
  //@{
    /**Get the current processes object instance. The {\it current process}
       is the one the application is running in.
       
       @return
       pointer to current process instance.
     */
    static PProcess & Current();

    /**Determine if the current processes object instance has been initialised.
       If this returns TRUE it is safe to use the PProcess::Current() function.
       
       @return
       TRUE if process class has been initialised.
     */
    static BOOL IsInitialised();

    /**Set the termination value for the process.
    
       The termination value is an operating system dependent integer which
       indicates the processes termiantion value. It can be considered a
       "return value" for an entire programme.
     */
    void SetTerminationValue(
      int value  ///< Value to return a process termination status.
    );

    /**Get the termination value for the process.
    
       The termination value is an operating system dependent integer which
       indicates the processes termiantion value. It can be considered a
       "return value" for an entire programme.
       
       @return
       integer termination value.
     */
    int GetTerminationValue() const;

    /**Get the programme arguments. Programme arguments are a set of strings
       provided to the programme in a platform dependent manner.
    
       @return
       argument handling class instance.
     */
    PArgList & GetArguments();

    /**Get the name of the manufacturer of the software. This is used in the
       default "About" dialog box and for determining the location of the
       configuration information as used by the #PConfig# class.

       The default for this information is the empty string.
    
       @return
       string for the manufacturer name eg "Equivalence".
     */
    virtual const PString & GetManufacturer() const;

    /**Get the name of the process. This is used in the
       default "About" dialog box and for determining the location of the
       configuration information as used by the #PConfig# class.

       The default is the title part of the executable image file.

       @return
       string for the process name eg "MyApp".
     */
    virtual const PString & GetName() const;

    /**Get the version of the software. This is used in the default "About"
       dialog box and for determining the location of the configuration
       information as used by the #PConfig# class.

       If the #full# parameter is TRUE then a version string
       built from the major, minor, status and build veriosn codes is
       returned. If FALSE then only the major and minor versions are
       returned.

       The default for this information is "1.0".
    
       @return
       string for the version eg "1.0b3".
     */
    virtual PString GetVersion(
      BOOL full = TRUE ///< TRUE for full version, FALSE for short version.
    ) const;

    /**Get the processes executable image file path.

       @return
       file path for program.
     */
    const PFilePath & GetFile() const;

    /**Get the platform dependent process identifier for the process. This is
       an arbitrary (and unique) integer attached to a process by the operating
       system.

       @return
       Process ID for process.
     */
    DWORD GetProcessID() const;

    /**Get the effective user name of the owner of the process, eg "root" etc.
       This is a platform dependent string only provided by platforms that are
       multi-user. Note that some value may be returned as a "simulated" user.
       For example, in MS-DOS an environment variable

       @return
       user name of processes owner.
     */
    PString GetUserName() const;

    /**Set the effective owner of the process.
       This is a platform dependent string only provided by platforms that are
       multi-user.

       For unix systems if the username may consist exclusively of digits and
       there is no actual username consisting of that string then the numeric
       uid value is used. For example "0" is the superuser. For the rare
       occassions where the users name is the same as their uid, if the
       username field starts with a '#' then the numeric form is forced.

       If an empty string is provided then original user that executed the
       process in the first place (the real user) is set as the effective user.

       The permanent flag indicates that the user will not be able to simple
       change back to the original user as indicated above, ie for unix
       systems setuid() is used instead of seteuid(). This is not necessarily
       meaningful for all platforms.

       @return
       TRUE if processes owner changed. The most common reason for failure is
       that the process does not have the privilege to change the effective user.
      */
    BOOL SetUserName(
      const PString & username, ///< New user name or uid
      BOOL permanent = FALSE    ///< Flag for if effective or real user
    );

    /**Get the effective group name of the owner of the process, eg "root" etc.
       This is a platform dependent string only provided by platforms that are
       multi-user. Note that some value may be returned as a "simulated" user.
       For example, in MS-DOS an environment variable

       @return
       group name of processes owner.
     */
    PString GetGroupName() const;

    /**Set the effective group of the process.
       This is a platform dependent string only provided by platforms that are
       multi-user.

       For unix systems if the groupname may consist exclusively of digits and
       there is no actual groupname consisting of that string then the numeric
       uid value is used. For example "0" is the superuser. For the rare
       occassions where the groups name is the same as their uid, if the
       groupname field starts with a '#' then the numeric form is forced.

       If an empty string is provided then original group that executed the
       process in the first place (the real group) is set as the effective
       group.

       The permanent flag indicates that the group will not be able to simply
       change back to the original group as indicated above, ie for unix
       systems setgid() is used instead of setegid(). This is not necessarily
       meaningful for all platforms.

       @return
       TRUE if processes group changed. The most common reason for failure is
       that the process does not have the privilege to change the effective
       group.
      */
    BOOL SetGroupName(
      const PString & groupname, ///< New group name or gid
      BOOL permanent = FALSE     ///< Flag for if effective or real group
    );

    /**Get the maximum file handle value for the process.
       For some platforms this is meaningless.

       @return
       user name of processes owner.
     */
    int GetMaxHandles() const;

    /**Set the maximum number of file handles for the process.
       For unix systems the user must be run with the approriate privileges
       before this function can set the value above the system limit.

       For some platforms this is meaningless.

       @return
       TRUE if successfully set the maximum file hadles.
      */
    BOOL SetMaxHandles(
      int newLimit  ///< New limit on file handles
    );

#ifdef P_CONFIG_FILE
    /**Get the default file to use in PConfig instances.
      */
    virtual PString GetConfigurationFile();
#endif

    /**Set the default file or set of directories to search for use in PConfig.
       To find the .ini file for use in the default PConfig() instance, this
       explicit filename is used, or if it is a set of directories separated
       by either ':' or ';' characters, then the application base name postfixed
       with ".ini" is searched for through those directories.

       The search is actually done when the GetConfigurationFile() is called,
       this function only sets the internal variable.

       Note for Windows, a path beginning with "HKEY_LOCAL_MACHINE\\" or
       "HKEY_CURRENT_USER\\" will actually search teh system registry for the
       application base name only (no ".ini") in that folder of the registry.
      */
    void SetConfigurationPath(
      const PString & path   ///< Explicit file or set of directories
    );
  //@}

  /**@name Operating System information functions */
  //@{
    /**Get the class of the operating system the process is running on, eg
       "unix".
       
       @return
       String for OS class.
     */
    static PString GetOSClass();

    /**Get the name of the operating system the process is running on, eg
       "Linux".
       
       @return
       String for OS name.
     */
    static PString GetOSName();

    /**Get the hardware the process is running on, eg "sparc".
       
       @return
       String for OS name.
     */
    static PString GetOSHardware();

    /**Get the version of the operating system the process is running on, eg
       "2.0.33".
       
       @return
       String for OS version.
     */
    static PString GetOSVersion();

    /**Get the configuration directory of the operating system the process is
       running on, eg "/etc" for Unix, "c:\windows" for Win95 or
       "c:\winnt\system32\drivers\etc" for NT.

       @return
       Directory for OS configuration files.
     */
    static PDirectory GetOSConfigDir();
  //@}

    PTimerList * GetTimerList();
    /* Get the list of timers handled by the application. This is an internal
       function and should not need to be called by the user.
       
       @return
       list of timers.
     */

    static void PreInitialise(
      int argc,     // Number of program arguments.
      char ** argv, // Array of strings for program arguments.
      char ** envp  // Array of string for the system environment
    );
    /* Internal initialisation function called directly from
       #_main()#. The user should never call this function.
     */

    static void PreShutdown();
    /* Internal shutdown function called directly from the ~PProcess
       #_main()#. The user should never call this function.
     */

    virtual int _main(void * arg = NULL);
    // Main function for process, called from real main after initialisation

    PTime GetStartTime() const;
    /* return the time at which the program was started 
    */

  private:
    void Construct();

  // Member variables
    static int p_argc;
    static char ** p_argv;
    static char ** p_envp;
    // main arguments

    int terminationValue;
    // Application return value

    PString manufacturer;
    // Application manufacturer name.

    PString productName;
    // Application executable base name from argv[0]

    WORD majorVersion;
    // Major version number of the product
    
    WORD minorVersion;
    // Minor version number of the product
    
    CodeStatus status;
    // Development status of the product
    
    WORD buildNumber;
    // Build number of the product

    PFilePath executableFile;
    // Application executable file from argv[0] (not open)

    PStringList configurationPaths;
    // Explicit file or set of directories to find default PConfig

    PArgList arguments;
    // The list of arguments

    PTimerList timers;
    // List of active timers in system

    PTime programStartTime;
    // time at which process was intantiated, i.e. started

    int maxHandles;
    // Maximum number of file handles process can open.


  friend class PThread;


// Include platform dependent part of class
#ifdef _WIN32
#include "msos/ptlib/pprocess.h"
#else
#include "unix/ptlib/pprocess.h"
#endif
};

/*
 *  one instance of this class (or any descendants) will be instantiated
 *  via PGenericFactory<PProessStartup> one "main" has been started, and then
 *  the OnStartup() function will be called. The OnShutdown function will
 *  be called after main exits, and the instances will be destroyed if they
 *  are not singletons
 */
class PProcessStartup : public PObject
{
  PCLASSINFO(PProcessStartup, PObject)
  public:
    virtual void OnStartup()  { }
    virtual void OnShutdown() { }
};

typedef PFactory<PProcessStartup> PProcessStartupFactory;

// using an inline definition rather than a #define crashes gcc 2.95. Go figure
#define P_DEFAULT_TRACE_OPTIONS ( PTrace::Blocks | PTrace::Timestamp | PTrace::Thread | PTrace::FileAndLine )

template <unsigned _level, unsigned _options = P_DEFAULT_TRACE_OPTIONS >
class PTraceLevelSetStartup : public PProcessStartup
{
  public:
    void OnStartup()
    { PTrace::Initialise(_level, NULL, _options); }
};

#endif

// End Of File ///////////////////////////////////////////////////////////////
