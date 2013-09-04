#include "g7231codec.h"

//////////////////////////////////////////////////////////////////////////////

G7231_DecoderContext::G7231_DecoderContext()
{
  if ((_codec = avcodec_find_decoder_by_name("g723_1")) == NULL) 
   { cout << "Codec g7231 not found for decoder\n"; return; }

  _context = avcodec_alloc_context3(NULL);
  if (_context == NULL) 
   { cout << "Failed to allocate context for g7231 decoder\n"; return; }

  _outputFrame = avcodec_alloc_frame();
  if (_outputFrame == NULL) 
   { cout << "Failed to allocate frame for g7231 decoder\n"; return; }

  av_new_packet(&_pkt, G7231_BytesPerFrame);

  _frameCount = 0;
  _dct = NULL;

  if (!OpenCodec()) 
  { cout << "Failed to open codec g7231 for decoder\n"; return; }

  cout << "G7231 decoder created\n";
}

G7231_DecoderContext::~G7231_DecoderContext()
{
  CloseCodec();
  av_free(_context);
  av_free(_outputFrame);
}

bool G7231_DecoderContext::OpenCodec()
{
  if (_codec == NULL) 
   { cout << "Decoder g7231 not initialized\n"; return 0; }

  _context->sample_rate = 8000;
  _context->channels = 1;
  _context->bit_rate = 6300;
  _context->sample_fmt = AV_SAMPLE_FMT_S16;

  if (avcodec_open2(_context, _codec, &_dct) < 0) 
   { cout << "Failed to open g7231 decoder\n"; return false; }

  cout << "G7231 decoder opened\n";
  return true;
}

int G7231_DecoderContext::DecodeFrames(const BYTE * src, BYTE * dst)
{
 int got_frame_ptr=0;

  if (_codec == NULL)
   { cout << "Decoder codec g7231 not initialized\n"; return 0; }

  avcodec_get_frame_defaults(_outputFrame);
  _outputFrame->nb_samples  = G7231_SamplesPerFrame;
  memcpy(_pkt.data, src, G7231_BytesPerFrame);
  _pkt.size = G7231_BytesPerFrame;
  avcodec_decode_audio4(_context, _outputFrame, &got_frame_ptr, &_pkt); 
  if(!got_frame_ptr) return 0;
  
  memcpy(dst,_outputFrame->data[0],G7231_SamplesPerFrame*2);
  _frameCount++; 
  return 1;
}


void G7231_DecoderContext::CloseCodec()
{
  if (_pkt.data != NULL) av_free_packet(&_pkt);
  if (_context != NULL && _context->codec != NULL) 
   { avcodec_close(_context); cout << "Closed g7231 decoder\n"; }
}

//###############################################################################

G7231_EncoderContext::G7231_EncoderContext()
{
  if ((_codec = avcodec_find_encoder_by_name("g723_1")) == NULL)
   { cout << "Codec g7231 not found for encoder\n"; return; }

  _context = avcodec_alloc_context3(NULL);
  if (_context == NULL) 
   { cout << "Failed to allocate context for g7231 encoder\n"; return; }

  _inputFrame = avcodec_alloc_frame();
  if (_inputFrame == NULL) 
   { cout << "Failed to allocate frame for g7231 encoder\n"; return; }

  av_new_packet(&_pkt, G7231_BytesPerFrame);
//  av_init_packet(&_pkt);

  _frameCount = 0;

  _dct = NULL;

  if (!OpenCodec())
  { cout << "Failed to open codec g7231 for encoder\n"; return; }

  cout << "G7231 encoder created\n";
}

G7231_EncoderContext::~G7231_EncoderContext()
{
  WaitAndSignal m(_mutex);
  CloseCodec();
  av_free(_context);
  av_free(_inputFrame);
}

bool G7231_EncoderContext::OpenCodec()
{
  if (_codec == NULL) 
   { cout << "Encoder codec g7231 not initialized\n"; return 0; }

  _context->sample_rate = 8000;
  _context->channels = 1;
  _context->bit_rate = 6300;
  _context->sample_fmt = AV_SAMPLE_FMT_S16;

  if (avcodec_open2(_context, _codec, &_dct) < 0) 
   { cout << "Failed to open g7231 encoder\n"; return false; }

  cout << "G7231 encoder codec opened\n";
  return true;
}

void G7231_EncoderContext::CloseCodec()
{
  if (_context != NULL && _context->codec != NULL) 
   { avcodec_close(_context); cout << "Closed g7231 encoder\n"; }
}

int G7231_EncoderContext::EncodeFrames(const BYTE * src, BYTE * dst)
{
 WaitAndSignal m(_mutex);

 int got_packet;
 
  if (_codec == NULL)
   { cout << "Encoder codec g7231 not initialized\n"; return 0; }

  avcodec_get_frame_defaults(_inputFrame);
  _inputFrame->nb_samples  = G7231_SamplesPerFrame;
  avcodec_fill_audio_frame(_inputFrame, 1, AV_SAMPLE_FMT_S16,
                                            src, 480, 1);
  got_packet = 0;
  _pkt.size = G7231_BytesPerFrame;
 
  if (avcodec_encode_audio2(_context, &_pkt, _inputFrame, &got_packet) < 0) 
   { cout << "G7231 audio encoding failed\n"; return(0); }
   
  memcpy(dst,_pkt.data,G7231_BytesPerFrame);

  _frameCount++; 
  return 1;
}


//###############################################################################




static void * create_encoder(const struct PluginCodec_Definition * codec)
{
  return new G7231_EncoderContext();
}

static int codec_encoder(const struct PluginCodec_Definition * codec,
                                           void * _context,
                                     const void * from,
                                       unsigned * fromLen,
                                           void * to,
                                       unsigned * toLen,
                                   unsigned int * flag)
{
  G7231_EncoderContext * context = (G7231_EncoderContext *)_context;

  if (*fromLen != G7231_SamplesPerFrame*2) return 0;

  context->EncodeFrames((const BYTE *)from, (BYTE *)to);
  *toLen   = G7231_BytesPerFrame;
  *fromLen = G7231_SamplesPerFrame*2;
  return 1;
}

static void destroy_encoder(const struct PluginCodec_Definition * codec, void * _context)
{
  G7231_EncoderContext * context = (G7231_EncoderContext *)_context;
  delete context;
}

static void * create_decoder(const struct PluginCodec_Definition * codec)
{
  G7231_DecoderContext * context;
  return new G7231_DecoderContext();
}

static int codec_decoder(const struct PluginCodec_Definition * codec,
                                           void * _context,
                                     const void * from,
                                       unsigned * fromLen,
                                           void * to,
                                       unsigned * toLen,
                                   unsigned int * flag)
{
  G7231_DecoderContext * context = (G7231_DecoderContext *)_context;
  if (*fromLen !=  G7231_BytesPerFrame) return 0;

  context->DecodeFrames((const BYTE *)from, (BYTE *)to);
  *toLen   = G7231_SamplesPerFrame*2;
  *fromLen = G7231_BytesPerFrame;
  return 1;
}

static void destroy_decoder(const struct PluginCodec_Definition * codec, void * _context)
{
  G7231_DecoderContext * context = (G7231_DecoderContext *)_context;
  delete context;
}

//////////////////////////////////////////////////////////////////////////////////////////////

static struct PluginCodec_Definition G7231CodecDefn[] = {
  DECLARE_G7231_PARAM(G7231)
};

extern "C" {
  PLUGIN_CODEC_IMPLEMENT(FFMPEG_G7231)

  PLUGIN_CODEC_DLL_API struct PluginCodec_Definition * PLUGIN_CODEC_GET_CODEC_FN(unsigned * count, unsigned version)
  {
    avcodec_register_all();

    if (version < PLUGIN_CODEC_VERSION_OPTIONS) {
      *count = 0;
      cout << "G7231\tCodec\tDisabled - plugin version mismatch\n";
      return NULL;
    }
    else {
      *count = sizeof(G7231CodecDefn) / sizeof(struct PluginCodec_Definition);
      cout << "G7231\tCodec\tEnabled with " << *count << " definitions\n";
      return G7231CodecDefn;
    }
  }

};
