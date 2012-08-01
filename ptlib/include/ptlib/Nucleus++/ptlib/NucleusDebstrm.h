/*
 * debstrm.h
 *
 * Debug output stream for Windows.
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
 * $Log: NucleusDebstrm.h,v $
 * Revision 1.1  2000/06/26 11:17:19  robertj
 * Nucleus++ port (incomplete).
 *
 *
 * Revision 1.6  1998/09/24 03:39:55  robertj
 * Added open software license.
 *
 * Revision 1.5  1997/07/08 12:57:50  robertj
 * DLL support
 *
 * Revision 1.4  1996/11/04 03:34:24  robertj
 * Fixed missing #define for multiple inclusion of file.
 *
 * Revision 1.3  1996/08/17 10:00:40  robertj
 * Changes for Windows DLL support.
 *
 * Revision 1.2  1994/12/21 11:57:19  robertj
 * Fixed debugging stream.
 *
 * Revision 1.1  1994/08/21  23:43:02  robertj
 * Initial revision
 *
 * Revision 1.1  1994/07/27  05:58:07  robertj
 * Initial revision
 *
 */


#ifndef _PDEBUGSTREAM
#define _PDEBUGSTREAM

///////////////////////////////////////////////////////////////////////////////
// PDebugStream for Nucleus

class PDebugStream : public ostream {
  public:
    PDebugStream();

  private:
    class Buffer : public streambuf {
      public:
        Buffer();
        virtual int overflow(int=EOF);
        virtual int underflow();
        virtual int sync();
        char buffer[250];
    } buffer;
};



#endif
