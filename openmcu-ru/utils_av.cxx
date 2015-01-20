
#include <ptlib.h>

#include "config.h"
#include "mcu.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL MCU_AVEncodeFrame(AVCodecID codec_id, const void * src, int src_size, void * dst, int & dst_size, int src_width, int src_height)
{
  PString trace_section = "MCU_AVEncodeFrame: ";
  AVCodecContext *context = NULL;
  AVCodec *codec = NULL;
  AVFrame *frame = NULL;
  AVPacket pkt = { 0 };
  PixelFormat frame_pix_fmt;
#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(54,1,0)
  MCUBuffer pkt_buffer(dst_size);
#endif
  MCUBuffer frame_buffer(0);
  int ret = 0;
  BOOL result = FALSE;

  av_register_all();

  if(codec_id == AV_CODEC_ID_MJPEG)
    //frame_pix_fmt = AV_PIX_FMT_YUVJ420P;
    frame_pix_fmt = AV_PIX_FMT_YUV420P;
  else
    return FALSE;

  frame = avcodec_alloc_frame();
  if(frame == NULL)
  {
    PTRACE(1, trace_section << "Failed to allocate frame");
    goto end;
  }

  codec = avcodec_find_encoder(codec_id);
  if(codec == NULL)
  {
    MCUTRACE(1, trace_section << "Could not find encoder " << AVCodecGetName(codec_id));
    goto end;
  }

#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(53,8,0)
  context = avcodec_alloc_context();
#else
  context = avcodec_alloc_context3(codec);
#endif
  if(context == NULL)
  {
    MCUTRACE(1, trace_section << "Failed to allocate context");
    goto end;
  }

  context->pix_fmt       = frame_pix_fmt;
  context->width         = src_width;
  context->height        = src_height;
  context->qmin          = 2;
  context->qmax          = 2;
  context->time_base.num = 1;
  context->time_base.den = 1;
  context->strict_std_compliance = FF_COMPLIANCE_UNOFFICIAL;

  // open codec
  avcodecMutex.Wait();
#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(53,8,0)
  ret = avcodec_open(context, codec);
#else
  ret = avcodec_open2(context, codec, NULL);
#endif
  avcodecMutex.Signal();
  if(ret < 0)
  {
    MCUTRACE(1, trace_section << "Could not open video codec: " << AVCodecGetName(codec_id) << " " << ret << " " << AVErrorToString(ret));
    goto end;
  }

  if(frame_pix_fmt == AV_PIX_FMT_YUV420P)
  {
    avpicture_fill((AVPicture *)frame, (uint8_t *)src, AV_PIX_FMT_YUV420P, src_width, src_height);
  }
  else
  {
    int frame_buffer_size = avpicture_get_size(frame_pix_fmt, src_width, src_height);
    frame_buffer.SetSize(frame_buffer_size);

    struct SwsContext *sws_ctx = sws_getContext(src_width, src_height, AV_PIX_FMT_YUV420P,
                                                src_width, src_height, frame_pix_fmt,
                                                SWS_BILINEAR, NULL, NULL, NULL);
    if(sws_ctx == NULL)
    {
      MCUTRACE(1, trace_section << "Impossible to create scale context for the conversion "
                  << src_width << "x" << src_height << "->" << src_width << "x" << src_height);
      goto end;
    }

    // initialize linesize
    AVPicture src_picture;
    avpicture_fill(&src_picture, (uint8_t *)src, AV_PIX_FMT_YUV420P, src_width, src_height);

    sws_scale(sws_ctx, src_picture.data, src_picture.linesize, 0, src_height,
                       frame->data, frame->linesize);

    sws_freeContext(sws_ctx);
  }

  av_init_packet(&pkt);
#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(54,1,0)
  ret = avcodec_encode_video(context, pkt_buffer.GetPointer(), pkt_buffer.GetSize(), frame);
  if(ret >= 0)
  {
    pkt.size = ret;
    pkt.data = pkt_buffer.GetPointer();
  }
#else
  int got_packet;
  ret = avcodec_encode_video2(context, &pkt, frame, &got_packet);
#endif
  // if size is zero, it means the image was buffered
  if(ret < 0)
  {
    MCUTRACE(1, trace_section << "error encoding video frame: " << AVCodecGetName(codec_id) << " " << ret << " " << AVErrorToString(ret));
    goto end;
  }

  if(pkt.size > dst_size)
  {
    MCUTRACE(1, trace_section << "Picture size " << pkt.size << " is larger than the buffer size " << dst_size);
    goto end;
  }

  dst_size = pkt.size;
  memcpy(dst, pkt.data, pkt.size);

  result = TRUE;

  end:
    if(context)
    {
      // close codec
      avcodecMutex.Wait();
      avcodec_close(context);
      avcodecMutex.Signal();
    }
    if(frame)
      av_free(frame);

    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL MCU_AVDecodeFrameFromFile(PString & filename, void *dst, int & dst_size, int & dst_width, int & dst_height)
{
  PString trace_section = "MCU_AVDecodeFrameFromFile: ";
  AVFormatContext *fmt_ctx = NULL;
  AVCodecContext *context = NULL;
  AVFrame *frame = NULL;
  AVPacket pkt = { 0 };
  int ret = 0;
  BOOL result = FALSE;

  av_register_all();

  frame = avcodec_alloc_frame();
  if(frame == NULL)
  {
    PTRACE(1, trace_section << "Failed to allocate frame");
    goto end;
  }

  if((ret = avformat_open_input(&fmt_ctx, filename, 0, 0)) < 0)
  {
    MCUTRACE(1, trace_section << "Could not open input file " << filename << " " << ret << " " << AVErrorToString(ret));
    goto end;
  }

  if((ret = avformat_find_stream_info(fmt_ctx, 0)) < 0)
  {
    MCUTRACE(1, trace_section << "Failed to retrieve input stream information from file " << filename << " " << ret << " " << AVErrorToString(ret));
    goto end;
  }

  av_dump_format(fmt_ctx, 0, filename, 0);

  for(unsigned i = 0; i < fmt_ctx->nb_streams; ++i)
  {
    if(fmt_ctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
    {
      AVStream *stream = fmt_ctx->streams[i];
      context = stream->codec;
      break;
    }
  }

  if(context == NULL)
  {
    MCUTRACE(1, trace_section << "Could not find video context in file " << filename);
    goto end;
  }

  av_init_packet(&pkt);
  ret = av_read_frame(fmt_ctx, &pkt);
  if(ret < 0)
  {
    MCUTRACE(1, trace_section << "Failed read frame from file " << filename << " " << ret << " " << AVErrorToString(ret));
    goto end;
  }

  context->codec = avcodec_find_decoder(context->codec_id);
  if(context->codec == NULL)
  {
    MCUTRACE(1, trace_section << "Could not find decoder " << AVCodecGetName(context->codec_id));
    goto end;
  }

  // open codec
  avcodecMutex.Wait();
#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(53,8,0)
  ret = avcodec_open(context, context->codec);
#else
  ret = avcodec_open2(context, context->codec, NULL);
#endif
  avcodecMutex.Signal();
  if(ret < 0)
  {
    MCUTRACE(1, trace_section << "Could not open video codec: " << AVCodecGetName(context->codec_id) << " " << ret << " " << AVErrorToString(ret));
    goto end;
  }

  int got_picture;
  ret = avcodec_decode_video2(context, frame, &got_picture, &pkt);
  if(ret < 0 || got_picture == 0)
  {
    MCUTRACE(1, trace_section << "Error decoding video frame: " << AVCodecGetName(context->codec_id) << " " << ret << " " << AVErrorToString(ret));
    goto end;
  }

  {
    int dst_picture_size = avpicture_get_size(AV_PIX_FMT_YUV420P, frame->width, frame->height);
    if(dst_picture_size > dst_size)
    {
      MCUTRACE(1, trace_section << "Picture size " << dst_picture_size << " is larger than the buffer size " << dst_size);
      goto end;
    }

    struct SwsContext *sws_ctx = sws_getContext(frame->width, frame->height, (PixelFormat)frame->format,
                                                frame->width, frame->height, AV_PIX_FMT_YUV420P,
                                                SWS_BILINEAR, NULL, NULL, NULL);
    if(sws_ctx == NULL)
    {
      MCUTRACE(1, trace_section << "Impossible to create scale context for the conversion "
                  << frame->width << "x" << frame->height << "->" << frame->width << "x" << frame->height);
      goto end;
    }

    // initialize linesize
    MCUBuffer dst_buffer(dst_picture_size);
    AVPicture dst_picture;
    avpicture_fill(&dst_picture, (uint8_t *)dst_buffer.GetPointer(), AV_PIX_FMT_YUV420P, frame->width, frame->height);

    sws_scale(sws_ctx, frame->data, frame->linesize, 0, frame->height,
                       dst_picture.data, dst_picture.linesize);

    sws_freeContext(sws_ctx);

    dst_width = frame->width;
    dst_height = frame->height;
    dst_size = dst_picture_size;
    memcpy(dst, dst_buffer.GetPointer(), dst_size);

    result = TRUE;
  }

  end:
    if(fmt_ctx)
      avformat_close_input(&fmt_ctx);
    if(context)
    {
      avcodecMutex.Wait();
      avcodec_close(context);
      avcodecMutex.Signal();
    }
    if(frame)
      av_free(frame);

    return result;
}

///////////////////////////////////////////////////////////////////////////
