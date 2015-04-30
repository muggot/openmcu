
#include "precompile.h"
#include "mcu.h"
#include "mcu_codecs.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

extern "C" {
  unsigned char linear2ulaw(int pcm_val);
  int ulaw2linear(unsigned char u_val);
  unsigned char linear2alaw(int pcm_val);
  int alaw2linear(unsigned char u_val);
};

////////////////////////////////////////////////////////////////////////////////////////////////////

MCUFramedAudioCodec::MCUFramedAudioCodec(const OpalMediaFormat & fmt, Direction direction, PluginCodec_Definition * _codec)
  : H323AudioCodec(fmt, direction), codec(_codec)
{
  if(codec != NULL && codec->createCodec != NULL)
    context = (*codec->createCodec)(codec);
  else
    context = NULL;

  sampleRate = mediaFormat.GetTimeUnits() * 1000;
  if(direction == Decoder)
    channels = mediaFormat.GetOptionInteger(OPTION_DECODER_CHANNELS, 1);
  else
    channels = mediaFormat.GetOptionInteger(OPTION_ENCODER_CHANNELS, 1);
  bytesPerFrame = mediaFormat.GetFrameSize();

  sampleBuffer = PShortArray(samplesPerFrame * channels);

  if(context == NULL)
  {
    PTRACE(1, "MCUFramedAudioCodec\tFailed create codec " << mediaFormat);
    return;
  }

  PluginCodec_ControlDefn * ctl = GetCodecControl(codec, SET_CODEC_OPTIONS_CONTROL);
  if(ctl != NULL)
  {
    PStringArray list;
    for(PINDEX i = 0; i < mediaFormat.GetOptionCount(); i++)
    {
      const OpalMediaOption & option = mediaFormat.GetOption(i);
      list += option.GetName();
      list += option.AsString();
      PTRACE(5, "MCUFramedAudioCodec\tSetting codec option '" << option.GetName() << "'=" << option.AsString());
    }
    char ** _options = list.ToCharArray();
    unsigned int optionsLen = sizeof(_options);
    (*ctl->control)(codec, context, SET_CODEC_OPTIONS_CONTROL, _options, &optionsLen);
    free(_options);
  }

#if PTRACING
  PTRACE(6,"MCUFramedAudioCodec Options");
  OpalMediaFormat::DebugOptionList(mediaFormat);
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////

MCUFramedAudioCodec::~MCUFramedAudioCodec()
{
  if(codec != NULL && codec->destroyCodec != NULL)
    (*codec->destroyCodec)(codec, context);
  codec = NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL MCUFramedAudioCodec::Open(H323Connection & connection)
{
  return connection.OpenAudioChannel(direction == Encoder, samplesPerFrame * channels * 2, *this);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL MCUFramedAudioCodec::EncodeFrame(BYTE * buffer, unsigned int & toLen)
{
  if(codec == NULL || direction != Encoder)
    return FALSE;

  unsigned int fromLen = codec->parm.audio.samplesPerFrame * channels * 2;
  toLen                = codec->parm.audio.bytesPerFrame;

  unsigned flags = 0;
  return (codec->codecFunction)(codec, context, (const unsigned char *)sampleBuffer.GetPointer(), &fromLen,
                                buffer, &toLen,
                                &flags) != 0;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL MCUFramedAudioCodec::DecodeFrame(const BYTE * buffer, unsigned length, unsigned & written, unsigned & bytesDecoded)
{
  if(codec == NULL || direction != Decoder)
    return FALSE;

  unsigned flags = 0;
  if((codec->codecFunction)(codec, context, buffer, &length, (unsigned char *)sampleBuffer.GetPointer(),
                            &bytesDecoded, &flags) == 0)
    return FALSE;

  written = length;
  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

unsigned MCUFramedAudioCodec::GetAverageSignalLevel()
{
  // Calculate the average signal level of this frame
  int sum = 0;

  const short * pcm = sampleBuffer;
  const short * end = pcm + samplesPerFrame;
  while(pcm != end)
  {
    if(*pcm < 0)
      sum -= *pcm++;
    else
      sum += *pcm++;
  }

  return sum/samplesPerFrame;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL MCUFramedAudioCodec::DetectSilence()
{
  // Can never have silence if NoSilenceDetection
  if(silenceDetectMode == NoSilenceDetection)
    return FALSE;

  PTRACE(6, "MCUFramedAudioCodec\tSilence detection enabled");

  // Can never have average signal level that high, this indicates that the
  // hardware cannot do silence detection.
  unsigned level = GetAverageSignalLevel();
  if(level == UINT_MAX)
    return FALSE;

  // Convert to a logarithmic scale - use uLaw which is complemented
  level = linear2ulaw(level) ^ 0xff;

  // Now if signal level above threshold we are "talking"
  BOOL haveSignal = level > levelThreshold;

  // If no change ie still talking or still silent, resent frame counter
  if(inTalkBurst == haveSignal)
    framesReceived = 0;
  else
  {
    framesReceived++;
    // If have had enough consecutive frames talking/silent, swap modes.
    if(framesReceived >= (inTalkBurst ? silenceDeadbandFrames : signalDeadbandFrames))
    {
      inTalkBurst = !inTalkBurst;
      PTRACE(4, "Codec\tSilence detection transition: "
             << (inTalkBurst ? "Talk" : "Silent")
             << " level=" << level << " threshold=" << levelThreshold);

      // If we had talk/silence transition restart adaptive threshold measurements
      signalMinimum = UINT_MAX;
      silenceMaximum = 0;
      signalFramesReceived = 0;
      silenceFramesReceived = 0;
    }
  }

  if(silenceDetectMode == FixedSilenceDetection)
    return !inTalkBurst;

  if(levelThreshold == 0)
  {
    if(level > 1)
    {
      // Bootstrap condition, use first frame level as silence level
      levelThreshold = level/2;
      if(levelThreshold>64) levelThreshold=64;
      PTRACE(4, "Codec\tSilence detection threshold initialised to: " << levelThreshold);
    }
    return TRUE; // inTalkBurst always FALSE here, so return silent
  }

  // Count the number of silent and signal frames and calculate min/max
  if(haveSignal)
  {
    if (level < signalMinimum)
      signalMinimum = level;
    signalFramesReceived++;
  } else {
    if (level > silenceMaximum)
      silenceMaximum = level;
    silenceFramesReceived++;
  }

  // See if we have had enough frames to look at proportions of silence/signal
  unsigned atff = (unsigned)(((unsigned long)adaptiveThresholdFrames) * sampleRate * channels / 8000);
  if((signalFramesReceived + silenceFramesReceived) > atff)
  {
    // Now we have had a period of time to look at some average values we can
    //  make some adjustments to the threshold. There are four cases:
    if(signalFramesReceived >= atff)
    {
      // If every frame was noisy, move threshold up. Don't want to move too
      // fast so only go a quarter of the way to minimum signal value over the
      // period. This avoids oscillations, and time will continue to make the
      // level go up if there really is a lot of background noise.
      int delta = (signalMinimum - levelThreshold)/4;
      if(delta != 0)
      {
        levelThreshold += delta;
        if(levelThreshold > 64)
          levelThreshold=64;
        PTRACE(4, "Codec\tSilence detection threshold increased to: " << levelThreshold);
      }
    }
    else if (silenceFramesReceived >= atff)
    {
      // If every frame was silent, move threshold down. Again do not want to
      // move too quickly, but we do want it to move faster down than up, so
      // move to halfway to maximum value of the quiet period. As a rule the
      // lower the threshold the better as it would improve response time to
      // the start of a talk burst.
      unsigned newThreshold = (levelThreshold + silenceMaximum)/2 + 1;
      if(levelThreshold != newThreshold)
      {
        levelThreshold = newThreshold;
        PTRACE(4, "Codec\tSilence detection threshold decreased to: " << levelThreshold);
      }
    }
    else if (signalFramesReceived > silenceFramesReceived)
    {
      // We haven't got a definitive silent or signal period, but if we are
      // constantly hovering at the threshold and have more signal than
      // silence we should creep up a bit.
      levelThreshold++;
      PTRACE(4, "Codec\tSilence detection threshold incremented to: " << levelThreshold
               << " signal=" << signalFramesReceived << ' ' << signalMinimum
               << " silence=" << silenceFramesReceived << ' ' << silenceMaximum);
    }

    signalMinimum = UINT_MAX;
    silenceMaximum = 0;
    signalFramesReceived = 0;
    silenceFramesReceived = 0;
  }

  return !inTalkBurst;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void MCUFramedAudioCodec::DecodeSilenceFrame(void * buffer, unsigned length)
{
  if((codec->flags & PluginCodec_DecodeSilence) == 0)
    memset(buffer, 0, length);
  else
  {
    unsigned flags = PluginCodec_CoderSilenceFrame;
    (codec->codecFunction)(codec, context, NULL, NULL, buffer, &length, &flags);
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL MCUFramedAudioCodec::Read(BYTE * buffer, unsigned & length, RTP_DataFrame &)
{
  PWaitAndSignal mutex(rawChannelMutex);

  if(direction != Encoder)
  {
    PTRACE(1, "MCUFramedAudioCodec\tAttempt to decode from encoder");
    return FALSE;
  }

  if(IsRawDataHeld)
  {
    // If connection is onHold
    PProcess::Sleep(5);  // Sleep to avoid CPU overload. <--Should be a better method but it works :)
    length = 0;
    return TRUE;
  }

  if(rawDataChannel == NULL)
  {
    PTRACE(1, "MCUFramedAudioCodec\tNo audio channel for read");
    return FALSE;
  }

  unsigned numBytes = samplesPerFrame * channels * 2;

  if(!rawDataChannel->Read(sampleBuffer.GetPointer(samplesPerFrame * channels), numBytes))
  {
    PTRACE(1, "MCUFramedAudioCodec\tAudio read failed: " << rawDataChannel->GetErrorText(PChannel::LastReadError));
    return FALSE;
  }

  length = rawDataChannel->GetLastReadCount();

#if MCU_OPENH323_VERSION < MCU_OPENH323_VERSION_INT(1,26,0)
  for(PINDEX i = 0; i < filters.GetSize(); i++)
  {
    FilterInfo info(*this, sampleBuffer.GetPointer(samplesPerFrame * channels), numBytes, length);
    filters[i](info, 0);
    length = info.bufferLength;
  }
#else
  for(PINDEX i = 0; i < filters.GetSize(); i++)
    length = filters[i].ProcessFilter(sampleBuffer.GetPointer(samplesPerFrame * channels), numBytes, length);
#endif

  if(length != numBytes)
  {
    PTRACE(1, "MCUFramedAudioCodec\tRead truncated frame of raw data. Wanted " << numBytes << " and got "<< length);
    return FALSE;
  }

  if(DetectSilence())
  {
    length = 0;
    return TRUE;
  }

  // Default length is the frame size
  length = bytesPerFrame;

  return EncodeFrame(buffer, length);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL MCUFramedAudioCodec::Write(const BYTE * buffer, unsigned length, const RTP_DataFrame & /*rtpFrame*/, unsigned & written)
{
  PWaitAndSignal mutex(rawChannelMutex);

  if(direction != Decoder)
  {
    PTRACE(1, "MCUFramedAudioCodec\tAttempt to encode from decoder");
    return FALSE;
  }

  // If length is zero then it indicates silence, do nothing.
  written = 0;

  unsigned bytesDecoded = samplesPerFrame * channels * 2;
  PTRACE(9,"MCUFramedAudioCodec\tWrite: length " << length << ", channels " << channels << ", samplesPerFrame " << samplesPerFrame << ", bytesDecoded " << bytesDecoded);

  if(length != 0)
  {
    if(length > bytesPerFrame)
      length = bytesPerFrame;
    written = bytesPerFrame;

    // Decode the data
    if(!DecodeFrame(buffer, length, written, bytesDecoded))
    {
      written = length;
      length = 0;
    }
  }

  // was memset(sampleBuffer.GetPointer(samplesPerFrame), 0, bytesDecoded);
  if(length == 0)
    DecodeSilenceFrame(sampleBuffer.GetPointer(bytesDecoded), bytesDecoded);

  // Write as 16bit PCM to sound channel
  if(IsRawDataHeld)
  {
    // If Connection on Hold
    PProcess::Sleep(5);	// Sleep to avoid CPU Overload <--- Must be a better way but need it to work.
    return TRUE;
  }

  if(rawDataChannel == NULL)
  {
    PTRACE(1, "MCUFramedAudioCodec\tNo audio channel for write");
    return FALSE;
  }

#if MCU_OPENH323_VERSION < MCU_OPENH323_VERSION_INT(1,26,0)
  for(PINDEX i = 0; i < filters.GetSize(); i++)
  {
    FilterInfo info(*this, sampleBuffer.GetPointer(), bytesDecoded, bytesDecoded);
    filters[i](info, 0);
    bytesDecoded = info.bufferLength;
  }
#else
  for(PINDEX i = 0; i < filters.GetSize(); i++)
    length = filters[i].ProcessFilter(sampleBuffer.GetPointer(samplesPerFrame * channels), bytesDecoded, bytesDecoded);
#endif

#if MCU_PTLIB_VERSION < MCU_PTLIB_VERSION_INT(2,9,0)
  if(!rawDataChannel->Write(sampleBuffer.GetPointer(), bytesDecoded))
#else
  if(!rawDataChannel->Write(sampleBuffer.GetPointer(), bytesDecoded, NULL))
#endif
  {
    PTRACE(1, "MCUFramedAudioCodec\tWrite failed: " << rawDataChannel->GetErrorText(PChannel::LastWriteError));
    return FALSE;
  }

  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

MCUStreamedAudioCodec::MCUStreamedAudioCodec(const OpalMediaFormat & fmtName, Direction direction, unsigned samples, unsigned bits, PluginCodec_Definition * _codec)
  : MCUFramedAudioCodec(fmtName, direction, _codec)
{
  samplesPerFrame = samples;
  bytesPerFrame = (samples*bits+7)/8;
  bitsPerSample = bits;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL MCUStreamedAudioCodec::EncodeFrame(BYTE * buffer, unsigned &)
{
  PINDEX i;
  unsigned short position = 0;
  BYTE encoded;
  switch (bitsPerSample) {
    case 8 :
      for (i = 0; i < (PINDEX)samplesPerFrame; i++)
        *buffer++ = (BYTE)Encode(sampleBuffer[i]);
      break;
    case 5 : // g.726-40 payload encoding....
      for (i = 0; i < (PINDEX)samplesPerFrame;i++)
      {
        // based on a 40 bits encoding, we have 8 words of 5 bits each
        encoded = (BYTE)Encode(sampleBuffer[i]);
        switch(position)
        {
          case 0: // 0 bits overflow
            *buffer = encoded;
            position++;
            break;
          case 1: // 2 bits overflow
            *buffer++ |= (encoded << 5);
            *buffer = (BYTE)(encoded >> 3);
            position++;
            break;
          case 2: 
            *buffer |= (encoded << 2);
            position++;
            break;
          case 3: // one bit left for word 4
            *buffer++ |= (encoded << 7);
            *buffer = (BYTE)(encoded >> 1);
            position++;
            break;
          case 4:
            *buffer++ |= (encoded << 4);
            *buffer = (BYTE)(encoded >> 4);
            position++;
            break;
          case 5:
            *buffer |= (encoded << 1);
            position++;
            break;
          case 6: //two bits left for the new encoded word
            *buffer++ |= (encoded << 6);
            *buffer =  (BYTE)(encoded >> 2);
            position++;
            break;
          case 7: // now five bits left for the last word
            *buffer++ |= (encoded << 3);
            position = 0;
            break;
        }
      }
      break;

    case 4 :
      for (i = 0; i < (PINDEX)samplesPerFrame; i++) {
        if ((i&1) == 0)
          *buffer = (BYTE)Encode(sampleBuffer[i]);
        else
          *buffer++ |= (BYTE)(Encode(sampleBuffer[i]) << 4);
      }
      break;

    case 3 :
      for (i = 0;i < (PINDEX)samplesPerFrame;i++)
      {
        encoded = (BYTE)Encode(sampleBuffer[i]);
        switch(position)
        {
          case 0: // 0 bits overflow
            *buffer = encoded;
            position++;
            break;
          case 1: // 2 bits overflow
            *buffer |= (encoded << 3);
            position++;
            break;
          case 2: 
            *buffer++ |= (encoded << 6);
            *buffer = (BYTE)(encoded >> 2);
            position++;
            break;
          case 3: // one bit left for word 4
            *buffer |= (encoded << 1);
            position++;
            break;
          case 4:
            *buffer |= (encoded << 4);
            position++;
            break;
          case 5:
            *buffer++ |= (encoded << 7);
            *buffer = (BYTE)(encoded >> 1);
            position++;
            break;
          case 6: //two bits left for the new encoded word
            *buffer |= (encoded << 2);
            position++;
            break;
          case 7: // now five bits left for the last word
            *buffer++ |= (encoded << 5);
            position = 0;
            break;
        }
      }
      break;

    case 2:
      for (i = 0; i < (PINDEX)samplesPerFrame; i++) 
      {
        switch(position)
        {
          case 0:
            *buffer = (BYTE)Encode(sampleBuffer[i]);
            position++;
            break;
          case 1:
            *buffer |= (BYTE)(Encode(sampleBuffer[i]) << 2);
            position++;
            break;
          case 2:
            *buffer |= (BYTE)(Encode(sampleBuffer[i]) << 4);
            position++;
            break;
          case 3:
            *buffer++ |= (BYTE)(Encode(sampleBuffer[i]) << 6);
            position = 0;
            break;
        }
      }
      break;

    default :
      PAssertAlways("Unsupported bit size");
      return FALSE;
  }

  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL MCUStreamedAudioCodec::DecodeFrame(const BYTE * buffer, unsigned length, unsigned & written, unsigned & decodedBytes)
{
  unsigned i;
  short * sampleBufferPtr = sampleBuffer.GetPointer(samplesPerFrame);
  short * out = sampleBufferPtr;
  unsigned short position = 0;
  unsigned remaining = 0;

  switch (bitsPerSample) {
    case 8 :
      for (i = 0; i < length; i++)
        *out++ = Decode(*buffer++);
      break;

    // those case are for ADPCM G.726
    case 5 :
      for (i = 0; i < length; i++) {
        switch(position)
        {
          case 0:
            *out++ = Decode(*buffer & 31);
            remaining = *buffer >> 5; // get the three remaining bytes for the next word
            buffer++;
            position++;
            break;
          case 1: // we can decode more than one word in second buffer
            *out++ = Decode (((*buffer&3) << 3) | remaining);
            *out++ = Decode( (*buffer >> 2) & 31);
            remaining = *buffer >> 7;
            buffer++;
            position++;
            break;
          case 2:
            *out++ = Decode( remaining | ((*buffer&15) << 1));
            remaining = *buffer >> 4;
            buffer++;
            position++;
            break;
          case 3:
            *out++ = Decode( remaining | ((*buffer&1) << 4));
            *out++ = Decode( (*buffer >> 1) & 31);
            remaining = *buffer >> 6;
            buffer++;
            position++;
            break;
          case 4 :
            *out++ = Decode( remaining | ((*buffer&7) << 2));
            *out++ = Decode(*buffer >> 3);
            buffer++;
            position = 0;
            break;
        }
      }
      break;

    case 4 :
      for (i = 0; i < length; i++) {
        *out++ = Decode(*buffer & 15);
        *out++ = Decode(*buffer >> 4);
        buffer++;
      }
      break;

    case 3:
      for (i = 0; i < length; i++) {
        switch(position)
        {
        case 0:
          *out++ = Decode(*buffer & 7);
          *out++ = Decode((*buffer>>3)&7);
          remaining = *buffer >> 6;
          buffer++;
          position++;
          break;
        case 1:
          *out++ = Decode(remaining | ((*buffer&1) << 2));
          *out++ = Decode((*buffer >> 1) & 7);
          *out++ = Decode((*buffer >> 4)&7);
          remaining = *buffer >> 7;
          buffer++;
          position++;
          break;
        case 2:
          *out++ = Decode(remaining | ((*buffer&3) << 1));
          *out++ = Decode((*buffer >> 2) & 7);
          *out++ = Decode((*buffer >> 5) & 7);
          buffer++;
          position = 0;
          break;
        }
      }
      break;

    case 2:
      for (i = 0; i < length; i++) 
      {
        *out++ = Decode(*buffer & 3);
        *out++ = Decode((*buffer >> 2) & 3);
        *out++ = Decode((*buffer >> 4) & 3);
        *out++ = Decode((*buffer >> 6) & 3);
        buffer++;
      }
      break;

    default :
      PAssertAlways("Unsupported bit size");
      return FALSE;
  }

  written = length;
  decodedBytes = (out - sampleBufferPtr)*2;
  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int MCUStreamedAudioCodec::Encode(short sample) const
{
  if(codec == NULL || direction != Encoder)
    return 0;
  unsigned int fromLen = sizeof(sample);
  int to;
  unsigned toLen = sizeof(to);
  unsigned flags = 0;
  (codec->codecFunction)(codec, context, (const unsigned char *)&sample, &fromLen, (unsigned char *)&to, &toLen, &flags);
   return to;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

short MCUStreamedAudioCodec::Decode(int sample) const
{
  if(codec == NULL || direction != Decoder)
    return 0;
  unsigned fromLen = sizeof(sample);
  short to;
  unsigned toLen   = sizeof(to);
  unsigned flags = 0;
  (codec->codecFunction)(codec, context,(const unsigned char *)&sample, &fromLen, (unsigned char *)&to, &toLen, &flags);
  return to;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

MCUVideoCodec::MCUVideoCodec(const OpalMediaFormat & fmt, Direction direction, PluginCodec_Definition * _codec)
  : H323VideoCodec(fmt, direction), codec(_codec)
{
  if(codec != NULL && codec->createCodec != NULL)
    context = (*codec->createCodec)(codec);
  else
    context = NULL;

  frameWidth = mediaFormat.GetOptionInteger(OpalVideoFormat::FrameWidthOption); 
  frameHeight = mediaFormat.GetOptionInteger(OpalVideoFormat::FrameHeightOption);
  maxWidth = 2048;
  maxHeight = 2048;
  targetFrameTimeMs = 40;
  lastPacketSent = true;
  lastFrameTimeRTP = 0;
  sendIntra = true;
  converter = NULL;

  // Need to allocate buffer to the maximum framesize statically
  // and clear the memory in the destructor to avoid segfault in destructor
  bytesPerFrame = (maxHeight * maxWidth * 3)/2;
  bufferRTP = RTP_DataFrame(sizeof(PluginCodec_Video_FrameHeader) + bytesPerFrame, TRUE);

  if(context == NULL)
  {
    PTRACE(1, "MCUVideoCodec\tFailed create codec " << mediaFormat);
    return;
  }

  PluginCodec_ControlDefn * ctl = GetCodecControl(codec, SET_CODEC_OPTIONS_CONTROL);
  if(ctl != NULL)
  {
    PStringArray list;
    for(PINDEX i = 0; i < mediaFormat.GetOptionCount(); i++)
    {
      const OpalMediaOption & option = mediaFormat.GetOption(i);
      list += option.GetName();
      list += option.AsString();
      PTRACE(5, "OpalPlugin\tSetting codec option '" << option.GetName() << "'=" << option.AsString());
    }
    char ** _options = list.ToCharArray();
    unsigned int optionsLen = sizeof(_options);
    (*ctl->control)(codec, context, SET_CODEC_OPTIONS_CONTROL, _options, &optionsLen);

    for(int i = 0; _options[i] != NULL; i += 2)
    {
      if(strcasecmp(_options[i], "Frame Width") == 0)
        frameWidth = atoi(_options[i+1]);
      if(strcasecmp(_options[i], "Frame Height") == 0)
        frameHeight = atoi(_options[i+1]);
      if(strcasecmp(_options[i], "Frame Time") == 0)
        targetFrameTimeMs = atoi(_options[i+1]);
    }
    free(_options);
  }

  if(targetFrameTimeMs > 1000)
    targetFrameTimeMs = 40; // for h.263 codecs

  // Полученные значение из кодека
  mediaFormat.SetOptionInteger(OPTION_FRAME_WIDTH, frameWidth);
  mediaFormat.SetOptionInteger(OPTION_FRAME_HEIGHT, frameHeight);

#if PTRACING
  PTRACE(6,"Codec Options");
  OpalMediaFormat::DebugOptionList(mediaFormat);
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////

MCUVideoCodec::~MCUVideoCodec()
{
  PWaitAndSignal mutex(videoHandlerActive);

  // Set the buffer memory to zero to prevent
  // memory leak
  bufferRTP.SetSize(0);

  if(codec != NULL && codec->destroyCodec != NULL)
    (*codec->destroyCodec)(codec, context);
  codec = NULL;

  if(converter != NULL)
    delete converter;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL MCUVideoCodec::Read(BYTE * buffer, unsigned & length, RTP_DataFrame & dst)
{
  unsigned flags = 0;
  return Read(buffer, length, dst, flags);
}

BOOL MCUVideoCodec::Read(BYTE * buffer, unsigned & length, RTP_DataFrame & dst, unsigned & flags)
{
  PWaitAndSignal mutex(videoHandlerActive);

  if(direction != Encoder)
  {
    PTRACE(1, "MCUVideoCodec\tAttempt to decode from encoder");
    return FALSE;
  }

  if(rawDataChannel == NULL)
  {
    PTRACE(1, "MCUVideoCodec\tNo channel to grab from, close down video transmission thread");
    return FALSE;
  }

  PVideoChannel *videoIn = (PVideoChannel *)rawDataChannel;

  PluginCodec_Video_FrameHeader * frameHeader = (PluginCodec_Video_FrameHeader *)bufferRTP.GetPayloadPtr();
  frameHeader->x = 0;
  frameHeader->y = 0;
  frameHeader->width = videoIn->GetGrabWidth();
  frameHeader->height = videoIn->GetGrabHeight();

  if(frameHeader->width == 0 || frameHeader->height == 0)
  {
    PTRACE(1,"MCUVideoCodec\tVideo grab dimension is 0, close down video transmission thread");
    videoIn->EnableAccess();
    return FALSE;
  }

  if(lastPacketSent)
  {
    videoIn->RestrictAccess();

    if(!videoIn->IsGrabberOpen())
    {
      PTRACE(1, "MCUVideoCodec\tVideo grabber is not initialised, close down video transmission thread");
      videoIn->EnableAccess();
      return FALSE;
    }

    if(!SetFrameSize(frameHeader->width, frameHeader->height))
    {
      PTRACE(1, "MCUVideoCodec\tFailed to resize, close down video transmission thread");
      videoIn->EnableAccess();
      return FALSE;
    }

    unsigned char * data = OPAL_VIDEO_FRAME_DATA_PTR(frameHeader);
    unsigned byteRead = (frameHeader->width * frameHeader->height * 3) / 2;
    if(!rawDataChannel->Read(data, byteRead))
    {
      PTRACE(3, "MCUVideoCodec\tFailed to read data from video grabber");
      videoIn->EnableAccess();
      length = 0;
      dst.SetPayloadSize(0);
      return TRUE; // and hope the error condition will fix itself
    }

    videoIn->EnableAccess();

    RenderFrame(data);

    PTimeInterval now = PTimer::Tick();
    if(lastFrameTick != 0)
      lastFrameTimeRTP = (now - lastFrameTick).GetInterval() * 90;
    lastFrameTick = now;
  }
  else
    lastFrameTimeRTP = 0;

//  frameHeader->width = frameWidth;
//  frameHeader->height = frameHeight;

  // get the size of the output buffer
  int outputDataSize;
  if(codec == NULL || !CallCodecControl(codec, context, GET_OUTPUT_DATA_SIZE_CONTROL, NULL, NULL, outputDataSize))
    outputDataSize = 1518-14-4-8-20-16; // Max Ethernet packet (1518 bytes) minus 802.3/CRC, 802.3, IP, UDP headers

  dst.SetMinSize(outputDataSize);

  unsigned int fromLen = bufferRTP.GetHeaderSize() + bufferRTP.GetPayloadSize();
  unsigned int toLen = outputDataSize;
  flags = sendIntra ? PluginCodec_CoderForceIFrame : 0;
  int retval = 0;

  retval = (codec->codecFunction)(codec, context, bufferRTP.GetPointer(), &fromLen, dst.GetPointer(), &toLen, &flags);

  if(retval == 0 && codec != NULL)
  {
    PTRACE(3,"MCUVideoCodec\tError encoding frame from plugin " << codec->descr);
    length = 0;
    return FALSE;
  }

  if((flags & PluginCodec_ReturnCoderIFrame) != 0)
  {
    //PTRACE(sendIntra ? 3 : 5,"MCUVideoCodec\tSent I-Frame" << (sendIntra ? ", in response to VideoFastUpdate" : ""));
    sendIntra = false;
  }

  if(toLen > 0)
    length = toLen - dst.GetHeaderSize();
  else
    length = 0;

  lastPacketSent = (flags & PluginCodec_ReturnCoderLastFrame);

  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL MCUVideoCodec::Write(const BYTE * buffer, unsigned length, const RTP_DataFrame & src, unsigned & written)
{
  PWaitAndSignal mutex(videoHandlerActive);

  if(direction != Decoder)
  {
    PTRACE(1, "MCUVideoCodec\tAttempt to decode from decoder");
    return FALSE;
  }

  if(rawDataChannel == NULL)
  {
    PTRACE(1, "MCUVideoCodec\tNo channel to render to, close down video reception thread");
    return FALSE;
  }

  if(length == 0)
  {
    written = length;
    return TRUE;
  }

  // get the size of the output buffer
  int outputDataSize;
  if (!CallCodecControl(codec, context, GET_OUTPUT_DATA_SIZE_CONTROL, NULL, NULL, outputDataSize))
    return FALSE;

  bufferRTP.SetMinSize(outputDataSize);

  unsigned int fromLen = src.GetHeaderSize() + src.GetPayloadSize();
  unsigned int toLen = bufferRTP.GetSize();
  unsigned int flags = 0;

  int retval = (codec->codecFunction)(codec, context, (const BYTE *)src, &fromLen,
                                      bufferRTP.GetPointer(toLen), &toLen,
                                      &flags);

  if(retval == 0)
  {
    PTRACE(3,"MCUVideoCodec\tError decoding frame from plugin " << codec->descr);
    return FALSE;
  }

  if(flags & PluginCodec_ReturnCoderRequestIFrame)
  {
    PTRACE(6,"MCUVideoCodec\tIFrame Request Decoder: Unimplemented.");
    ((MCU_RTPChannel *)logicalChannel)->SendMiscCommand(H245_MiscellaneousCommand_type::e_videoFastUpdatePicture);
  }

  if(toLen < (unsigned)bufferRTP.GetHeaderSize())
  {
    PTRACE(9,"MCUVideoCodec\tPartial Frame received " << codec->descr << " Ignoring rendering.");
    written = length;
    return TRUE;
  }

  PluginCodec_Video_FrameHeader * frameHeader = (PluginCodec_Video_FrameHeader *)(bufferRTP.GetPayloadPtr());

  if(frameHeader->width != (unsigned int)frameWidth || frameHeader->height != (unsigned int)frameHeight)
  {
    PVideoChannel *videoOut = (PVideoChannel *)rawDataChannel;
    SetFrameSize(frameHeader->width, frameHeader->height);
    videoOut->SetRenderFrameSize(frameWidth, frameHeight);
  }

  if(flags & PluginCodec_ReturnCoderLastFrame)
  {
    SetFrameSize(frameHeader->width, frameHeader->height);
    RenderFrame(OPAL_VIDEO_FRAME_DATA_PTR(frameHeader));
  }

  written = length;

  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL MCUVideoCodec::RenderFrame(const BYTE * buffer)
{
  PVideoChannel *videoOut = (PVideoChannel *)rawDataChannel;

  if(!videoOut->IsRenderOpen())
    return TRUE;

  //videoOut->SetRenderFrameSize(frameWidth, frameHeight);
  //videoOut->SetRenderFrameSize(videoOut->GetGrabWidth(), videoOut->GetGrabHeight());

  PTRACE(9, "MCUVideoCodec\tWrite data to video renderer");
  return videoOut->Write(buffer, 0 /*unused parameter*/);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL MCUVideoCodec::SetFrameSize(int _width, int _height)
{
  if(frameWidth == _width && frameHeight == _height)
    return TRUE;

  if(_width == 0 || _height == 0)
    return FALSE;

  if(_width > maxWidth || _height > maxHeight)
  {
    PTRACE(3, "MCUVideoCodec\tERROR: Frame Size " << _width << "x" << _height  << " exceeds codec limits " << maxWidth << "x" << maxHeight); 
    return FALSE;
  }

  frameWidth = _width;
  frameHeight = _height;

  PTRACE(3,"MCUVideoCodec\tResize to width:" << frameWidth << " height:" << frameHeight);

  mediaFormat.SetOptionInteger(OpalVideoFormat::FrameWidthOption, frameWidth);
  mediaFormat.SetOptionInteger(OpalVideoFormat::FrameHeightOption, frameHeight);

  bytesPerFrame = (frameWidth * frameHeight * 3) / 2;

  if(direction == Encoder)
  {
    bufferRTP.SetPayloadSize(sizeof(PluginCodec_Video_FrameHeader) + bytesPerFrame);
    PluginCodec_Video_FrameHeader * header = (PluginCodec_Video_FrameHeader *)(bufferRTP.GetPayloadPtr());
    header->x = header->y = 0;
    header->width = frameWidth;
    header->height = frameHeight;
  }

  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

