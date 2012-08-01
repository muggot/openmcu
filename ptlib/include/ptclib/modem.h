/*
 * modem.h
 *
 * AT command set modem on asynchonous port class.
 *
 * Portable Windows Library
 *
 * Copyright (c) 1993-2002 Equivalence Pty. Ltd.
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
 * $Log: modem.h,v $
 * Revision 1.16  2005/11/30 12:47:37  csoutheren
 * Removed tabs, reformatted some code, and changed tags for Doxygen
 *
 * Revision 1.15  2004/11/11 07:34:50  csoutheren
 * Added #include <ptlib.h>
 *
 * Revision 1.14  2002/11/06 22:47:24  robertj
 * Fixed header comment (copyright etc)
 *
 * Revision 1.13  2002/09/16 01:08:59  robertj
 * Added #define so can select if #pragma interface/implementation is used on
 *   platform basis (eg MacOS) rather than compiler, thanks Robert Monaghan.
 *
 * Revision 1.12  1999/03/09 08:01:46  robertj
 * Changed comments for doc++ support (more to come).
 *
 * Revision 1.11  1999/02/16 08:07:10  robertj
 * MSVC 6.0 compatibility changes.
 *
 * Revision 1.10  1998/12/02 03:51:39  robertj
 * Unix compatibility changes
 *
 * Revision 1.9  1998/09/23 06:20:53  robertj
 * Added open source copyright license.
 *
 * Revision 1.8  1995/06/17 11:12:46  robertj
 * Documentation update.
 *
 * Revision 1.7  1995/03/14 12:41:51  robertj
 * Updated documentation to use HTML codes.
 *
 * Revision 1.6  1995/01/06  10:31:02  robertj
 * Documentation.
 *
 * Revision 1.5  1994/08/23  11:32:52  robertj
 * Oops
 *
 * Revision 1.4  1994/08/22  00:46:48  robertj
 * Added pragma fro GNU C++ compiler.
 *
 * Revision 1.3  1994/08/21  23:43:02  robertj
 * Moved meta-string transmitter from PModem to PChannel.
 *
 * Revision 1.2  1994/07/25  03:32:29  robertj
 * Fixed bug in GCC with enums.
 *
 * Revision 1.1  1994/06/25  11:55:15  robertj
 * Initial revision
 *
 */


#define _PMODEM

#ifdef P_USE_PRAGMA
#pragma interface
#endif

#include <ptlib.h>

#ifndef _PSERIALCHANNEL
#include <ptlib/serchan.h>
#endif


/** A class representing a modem attached to a serial port. This adds the usual
   modem operations to the basic serial port.
   
   A modem object is always in a particular state. This state determines what
   operations are allowed which then move the object to other states. The
   operations are the exchange of strings in "chat" script.
   
   The following defaults are used for command strings:
       initialise         <CODE>ATZ\r\w2sOK\w100m</CODE>
       deinitialise       <CODE>\d2s+++\d2sATH0\r</CODE>
       pre-dial           <CODE>ATDT</CODE>
       post-dial          <CODE>\r</CODE>
       busy reply         <CODE>BUSY</CODE>
       no carrier reply   <CODE>NO CARRIER</CODE>
       connect reply      <CODE>CONNECT</CODE>
       hang up            <CODE>\d2s+++\d2sATH0\r</CODE>

 */
class PModem : public PSerialChannel
{
  PCLASSINFO(PModem, PSerialChannel)

  public:
    /** Create a modem object on the serial port specified. If no port was
       specified do not open it. It does not initially have a valid port name.
       
       See the <A>PSerialChannel</A> class for more information on the
       parameters.
     */
    PModem();
    PModem(
      const PString & port,   ///< Serial port name to open.
      DWORD speed = 0,        ///< Speed of serial port.
      BYTE data = 0,          ///< Number of data bits for serial port.
      Parity parity = DefaultParity,  ///< Parity for serial port.
      BYTE stop = 0,          ///< Number of stop bits for serial port.
      FlowControl inputFlow = DefaultFlowControl,   ///< Input flow control.
      FlowControl outputFlow = DefaultFlowControl   ///< Output flow control.
    );

    /** Open the modem serial channel obtaining the parameters from standard
       variables in the configuration file. Note that it assumed that the
       correct configuration file section is already set.
     */
    PModem(
      PConfig & cfg   ///< Configuration file to read parameters from.
    );


  // Overrides from class PChannel
    virtual BOOL Close();
    // Close the modem serial port channel.


  // Overrides from class PSerialChannel
    /** Open the modem serial channel on the specified port.
       
       See the <A>PSerialChannel</A> class for more information on the
       parameters.
       
       @return
       TRUE if the modem serial port was successfully opened.
     */
    virtual BOOL Open(
      const PString & port,   ///< Serial port name to open.
      DWORD speed = 0,        ///< Speed of serial port.
      BYTE data = 0,          ///< Number of data bits for serial port.
      Parity parity = DefaultParity,  ///< Parity for serial port.
      BYTE stop = 0,          ///< Number of stop bits for serial port.
      FlowControl inputFlow = DefaultFlowControl,   ///< Input flow control.
      FlowControl outputFlow = DefaultFlowControl   ///< Output flow control.
    );

    /** Open the modem serial port obtaining the parameters from standard
       variables in the configuration file. Note that it assumed that the
       correct configuration file section is already set.

       @return
       TRUE if the modem serial port was successfully opened.
     */
    virtual BOOL Open(
      PConfig & cfg   ///< Configuration file to read parameters from.
    );

    virtual void SaveSettings(
      PConfig & cfg   ///< Configuration file to write parameters to.
    );
    // Save the current modem serial port settings into the configuration file.


  // New member functions
    /** Set the modem initialisation meta-command string.

       See the <A>PChannel::SendCommandString()</A> function for more
       information on the format of the command string.

       Note there is an implied <CODE>\s</CODE> before the string.
     */
    void SetInitString(
      const PString & str   ///< New initialisation command string.
    );

    /** Get the modem initialisation meta-command string.
    
       See the <A>PChannel::SendCommandString()</A> function for more
       information on the format of the command string.

       @return
       string for initialisation command.
     */
    PString GetInitString() const;

    /** The modem is in a state that allows the initialise to start.
    
       @return
       TRUE if the <A>Initialise()</A> function may proceeed.
     */
    BOOL CanInitialise() const;

    /** Send the initialisation meta-command string to the modem. The return
       value indicates that the conditions for the operation to start were met,
       ie the serial port was open etc and the command was successfully
       sent with all replies met.

       @return
       TRUE if command string sent successfully and the objects state has
       changed.
     */
    BOOL Initialise();

    /** Set the modem de-initialisation meta-command string.

       See the <A>PChannel::SendCommandString()</A> function for more
       information on the format of the command string.

       Note there is an implied <CODE>\s</CODE> before the string.
     */
    void SetDeinitString(
      const PString & str   ///< New de-initialisation command string.
    );

    /** Get the modem de-initialisation meta-command string.
    
       See the <A>PChannel::SendCommandString()</A> function for more
       information on the format of the command string.

       @return
       string for de-initialisation command.
     */
    PString GetDeinitString() const;

    /** The modem is in a state that allows the de-initialise to start.
    
       @return
       TRUE if the <A>Deinitialise()</A> function may proceeed.
     */
    BOOL CanDeinitialise() const;

    /** Send the de-initialisation meta-command string to the modem. The return
       value indicates that the conditions for the operation to start were met,
       ie the serial port was open etc and the command was successfully
       sent with all replies met.

       @return
       TRUE if command string sent successfully and the objects state has
       changed.
     */
    BOOL Deinitialise();

    /** Set the modem pre-dial meta-command string.

       See the <A>PChannel::SendCommandString()</A> function for more
       information on the format of the command string.

       Note there is an implied <CODE>\s</CODE> before the string.
     */
    void SetPreDialString(
      const PString & str   ///< New pre-dial command string.
    );

    /** Get the modem pre-dial meta-command string.
    
       See the <A>PChannel::SendCommandString()</A> function for more
       information on the format of the command string.

       @return
       string for pre-dial command.
     */
    PString GetPreDialString() const;

    /** Set the modem post-dial meta-command string.

       See the <A>PChannel::SendCommandString()</A> function for more
       information on the format of the command string.

       Note there is <EM>not</EM> an implied <CODE>\s</CODE> before the
       string, unlike the pre-dial string.
     */
    void SetPostDialString(
      const PString & str   ///< New post-dial command string.
    );

    /** Get the modem post-dial meta-command string.
    
       See the <A>PChannel::SendCommandString()</A> function for more
       information on the format of the command string.

       @return
       string for post-dial command.
     */
    PString GetPostDialString() const;

    /** Set the modem busy response meta-command string.

       See the <A>PChannel::SendCommandString()</A> function for more
       information on the format of the command string.

       Note there is an implied <CODE>\w120s</CODE> before the string. Also
       the <CODE>\s</CODE> and <CODE>\d</CODE> commands do not operate and
       will simply terminate the string match.
     */
    void SetBusyString(
      const PString & str   ///< New busy response command string.
    );

    /** Get the modem busy response meta-command string.
    
       See the <A>PChannel::SendCommandString()</A> function for more
       information on the format of the command string.

       @return
       string for busy response command.
     */
    PString GetBusyString() const;

    /** Set the modem no carrier response meta-command string.

       See the <A>PChannel::SendCommandString()</A> function for more
       information on the format of the command string.

       Note there is an implied <CODE>\w120s</CODE> before the string. Also
       the <CODE>\s</CODE> and <CODE>\d</CODE> commands do not operate and
       will simply terminate the string match.
     */
    void SetNoCarrierString(
      const PString & str   ///< New no carrier response command string.
    );

    /** Get the modem no carrier response meta-command string.
    
       See the <A>PChannel::SendCommandString()</A> function for more
       information on the format of the command string.

       @return
       string for no carrier response command.
     */
    PString GetNoCarrierString() const;

    /** Set the modem connect response meta-command string.

       See the <A>PChannel::SendCommandString()</A> function for more
       information on the format of the command string.

       Note there is an implied <CODE>\w120s</CODE> before the string. Also
       the <CODE>\s</CODE> and <CODE>\d</CODE> commands do not operate and
       will simply terminate the string match.
     */
    void SetConnectString(
      const PString & str   ///< New connect response command string.
    );

    /** Get the modem connect response meta-command string.
    
       See the <A>PChannel::SendCommandString()</A> function for more
       information on the format of the command string.

       @return
       string for connect response command.
     */
    PString GetConnectString() const;

    /** The modem is in a state that allows the dial to start.
    
       @return
       TRUE if the <A>Dial()</A> function may proceeed.
     */
    BOOL CanDial() const;

    /** Send the dial meta-command strings to the modem. The return
       value indicates that the conditions for the operation to start were met,
       ie the serial port was open etc and the command was successfully
       sent with all replies met.

       The string sent to the modem is the concatenation of the pre-dial
       string, a <CODE>\s</CODE>, the <CODE>number</CODE> parameter and the
       post-dial string.

       @return
       TRUE if command string sent successfully and the objects state has
       changed.
     */
    BOOL Dial(const PString & number);

    /** Set the modem hang up meta-command string.

       See the <A>PChannel::SendCommandString()</A> function for more
       information on the format of the command string.

       Note there is an implied <CODE>\s</CODE> before the string.
     */
    void SetHangUpString(
      const PString & str   ///< New hang up command string.
    );

    /** Get the modem hang up meta-command string.
    
       See the <A>PChannel::SendCommandString()</A> function for more
       information on the format of the command string.

       @return
       string for hang up command.
     */
    PString GetHangUpString() const;

    /** The modem is in a state that allows the hang up to start.
    
       @return
       TRUE if the <A>HangUp()</A> function may proceeed.
     */
    BOOL CanHangUp() const;

    /** Send the hang up meta-command string to the modem. The return
       value indicates that the conditions for the operation to start were met,
       ie the serial port was open etc and the command was successfully
       sent with all replies met.

       @return
       TRUE if command string sent successfully and the objects state has
       changed.
     */
    BOOL HangUp();

    /** The modem is in a state that allows the user command to start.
    
       @return
       TRUE if the <A>SendUser()</A> function may proceeed.
     */
    BOOL CanSendUser() const;

    /** Send an arbitrary user meta-command string to the modem. The return
       value indicates that the conditions for the operation to start were met,
       ie the serial port was open etc and the command was successfully
       sent with all replies met.

       @return
       TRUE if command string sent successfully.
     */
    BOOL SendUser(
      const PString & str   ///< User command string to send.
    );

    void Abort();
    // Abort the current meta-string command operation eg dial, hang up etc.

    /** The modem is in a state that allows the user application to read from
       the channel. Reading while this is TRUE can interfere with the operation
       of the meta-string processing. This function is only usefull when
       multi-threading is used.

       @return
       TRUE if <A>Read()</A> operations are "safe".
     */
    BOOL CanRead() const;

    enum Status {
      Unopened,           ///< Has not been opened yet
      Uninitialised,      ///< Is open but has not yet been initialised
      Initialising,       ///< Is currently initialising the modem
      Initialised,        ///< Has been initialised but is not connected
      InitialiseFailed,   ///< Initialisation sequence failed
      Dialling,           ///< Is currently dialling
      DialFailed,         ///< Dial failed
      AwaitingResponse,   ///< Dialling in progress, awaiting connection
      LineBusy,           ///< Dial failed due to line busy
      NoCarrier,          ///< Dial failed due to no carrier
      Connected,          ///< Dial was successful and modem has connected
      HangingUp,          ///< Is currently hanging up the modem
      HangUpFailed,       ///< The hang up failed
      Deinitialising,     ///< is currently de-initialising the modem
      DeinitialiseFailed, ///< The de-initialisation failed
      SendingUserCommand, ///< Is currently sending a user command
      NumStatuses
    };
    // Modem object states.

    /** Get the modem objects current state.

       @return
       modem status.
     */
    Status GetStatus() const;


  protected:
    // Member variables
    PString initCmd, deinitCmd, preDialCmd, postDialCmd,
            busyReply, noCarrierReply, connectReply, hangUpCmd;
      // Modem command meta-strings.

    Status status;
      // Current modem status
};


// End Of File ///////////////////////////////////////////////////////////////
