/*
 * conchan.h
 *
 * Console I/O channel class.
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
 * $Log: conchan.h,v $
 * Revision 1.5  2003/09/17 05:41:58  csoutheren
 * Removed recursive includes
 *
 * Revision 1.4  2003/09/17 01:18:02  csoutheren
 * Removed recursive include file system and removed all references
 * to deprecated coooperative threading support
 *
 * Revision 1.3  2002/09/16 01:08:59  robertj
 * Added #define so can select if #pragma interface/implementation is used on
 *   platform basis (eg MacOS) rather than compiler, thanks Robert Monaghan.
 *
 * Revision 1.2  2001/05/22 12:49:32  robertj
 * Did some seriously wierd rewrite of platform headers to eliminate the
 *   stupid GNU compiler warning about braces not matching.
 *
 * Revision 1.1  1999/06/13 13:54:07  robertj
 * Added PConsoleChannel class for access to stdin/stdout/stderr.
 *
 */

#ifndef _PCONSOLECHANNEL
#define _PCONSOLECHANNEL

#ifdef P_USE_PRAGMA
#pragma interface
#endif

///////////////////////////////////////////////////////////////////////////////
// Console Channel

/**This class defines an I/O channel that communicates via a console.
 */
class PConsoleChannel : public PChannel
{
  PCLASSINFO(PConsoleChannel, PChannel);

  public:
    enum ConsoleType {
      StandardInput,
      StandardOutput,
      StandardError
    };

  /**@name Construction */
  //@{
    /// Create a new console channel object, leaving it unopen.
    PConsoleChannel();

    /// Create a new console channel object, connecting to the I/O stream.
    PConsoleChannel(
      ConsoleType type  /// Type of console for object
    );
  //@}


  /**@name Open functions */
  //@{
    /**Open a serial channal.
       The channel is opened it on the specified port and with the specified
       attributes.
     */
    virtual BOOL Open(
      ConsoleType type  /// Type of console for object
    );
  //@}


// Include platform dependent part of class
#ifdef _WIN32
#include "msos/ptlib/conchan.h"
#else
#include "unix/ptlib/conchan.h"
#endif

};

#endif

// End Of File ///////////////////////////////////////////////////////////////
