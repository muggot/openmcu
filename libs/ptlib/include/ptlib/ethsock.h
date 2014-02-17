/*
 * ethsock.h
 *
 * Direct Ethernet socket I/O channel class.
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
 * $Log: ethsock.h,v $
 * Revision 1.18  2005/11/25 03:43:47  csoutheren
 * Fixed function argument comments to be compatible with Doxygen
 *
 * Revision 1.17  2004/04/18 04:33:36  rjongbloed
 * Changed all operators that return BOOL to return standard type bool. This is primarily
 *   for improved compatibility with std STL usage removing many warnings.
 *
 * Revision 1.16  2003/09/17 05:41:58  csoutheren
 * Removed recursive includes
 *
 * Revision 1.15  2003/09/17 01:18:02  csoutheren
 * Removed recursive include file system and removed all references
 * to deprecated coooperative threading support
 *
 * Revision 1.14  2002/09/16 01:08:59  robertj
 * Added #define so can select if #pragma interface/implementation is used on
 *   platform basis (eg MacOS) rather than compiler, thanks Robert Monaghan.
 *
 * Revision 1.13  2001/10/03 03:15:05  robertj
 * Changed to allow use of NULL pointer to indicate address of all zeros.
 *
 * Revision 1.12  2001/05/22 12:49:32  robertj
 * Did some seriously wierd rewrite of platform headers to eliminate the
 *   stupid GNU compiler warning about braces not matching.
 *
 * Revision 1.11  1999/03/09 02:59:49  robertj
 * Changed comments to doc++ compatible documentation.
 *
 * Revision 1.10  1999/02/16 08:07:11  robertj
 * MSVC 6.0 compatibility changes.
 *
 * Revision 1.9  1998/11/20 03:18:24  robertj
 * Split rad and write buffers to separate pools.
 *
 * Revision 1.8  1998/11/19 05:18:21  robertj
 * Added route table manipulation functions to PIPSocket class.
 *
 * Revision 1.7  1998/10/12 09:34:40  robertj
 * New method for getting IP addresses of interfaces.
 *
 * Revision 1.6  1998/09/23 06:20:31  robertj
 * Added open source copyright license.
 *
 * Revision 1.5  1998/09/14 12:27:21  robertj
 * Added function to parse type out of ethernet/802.2 frame.
 *
 * Revision 1.4  1998/08/25 11:06:34  robertj
 * Fixed output of PEthSocket::Address variables to streams.
 *
 * Revision 1.3  1998/08/22 04:07:42  robertj
 * Fixed GNU problem with structure packing
 *
 * Revision 1.2  1998/08/21 05:26:34  robertj
 * Fine tuning of interface.
 *
 * Revision 1.1  1998/08/20 05:46:45  robertj
 * Initial revision
 *
 */

#ifndef _PETHSOCKET
#define _PETHSOCKET

#ifdef P_USE_PRAGMA
#pragma interface
#endif

#include <ptlib/socket.h>

#ifdef _WIN32
class PWin32PacketDriver;
class PWin32SnmpLibrary;
class PWin32PacketBuffer;

PARRAY(PWin32PackBufArray, PWin32PacketBuffer);
#endif

/**This class describes a type of socket that will communicate using
   raw ethernet packets.
 */
class PEthSocket : public PSocket
{
  PCLASSINFO(PEthSocket, PSocket);

  public:
  /**@name Constructor */
  //@{
    /**Create a new ethernet packet socket. Some platforms require a set of
       buffers to be allocated to avoid losing frequent packets.
     */
    PEthSocket(
      PINDEX nReadBuffers = 8,  ///< Number of buffers used for reading.
      PINDEX nWriteBuffers = 1, ///< Number of buffers used for writing.
      PINDEX size = 1514        ///< Size of each buffer.
    );

      /// Close the socket
    ~PEthSocket();
  //@}


  public:
#pragma pack(1)
    /** An ethernet MAC Address specification.
     */
    union Address {
      BYTE b[6];
      WORD w[3];
      struct {
        DWORD l;
        WORD  s;
      } ls;

      Address();
      Address(const BYTE * addr);
      Address(const Address & addr);
      Address(const PString & str);
      Address & operator=(const Address & addr);
      Address & operator=(const PString & str);

      bool operator==(const BYTE * eth) const;
      bool operator!=(const BYTE * eth) const;
      bool operator==(const Address & eth) const { return ls.l == eth.ls.l && ls.s == eth.ls.s; }
      bool operator!=(const Address & eth) const { return ls.l != eth.ls.l || ls.s != eth.ls.s; }

      operator PString() const;

      friend ostream & operator<<(ostream & s, const Address & a)
        { return s << (PString)a; }
    };

    /** An ethernet MAC frame.
     */
    struct Frame {
      Address dst_addr;
      Address src_addr;
      union {
        struct {
          WORD type;
          BYTE payload[1500];
        } ether;
        struct {
          WORD length;
          BYTE dsap;
          BYTE ssap;
          BYTE ctrl;
          BYTE oui[3];
          WORD type;
          BYTE payload[1492];
        } snap;
      };

      /**Parse the Ethernet Frame to extract the frame type and the address of
         the payload. The length should be the original bytes read in the frame
         and may be altered to information contained in the frame, if available.
       */
      void Parse(
        WORD & type,      // Type of frame
        BYTE * & payload, // Pointer to payload
        PINDEX & length   // Length of payload (on input is full frame length)
      );
    };
#pragma pack()

  /**@name Overrides from class PChannel */
  //@{
    /**Close the channel, shutting down the link to the data source.

       @return
       TRUE if the channel successfully closed.
     */
    virtual BOOL Close();

    /**Low level read from the channel. This function may block until the
       requested number of characters were read or the read timeout was
       reached. The GetLastReadCount() function returns the actual number
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

    /**Low level write to the channel. This function will block until the
       requested number of characters are written or the write timeout is
       reached. The GetLastWriteCount() function returns the actual number
       of bytes written.

       The GetErrorCode() function should be consulted after Write() returns
       FALSE to determine what caused the failure.

       @return
       TRUE if at least len bytes were written to the channel.
     */
    virtual BOOL Write(
      const void * buf, ///< Pointer to a block of memory to write.
      PINDEX len        ///< Number of bytes to write.
    );
  //@}

  /**@name Overrides from class PSocket */
  //@{
    /**Connect a socket to an interface. The first form opens an interface by
       a name as returned by the EnumInterfaces() function. The second opens
       the interface that has the specified MAC address.

       @return
       TRUE if the channel was successfully connected to the interface.
     */
    virtual BOOL Connect(
      const PString & address   ///< Name of interface to connect to.
    );

    /**This function is illegal and will assert if attempted. You must be
       connected to an interface using Connect() to do I/O on the socket.

       @return
       TRUE if the channel was successfully opened.
     */
    virtual BOOL Listen(
      unsigned queueSize = 5,  ///< Number of pending accepts that may be queued.
      WORD port = 0,           ///< Port number to use for the connection.
      Reusability reuse = AddressIsExclusive ///< Can/Cant listen more than once.
    );
  //@}


  /**@name Information functions */
  //@{
    /**Enumerate all the interfaces that are capable of being accessed at the
       ethernet level. Begin with index 0, and increment until the function
       returns FALSE. The name string returned can be passed, unchanged, to
       the Connect() function.

       Note that the driver does not need to be open for this function to work.

       @return
       TRUE if an interface has the index supplied.
     */
    BOOL EnumInterfaces(
      PINDEX idx,      ///< Index of interface
      PString & name   ///< Interface name
    );


    /**Get the low level MAC address of the open interface.

       @return
       TRUE if the address is returned, FALSE on error.
     */
    BOOL GetAddress(
      Address & addr   ///< Variable to receive the MAC address.
    );

    /**Get the prime IP number bound to the open interface.

       @return
       TRUE if the address is returned, FALSE on error.
     */
    BOOL GetIpAddress(
      PIPSocket::Address & addr     ///< Variable to receive the IP address.
    );

    /**Get the prime IP number bound to the open interface.
       This also returns the net mask associated with the open interface.

       @return
       TRUE if the address is returned, FALSE on error.
     */
    BOOL GetIpAddress(
      PIPSocket::Address & addr,    ///< Variable to receive the IP address.
      PIPSocket::Address & netMask  ///< Variable to receive the net mask.
    );

    /**Enumerate all of the IP addresses and net masks bound to the open
       interface. This allows all the addresses to be found on multi-homed
       hosts. Begin with index 0 and increment until the function returns
       FALSE to enumerate all the addresses.

       @return
       TRUE if the address is returned, FALSE on error or if there are no more
       addresses bound to the interface.
     */
    BOOL EnumIpAddress(
      PINDEX idx,                   ///< Index 
      PIPSocket::Address & addr,    ///< Variable to receive the IP address.
      PIPSocket::Address & netMask  ///< Variable to receive the net mask.
    );


    /// Medium types for the open interface.
    enum MediumTypes {
      /// A Loopback Network
      MediumLoop,     
      /// An ethernet Network Interface Card (10base2, 10baseT etc)
      Medium802_3,    
      /// A Wide Area Network (modem etc)
      MediumWan,      
      /// Something else
      MediumUnknown,  
      NumMediumTypes
    };
    /**Return the type of the interface.

       @return
       Type enum for the interface, or NumMediumTypes if interface not open.
     */
    MediumTypes GetMedium();
  //@}


  /**@name Filtering functions */
  //@{
    /// Type codes for ethernet frames.
    enum EthTypes {
      /// All frames (3 is value for Linux)
      TypeAll = 3,          
      /// Internet Protocol
      TypeIP  = 0x800,      
      /// X.25
      TypeX25 = 0x805,      
      /// Address Resolution Protocol
      TypeARP = 0x806,      
      /// Appletalk DDP
      TypeAtalk = 0x809B,   
      /// Appletalk AARP
      TypeAARP = 0x80F3,    
      /// Novell IPX
      TypeIPX = 0x8137,     
      /// Bluebook IPv6
      TypeIPv6 = 0x86DD     
    };

    /// Mask filter bits for GetFilter() function.
    enum FilterMask {
      /// Packets directed at the interface.
      FilterDirected     = 0x01,    
      /// Multicast packets directed at the interface.
      FilterMulticast    = 0x02,    
      /// All multicast packets.
      FilterAllMulticast = 0x04,    
      /// Packets with a broadcast address.
      FilterBroadcast    = 0x08,    
      /// All packets.
      FilterPromiscuous  = 0x10     
    };

    /**Get the current filtering criteria for receiving packets.

       A bit-wise OR of the FilterMask values will filter packets so that
       they do not appear in the Read() function at all.

       The type is be the specific frame type to accept. A value of TypeAll
       may be used to match all frame types.

       @return
       A bit mask is returned, a value of 0 indicates an error.
     */
    BOOL GetFilter(
      unsigned & mask,  ///< Bits for filtering on address
      WORD & type       ///< Code for filtering on type.
    );

    /**Set the current filtering criteria for receiving packets. A bit-wise OR
       of the FilterMask values will filter packets so that they do not appear
       in the Read() function at all.

       The type is be the specific frame type to accept. A value of TypeAll
       may be used to match all frame types.

       A value of zero for the filter mask is useless and will assert.

       @return
       TRUE if the address is returned, FALSE on error.
     */
    BOOL SetFilter(
      unsigned mask,       ///< Bits for filtering on address
      WORD type = TypeAll  ///< Code for filtering on type.
    );
  //@}


  /**@name I/O functions */
  //@{
    /**Reset the interface.
     */
    BOOL ResetAdaptor();

    /**Read a packet from the interface and parse out the information
       specified by the parameters. This will automatically adjust for 802.2
       and 802.3 ethernet frames.

       @return
       TRUE if the packet read, FALSE on error.
     */
    BOOL ReadPacket(
      PBYTEArray & buffer,  ///< Buffer to receive the raw packet
      Address & dest,       ///< Destination address of packet
      Address & src,        ///< Source address of packet
      WORD & type,          ///< Packet frame type ID
      PINDEX & len,         ///< Length of payload
      BYTE * & payload      ///< Pointer into #buffer# of payload.
    );
  //@}

  protected:
    virtual BOOL OpenSocket();
    virtual const char * GetProtocolName() const;


    WORD filterType;  // Remember the set filter frame type


// Include platform dependent part of class
#ifdef _WIN32
#include "msos/ptlib/ethsock.h"
#else
#include "unix/ptlib/ethsock.h"
#endif
};

#endif

// End Of File ///////////////////////////////////////////////////////////////
