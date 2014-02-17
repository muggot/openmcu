/*
 * mail.h
 *
 * Electronic Mail abstraction class.
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
 * $Log: mail.h,v $
 * Revision 1.14  2005/11/25 03:43:47  csoutheren
 * Fixed function argument comments to be compatible with Doxygen
 *
 * Revision 1.13  2003/09/17 05:41:58  csoutheren
 * Removed recursive includes
 *
 * Revision 1.12  2003/09/17 01:18:02  csoutheren
 * Removed recursive include file system and removed all references
 * to deprecated coooperative threading support
 *
 * Revision 1.11  2002/09/16 01:08:59  robertj
 * Added #define so can select if #pragma interface/implementation is used on
 *   platform basis (eg MacOS) rather than compiler, thanks Robert Monaghan.
 *
 * Revision 1.10  2001/05/22 12:49:32  robertj
 * Did some seriously wierd rewrite of platform headers to eliminate the
 *   stupid GNU compiler warning about braces not matching.
 *
 * Revision 1.9  1999/03/09 02:59:50  robertj
 * Changed comments to doc++ compatible documentation.
 *
 * Revision 1.8  1999/02/16 08:12:00  robertj
 * MSVC 6.0 compatibility changes.
 *
 * Revision 1.7  1999/02/10 13:20:53  robertj
 * Added ability to have attachments in mail messages.
 *
 * Revision 1.6  1998/09/23 06:20:51  robertj
 * Added open source copyright license.
 *
 * Revision 1.5  1997/05/16 12:14:08  robertj
 * Added BCC capability to send mail.
 *
 * Revision 1.4  1995/07/02 01:19:46  robertj
 * Change GetMessageBidy to return BOOL and have body string as
 *   parameter, due to slight change in semantics for large bodies.
 *
 * Revision 1.3  1995/06/17 00:42:22  robertj
 * Added mail reading interface.
 * Changed name to simply PMail
 *
 * Revision 1.2  1995/04/01 08:27:57  robertj
 * Added GUI support.
 *
 * Revision 1.1  1995/03/14  12:44:11  robertj
 * Initial revision
 *
 */

#ifndef _PMAIL
#define _PMAIL

#ifdef P_USE_PRAGMA
#pragma interface
#endif

#ifdef _WIN32

#  ifndef P_HAS_MAPI
#  define P_HAS_MAPI 1
#  endif

#  ifndef P_HAS_CMC
#  define P_HAS_CMC 1
#  endif

#  if P_HAS_MAPI
#  include <mapi.h>
#  endif

#  if P_HAS_CMC
#  include <xcmc.h>
#  endif

#endif  // _WIN32


/**This class establishes a mail session with the platforms mail system.
*/
class PMail : public PObject
{
  PCLASSINFO(PMail, PObject);

  public:
  /**@name Construction */
  //@{
    /**Create a mail session. It is initially not logged in.
     */
    PMail();

    /**Create a mail session.
       Attempt to log in using the parameters provided.
     */
    PMail(
      const PString & username,  ///< User withing mail system to use.
      const PString & password   ///< Password for user in mail system.
    );

    /**Create a mail session.
       Attempt to log in using the parameters provided.
     */
    PMail(
      const PString & username,  ///< User withing mail system to use.
      const PString & password,  ///< Password for user in mail system.
      const PString & service
      /**A platform dependent string indicating the location of the underlying
         messaging service, eg the path to a message store or node name of the
         mail server.
       */
    );


    virtual ~PMail();
    /* Destroy the mail session, logging off the mail system if necessary.
     */
  //@}

  /**@name Log in/out functions */
  //@{
    /**Attempt to log on to the mail system using the parameters provided.

       @return
       TRUE if successfully logged on.
     */
    BOOL LogOn(
      const PString & username,  ///< User withing mail system to use.
      const PString & password   ///< Password for user in mail system.
    );

    /**Attempt to log on to the mail system using the parameters provided.

       @return
       TRUE if successfully logged on.
     */
    BOOL LogOn(
      const PString & username,  ///< User withing mail system to use.
      const PString & password,  ///< Password for user in mail system.
      const PString & service
      /**A platform dependent string indicating the location of the underlying
         messaging service, eg the path to a message store or node name of the
         mail server.
       */
    );

    /**Log off from the mail system.

       @return
       TRUE if successfully logged off.
     */
    virtual BOOL LogOff();

    /**Determine if the mail session is active and logged into the mail system.

       @return
       TRUE if logged into the mail system.
     */
    BOOL IsLoggedOn() const;
  //@}

  /**@name Send message functions */
  //@{
    /**Send a new simple mail message.

       @return
       TRUE if the mail message was successfully queued. Note that this does
       {\bf not} mean that it has been delivered.
     */
    BOOL SendNote(
      const PString & recipient,  ///< Name of recipient of the mail message.
      const PString & subject,    ///< Subject name for the mail message.
      const char * body           ///< Text body of the mail message.
    );

    /**Send a new simple mail message.

       @return
       TRUE if the mail message was successfully queued. Note that this does
       {\bf not} mean that it has been delivered.
     */
    BOOL SendNote(
      const PString & recipient,  ///< Name of recipient of the mail message.
      const PString & subject,    ///< Subject name for the mail message.
      const char * body,          ///< Text body of the mail message.
      const PStringList & attachments
                        ///< List of files to attach to the mail message.
    );

    /**Send a new simple mail message.

       @return
       TRUE if the mail message was successfully queued. Note that this does
       {\bf not} mean that it has been delivered.
     */
    BOOL SendNote(
      const PString & recipient,  ///< Name of recipient of the mail message.
      const PStringList & carbonCopies, ///< Name of CC recipients.
      const PStringList & blindCarbons, ///< Name of BCC recipients.
      const PString & subject,        ///< Subject name for the mail message.
      const char * body,              ///< Text body of the mail message.
      const PStringList & attachments
                        ///< List of files to attach to the mail message.
    );
  //@}

  /**@name Read message functions */
  //@{
    /**Get a list of ID strings for all messages in the mail box.

       @return
       An array of ID strings.
     */
    PStringArray GetMessageIDs(
      BOOL unreadOnly = TRUE    ///< Only get the IDs for unread messages.
    );

    /// Message header for each mail item.
    struct Header {
      /// Subject for message.
      PString  subject;           
      /// Full name of message originator.
      PString  originatorName;    
      /// Return address of message originator.
      PString  originatorAddress; 
      /// Time message received.
      PTime    received;          
    };

    /**Get the header information for a message.

       @return
       TRUE if header information was successfully obtained.
     */
    BOOL GetMessageHeader(
      const PString & id,      ///< Identifier of message to get header.
      Header & hdrInfo         ///< Header info for the message.
    );

    /**Get the body text for a message into the #body# string
       parameter.

       Note that if the body text for the mail message is very large, the
       function will return FALSE. To tell between an error getting the message
       body and having a large message body the #GetErrorCode()# function
       must be used.

       To get a large message body, the #GetMessageAttachments()# should
       be used with the #includeBody# parameter set to TRUE so that
       the message body is placed into a disk file.

       @return
       TRUE if the body text was retrieved, FALSE if the body was too large or
       some other error occurred.
     */
    BOOL GetMessageBody(
      const PString & id,      ///< Identifier of message to get body.
      PString & body,          ///< Body text of mail message.
      BOOL markAsRead = FALSE  ///< Mark the message as read.
    );

    /**Get all of the attachments for a message as disk files.

       @return
       TRUE if attachments were successfully obtained.
     */
    BOOL GetMessageAttachments(
      const PString & id,       ///< Identifier of message to get attachments.
      PStringArray & filenames, ///< File names for each attachment.
      BOOL includeBody = FALSE, ///< Include the message body as first attachment
      BOOL markAsRead = FALSE   ///< Mark the message as read
    );

    /**Mark the message as read.

       @return
       TRUE if message was successfully marked as read.
     */
    BOOL MarkMessageRead(
      const PString & id      ///< Identifier of message to get header.
    );

    /**Delete the message from the system.

       @return
       TRUE if message was successfully deleted.
     */
    BOOL DeleteMessage(
      const PString & id      ///< Identifier of message to get header.
    );
  //@}

  /**@name User look up functions */
  //@{
    /// Result of a lookup operation with the #LookUp()# function.
    enum LookUpResult {
      /// User name is unknown in mail system.
      UnknownUser,    
      /// User is ambiguous in mail system.
      AmbiguousUser,  
      /// User is a valid, unique name in mail system.
      ValidUser,      
      /// An error occurred during the look up
      LookUpError     
    };

    /**Look up the specified name and verify that they are a valid address in
       the mail system.

       @return
       result of the name lookup.
     */
    LookUpResult LookUp(
      const PString & name,  ///< Name to look up.
      PString * fullName = NULL
      /**String to receive full name of user passed in #name#. If
         NULL then the full name is {\bf not} returned.
       */
    );
  //@}

  /**@name Error functions */
  //@{
    /**Get the internal error code for the last error by a function in this
       mail session.

       @return
       integer error code for last operation.
     */
    int GetErrorCode() const;

    /**Get the internal error description for the last error by a function in
       this mail session.

       @return
       string error text for last operation.
     */
    PString GetErrorText() const;
  //@}


  protected:
    void Construct();
    // Common construction code.

    /// Flag indicating the session is active.
    BOOL loggedOn;


// Include platform dependent part of class
#ifdef _WIN32
#include "msos/ptlib/mail.h"
#else
#include "unix/ptlib/mail.h"
#endif
};

#endif

// End Of File ///////////////////////////////////////////////////////////////
