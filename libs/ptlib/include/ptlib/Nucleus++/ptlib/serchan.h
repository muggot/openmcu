/*
 * serchan.h
 *
 * Asynchronous serial I/O channel class.
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
 * $Log: serchan.h,v $
 * Revision 1.1  2000/06/26 11:17:20  robertj
 * Nucleus++ port (incomplete).
 *
 * Revision 1.8  1998/11/30 22:07:08  robertj
 * New directory structure.
 *
 * Revision 1.7  1998/09/24 04:11:53  robertj
 * Added open software license.
 *
 * Revision 1.6  1996/08/03 12:08:19  craigs
 * Changed for new common directories
 *
 * Revision 1.5  1996/05/03 13:12:07  craigs
 * More Sun4 fixes
 *
 * Revision 1.4  1996/05/02 12:01:47  craigs
 * More Sun4 fixed
 *
 * Revision 1.3  1996/05/02 11:55:01  craigs
 * Fixed problem with compiling on Sun4
 *
 * Revision 1.2  1995/12/08 13:14:52  craigs
 * Added new function
 *
 * Revision 1.1  1995/01/23 18:43:27  craigs
 * Initial revision
 *
 */

#ifndef _PSERIALCHANNEL

#ifndef __NUCLEUS_PLUS__
#pragma interface
#endif

#ifdef __NUCLEUS_MNT__
#include "hardware/uart.h"
#else
extern "C" {
#include "uart.h"
}
#endif

#include "../../serchan.h"
  public:
    BOOL Close();

  private:
    DWORD  baudRate;
    BYTE   dataBits;
    Parity parityBits;
    BYTE   stopBits;
    
    UNSIGNED_CHAR smc;
    UART_INIT uart[2];
    
    virtual BOOL Read(
      void * buf,   /// Pointer to a block of memory to receive the read bytes.
      PINDEX len    /// Maximum number of bytes to read into the buffer.
      );

    virtual int ReadChar();

    virtual BOOL ReadAsync(
      void * buf,   /// Pointer to a block of memory to receive the read bytes.
      PINDEX len    /// Maximum number of bytes to read into the buffer.
      );

    virtual void OnReadComplete(
      void * buf, /// Pointer to a block of memory that received the read bytes.
      PINDEX len  /// Actual number of bytes to read into the buffer.
      );

    virtual BOOL Write(
      const void * buf, /// Pointer to a block of memory to write.
      PINDEX len        /// Number of bytes to write.
      );

    virtual BOOL WriteAsync(
      const void * buf, /// Pointer to a block of memory to write.
      PINDEX len        /// Number of bytes to write.
      );

    virtual void OnWriteComplete(
      const void * buf, /// Pointer to a block of memory to write.
      PINDEX len        /// Number of bytes to write.
      );
  };

#endif
