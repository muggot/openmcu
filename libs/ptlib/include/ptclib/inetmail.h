/*
 * inetmail.h
 *
 * Internet Mail channel classes
 * Simple Mail Transport Protocol & Post Office Protocol v3
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
 * Contributor(s): Federico Pinna and Reitek S.p.A.
 *
 * $Log: inetmail.h,v $
 * Revision 1.21  2007/04/10 05:08:46  rjongbloed
 * Fixed issue with use of static C string variables in DLL environment,
 *   must use functional interface for correct initialisation.
 *
 * Revision 1.20  2005/11/30 12:47:37  csoutheren
 * Removed tabs, reformatted some code, and changed tags for Doxygen
 *
 * Revision 1.19  2004/04/21 00:29:55  csoutheren
 * Added SASL authentication to PPOP3Client and PSMTPClient
 * Thanks to Federico Pinna and Reitek S.p.A.
 *
 * Revision 1.18  2002/11/06 22:47:24  robertj
 * Fixed header comment (copyright etc)
 *
 * Revision 1.17  2002/09/16 01:08:59  robertj
 * Added #define so can select if #pragma interface/implementation is used on
 *   platform basis (eg MacOS) rather than compiler, thanks Robert Monaghan.
 *
 * Revision 1.16  2000/11/10 01:08:11  robertj
 * Added content transfer encoding and automatic base64 translation.
 *
 * Revision 1.15  2000/11/09 06:01:58  robertj
 * Added MIME version and content disposition to RFC822 class.
 *
 * Revision 1.14  2000/11/09 05:50:23  robertj
 * Added RFC822 aware channel class for doing internet mail.
 *
 * Revision 1.13  2000/06/21 01:01:21  robertj
 * AIX port, thanks Wolfgang Platzer (wolfgang.platzer@infonova.at).
 *
 * Revision 1.12  2000/06/19 11:33:53  robertj
 * Fixed incorrect comment documentation
 *
 * Revision 1.11  1999/03/09 08:01:46  robertj
 * Changed comments for doc++ support (more to come).
 *
 * Revision 1.10  1999/02/16 08:07:10  robertj
 * MSVC 6.0 compatibility changes.
 *
 * Revision 1.9  1998/11/30 02:50:51  robertj
 * New directory structure
 *
 * Revision 1.8  1998/09/23 06:19:36  robertj
 * Added open source copyright license.
 *
 * Revision 1.7  1996/12/21 01:24:15  robertj
 * Added missing open message to pop server.
 *
 * Revision 1.6  1996/09/16 12:57:45  robertj
 * Removed redundant functions.
 *
 * Revision 1.5  1996/09/14 13:17:59  robertj
 * Renamed file and changed to be a protocol off new indirect channel to separate
 *   the protocol from the low level byte transport channel.
 *
 * Revision 1.4  1996/07/27 04:14:49  robertj
 * Redesign and reimplement of mail sockets.
 *
 * Revision 1.3  1996/06/28 13:16:32  robertj
 * Changed SMTP incoming message handler so can tell when started, processing or ended message.
 *
 * Revision 1.2  1996/03/16 04:38:24  robertj
 * Added ParseReponse() for splitting reponse line into code and info.
 *
 * Revision 1.1  1996/01/23 13:04:20  robertj
 * Initial revision
 *
 * Revision 1.3  1995/06/17 11:12:15  robertj
 * Documentation update.
 *
 * Revision 1.2  1995/06/17 00:39:53  robertj
 * More implementation.
 *
 * Revision 1.1  1995/06/04 13:17:16  robertj
 * Initial revision
 *
 */

#ifndef _PMAILPROTOCOL
#define _PMAILPROTOCOL

#ifdef P_USE_PRAGMA
#pragma interface
#endif

#include <ptclib/inetprot.h>
#include <ptclib/mime.h>

class PSocket;


//////////////////////////////////////////////////////////////////////////////
// PSMTP

/** A TCP/IP socket for the Simple Mail Transfer Protocol.

   When acting as a client, the procedure is to make the connection to a
   remote server, then to send a message using the following procedure:
      <PRE><CODE>
      PSMTPClient mail("mailserver");
      if (mail.IsOpen()) {
        mail.BeginMessage("Me@here.com.au", "Fred@somwhere.com");
        mail.Write(myMessage);
        if (!mail.EndMessage())
          PError << "Mail send failed." << endl;
      }
      else
         PError << "Mail conection failed." << endl;
      </PRE></CODE>

    When acting as a server, a descendant class would be created to override
    at least the <A>LookUpName()</A> and <A>HandleMessage()</A> functions.
    Other functions may be overridden for further enhancement to the sockets
    capabilities, but these two will give a basic SMTP server functionality.

    The server socket thread would continuously call the
    <A>ProcessMessage()</A> function until it returns FALSE. This will then
    call the appropriate virtual function on parsing the SMTP protocol.
*/
class PSMTP : public PInternetProtocol
{
  PCLASSINFO(PSMTP, PInternetProtocol)

  public:
  // New functions for class.
    enum Commands {
      HELO, EHLO, QUIT, HELP, NOOP,
      TURN, RSET, VRFY, EXPN, RCPT,
      MAIL, SEND, SAML, SOML, DATA,
      AUTH, NumCommands
    };

  protected:
    PSMTP();
    // Create a new SMTP protocol channel.
};


/** A TCP/IP socket for the Simple Mail Transfer Protocol.

   When acting as a client, the procedure is to make the connection to a
   remote server, then to send a message using the following procedure:
      <PRE><CODE>
      PSMTPSocket mail("mailserver");
      if (mail.IsOpen()) {
        mail.BeginMessage("Me@here.com.au", "Fred@somwhere.com");
        mail.Write(myMessage);
        if (!mail.EndMessage())
          PError << "Mail send failed." << endl;
      }
      else
         PError << "Mail conection failed." << endl;
      </PRE></CODE>
*/
class PSMTPClient : public PSMTP
{
  PCLASSINFO(PSMTPClient, PSMTP)

  public:
    /** Create a TCP/IP SMPTP protocol socket channel. The parameterless form
       creates an unopened socket, the form with the <CODE>address</CODE>
       parameter makes a connection to a remote system, opening the socket. The
       form with the <CODE>socket</CODE> parameter opens the socket to an
       incoming call from a "listening" socket.
     */
    PSMTPClient();

    /** Destroy the channel object. This will close the channel and as a
       client, QUIT from remote SMTP server.
     */
    ~PSMTPClient();


  // Overrides from class PChannel.
    /** Close the socket, and if connected as a client, QUITs from server.

       @return
       TRUE if the channel was closed and the QUIT accepted by the server.
     */
    virtual BOOL Close();


  // New functions for class.
    /** Log into the SMTP server using the mailbox and access codes specified.
        Login is actually attempted only if the server supports SASL authentication
        and a common method is found

       @return
       TRUE if logged in.
     */
    BOOL LogIn(
      const PString & username,   ///< User name on remote system.
      const PString & password    ///< Password for user name.
    );

    /** Begin transmission of a message using the SMTP socket as a client. This
       negotiates with the remote server and establishes the protocol state
       for data transmission. The usual Write() or stream commands may then
       be used to transmit the data itself.

       @return
       TRUE if message was handled, FALSE if an error occurs.
     */
    BOOL BeginMessage(
      const PString & from,        ///< User name of sender.
      const PString & to,          ///< User name of recipient.
      BOOL eightBitMIME = FALSE    ///< Mesage will be 8 bit MIME.
    );
    BOOL BeginMessage(
      const PString & from,        ///< User name of sender.
      const PStringList & toList,  ///< List of user names of recipients.
      BOOL eightBitMIME = FALSE    ///< Mesage will be 8 bit MIME.
    );

    /** End transmission of a message using the SMTP socket as a client.

       @return
       TRUE if message was accepted by remote server, FALSE if an error occurs.
     */
    BOOL EndMessage();


  protected:
    BOOL OnOpen();

    BOOL    haveHello;
    BOOL    extendedHello;
    BOOL    eightBitMIME;
    PString fromAddress;
    PStringList toNames;
    BOOL    sendingData;

  private:
    BOOL _BeginMessage();
};


/** A TCP/IP socket for the Simple Mail Transfer Protocol.

   When acting as a client, the procedure is to make the connection to a
   remote server, then to send a message using the following procedure:
      <PRE><CODE>
      PSMTPSocket mail("mailserver");
      if (mail.IsOpen()) {
        mail.BeginMessage("Me@here.com.au", "Fred@somwhere.com");
        mail.Write(myMessage);
        if (!mail.EndMessage())
          PError << "Mail send failed." << endl;
      }
      else
         PError << "Mail conection failed." << endl;
      </PRE></CODE>

    When acting as a server, a descendant class would be created to override
    at least the <A>LookUpName()</A> and <A>HandleMessage()</A> functions.
    Other functions may be overridden for further enhancement to the sockets
    capabilities, but these two will give a basic SMTP server functionality.

    The server socket thread would continuously call the
    <A>ProcessMessage()</A> function until it returns FALSE. This will then
    call the appropriate virtual function on parsing the SMTP protocol.
*/
class PSMTPServer : public PSMTP
{
  PCLASSINFO(PSMTPServer, PSMTP)

  public:
    /** Create a TCP/IP SMPTP protocol socket channel. The parameterless form
       creates an unopened socket, the form with the <CODE>address</CODE>
       parameter makes a connection to a remote system, opening the socket. The
       form with the <CODE>socket</CODE> parameter opens the socket to an
       incoming call from a "listening" socket.
     */
    PSMTPServer();


  // New functions for class.
    /** Process commands, dispatching to the appropriate virtual function. This
       is used when the socket is acting as a server.

       @return
       TRUE if more processing may be done, FALSE if the QUIT command was
       received or the <A>OnUnknown()</A> function returns FALSE.
     */
    BOOL ProcessCommand();

    void ServerReset();
    // Reset the state of the SMTP server socket.

    enum ForwardResult {
      LocalDomain,    ///< User may be on local machine, do LookUpName().
      WillForward,    ///< User may be forwarded to another SMTP host.
      CannotForward   ///< User cannot be forwarded.
    };
    // Result of forward check

    /** Determine if a user for this domain may be on the local system, or
       should be forwarded.

       @return
       Result of forward check operation.
     */
    virtual ForwardResult ForwardDomain(
      PCaselessString & userDomain,       ///< Domain for user
      PCaselessString & forwardDomainList ///< Domains forwarding to
    );

    enum LookUpResult {
      ValidUser,      ///< User name was valid and unique.
      AmbiguousUser,  ///< User name was valid but ambiguous.
      UnknownUser,    ///< User name was invalid.
      LookUpError     ///< Some other error occurred in look up.
    };
    // Result of user name look up

    /** Look up a name in the context of the SMTP server.

       The default bahaviour simply returns FALSE.

       @return
       Result of name look up operation.
     */
    virtual LookUpResult LookUpName(
      const PCaselessString & name,    ///< Name to look up.
      PString & expandedName           ///< Expanded form of name (if found).
    );

    /** Handle a received message. The <CODE>buffer</CODE> parameter contains
       the partial or complete message received, depending on the
       <CODE>completed</CODE> parameter.

       The default behaviour is to simply return FALSE;

       @return
       TRUE if message was handled, FALSE if an error occurs.
     */
    virtual BOOL HandleMessage(
      PCharArray & buffer,  ///< Buffer containing message data received.
      BOOL starting,        ///< This is the first call for the message.
      BOOL completed        ///< This is the last call for the message.
      ///< Indication that the entire message has been received.
    );


  protected:
    BOOL OnOpen();

    virtual void OnHELO(
      const PCaselessString & remoteHost  ///< Name of remote host.
    );
    // Start connection.

    virtual void OnEHLO(
      const PCaselessString & remoteHost  ///< Name of remote host.
    );
    // Start extended SMTP connection.

    virtual void OnQUIT();
    // close connection and die.

    virtual void OnHELP();
    // get help.

    virtual void OnNOOP();
    // do nothing
    
    virtual void OnTURN();
    // switch places
    
    virtual void OnRSET();
    // Reset state.

    virtual void OnVRFY(
      const PCaselessString & name    ///< Name to verify.
    );
    // Verify address.

    virtual void OnEXPN(
      const PCaselessString & name    ///< Name to expand.
    );
    // Expand alias.

    virtual void OnRCPT(
      const PCaselessString & recipient   ///< Name of recipient.
    );
    // Designate recipient

    virtual void OnMAIL(
      const PCaselessString & sender  ///< Name of sender.
    );
    // Designate sender
    
    virtual void OnSEND(
      const PCaselessString & sender  ///< Name of sender.
    );
    // send message to screen

    virtual void OnSAML(
      const PCaselessString & sender  ///< Name of sender.
    );
    // send AND mail
    
    virtual void OnSOML(
      const PCaselessString & sender  ///< Name of sender.
    );
    // send OR mail

    virtual void OnDATA();
    // Message text.

    /** Handle an unknown command.

       @return
       TRUE if more processing may be done, FALSE if the
       <A>ProcessCommand()</A> function is to return FALSE.
     */
    virtual BOOL OnUnknown(
      const PCaselessString & command  ///< Complete command line received.
    );

    virtual void OnSendMail(
      const PCaselessString & sender  ///< Name of sender.
    );
    // Common code for OnMAIL(), OnSEND(), OnSOML() and OnSAML() funtions.

    /** Read a standard text message that is being received by the socket. The
       text message is terminated by a line with a '.' character alone.

       The default behaviour is to read the data into the <CODE>buffer</CODE>
       parameter until either the end of the message or when the
       <CODE>messageBufferSize</CODE> bytes have been read.

       @return
       TRUE if partial message received, FALSE if the end of the data was
       received.
     */
    virtual BOOL OnTextData(PCharArray & buffer, BOOL & completed);

    /** Read an eight bit MIME message that is being received by the socket. The
       MIME message is terminated by the CR/LF/./CR/LF sequence.

       The default behaviour is to read the data into the <CODE>buffer</CODE>
       parameter until either the end of the message or when the
       <CODE>messageBufferSize</CODE> bytes have been read.

       @return
       TRUE if partial message received, FALSE if the end of the data was
       received.
     */
    virtual BOOL OnMIMEData(PCharArray & buffer, BOOL & completed);


  // Member variables
    BOOL        extendedHello;
    BOOL        eightBitMIME;
    PString     fromAddress;
    PString     fromPath;
    PStringList toNames;
    PStringList toDomains;
    PINDEX      messageBufferSize;
    enum { WasMAIL, WasSEND, WasSAML, WasSOML } sendCommand;
    StuffState  endMIMEDetectState;
};


//////////////////////////////////////////////////////////////////////////////
// PPOP3

/** A TCP/IP socket for the Post Office Protocol version 3.

   When acting as a client, the procedure is to make the connection to a
   remote server, then to retrieve a message using the following procedure:
      <PRE><CODE>
      PPOP3Client mail("popserver");
      if (mail.IsOpen()) {
        if (mail.LogIn("Me", "password")) {
          if (mail.GetMessageCount() > 0) {
            PUnsignedArray sizes = mail.GetMessageSizes();
            for (PINDEX i = 0; i < sizes.GetSize(); i++) {
              if (mail.BeginMessage(i+1))
                mail.Read(myMessage, sizes[i]);
              else
                PError << "Error getting mail message." << endl;
            }
          }
          else
            PError << "No mail messages." << endl;
        }
        else
           PError << "Mail log in failed." << endl;
      }
      else
         PError << "Mail conection failed." << endl;
      </PRE></CODE>

    When acting as a server, a descendant class would be created to override
    at least the <A>HandleOpenMailbox()</A>, <A>HandleSendMessage()</A> and
    <A>HandleDeleteMessage()</A> functions. Other functions may be overridden
    for further enhancement to the sockets capabilities, but these will give a
    basic POP3 server functionality.

    The server socket thread would continuously call the
    <A>ProcessMessage()</A> function until it returns FALSE. This will then
    call the appropriate virtual function on parsing the POP3 protocol.
 */
class PPOP3 : public PInternetProtocol
{
  PCLASSINFO(PPOP3, PInternetProtocol)

  public:
    enum Commands {
      USER, PASS, QUIT, RSET, NOOP, STATcmd,
      LIST, RETR, DELE, APOP, TOP,  UIDL,
      AUTH, NumCommands
    };


  protected:
    PPOP3();

    /** Parse a response line string into a response code and any extra info
       on the line. Results are placed into the member variables
       <CODE>lastResponseCode</CODE> and <CODE>lastResponseInfo</CODE>.

       The default bahaviour looks for a space or a '-' and splits the code
       and info either side of that character, then returns FALSE.

       @return
       Position of continuation character in response, 0 if no continuation
       lines are possible.
     */
    virtual PINDEX ParseResponse(
      const PString & line ///< Input response line to be parsed
    );

  // Member variables
    static PString okResponse;
    static PString errResponse;
};


/** A TCP/IP socket for the Post Office Protocol version 3.

   When acting as a client, the procedure is to make the connection to a
   remote server, then to retrieve a message using the following procedure:
      <PRE><CODE>
      PPOP3Client mail;
      if (mail.Connect("popserver")) {
        if (mail.LogIn("Me", "password")) {
          if (mail.GetMessageCount() > 0) {
            PUnsignedArray sizes = mail.GetMessageSizes();
            for (PINDEX i = 0; i < sizes.GetSize(); i++) {
              if (mail.BeginMessage(i+1))
                mail.Read(myMessage, sizes[i]);
              else
                PError << "Error getting mail message." << endl;
            }
          }
          else
            PError << "No mail messages." << endl;
        }
        else
           PError << "Mail log in failed." << endl;
      }
      else
         PError << "Mail conection failed." << endl;
      </PRE></CODE>
 */
class PPOP3Client : public PPOP3
{
  PCLASSINFO(PPOP3Client, PPOP3)

  public:
    /** Create a TCP/IP POP3 protocol socket channel. The parameterless form
       creates an unopened socket, the form with the <CODE>address</CODE>
       parameter makes a connection to a remote system, opening the socket. The
       form with the <CODE>socket</CODE> parameter opens the socket to an
       incoming call from a "listening" socket.
     */
    PPOP3Client();

    /** Destroy the channel object. This will close the channel and as a
       client, QUIT from remote POP3 server.
     */
    ~PPOP3Client();


  // Overrides from class PChannel.
    /** Close the socket, and if connected as a client, QUITs from server.

       @return
       TRUE if the channel was closed and the QUIT accepted by the server.
     */
    virtual BOOL Close();


  // New functions for class.
    enum LoginOptions
    {
      AllowUserPass = 1,      ///< Allow the use of the plain old USER/PASS if APOP
                              ///< or SASL are not available
      UseSASL = 2,            ///< Use SASL if the AUTH command is supported by
                              ///< the server
      AllowClearTextSASL = 4  ///< Allow LOGIN and PLAIN mechanisms to be used
    };

    /** Log into the POP server using the mailbox and access codes specified.

       @return
       TRUE if logged in.
     */
    BOOL LogIn(
      const PString & username,       ///< User name on remote system.
      const PString & password,       ///< Password for user name.
      int options = AllowUserPass     ///< See LoginOptions above
    );

    /** Get a count of the number of messages in the mail box.

       @return
       Number of messages in mailbox or -1 if an error occurred.
     */
    int GetMessageCount();

    /** Get an array of a integers representing the sizes of each of the
       messages in the mail box.

       @return
       Array of integers representing the size of each message.
     */
    PUnsignedArray GetMessageSizes();

    /** Get an array of a strings representing the standard internet message
       headers of each of the messages in the mail box.

       Note that the remote server may not support this function, in which
       case an empty array will be returned.

       @return
       Array of strings continaing message headers.
     */
    PStringArray GetMessageHeaders();


    /* Begin the retrieval of an entire message. The application may then use
       the <A>PApplicationSocket::ReadLine()</A> function with the
       <CODE>unstuffLine</CODE> parameter set to TRUE. Repeated calls until
       its return valus is FALSE will read the message headers and body.

       @return
       Array of strings continaing message headers.
     */
    BOOL BeginMessage(
      PINDEX messageNumber
        /** Number of message to retrieve. This is an integer from 1 to the
           maximum number of messages available.
         */
    );

    /** Delete the message specified from the mail box.

       @return
       Array of strings continaing message headers.
     */
    BOOL DeleteMessage(
      PINDEX messageNumber
        /* Number of message to retrieve. This is an integer from 1 to the
           maximum number of messages available.
         */
    );


  protected:
    BOOL OnOpen();

  // Member variables
    BOOL loggedIn;
    PString apopBanner;
};


/** A TCP/IP socket for the Post Office Protocol version 3.

    When acting as a server, a descendant class would be created to override
    at least the <A>HandleOpenMailbox()</A>, <A>HandleSendMessage()</A> and
    <A>HandleDeleteMessage()</A> functions. Other functions may be overridden
    for further enhancement to the sockets capabilities, but these will give a
    basic POP3 server functionality.

    The server socket thread would continuously call the
    <A>ProcessMessage()</A> function until it returns FALSE. This will then
    call the appropriate virtual function on parsing the POP3 protocol.
 */
class PPOP3Server : public PPOP3
{
  PCLASSINFO(PPOP3Server, PPOP3)

  public:
    /** Create a TCP/IP POP3 protocol socket channel. The parameterless form
       creates an unopened socket, the form with the <CODE>address</CODE>
       parameter makes a connection to a remote system, opening the socket. The
       form with the <CODE>socket</CODE> parameter opens the socket to an
       incoming call from a "listening" socket.
     */
    PPOP3Server();


  // New functions for class.
    /** Process commands, dispatching to the appropriate virtual function. This
       is used when the socket is acting as a server.

       @return
       TRUE if more processing may be done, FALSE if the QUIT command was
       received or the <A>OnUnknown()</A> function returns FALSE.
     */
    BOOL ProcessCommand();

    /** Log the specified user into the mail system and return sizes of each
       message in mail box.

       The user override of this function is expected to populate the protected
       member fields <CODE>messageSizes</CODE> and <CODE>messageIDs</CODE>.

       @return
       TRUE if user and password were valid.
     */
    virtual BOOL HandleOpenMailbox(
      const PString & username,  ///< User name for mail box
      const PString & password   ///< Password for user name
    );

    /** Handle the sending of the specified message to the remote client. The
       data written to the socket will automatically have the '.' character
       stuffing enabled.

       @return
       TRUE if successfully sent message.
     */
    virtual void HandleSendMessage(
      PINDEX messageNumber, ///< Number of message to send.
      const PString & id,   ///< Unique id of message to send.
      PINDEX lines          ///< Nuumber of lines in body of message to send.
    );
    
    /** Handle the deleting of the specified message from the mail box. This is
       called when the OnQUIT command is called for each message that was
       deleted using the DELE command.

       @return
       TRUE if successfully sent message.
     */
    virtual void HandleDeleteMessage(
      PINDEX messageNumber, ///< Number of message to send.
      const PString & id    ///< Unique id of message to send.
    );
    

  protected:
    BOOL OnOpen();

    virtual void OnUSER(
      const PString & name  ///< Name of user.
    );
    // Specify user name (mailbox).

    virtual void OnPASS(
      const PString & passwd  ///< Password for account.
    );
    // Specify password and log user in.

    virtual void OnQUIT();
    // End connection, saving all changes (delete messages).

    virtual void OnRSET();
    // Reset connection (undelete messages).

    virtual void OnNOOP();
    // Do nothing.

    virtual void OnSTAT();
    // Get number of messages in mailbox.

    /** Get the size of a message in mailbox. If <CODE>msg</CODE> is 0 then get
       sizes of all messages in mailbox.
     */
    virtual void OnLIST(
      PINDEX msg  ///< Number of message.
    );

    virtual void OnRETR(
      PINDEX msg  ///< Number of message.
    );
    // Retrieve a message from mailbox.

    virtual void OnDELE(
      PINDEX msg  ///< Number of message.
    );
    // Delete a message from mailbox.

    virtual void OnTOP(
      PINDEX msg,  ///< Number of message.
      PINDEX count ///< Count of messages
    );
    // Get the message header and top <CODE>count</CODE> lines of message.

    /** Get unique ID for message in mailbox. If <CODE>msg</CODE> is 0 then get
       all IDs for all messages in mailbox.
     */
    virtual void OnUIDL(
      PINDEX msg  ///< Number of message.
    );

    /** Handle an unknown command.

       @return
       TRUE if more processing may be done, FALSE if the
       <A>ProcessCommand()</A> function is to return FALSE.
     */
    virtual BOOL OnUnknown(
      const PCaselessString & command  ///< Complete command line received.
    );


  // Member variables
    PString        username;
    PUnsignedArray messageSizes;
    PStringArray   messageIDs;
    PBYTEArray     messageDeletions;
};


/**A channel for sending/receiving RFC822 compliant mail messages.
   This encpsulates all that is required to send an RFC822 compliant message
   via another channel. It automatically adds/strips header information from
   the stream so the Read() and Write() functions only deal with the message
   body.
   For example to send a message using the SMTP classes:
      <PRE><CODE>
      PSMTPClient mail("mailserver");
      if (mail.IsOpen()) {
        PRFC822Channel message;
        message.SetFromAddress("Me@here.com.au");
        message.SetToAddress("Fred@somwhere.com");
        if (message.Open(mail)) {
          if (mail.BeginMessage("Me@here.com.au", "Fred@somwhere.com")) {
            if (!message.Write(myMessageBody))
              PError << "Mail write failed." << endl;
            if (!message.EndMessage())
              PError << "Mail send failed." << endl;
          }
        }
      }
      else
         PError << "Mail conection failed." << endl;
      </PRE></CODE>
  */
class PRFC822Channel : public PIndirectChannel
{
    PCLASSINFO(PRFC822Channel, PIndirectChannel);
  public:
    enum Direction {
      Sending,
      Receiving
    };
    /**Construct a RFC822 aware channel.
      */
    PRFC822Channel(
      Direction direction ////< Indicates are sending or receiving a message
    );

    /**Close the channel before destruction.
      */
    ~PRFC822Channel();


  // Overrides from class PChannel.
    /**Close the channel.
       This assures that all mime fields etc are closed off before closing
       the underliying channel.
      */
    BOOL Close();

    /** Low level write to the channel.

       This override assures that the header is written before the body that
       will be output via this function.

       @return
       TRUE if at least len bytes were written to the channel.
     */
    virtual BOOL Write(
      const void * buf, ///< Pointer to a block of memory to write.
      PINDEX len        ///< Number of bytes to write.
    );


    /**Begin a new message.
       This may be used if the object is to encode 2 or more messages
       sequentially. It resets the internal state of the object.
      */
    void NewMessage(
      Direction direction  ///< Indicates are sending or receiving a message
    );

    /**Enter multipart MIME message mode.
       This indicates that the message, or individual part within a message as
       MIME is nestable, is a multipart message. This form returns the
       boundary indicator string generated internally which must then be used
       in all subsequent NextPart() calls.

       Note this must be called before any writes are done to the message or
       part.
      */
    PString MultipartMessage();

    /**Enter multipart MIME message mode.
       This indicates that the message, or individual part within a message as
       MIME is nestable, is a multipart message. In this form the user
       supplies a boundary indicator string which must then be used in all
       subsequent NextPart() calls.

       Note this must be called before any writes are done to the message or
       part.
      */
    BOOL MultipartMessage(
      const PString & boundary
    );

    /**Indicate that a new multipart message part is to begin.
       This will close off the previous part, and any nested multipart
       messages contained therein, and allow a new part to begin.

       The user may adjust the parts content type and other header fields
       after this call and before the first write of the parts body. The
       default Content-Type is "text/plain".

       Note that all header fields are cleared from the previous part.
      */
    void NextPart(
      const PString & boundary
    );


    /**Set the sender address.
       This must be called before any writes are done to the channel.
      */
    void SetFromAddress(
      const PString & fromAddress  ///< Senders e-mail address
    );

    /**Set the recipient address(es).
       This must be called before any writes are done to the channel.
      */
    void SetToAddress(
      const PString & toAddress ///< Recipients e-mail address (comma separated)
    );

    /**Set the Carbon Copy address(es).
       This must be called before any writes are done to the channel.
      */
    void SetCC(
      const PString & ccAddress ///< Recipients e-mail address (comma separated)
    );

    /**Set the Blind Carbon Copy address(es).
       This must be called before any writes are done to the channel.
      */
    void SetBCC(
      const PString & bccAddress ///< Recipients e-mail address (comma separated)
    );

    /**Set the message subject.
       This must be called before any writes are done to the channel.
      */
    void SetSubject(
      const PString & subject  ///< Subject string
    );

    /**Set the content type.
       This must be called before any writes are done to the channel. It may
       be set again immediately after any call to NextPart() when multipart
       mime is being used.

       The default Content-Type is "text/plain".
      */
    void SetContentType(
      const PString & contentType   ///< Content type in form major/minor
    );

    /**Set the content disposition for attachments.
       This must be called before any writes are done to the channel. It may
       be set again immediately after any call to NextPart() when multipart
       mime is being used.

       Note that this will alter the Content-Type field to 
      */
    void SetContentAttachment(
      const PFilePath & filename   ///< Attachment filename
    );

    /**Set the content transfer encoding.
       This must be called before any writes are done to the channel. It may
       be set again immediately after any call to NextPart() when multipart
       mime is being used.

       If the encoding is "base64" (case insensitive) and , all writes will be
       treated as binary and translated into base64 encoding before output to
       the underlying channel.
      */
    void SetTransferEncoding(
      const PString & encoding,   ///< Encoding type
      BOOL autoTranslate = TRUE   ///< Automatically convert to encoding type
    );


    /**Set the and arbitrary header field.
       This must be called before any writes are done to the channel.
      */
    void SetHeaderField(
      const PString & name,   ///< MIME fields tag
      const PString & value   ///< MIME fields contents
    );

    // Common MIME header tags
    static const PString & MimeVersionTag();
    static const PString & FromTag();
    static const PString & ToTag();
    static const PString & CCTag();
    static const PString & BCCTag();
    static const PString & SubjectTag();
    static const PString & DateTag();
    static const PString & ReturnPathTag();
    static const PString & ReceivedTag();
    static const PString & MessageIDTag();
    static const PString & MailerTag();
    static const PString & ContentTypeTag();
    static const PString & ContentDispositionTag();
    static const PString & ContentTransferEncodingTag();

    /**Send this message using an SMTP socket.
       This will create a PSMTPClient and connect to the specified host then
       send the message to the remote SMTP server.
      */
    BOOL SendWithSMTP(
      const PString & hostname
    );

    /**Send this message using an SMTP socket.
       This assumes PSMTPClient is open the sends the message to the remote
       SMTP server.
      */
    BOOL SendWithSMTP(
      PSMTPClient * smtp
    );


  protected:
    BOOL OnOpen();

    BOOL        writeHeaders;
    PMIMEInfo   headers;
    BOOL        writePartHeaders;
    PMIMEInfo   partHeaders;
    PStringList boundaries;
    PBase64   * base64;
};


#endif  // _PMAILPROTOCOL


// End Of File ///////////////////////////////////////////////////////////////
