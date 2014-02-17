/*
 * qchannel.h
 *
 * Class for implementing a serial queue channel in memory.
 *
 * Portable Windows Library
 *
 * Copyright (c) 2001 Equivalence Pty. Ltd.
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
 * $Log: qchannel.h,v $
 * Revision 1.5  2007/04/19 04:33:53  csoutheren
 * Fixed problems with pre-compiled headers
 *
 * Revision 1.4  2005/11/30 12:47:37  csoutheren
 * Removed tabs, reformatted some code, and changed tags for Doxygen
 *
 * Revision 1.3  2004/11/11 07:34:50  csoutheren
 * Added #include <ptlib.h>
 *
 * Revision 1.2  2002/09/16 01:08:59  robertj
 * Added #define so can select if #pragma interface/implementation is used on
 *   platform basis (eg MacOS) rather than compiler, thanks Robert Monaghan.
 *
 * Revision 1.1  2001/07/10 03:07:07  robertj
 * Added queue channel and delay channel classes to ptclib.
 *
 */

#ifndef _QCHANNEL_H
#define _QCHANNEL_H


#ifdef P_USE_PRAGMA
#pragma interface
#endif

#ifndef _PTLIB_H
#include <ptlib.h>
#endif

/** Class for implementing a serial queue channel in memory.
    This implements a simple memory based First In First Out queue. Data
    written to an instance of the class may be read from the same instance at
    some later time.

    The queue will block the read for the Read Timeout if the queue is empty.
    Similarly a write will be clocked for Write Timeout if the queue is full.

    If there is any data to be read then it returns immediately with those
    bytes, so you must check the GetLastReadCount() to determine the actual
    number of bytes read and not rely on the count being passed into the read
    function.
  */
class PQueueChannel : public PChannel
{
    PCLASSINFO(PQueueChannel, PChannel);
  public:
  /**@name Construction */
  //@{
    /** Create a new queue channel with the specified maximum size.
      */
    PQueueChannel(
      PINDEX queueSize = 0   ///< Queue size
    );

    /**Delete queue and release memory used.
      */
    ~PQueueChannel();
  //@}


  /**@name Overrides from class PChannel */
  //@{
    /**Low level read from the file channel. The read timeout is ignored for
       file I/O. The GetLastReadCount() function returns the actual number
       of bytes read.

       The GetErrorCode() function should be consulted after Read() returns
       FALSE to determine what caused the failure.

       @return
       TRUE indicates that at least one character was read from the channel.
       FALSE means no bytes were read due to timeout or some other I/O error.
     */
    virtual BOOL Read(
      void * buf,   ///< Pointer to a block of memory to receive the read bytes.
      PINDEX len    ///< Maximum number of bytes to read into the buffer.
    );

    /**Low level write to the file channel. The write timeout is ignored for
       file I/O. The GetLastWriteCount() function returns the actual number
       of bytes written.

       The GetErrorCode() function should be consulted after Write() returns
       FALSE to determine what caused the failure.

       @return TRUE if at least len bytes were written to the channel.
     */
    virtual BOOL Write(
      const void * buf, ///< Pointer to a block of memory to write.
      PINDEX len        ///< Number of bytes to write.
    );

    /** Close the file channel.
        @return TRUE if close was OK.
      */
    virtual BOOL Close();
  //@}


  /**@name Queue manipulation functions */
  //@{
    /**Open a queue, allocating the queueSize bytes.
      */
    virtual BOOL Open(
      PINDEX queueSize   ///< Queue size
    );

    /// Get the queue size.
    PINDEX GetSize() const { return queueSize; }

    /// Get the current queue length.
    PINDEX GetLength() const { return queueLength; }
  //@}

  protected:
    PMutex     mutex;
    BYTE     * queueBuffer;
    PINDEX     queueSize, queueLength, enqueuePos, dequeuePos;
    PSyncPoint unempty;
    PSyncPoint unfull;
};


#endif // _QCHANNEL_H


// End Of File ///////////////////////////////////////////////////////////////
