/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <phk@FreeBSD.org> wrote this file.  As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.   Poul-Henning Kamp
 * ----------------------------------------------------------------------------
 *
 * Extract DTMF signals from 16 bit PCM audio
 *
 * Originally written by Poul-Henning Kamp <phk@freebsd.org>
 * Made into a C++ class by Roger Hardiman <roger@freebsd.org>, January 2002
 *
 * $Log: dtmf.h,v $
 * Revision 1.11  2007/07/19 08:10:18  csoutheren
 * Add detection of CNG
 *
 * Revision 1.10  2007/04/19 04:33:53  csoutheren
 * Fixed problems with pre-compiled headers
 *
 * Revision 1.9  2006/12/13 04:56:03  csoutheren
 * Applied 1613270 - fixed for dtmfEncoder
 * Thanks to Frederic Heem
 *
 * Revision 1.8  2006/10/25 08:18:20  rjongbloed
 * Major upgrade of tone generation subsystem.
 *
 * Revision 1.7  2005/11/30 12:47:37  csoutheren
 * Removed tabs, reformatted some code, and changed tags for Doxygen
 *
 * Revision 1.6  2004/11/11 07:34:50  csoutheren
 * Added #include <ptlib.h>
 *
 * Revision 1.5  2004/09/09 23:50:48  csoutheren
 * Fixed problem with duplicate definition of sinetab causing problems
 *
 * Revision 1.4  2004/09/09 05:23:37  dereksmithies
 * Add utility function to report on dtmf characters used.
 *
 * Revision 1.3  2004/09/09 04:00:00  csoutheren
 * Added DTMF encoding functions
 *
 * Revision 1.2  2002/09/16 01:08:59  robertj
 * Added #define so can select if #pragma interface/implementation is used on
 *   platform basis (eg MacOS) rather than compiler, thanks Robert Monaghan.
 *
 * Revision 1.1  2002/01/23 11:43:26  rogerh
 * Add DTMF Decoder class. This can be passed PCM audio data
 * (at 16 bit, 8 KHz) and returns any DTMF codes detected.
 * Tested with NetMeeting sending DTMF over a G.711 stream.
 *
 */
 
#ifndef _DTMF_H
#define _DTMF_H

#ifdef P_USE_PRAGMA
#pragma interface
#endif

#ifndef _PTLIB_H
#include <ptlib.h>
#endif

class PDTMFDecoder : public PObject
{
  PCLASSINFO(PDTMFDecoder, PObject)

  public:
    PDTMFDecoder();
    PString Decode(const short * sampleData, PINDEX numSamples);

  protected:
    enum {
      NumTones = 9
    };

    // key lookup table (initialised once)
    char key[256];

    // frequency table (initialised once)
    int p1[NumTones];

    // variables to be retained on each cycle of the decode function
    int h[NumTones], k[NumTones], y[NumTones];
    int nn, so, ia;
};


/** This class can be used to generate PCM data for tones (such as telephone
    calling tones and DTMF) at a sample rate of 8khz.

    The class contains a  master volume which is applied as well as the
    individual tone volumes. Thus a master volume ot 50% and a tone voluem
    of 50%  would result in a net volume of 25%.

    Tones may be described via a list of descriptor strings based on an
    ITU-T "semi-standard", one used within various standard documents but not
    a standard in itself. This format was enhanced to allow for multiple
    tones and volume indications.

    The basic format is:

          [volume % ] frequency ':' cadence [ '/' ... ]

      where frequency is one of
          frequency         single frequency tone
          freq1 '+' freq2   two frequency juxtaposed (simple mixing)
          freq1 'x' freq2   first frequency modulated by second frequency
          freq1 '-' freq2   Alternate frequencies, generated tone is freq1
                            used for compatibility with tone filters
      and cadence is
          mintime
          ontime '-' offtime [ '-' ontime '-' offtime [ ... ] ]

      and volume is a percentage of full volume

      examples:
          300:0.25              300Hz for minimum 250ms
          1100:0.4-0.4          1100Hz with cadence 400ms on, 400ms off
          900-1300:1.5          900Hz for 1.5 seconds
          350+440:1             350Hz superimposed with 440Hz (US dial tone) for 1 second
          425x15:0.4-0.2-0.4-2  425Hz modulated with 15Hz (Aus ring back tone)
                                with cadence 400ms on, 200ms off, 400ms on, 2s off
          425:0.4-0.1/50%425:0.4-0.1   425Hz with cadence 400ms on, 100ms off,
                                       400ms on, 100ms off, where second tone is
                                       reduced in volume by 50%

      A database of tones for all contries in the worls is available at:
          http://www.3amsystems.com/wireline/tone-search.htm

  */
class PTones : public PShortArray
{
  PCLASSINFO(PTones, PShortArray)

  public:
    enum {
        MaxVolume = 100,
        SampleRate = 8000,
        MaxFrequency = (SampleRate/4),
        MinFrequency = 30,
        MinModulation = 5,
        SineScale = 1000
    };

    /** Create an empty tone buffer. Tones added will use the specified
        master volume.
      */
    PTones(
        unsigned masterVolume = MaxVolume ///< Percentage volume
    );

    /** Create a filled tone buffer using the specified descriptor.
      */
    PTones(
      const PString & descriptor,    ///< Descriptor string for tone(s). See class notes.
      unsigned masterVolume = MaxVolume ///< Percentage volume
    );

    /** Generate a tone using the specified descriptor.
        See class general notes for format of the descriptor string.
      */
    bool Generate(
      const PString & descriptor    ///< Descriptor string for tone(s). See class notes.
    );

    /** Generate a tone using the specified values.
        The operation parameter may be '+', 'x', '-' or ' ' for summing, modulation,
        pure tone or silence resepctively.
        The tones duration is always rounded up to the nearest even multiple of the
        tone cycle to assure correct zero crossing when tones change.
      */
    bool Generate(
      char operation,
      unsigned frequency1,        ///< Primary frequency for tone
      unsigned frequency2,        ///< Secondary frequency for summing or modulation
      unsigned milliseconds,      ///< Duration of tone
      unsigned volume = MaxVolume ///< Percentage volume
    );

  protected:
    bool Juxtapose(unsigned frequency1, unsigned frequency2, unsigned milliseconds, unsigned volume);
    bool Modulate (unsigned frequency, unsigned modulate, unsigned milliseconds, unsigned volume);
    bool PureTone (unsigned frequency, unsigned milliseconds, unsigned volume);
    bool Silence  (unsigned milliseconds);

    unsigned CalcSamples(unsigned milliseconds, unsigned frequency1, unsigned frequency2 = 0);

    void AddSample(int sample, unsigned volume);

    unsigned masterVolume;
    char     lastOperation;
    unsigned lastFrequency1, lastFrequency2;
    int      angle1, angle2;
};


/**
  * this class can be used to generate PCM data for DTMF tones
  * at a sample rate of 8khz
  */
class PDTMFEncoder : public PTones
{
  PCLASSINFO(PDTMFEncoder, PTones)

  public:
    enum { DefaultToneLen = 100 };

    /**
      * Create PCM data for the specified DTMF sequence 
      */
    PDTMFEncoder(
        const char * dtmf = NULL,      ///< character string to encode
        unsigned milliseconds = DefaultToneLen  ///< length of each DTMF tone in milliseconds
    );

    /**
      * Create PCM data for the specified dtmf key
      */
    PDTMFEncoder(
        char key,      ///< character string to encode
        unsigned milliseconds = DefaultToneLen  ///< length of each DTMF tone in milliseconds
    );    

    /**
      * Add the PCM data for the specified tone sequence to the buffer
      */
    void AddTone(
        const char * str,              ///< string to encode
        unsigned milliseconds = DefaultToneLen  ///< length of DTMF tone in milliseconds
    );

    /**
      * Add the PCM data for the specified tone to the buffer
      */
    void AddTone(
        char ch,                       ///< character to encode
        unsigned milliseconds = DefaultToneLen  ///< length of DTMF tone in milliseconds
    );

    /**
      * Add the PCM data for the specified dual-frequency tone to the buffer
      * frequency2 can be zero, which will generate a single frequency tone
      */
    void AddTone(
        double frequency1,                  // primary frequency
        double frequency2 = 0,              // secondary frequency, or 0 if no secondary frequency
        unsigned milliseconds = DefaultToneLen  // length of DTMF tone in milliseconds
    );

    /**
      * Generate PCM data for a single cadence of the US standard ring tone
      * of 440/480hz for 2 seconds, followed by 5 seconds of silence
      */
    void GenerateRingBackTone()
    {
      Generate("440+480:2-4");
    }

    /**
      * Generate PCM data for 1 second of US standard dial tone 
      * of 350/440hz 
      */
    void GenerateDialTone()
    {
      Generate("350+440:1");
    }

    /**
      * Generate PCM data for a single cadence of the US standard busy tone
      * of 480/620hz for 1/2 second, 1/2 second of silence
      */
    void GenerateBusyTone()
    {
      Generate("480+620:0.5-0.5");
    }

    /**
     * Convenience function to get the ASCII character for a DTMF index, 
     * where the index varies from 0 to 15
     *
     * @returns ASCII value
     */

    char DtmfChar(
        PINDEX i    ///< index of tone
    );
    // Overiding GetSize() screws up the SetSize()
};

#endif /* _DTMF_H */
