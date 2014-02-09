/*
 * win32.cxx
 *
 * Miscellaneous implementation of classes for Win32
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
 * $Log: win32.cxx,v $
 * Revision 1.165  2007/10/03 20:52:27  dsandras
 * Applied patch from Matthias Schneider for mingw compilation. Thanks a lot!
 *
 * Revision 1.164  2007/10/03 01:18:47  rjongbloed
 * Fixed build for Windows Mobile 5 and added Windows Mobile 6
 *
 * Revision 1.163  2007/09/17 11:14:48  rjongbloed
 * Added "No Trace" build configuration.
 *
 * Revision 1.162  2007/09/08 11:34:29  rjongbloed
 * Improved memory checking (leaks etc), especially when using MSVC debug library.
 *
 * Revision 1.161  2007/09/01 05:16:18  rjongbloed
 * Added Windows Vista to OS descirption functions.
 *
 * Revision 1.160  2007/07/03 08:43:31  rjongbloed
 * Fixed corect reporting of a socket being closed by OS as "NotOpen" aka EBADF.
 *
 * Revision 1.159  2007/04/02 05:29:55  rjongbloed
 * Tidied some trace logs to assure all have a category (bit before a tab character) set.
 *
 * Revision 1.158  2006/06/21 04:20:07  csoutheren
 * Fixes for VS.net
 *
 * Revision 1.157  2006/04/09 11:04:00  csoutheren
 * Remove warnings on VS.net 2005
 *
 * Revision 1.156  2005/11/30 12:47:42  csoutheren
 * Removed tabs, reformatted some code, and changed tags for Doxygen
 *
 * Revision 1.155  2005/11/17 22:54:26  csoutheren
 * Fixed missed functions in de-consting PMutex functions
 *
 * Revision 1.154  2005/11/14 22:29:13  csoutheren
 * Reverted Wait and Signal to non-const - there is no way we can guarantee that all
 * descendant classes everywhere will be changed over, so we have to keep the
 * original  API
 *
 * Revision 1.153  2005/11/09 09:19:10  csoutheren
 * Now actually remove the call :)
 *
 * Revision 1.152  2005/11/09 09:11:39  csoutheren
 * Moved Windows-specific AttachThreadInput callsto seperate member function
 * on PThread. This removes a linearly increasing delay in creating new threads
 *
 * Revision 1.151  2005/11/04 06:34:20  csoutheren
 * Added new class PSync as abstract base class for all mutex/sempahore classes
 * Changed PCriticalSection to use Wait/Signal rather than Enter/Leave
 * Changed Wait/Signal to be const member functions
 * Renamed PMutex to PTimedMutex and made PMutex synonym for PCriticalSection.
 * This allows use of very efficient mutex primitives in 99% of cases where timed waits
 * are not needed
 *
 * Revision 1.150  2005/09/23 15:30:46  dominance
 * more progress to make mingw compile nicely. Thanks goes to Julien Puydt for pointing out to me how to do it properly. ;)
 *
 * Revision 1.149  2005/09/18 13:01:43  dominance
 * fixed pragma warnings when building with gcc.
 *
 * Revision 1.148  2005/07/13 12:48:32  csoutheren
 * Backported fix from isvo branch
 *
 * Revision 1.147  2005/06/07 07:41:42  csoutheren
 * Applied patch 1176459 for PocketPC. Thanks to Matthias Weber
 *
 * Revision 1.146.2.1  2005/04/25 13:12:39  shorne
 * Fixed OSConfigDir for win32/NT/XP
 *
 * Revision 1.146  2005/02/02 23:21:16  csoutheren
 * Fixed problem with race condition in HousekeepingThread
 * Thanks to an idea from Auri Vizgaitis
 *
 * Revision 1.145  2005/01/25 11:28:25  csoutheren
 * Changed parms to CreateEvent to be more clear
 *
 * Revision 1.144  2005/01/16 23:00:36  csoutheren
 * Fixed problem when calling WaitForTermination from within the same thread
 *
 * Revision 1.143  2005/01/11 06:57:15  csoutheren
 * Fixed namespace collisions with plugin starup factories
 *
 * Revision 1.142  2005/01/04 07:44:04  csoutheren
 * More changes to implement the new configuration methodology, and also to
 * attack the global static problem
 *
 * Revision 1.141  2004/11/17 12:50:44  csoutheren
 * Win32 DCOM support, thanks to Simon Horne
 *
 * Revision 1.140  2004/10/31 22:22:06  csoutheren
 * Added pragma to include ole32.lib for static builds
 *
 * Revision 1.139  2004/10/23 10:50:28  ykiryanov
 * Added ifdef _WIN32_WCE for PocketPC 2003 SDK port
 *
 * Revision 1.138  2004/09/17 04:05:12  csoutheren
 * Changed Windows PDirectory semantics to be the same as Unix
 *
 * Revision 1.137  2004/06/09 13:35:11  csoutheren
 * Disabled "wait for key" at end of program unless in debug mode or PMEMORY_CHECK
 * is enabled
 *
 * Revision 1.136  2004/05/21 00:28:40  csoutheren
 * Moved PProcessStartup creation to PProcess::Initialise
 * Added PreShutdown function and called it from ~PProcess to handle PProcessStartup removal
 *
 * Revision 1.135  2004/04/09 06:52:18  rjongbloed
 * Removed #pargma linker command for /delayload of DLL as documentations sais that
 *   you cannot do this.
 *
 * Revision 1.134  2004/04/03 06:54:30  rjongbloed
 * Many and various changes to support new Visual C++ 2003
 *
 * Revision 1.133  2004/02/23 23:52:20  csoutheren
 * Added pragmas to avoid every Windows application needing to include libs explicitly
 *
 * Revision 1.132  2003/11/10 20:52:26  dereksmithies
 * add fix from Diego Tartara to recognize win XP and 2003 Server. Many thanks.
 *
 * Revision 1.131  2003/11/08 01:43:05  rjongbloed
 * Fixed race condition that could start two housekeeping threads, thanks Ted Szoczei
 *
 * Revision 1.130  2003/11/05 05:56:08  csoutheren
 * Added #pragma to include required libs
 *
 * Revision 1.129  2003/10/27 03:29:11  csoutheren
 * Added support for QoS
 *    Thanks to Henry Harrison of AliceStreet
 *
 * Revision 1.128  2003/09/17 05:45:10  csoutheren
 * Removed recursive includes
 *
 * Revision 1.127  2003/02/26 01:12:52  robertj
 * Fixed race condition where thread can terminatebefore an IsSuspeded() call
 *   occurs and cause an assert, thanks Sebastian Meyer
 *
 * Revision 1.126  2002/12/11 22:25:04  robertj
 * Added ability to set user identity temporarily and permanently.
 * Added get and set users group functions.
 *
 * Revision 1.125  2002/11/20 02:38:38  robertj
 * Fixed file path parsing for common unix/dos path error.
 *
 * Revision 1.124  2002/11/20 00:58:58  robertj
 * Made file path parsing slightly smarter for common unix/dos path error.
 *
 * Revision 1.123  2002/11/19 10:28:50  robertj
 * Changed PFilePath so can be empty string, indicating illegal path.
 *
 * Revision 1.122  2002/09/23 07:17:24  robertj
 * Changes to allow winsock2 to be included.
 *
 * Revision 1.121  2002/06/04 00:25:31  robertj
 * Fixed incorrectly initialised trace indent, thanks Artis Kugevics
 *
 * Revision 1.120  2002/04/24 01:11:05  robertj
 * Fixed problem with PTRACE_BLOCK indent level being correct across threads.
 *
 * Revision 1.119  2002/01/26 15:05:35  yurik
 * Removed extra ifdefs
 *
 * Revision 1.118  2002/01/23 04:45:50  craigs
 * Added copy Constructors for PSemaphore, PMutex and PSyncPoint
 *
 * Revision 1.117  2001/12/08 00:22:37  robertj
 * Prevented assert if doing SetUserName() with empty string.
 *
 * Revision 1.116  2001/11/23 06:59:00  robertj
 * Added PProcess::SetUserName() function for effective user changes.
 *
 * Revision 1.115  2001/10/26 04:20:25  craigs
 * Changed housekeeping thread to be Normal priority to avoide starvation
 * of PTimer dependent threads
 *
 * Revision 1.114  2001/10/23 05:42:48  robertj
 * Fixed bug in retry loop waiting for termination, applies only to heavily
 *   laoded Win98 class machines.
 *
 * Revision 1.113  2001/10/07 16:05:59  yurik
 * Removed MFC dependency
 *
 * Revision 1.112  2001/09/11 03:27:46  robertj
 * Improved error processing on high level protocol failures, usually
 *   caused by unexpected shut down of a socket.
 *
 * Revision 1.111  2001/09/10 02:51:23  robertj
 * Major change to fix problem with error codes being corrupted in a
 *   PChannel when have simultaneous reads and writes in threads.
 *
 * Revision 1.110  2001/08/07 03:20:39  robertj
 * Fixed close of DLL so flagged as closed, thanks Stefan Ditscheid.
 *
 * Revision 1.109  2001/07/09 01:35:48  robertj
 * Added name to housekeeper thread.
 *
 * Revision 1.108  2001/06/01 04:03:05  yurik
 * Removed dependency on obsolete function
 *
 * Revision 1.107  2001/05/29 00:49:18  robertj
 * Added ability to put in a printf %x in thread name to get thread object
 *   address into user settable thread name.
 *
 * Revision 1.106  2001/05/10 15:21:30  yurik
 * Fixed bug in PSemaphore::Signal(), courtesy of Dave Cassel, dcassel@cyberfone.com.
 * Also Refined thread priorities for WinCE.
 *
 * Revision 1.105  2001/05/04 05:42:23  yurik
 * GetUserName for Pocket PC (Windows CE 3.0) implemented
 *
 * Revision 1.104  2001/04/26 06:07:34  yurik
 * UI improvements
 *
 * Revision 1.103  2001/04/15 03:38:42  yurik
 * Removed shutdown flag. Use IsTerminated() instead
 *
 * Revision 1.102  2001/04/14 04:54:03  yurik
 * Added process shutdown flag
 *
 * Revision 1.101  2001/03/24 05:52:42  robertj
 * Added Windows 98 and ME to GetOSName()
 * Added build number to GetOSVersion()
 *
 * Revision 1.100  2001/03/03 00:55:02  yurik
 * Proper fix for filetime routines used in guid calc done for WinCE
 *
 * Revision 1.99  2001/02/13 06:55:21  robertj
 * Fixed problem with operator= in PDirectory class, part of larger change previously made.
 *
 * Revision 1.98  2001/01/30 06:37:52  yurik
 * Modification submitted by Dave Cassel, dcassel@cyberfone.com
 * DC: ARM, x86em were content without this change, but SH4 insisted
 *
 * Revision 1.97  2001/01/29 01:19:32  robertj
 * Fixed Win32 compile broken by WinCE changes.
 *
 * Revision 1.96  2001/01/28 01:50:46  yurik
 * WinCE port-related. System version check and new semaphore code
 *
 * Revision 1.95  2001/01/24 06:44:35  yurik
 * Windows CE port-related changes
 *
 * Revision 1.94  2000/11/02 01:31:11  robertj
 * Fixed problem with PSemaphore::WillBlock actually locking semaphore.
 *
 * Revision 1.93  2000/10/20 05:31:53  robertj
 * Added function to change auto delete flag on a thread.
 *
 * Revision 1.92  2000/08/25 08:07:48  robertj
 * Added Windows 2000 to operating system reporting.
 *
 * Revision 1.91  2000/04/05 02:50:18  robertj
 * Added microseconds to PTime class.
 *
 * Revision 1.90  2000/03/29 04:31:59  robertj
 * Removed assertion on terminating terminated thread, this is really OK.
 *
 * Revision 1.89  2000/03/04 08:07:27  robertj
 * Fixed problem with window not appearing when assert on GUI based win32 apps.
 *
 * Revision 1.88  2000/02/29 12:26:15  robertj
 * Added named threads to tracing, thanks to Dave Harvey
 *
 * Revision 1.87  2000/01/06 14:09:42  robertj
 * Fixed problems with starting up timers,losing up to 10 seconds
 *
 * Revision 1.86  1999/11/18 02:22:53  robertj
 * Fixed bug in GetErrorText() occasionally returning incorrect empty string, thanks Ulrich Findeisen
 *
 * Revision 1.85  1999/07/06 13:37:07  robertj
 * Fixed bug in PThread::IsSuspended(), returned exactly the opposite state!
 *
 * Revision 1.84  1999/07/06 04:46:01  robertj
 * Fixed being able to case an unsigned to a PTimeInterval.
 * Improved resolution of PTimer::Tick() to be millisecond accurate.
 *
 * Revision 1.83  1999/03/09 10:30:19  robertj
 * Fixed ability to have PMEMORY_CHECK on/off on both debug/release versions.
 *
 * Revision 1.82  1999/03/09 08:19:15  robertj
 * Adjustment found during documentation frenzy.
 *
 * Revision 1.81  1999/02/12 01:01:57  craigs
 * Fixed problem with linking static versions of libraries
 *
 * Revision 1.80  1999/01/30 14:28:25  robertj
 * Added GetOSConfigDir() function.
 *
 * Revision 1.79  1999/01/16 02:00:29  robertj
 * Added hardware description funtion.
 *
 * Revision 1.78  1998/12/04 10:10:47  robertj
 * Added virtual for determining if process is a service. Fixes linkage problem.
 *
 * Revision 1.77  1998/11/30 07:31:18  robertj
 * New directory structure
 * Fission of file into pipe.cxx, winserial.cxx and wincfg.cxx
 *
 * Revision 1.76  1998/11/26 10:35:08  robertj
 * Improved support of FAT32 and large NTFS volumes in GetFreeSpace().
 *
 * Revision 1.75  1998/11/20 03:17:19  robertj
 * Added thread WaitForTermination() function.
 *
 * Revision 1.74  1998/11/19 05:19:53  robertj
 * Bullet proofed WaitForMultipleObjects under 95.
 *
 * Revision 1.73  1998/11/02 10:07:20  robertj
 * Added capability of pip output to go to stdout/stderr.
 *
 * Revision 1.72  1998/10/31 12:50:47  robertj
 * Removed ability to start threads immediately, race condition with vtable (Main() function).
 * Rearranged PPipChannel functions to help with multi-platform-ness.
 *
 * Revision 1.71  1998/10/29 11:29:20  robertj
 * Added ability to set environment in sub-process.
 *
 * Revision 1.70  1998/10/28 00:59:12  robertj
 * Fixed problem when reading standard error from pipe channel, no terminating null on string.
 *
 * Revision 1.69  1998/10/26 09:11:31  robertj
 * Added ability to separate out stdout from stderr on pipe channels.
 *
 * Revision 1.68  1998/10/15 02:20:26  robertj
 * Added message for connection aborted error.
 *
 * Revision 1.67  1998/10/13 14:13:36  robertj
 * Removed uneeded heap allocation.
 *
 * Revision 1.66  1998/09/24 03:30:59  robertj
 * Added open software license.
 *
 * Revision 1.65  1998/09/18 13:56:20  robertj
 * Added support of REG_BINARY registry types in PConfig class.
 *
 * Revision 1.64  1998/08/20 06:05:28  robertj
 * Allowed Win32 class to be used in other compilation modules
 *
 * Revision 1.63  1998/04/01 01:52:42  robertj
 * Fixed problem with NoAutoDelete threads.
 *
 * Revision 1.62  1998/03/29 06:16:56  robertj
 * Rearranged initialisation sequence so PProcess descendent constructors can do "things".
 *
 * Revision 1.61  1998/03/27 10:52:39  robertj
 * Fixed crash bug in win95 OSR2 GetVolumeSpace().
 * Fixed error 87 problem with threads.
 * Fixed GetVolumeSpace() when UNC used.
 *
 * Revision 1.60  1998/03/20 03:19:49  robertj
 * Added special classes for specific sepahores, PMutex and PSyncPoint.
 *
 * Revision 1.59  1998/03/17 10:17:09  robertj
 * Fixed problem with viewing registry entries where the section ends with a \.
 *
 * Revision 1.58  1998/03/09 11:17:38  robertj
 * FAT32 compatibility
 *
 * Revision 1.57  1998/03/05 12:48:37  robertj
 * Fixed bug in get free space on volume.
 * Added cluster size.
 * MemCheck fixes.
 *
 * Revision 1.56  1998/02/16 00:10:45  robertj
 * Added function to open a URL in a browser.
 * Added functions to validate characters in a filename.
 *
 * Revision 1.55  1998/01/26 00:57:09  robertj
 * Fixed uninitialised source in PConfig when getting environment.
 *
 * Revision 1.54  1997/08/28 12:50:21  robertj
 * Fixed race condition in cleaning up threads on application termination.
 *
 * Revision 1.53  1997/08/21 13:27:41  robertj
 * Attempt to fix very slight possibility of endless loop in housekeeping thread.
 *
 * Revision 1.52  1997/08/21 12:44:56  robertj
 * Removed extension from DLL "short" name.
 *
 * Revision 1.51  1997/08/07 11:57:42  robertj
 * Added ability to get registry data from other applications and anywhere in system registry.
 *
 * Revision 1.50  1997/08/04 10:38:43  robertj
 * Fixed infamous error 87 assert failure in housekeeping thread.
 *
 * Revision 1.49  1997/07/14 11:47:22  robertj
 * Added "const" to numerous variables.
 *
 * Revision 1.48  1997/06/16 13:15:53  robertj
 * Added function to get a dyna-link libraries name.
 *
 * Revision 1.47  1997/06/08 04:42:41  robertj
 * Added DLL file extension string function.
 *
 * Revision 1.46  1997/03/28 04:36:30  robertj
 * Added assert for error in thread cleanup wait.
 *
 * Revision 1.45  1997/02/05 11:50:58  robertj
 * Changed current process function to return reference and validate objects descendancy.
 *
 * Revision 1.44  1997/01/12 04:24:16  robertj
 * Added function to get disk size and free space.
 *
 * Revision 1.43  1997/01/01 11:17:06  robertj
 * Added implementation for PPipeChannel::GetReturnCode and PPipeChannel::IsRunning
 *
 * Revision 1.44  1996/12/29 13:05:03  robertj
 * Added wait and abort for pipe channel commands.
 * Added setting of error codes on status error.
 *
 * Revision 1.43  1996/12/29 02:53:13  craigs
 * Added implementation for PPipeChannel::GetReturnCode and
 *   PPipeChannel::IsRunning
 *
 * Revision 1.42  1996/12/17 13:13:05  robertj
 * Fixed win95 support for registry security code,
 *
 * Revision 1.41  1996/12/17 11:00:28  robertj
 * Fixed register entry security access control lists.
 *
 * Revision 1.40  1996/11/16 10:52:48  robertj
 * Fixed bug in PPipeChannel test for open channel, win95 support.
 * Put time back to C function as run time library bug fixed now.
 *
 * Revision 1.39  1996/11/04 03:36:31  robertj
 * Added extra error message for UDP packet truncated.
 *
 * Revision 1.38  1996/10/26 01:42:51  robertj
 * Added more translations for winsock error codes to standard error codes.
 *
 * Revision 1.37  1996/10/14 03:11:25  robertj
 * Changed registry key so when reading only opens in ReadOnly mode.
 *
 * Revision 1.36  1996/10/08 13:03:47  robertj
 * Added new error messages.
 *
 * Revision 1.35  1996/08/08 10:03:43  robertj
 * Fixed static error text returned when no osError value.
 *
 * Revision 1.34  1996/07/27 04:05:31  robertj
 * Created static version of ConvertOSError().
 * Created static version of GetErrorText().
 * Changed thread creation to use C library function instead of direct WIN32.
 * Fixed bug in auto-deleting the housekeeping thread.
 *
 * Revision 1.33  1996/07/20 05:34:05  robertj
 * Fixed order of registry section tree traversal so can delete whole trees.
 *
 * Revision 1.32  1996/06/28 13:24:33  robertj
 * Fixed enumeration of sections to recurse into registry tree.
 *
 * Revision 1.31  1996/06/17 11:38:58  robertj
 * Fixed memory leak on termination of threads.
 *
 * Revision 1.30  1996/06/13 13:32:13  robertj
 * Rewrite of auto-delete threads, fixes Windows95 total crash.
 *
 * Revision 1.29  1996/06/10 09:54:35  robertj
 * Fixed Win95 compatibility for semaphores.
 *
 * Revision 1.28  1996/05/30 11:48:51  robertj
 * Fixed error on socket timeout to return "Timed Out".
 *
 * Revision 1.27  1996/05/23 10:05:36  robertj
 * Fixed bug in PConfig::GetBoolean().
 * Changed PTimeInterval millisecond access function so can get int64.
 * Moved service process code into separate module.
 *
 * Revision 1.26  1996/04/29 12:23:22  robertj
 * Fixed ability to access GDI stuff from subthreads.
 * Added function to return process ID.
 *
 * Revision 1.25  1996/04/17 12:09:30  robertj
 * Added service dependencies.
 * Started win95 support.
 *
 * Revision 1.24  1996/04/09 03:33:58  robertj
 * Fixed bug in incorrect report of timeout on socket read.
 *
 * Revision 1.23  1996/04/01 13:33:19  robertj
 * Fixed bug in install of service, incorrectly required installation before install.
 *
 * Revision 1.22  1996/03/31 09:10:33  robertj
 * Added use of "CurrentVersion" key in registry.
 * Added version display to service process.
 * Added another socket error text message.
 *
 * Revision 1.21  1996/03/12 11:31:39  robertj
 * Moved PProcess destructor to platform dependent code.
 * Fixed bug in deleting Event Viewer registry entry for service process.
 *
 * Revision 1.20  1996/03/10 13:16:49  robertj
 * Implemented system version functions.
 *
 * Revision 1.19  1996/03/04 13:07:33  robertj
 * Allowed for auto deletion of threads on termination.
 *
 * Revision 1.18  1996/02/25 11:15:29  robertj
 * Added platform dependent Construct function to PProcess.
 *
 * Revision 1.17  1996/02/25 03:12:48  robertj
 * Added consts to all GetXxxx functions in PConfig.
 * Fixed bug in PTime::GetTimeZone(), incorrect sign!
 * Fixed problem with PConfig get functions and their WIN32 types should be
 *    able to interchange strings and numbers.
 *
 * Revision 1.16  1996/02/19 13:53:21  robertj
 * Fixed error reporting for winsock classes.
 *
 * Revision 1.15  1996/02/15 14:54:06  robertj
 * Compensated for C library bug in time().
 *
 * Revision 1.14  1996/02/08 12:30:41  robertj
 * Time zone changes.
 * Added OS identification strings to PProcess.
 *
 * Revision 1.13  1996/01/28 02:56:56  robertj
 * Fixed bug in PFilePath functions for if path ends in a directory separator.
 * Made sure all directory separators are correct character in normalised path.
 *
 * Revision 1.12  1996/01/23 13:25:21  robertj
 * Added time zones.
 * Fixed bug if daylight savings indication.
 *
 * Revision 1.11  1996/01/02 12:58:33  robertj
 * Fixed copy of directories.
 * Changed process construction mechanism.
 * Made service process "common".
 *
 * Revision 1.10  1995/12/10 12:05:48  robertj
 * Changes to main() startup mechanism to support Mac.
 * Moved error code for specific WIN32 and MS-DOS versions.
 * Added WIN32 registry support for PConfig objects.
 * Added asserts in WIN32 semaphores.
 *
 * Revision 1.9  1995/11/21 11:53:24  robertj
 * Added timeout on semaphore wait.
 *
 * Revision 1.8  1995/10/14 15:13:04  robertj
 * Fixed bug in WIN32 service command line parameters.
 *
 * Revision 1.7  1995/08/24 12:42:33  robertj
 * Changed PChannel so not a PContainer.
 * Rewrote PSerialChannel::Read yet again so can break out of I/O.
 *
 * Revision 1.6  1995/07/02 01:26:52  robertj
 * Changed thread internal variables.
 * Added service process support for NT.
 *
 * Revision 1.5  1995/06/17 01:03:08  robertj
 * Added NT service process type.
 *
 * Revision 1.4  1995/06/04 12:48:52  robertj
 * Fixed bug in directory path creation.
 * Fixed bug in comms channel open error.
 *
 * Revision 1.3  1995/04/25 11:33:54  robertj
 * Fixed Borland compiler warnings.
 *
 * Revision 1.2  1995/03/22 13:56:18  robertj
 * Fixed directory handle value check for closing directory.
 *
// Revision 1.1  1995/03/14  12:45:20  robertj
// Initial revision
//
 */

#include <ptlib.h>
#include <ptlib/pprocess.h>

#include <ptlib/msos/ptlib/debstrm.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#ifdef __MINGW32__
#include <process.h>
#endif

#if defined(_MSC_VER) && !defined(_WIN32_WCE)
  #include <process.h>
  #pragma comment(lib, "mpr.lib")
#endif

#if defined(_WIN32_DCOM) 
  #include <objbase.h>
  #ifdef _MSC_VER
    #pragma comment(lib, _OLE_LIB)
  #endif
#endif

//#include "../common/pglobalstatic.cxx"


#define new PNEW


///////////////////////////////////////////////////////////////////////////////
// PTime

PTime::PTime()
{
  // Magic constant to convert epoch from 1601 to 1970
  static const PInt64 delta = ((PInt64)369*365+(369/4)-3)*24*60*60U;
  static const PInt64 scale = 10000000;

  PInt64 timestamp;

#ifndef _WIN32_WCE
  GetSystemTimeAsFileTime((LPFILETIME)&timestamp);
#else
  SYSTEMTIME SystemTime;
  GetSystemTime(&SystemTime);
  SystemTimeToFileTime(&SystemTime, (LPFILETIME)&timestamp);
#endif

  theTime = (time_t)(timestamp/scale - delta);
  microseconds = (long)(timestamp%scale/10);
}

#ifdef UNICODE
static void PWIN32GetLocaleInfo(LCID Locale,LCTYPE LCType,LPSTR lpLCData,int cchData)
{
  TCHAR* pw = new TCHAR[cchData+1];
  GetLocaleInfo(Locale,LCType,pw,cchData);
  lpLCData[0]=0;
  WideCharToMultiByte(GetACP(), 0, pw, -1, lpLCData, cchData, NULL, NULL);
}
#else

#define PWIN32GetLocaleInfo GetLocaleInfo

#endif



PString PTime::GetTimeSeparator()
{
  PString str;
  PWIN32GetLocaleInfo(GetUserDefaultLCID(), LOCALE_STIME, str.GetPointer(100), 99);
  str.MakeMinimumSize();
  return str;
}


BOOL PTime::GetTimeAMPM()
{
  char str[2];
  PWIN32GetLocaleInfo(GetUserDefaultLCID(), LOCALE_ITIME, str, sizeof(str));
  return str[0] == '0';
}


PString PTime::GetTimeAM()
{
  PString str;
  PWIN32GetLocaleInfo(GetUserDefaultLCID(), LOCALE_S1159, str.GetPointer(100), 99);
  str.MakeMinimumSize();
  return str;
}


PString PTime::GetTimePM()
{
  PString str;
  PWIN32GetLocaleInfo(GetUserDefaultLCID(), LOCALE_S2359, str.GetPointer(100), 99);
  str.MakeMinimumSize();
  return str;
}


PString PTime::GetDayName(Weekdays dayOfWeek, NameType type)
{
  PString str;
  // Of course Sunday is 6 and Monday is 1...
  PWIN32GetLocaleInfo(GetUserDefaultLCID(), (dayOfWeek+6)%7 +
          (type == Abbreviated ? LOCALE_SABBREVDAYNAME1 : LOCALE_SDAYNAME1),
          str.GetPointer(100), 99);
  str.MakeMinimumSize();
  return str;
}


PString PTime::GetDateSeparator()
{
  PString str;
  PWIN32GetLocaleInfo(GetUserDefaultLCID(), LOCALE_SDATE, str.GetPointer(100), 99);
  str.MakeMinimumSize();
  return str;
}


PString PTime::GetMonthName(Months month, NameType type)
{
  PString str;
  PWIN32GetLocaleInfo(GetUserDefaultLCID(), month-1 +
      (type == Abbreviated ? LOCALE_SABBREVMONTHNAME1 : LOCALE_SMONTHNAME1),
      str.GetPointer(100), 99);
  str.MakeMinimumSize();
  return str;
}


PTime::DateOrder PTime::GetDateOrder()
{
  char str[2];
  PWIN32GetLocaleInfo(GetUserDefaultLCID(), LOCALE_IDATE, str, sizeof(str));
  return (DateOrder)(str[0] - '0');
}


BOOL PTime::IsDaylightSavings()
{
  TIME_ZONE_INFORMATION tz;
  DWORD result = GetTimeZoneInformation(&tz);
  PAssertOS(result != 0xffffffff);
  return result == TIME_ZONE_ID_DAYLIGHT;
}


int PTime::GetTimeZone(TimeZoneType type)
{
  TIME_ZONE_INFORMATION tz;
  PAssertOS(GetTimeZoneInformation(&tz) != 0xffffffff);
  if (type == DaylightSavings)
    tz.Bias += tz.DaylightBias;
  return -tz.Bias;
}


PString PTime::GetTimeZoneString(TimeZoneType type)
{
  TIME_ZONE_INFORMATION tz;
  PAssertOS(GetTimeZoneInformation(&tz) != 0xffffffff);
  return (const WORD *)(type == StandardTime ? tz.StandardName : tz.DaylightName);
}


///////////////////////////////////////////////////////////////////////////////
// PTimeInterval 

static unsigned GetDivisor()
{
  LARGE_INTEGER frequency;
  if (QueryPerformanceFrequency(&frequency))
    return (unsigned)frequency.QuadPart/1000;

  return 0;
}

PTimeInterval PTimer::Tick()
{
  static unsigned divisor = GetDivisor();

  if (divisor == 0)
    return (int)(GetTickCount()&0x7fffffff);
  
  LARGE_INTEGER count;
  QueryPerformanceCounter(&count);
  return count.QuadPart/divisor;
}


unsigned PTimer::Resolution()
{
  LARGE_INTEGER frequency;
  if (QueryPerformanceFrequency(&frequency) && frequency.QuadPart >= 1000)
    return 1;

#ifndef _WIN32_WCE
  DWORD err = GetLastError();
  DWORD timeAdjustment;
  DWORD timeIncrement;
  BOOL timeAdjustmentDisabled;
  if (GetSystemTimeAdjustment(&timeAdjustment, &timeIncrement, &timeAdjustmentDisabled))
    return timeIncrement/10000;

  err = GetLastError();
#endif

  return 55;
}


///////////////////////////////////////////////////////////////////////////////
// Directories

void PDirectory::Construct()
{
  hFindFile = INVALID_HANDLE_VALUE;
  fileinfo.cFileName[0] = '\0';
  PCaselessString::AssignContents(CreateFullPath(*this, TRUE));
}


void PDirectory::CopyContents(const PDirectory & dir)
{
  scanMask  = dir.scanMask;
  hFindFile = INVALID_HANDLE_VALUE;
  fileinfo  = dir.fileinfo;
}


BOOL PDirectory::Open(int newScanMask)
{
  scanMask = newScanMask;
#ifdef UNICODE
  USES_CONVERSION;
  hFindFile = FindFirstFile(A2T(operator+("*.*")), &fileinfo);
#else
  hFindFile = FindFirstFile(operator+("*.*"), &fileinfo);
#endif
  if (hFindFile == INVALID_HANDLE_VALUE)
    return FALSE;

  return Filtered() ? Next() : TRUE;
}


BOOL PDirectory::Next()
{
  if (hFindFile == INVALID_HANDLE_VALUE)
    return FALSE;

  do {
    if (!FindNextFile(hFindFile, &fileinfo))
      return FALSE;
  } while (Filtered());

  return TRUE;
}


PCaselessString PDirectory::GetEntryName() const
{
  return fileinfo.cFileName;
}


BOOL PDirectory::IsSubDir() const
{
  return (fileinfo.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY) != 0;
}


PCaselessString PDirectory::GetVolume() const
{
#ifdef _WIN32_WCE
  return PCaselessString("\\");
#else
  char volName[100];
  PAssertOS(GetVolumeInformation(NULL, volName, sizeof(volName), NULL, NULL, NULL, NULL, 0));
  return PCaselessString(volName);
#endif
}


void PDirectory::Close()
{
  if (hFindFile != INVALID_HANDLE_VALUE) {
    FindClose(hFindFile);
    hFindFile = INVALID_HANDLE_VALUE;
  }
}


PString PDirectory::CreateFullPath(const PString & path, BOOL isDirectory)
{
  if (path.IsEmpty() && !isDirectory)
    return path;

#ifdef _WIN32_WCE //doesn't support Current Directory so the path suppose to be full
  PString fullpath=path;
  PINDEX len = fullpath.GetLength();

#else
  PString partialpath = path;

  // Look for special case of "\c:\" at start of string as some generalised
  // directory processing algorithms have a habit of adding a leading
  // PDIR_SEPARATOR as it would be for Unix.
  if (partialpath.NumCompare("\\\\\\") == EqualTo ||
        (partialpath.GetLength() > 3 &&
         partialpath[0] == PDIR_SEPARATOR &&
         partialpath[2] == ':'))
    partialpath.Delete(0, 1);

  LPSTR dummy;
  PString fullpath;
  PINDEX len = (PINDEX)GetFullPathName(partialpath,
                           _MAX_PATH, fullpath.GetPointer(_MAX_PATH), &dummy);
#endif
  if (isDirectory && len > 0 && fullpath[len-1] != PDIR_SEPARATOR)
    fullpath += PDIR_SEPARATOR;
  PINDEX pos = 0;
  while ((pos = fullpath.Find('/', pos)) != P_MAX_INDEX)
    fullpath[pos] = PDIR_SEPARATOR;
  return fullpath;
}


typedef BOOL (WINAPI *GetDiskFreeSpaceExType)(LPCTSTR lpDirectoryName,
                                              PULARGE_INTEGER lpFreeBytesAvailableToCaller,
                                              PULARGE_INTEGER lpTotalNumberOfBytes,
                                              PULARGE_INTEGER lpTotalNumberOfFreeBytes);


BOOL PDirectory::GetVolumeSpace(PInt64 & total, PInt64 & free, DWORD & clusterSize) const
{
  clusterSize = 512;
  total = free = ULONG_MAX;

  PString root;
  if ((*this)[1] == ':')
    root = Left(3);
  else if (theArray[0] == '\\' && theArray[1] == '\\') {
    PINDEX backslash = Find('\\', 2);
    if (backslash != P_MAX_INDEX) {
      backslash = Find('\\', backslash+1);
      if (backslash != P_MAX_INDEX)
        root = Left(backslash+1);
    }
  }

  if (root.IsEmpty())
    return FALSE;

#ifndef _WIN32_WCE
  BOOL needTotalAndFree = TRUE;

  static GetDiskFreeSpaceExType GetDiskFreeSpaceEx =
        (GetDiskFreeSpaceExType)GetProcAddress(LoadLibrary("KERNEL32.DLL"), "GetDiskFreeSpaceExA");
  if (GetDiskFreeSpaceEx != NULL) {
    ULARGE_INTEGER freeBytesAvailableToCaller;
    ULARGE_INTEGER totalNumberOfBytes; 
    ULARGE_INTEGER totalNumberOfFreeBytes;
    if (GetDiskFreeSpaceEx(root,
                           &freeBytesAvailableToCaller,
                           &totalNumberOfBytes,
                           &totalNumberOfFreeBytes)) {
      total = totalNumberOfBytes.QuadPart;
      free = totalNumberOfFreeBytes.QuadPart;
      needTotalAndFree = FALSE;
    }
  }

  clusterSize = 0;
  char fsName[100];
  if (GetVolumeInformation(root, NULL, 0, NULL, NULL, NULL, fsName, sizeof(fsName))) {
    if (strcasecmp(fsName, "FAT32") == 0) {
      clusterSize = 4096; // Cannot use GetDiskFreeSpace() results for FAT32
      if (!needTotalAndFree)
        return TRUE;
    }
  }

  DWORD sectorsPerCluster;      // address of sectors per cluster 
  DWORD bytesPerSector;         // address of bytes per sector 
  DWORD numberOfFreeClusters;   // address of number of free clusters  
  DWORD totalNumberOfClusters;  // address of total number of clusters 

  if (!GetDiskFreeSpace(root,
                        &sectorsPerCluster,
                        &bytesPerSector,
                        &numberOfFreeClusters,
                        &totalNumberOfClusters)) 
{
    if (root[0] != '\\' || ::GetLastError() != ERROR_NOT_SUPPORTED)
      return FALSE;

    PString drive = "A:";
    while (WNetAddConnection(root, NULL, drive) != NO_ERROR) {
      if (GetLastError() != ERROR_ALREADY_ASSIGNED)
        return FALSE;
      drive[0]++;
    }
    BOOL ok = GetDiskFreeSpace(drive+'\\',
                               &sectorsPerCluster,
                               &bytesPerSector,
                               &numberOfFreeClusters,
                               &totalNumberOfClusters);
    WNetCancelConnection(drive, TRUE);
    if (!ok)
      return FALSE;
  }

  if (needTotalAndFree) {
    free = numberOfFreeClusters*sectorsPerCluster*bytesPerSector;
    total = totalNumberOfClusters*sectorsPerCluster*bytesPerSector;
  }

  if (clusterSize == 0)
    clusterSize = bytesPerSector*sectorsPerCluster;

  return TRUE;
#elif _WIN32_WCE < 300
  USES_CONVERSION;
    ULARGE_INTEGER freeBytesAvailableToCaller;
    ULARGE_INTEGER totalNumberOfBytes; 
    ULARGE_INTEGER totalNumberOfFreeBytes;
    if (GetDiskFreeSpaceEx(A2T(root),
                           &freeBytesAvailableToCaller,
                           &totalNumberOfBytes,
                           &totalNumberOfFreeBytes)) 
  {
    total = totalNumberOfBytes.QuadPart;
    free = totalNumberOfFreeBytes.QuadPart;
    clusterSize = 512; //X3
    return TRUE;
  }
  return FALSE;
#else
  return FALSE;
#endif
}


///////////////////////////////////////////////////////////////////////////////
// PFilePath

static PString IllegalFilenameCharacters =
  "\\/:*?\"<>|"
  "\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f\0x10"
  "\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1a\x1b\x1c\x1d\x1e\x1f";

BOOL PFilePath::IsValid(char c)
{
  return IllegalFilenameCharacters.Find(c) == P_MAX_INDEX;
}


BOOL PFilePath::IsValid(const PString & str)
{
  return str != "." && str != ".." &&
         str.FindOneOf(IllegalFilenameCharacters) == P_MAX_INDEX;
}



///////////////////////////////////////////////////////////////////////////////
// PChannel

PString PChannel::GetErrorText(Errors lastError, int osError)
{
  if (osError == 0) {
    if (lastError == NoError)
      return PString();

    static int const errors[NumNormalisedErrors] = {
      0, ENOENT, EEXIST, ENOSPC, EACCES, EBUSY, EINVAL, ENOMEM, EBADF, EAGAIN, EINTR,
      WSAEMSGSIZE|PWIN32ErrorFlag, EIO, 0x1000000|PWIN32ErrorFlag
    };
    osError = errors[lastError];
  }
#ifndef _WIN32_WCE
  if (osError > 0 && osError < _sys_nerr && _sys_errlist[osError][0] != '\0')
    return _sys_errlist[osError];
#endif
  if ((osError & PWIN32ErrorFlag) == 0)
    return psprintf("C runtime error %u", osError);

  DWORD err = osError & ~PWIN32ErrorFlag;

  static const struct {
    DWORD id;
    const char * msg;
  } win32_errlist[] = {
    { ERROR_FILE_NOT_FOUND,     "File not found" },
    { ERROR_PATH_NOT_FOUND,     "Path not found" },
    { ERROR_ACCESS_DENIED,      "Access denied" },
    { ERROR_NOT_ENOUGH_MEMORY,  "Not enough memory" },
    { ERROR_INVALID_FUNCTION,   "Invalid function" },
    { WSAEADDRINUSE,            "Address in use" },
    { WSAENETDOWN,              "Network subsystem failed" },
    { WSAEISCONN,               "Socket is already connected" },
    { WSAENETUNREACH,           "Network unreachable" },
    { WSAEHOSTUNREACH,          "Host unreachable" },
    { WSAECONNREFUSED,          "Connection refused" },
    { WSAEINVAL,                "Invalid operation" },
    { WSAENOTCONN,              "Socket not connected" },
    { WSAECONNABORTED,          "Connection aborted" },
    { WSAECONNRESET,            "Connection reset" },
    { WSAESHUTDOWN,             "Connection shutdown" },
    { WSAENOTSOCK,              "Socket closed or invalid" },
    { WSAETIMEDOUT,             "Timed out" },
    { WSAEMSGSIZE,              "Message larger than buffer" },
    { WSAEWOULDBLOCK,           "Would block" },
    { 0x1000000,                "High level protocol failure" }
  };

  for (PINDEX i = 0; i < PARRAYSIZE(win32_errlist); i++)
    if (win32_errlist[i].id == err)
      return win32_errlist[i].msg;

  return psprintf("WIN32 error %u", err);
}


BOOL PChannel::ConvertOSError(int status, Errors & lastError, int & osError)
{
  if (status >= 0) {
    lastError = NoError;
    osError = 0;
    return TRUE;
  }

  if (status != -2)
    osError = errno;
  else {
    osError = GetLastError();
    switch (osError) {
      case ERROR_INVALID_HANDLE :
      case WSAEBADF :
      case WSAENOTSOCK :
        osError = EBADF;
        break;
      case ERROR_INVALID_PARAMETER :
      case WSAEINVAL :
        osError = EINVAL;
        break;
      case ERROR_ACCESS_DENIED :
      case WSAEACCES :
        osError = EACCES;
        break;
      case ERROR_NOT_ENOUGH_MEMORY :
        osError = ENOMEM;
        break;
      case WSAEINTR :
        osError = EINTR;
        break;
      case WSAEMSGSIZE :
        osError |= PWIN32ErrorFlag;
        lastError = BufferTooSmall;
        return FALSE;
      case WSAEWOULDBLOCK :
      case WSAETIMEDOUT :
        osError |= PWIN32ErrorFlag;
        lastError = Timeout;
        return FALSE;
      default :
        osError |= PWIN32ErrorFlag;
    }
  }

  switch (osError) {
    case 0 :
      lastError = NoError;
      return TRUE;
    case ENOENT :
      lastError = NotFound;
      break;
    case EEXIST :
      lastError = FileExists;
      break;
    case EACCES :
      lastError = AccessDenied;
      break;
    case ENOMEM :
      lastError = NoMemory;
      break;
    case ENOSPC :
      lastError = DiskFull;
      break;
    case EINVAL :
      lastError = BadParameter;
      break;
    case EBADF :
      lastError = NotOpen;
      break;
    case EAGAIN :
      lastError = Timeout;
      break;
    case EINTR :
      lastError = Interrupted;
      break;
    default :
      lastError = Miscellaneous;
  }

  return FALSE;
}


///////////////////////////////////////////////////////////////////////////////
// PWin32Overlapped

PWin32Overlapped::PWin32Overlapped()
{
  memset(this, 0, sizeof(*this));
  hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
}

PWin32Overlapped::~PWin32Overlapped()
{
  if (hEvent != NULL)
    CloseHandle(hEvent);
}

void PWin32Overlapped::Reset()
{
  Offset = OffsetHigh = 0;
  if (hEvent != NULL)
    ResetEvent(hEvent);
}


///////////////////////////////////////////////////////////////////////////////
// Threads

UINT __stdcall PThread::MainFunction(void * threadPtr)
{
  PThread * thread = (PThread *)PAssertNULL(threadPtr);
  thread->SetThreadName(thread->GetThreadName());

  PProcess & process = PProcess::Current();

/*
 * Removed this code because it causes a linear increase
 * in thread startup time when there are many (< 500) threads.
 * If this functionality is needed, call Win32AttachThreadInput
 * after the thread has been started
 *
#ifndef _WIN32_WCE
  AttachThreadInput(thread->threadId, ((PThread&)process).threadId, TRUE);
  AttachThreadInput(((PThread&)process).threadId, thread->threadId, TRUE);
#endif
*/

  process.activeThreadMutex.Wait();
  process.activeThreads.SetAt(thread->threadId, thread);
  process.activeThreadMutex.Signal();

  process.SignalTimerChange();

#if defined(_WIN32_DCOM)
  ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
#endif

  thread->Main();

#if defined(_WIN32_DCOM)
  ::CoUninitialize();
#endif

  return 0;
}


void PThread::Win32AttachThreadInput()
{
#ifndef _WIN32_WCE
  PProcess & process = PProcess::Current();
  ::AttachThreadInput(threadId, ((PThread&)process).threadId, TRUE);
  ::AttachThreadInput(((PThread&)process).threadId, threadId, TRUE);
#endif
}


PThread::PThread(PINDEX stackSize,
                 AutoDeleteFlag deletion,
                 Priority priorityLevel,
                 const PString & name)
  : threadName(name)
{
  PAssert(stackSize > 0, PInvalidParameter);
  originalStackSize = stackSize;

  autoDelete = deletion == AutoDeleteThread;

#ifndef _WIN32_WCE
  threadHandle = (HANDLE)_beginthreadex(NULL, stackSize, MainFunction,
                                        this, CREATE_SUSPENDED, &threadId);
#else
   threadHandle = CreateThread(NULL, stackSize, 
                               (LPTHREAD_START_ROUTINE)MainFunction,
                               this, CREATE_SUSPENDED, (LPDWORD) &threadId);
#endif

  PAssertOS(threadHandle != NULL);

  SetPriority(priorityLevel);

#if PTRACING
  traceBlockIndentLevel = 0;
#endif

  if (autoDelete) {
    PProcess & process = PProcess::Current();
    process.deleteThreadMutex.Wait();
    process.autoDeleteThreads.Append(this);
    process.deleteThreadMutex.Signal();
  }
}


PThread::~PThread()
{
  if (originalStackSize <= 0)
    return;

  PProcess & process = PProcess::Current();
  process.activeThreadMutex.Wait();
  process.activeThreads.SetAt(threadId, NULL);
  process.activeThreadMutex.Signal();

  if (!IsTerminated())
    Terminate();

  if (threadHandle != NULL)
    CloseHandle(threadHandle);
}


void PThread::Restart()
{
  PAssert(IsTerminated(), "Cannot restart running thread");

#ifndef _WIN32_WCE
  threadHandle = (HANDLE)_beginthreadex(NULL,
                         originalStackSize, MainFunction, this, 0, &threadId);
#else
   threadHandle = CreateThread(NULL, originalStackSize, 
                (LPTHREAD_START_ROUTINE) MainFunction,
                  this, 0, (LPDWORD) &threadId);
#endif
  PAssertOS(threadHandle != NULL);
}


void PThread::Terminate()
{
  PAssert(originalStackSize > 0, PLogicError);

  if (Current() == this)
    ExitThread(0);
  else
    TerminateThread(threadHandle, 1);
}


BOOL PThread::IsTerminated() const
{
  if (this == PThread::Current())
    return FALSE;

  return WaitForTermination(0);
}


void PThread::WaitForTermination() const
{
  WaitForTermination(PMaxTimeInterval);
}


BOOL PThread::WaitForTermination(const PTimeInterval & maxWait) const
{
  if ((this == PThread::Current()) || threadHandle == NULL) {
    PTRACE(3, "PWLib\tWaitForTermination short circuited");
    return TRUE;
  }

  DWORD result;
  PINDEX retries = 10;
  while ((result = WaitForSingleObject(threadHandle, maxWait.GetInterval())) != WAIT_TIMEOUT) {
    if (result == WAIT_OBJECT_0)
      return TRUE;

    if (::GetLastError() != ERROR_INVALID_HANDLE) {
      PAssertAlways(POperatingSystemError);
      return TRUE;
    }

    if (retries == 0)
      return TRUE;

    retries--;
  }

  return FALSE;
}


void PThread::Suspend(BOOL susp)
{
  PAssert(!IsTerminated(), "Operation on terminated thread");
  if (susp)
    SuspendThread(threadHandle);
  else
    Resume();
}


void PThread::Resume()
{
  PAssert(!IsTerminated(), "Operation on terminated thread");
  ResumeThread(threadHandle);
}


BOOL PThread::IsSuspended() const
{
  SuspendThread(threadHandle);
  return ResumeThread(threadHandle) > 1;
}


void PThread::SetAutoDelete(AutoDeleteFlag deletion)
{
  PAssert(deletion != AutoDeleteThread || this != &PProcess::Current(), PLogicError);

  PProcess & process = PProcess::Current();

  if (autoDelete && deletion != AutoDeleteThread) {
    process.deleteThreadMutex.Wait();
    process.autoDeleteThreads.DisallowDeleteObjects();
    process.autoDeleteThreads.Remove(this);
    process.autoDeleteThreads.AllowDeleteObjects();
    process.deleteThreadMutex.Signal();
  }
  else if (!autoDelete && deletion == AutoDeleteThread) {
    process.deleteThreadMutex.Wait();
    process.autoDeleteThreads.Append(this);
    process.deleteThreadMutex.Signal();
  }

  autoDelete = deletion == AutoDeleteThread;
}

#if !defined(_WIN32_WCE) || (_WIN32_WCE < 300)
#define PTHREAD_PRIORITY_LOWEST THREAD_PRIORITY_LOWEST
#define PTHREAD_PRIORITY_BELOW_NORMAL THREAD_PRIORITY_BELOW_NORMAL
#define PTHREAD_PRIORITY_NORMAL THREAD_PRIORITY_NORMAL
#define PTHREAD_PRIORITY_ABOVE_NORMAL THREAD_PRIORITY_ABOVE_NORMAL
#define PTHREAD_PRIORITY_HIGHEST THREAD_PRIORITY_HIGHEST
#else
#define PTHREAD_PRIORITY_LOWEST 243
#define PTHREAD_PRIORITY_BELOW_NORMAL 245
#define PTHREAD_PRIORITY_NORMAL 247
#define PTHREAD_PRIORITY_ABOVE_NORMAL 249
#define PTHREAD_PRIORITY_HIGHEST 251
#endif

void PThread::SetPriority(Priority priorityLevel)
{
  PAssert(!IsTerminated(), "Operation on terminated thread");

  static int const priorities[NumPriorities] = {
    PTHREAD_PRIORITY_LOWEST,
    PTHREAD_PRIORITY_BELOW_NORMAL,
    PTHREAD_PRIORITY_NORMAL,
    PTHREAD_PRIORITY_ABOVE_NORMAL,
    PTHREAD_PRIORITY_HIGHEST
  };
  SetThreadPriority(threadHandle, priorities[priorityLevel]);
}


PThread::Priority PThread::GetPriority() const
{
  PAssert(!IsTerminated(), "Operation on terminated thread");

  switch (GetThreadPriority(threadHandle)) {
    case PTHREAD_PRIORITY_LOWEST :
      return LowestPriority;
    case PTHREAD_PRIORITY_BELOW_NORMAL :
      return LowPriority;
    case PTHREAD_PRIORITY_NORMAL :
      return NormalPriority;
    case PTHREAD_PRIORITY_ABOVE_NORMAL :
      return HighPriority;
    case PTHREAD_PRIORITY_HIGHEST :
      return HighestPriority;
  }
  PAssertAlways(POperatingSystemError);
  return LowestPriority;
}


void PThread::Yield()
{
  ::Sleep(0);
}


void PThread::InitialiseProcessThread()
{
  originalStackSize = 0;
  autoDelete = FALSE;
  threadHandle = GetCurrentThread();
  threadId = GetCurrentThreadId();
  ((PProcess *)this)->activeThreads.DisallowDeleteObjects();
  ((PProcess *)this)->activeThreads.SetAt(threadId, this);
#if PTRACING
  traceBlockIndentLevel = 0;
#endif
}


PThread * PThread::Current()
{
  PProcess & process = PProcess::Current();
  process.activeThreadMutex.Wait();
  PThread * thread = process.activeThreads.GetAt(GetCurrentThreadId());
  process.activeThreadMutex.Signal();
  return thread;
}


///////////////////////////////////////////////////////////////////////////////
// PProcess::TimerThread

PProcess::HouseKeepingThread::HouseKeepingThread()
  : PThread(1000, NoAutoDeleteThread, NormalPriority, "PWLib Housekeeper")
{
  Resume();
}


void PProcess::HouseKeepingThread::Main()
{
  PProcess & process = PProcess::Current();

  for (;;) {

    // collect a list of thread handles to check, and clean up 
    // handles for threads that disappeared without telling us
    process.deleteThreadMutex.Wait();
    HANDLE handles[MAXIMUM_WAIT_OBJECTS];
    DWORD numHandles = 1;
    handles[0] = breakBlock.GetHandle();
    for (PINDEX i = 0; i < process.autoDeleteThreads.GetSize(); i++) {
      PThread & thread = process.autoDeleteThreads[i];
      if (thread.IsTerminated())
        process.autoDeleteThreads.RemoveAt(i--);

      else {
        handles[numHandles] = thread.GetHandle();

        // make sure we don't put invalid handles into the list
#ifndef _WIN32_WCE
        DWORD dwFlags;
        if (GetHandleInformation(handles[numHandles], &dwFlags) == 0) {
          PTRACE(2, "PWLib\tRefused to put invalid handle into wait list");
        }
        else
#endif
        // don't put the handle for the current process in the list
    if (handles[numHandles] != process.GetHandle()) {
          numHandles++;
          if (numHandles >= MAXIMUM_WAIT_OBJECTS)
            break;
        }
      }
    }
    process.deleteThreadMutex.Signal();

    PTimeInterval nextTimer = process.timers.Process();
    DWORD delay;
    if (nextTimer == PMaxTimeInterval)
      delay = INFINITE;
    else if (nextTimer > 1000)
      delay = 1000;
    else
      delay = nextTimer.GetInterval();

    DWORD result;
    int retries = 100;

    while ((result = WaitForMultipleObjects(numHandles, handles, FALSE, delay)) == WAIT_FAILED) {

      // if we get an invalid handle error, than assume this is because a thread ended between
      // creating the handle list and testing it. So, cleanup the list before calling 
      // WaitForMultipleObjects again
      if (::GetLastError() == ERROR_INVALID_HANDLE)
        break;

      // sometimes WaitForMultipleObjects fails. No idea why, so allow some retries
      else {
        retries--;
        if (retries <= 0)
          break;
      }
    }
  }
}


void PProcess::SignalTimerChange()
{
  deleteThreadMutex.Wait();
  if (houseKeeper == NULL)
    houseKeeper = new HouseKeepingThread;
  else
    houseKeeper->breakBlock.Signal();
  deleteThreadMutex.Signal();
}


///////////////////////////////////////////////////////////////////////////////
// PProcess

PProcess::~PProcess()
{
  // do whatever needs to shutdown
  PreShutdown();

  Sleep(100);  // Give threads time to die a natural death

  // Get rid of the house keeper (majordomocide)
  delete houseKeeper;

  // OK, if there are any left we get really insistent...
  activeThreadMutex.Wait();
  for (PINDEX i = 0; i < activeThreads.GetSize(); i++) {
    PThread & thread = activeThreads.GetDataAt(i);
    if (this != &thread && !thread.IsTerminated())
      TerminateThread(thread.GetHandle(), 1);  // With extreme prejudice
  }
  activeThreadMutex.Signal();

  deleteThreadMutex.Wait();
  autoDeleteThreads.RemoveAll();
  deleteThreadMutex.Signal();

#if _DEBUG
  extern void PWaitOnExitConsoleWindow();
  PWaitOnExitConsoleWindow();
#endif
}


PString PProcess::GetOSClass()
{
  return "Windows";
}


PString PProcess::GetOSName()
{
  OSVERSIONINFOEX info;
  info.dwOSVersionInfoSize = sizeof(info);
  GetVersionEx((OSVERSIONINFO*)&info);
  switch (info.dwPlatformId) {
    case VER_PLATFORM_WIN32s :
      return "32s";

#ifdef VER_PLATFORM_WIN32_CE
    case VER_PLATFORM_WIN32_CE :
      return "CE";
#endif

    case VER_PLATFORM_WIN32_WINDOWS :
      if (info.dwMinorVersion < 10)
        return "95";
      if (info.dwMinorVersion < 90)
        return "98";
      return "ME";

    case VER_PLATFORM_WIN32_NT :
      switch (info.dwMajorVersion) {
        case 4 :
          return "NT";
        case 5:
          switch (info.dwMinorVersion) {
            case 0 :
              return "2000";
            case 1 :
              return "XP";
          }
          if(info.wProductType!=VER_NT_WORKSTATION) return "Server 2003";
          else return "XP Professional x64 Edition";

        case 6 :
          if(info.wProductType==VER_NT_WORKSTATION)
          switch (info.dwMinorVersion) {
            case 0 :
              return "Vista";
            case 1 :
              return "7";
            case 2 :
              return "8";
          }
          else
          switch (info.dwMinorVersion) {
            case 0 :
              return "Server 2008";
            case 1 :
              return "Server 2008 R2";
            case 2 :
              return "Server 2012";
          }
      }
  }
  return "?";
}


PString PProcess::GetOSHardware()
{
  SYSTEM_INFO info;
  GetSystemInfo(&info);
  switch (info.wProcessorArchitecture) {
    case PROCESSOR_ARCHITECTURE_INTEL :
      switch (info.dwProcessorType) {
        case PROCESSOR_INTEL_386 :
          return "i386";
        case PROCESSOR_INTEL_486 :
          return "i486";
        case PROCESSOR_INTEL_PENTIUM :
          return psprintf("i586 (Model=%u Stepping=%u)", info.wProcessorRevision>>8, info.wProcessorRevision&0xff);
      }
      return "iX86";

    case PROCESSOR_ARCHITECTURE_MIPS :
      return "mips";

    case PROCESSOR_ARCHITECTURE_ALPHA :
      return "alpha";

    case PROCESSOR_ARCHITECTURE_PPC :
      return "ppc";
  }
  return "?";
}


PString PProcess::GetOSVersion()
{
  OSVERSIONINFO info;
  info.dwOSVersionInfoSize = sizeof(info);
  GetVersionEx(&info);
  WORD wBuildNumber = (WORD)info.dwBuildNumber;
  return psprintf(wBuildNumber > 0 ? "v%u.%u.%u" : "v%u.%u",
                  info.dwMajorVersion, info.dwMinorVersion, wBuildNumber);
}


PDirectory PProcess::GetOSConfigDir()
{
#ifdef _WIN32_WCE
  return PString("\\Windows");
#else
  OSVERSIONINFO info;
  info.dwOSVersionInfoSize = sizeof(info);
  GetVersionEx(&info);

  char dir[_MAX_PATH];

  if (info.dwPlatformId != VER_PLATFORM_WIN32_NT) {
    PAssertOS(GetWindowsDirectory(dir, sizeof(dir)) != 0);
    return dir;
  }

  PAssertOS(GetSystemDirectory(dir, sizeof(dir)) != 0);
  PDirectory sysdir = dir;
  return sysdir;  //+ "drivers\\etc";
#endif
}

PString PProcess::GetUserName() const
{
  PString username;
#ifndef _WIN32_WCE
  unsigned long size = 50;
  ::GetUserName(username.GetPointer((PINDEX)size), &size);
#else
  TCHAR wcsuser[50] = {0};
  HKEY hKeyComm, hKeyIdent;
  RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("Comm"), 0, 0, &hKeyComm);
  RegOpenKeyEx(hKeyComm, _T("Ident"), 0, 0, &hKeyIdent);

  DWORD dwType = REG_SZ; DWORD dw = 50;
  if( ERROR_SUCCESS != RegQueryValueEx(
    hKeyIdent, _T("Username"), NULL, &dwType, (LPBYTE) wcsuser, &dw) 
    || !*wcsuser )
  {
  RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("Ident"), 0, 0, &hKeyIdent);
  dw = 50L;
  if( ERROR_SUCCESS == RegQueryValueEx( 
    hKeyIdent, _T("Name"), NULL, &dwType, (LPBYTE) wcsuser, &dw))
      wcscat( wcsuser, _T(" user") ); // like "Pocket_PC User"
  }
  
  USES_CONVERSION;
  username = T2A(wcsuser);
#endif
  username.MakeMinimumSize();
  return username;
}


BOOL PProcess::SetUserName(const PString & username, BOOL)
{
  if (username.IsEmpty())
    return FALSE;

  PAssertAlways(PUnimplementedFunction);
  return FALSE;
}


PString PProcess::GetGroupName() const
{
  return "Users";
}


BOOL PProcess::SetGroupName(const PString & groupname, BOOL)
{
  if (groupname.IsEmpty())
    return FALSE;

  PAssertAlways(PUnimplementedFunction);
  return FALSE;
}


DWORD PProcess::GetProcessID() const
{
  return GetCurrentProcessId();
}


BOOL PProcess::IsServiceProcess() const
{
  return FALSE;
}


BOOL PProcess::IsGUIProcess() const
{
  return FALSE;
}


///////////////////////////////////////////////////////////////////////////////
// PSemaphore

PSemaphore::PSemaphore(HANDLE h)
{
  handle = h;
  PAssertOS(handle != NULL);
}


PSemaphore::PSemaphore(unsigned initial, unsigned maxCount)
{
  initialVal  = initial;
  maxCountVal = maxCount;

  if (initial > maxCount)
    initial = maxCount;
  handle = CreateSemaphore(NULL, initial, maxCount, NULL);
  PAssertOS(handle != NULL);
}

PSemaphore::PSemaphore(const PSemaphore & sem)
{
  initialVal  = sem.GetInitialVal();
  maxCountVal = sem.GetMaxCountVal();

  if (initialVal > maxCountVal)
    initialVal = maxCountVal;
  handle = CreateSemaphore(NULL, initialVal, maxCountVal, NULL);
  PAssertOS(handle != NULL);
}

PSemaphore::~PSemaphore()
{
  if (handle != NULL)
    PAssertOS(CloseHandle(handle));
}


void PSemaphore::Wait()
{
  PAssertOS(WaitForSingleObject(handle, INFINITE) != WAIT_FAILED);
}


BOOL PSemaphore::Wait(const PTimeInterval & timeout)
{
  DWORD result = WaitForSingleObject(handle, timeout.GetInterval());
  PAssertOS(result != WAIT_FAILED);
  return result != WAIT_TIMEOUT;
}


void PSemaphore::Signal()
{
  if (!ReleaseSemaphore(handle, 1, NULL))
    PAssertOS(GetLastError() != ERROR_INVALID_HANDLE);
  SetLastError(ERROR_SUCCESS);
}


BOOL PSemaphore::WillBlock() const
{
  PSemaphore * unconst = (PSemaphore *)this;

  if (!unconst->Wait(0))
    return TRUE;

  unconst->Signal();
  return FALSE;
}


///////////////////////////////////////////////////////////////////////////////
// PTimedMutex

PTimedMutex::PTimedMutex()
  : PSemaphore(::CreateMutex(NULL, FALSE, NULL))
{
}

PTimedMutex::PTimedMutex(const PTimedMutex &)
  : PSemaphore(::CreateMutex(NULL, FALSE, NULL))
{
}

void PTimedMutex::Signal()
{
  PAssertOS(::ReleaseMutex(handle));
}

///////////////////////////////////////////////////////////////////////////////
// PSyncPoint

PSyncPoint::PSyncPoint()
  : PSemaphore(::CreateEvent(NULL, FALSE, FALSE, NULL))
{
}

PSyncPoint::PSyncPoint(const PSyncPoint &)
  : PSemaphore(::CreateEvent(NULL, FALSE, FALSE, NULL))
{
}

void PSyncPoint::Signal()
{
  PAssertOS(::SetEvent(handle));
}


///////////////////////////////////////////////////////////////////////////////
// PDynaLink

PDynaLink::PDynaLink()
{
  _hDLL = NULL;
}


PDynaLink::PDynaLink(const PString & name)
{
  Open(name);
}


PDynaLink::~PDynaLink()
{
  Close();
}


PString PDynaLink::GetExtension()
{
  return ".DLL";
}


BOOL PDynaLink::Open(const PString & name)
{
#ifdef UNICODE
  USES_CONVERSION;
  _hDLL = LoadLibrary(A2T(name));
#else
  _hDLL = LoadLibrary(name);
#endif
  return _hDLL != NULL;
}


void PDynaLink::Close()
{
  if (_hDLL != NULL) {
    FreeLibrary(_hDLL);
    _hDLL = NULL;
  }
}


BOOL PDynaLink::IsLoaded() const
{
  return _hDLL != NULL;
}


PString PDynaLink::GetName(BOOL full) const
{
  PFilePathString str;
  if (_hDLL != NULL) 
{

#ifdef UNICODE
  TCHAR path[_MAX_PATH];
    GetModuleFileName(_hDLL, path, _MAX_PATH-1);
    str=PString(path);
#else
    GetModuleFileName(_hDLL, str.GetPointer(_MAX_PATH), _MAX_PATH-1);
#endif
    if (!full) 
    {
      str.Delete(0, str.FindLast('\\')+1);
      PINDEX pos = str.Find(".DLL");
      if (pos != P_MAX_INDEX)
        str.Delete(pos, P_MAX_INDEX);
    }
  }
  str.MakeMinimumSize();
  return str;
}


BOOL PDynaLink::GetFunction(PINDEX index, Function & func)
{
  if (_hDLL == NULL)
    return FALSE;

#ifndef _WIN32_WCE 
  FARPROC p = GetProcAddress(_hDLL, (LPSTR)(DWORD)LOWORD(index));
#else
 FARPROC p = GetProcAddress(_hDLL, (LPTSTR)(DWORD)LOWORD(index));
#endif
  if (p == NULL)
    return FALSE;

  func = (Function)p;
  return TRUE;
}


BOOL PDynaLink::GetFunction(const PString & name, Function & func)
{
  if (_hDLL == NULL)
    return FALSE;

#ifdef UNICODE
  USES_CONVERSION;
  FARPROC p = GetProcAddress(_hDLL, A2T(name));
#else
  FARPROC p = GetProcAddress(_hDLL, name);
#endif
  if (p == NULL)
    return FALSE;

  func = (Function)p;
  return TRUE;
}


///////////////////////////////////////////////////////////////////////////////
// PDebugStream

PDebugStream::PDebugStream()
  : ostream(&buffer)
{
}


PDebugStream::Buffer::Buffer()
{
  setg(buffer, buffer, &buffer[sizeof(buffer)-2]);
  setp(buffer, &buffer[sizeof(buffer)-2]);
}


int PDebugStream::Buffer::overflow(int c)
{
  int bufSize = pptr() - pbase();

  if (c != EOF) {
    *pptr() = (char)c;
    bufSize++;
  }

  if (bufSize != 0) {
    char * p = pbase();
    setp(p, epptr());
    p[bufSize] = '\0';

#ifdef UNICODE
    USES_CONVERSION;
    OutputDebugString(A2T(p));
#else
    OutputDebugString(p);
#endif
  }

  return 0;
}


int PDebugStream::Buffer::underflow()
{
  return EOF;
}


int PDebugStream::Buffer::sync()
{
  return overflow(EOF);
}

// End Of File ///////////////////////////////////////////////////////////////
