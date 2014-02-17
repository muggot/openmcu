/*
 * svcproc.h
 *
 * Service Process (daemon) class.
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
 * $Log: svcproc.h,v $
 * Revision 1.26  2006/06/21 03:28:41  csoutheren
 * Various cleanups thanks for Frederic Heem
 *
 * Revision 1.25  2005/11/25 03:43:47  csoutheren
 * Fixed function argument comments to be compatible with Doxygen
 *
 * Revision 1.24  2003/09/17 05:41:59  csoutheren
 * Removed recursive includes
 *
 * Revision 1.23  2003/09/17 01:18:02  csoutheren
 * Removed recursive include file system and removed all references
 * to deprecated coooperative threading support
 *
 * Revision 1.22  2002/10/22 07:42:52  robertj
 * Added extra debugging for file handle and thread leak detection.
 *
 * Revision 1.21  2002/09/16 01:08:59  robertj
 * Added #define so can select if #pragma interface/implementation is used on
 *   platform basis (eg MacOS) rather than compiler, thanks Robert Monaghan.
 *
 * Revision 1.20  2002/01/26 23:55:55  craigs
 * Changed for GCC 3.0 compatibility, thanks to manty@manty.net
 *
 * Revision 1.19  2001/05/22 12:49:32  robertj
 * Did some seriously wierd rewrite of platform headers to eliminate the
 *   stupid GNU compiler warning about braces not matching.
 *
 * Revision 1.18  1999/09/21 08:20:16  robertj
 * Fixed name space problem with PSYSTEMLOG() macro.
 *
 * Revision 1.17  1999/09/13 13:15:06  robertj
 * Changed PTRACE so will output to system log in PServiceProcess applications.
 *
 * Revision 1.16  1999/03/09 02:59:51  robertj
 * Changed comments to doc++ compatible documentation.
 *
 * Revision 1.15  1999/02/16 08:11:17  robertj
 * MSVC 6.0 compatibility changes.
 *
 * Revision 1.14  1998/10/13 14:06:15  robertj
 * Complete rewrite of memory leak detection code.
 *
 * Revision 1.13  1998/09/23 06:21:31  robertj
 * Added open source copyright license.
 *
 * Revision 1.12  1998/04/07 13:33:21  robertj
 * Changed startup code to support PApplication class.
 *
 * Revision 1.11  1998/03/29 06:16:50  robertj
 * Rearranged initialisation sequence so PProcess descendent constructors can do "things".
 *
 * Revision 1.10  1998/02/16 00:13:16  robertj
 * Added tray icon support.
 *
 * Revision 1.9  1998/02/03 06:19:14  robertj
 * Added extra log levels.
 *
 * Revision 1.8  1997/07/08 13:02:32  robertj
 * DLL support.
 *
 * Revision 1.7  1997/02/05 11:51:15  robertj
 * Changed current process function to return reference and validate objects descendancy.
 *
 * Revision 1.6  1996/08/19 13:39:20  robertj
 * Added "Debug" level to system log.
 * Moved PSYSTEMLOG macro to common code.
 * Changed PSYSTEMLOG macro so does not execute << expression if below debug level.
 * Fixed memory leak in PSystemLog stream buffer.
 *
 * Revision 1.5  1996/08/17 10:00:27  robertj
 * Changes for Windows DLL support.
 *
 * Revision 1.4  1996/08/09 11:16:53  robertj
 * Moved log macro to platform dependent header.
 *
 * Revision 1.3  1996/07/30 12:24:13  robertj
 * Added SYSTEMLOG macro for GNU compiler compatibility.
 *
 * Revision 1.2  1996/07/27 04:10:06  robertj
 * Changed SystemLog to be stream based rather than printf based.
 *
 * Revision 1.1  1995/12/23 03:47:25  robertj
 * Initial revision
 *
 * Revision 1.3  1995/12/10 11:50:05  robertj
 * Numerous fixes for WIN32 service processes.
 *
 * Revision 1.2  1995/07/02 01:23:27  robertj
 * Set up service process to be in subthread not main thread.
 *
 * Revision 1.1  1995/06/17 00:50:54  robertj
 * Initial revision
 *
 */

#ifndef _PSERVICEPROCESS
#define _PSERVICEPROCESS

#ifdef P_USE_PRAGMA
#pragma interface
#endif

#include <ptlib/pprocess.h>

/** This class abstracts the operating system dependent error logging facility.
To send messages to the system error log, the PSYSTEMLOG macro should be used. 
  */

class PSystemLog : public PObject, public iostream {
  PCLASSINFO(PSystemLog, PObject);

  public:
  /**@name Construction */
  //@{
    /// define the different error log levels
    enum Level {
      /// Log from standard error stream
      StdError = -1,
      /// Log a fatal error
      Fatal,   
      /// Log a non-fatal error
      Error,    
      /// Log a warning
      Warning,  
      /// Log general information
      Info,     
      /// Log debugging information
      Debug,    
      /// Log more debugging information
      Debug2,   
      /// Log even more debugging information
      Debug3,   
      /// Log a lot of debugging information
      Debug4,   
      /// Log a real lot of debugging information
      Debug5,   
      /// Log a bucket load of debugging information
      Debug6,   

      NumLogLevels
    };

    /// Create a system log stream
    PSystemLog(
     Level level   ///< only messages at this level or higher will be logged
    ) : iostream(cout.rdbuf()) { logLevel = level; buffer.log = this; init(&buffer); }

    /// Destroy the string stream, deleting the stream buffer
    ~PSystemLog() { flush(); }
  //@}

  /**@name Output functions */
  //@{
    /** Log an error into the system log.
     */
    static void Output(
      Level level,      ///< Log level for this log message.
      const char * msg  ///< Message to be logged
    );
  //@}

  /**@name Miscellaneous functions */
  //@{
    /** Set the level at which errors are logged. Only messages higher than or
       equal to the specified level will be logged.
      */
    void SetLevel(
      Level level  ///< New log level
    ) { logLevel = level; }

    /** Get the current level for logging.

       @return
       Log level.
     */
    Level GetLevel() const { return logLevel; }
  //@}

  private:
    PSystemLog(const PSystemLog &) : iostream(cout.rdbuf()) { }
    PSystemLog & operator=(const PSystemLog &) { return *this; }

    class Buffer : public streambuf {
      public:
        virtual int overflow(int=EOF);
        virtual int underflow();
        virtual int sync();
        PSystemLog * log;
        PString string;
    } buffer;
    friend class Buffer;

    Level logLevel;
};


/** Log a message to the system log.
The current log level is checked and if allowed, the second argument is evaluated
as a stream output sequence which is them output to the system log.
*/
#define PSYSTEMLOG(level, variables) \
  if (PServiceProcess::Current().GetLogLevel() >= PSystemLog::level) { \
    PSystemLog P_systemlog(PSystemLog::level); \
    P_systemlog << variables; \
  } else (void)0



/** A process type that runs as a "background" service.
    This may be a service under the Windows NT operating system, or a "daemon" under Unix, or a hidden application under Windows.
 */
class PServiceProcess : public PProcess
{
  PCLASSINFO(PServiceProcess, PProcess);

  public:
  /**@name Construction */
  //@{
    /** Create a new service process.
     */
    PServiceProcess(
      const char * manuf,   ///< Name of manufacturer
      const char * name,    ///< Name of product
      WORD majorVersion,    ///< Major version number of the product
      WORD minorVersion,    ///< Minor version number of the product
      CodeStatus status,    ///< Development status of the product
      WORD buildNumber      ///< Build number of the product
    );
  //@}

  /**@name Callback functions */
  //@{
    /** Called when the service is started. This typically initialises the
       service and returns TRUE if the service is ready to run. The
       #Main()# function is then executed.

       @return
       TRUE if service may start, FALSE if an initialisation failure occurred.
     */
    virtual BOOL OnStart() = 0;

    /** Called by the system when the service is stopped. One return from this
       function there is no guarentee that any more user code will be executed.
       Any cleaning up or closing of resource must be done in here.
     */
    virtual void OnStop();

    /** Called by the system when the service is to be paused. This will
       suspend any actions that the service may be executing. Usually this is
       less expensive in resource allocation etc than stopping and starting
       the service.

       @return
       TRUE if the service was successfully paused.
     */
    virtual BOOL OnPause();

    /** Resume after the service was paused.
     */
    virtual void OnContinue();

    /** The Control menu option was used in the SysTray menu.
     */
    virtual void OnControl() = 0;
  //@}

  /**@name Miscellaneous functions */
  //@{
    /** Get the current service process object.

       @return
       Pointer to service process.
     */
    static PServiceProcess & Current();


    /** Set the level at which errors are logged. Only messages higher than or
       equal to the specified level will be logged.
    
       The default is #LogError# allowing fatal errors and ordinary\
       errors to be logged and warning and information to be ignored.

       If in debug mode then the default is #LogInfo# allowing all
       messages to be displayed.
     */
    void SetLogLevel(
      PSystemLog::Level level  ///< New log level
    ) { currentLogLevel = level; }

    /** Get the current level for logging.

       @return
       Log level.
     */
    PSystemLog::Level GetLogLevel() const { return currentLogLevel; }
  //@}


    /* Internal initialisation function called directly from
       #main()#. The user should never call this function.
     */
    virtual int _main(void * arg = NULL);


  protected:
  // Member variables
    /// Flag to indicate service is run in simulation mode.
    BOOL debugMode;

    /// Current log level for #PSYSTEMLOG# calls.
    PSystemLog::Level currentLogLevel;

    friend void PSystemLog::Output(PSystemLog::Level, const char *);


// Include platform dependent part of class
#ifdef _WIN32
#include "msos/ptlib/svcproc.h"
#else
#include "unix/ptlib/svcproc.h"
#endif
};

#endif


// End Of File ///////////////////////////////////////////////////////////////
