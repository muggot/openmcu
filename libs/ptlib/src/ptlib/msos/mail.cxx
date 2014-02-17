/*
 * mail.cxx
 *
 * Electronic mail class.
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
 * $Log: mail.cxx,v $
 * Revision 1.17  2005/11/30 12:47:42  csoutheren
 * Removed tabs, reformatted some code, and changed tags for Doxygen
 *
 * Revision 1.16  2005/11/19 01:17:37  csoutheren
 * Removed obsolete C++ construct
 *
 * Revision 1.15  2005/11/14 23:16:26  csoutheren
 * Removed obsolete constructs that cause compiler warnings in new compilers
 *
 * Revision 1.14  1999/02/10 13:20:53  robertj
 * Added ability to have attachments in mail messages.
 *
 * Revision 1.13  1998/11/30 04:48:40  robertj
 * New directory structure
 *
 * Revision 1.12  1998/10/15 05:41:49  robertj
 * New memory leak check code.
 *
 * Revision 1.11  1998/09/24 03:30:48  robertj
 * Added open software license.
 *
 * Revision 1.10  1997/07/14 11:47:19  robertj
 * Added "const" to numerous variables.
 *
 * Revision 1.9  1997/05/16 12:05:05  robertj
 * Added BCC capability to send mail.
 *
 * Revision 1.8  1997/02/05 11:48:08  robertj
 * Fixed compatibility with MSVC debug memory allocation macros.
 *
 * Revision 1.7  1996/11/18 11:30:15  robertj
 * Fixed support for new libraries.
 *
 * Revision 1.6  1996/07/15 10:26:31  robertj
 * MSVC 4.1 Support
 *
 * Revision 1.5  1996/02/15 14:55:01  robertj
 * Win16 compatibility
 *
 * Revision 1.4  1995/08/24 12:41:25  robertj
 * Implementation of mail for GUIs.
 *
 * Revision 1.3  1995/07/02 01:22:50  robertj
 * Changed mail to use CMC then MAPI if available.
 *
 * Revision 1.2  1995/04/01 08:05:04  robertj
 * Added GUI support.
 *
 * Revision 1.1  1995/03/14 12:45:14  robertj
 * Initial revision
 *
 */

#include <ptlib.h>
#include <ptlib/mail.h>

#include <ctype.h>

#if P_HAS_CMC
#include <xcmcext.h>
#include <xcmcmsxt.h>
#endif


#define new PNEW


//////////////////////////////////////////////////////////////////////////////
// PMail

PMail::PMail()
{
  Construct();
}


PMail::PMail(const PString & username, const PString & password)
{
  Construct();
  LogOnCommonInterface(username, password, NULL);
}


PMail::PMail(const PString & username,
             const PString & password,
             const PString & service)
{
  Construct();
  LogOnCommonInterface(username, password, service);
}


PMail::~PMail()
{
  LogOff();
}


void PMail::Construct()
{
  loggedOn = FALSE;
  hUserInterface = NULL;
}


BOOL PMail::LogOn(const PString & username, const PString & password)
{
  return LogOnCommonInterface(username, password, NULL);
}


BOOL PMail::LogOn(const PString & username,
                  const PString & password,
                  const PString & service)
{
  return LogOnCommonInterface(username, password, service);
}


BOOL PMail::LogOnCommonInterface(const char * username,
                                 const char * password,
                                 const char * service)
{
  if (!LogOff())
    return FALSE;

#if P_HAS_CMC
  if (cmc.IsLoaded()) {
    CMC_X_COM_support support[2];
    support[0].item_code = CMC_XS_COM;
    support[0].flags = 0;
    support[1].item_code = CMC_XS_MS;
    support[1].flags = 0;
    CMC_extension extension;
    extension.item_code = CMC_X_COM_SUPPORT_EXT;
    extension.item_data = PARRAYSIZE(support);
    extension.item_reference = support;
    extension.extension_flags = CMC_EXT_LAST_ELEMENT;
    lastError = cmc.logon((CMC_string)service,
                          (CMC_string)username,
                          (CMC_string)password,
                          NULL,
                          (CMC_ui_id)hUserInterface,
                          100,
                          hUserInterface == NULL ? 0 :
                                 (CMC_LOGON_UI_ALLOWED | CMC_ERROR_UI_ALLOWED),
                          &sessionId,
                          &extension);
    loggedOn = lastError == CMC_SUCCESS;
    return loggedOn;
  }
#endif

#if P_HAS_MAPI
  if (mapi.IsLoaded()) {
    PAssert(service == NULL, "Cannot have variable services");

    lastError = mapi.Logon((HWND)hUserInterface, username, password, 0, 0, &sessionId);
    loggedOn = lastError == SUCCESS_SUCCESS;
    return loggedOn;
  }
#endif

  lastError = 1;
  return FALSE;
}


BOOL PMail::LogOff()
{
  if (!loggedOn)
    return TRUE;

#if P_HAS_CMC
  if (cmc.IsLoaded()) {
    lastError = cmc.logoff(sessionId,
                           (CMC_ui_id)hUserInterface,
                           hUserInterface == NULL ? 0
                              : (CMC_LOGOFF_UI_ALLOWED | CMC_ERROR_UI_ALLOWED),
                           NULL);
    switch (lastError) {
      case CMC_SUCCESS :
      case CMC_E_INVALID_SESSION_ID :
      case CMC_E_USER_NOT_LOGGED_ON :
        loggedOn = FALSE;
    }
    return lastError == CMC_SUCCESS;
  }
#endif

#if P_HAS_MAPI
  if (mapi.IsLoaded()) {
    lastError = mapi.Logoff(sessionId, (HWND)hUserInterface, 0, 0);
    if (lastError == SUCCESS_SUCCESS || lastError == MAPI_E_INVALID_SESSION)
      loggedOn = FALSE;
    return lastError == SUCCESS_SUCCESS;
  }
#endif

  lastError = 1;
  return FALSE;
}


BOOL PMail::IsLoggedOn() const
{
  return loggedOn;
}


BOOL PMail::SendNote(const PString & recipient,
                     const PString & subject,
                     const char * body)
{
  PStringList dummy;
  return SendNote(recipient, dummy, dummy, subject, body, dummy);
}


BOOL PMail::SendNote(const PString & recipient,
                     const PString & subject,
                     const char * body,
                     const PStringList & attachments)
{
  PStringList dummy;
  return SendNote(recipient, dummy, dummy, subject, body, attachments);
}


BOOL PMail::SendNote(const PString & recipient,
                     const PStringList & carbonCopies,
                     const PStringList & blindCarbons,
                     const PString & subject,
                     const char * body,
                     const PStringList & attachments)
{
#if P_HAS_CMC
  if (cmc.IsLoaded()) {
    CMC_message message;
    memset(&message, 0, sizeof(message));

    PINDEX size = carbonCopies.GetSize() + blindCarbons.GetSize() + 1;
    message.recipients = new CMC_recipient[size];
    memset(message.recipients, 0, size*sizeof(CMC_recipient));

    message.recipients[0].role = CMC_ROLE_TO;
    message.recipients[0].name = (CMC_string)(const char *)recipient;

    PINDEX count = 0;
    PINDEX i;
    for (i = 0 ; i < carbonCopies.GetSize(); i++) {
      message.recipients[++count].role = CMC_ROLE_CC;
      message.recipients[count].name = (CMC_string)(const char *)carbonCopies[i];
    }
    for (i = 0 ; i < blindCarbons.GetSize(); i++) {
      message.recipients[++count].role = CMC_ROLE_BCC;
      message.recipients[count].name = (CMC_string)(const char *)blindCarbons[i];
    }
    message.recipients[count].recip_flags = CMC_RECIP_LAST_ELEMENT;

    message.subject = (CMC_string)(const char *)subject;
    message.text_note = (CMC_string)body;
    message.message_flags = CMC_MSG_LAST_ELEMENT;

    if (!attachments.IsEmpty()) {
      message.attachments = new CMC_attachment[attachments.GetSize()];
      memset(message.attachments, 0, attachments.GetSize()*sizeof(CMC_attachment));
      for (i = 0 ; i < attachments.GetSize(); i++) {
        message.attachments[i].attach_type = CMC_ATT_OID_BINARY;
        message.attachments[i].attach_filename = (CMC_string)(const char *)attachments[i];
      }
      message.attachments[i-1].attach_flags = CMC_ATT_LAST_ELEMENT;
    }

    lastError = cmc.send(sessionId, &message, 0, (CMC_ui_id)hUserInterface, NULL);

    delete [] message.attachments;
    delete [] message.recipients;

    return lastError == CMC_SUCCESS;
  }
#endif

#if P_HAS_MAPI
  if (mapi.IsLoaded()) {
    MapiMessage message;
    memset(&message, 0, sizeof(message));

    message.nRecipCount = carbonCopies.GetSize() + blindCarbons.GetSize() + 1;
    message.lpRecips = new MapiRecipDesc[message.nRecipCount];
    memset(message.lpRecips, 0, message.nRecipCount*sizeof(MapiRecipDesc));

    message.lpRecips[0].ulRecipClass = MAPI_TO;
    message.lpRecips[0].lpszName = (char *)(const char *)recipient;

    PINDEX count = 0;
    PINDEX i;
    for (i = 0 ; i < carbonCopies.GetSize(); i++) {
      message.lpRecips[++count].ulRecipClass = MAPI_CC;
      message.lpRecips[count].lpszName = (char *)(const char *)carbonCopies[i];
    }
    for (i = 0 ; i < blindCarbons.GetSize(); i++) {
      message.lpRecips[++count].ulRecipClass = MAPI_BCC;
      message.lpRecips[count].lpszName = (char *)(const char *)blindCarbons[i];
    }

    message.lpszSubject = (char *)(const char *)subject;
    message.lpszNoteText = (char *)body;

    if (!attachments.IsEmpty()) {
      message.lpFiles = new MapiFileDesc[attachments.GetSize()];
      memset(message.lpFiles, 0, attachments.GetSize()*sizeof(MapiFileDesc));
      for (i = 0 ; i < attachments.GetSize(); i++) {
        message.lpFiles[i].nPosition = (DWORD)-1;
        message.lpFiles[i].lpszPathName = (CMC_string)(const char *)attachments[i];
      }
    }

    lastError = mapi.SendMail(sessionId, (HWND)hUserInterface, &message, 0, 0);

    delete [] message.lpRecips;

    return lastError == SUCCESS_SUCCESS;
  }
#endif

  lastError = 1;
  return FALSE;
}


PStringArray PMail::GetMessageIDs(BOOL unreadOnly)
{
  PStringArray msgIDs;

#if P_HAS_CMC
  if (cmc.IsLoaded()) {
    CMC_flags flags = CMC_LIST_MSG_REFS_ONLY;
    if (unreadOnly)
      flags |= CMC_LIST_UNREAD_ONLY;
    if (hUserInterface != NULL)
      flags |= CMC_ERROR_UI_ALLOWED;
    CMC_uint32 count = 0;
    CMC_message_summary * messages;

    lastError = cmc.list(sessionId,
                   NULL, flags, NULL, &count, hUserInterface, &messages, NULL);
    if (lastError == CMC_SUCCESS) {
      msgIDs.SetSize((PINDEX)count);
      for (PINDEX m = 0; m < (PINDEX)count; m++) {
        CMC_uint32 c;
        for (c = 0; c < messages[m].message_reference->length; c++)
          if (!isprint(messages[m].message_reference->string[c]))
            break;
        if (c >= messages[m].message_reference->length)
          msgIDs[m] = 'L' + PString(messages[m].message_reference->string,
                                    (PINDEX)messages[m].message_reference->length);
        else {
          PCharArray buf((PINDEX)(messages[m].message_reference->length*2 + 6));
          char * ptr = buf.GetPointer();
          *ptr++ = 'H';
          for (c = 0; c < messages[m].message_reference->length; c++) {
            sprintf(ptr, "%02x", messages[m].message_reference->string[c]);
            ptr += 2;
          }
          msgIDs[m] = buf;
        }
      }
      cmc.free_buf(messages);
    }
    return msgIDs;
  }
#endif

#if P_HAS_MAPI
  if (mapi.IsLoaded()) {
    FLAGS flags = unreadOnly ? MAPI_UNREAD_ONLY : 0;
    PINDEX count = 0;
    const char * seed = NULL;
    char msgIdBuffer[64];

    while ((lastError = mapi.FindNext(sessionId, (HWND)hUserInterface,
                      NULL, seed, flags, 0, msgIdBuffer)) == SUCCESS_SUCCESS) {
      if (count >= msgIDs.GetSize())
        msgIDs.SetSize(count+10);
      msgIDs[count] = msgIdBuffer;
      seed = msgIDs[count++];
    }

    msgIDs.SetSize(count);

    return msgIDs;
  }
#endif

  lastError = 1;
  return msgIDs;
}


#if P_HAS_CMC
class CMC_message_reference_ptr
{
  public:
    CMC_message_reference_ptr(const PString & id);
    ~CMC_message_reference_ptr() { free(ref); }
    operator CMC_message_reference *() { return ref; }
  private:
    CMC_message_reference * ref;
};

CMC_message_reference_ptr::CMC_message_reference_ptr(const PString & id)
{
  PINDEX len = id.GetLength();
  if (id[0] == 'H') {
    ref = (CMC_message_reference *)malloc(sizeof(ref)+(len-1)/2);
    ref->length = (len-1)/2;
    PINDEX i;
    for (i = 0; i < (PINDEX)ref->length; i++) {
      int val = 0;
      PINDEX j;
      for (j = 1; j <= 2; j++) {
        char c = id[i*2+j];
        if (isdigit(c))
          val += c - '0';
        else
          val += toupper(c) - '0' - 7;
      }
      ref->string[i] = (char)val;
    }
  }
  else if (id[0] == 'L') {
    ref = (CMC_message_reference *)malloc(sizeof(ref)+len-1);
    ref->length = len-1;
    memcpy(ref->string, ((const char *)id)+1, len-1);
  }
  else {
    ref = (CMC_message_reference *)malloc(sizeof(ref)+len);
    ref->length = len;
    memcpy(ref->string, (const char *)id, len);
  }
}
#endif


BOOL PMail::GetMessageHeader(const PString & id,
                             Header & hdrInfo)
{
#if P_HAS_CMC
  if (cmc.IsLoaded()) {
    CMC_flags flags =  hUserInterface != NULL ? CMC_ERROR_UI_ALLOWED : 0;
    CMC_uint32 count = 1;
    CMC_message_summary * message;
    CMC_message_reference_ptr seed = id;

    lastError = cmc.list(sessionId,
                    NULL, flags, seed, &count, hUserInterface, &message, NULL);
    if (lastError != CMC_SUCCESS)
      return FALSE;

    hdrInfo.subject = message->subject;
    hdrInfo.received = PTime(0, message->time_sent.minute,
                                message->time_sent.hour,
                                message->time_sent.day,
                                message->time_sent.month,
                                message->time_sent.year);
    hdrInfo.originatorName = message->originator->name;
    if (message->originator->address != 0)
      hdrInfo.originatorAddress = message->originator->address;
    else
      hdrInfo.originatorAddress = '"' + hdrInfo.originatorName + '"';

    cmc.free_buf(message);
    return TRUE;
  }
#endif

#if P_HAS_MAPI
  if (mapi.IsLoaded()) {
    MapiMessage * message;
    lastError = mapi.ReadMail(sessionId,
                    (HWND)hUserInterface, id, MAPI_ENVELOPE_ONLY, 0, &message);
    if (lastError != SUCCESS_SUCCESS)
      return FALSE;

    PStringStream str = message->lpszDateReceived;
    int min, hr, day, mon, yr;
    char c;
    str >> yr >> c >> mon >> c >> day >> hr >> c >> min;

    hdrInfo.subject = message->lpszSubject;
    hdrInfo.received = PTime(0, min, hr, day, mon, yr);
    hdrInfo.originatorName = message->lpOriginator->lpszName;
    if (message->lpOriginator->lpszAddress != 0)
      hdrInfo.originatorAddress = message->lpOriginator->lpszAddress;
    else
      hdrInfo.originatorAddress = '"' + hdrInfo.originatorName + '"';

    mapi.FreeBuffer(message);
    return TRUE;
  }
#endif

  lastError = 1;
  return FALSE;
}


BOOL PMail::GetMessageBody(const PString & id, PString & body, BOOL markAsRead)
{
  body = PString();

#if P_HAS_CMC
  if (cmc.IsLoaded()) {
    CMC_flags flags = CMC_MSG_AND_ATT_HDRS_ONLY;
    if (!markAsRead)
      flags |= CMC_DO_NOT_MARK_AS_READ;
    if (hUserInterface != NULL)
      flags |= CMC_ERROR_UI_ALLOWED;

    CMC_message_reference_ptr seed = id;
    CMC_message * message;

    lastError = cmc.read(sessionId,seed,flags,&message,hUserInterface,NULL);
    if (lastError != CMC_SUCCESS)
      return FALSE;

    if (message->text_note != NULL)
      body = message->text_note;

    BOOL ok = (message->message_flags&CMC_MSG_TEXT_NOTE_AS_FILE) == 0;
    cmc.free_buf(message);

    return ok;
  }
#endif

#if P_HAS_MAPI
  if (mapi.IsLoaded()) {
    FLAGS flags = MAPI_SUPPRESS_ATTACH;
    if (!markAsRead)
      flags |= MAPI_PEEK;

    MapiMessage * message;

    lastError = mapi.ReadMail(sessionId,
                                 (HWND)hUserInterface, id, flags, 0, &message);
    if (lastError != SUCCESS_SUCCESS)
      return FALSE;

    body = message->lpszNoteText;
    mapi.FreeBuffer(message);
    return TRUE;
  }
#endif

  lastError = 1;
  return FALSE;
}


BOOL PMail::GetMessageAttachments(const PString & id,
                                  PStringArray & filenames,
                                  BOOL includeBody,
                                  BOOL markAsRead)
{
  filenames.SetSize(0);

#if P_HAS_CMC
  if (cmc.IsLoaded()) {
    CMC_flags flags = 0;
    if (!markAsRead)
      flags |= CMC_DO_NOT_MARK_AS_READ;
    if (hUserInterface != NULL)
      flags |= CMC_ERROR_UI_ALLOWED;

    CMC_message_reference_ptr seed = id;
    CMC_message * message;

    lastError = cmc.read(sessionId,seed,flags,&message,hUserInterface,NULL);
    if (lastError != CMC_SUCCESS)
      return FALSE;

    if (message->attachments != NULL) {
      PINDEX total = 1;
      CMC_attachment * attachment;
      for (attachment = message->attachments;
            (attachment->attach_flags&CMC_ATT_LAST_ELEMENT) != 0; attachment++)
        total++;
      filenames.SetSize(total);

      PINDEX attnum = 0;
      attachment = &message->attachments[attnum];
      do {
        if (includeBody ||
                       (message->message_flags&CMC_MSG_TEXT_NOTE_AS_FILE) == 0)
          filenames[attnum++] = attachment->attach_filename;
      } while (((attachment++)->attach_flags&CMC_ATT_LAST_ELEMENT) != 0);
    }

    cmc.free_buf(message);
    return TRUE;
  }
#endif

#if P_HAS_MAPI
  if (mapi.IsLoaded()) {
    FLAGS flags = 0;
    if (includeBody)
      flags |= MAPI_BODY_AS_FILE;
    if (!markAsRead)
      flags |= MAPI_PEEK;

    MapiMessage * message;
    lastError = mapi.ReadMail(sessionId,
                                 (HWND)hUserInterface, id, flags, 0, &message);
    if (lastError != SUCCESS_SUCCESS)
      return FALSE;

    filenames.SetSize(message->nFileCount);
    PINDEX i;
    for (i = 0; i < filenames.GetSize(); i++)
      filenames[i] = message->lpFiles[i].lpszPathName;

    mapi.FreeBuffer(message);
    return TRUE;
  }
#endif

  lastError = 1;
  return FALSE;
}


BOOL PMail::MarkMessageRead(const PString & id)
{
#if P_HAS_CMC
  if (cmc.IsLoaded()) {
    CMC_flags flags = CMC_MSG_AND_ATT_HDRS_ONLY;
    if (hUserInterface != NULL)
      flags |= CMC_ERROR_UI_ALLOWED;

    CMC_message_reference_ptr seed = id;
    CMC_message * message;

    lastError = cmc.read(sessionId,seed,flags,&message,hUserInterface,NULL);
    if (lastError != CMC_SUCCESS)
      return FALSE;

    cmc.free_buf(message);
    return TRUE;
  }
#endif

#if P_HAS_MAPI
  if (mapi.IsLoaded()) {
    MapiMessage * message;
    lastError = mapi.ReadMail(sessionId,
                    (HWND)hUserInterface, id, MAPI_ENVELOPE_ONLY, 0, &message);
    if (lastError != SUCCESS_SUCCESS)
      return FALSE;

    mapi.FreeBuffer(message);
    return TRUE;
  }
#endif

  lastError = 1;
  return FALSE;
}


BOOL PMail::DeleteMessage(const PString & id)
{
#if P_HAS_CMC
  if (cmc.IsLoaded()) {
    CMC_flags flags =  hUserInterface != NULL ? CMC_ERROR_UI_ALLOWED : 0;
    CMC_message_reference_ptr seed = id;

    lastError = cmc.act_on(sessionId,
                         seed, CMC_ACT_ON_DELETE, flags, hUserInterface, NULL);
    return lastError == CMC_SUCCESS;
  }
#endif

#if P_HAS_MAPI
  if (mapi.IsLoaded()) {
    lastError = mapi.DeleteMail(sessionId, (HWND)hUserInterface, id, 0, 0);
    return lastError == SUCCESS_SUCCESS;
  }
#endif

  lastError = 1;
  return FALSE;
}


PMail::LookUpResult PMail::LookUp(const PString & name, PString * fullName)
{
#if P_HAS_CMC
  if (cmc.IsLoaded()) {
    CMC_recipient recip_in;
    memset(&recip_in, 0, sizeof(recip_in));
    recip_in.name = (CMC_string)(const char *)name;
    recip_in.recip_flags = CMC_RECIP_LAST_ELEMENT;

    CMC_recipient * recip_out;
    CMC_uint32 count = 1;
    lastError = cmc.look_up(sessionId, &recip_in, CMC_LOOKUP_RESOLVE_IDENTITY,
                          (CMC_ui_id)hUserInterface, &count, &recip_out, NULL);
    switch (lastError) {
      case CMC_SUCCESS :
        if (fullName != NULL)
          *fullName = recip_out->name;
        cmc.free_buf(recip_out);
        return ValidUser;

      case CMC_E_AMBIGUOUS_RECIPIENT :
        return AmbiguousUser;

      case CMC_E_RECIPIENT_NOT_FOUND :
        return UnknownUser;
    }
  
    return LookUpError;
  }
#endif

#if P_HAS_MAPI
  if (mapi.IsLoaded()) {
    MapiRecipDesc * recip;
    lastError = mapi.ResolveName(sessionId,
                      (HWND)hUserInterface, name, MAPI_AB_NOMODIFY, 0, &recip);

    switch (lastError) {
      case SUCCESS_SUCCESS :
        if (fullName != NULL)
          *fullName = recip->lpszName;
        mapi.FreeBuffer(recip);
        return ValidUser;

      case MAPI_E_AMBIGUOUS_RECIPIENT :
        return AmbiguousUser;

      case MAPI_E_UNKNOWN_RECIPIENT :
        return UnknownUser;
    }
  
    return LookUpError;
  }
#endif

  lastError = 1;
  return LookUpError;
}


int PMail::GetErrorCode() const
{
  return (int)lastError;
}


PString PMail::GetErrorText() const
{
#if P_HAS_CMC
  static const char * const cmcErrMsg[] = {
    "CMC_SUCCESS",
    "CMC_E_AMBIGUOUS_RECIPIENT",
    "CMC_E_ATTACHMENT_NOT_FOUND",
    "CMC_E_ATTACHMENT_OPEN_FAILURE",
    "CMC_E_ATTACHMENT_READ_FAILURE",
    "CMC_E_ATTACHMENT_WRITE_FAILURE",
    "CMC_E_COUNTED_STRING_UNSUPPORTED",
    "CMC_E_DISK_FULL",
    "CMC_E_FAILURE",
    "CMC_E_INSUFFICIENT_MEMORY",
    "CMC_E_INVALID_CONFIGURATION",
    "CMC_E_INVALID_ENUM",
    "CMC_E_INVALID_FLAG",
    "CMC_E_INVALID_MEMORY",
    "CMC_E_INVALID_MESSAGE_PARAMETER",
    "CMC_E_INVALID_MESSAGE_REFERENCE",
    "CMC_E_INVALID_PARAMETER",
    "CMC_E_INVALID_SESSION_ID",
    "CMC_E_INVALID_UI_ID",
    "CMC_E_LOGON_FAILURE",
    "CMC_E_MESSAGE_IN_USE",
    "CMC_E_NOT_SUPPORTED",
    "CMC_E_PASSWORD_REQUIRED",
    "CMC_E_RECIPIENT_NOT_FOUND",
    "CMC_E_SERVICE_UNAVAILABLE",
    "CMC_E_TEXT_TOO_LARGE",
    "CMC_E_TOO_MANY_FILES",
    "CMC_E_TOO_MANY_RECIPIENTS",
    "CMC_E_UNABLE_TO_NOT_MARK_AS_READ",
    "CMC_E_UNRECOGNIZED_MESSAGE_TYPE",
    "CMC_E_UNSUPPORTED_ACTION",
    "CMC_E_UNSUPPORTED_CHARACTER_SET",
    "CMC_E_UNSUPPORTED_DATA_EXT",
    "CMC_E_UNSUPPORTED_FLAG",
    "CMC_E_UNSUPPORTED_FUNCTION_EXT",
    "CMC_E_UNSUPPORTED_VERSION",
    "CMC_E_USER_CANCEL",
    "CMC_E_USER_NOT_LOGGED_ON"
  };
  if (cmc.IsLoaded()) {
    if (lastError < PARRAYSIZE(cmcErrMsg))
      return cmcErrMsg[lastError];
    return PString(PString::Printf, "CMC Error=%lu", lastError);
  }
#endif

#if P_HAS_MAPI
  static const char * const mapiErrMsg[] = {
    "SUCCESS_SUCCESS",
    "MAPI_USER_ABORT",
    "MAPI_E_FAILURE",
    "MAPI_E_LOGIN_FAILURE",
    "MAPI_E_DISK_FULL",
    "MAPI_E_INSUFFICIENT_MEMORY",
    "MAPI_E_ACCESS_DENIED",
    "MAPI_E_UNKNOWN",
    "MAPI_E_TOO_MANY_SESSIONS",
    "MAPI_E_TOO_MANY_FILES",
    "MAPI_E_TOO_MANY_RECIPIENTS",
    "MAPI_E_ATTACHMENT_NOT_FOUND",
    "MAPI_E_ATTACHMENT_OPEN_FAILURE",
    "MAPI_E_ATTACHMENT_WRITE_FAILURE",
    "MAPI_E_UNKNOWN_RECIPIENT",
    "MAPI_E_BAD_RECIPTYPE",
    "MAPI_E_NO_MESSAGES",
    "MAPI_E_INVALID_MESSAGE",
    "MAPI_E_TEXT_TOO_LARGE",
    "MAPI_E_INVALID_SESSION",
    "MAPI_E_TYPE_NOT_SUPPORTED",
    "MAPI_E_AMBIGUOUS_RECIPIENT",
    "MAPI_E_MESSAGE_IN_USE",
    "MAPI_E_NETWORK_FAILURE",
    "MAPI_E_INVALID_EDITFIELDS",
    "MAPI_E_INVALID_RECIPS",
    "MAPI_E_NOT_SUPPORTED"
  };
  if (mapi.IsLoaded()) {
    if (lastError < PARRAYSIZE(mapiErrMsg))
      return mapiErrMsg[lastError];
    return PString(PString::Printf, "MAPI Error=%lu", lastError);
  }
#endif

  return "No mail library loaded.";
}


#if P_HAS_CMC
PMail::CMCDLL::CMCDLL()
#ifdef _WIN32
  : PDynaLink("MAPI32.DLL")
#else
  : PDynaLink("CMC.DLL")
#endif
{
  if (!GetFunction("cmc_logon", (Function &)logon) ||
      !GetFunction("cmc_logoff", (Function &)logoff) ||
      !GetFunction("cmc_free", (Function &)free_buf) ||
      !GetFunction("cmc_query_configuration", (Function &)query_configuration) ||
      !GetFunction("cmc_look_up", (Function &)look_up) ||
      !GetFunction("cmc_list", (Function &)list) ||
      !GetFunction("cmc_send", (Function &)send) ||
      !GetFunction("cmc_read", (Function &)read) ||
      !GetFunction("cmc_act_on", (Function &)act_on))
    Close();
}
#endif


#if P_HAS_MAPI
PMail::MAPIDLL::MAPIDLL()
#ifdef _WIN32
  : PDynaLink("MAPI32.DLL")
#else
  : PDynaLink("MAPI.DLL")
#endif
{
  if (!GetFunction("MAPILogon", (Function &)Logon) ||
      !GetFunction("MAPILogoff", (Function &)Logoff) ||
      !GetFunction("MAPISendMail", (Function &)SendMail) ||
      !GetFunction("MAPISendDocuments", (Function &)SendDocuments) ||
      !GetFunction("MAPIFindNext", (Function &)FindNext) ||
      !GetFunction("MAPIReadMail", (Function &)ReadMail) ||
      !GetFunction("MAPISaveMail", (Function &)SaveMail) ||
      !GetFunction("MAPIDeleteMail", (Function &)DeleteMail) ||
      !GetFunction("MAPIFreeBuffer", (Function &)FreeBuffer) ||
      !GetFunction("MAPIAddress", (Function &)Address) ||
      !GetFunction("MAPIDetails", (Function &)Details) ||
      !GetFunction("MAPIResolveName", (Function &)ResolveName))
    Close();
}
#endif


// End Of File ///////////////////////////////////////////////////////////////
