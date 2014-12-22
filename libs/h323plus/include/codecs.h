/*
 * codecs.h
 *
 * H.323 protocol handler
 *
 * Open H323 Library
 *
 * Copyright (c) 1998-2000 Equivalence Pty. Ltd.
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
 * The Original Code is Open H323 Library.
 *
 * The Initial Developer of the Original Code is Equivalence Pty. Ltd.
 *
 * Portions of this code were written with the assisance of funding from
 * Vovida Networks, Inc. http://www.vovida.com.
 *
 * Contributor(s): ______________________________________.
 *
 * $Log: codecs.h,v $
 * Revision 1.2  2007/10/19 19:53:44  shorne
 * ported latest Video updates in OpenH323 committed after h323plus initial fork thanks
 *  Robert
 *
 * Revision 1.1  2007/08/06 20:50:48  shorne
 * First commit of h323plus
 *
 * Revision 1.81.2.5  2007/07/19 20:10:27  shorne
 * Changed HAS_AEC to H323_AEC
 *
 * Revision 1.81.2.4  2007/02/10 23:56:40  shorne
 * Added comment
 *
 * Revision 1.81.2.3  2007/02/05 05:17:00  shorne
 * Added ability to send general codec options to Video Plugins
 *
 * Revision 1.81.2.2  2007/01/30 14:39:55  shorne
 * Corrections and Improvements patch supplied by Guilhem Tardy
 *
 * Revision 1.81.2.1  2006/12/23 19:08:02  shorne
 * Plugin video codecs & sundry
 *
 * Revision 1.81  2006/08/01 06:17:48  csoutheren
 * Fix bit rate control units
 * Add access function on code for max bit rate
 * Thanks to Paolo Amadini
 *
 * Revision 1.80  2006/05/16 11:28:58  shorne
 * added AEC support and  more call hold support.
 *
 * Revision 1.79  2006/01/26 03:31:08  shorne
 * Add the ability to remove a local input device when placing a call on hold
 *
 * Revision 1.78  2005/11/30 13:05:01  csoutheren
 * Changed tags for Doxygen
 *
 * Revision 1.77  2005/06/07 07:09:13  csoutheren
 * Removed compiler warnings on Windows
 *
 * Revision 1.76  2005/06/07 03:22:22  csoutheren
 * Added patch 1198741 with support for plugin codecs with generic capabilities
 * Added patch 1198754 with support for setting quality level on audio codecs
 * Added patch 1198760 with GSM-AMR codec support
 * Many thanks to Richard van der Hoff for his work
 *
 * Revision 1.75  2005/01/03 06:25:52  csoutheren
 * Added extensive support for disabling code modules at compile time
 *
 * Revision 1.74  2004/07/03 06:48:26  rjongbloed
 * Split video temporal/spatial trade off H.245 packets to separate command and
 *   indication functions and added quality parameter, thanks Guilhem Tardy.
 *
 * Revision 1.73  2004/05/09 10:08:34  csoutheren
 * Changed new DecodeFrame to return bytes decoded rather than samples decoded
 * Added support for new DecodeFrame to plugin manager
 *
 * Revision 1.72  2004/05/02 04:52:23  rjongbloed
 * Fixed problems with G.711 caused by fixing problem with G.723.1-5k3 mode.
 *
 * Revision 1.71  2004/03/24 00:39:12  dereksmithies
 * Changes from Michael Smith for use in reporting frames/second. Many Thanks
 *
 * Revision 1.70  2003/12/14 10:42:29  rjongbloed
 * Changes for compilability without video support.
 *
 * Revision 1.69  2003/11/12 11:14:21  csoutheren
 * Added H323FramedAudioCodec::DecodeSilenceFrame thanks to Henry Harrison of AliceStreet
 *
 * Revision 1.68  2003/07/16 10:43:12  csoutheren
 * Added SwapChannel function to H323Codec to allow media hold channels
 * to work better. Thanks to Federico Pinna
 *
 * Revision 1.67  2003/05/27 09:39:15  dereksmithies
 * Add frames per sec variable to the videocodec class
 *
 * Revision 1.66  2002/12/16 09:11:15  robertj
 * Added new video bit rate control, thanks Walter H. Whitlock
 *
 * Revision 1.65  2002/09/16 01:14:15  robertj
 * Added #define so can select if #pragma interface/implementation is used on
 *   platform basis (eg MacOS) rather than compiler, thanks Robert Monaghan.
 *
 * Revision 1.64  2002/08/05 10:03:47  robertj
 * Cosmetic changes to normalise the usage of pragma interface/implementation.
 *
 * Revision 1.63  2002/01/23 06:13:52  robertj
 * Added filter function hooks to codec raw data channel.
 *
 * Revision 1.62  2002/01/23 01:58:25  robertj
 * Added function to determine if codecs raw data channel is native format.
 *
 * Revision 1.61  2002/01/22 16:17:19  rogerh
 * Back out the DTMF Decoder class. There will soon be a better way
 * to do this.
 *
 * Revision 1.60  2002/01/22 16:09:38  rogerh
 * Back out the DTMF detection from H323FramedAudioCodec::Write().
 * There will shortly be a better place for it.
 *
 * Revision 1.59  2002/01/22 15:21:47  rogerh
 * Add DTMF decoding to PCM audio streams. This has been tested with
 * NetMeeting sending Dial Pad codes, using the G.711 codec.
 * At this time, DTMF codes (fron NetMeeting) are just displayed on the
 * screen and are not passed up to the users application.
 *
 * Revision 1.58  2002/01/13 23:56:01  robertj
 * Added mutex so can change raw data channel while reading/writing from codec.
 *
 * Revision 1.57  2001/12/04 05:13:09  robertj
 * Added videa bandwidth limiting code for H.261, thanks Jose Luis Urien.
 *
 * Revision 1.56  2001/11/16 01:05:21  craigs
 * Changed to allow access to uLaw/ALaw to/from linear functions
 *
 * Revision 1.55  2001/10/23 02:18:06  dereks
 * Initial release of CU30 video codec.
 *
 * Revision 1.54  2001/09/25 03:14:47  dereks
 * Add constant bitrate control for the h261 video codec.
 * Thanks Tiziano Morganti for the code to set bit rate. Good work!
 *
 * Revision 1.53  2001/09/11 01:24:36  robertj
 * Added conditional compilation to remove video and/or audio codecs.
 *
 * Revision 1.52  2001/08/28 09:27:38  robertj
 * Updated documentation to reflect some relationships between functions.
 *
 * Revision 1.51  2001/03/29 23:44:35  robertj
 * Added ability to get current silence detect state and threshold.
 * Changed silence detection to use G.723.1 SID frames as indicator of
 *   silence instead of using the average energy and adaptive threshold.
 * Changed default signal on deadband time to be much shorter.
 *
 * Revision 1.50  2001/02/09 05:16:24  robertj
 * Added #pragma interface for GNU C++.
 *
 * Revision 1.49  2001/01/25 07:27:14  robertj
 * Major changes to add more flexible OpalMediaFormat class to normalise
 *   all information about media types, especially codecs.
 *
 * Revision 1.48  2000/12/19 22:33:44  dereks
 * Adjust so that the video channel is used for reading/writing raw video
 * data, which better modularizes the video codec.
 *
 * Revision 1.47  2000/10/13 01:47:26  dereks
 * Include command option for setting the number of transmitted video
 * frames per second.   use --videotxfps n
 *
 * Revision 1.46  2000/09/08 06:41:37  craigs
 * Added ability to set video device
 * Added ability to select test input frames
 *
 * Revision 1.45  2000/08/31 08:15:32  robertj
 * Added support for dynamic RTP payload types in H.245 OpenLogicalChannel negotiations.
 *
 * Revision 1.44  2000/08/21 04:45:06  dereks
 * Fix dangling pointer that caused segfaults for windows&unix users.
 * Improved the test image which is used when video grabber won't open.
 * Added code to handle setting of video Tx Quality.
 * Added code to set the number of background blocks sent with every frame.
 *
 * Revision 1.43  2000/05/16 02:04:16  craigs
 * Added access functions for silence compression mode
 *
 * Revision 1.42  2000/05/02 04:32:24  robertj
 * Fixed copyright notice comment.
 *
 * Revision 1.41  2000/04/14 20:11:16  robertj
 * Removed redundent member variable from H323FramedAudioCodec.
 *
 * Revision 1.40  2000/03/30 23:10:50  robertj
 * Fixed error in comments regarding GetFramerate() function.
 *
 * Revision 1.39  2000/03/21 03:06:47  robertj
 * Changes to make RTP TX of exact numbers of frames in some codecs.
 *
 * Revision 1.38  2000/02/28 13:00:22  robertj
 * Added some virtual keywords at the request of Yura Ershov
 *
 * Revision 1.37  2000/02/10 03:08:02  craigs
 * Added ability to specify NTSC or PAL video format
 *
 * Revision 1.36  2000/02/04 05:00:08  craigs
 * Changes for video transmission
 *
 * Revision 1.35  2000/01/13 04:03:45  robertj
 * Added video transmission
 *
 * Revision 1.34  1999/12/31 00:05:36  robertj
 * Added Microsoft ACM G.723.1 codec capability.
 *
 * Revision 1.33  1999/12/29 01:19:16  craigs
 * Fixed problem with RTP payload type variable
 *
 * Revision 1.32  1999/12/23 23:02:34  robertj
 * File reorganision for separating RTP from H.323 and creation of LID for VPB support.
 *
 * Revision 1.31  1999/11/29 09:01:46  craigs
 * Added new code for new video code interface
 *
 * Revision 1.30  1999/11/29 04:50:11  robertj
 * Added adaptive threshold calculation to silence detection.
 *
 * Revision 1.29  1999/11/20 00:53:46  robertj
 * Fixed ability to have variable sized frames in single RTP packet under G.723.1
 *
 * Revision 1.28  1999/11/13 14:10:32  robertj
 * Changes to make silence detection selectable.
 *
 * Revision 1.27  1999/11/11 23:28:52  robertj
 * Added first cut silence detection algorithm.
 *
 * Revision 1.26  1999/11/04 00:43:42  robertj
 * Added extra constructors for nonStandard codecs.
 *
 * Revision 1.25  1999/10/14 12:03:08  robertj
 * Fixed comment.
 *
 * Revision 1.24  1999/10/08 09:59:01  robertj
 * Rewrite of capability for sending multiple audio frames
 *
 * Revision 1.23  1999/10/08 04:58:37  robertj
 * Added capability for sending multiple audio frames in single RTP packet
 *
 * Revision 1.22  1999/09/23 07:25:12  robertj
 * Added open audio and video function to connection and started multi-frame codec send functionality.
 *
 * Revision 1.21  1999/09/21 14:51:33  robertj
 * Fixed NonStandardCapabilityInfo class virtual destructor (and name).
 *
 * Revision 1.20  1999/09/21 14:04:41  robertj
 * Added non-standard codec capability classes
 *
 * Revision 1.19  1999/09/21 08:12:49  craigs
 * Added support for video codecs and H261
 *
 * Revision 1.18  1999/09/18 13:27:24  craigs
 * Added ability disable jitter buffer for codecs
 * Added ability to access entire RTP frame from codec Write
 *
 * Revision 1.17  1999/09/08 04:05:48  robertj
 * Added support for video capabilities & codec, still needs the actual codec itself!
 *
 * Revision 1.16  1999/08/31 12:34:18  robertj
 * Added gatekeeper support.
 *
 * Revision 1.15  1999/08/25 03:08:23  robertj
 * Added UserInput capability.
 * Allowed the attachment of a channel on a codec to optionally delete the channel object,
 *
 * Revision 1.14  1999/07/22 14:32:48  robertj
 * Fixed erroneous comments
 *
 * Revision 1.13  1999/07/16 16:05:48  robertj
 * Added "human readable" codec type name display.
 *
 * Revision 1.12  1999/07/15 14:45:35  robertj
 * Added propagation of codec open error to shut down logical channel.
 * Fixed control channel start up bug introduced with tunnelling.
 *
 * Revision 1.11  1999/07/13 09:53:24  robertj
 * Fixed some problems with jitter buffer and added more debugging.
 *
 * Revision 1.10  1999/07/09 06:09:49  robertj
 * Major implementation. An ENORMOUS amount of stuff added everywhere.
 *
 * Revision 1.9  1999/06/24 13:32:45  robertj
 * Fixed ability to change sound device on codec and fixed NM3 G.711 compatibility
 *
 * Revision 1.8  1999/06/22 13:49:40  robertj
 * Added GSM support and further RTP protocol enhancements.
 *
 * Revision 1.7  1999/06/14 15:08:40  robertj
 * Added GSM codec class frame work (still no actual codec).
 *
 * Revision 1.6  1999/06/14 06:39:08  robertj
 * Fixed problem with getting transmit flag to channel from PDU negotiator
 *
 * Revision 1.5  1999/06/14 05:15:55  robertj
 * Changes for using RTP sessions correctly in H323 Logical Channel context
 *
 * Revision 1.4  1999/06/13 12:41:14  robertj
 * Implement logical channel transmitter.
 * Fixed H245 connect on receiving call.
 *
 * Revision 1.3  1999/06/09 05:26:19  robertj
 * Major restructuring of classes.
 *
 * Revision 1.2  1999/06/06 06:06:36  robertj
 * Changes for new ASN compiler and v2 protocol ASN files.
 *
 * Revision 1.1  1999/01/16 01:31:01  robertj
 * Initial revision
 *
 */

#ifndef __CODECS_H
#define __CODECS_H

#ifdef P_USE_PRAGMA
#pragma interface
#endif


#include <mediafmt.h>
#include <rtp.h>
#include <channels.h>
#include "openh323buildopts.h"


/* The following classes have forward references to avoid including the VERY
   large header files for H225 and H245. If an application requires access
   to the protocol classes they can include them, but for simple usage their
   inclusion can be avoided.
 */
class H245_MiscellaneousCommand_type;
class H245_MiscellaneousIndication_type;
class H323Connection;



///////////////////////////////////////////////////////////////////////////////

/**This class embodies the implementation of a specific codec instance
   used to transfer data via the logical channels opened and managed by the
   H323 control channel.

   An application may create a descendent off this class and override
   functions as required for descibing a codec.
 */
class PAec;
class H323Codec : public PObject
{
  PCLASSINFO(H323Codec, PObject);

  public:
    enum Direction {
      Encoder,
      Decoder
    };

    H323Codec(
      const OpalMediaFormat & mediaFormat, ///< Media format for codec
      Direction direction       ///< Direction in which this instance runs
    );


    /**Open the codec.
       This will open the codec for encoding or decoding, it is called after
       the logical channel have been established and the background threads to
       drive them have been started. This is primarily used to delay allocation
       of resources until the last millisecond.

       A descendent class may be created by the application and it may cast
       the connection parameter to the application defined descendent of 
       H323Connection to obtain information needed to open the codec.

       The default behaviour does nothing.
      */
    virtual BOOL Open(
      H323Connection & connection ///< Connection between the endpoints
    );

    /**Close the codec.
      */
    virtual void Close() = 0;

    /**Encode the data from the appropriate device.
       This will encode data for transmission. The exact size and description
       of the data placed in the buffer is codec dependent but should be less
       than OpalMediaFormat::GetFrameSize() in length.

       The length parameter is filled with the actual length of the encoded
       data, often this will be the same as OpalMediaFormat::GetFrameSize().

       This function is called every GetFrameRate() timestamp units, so MUST
       take less than (or equal to) that amount of time to complete. It
       should always return the amount of data that corresponds to the
       GetFrameRate() timestamp units as well.

       A returned length of zero indicates that time has passed but there is
       no data encoded. This is typically used for silence detection in an
       audio codec.
     */
    virtual BOOL Read(
      BYTE * buffer,            ///< Buffer of encoded data
      unsigned & length,        ///< Actual length of encoded data buffer
      RTP_DataFrame & rtpFrame  ///< RTP data frame
    ) = 0;

    /**Decode the data and output it to appropriate device.
       This will decode a single frame of received data. The exact size and
       description of the data required in the buffer is codec dependent but
       should be at least than OpalMediaFormat::GetFrameSize() in length.

       It is expected this function anunciates the data. That is, for example
       with audio data, the sound is output on a speaker.

       This function is called every GetFrameRate() timestamp units, so MUST
       take less than that amount of time to complete!
     */
    virtual BOOL Write(
      const BYTE * buffer,          ///< Buffer of encoded data
      unsigned length,              ///< Length of encoded data buffer
      const RTP_DataFrame & frame,  ///< Entire RTP frame
      unsigned & written            ///< Number of bytes used from data buffer
    ) = 0;

    /**Get the frame rate in RTP timestamp units.
      */
    virtual unsigned GetFrameRate() const;

    /**Limit bit flow for the logical channel.
       The default behaviour does nothing.
     */
    virtual void OnFlowControl(
      long bitRateRestriction   ///< Bit rate limitation
    );

    /**Process a miscellaneous command on the logical channel.
       The default behaviour does nothing.
     */
    virtual void OnMiscellaneousCommand(
      const H245_MiscellaneousCommand_type & type  ///< Command to process
    );

    /**Process a miscellaneous indication on the logical channel.
       The default behaviour does nothing.
     */
    virtual void OnMiscellaneousIndication(
      const H245_MiscellaneousIndication_type & type  ///< Indication to process
    );

    Direction GetDirection()   const { return direction; }

    virtual const OpalMediaFormat & GetMediaFormat() const { return mediaFormat; }
    OpalMediaFormat & GetWritableMediaFormat() { return mediaFormat; }

    virtual BOOL SetFrameSize(int /*frameWidth*/, int /*frameHeight*/) { return FALSE; };
    virtual void SetVideoSize(int, int) { return; };

    /**Attach the raw data channel for use by codec.
       Note the channel provided will be deleted on destruction of the codec.
       
       The channel connects the codec (audio or video) with hardware to read/write data.
       Thus, the video codec provides a pointer to the data, which the renderer/grabber
       then accesses to display/grab the image from/to.
      */
    virtual BOOL AttachChannel(
      PChannel * channel,     ///< Channel to read/write raw codec data
      BOOL autoDelete = TRUE  ///< Channel is to be automatically deleted
    );

    /**Attach a new channel and returns the previous one, without neither
       deleting it nor closing it. This method is used to, for example,
       when swapping to or from a hold media channel
     */
    virtual PChannel * SwapChannel(
      PChannel * newChannel,  ///< Channel to read/write raw codec data
      BOOL autoDelete = TRUE  ///< Channel is to be automatically deleted
    );

    /**Close the raw data channel, described in H323Codec::AttachChannel
      */
    virtual BOOL CloseRawDataChannel();

    /**Return a pointer to the raw data channel, which can then be used to
       access the recording/playing device. (or testing if channel is attached).
    */
    PChannel *GetRawDataChannel()
      {	return rawDataChannel; }

    /**Return flag indicating raw channel is native.
       For audio codecs, FALSE typically means that the format is PCM-16.
       For video codecs, FALSE typically means that the format is YUV411P.

       The default behaviour returns FALSE.
      */
    virtual BOOL IsRawDataChannelNative() const;

    /**Read from the raw data channel.
      */
    BOOL ReadRaw(
      void * data,
      PINDEX size,
      PINDEX & length
    );

    /**Write from the raw data channel.
      */
    BOOL WriteRaw(
      void * data,
      PINDEX length
    );

    /**Attach the logical channel, for use by the codec.
       The channel provided is not deleted on destruction, it is just used.

       The logical channel provides a means for the codec to send control messages.
       E.G. the receive video codec wants to receive a frame update.
    */
    BOOL AttachLogicalChannel(H323Channel *channel);

    class FilterInfo : public PObject {
        PCLASSINFO(FilterInfo, PObject);
      public:
        FilterInfo(H323Codec & c, void * b, PINDEX s, PINDEX l)
          : codec(c), buffer(b), bufferSize(s), bufferLength(l) { }

        H323Codec & codec;
        void      * buffer;
        PINDEX      bufferSize;
        PINDEX      bufferLength;
    };

    /**Add a filter to the codec.
       The call back function is executed just after reading from, or just
       before writing to, the raw data channel. The callback is passed the
       H323Codec::FilterInfo structure containing the data that is being
       read or written.

       To use define:
         PDECLARE_NOTIFIER(H323Codec::FilterInfo, YourClass, YourFunction);
       and
         void YourClass::YourFunction(H323Codec::FilterInfo & info, INT)
         {
           // DO something with data
         }
       and to connect to a codec:
         BOOL YourClass::OnStartLogicalChannel(H323Channel & channel)
         {
           H323Codec * codec = channel.GetCodec();
           codec->AddFilter(PCREATE_NOTIFIER(YourFunction));
         }
       for example. Other places can be used to set the filter.
      */
    void AddFilter(
      const PNotifier & notifier
    );

   /**SetRawDataHeld is called when the cuurent call has been held and the raw 
         data channel has been swapped out and released.
    */
    virtual BOOL SetRawDataHeld(BOOL hold );

    virtual void SetFormatString(const PString & _formatString) { formatString = _formatString; }
    virtual PString GetFormatString() { return formatString; }

    virtual void SetCacheMode(unsigned _cacheMode) { cacheMode = _cacheMode; }
    virtual unsigned GetCacheMode() { return cacheMode; }

    virtual void SetEncoderCacheKey(unsigned key) { encoderCacheKey = key; }
    virtual unsigned GetEncoderCacheKey() { return encoderCacheKey; }

    virtual int CheckCacheRTP() { return 0; }
    virtual void AttachCacheRTP() { return; }
    virtual void DetachCacheRTP() { return; }
    virtual void DeleteCacheRTP() { return; }
    virtual void NewCacheRTP() { return; }
    virtual int GetCacheUsersNumber() { return 0; }
    virtual unsigned GetEncoderSeqN() { return 0; }

  protected:
    Direction direction;
    OpalMediaFormat mediaFormat;

    H323Channel * logicalChannel; // sends messages from receive codec to tx codec.

    PChannel * rawDataChannel;  // connection to the hardware for reading/writing data.
    BOOL       deleteChannel;
    PMutex     rawChannelMutex;

    PINDEX     lastSequenceNumber;  // Detects lost RTP packets in the video codec.

    unsigned encoderSeqN;
    unsigned encoderCacheKey; // used for caching encoded frames
    unsigned cacheMode;       // 0 - no cache, 1 - cached, 2 - caching
    PString formatString;

    PLIST(FilterList, PNotifier);
    FilterList filters;
};

#ifndef NO_H323_AUDIO_CODECS


/**This class defines a codec class that will use the standard platform PCM
   output device.

   An application may create a descendent off this class and override
   functions as required for descibing a specific codec.
 */
class PAec;
class H323AudioCodec : public H323Codec
{
  PCLASSINFO(H323AudioCodec, H323Codec);

  public:
    /** Create a new audio codec.
        This opens the standard PCM audio output device, for input and output
        and allows descendent codec classes to do audio I/O after
        decoding/encoding.
      */
    H323AudioCodec(
      const OpalMediaFormat & mediaFormat, ///< Media format for codec
      Direction direction       ///< Direction in which this instance runs
    );

    ~H323AudioCodec();

    /**Open the codec.
       This will open the codec for encoding or decoding. This is primarily
       used to delay allocation of resources until the last minute.

       The default behaviour calls the H323EndPoint::OpenAudioChannel()
       function and assigns the result of that function to the raw data
       channel in the H323Codec class.
      */
    virtual BOOL Open(
      H323Connection & connection ///< Connection between the endpoints
    );

    /**Close down the codec.
       This will close the codec breaking any block on the Read() or Write()
       functions.

       The default behaviour will close the rawDataChannel if it is not NULL
       and thene delete it if delteChannel is TRUE.
      */
    virtual void Close();

    /**Get the frame rate in RTP timestamp units.
      */
    virtual unsigned GetFrameRate() const;
    
    virtual unsigned GetSampleRate() { return 0; }

    enum SilenceDetectionMode {
      NoSilenceDetection,
      FixedSilenceDetection,
      AdaptiveSilenceDetection
    };

    /**Enable/Disable silence detection.
       The deadband periods are in audio samples of 8kHz.
      */
    void SetSilenceDetectionMode(
      SilenceDetectionMode mode,   ///< New silence detection mode
      unsigned threshold = 0,      ///< Threshold value if FixedSilenceDetection
      unsigned signalDeadband = 80,    ///< 10 milliseconds of signal needed
      unsigned silenceDeadband = 3200, ///< 400 milliseconds of silence needed
      unsigned adaptivePeriod = 4800   ///< 600 millisecond window for adaptive threshold
    );

    /**Get silence detection mode

       The inTalkBurst value is TRUE if packet transmission is enabled and
       FALSE if it is being suppressed due to silence.

       The currentThreshold value is the value from 0 to 32767 which is used
       as the threshold value for 16 bit PCM data.
      */
    SilenceDetectionMode GetSilenceDetectionMode(
      BOOL * isInTalkBurst = NULL,        ///< Current silence detct state.
      unsigned * currentThreshold = NULL  ///< Current signal/silence threshold
    ) const;

    
    /** for codecs which support it, this sets the quality level of the
        transmitted audio.
        In order to have consistency between different codecs, the qlevel
        parameter is defined to range from 1 (good) to 31 (poor), even
        if the individual codec defines fewer levels than this.
     */
    virtual void SetTxQualityLevel(int /*qlevel*/) {}

    /** for codecs which support it, this gets the quality level of the
     * transmitted audio.
     */
    virtual int GetTxQualityLevel(int /*qlevel*/) { return 1; }
    
    /**Check frame for a talk burst.
       This does the deadband calculations on the average signal levels
       returned by the GetAverageSignalLevel() function and based on the
       levelThreshold, signalDeadbandFrames and silenceDeadbandFrames
       member variables.
      */
    virtual BOOL DetectSilence(unsigned sampleRate, unsigned codecChannels);

    /**Get the average signal level in the audio stream.
       This is called from within DetectSilence() to calculate the average
       signal level since the last call to DetectSilence().

       The default behaviour returns UINT_MAX which disables the silence
       detection algorithm.
      */
    virtual unsigned GetAverageSignalLevel();

   /**SetRawDataHeld is called when the call has been held and the raw 
      data channel has been swapped out and released for another connection.
      */
    virtual BOOL SetRawDataHeld(BOOL hold);

#ifdef H323_AEC	
	/** Attach Acoustic Echo Cancellation.
	*/
	virtual void AttachAEC(
       PAec * /*_ARC*/   ///* Acoustic Echo Cancellation Instance
    ) {};
#endif
	virtual void EnableAGC( int ) {};

  protected:
    unsigned samplesPerFrame;

    unsigned codecChannels;

    SilenceDetectionMode silenceDetectMode;

    unsigned signalDeadbandFrames;  // Frames of signal before talk burst starts
    unsigned silenceDeadbandFrames; // Frames of silence before talk burst ends
    unsigned adaptiveThresholdFrames; // Frames to min/max over for adaptive threshold

    BOOL     inTalkBurst;           // Currently sending RTP data
    unsigned framesReceived;        // Signal/Silence frames received so far.
    unsigned levelThreshold;        // Threshold level for silence/signal
    unsigned signalMinimum;         // Minimum of frames above threshold
    unsigned silenceMaximum;        // Maximum of frames below threshold
    unsigned signalFramesReceived;  // Frames of signal received
    unsigned silenceFramesReceived; // Frames of silence received
    BOOL	 IsRawDataHeld;
};


/**This class defines a codec class that will use the standard platform PCM
   output device, and the encoding/decoding has fixed blocks. That is each
   input block of n samples is encoded to exactly the same sized compressed
   data, eg G.711, GSM etc.

   An application may create a descendent off this class and override
   functions as required for descibing a specific codec.
 */
class PAec;
class H323FramedAudioCodec : public H323AudioCodec
{
  PCLASSINFO(H323FramedAudioCodec, H323AudioCodec);

  public:
    /** Create a new audio codec.
        This opens the standard PCM audio output device, for input and output
        and allows descendent codec classes to do audio I/O after
        decoding/encoding.
      */
    H323FramedAudioCodec(
      const OpalMediaFormat & mediaFormat, ///< Media format for codec
      Direction direction       ///< Direction in which this instance runs
    );

    /**Encode the data from the appropriate device.
       This will encode data for transmission. The exact size and description
       of the data placed in the buffer is codec dependent but should be less
       than OpalMediaFormat::GetFrameSize() in length.

       The length parameter is filled with the actual length of the encoded
       data, often this will be the same as OpalMediaFormat::GetFrameSize().

       This function is called every GetFrameRate() timestamp units, so MUST
       take less than (or equal to) that amount of time to complete. It
       should always return the amount of data that corresponds to the
       GetFrameRate() timestamp units as well.

       A returned length of zero indicates that time has passed but there is
       no data encoded. This is typically used for silence detection in an
       audio codec.
     */
    virtual BOOL Read(
      BYTE * buffer,            ///< Buffer of encoded data
      unsigned & length,        ///< Actual length of encoded data buffer
      RTP_DataFrame & rtpFrame  ///< RTP data frame
    );

    /**Decode the data and output it to appropriate device.
       This will decode a single frame of received data. The exact size and
       description of the data required in the buffer is codec dependent but
       should be less than H323Capability::GetRxFramesInPacket() *
       OpalMediaFormat::GetFrameSize()  in length.

       It is expected this function anunciates the data. That is, for example
       with audio data, the sound is output on a speaker.

       This function is called every GetFrameRate() timestamp units, so MUST
       take less than that amount of time to complete!
     */
    virtual BOOL Write(
      const BYTE * buffer,            ///< Buffer of encoded data
      unsigned length,                ///< Length of encoded data buffer
      const RTP_DataFrame & rtpFrame, ///< RTP data frame
      unsigned & written              ///< Number of bytes used from data buffer
    );


    /**Get the average signal level in the audio stream.
       This is called from within DetectSilence() to calculate the average
       signal level since the last call to DetectSilence().
      */
    virtual unsigned GetAverageSignalLevel();


    /**Encode a sample block into the buffer specified.
       The samples have been read and are waiting in the readBuffer member
       variable. it is expected this function will encode exactly
       bytesPerFrame bytes.
     */
    virtual BOOL EncodeFrame(
      BYTE * buffer,    ///< Buffer into which encoded bytes are placed
      unsigned & length ///< Actual length of encoded data buffer
    ) = 0;

    /**Decode a sample block from the buffer specified.
       The samples must be placed into the writeBuffer member variable. It is
       expected that exactly samplesPerFrame samples is decoded.
     */
    virtual BOOL DecodeFrame(
      const BYTE * buffer,   ///< Buffer from which encoded data is found
      unsigned length,       ///< Length of encoded data buffer
      unsigned & written,    ///< Number of bytes used from data buffer
      unsigned & bytesOutput ///< Number of bytes in decoded data
    );
    virtual BOOL DecodeFrame(
      const BYTE * buffer,  ///< Buffer from which encoded data is found
      unsigned length,      ///< Length of encoded data buffer
      unsigned & written    ///< Number of bytes used from data buffer
    );

    /**
      Called when a frame is missed due to late arrival or other reasons
      By default, this function fills the buffer with silence
      */
    virtual void DecodeSilenceFrame(
      void * buffer,  ///< Buffer from which encoded data is found
      unsigned length       ///< Length of encoded data buffer
    )
    { memset(buffer, 0, length); }

#ifdef H323_AEC	
	/** Attach Acoustic Echo Cancellation.
	*/
	virtual void AttachAEC(
       PAec * _ARC   ///* Acoustic Echo Cancellation Instance
    );
#endif

    virtual void EnableAGC(int);
    virtual void AutoGainControl(const short * pcm, unsigned samplesPerFrame, unsigned codecChannels, unsigned sampleRate, unsigned level, float* currVolCoef);

  protected:
    PAec * aec;     // Acoustic Echo Canceller
    PShortArray sampleBuffer;
    unsigned    bytesPerFrame;
    unsigned    sampleRate;
    unsigned long lastReadTime, readPerSecond; // lame values to calc sampleRate added by kay27
    float currVolCoef;
    int agc;
};


/**This class defines a codec class that will use the standard platform PCM
   output device, and the encoding/decoding is streamed. That is each input
   16 bit PCM sample is encoded to 8 bits or less of encoded data and no
   blocking of PCM data is required, eg G.711, G.721 etc.

   An application may create a descendent off this class and override
   functions as required for descibing a specific codec.
 */
class H323StreamedAudioCodec : public H323FramedAudioCodec
{
  PCLASSINFO(H323StreamedAudioCodec, H323FramedAudioCodec);

  public:
    /** Create a new audio codec.
        This opens the standard PCM audio output device, for input and output
        and allows descendent codec classes to do audio I/O after
        decoding/encoding.
      */
    H323StreamedAudioCodec(
      const OpalMediaFormat & mediaFormat, ///< Media format for codec
      Direction direction,      ///< Direction in which this instance runs
      unsigned samplesPerFrame, ///< Number of samples in a frame
      unsigned bits             ///< Bits per sample
    );

    /**Encode a sample block into the buffer specified.
       The samples have been read and are waiting in the readBuffer member
       variable. it is expected this function will encode exactly
       encodedBlockSize bytes.
     */
    virtual BOOL EncodeFrame(
      BYTE * buffer,    ///< Buffer into which encoded bytes are placed
      unsigned & length ///< Actual length of encoded data buffer
    );

    /**Decode a sample block from the buffer specified.
       The samples must be placed into the writeBuffer member variable. It is
       expected that no more than frameSamples is decoded. The return value
       is the number of samples decoded. Zero indicates an error.
     */
    virtual BOOL DecodeFrame(
      const BYTE * buffer,  ///< Buffer from which encoded data is found
      unsigned length,      ///< Length of encoded data buffer
      unsigned & written,   ///< Number of bytes used from data buffer
      unsigned & samples    ///< Number of sample output from frame
    );

    /**Encode a single sample value.
     */
    virtual int Encode(short sample) const = 0;

    /**Decode a single sample value.
     */
    virtual short Decode(int sample) const = 0;

  protected:
    unsigned bitsPerSample;
};

#endif // NO_H323_AUDIO_CODECS


#ifndef NO_H323_VIDEO

/**This class defines a codec class that will use the standard platform image
   output device.

   An application may create a descendent off this class and override
   functions as required for descibing a specific codec.
 */
class H323VideoCodec : public H323Codec
{
  PCLASSINFO(H323VideoCodec, H323Codec);

  public:
    /** Create a new video codec.
        This opens the standard image output device, for input and output
        and allows descendent codec classes to do video I/O after
        decoding/encoding.
      */
    H323VideoCodec(
      const OpalMediaFormat & mediaFormat, ///< Media format for codec
      Direction direction      ///< Direction in which this instance runs
    );

    ~H323VideoCodec();

    /**Open the codec.
       This will open the codec for encoding or decoding. This is primarily
       used to delay allocation of resources until the last minute.

       The default behaviour calls the H323EndPoint::OpenVideoDevice()
       function and assigns the result of that function to the raw data
       channel in the H323Codec class.
      */
    virtual BOOL Open(
      H323Connection & connection ///< Connection between the endpoints
    );

    /**Close down the codec.
       This will close the codec breaking any block on the Read() or Write()
       functions.

       The default behaviour will close the rawDataChannel if it is not NULL
       and thene delete it if delteChannel is TRUE.
      */
    virtual void Close();


    /**Process a miscellaneous command on the logical channel.
       The default behaviour does nothing.
     */
    virtual void OnMiscellaneousCommand(
      const H245_MiscellaneousCommand_type & type  ///< Command to process
    );

    /**Process a miscellaneous indication on the logical channel.
       The default behaviour does nothing.
     */
    virtual void OnMiscellaneousIndication(
      const H245_MiscellaneousIndication_type & type  ///< Indication to process
    );

    //    /**Attach the raw data device for use by codec.
    //   Note the device provided will be deleted on destruction of the codec.
    //   */
    // virtual BOOL AttachDevice(
    //  H323VideoDevice * device, ///< Device to read/write data
    //  BOOL autoDelete = TRUE    ///< Device is to be automatically deleted
    // );

    /**Process a FreezePicture command from remote endpoint.
       The default behaviour does nothing.
     */
    virtual void OnFreezePicture();
    virtual void OnFreezeVideo(int disable);



    /**Process a FastUpdatePicture command from remote endpoint.
       The default behaviour does nothing.
     */
    virtual void OnFastUpdatePicture();

    /**Process a FastUpdateGOB command from remote endpoint.
       The default behaviour does nothing.
     */
    virtual void OnFastUpdateGOB(unsigned firstGOB, unsigned numberOfGOBs);

    /**Process a FastUpdateMB command from remote endpoint.
       The default behaviour does nothing.
     */
    virtual void OnFastUpdateMB(int firstGOB, int firstMB, unsigned numberOfMBs);

    /**Process a H.245 videoIndicateReadyToActivate indication from remote endpoint.
       The default behaviour does nothing.
     */
    virtual void OnVideoIndicateReadyToActivate();

    /**Process a H.245 ideoTemporalSpatialTradeOff command from remote endpoint.
       The default behaviour does nothing.
     */
    virtual void OnVideoTemporalSpatialTradeOffCommand(int newQuality);

    /**Process a H.245 videoTemporalSpatialTradeOff indication from remote endpoint.
       The default behaviour does nothing.
     */
    virtual void OnVideoTemporalSpatialTradeOffIndication(int newQuality);

    /**Process a H.245 videoNotDecodedMBs indication from remote endpoint.
       The default behaviour does nothing.
     */
    virtual void OnVideoNotDecodedMBs(
      unsigned firstMB,
      unsigned numberOfMBs,
      unsigned temporalReference
    );

    /**Process a request for a new frame, 
       as part of the picture has been lost.
    */
    virtual void OnLostPartialPicture();

    /**Process a request for a new frame, 
       as the entire picture has been lost.
    */
    virtual void OnLostPicture();

    /** Get width of video
     */ 
    virtual unsigned GetWidth() const { return frameWidth; }

    /** Get height of video
     */ 
    virtual unsigned GetHeight() const { return frameHeight; }

    /** Get frame rate of video
     */ 
    virtual unsigned GetTargetFrameRate() const { if(targetFrameTimeMs==0) return 30; else return 1000/targetFrameTimeMs; }

    /**Quality of the transmitted video. 1 is good, 31 is poor.
     */
    virtual void SetTxQualityLevel(int qlevel) {videoQuality = qlevel; }

    /**Minimum quality limit for the transmitted video.
     * Default is 1.  Encode quality will not be set below this value.
     */
    virtual void SetTxMinQuality(int qlevel) {videoQMin = qlevel; }

    /**Maximum quality limit for the transmitted video.
     * Default is 24.  Encode quality will not be set above this value.
     */
    virtual void SetTxMaxQuality(int qlevel) {videoQMax = qlevel; }

    /**number of blocks (that haven't changed) transmitted with each 
       frame. These blocks fill in the background */
    virtual void SetBackgroundFill(int idle) {fillLevel= idle; }
 
    enum BitRateModeBits {
      None                = 0x00,
      DynamicVideoQuality = 0x01,
      AdaptivePacketDelay = 0x02
    };

    /**Get the current value for video control mode
     */
    virtual unsigned GetVideoMode(void) {return videoBitRateControlModes;}

    /**Set the current value for video control mode
     * return the resulting value video control mode
     */
    virtual void SetVideoMode(int mode) {videoBitRateControlModes = mode;}

    /**Set maximum bitrate when transmitting video, in bps. A value of 0
       disables bit rate control. The average bitrate will be less depending
       on channel dead time, i.e. time that the channel could be transmitting
       bits but is not.

       @return TRUE if success
    */
    virtual BOOL SetMaxBitRate(
      unsigned bitRate ///< New bit rate
    );

    /**Get the current value of the maximum bitrate, in bps. If SetMaxBitRate
       was never called, the return value depends on the derived class
       implementation.
    */
    virtual unsigned GetMaxBitRate() const { return bitRateHighLimit; }

    /**Set target time in milliseconds between video frames going through
       the channel.  This sets the video frame rate through the channel,
       which is <= grabber frame rate.  Encoder quality will be adjusted
       dynamically by the codec to find a frame size that allows sending
       at this rate.  Default = 167 ms = 6 frames per second.  A value of 0
       means the channel will attempt to run at the video grabber frame rate
       Sometimes the channel cannot transmit as fast as the video grabber.
    */
    virtual BOOL SetTargetFrameTimeMs(
      unsigned ms ///< new time between frames
    );

    /**
       Set a miscellaneous option setting in the video codec.
       This message is used for Video Plugin Codecs.
    */
    virtual void SetGeneralCodecOption(
		const char * opt,    ///< Option string to set
		int val              ///< New Value to set to
	);

    /**
       Send a miscellaneous command to the remote transmitting video codec.
       This message is sent via the H245 Logical Channel.
    */
    void SendMiscCommand(unsigned command);
 
   /** 
       Returns the number of frames transmitted or received so far. 
   */
   virtual int GetFrameNum() { return frameNum; }

   PMutex & GetVideoHandlerMutex() { return videoHandlerActive; }

  protected:

    int frameWidth;
    int frameHeight;
    int fillLevel;

    // used in h261codec.cxx
    unsigned videoBitRateControlModes;
    // variables used for video bit rate control
    int bitRateHighLimit; // maximum instantaneous bit rate allowed
    unsigned oldLength;
    PTimeInterval oldTime;
    PTimeInterval newTime;
    // variables used for dynamic video quality control
    int targetFrameTimeMs; //targetFrameTimeMs = 1000 / videoSendFPS
    int frameBytes; // accumulate count of bytes per frame
    int sumFrameTimeMs, sumAdjFrameTimeMs, sumFrameBytes; // accumulate running average
    int videoQMax, videoQMin; // dynamic video quality min/max limits
    int videoQuality; // current video encode quality setting, 1..31
    PTimeInterval frameStartTime;
    PTimeInterval grabInterval;
    
    int frameNum, packetNum, oldPacketNum;
    int framesPerSec;

    PMutex  videoHandlerActive;    
};

#endif // NO_H323_VIDEO

#ifndef NO_H323_AUDIO_CODECS

///////////////////////////////////////////////////////////////////////////////
// The simplest codec is the G.711 PCM codec.

/**This class is a G711 ALaw codec.
 */
class H323_ALawCodec : public H323StreamedAudioCodec
{
  PCLASSINFO(H323_ALawCodec, H323StreamedAudioCodec)

  public:
  /**@name Construction */
  //@{
    /**Create a new G.711 codec for ALaw.
     */
    H323_ALawCodec(
      Direction direction,  ///< Direction in which this instance runs
      BOOL at56kbps,        ///< Encoding bit rate.
      unsigned frameSize    ///< Size of frame in bytes
    );
  //@}

    virtual int   Encode(short sample) const { return EncodeSample(sample); }
    virtual short Decode(int   sample) const { return DecodeSample(sample); }

    static int   EncodeSample(short sample);
    static short DecodeSample(int   sample);

  protected:
    BOOL sevenBit;
};


/**This class is a G711 uLaw codec.
 */
class H323_muLawCodec : public H323StreamedAudioCodec
{
  PCLASSINFO(H323_muLawCodec, H323StreamedAudioCodec)

  public:
  /**@name Construction */
  //@{
    /**Create a new G.711 codec for muLaw.
     */
    H323_muLawCodec(
      Direction direction,  ///< Direction in which this instance runs
      BOOL at56kbps,        ///< Encoding bit rate.
      unsigned frameSize    ///< Size of frame in bytes
    );
  //@}

    virtual int   Encode(short sample) const { return EncodeSample(sample); }
    virtual short Decode(int   sample) const { return DecodeSample(sample); }

    static int   EncodeSample(short sample);
    static short DecodeSample(int   sample);

  protected:
    BOOL sevenBit;
};

#endif // NO_H323_AUDIO_CODECS


#endif // __CODECS_H


/////////////////////////////////////////////////////////////////////////////
