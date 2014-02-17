/*
 * random.h
 *
 * ISAAC random number generator by Bob Jenkins.
 *
 * Portable Windows Library
 *
 * Copyright (c) 1993-2000 Equivalence Pty. Ltd.
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
 * Contributor(s): ______________________________________.
 *
 * $Log: random.h,v $
 * Revision 1.8  2007/04/18 23:49:50  csoutheren
 * Add usage of precompiled headers
 *
 * Revision 1.7  2007/03/08 04:31:08  csoutheren
 * Applied 1613299 - add new function to the PRandom class
 * Thanks to Frederic Heem
 *
 * Revision 1.6  2005/11/30 12:47:37  csoutheren
 * Removed tabs, reformatted some code, and changed tags for Doxygen
 *
 * Revision 1.5  2004/11/11 07:34:50  csoutheren
 * Added #include <ptlib.h>
 *
 * Revision 1.4  2002/09/16 01:08:59  robertj
 * Added #define so can select if #pragma interface/implementation is used on
 *   platform basis (eg MacOS) rather than compiler, thanks Robert Monaghan.
 *
 * Revision 1.3  2001/03/03 05:12:47  robertj
 * Fixed yet another transcription error of random number generator code.
 *
 * Revision 1.2  2001/02/27 03:33:44  robertj
 * Changed random number generator due to licensing issues.
 *
 * Revision 1.1  2000/02/17 12:05:02  robertj
 * Added better random number generator after finding major flaws in MSVCRT version.
 *
 */

#ifndef _PRANDOM
#define _PRANDOM


#ifdef P_USE_PRAGMA
#pragma interface
#endif

#ifndef _PTLIB_H
#include <ptlib.h>
#endif

/**Mersenne Twister random number generator.
   An application would create a static instance of this class, and then use
   if to generate a sequence of psuedo-random numbers.

   Usually an application would simply use PRandom::Number() but if
   performance is an issue then it could also create a static local variable
   such as:
        {
          static PRandom rand;
          for (i = 0; i < 10000; i++)
             array[i] = rand;
        }

    This method is not thread safe, so it is the applications responsibility
    to assure that its calls are single threaded.
  */
class PRandom
{
  public:
    /**Construct the random number generator.
       This version will seed the random number generator with a value based
       on the system time as returned by time() and clock().
      */
    PRandom();

    /**Construct the random number generator.
       This version allows the application to choose the seed, thus letting it
       get the same sequence of values on each run. Useful for debugging.
      */
    PRandom(
      DWORD seed    ///< New seed value, must not be zero
    );

    /**Set the seed for the random number generator.
      */
    void SetSeed(
      DWORD seed    ///< New seed value, must not be zero
    );

    /**Get the next psuedo-random number in sequence.
       This generates one pseudorandom unsigned integer (32bit) which is
       uniformly distributed among 0 to 2^32-1 for each call.
      */
    unsigned Generate();

    /**Get the next psuedo-random number in sequence.
      */
    inline operator unsigned() { return Generate(); }


    /**Get the next psuedo-random number in sequence.
       This utilises a single system wide thread safe PRandom variable. All
       threads etc will share the same psuedo-random sequence.
      */
    static unsigned Number();

    /** Get a random number between min and max
    */
    static unsigned int Number(unsigned int min, unsigned int max);

  protected:
    enum {
      RandBits = 8, ///< I recommend 8 for crypto, 4 for simulations
      RandSize = 1<<RandBits
    };

    DWORD randcnt;
    DWORD randrsl[RandSize];
    DWORD randmem[RandSize];
    DWORD randa;
    DWORD randb;
    DWORD randc;
};


#endif  // _PRANDOM


// End Of File ///////////////////////////////////////////////////////////////
