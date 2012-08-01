/*
 * remconn.h
 *
 * Remote networking connection class.
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
 * $Log: remconn.h,v $
 * Revision 1.18  2005/11/25 03:43:47  csoutheren
 * Fixed function argument comments to be compatible with Doxygen
 *
 * Revision 1.17  2003/09/17 05:41:59  csoutheren
 * Removed recursive includes
 *
 * Revision 1.16  2003/09/17 01:18:02  csoutheren
 * Removed recursive include file system and removed all references
 * to deprecated coooperative threading support
 *
 * Revision 1.15  2002/09/16 01:08:59  robertj
 * Added #define so can select if #pragma interface/implementation is used on
 *   platform basis (eg MacOS) rather than compiler, thanks Robert Monaghan.
 *
 * Revision 1.14  2001/05/22 12:49:32  robertj
 * Did some seriously wierd rewrite of platform headers to eliminate the
 *   stupid GNU compiler warning about braces not matching.
 *
 * Revision 1.13  1999/03/09 02:59:50  robertj
 * Changed comments to doc++ compatible documentation.
 *
 * Revision 1.12  1999/02/16 08:11:10  robertj
 * MSVC 6.0 compatibility changes.
 *
 * Revision 1.11  1998/09/23 06:21:17  robertj
 * Added open source copyright license.
 *
 * Revision 1.10  1998/07/24 06:58:27  robertj
 * Added ability to get IP number of RAS connection.
 *
 * Revision 1.9  1998/02/03 06:28:46  robertj
 * Added more error codes.
 *
 * Revision 1.8  1998/01/26 00:34:51  robertj
 * Added parameter to PRemoteConnection to open only if already connected.
 * Added function to PRemoteConnection to get at OS error code.
 *
 * Revision 1.7  1997/04/01 06:00:05  robertj
 * Added Remove Configuration.
 *
 * Revision 1.6  1997/01/12 04:15:11  robertj
 * Added ability to add/change new connections.
 *
 * Revision 1.5  1996/11/04 03:40:43  robertj
 * Added more debugging for remote drop outs.
 *
 * Revision 1.4  1996/08/11 07:03:45  robertj
 * Changed remote connection to late bind DLL.
 *
 * Revision 1.3  1996/04/23 11:33:04  robertj
 * Added username and password.
 *
 * Revision 1.2  1996/03/02 03:09:48  robertj
 * Added function to get all possible remote access connection names.
 *
 * Revision 1.1  1995/12/10 13:04:46  robertj
 * Initial revision
 *
 */

#ifndef _PREMOTECONNECTION
#define _PREMOTECONNECTION

#ifdef P_USE_PRAGMA
#pragma interface
#endif

#include <ptlib/pipechan.h>

#ifdef _WIN32
#include <ras.h>
#include <raserror.h>
#endif

/** Remote Access Connection class.
*/
class PRemoteConnection : public PObject
{
  PCLASSINFO(PRemoteConnection, PObject);

  public:
  /**@name Construction */
  //@{
    /// Create a new remote connection.
    PRemoteConnection();

    /**Create a new remote connection.
       This will initiate the connection using the specified settings.
     */
    PRemoteConnection(
      const PString & name  ///< Name of RAS configuration.
    );

    /// Disconnect remote connection.
    ~PRemoteConnection();
  //@}

  /**@name Overrides from class PObject */
  //@{
    /** Compare two connections.
      @return EqualTo of same RAS connectionconfiguration.
     */
    virtual Comparison Compare(
      const PObject & obj     ///< Another connection instance.
    ) const;

    /** Get has value for the connection
        @return Hash value of the connection name string.
      */
    virtual PINDEX HashFunction() const;
  //@}

  /**@name Dial/Hangup functions */
  //@{
    /** Open the remote connection.
     */
    BOOL Open(
      BOOL existing = FALSE  ///< Flag for open only if already connected.
    );

    /** Open the remote connection.
     */
    BOOL Open(
      const PString & name,   ///< RAS name of of connection to open.
      BOOL existing = FALSE   ///< Flag for open only if already connected.
    );

    /** Open the remote connection.
     */
    BOOL Open(
      const PString & name,     ///< RAS name of of connection to open.
      const PString & username, ///< Username for remote log in.
      const PString & password, ///< password for remote log in.
      BOOL existing = FALSE     ///< Flag for open only if already connected.
    );

    /** Close the remote connection.
        This will hang up/dosconnect the connection, net access will no longer
        be available to this site.
      */
    void Close();
  //@}

  /**@name Error/Status functions */
  //@{
    /// Status codes for remote connection.
    enum Status {
      /// Connection has not been made and no attempt is being made.
      Idle,
      /// Connection is completed and active.
      Connected,
      /// Connection is in progress.
      InProgress,
      /// Connection failed due to the line being busy.
      LineBusy,
      /// Connection failed due to the line havin no dial tone.
      NoDialTone,
      /// Connection failed due to the remote not answering.
      NoAnswer,
      /// Connection failed due to the port being in use.
      PortInUse,
      /// Connection failed due to the RAS setting name/number being incorrect.
      NoNameOrNumber,
      /// Connection failed due to insufficient privilege.
      AccessDenied,
      /// Connection failed due to a hardware failure.
      HardwareFailure,
      /// Connection failed due to a general failure.
      GeneralFailure,
      /// Connection was lost after successful establishment.
      ConnectionLost,
      /// The Remote Access Operating System support is not installed.
      NotInstalled,
      NumStatuses
    };

    /**Get the current status of the RAS connection.

       @return
       Status code.
     */
    Status GetStatus() const;

    /**Get the error code for the last operation.

       @return
       Operating system error code.
     */
    DWORD GetErrorCode() const { return osError; }
  //@}

  /**@name Information functions */
  //@{
    /**Get the name of the RAS connection.

       @return
       String for IP address, or empty string if none.
     */
    const PString & GetName() const { return remoteName; }

    /**Get the IP address in dotted decimal form for the RAS connection.

       @return
       String for IP address, or empty string if none.
     */
    PString GetAddress();

    /**Get an array of names for all of the available remote connections on
       this system.

       @return
       Array of strings for remote connection names.
     */
    static PStringArray GetAvailableNames();
  //@}

  /**@name Configuration functions */
  //@{
    /// Structure for a RAS configuration.
    struct Configuration {
      /// Device name for connection eg /dev/modem
      PString device;
      /// Telephone number to call to make the connection.
      PString phoneNumber;
      /// IP address of local machine after connection is made.
      PString ipAddress;
      /// DNS host on remote site.
      PString dnsAddress;
      /// Script name for doing remote log in.
      PString script;
      /// Sub-entry number when Multi-link PPP is used.
      PINDEX  subEntries;
      /// Always establish maximum bandwidth when Multi-link PPP is used.
      BOOL    dialAllSubEntries;
    };

    /**Get the configuration of the specified remote access connection.

       @return
       #Connected# if the configuration information was obtained,
       #NoNameOrNumber# if the particular RAS name does not exist,
       #NotInstalled# if there is no RAS support in the operating system,
       #GeneralFailure# on any other error.
     */
    Status GetConfiguration(
      Configuration & config  ///< Configuration of remote connection
    );

    /**Get the configuration of the specified remote access connection.

       @return
       #Connected# if the configuration information was obtained,
       #NoNameOrNumber# if the particular RAS name does not exist,
       #NotInstalled# if there is no RAS support in the operating system,
       #GeneralFailure# on any other error.
     */
    static Status GetConfiguration(
      const PString & name,   ///< Remote connection name to get configuration
      Configuration & config  ///< Configuration of remote connection
    );

    /**Set the configuration of the specified remote access connection.

       @return
       #Connected# if the configuration information was set,
       #NoNameOrNumber# if the particular RAS name does not exist,
       #NotInstalled# if there is no RAS support in the operating system,
       #GeneralFailure# on any other error.
     */
    Status SetConfiguration(
      const Configuration & config,  ///< Configuration of remote connection
      BOOL create = FALSE            ///< Flag to create connection if not present
    );

    /**Set the configuration of the specified remote access connection.

       @return
       #Connected# if the configuration information was set,
       #NoNameOrNumber# if the particular RAS name does not exist,
       #NotInstalled# if there is no RAS support in the operating system,
       #GeneralFailure# on any other error.
     */
    static Status SetConfiguration(
      const PString & name,          ///< Remote connection name to configure
      const Configuration & config,  ///< Configuration of remote connection
      BOOL create = FALSE            ///< Flag to create connection if not present
    );

    /**Remove the specified remote access connection.

       @return
       #Connected# if the configuration information was removed,
       #NoNameOrNumber# if the particular RAS name does not exist,
       #NotInstalled# if there is no RAS support in the operating system,
       #GeneralFailure# on any other error.
     */
    static Status RemoveConfiguration(
      const PString & name          ///< Remote connection name to configure
    );
  //@}
    
  protected:
    PString remoteName;
    PString userName;
    PString password;
    DWORD osError;

  private:
    PRemoteConnection(const PRemoteConnection &) { }
    void operator=(const PRemoteConnection &) { }
    void Construct();


// Include platform dependent part of class
#ifdef _WIN32
#include "msos/ptlib/remconn.h"
#else
#include "unix/ptlib/remconn.h"
#endif
};

#endif

// End Of File ///////////////////////////////////////////////////////////////
