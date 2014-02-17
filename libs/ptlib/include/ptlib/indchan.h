/*
 * indchan.h
 *
 * Indirect I/O channel class.
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
 * $Log: indchan.h,v $
 * Revision 1.11  2006/06/21 03:28:41  csoutheren
 * Various cleanups thanks for Frederic Heem
 *
 * Revision 1.10  2005/11/25 03:43:47  csoutheren
 * Fixed function argument comments to be compatible with Doxygen
 *
 * Revision 1.9  2002/09/16 01:08:59  robertj
 * Added #define so can select if #pragma interface/implementation is used on
 *   platform basis (eg MacOS) rather than compiler, thanks Robert Monaghan.
 *
 * Revision 1.8  2001/09/10 02:51:22  robertj
 * Major change to fix problem with error codes being corrupted in a
 *   PChannel when have simultaneous reads and writes in threads.
 *
 * Revision 1.7  2001/05/22 12:49:32  robertj
 * Did some seriously wierd rewrite of platform headers to eliminate the
 *   stupid GNU compiler warning about braces not matching.
 *
 * Revision 1.6  2000/11/14 08:25:25  robertj
 * Added function to propagate the error text through to indirect channel.
 *
 * Revision 1.5  1999/06/17 13:38:11  robertj
 * Fixed race condition on indirect channel close, mutex needed in PIndirectChannel.
 *
 * Revision 1.4  1999/03/09 02:59:49  robertj
 * Changed comments to doc++ compatible documentation.
 *
 * Revision 1.3  1999/02/16 08:12:00  robertj
 * MSVC 6.0 compatibility changes.
 *
 * Revision 1.2  1998/09/23 06:20:41  robertj
 * Added open source copyright license.
 *
 * Revision 1.1  1996/09/14 13:00:56  robertj
 * Initial revision
 *
 */

#ifndef _PINDIRECTCHANNEL
#define _PINDIRECTCHANNEL

#ifdef P_USE_PRAGMA
#pragma interface
#endif

#include <ptlib/channel.h>


/**This is a channel that operates indirectly through another channel(s). This
   allows for a protocol to operate through a "channel" mechanism and for its
   low level byte exchange (Read and Write) to operate via a completely
   different channel, eg TCP or Serial port etc.
 */
class PIndirectChannel : public PChannel
{
  PCLASSINFO(PIndirectChannel, PChannel);

  public:
  /**@name Construction */
  //@{
    /**Create a new indirect channel without any channels to redirect to. If
       an attempt to read or write is made before Open() is called the the
       functions will assert.
     */
    PIndirectChannel();

    /// Close the indirect channel, deleting read/write channels if desired.
    ~PIndirectChannel();
  //@}


  /**@name Overrides from class PObject */
  //@{
    /**Determine if the two objects refer to the same indirect channel. This
       actually compares the channel pointers.

       @return
       EqualTo if channel pointer identical.
     */
    Comparison Compare(
      const PObject & obj   ///< Another indirect channel to compare against.
    ) const;
  //@}


  /**@name Overrides from class PChannel */
  //@{
    /**Get the name of the channel. This is a combination of the channel
       pointers names (or simply the channel pointers name if the read and
       write channels are the same) or empty string if both null.
    
       @return
       string for the channel names.
     */
    virtual PString GetName() const;

    /**Close the channel. This will detach itself from the read and write
       channels and delete both of them if they are auto delete.

       @return
       TRUE if the channel is closed.
     */
    virtual BOOL Close();

    /**Determine if the channel is currently open and read and write operations
       can be executed on it. For example, in the #PFile# class it returns
       if the file is currently open.

       @return
       TRUE if the channel is open.
     */
    virtual BOOL IsOpen() const;

    /**Low level read from the channel. This function may block until the
       requested number of characters were read or the read timeout was
       reached. The GetLastReadCount() function returns the actual number
       of bytes read.

       This will use the #readChannel# pointer to actually do the
       read. If #readChannel# is null the this asserts.

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

    /**Low level write to the channel. This function will block until the
       requested number of characters are written or the write timeout is
       reached. The GetLastWriteCount() function returns the actual number
       of bytes written.

       This will use the #writeChannel# pointer to actually do the
       write. If #writeChannel# is null the this asserts.

       The GetErrorCode() function should be consulted after Write() returns
       FALSE to determine what caused the failure.

       @return
       TRUE if at least len bytes were written to the channel.
     */
    virtual BOOL Write(
      const void * buf, ///< Pointer to a block of memory to write.
      PINDEX len        ///< Number of bytes to write.
    );

    /**Close one or both of the data streams associated with a channel.

       The behavour here is to pass the shutdown on to its read and write
       channels.

       @return
       TRUE if the shutdown was successfully performed.
     */
    virtual BOOL Shutdown(
      ShutdownValue option   ///< Flag for shut down of read, write or both.
    );


    /**This function returns the eventual base channel for reading of a series
       of indirect channels provided by descendents of #PIndirectChannel#.

       The behaviour for this function is to return "this".
       
       @return
       Pointer to base I/O channel for the indirect channel.
     */
    virtual PChannel * GetBaseReadChannel() const;

    /**This function returns the eventual base channel for writing of a series
       of indirect channels provided by descendents of #PIndirectChannel#.

       The behaviour for this function is to return "this".
       
       @return
       Pointer to base I/O channel for the indirect channel.
     */
    virtual PChannel * GetBaseWriteChannel() const;

    /** Get error message description.
        Return a string indicating the error message that may be displayed to
       the user. The error for the last I/O operation in this object is used.
      @return Operating System error description string.
     */
    virtual PString GetErrorText(
      ErrorGroup group = NumErrorGroups   ///< Error group to get
    ) const;
  //@}

  /**@name Channel establish functions */
  //@{
    /**Set the channel for both read and write operations. This then checks
       that they are open and then calls the OnOpen() virtual function. If
       it in turn returns TRUE then the Open() function returns success.

       @return
       TRUE if both channels are set, open and OnOpen() returns TRUE.
     */
    BOOL Open(
      PChannel & channel   ///< Channel to be used for both read and write operations.
    );

    /**Set the channel for both read and write operations. This then checks
       that they are open and then calls the OnOpen() virtual function. If
       it in turn returns TRUE then the Open() function returns success.

       The channel pointed to by #channel# may be automatically deleted
       when the PIndirectChannel is destroyed or a new subchannel opened.

       @return
       TRUE if both channels are set, open and OnOpen() returns TRUE.
     */
    BOOL Open(
      PChannel * channel,      ///< Channel to be used for both read and write operations.
      BOOL autoDelete = TRUE   ///< Automatically delete the channel
    );

    /**Set the channel for both read and write operations. This then checks
       that they are open and then calls the OnOpen() virtual function. If
       it in turn returns TRUE then the Open() function returns success.

       The channels pointed to by #readChannel# and #writeChannel# may be
       automatically deleted when the PIndirectChannel is destroyed or a
       new subchannel opened.

       @return
       TRUE if both channels are set, open and OnOpen() returns TRUE.
     */
    BOOL Open(
      PChannel * readChannel,      ///< Channel to be used for both read operations.
      PChannel * writeChannel,     ///< Channel to be used for both write operations.
      BOOL autoDeleteRead = TRUE,  ///< Automatically delete the read channel
      BOOL autoDeleteWrite = TRUE  ///< Automatically delete the write channel
    );

    /**Get the channel used for read operations.
    
       @return
       pointer to the read channel.
     */
    PChannel * GetReadChannel() const;

    /**Set the channel for read operations.

       @return
       Returns TRUE if both channels are set and are both open.
     */
    BOOL SetReadChannel(
      PChannel * channel,      ///< Channel to be used for both read operations.
      BOOL autoDelete = TRUE   ///< Automatically delete the channel
    );

    /**Get the channel used for write operations.
    
       @return
       pointer to the write channel.
     */
    PChannel * GetWriteChannel() const;

    /**Set the channel for read operations.

       @return
       Returns TRUE if both channels are set and are both open.
    */
    BOOL SetWriteChannel(
      PChannel * channel,      ///< Channel to be used for both write operations.
      BOOL autoDelete = TRUE   ///< Automatically delete the channel
    );
  //@}


  protected:
    /**This callback is executed when the Open() function is called with
       open channels. It may be used by descendent channels to do any
       handshaking required by the protocol that channel embodies.

       The default behaviour is to simply return TRUE.

       @return
       Returns TRUE if the protocol handshaking is successful.
     */
    virtual BOOL OnOpen();


  // Member variables
    /// Channel for read operations.
    PChannel * readChannel;

    /// Automatically delete read channel on destruction.
    BOOL readAutoDelete;

    /// Channel for write operations.
    PChannel * writeChannel;

    /// Automatically delete write channel on destruction.
    BOOL writeAutoDelete;

    /// Race condition prevention on closing channel
    PReadWriteMutex channelPointerMutex;
};


#endif // _PINDIRECTCHANNEL


// End Of File ///////////////////////////////////////////////////////////////
