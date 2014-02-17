/*
 * syncpoint.h
 *
 * Single thread synchronisation point (event) class.
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
 * $Log: syncpoint.h,v $
 * Revision 1.10  2003/09/17 05:41:59  csoutheren
 * Removed recursive includes
 *
 * Revision 1.9  2003/09/17 01:18:02  csoutheren
 * Removed recursive include file system and removed all references
 * to deprecated coooperative threading support
 *
 * Revision 1.8  2002/09/16 01:08:59  robertj
 * Added #define so can select if #pragma interface/implementation is used on
 *   platform basis (eg MacOS) rather than compiler, thanks Robert Monaghan.
 *
 * Revision 1.7  2002/01/23 04:26:36  craigs
 * Added copy constructors for PSemaphore, PMutex and PSyncPoint to allow
 * use of default copy constructors for objects containing instances of
 * these classes
 *
 * Revision 1.6  2001/05/22 12:49:32  robertj
 * Did some seriously wierd rewrite of platform headers to eliminate the
 *   stupid GNU compiler warning about braces not matching.
 *
 * Revision 1.5  1999/03/09 02:59:51  robertj
 * Changed comments to doc++ compatible documentation.
 *
 * Revision 1.4  1999/02/16 08:11:17  robertj
 * MSVC 6.0 compatibility changes.
 *
 * Revision 1.3  1998/11/30 02:52:00  robertj
 * New directory structure
 *
 * Revision 1.2  1998/09/23 06:21:34  robertj
 * Added open source copyright license.
 *
 * Revision 1.1  1998/03/23 02:41:34  robertj
 * Initial revision
 *
 */

#ifndef _PSYNCPOINT
#define _PSYNCPOINT

#ifdef P_USE_PRAGMA
#pragma interface
#endif

#include <ptlib/semaphor.h>


/** This class defines a thread synchonisation object.
  This form of semaphore is used to indicate an {\it event} has occurred. A
  thread may block on theis sync point and wait until another thread signals
  that it may continue. eg:
\begin{verbatim}
    ... thread one
    while (condition) {
      sync.Wait();
      do_something();
    }

    ... thread 2
    do_something_else();
    sync.Signal();    // At this point thread 1 wake up and does something.
    do_yet_more();

\end{verbatim}
 */
class PSyncPoint : public PSemaphore
{
  PCLASSINFO(PSyncPoint, PSemaphore);

  public:
    /** Create a new sync point.
     */
    PSyncPoint();
    PSyncPoint(const PSyncPoint &);


// Include platform dependent part of class
#ifdef _WIN32
#include "msos/ptlib/syncpoint.h"
#else
#include "unix/ptlib/syncpoint.h"
#endif
};

#endif


// End Of File ///////////////////////////////////////////////////////////////
