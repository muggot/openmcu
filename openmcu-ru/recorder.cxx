
#include "mcu.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef av_err2str
char *av_make_error_string(char *errbuf, size_t errbuf_size, int errnum)
{
  av_strerror(errnum, errbuf, errbuf_size);
  return errbuf;
}
/**
 * Convenience macro, the return value should be used only directly in
 * function arguments but never stand-alone.
 */
#define av_err2str(errnum) \
  av_make_error_string((char[64]){0}, 64, errnum)
#endif

#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(54, 0, 0)
  #define AV_CODEC_ID_NONE      CODEC_ID_NONE
#endif
#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(55, 0, 0)
  #define AV_PIX_FMT_YUV420P    PIX_FMT_YUV420P
#endif

#define ALIGN 1

////////////////////////////////////////////////////////////////////////////////////////////////////

ConferenceRecorder::ConferenceRecorder(Conference *_conference)
  : ConferenceMember(_conference, (void *)this)
{
  Reset();

  trace_section = "ConferenceRecorder: ";

  if(conference == NULL)
    return;

  conference->AddMember(this);

  MCUTRACE(1, trace_section << "create");
}

////////////////////////////////////////////////////////////////////////////////////////////////////

ConferenceRecorder::~ConferenceRecorder()
{
  Stop();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void ConferenceRecorder::Reset()
{
  running = FALSE;
  thread = NULL;

  swr_ctx = NULL;
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

  if(thread)
  {
    running = FALSE;
    thread->WaitForTermination(10000);
    delete thread;
  }

  if(swr_ctx)
    swr_free(&swr_ctx);

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
    av_free(audio_frame);
  if(video_frame)
    av_free(video_frame);

  if(audio_st)
    avcodec_close(audio_st->codec);
  if(video_st)
    avcodec_close(video_st->codec);

  if(fmt_context)
    avformat_free_context(fmt_context);

  Reset();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL ConferenceRecorder::Start()
{
  PWaitAndSignal m(mutex);

  if(IsRunning() == TRUE)
    return TRUE;

  OpenMCU & mcu = OpenMCU::Current();

  // video
  video_width = mcu.vr_framewidth;
  video_height = mcu.vr_frameheight;
  video_framerate = mcu.vr_framerate;

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
  video_bitrate = video_width*video_height*video_framerate/10;

  // audio
  audio_samplerate = OpenMCU::Current().vr_sampleRate;
  if(audio_samplerate < 8000)       { audio_samplerate = 8000; PTRACE(1, trace_section << "sample rate changed to 8000"); }
  else if(audio_samplerate > 48000) { audio_samplerate = 48000; PTRACE(1, trace_section << "sample rate changed to 48000"); }
  audio_channels = OpenMCU::Current().vr_audioChans;
  if(audio_channels < 1)      { audio_channels = 1; PTRACE(1, trace_section << "audio channels changed to 1"); }
  else if(audio_channels > 8) { audio_channels = 8; PTRACE(1, trace_section << "audio channels changed to 8"); }
  audio_bitrate = audio_channels*64000;

  // filename format: room101__2013-0516-1058270__704x576x10
  PStringStream t;
  t << mcu.vr_ffmpegDir << PATH_SEPARATOR
    << conference->GetNumber()
    << "__" << PTime().AsString("yyyy-MMdd-hhmmssu", PTime::Local) << "__"
    << video_width << "x"
    << video_height << "x"
    << video_framerate << ".avi";
  filename = t;
  format_name = "avi";

  if(InitRecorder() == FALSE)
  {
    MCUTRACE(1, trace_section << "failed initialise recorder");
    return FALSE;
  }

  thread = PThread::Create(PCREATE_NOTIFIER(Recorder), 0, PThread::NoAutoDeleteThread, PThread::NormalPriority, "conference_recorder:%0x");
  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL ConferenceRecorder::InitRecorder()
{
  int ret;

  // Initialize libavcodec, and register all codecs and formats
  av_register_all();

  // allocate the output media context
  avformat_alloc_output_context2(&fmt_context, NULL, format_name, filename);
  if(fmt_context == NULL)
  {
    MCUTRACE(1, trace_section << "could not allocate the output context");
    return FALSE;
  }

  if(fmt_context->oformat->audio_codec != AV_CODEC_ID_NONE)
    audio_st = AddStream(AVMEDIA_TYPE_AUDIO);

  if(fmt_context->oformat->video_codec != AV_CODEC_ID_NONE)
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
    MCUTRACE(1, trace_section << "could not open " << filename << " " << ret << " " << av_err2str(ret));
    return FALSE;
  }

  // write the stream header
  ret = avformat_write_header(fmt_context, NULL);
  if(ret < 0)
  {
    MCUTRACE(1, trace_section << "error occurred when opening output file: " << ret << " " << av_err2str(ret));
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
    MCUTRACE(1, trace_section << "could not find encoder for " << codec_type);
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
#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(55, 0, 0)
    context->sample_fmt    = AV_SAMPLE_FMT_FLT;
#else
    context->sample_fmt    = AV_SAMPLE_FMT_FLTP;
#endif
    context->codec_id      = fmt_context->oformat->audio_codec;
    context->bit_rate      = audio_bitrate;
    context->sample_rate   = audio_samplerate;
    context->channels      = audio_channels;
    context->strict_std_compliance = -2;
  }
  else if(codec_type == AVMEDIA_TYPE_VIDEO)
  {
    context->pix_fmt       = AV_PIX_FMT_YUV420P;
    context->codec_id      = fmt_context->oformat->video_codec;
    context->bit_rate      = video_bitrate;
    context->width         = video_width;
    context->height        = video_height;
    context->gop_size      = 12;
    context->time_base.num = 1;
    context->time_base.den = video_framerate;
  }

  // Some formats want stream headers to be separate
  if(fmt_context->oformat->flags & AVFMT_GLOBALHEADER)
    context->flags |= CODEC_FLAG_GLOBAL_HEADER;

  return st;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int ConferenceRecorder::WriteFrame(AVStream *st, AVPacket *pkt)
{
  AVCodecContext *context = st->codec;
  int ret;

  pkt->stream_index = st->index;
  // write the compressed frame to the media file
  ret = av_interleaved_write_frame(fmt_context, pkt);
  if(ret >= 0)
  {
    if(context->codec_type == AVMEDIA_TYPE_AUDIO)
      audio_frame->pts += av_rescale_q(1, st->codec->time_base, st->time_base);
    else if(context->codec_type == AVMEDIA_TYPE_VIDEO)
      video_frame->pts += av_rescale_q(1, st->codec->time_base, st->time_base);
  }
  return ret;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL ConferenceRecorder::OpenAudio()
{
  AVCodecContext *context = audio_st->codec;
  int ret;

  // open codec
  ret = avcodec_open2(context, context->codec, NULL);
  if(ret < 0)
  {
    MCUTRACE(1, trace_section << "could not open audio codec: " << ret << " " << av_err2str(ret));
    return FALSE;
  }

  // allocate and init a frame
  audio_frame = avcodec_alloc_frame();
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
  int ret;

  src_samples = context->frame_size;
  src_samples_size = av_samples_get_buffer_size(NULL, context->channels, src_samples, AV_SAMPLE_FMT_S16, ALIGN);
  src_samples_data = (uint8_t **)av_malloc(context->channels * sizeof(uint8_t *));
  ret = av_samples_alloc(src_samples_data, NULL, context->channels, src_samples, AV_SAMPLE_FMT_S16, ALIGN);
  if(ret < 0)
  {
    MCUTRACE(1, trace_section << "could not allocate source samples: " << src_samples << " " << ret << " " << av_err2str(ret));
    return FALSE;
  }

  // create resampler context
  if(context->sample_fmt != AV_SAMPLE_FMT_S16 && context->sample_fmt != AV_SAMPLE_FMT_FLT)
  {
    swr_ctx = swr_alloc();
    if(swr_ctx == NULL)
    {
      MCUTRACE(1, trace_section << "could not allocate resampler context");
      return FALSE;
    }

    // set options
    av_opt_set_int(swr_ctx, "in_channel_count",    context->channels,      0);
    av_opt_set_int(swr_ctx, "in_sample_rate",      context->sample_rate,   0);
    av_opt_set_int(swr_ctx, "in_sample_fmt",       AV_SAMPLE_FMT_S16,      0);
    av_opt_set_int(swr_ctx, "out_channel_count",   context->channels,      0);
    av_opt_set_int(swr_ctx, "out_sample_rate",     context->sample_rate,   0);
    av_opt_set_int(swr_ctx, "out_sample_fmt",      context->sample_fmt,    0);

    // initialize the resampling context
    ret = swr_init(swr_ctx);
    if(ret < 0)
    {
      MCUTRACE(1, trace_section << "failed to initialize the resampling context: " << ret << " " << av_err2str(ret));
      return FALSE;
    }
  }

  // allocate destination samples
  dst_samples = av_rescale_rnd(src_samples, context->sample_rate, context->sample_rate, AV_ROUND_UP);
  dst_samples_size = av_samples_get_buffer_size(NULL, context->channels, dst_samples, context->sample_fmt, ALIGN);
  dst_samples_data = (uint8_t **)av_malloc(context->channels * sizeof(uint8_t *));
  ret = av_samples_alloc(dst_samples_data, NULL, context->channels, dst_samples, context->sample_fmt, ALIGN);
  if(ret < 0)
  {
    MCUTRACE(1, trace_section << "could not allocate destination samples: " << dst_samples << " " << ret << " " << av_err2str(ret));
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
  int ret;

  // convert to destination format
  if(context->sample_fmt == AV_SAMPLE_FMT_S16)
  {
    dst_samples = src_samples;
    dst_samples_size = src_samples_size;
    dst_samples_data[0] = src_samples_data[0];
  } else if(context->sample_fmt == AV_SAMPLE_FMT_FLT) {
    for(int i = 0; i < src_samples; i++)
      ((float *)dst_samples_data[0])[i] = ((int16_t *)src_samples_data[0])[i] * (1.0 / (1<<15));
  } else {
    // convert samples from native format to destination codec format, using the resampler
    ret = swr_convert(swr_ctx, dst_samples_data, dst_samples, (const uint8_t **)src_samples_data, src_samples);
    if(ret < 0)
    {
      MCUTRACE(1, trace_section << "error while converting: " << ret << " " << av_err2str(ret));
      return FALSE;
    }
  }

  audio_frame->nb_samples = dst_samples;
  avcodec_fill_audio_frame(audio_frame, context->channels, context->sample_fmt, dst_samples_data[0], dst_samples_size, ALIGN);

  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL ConferenceRecorder::GetAudioFrame()
{
  AVCodecContext *context = audio_st->codec;

  ReadAudio(src_samples_data[0], src_samples_size, context->sample_rate, context->channels);
  return Resampler();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL ConferenceRecorder::WriteAudioFrame()
{
  AVCodecContext *context = audio_st->codec;
  int ret, got_packet;

  AVPacket pkt = { 0 };
  av_init_packet(&pkt);

  if(GetAudioFrame() == FALSE)
    return FALSE;

  ret = avcodec_encode_audio2(context, &pkt, audio_frame, &got_packet);
  if(ret < 0)
  {
    MCUTRACE(1, trace_section << "error encoding audio frame: " << ret << " " << av_err2str(ret));
    return FALSE;
  }

  if(got_packet == 0)
    return TRUE;

  ret = WriteFrame(audio_st, &pkt);
  if(ret < 0)
  {
    MCUTRACE(1, trace_section << "error while writing audio frame: " << ret << " " << av_err2str(ret));
    return FALSE;
  }

  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL ConferenceRecorder::OpenVideo()
{
  AVCodecContext *context = video_st->codec;
  int ret;

  // open the codec
  ret = avcodec_open2(context, context->codec, NULL);
  if(ret < 0)
  {
    MCUTRACE(1, trace_section << "could not open video codec: " << ret << " " << av_err2str(ret));
    return FALSE;
  }

  // allocate and init a re-usable frame
  video_frame = avcodec_alloc_frame();
  if(video_frame == NULL)
  {
    MCUTRACE(1, trace_section << "could not allocate video frame");
    return FALSE;
  }

  video_frame->width = context->width;
  video_frame->height = context->height;
  video_frame->format = context->pix_fmt;
  video_frame->pts = 0;

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

BOOL ConferenceRecorder::WriteVideoFrame()
{
  AVCodecContext *context = video_st->codec;
  int ret, got_packet;

  GetVideoFrame();

  AVPacket pkt = { 0 };
  av_init_packet(&pkt);

  // encode the frame
#if LIBAVCODEC_VERSION_INT >= AV_VERSION_INT(54, 0, 0)
  ret = avcodec_encode_video2(context, &pkt, video_frame, &got_packet);
#else
  ret = avcodec_encode_video(context, video_outbuf, video_outbuf_size, video_frame);
  if(ret >= 0)
  {
    got_packet = 1;
    pkt.size = ret;
    pkt.data = video_outbuf;
  }
#endif

  // if size is zero, it means the image was buffered
  if(ret < 0)
  {
    MCUTRACE(1, trace_section << "error encoding video frame: " << ret << " " << av_err2str(ret));
    return FALSE;
  }

  if(got_packet == 0)
    return TRUE;

  ret = WriteFrame(video_st, &pkt);
  if(ret < 0)
  {
    MCUTRACE(1, trace_section << "error while writing video frame: " << ret << " " << av_err2str(ret));
    return FALSE;
  }

  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void ConferenceRecorder::Recorder(PThread &, INT)
{
  MCUTRACE(1, trace_section << "thread started");

  startTime = PTime();

  PTime audio_time(0);
  int audio_delay_ms = audio_st ? av_q2d(audio_st->time_base)*1000 : 0;

  PTime video_time(0);
  int video_delay_ms = video_st ? av_q2d(video_st->time_base)*1000 : 0;

  running = TRUE;
  while(running)
  {
    PTime now;
    if(audio_st && now > audio_time + audio_delay_ms)
    {
      audio_time = now;
      WriteAudioFrame();
    }
    if(video_st && now > video_time + video_delay_ms)
    {
      video_time = now;
      WriteVideoFrame();
    }
  }

  // write the trailer
  av_write_trailer(fmt_context);

  // close the output file
  avio_close(fmt_context->pb);

  MCUTRACE(1, trace_section << "thread ended");
}

////////////////////////////////////////////////////////////////////////////////////////////////////
