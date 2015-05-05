
#include "precompile.h"

#ifndef _MCU_RTP_H
#define _MCU_RTP_H

#include "utils.h"
#include "mcu_rtp_cache.h"
#include "mcu_rtp_secure.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

#define	MAX_PAYLOAD_TYPE_MISMATCHES 8
#define RTP_TRACE_DISPLAY_RATE 16000 // 2 seconds

////////////////////////////////////////////////////////////////////////////////////////////////////

class MCU_RTPChannel : public H323_RTPChannel
{
  PCLASSINFO(MCU_RTPChannel, H323_RTPChannel);
  public:
    MCU_RTPChannel(H323Connection & connection, const H323Capability & capability, Directions direction, RTP_Session & rtp);
    ~MCU_RTPChannel();

    virtual BOOL Open();
    virtual BOOL Start();
    virtual void CleanUpOnTermination();
    virtual void Receive();
    virtual void Transmit();

    virtual BOOL WriteFrame(RTP_DataFrame & frame);
    virtual BOOL ReadFrame(DWORD & rtpTimestamp, RTP_DataFrame & frame);

    void SendMiscCommand(unsigned command);
    virtual void SendMiscIndication(unsigned command);

    virtual void OnFlowControl(long bitRateRestriction);
    virtual void OnMiscellaneousCommand(const H245_MiscellaneousCommand_type & type);
    virtual void OnMiscellaneousIndication(const H245_MiscellaneousIndication_type & type);
    virtual BOOL SetInitialBandwidth();

    H323Codec * GetCodec() const;

    void SetCacheMode(int _cacheMode)
    { cacheMode = _cacheMode; }

    int GetCacheMode() const
    { return cacheMode; }

    void SetCacheName(const PString & _cacheName)
    { cacheName = _cacheName; }

    const PString & GetCacheName() const
    { return cacheName; }

    void OnFastUpdatePicture()
    {
      if(cache)
        cache->OnFastUpdatePicture();
      else
        ((H323VideoCodec *)codec)->OnFastUpdatePicture();
    }

    void SetFreeze(bool enable);

    void SetIntraRefreshPeriod(int period)
    { intraRefreshPeriod = period; }

    void SetIntraRequestPeriod(int period)
    { intraRequestPeriod = period; }

    void SetAudioJitterEnable(bool enable)
    { audioJitterEnable = enable; }

  protected:
    bool freezeWrite;
    bool isAudio;
    bool audioJitterEnable;

    int intraRefreshPeriod;
    int intraRequestPeriod;

    unsigned encoderSeqN;
    int cacheMode; // -1 - default no cache, 0 - no cache, 1 - cached, 2 - caching
    PString cacheName;
    CacheRTP *cache;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class MCUH323_RTPChannel : public MCU_RTPChannel
{
  PCLASSINFO(MCUH323_RTPChannel, H323_RTPChannel);
  public:
    MCUH323_RTPChannel(H323Connection & connection, const H323Capability & capability, Directions direction, RTP_Session & rtp);
    ~MCUH323_RTPChannel() { };

    virtual BOOL Open();
    virtual BOOL Start();
    virtual void CleanUpOnTermination();
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class MCUSIP_RTPChannel : public MCU_RTPChannel
{
  PCLASSINFO(MCUSIP_RTPChannel, MCU_RTPChannel);

  public:
    MCUSIP_RTPChannel(H323Connection & connection, const H323Capability & capability, Directions direction, RTP_Session & rtp);
    ~MCUSIP_RTPChannel();

    virtual BOOL Open();
    virtual BOOL Start();
    virtual void CleanUpOnTermination();
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class MCU_RTP_UDP : public RTP_UDP
{
  public:
    MCU_RTP_UDP(
#ifdef H323_RTP_AGGREGATE
      PHandleAggregator * aggregator,
#endif
      unsigned id, BOOL remoteIsNat = FALSE
               );

    ~MCU_RTP_UDP();

    virtual BOOL ReadData(RTP_DataFrame & frame, BOOL loop);
    virtual SendReceiveStatus OnReceiveData(const RTP_DataFrame & frame, const RTP_UDP & rtp);

    virtual BOOL WriteData(RTP_DataFrame & frame);
    virtual BOOL PreWriteData(RTP_DataFrame & frame);
    virtual BOOL PostWriteData(RTP_DataFrame & frame);

    virtual BOOL WriteControl(RTP_ControlFrame & frame);

    // non-virtual
    //BOOL ReadBufferedData(DWORD timestamp, RTP_DataFrame & frame);

    void SetFreezeRead(bool enable)
    { freezeRead = enable; }

    virtual void OnRxSenderReport(const SenderReport & sender, const ReceiverReportArray & reports);
    virtual void OnRxReceiverReport(DWORD src, const ReceiverReportArray & reports);

    virtual SendReceiveStatus OnReceiveControl(RTP_ControlFrame & frame);

    // Get total number of control packets received in session.
    DWORD GetRtpcReceived() const { return rtpcReceived; }

    // Get total number transmitted packets lost in session (via RTCP).
    DWORD GetPacketsLostTx() const { return packetsLostTx; }


    BOOL           zrtp_secured;
    PString        zrtp_sas_token;
    BOOL           srtp_secured;

  protected:
    bool freezeRead;
    DWORD rtpcReceived;
    DWORD packetsLostTx;

    unsigned writeDataTimeout;
    unsigned writeDataErrors;
    MCUTime writeDataErrorsTime;
    unsigned writeControlErrors;

    std::map<WORD, RTP_DataFrame *> frameQueue;
    PTime  lastWriteTime;
    DWORD  lastRcvdTimeStamp;
    void   SetLastTimeRTPQueue();
    BOOL   ReadRTPQueue(RTP_DataFrame&);
    BOOL   ProcessRTPQueue(RTP_DataFrame&);
    void   CopyRTPDataFrame(RTP_DataFrame &, RTP_DataFrame &);
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class MCUH323_RTP_UDP : public MCU_RTP_UDP
{
  public:
    MCUH323_RTP_UDP(
#ifdef H323_RTP_AGGREGATE
      PHandleAggregator * aggregator,
#endif
      unsigned id, BOOL remoteIsNat = FALSE
              );
    ~MCUH323_RTP_UDP();

    virtual BOOL ReadData(RTP_DataFrame & frame, BOOL loop);
    virtual SendReceiveStatus OnReceiveData(const RTP_DataFrame & frame, const RTP_UDP & rtp);

    virtual BOOL WriteData(RTP_DataFrame & frame);
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class MCUSIP_RTP_UDP : public MCU_RTP_UDP
{
  public:
    MCUSIP_RTP_UDP(
#ifdef H323_RTP_AGGREGATE
      PHandleAggregator * aggregator,
#endif
      unsigned id, BOOL remoteIsNat = FALSE
              );
    ~MCUSIP_RTP_UDP();

    virtual BOOL ReadData(RTP_DataFrame & frame, BOOL loop);
    virtual SendReceiveStatus OnReceiveData(const RTP_DataFrame & frame, const RTP_UDP & rtp);

    virtual BOOL WriteData(RTP_DataFrame & frame);

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

class MCU_RTP_DataFrame : public RTP_DataFrame
{
  PCLASSINFO(MCU_RTP_DataFrame, RTP_DataFrame);

  public:
    MCU_RTP_DataFrame(PINDEX payloadSize = 2048, BOOL dynamicAllocation = TRUE)
      : RTP_DataFrame(payloadSize, dynamicAllocation)
    { }

    PTime  localTimeStamp;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // _MCU_RTP_H
