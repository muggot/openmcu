/*
 * recorder.h
 *
 * Copyright (C) 2014-2015 Andrey Burbovskiy, OpenMCU-ru, All Rights Reserved
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

#ifndef _MCU_RECORDER_H
#define _MCU_RECORDER_H

#include "conference.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

static struct recorder_resolution {
  unsigned width;
  unsigned height;
  unsigned macroblocks;
} const recorder_resolutions[] = {
  { 176,  144,  99   },
  { 320,  240,  300  },
  { 352,  288,  396  },
  { 640,  360,  920  },
  { 640,  480,  1200 },
  { 704,  576,  1584 },
  { 854,  480,  1620 },
  { 800,  600,  1900 },
  { 1024, 768,  3072 },
  { 1280, 720,  3600 },
  { 1366, 768,  4128 },
  { 1280, 1024, 5120 },
  { 1408, 1152, 6336 },
  { 1920, 1080, 8160 },
  { 0 }
};

////////////////////////////////////////////////////////////////////////////////////////////////////

PString GetRecorderCodecs(int media_type);

////////////////////////////////////////////////////////////////////////////////////////////////////

class ConferenceRecorder : public ConferenceMember
{
  PCLASSINFO(ConferenceRecorder, ConferenceMember);

  public:
    ConferenceRecorder(Conference *_conference);
    ~ConferenceRecorder();

    virtual void Close();

    virtual PString GetName() const
    { return "conference recorder"; }

    virtual PTime GetStartTime() const
    { return (running ? startTime : PTime()); }

    BOOL IsRunning()
    { return running; }

    BOOL Start();
    void Stop();

  protected:

    BOOL running;

    PString filename;
    PString format_name;
    PString trace_section;

    unsigned audio_bitrate; // kbit
    unsigned audio_samplerate;
    unsigned audio_channels;

    unsigned video_bitrate; // kbit
    unsigned video_width;
    unsigned video_height;
    unsigned video_framerate;

    int audio_frame_count;
    int video_frame_count;

    AVCodecID audio_codec_id;
    AVCodecID video_codec_id;

#if USE_SWRESAMPLE
    struct SwrContext *swrc;
#elif USE_AVRESAMPLE
    struct AVAudioResampleContext * swrc;
#endif

    uint8_t **src_samples_data;
    int src_samples;
    int src_samples_size;

    uint8_t **dst_samples_data;
    int dst_samples;
    int dst_samples_size;

    uint8_t *video_framebuf;
    uint8_t *video_outbuf;
    PINDEX video_framebuf_size;
    int video_outbuf_size;

    AVFrame *audio_frame;
    AVFrame *video_frame;

    AVStream *audio_st;
    AVStream *video_st;

    AVFormatContext *fmt_context;

    void Reset();
    BOOL InitRecorder();

    AVStream *AddStream(AVMediaType codec_type);

    BOOL OpenAudio();
    BOOL GetAudioFrame(const uint64_t & timestamp);
    BOOL WriteAudio(const uint64_t & timestamp);

    BOOL OpenVideo();
    BOOL GetVideoFrame();
    BOOL WriteVideo();

    BOOL OpenResampler();
    BOOL Resampler();
    int WritePacket(AVStream *st, AVPacket *pkt);

    PThread *thread_audio;
    PDECLARE_NOTIFIER(PThread, ConferenceRecorder, RecorderAudio);

    PThread *thread_video;
    PDECLARE_NOTIFIER(PThread, ConferenceRecorder, RecorderVideo);

    PMutex mutex;
    PMutex write_mutex;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // _MCU_RECORDER_H
