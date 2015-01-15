
#ifndef _MCU_RTP_H
#define _MCU_RTP_H

#include <ptlib.h>
#include <ptclib/cypher.h>

#include <channels.h>
#include <h323rtp.h>
#include <h323con.h>

#include "config.h"
#include "utils.h"

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

void sip_rtp_init();
void sip_rtp_shutdown();

PString srtp_get_random_keysalt();

////////////////////////////////////////////////////////////////////////////////////////////////////

#define FRAME_MASK	0xFFFFFF00
#define FRAME_BUF_SIZE	0x2000
#define FRAME_OFFSET	0x100

#define	MAX_PAYLOAD_TYPE_MISMATCHES 8
#define RTP_TRACE_DISPLAY_RATE 16000 // 2 seconds

////////////////////////////////////////////////////////////////////////////////////////////////////

class CacheRTP
{
  public:
    CacheRTP(long _id, const PString & _name)
    {
      id = _id;
      name = _name;
      seqN = FRAME_BUF_SIZE;
      lastN = 0;
      iframeN = 0;
      uN = 0;
      fastUpdate = false;
    }

   ~CacheRTP()
    {
      for(CacheRTPUnitMap::iterator r = unitList.begin(); r != unitList.end(); )
      {
        CacheRTPUnit *unit = r->second;
        unitList.erase(r++);
        if(unit)
          delete unit;
      }
    }

    long GetID() const
    { return id; }

    const PString & GetName() const
    { return name; }

    void OnFastUpdatePicture()
    { fastUpdate = true; }

    bool GetMarker (unsigned char *pkt)
    { return (pkt[1] & 0x80); }

    void IncrementUsersNumber()
    { uN++; }

    void DecrementUsersNumber()
    { uN--; }

    unsigned GetUsersNumber() const
    { return uN; }

    void GetFastUpdate(unsigned & flags)
    {
      if(!fastUpdate)
        return;
      if(seqN - iframeN <= FRAME_OFFSET * 10)
        return;
      MCUTRACE(1, "CacheRTP " << name << " FastUpdate needed");
      flags |= PluginCodec_CoderForceIFrame;
      fastUpdate = false;
      iframeN = (seqN & FRAME_MASK) + FRAME_OFFSET;
    }

    unsigned int GetLastFrameNum()
    {
      CacheRTPUnitMap::reverse_iterator r = unitList.rbegin();
      if(r != unitList.rend())
        return r->first;
      return 0;
    }

    void PutFrame(RTP_DataFrame & frame, unsigned len, unsigned flags)
    {
      CacheRTPUnit *unit;
      unsigned int l;
      //cout << "Frame seqN/lastN " << seqN << "/" << lastN << "\n";
      CacheRTPUnitMap::iterator r = unitList.find(lastN);
      while(r == unitList.end() && seqN - lastN >= FRAME_BUF_SIZE)
      {
        lastN = (lastN & FRAME_MASK) + FRAME_OFFSET;
        r = unitList.find(lastN);
      };

      if(r == unitList.end())
        unit = new CacheRTPUnit();
      else
      { unit = r->second; unitList.erase(r); lastN++; }
      unit->PutFrame(frame);
      unit->len = len;
      unit->lock = 0;
      unitList.insert(CacheRTPUnitMap::value_type(seqN, unit));
      //if(flags&PluginCodec_ReturnCoderIFrame)
      //{ fastUpdate = false; iframeN = seqN;  cout << "IFrame found\n"; }
      if(GetMarker(frame.GetPointer()))
      {
	seqN = (seqN & FRAME_MASK) + FRAME_OFFSET;
        l = seqN - FRAME_BUF_SIZE / 2;
	do // lock all frame
        {
	  r = unitList.find(l);
	  l++;
          if(r != unitList.end())
            r->second->lock = 1;
	}
        while(r != unitList.end());
      }
      else
        seqN++;
    }

    void GetFrame(RTP_DataFrame & frame, unsigned & toLen, unsigned & num, unsigned & flags)
    {
      while(num >= seqN)
        MCUTime::Sleep(10);
      CacheRTPUnitMap::iterator r = unitList.find(num);
      int i=0;
      while((r == unitList.end() || r->second->lock) && num < seqN)
      { // for debug
        PTRACE_IF(3, i > 0, "H323READ\t Lost Packet " << i << " " << num);
        PTRACE_IF(3, (r != unitList.end() && r->second->lock), "H323READ\t Lost Packet " << i << " " << num << " " << r->second->lock);
        num = (num & FRAME_MASK) + FRAME_OFFSET; // may be lost frames, fix it
        r = unitList.find(num);
        i++;
      }
      while(r == unitList.end())
      {
        MCUTime::Sleep(10);
        r = unitList.find(num);
      }
      r->second->GetFrame(frame);
      toLen = r->second->len;
      flags = 0;
      if(GetMarker(frame.GetPointer()))
        flags |= PluginCodec_ReturnCoderLastFrame;
      if(num == iframeN)
        flags |= PluginCodec_ReturnCoderIFrame;
      num++;
    }

  private:

    long id;
    PString name;
    unsigned seqN;
    unsigned lastN;
    unsigned iframeN;
    bool fastUpdate;
    unsigned uN;

    class CacheRTPUnit
    {
        friend class CacheRTP;

        CacheRTPUnit() {};
        ~CacheRTPUnit() {};

        void PutFrame(RTP_DataFrame &srcFrame)
        {
          int sz = srcFrame.GetHeaderSize()+srcFrame.GetPayloadSize();
          frame.SetMinSize(sz);
          memcpy(frame.GetPointer(), srcFrame.GetPointer(), sz);
          frame.SetPayloadSize(srcFrame.GetPayloadSize());
        }

        void GetFrame(RTP_DataFrame &dstFrame)
        {
          int sz = frame.GetHeaderSize() + frame.GetPayloadSize();
          dstFrame.SetMinSize(sz);
          memcpy(dstFrame.GetPointer(), frame.GetPointer(), sz);
          dstFrame.SetPayloadSize(frame.GetPayloadSize());
        }

        int lock;
        unsigned int len;
        RTP_DataFrame frame;
    };
    typedef std::map<unsigned int, CacheRTPUnit *> CacheRTPUnitMap;
    CacheRTPUnitMap unitList;
};

typedef MCUSharedList<CacheRTP> MCUCacheRTPList;
extern MCUCacheRTPList cacheRTPList;

////////////////////////////////////////////////////////////////////////////////////////////////////

inline CacheRTP * CreateCacheRTP(const PString & key)
{
  MCUCacheRTPList::shared_iterator it = cacheRTPList.Find(key);
  if(it == cacheRTPList.end())
  {
    long id = cacheRTPList.GetNextID();
    CacheRTP *cache = new CacheRTP(id, key);
    cacheRTPList.Insert(id, cache, key);
    return cache;
  }
  return NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

inline void DeleteCacheRTP(CacheRTP *& cache)
{
  if(cache == NULL)
    return;
  PString key = cache->GetName();
  cacheRTPList.Release(cache->GetID());
  MCUCacheRTPList::shared_iterator it = cacheRTPList.Find(key);
  if(it != cacheRTPList.end())
  {
    MCUTRACE(1, "CacheRTP " << key << " Delete");
    cache = it.GetObject();
    if(cacheRTPList.Erase(it))
      delete cache;
  }
  cache = NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

inline bool FindCacheRTP(const PString & key)
{
  MCUCacheRTPList::shared_iterator it = cacheRTPList.Find(key);
  if(it == cacheRTPList.end())
    return false;
  return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

inline void PutCacheRTP(CacheRTP *& cache, RTP_DataFrame & frame, unsigned int len, unsigned int flags)
{
  if(cache == NULL)
  {
    MCUTRACE(1, "CacheRTP Put - No cache!");
    return;
  }
  //cout << "PutCacheRTP length=" << len << " marker=" << frame.GetMarker() << "\n";
  cache->PutFrame(frame, len, flags);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

inline void GetCacheRTP(CacheRTP *& cache, RTP_DataFrame & frame, unsigned & toLen, unsigned & seqN, unsigned & flags)
{
  if(!cache)
  {
    MCUTRACE(1, "CacheRTP Get - No cache!");
    seqN = 0xFFFFFFFF;
    return;
  }
  if(flags & PluginCodec_CoderForceIFrame)
    cache->OnFastUpdatePicture();
  cache->GetFrame(frame, toLen, seqN, flags);
  //cout << "GetCacheRTP length=" << toLen << " marker=" << frame.GetMarker() << " flags=" << flags  << "\n";
}

////////////////////////////////////////////////////////////////////////////////////////////////////

inline bool AttachCacheRTP(CacheRTP *& cache, const PString & key, unsigned & encoderSeqN)
{
  MCUCacheRTPList::shared_iterator it = cacheRTPList.Find(key);
  if(it == cacheRTPList.end())
    return false;
  MCUTRACE(1, "CacheRTP " << key << " Attach");
  cache = it.GetCapturedObject();
  cache->IncrementUsersNumber();
  encoderSeqN = cache->GetLastFrameNum();
  return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

inline void DetachCacheRTP(CacheRTP *& cache)
{
  if(cache == NULL)
    return;
  MCUTRACE(1, "CacheRTP " << cache->GetName() << " Detach");
  cache->DecrementUsersNumber();
  cacheRTPList.Release(cache->GetID());
  cache = NULL;
}

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
        fastUpdate = true;
    }

    void SetFreeze(bool enable);

    void SetIntraRefreshPeriod(int period)
    { intraRefreshPeriod = period; }

    void SetIntraRequestPeriod(int period)
    { intraRequestPeriod = period; }

    void SetAudioJitterEnable(bool enable)
    { audioJitterEnable = enable; }

  protected:
    bool fastUpdate;
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
