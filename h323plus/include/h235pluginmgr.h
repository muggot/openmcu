/*
 * h235pluginmgr.h
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
 * $Log: h235pluginmgr.h,v $
 * Revision 1.1  2007/08/06 20:50:49  shorne
 * First commit of h323plus
 *
 *
 *
*/

#include <ptlib.h>
#include <ptlib/pluginmgr.h>
#include <h235auth.h>

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef _MSC_VER
#pragma warning(disable:4100)
#endif

//////////////////////////////////////////////////////////////////////////////////////

struct Pluginh235_Definition;
class H235PluginAuthenticator : public H235Authenticator
{
    PCLASSINFO(H235PluginAuthenticator, H235Authenticator);

  public:

    H235PluginAuthenticator(Pluginh235_Definition * _def);

    H235_ClearToken * CreateClearToken();
    H225_CryptoH323Token * CreateCryptoToken();

    BOOL Finalise(
      PBYTEArray & rawPDU
    );

	const char * GetName() const { return h235name; };
	void SetName(PString & name) { h235name = name; };

    ValidationResult ValidateClearToken(
      const H235_ClearToken & clearToken
    );

    ValidationResult ValidateCryptoToken(
      const H225_CryptoH323Token & cryptoToken,
      const PBYTEArray & rawPDU
    );

    BOOL IsCapability(
      const H235_AuthenticationMechanism & mechansim,
      const PASN_ObjectId & algorithmOID
    );

    BOOL SetCapability(
      H225_ArrayOf_AuthenticationMechanism & mechansims,
      H225_ArrayOf_PASN_ObjectId & algorithmOIDs
    );

    BOOL UseGkAndEpIdentifiers() const;

    BOOL IsSecuredPDU(
      unsigned rasPDU,
      BOOL received
    ) const;

    BOOL IsSecuredSignalPDU(
      unsigned signalPDU,
      BOOL received
    ) const;

    BOOL IsActive() const;

    const PString & GetRemoteId() const;
    void SetRemoteId(const PString & id);

    const PString & GetLocalId() const;
    void SetLocalId(const PString & id);

    const PString & GetPassword() const;
    void SetPassword(const PString & pw);

    int GetTimestampGracePeriod() const;
    void SetTimestampGracePeriod(int grace);


    Application GetApplication(); 

protected:
	PString h235name;
	unsigned type;
    Pluginh235_Definition * def; 
};



/////////////////////////////////////////////////////////////////////////////////////

class h235PluginDeviceManager : public PPluginModuleManager
{
  PCLASSINFO(h235PluginDeviceManager, PPluginModuleManager);
  public:
    h235PluginDeviceManager(PPluginManager * pluginMgr = NULL);
    ~h235PluginDeviceManager();

    void OnLoadPlugin(PDynaLink & dll, INT code);

    virtual void OnShutdown();

    static void Bootstrap();

    virtual BOOL Registerh235(unsigned int count, void * _h235List);
    virtual BOOL Unregisterh235(unsigned int count, void * _h235List);

    void CreateH235Authenticator(Pluginh235_Definition * h235authenticator);

};

static PFactory<PPluginModuleManager>::Worker<h235PluginDeviceManager> h323PluginCodecManagerFactory("h235PluginDeviceManager", true);

///////////////////////////////////////////////////////////////////////////////

typedef PFactory<H235Authenticator> h235Factory;

#define H235_REGISTER(cls, h235Name)   static h235Factory::Worker<cls> cls##Factory(h235Name, true); \

#define H235_DEFINE_AUTHENTICATOR(cls, h235Name, fmtName) \
class cls : public H235PluginAuthenticator { \
  public: \
    cls() : H235PluginAuthenticator() { } \
    PString GetName() const \
    { return fmtName; } \
}; \
 H235_REGISTER(cls, capName) \

/////////////////////////////////////////////////////////////////////////////

