/*
 * h235pugin.h
 *
 * h235 Implementation for the h323plus library.
 *
 * Copyright (c) 2006 ISVO (Asia) Pte Ltd. All Rights Reserved.
 *
 * The contents of this file are subject to the Mozilla Public License
 * Version 1.1 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Alternatively, the contents of this file may be used under the terms
 * of the General Public License (the  "GNU License"), in which case the
 * provisions of GNU License are applicable instead of those
 * above. If you wish to allow use of your version of this file only
 * under the terms of the GNU License and not to allow others to use
 * your version of this file under the MPL, indicate your decision by
 * deleting the provisions above and replace them with the notice and
 * other provisions required by the GNU License. If you do not delete
 * the provisions above, a recipient may use your version of this file
 * under either the MPL or the GNU License."
 *
 * Software distributed under the License is distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See
 * the License for the specific language governing rights and limitations
 * under the License.
 *
 *
 * Contributor(s): ______________________________________.
 *
 * $Log: h235plugin.h,v $
 * Revision 1.1  2007/08/06 20:50:49  shorne
 * First commit of h323plus
 *
 *
 *
 *
 */

#ifndef __h235PLUGIN_H
#define __h235PLUGIN_H

#include <time.h>


#ifdef __cplusplus
extern "C" {
#endif

/////////////////////////////////////////////////////////////////////////////////
//
// h235 Plugins

#define PWLIB_PLUGIN_API_VERSION     0
#define	PLUGIN_H235_VERSION          1  


// Function calls
static const char H235_BuildClear[]       = "Build_Clear";
static const char H235_BuildCrypto[]      = "Build_Crypto";
static const char H235_FinaliseCrypto[]   = "Finalise_Crypto";
static const char H235_ValidateClear[]    = "Validate_Clear";
static const char H235_ValidateCrypto[]   = "Validate_Crypto";

// Settings direction
static const char GET_PLUGINH235_SETTINGS[]    = "get_h235_settings";
static const char SET_PLUGINH235_SETTINGS[]    = "set_h235_settings";

// Validation Constants
#define Pluginh235_Val_OK            0     ///< Security parameters and Msg are ok, no security attacks
#define Pluginh235_Val_Absent        1     ///< Security parameters are expected but absent
#define Pluginh235_Val_Error         2     ///< Security parameters are present but incorrect
#define Pluginh235_Val_InvalidTime   3     ///< Security parameters indicate peer has bad real time clock
#define Pluginh235_Val_BadPassword   4     ///< Security parameters indicate bad password in token
#define Pluginh235_Val_ReplyAttack   5     ///< Security parameters indicate an attack was made
#define Pluginh235_Val_Disabled      6     ///< Security is disabled by local system


// Application Constants
#define Pluginh235_App_GKAdmission       0     ///< To Be Used for GK Admission
#define Pluginh235_App_EPAuthentication  1     ///< To Be Used for EP Authentication
#define Pluginh235_App_LRQOnly           2     ///< To Be Used for Location Request Authentication
#define Pluginh235_App_AnyApplication    3     ///< To Be Used for Any Application


// Settings constants
static const char Pluginh235_Set_Application[]             ="Application";
static const char Pluginh235_Set_UseGkAndEpIdentifiers[]   ="UseGkAndEpIdentifiers";
static const char Pluginh235_Set_IsSecuredPDU[]            ="IsSecuredPDU";
static const char Pluginh235_Set_IsSecuredSignalPDU[]      ="IsSecuredSignalPDU";
static const char Pluginh235_Set_IsActive[]                ="IsActive";
static const char Pluginh235_Set_RemoteId[]                ="RemoteId";       
static const char Pluginh235_Set_LocalId[]                 ="LocalId";   
static const char Pluginh235_Set_Password[]                ="Password";
static const char Pluginh235_Set_TimestampGracePeriod[]    ="TimestampGracePeriod";

// Ras Message constants
#define Pluginh235_msg_gatekeeperRequest           0
#define Pluginh235_msg_gatekeeperConfirm	       1
#define Pluginh235_msg_gatekeeperReject            2
#define Pluginh235_msg_registrationRequest         3
#define Pluginh235_msg_registrationConfirm         4
#define Pluginh235_msg_registrationReject          5
#define Pluginh235_msg_unregistrationRequest       6
#define Pluginh235_msg_unregistrationConfirm       7
#define Pluginh235_msg_unregistrationReject        8
#define Pluginh235_msg_admissionRequest            9
#define Pluginh235_msg_admissionConfirm            10
#define Pluginh235_msg_admissionReject             11
#define Pluginh235_msg_bandwidthRequest            12
#define Pluginh235_msg_bandwidthConfirm            13
#define Pluginh235_msg_bandwidthReject             14
#define Pluginh235_msg_disengageRequest            15
#define Pluginh235_msg_disengageConfirm            16
#define Pluginh235_msg_disengageReject             17
#define Pluginh235_msg_locationRequest             18
#define Pluginh235_msg_locationConfirm             19
#define Pluginh235_msg_locationReject              20
#define Pluginh235_msg_infoRequest                 21
#define Pluginh235_msg_infoRequestResponse         22

// Signal message constants
#define Pluginh235_msg_setup                       0
#define Pluginh235_msg_callProceeding              1
#define Pluginh235_msg_connect                     2
#define Pluginh235_msg_alerting                    3
#define Pluginh235_msg_information                 4
#define Pluginh235_msg_releaseComplete             5
#define Pluginh235_msg_facility                    6
#define Pluginh235_msg_progress                    7
#define Pluginh235_msg_empty                       8
#define Pluginh235_msg_status                      9
#define Pluginh235_msg_statusInquiry              10
#define Pluginh235_msg_setupAcknowledge           11
#define Pluginh235_msg_notify                     12


/////////////////////////////////////////////////////////////////////////////////

enum Pluginh235_Flags {
 // Token type
  Pluginh235_TokenTypeMask           = 0x000f,
  Pluginh235_TokenTypeclear          = 0x0000,
  Pluginh235_TokenTypecrypto         = 0x0001,
  Pluginh235_TokenTypeBoth           = 0x0002,

 // CryptoToken Style
  Pluginh235_TokenStyleMask          = 0x0010,
  Pluginh235_TokenStyleHash          = 0x0010,
  Pluginh235_TokenStyleSigned        = 0x0011,
  Pluginh235_TokenStyleEncrypted     = 0x0012
};

struct Pluginh235_Definition;

struct Pluginh235_ControlDefn {
  const char * name;
  int (*control)(const struct Pluginh235_Definition * def, void * context, 
                 const char * parm, const char * value);
};

/////////////////////////////////////////////////////////////////////////////////

struct Pluginh235_information {
  // start of version 1 fields
  time_t timestamp;                     // codec creation time and date - obtain with command: date -u "+%c = %s"

  const char * sourceAuthor;            // source code author
  const char * sourceVersion;           // source code version
  const char * sourceEmail;             // source code email contact information
  const char * sourceURL;               // source code web site
  const char * sourceCopyright;         // source code copyright
  const char * sourceLicense;           // source code license
  unsigned char sourceLicenseCode;      // source code license

  const char * h235Description;          // h235 description
  const char * h235Manufacturer;         // h235 Manufacturer
  const char * h235Model;                // h235 Model
  const char * h235Email;		         // h235 email contact information
  const char * h235URL;                  // h235 Manufacturer web site
  // end of version 1 fields
};

struct Pluginh235_Definition {
  unsigned int version;			               // codec structure version

  // start of version 1 fields
  struct Pluginh235_information * info;                // license information

  unsigned int flags;                                  // Pluginh235_Flags,        
  const char * desc;    		                       // text decription
  const char * identifier;                             // OID Identifier
  const void * userData;                               // user data value

  void *(*createh235)(const struct Pluginh235_Definition * def);	               // create h235 
  void (*destroyh235)(const struct Pluginh235_Definition * def);                   // destroy h235
  int (*h235function)(const struct Pluginh235_Definition * def,  void * context,   // do H235 function
	                  const char * function,const void * data, unsigned * dataLen,
					  const void * raw, unsigned * rawLen);

  struct Pluginh235_ControlDefn * h235Controls;                                     // do control function

  // end of version 1 fields
};

typedef struct Pluginh235_Definition * (* Pluginh235_Geth235Function)(unsigned int *, unsigned int);
typedef unsigned (* Pluginh235_GetAPIVersionFunction)();


//////////////////////////////////////////////////////////////////////////////
//
// Plugin Definition
//

// API version
#define PLUGIN_H235_API_VER_FN       PWLibPlugin_GetAPIVersion
#define PLUGIN_H235_API_VER_FN_STR   "PWLibPlugin_GetAPIVersion"

// Harware Input Device
#define PLUGIN_H235_GET_DEVICE_FN	Opalh235Plugin_GetDevice
#define PLUGIN_H235_GET_DEVICE_FN_STR "Opalh235Plugin_GetDevice"


#  define PLUGIN_H235_IMPLEMENT(name) \
PLUGIN_CODEC_DLL_API unsigned int PLUGIN_H235_API_VER_FN() \
{ return PWLIB_PLUGIN_API_VERSION; } \


#ifdef __cplusplus
};
#endif

#endif // __h235PLUGIN_H
