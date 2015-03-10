
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
  { 640,  360,  900  },
  { 640,  480,  1200 },
  { 704,  576,  1584 },
  { 854,  480,  1601 },
  { 800,  600,  1900 },
  { 1024, 768,  3072 },
  { 1280, 720,  3600 },
  { 1366, 768,  4098 },
  { 1280, 1024, 5120 },
  { 1408, 1152, 6336 },
  { 1920, 1080, 8100 },
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
