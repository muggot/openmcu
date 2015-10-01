
#include "precompile.h"
#include "mcu.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

PString GetRecorderCodecs(int media_type)
{
  PString codecs = "Disabled";
  if(media_type == 0)
  {
    if(avcodec_find_encoder_by_name("ac3"))
      codecs += ",ac3";
    if(avcodec_find_encoder_by_name("pcm_s16le"))
      codecs += ",pcm_s16le";
  } else {
    if(avcodec_find_encoder_by_name("libx264"))
      codecs += ",libx264";
    if(avcodec_find_encoder_by_name("libvpx"))
      codecs += ",libvpx";
    if(avcodec_find_encoder_by_name("mpeg4"))
      codecs += ",mpeg4";
    if(avcodec_find_encoder_by_name("msmpeg4"))
      codecs += ",msmpeg4";
  }
  return codecs;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

AVCodecID GetCodecId(int media_type, PString codec_name)
{
  if(codec_name == "")
  {
    if(media_type == 0)
      return AV_CODEC_ID_AC3;
    else
      return AV_CODEC_ID_MPEG4;
  }

  AVCodec *codec = avcodec_find_encoder_by_name(codec_name);
  if(codec)
    return codec->id;

  return AV_CODEC_ID_NONE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

ConferenceRecorder::ConferenceRecorder(Conference *_conference)
  : ConferenceMember(_conference)
{
  trace_section = "ConferenceRecorder: ";
  memberType = MEMBER_TYPE_RECORDER;
  Reset();
  MCUTRACE(1, trace_section << "create");
}

////////////////////////////////////////////////////////////////////////////////////////////////////

ConferenceRecorder::~ConferenceRecorder()
{
  Stop();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void ConferenceRecorder::Close()
{
  PWaitAndSignal m(mutex);
  Stop();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void ConferenceRecorder::Reset()
{
  running = FALSE;
  thread_audio = NULL;
  thread_video = NULL;

  audio_frame_count = 0;
  video_frame_count = 0;

#if USE_SWRESAMPLE || USE_AVRESAMPLE
  swrc = NULL;
#endif

  dst_samples_data = NULL;
  src_samples_data = NULL;

  video_framebuf = NULL;
  video_outbuf = NULL;

  audio_frame = NULL;
  video_frame = NULL;

  audio_st = NULL;
  video_st = NULL;

  fmt_context = NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void ConferenceRecorder::Stop()
{
  PWaitAndSignal m(mutex);

  running = FALSE;
  if(thread_audio)
  {
    thread_audio->WaitForTermination(10000);
    delete thread_audio;
  }
  if(thread_video)
  {
    thread_video->WaitForTermination(10000);
    delete thread_video;
  }

  if(fmt_context && (thread_audio || thread_video))
  {
    // write the trailer
    av_write_trailer(fmt_context);
  }

  avcodecMutex.Wait();
  if(audio_st)
    avcodec_close(audio_st->codec);
  if(video_st)
    avcodec_close(video_st->codec);
  avcodecMutex.Signal();

  if(fmt_context)
  {
    // close the output file
    if(fmt_context->pb && fmt_context->oformat && !(fmt_context->oformat->flags & AVFMT_NOFILE))
      avio_close(fmt_context->pb);

    // free the stream
    avformat_free_context(fmt_context);
  }

#if USE_SWRESAMPLE
  if(swrc)
    swr_free(&swrc);
#elif USE_AVRESAMPLE
  if(swrc)
    avresample_free(&swrc);
#endif

  if(dst_samples_data)
  {
    if(src_samples_data && src_samples_data[0] != dst_samples_data[0])
      av_free(dst_samples_data[0]);
    av_free(dst_samples_data);
  }
  if(src_samples_data)
  {
    av_free(src_samples_data[0]);
    av_free(src_samples_data);
  }

  if(video_framebuf)
    av_free(video_framebuf);
  if(video_outbuf)
    av_free(video_outbuf);

  if(audio_frame)
    AVFrameFree(&audio_frame);
  if(video_frame)
    AVFrameFree(&video_frame);

  Reset();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL ConferenceRecorder::Start()
{
  PWaitAndSignal m(mutex);

  if(conference == NULL)
    return FALSE;

  if(IsRunning() == TRUE)
    return TRUE;

  OpenMCU & mcu = OpenMCU::Current();
  MCUConfig cfg("Parameters");

  // video
  PString res = cfg.GetString(RecorderResolutionKey, PString(DefaultRecorderFrameWidth)+"x"+PString(DefaultRecorderFrameHeight));
  video_width = res.Tokenise("x")[0].AsInteger();
  video_height = res.Tokenise("x")[1].AsInteger();
  video_framerate = cfg.GetInteger(RecorderFrameRateKey, DefaultRecorderFrameRate);

  unsigned max_fs = GetVideoMacroBlocks(video_width, video_height);
  for(int i = 0; recorder_resolutions[i].macroblocks != 0; ++i)
  {
    if(max_fs > recorder_resolutions[i].macroblocks && recorder_resolutions[i+1].macroblocks != 0)
      continue;

    if(video_width != recorder_resolutions[i].width || video_height != recorder_resolutions[i].height)
    { PTRACE(1, trace_section << "resolution changed to " << video_width << "x" << video_height); }

    video_width = recorder_resolutions[i].width;
    video_height = recorder_resolutions[i].height;
    break;
  }

  if(video_framerate < 1)       { video_framerate = 1; PTRACE(1, trace_section << "resolution changed to 1"); }
  else if(video_framerate > 30) { video_framerate = 30; PTRACE(1, trace_section << "resolution changed to 30"); }

  video_bitrate = cfg.GetInteger(RecorderVideoBitrateKey);
  if(video_bitrate == 0) video_bitrate = video_width*video_height*video_framerate/10000;
  if(video_bitrate < 64) video_bitrate = 64;
  else if(video_bitrate > 4000) video_bitrate = 4000;

  // audio
  audio_samplerate = cfg.GetInteger(RecorderSampleRateKey, DefaultRecorderSampleRate);
  audio_channels = cfg.GetInteger(RecorderAudioChansKey, DefaultRecorderAudioChans);
  if(audio_samplerate < 8000)       { audio_samplerate = 8000; PTRACE(1, trace_section << "sample rate changed to 8000"); }
  else if(audio_samplerate > 48000) { audio_samplerate = 48000; PTRACE(1, trace_section << "sample rate changed to 48000"); }
  if(audio_channels < 1)      { audio_channels = 1; PTRACE(1, trace_section << "audio channels changed to 1"); }
  else if(audio_channels > 8) { audio_channels = 8; PTRACE(1, trace_section << "audio channels changed to 8"); }
  audio_bitrate = 64;

  // codecs
  audio_codec_id = GetCodecId(0, cfg.GetString(RecorderAudioCodecKey, RecorderDefaultAudioCodec));
  video_codec_id = GetCodecId(1, cfg.GetString(RecorderVideoCodecKey, RecorderDefaultVideoCodec));
  if(audio_codec_id == AV_CODEC_ID_NONE && video_codec_id == AV_CODEC_ID_NONE)
  {
    MCUTRACE(1, trace_section << "failed initialise recorder, codecs not found");
    return FALSE;
  }

  // file format: room101__2013-0516-1058270__704x576x10
  PStringStream t;
  t << mcu.vr_ffmpegDir << PATH_SEPARATOR
    << conference->GetNumber()
    << "__" << PTime().AsString("yyyy-MMdd-hhmmssu", PTime::Local) << "__"
    << video_width << "x"
    << video_height << "x"
    << video_framerate;
  filename = t;
  if((video_codec_id == AV_CODEC_ID_MPEG4 || video_codec_id == AV_CODEC_ID_MSMPEG4V3) && audio_codec_id != AV_CODEC_ID_PCM_S16LE)
    format_name = "asf";
  else
    format_name = "mkv";
  filename += "."+format_name;

  //
  if(InitRecorder() == FALSE)
  {
    MCUTRACE(1, trace_section << "failed initialise recorder");
    Stop();
    return FALSE;
  }

  startTime = PTime();

  if(audio_st)
    thread_audio = PThread::Create(PCREATE_NOTIFIER(RecorderAudio), 0, PThread::NoAutoDeleteThread, PThread::NormalPriority, "conference_recorder:%0x");
  if(video_st)
    thread_video = PThread::Create(PCREATE_NOTIFIER(RecorderVideo), 0, PThread::NoAutoDeleteThread, PThread::NormalPriority, "conference_recorder:%0x");

  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL ConferenceRecorder::InitRecorder()
{
  int ret = 0;

  // Initialize libavcodec, and register all codecs and formats
  av_register_all();

  // allocate the output media context
  fmt_context = avformat_alloc_context();
  fmt_context->oformat = av_guess_format(format_name, filename, NULL);
  if(fmt_context->oformat == NULL)
  {
    MCUTRACE(1, trace_section << "could not allocate the output context");
    return FALSE;
  }

  fmt_context->oformat->audio_codec = audio_codec_id;
  fmt_context->oformat->video_codec = video_codec_id;

  audio_st = AddStream(AVMEDIA_TYPE_AUDIO);
  video_st = AddStream(AVMEDIA_TYPE_VIDEO);

  if(audio_st && OpenAudio() == FALSE)
    return FALSE;
  if(video_st && OpenVideo() == FALSE)
    return FALSE;

  av_dump_format(fmt_context, 0, filename, 1);

  // open the output file
  ret = avio_open(&fmt_context->pb, filename, AVIO_FLAG_WRITE);
  if(ret < 0)
  {
    MCUTRACE(1, trace_section << "could not open " << filename << " " << ret << " " << AVErrorToString(ret));
    return FALSE;
  }

  // write the stream header
  ret = avformat_write_header(fmt_context, NULL);
  if(ret < 0)
  {
    MCUTRACE(1, trace_section << "error occurred when opening output file: " << ret << " " << AVErrorToString(ret));
    return FALSE;
  }

  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

AVStream * ConferenceRecorder::AddStream(AVMediaType codec_type)
{
  AVStream *st = NULL;
  AVCodecContext *context = NULL;
  AVCodec *codec = NULL;

  // find the encoder
  if(codec_type == AVMEDIA_TYPE_AUDIO)
    codec = avcodec_find_encoder(fmt_context->oformat->audio_codec);
  else if(codec_type == AVMEDIA_TYPE_VIDEO)
    codec = avcodec_find_encoder(fmt_context->oformat->video_codec);

  if(codec == NULL)
  {
    MCUTRACE(1, trace_section << "could not find encoder for " << (codec_type == AVMEDIA_TYPE_AUDIO ? "audio" : "video"));
    return NULL;
  }

  st = avformat_new_stream(fmt_context, codec);
  if(st == NULL)
  {
    MCUTRACE(1, trace_section << "could not allocate stream");
    return NULL;
  }
  st->id = fmt_context->nb_streams-1;
  context = st->codec;
  context->codec = codec;

  if(codec_type == AVMEDIA_TYPE_AUDIO)
  {
    if(codec->id == AV_CODEC_ID_PCM_S16LE)
      context->sample_fmt  = AV_SAMPLE_FMT_S16;
    else
#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(55, 0, 0)
      context->sample_fmt  = AV_SAMPLE_FMT_FLT;
#else
      context->sample_fmt  = AV_SAMPLE_FMT_FLTP;
#endif
    context->codec_id      = codec->id;
    context->bit_rate      = audio_bitrate * audio_channels * 1000;
    context->sample_rate   = audio_samplerate;
    context->channels      = audio_channels;
    context->channel_layout = MCU_AV_CH_Layout_Selector[context->channels];
    context->time_base.num = 12 * 16000 / context->sample_rate;
    context->time_base.den = 125;
    //context->strict_std_compliance = -2;
  }
  else if(codec_type == AVMEDIA_TYPE_VIDEO)
  {
    context->pix_fmt       = AV_PIX_FMT_YUV420P;
    context->codec_id      = codec->id;
    context->bit_rate      = video_bitrate*1000;
    context->width         = video_width;
    context->height        = video_height;
    context->qmin          = 2;
    context->qmax          = 31;
    context->gop_size      = 12;
    context->time_base.num = 1;
    context->time_base.den = video_framerate;

    if(context->codec->id == AV_CODEC_ID_H264)
    {
      context->profile = FF_PROFILE_H264_BASELINE;
      av_opt_set(context->priv_data, "preset", "ultrafast", 0);
      av_opt_set(context->priv_data, "tune", "zerolatency", 0);
    }
    if(context->codec->id == AV_CODEC_ID_VP8)
    {
      av_opt_set(context->priv_data, "speed", "-5", 0);
    }
  }

  // Some formats want stream headers to be separate
  if(fmt_context->oformat->flags & AVFMT_GLOBALHEADER)
    context->flags |= CODEC_FLAG_GLOBAL_HEADER;

  return st;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int ConferenceRecorder::WritePacket(AVStream *st, AVPacket *pkt)
{
  int ret = 0;

  pkt->stream_index = st->index;
  pkt->pts = av_rescale_q(pkt->pts, st->codec->time_base, st->time_base);
  pkt->dts = AV_NOPTS_VALUE;

  // write the compressed frame to the media file
  PWaitAndSignal m(write_mutex);
  ret = av_interleaved_write_frame(fmt_context, pkt);

  //double audio_time = audio_st ? audio_st->pts.val * av_q2d(audio_st->time_base) : 0.0;
  //double video_time = video_st ? video_st->pts.val * av_q2d(video_st->time_base) : 0.0;
  //if(audio_st) cout << "audio=" << audio_time << " " << av_q2d(audio_st->time_base) << " " << av_q2d(audio_st->codec->time_base) << "\n";
  //if(video_st) cout << "video=" << video_time << " " << av_q2d(video_st->time_base) << " " << av_q2d(video_st->codec->time_base) << "\n";

  return ret;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL ConferenceRecorder::OpenAudio()
{
  AVCodecContext *context = audio_st->codec;
  int ret = 0;

  // open codec
  avcodecMutex.Wait();
  ret = avcodec_open2(context, context->codec, NULL);
  avcodecMutex.Signal();
  if(ret < 0)
  {
    MCUTRACE(1, trace_section << "could not open audio codec: " << ret << " " << AVErrorToString(ret));
    return FALSE;
  }

  audio_frame = AVFrameAlloc();
  if(audio_frame == NULL)
  {
    MCUTRACE(1, trace_section << "could not allocate audio frame");
    return FALSE;
  }

  return OpenResampler();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL ConferenceRecorder::OpenResampler()
{
  AVCodecContext *context = audio_st->codec;
  int ret = 0;

  // check resampler
  if(USE_SWRESAMPLE == 0 && USE_AVRESAMPLE == 0 && context->sample_fmt == AV_SAMPLE_FMT_FLTP)
  {
    MCUTRACE(1, trace_section << "resampler not found");
    return FALSE;
  }

  // set frame size
  if(context->frame_size == 0)
    context->frame_size = context->sample_rate * av_q2d(audio_st->codec->time_base);

  src_samples = context->frame_size;
  src_samples_size = av_samples_get_buffer_size(NULL, context->channels, src_samples, AV_SAMPLE_FMT_S16, AV_ALIGN);
  src_samples_data = (uint8_t **)av_malloc(context->channels * sizeof(uint8_t *));
  ret = av_samples_alloc(src_samples_data, NULL, context->channels, src_samples, AV_SAMPLE_FMT_S16, AV_ALIGN);
  if(ret < 0)
  {
    MCUTRACE(1, trace_section << "could not allocate source samples: " << src_samples << " " << ret << " " << AVErrorToString(ret));
    return FALSE;
  }

#if USE_SWRESAMPLE || USE_AVRESAMPLE
  // create resampler context
  if(context->sample_fmt == AV_SAMPLE_FMT_FLTP)
  {
#if USE_SWRESAMPLE
    swrc = swr_alloc();
#elif USE_AVRESAMPLE
    swrc = avresample_alloc_context();
#endif
    if(swrc == NULL)
    {
      MCUTRACE(1, trace_section << "could not allocate resampler context");
      return FALSE;
    }

    // set options
    av_opt_set_int(swrc, "in_sample_fmt",       AV_SAMPLE_FMT_S16,        0);
    av_opt_set_int(swrc, "in_sample_rate",      context->sample_rate,     0);
    av_opt_set_int(swrc, "in_channel_count",    context->channels,        0);
    av_opt_set_int(swrc, "in_channel_layout",   context->channel_layout,  0);
    av_opt_set_int(swrc, "out_sample_fmt",      context->sample_fmt,      0);
    av_opt_set_int(swrc, "out_sample_rate",     context->sample_rate,     0);
    av_opt_set_int(swrc, "out_channel_count",   context->channels,        0);
    av_opt_set_int(swrc, "out_channel_layout",  context->channel_layout,  0);

    // initialize the resampling context
#if USE_SWRESAMPLE
    ret = swr_init(swrc);
#elif USE_AVRESAMPLE
    ret = avresample_open(swrc);
#endif
    if(ret < 0)
    {
      MCUTRACE(1, trace_section << "failed to initialize the resampling context: " << ret << " " << AVErrorToString(ret));
      return FALSE;
    }
  }
#endif

  // allocate destination samples
  dst_samples = src_samples;
  dst_samples_size = av_samples_get_buffer_size(NULL, context->channels, dst_samples, context->sample_fmt, AV_ALIGN);
  dst_samples_data = (uint8_t **)av_malloc(context->channels * sizeof(uint8_t *));
  ret = av_samples_alloc(dst_samples_data, NULL, context->channels, dst_samples, context->sample_fmt, AV_ALIGN);
  if(ret < 0)
  {
    MCUTRACE(1, trace_section << "could not allocate destination samples: " << dst_samples << " " << ret << " " << AVErrorToString(ret));
    return FALSE;
  }
  if(context->sample_fmt == AV_SAMPLE_FMT_S16)
  {
    av_free(dst_samples_data[0]);
  }

  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL ConferenceRecorder::Resampler()
{
  AVCodecContext *context = audio_st->codec;
  int ret = 0;

  // convert to destination format
  if(context->sample_fmt == AV_SAMPLE_FMT_S16)
  {
    dst_samples = src_samples;
    dst_samples_size = src_samples_size;
    dst_samples_data[0] = src_samples_data[0];
  } else if(context->sample_fmt == AV_SAMPLE_FMT_FLT) {
    for(int i = 0; i < src_samples * context->channels; ++i)
      ((float *)dst_samples_data[0])[i] = ((int16_t *)src_samples_data[0])[i] * (1.0 / (1<<15));
  } else {
    // convert samples from native format to destination codec format, using the resampler
#if USE_SWRESAMPLE
    ret = swr_convert(swrc, dst_samples_data, dst_samples, (const uint8_t **)src_samples_data, src_samples);
#elif USE_AVRESAMPLE
    ret = avresample_convert(swrc, dst_samples_data, dst_samples_size, dst_samples, (uint8_t **)src_samples_data, src_samples_size, src_samples);
#endif
    if(ret < 0)
    {
      MCUTRACE(1, trace_section << "error while converting: " << ret << " " << AVErrorToString(ret));
      return FALSE;
    }
  }

  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL ConferenceRecorder::GetAudioFrame(const uint64_t & timestamp)
{
  AVCodecContext *context = audio_st->codec;

  ReadAudio(timestamp, src_samples_data[0], src_samples_size, context->sample_rate, context->channels);
  if(Resampler() == FALSE)
    return FALSE;

  audio_frame->nb_samples = dst_samples;
  avcodec_fill_audio_frame(audio_frame, context->channels, context->sample_fmt, dst_samples_data[0], dst_samples_size, AV_ALIGN);
  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL ConferenceRecorder::WriteAudio(const uint64_t & timestamp)
{
  AVCodecContext *context = audio_st->codec;
  int ret = 0, got_packet = 0;

  AVPacket pkt = { 0 };
  av_init_packet(&pkt);

  // first, increase the counter, set PTS for packet
  audio_frame->pts = ++audio_frame_count;

  if(GetAudioFrame(timestamp) == FALSE)
    return FALSE;

  ret = avcodec_encode_audio2(context, &pkt, audio_frame, &got_packet);
  if(ret < 0)
  {
    MCUTRACE(1, trace_section << "error encoding audio frame: " << ret << " " << AVErrorToString(ret));
    return FALSE;
  }

  ret = WritePacket(audio_st, &pkt);
  if(ret < 0)
  {
    MCUTRACE(1, trace_section << "error while writing audio frame: " << ret << " " <<  AVErrorToString(ret));
    return FALSE;
  }

  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL ConferenceRecorder::OpenVideo()
{
  AVCodecContext *context = video_st->codec;
  int ret = 0;

  // open the codec
  avcodecMutex.Wait();
  ret = avcodec_open2(context, context->codec, NULL);
  avcodecMutex.Signal();
  if(ret < 0)
  {
    MCUTRACE(1, trace_section << "could not open video codec: " << ret << " " << AVErrorToString(ret));
    return FALSE;
  }

  video_frame = AVFrameAlloc();
  if(video_frame == NULL)
  {
    MCUTRACE(1, trace_section << "could not allocate video frame");
    return FALSE;
  }

  video_frame->width = context->width;
  video_frame->height = context->height;
  video_frame->format = context->pix_fmt;

  // initialize frame->linesize
  avpicture_fill((AVPicture*)video_frame, NULL, context->pix_fmt, context->width, context->height);

  // allocate a buffer large enough for all data
  video_framebuf_size = avpicture_get_size(context->pix_fmt, context->width, context->height);
  video_framebuf = (uint8_t *)av_malloc(video_framebuf_size);
  video_outbuf_size = 200000;
  video_outbuf = (uint8_t *)av_malloc(video_outbuf_size);

  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL ConferenceRecorder::GetVideoFrame()
{
  AVCodecContext *context = video_st->codec;

  // read a block of data
  conference->ReadMemberVideo(this, video_framebuf, context->width, context->height, video_framebuf_size);
  // fill frame
  avpicture_fill((AVPicture*)video_frame, video_framebuf, AV_PIX_FMT_YUV420P, context->width, context->height);
  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL ConferenceRecorder::WriteVideo()
{
  AVCodecContext *context = video_st->codec;
  int ret = 0, got_packet = 0;

  AVPacket pkt = { 0 };
  av_init_packet(&pkt);

  // first, increase the counter, set PTS for packet
  video_frame->pts = ++video_frame_count;

  GetVideoFrame();

#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(54,1,0)
  ret = avcodec_encode_video(context, video_outbuf, video_outbuf_size, video_frame);
  if(ret >= 0)
  {
    got_packet = 1;
    pkt.size = ret;
    pkt.data = video_outbuf;
  }
#else
  ret = avcodec_encode_video2(context, &pkt, video_frame, &got_packet);
#endif
  // if size is zero, it means the image was buffered
  if(ret < 0)
  {
    MCUTRACE(1, trace_section << "error encoding video frame: " << ret << " " << AVErrorToString(ret));
    return FALSE;
  }

  ret = WritePacket(video_st, &pkt);
  if(ret < 0)
  {
    MCUTRACE(1, trace_section << "error while writing video frame: " << ret << " " <<  AVErrorToString(ret));
    return FALSE;
  }

  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void ConferenceRecorder::RecorderAudio(PThread &, INT)
{
  MCUTRACE(1, trace_section << "audio thread started");

  unsigned delay_us = av_q2d(audio_st->codec->time_base)*1000000;
  if(delay_us <= 1000)
    delay_us = src_samples*1000000/audio_samplerate;

  MCUDelay delay;

  running = TRUE;
  while(running)
  {
    WriteAudio(delay.GetDelayTimestampUsec());
    delay.DelayUsec(delay_us);
  }
  running = FALSE;

  MCUTRACE(1, trace_section << "audio thread ended");
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void ConferenceRecorder::RecorderVideo(PThread &, INT)
{
  MCUTRACE(1, trace_section << "video thread started");

  unsigned delay_us = av_q2d(video_st->codec->time_base)*1000000;
  if(delay_us <= 1000)
    delay_us = 1000000/video_framerate;

  firstFrameSendTime = PTime();

  MCUDelay delay;

  running = TRUE;
  while(running)
  {
    WriteVideo();
    delay.DelayUsec(delay_us);
  }
  running = FALSE;

  MCUTRACE(1, trace_section << "video thread ended");
}

////////////////////////////////////////////////////////////////////////////////////////////////////
