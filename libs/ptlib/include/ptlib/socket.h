/*
 * socket.h
 *
 * Berkley Socket channel ancestor class.
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
 * $Log: socket.h,v $
 * Revision 1.49  2005/11/25 03:43:47  csoutheren
 * Fixed function argument comments to be compatible with Doxygen
 *
 * Revision 1.48  2004/08/24 07:08:13  csoutheren
 * Added use of recvmsg to determine which interface UDP packets arrive on
 *
 * Revision 1.47  2004/05/06 11:28:30  rjongbloed
 * Changed P_fd_set to use malloc/free isntead of new/delete due to pedantry about [].
 *
 * Revision 1.46  2004/04/27 04:37:50  rjongbloed
 * Fixed ability to break of a PSocket::Select call under linux when a socket
 *   is closed by another thread.
 *
 * Revision 1.45  2004/01/28 08:53:02  csoutheren
 * Fixed missing delete[] operator. Thanks to Borko Jandras
 *
 * Revision 1.44  2003/09/17 05:41:59  csoutheren
 * Removed recursive includes
 *
 * Revision 1.43  2003/09/17 01:18:02  csoutheren
 * Removed recursive include file system and removed all references
 * to deprecated coooperative threading support
 *
 * Revision 1.42  2002/10/18 08:07:41  robertj
 * Fixed use of FD_ZERO as (strangely) crashes on some paltforms and would
 *   not have cleared enough of an enlarges fd_set anyway.
 *
 * Revision 1.41  2002/10/17 07:17:42  robertj
 * Added ability to increase maximum file handles on a process.
 *
 * Revision 1.40  2002/09/16 01:08:59  robertj
 * Added #define so can select if #pragma interface/implementation is used on
 *   platform basis (eg MacOS) rather than compiler, thanks Robert Monaghan.
 *
 * Revision 1.39  2002/04/12 01:42:41  robertj
 * Changed return value on os_connect() and os_accept() to make sure
 *   get the correct error codes propagated up under unix.
 *
 * Revision 1.38  2002/02/14 03:34:18  craigs
 * Added comment on using SetReadTimeout to set maximum wait for Connect
 *
 * Revision 1.37  2001/09/06 02:30:17  robertj
 * Fixed mismatched declarations, thanks Vjacheslav Andrejev
 *
 * Revision 1.36  2001/05/22 12:49:32  robertj
 * Did some seriously wierd rewrite of platform headers to eliminate the
 *   stupid GNU compiler warning about braces not matching.
 *
 * Revision 1.35  2001/03/20 06:44:25  robertj
 * Lots of changes to fix the problems with terminating threads that are I/O
 *   blocked, especially when doing orderly shutdown of service via SIGTERM.
 *
 * Revision 1.34  2000/06/26 13:58:42  robertj
 * Nucleus port (again)
 *
 * Revision 1.33  2000/06/26 11:17:19  robertj
 * Nucleus++ port (incomplete).
 *
 * Revision 1.32  2000/02/18 09:55:21  robertj
 * Added parameter so get/setsockopt can have other levels to SOL_SOCKET.
 *
 * Revision 1.31  1999/03/09 02:59:51  robertj
 * Changed comments to doc++ compatible documentation.
 *
 * Revision 1.30  1999/02/16 08:11:10  robertj
 * MSVC 6.0 compatibility changes.
 *
 * Revision 1.29  1998/11/30 02:51:58  robertj
 * New directory structure
 *
 * Revision 1.28  1998/09/23 06:21:25  robertj
 * Added open source copyright license.
 *
 * Revision 1.27  1998/08/27 00:46:58  robertj
 * Resolved signedness problems with various GNU libraries.
 *
 * Revision 1.26  1998/08/25 14:07:42  robertj
 * Added getprotobyxxx wrapper functions.
 *
 * Revision 1.25  1998/01/26 00:35:21  robertj
 * Fixed documentation of PSocket::Select().
 *
 * Revision 1.24  1996/09/14 13:09:24  robertj
 * Major upgrade:
 *   rearranged sockets to help support IPX.
 *   added indirect channel class and moved all protocols to descend from it,
 *   separating the protocol from the low level byte transport.
 *
 * Revision 1.23  1996/07/27 04:14:00  robertj
 * Changed Select() calls to return error codes.
 *
 * Revision 1.22  1996/05/15 10:11:38  robertj
 * Added timeout to accept function.
 *
 * Revision 1.21  1996/03/31 08:52:36  robertj
 * Added socket shutdown function.
 *
 * Revision 1.20  1996/03/18 13:33:12  robertj
 * Fixed incompatibilities to GNU compiler where PINDEX != int.
 *
 * Revision 1.19  1996/03/03 07:37:58  robertj
 * Added Reusability clause to the Listen() function on sockets.
 *
 * Revision 1.18  1996/03/02 03:10:18  robertj
 * Added Apability to get and set Berkeley socket options.
 *
 * Revision 1.17  1996/02/25 03:02:14  robertj
 * Moved some socket functions to platform dependent code.
 * Added array of fds to os_select for unix threading support.
 *
 * Revision 1.16  1996/02/15 14:46:43  robertj
 * Added Select() function to PSocket.
 *
 * Revision 1.15  1995/12/23 03:46:54  robertj
 * Fixed portability issue with closingh sockets.
 *
 * Revision 1.14  1995/12/10 11:35:21  robertj
 * Numerous fixes for sockets.
 *
 * Revision 1.13  1995/10/14 15:05:54  robertj
 * Added functions for changing integer from host to network byte order.
 *
 * Revision 1.12  1995/06/17 11:13:25  robertj
 * Documentation update.
 *
 * Revision 1.11  1995/06/17 00:44:35  robertj
 * More logical design of port numbers and service names.
 * Changed overloaded Open() calls to 3 separate function names.
 *
 * Revision 1.10  1995/06/04 12:36:37  robertj
 * Slight redesign of port numbers on sockets.
 *
 * Revision 1.9  1995/03/14 12:42:39  robertj
 * Updated documentation to use HTML codes.
 *
 * Revision 1.8  1995/03/12  04:45:40  robertj
 * Added more functionality.
 *
 * Revision 1.7  1995/01/03  09:36:19  robertj
 * Documentation.
 *
 * Revision 1.6  1995/01/02  12:16:17  robertj
 * Moved constructor to platform dependent code.
 *
 * Revision 1.5  1994/08/23  11:32:52  robertj
 * Oops
 *
 * Revision 1.4  1994/08/22  00:46:48  robertj
 * Added pragma fro GNU C++ compiler.
 *
 * Revision 1.3  1994/08/21  23:43:02  robertj
 * Changed type of socket port number for better portability.
 *
 * Revision 1.2  1994/07/25  03:36:03  robertj
 * Added sockets to common, normalising to same comment standard.
 *
 */

#ifndef _PSOCKETS
#define _PSOCKETS

#ifdef P_USE_PRAGMA
#pragma interface
#endif

#include <ptlib/channel.h>

#ifdef __NUCLEUS_PLUS__
#include <sys/socket.h>
#endif

class PSocket;

PLIST(PSocketList, PSocket);


/**A network communications channel. This is based on the concepts in the
   Berkley Sockets library.
   
   A socket represents a bidirectional communications channel to a {\it port}
   at a remote {\it host}.
 */
class PSocket : public PChannel
{
  PCLASSINFO(PSocket, PChannel);

  protected:
    PSocket();

  public:
  /**@name Socket establishment functions */
  //@{
    /**Connect a socket to a remote host on the specified port number. This is
       typically used by the client or initiator of a communications channel.
       This connects to a "listening" socket at the other end of the
       communications channel.

       Use the SetReadTimeout function to set a maximum time for the Connect

       @return
       TRUE if the channel was successfully connected to the remote host.
     */
    virtual BOOL Connect(
      const PString & address   ///< Address of remote machine to connect to.
    );


    /// Flags to reuse of port numbers in Listen() function.
    enum Reusability {
      CanReuseAddress,
      AddressIsExclusive
    };

    /**Listen on a socket for a remote host on the specified port number. This
       may be used for server based applications. A "connecting" socket begins
       a connection by initiating a connection to this socket. An active socket
       of this type is then used to generate other "accepting" sockets which
       establish a two way communications channel with the "connecting" socket.

       If the #port# parameter is zero then the port number as
       defined by the object instance construction or the descendent classes
       SetPort() or SetService() function.

       @return
       TRUE if the channel was successfully opened.
     */
    virtual BOOL Listen(
      unsigned queueSize = 5,  ///< Number of pending accepts that may be queued.
      WORD port = 0,           ///< Port number to use for the connection.
      Reusability reuse = AddressIsExclusive ///< Can/Cant listen more than once.
    );


    /**Open a socket to a remote host on the specified port number. This is an
       "accepting" socket. When a "listening" socket has a pending connection
       to make, this will accept a connection made by the "connecting" socket
       created to establish a link.

       The port that the socket uses is the one used in the #Listen()#
       command of the #socket# parameter. Note an error occurs if
       the #socket# parameter has not had the #Listen()#
       function called on it.

       Note that this function will block until a remote system connects to the
       port number specified in the "listening" socket. The time that the
       function will block is determined by the read timeout of the
       #socket# parameter. This will normally be
       #PMaxTimeInterval# which indicates an infinite time.

       The default behaviour is to assert.

       @return
       TRUE if the channel was successfully opened.
     */
    virtual BOOL Accept(
      PSocket & socket          ///< Listening socket making the connection.
    );

    /**Close one or both of the data streams associated with a socket 

       @return
       TRUE if the shutdown was performed
     */
    virtual BOOL Shutdown(
      ShutdownValue option   ///< Flag for shutdown of read, write or both.
    );
  //@}

  /**@name Socket options functions */
  //@{
    /**Set options on the socket. These options are defined as Berkeley socket
       options of the class SOL_SOCKET.

       @return
       TRUE if the option was successfully set.
     */
    BOOL SetOption(
      int option,             ///< Option to set.
      int value,              ///< New value for option.
      int level = SOL_SOCKET  ///< Level for option
    );

    /**Set options on the socket. These options are defined as Berkeley socket
       options of the class SOL_SOCKET.

       @return
       TRUE if the option was successfully set.
     */
    BOOL SetOption(
      int option,             ///< Option to set.
      const void * valuePtr,  ///< Pointer to new value for option.
      PINDEX valueSize,       ///< Size of new value.
      int level = SOL_SOCKET  ///< Level for option
    );

    /**Get options on the socket. These options are defined as Berkeley socket
       options of the class SOL_SOCKET.

       @return
       TRUE if the option was successfully retreived.
     */
    BOOL GetOption(
      int option,             ///< Option to get.
      int & value,            ///< Integer to receive value.
      int level = SOL_SOCKET  ///< Level for option
    );

    /**Get options on the socket. These options are defined as Berkeley socket
       options of the class SOL_SOCKET.

       @return
       TRUE if the option was successfully retreived.
     */
    BOOL GetOption(
      int option,             ///< Option to get.
      void * valuePtr,        ///< Pointer to buffer for value.
      PINDEX valueSize,       ///< Size of buffer to receive value.
      int level = SOL_SOCKET  ///< Level for option
    );
  //@}

  /**@name Port/Service database functions */
  //@{
    /**Get the number of the protocol associated with the specified name.

       @return
       Number of protocol or 0 if the protocol was not found.
     */
    static WORD GetProtocolByName(
      const PString & name      ///< Name of protocol
    );

    /**Get the name of the protocol number specified.

       @return
       Name of protocol or the number if the protocol was not found.
     */
    static PString GetNameByProtocol(
      WORD proto                ///< Number of protocol
    );


    /**Get the port number for the specified service name. */
    virtual WORD GetPortByService(
      const PString & service   ///< Name of service to get port number for.
    ) const;
    /**Get the port number for the specified service name.
    
       A name is a unique string contained in a system database. The parameter
       here may be either this unique name, an integer value or both separated
       by a space (name then integer). In the latter case the integer value is
       used if the name cannot be found in the database.
    
       The exact behviour of this function is dependent on whether TCP or UDP
       transport is being used. The #PTCPSocket# and #PUDPSocket#
       classes will implement this function.

       The static version of the function is independent of the socket type as
       its first parameter may be "tcp" or "udp", 

       @return
       Port number for service name, or 0 if service cannot be found.
     */
    static WORD GetPortByService(
      const char * protocol,     ///< Protocol type for port lookup
      const PString & service    ///< Name of service to get port number for.
    );

    /**Get the service name from the port number. */
    virtual PString GetServiceByPort(
      WORD port   ///< Number for service to find name of.
    ) const;
    /**Get the service name from the port number.
    
       A service name is a unique string contained in a system database. The
       parameter here may be either this unique name, an integer value or both
       separated by a space (name then integer). In the latter case the
       integer value is used if the name cannot be found in the database.
    
       The exact behviour of this function is dependent on whether TCP or UDP
       transport is being used. The #PTCPSocket# and #PUDPSocket#
       classes will implement this function.

       The static version of the function is independent of the socket type as
       its first parameter may be "tcp" or "udp", 

       @return
       Service name for port number.
     */
    static PString GetServiceByPort(
      const char * protocol,  ///< Protocol type for port lookup
      WORD port   ///< Number for service to find name of.
    );


    /**Set the port number for the channel. */
    void SetPort(
      WORD port   ///< New port number for the channel.
    );
    /**Set the port number for the channel. This a 16 bit number representing
       an agreed high level protocol type. The string version looks up a
       database of names to find the number for the string name.

       A service name is a unique string contained in a system database. The
       parameter here may be either this unique name, an integer value or both
       separated by a space (name then integer). In the latter case the
       integer value is used if the name cannot be found in the database.
    
       The port number may not be changed while the port is open and the
       function will assert if an attempt is made to do so.
     */
    void SetPort(
      const PString & service   ///< Service name to describe the port number.
    );

    /**Get the port the TCP socket channel object instance is using.

       @return
       Port number.
     */
    WORD GetPort() const;

    /**Get a service name for the port number the TCP socket channel object
       instance is using.

       @return
       string service name or a string representation of the port number if no
       service with that number can be found.
     */
    PString GetService() const;
  //@}

  /**@name Multiple socket selection functions */
  //@{
    /// List of sockets used for #Select()# function
    class SelectList : public PSocketList
    {
      PCLASSINFO(SelectList, PSocketList)
      public:
        SelectList()
          { DisallowDeleteObjects(); }
        /** Add a socket to list .*/
        void operator+=(PSocket & sock /** Socket to add. */)
          { Append(&sock); }
        /** Remove a socket from list .*/
        void operator-=(PSocket & sock /** Socket to remove. */)
          { Remove(&sock); }
    };

    /**Select a socket with available data. */
    static int Select(
      PSocket & sock1,        ///< First socket to check for readability.
      PSocket & sock2         ///< Second socket to check for readability.
    );
    /**Select a socket with available data. */
    static int Select(
      PSocket & sock1,        ///< First socket to check for readability.
      PSocket & sock2,        ///< Second socket to check for readability.
      const PTimeInterval & timeout ///< Timeout for wait on read/write data.
    );
    /**Select a socket with available data. */
    static Errors Select(
      SelectList & read       ///< List of sockets to check for readability.
    );
    /**Select a socket with available data. */
    static Errors Select(
      SelectList & read,      ///< List of sockets to check for readability.
      const PTimeInterval & timeout ///< Timeout for wait on read/write data.
    );
    /**Select a socket with available data. */
    static Errors Select(
      SelectList & read,      ///< List of sockets to check for readability.
      SelectList & write      ///< List of sockets to check for writability.
    );
    /**Select a socket with available data. */
    static Errors Select(
      SelectList & read,      ///< List of sockets to check for readability.
      SelectList & write,     ///< List of sockets to check for writability.
      const PTimeInterval & timeout ///< Timeout for wait on read/write data.
    );
    /**Select a socket with available data. */
    static Errors Select(
      SelectList & read,      ///< List of sockets to check for readability.
      SelectList & write,     ///< List of sockets to check for writability.
      SelectList & except     ///< List of sockets to check for exceptions.
    );
    /**Select a socket with available data. This function will block until the
       timeout or data is available to be read or written to the specified
       sockets.

       The #read#, #write# and #except# lists
       are modified by the call so that only the sockets that have data
       available are present. If the call timed out then all of these lists
       will be empty.

       If no timeout is specified then the call will block until a socket
       has data available.

       @return
       TRUE if the select was successful or timed out, FALSE if an error
       occurred. If a timeout occurred then the lists returned will be empty.

       For the versions taking sockets directly instead of lists the integer
       returned is >0 for an error being a value from the PChannel::Errors
       enum, 0 for a timeout, -1 for the first socket having read data,
       -2 for the second socket and -3 for both.
     */
    static Errors Select(
      SelectList & read,      ///< List of sockets to check for readability.
      SelectList & write,     ///< List of sockets to check for writability.
      SelectList & except,    ///< List of sockets to check for exceptions.
      const PTimeInterval & timeout ///< Timeout for wait on read/write data.
    );
  //@}

  /**@name Integer conversion functions */
  //@{
    /// Convert from host to network byte order
    inline static WORD  Host2Net(WORD  v) { return htons(v); }
    /// Convert from host to network byte order
    inline static DWORD Host2Net(DWORD v) { return htonl(v); }

    /// Convert from network to host byte order
    inline static WORD  Net2Host(WORD  v) { return ntohs(v); }
    /// Convert from network to host byte order
    inline static DWORD Net2Host(DWORD v) { return ntohl(v); }
  //@}

  protected:
    /*This function calls os_socket() with the correct parameters for the
       socket protocol type.
     */
    virtual BOOL OpenSocket() = 0;

    /**This function returns the protocol name for the socket type.
     */
    virtual const char * GetProtocolName() const = 0;


    int os_close();
    int os_socket(int af, int type, int proto);
    BOOL os_connect(
      struct sockaddr * sin,
      PINDEX size
    );
    BOOL os_recvfrom(
      void * buf,
      PINDEX len,
      int flags,
      struct sockaddr * from,
      PINDEX * fromlen
    );
    BOOL os_sendto(
      const void * buf,
      PINDEX len,
      int flags,
      struct sockaddr * to,
      PINDEX tolen
    );
    BOOL os_accept(
      PSocket & listener,
      struct sockaddr * addr,
      PINDEX * size
    );


  // Member variables
    /// Port to be used by the socket when opening the channel.
    WORD port;

#if P_HAS_RECVMSG
    BOOL catchReceiveToAddr;
    virtual void SetLastReceiveAddr(void * /*addr*/, int /*addrLen*/)
    { }
#endif

// Include platform dependent part of class
#ifdef _WIN32
#include "msos/ptlib/socket.h"
#else
#include "unix/ptlib/socket.h"
#endif
};


// Utility classes

class P_fd_set {
  public:
    P_fd_set();
    P_fd_set(SOCKET fd);
    ~P_fd_set()
      {
        free(set);
      }

    P_fd_set & operator=(SOCKET fd);
    P_fd_set & operator+=(SOCKET fd);
    P_fd_set & operator-=(SOCKET fd);

    void Zero();

    BOOL IsPresent(SOCKET fd) const
      {
        return FD_ISSET(fd, set);
      }

    operator fd_set*() const
      {
        return set;
      }

  private:
    void Construct();

    SOCKET max_fd;
    fd_set * set;
};


class P_timeval {
  public:
    P_timeval();
    P_timeval(const PTimeInterval & time)
      {
        operator=(time);
      }

    P_timeval & operator=(const PTimeInterval & time);

    operator timeval*()
      {
        return infinite ? NULL : &tval;
      }

    timeval * operator->()
      {
        return &tval;
      }

    timeval & operator*()
      {
        return tval;
      }

  private:
    struct timeval tval;
    BOOL infinite;
};

#ifdef _WIN32
class PWinSock : public PSocket
{
  PCLASSINFO(PWinSock, PSocket)
// Must be one and one only instance of this class, and it must be static!.
  public:
    PWinSock();
    ~PWinSock();
  private:
    virtual BOOL OpenSocket();
    virtual const char * GetProtocolName() const;
};
#endif

#endif

// End Of File ///////////////////////////////////////////////////////////////
