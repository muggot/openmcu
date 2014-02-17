/*
 * object.cxx
 *
 * Global object support.
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
 * $Log: object.cxx,v $
 * Revision 1.92  2007/10/03 01:18:46  rjongbloed
 * Fixed build for Windows Mobile 5 and added Windows Mobile 6
 *
 * Revision 1.91  2007/09/12 18:44:26  ykiryanov
 * Added !defined(_WIN32_WCE)  to new memory leakage dectection code. Memory organized other way on WCE based devices and crtdbg.h API  is not present
 *
 * Revision 1.90  2007/09/12 00:55:41  rjongbloed
 * Improved memory leak detection, reduce false positives.
 *
 * Revision 1.89  2007/09/09 09:40:26  rjongbloed
 * Prevented memory leak detection from considering anything
 *   allocated before the PProcess constructor is complete.
 *
 * Revision 1.88  2007/09/08 11:34:29  rjongbloed
 * Improved memory checking (leaks etc), especially when using MSVC debug library.
 *
 * Revision 1.87  2007/07/06 02:44:33  csoutheren
 * Fixed compile on Linux
 *
 * Revision 1.86  2007/05/03 14:44:51  vfrolov
 * Fixed using destructed c-string in PAssertFunc
 *
 * Revision 1.85  2007/04/08 01:53:37  ykiryanov
 * Build to support ptlib dll creation
 *
 * Revision 1.84  2006/06/21 03:28:44  csoutheren
 * Various cleanups thanks for Frederic Heem
 *
 * Revision 1.83  2005/12/12 00:24:26  dereksmithies
 * Add recursive mutex to Wrapper object, so one can successfully Dump Memory
 * usage Statistics & objects during program operation.
 *
 * Revision 1.82  2005/09/23 15:30:46  dominance
 * more progress to make mingw compile nicely. Thanks goes to Julien Puydt for pointing out to me how to do it properly. ;)
 *
 * Revision 1.81  2004/10/24 03:43:55  rjongbloed
 * Used correct test for inclusion of pragma implementation
 *
 * Revision 1.80  2004/10/23 10:55:40  ykiryanov
 * Added ifdef _WIN32_WCE for PocketPC 2003 SDK port
 *
 * Revision 1.79  2004/07/11 07:56:36  csoutheren
 * Applied jumbo VxWorks patch, thanks to Eize Slange
 *
 * Revision 1.78  2004/07/01 11:41:30  csoutheren
 * Fixed compile and run problems on Linux
 *
 * Revision 1.77  2004/06/30 12:17:06  rjongbloed
 * Rewrite of plug in system to use single global variable for all factories to avoid all sorts
 *   of issues with startup orders and Windows DLL multiple instances.
 *
 * Revision 1.76  2004/06/29 08:20:46  rogerhardiman
 * The throws are needed on new and delete in GCC 2.95.x too.
 * This is now important as other code changes (factory and #include <string>)
 * now cause /usr/include/g++/new to be included too. Tested on FreeBSD 4.10
 *
 * Revision 1.75  2004/06/17 22:02:51  csoutheren
 * Added check to prevent allocationBreakpoint from causing unintentional break every 2^32 memory allocations
 *
 * Revision 1.74  2004/06/01 05:22:43  csoutheren
 * Restored memory check functionality
 *
 * Revision 1.73  2004/04/14 06:58:05  csoutheren
 * Fixed PAtomicInteger and PSmartPointer to use real atomic operations
 *
 * Revision 1.72  2004/04/13 11:47:52  csoutheren
 * Changed PSmartPtr to use PAtomicInteger
 *
 * Revision 1.71  2004/04/03 08:22:21  csoutheren
 * Remove pseudo-RTTI and replaced with real RTTI
 *
 * Revision 1.70  2004/04/03 08:09:16  csoutheren
 * Fixed compile problem on Windows
 *
 * Revision 1.69  2004/04/03 07:41:01  csoutheren
 * Fixed compile problem with ostringstream/ostrstream
 *
 * Revision 1.68  2004/04/03 06:54:28  rjongbloed
 * Many and various changes to support new Visual C++ 2003
 *
 * Revision 1.67  2003/09/17 09:02:14  csoutheren
 * Removed memory leak detection code
 *
 * Revision 1.66  2003/09/17 05:55:10  csoutheren
 * Removed recursive includes
 *
 * Revision 1.65  2003/02/11 08:05:55  robertj
 * Fixed problems in dumping memory leaks and statistics as PMemoryHeap
 *   is flagged deleted too early, thanks Diego Tártara
 *
 * Revision 1.64  2003/02/06 21:03:13  dereks
 * Patch from Klaus Kaempf to fix warning message on compiling with gcc 3.x Thanks!
 *
 * Revision 1.63  2002/11/22 10:14:58  robertj
 * Fixed correct free of memory blocks in exceptional circumstances. Partially
 *   trashed heap or objects etc.
 *
 * Revision 1.62  2002/10/21 12:52:27  rogerh
 * Add throw()s to new and delete. Error reported by FreeBSD 5.0 and GCC 3.2.1
 *
 * Revision 1.61  2002/10/10 04:43:44  robertj
 * VxWorks port, thanks Martijn Roest
 *
 * Revision 1.60  2002/09/06 05:29:42  craigs
 * Reversed order of memory block check on delete to improve performance in
 * Linux debug mode
 *
 * Revision 1.59  2002/09/04 05:23:53  robertj
 * Fixed crashable way of checking pointer is in PWLib heap check.
 *
 * Revision 1.58  2002/08/13 01:28:42  robertj
 * Added allocation number to memory statistics dump to aid in leak finding.
 *
 * Revision 1.57  2002/06/28 05:17:06  yurik
 * ifndef'd ostrstream as it does not yet exist on wince
 *
 * Revision 1.56  2002/06/27 06:10:39  robertj
 * Removed PAssert on allocation breakpoint as can often caus a deadlock.
 *
 * Revision 1.55  2002/06/25 10:33:55  robertj
 * REmoved usage of sstream for GNU v3 as it is completely different to the
 *   old ostrstream classes.
 *
 * Revision 1.54  2002/06/25 02:23:57  robertj
 * Improved assertion system to allow C++ class name to be displayed if
 *   desired, especially relevant to container classes.
 *
 * Revision 1.53  2002/06/13 08:50:11  rogerh
 * GCC 3.1 uses slightly different #includes
 *
 * Revision 1.52  2001/09/18 05:56:03  robertj
 * Fixed numerous problems with thread suspend/resume and signals handling.
 *
 * Revision 1.51  2001/08/16 11:58:22  rogerh
 * Add more Mac OS X changes from John Woods <jfw@jfwhome.funhouse.com>
 *
 * Revision 1.50  2001/06/20 06:05:47  rogerh
 * Updates for Mac OS X from Pai-Hsiang Hsiao <shawn@eecs.harvard.edu>
 *
 * Revision 1.49  2001/05/03 06:27:29  robertj
 * Added return value to PMemoryCheck::SetIgnoreAllocations() so get previous state.
 *
 * Revision 1.48  2001/02/18 23:16:02  robertj
 * Fixed possible NULL pointer reference in memory check validation
 *   function, thanks Peter Ehlin.
 *
 * Revision 1.47  2001/02/13 03:31:02  robertj
 * Added function to do heap validation.
 *
 * Revision 1.46  2001/02/07 04:47:49  robertj
 * Added changes for possible random crashes in multi DLL environment
 *   due to memory allocation wierdness, thanks Milan Dimitrijevic.
 *
 * Revision 1.45  2001/01/25 07:14:39  robertj
 * Fixed spurios memory leak message. Usual static global problem.
 *
 * Revision 1.44  2000/06/26 11:17:20  robertj
 * Nucleus++ port (incomplete).
 *
 * Revision 1.43  2000/01/05 00:29:12  robertj
 * Fixed alignment problems in memory checking debug functions.
 *
 * Revision 1.42  1999/11/01 00:17:20  robertj
 * Added override of new functions for MSVC memory check code.
 *
 * Revision 1.41  1999/08/22 13:38:39  robertj
 * Fixed termination hang up problem with memory check code under unix pthreads.
 *
 * Revision 1.40  1999/08/10 10:45:09  robertj
 * Added mutex in memory check detection code.
 *
 * Revision 1.39  1999/07/18 15:08:48  robertj
 * Fixed 64 bit compatibility
 *
 * Revision 1.38  1999/05/01 11:29:20  robertj
 * Alpha linux port changes.
 *
 * Revision 1.37  1999/03/09 10:30:17  robertj
 * Fixed ability to have PMEMORY_CHECK on/off on both debug/release versions.
 *
 * Revision 1.36  1999/02/22 10:48:14  robertj
 * Fixed delete operator prototypes for MSVC6 and GNU compatibility.
 *
 * Revision 1.35  1998/12/22 10:24:17  robertj
 * Fixed MSVC warnings caused by changes made in linux PPC support.
 *
 * Revision 1.34  1998/12/15 09:01:10  robertj
 * Fixed 8 byte alignment problem in memory leak check code for sparc.
 *
 * Revision 1.33  1998/11/30 05:33:00  robertj
 * Fixed duplicate debug stream class, ther can be only one.
 *
 * Revision 1.32  1998/11/03 03:11:53  robertj
 * Fixed memory leak question so correctly detects leaks and can be ^C'd.
 *
 * Revision 1.31  1998/11/03 00:55:31  robertj
 * Added allocation breakpoint variable.
 *
 * Revision 1.30  1998/10/15 07:48:56  robertj
 * Added hex dump to memory leak.
 * Added ability to ignore G++lib memory leaks.
 *
 * Revision 1.29  1998/10/15 01:53:35  robertj
 * GNU compatibility.
 *
 * Revision 1.28  1998/10/13 14:06:26  robertj
 * Complete rewrite of memory leak detection code.
 *
 * Revision 1.27  1998/09/23 06:22:22  robertj
 * Added open source copyright license.
 *
 * Revision 1.26  1998/05/30 13:27:02  robertj
 * Changed memory check code so global statics are not included in leak check.
 *
 * Revision 1.25  1997/07/08 13:07:07  robertj
 * DLL support.
 *
 * Revision 1.24  1997/02/09 03:45:28  robertj
 * Fixed unix/dos compatibility with include file.
 *
 * Revision 1.23  1997/02/05 11:54:12  robertj
 * Fixed problems with memory check and leak detection.
 *
 * Revision 1.22  1996/08/08 10:08:46  robertj
 * Directory structure changes for common files.
 *
 * Revision 1.21  1996/07/15 10:35:11  robertj
 * Changed memory leak dump to use static class rather than atexit for better portability.
 *
 * Revision 1.20  1996/06/17 11:35:47  robertj
 * Fixed display of memory leak info, needed flush and use of cin as getchar() does not work with services.
 *
 * Revision 1.19  1996/05/09 12:19:29  robertj
 * Fixed up 64 bit integer class for Mac platform.
 * Fixed incorrect use of memcmp/strcmp return value.
 *
 * Revision 1.18  1996/03/26 00:55:20  robertj
 * Added keypress before dumping memory leaks.
 *
 * Revision 1.17  1996/01/28 02:50:27  robertj
 * Added missing bit shift operators to 64 bit integer class.
 * Added assert into all Compare functions to assure comparison between compatible objects.
 *
 * Revision 1.16  1996/01/23 13:15:52  robertj
 * Mac Metrowerks compiler support.
 *
 * Revision 1.15  1996/01/02 12:52:02  robertj
 * Mac OS compatibility changes.
 *
 * Revision 1.14  1995/11/21 11:51:54  robertj
 * Improved streams compatibility.
 *
 * Revision 1.12  1995/04/25 11:30:34  robertj
 * Fixed Borland compiler warnings.
 * Fixed function hiding ancestors virtual.
 *
 * Revision 1.11  1995/03/12 04:59:53  robertj
 * Re-organisation of DOS/WIN16 and WIN32 platforms to maximise common code.
 * Used built-in equate for WIN32 API (_WIN32).
 *
 * Revision 1.10  1995/02/19  04:19:21  robertj
 * Added dynamically linked command processing.
 *
 * Revision 1.9  1995/01/15  04:52:02  robertj
 * Mac compatibility.
 * Added memory stats function.
 *
// Revision 1.8  1995/01/09  12:38:07  robertj
// Changed variable names around during documentation run.
// Fixed smart pointer comparison.
// Fixed serialisation stuff.
//
// Revision 1.7  1995/01/07  04:39:45  robertj
// Redesigned font enumeration code and changed font styles.
//
// Revision 1.6  1995/01/04  10:57:08  robertj
// Changed for HPUX and GNU2.6.x
//
// Revision 1.5  1995/01/03  09:39:10  robertj
// Put standard malloc style memory allocation etc into memory check system.
//
// Revision 1.4  1994/12/21  11:43:29  robertj
// Added extra memory stats.
//
// Revision 1.3  1994/12/13  11:54:54  robertj
// Added some memory usage statistics.
//
// Revision 1.2  1994/12/12  10:08:32  robertj
// Renamed PWrapper to PSmartPointer..
//
// Revision 1.1  1994/10/30  12:02:15  robertj
// Initial revision
//
 */

#ifdef __GNUC__
#pragma implementation "pfactory.h"
#endif // __GNUC__

#include <ptlib.h>
#include <ptlib/pfactory.h>
#include <ctype.h>
#ifdef _WIN32
#include <ptlib/msos/ptlib/debstrm.h>
#if defined(_MSC_VER) && !defined(_WIN32_WCE)
#include <crtdbg.h>
#endif
#elif defined(__NUCLEUS_PLUS__)
#include <ptlib/NucleusDebstrm.h>
#else
#include <signal.h>
#endif

#ifdef P_LINUX
extern PString PX_GetThreadName(pthread_t);
#endif

PFactoryBase::FactoryMap & PFactoryBase::GetFactories()
{
  static FactoryMap factories;
  return factories;
}

PMutex & PFactoryBase::GetFactoriesMutex()
{
  static PMutex mutex;
  return mutex;
}

PFactoryBase::FactoryMap::~FactoryMap()
{
  FactoryMap::iterator entry;
  for (entry = begin(); entry != end(); ++entry){
    delete entry->second;
    entry->second = NULL;
  }  
}

void PAssertFunc(const char * file,
                 int line,
                 const char * className,
                 PStandardAssertMessage msg)
{
  if (msg == POutOfMemory) {
    // Special case, do not use ostrstream in other PAssertFunc if have
    // a memory out situation as that would probably also fail!
    static const char fmt[] = "Out of memory at file %.100s, line %u, class %.30s";
    char msgbuf[sizeof(fmt)+100+10+30];
    sprintf(msgbuf, fmt, file, line, className);
    PAssertFunc(msgbuf);
    return;
  }

  static const char * const textmsg[PMaxStandardAssertMessage] = {
    NULL,
    "Out of memory",
    "Null pointer reference",
    "Invalid cast to non-descendant class",
    "Invalid array index",
    "Invalid array element",
    "Stack empty",
    "Unimplemented function",
    "Invalid parameter",
    "Operating System error",
    "File not open",
    "Unsupported feature",
    "Invalid or closed operating system window"
  };

  const char * theMsg;
  char msgbuf[20];
  if (msg < PMaxStandardAssertMessage)
    theMsg = textmsg[msg];
  else {
    sprintf(msgbuf, "Assertion %i", msg);
    theMsg = msgbuf;
  }
  PAssertFunc(file, line, className, theMsg);
}


void PAssertFunc(const char * file, int line, const char * className, const char * msg)
{
#if defined(_WIN32)
  DWORD err = GetLastError();
#else
  int err = errno;
#endif

  #if (__GNUC__ >= 3) && defined (__USE_STL__)
  ostringstream str;
  #else
  ostrstream str;
  #endif
  str << "Assertion fail: ";
  if (msg != NULL)
    str << msg << ", ";
  str << "file " << file << ", line " << line;
  if (className != NULL)
    str << ", class " << className;
  if (err != 0)
    str << ", Error=" << err;
  str << ends;
  
  #if (__GNUC__ >= 3) && defined (__USE_STL__)
  string sstr = str.str();
  const char * s = sstr.c_str();
  #else
  const char * s = str.str();
  #endif

  PAssertFunc(s);
}

PObject::Comparison PObject::CompareObjectMemoryDirect(const PObject&obj) const
{
  int retval = memcmp(this, &obj, sizeof(PObject));
  if (retval < 0)
    return LessThan;
  if (retval > 0)
    return GreaterThan;
  return EqualTo;
}


PObject * PObject::Clone() const
{
  PAssertAlways(PUnimplementedFunction);
  return NULL;
}


PObject::Comparison PObject::Compare(const PObject & obj) const
{
  return (Comparison)CompareObjectMemoryDirect(obj);
}


void PObject::PrintOn(ostream & strm) const
{
  strm << GetClass();
}


void PObject::ReadFrom(istream &)
{
}


PINDEX PObject::HashFunction() const
{
  return 0;
}


///////////////////////////////////////////////////////////////////////////////
// General reference counting support

PSmartPointer::PSmartPointer(const PSmartPointer & ptr)
{
  object = ptr.object;
  if (object != NULL)
    ++object->referenceCount;
}


PSmartPointer & PSmartPointer::operator=(const PSmartPointer & ptr)
{
  if (object == ptr.object)
    return *this;

  if ((object != NULL) && (--object->referenceCount == 0))
      delete object;

  object = ptr.object;
  if (object != NULL)
    ++object->referenceCount;

  return *this;
}


PSmartPointer::~PSmartPointer()
{
  if ((object != NULL) && (--object->referenceCount == 0))
    delete object;
}


PObject::Comparison PSmartPointer::Compare(const PObject & obj) const
{
  PAssert(PIsDescendant(&obj, PSmartPointer), PInvalidCast);
  PSmartObject * other = ((const PSmartPointer &)obj).object;
  if (object == other)
    return EqualTo;
  return object < other ? LessThan : GreaterThan;
}



//////////////////////////////////////////////////////////////////////////////////////////

#if PMEMORY_CHECK

#undef malloc
#undef realloc
#undef free

#if (__GNUC__ >= 3) || ((__GNUC__ == 2)&&(__GNUC_MINOR__ >= 95)) //2.95.X & 3.X
void * operator new(size_t nSize) throw (std::bad_alloc)
#else
void * operator new(size_t nSize)
#endif
{
  return PMemoryHeap::Allocate(nSize, (const char *)NULL, 0, NULL);
}


#if (__GNUC__ >= 3) || ((__GNUC__ == 2)&&(__GNUC_MINOR__ >= 95)) //2.95.X & 3.X
void * operator new[](size_t nSize) throw (std::bad_alloc)
#else
void * operator new[](size_t nSize)
#endif
{
  return PMemoryHeap::Allocate(nSize, (const char *)NULL, 0, NULL);
}


#if (__GNUC__ >= 3) || ((__GNUC__ == 2)&&(__GNUC_MINOR__ >= 95)) //2.95.X & 3.X
void operator delete(void * ptr) throw()
#else
void operator delete(void * ptr)
#endif
{
  PMemoryHeap::Deallocate(ptr, NULL);
}


#if (__GNUC__ >= 3) || ((__GNUC__ == 2)&&(__GNUC_MINOR__ >= 95)) //2.95.X & 3.X
void operator delete[](void * ptr) throw()
#else
void operator delete[](void * ptr)
#endif
{
  PMemoryHeap::Deallocate(ptr, NULL);
}


DWORD PMemoryHeap::allocationBreakpoint = 0;
char PMemoryHeap::Header::GuardBytes[NumGuardBytes];


PMemoryHeap::Wrapper::Wrapper()
{
  // The following is done like this to get over brain dead compilers that cannot
  // guarentee that a static global is contructed before it is used.
  static PMemoryHeap real_instance;
  instance = &real_instance;
  if (instance->isDestroyed)
    return;

#if defined(_WIN32)
  EnterCriticalSection(&instance->mutex);
#elif defined(P_MAC_MPTHREADS)
  long err;
  PAssertOS((err = MPEnterCriticalRegion(instance->mutex, kDurationForever)) == 0);
#elif defined(P_PTHREADS)
  pthread_mutex_lock(&instance->mutex);
#elif defined(P_VXWORKS)
  semTake((SEM_ID)instance->mutex, WAIT_FOREVER);
#endif
}


PMemoryHeap::Wrapper::~Wrapper()
{
  if (instance->isDestroyed)
    return;

#if defined(_WIN32)
  LeaveCriticalSection(&instance->mutex);
#elif defined(P_MAC_MPTHREADS)
  long err;
  PAssertOS((err = MPExitCriticalRegion(instance->mutex)) == 0 || instance->isDestroyed);
#elif defined(P_PTHREADS)
  pthread_mutex_unlock(&instance->mutex);
#elif defined(P_VXWORKS)
  semGive((SEM_ID)instance->mutex);
#endif
}


PMemoryHeap::PMemoryHeap()
{
  isDestroyed = FALSE;

  listHead = NULL;
  listTail = NULL;

  allocationRequest = 1;
  firstRealObject = 0;
  flags = NoLeakPrint;

  allocFillChar = '\x5A';
  freeFillChar = '\xA5';

  currentMemoryUsage = 0;
  peakMemoryUsage = 0;
  currentObjects = 0;
  peakObjects = 0;
  totalObjects = 0;

  for (PINDEX i = 0; i < Header::NumGuardBytes; i++)
    Header::GuardBytes[i] = (i&1) == 0 ? '\x55' : '\xaa';

#if defined(_WIN32)
  InitializeCriticalSection(&mutex);
  static PDebugStream debug;
  leakDumpStream = &debug;
#else
#if defined(P_PTHREADS)
#ifdef PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP
  pthread_mutex_t recursiveMutex = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP;
  mutex = recursiveMutex;
#else
 pthread_mutex_init(&mutex, NULL);
#endif
#elif defined(P_VXWORKS)
  mutex = semMCreate(SEM_Q_FIFO);
#endif
  leakDumpStream = &cerr;
#endif
}


PMemoryHeap::~PMemoryHeap()
{
  if (leakDumpStream != NULL) {
    InternalDumpStatistics(*leakDumpStream);
    InternalDumpObjectsSince(firstRealObject, *leakDumpStream);
  }

  isDestroyed = TRUE;

#if defined(_WIN32)
  DeleteCriticalSection(&mutex);
  extern void PWaitOnExitConsoleWindow();
  PWaitOnExitConsoleWindow();
#elif defined(P_PTHREADS)
  pthread_mutex_destroy(&mutex);
#elif defined(P_VXWORKS)
  semDelete((SEM_ID)mutex);
#endif
}


void * PMemoryHeap::Allocate(size_t nSize, const char * file, int line, const char * className)
{
  Wrapper mem;
  return mem->InternalAllocate(nSize, file, line, className);
}


void * PMemoryHeap::Allocate(size_t count, size_t size, const char * file, int line)
{
  Wrapper mem;

  char oldFill = mem->allocFillChar;
  mem->allocFillChar = '\0';

  void * data = mem->InternalAllocate(count*size, file, line, NULL);

  mem->allocFillChar = oldFill;

  return data;
}


void * PMemoryHeap::InternalAllocate(size_t nSize, const char * file, int line, const char * className)
{
  if (isDestroyed)
    return malloc(nSize);

  Header * obj = (Header *)malloc(sizeof(Header) + nSize + sizeof(Header::GuardBytes));
  if (obj == NULL) {
    PAssertAlways(POutOfMemory);
    return NULL;
  }

  // Ignore all allocations made before main() is called. This is indicated
  // by PProcess::PreInitialise() clearing the NoLeakPrint flag. Why do we do
  // this? because the GNU compiler is broken in the way it does static global
  // C++ object construction and destruction.
  if (firstRealObject == 0 && (flags&NoLeakPrint) == 0)
    firstRealObject = allocationRequest;

  if (allocationBreakpoint != 0 && allocationRequest == allocationBreakpoint) {
#ifdef _WIN32
    __asm int 3;
#elif defined(P_VXWORKS)
    kill(taskIdSelf(), SIGABRT);
#else
    kill(getpid(), SIGABRT);
#endif 
  }

  currentMemoryUsage += nSize;
  if (currentMemoryUsage > peakMemoryUsage)
    peakMemoryUsage = currentMemoryUsage;

  currentObjects++;
  if (currentObjects > peakObjects)
    peakObjects = currentObjects;
  totalObjects++;

  char * data = (char *)&obj[1];

  obj->prev      = listTail;
  obj->next      = NULL;
  obj->size      = nSize;
  obj->fileName  = file;
  obj->line      = (WORD)line;
#ifdef P_LINUX
   obj->thread    = pthread_self();
#endif
  obj->className = className;
  obj->request   = allocationRequest++;
  obj->flags     = flags;
  memcpy(obj->guard, obj->GuardBytes, sizeof(obj->guard));
  memset(data, allocFillChar, nSize);
  memcpy(&data[nSize], obj->GuardBytes, sizeof(obj->guard));

  if (listTail != NULL)
    listTail->next = obj;

  listTail = obj;

  if (listHead == NULL)
    listHead = obj;

  return data;
}


void * PMemoryHeap::Reallocate(void * ptr, size_t nSize, const char * file, int line)
{
  if (ptr == NULL)
    return Allocate(nSize, file, line, NULL);

  if (nSize == 0) {
    Deallocate(ptr, NULL);
    return NULL;
  }

  Wrapper mem;

  if (mem->isDestroyed)
    return realloc(ptr, nSize);

  if (mem->InternalValidate(ptr, NULL, mem->leakDumpStream) != Ok)
    return NULL;

  Header * obj = (Header *)realloc(((Header *)ptr)-1, sizeof(Header) + nSize + sizeof(obj->guard));
  if (obj == NULL) {
    PAssertAlways(POutOfMemory);
    return NULL;
  }

  if (mem->allocationBreakpoint != 0 && mem->allocationRequest == mem->allocationBreakpoint) {
#ifdef _WIN32
    __asm int 3;
#elif defined(P_VXWORKS)
    kill(taskIdSelf(), SIGABRT);
#else
    kill(getpid(), SIGABRT);
#endif 
  }

  mem->currentMemoryUsage -= obj->size;
  mem->currentMemoryUsage += nSize;
  if (mem->currentMemoryUsage > mem->peakMemoryUsage)
    mem->peakMemoryUsage = mem->currentMemoryUsage;

  char * data = (char *)&obj[1];
  memcpy(&data[nSize], obj->GuardBytes, sizeof(obj->guard));

  obj->size      = nSize;
  obj->fileName  = file;
  obj->line      = (WORD)line;
  obj->request   = mem->allocationRequest++;
  if (obj->prev != NULL)
    obj->prev->next = obj;
  else
    mem->listHead = obj;
  if (obj->next != NULL)
    obj->next->prev = obj;
  else
    mem->listTail = obj;

  return data;
}


void PMemoryHeap::Deallocate(void * ptr, const char * className)
{
  if (ptr == NULL)
    return;

  Wrapper mem;
  Header * obj = ((Header *)ptr)-1;

  if (mem->isDestroyed) {
    free(obj);
    return;
  }

  switch (mem->InternalValidate(ptr, className, mem->leakDumpStream)) {
    case Ok :
      break;
    case Trashed :
      free(ptr);
      return;
    case Bad :
      free(obj);
      return;
  }

  if (obj->prev != NULL)
    obj->prev->next = obj->next;
  else
    mem->listHead = obj->next;
  if (obj->next != NULL)
    obj->next->prev = obj->prev;
  else
    mem->listTail = obj->prev;

  mem->currentMemoryUsage -= obj->size;
  mem->currentObjects--;

  memset(ptr, mem->freeFillChar, obj->size);  // Make use of freed data noticable
  free(obj);
}


PMemoryHeap::Validation PMemoryHeap::Validate(const void * ptr,
                                              const char * className,
                                              ostream * error)
{
  Wrapper mem;
  return mem->InternalValidate(ptr, className, error);
}


PMemoryHeap::Validation PMemoryHeap::InternalValidate(const void * ptr,
                                                      const char * className,
                                                      ostream * error)
{
  if (isDestroyed)
    return Bad;

  if (ptr == NULL)
    return Trashed;

  Header * obj = ((Header *)ptr)-1;

  Header * link = listTail;  
  while (link != NULL && link != obj) 
    link = link->prev;  

  if (link == NULL) {
    if (error != NULL)
      *error << "Block " << ptr << " not in heap!" << endl;
    return Trashed;
  }

  if (memcmp(obj->guard, obj->GuardBytes, sizeof(obj->guard)) != 0) {
    if (error != NULL)
      *error << "Underrun at " << ptr << '[' << obj->size << "] #" << obj->request << endl;
    return Bad;
  }
  
  if (memcmp((char *)ptr+obj->size, obj->GuardBytes, sizeof(obj->guard)) != 0) {
    if (error != NULL)
      *error << "Overrun at " << ptr << '[' << obj->size << "] #" << obj->request << endl;
    return Bad;
  }
  
  if (!(className == NULL && obj->className == NULL) &&
       (className == NULL || obj->className == NULL ||
       (className != obj->className && strcmp(obj->className, className) != 0))) {
    if (error != NULL)
      *error << "PObject " << ptr << '[' << obj->size << "] #" << obj->request
             << " allocated as \"" << (obj->className != NULL ? obj->className : "<NULL>")
             << "\" and should be \"" << (className != NULL ? className : "<NULL>")
             << "\"." << endl;
    return Bad;
  }

  return Ok;
}


BOOL PMemoryHeap::ValidateHeap(ostream * error)
{
  Wrapper mem;

  if (error == NULL)
    error = mem->leakDumpStream;

  Header * obj = mem->listHead;
  while (obj != NULL) {
    if (memcmp(obj->guard, obj->GuardBytes, sizeof(obj->guard)) != 0) {
      if (error != NULL)
        *error << "Underrun at " << (obj+1) << '[' << obj->size << "] #" << obj->request << endl;
      return FALSE;
    }
  
    if (memcmp((char *)(obj+1)+obj->size, obj->GuardBytes, sizeof(obj->guard)) != 0) {
      if (error != NULL)
        *error << "Overrun at " << (obj+1) << '[' << obj->size << "] #" << obj->request << endl;
      return FALSE;
    }

    obj = obj->next;
  }

#if defined(_WIN32) && defined(_DEBUG)
  if (!_CrtCheckMemory()) {
    if (error != NULL)
      *error << "Heap failed MSVCRT validation!" << endl;
    return FALSE;
  }
#endif
  if (error != NULL)
    *error << "Heap passed validation." << endl;
  return TRUE;
}


BOOL PMemoryHeap::SetIgnoreAllocations(BOOL ignore)
{
  Wrapper mem;

  BOOL ignoreAllocations = (mem->flags&NoLeakPrint) != 0;

  if (ignore)
    mem->flags |= NoLeakPrint;
  else
    mem->flags &= ~NoLeakPrint;

  return ignoreAllocations;
}


void PMemoryHeap::DumpStatistics()
{
  Wrapper mem;
  if (mem->leakDumpStream != NULL)
    mem->InternalDumpStatistics(*mem->leakDumpStream);
}


void PMemoryHeap::DumpStatistics(ostream & strm)
{
  Wrapper mem;
  mem->InternalDumpStatistics(strm);
}


void PMemoryHeap::InternalDumpStatistics(ostream & strm)
{
  strm << "\nCurrent memory usage: " << currentMemoryUsage << " bytes";
  if (currentMemoryUsage > 2048)
    strm << ", " << (currentMemoryUsage+1023)/1024 << "kb";
  if (currentMemoryUsage > 2097152)
    strm << ", " << (currentMemoryUsage+1048575)/1048576 << "Mb";

  strm << ".\nCurrent objects count: " << currentObjects
       << "\nPeak memory usage: " << peakMemoryUsage << " bytes";
  if (peakMemoryUsage > 2048)
    strm << ", " << (peakMemoryUsage+1023)/1024 << "kb";
  if (peakMemoryUsage > 2097152)
    strm << ", " << (peakMemoryUsage+1048575)/1048576 << "Mb";

  strm << ".\nPeak objects created: " << peakObjects
       << "\nTotal objects created: " << totalObjects
       << "\nNext allocation request: " << allocationRequest
       << '\n' << endl;
}


void PMemoryHeap::GetState(State & state)
{
  Wrapper mem;
  state.allocationNumber = mem->allocationRequest;
}


void PMemoryHeap::SetAllocationBreakpoint(DWORD point)
{
  allocationBreakpoint = point;
}


void PMemoryHeap::DumpObjectsSince(const State & state)
{
  Wrapper mem;
  if (mem->leakDumpStream != NULL)
    mem->InternalDumpObjectsSince(state.allocationNumber, *mem->leakDumpStream);
}


void PMemoryHeap::DumpObjectsSince(const State & state, ostream & strm)
{
  Wrapper mem;
  mem->InternalDumpObjectsSince(state.allocationNumber, strm);
}


void PMemoryHeap::InternalDumpObjectsSince(DWORD objectNumber, ostream & strm)
{
  BOOL first = TRUE;
  for (Header * obj = listHead; obj != NULL; obj = obj->next) {
    if (obj->request < objectNumber || (obj->flags&NoLeakPrint) != 0)
      continue;

    if (first && isDestroyed) {
      *leakDumpStream << "\nMemory leaks detected, press Enter to display . . ." << flush;
#if !defined(_WIN32)
      cin.get();
#endif
      first = FALSE;
    }

    BYTE * data = (BYTE *)&obj[1];

    if (obj->fileName != NULL)
      strm << obj->fileName << '(' << obj->line << ") : ";

    strm << '#' << obj->request << ' ' << (void *)data << " [" << obj->size << "] ";

#ifdef P_LINUX
    strm << '"' << PX_GetThreadName(obj->thread) << "\" ";
#endif

    if (obj->className != NULL)
      strm << '"' << obj->className << "\" ";

    strm << '\n' << hex << setfill('0') << PBYTEArray(data, PMIN(16, obj->size), FALSE)
                 << dec << setfill(' ') << endl;
  }
}


#else // PMEMORY_CHECK

#if defined(_MSC_VER) && defined(_DEBUG) && !defined(_WIN32_WCE)

static _CRT_DUMP_CLIENT pfnOldCrtDumpClient;
static bool hadCrtDumpLeak = false;

static void __cdecl MyCrtDumpClient(void * ptr, size_t size)
{
  if(_CrtReportBlockType(ptr) == P_CLIENT_BLOCK) {
    const PObject * obj = (PObject *)ptr;
    _RPT1(_CRT_WARN, "Class %s\n", obj->GetClass());
    hadCrtDumpLeak = true;
  }

  if (pfnOldCrtDumpClient != NULL)
    pfnOldCrtDumpClient(ptr, size);
}


PMemoryHeap::PMemoryHeap()
{
  _CrtMemCheckpoint(&initialState);
  pfnOldCrtDumpClient = _CrtSetDumpClient(MyCrtDumpClient);
  _CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) & ~_CRTDBG_ALLOC_MEM_DF);
}


PMemoryHeap::~PMemoryHeap()
{
  _CrtMemDumpAllObjectsSince(&initialState);

  if (hadCrtDumpLeak) {
    extern void PWaitOnExitConsoleWindow();
    PWaitOnExitConsoleWindow();
  }

  _CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) & ~_CRTDBG_LEAK_CHECK_DF);
}


static PMemoryHeap & MemoryHeapInstance()
{
  static PMemoryHeap instance;
  return instance;
}


void * PMemoryHeap::Allocate(size_t nSize, const char * file, int line, const char * className)
{
  MemoryHeapInstance();
  return _malloc_dbg(nSize, className != NULL ? P_CLIENT_BLOCK : _NORMAL_BLOCK, file, line);
}


void * PMemoryHeap::Allocate(size_t count, size_t iSize, const char * file, int line)
{
  MemoryHeapInstance();
  return _calloc_dbg(count, iSize, _NORMAL_BLOCK, file, line);
}


void * PMemoryHeap::Reallocate(void * ptr, size_t nSize, const char * file, int line)
{
  MemoryHeapInstance();
  return _realloc_dbg(ptr, nSize, _NORMAL_BLOCK, file, line);
}


void PMemoryHeap::Deallocate(void * ptr, const char * className)
{
  _free_dbg(ptr, className != NULL ? P_CLIENT_BLOCK : _NORMAL_BLOCK);
}


PMemoryHeap::Validation PMemoryHeap::Validate(const void * ptr, const char * className, ostream * /*strm*/)
{
  if (!_CrtIsValidHeapPointer(ptr))
    return Bad;

  if (_CrtReportBlockType(ptr) != P_CLIENT_BLOCK)
    return Ok;

  const PObject * obj = (PObject *)ptr;
  return strcmp(obj->GetClass(), className) == 0 ? Ok : Trashed;
}


BOOL PMemoryHeap::ValidateHeap(ostream * /*strm*/)
{
  return _CrtCheckMemory();
}


BOOL PMemoryHeap::SetIgnoreAllocations(BOOL ignore)
{
  int flags = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
  if (ignore)
    _CrtSetDbgFlag(flags & ~_CRTDBG_ALLOC_MEM_DF);
  else
    _CrtSetDbgFlag(flags | _CRTDBG_ALLOC_MEM_DF);
  return (flags & _CRTDBG_ALLOC_MEM_DF) == 0;
}


void PMemoryHeap::DumpStatistics()
{
  _CrtMemState state;
  _CrtMemCheckpoint(&state);
  _CrtMemDumpStatistics(&state);
}


void PMemoryHeap::DumpStatistics(ostream & /*strm*/)
{
  DumpStatistics();
}


void PMemoryHeap::GetState(State & state)
{
  _CrtMemCheckpoint(&state);
}


void PMemoryHeap::DumpObjectsSince(const State & state)
{
  _CrtMemDumpAllObjectsSince(&state);
}


void PMemoryHeap::DumpObjectsSince(const State & state, ostream & /*strm*/)
{
  DumpObjectsSince(state);
}


void PMemoryHeap::SetAllocationBreakpoint(DWORD objectNumber)
{
  _CrtSetBreakAlloc(objectNumber);
}


#else // defined(_MSC_VER) && defined(_DEBUG)

#if !defined(P_VXWORKS) && !defined(_WIN32_WCE)

#if (__GNUC__ >= 3) || ((__GNUC__ == 2)&&(__GNUC_MINOR__ >= 95)) //2.95.X & 3.X
void * operator new[](size_t nSize) throw (std::bad_alloc)
#else
void * operator new[](size_t nSize)
#endif
{
  return malloc(nSize);
}

#if (__GNUC__ >= 3) || ((__GNUC__ == 2)&&(__GNUC_MINOR__ >= 95)) //2.95.X & 3.X
void operator delete[](void * ptr) throw ()
#else
void operator delete[](void * ptr)
#endif
{
  free(ptr);
}

#endif // !P_VXWORKS

#endif // defined(_MSC_VER) && defined(_DEBUG)

#endif // PMEMORY_CHECK




///////////////////////////////////////////////////////////////////////////////
// Large integer support

#ifdef P_NEEDS_INT64

void PInt64__::Add(const PInt64__ & v)
{
  unsigned long old = low;
  high += v.high;
  low += v.low;
  if (low < old)
    high++;
}


void PInt64__::Sub(const PInt64__ & v)
{
  unsigned long old = low;
  high -= v.high;
  low -= v.low;
  if (low > old)
    high--;
}


void PInt64__::Mul(const PInt64__ & v)
{
  DWORD p1 = (low&0xffff)*(v.low&0xffff);
  DWORD p2 = (low >> 16)*(v.low >> 16);
  DWORD p3 = (high&0xffff)*(v.high&0xffff);
  DWORD p4 = (high >> 16)*(v.high >> 16);
  low = p1 + (p2 << 16);
  high = (p2 >> 16) + p3 + (p4 << 16);
}


void PInt64__::Div(const PInt64__ & v)
{
  long double dividend = high;
  dividend *=  4294967296.0;
  dividend += low;
  long double divisor = high;
  divisor *=  4294967296.0;
  divisor += low;
  long double quotient = dividend/divisor;
  low = quotient;
  high = quotient/4294967296.0;
}


void PInt64__::Mod(const PInt64__ & v)
{
  PInt64__ t = *this;
  t.Div(v);
  t.Mul(t);
  Sub(t);
}


void PInt64__::ShiftLeft(int bits)
{
  if (bits >= 32) {
    high = low << (bits - 32);
    low = 0;
  }
  else {
    high <<= bits;
    high |= low >> (32 - bits);
    low <<= bits;
  }
}


void PInt64__::ShiftRight(int bits)
{
  if (bits >= 32) {
    low = high >> (bits - 32);
    high = 0;
  }
  else {
    low >>= bits;
    low |= high << (32 - bits);
    high >>= bits;
  }
}


BOOL PInt64::Lt(const PInt64 & v) const
{
  if ((long)high < (long)v.high)
    return TRUE;
  if ((long)high > (long)v.high)
    return FALSE;
  if ((long)high < 0)
    return (long)low > (long)v.low;
  return (long)low < (long)v.low;
}


BOOL PInt64::Gt(const PInt64 & v) const
{
  if ((long)high > (long)v.high)
    return TRUE;
  if ((long)high < (long)v.high)
    return FALSE;
  if ((long)high < 0)
    return (long)low < (long)v.low;
  return (long)low > (long)v.low;
}


BOOL PUInt64::Lt(const PUInt64 & v) const
{
  if (high < v.high)
    return TRUE;
  if (high > v.high)
    return FALSE;
  return low < high;
}


BOOL PUInt64::Gt(const PUInt64 & v) const
{
  if (high > v.high)
    return TRUE;
  if (high < v.high)
    return FALSE;
  return low > high;
}


static void Out64(ostream & stream, PUInt64 num)
{
  char buf[25];
  char * p = &buf[sizeof(buf)];
  *--p = '\0';

  switch (stream.flags()&ios::basefield) {
    case ios::oct :
      while (num != 0) {
        *--p = (num&7) + '0';
        num >>= 3;
      }
      break;

    case ios::hex :
      while (num != 0) {
        *--p = (num&15) + '0';
        if (*p > '9')
          *p += 7;
        num >>= 4;
      }
      break;

    default :
      while (num != 0) {
        *--p = num%10 + '0';
        num /= 10;
      }
  }

  if (*p == '\0')
    *--p = '0';

  stream << p;
}


ostream & operator<<(ostream & stream, const PInt64 & v)
{
  if (v >= 0)
    Out64(stream, v);
  else {
    int w = stream.width();
    stream.put('-');
    if (w > 0)
      stream.width(w-1);
    Out64(stream, -v);
  }

  return stream;
}


ostream & operator<<(ostream & stream, const PUInt64 & v)
{
  Out64(stream, v);
  return stream;
}


static PUInt64 Inp64(istream & stream)
{
  int base;
  switch (stream.flags()&ios::basefield) {
    case ios::oct :
      base = 8;
      break;
    case ios::hex :
      base = 16;
      break;
    default :
      base = 10;
  }

  if (isspace(stream.peek()))
    stream.get();

  PInt64 num = 0;
  while (isxdigit(stream.peek())) {
    int c = stream.get() - '0';
    if (c > 9)
      c -= 7;
    if (c > 9)
      c -= 32;
    num = num*base + c;
  }

  return num;
}


istream & operator>>(istream & stream, PInt64 & v)
{
  if (isspace(stream.peek()))
    stream.get();

  switch (stream.peek()) {
    case '-' :
      stream.ignore();
      v = -(PInt64)Inp64(stream);
      break;
    case '+' :
      stream.ignore();
    default :
      v = (PInt64)Inp64(stream);
  }

  return stream;
}


istream & operator>>(istream & stream, PUInt64 & v)
{
  v = Inp64(stream);
  return stream;
}


#endif


#ifdef P_TORNADO

// the library provided with Tornado 2.0 does not contain implementation 
// for the functions defined below, therefor the own implementation

ostream & ostream::operator<<(PInt64 v)
{
  return *this << (long)(v >> 32) << (long)(v & 0xFFFFFFFF);
}


ostream & ostream::operator<<(PUInt64 v)
{
  return *this << (long)(v >> 32) << (long)(v & 0xFFFFFFFF);
}

istream & istream::operator>>(PInt64 & v)
{
  return *this >> (long)(v >> 32) >> (long)(v & 0xFFFFFFFF);
}


istream & istream::operator>>(PUInt64 & v)
{
  return *this >> (long)(v >> 32) >> (long)(v & 0xFFFFFFFF);
}

#endif // P_TORNADO


// End Of File ///////////////////////////////////////////////////////////////
