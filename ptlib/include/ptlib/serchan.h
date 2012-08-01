/*
 * serchan.h
 *
 * Asynchronous Serial I/O channel class.
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
 * Revision 1.18  2005/11/25 03:43:47  csoutheren
 * Fixed function argument comments to be compatible with Doxygen
 *
 * Revision 1.17  2005/02/20 21:18:30  dereksmithies
 * Fix documentation typos.
 *
 * Revision 1.16  2003/09/17 05:41:59  csoutheren
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
 * Revision 1.13  2001/05/22 12:49:32  robertj
 * Did some seriously wierd rewrite of platform headers to eliminate the
 *   stupid GNU compiler warning about braces not matching.
 *
 * Revision 1.12  1999/03/09 02:59:50  robertj
 * Changed comments to doc++ compatible documentation.
 *
 * Revision 1.11  1999/02/16 08:11:10  robertj
 * MSVC 6.0 compatibility changes.
 *
 * Revision 1.10  1998/09/23 06:21:21  robertj
 * Added open source copyright license.
 *
 * Revision 1.9  1995/07/31 12:15:46  robertj
 * Removed PContainer from PChannel ancestor.
 *
 * Revision 1.8  1995/06/17 11:13:18  robertj
 * Documentation update.
 *
 * Revision 1.7  1995/03/14 12:42:33  robertj
 * Updated documentation to use HTML codes.
 *
 * Revision 1.6  1995/01/14  06:19:37  robertj
 * Documentation
 *
 * Revision 1.5  1994/08/23  11:32:52  robertj
 * Oops
 *
 * Revision 1.4  1994/08/22  00:46:48  robertj
 * Added pragma fro GNU C++ compiler.
 *
 * Revision 1.3  1994/07/17  10:46:06  robertj
 * Moved data to PChannel class.
 *
 * Revision 1.2  1994/06/25  11:55:15  robertj
 * Unix version synchronisation.
 *
 * Revision 1.1  1994/04/20  12:17:44  robertj
 * Initial revision
 *
 */

#ifndef _PSERIALCHANNEL
#define _PSERIALCHANNEL

#ifdef P_USE_PRAGMA
#pragma interface
#endif


class PConfig;


///////////////////////////////////////////////////////////////////////////////
// Serial Channel

/**This class defines an I/O channel that communicates via a serial port. This
   is usually an RS-232 port.
 */
class PSerialChannel : public PChannel
{
  PCLASSINFO(PSerialChannel, PChannel);

  public:
  /**@name Construction */
  //@{
    /// Create a new serial channel object, but do not open it.
    PSerialChannel();

    /// Configuration of serial port parity options.
    enum Parity {
      /// Use the default parity, ie do not change it.
      DefaultParity,    
      /// Set the port for no parity bit.
      NoParity,         
      /// Set the port to generate parity and make it even.
      EvenParity,       
      /// Set the port to generate parity and make it odd.
      OddParity,        
      /// Set the port parity bit to mark only.
      MarkParity,       
      /// Set the port parity bit to space only.
      SpaceParity       
    };

    /// Configuration of serial port flow control options.
    enum FlowControl {
      /// Use the default flow control, ie do not change it.
      DefaultFlowControl,
      /// Set the port for no flow control.
      NoFlowControl,    
      /// Set the port for software or XON/XOFF flow control.
      XonXoff,         
      /// Set the port for hardware or RTS/CTS flow control.
      RtsCts           
    };

    /**Create a serial channal.
       The channel is opened it on the specified port and with the specified
       attributes.
     */
    PSerialChannel(
      /**The name of the serial port to connect to. This is a platform
         dependent string and woiuld rarely be a literal. The static function
         #GetPortNames()# can be used to find the platforms serial ports.
       */
      const PString & port,
      /**Serial port speed or baud rate. The actual values possible here are
         platform dependent, but the standard value of 300, 1200, 2400, 4800,
         9600, 19200, 38400 always be legal.
       */
      DWORD speed = 0,
      /**Number of data bits for serial port. The actual values possible here
         are platform dependent, but 7 and 8 should always be legal.
       */
      BYTE data = 0,
      /**Parity for serial port. The actual values possible here are platform
         dependent, but #NoParity#, #OddParity# and
         #EvenParity# should always be legal.
       */
      Parity parity = DefaultParity,
      /**Number of stop bits for serial port. The actual values possible here
         are platform dependent, but 1 and 2 should always be legal.
       */
      BYTE stop = 0,
      /**Flow control for data from the remote system into this conputer.*/
      FlowControl inputFlow = DefaultFlowControl,
      /**Flow control for data from this conputer out to remote system. */
      FlowControl outputFlow = DefaultFlowControl      
    );

    /**Create a serial channal.
       The channel is opened using attributes obtained from standard variables
       in the configuration file. Note that it assumed that the correct
       configuration file section is already set.
     */
    PSerialChannel(
      PConfig & cfg  ///< Configuration file to read serial port attributes from.
    );

    /// Close the serial channel on destruction.
    ~PSerialChannel();
  //@}


  /**@name Open functions */
  //@{
    /**Open a serial channal.
       The channel is opened it on the specified port and with the specified
       attributes.
     */
    virtual BOOL Open(
      /**The name of the serial port to connect to. This is a platform
         dependent string and woiuld rarely be a literal. The static function
         #GetPortNames()# can be used to find the platforms serial ports.
       */
      const PString & port,
      /**Serial port speed or baud rate. The actual values possible here are
         platform dependent, but the standard value of 300, 1200, 2400, 4800,
         9600, 19200, 38400 always be legal.
       */
      DWORD speed = 0,
      /**Number of data bits for serial port. The actual values possible here
         are platform dependent, but 7 and 8 should always be legal.
       */
      BYTE data = 0,
      /**Parity for serial port. The actual values possible here are platform
         dependent, but #NoParity#, #OddParity# and
         #EvenParity# should always be legal.
       */
      Parity parity = DefaultParity,
      /**Number of stop bits for serial port. The actual values possible here
         are platform dependent, but 1 and 2 should always be legal.
       */
      BYTE stop = 0,
      /**Flow control for data from the remote system into this conputer.*/
      FlowControl inputFlow = DefaultFlowControl,
      /**Flow control for data from this conputer out to remote system. */
      FlowControl outputFlow = DefaultFlowControl      
    );

    /**Open a serial channal.
       The channel is opened using attributes obtained from standard variables
       in the configuration file. Note that it assumed that the correct
       configuration file section is already set.
     */
    virtual BOOL Open(
      PConfig & cfg  ///< Configuration file to read serial port attributes from.
    );

    /**Get a list of the available serial ports. This returns a set of
       platform dependent strings which describe the serial ports of the
       computer. For example under unix it may be "ttyS0", under MS-DOS or
       NT it would be "COM1" and for the Macintosh it could be "Modem".

       @return
       list of strings for possible serial ports.
     */
    static PStringList GetPortNames();
  //@}


  /**@name Configuration functions */
  //@{
    /**Set the speed (baud rate) of the serial channel.

       @return
       TRUE if the change was successfully made.
     */
    BOOL SetSpeed(
      DWORD speed   ///< New speed for serial channel.
    );

    /**Get the speed (baud rate) of the serial channel.

       @return
       current setting.
     */
    DWORD GetSpeed() const;

    /**Set the data bits (5, 6, 7 or 8) of the serial port.

       @return
       TRUE if the change was successfully made.
     */
    BOOL SetDataBits(
      BYTE data   ///< New number of data bits for serial channel.
    );

    /**Get the data bits (5, 6, 7 or 8) of the serial port.

       @return
       current setting.
     */
    BYTE GetDataBits() const;

    /**Set the parity of the serial port.

       @return
       TRUE if the change was successfully made.
     */
    BOOL SetParity(
      Parity parity   ///< New parity option for serial channel.
    );

    /**Get the parity of the serial port.

       @return
       current setting.
     */
    Parity GetParity() const;

    /**Set the stop bits (1 or 2) of the serial port.

       @return
       TRUE if the change was successfully made.
     */
    BOOL SetStopBits(
      BYTE stop   ///< New number of stop bits for serial channel.
    );

    /**Get the stop bits (1 or 2) of the serial port.

       @return
       current setting.
     */
    BYTE GetStopBits() const;

    /**Set the flow control (handshaking) protocol of the input to the serial
       port.

       @return
       TRUE if the change was successfully made.
     */
    BOOL SetInputFlowControl(
      FlowControl flowControl   ///< New flow control for serial channel input.
    );

    /**Get the flow control (handshaking) protocol of the input to the serial
       port.

       @return
       current setting.
     */
    FlowControl GetInputFlowControl() const;

    /**Set the flow control (handshaking) protocol of the output to the serial
       port.

       @return
       TRUE if the change was successfully made.
     */
    BOOL SetOutputFlowControl(
      FlowControl flowControl   ///< New flow control for serial channel output.
    );

    /**Get the flow control (handshaking) protocol of the output from the
       serial port.

       @return
       current setting.
     */
    FlowControl GetOutputFlowControl() const;

    /**Save the current port settings into the configuration file. Note that
       it assumed that the correct configuration file section is already set.
     */
    virtual void SaveSettings(
      PConfig & cfg   ///< Configuration file to save setting into.
    );
  //@}


  /**@name Status functions */
  //@{
    /** Set the Data Terminal Ready signal of the serial port. */
    void SetDTR(
      BOOL state = TRUE   ///< New state of the DTR signal.
    );

    /**Clear the Data Terminal Ready signal of the serial port. This is
       equivalent to #SetDTR(FALSE)#.
     */
    void ClearDTR();

    /**Set the Request To Send signal of the serial port. */
    void SetRTS(
      BOOL state = TRUE   ///< New state of the RTS signal.
    );

    /**Clear the Request To Send signal of the serial port. This is equivalent
       to #SetRTS(FALSE)#.
     */
    void ClearRTS();

    /** Set the break condition of the serial port. */
    void SetBreak(
      BOOL state = TRUE   ///< New state of the serial port break condition.
    );

    /**Clear the break condition of the serial port. This is equivalent to
       #SetBreak(FALSE)#.
     */
    void ClearBreak();

    /**Get the Clear To Send signal of the serial port.
    
       @return
       TRUE if the CTS signal is asserted.
     */
    BOOL GetCTS();

    /**Get the Data Set Ready signal of the serial port.
    
       @return
       TRUE if the DSR signal is asserted.
     */
    BOOL GetDSR();

    /**Get the Data Carrier Detect signal of the serial port.
    
       @return
       TRUE if the DCD signal is asserted.
     */
    BOOL GetDCD();

    /**Get the Ring Indicator signal of the serial port.
    
       @return
       TRUE if the RI signal is asserted.
     */
    BOOL GetRing();
  //@}


  private:
    void Construct();
    // Platform dependent construct of the serial channel.


// Include platform dependent part of class
#ifdef _WIN32
#include "msos/ptlib/serchan.h"
#else
#include "unix/ptlib/serchan.h"
#endif
};

#endif

// End Of File ///////////////////////////////////////////////////////////////
