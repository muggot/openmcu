
#include <ptlib.h>
#include "mcu.h"
#include "mcu_rtp.h"
#include "mcu_codecs.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

MCUCacheRTPList cacheRTPList;

////////////////////////////////////////////////////////////////////////////////////////////////////

#if MCUSIP_SRTP
#if STRP_TRACING
  static BOOL SRTPError(err_status_t err, const char * fn, const char * file, int line)
  {
    if(err == err_status_ok)
      return FALSE;
    ostream & trace = PTrace::Begin(STRP_TRACING_LEVEL, file, line);
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
  #define SRTP_ERROR(fn, param) SRTPError(fn param, #fn, __FILE__, __LINE__)
#else
  #define SRTP_ERROR(fn, param) ((fn param) != err_status_ok)
#endif // SRTP_PTRACING
#endif // MCUSIP_SRTP

////////////////////////////////////////////////////////////////////////////////////////////////////

#if MCUSIP_ZRTP
#if ZTRP_TRACING
  static BOOL ZRTPError(zrtp_status_t err, const char * fn, const char * file, int line)
  {
    if(err == zrtp_status_ok)
      return FALSE;
    ostream & trace = PTrace::Begin(ZTRP_TRACING_LEVEL, file, line);
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
  #define ZRTP_ERROR(fn, param) ZRTPError(fn param, #fn, __FILE__, __LINE__)
#else
  #define ZRTP_ERROR(fn, param) ((fn param) != zrtp_status_ok)
#endif // ZRTP_PTRACING
#endif // MCUSIP_ZRTP

////////////////////////////////////////////////////////////////////////////////////////////////////

PString srtp_get_random_keysalt()
{
#if MCUSIP_SRTP
  PBYTEArray key_salt(30);
  // set key to random value
  crypto_get_random(key_salt.GetPointer(), 30);
#endif
  return PBase64::Encode(key_salt);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

#if MCUSIP_ZRTP
static zrtp_zid_t zid = { "MCU" };
zrtp_global_t        * zrtp_global;
BOOL                   zrtp_global_initialized = FALSE;

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

#endif // MCUSIP_ZRTP

////////////////////////////////////////////////////////////////////////////////////////////////////

void sip_rtp_init()
{
#if MCUSIP_SRTP
  srtp_init();
#endif
}

void sip_zrtp_init()
{
#if MCUSIP_ZRTP
  if(!zrtp_global_initialized)
  {
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

    //zrtp_log_set_log_engine((zrtp_log_engine *) zrtp_logger);
    zrtp_log_set_level(4);

    // initialize libzrtp.
    if(zrtp_init(&zrtp_config, &zrtp_global) != zrtp_status_ok)
      return;
    zrtp_global_initialized = TRUE;
  }
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void sip_rtp_shutdown()
{
#if MCUSIP_SRTP
//  srtp_shutdown();
#endif
#if MCUSIP_ZRTP
  zrtp_global_initialized = FALSE;
  zrtp_down(zrtp_global);
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////

MCU_RTPChannel::MCU_RTPChannel(H323Connection & conn, const H323Capability & cap, Directions direction, RTP_Session & r)
  : H323_RTPChannel(conn, cap, direction, r)
{
  // Инициализировать сразу
  // не виртуальная
  GetCodec();

  isAudio = (capability->GetMainType() == MCUCapability::e_Audio);
  fastUpdate = true;
  freezeWrite = false;
  audioJitterEnable = true;

  intraRefreshPeriod = 0;
  intraRequestPeriod = 0;

  cache = NULL;
  cacheMode = -1;
  encoderSeqN = 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

MCU_RTPChannel::~MCU_RTPChannel()
{
  if(codec)
  {
    avcodecMutex.Wait();
    delete codec;
    codec = NULL;
    avcodecMutex.Signal();
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

H323Codec * MCU_RTPChannel::GetCodec() const
{
  if(codec == NULL)
  {
    avcodecMutex.Wait();
    ((MCU_RTPChannel*)this)->codec = MCUCapability::CreateCodec(capability, GetDirection() == IsReceiver ? MCUCodec::Decoder : MCUCodec::Encoder);
    avcodecMutex.Signal();

#ifdef H323_AUDIO_CODECS
    if(codec && PIsDescendant(codec, H323AudioCodec))
      ((H323AudioCodec*)codec)->SetSilenceDetectionMode(endpoint.GetSilenceDetectionMode());
#endif
  }

  PTRACE(1, "MCU_RTPChannel\t" << (GetDirection() == IsReceiver ? "Receive" : "Transmit") <<  " Get codec " << codec);
  return codec;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL MCU_RTPChannel::Start()
{
  return H323_RTPChannel::Start();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL MCU_RTPChannel::Open()
{
  PTRACE(1, "MCU_RTPChannel\t" << (GetDirection() == IsReceiver ? "Receive" : "Transmit") <<  " Open");

  if(opened)
    return TRUE;

  if(GetCodec() == NULL)
  {
    PTRACE(1, "MCU_RTPChannel\t" << (GetDirection() == IsReceiver ? "Receive" : "Transmit") << " thread aborted (could not create codec)");
    return FALSE;
  }

  if(!codec->GetMediaFormat().IsValid())
  {
    PTRACE(1, "MCU_RTPChannel\t" << (GetDirection() == IsReceiver ? "Receive" : "Transmit") << " thread aborted (invalid media format)");
    return FALSE;
  }

  codec->AttachLogicalChannel((H323Channel*)this);

  // Open the codec
  if(!codec->Open(connection))
  {
    PTRACE(1, "MCU_RTPChannel\t" << (GetDirection() == IsReceiver ? "Receive" : "Transmit") << " thread aborted (open fail) for "<< *capability);
    return FALSE;
  }

  // Give the connection (or endpoint) a chance to do something with
  // the opening of the codec. Default sets up various filters.
  if(!connection.OnStartLogicalChannel(*this))
  {
    PTRACE(1, "MCU_RTPChannel\t" << (GetDirection() == IsReceiver ? "Receive" : "Transmit") << " thread aborted (OnStartLogicalChannel fail)");
    return FALSE;
  }

  PTRACE(3, "MCU_RTPChannel\tOpened using capability " << *capability);

  opened = TRUE;

  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCU_RTPChannel::SendMiscCommand(unsigned command)
{
  ((MCUH323Connection &)connection).SendLogicalChannelMiscCommand(*this, command); 
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCU_RTPChannel::SendMiscIndication(unsigned command)
{
  ((MCUH323Connection &)connection).SendLogicalChannelMiscIndication(*this, command);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCU_RTPChannel::CleanUpOnTermination()
{
  H323_RTPChannel::CleanUpOnTermination();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCU_RTPChannel::OnFlowControl(long bitRateRestriction)
{
  if(GetCodec() != NULL)
    codec->OnFlowControl(bitRateRestriction);
  else
    PTRACE(3, "MCU_RTPChannel\tOnFlowControl: " << bitRateRestriction);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCU_RTPChannel::OnMiscellaneousCommand(const H245_MiscellaneousCommand_type & type)
{
  if(GetCodec() != NULL)
    codec->OnMiscellaneousCommand(type);
  else
    PTRACE(3, "MCU_RTPChannel\tOnMiscellaneousCommand: chan=" << number << ", type=" << type.GetTagName());
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCU_RTPChannel::OnMiscellaneousIndication(const H245_MiscellaneousIndication_type & type)
{
  if(GetCodec() != NULL)
    codec->OnMiscellaneousIndication(type);
  else
    PTRACE(3, "MCU_RTPChannel\tOnMiscellaneousIndication: chan=" << number << ", type=" << type.GetTagName());
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL MCU_RTPChannel::SetInitialBandwidth()
{
  if(GetCodec() == NULL)
    return TRUE;

  return SetBandwidthUsed(codec->GetMediaFormat().GetBandwidth()/100);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL MCU_RTPChannel::ReadFrame(DWORD & rtpTimestamp, RTP_DataFrame & frame)
{
  if(!rtpSession.ReadBufferedData(rtpTimestamp, frame))
    return FALSE;
  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL MCU_RTPChannel::WriteFrame(RTP_DataFrame & frame)
{
  MCU_RTP_UDP & session = (MCU_RTP_UDP &)rtpSession;

  if(!session.PreWriteData(frame))
    goto error;

  if(!session.WriteData(frame))
    goto error;

  return TRUE;

  error:
    // Завершать соединение только при ошибке записи,
    // при закрытии канала(terminating) не завершать соединение, это не ошибка.
    if(terminating == FALSE)
    {
      PTRACE(1, "MCU_RTPChannel\tTransmit " << (isAudio ? "audio" : "video") << " channel write error, shutdown connection");
      ((MCUH323Connection &)connection).LeaveMCU();
    }
    return FALSE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCU_RTPChannel::SetFreeze(bool enable)
{
  if(receiver)
  {
    // обнуляется в OnReceiveData
    ((MCU_RTP_UDP &)rtpSession).SetFreezeRead(enable);
    // Запрос intra-frame
    if(!isAudio && !enable)
      SendMiscCommand(H245_MiscellaneousCommand_type::e_videoFastUpdatePicture);
  }
  else
  {
    freezeWrite = enable;
    // Отправка intra-frame
    if(!isAudio && !enable)
      ((H323VideoCodec *)codec)->OnFastUpdatePicture();
  }
  MCUTRACE(1, "MCU_RTPChannel " << (receiver ? "Receive" : "Transmit") << " " << (isAudio ? "audio" : "video") << " thread " << (enable ? "freeze" : "unfreeze"));
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCU_RTPChannel::Receive()
{
  if(terminating)
  {
    PTRACE(3, "MCU_RTPChannel\tReceive thread terminated on start up");
    return;
  }

  const OpalMediaFormat & mediaFormat = codec->GetMediaFormat();

  PTRACE(2, "MCU_RTPChannel\tReceive " << mediaFormat << " thread started.");

  // if jitter buffer required, start the thread that is on the other end of it
  if(isAudio && audioJitterEnable && mediaFormat.NeedsJitterBuffer())
  {
    rtpSession.SetJitterBufferSize(connection.GetMinAudioJitterDelay()*mediaFormat.GetTimeUnits(),
                                   connection.GetMaxAudioJitterDelay()*mediaFormat.GetTimeUnits(),
                                   endpoint.GetJitterThreadStackSize());
  }

  // Keep time using th RTP timestamps.
  DWORD codecFrameRate = codec->GetFrameRate();
  DWORD rtpTimestamp = 0;
#if PTRACING
  DWORD lastDisplayedTimestamp = 0;
#endif

  rtpPayloadType = GetRTPPayloadType();
  if(rtpPayloadType == RTP_DataFrame::IllegalPayloadType)
  {
     PTRACE(1, "MCU_RTPChannel\tReceive " << mediaFormat << " thread ended (illegal payload type)");
     return;
  }

  // keep track of consecutive payload type mismatches
  int consecutiveMismatches = 0;

  // do not change payload type for audio and video
  BOOL allowRtpPayloadChange = FALSE;

  MCU_RTP_DataFrame frame;
  while(1)
  {
    // Запрос intra-frame
    if(!isAudio && intraRequestPeriod > 0 && rtpSession.GetPacketsReceived() % intraRequestPeriod == 0)
      SendMiscCommand(H245_MiscellaneousCommand_type::e_videoFastUpdatePicture);

    if(!ReadFrame(rtpTimestamp, frame))
      break;

    filterMutex.Wait();
    for(PINDEX i = 0; i < filters.GetSize(); i++)
      filters[i](frame, 0);
    filterMutex.Signal();

    int size = frame.GetPayloadSize();
    rtpTimestamp = frame.GetTimestamp();

#if PTRACING
    if(rtpTimestamp - lastDisplayedTimestamp > RTP_TRACE_DISPLAY_RATE)
    {
      PTRACE(9, "MCU_RTPChannel\tReceiver written timestamp " << rtpTimestamp);
      lastDisplayedTimestamp = rtpTimestamp;
    }
#endif

    unsigned written;
    BOOL ok = TRUE;
    if(size == 0)
    {
      ok = codec->Write(NULL, 0, frame, written);
      rtpTimestamp += codecFrameRate;
    } else {
      silenceStartTick = PTimer::Tick();

      BOOL isCodecPacket = TRUE;

      if(frame.GetPayloadType() == rtpPayloadType)
      {
        PTRACE_IF(2, consecutiveMismatches > 0, "MCU_RTPChannel\tPayload type matched again " << rtpPayloadType);
        consecutiveMismatches = 0;
      }
      else
      {
        consecutiveMismatches++;
        if(allowRtpPayloadChange && consecutiveMismatches >= MAX_PAYLOAD_TYPE_MISMATCHES)
        {
          rtpPayloadType = frame.GetPayloadType();
          consecutiveMismatches = 0;
          PTRACE(1, "MCU_RTPChannel\tResetting expected payload type to " << rtpPayloadType);
        }
        PTRACE_IF(2, consecutiveMismatches < MAX_PAYLOAD_TYPE_MISMATCHES, "MCU_RTPChannel\tPayload type mismatch: expected "
                  << rtpPayloadType << ", got " << frame.GetPayloadType()
                  << ". Ignoring packet.");
      }

      if(isCodecPacket && consecutiveMismatches == 0)
      {
        const BYTE * ptr = frame.GetPayloadPtr();
        while(ok && size > 0)
        {
          ok = codec->Write(ptr, paused ? 0 : size, frame, written);
          rtpTimestamp += codecFrameRate;
          size -= written != 0 ? written : size;
          ptr += written;
          PTRACE(9, "MCU_RTPChannel\tWrite to decoder");
        }
        PTRACE_IF(1, size < 0, "MCU_RTPChannel\tPayload size too small, short " << -size << " bytes.");
      }
    }

    if(terminating)
      break;

    if(!ok)
    {
      connection.CloseLogicalChannelNumber(number);
      break;
    }
  }

  PTRACE(2, "MCU_RTPChannel\tReceive " << mediaFormat << " thread ended");
}

////////////////////////////////////////////////////////////////////////////////////////////////////

#if PTRACING
class CodecReadAnalyser
{
  enum { MaxSamples = 1000 };
  public:
    CodecReadAnalyser()
    { count = 0; }
    void AddSample(DWORD timestamp)
    {
      if(count < MaxSamples)
      {
        tick[count] = PTimer::Tick();
        rtp[count] = timestamp;
        count++;
      }
    }
    friend ostream & operator<<(ostream & strm, const CodecReadAnalyser & analysis)
    {
      PTimeInterval minimum = PMaxTimeInterval;
      PTimeInterval maximum;
      for(PINDEX i = 1; i < analysis.count; i++)
      {
        PTimeInterval delta = analysis.tick[i] - analysis.tick[i-1];
        strm << setw(6) << analysis.rtp[i] << ' '
               << setw(6) << (analysis.tick[i] - analysis.tick[0]) << ' '
               << setw(6) << delta
               << '\n';
        if(delta > maximum)
          maximum = delta;
        if(delta < minimum)
          minimum = delta;
      }
      strm << "Maximum delta time: " << maximum << "\n"
              "Minimum delta time: " << minimum << '\n';
      return strm;
    }
  private:
    PTimeInterval tick[MaxSamples];
    DWORD rtp[MaxSamples];
    PINDEX count;
};
#endif

void MCU_RTPChannel::Transmit()
{
  if(terminating)
  {
    PTRACE(3, "MCU_RTPChannel\tTransmit thread terminated on start up");
    return;
  }

  const OpalMediaFormat & mediaFormat = codec->GetMediaFormat();

  // Get parameters from the codec on time and data sizes
  unsigned framesInPacket = capability->GetTxFramesInPacket();

  rtpPayloadType = GetRTPPayloadType();
  if(rtpPayloadType == RTP_DataFrame::G722)
     framesInPacket /= 10;

  unsigned maxFrameSize = mediaFormat.GetFrameSize();
  if(maxFrameSize == 0)
    maxFrameSize = isAudio ? 8 : 2000;
  MCU_RTP_DataFrame frame(framesInPacket * maxFrameSize);

  if(rtpPayloadType == RTP_DataFrame::IllegalPayloadType)
  {
     PTRACE(1, "MCU_RTPChannel\tTransmit " << mediaFormat << " thread ended (illegal payload type)");
     return;
  }
  frame.SetPayloadType(rtpPayloadType); 

  PTRACE(2, "MCU_RTPChannel\tTransmit " << mediaFormat << " thread started:"
            " rate=" << codec->GetFrameRate() <<
            " time=" << (codec->GetFrameRate()/(mediaFormat.GetTimeUnits() > 0 ? mediaFormat.GetTimeUnits() : 1)) << "ms" <<
            " size=" << framesInPacket << '*' << maxFrameSize << '='
                    << (framesInPacket*maxFrameSize) );

  // This is real time so need to keep track of elapsed milliseconds
  BOOL silent = TRUE;
  unsigned length;
  unsigned frameOffset = 0;
  unsigned frameCount = 0;
  DWORD rtpFirstTimestamp = rand();
  DWORD rtpTimestamp = rtpFirstTimestamp;
  PTimeInterval firstFrameTick = PTimer::Tick();
  frame.SetPayloadSize(0);

#if PTRACING
  DWORD lastDisplayedTimestamp = 0;
  CodecReadAnalyser * codecReadAnalysis = NULL;
  if(PTrace::GetLevel() >= 5)
    codecReadAnalysis = new CodecReadAnalyser;
#endif

  while(1)
  {
    // periodic intra-frame refresh
    if(!isAudio && intraRefreshPeriod > 0 && rtpSession.GetPacketsSent() % intraRefreshPeriod == 0)
      ((H323VideoCodec *)codec)->OnFastUpdatePicture();

    BOOL retval = FALSE;

    if(cacheMode == -1 || cacheMode == 0 || cacheMode == 1 || encoderSeqN == 0xFFFFFFFF)
      retval = codec->Read(frame.GetPayloadPtr() + frameOffset, length, frame);

    if(cacheMode == 2 && encoderSeqN != 0xFFFFFFFF)
    {
      if(cache == NULL || cache->GetName() != cacheName)
      {
        DetachCacheRTP(cache);
        while(!AttachCacheRTP(cache, cacheName, encoderSeqN))
          PThread::Sleep(100);
      }

      unsigned flags = 0;
      if(!isAudio && fastUpdate)
        flags = PluginCodec_CoderForceIFrame;

      GetCacheRTP(cache, frame, length, encoderSeqN, flags);

      if(!isAudio && flags & PluginCodec_ReturnCoderIFrame)
        fastUpdate = false;

      retval = TRUE;
    }

    if(retval == FALSE)
      break;

    // ???
    if(freezeWrite)
      length = 0;

    if(paused)
      length = 0; // Act as though silent/no video

    // Handle marker bit for audio codec
    if(isAudio)
    {
      // If switching from silence to signal
      if(silent && length > 0)
      {
        silent = FALSE;
        frame.SetMarker(TRUE);  // Set flag for start of sound
        PTRACE(3, "MCU_RTPChannel\tTransmit start of talk burst: " << rtpTimestamp);
      }
      // If switching from signal to silence
      else if (!silent && length == 0)
      {
        silent = TRUE;
        // If had some data waiting to go out
        if(frameOffset > 0)
          frameCount = framesInPacket;  // Force the RTP write
        PTRACE(3, "MCU_RTPChannel\tTransmit  end  of talk burst: " << rtpTimestamp);
      }
    }

    // See if is silence or have some audio data to stuff in the RTP packet
    if(length == 0)
      frame.SetTimestamp(rtpTimestamp);
    else
    {
      silenceStartTick = PTimer::Tick();

      // If first read frame in packet, set timestamp for it
      if(frameOffset == 0)
        frame.SetTimestamp(rtpTimestamp);
      frameOffset += length;

      // Look for special cases
      if(rtpPayloadType == RTP_DataFrame::G729 && length == 2)
      {
        // If we have a G729 sid frame (ie 2 bytes instead of 10) then we must
        // not send any more frames in the RTP packet.
        frameCount = framesInPacket;
      }
      else
      {
        // Increment by number of frames that were read in one hit Note a
        // codec that does variable length frames should never return more
        // than one frame per Read() call or confusion will result.
        frameCount += (length + maxFrameSize - 1) / maxFrameSize;
      }
    }

    BOOL sendPacket = FALSE;

    // Have read number of frames for packet (or just went silent)
    if(frameCount >= framesInPacket)
    {
      // Set payload size to frame offset, now length of frame.
      frame.SetPayloadSize(frameOffset);
      frame.SetPayloadType(rtpPayloadType);

      frameOffset = 0;
      frameCount = 0;

      sendPacket = TRUE;
    }

    if(isAudio)
    {
      filterMutex.Wait();
      for(PINDEX i = 0; i < filters.GetSize(); i++)
        filters[i](frame, (INT)&sendPacket);
      filterMutex.Signal();
    }

    if(sendPacket || (silent && frame.GetPayloadSize() > 0))
    {
      // Send the frame of coded data we have so far to RTP transport
      if(!WriteFrame(frame))
         break;

      if(!isAudio && !frame.GetMarker())
        PThread::Sleep(1);

      // Reset flag for in talk burst
      if(isAudio)
        frame.SetMarker(FALSE);

      frame.SetPayloadSize(0);
      frameOffset = 0;
      frameCount = 0;
    }

    // Calculate the timestamp and real time to take in processing
    if(isAudio)
    {
      rtpTimestamp += codec->GetFrameRate();
    }
    else
    {
      if(frame.GetMarker())
        rtpTimestamp = rtpFirstTimestamp + ((PTimer::Tick() - firstFrameTick).GetInterval() * 90);
    }

#if PTRACING
    if(rtpTimestamp - lastDisplayedTimestamp > RTP_TRACE_DISPLAY_RATE)
    {
      PTRACE(9, "MCU_RTPChannel\tTransmitter sent timestamp " << rtpTimestamp);
      lastDisplayedTimestamp = rtpTimestamp;
    }

    if(codecReadAnalysis != NULL)
      codecReadAnalysis->AddSample(rtpTimestamp);
#endif

    if(terminating)
      break;

  }

  // detach cache
  DetachCacheRTP(cache);

#if PTRACING
  PTRACE_IF(5, codecReadAnalysis != NULL, "MCU_RTPChannel\tTransmit Codec read timing:\n" << *codecReadAnalysis);
  delete codecReadAnalysis;
#endif

  if(!terminating)
    connection.CloseLogicalChannelNumber(number);

  PTRACE(2, "MCU_RTPChannel\tTransmit " << mediaFormat << " thread ended");
}

////////////////////////////////////////////////////////////////////////////////////////////////////

MCUH323_RTPChannel::MCUH323_RTPChannel(H323Connection & conn, const H323Capability & cap, Directions direction, RTP_Session & r)
  : MCU_RTPChannel(conn, cap, direction, r)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL MCUH323_RTPChannel::Start()
{
  return MCU_RTPChannel::Start();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL MCUH323_RTPChannel::Open()
{
  return MCU_RTPChannel::Open();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCUH323_RTPChannel::CleanUpOnTermination()
{
  MCU_RTPChannel::CleanUpOnTermination();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

MCUSIP_RTPChannel::MCUSIP_RTPChannel(H323Connection & conn, const H323Capability & cap, Directions direction, RTP_Session & r)
  : MCU_RTPChannel(conn, cap, direction, r)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////

MCUSIP_RTPChannel::~MCUSIP_RTPChannel()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL MCUSIP_RTPChannel::Open()
{
  return MCU_RTPChannel::Open();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL MCUSIP_RTPChannel::Start()
{
  BOOL status = MCU_RTPChannel::Start();
  if(status)
    ((MCUSIP_RTP_UDP *)&rtpSession)->SetState(!receiver, 1);
  return status;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCUSIP_RTPChannel::CleanUpOnTermination()
{
  ((MCUSIP_RTP_UDP *)&rtpSession)->SetState(!receiver, 0);
  if(terminating)
    return;
  MCU_RTPChannel::CleanUpOnTermination();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

static RTP_Session::ReceiverReportArray BuildReceiverReportArray(const RTP_ControlFrame & frame, PINDEX offset)
{
  RTP_Session::ReceiverReportArray reports;

  const RTP_ControlFrame::ReceiverReport * rr = (const RTP_ControlFrame::ReceiverReport *)(frame.GetPayloadPtr()+offset);
  for(PINDEX repIdx = 0; repIdx < (PINDEX)frame.GetCount(); repIdx++)
  {
    RTP_Session::ReceiverReport * report = new RTP_Session::ReceiverReport;
    report->sourceIdentifier = rr->ssrc;
    report->fractionLost = rr->fraction;
    report->totalLost = rr->GetLostPackets();
    report->lastSequenceNumber = rr->last_seq;
    report->jitter = rr->jitter;
    report->lastTimestamp = (PInt64)(DWORD)rr->lsr;
    report->delay = ((PInt64)rr->dlsr << 16)/1000;
    reports.SetAt(repIdx, report);
    rr++;
  }

  return reports;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

MCU_RTP_UDP::MCU_RTP_UDP(
#ifdef H323_RTP_AGGREGATE
      PHandleAggregator * aggregator,
#endif
      unsigned id, BOOL remoteIsNat
               )
                : RTP_UDP(
#ifdef H323_RTP_AGGREGATE
      aggregator,
#endif
      id, remoteIsNat
                         )
{
  freezeRead = false;

  rtpcReceived = 0;
  packetsLostTx = 0;

  writeDataTimeout = 60;
  writeDataErrors = 0;
  writeDataErrorsTime = 0;
  writeControlErrors = 0;

  zrtp_secured = FALSE;
  srtp_secured = FALSE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

MCU_RTP_UDP::~MCU_RTP_UDP()
{
  std::map<WORD, RTP_DataFrame *>::iterator r;
  while(frameQueue.size() > 0)
  {
    r = frameQueue.begin();
    delete r->second;
    frameQueue.erase(r);
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

RTP_Session::SendReceiveStatus MCU_RTP_UDP::OnReceiveControl(RTP_ControlFrame & frame)
{
  unsigned size = frame.GetPayloadSize();
  if(frame.GetPayloadType() == RTP_ControlFrame::e_SenderReport && size >= sizeof(RTP_ControlFrame::SenderReport))
  {
    RTP_Session::ReceiverReportArray ra = BuildReceiverReportArray(frame, sizeof(RTP_ControlFrame::SenderReport));
    if(ra.GetSize() > 0)
      packetsLostTx = ra[ra.GetSize()-1].totalLost;
  }
  else if(frame.GetPayloadType() == RTP_ControlFrame::e_ReceiverReport && size >= 4)
  {
    RTP_Session::ReceiverReportArray ra = BuildReceiverReportArray(frame, sizeof(PUInt32b));
    if(ra.GetSize() > 0)
      packetsLostTx = ra[ra.GetSize()-1].totalLost;
  }

  return RTP_UDP::OnReceiveControl(frame);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCU_RTP_UDP::OnRxSenderReport(const SenderReport & PTRACE_PARAM(sender), const ReceiverReportArray & PTRACE_PARAM(reports))
{
  rtpcReceived++;
  RTP_UDP::OnRxSenderReport(PTRACE_PARAM(sender), PTRACE_PARAM(reports));
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCU_RTP_UDP::OnRxReceiverReport(DWORD PTRACE_PARAM(src), const ReceiverReportArray & PTRACE_PARAM(reports))
{
  rtpcReceived++;
  RTP_UDP::OnRxReceiverReport(PTRACE_PARAM(src), PTRACE_PARAM(reports));
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL MCU_RTP_UDP::WriteData(RTP_DataFrame & frame)
{
  return PostWriteData(frame);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL MCU_RTP_UDP::PreWriteData(RTP_DataFrame & frame)
{
  if(shutdownWrite)
  {
    PTRACE(3, "MCU_RTP_UDP\tSession " << sessionID << ", Write shutdown.");
    shutdownWrite = FALSE;
    return FALSE;
  }

  // Trying to send a PDU before we are set up!
  if(remoteAddress.IsAny() || !remoteAddress.IsValid() || remoteDataPort == 0)
    return TRUE;

  switch (OnSendData(frame))
  {
    case e_ProcessPacket :
      break;
    case e_IgnorePacket :
      return TRUE;
    case e_AbortTransport :
      return FALSE;
  }

  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL MCU_RTP_UDP::PostWriteData(RTP_DataFrame & frame)
{
  // Сделать несколько попыток записи, трассировка на последней попытке.
  // Возвращает FALSE если невозможно записать в течении writeDataTimeout, в MCU_RTPChannel::WriteFrame обработка ошибки.
  int writeAttempts = 0;
  while(!dataSocket->WriteTo(frame.GetPointer(), frame.GetHeaderSize()+frame.GetPayloadSize(), remoteAddress, remoteDataPort))
  {
    writeAttempts++;
    if(writeAttempts < 3)
      continue;

    switch(dataSocket->GetErrorNumber())
    {
      case ECONNRESET :
      case ECONNREFUSED :
        PTRACE(2, "MCU_RTP_UDP\tSession " << sessionID << ", data port on remote not ready.");
        break;
      default:
        PTRACE(1, "MCU_RTP_UDP\tSession " << sessionID << ", Write error on data port ("
               << dataSocket->GetErrorNumber(PChannel::LastWriteError) << "): "
               << dataSocket->GetErrorText(PChannel::LastWriteError));
    }

    writeDataErrors++;
    if(writeDataErrorsTime == 0)
      writeDataErrorsTime = MCUTime();
    if(MCUTime().GetSeconds() - writeDataErrorsTime.GetSeconds() < writeDataTimeout)
      return TRUE;

    PTRACE(1, "MCU_RTP_UDP\tSession " << sessionID << ", return error after timeout " << writeDataTimeout);
    return FALSE;
  }

  writeDataErrorsTime = 0;
  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL MCU_RTP_UDP::WriteControl(RTP_ControlFrame & frame)
{
  // Trying to send a PDU before we are set up!
  if(!remoteAddress.IsValid() || remoteControlPort == 0)
    return TRUE;

  // Сделать несколько попыток записи, трассировка на последней попытке.
  // Всегда возвращает TRUE.
  int writeAttempts = 0;
  while(!controlSocket->WriteTo(frame.GetPointer(), frame.GetCompoundSize(), remoteAddress, remoteControlPort))
  {
    writeAttempts++;
    if(writeAttempts < 3)
      continue;

    switch(controlSocket->GetErrorNumber())
    {
      case ECONNRESET :
      case ECONNREFUSED :
        PTRACE(2, "RTP_UDP\tSession " << sessionID << ", control port on remote not ready.");
        break;
      default:
        PTRACE(1, "RTP_UDP\tSession " << sessionID << ", Write error on control port ("
               << controlSocket->GetErrorNumber(PChannel::LastWriteError) << "): "
               << controlSocket->GetErrorText(PChannel::LastWriteError));
    }

    writeControlErrors++;
    return TRUE;
  }

  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCU_RTP_UDP::SetLastTimeRTPQueue(void)
{
  PTime oldTime;
  std::map<WORD, RTP_DataFrame *>::iterator r;

  for(r = frameQueue.begin(); r != frameQueue.end(); r++)
  {
    MCU_RTP_DataFrame *frame = (MCU_RTP_DataFrame *)r->second;
    if(oldTime > frame->localTimeStamp)
      oldTime = frame->localTimeStamp;
  }
  lastWriteTime = oldTime;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCU_RTP_UDP::CopyRTPDataFrame(RTP_DataFrame & dstFrame, RTP_DataFrame & srcFrame)
{
  PINDEX frameSize = srcFrame.GetSize();
  dstFrame.SetSize(frameSize);
  memcpy(dstFrame.GetPointer(), srcFrame.GetPointer(), frameSize);
  frameSize = srcFrame.GetPayloadSize();
  dstFrame.SetPayloadSize(frameSize);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL MCU_RTP_UDP::ReadRTPQueue(RTP_DataFrame & frame)
{
  if(frameQueue.size() == 0)
    return FALSE; // queue is empty

  std::map<WORD, RTP_DataFrame *>::iterator r = frameQueue.find(expectedSequenceNumber);

  if(r != frameQueue.end())
  {
    CopyRTPDataFrame(frame,*(r->second));
    delete r->second;
    frameQueue.erase(r);
    SetLastTimeRTPQueue();
    PTRACE(6, "MCU_RTP_UDP\tReadRTPQueue Get frame from queue " << expectedSequenceNumber << " " << frame.GetSequenceNumber());
    return TRUE;
  }

  if((PTime() - lastWriteTime).GetMilliSeconds() > 250)
  {
    WORD i = 0; while( (r = frameQueue.find(expectedSequenceNumber + i)) == frameQueue.end()) i++;
    PTRACE(6, "MCU_RTP_UDP\tReadRTPQueue Timeout, return first frame from queue " << expectedSequenceNumber + i);
    CopyRTPDataFrame(frame,*(r->second));
    delete r->second;
    frameQueue.erase(r);
    return TRUE;
  }

  return FALSE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL MCU_RTP_UDP::ProcessRTPQueue(RTP_DataFrame & frame)
{
  WORD sequenceNumber = frame.GetSequenceNumber();
  if(sequenceNumber == expectedSequenceNumber)
    return TRUE;
  if(frame.GetTimestamp() < lastRcvdTimeStamp)
  {
    PTRACE(6, "MCU_RTP_UDP\tProcessRTPQueue out of order old frame received " << sequenceNumber << " " << lastRcvdTimeStamp << " > " << frame.GetTimestamp());
    return TRUE;
  }

  PTime now;  // Get timestamp now

// if((now - lastWriteTime).GetMilliSeconds() > 250 && frameQueue.size() == 0) return TRUE;

// Out of order frame received, needs to put it in queue
  std::map<WORD, RTP_DataFrame *>::iterator r = frameQueue.find(sequenceNumber);
  if(r != frameQueue.end()) // duplicate frame
  {
    delete r->second;
    frameQueue.erase(r);
    SetLastTimeRTPQueue();
  }

  ((MCU_RTP_DataFrame &)frame).localTimeStamp = now;

  MCU_RTP_DataFrame * newFrame = new MCU_RTP_DataFrame();
  CopyRTPDataFrame(*newFrame,frame);

  frameQueue.insert(std::map<WORD, RTP_DataFrame *>::value_type(sequenceNumber, newFrame));
  SetLastTimeRTPQueue();
  PTRACE(6, "MCU_RTP_UDP\tProcessRTPQueue Put frame into queue " << sequenceNumber);

  r = frameQueue.find(expectedSequenceNumber);

  if(r != frameQueue.end())
  {
    PTRACE(6, "MCU_RTP_UDP\tProcessRTPQueue Get frame from queue " << expectedSequenceNumber);
    CopyRTPDataFrame(frame,*(r->second));
    delete r->second;
    frameQueue.erase(r);
    SetLastTimeRTPQueue();
    return TRUE;
  }

  if((now - lastWriteTime).GetMilliSeconds() > 250) // Timeout, return first frame from queue
  {
    WORD i = 0; while( (r = frameQueue.find(expectedSequenceNumber + i)) == frameQueue.end()) i++;
    PTRACE(6, "MCU_RTP_UDP\tProcessRTPQueue Timeout, return first frame from queue " << expectedSequenceNumber + i);
    CopyRTPDataFrame(frame,*(r->second));
    delete r->second;
    frameQueue.erase(r);
    return TRUE;
  }

  frame.SetSequenceNumber(expectedSequenceNumber-1);
  return FALSE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL MCU_RTP_UDP::ReadData(RTP_DataFrame & frame, BOOL loop)
{
  do
  {
    if(jitter == NULL && ReadRTPQueue(frame))
    {
      OnReceiveData(frame, *this);
      return TRUE; // Got frame from queue
    }

#ifdef H323_RTP_AGGREGATE
    PTime start;
#endif
    int selectStatus = PSocket::Select(*dataSocket, *controlSocket, reportTimer);
#ifdef H323_RTP_AGGREGATE
    unsigned duration = (unsigned)(PTime() - start).GetMilliSeconds();
    if(duration > 50)
      PTRACE(4, "Warning: aggregator read routine was of extended duration = " << duration << " msecs");
#endif

    if(shutdownRead)
    {
      PTRACE(3, "MCU_RTP_UDP\tSession " << sessionID << ", Read shutdown.");
      shutdownRead = FALSE;
      return FALSE;
    }

    switch (selectStatus) {
      case -2 :
        if (ReadControlPDU() == e_AbortTransport)
          return FALSE;
        break;

      case -3 :
        if (ReadControlPDU() == e_AbortTransport)
          return FALSE;
        // Then do -1 case

      case -1 :
        switch (ReadDataPDU(frame)) {
          case e_ProcessPacket :
            if (!shutdownRead)
              return TRUE;
          case e_IgnorePacket :
            break;
          case e_AbortTransport :
            return FALSE;
        }
        break;

      case 0 :
        PTRACE(5, "MCU_RTP_UDP\tSession " << sessionID << ", check for sending report.");
        if (!SendReport())
          return FALSE;
        break;

      case PSocket::Interrupted:
        PTRACE(3, "MCU_RTP_UDP\tSession " << sessionID << ", Interrupted.");
        return FALSE;

      default :
        PTRACE(1, "MCU_RTP_UDP\tSession " << sessionID << ", Select error: " << PChannel::GetErrorText((PChannel::Errors)selectStatus));
        return FALSE;
    }
  } while (loop);

  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

RTP_Session::SendReceiveStatus MCU_RTP_UDP::OnReceiveData(const RTP_DataFrame & frame, const RTP_UDP & rtp)
{
  // Check that the PDU is the right version
  if(frame.GetVersion() != RTP_DataFrame::ProtocolVersion)
    return e_IgnorePacket; // Non fatal error, just ignore

  // Check for if a control packet rather than data packet.
  if(frame.GetPayloadType() > RTP_DataFrame::MaxPayloadType)
    return e_IgnorePacket; // Non fatal error, just ignore

  PTimeInterval tick = PTimer::Tick();  // Get timestamp now

  // Have not got SSRC yet, so grab it now
  if(syncSourceIn == 0)
    syncSourceIn = frame.GetSyncSource();

  // Check packet sequence numbers
  if(packetsReceived == 0)
  {
    expectedSequenceNumber = (WORD)(frame.GetSequenceNumber() + 1);
    lastRcvdTimeStamp = frame.GetTimestamp();
    firstDataReceivedTime = PTime();
    PTRACE(2, "RTP\tFirst data:"
              " ver=" << frame.GetVersion()
           << " pt=" << frame.GetPayloadType()
           << " psz=" << frame.GetPayloadSize()
           << " m=" << frame.GetMarker()
           << " x=" << frame.GetExtension()
           << " seq=" << frame.GetSequenceNumber()
           << " ts=" << frame.GetTimestamp()
           << " src=" << frame.GetSyncSource()
           << " ccnt=" << frame.GetContribSrcCount());
  }
  else
  {
    if(ignoreOtherSources && frame.GetSyncSource() != syncSourceIn)
    {
      PTRACE(2, "RTP\tPacket from SSRC=" << frame.GetSyncSource() << " ignored, expecting SSRC=" << syncSourceIn);
      return e_IgnorePacket; // Non fatal error, just ignore
    }

    if(jitter == NULL)
      ProcessRTPQueue(const_cast <RTP_DataFrame &> (frame));

    WORD sequenceNumber = frame.GetSequenceNumber();
    if(sequenceNumber == expectedSequenceNumber)
    {
      lastRcvdTimeStamp = frame.GetTimestamp();
      expectedSequenceNumber++;
      consecutiveOutOfOrderPackets = 0;
      // Only do statistics on packets after first received in talk burst
      if(!frame.GetMarker())
      {
        DWORD diff = (tick - lastReceivedPacketTime).GetInterval();

        averageReceiveTimeAccum += diff;
        if(diff > maximumReceiveTimeAccum)
          maximumReceiveTimeAccum = diff;
        if(diff < minimumReceiveTimeAccum)
          minimumReceiveTimeAccum = diff;
        rxStatisticsCount++;

        // The following has the implicit assumption that something that has jitter
        // is an audio codec and thus is in 8kHz timestamp units.
        diff *= 8;
        long variance = diff - lastTransitTime;
        lastTransitTime = diff;
        if (variance < 0)
          variance = -variance;
        jitterLevel += variance - ((jitterLevel+8) >> 4);
        if (jitterLevel > maximumJitterLevel)
          maximumJitterLevel = jitterLevel;
      }
    }
    else if (sequenceNumber < expectedSequenceNumber)
    {
      PTRACE(3, "RTP\tOut of order packet, received "
             << sequenceNumber << " expected " << expectedSequenceNumber
             << " ssrc=" << syncSourceIn);
      packetsOutOfOrder++;

      // Check for Cisco bug where sequence numbers suddenly start incrementing
      // from a different base.
      if(++consecutiveOutOfOrderPackets > 10)
      {
        expectedSequenceNumber = (WORD)(sequenceNumber + 1);
        PTRACE(1, "RTP\tAbnormal change of sequence numbers, adjusting to expect "
               << expectedSequenceNumber << " ssrc=" << syncSourceIn);
      }

      if(ignoreOutOfOrderPackets)
        return e_IgnorePacket; // Non fatal error, just ignore
    }
    else
    {
      unsigned dropped = sequenceNumber - expectedSequenceNumber;
      packetsLost += dropped;
      packetsLostSinceLastRR += dropped;
      PTRACE(3, "RTP\tDropped " << dropped << " packet(s) at " << sequenceNumber
             << ", ssrc=" << syncSourceIn);
      expectedSequenceNumber = (WORD)(sequenceNumber + 1);
      consecutiveOutOfOrderPackets = 0;
    }
  }

  lastReceivedPacketTime = tick;

  octetsReceived += frame.GetPayloadSize();
  packetsReceived++;

  if(rtp.GetRemoteDataPort() > 0 && localAddress.AsString().IsEmpty())
  {
    localAddress = rtp.GetLocalAddress().AsString() + ":" + PString(rtp.GetLocalDataPort());
    remoteAddress = rtp.GetRemoteAddress().AsString() + ":" + PString(rtp.GetRemoteDataPort());
  }

  // Call the statistics call-back on the first PDU with total count == 1
  if(packetsReceived == 1 && userData != NULL)
    userData->OnRxStatistics(*this);

  if(!SendReport())
    return e_AbortTransport;

  if(rxStatisticsCount < rxStatisticsInterval)
    return e_ProcessPacket;

  rxStatisticsCount = 0;

  averageReceiveTime = averageReceiveTimeAccum/rxStatisticsInterval;
  maximumReceiveTime = maximumReceiveTimeAccum;
  minimumReceiveTime = minimumReceiveTimeAccum;

  averageReceiveTimeAccum = 0;
  maximumReceiveTimeAccum = 0;
  minimumReceiveTimeAccum = 0xffffffff;

  PTRACE(2, "RTP\tReceive statistics: "
            " packets=" << packetsReceived <<
            " octets=" << octetsReceived <<
            " lost=" << packetsLost <<
            " tooLate=" << GetPacketsTooLate() <<
            " order=" << packetsOutOfOrder <<
            " avgTime=" << averageReceiveTime <<
            " maxTime=" << maximumReceiveTime <<
            " minTime=" << minimumReceiveTime <<
            " jitter=" << (jitterLevel >> 7) <<
            " maxJitter=" << (maximumJitterLevel >> 7)
            );

  if(userData != NULL)
    userData->OnRxStatistics(*this);

  return e_ProcessPacket;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

MCUH323_RTP_UDP::MCUH323_RTP_UDP(
#ifdef H323_RTP_AGGREGATE
                             PHandleAggregator * aggregator,
#endif
                             unsigned id, BOOL remoteIsNat
                            )
                              : MCU_RTP_UDP(
#ifdef H323_RTP_AGGREGATE
                                        aggregator,
#endif
                                        id, remoteIsNat
                                       )
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////

MCUH323_RTP_UDP::~MCUH323_RTP_UDP()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL MCUH323_RTP_UDP::ReadData(RTP_DataFrame & frame, BOOL loop)
{
  if(!MCU_RTP_UDP::ReadData(frame, loop))
    return FALSE;
  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

RTP_Session::SendReceiveStatus MCUH323_RTP_UDP::OnReceiveData(const RTP_DataFrame & frame, const RTP_UDP & rtp)
{
  SendReceiveStatus status = MCU_RTP_UDP::OnReceiveData(frame, rtp);
  if(freezeRead)
  {
    RTP_DataFrame & _frame = *PRemoveConst(RTP_DataFrame, &frame);
    _frame.SetPayloadSize(0);
    return e_ProcessPacket;
  }
  return status;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL MCUH323_RTP_UDP::WriteData(RTP_DataFrame & frame)
{
  return MCU_RTP_UDP::WriteData(frame);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

MCUSIP_RTP_UDP::MCUSIP_RTP_UDP(
#ifdef H323_RTP_AGGREGATE
                             PHandleAggregator * aggregator,
#endif
                             unsigned id, BOOL remoteIsNat
                            )
                              : MCU_RTP_UDP(
#ifdef H323_RTP_AGGREGATE
                                        aggregator,
#endif
                                        id, remoteIsNat
                                       )
{
  transmitter_state = 0;
  receiver_state = 0;
  zrtp_master = FALSE;
  conn = NULL;
  ssrc = random()%100000;
  zrtp_initialised = FALSE;
#if MCUSIP_SRTP
  srtp_read = NULL;
  srtp_write = NULL;
#endif
#if MCUSIP_ZRTP
  zrtp_profile = NULL;
  zrtp_session = NULL;
  zrtp_stream = NULL;
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////

MCUSIP_RTP_UDP::~MCUSIP_RTP_UDP()
{
#if MCUSIP_SRTP
  if(srtp_read) delete srtp_read;
  if(srtp_write) delete srtp_write;
#endif
#if MCUSIP_ZRTP
  if(zrtp_session) zrtp_session_down(zrtp_session);
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCUSIP_RTP_UDP::SetState(int dir, int state)
{
  if(!dir)
    receiver_state = state;
  else
    transmitter_state = state;

#if MCUSIP_ZRTP
  if(zrtp_initialised && zrtp_master && transmitter_state == 1 && receiver_state == 1)
    zrtp_stream_registration_start(zrtp_stream, ssrc);
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL MCUSIP_RTP_UDP::ReadData(RTP_DataFrame & frame, BOOL loop)
{
  if(!MCU_RTP_UDP::ReadData(frame, loop))
    return FALSE;
  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL MCUSIP_RTP_UDP::WriteData(RTP_DataFrame & frame)
{
#if MCUSIP_SRTP
  if(srtp_write)
  {
    int len = frame.GetHeaderSize() + frame.GetPayloadSize();
    frame.SetMinSize(len + SRTP_MAX_TRAILER_LEN);
    if(SRTP_ERROR(srtp_protect, (srtp_write->GetSession(), frame.GetPointer(), &len)))
      return TRUE;
    //cout << "SRTP Protected RTP packet\n";
    frame.SetPayloadSize(len - frame.GetHeaderSize());
  }
#endif
#if MCUSIP_ZRTP
  if(zrtp_initialised)
  {
    //cout << "ZRTP OnSendData\n";
    unsigned len = frame.GetHeaderSize() + frame.GetPayloadSize();
    frame.SetSize(2060);
    if(ZRTP_ERROR(zrtp_process_rtp, (zrtp_stream, (char *)frame.GetPointer(), &len)))
      return TRUE;
    frame.SetSize(len);
    frame.SetPayloadSize(len - frame.GetHeaderSize());
  }
#endif

  return MCU_RTP_UDP::WriteData(frame);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL MCUSIP_RTP_UDP::WriteDataZRTP(RTP_DataFrame & frame)
{
  if(transmitter_state == 0)
    return TRUE;

  if(!dataSocket)
    return FALSE;

  if(shutdownWrite)
  {
    shutdownWrite = FALSE;
    return FALSE;
  }

  // Trying to send a PDU before we are set up!
  if(remoteAddress.IsAny() || !remoteAddress.IsValid() || remoteDataPort == 0)
    return TRUE;

  if(!MCU_RTP_UDP::WriteData(frame))
    return FALSE;

  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

RTP_Session::SendReceiveStatus MCUSIP_RTP_UDP::OnReceiveData(const RTP_DataFrame & frame, const RTP_UDP & rtp)
{
  SendReceiveStatus status = MCU_RTP_UDP::OnReceiveData(frame, rtp);

  if(freezeRead && (!zrtp_initialised || zrtp_sas_token != ""))
  {
    RTP_DataFrame & _frame = *PRemoveConst(RTP_DataFrame, &frame);
    _frame.SetPayloadSize(0);
    return e_ProcessPacket;
  }

#if MCUSIP_SRTP
  if(srtp_read)
  {
    RTP_DataFrame & _frame = *PRemoveConst(RTP_DataFrame, &frame);
    int len = _frame.GetHeaderSize() + _frame.GetPayloadSize();
    if(SRTP_ERROR(srtp_unprotect, (srtp_read->GetSession(), _frame.GetPointer(), &len)))
    {
      _frame.SetPayloadSize(0);
      return e_ProcessPacket;
    }
    //cout << "SRTP Unprotected RTP packet\n";
    _frame.SetPayloadSize(len - _frame.GetHeaderSize());
  }
#endif
#if MCUSIP_ZRTP
  if(zrtp_initialised)
  {
    // ZRTP frame proto version validation fails in RTP_UDP::OnReceiveData
    //if(status == e_IgnorePacket && frame.GetVersion() != RTP_DataFrame::ProtocolVersion)
    //  return e_ProcessPacket;

    RTP_DataFrame & _frame = *PRemoveConst(RTP_DataFrame, &frame);
    unsigned len = _frame.GetPayloadSize() + _frame.GetHeaderSize();
    unsigned hlen = _frame.GetHeaderSize();
    //cout << "ZRTP OnReceiveData " << len << "\n";
    if(ZRTP_ERROR(zrtp_process_srtp, (zrtp_stream, (char *)_frame.GetPointer(), &len)))
      return e_IgnorePacket;
    _frame.SetPayloadSize(len - hlen);
  }
#endif
  return status;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL MCUSIP_RTP_UDP::CreateSRTP(int dir, const PString & crypto, const PString & key_str)
{
#if MCUSIP_SRTP
  if(dir == 0)
  {
    if(srtp_read) return FALSE;
    srtp_read = new SipSRTP();
    if(!srtp_read->Init(crypto, key_str)) { delete srtp_read; srtp_read = NULL; return FALSE; }
  } else {
    if(srtp_write) return FALSE;
    srtp_write = new SipSRTP();
    if(!srtp_write->Init(crypto, key_str)) { delete srtp_write; srtp_write = NULL; return FALSE; }
  }
#endif
  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL MCUSIP_RTP_UDP::CreateZRTP()
{
#if MCUSIP_ZRTP
  if(!zrtp_global_initialized)
    sip_zrtp_init();

  if(!zrtp_global_initialized)
    return FALSE;

  if(zrtp_profile || zrtp_initialised)
    return TRUE;

  if(!zrtp_master) // extended stream
    return TRUE;

  zrtp_profile = (zrtp_profile_t *)malloc(sizeof(*zrtp_profile));
  zrtp_profile_defaults(zrtp_profile, zrtp_global);
  //zrtp_profile->active         = 1;
  zrtp_profile->autosecure     = 1;
  zrtp_profile->allowclear     = 0; // allowclear: OFF
  //zrtp_profile->disclose_bit   = 0;
  zrtp_profile->cache_ttl      = (uint32_t)-1;
  //zrtp_profile->cache_ttl      = ZRTP_CACHE_DEFAULT_TTL;

  //ZRTP_SIGNALING_ROLE_UNKNOWN //ZRTP_SIGNALING_ROLE_INITIATOR //ZRTP_SIGNALING_ROLE_RESPONDER
  if(ZRTP_ERROR(zrtp_session_init, (zrtp_global, zrtp_profile, zid, ZRTP_SIGNALING_ROLE_UNKNOWN, &zrtp_session)))
    return FALSE;
  zrtp_session_set_userdata(zrtp_session, this);

  if(ZRTP_ERROR(zrtp_stream_attach, (zrtp_session, &zrtp_stream)))
    return FALSE;
  zrtp_stream_set_userdata(zrtp_stream, this);

  // start only if transmitter and receiver is running
  //zrtp_stream_registration_start(zrtp_stream, ssrc);
  //zrtp_stream_start(zrtp_stream, ssrc);

  zrtp_initialised = TRUE;
#endif
  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
#if MCUSIP_SRTP
////////////////////////////////////////////////////////////////////////////////////////////////////

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

////////////////////////////////////////////////////////////////////////////////////////////////////
#endif // MCUSIP_SRTP
////////////////////////////////////////////////////////////////////////////////////////////////////

