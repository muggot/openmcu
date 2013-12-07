/*
 * h235pluginmgr.cxx
 *
 * h235 Implementation for the h323plus library.
 *
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
 *
 * Contributor(s): ______________________________________.
 *
 * $Log: h235pluginmgr.cxx,v $
 * Revision 1.3  2007/11/02 03:58:30  shorne
 * fixed missed warning on last commit
 *
 * Revision 1.2  2007/11/01 22:26:27  shorne
 * fix linux compile warnings
 *
 * Revision 1.1  2007/08/06 20:51:05  shorne
 * First commit of h323plus
 *
 *
 *
*/

#ifdef __GNUC__
#pragma implementation "h235pluginmgr.h"
#endif

#include <ptlib.h>
#include <ptclib/asner.h>

#include "h235pluginmgr.h"
#include "h235plugin.h"
#include <h225.h>
#include <h235.h>

#ifdef _MSC_VER
#pragma warning(disable:4700)
#endif


static int PluginControl(Pluginh235_Definition * h235, 
                                       void * context,
                                 const char * name,
                                 const char * parm, 
							     const char * val)
{
  Pluginh235_ControlDefn * controls = h235->h235Controls;
  if (controls == NULL)
    return 0;

  while (controls->name != NULL) {
    if (strcmp(controls->name, name) == 0)
      return (*controls->control)(h235, context, parm, val);
    controls++;
  }

  return 0;
}

H235PluginAuthenticator::H235PluginAuthenticator(Pluginh235_Definition * _def)
  : def(_def)
{
  switch (def->flags & Pluginh235_TokenTypeMask) {
	case Pluginh235_TokenTypecrypto:  
            switch (def->flags & Pluginh235_TokenTypeMask) {
              case Pluginh235_TokenStyleHash:
				type = H235_AuthenticationMechanism::e_pwdHash;
                break;
              case Pluginh235_TokenStyleSigned:
                type = H235_AuthenticationMechanism::e_certSign;
                break;
              case Pluginh235_TokenStyleEncrypted:
                type = H235_AuthenticationMechanism::e_pwdSymEnc;
                break;
              default:
                type = H235_AuthenticationMechanism::e_nonStandard;
             }
            break;
	case Pluginh235_TokenTypeclear: 
	        type = H235_AuthenticationMechanism::e_authenticationBES;
            break;
	default:
             type = H235_AuthenticationMechanism::e_nonStandard;
	    break;
  }	

   SetTimestampGracePeriod(2*60*60+10);  /// 2hrs 10min
}

H235_ClearToken * H235PluginAuthenticator::CreateClearToken()
{
	BYTE data;
	unsigned dataLen;
	int ret = (*def->h235function)(def, NULL, H235_BuildClear,
								  &data, &dataLen,NULL,0);

	if (ret == 0)
		return NULL;

	PPER_Stream raw(&data,dataLen);
    H235_ClearToken * token = new H235_ClearToken;
	token->Decode(raw);
    return token;
}

H225_CryptoH323Token * H235PluginAuthenticator::CreateCryptoToken()
{
	BYTE data;
	unsigned dataLen;
	int ret = (*def->h235function)(def, NULL, H235_BuildCrypto,
								  &data, &dataLen,NULL,0);
	if (ret == 0)
		return NULL;

	PPER_Stream raw(&data,dataLen);
    H225_CryptoH323Token * token = new H225_CryptoH323Token;
	token->Decode(raw);
    return token;
}

BOOL H235PluginAuthenticator::Finalise(PBYTEArray & rawPDU)
{
   BYTE * data = rawPDU.GetPointer();
   unsigned dataLen = rawPDU.GetSize();
   int ret = (*def->h235function)(def, NULL, H235_FinaliseCrypto, data, &dataLen,NULL,0);
   
   if (ret == 0)
	   return FALSE;
   
   PBYTEArray newPDU(data,dataLen);
   rawPDU = newPDU;
   return TRUE;
}

H235Authenticator::ValidationResult H235PluginAuthenticator::ValidateClearToken(const H235_ClearToken & clearToken)
{
	PPER_Stream enc;
	clearToken.Encode(enc);

	BYTE * data = enc.GetPointer();
	unsigned dataLen = enc.GetSize();

	int ret = (*def->h235function)(def, NULL, H235_ValidateClear,
								  data, &dataLen,NULL,0);

    return (H235Authenticator::ValidationResult)ret;
}

H235Authenticator::ValidationResult H235PluginAuthenticator::ValidateCryptoToken(const H225_CryptoH323Token & cryptoToken,
                                                                                 const PBYTEArray & rawPDU)
{
   	PPER_Stream enc;
	cryptoToken.Encode(enc);

	BYTE * data = enc.GetPointer();
	unsigned dataLen = enc.GetSize();
	const BYTE * raw = rawPDU;
	unsigned rawLen = rawPDU.GetSize();

	int ret = (*def->h235function)(def, NULL, H235_ValidateClear,
								  data, &dataLen, raw, &rawLen);

    return (H235Authenticator::ValidationResult)ret;
}

BOOL H235PluginAuthenticator::IsCapability(const H235_AuthenticationMechanism & mechanism,
                                           const PASN_ObjectId & algorithmOID)
{
  return ((mechanism.GetTag() == type) && (algorithmOID.AsString() == def->identifier));
}

BOOL H235PluginAuthenticator::SetCapability(H225_ArrayOf_AuthenticationMechanism & mechanisms,
                                            H225_ArrayOf_PASN_ObjectId & algorithmOIDs)
{
  return AddCapability(type, def->identifier,mechanisms, algorithmOIDs);
}

BOOL H235PluginAuthenticator::UseGkAndEpIdentifiers() const
{
   return (PluginControl(def, NULL,GET_PLUGINH235_SETTINGS, Pluginh235_Set_UseGkAndEpIdentifiers, NULL)); 
}

BOOL H235PluginAuthenticator::IsSecuredPDU(unsigned rasPDU,BOOL received) const
{
   return (PluginControl(def, NULL,GET_PLUGINH235_SETTINGS, Pluginh235_Set_IsSecuredPDU, PString(rasPDU))); 
}

BOOL H235PluginAuthenticator::IsSecuredSignalPDU(unsigned signalPDU,
                                                 BOOL received) const
{
   return (PluginControl(def, NULL,GET_PLUGINH235_SETTINGS, Pluginh235_Set_IsSecuredSignalPDU, PString(signalPDU)));
}

BOOL H235PluginAuthenticator::IsActive() const
{
   return (PluginControl(def, NULL,GET_PLUGINH235_SETTINGS, Pluginh235_Set_IsActive, NULL));
}

const PString & H235PluginAuthenticator::GetRemoteId() const
{
   return remoteId;
}

void H235PluginAuthenticator::SetRemoteId(const PString & id)
{
   remoteId = id;
   PluginControl(def, NULL,SET_PLUGINH235_SETTINGS, Pluginh235_Set_RemoteId, remoteId); 
}

const PString & H235PluginAuthenticator::GetLocalId() const
{
   return localId;
}

void H235PluginAuthenticator::SetLocalId(const PString & id)
{
   localId = id;
   PluginControl(def, NULL,SET_PLUGINH235_SETTINGS, Pluginh235_Set_LocalId, id); 
}

const PString & H235PluginAuthenticator::GetPassword() const
{
   return password;
}
    
void H235PluginAuthenticator::SetPassword(const PString & pw)
{
   password = pw;
   PluginControl(def, NULL,SET_PLUGINH235_SETTINGS, Pluginh235_Set_Password, password); 
}

int H235PluginAuthenticator::GetTimestampGracePeriod() const
{
   return timestampGracePeriod; 
}
    
void H235PluginAuthenticator::SetTimestampGracePeriod(int grace)
{
   timestampGracePeriod = grace;
   PluginControl(def, NULL,SET_PLUGINH235_SETTINGS, Pluginh235_Set_TimestampGracePeriod, PString(timestampGracePeriod)); 
}

H235Authenticator::Application H235PluginAuthenticator::GetApplication()
{
    return (H235Authenticator::Application)PluginControl(def, NULL,GET_PLUGINH235_SETTINGS, 
		      Pluginh235_Set_Application, NULL) ;
}

/////////////////////////////////////////////////////////////////////////////////////

h235PluginDeviceManager::h235PluginDeviceManager(PPluginManager * _pluginMgr)
 : PPluginModuleManager(PLUGIN_H235_GET_DEVICE_FN_STR, _pluginMgr)
{
    PTRACE(3, "H323h235\tPlugin loading h235 "); 

    // cause the plugin manager to load all dynamic plugins
    pluginMgr->AddNotifier(PCREATE_NOTIFIER(OnLoadModule), TRUE);
}
    
h235PluginDeviceManager::~h235PluginDeviceManager()
{

}

void h235PluginDeviceManager::OnLoadPlugin(PDynaLink & dll, INT code)
{
  PDynaLink::Function geth235;
  if (!dll.GetFunction(PString(signatureFunctionName), geth235))
  {
    PTRACE(3, "H323h235\tPlugin DLL " << dll.GetName() << " is not a H235 plugin");	  
    return;
  }

  unsigned int count;
  Pluginh235_Definition * h235 = (*((Pluginh235_Geth235Function)geth235))(&count, PLUGIN_H235_VERSION);
  if (h235 == NULL || count == 0) {
    PTRACE(3, "H323PLUGIN\tPlugin DLL " << dll.GetName() << " contains no H235 definitions" );
    return;
  } 

  PTRACE(3, "H323PLUGIN\tLoading H235 plugin  " << dll.GetName() );

  switch (code) {

    // plugin loaded
    case 0:
      Registerh235(count, h235);
      break;

    // plugin unloaded
    case 1:
      Unregisterh235(count, h235);
      break;

    default:
      break;
  }
}

void h235PluginDeviceManager::OnShutdown()
{
  // unregister the H235 plugin 
    h235Factory::UnregisterAll();
}

void h235PluginDeviceManager::Bootstrap()
{

}

BOOL h235PluginDeviceManager::Registerh235(unsigned int count, void * _h235List)
{
  Pluginh235_Definition * h235List = (Pluginh235_Definition *)_h235List;

  unsigned i;
  for (i = 0; i < count; i++) {
     CreateH235Authenticator(&h235List[i]);
  }

  return TRUE;
}

BOOL h235PluginDeviceManager::Unregisterh235(unsigned int /*count*/, void * /*_h235List*/)
{

	return FALSE;
}

static PString Createh235Name(Pluginh235_Definition * h235, unsigned int h235type)
{
  PString str;

  switch (h235type) {
      case Pluginh235_TokenStyleHash:
 	     str  = h235->desc + PString(" {hash}");
         break;
      case Pluginh235_TokenStyleSigned:
 	     str  = h235->desc + PString(" {sign}");
         break;
      case Pluginh235_TokenStyleEncrypted:
 	     str  = h235->desc + PString(" {enc}");
         break;
      case Pluginh235_TokenTypeclear: 
 	     str  = h235->desc + PString(" {clear}");
         break;	    
      default:
         str = h235->desc;
  }

  return str;
}

void h235PluginDeviceManager::CreateH235Authenticator(Pluginh235_Definition * h235)
{
  // make sure all non-timestamped codecs have the same concept of "now"
  static time_t mediaNow = time(NULL);

  // deal with codec having no info, or timestamp in future
  time_t timeStamp = h235->info == NULL ? mediaNow : h235->info->timestamp;
  if (timeStamp > mediaNow)
    timeStamp = mediaNow;

// Authenticator Name
  PString h235Name = PString(); 
  H235PluginAuthenticator * auth = NULL;


// Type of h235 Plugin
  switch (h235->flags & Pluginh235_TokenTypeMask) {
	case Pluginh235_TokenTypecrypto:  
            switch (h235->flags & Pluginh235_TokenTypeMask) {
              case Pluginh235_TokenStyleHash:
		        h235Name = Createh235Name(h235,Pluginh235_TokenStyleHash);
                break;
              case Pluginh235_TokenStyleSigned:
		        h235Name = Createh235Name(h235,Pluginh235_TokenStyleSigned);
                break;
              case Pluginh235_TokenStyleEncrypted:
		        h235Name = Createh235Name(h235,Pluginh235_TokenStyleEncrypted);
                break;
              default:
                h235Name = h235->desc;
             }
            break;
	case Pluginh235_TokenTypeclear: 
	    h235Name = Createh235Name(h235, Pluginh235_TokenTypeclear);
            break;
	default:
            h235Name = h235->desc;
	    break;
  }	
   		
  auth = new H235PluginAuthenticator(h235);
  auth->SetName(h235Name);
		  

  if (auth != NULL)
     h235Factory::Register(h235Name, auth);
}

#ifdef _MSC_VER
#pragma warning(default:4700)
#endif
