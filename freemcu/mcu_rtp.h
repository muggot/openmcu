
#ifndef _MCU_RTP_H
#define _MCU_RTP_H

#include <ptlib.h>
#include <ptclib/cypher.h>

#include <channels.h>
#include <h323rtp.h>
#include <h323con.h>

#include "config.h"

#if MCUSIP_SRTP
  #include "srtp.h"
#endif

#if MCUSIP_ZRTP
  #include "zrtp.h"
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////

#define STRP_TRACING 1
#define STRP_TRACING_LEVEL 6

#define ZTRP_TRACING 1
#define ZTRP_TRACING_LEVEL 6

////////////////////////////////////////////////////////////////////////////////////////////////////

static const PString AES_CM_128_HMAC_SHA1_80("AES_CM_128_HMAC_SHA1_80");
static const PString AES_CM_128_HMAC_SHA1_32("AES_CM_128_HMAC_SHA1_32");

class MCUSipConnnection;
class SipRTP_UDP;

void sip_rtp_init();
void sip_rtp_shutdown();

PString srtp_get_random_keysalt();

////////////////////////////////////////////////////////////////////////////////////////////////////

class MCU_RTPChannel  : public H323_RTPChannel
{
  PCLASSINFO(MCU_RTPChannel, H323_RTPChannel);
  public:
    MCU_RTPChannel(H323Connection & connection, const H323Capability & capability, Directions direction, RTP_Session & rtp)
      : H323_RTPChannel(connection, capability, direction, rtp) { };
    ~MCU_RTPChannel() { };
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class SipRTPChannel  : public MCU_RTPChannel
{
  PCLASSINFO(SipRTPChannel, MCU_RTPChannel);

  public:
    SipRTPChannel(H323Connection & connection, const H323Capability & capability, Directions direction, RTP_Session & rtp);
    ~SipRTPChannel();

    virtual BOOL Start();
    virtual void CleanUpOnTermination();

    virtual BOOL ReadFrame(DWORD & rtpTimestamp, RTP_DataFrame & frame);
    virtual BOOL WriteFrame(RTP_DataFrame & frame);

};

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

class MCU_RTP_UDP : public RTP_UDP
{
  public:
    MCU_RTP_UDP(
#ifdef H323_RTP_AGGREGATE
      PHandleAggregator * aggregator,
#endif
      unsigned id, BOOL remoteIsNat = FALSE
               )
                : RTP_UDP(
#ifdef H323_RTP_AGGREGATE
      aggregator,
#endif
      id, remoteIsNat
                         )
    {
      zrtp_secured = FALSE;
      srtp_secured = FALSE;
    };

    BOOL           zrtp_secured;
    PString        zrtp_sas_token;
    BOOL           srtp_secured;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class SipRTP_UDP : public MCU_RTP_UDP
{
  public:
    SipRTP_UDP(
#ifdef H323_RTP_AGGREGATE
      PHandleAggregator * aggregator,
#endif
      unsigned id, BOOL remoteIsNat = FALSE
              );
    ~SipRTP_UDP();

    virtual BOOL ReadData(RTP_DataFrame & frame, BOOL loop);
    virtual BOOL WriteData(RTP_DataFrame & frame);
    virtual BOOL PreWriteData(RTP_DataFrame & frame);
    virtual BOOL PostWriteData(RTP_DataFrame & frame);

    virtual SendReceiveStatus OnSendData(RTP_DataFrame & frame);
    virtual SendReceiveStatus OnReceiveData(const RTP_DataFrame & frame, const RTP_UDP & rtp);

    BOOL CreateSRTP(int dir, const PString & crypto, const PString & key_str);
    BOOL CreateZRTP();

    BOOL WriteDataZRTP(RTP_DataFrame & frame);

    void SetConnection(MCUH323Connection *_conn) { conn = _conn; };
    MCUH323Connection * GetConnection() { return conn; };

    void SetState(int dir, int state);
    void SetMaster(BOOL master) { zrtp_master = master; };

    BOOL                    zrtp_initialised;
    BOOL                    zrtp_master;
    uint32_t                ssrc;
    int                     transmitter_state; // 0-stop, 1-running
    int                     receiver_state;    // 0-stop, 1-running

#if MCUSIP_ZRTP
    zrtp_profile_t        * zrtp_profile;
    zrtp_session_t        * zrtp_session;
    zrtp_stream_t         * zrtp_stream;
    //zrtp_session_info_t    zrtp_session_info;
#endif

  protected:
    MCUH323Connection * conn;
#if MCUSIP_SRTP
    SipSRTP               * srtp_read;
    SipSRTP               * srtp_write;
#endif
};

////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // _MCU_RTP_H
