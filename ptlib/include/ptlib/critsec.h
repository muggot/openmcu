/*
 * critsec.h
 *
 * Critical section mutex class.
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
 * $Log: critsec.h,v $
 * Revision 1.20  2007/09/05 11:09:09  csoutheren
 * Removed misleading and incorrect code from Linux implementation of
 * PCriticalSection. Apologies to Hannes Friederich :(
 *
 * Revision 1.19  2006/09/22 00:32:21  csoutheren
 * Forced PAtomicInteger::operator= to be private in all compile paths
 *
 * Revision 1.18  2006/08/07 06:41:16  csoutheren
 * Add PCriticalSection::Clone
 *
 * Revision 1.17  2006/03/20 00:24:56  csoutheren
 * Applied patch #1446482
 * Thanks to Adam Butcher
 *
 * Revision 1.16  2006/01/18 07:17:59  csoutheren
 * Added explicit copy constructor for PCriticalSection on Windows
 *
 * Revision 1.15  2005/11/30 12:47:37  csoutheren
 * Removed tabs, reformatted some code, and changed tags for Doxygen
 *
 * Revision 1.14  2005/11/25 03:43:47  csoutheren
 * Fixed function argument comments to be compatible with Doxygen
 *
 * Revision 1.13  2005/11/14 22:29:13  csoutheren
 * Reverted Wait and Signal to non-const - there is no way we can guarantee that all
 * descendant classes everywhere will be changed over, so we have to keep the
 * original  API
 *
 * Revision 1.12  2005/11/08 10:44:37  dsandras
 * Fixed deadlock with code using the old API.
 *
 * Revision 1.11  2005/11/04 07:20:30  csoutheren
 * Provide backwards compatibility functions and typedefs
 *
 * Revision 1.10  2005/11/04 06:34:20  csoutheren
 * Added new class PSync as abstract base class for all mutex/sempahore classes
 * Changed PCriticalSection to use Wait/Signal rather than Enter/Leave
 * Changed Wait/Signal to be const member functions
 * Renamed PMutex to PTimedMutex and made PMutex synonym for PCriticalSection.
 * This allows use of very efficient mutex primitives in 99% of cases where timed waits
 * are not needed
 *
 * Revision 1.9  2004/05/16 23:31:07  csoutheren
 * Updated API documentation
 *
 * Revision 1.8  2004/05/12 04:36:13  csoutheren
 * Fixed problems with using sem_wait and friends on systems that do not
 * support atomic integers
 *
 * Revision 1.7  2004/04/21 11:22:56  csoutheren
 * Modified to work with gcc 3.4.0
 *
 * Revision 1.6  2004/04/14 06:58:00  csoutheren
 * Fixed PAtomicInteger and PSmartPointer to use real atomic operations
 *
 * Revision 1.5  2004/04/12 03:35:26  csoutheren
 * Fixed problems with non-recursuve mutexes and critical sections on
 * older compilers and libc
 *
 * Revision 1.4  2004/04/12 00:58:45  csoutheren
 * Fixed PAtomicInteger on Linux, and modified PMutex to use it
 *
 * Revision 1.3  2004/04/12 00:36:04  csoutheren
 * Added new class PAtomicInteger and added Windows implementation
 *
 * Revision 1.2  2004/04/11 03:20:41  csoutheren
 * Added Unix implementation of PCriticalSection
 *
 * Revision 1.1  2004/04/11 02:55:17  csoutheren
 * Added PCriticalSection for Windows
 * Added compile time option for PContainer to use critical sections to provide thread safety under some circumstances
 *
 */

#ifndef _PCRITICALSECTION
#define _PCRITICALSECTION

#include <ptlib/psync.h>

#if P_HAS_ATOMIC_INT
#if P_NEEDS_GNU_CXX_NAMESPACE
#define EXCHANGE_AND_ADD(v,i)   __gnu_cxx::__exchange_and_add(v,i)
#else
#define EXCHANGE_AND_ADD(v,i)   __exchange_and_add(v,i)
#endif
#endif

/** This class implements critical section mutexes using the most
  * efficient mechanism available on the host platform.
  * For Windows, CriticalSection is used.
  * On other platforms, pthread_mutex_t is used
  */

#ifdef _WIN32

class PCriticalSection : public PSync
{
  PCLASSINFO(PCriticalSection, PSync);

  public:
  /**@name Construction */
  //@{
    /**Create a new critical section object .
     */
    PCriticalSection();
    PCriticalSection(const PCriticalSection &);

    /**Destroy the critical section object
     */
    ~PCriticalSection();
  //@}

  /**@name Operations */
  //@{
    /** Enter the critical section by waiting for exclusive access.
     */
    void Wait();
    inline void Enter()
    { Wait(); }

    /** Leave the critical section by unlocking the mutex
     */
    void Signal();
    inline void Leave()
    { Signal(); }

    /** Create a new PCriticalSection
      */
    PObject * Clone() const
    { return new PCriticalSection(); }

  //@}

  private:
    PCriticalSection & operator=(const PCriticalSection &) { return *this; }

#include "msos/ptlib/critsec.h"

};

#endif

typedef PWaitAndSignal PEnterAndLeave;

/** This class implements an integer that can be atomically 
  * incremented and decremented in a thread-safe manner.
  * On Windows, the integer is of type long and this class is implemented using InterlockedIncrement
  * and InterlockedDecrement integer is of type long.
  * On Unix systems with GNU std++ support for EXCHANGE_AND_ADD, the integer is of type _Atomic_word (normally int)
  * On all other systems, this class is implemented using PCriticalSection and the integer is of type int.
  */

class PAtomicInteger 
{
#if defined(_WIN32) || defined(DOC_PLUS_PLUS)
    public:
      /** Create a PAtomicInteger with the specified initial value
        */
      inline PAtomicInteger(
        long v = 0                     ///< initial value
      )
        : value(v) { }

      /**
        * Test if an atomic integer has a zero value. Note that this
        * is a non-atomic test - use the return value of the operator++() or
        * operator--() tests to perform atomic operations
        *
        * @return TRUE if the integer has a value of zero
        */
      BOOL IsZero() const                 { return value == 0; }

      /**
        * atomically increment the integer value
        *
        * @return Returns the value of the integer after the increment
        */
      inline long operator++()            { return InterlockedIncrement(&value); }

      /**
        * atomically decrement the integer value
        *
        * @return Returns the value of the integer after the decrement
        */
      inline long operator--()            { return InterlockedDecrement(&value); }

      /**
        * @return Returns the value of the integer
        */
      inline operator long () const       { return value; }

      /**
        * Set the value of the integer
        */
      inline void SetValue(
        long v                          ///< value to set
      )
      { value = v; }
    protected:
      long value;
#elif defined(_STLP_INTERNAL_THREADS_H) && defined(_STLP_ATOMIC_EXCHANGE)
    public:
      inline PAtomicInteger(__stl_atomic_t v = 0)
        : value(v) { }
      BOOL IsZero() const                { return value == 0; }
      inline int operator++()            { return _STLP_ATOMIC_INCREMENT(&value); }
      inline int unsigned operator--()   { return _STLP_ATOMIC_DECREMENT(&value); }
      inline operator int () const       { return value; }
      inline void SetValue(int v)        { value = v; }
    protected:
      __stl_atomic_t value;
#elif !defined(_STLP_INTERNAL_THREADS_H) && P_HAS_ATOMIC_INT
    public:
      inline PAtomicInteger(int v = 0)
        : value(v) { }
      BOOL IsZero() const                { return value == 0; }
      inline int operator++()            { return EXCHANGE_AND_ADD(&value, 1) + 1; }
      inline int unsigned operator--()   { return EXCHANGE_AND_ADD(&value, -1) - 1; }
      inline operator int () const       { return value; }
      inline void SetValue(int v)        { value = v; }
    protected:
      _Atomic_word value;
#else 
    protected:
      PCriticalSection critSec;
    public:
      inline PAtomicInteger(int v = 0)
        : value(v) { }
      BOOL IsZero() const                { return value == 0; }
      inline int operator++()            { PWaitAndSignal m(critSec); value++; return value;}
      inline int operator--()            { PWaitAndSignal m(critSec); value--; return value;}
      inline operator int () const       { return value; }
      inline void SetValue(int v)        { value = v; }
    protected:
      int value;
#endif
    private:
      PAtomicInteger & operator=(const PAtomicInteger & ref) { value = (int)ref; return *this; }
};

#endif

// End Of File ///////////////////////////////////////////////////////////////
