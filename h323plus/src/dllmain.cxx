/*
 * dllmain.cxx
 *
 * DLL main entry point for OpenH323.dll
 *
 * Open H323 Library
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
 * The Original Code is Open H323 Library.
 *
 * The Initial Developer of the Original Code is Equivalence Pty. Ltd.
 *
 * Contributor(s): ______________________________________.
 *
 * $Log: dllmain.cxx,v $
 * Revision 1.1  2007/08/06 20:51:03  shorne
 * First commit of h323plus
 *
 * Revision 1.3  2002/11/10 23:14:40  robertj
 * Made sure static variables are included in DLL
 *
 * Revision 1.2  2000/05/02 04:32:26  robertj
 * Fixed copyright notice comment.
 *
 * Revision 1.1  2000/04/13 00:02:01  robertj
 * Added ability to create DLL version of library.
 *
 */

#include <ptlib.h>

#define H323_STATIC_LIB
#include <h323.h>


HINSTANCE PDllInstance;

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID)
{
  if (fdwReason == DLL_PROCESS_ATTACH)
    PDllInstance = hinstDLL;
  return TRUE;
}

