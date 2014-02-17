#include <codec/opalplugin.h>

#include <windows.h>
#include <mmreg.h>
#include <msacm.h>

#pragma comment(lib, "msacm32.lib")

class WaveFormat
{
  public:
    WaveFormat()
    {
      size = 0;
      waveFormat = NULL;
    }

    WaveFormat(const WaveFormat & fmt)
    {
      size = fmt.size;
      waveFormat = (WAVEFORMATEX *)malloc(size);
      memcpy(waveFormat, fmt.waveFormat, size);
    }

    ~WaveFormat()
    {
      if (waveFormat != NULL)
        free(waveFormat);
    }

    WaveFormat & operator=(const WaveFormat & fmt)
    {
      if (this == &fmt)
        return *this;
      if (waveFormat != NULL)
        free(waveFormat);
      size = fmt.size;
      waveFormat = (WAVEFORMATEX *)malloc(size);
      memcpy(waveFormat, fmt.waveFormat, size);
      return *this;
    }

    void SetFormat(unsigned numChannels,
                                unsigned sampleRate,
                                unsigned bitsPerSample)
    {
      if (waveFormat != NULL)
        free(waveFormat);

      size = sizeof(WAVEFORMATEX);
      waveFormat = (WAVEFORMATEX *)malloc(sizeof(WAVEFORMATEX));

      waveFormat->wFormatTag      = WAVE_FORMAT_PCM;
      waveFormat->nChannels       = (WORD)numChannels;
      waveFormat->nSamplesPerSec  = sampleRate;
      waveFormat->wBitsPerSample  = (WORD)bitsPerSample;
      waveFormat->nBlockAlign     = (WORD)(numChannels*(bitsPerSample+7)/8);
      waveFormat->nAvgBytesPerSec = waveFormat->nSamplesPerSec*waveFormat->nBlockAlign;
      waveFormat->cbSize          = 0;
    }

    void SetFormat(const void * data, unsigned size)
    {
      SetSize(size);
      memcpy(waveFormat, data, size);
    }


    BOOL SetSize(unsigned sz)
    {
      if (waveFormat != NULL)
        free(waveFormat);
      size = sz;
      if (sz == 0)
        waveFormat = NULL;
      else {
        if (sz < sizeof(WAVEFORMATEX))
          sz = sizeof(WAVEFORMATEX);
        waveFormat = (WAVEFORMATEX *)calloc(sz, 1);
        waveFormat->cbSize = (WORD)(sz - sizeof(WAVEFORMATEX));
      }
      return waveFormat != NULL;
    }

    unsigned GetSize() const 
    { 
      return  size;
    }

    void         * GetPointer() const { return  waveFormat; }
    WAVEFORMATEX * operator->() const { return  waveFormat; }
    WAVEFORMATEX & operator *() const { return *waveFormat; }
    operator   WAVEFORMATEX *() const { return  waveFormat; }

  protected:
    unsigned       size;
    WAVEFORMATEX * waveFormat;
};

/////////////////////////////////////////////////////////////////////////////

static const struct {
  WAVEFORMATEX wf;
  BYTE extra[10];
} g7231format = {
  { 66, 1, 8000, 800, 24, 0, 10 },
  {  2, 0, 0xce, 0x9a, 0x32, 0xf7, 0xa2, 0xae, 0xde, 0xac }
};

static const unsigned G7231PacketSizes[4] = { 24, 20, 4, 1 };

/////////////////////////////////////////////////////////////////////////////

static void * create_encoder(const struct PluginCodec_Definition *)
{
  WaveFormat srcFormat; srcFormat.SetFormat(1, 8000, 16);
  WaveFormat dstFormat; dstFormat.SetFormat(&g7231format, sizeof(g7231format));

  HACMSTREAM  hStream;
  MMRESULT result = acmStreamOpen(&hStream,
                                  NULL, // driver
                                  srcFormat, // source format
                                  dstFormat, // destination format
                                  NULL, // no filter
                                  NULL, // no callback
                                  0, // instance data (not used)
                                  0); // flags

  if (result != 0)
    return NULL;

  return (void *)hStream;
}


static int codec_encoder(const struct PluginCodec_Definition * /*defn*/,
                                           void * context,
                                     const void * from, 
                                       unsigned * fromLen,
                                           void * to,         
                                       unsigned * toLen,
                                   unsigned int * /*flag*/)
{
  HACMSTREAM hStream = (HACMSTREAM)context;

  if (hStream == NULL)
    return FALSE;

  ACMSTREAMHEADER header;
  memset(&header, 0, sizeof(header));
  header.cbStruct    = sizeof(header);
  header.pbSrc       = (unsigned char *)from;
  header.cbSrcLength = *fromLen;
  header.pbDst       = (unsigned char *)to;
  header.cbDstLength = *toLen;

  // prep the header
  MMRESULT result = acmStreamPrepareHeader(hStream, &header, 0);
  if (result != 0) {
    //PTRACE(1, "Codec\tError in encode acmStreamPrepareHeader: error=" << result);
    return FALSE;
  }

  result = acmStreamConvert(hStream, &header, 0);
  if (result != 0) {
    //PTRACE(1, "Codec\tError in encode acmStreamConvert: error=" << result);
    return FALSE;
  }

  *fromLen = 240*2;
  *toLen   = G7231PacketSizes[((unsigned char *)to)[0]&3];

  return TRUE;

}

static void destroy_encoder(const struct PluginCodec_Definition * /*defn*/, void * context)
{
  HACMSTREAM hStream = (HACMSTREAM)context;
  if (hStream != NULL)
    acmStreamClose(hStream, 0);
}

/////////////////////////////////////////////////////////////////////////////

static void * create_decoder(const struct PluginCodec_Definition * /*defn*/)
{
  WaveFormat srcFormat; srcFormat.SetFormat(&g7231format, sizeof(g7231format));
  WaveFormat dstFormat; dstFormat.SetFormat(1, 8000, 16);

  HACMSTREAM  hStream;
  MMRESULT result = acmStreamOpen(&hStream,
                                  NULL, // driver
                                  srcFormat, // source format
                                  dstFormat, // destination format
                                  NULL, // no filter
                                  NULL, // no callback
                                  0, // instance data (not used)
                                  0); // flags

  if (result != 0)
    return NULL;

  return (void *)hStream;
}

static int codec_decoder(const struct PluginCodec_Definition * /*defn*/,
                                           void * context,
                                     const void * from, 
                                       unsigned * fromLen,
                                           void * to,         
                                       unsigned * toLen,
                                   unsigned int * /*flag*/)
{
  HACMSTREAM hStream = (HACMSTREAM)context;

  if (hStream == NULL)
    return FALSE;

  ACMSTREAMHEADER header;
  memset(&header, 0, sizeof(header));
  header.cbStruct = sizeof(header);
  header.pbSrc = (BYTE *)from;

  *fromLen = header.cbSrcLength = G7231PacketSizes[((unsigned char *)from)[0]&3];

  if (header.cbSrcLength > *fromLen)
    header.cbSrcLength = *fromLen;

  header.pbDst       = (unsigned char *)to;
  header.cbDstLength = *toLen;

  // make sure all frames are 24 bytes long
  static BYTE frameBuffer[24];
  if (header.cbSrcLength < 24) {
    memcpy(frameBuffer, header.pbSrc, header.cbSrcLength);
    header.cbSrcLength = 24;
    header.pbSrc       = frameBuffer;
  }

  // prep the header
  MMRESULT result = acmStreamPrepareHeader(hStream, &header, 0); 
  if (result != 0) {
    //PTRACE(1, "Codec\tError in decode acmStreamPrepareHeader: error=" << result);
    return FALSE;
  }

  result = acmStreamConvert(hStream, &header, 0);
  if (result != 0) {
    //PTRACE(1, "Codec\tError in decode acmStreamConvert: error=" << result);
    return FALSE;
  }

  *toLen = header.cbSrcLength;

  return TRUE;

}

static void destroy_decoder(const struct PluginCodec_Definition * /*defn*/, void * context)
{
  HACMSTREAM hStream = (HACMSTREAM)context;
  if (hStream != NULL)
    acmStreamClose(hStream, 0);
}

/////////////////////////////////////////////////////////////////////////////

static struct PluginCodec_information licenseInfo = {
  1073357308,                                                  // timestamp = Tue 06 Jan 2004 02:48:28 AM UTC

  "Craig Southeren, Post Increment",                           // source code author
  "$Ver$",                                                     // source code version
  "craigs@postincrement.com",                                  // source code email
  "http://www.postincrement.com",                              // source code URL
  "Copyright (C) 2004 by Post Increment, All Rights Reserved", // source code copyright
  "MPL 1.0",                                                   // source code license
  PluginCodec_License_MPL,                                     // source code license

  "G.723.1",                                                   // codec description
  "Microsoft Corp.",                                           // codec author
  NULL,                                                        // codec version
  NULL,                                                        // codec email
  NULL,                                                        // codec URL
  NULL,                                                        // codec copyright information
  NULL,                                                        // codec license
  PluginCodec_License_RoyaltiesRequired                        // codec license code
};

static const char L16Desc[]          = { "L16" };

static const char G7231Desc[]        = { "G.723.1" };

static const char CiscoG7231A[]      = { "Cisco G.7231A" };
static const char CiscoG7231AR[]     = { "Cisco G.7231AR" };

static const char G7231ar[]          = { "G7231ar" };

static const char ianaName[]         = { "G723" };
static const int  ianaCode           = 4;

static struct PluginCodec_H323NonStandardCodecData g7231arCap =
{
  NULL,                               // object ID
  181,                                // T35 country code
  0,                                  // T35 extension code
  18,                                 // T35 manufacturer code
  (const unsigned char *)G7231ar,     // data
  sizeof(G7231ar)-1,                  // data length
  NULL                                // capability match function
};


static PluginCodec_Definition g7231CodecDefn[] = {

{ 
  // G.723.1 encoder
  PLUGIN_CODEC_VERSION,               // codec API version
  &licenseInfo,                       // license information

  PluginCodec_MediaTypeAudio |        // audio codec
  PluginCodec_InputTypeRaw |          // raw input data
  PluginCodec_OutputTypeRaw |         // raw output data
  PluginCodec_RTPTypeExplicit,        // explicit RTP type

  G7231Desc,                          // text decription
  L16Desc,                            // source format
  G7231Desc,                          // destination format

  NULL,                               // user data

  8000,                               // samples per second
  6300,                               // raw bits per second
  30000,                              // nanoseconds per frame
  240,                                // samples per frame
  24,                                 // bytes per frame
  1,                                  // recommended number of frames per packet
  4,                                  // maximum number of frames per packet
  ianaCode,                           // IANA RTP payload code
  ianaName,                           // use standard IANA payload name for SDP

  create_encoder,                     // create codec function
  destroy_encoder,                    // destroy codec
  codec_encoder,                      // encode/decode
  NULL,                                // codec controls

  PluginCodec_H323AudioCodec_g7231,   // h323CapabilityType
  0,                                  // h323CapabilityData (silence supression)
},

{ 
  // G.723.1 decoder
  PLUGIN_CODEC_VERSION,               // codec API version
  &licenseInfo,                       // license information

  PluginCodec_MediaTypeAudio |        // audio codec
  PluginCodec_InputTypeRaw |          // raw input data
  PluginCodec_OutputTypeRaw |         // raw output data
  PluginCodec_RTPTypeExplicit,        // explicit RTP type

  G7231Desc,                          // text decription
  G7231Desc,                          // source format
  L16Desc,                            // destination format

  NULL,                               // user data

  8000,                               // samples per second
  6300,                               // raw bits per second
  30000,                              // nanoseconds per frame
  240,                                // samples per frame
  24,                                 // bytes per frame
  1,                                  // recommended number of frames per packet
  4,                                  // maximum number of frames per packet
  ianaCode,                           // IANA RTP payload code
  ianaName,                           // use standard IANA payload name for SDP

  create_decoder,                     // create codec function
  destroy_decoder,                    // destroy codec
  codec_decoder,                      // encode/decode
  NULL,                                // codec controls

  PluginCodec_H323AudioCodec_g7231,   // h323CapabilityType
  0,                                  // h323CapabilityData (silence supression)
},

{ 
  // Cisco g7231a encoder
  PLUGIN_CODEC_VERSION,             // codec API version
  &licenseInfo,                     // license information

  PluginCodec_MediaTypeAudio |        // audio codec
  PluginCodec_InputTypeRaw |          // raw input data
  PluginCodec_OutputTypeRaw |         // raw output data
  PluginCodec_RTPTypeExplicit,        // explicit RTP type

  CiscoG7231A,                      // text decription
  L16Desc,                          // source format
  CiscoG7231A,                      // destination format

  NULL,                               // user data

  8000,                             // samples per second
  6300,                             // raw bits per second
  30000,                            // nanoseconds per frame
  240,                              // samples per frame
  24,                               // bytes per frame
  1,                                // recommended number of frames per packet
  4,                                // maximum number of frames per packet
  ianaCode,                           // IANA RTP payload code
  NULL,                               // No IANA payload name for SDP

  create_encoder,                   // create codec function
  destroy_encoder,                  // destroy codec
  codec_encoder,                    // encode/decode
  NULL,                                // codec controls

  PluginCodec_H323AudioCodec_g7231, // h323CapabilityType
  (void *)1,                        // h323CapabilityData (silence supression)
},

{ 
  // Cisco g7231a decoder
  PLUGIN_CODEC_VERSION,               // codec API version
  &licenseInfo,                       // license information

  PluginCodec_MediaTypeAudio |        // audio codec
  PluginCodec_InputTypeRaw |          // raw input data
  PluginCodec_OutputTypeRaw |         // raw output data
  PluginCodec_RTPTypeExplicit,        // explicit RTP type

  CiscoG7231A,                        // text decription
  CiscoG7231A,                        // source format
  L16Desc,                            // destination format

  NULL,                               // user data

  8000,                               // samples per second
  6300,                               // raw bits per second
  30000,                              // nanoseconds per frame
  240,                                // samples per frame
  24,                                 // bytes per frame
  1,                                  // recommended number of frames per packet
  4,                                  // maximum number of frames per packet
  ianaCode,                           // IANA RTP payload code
  NULL,                               // No IANA payload name for SDP

  create_decoder,                     // create codec function
  destroy_decoder,                    // destroy codec
  codec_decoder,                      // encode/decode
  NULL,                                // codec controls

  PluginCodec_H323AudioCodec_g7231,   // h323CapabilityType
  (void *)1,                          // h323CapabilityData (silence supression)
},

{ 
  // Cisco g7231ar encoder
  PLUGIN_CODEC_VERSION,              // codec API version
  &licenseInfo,                      // license information

  PluginCodec_MediaTypeAudio |        // audio codec
  PluginCodec_InputTypeRaw |          // raw input data
  PluginCodec_OutputTypeRaw |         // raw output data
  PluginCodec_RTPTypeExplicit,        // explicit RTP type

  CiscoG7231AR,                      // text decription
  L16Desc,                           // source format
  CiscoG7231AR,                      // destination format

  NULL,                               // user data

  8000,                              // samples per second
  6300,                              // raw bits per second
  30000,                             // nanoseconds per frame
  240,                               // samples per frame
  24,                                // bytes per frame
  1,                                 // recommended number of frames per packet
  4,                                 // maximum number of frames per packet
  ianaCode,                          // IANA RTP payload code
  NULL,                              // No IANA payload name for SDP

  create_encoder,                    // create codec function
  destroy_encoder,                   // destroy codec
  codec_encoder,                     // encode/decode
  NULL,                                // codec controls

  PluginCodec_H323Codec_nonStandard, // h323CapabilityType
  &g7231arCap                          // h323CapabilityData 
},

{ 
  // Cisco g7231ar decoder
  PLUGIN_CODEC_VERSION,                // codec API version
  &licenseInfo,                        // license information

  PluginCodec_MediaTypeAudio |        // audio codec
  PluginCodec_InputTypeRaw |          // raw input data
  PluginCodec_OutputTypeRaw |         // raw output data
  PluginCodec_RTPTypeExplicit,        // explicit RTP type

  CiscoG7231AR,                        // text decription
  CiscoG7231AR,                        // source format
  L16Desc,                             // destination format

  NULL,                               // user data

  8000,                                // samples per second
  6300,                                // raw bits per second
  30000,                               // nanoseconds per frame
  240,                                 // samples per frame
  24,                                  // bytes per frame
  1,                                   // recommended number of frames per packet
  4,                                   // maximum number of frames per packet
  ianaCode,                            // IANA RTP payload code
  NULL,                                // No IANA payload name for SDP

  create_decoder,                      // create codec function
  destroy_decoder,                     // destroy codec
  codec_decoder,                       // encode/decode
  NULL,                                // codec controls

  PluginCodec_H323Codec_nonStandard,   // h323CapabilityType
  &g7231arCap                          // h323CapabilityData 
},
};

#define NUM_DEFNS   (sizeof(g7231CodecDefn) / sizeof(PluginCodec_Definition))

/////////////////////////////////////////////////////////////////////////////

extern "C" {

PLUGIN_CODEC_IMPLEMENT(G7231_ACM)

PLUGIN_CODEC_DLL_API PluginCodec_Definition * PLUGIN_CODEC_GET_CODEC_FN(unsigned * count, unsigned /*version*/)
{
  *count = NUM_DEFNS;
  return g7231CodecDefn;
}

};
