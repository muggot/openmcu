/*
 * osutils.cxx
 *
 * Operating System utilities.
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
 * $Log: osutils.cxx,v $
 * Revision 1.262  2007/09/17 12:45:17  rjongbloed
 * Fixed DevStudio 2003 build.
 *
 * Revision 1.261  2007/09/17 11:14:46  rjongbloed
 * Added "No Trace" build configuration.
 *
 * Revision 1.260  2007/09/17 05:30:43  rjongbloed
 * Added thread local storage in tracing to avoid a certain class of deadlocks.
 *
 * Revision 1.259  2007/09/12 18:28:40  ykiryanov
 * Added code to retrieve module name for WCE based projects
 *
 * Revision 1.258  2007/09/09 09:40:26  rjongbloed
 * Prevented memory leak detection from considering anything
 *   allocated before the PProcess constructor is complete.
 *
 * Revision 1.257  2007/09/08 11:34:29  rjongbloed
 * Improved memory checking (leaks etc), especially when using MSVC debug library.
 *
 * Revision 1.256  2007/09/08 08:40:58  hfriederich
 * Make PTRACE pthread_mutex recursive to avoid deadlocks
 *
 * Revision 1.255  2007/09/06 00:01:19  rjongbloed
 * Fixed recursion problem in initialising PTRACE under linux.
 *
 * Revision 1.254  2007/09/04 02:00:03  rjongbloed
 * Added environment variables PWLIB_TRACE_LEVEL, PWLIB_TRACE_OPTIONS
 *   and PWLIB_TRACE_FILE so can get tracing during early stages of application
 *   initialisation, or from applications that do not allow tracing to be adjusted.
 *
 * Revision 1.253  2007/07/06 02:44:33  csoutheren
 * Fixed compile on Linux
 *
 * Revision 1.252  2007/04/13 07:19:23  rjongbloed
 * Removed separate Win32 solution for "plug in static loading" issue,
 *   and used the PLOAD_FACTORY() mechanism for everything.
 * Slight clean up of the PLOAD_FACTORY macro.
 *
 * Revision 1.251  2007/04/04 06:09:05  ykiryanov
 * This is a first cut of Windows Mobile 5.0 PocketPC SDK ARM4I port
 *
 * Revision 1.250  2007/04/04 03:22:52  rjongbloed
 * Fixed unix compile issue
 *
 * Revision 1.249  2007/04/04 01:51:38  rjongbloed
 * Reviewed and adjusted PTRACE log levels
 *   Now follows 1=error,2=warn,3=info,4+=debug
 *
 * Revision 1.248  2007/03/29 07:04:09  rjongbloed
 * Fixed trace log stopping all output if ever get a iostream output error (failbit)
 *
 * Revision 1.247  2007/02/19 04:38:46  csoutheren
 * Fixed problem in Shift when only one arg
 *
 * Revision 1.246  2006/11/26 08:02:13  rjongbloed
 * Added ability to send trace output to debugger output window with
 *   a psudeo-filename DEBUGSTREAM.
 *
 * Revision 1.245  2006/10/31 04:10:40  csoutheren
 * Make sure PVidFileDev class is loaded, and make it work with OPAL
 *
 * Revision 1.244  2006/07/19 05:37:39  csoutheren
 * Applied 1523190 - PWLIB - Delayed Application Startup
 * Thanks to Ben Lear
 *
 * Revision 1.243  2006/07/14 04:55:10  csoutheren
 * Applied 1520151 - Adds PID to tracefile + Rolling Date pattern
 * Thanks to Paul Nader
 *
 * Revision 1.242  2006/06/26 10:50:24  shorne
 * Fixed compile problem on MSVC6
 *
 * Revision 1.241  2006/06/21 03:28:44  csoutheren
 * Various cleanups thanks for Frederic Heem
 *
 * Revision 1.240  2006/06/20 08:13:52  csoutheren
 * Applied part of patch 1469188
 * PTrace Rotate Daily
 * Thanks to Dave Moss
 *
 * Revision 1.239  2006/05/23 22:28:11  csoutheren
 * Add timer protection for backwards running clocks
 *
 * Revision 1.238  2006/05/23 00:57:30  csoutheren
 * Fix race condition in timer startup (maybe)
 *
 * Revision 1.237  2005/12/04 22:43:30  csoutheren
 * Cleanup patches from Kilian Krause
 *
 * Revision 1.236  2005/11/30 12:47:42  csoutheren
 * Removed tabs, reformatted some code, and changed tags for Doxygen
 *
 * Revision 1.235  2005/11/09 09:11:39  csoutheren
 * Moved Windows-specific AttachThreadInput callsto seperate member function
 * on PThread. This removes a linearly increasing delay in creating new threads
 *
 * Revision 1.234  2005/10/22 04:50:23  csoutheren
 * Fixed hole in mutex locking of PTrace
 *
 * Revision 1.233  2005/08/30 06:36:39  csoutheren
 * Added ability to rotate output logs on a daily basis
 *
 * Revision 1.232  2005/03/19 02:52:55  csoutheren
 * Fix warnings from gcc 4.1-20050313 shapshot
 *
 * Revision 1.231  2005/01/31 08:05:40  csoutheren
 * More patches for MacOSX, thanks to Hannes Friederich
 *
 * Revision 1.230  2005/01/26 05:37:59  csoutheren
 * Added ability to remove config file support
 *
 * Revision 1.229  2005/01/04 07:44:03  csoutheren
 * More changes to implement the new configuration methodology, and also to
 * attack the global static problem
 *
 * Revision 1.228  2004/08/16 06:40:59  csoutheren
 * Added adapters template to make device plugins available via the abstract factory interface
 *
 * Revision 1.227  2004/06/30 12:17:06  rjongbloed
 * Rewrite of plug in system to use single global variable for all factories to avoid all sorts
 *   of issues with startup orders and Windows DLL multiple instances.
 *
 * Revision 1.226  2004/06/03 13:30:58  csoutheren
 * Renamed INSTANTIATE_FACTORY to avoid potential namespace collisions
 * Added documentaton on new PINSTANTIATE_FACTORY macro
 * Added generic form of PINSTANTIATE_FACTORY
 *
 * Revision 1.225  2004/06/03 12:47:59  csoutheren
 * Decomposed PFactory declarations to hopefully avoid problems with Windows DLLs
 *
 * Revision 1.224  2004/06/01 05:22:44  csoutheren
 * Restored memory check functionality
 *
 * Revision 1.223  2004/05/23 12:34:38  rjongbloed
 * Fixed PProcess startup up execution to after PProcess instance is created
 *   so does not crash if using startup technique to initialise tracing.
 *
 * Revision 1.222  2004/05/21 00:28:39  csoutheren
 * Moved PProcessStartup creation to PProcess::Initialise
 * Added PreShutdown function and called it from ~PProcess to handle PProcessStartup removal
 *
 * Revision 1.221  2004/05/18 21:49:25  csoutheren
 * Added ability to display trace output from program startup via environment
 * variable or by application creating a PProcessStartup descendant
 *
 * Revision 1.220  2004/05/18 12:43:31  csoutheren
 * Fixed compile problem on MSVC 6
 *
 * Revision 1.219  2004/05/18 06:01:06  csoutheren
 * Deferred plugin loading until after main has executed by using abstract factory classes
 *
 * Revision 1.218  2004/05/18 02:32:09  csoutheren
 * Fixed linking problems with PGenericFactory classes
 *
 * Revision 1.217  2004/05/13 15:05:43  csoutheren
 * Added <vector.h>
 *
 * Revision 1.216  2004/05/13 14:54:57  csoutheren
 * Implement PProcess startup and shutdown handling using abstract factory classes
 *
 * Revision 1.215  2004/04/24 06:27:56  rjongbloed
 * Fixed GCC 3.4.0 warnings about PAssertNULL and improved recoverability on
 *   NULL pointer usage in various bits of code.
 *
 * Revision 1.214  2004/04/12 07:33:46  csoutheren
 * Temporarily disabled removal of empty TRACE output on Linux
 *
 * Revision 1.213  2004/04/03 08:22:21  csoutheren
 * Remove pseudo-RTTI and replaced with real RTTI
 *
 * Revision 1.212  2004/04/03 06:54:29  rjongbloed
 * Many and various changes to support new Visual C++ 2003
 *
 * Revision 1.211  2004/03/22 10:15:27  rjongbloed
 * Added classes similar to PWaitAndSignal to automatically unlock a PReadWriteMutex
 *   when goes out of scope.
 *
 * Revision 1.210  2004/03/20 09:08:15  rjongbloed
 * Changed interaction between PTrace and PSystemLog so that the tracing code does
 *   not need to know about the system log, thus reducing the code footprint for most apps.
 *
 * Revision 1.209  2003/11/13 21:42:32  csoutheren
 * Fixed problem with thread name display under Windows thanks to Ted Szoczei
 *
 * Revision 1.208  2003/11/08 01:42:19  rjongbloed
 * Added thread names to DevStudio display, thanks Ted Szoczei
 *
 * Revision 1.207  2003/09/17 09:02:14  csoutheren
 * Removed memory leak detection code
 *
 * Revision 1.206  2003/09/17 01:18:03  csoutheren
 * Removed recursive include file system and removed all references
 * to deprecated coooperative threading support
 *
 * Revision 1.205  2003/07/24 22:01:42  dereksmithies
 * Add fixes from Peter Nixon  for fixing install problems. Thanks.
 *
 * Revision 1.204  2003/03/27 07:27:44  robertj
 * Added function to get a bunch of arguments as a string array.
 *
 * Revision 1.203  2003/01/24 10:21:06  robertj
 * Fixed issues in RTEMS support, thanks Vladimir Nesic
 *
 * Revision 1.202  2002/12/18 05:31:31  robertj
 * Moved PTimeInterval::GetInterval() to common code.
 *
 * Revision 1.201  2002/12/11 03:23:27  robertj
 * Fixed deadlock in read/write mutex, at price of not having seemless upgrading
 *   of read lock to write lock. There is now a window in which some other
 *   thread may gain write lock from the thread that was trying to upgrade.
 *
 * Revision 1.200  2002/12/10 02:39:07  robertj
 * Avoid odd trace output to stderr before trace file is set.
 *
 * Revision 1.199  2002/10/09 00:46:19  robertj
 * Changed PThread::Create() so does not return PThread pointer if the thread
 *   has been created auto-delete, the pointer is extremely dangerous to use
 *   as it could be deleted at any time, so to remove temptation ...
 *
 * Revision 1.198  2002/10/08 03:35:15  robertj
 * Fixed BSD warning
 *
 * Revision 1.197  2002/10/04 08:21:26  robertj
 * Changed read/write mutex so can be called by same thread without deadlock.
 *
 * Revision 1.196  2002/07/30 02:55:09  craigs
 * Added program start time to PProcess
 *
 * Revision 1.195  2002/06/27 06:38:58  robertj
 * Changes to remove memory leak display for things that aren't memory leaks.
 *
 * Revision 1.194  2002/06/15 02:16:36  robertj
 * Fixed bug (from rev 1.190) so can now use PTimer::Reset() after the timer
 *   had previously expired (resetTiem was being zeroed), thanks Ted Szoczei
 *
 * Revision 1.193  2002/06/05 12:29:15  craigs
 * Changes for gcc 3.1
 *
 * Revision 1.192  2002/06/04 00:25:31  robertj
 * Fixed incorrectly initialised trace indent, thanks Artis Kugevics
 *
 * Revision 1.191  2002/05/31 04:10:44  robertj
 * Fixed missing mutex in PTrace::SetStream, thanks Federico Pinna
 *
 * Revision 1.190  2002/05/28 13:05:26  robertj
 * Fixed PTimer::SetInterval so it restarts timer as per operator=()
 *
 * Revision 1.189  2002/05/22 00:42:03  craigs
 * Added GMTTime flag to tracing options
 *
 * Revision 1.188  2002/05/01 03:45:09  robertj
 * Added initialisation of PreadWriteMutex and changed slightly to agree
 *   with the text book definition of a semaphore for one of the mutexes.
 *
 * Revision 1.187  2002/04/30 06:21:38  robertj
 * Fixed PReadWriteMutex class to implement text book algorithm!
 *
 * Revision 1.186  2002/04/30 03:39:21  robertj
 * Changed PTimer::Stop() so does not return until timer is REALLY stopped, in
 *   particular when a possibly executing OnTimeout() function has completed.
 *
 * Revision 1.185  2002/04/24 01:19:07  robertj
 * Added milliseconds to PTRACE output timestamp
 *
 * Revision 1.184  2002/04/24 01:10:28  robertj
 * Fixed problem with PTRACE_BLOCK indent level being correct across threads.
 *
 * Revision 1.183  2002/04/19 00:43:17  craigs
 * Fixed problem with file modes
 *
 * Revision 1.182  2002/04/19 00:20:31  craigs
 * Added option to append to log file rather than create anew each time
 *
 * Revision 1.181  2002/02/14 05:14:51  robertj
 * Fixed possible deadlock if a timer is deleted (however indirectly) in the
 *   OnTimeout of another timer.
 *
 * Revision 1.180  2002/02/11 04:07:00  robertj
 * Fixed possibly race condition in PTRACE of first message. Consequence is
 *   that cannot PTRACE until have PProcess, ie before main() is executed.
 *
 * Revision 1.179  2002/01/31 08:14:16  robertj
 * Put back code taken out by GCC 3.0 patch. It really SHOULD be there!
 *
 * Revision 1.178  2002/01/26 23:57:45  craigs
 * Changed for GCC 3.0 compatibility, thanks to manty@manty.net
 *
 * Revision 1.177  2002/01/26 15:04:44  yurik
 * Fixed trace stream to a file
 *
 * Revision 1.176  2001/12/18 23:22:54  robertj
 * Fixed problem for if excecutable is "renamed" using unix exec() and the
 *   argv0 does not point to executable file.
 *
 * Revision 1.175  2001/12/15 04:49:17  robertj
 * Added stream I/O functions for argument list.
 *
 * Revision 1.174  2001/12/14 00:42:56  robertj
 * Fixed unix compatibility with trace of threads not created by pwlib.
 *
 * Revision 1.173  2001/12/13 09:21:43  robertj
 * Changed trace so shows thread id if current thread not created by PWLib.
 *
 * Revision 1.172  2001/11/30 04:19:26  robertj
 * Fixed correct setting of option bits in PTrace::Initialise()
 * Added date and time to first message in PTrace::Initialise()
 *
 * Revision 1.171  2001/11/14 06:06:26  robertj
 * Added functions on PTimer to get reset value and restart timer to it.
 *
 * Revision 1.170  2001/10/15 00:48:02  robertj
 * Fixed warning on later MSVC compilers.
 *
 * Revision 1.169  2001/09/03 08:08:31  robertj
 * Added tab so get extra "column" in trace output.
 *
 * Revision 1.168  2001/08/20 06:56:47  robertj
 * Fixed memory leak report which isn't a memory leak.
 *
 * Revision 1.167  2001/07/20 04:14:19  robertj
 * Removed GNU warning.
 *
 * Revision 1.166  2001/05/29 02:50:56  robertj
 * Fixed GNU compatibility
 *
 * Revision 1.165  2001/05/29 00:49:18  robertj
 * Added ability to put in a printf %x in thread name to get thread object
 *   address into user settable thread name.
 *
 * Revision 1.164  2001/05/03 06:26:22  robertj
 * Fixed strange problem that occassionally crashes on exit. Mutex cannot be
 *   destroyed before program exit.
 *
 * Revision 1.163  2001/04/27 01:05:26  yurik
 * Exit crash removal try
 *
 * Revision 1.162  2001/04/15 03:39:24  yurik
 * Removed shutdown flag. Use IsTerminated() instead
 *
 * Revision 1.161  2001/04/14 04:53:01  yurik
 * Got rid of init_seg pragma and added process shutdown flag
 *
 * Revision 1.160  2001/03/23 20:28:54  yurik
 * Got rid of pragma warning for WinCE port
 *
 * Revision 1.159  2001/03/09 05:50:48  robertj
 * Added ability to set default PConfig file or path to find it.
 *
 * Revision 1.158  2001/03/02 22:29:08  yurik
 * New pragma for WinCE related port which enables (de)construction of library static objects be before applications'
 * Eliminated nasty access violation stemmed from using static PMutex object in PTrace code. Thanks to Yuriy Gorvitovskiy.
 *
 * Revision 1.157  2001/02/22 22:31:44  robertj
 * Changed PProcess version display to show build number even if zero.
 *
 * Revision 1.156  2001/02/22 08:16:42  robertj
 * Added standard trace file setup subroutine.
 *
 * Revision 1.155  2001/02/19 03:44:20  robertj
 * Changed "pl" in version number string to a simple ".", Now like 1.1.19
 *
 * Revision 1.154  2001/01/28 00:53:00  yurik
 * WinCE port-related - streams refined
 *
 * Revision 1.153  2001/01/25 07:14:39  robertj
 * Fixed spurios memory leak message. Usual static global problem.
 *
 * Revision 1.152  2001/01/24 18:27:44  yurik
 * Added if !#defined WinCE when asm code used. Asm can't be used under WCE
 *
 * Revision 1.151  2001/01/02 07:47:44  robertj
 * Fixed very narrow race condition in timers (destroyed while in OnTimeout()).
 *
 * Revision 1.150  2000/12/21 12:37:03  craigs
 * Fixed deadlock problem with creating PTimer inside OnTimeout
 *
 * Revision 1.149  2000/11/28 12:55:37  robertj
 * Added static function to create a new thread class and automatically
 *   run a function on another class in the context of that thread.
 *
 * Revision 1.148  2000/10/20 05:31:32  robertj
 * Added function to change auto delete flag on a thread.
 *
 * Revision 1.147  2000/08/31 01:12:36  robertj
 * Fixed problem with no new lines in trace output to stderr.
 *
 * Revision 1.146  2000/08/30 05:56:07  robertj
 * Fixed free running timers broken by previous change.
 *
 * Revision 1.145  2000/08/30 03:17:00  robertj
 * Improved multithreaded reliability of the timers under stress.
 *
 * Revision 1.144  2000/06/26 11:17:20  robertj
 * Nucleus++ port (incomplete).
 *
 * Revision 1.143  2000/06/26 09:27:16  robertj
 * Added ability to get at the PTraceStream without timestamps etc, use UINT_MAX trace level.
 *
 * Revision 1.142  2000/06/02 01:38:07  craigs
 * Fixed typos
 *
 * Revision 1.141  2000/06/02 01:35:56  craigs
 * Added more guards for NULL PStrings in PConfigArg handling
 *
 * Revision 1.140  2000/05/25 14:45:07  robertj
 * Fixed detection of real argument over configured value.
 *
 * Revision 1.139  2000/05/25 13:47:51  robertj
 * Fixed warning with GNU.
 *
 * Revision 1.138  2000/05/25 11:05:55  robertj
 * Added PConfigArgs class so can save program arguments to config files.
 *
 * Revision 1.137  2000/05/05 10:08:29  robertj
 * Fixed some GNU compiler warnings
 *
 * Revision 1.136  2000/04/28 06:58:50  robertj
 * Fixed bug introduced when added Ashley Untts fix, forgot to take out old code!
 *
 * Revision 1.135  2000/04/27 04:19:27  robertj
 * Fixed bug in restarting free running timers, thanks Ashley Unitt.
 *
 * Revision 1.134  2000/04/03 18:42:40  robertj
 * Added function to determine if PProcess instance is initialised.
 *
 * Revision 1.133  2000/03/29 20:12:00  robertj
 * Fixed GNU C++ warning
 *
 * Revision 1.132  2000/03/29 01:55:52  robertj
 * Fixed infinite recursion on PProcess::Current() = NULL assertion.
 *
 * Revision 1.131  2000/03/02 05:43:12  robertj
 * Fixed handling of NULL pointer on current thread in PTRACE output.
 *
 * Revision 1.130  2000/02/29 12:26:14  robertj
 * Added named threads to tracing, thanks to Dave Harvey
 *
 * Revision 1.129  2000/02/17 11:34:28  robertj
 * Changed PTRACE output to help line up text after filename output.
 *
 * Revision 1.128  2000/01/06 14:09:42  robertj
 * Fixed problems with starting up timers,losing up to 10 seconds
 *
 * Revision 1.127  1999/10/19 09:21:30  robertj
 * Added functions to get current trace options and level.
 *
 * Revision 1.126  1999/10/14 08:08:27  robertj
 * Fixed problem, assuring millisecond accuracy in timestamp of trace output.
 *
 * Revision 1.125  1999/09/14 13:02:52  robertj
 * Fixed PTRACE to PSYSTEMLOG conversion problem under Unix.
 *
 * Revision 1.124  1999/09/13 13:15:07  robertj
 * Changed PTRACE so will output to system log in PServiceProcess applications.
 *
 * Revision 1.123  1999/08/22 12:54:35  robertj
 * Fixed warnings about inlines on older GNU compiler
 *
 * Revision 1.122  1999/06/23 14:19:46  robertj
 * Fixed core dump problem with SIGINT/SIGTERM terminating process.
 *
 * Revision 1.121  1999/06/14 07:59:38  robertj
 * Enhanced tracing again to add options to trace output (timestamps etc).
 *
 * Revision 1.120  1999/04/26 08:06:51  robertj
 * Added missing function in cooperative threading.
 *
 * Revision 1.119  1999/03/01 13:51:30  craigs
 * Fixed ugly little bug in the cooperative multithreading that meant that threads blocked
 * on timers didn't always get rescheduled.
 *
 * Revision 1.118  1999/02/23 10:13:31  robertj
 * Changed trace to only diplay filename and not whole path.
 *
 * Revision 1.117  1999/02/23 07:11:27  robertj
 * Improved trace facility adding trace levels and #define to remove all trace code.
 *
 * Revision 1.116  1998/11/30 12:45:54  robertj
 * Fissioned into pchannel.cxx and pconfig.cxx
 *
 * Revision 1.115  1998/11/24 01:17:33  robertj
 * Type discrepency between declaration and definition for PFile::SetPosition
 *
 * Revision 1.114  1998/11/06 02:37:53  robertj
 * Fixed the fix for semaphore timeout race condition.
 *
 * Revision 1.113  1998/11/03 10:52:19  robertj
 * Fixed bug in semaphores with timeout saying timed out when really signalled.
 *
 * Revision 1.112  1998/11/03 03:44:05  robertj
 * Fixed missng strings on multiple parameters of same letter.
 *
 * Revision 1.111  1998/11/02 10:13:01  robertj
 * Removed GNU warning.
 *
 * Revision 1.110  1998/11/01 04:56:53  robertj
 * Added BOOl return value to Parse() to indicate there are parameters available.
 *
 * Revision 1.109  1998/10/31 14:02:20  robertj
 * Removed StartImmediate capability as causes race condition in preemptive version.
 *
 * Revision 1.108  1998/10/31 12:47:10  robertj
 * Added conditional mutex and read/write mutex thread synchronisation objects.
 *
 * Revision 1.107  1998/10/30 12:24:15  robertj
 * Added ability to get all key values as a dictionary.
 * Fixed warnings in GNU C.
 *
 * Revision 1.106  1998/10/30 11:22:15  robertj
 * Added constructors that take strings as well as const char *'s.
 *
 * Revision 1.105  1998/10/30 05:25:09  robertj
 * Allow user to shift past some arguments before parsing for the first time.
 *
 * Revision 1.104  1998/10/29 05:35:17  robertj
 * Fixed porblem with GetCount() == 0 if do not call Parse() function.
 *
 * Revision 1.103  1998/10/28 03:26:43  robertj
 * Added multi character arguments (-abc style) and options precede parameters mode.
 *
 * Revision 1.102  1998/10/28 00:59:49  robertj
 * New improved argument parsing.
 *
 * Revision 1.101  1998/10/19 00:19:59  robertj
 * Moved error and trace stream functions to common code.
 *
 * Revision 1.100  1998/10/18 14:28:45  robertj
 * Renamed argv/argc to eliminate accidental usage.
 *
 * Revision 1.99  1998/10/13 14:06:28  robertj
 * Complete rewrite of memory leak detection code.
 *
 * Revision 1.98  1998/09/24 07:23:54  robertj
 * Moved structured fiel into separate module so don't need silly implementation file for GNU C.
 *
 * Revision 1.97  1998/09/23 06:22:24  robertj
 * Added open source copyright license.
 *
 * Revision 1.96  1998/06/13 15:11:56  robertj
 * Added stack check in Yield().
 * Added immediate schedule of semaphore timeout thread.
 *
 * Revision 1.95  1998/05/30 13:28:18  robertj
 * Changed memory check code so global statics are not included in leak check.
 * Fixed deadlock in cooperative threading.
 * Added PSyncPointAck class.
 *
 * Revision 1.94  1998/05/25 09:05:56  robertj
 * Fixed close of channels on destruction.
 *
 * Revision 1.93  1998/04/07 13:33:33  robertj
 * Changed startup code to support PApplication class.
 *
 * Revision 1.92  1998/03/29 06:16:45  robertj
 * Rearranged initialisation sequence so PProcess descendent constructors can do "things".
 *
 * Revision 1.91  1998/03/20 03:18:17  robertj
 * Added special classes for specific sepahores, PMutex and PSyncPoint.
 *
 * Revision 1.90  1998/02/05 13:33:12  robertj
 * Fixed close of non-autodelete PIndirectChannels
 *
 * Revision 1.89  1998/02/03 06:19:55  robertj
 * Added new function to read a block with minimum number of bytes.
 *
 * Revision 1.88  1998/01/26 00:47:13  robertj
 * Added functions to get/set 64bit integers from a PConfig.
 *
 * Revision 1.87  1998/01/04 07:22:16  robertj
 * Fixed bug in thread deletion not removing it from active thread list.
 *
 * Revision 1.86  1997/10/10 10:41:22  robertj
 * Fixed problem with cooperative threading and Sleep() function returning immediately.
 *
 * Revision 1.85  1997/08/28 12:49:00  robertj
 * Fixed possible assert on exit of application.
 *
 * Revision 1.84  1997/07/08 13:08:12  robertj
 * DLL support.
 *
 * Revision 1.83  1997/04/27 05:50:15  robertj
 * DLL support.
 *
 * Revision 1.82  1997/02/09 04:05:56  robertj
 * Changed PProcess::Current() from pointer to reference.
 *
 * Revision 1.81  1997/02/05 11:51:42  robertj
 * Changed current process function to return reference and validate objects descendancy.
 *
 * Revision 1.80  1996/12/21 05:54:38  robertj
 * Fixed possible deadlock in timers.
 *
 * Revision 1.79  1996/12/05 11:44:22  craigs
 * Made indirect close from different thread less likely to have
 * race condition
 *
 * Revision 1.78  1996/11/30 12:08:42  robertj
 * Removed extraneous compiler warning.
 *
 * Revision 1.77  1996/11/10 21:05:43  robertj
 * Fixed bug of missing flush in close of indirect channel.
 *
 * Revision 1.76  1996/10/08 13:07:07  robertj
 * Fixed bug in indirect channel being reopened double deleting subchannel.
 *
 * Revision 1.75  1996/09/14 13:09:37  robertj
 * Major upgrade:
 *   rearranged sockets to help support IPX.
 *   added indirect channel class and moved all protocols to descend from it,
 *   separating the protocol from the low level byte transport.
 *
 * Revision 1.74  1996/08/11 06:53:04  robertj
 * Fixed bug in Sleep() function (nonpreemptive version).
 *
 * Revision 1.73  1996/07/27 04:12:09  robertj
 * Fixed bug in timer thread going into busy loop instead of blocking.
 *
 * Revision 1.72  1996/07/15 10:36:12  robertj
 * Fixed bug in timer on startup, getting LARGE times timing out prematurely.
 *
 * Revision 1.71  1996/06/28 13:22:43  robertj
 * Rewrite of timers to make OnTimeout more thread safe.
 *
 * Revision 1.70  1996/06/13 13:31:05  robertj
 * Rewrite of auto-delete threads, fixes Windows95 total crash.
 *
 * Revision 1.69  1996/06/03 10:01:31  robertj
 * Fixed GNU support bug fix for the fix.
 *
 * Revision 1.68  1996/06/01 05:03:37  robertj
 * Fixed GNU compiler having difficulty with PTimeInterval *this.
 *
 * Revision 1.67  1996/05/26 03:46:56  robertj
 * Compatibility to GNU 2.7.x
 *
 * Revision 1.66  1996/05/23 23:05:07  robertj
 * Fixed process filename on MSOS platforms.
 *
 * Revision 1.65  1996/05/23 09:56:57  robertj
 * Added mutex to timer list.
 *
 * Revision 1.64  1996/05/18 09:18:33  robertj
 * Added mutex to timer list.
 *
 * Revision 1.63  1996/05/09 12:19:00  robertj
 * Resolved C++ problems with 64 bit PTimeInterval for Mac platform.
 *
 * Revision 1.62  1996/04/14 02:53:34  robertj
 * Split serial and pipe channel into separate compilation units for Linux executable size reduction.
 *
 * Revision 1.61  1996/04/10 12:51:29  robertj
 * Fixed startup race condtion in timer thread.
 *
 * Revision 1.60  1996/04/09 03:32:58  robertj
 * Fixed bug in config GetTime() cannot use PTime(0) in western hemisphere.
 *
 * Revision 1.59  1996/04/02 11:29:19  robertj
 * Eliminated printing of patch level in version when there isn't one.
 *
 * Revision 1.58  1996/03/31 09:06:14  robertj
 * Fixed WriteString() so works with sockets.
 * Changed PPipeSokcet argument string list to array.
 *
 * Revision 1.57  1996/03/16 04:51:50  robertj
 * Fixed yet another bug in the scheduler.
 *
 * Revision 1.56  1996/03/12 11:30:50  robertj
 * Moved PProcess destructor to platform dependent code.
 *
 * Revision 1.55  1996/03/05 14:05:51  robertj
 * Fixed some more bugs in scheduling.
 *
 * Revision 1.54  1996/03/04 12:22:46  robertj
 * Fixed threading for unix stack check and loop list start point.
 *
 * Revision 1.53  1996/03/03 07:39:51  robertj
 * Fixed bug in thread scheduler for correct termination of "current" thread.
 *
 * Revision 1.52  1996/03/02 03:24:48  robertj
 * Changed timer thread to update timers periodically, this allows timers to be
 *    views dynamically by other threads.
 * Added automatic deletion of thread object instances on thread completion.
 *
 * Revision 1.51  1996/02/25 11:15:27  robertj
 * Added platform dependent Construct function to PProcess.
 *
 * Revision 1.50  1996/02/25 03:09:46  robertj
 * Added consts to all GetXxxx functions in PConfig.
 *
 * Revision 1.49  1996/02/15 14:44:09  robertj
 * Used string constructor for PTime, more "efficient".
 *
 * Revision 1.48  1996/02/13 12:59:30  robertj
 * Changed GetTimeZone() so can specify standard/daylight time.
 * Split PTime into separate module after major change to ReadFrom().
 *
 * Revision 1.47  1996/02/08 12:26:55  robertj
 * Changed time for full support of time zones.
 *
 * Revision 1.46  1996/02/03 11:06:49  robertj
 * Added string constructor for times, parses date/time from string.
 *
 * Revision 1.45  1996/01/28 14:09:39  robertj
 * Fixed bug in time reading function for dates before 1980.
 * Fixed bug in time reading, was out by one month.
 * Added time functions to PConfig.
 *
 * Revision 1.44  1996/01/28 02:52:04  robertj
 * Added assert into all Compare functions to assure comparison between compatible objects.
 *
 * Revision 1.43  1996/01/23 13:16:30  robertj
 * Mac Metrowerks compiler support.
 * Fixed timers so background thread not created if a windows app.
 *
 * Revision 1.42  1996/01/03 23:15:39  robertj
 * Fixed some PTime bugs.
 *
 * Revision 1.41  1996/01/03 11:09:35  robertj
 * Added Universal Time and Time Zones to PTime class.
 *
 * Revision 1.39  1995/12/23 03:40:40  robertj
 * Changed version number system
 *
 * Revision 1.38  1995/12/10 11:41:12  robertj
 * Added extra user information to processes and applications.
 * Implemented timer support in text only applications with platform threads.
 * Fixed bug in non-platform threads and semaphore timeouts.
 *
 * Revision 1.37  1995/11/21 11:50:57  robertj
 * Added timeout on semaphore wait.
 *
 * Revision 1.36  1995/11/09 12:22:58  robertj
 * Fixed bug in stream when reading an FF (get EOF).
 *
 * Revision 1.35  1995/07/31 12:09:25  robertj
 * Added semaphore class.
 * Removed PContainer from PChannel ancestor.
 *
 * Revision 1.34  1995/06/04 12:41:08  robertj
 * Fixed bug in accessing argument strings with no argument.
 *
 * Revision 1.33  1995/04/25 11:30:06  robertj
 * Fixed Borland compiler warnings.
 *
 * Revision 1.32  1995/04/22 00:51:00  robertj
 * Changed file path strings to use PFilePath object.
 * Changed semantics of Rename().
 *
 * Revision 1.31  1995/04/02 09:27:31  robertj
 * Added "balloon" help.
 *
 * Revision 1.30  1995/04/01 08:30:58  robertj
 * Fixed bug in timeout code of timers.
 *
 * Revision 1.29  1995/01/27 11:15:17  robertj
 * Removed enum to int warning from GCC.
 *
 * Revision 1.28  1995/01/18  09:02:43  robertj
 * Added notifier to timer.
 *
 * Revision 1.27  1995/01/15  04:57:15  robertj
 * Implemented PTime::ReadFrom.
 * Fixed flush of iostream at end of file.
 *
 * Revision 1.26  1995/01/11  09:45:14  robertj
 * Documentation and normalisation.
 *
 * Revision 1.25  1995/01/10  11:44:15  robertj
 * Removed PString parameter in stdarg function for GNU C++ compatibility.
 *
 * Revision 1.24  1995/01/09  12:31:51  robertj
 * Removed unnecesary return value from I/O functions.
 *
 * Revision 1.23  1994/12/12  10:09:24  robertj
 * Fixed flotain point configuration variable format.
 *
 * Revision 1.22  1994/11/28  12:38:23  robertj
 * Async write functions should have const pointer.
 *
 * Revision 1.21  1994/10/30  11:36:58  robertj
 * Fixed missing space in tine format string.
 *
 * Revision 1.20  1994/10/23  03:46:41  robertj
 * Shortened OS error assert.
 *
 * Revision 1.19  1994/09/25  10:51:04  robertj
 * Fixed error conversion code to use common function.
 * Added pipe channel.
 *
 * Revision 1.18  1994/08/21  23:43:02  robertj
 * Moved meta-string transmitter from PModem to PChannel.
 * Added SuspendBlock state to cooperative multi-threading to fix logic fault.
 * Added "force" option to Remove/Rename etc to override write protection.
 * Added common entry point to convert OS error to PChannel error.
 *
 * Revision 1.17  1994/08/04  12:57:10  robertj
 * Changed CheckBlock() to better name.
 * Moved timer porcessing so is done at every Yield().
 *
 * Revision 1.16  1994/08/01  03:39:42  robertj
 * Fixed temporary variable problem with GNU C++
 *
 * Revision 1.15  1994/07/27  05:58:07  robertj
 * Synchronisation.
 *
 * Revision 1.14  1994/07/25  03:39:22  robertj
 * Fixed problems with C++ temporary variables.
 *
 * Revision 1.13  1994/07/21  12:33:49  robertj
 * Moved cooperative threads to common.
 *
 * Revision 1.12  1994/07/17  10:46:06  robertj
 * Fixed timer bug.
 * Moved handle from file to channel.
 * Changed args to use container classes.
 *
 * Revision 1.11  1994/07/02  03:03:49  robertj
 * Time interval and timer redesign.
 *
 * Revision 1.10  1994/06/25  11:55:15  robertj
 * Unix version synchronisation.
 *
 * Revision 1.9  1994/04/20  12:17:44  robertj
 * assert changes
 *
 * Revision 1.8  1994/04/01  14:05:06  robertj
 * Text file streams
 *
 * Revision 1.7  1994/03/07  07:47:00  robertj
 * Major upgrade
 *
 * Revision 1.6  1994/01/03  04:42:23  robertj
 * Mass changes to common container classes and interactors etc etc etc.
 *
 * Revision 1.5  1993/12/31  06:53:02  robertj
 * Made inlines optional for debugging purposes.
 *
 * Revision 1.4  1993/12/29  04:41:26  robertj
 * Mac port.
 *
 * Revision 1.3  1993/11/20  17:26:28  robertj
 * Removed separate osutil.h
 *
 * Revision 1.2  1993/08/31  03:38:02  robertj
 * G++ needs explicit casts for char * / void * interchange.
 *
 * Revision 1.1  1993/08/27  18:17:47  robertj
 * Initial revision
 *
 */

#include <ptlib.h>
#include <vector>
#include <map>

#include <ctype.h>
#include <ptlib/pfactory.h>
#include <ptlib/pprocess.h>

#ifdef _WIN32
#include <ptlib/msos/ptlib/debstrm.h>
#endif

#ifdef __MACOSX__
namespace PWLibStupidOSXHacks {
  extern int loadShmVideoStuff;
  extern int loadCoreAudioStuff;
  extern int loadFakeVideoStuff;
};
#endif

class PSimpleThread : public PThread
{
    PCLASSINFO(PSimpleThread, PThread);
  public:
    PSimpleThread(
      const PNotifier & notifier,
      INT parameter,
      AutoDeleteFlag deletion,
      Priority priorityLevel,
      const PString & threadName,
      PINDEX stackSize
    );
    void Main();
  protected:
    PNotifier callback;
    INT parameter;
};


#ifndef __NUCLEUS_PLUS__
static ostream * PErrorStream = &cerr;
#else
static ostream * PErrorStream = NULL;
#endif

ostream & PGetErrorStream()
{
  return *PErrorStream;
}


void PSetErrorStream(ostream * s)
{
#ifndef __NUCLEUS_PLUS__
  PErrorStream = s != NULL ? s : &cerr;
#else
  PErrorStream = s;
#endif
}

//////////////////////////////////////////////////////////////////////////////

#if PTRACING

class PTraceInfo
{
  /* NOTE you cannot have any complex types in this structure. Anything
     that might do an asert or PTRACE will crash due to recursion.
   */

public:
  unsigned      currentLevel;
  unsigned      options;
  unsigned      thresholdLevel;
  const char  * filename;
  ostream     * stream;
  PTimeInterval startTick;
  const char  * rolloverPattern;
  unsigned      lastDayOfYear;

#if defined(_WIN32)
  CRITICAL_SECTION mutex;
  void InitMutex() { InitializeCriticalSection(&mutex); }
  void Lock()      { EnterCriticalSection(&mutex); }
  void Unlock()    { LeaveCriticalSection(&mutex); }
#elif defined(P_PTHREADS) && P_HAS_RECURSIVE_MUTEX
  pthread_mutex_t mutex;
  void InitMutex() {
    // NOTE this should actually guard against various errors
    // returned.
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr,
#if P_HAS_RECURSIVE_MUTEX == 2
PTHREAD_MUTEX_RECURSIVE
#else
PTHREAD_MUTEX_RECURSIVE_NP
#endif
    );
    pthread_mutex_init(&mutex, &attr);
    pthread_mutexattr_destroy(&attr);
  }
  void Lock()      { pthread_mutex_lock(&mutex); }
  void Unlock()    { pthread_mutex_unlock(&mutex); }
#else
  PMutex * mutex;
  void InitMutex() { mutex = new PMutex; }
  void Lock()      { mutex->Wait(); }
  void Unlock()    { mutex->Signal(); }
#endif



  PTraceInfo()
    : currentLevel(0)
    , filename(NULL)
#ifdef __NUCLEUS_PLUS__
    , stream(NULL)
#else
    , stream(&cerr)
#endif
    , startTick(PTimer::Tick())
    , rolloverPattern("yyyy_MM_dd")
    , lastDayOfYear(0)
  {
    InitMutex();

    const char * env = getenv("PWLIB_TRACE_STARTUP"); // Backward compatibility test
    if (env != NULL) {
      thresholdLevel = atoi(env);
      options = PTrace::Blocks | PTrace::Timestamp | PTrace::Thread | PTrace::FileAndLine;
    }
    else {
      env = getenv("PWLIB_TRACE_LEVEL");
      thresholdLevel = env != NULL ? atoi(env) : 0;

      env = getenv("PWLIB_TRACE_OPTIONS");
      options = env != NULL ? atoi(env) : PTrace::FileAndLine;
    }

    OpenTraceFile(getenv("PWLIB_TRACE_FILE"));
  }

  static PTraceInfo & Instance()
  {
    static PTraceInfo info;
    return info;
  }

  void SetStream(ostream * newStream)
  {
#ifndef __NUCLEUS_PLUS__
    if (newStream == NULL)
      newStream = &cerr;
#endif

    Lock();

    if (stream != &cerr && stream != &cout)
      delete stream;
    stream = newStream;

    Unlock();
  }

  void OpenTraceFile(const char * newFilename)
  {
    if (newFilename != NULL)
      filename = newFilename;

    if (filename == NULL)
      return;

    PMEMORY_IGNORE_ALLOCATIONS_FOR_SCOPE;

    if (strcasecmp(filename, "stderr") == 0)
      SetStream(&cerr);
    else if (strcasecmp(filename, "stdout") == 0)
      SetStream(&cout);
#ifdef _WIN32
    else if (strcasecmp(filename, "DEBUGSTREAM") == 0)
      SetStream(new PDebugStream);
#endif
    else {
      PFilePath fn(filename);
      fn.Replace("%P", PString((unsigned int) PProcess::Current().GetProcessID()));
     
      if ((options & PTrace::RotateDaily) != 0)
      {
          PTime now;
          fn = PFilePath(fn.GetDirectory() + fn.GetTitle() + now.AsString(rolloverPattern, ((options&PTrace::GMTTime) ? PTime::GMT : PTime::Local)) + fn.GetType());
      }

      PTextFile * traceOutput;
      if (options & PTrace::AppendToFile) {
        traceOutput = new PTextFile(fn, PFile::ReadWrite);
        traceOutput->SetPosition(0, PFile::End);
      }
      else 
        traceOutput = new PTextFile(fn, PFile::WriteOnly);

      if (traceOutput->IsOpen())
        SetStream(traceOutput);
      else {
        PTRACE(0, PProcess::Current().GetName() << "Could not open trace output file \"" << fn << '"');
        delete traceOutput;
      }
    }
  }
};


void PTrace::SetStream(ostream * s)
{
  PTraceInfo::Instance().SetStream(s);
}

void PTrace::Initialise(
    unsigned level,
    const char * filename,
    unsigned options
)
{
  Initialise(level, filename, NULL, options);
}

void PTrace::Initialise(unsigned level, const char * filename, const char * rolloverPattern, unsigned options)
{
  PTraceInfo & info = PTraceInfo::Instance();

  info.options = options;
  info.thresholdLevel = level;
  info.rolloverPattern = rolloverPattern != NULL ? rolloverPattern : "yyyy_MM_dd";
  // Does PTime::GetDayOfYear() etc. want to take zone param like PTime::AsString() to switch 
  // between os_gmtime and os_localtime?
  info.lastDayOfYear = (options & RotateDaily) != 0 ? PTime().GetDayOfYear() : 0;

  info.OpenTraceFile(filename);

#if PTRACING
  PProcess & process = PProcess::Current();
  Begin(0, "", 0) << "\tVersion " << process.GetVersion(TRUE)
                  << " by " << process.GetManufacturer()
                  << " on " << process.GetOSClass() << ' ' << process.GetOSName()
                  << " (" << process.GetOSVersion() << '-' << process.GetOSHardware()
                  << ") at " << PTime().AsString("yyyy/M/d h:mm:ss.uuu")
                  << End;
#endif

}


void PTrace::SetOptions(unsigned options)
{
  PTraceInfo::Instance().options |= options;
}


void PTrace::ClearOptions(unsigned options)
{
  PTraceInfo::Instance().options &= ~options;
}


unsigned PTrace::GetOptions()
{
  return PTraceInfo::Instance().options;
}


void PTrace::SetLevel(unsigned level)
{
  PTraceInfo::Instance().thresholdLevel = level;
}


unsigned PTrace::GetLevel()
{
  return PTraceInfo::Instance().thresholdLevel;
}


BOOL PTrace::CanTrace(unsigned level)
{
  return level <= PTraceInfo::Instance().thresholdLevel;
}


ostream & PTrace::Begin(unsigned level, const char * fileName, int lineNum)
{
  PTraceInfo & info = PTraceInfo::Instance();

  if (level == UINT_MAX)
    return *info.stream;

  info.Lock();

  if ((info.filename != NULL) && (info.options&RotateDaily) != 0) {
    unsigned day = PTime().GetDayOfYear();
    if (day != info.lastDayOfYear) {
      info.OpenTraceFile(NULL);
      info.lastDayOfYear = day;
      if (info.stream == NULL)
        info.SetStream(&cerr);
    }
  }

  PThread * thread = PThread::Current();

  ostream & stream = thread != NULL ? (ostream &)thread->traceStream : *info.stream;

  // Before we do new trace, make sure we clear any errors on the stream
  stream.clear();

  if ((info.options&SystemLogStream) == 0) {
    if ((info.options&DateAndTime) != 0) {
      PTime now;
      stream << now.AsString("yyyy/MM/dd hh:mm:ss.uuu\t", (info.options&GMTTime) ? PTime::GMT : PTime::Local);
    }

    if ((info.options&Timestamp) != 0)
      stream << setprecision(3) << setw(10) << (PTimer::Tick()-info.startTick) << '\t';

    if ((info.options&Thread) != 0) {
// always show ThreadID
      stream << "ThreadID=0x"
             << setfill('0') << hex << setw(8)
             << PThread::GetCurrentThreadId()
             << setfill(' ') << dec;
      if(thread != NULL)
      {
        stream << " ";
        PString name = thread->GetThreadName();
        if (name.GetLength() <= 12)
          stream << setw(12) << name;
        else
          stream << name.Left(10) << "..." << name.Right(10);
      }
/*
      if (thread == NULL)
        stream << "ThreadID=0x"
               << setfill('0') << hex << setw(8)
               << PThread::GetCurrentThreadId()
               << setfill(' ') << dec;
      else {
        PString name = thread->GetThreadName();
        if (name.GetLength() <= 12)
          stream << setw(12) << name;
        else
          stream << name.Left(10) << "..." << name.Right(10);
      }
*/
      stream << '\t';
    }

    if ((info.options&ThreadAddress) != 0)
      stream << hex << setfill('0')
             << setw(7) << (void *)PThread::Current()
             << dec << setfill(' ') << '\t';
  }

  if ((info.options&TraceLevel) != 0)
    stream << level << '\t';

  if ((info.options&FileAndLine) != 0 && fileName != NULL) {
    const char * file = strrchr(fileName, '/');
    if (file != NULL)
      file++;
    else {
      file = strrchr(fileName, '\\');
      if (file != NULL)
        file++;
      else
        file = fileName;
    }

    stream << setw(16) << file << '(' << lineNum << ")\t";
  }

  // Save log level for this message so End() function can use. This is
  // protected by the PTraceMutex or is thread local
  if (thread == NULL)
    info.currentLevel = level;
  else {
    thread->traceLevel = level;
    info.Unlock();
  }

  return stream;
}


ostream & PTrace::End(ostream & paramStream)
{
  PTraceInfo & info = PTraceInfo::Instance();

  PThread * thread = PThread::Current();

  if (thread != NULL) {
    PAssert(&paramStream == &thread->traceStream, PLogicError);
    info.Lock();
    *info.stream << thread->traceStream;
    thread->traceStream = PString::Empty();
  }
  else {
    PAssert(&paramStream == info.stream, PLogicError);
  }

  if ((info.options&SystemLogStream) != 0) {
    // Get the trace level for this message and set the stream width to that
    // level so that the PSystemLog can extract the log level back out of the
    // ios structure. There could be portability issues with this though it
    // should work pretty universally.
    info.stream->width((thread != NULL ? thread->traceLevel : info.currentLevel) + 1);
    info.stream->flush();
  }
  else
    *info.stream << endl;

  info.Unlock();
  return paramStream;
}


PTrace::Block::Block(const char * fileName, int lineNum, const char * traceName)
{
  file = fileName;
  line = lineNum;
  name = traceName;

  if ((PTraceInfo::Instance().options&Blocks) != 0) {
    PThread * thread = PThread::Current();
    thread->traceBlockIndentLevel += 2;

    ostream & s = PTrace::Begin(1, file, line);
    s << "B-Entry\t";
    for (unsigned i = 0; i < thread->traceBlockIndentLevel; i++)
      s << '=';
    s << "> " << name << PTrace::End;
  }
}


PTrace::Block::~Block()
{
  if ((PTraceInfo::Instance().options&Blocks) != 0) {
    PThread * thread = PThread::Current();

    ostream & s = PTrace::Begin(1, file, line);
    s << "B-Exit\t<";
    for (unsigned i = 0; i < thread->traceBlockIndentLevel; i++)
      s << '=';
    s << ' ' << name << PTrace::End;

    thread->traceBlockIndentLevel -= 2;
  }
}

#endif // PTRACING


///////////////////////////////////////////////////////////////////////////////
// PDirectory

void PDirectory::CloneContents(const PDirectory * d)
{
  CopyContents(*d);
}


///////////////////////////////////////////////////////////////////////////////
// PTimeInterval

DWORD PTimeInterval::GetInterval() const
{
  if (milliseconds <= 0)
    return 0;

  if (milliseconds >= UINT_MAX)
    return UINT_MAX;

  return (DWORD)milliseconds;
}


///////////////////////////////////////////////////////////////////////////////
// PTimer

PTimer::PTimer(long millisecs, int seconds, int minutes, int hours, int days)
  : resetTime(millisecs, seconds, minutes, hours, days)
{
  Construct();
}


PTimer::PTimer(const PTimeInterval & time)
  : resetTime(time)
{
  Construct();
}


void PTimer::Construct()
{
  state = Starting;

  timerList = PProcess::Current().GetTimerList();

  timerList->listMutex.Wait();
  timerList->Append(this);
  timerList->listMutex.Signal();

  timerList->processingMutex.Wait();
  StartRunning(TRUE);
}


PTimer & PTimer::operator=(DWORD milliseconds)
{
  timerList->processingMutex.Wait();
  resetTime.SetInterval(milliseconds);
  StartRunning(oneshot);
  return *this;
}


PTimer & PTimer::operator=(const PTimeInterval & time)
{
  timerList->processingMutex.Wait();
  resetTime = time;
  StartRunning(oneshot);
  return *this;
}


PTimer::~PTimer()
{
  timerList->listMutex.Wait();
  timerList->Remove(this);
  BOOL isCurrentTimer = this == timerList->currentTimer;
  timerList->listMutex.Signal();

  // Make sure that the OnTimeout for this timer has completed before
  // destroying the timer
  if (isCurrentTimer) {
    timerList->inTimeoutMutex.Wait();
    timerList->inTimeoutMutex.Signal();
  }
}


void PTimer::SetInterval(PInt64 milliseconds,
                         long seconds,
                         long minutes,
                         long hours,
                         int days)
{
  timerList->processingMutex.Wait();
  resetTime.SetInterval(milliseconds, seconds, minutes, hours, days);
  StartRunning(oneshot);
}


void PTimer::RunContinuous(const PTimeInterval & time)
{
  timerList->processingMutex.Wait();
  resetTime = time;
  StartRunning(FALSE);
}


void PTimer::StartRunning(BOOL once)
{
  PTimeInterval::operator=(resetTime);
  oneshot = once;
  state = (*this) != 0 ? Starting : Stopped;

  if (IsRunning())
    PProcess::Current().SignalTimerChange();

  // This must have been set by the caller
  timerList->processingMutex.Signal();
}


void PTimer::Stop()
{
  timerList->processingMutex.Wait();
  state = Stopped;
  milliseconds = 0;
  BOOL isCurrentTimer = this == timerList->currentTimer;
  timerList->processingMutex.Signal();

  // Make sure that the OnTimeout for this timer has completed before
  // retruning from Stop() function,
  if (isCurrentTimer) {
    timerList->inTimeoutMutex.Wait();
    timerList->inTimeoutMutex.Signal();
  }
}


void PTimer::Pause()
{
  timerList->processingMutex.Wait();
  if (IsRunning())
    state = Paused;
  timerList->processingMutex.Signal();
}


void PTimer::Resume()
{
  timerList->processingMutex.Wait();
  if (state == Paused)
    state = Starting;
  timerList->processingMutex.Signal();
}


void PTimer::Reset()
{
  timerList->processingMutex.Wait();
  StartRunning(oneshot);
}


void PTimer::OnTimeout()
{
  if (!callback.IsNULL())
    callback(*this, IsRunning());
}


void PTimer::Process(const PTimeInterval & delta, PTimeInterval & minTimeLeft)
{
  /*Ideally there should be a processingMutex for each individual timer, but
    that seems incredibly profligate of system resources as there  can be a
    LOT of PTimer instances about. So use one one mutex for all.
   */
  timerList->processingMutex.Wait();

  switch (state) {
    case Starting :
      state = Running;
      if (resetTime < minTimeLeft)
        minTimeLeft = resetTime;
      break;

    case Running :
      operator-=(delta);

      if (milliseconds > 0) {
        if (milliseconds < minTimeLeft.GetMilliSeconds())
          minTimeLeft = *this;
      }
      else {
        if (oneshot) {
          milliseconds = 0;
          state = Stopped;
        }
        else {
          PTimeInterval::operator=(resetTime);
          if (resetTime < minTimeLeft)
            minTimeLeft = resetTime;
        }

        timerList->processingMutex.Signal();

        /* This must be outside the mutex or if OnTimeout() changes the
           timer value (quite plausible) it deadlocks.
         */
        OnTimeout();
        return;
      }
      break;

    default : // Stopped or Paused, do nothing.
      break;
  }

  timerList->processingMutex.Signal();
}


///////////////////////////////////////////////////////////////////////////////
// PTimerList

PTimerList::PTimerList()
{
  DisallowDeleteObjects();
  currentTimer = NULL;
}


PTimeInterval PTimerList::Process()
{
  PINDEX i;
  PTimeInterval minTimeLeft = PMaxTimeInterval;

  listMutex.Wait();

  PTimeInterval now = PTimer::Tick();
  PTimeInterval sampleTime;
  if (lastSample == 0 || lastSample > now)
    sampleTime = 0;
  else {
    sampleTime = now - lastSample;
    if (now < lastSample)
      sampleTime += PMaxTimeInterval;
  }
  lastSample = now;

  for (i = 0; i < GetSize(); i++) {
    currentTimer = (PTimer *)GetAt(i);
    inTimeoutMutex.Wait();
    listMutex.Signal();
    currentTimer->Process(sampleTime, minTimeLeft);
    listMutex.Wait();
    inTimeoutMutex.Signal();
  }
  currentTimer = NULL;
  
  listMutex.Signal();

  return minTimeLeft;
}


///////////////////////////////////////////////////////////////////////////////
// PArgList

PArgList::PArgList(const char * theArgStr,
                   const char * theArgumentSpec,
                   BOOL optionsBeforeParams)
{
  // get the program arguments
  if (theArgStr != NULL)
    SetArgs(theArgStr);

  // if we got an argument spec - so process them
  if (theArgumentSpec != NULL)
    Parse(theArgumentSpec, optionsBeforeParams);
}


PArgList::PArgList(const PString & theArgStr,
                   const char * argumentSpecPtr,
                   BOOL optionsBeforeParams)
{
  // get the program arguments
  SetArgs(theArgStr);

  // if we got an argument spec - so process them
  if (argumentSpecPtr != NULL)
    Parse(argumentSpecPtr, optionsBeforeParams);
}


PArgList::PArgList(const PString & theArgStr,
                   const PString & argumentSpecStr,
                   BOOL optionsBeforeParams)
{
  // get the program arguments
  SetArgs(theArgStr);

  // if we got an argument spec - so process them
  Parse(argumentSpecStr, optionsBeforeParams);
}


PArgList::PArgList(int theArgc, char ** theArgv,
                   const char * theArgumentSpec,
                   BOOL optionsBeforeParams)
{
  // get the program arguments
  SetArgs(theArgc, theArgv);

  // if we got an argument spec - so process them
  if (theArgumentSpec != NULL)
    Parse(theArgumentSpec, optionsBeforeParams);
}


PArgList::PArgList(int theArgc, char ** theArgv,
                   const PString & theArgumentSpec,
                   BOOL optionsBeforeParams)
{
  // get the program name and path
  SetArgs(theArgc, theArgv);
  // we got an argument spec - so process them
  Parse(theArgumentSpec, optionsBeforeParams);
}


void PArgList::PrintOn(ostream & strm) const
{
  for (PINDEX i = 0; i < argumentArray.GetSize(); i++) {
    if (i > 0)
      strm << strm.fill();
    strm << argumentArray[i];
  }
}


void PArgList::ReadFrom(istream & strm)
{
  PString line;
  strm >> line;
  SetArgs(line);
}


void PArgList::SetArgs(const PString & argStr)
{
  argumentArray.SetSize(0);

  const char * str = argStr;

  for (;;) {
    while (isspace(*str)) // Skip leading whitespace
      str++;
    if (*str == '\0')
      break;

    PString & arg = argumentArray[argumentArray.GetSize()];
    while (*str != '\0' && !isspace(*str)) {
      switch (*str) {
        case '"' :
          str++;
          while (*str != '\0' && *str != '"')
            arg += *str++;
          if (*str != '\0')
            str++;
          break;

        case '\'' :
          str++;
          while (*str != '\0' && *str != '\'')
            arg += *str++;
          if (*str != '\0')
            str++;
          break;

        default :
          if (str[0] == '\\' && str[1] != '\0')
            str++;
          arg += *str++;
      }
    }
  }

  SetArgs(argumentArray);
}


void PArgList::SetArgs(const PStringArray & theArgs)
{
  argumentArray = theArgs;
  shift = 0;
  optionLetters = "";
  optionNames.SetSize(0);
  parameterIndex.SetSize(argumentArray.GetSize());
  for (PINDEX i = 0; i < argumentArray.GetSize(); i++)
    parameterIndex[i] = i;
}


BOOL PArgList::Parse(const char * spec, BOOL optionsBeforeParams)
{
  if (PAssertNULL(spec) == NULL)
    return FALSE;

  // Find starting point, start at shift if first Parse() call.
  PINDEX arg = optionLetters.IsEmpty() ? shift : 0;

  // If not in parse all mode, have been parsed before, and had some parameters
  // from last time, then start argument parsing somewhere along instead of start.
  if (optionsBeforeParams && !optionLetters && parameterIndex.GetSize() > 0)
    arg = parameterIndex[parameterIndex.GetSize()-1] + 1;

  // Parse the option specification
  optionLetters = "";
  optionNames.SetSize(0);
  PIntArray canHaveOptionString;

  PINDEX codeCount = 0;
  while (*spec != '\0') {
    if (*spec == '-')
      optionLetters += ' ';
    else
      optionLetters += *spec++;
    if (*spec == '-') {
      const char * base = ++spec;
      while (*spec != '\0' && *spec != '.' && *spec != ':' && *spec != ';')
        spec++;
      optionNames[codeCount] = PString(base, spec-base);
      if (*spec == '.')
        spec++;
    }
    if (*spec == ':' || *spec == ';') {
      canHaveOptionString.SetSize(codeCount+1);
      canHaveOptionString[codeCount] = *spec == ':' ? 2 : 1;
      spec++;
    }
    codeCount++;
  }

  // Clear and reset size of option information
  optionCount.SetSize(0);
  optionCount.SetSize(codeCount);
  optionString.SetSize(0);
  optionString.SetSize(codeCount);

  // Clear parameter indexes
  parameterIndex.SetSize(0);
  shift = 0;

  // Now work through the arguments and split out the options
  PINDEX param = 0;
  BOOL hadMinusMinus = FALSE;
  while (arg < argumentArray.GetSize()) {
    const PString & argStr = argumentArray[arg];
    if (hadMinusMinus || argStr[0] != '-' || argStr[1] == '\0') {
      // have a parameter string
      parameterIndex.SetSize(param+1);
      parameterIndex[param++] = arg;
    }
    else if (optionsBeforeParams && parameterIndex.GetSize() > 0)
      break;
    else if (argStr == "--") // ALL remaining args are parameters not options
      hadMinusMinus = TRUE;
    else if (argStr[1] == '-')
      ParseOption(optionNames.GetValuesIndex(argStr.Mid(2)), 0, arg, canHaveOptionString);
    else {
      for (PINDEX i = 1; i < argStr.GetLength(); i++)
        if (ParseOption(optionLetters.Find(argStr[i]), i+1, arg, canHaveOptionString))
          break;
    }

    arg++;
  }

  return param > 0;
}


BOOL PArgList::ParseOption(PINDEX idx, PINDEX offset, PINDEX & arg,
                           const PIntArray & canHaveOptionString)
{
  if (idx == P_MAX_INDEX) {
    UnknownOption(argumentArray[arg]);
    return FALSE;
  }

  optionCount[idx]++;
  if (canHaveOptionString[idx] == 0)
    return FALSE;

  if (!optionString[idx])
    optionString[idx] += '\n';

  if (offset != 0 &&
        (canHaveOptionString[idx] == 1 || argumentArray[arg][offset] != '\0')) {
    optionString[idx] += argumentArray[arg].Mid(offset);
    return TRUE;
  }

  if (++arg >= argumentArray.GetSize())
    return FALSE;

  optionString[idx] += argumentArray[arg];
  return TRUE;
}


PINDEX PArgList::GetOptionCount(char option) const
{
  return GetOptionCountByIndex(optionLetters.Find(option));
}


PINDEX PArgList::GetOptionCount(const char * option) const
{
  return GetOptionCountByIndex(optionNames.GetValuesIndex(PString(option)));
}


PINDEX PArgList::GetOptionCount(const PString & option) const
{
  return GetOptionCountByIndex(optionNames.GetValuesIndex(option));
}


PINDEX PArgList::GetOptionCountByIndex(PINDEX idx) const
{
  if (idx < optionCount.GetSize())
    return optionCount[idx];

  return 0;
}


PString PArgList::GetOptionString(char option, const char * dflt) const
{
  return GetOptionStringByIndex(optionLetters.Find(option), dflt);
}


PString PArgList::GetOptionString(const char * option, const char * dflt) const
{
  return GetOptionStringByIndex(optionNames.GetValuesIndex(PString(option)), dflt);
}


PString PArgList::GetOptionString(const PString & option, const char * dflt) const
{
  return GetOptionStringByIndex(optionNames.GetValuesIndex(option), dflt);
}


PString PArgList::GetOptionStringByIndex(PINDEX idx, const char * dflt) const
{
  if (idx < optionString.GetSize() && optionString.GetAt(idx) != NULL)
    return optionString[idx];

  if (dflt != NULL)
    return dflt;

  return PString();
}


PStringArray PArgList::GetParameters(PINDEX first, PINDEX last) const
{
  PStringArray array;

  last += shift;
  if (last < 0)
    return array;

  if (last >= parameterIndex.GetSize())
    last = parameterIndex.GetSize()-1;

  first += shift;
  if (first < 0)
    first = 0;

  if (first > last)
    return array;

  array.SetSize(last-first+1);

  PINDEX idx = 0;
  while (first <= last)
    array[idx++] = argumentArray[parameterIndex[first++]];

  return array;
}


PString PArgList::GetParameter(PINDEX num) const
{
  int idx = shift+(int)num;
  if (idx >= 0 && idx < (int)parameterIndex.GetSize())
    return argumentArray[parameterIndex[idx]];

  IllegalArgumentIndex(idx);
  return PString();
}


void PArgList::Shift(int sh) 
{
  shift += sh;
  if (shift < 0)
    shift = 0;
  else if (shift > (int)parameterIndex.GetSize())
    shift = parameterIndex.GetSize() - 1;
}


void PArgList::IllegalArgumentIndex(PINDEX idx) const
{
  PError << "attempt to access undefined argument at index "
         << idx << endl;
}
 

void PArgList::UnknownOption(const PString & option) const
{
  PError << "unknown option \"" << option << "\"\n";
}


void PArgList::MissingArgument(const PString & option) const
{
  PError << "option \"" << option << "\" requires argument\n";
}

#ifdef P_CONFIG_FILE

///////////////////////////////////////////////////////////////////////////////
// PConfigArgs

PConfigArgs::PConfigArgs(const PArgList & args)
  : PArgList(args),
    sectionName(config.GetDefaultSection()),
    negationPrefix("no-")
{
}


PINDEX PConfigArgs::GetOptionCount(char option) const
{
  PINDEX count;
  if ((count = PArgList::GetOptionCount(option)) > 0)
    return count;

  PString stropt = CharToString(option);
  if (stropt.IsEmpty())
    return 0;

  return GetOptionCount(stropt);
}


PINDEX PConfigArgs::GetOptionCount(const char * option) const
{
  return GetOptionCount(PString(option));
}


PINDEX PConfigArgs::GetOptionCount(const PString & option) const
{
  // if specified on the command line, use that option
  PINDEX count = PArgList::GetOptionCount(option);
  if (count > 0)
    return count;

  // if user has specified "no-option", then ignore config file
  if (PArgList::GetOptionCount(negationPrefix + option) > 0)
    return 0;

  return config.HasKey(sectionName, option) ? 1 : 0;
}


PString PConfigArgs::GetOptionString(char option, const char * dflt) const
{
  if (PArgList::GetOptionCount(option) > 0)
    return PArgList::GetOptionString(option, dflt);

  PString stropt = CharToString(option);
  if (stropt.IsEmpty()) {
    if (dflt != NULL)
      return dflt;
    return PString();
  }

  return GetOptionString(stropt, dflt);
}


PString PConfigArgs::GetOptionString(const char * option, const char * dflt) const
{
  return GetOptionString(PString(option), dflt);
}


PString PConfigArgs::GetOptionString(const PString & option, const char * dflt) const
{
  // if specified on the command line, use that option
  if (PArgList::GetOptionCount(option) > 0)
    return PArgList::GetOptionString(option, dflt);

  // if user has specified "no-option", then ignore config file
  if (PArgList::HasOption(negationPrefix + option)) {
    if (dflt != NULL)
      return dflt;
    return PString();
  }

  return config.GetString(sectionName, option, dflt != NULL ? dflt : "");
}


void PConfigArgs::Save(const PString & saveOptionName)
{
  if (PArgList::GetOptionCount(saveOptionName) == 0)
    return;

  config.DeleteSection(sectionName);

  for (PINDEX i = 0; i < optionCount.GetSize(); i++) {
    PString optionName = optionNames[i];
    if (optionCount[i] > 0 && optionName != saveOptionName) {
      if (optionString.GetAt(i) != NULL)
        config.SetString(sectionName, optionName, optionString[i]);
      else
        config.SetBoolean(sectionName, optionName, TRUE);
    }
  }
}


PString PConfigArgs::CharToString(char ch) const
{
  PINDEX index = optionLetters.Find(ch);
  if (index == P_MAX_INDEX)
    return PString();

  if (optionNames.GetAt(index) == NULL)
    return PString();

  return optionNames[index];
}

#endif // P_CONFIG_ARGS

///////////////////////////////////////////////////////////////////////////////
// PProcess

PProcess * PProcessInstance;
int PProcess::p_argc;
char ** PProcess::p_argv;
char ** PProcess::p_envp;

typedef std::map<PString, PProcessStartup *> PProcessStartupList;

int PProcess::_main(void *)
{
  Main();
  return terminationValue;
}


void PProcess::PreInitialise(int c, char ** v, char ** e)
{
  p_argc = c;
  p_argv = v;
  p_envp = e;
}


static PProcessStartupList & GetPProcessStartupList()
{
  static PProcessStartupList list;
  return list;
}


PProcess::PProcess(const char * manuf, const char * name,
                           WORD major, WORD minor, CodeStatus stat, WORD build)
  : manufacturer(manuf), productName(name)
{
  PProcessInstance = this;
  terminationValue = 0;

  majorVersion = major;
  minorVersion = minor;
  status = stat;
  buildNumber = build;

#ifndef P_RTEMS
  if (p_argv != 0 && p_argc > 0) {
    arguments.SetArgs(p_argc-1, p_argv+1);

    
#if defined(_WIN32) 
    // Try to get the real image path for this process
#ifndef _WIN32_WCE
	GetModuleFileName(GetModuleHandle(NULL), executableFile.GetPointer(1024), 1024);
#else
	wchar_t wcsModuleName[1024];
	if(GetModuleFileName(GetModuleHandle(NULL), wcsModuleName, 1024))
		wcstombs(executableFile.GetPointer(1024), wcsModuleName, 1024);
#endif
    executableFile.Replace("\\??\\","");

    if(executableFile.IsEmpty()){
	// Ok something went wrong, just use the default
    executableFile = PString(p_argv[0]);
    }
#endif

    if (!PFile::Exists(executableFile)) {
      PString execFile = executableFile + ".exe";
      if (PFile::Exists(execFile))
        executableFile = execFile;
    }

    if (productName.IsEmpty())
      productName = executableFile.GetTitle().ToLower();
  }
#else
  cout << "Enter program arguments:\n";
  arguments.ReadFrom(cin);
#endif

  InitialiseProcessThread();

  Construct();

#ifdef __MACOSX__
  
#ifdef HAS_VIDEO
  PWLibStupidOSXHacks::loadFakeVideoStuff = 1;
#ifdef USE_SHM_VIDEO_DEVICES
  PWLibStupidOSXHacks::loadShmVideoStuff = 1;
#endif // USE_SHM_VIDEO_DEVICES
#endif // HAS_VIDEO
  
#ifdef HAS_AUDIO
  PWLibStupidOSXHacks::loadCoreAudioStuff = 1;
#endif // HAS_AUDIO
  
#endif // __MACOSX__

  // create one instance of each class registered in the 
  // PProcessStartup abstract factory
  PProcessStartupList & startups = GetPProcessStartupList();
  {
    PProcessStartup * levelSet = PFactory<PProcessStartup>::CreateInstance("SetTraceLevel");
    if (levelSet != NULL) 
      levelSet->OnStartup();

    PProcessStartupFactory::KeyList_T list = PProcessStartupFactory::GetKeyList();
    PProcessStartupFactory::KeyList_T::const_iterator r;
    for (r = list.begin(); r != list.end(); ++r) {
      if (*r != "SetTraceLevel") {
        PProcessStartup * instance = PProcessStartupFactory::CreateInstance(*r);
        instance->OnStartup();
        startups.insert(std::pair<PString, PProcessStartup *>(*r, instance));
      }
    }
  }

#if PMEMORY_HEAP
  // Now we start looking for memory leaks!
  PMemoryHeap::SetIgnoreAllocations(FALSE);
#endif
}


void PProcess::PreShutdown()
{
  PProcessStartupList & startups = GetPProcessStartupList();

  // call OnShutfdown for the PProcessInstances previously created
  // make sure we handle singletons correctly
  {
    while (startups.size() > 0) {
      PProcessStartupList::iterator r = startups.begin();
      PProcessStartup * instance = r->second;
      instance->OnShutdown();
      if (!PProcessStartupFactory::IsSingleton(r->first))
        delete instance;
      startups.erase(r);
    }
  }
}


PProcess & PProcess::Current()
{
  if (PProcessInstance == NULL) {
    cerr << "Catastrophic failure, PProcess::Current() = NULL!!\n";
#if defined(_MSC_VER) && defined(_DEBUG) && !defined(_WIN32_WCE)
    __asm int 3;
#endif
    _exit(1);
  }
  return *PProcessInstance;
}


BOOL PProcess::IsInitialised()
{
  return PProcessInstance != NULL;
}


PObject::Comparison PProcess::Compare(const PObject & obj) const
{
  PAssert(PIsDescendant(&obj, PProcess), PInvalidCast);
  return productName.Compare(((const PProcess &)obj).productName);
}


void PProcess::Terminate()
{
#ifdef _WINDLL
  FatalExit(terminationValue);
#else
  exit(terminationValue);
#endif
}


PString PProcess::GetThreadName() const
{
  return GetName(); 
}
 
 
void PProcess::SetThreadName(const PString & /*name*/)
{
}

PTime PProcess::GetStartTime() const
{ 
  return programStartTime; 
}

PString PProcess::GetVersion(BOOL full) const
{
  const char * const statusLetter[NumCodeStatuses] =
    { "alpha", "beta", "." };
  return psprintf(full ? "%u.%u%s%u" : "%u.%u",
                  majorVersion, minorVersion, statusLetter[status], buildNumber);
}


void PProcess::SetConfigurationPath(const PString & path)
{
  configurationPaths = path.Tokenise(";:", FALSE);
}


///////////////////////////////////////////////////////////////////////////////
// PThread

void PThread::PrintOn(ostream & strm) const
{
  strm << GetThreadName();
}


PString PThread::GetThreadName() const
{
  return threadName; 
}

#if defined(_DEBUG) && defined(_MSC_VER)

typedef struct tagTHREADNAME_INFO
{
  DWORD dwType ;                       // must be 0x1000
  LPCSTR szName ;                      // pointer to name (in user addr space)
  DWORD dwThreadID ;                   // thread ID (-1=caller thread, but seems to set more than one thread's name)
  DWORD dwFlags ;                      // reserved for future use, must be zero
} THREADNAME_INFO ;


void SetWinDebugThreadName (THREADNAME_INFO * info)
{
  __try
  {
    RaiseException (0x406D1388, 0, sizeof(THREADNAME_INFO)/sizeof(DWORD), (DWORD *) info) ;
  }                              // if not running under debugger exception comes back
  __except(EXCEPTION_CONTINUE_EXECUTION)
  {                              // just keep on truckin'
  }
}
#endif // defined(_DEBUG) && defined(_MSC_VER)


void PThread::SetThreadName(const PString & name)
{
  if (name.IsEmpty())
    threadName = psprintf("%s:%08x", GetClass(), (INT)this);
  else
    threadName = psprintf(name, (INT)this);

#if defined(_DEBUG) && defined(_MSC_VER)
  if (threadId) {       // make thread name known to debugger
    THREADNAME_INFO Info = { 0x1000, (const char *) threadName, threadId, 0 } ;
    SetWinDebugThreadName (&Info) ;
  }
#endif // defined(_DEBUG) && defined(_MSC_VER)
}
 
PThread * PThread::Create(const PNotifier & notifier,
                          INT parameter,
                          AutoDeleteFlag deletion,
                          Priority priorityLevel,
                          const PString & threadName,
                          PINDEX stackSize)
{
  PThread * thread = new PSimpleThread(notifier,
                                       parameter,
                                       deletion,
                                       priorityLevel,
                                       threadName,
                                       stackSize);
  if (deletion != AutoDeleteThread)
    return thread;

  // Do not return a pointer to the thread if it is auto-delete as this
  // pointer is extremely dangerous to use, it could be deleted at any moment
  // from now on so using the pointer could crash the program.
  return NULL;
}


PSimpleThread::PSimpleThread(const PNotifier & notifier,
                             INT param,
                             AutoDeleteFlag deletion,
                             Priority priorityLevel,
                             const PString & threadName,
                             PINDEX stackSize)
  : PThread(stackSize, deletion, priorityLevel, threadName),
    callback(notifier),
    parameter(param)
{
  Resume();
}


void PSimpleThread::Main()
{
  callback(*this, parameter);
}

/////////////////////////////////////////////////////////////////////////////

void PSyncPointAck::Signal()
{
  PSyncPoint::Signal();
  ack.Wait();
}


void PSyncPointAck::Signal(const PTimeInterval & wait)
{
  PSyncPoint::Signal();
  ack.Wait(wait);
}


void PSyncPointAck::Acknowledge()
{
  ack.Signal();
}


/////////////////////////////////////////////////////////////////////////////

void PCondMutex::WaitCondition()
{
  for (;;) {
    Wait();
    if (Condition())
      return;
    PMutex::Signal();
    OnWait();
    syncPoint.Wait();
  }
}


void PCondMutex::Signal()
{
  if (Condition())
    syncPoint.Signal();
  PMutex::Signal();
}


void PCondMutex::OnWait()
{
  // Do nothing
}


/////////////////////////////////////////////////////////////////////////////

PIntCondMutex::PIntCondMutex(int val, int targ, Operation op)
{
  value = val;
  target = targ;
  operation = op;
}


void PIntCondMutex::PrintOn(ostream & strm) const
{
  strm << '(' << value;
  switch (operation) {
    case LT :
      strm << " < ";
    case LE :
      strm << " <= ";
    case GE :
      strm << " >= ";
    case GT :
      strm << " > ";
    default:
      strm << " == ";
  }
  strm << target << ')';
}


BOOL PIntCondMutex::Condition()
{
  switch (operation) {
    case LT :
      return value < target;
    case LE :
      return value <= target;
    case GE :
      return value >= target;
    case GT :
      return value > target;
    default :
      break;
  }
  return value == target;
}


PIntCondMutex & PIntCondMutex::operator=(int newval)
{
  Wait();
  value = newval;
  Signal();
  return *this;
}


PIntCondMutex & PIntCondMutex::operator++()
{
  Wait();
  value++;
  Signal();
  return *this;
}


PIntCondMutex & PIntCondMutex::operator+=(int inc)
{
  Wait();
  value += inc;
  Signal();
  return *this;
}


PIntCondMutex & PIntCondMutex::operator--()
{
  Wait();
  value--;
  Signal();
  return *this;
}


PIntCondMutex & PIntCondMutex::operator-=(int dec)
{
  Wait();
  value -= dec;
  Signal();
  return *this;
}


/////////////////////////////////////////////////////////////////////////////

PReadWriteMutex::PReadWriteMutex()
  : readerSemaphore(1, 1),
    writerSemaphore(1, 1)
{
  readerCount = 0;
  writerCount = 0;
}


PReadWriteMutex::Nest * PReadWriteMutex::GetNest() const
{
  PWaitAndSignal mutex(nestingMutex);
  return nestedThreads.GetAt(POrdinalKey((PINDEX)PThread::GetCurrentThreadId()));
}


void PReadWriteMutex::EndNest()
{
  nestingMutex.Wait();
  nestedThreads.RemoveAt(POrdinalKey((PINDEX)PThread::GetCurrentThreadId()));
  nestingMutex.Signal();
}


PReadWriteMutex::Nest & PReadWriteMutex::StartNest()
{
  POrdinalKey threadId = (PINDEX)PThread::GetCurrentThreadId();

  nestingMutex.Wait();

  Nest * nest = nestedThreads.GetAt(threadId);

  if (nest == NULL) {
    nest = new Nest;
    nestedThreads.SetAt(threadId, nest);
  }

  nestingMutex.Signal();

  return *nest;
}


void PReadWriteMutex::StartRead()
{
  // Get the nested thread info structure, create one it it doesn't exist
  Nest & nest = StartNest();

  // One more nested call to StartRead() by this thread, note this does not
  // need to be mutexed as it is always in the context of a single thread.
  nest.readerCount++;

  // If this is the first call to StartRead() and there has not been a
  // previous call to StartWrite() then actually do the text book read only
  // lock, otherwise we leave it as just having incremented the reader count.
  if (nest.readerCount == 1 && nest.writerCount == 0)
    InternalStartRead();
}


void PReadWriteMutex::InternalStartRead()
{
  // Text book read only lock

  starvationPreventer.Wait();
   readerSemaphore.Wait();
    readerMutex.Wait();

     readerCount++;
     if (readerCount == 1)
       writerSemaphore.Wait();

    readerMutex.Signal();
   readerSemaphore.Signal();
  starvationPreventer.Signal();
}


void PReadWriteMutex::EndRead()
{
  // Get the nested thread info structure for the curent thread
  Nest * nest = GetNest();

  // If don't have an active read or write lock or there is a write lock but
  // the StartRead() was never called, then assert and ignore call.
  if (nest == NULL || nest->readerCount == 0) {
    PAssertAlways("Unbalanced PReadWriteMutex::EndRead()");
    return;
  }

  // One less nested lock by this thread, note this does not
  // need to be mutexed as it is always in the context of a single thread.
  nest->readerCount--;

  // If this is a nested read or a write lock is present then we don't do the
  // real unlock, the decrement is enough.
  if (nest->readerCount > 0 || nest->writerCount > 0)
    return;

  // Do text book read lock
  InternalEndRead();

  // At this point all read and write locks are gone for this thread so we can
  // reclaim the memory.
  EndNest();
}


void PReadWriteMutex::InternalEndRead()
{
  // Text book read only unlock

  readerMutex.Wait();

  readerCount--;
  if (readerCount == 0)
    writerSemaphore.Signal();

  readerMutex.Signal();
}


void PReadWriteMutex::StartWrite()
{
  // Get the nested thread info structure, create one it it doesn't exist
  Nest & nest = StartNest();

  // One more nested call to StartWrite() by this thread, note this does not
  // need to be mutexed as it is always in the context of a single thread.
  nest.writerCount++;

  // If is a nested call to StartWrite() then simply return, the writer count
  // increment is all we haev to do.
  if (nest.writerCount > 1)
    return;

  // If have a read lock already in this thread then do the "real" unlock code
  // but do not change the lock count, calls to EndRead() will now just
  // decrement the count instead of doing the unlock (its already done!)
  if (nest.readerCount > 0)
    InternalEndRead();

  // Note in this gap another thread could grab the write lock, thus

  // Now do the text book write lock
  writerMutex.Wait();

  writerCount++;
  if (writerCount == 1)
    readerSemaphore.Wait();

  writerMutex.Signal();

  writerSemaphore.Wait();
}


void PReadWriteMutex::EndWrite()
{
  // Get the nested thread info structure for the curent thread
  Nest * nest = GetNest();

  // If don't have an active read or write lock or there is a read lock but
  // the StartWrite() was never called, then assert and ignore call.
  if (nest == NULL || nest->writerCount == 0) {
    PAssertAlways("Unbalanced PReadWriteMutex::EndWrite()");
    return;
  }

  // One less nested lock by this thread, note this does not
  // need to be mutexed as it is always in the context of a single thread.
  nest->writerCount--;

  // If this is a nested write lock then the decrement is enough and we
  // don't do the actual write unlock.
  if (nest->writerCount > 0)
    return;

  // Begin text book write unlock
  writerSemaphore.Signal();

  writerMutex.Wait();

  writerCount--;
  if (writerCount == 0)
    readerSemaphore.Signal();

  writerMutex.Signal();
  // End of text book write unlock

  // Now check to see if there was a read lock present for this thread, if so
  // then reacquire the read lock (not changing the count) otherwise clean up the
  // memory for the nested thread info structure
  if (nest->readerCount > 0)
    InternalStartRead();
  else
    EndNest();
}


/////////////////////////////////////////////////////////////////////////////

PReadWaitAndSignal::PReadWaitAndSignal(const PReadWriteMutex & rw, BOOL start)
  : mutex((PReadWriteMutex &)rw)
{
  if (start)
    mutex.StartRead();
}


PReadWaitAndSignal::~PReadWaitAndSignal()
{
  mutex.EndRead();
}


/////////////////////////////////////////////////////////////////////////////

PWriteWaitAndSignal::PWriteWaitAndSignal(const PReadWriteMutex & rw, BOOL start)
  : mutex((PReadWriteMutex &)rw)
{
  if (start)
    mutex.StartWrite();
}


PWriteWaitAndSignal::~PWriteWaitAndSignal()
{
  mutex.EndWrite();
}


// End Of File ///////////////////////////////////////////////////////////////
