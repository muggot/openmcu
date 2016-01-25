/*
 * mcu_rtp_secure.h
 *
 * Copyright (C) 2013-2015 Andrey Burbovskiy, OpenMCU-ru, All Rights Reserved
 *
 * The Initial Developer of the Original Code is Andrey Burbovskiy (andrewb@yandex.ru), All Rights Reserved
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
 * Contributor(s):  Andrey Burbovskiy (andrewb@yandex.ru)
 *
 */

#include "precompile.h"

#ifndef _MCU_RTP_SECURE_H
#define _MCU_RTP_SECURE_H

#include "utils.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

static const PString AES_CM_128_HMAC_SHA1_80("AES_CM_128_HMAC_SHA1_80");
static const PString AES_CM_128_HMAC_SHA1_32("AES_CM_128_HMAC_SHA1_32");

void sip_rtp_init();
void sip_rtp_shutdown();

PString srtp_get_random_keysalt();

#if MCUSIP_ZRTP
extern zrtp_zid_t zrtp_zid;
extern zrtp_global_t *zrtp_global;
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////

#if MCUSIP_SRTP
#define STRP_TRACE_LEVEL 6
BOOL SRTPError(err_status_t err, const char * fn, const char * file, int line);
#define SRTP_ERROR(fn, param) SRTPError(fn param, #fn, __FILE__, __LINE__)
//#define SRTP_ERROR(fn, param) ((fn param) != err_status_ok)
#endif

#if MCUSIP_ZRTP
#define ZTRP_TRACE_LEVEL 6
BOOL ZRTPError(zrtp_status_t err, const char * fn, const char * file, int line);
#define ZRTP_ERROR(fn, param) ZRTPError(fn param, #fn, __FILE__, __LINE__)
//#define ZRTP_ERROR(fn, param) ((fn param) != zrtp_status_ok)
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////

#if MCUSIP_SRTP
class SipSRTP
{
  public:
    SipSRTP()
    {
      memset(&m_policy, 0, sizeof(m_policy));
      m_policy.next = NULL;
      SetCryptoPolicy(AES_CM_128_HMAC_SHA1_80);
    };
    ~SipSRTP()
    {
      srtp_dealloc(m_session);
    };

    BOOL Init(const PString & crypto, const PString & key_str);
    BOOL SetCryptoPolicy(const PString & crypto);
    BOOL SetKey(const PString & key_str);
    BOOL SetKey(const PBYTEArray & key_salt);
    void SetRandomKey();
    PString GetKey() const;
    srtp_t & GetSession() { return m_session; };

  protected:
    srtp_profile_t    m_profile;
    srtp_t            m_session;
    srtp_policy_t     m_policy;

    PBYTEArray        m_key;
    PBYTEArray        m_salt;
    PINDEX            m_key_bits;
    PINDEX            m_key_length;
    PINDEX            m_salt_bits;
    PINDEX            m_salt_length;
};
#endif // MCUSIP_SRTP

////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // _MCU_RTP_SECURE_H
