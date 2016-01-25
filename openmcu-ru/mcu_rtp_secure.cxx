/*
 * mcu_rtp_secure.cxx
 *
 * Copyright (C) 2013-2015 Andrey Burbovskiy, OpenMCU-ru, All Rights Reserved
 * Copyright (C) 2015 Konstantin Yeliseyev, OpenMCU-ru, All Rights Reserved
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
 *                  Konstantin Yeliseyev (kay27@bk.ru)
 *
 */


#include "precompile.h"
#include "mcu.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

#if MCUSIP_ZRTP
zrtp_zid_t zrtp_zid = { "MCU" };
zrtp_global_t *zrtp_global = NULL;
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////

#if MCUSIP_SRTP
BOOL SRTPError(err_status_t err, const char * fn, const char * file, int line)
{
  if(err == err_status_ok)
    return FALSE;
  if(!PTrace::CanTrace(STRP_TRACE_LEVEL))
    return TRUE;
  ostream & trace = PTrace::Begin(STRP_TRACE_LEVEL, file, line);
  trace << "SRTP\t" << fn << "() error code " << err << " - ";
  switch(err)
  {
    case err_status_fail :                 trace << "unspecified failure"; break;
    case err_status_bad_param :            trace << "unsupported parameter"; break;
    case err_status_alloc_fail :           trace << "couldn't allocate memory"; break;
    case err_status_dealloc_fail :         trace << "couldn't deallocate properly"; break;
    case err_status_init_fail :            trace << "couldn't initialize"; break;
    case err_status_terminus :             trace << "can't process as much data as requested"; break;
    case err_status_auth_fail :            trace << "authentication failure"; break;
    case err_status_cipher_fail :          trace << "cipher failure"; break;
    case err_status_replay_fail :          trace << "replay check failed (bad index)"; break;
    case err_status_replay_old :           trace << "replay check failed (index too old)"; break;
    case err_status_algo_fail :            trace << "algorithm failed test routine"; break;
    case err_status_no_such_op :           trace << "unsupported operation"; break;
    case err_status_no_ctx :               trace << "no appropriate context found"; break;
    case err_status_cant_check :           trace << "unable to perform desired validation"; break;
    case err_status_key_expired :          trace << "can't use key any more"; break;
    case err_status_socket_err :           trace << "error in use of socket"; break;
    case err_status_signal_err :           trace << "error in use POSIX signals"; break;
    case err_status_nonce_bad :            trace << "nonce check failed"; break;
    case err_status_read_fail :            trace << "couldn't read data"; break;
    case err_status_write_fail :           trace << "couldn't write data"; break;
    case err_status_parse_err :            trace << "error pasring data"; break;
    case err_status_encode_err :           trace << "error encoding data"; break;
    case err_status_semaphore_err :        trace << "error while using semaphores"; break;
    case err_status_pfkey_err :            trace << "error while using pfkey"; break;
    default :                              trace << "unknown error " << err;
  }
  trace << PTrace::End;
  return TRUE;
}
#endif // MCUSIP_SRTP

////////////////////////////////////////////////////////////////////////////////////////////////////

#if MCUSIP_ZRTP
BOOL ZRTPError(zrtp_status_t err, const char * fn, const char * file, int line)
{
  if(err == zrtp_status_ok)
    return FALSE;
  if(!PTrace::CanTrace(ZTRP_TRACE_LEVEL))
    return TRUE;
  ostream & trace = PTrace::Begin(ZTRP_TRACE_LEVEL, file, line);
  trace << "ZRTP\t" << fn << "() error code " << err << " - ";
  switch(err)
  {
    case zrtp_status_fail :                trace << "General, unspecified failure"; break;
    case zrtp_status_bad_param :           trace << "Wrong, unsupported parameter"; break;
    case zrtp_status_alloc_fail :          trace << "Fail allocate memory"; break;
    case zrtp_status_auth_fail :           trace << "SRTP authentication failure"; break;
    case zrtp_status_cipher_fail :         trace << "Cipher failure on RTP encrypt/decrypt"; break;
    case zrtp_status_algo_fail :           trace << "General Crypto Algorithm failure"; break;
    case zrtp_status_key_expired :         trace << "SRTP can't use key any longer"; break;
    case zrtp_status_buffer_size :         trace << "Input buffer too small"; break;
    case zrtp_status_drop :                trace << "Packet process DROP status"; break;
    case zrtp_status_open_fail :           trace << "Failed to open file/device"; break;
    case zrtp_status_read_fail :           trace << "Unable to read data from the file/stream"; break;
    case zrtp_status_write_fail :          trace << "Unable to write to the file/stream"; break;
    case zrtp_status_old_pkt :             trace << "SRTP packet is out of sliding window"; break;
    case zrtp_status_rp_fail :             trace << "RTP replay protection failed"; break;
    case zrtp_status_zrp_fail :            trace << "ZRTP replay protection failed"; break;
    case zrtp_status_crc_fail :            trace << "ZRTP packet CRC is wrong"; break;
    case zrtp_status_rng_fail :            trace << "Can't generate random value"; break;
    case zrtp_status_wrong_state :         trace << "Illegal operation in current state"; break;
    case zrtp_status_attack :              trace << "Attack detected"; break;
    case zrtp_status_notavailable :        trace << "Function is not available in current configuration"; break;
    default :                              trace << "unknown error " << err;
  }
  trace << PTrace::End;
  return TRUE;
}
#endif // MCUSIP_ZRTP

////////////////////////////////////////////////////////////////////////////////////////////////////

#if MCUSIP_ZRTP

static void zrtp_event_security(zrtp_stream_t *stream, zrtp_security_event_t event)
{
}

static void zrtp_event_protocol(zrtp_stream_t *stream, zrtp_protocol_event_t event)
{
  MCUSIP_RTP_UDP *rtp_session = (MCUSIP_RTP_UDP *)zrtp_stream_get_userdata(stream);
  if(!rtp_session)
    return;

  zrtp_session_info_t zrtp_session_info;
  if(ZRTP_ERROR(zrtp_session_get, (stream->session, &zrtp_session_info)))
    return;

  switch(event)
  {
    case ZRTP_EVENT_IS_SECURE:
    {
      if(zrtp_session_info.sas_is_ready)
      {
        zrtp_verified_set(zrtp_global, &stream->session->zid, &stream->session->peer_zid, (uint8_t)1);
        rtp_session->zrtp_secured = TRUE;
        rtp_session->zrtp_sas_token = stream->session->sas1.buffer;
      }
      if(rtp_session->zrtp_master && rtp_session->GetConnection())
      {
        // attach extended stream
        MCUSIP_RTP_UDP *video_rtp_session = (MCUSIP_RTP_UDP*)rtp_session->GetConnection()->GetSession(RTP_Session::DefaultVideoSessionID);
        if(video_rtp_session && video_rtp_session->transmitter_state == 1 && video_rtp_session->receiver_state == 1)
        {
          if(!ZRTP_ERROR(zrtp_stream_attach, (stream->session, &video_rtp_session->zrtp_stream)))
          {
            zrtp_stream_set_userdata(video_rtp_session->zrtp_stream, video_rtp_session);
            zrtp_stream_registration_start(video_rtp_session->zrtp_stream, video_rtp_session->ssrc);
            //zrtp_stream_start(video_rtp_session->zrtp_stream, video_rtp_session->ssrc);
            video_rtp_session->zrtp_initialised = TRUE;
          }
        }
      }
      break;
    }
    case ZRTP_EVENT_NO_ZRTP_QUICK:
    {
      rtp_session->zrtp_secured = FALSE;
      break;
    }
    case ZRTP_EVENT_IS_CLIENT_ENROLLMENT:
      break;
    case ZRTP_EVENT_USER_ALREADY_ENROLLED:
      break;
    case ZRTP_EVENT_NEW_USER_ENROLLED:
      break;
    case ZRTP_EVENT_USER_UNENROLLED :
      break;
    case ZRTP_EVENT_IS_PENDINGCLEAR:
      break;
    case ZRTP_EVENT_NO_ZRTP:
    {
      rtp_session->zrtp_secured = FALSE;
      break;
    }
    default:
      break;
  }
}

static int zrtp_on_send_packet(const zrtp_stream_t *stream, char *packet, unsigned int len)
{
  MCUSIP_RTP_UDP *session = (MCUSIP_RTP_UDP *)zrtp_stream_get_userdata(stream);
  if(!session)
    return zrtp_status_write_fail;

  if(session->GetDataSocketHandle() == -1)
    return zrtp_status_write_fail;

  PBYTEArray tmp((BYTE *)packet, len);
  RTP_DataFrame frame(tmp.GetSize());
  memcpy(frame.GetPointer(), tmp.GetPointer(), tmp.GetSize());
  frame.SetPayloadSize(len-frame.GetHeaderSize());

  if(!session->WriteDataZRTP(frame))
    return zrtp_status_write_fail;

  return zrtp_status_ok;
}

void zrtp_logger(int level, char *buffer, int len, int offset)
{
  if(!PTrace::CanTrace(ZTRP_TRACE_LEVEL))
    return;
  PTRACE(ZTRP_TRACE_LEVEL, buffer);
  cout << setw(8) << PTime() - PProcess::Current().GetStartTime() << " " << buffer;
}

void sip_zrtp_init()
{
  if(!zrtp_global)
  {
    zrtp_log_set_log_engine((zrtp_log_engine *)zrtp_logger);
    zrtp_log_set_level(3);

    zrtp_config_t zrtp_config;
    zrtp_config_defaults(&zrtp_config);

    strcpy(zrtp_config.client_id, "MCU");
    zrtp_config.is_mitm = 1;
    // ZRTP_LICENSE_MODE_ACTIVE // ZRTP_LICENSE_MODE_UNLIMITED // ZRTP_LICENSE_MODE_PASSIVE
    zrtp_config.lic_mode = ZRTP_LICENSE_MODE_ACTIVE;

    PString zrtp_cache_path = PString(SERVER_LOGS)+PString(PATH_SEPARATOR)+"zrtp.cache";
    ZSTR_SET_EMPTY(zrtp_config.def_cache_path);
    if(zrtp_cache_path.GetLength() < zrtp_config.def_cache_path.max_length)
      zrtp_zstrcpyc(ZSTR_GV(zrtp_config.def_cache_path), zrtp_cache_path);

    zrtp_config.cb.event_cb.on_zrtp_protocol_event = zrtp_event_protocol;
    zrtp_config.cb.misc_cb.on_send_packet = zrtp_on_send_packet;
    zrtp_config.cb.event_cb.on_zrtp_security_event = zrtp_event_security;

    // initialize libzrtp.
    ZRTP_ERROR(zrtp_init, (&zrtp_config, &zrtp_global));
  }
}

#endif // MCUSIP_ZRTP

////////////////////////////////////////////////////////////////////////////////////////////////////

#if MCUSIP_SRTP

BOOL SipSRTP::Init(const PString & crypto, const PString & key_str)
{
  if(!SetCryptoPolicy(crypto))
    return FALSE;
  if(!SetKey(key_str))
    return FALSE;

  BYTE key_salt[32];
  // This is all a bit vague in docs for libSRTP. Had to look into source to figure it out.
  memcpy(key_salt, m_key, std::min(m_key_length, m_key.GetSize()));
  memcpy(&key_salt[m_key_length], m_salt, std::min(m_salt_length, m_salt.GetSize()));
  //append_salt_to_key(key_salt, std::min(m_key_length, m_key.GetSize()),
  //                             m_salt.GetPointer(), m_salt.GetSize());
  m_policy.key = key_salt;
  m_policy.ssrc.value = 0;
  m_policy.ssrc.type = ssrc_any_inbound;

  if(SRTP_ERROR(srtp_create, (&m_session, NULL)))
    return FALSE;

  if(SRTP_ERROR(srtp_add_stream, (m_session, &m_policy)))
    return FALSE;

  PTRACE(1, "SRTP\tCreate SRTP session for direction");
  return TRUE;
}

BOOL SipSRTP::SetCryptoPolicy(const PString & type)
{
  if(type == AES_CM_128_HMAC_SHA1_80)
  {
    m_profile = srtp_profile_aes128_cm_sha1_80;
    m_key_bits = 128;
    m_salt_bits = 112;
    m_key_length = srtp_profile_get_master_key_length(m_profile);
    m_salt_length = srtp_profile_get_master_salt_length(m_profile);
    crypto_policy_set_aes_cm_128_hmac_sha1_80(&m_policy.rtp);
    return TRUE;
  }
  else if(type == AES_CM_128_HMAC_SHA1_32)
  {
    m_profile = srtp_profile_aes128_cm_sha1_32;
    m_key_bits = 128;
    m_salt_bits = 32;
    m_key_length = srtp_profile_get_master_key_length(m_profile);
    m_salt_length = srtp_profile_get_master_salt_length(m_profile);
    crypto_policy_set_aes_cm_128_hmac_sha1_32(&m_policy.rtp);
    return TRUE;
  }
  PTRACE(1, "SRTP\tunknown policy!");
  return FALSE;
}

BOOL SipSRTP::SetKey(const PString & key_str)
{

  PBYTEArray key_salt;
  if(!PBase64::Decode(key_str, key_salt))
  {
    PTRACE(1, "SRTP\tInvalid base64-decoded key/salt string \"" << key_str << '"');
    return FALSE;
  }
  return SetKey(key_salt);
}

BOOL SipSRTP::SetKey(const PBYTEArray & key_salt)
{
  if(key_salt.GetSize() < m_key_length+m_salt_length)
  {
    PTRACE(1, "SRTP\tIncorrect key/salt size (" << key_salt.GetSize() << ") bytes)");
    return FALSE;
  }
  m_key = PBYTEArray(key_salt, m_key_length);
  m_salt = PBYTEArray(key_salt+m_key_length, key_salt.GetSize()-m_key_length);
  return TRUE;
}

void SipSRTP::SetRandomKey()
{
  SetKey(srtp_get_random_keysalt());
}

PString SipSRTP::GetKey() const
{
  PBYTEArray key_salt = PBYTEArray(m_key.GetSize()+m_salt.GetSize());
  memcpy(key_salt.GetPointer(), m_key, m_key.GetSize());
  memcpy(key_salt.GetPointer()+m_key.GetSize(), m_salt, m_salt.GetSize());
  return PBase64::Encode(key_salt);
}

#endif // MCUSIP_SRTP

////////////////////////////////////////////////////////////////////////////////////////////////////

void sip_rtp_init()
{
#if MCUSIP_SRTP
  PTRACE(2,"MCU\tBegin of srtp_init()");
  srtp_init();
  PTRACE(2,"MCU\tEnd of srtp_init()");
#endif
#if MCUSIP_ZRTP
  sip_zrtp_init();
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void sip_rtp_shutdown()
{
#if MCUSIP_SRTP
//  srtp_shutdown();
#endif
#if MCUSIP_ZRTP
  zrtp_down(zrtp_global);
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////

PString srtp_get_random_keysalt()
{
#if MCUSIP_SRTP
  PBYTEArray key_salt(30);
  // set key to random value
  crypto_get_random(key_salt.GetPointer(), 30);
  return PBase64::Encode(key_salt);
#else
  return "";
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////

