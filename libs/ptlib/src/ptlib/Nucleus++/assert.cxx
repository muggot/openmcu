/*
 * assert.cxx
 *
 * Assert function implementation.
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
 * $Log: assert.cxx,v $
 * Revision 1.2  2002/06/25 02:35:36  robertj
 * Improved assertion system to allow C++ class name to be displayed if
 *   desired, especially relevant to container classes.
 *
 * Revision 1.1  2000/06/26 11:17:20  robertj
 * Nucleus++ port (incomplete).
 *
 * Revision 1.4  1999/06/23 14:19:46  robertj
 * Fixed core dump problem with SIGINT/SIGTERM terminating process.
 *
 * Revision 1.3  1998/09/24 04:12:08  robertj
 * Added open software license.
 *
 * Revision 1.2  1998/06/17 14:47:47  robertj
 * Fixed continuous display of assert if input is from /dev/null
 *
 * Revision 1.1  1996/01/26 11:09:15  craigs
 * Initial revision
 *
 */

#include <ptlib.h>

#include <ctype.h>
#include <signal.h>

void PAssertFunc (const char * msg)

{
  PError << msg << endl;
  BREAKPOINT;
  return;
}

// End Of File ///////////////////////////////////////////////////////////////
