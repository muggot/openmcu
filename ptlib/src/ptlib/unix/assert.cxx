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
 * Revision 1.22  2007/06/02 12:50:05  dsandras
 * Removed redundant header.
 *
 * Revision 1.21  2007/02/12 23:46:34  csoutheren
 * Applied 1611486 - The environment variable PWLIB_ASSERT_ACTION
 * Thanks to Vyacheslav Frolov
 *
 * Revision 1.20  2007/02/01 05:04:20  csoutheren
 * Allow compilation without C++ exceptions enabled
 *
 * Revision 1.19  2006/12/08 05:02:24  csoutheren
 * Apply 1602184 - assert can also throw exception
 * Thanks to Frederic Heem
 *
 * Revision 1.18  2006/06/21 03:28:44  csoutheren
 * Various cleanups thanks for Frederic Heem
 *
 * Revision 1.17  2004/07/11 07:56:36  csoutheren
 * Applied jumbo VxWorks patch, thanks to Eize Slange
 *
 * Revision 1.16  2002/06/25 04:05:19  robertj
 * Fixed new assert function that does not have file/line parameters.
 *
 * Revision 1.15  2002/06/25 02:26:05  robertj
 * Improved assertion system to allow C++ class name to be displayed if
 *   desired, especially relevant to container classes.
 *
 * Revision 1.14  2001/09/03 08:13:54  robertj
 * Changed "stack dump" option to "run debugger" when get assert.
 *
 * Revision 1.13  2001/08/30 08:58:09  robertj
 * Added explicit output to trace file if get assert.
 *
 * Revision 1.12  2001/07/03 04:41:25  yurik
 * Corrections to Jac's submission from 6/28
 *
 * Revision 1.11  2001/06/30 06:59:07  yurik
 * Jac Goudsmit from Be submit these changes 6/28. Implemented by Yuri Kiryanov
 *
 * Revision 1.10  2001/05/03 01:14:09  robertj
 * Put in check to ignore assert if stdin not TTY or not open.
 * Changed default action on assert to ignore if get EOF.
 *
 * Revision 1.9  2001/04/20 10:13:02  robertj
 * Made sure cannot have nested asserts.
 *
 * Revision 1.8  2000/03/27 18:20:09  craigs
 * Added the ability to get a stack dump on assert
 *
 * Revision 1.7  2000/03/21 03:09:54  craigs
 * Fixed the fix with EOF
 *
 * Revision 1.6  2000/03/20 22:59:18  craigs
 * Fixed problem with asserts generating unlimited output when input is redirected
 *
 * Revision 1.5  2000/02/18 01:49:18  craigs
 * Added VxWorks code
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
#include <ptlib/pprocess.h>

#ifndef __BEOS__
#ifndef P_VXWORKS

static BOOL PAssertAction(int c, const char * msg)
{
  switch (c) {
    case 'a' :
    case 'A' :
      PError << "\nAborting.\n";
      _exit(1);
      break;

#if P_EXCEPTIONS
    case 't' :
    case 'T' :
      PError << "\nThrowing exception\n";
      throw std::runtime_error(msg);
      return TRUE;
#endif
        
#ifdef _DEBUG
    case 'd' :
    case 'D' :
      {
        PString cmd = "gdb " + PProcess::Current().GetFile();
        cmd.sprintf(" %d", getpid());
        system((const char *)cmd);
      }
      break;
#endif

    case 'c' :
    case 'C' :
      PError << "\nDumping core.\n";
      kill(getpid(), SIGABRT);

    case 'i' :
    case 'I' :
    case EOF :
      PError << "\nIgnoring.\n";
      return TRUE;
  }
  return FALSE;
}
#endif
#endif

void PAssertFunc(const char * msg)

{
#ifdef __BEOS__
  // Print location in Eddie-compatible format
  PError << msg << endl;
  // Pop up the debugger dialog that gives the user the necessary choices
  // "Ignore" is not supported on BeOS but you can instruct the
  // debugger to continue executing.
  // Note: if you choose "debug" you get a debug prompt. Type bdb to
  // start the Be Debugger.
  debugger(msg);
#else
  static BOOL inAssert;
  if (inAssert)
    return;
  inAssert = TRUE;

  ostream & trace = PTrace::Begin(0, __FILE__, __LINE__);
  trace << "PWLib\t" << msg << PTrace::End;

  if (&trace != &PError)
    PError << msg << endl;

#if P_EXCEPTIONS
  //Throw a runtime exception if the environment variable PWLIB_ASSERT_EXCEPTION is set
  char * env = ::getenv("PWLIB_ASSERT_EXCEPTION");
  if (env != NULL){
    throw std::runtime_error(msg);
  }
#endif
  
#ifndef P_VXWORKS
  char * env = ::getenv("PWLIB_ASSERT_ACTION");
  if (env != NULL && *env != EOF && PAssertAction(*env, msg)) {
    inAssert = FALSE;
    return;
  }

  // Check for if stdin is not a TTY and just ignore the assert if so.
  if (!isatty(STDIN_FILENO)) {
    inAssert = FALSE;
    return;
  }

  for(;;) {
    PError << "\n<A>bort, <C>ore dump"
#if P_EXCEPTIONS
           << ", <I>gnore <T>hrow exception"
#endif
#ifdef _DEBUG
           << ", <D>ebug"
#endif
           << "? " << flush;

    int c = getchar();

    if (PAssertAction(c, msg))
      break;
   }
   inAssert = FALSE;

#else // P_VXWORKS

  PThread::Trace(); // Get debugging dump
  exit(1);
  kill(taskIdSelf(), SIGABRT);

#endif // P_VXWORKS
#endif // __BEOS__
}

// End Of File ///////////////////////////////////////////////////////////////
