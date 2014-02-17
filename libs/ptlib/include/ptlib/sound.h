/*
 * sound.h
 *
 * Sound interface class.
 *
 * Portable Windows Library
 *
 * Copyright (c) 1993-1998 Equivalence Pty. Ltd.
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
 * The Original Code is Portable Windows Library.
 *
 * The Initial Developer of the Original Code is Equivalence Pty. Ltd.
 *
 * Portions are Copyright (C) 1993 Free Software Foundation, Inc.
 * All Rights Reserved.
 *
 * Contributor(s): ______________________________________.
 *
 * $Log: sound.h,v $
 * Revision 1.40  2007/04/13 07:03:19  rjongbloed
 * Added WAV file audio device "plug in".
 *
 * Revision 1.39  2007/02/01 03:16:15  csoutheren
 * Implement GetName for sound channels
 *
 * Revision 1.38  2006/10/19 12:05:32  rjongbloed
 * Added missing Close() function apss thru on PSoundChannel wrapper class.
 *
 * Revision 1.37  2006/06/21 03:28:41  csoutheren
 * Various cleanups thanks for Frederic Heem
 *
 * Revision 1.36  2005/11/25 03:43:47  csoutheren
 * Fixed function argument comments to be compatible with Doxygen
 *
 * Revision 1.35  2005/08/09 09:08:09  rjongbloed
 * Merged new video code from branch back to the trunk.
 *
 * Revision 1.34.2.2  2005/07/17 11:47:45  rjongbloed
 * Fixed backward compatibility
 *
 * Revision 1.34.2.1  2005/07/17 09:27:04  rjongbloed
 * Major revisions of the PWLib video subsystem including:
 *   removal of F suffix on colour formats for vertical flipping, all done with existing bool
 *   working through use of RGB and BGR formats so now consistent
 *   cleaning up the plug in system to use virtuals instead of pointers to functions.
 *   rewrite of SDL to be a plug in compatible video output device.
 *   extensive enhancement of video test program
 *
 * Revision 1.34  2005/07/13 13:02:35  csoutheren
 * Unified interface across Windows and Unix
 *
 * Revision 1.33  2005/07/04 11:35:03  shorne
 * Added ability to play sound to specfied device (Win32).
 *
 * Revision 1.32  2005/01/04 07:44:03  csoutheren
 * More changes to implement the new configuration methodology, and also to
 * attack the global static problem
 *
 * Revision 1.31  2004/11/01 23:16:59  ykiryanov
 * Added macro declaring sound plugin to be static for BeOS
 *
 * Revision 1.30  2003/12/28 02:03:18  csoutheren
 * Fixed problem with GetLastReadCount/GetLastWriteCount  on Windows sound devices
 *
 * Revision 1.29  2003/11/18 10:50:26  csoutheren
 * Changed name of Windows sound device
 *
 * Revision 1.28  2003/11/14 05:59:09  csoutheren
 * Added Read function, thanks to Derek Smithies
 *
 * Revision 1.27  2003/11/12 10:25:41  csoutheren
 * Changes to allow operation of static plugins under Windows
 *
 * Revision 1.26  2003/11/12 05:18:04  csoutheren
 * Added more backwards compatibility functions for PSoundChannel
 *
 * Revision 1.25  2003/11/12 04:33:32  csoutheren
 * Fixed problem with static linking of sound plugins
 * Fixed problem with Windows sound driver
 *
 * Revision 1.24  2003/11/12 03:29:51  csoutheren
 * Initial version of plugin code from Snark of GnomeMeeting with changes
 *    by Craig Southeren of Post Increment
 *
 * Revision 1.23.2.2  2003/10/13 02:42:39  dereksmithies
 * Add additional functions, so plugins work better.
 *
 * Revision 1.23.2.1  2003/10/07 03:02:28  csoutheren
 * Initial checkin of pwlib code to do plugins.
 * Modified from original code and concept provided by Snark of Gnomemeeting
 *
 * Revision 1.23  2003/09/17 05:41:59  csoutheren
 * Removed recursive includes
 *
 * Revision 1.22  2003/09/17 01:18:02  csoutheren
 * Removed recursive include file system and removed all references
 * to deprecated coooperative threading support
 *
 * Revision 1.21  2002/09/16 01:08:59  robertj
 * Added #define so can select if #pragma interface/implementation is used on
 *   platform basis (eg MacOS) rather than compiler, thanks Robert Monaghan.
 *
 * Revision 1.20  2002/02/08 09:58:44  robertj
 * Slight adjustment to API and documentation for volume functions.
 *
 * Revision 1.19  2002/02/07 20:57:21  dereks
 * add SetVolume and GetVolume methods to PSoundChannel
 *
 * Revision 1.18  2001/05/22 12:49:32  robertj
 * Did some seriously wierd rewrite of platform headers to eliminate the
 *   stupid GNU compiler warning about braces not matching.
 *
 * Revision 1.17  2001/02/07 03:33:43  craigs
 * Added functions to get sound channel parameters
 *
 * Revision 1.16  2000/03/04 10:15:32  robertj
 * Added simple play functions for sound files.
 *
 * Revision 1.15  1999/05/28 14:04:10  robertj
 * Added function to get default audio device.
 *
 * Revision 1.14  1999/03/09 02:59:51  robertj
 * Changed comments to doc++ compatible documentation.
 *
 * Revision 1.13  1999/02/22 10:15:14  robertj
 * Sound driver interface implementation to Linux OSS specification.
 *
 * Revision 1.12  1999/02/16 06:02:27  robertj
 * Major implementation to Linux OSS model
 *
 * Revision 1.11  1998/09/23 06:21:27  robertj
 * Added open source copyright license.
 *
 * Revision 1.10  1995/06/17 11:13:26  robertj
 * Documentation update.
 *
 * Revision 1.9  1995/03/14 12:42:40  robertj
 * Updated documentation to use HTML codes.
 *
 * Revision 1.8  1995/01/16  09:42:05  robertj
 * Documentation.
 *
 * Revision 1.7  1994/08/23  11:32:52  robertj
 * Oops
 *
 * Revision 1.6  1994/08/22  00:46:48  robertj
 * Added pragma fro GNU C++ compiler.
 *
 * Revision 1.5  1994/06/25  11:55:15  robertj
 * Unix version synchronisation.
 *
 * Revision 1.4  1994/01/03  04:42:23  robertj
 * Mass changes to common container classes and interactors etc etc etc.
 *
 * Revision 1.3  1993/09/29  03:06:30  robertj
 * Added unix compatibility to Beep()
 *
 * Revision 1.2  1993/07/14  12:49:16  robertj
 * Fixed RCS keywords.
 *
 */


#ifndef _PSOUND
#define _PSOUND

#ifdef P_USE_PRAGMA
#pragma interface
#endif

#include <ptlib/plugin.h>
#include <ptlib/pluginmgr.h>

/** A class representing a sound. A sound is a highly platform dependent
   entity that is abstracted for use here. Very little manipulation of the
   sounds are possible.

   The most common sound to use is the static function #Beep()# which
   emits the system standard "warning" or "attention" sound.
 */
class PSound : public PBYTEArray
{
  PCLASSINFO(PSound, PBYTEArray);

  public:
  /**@name Construction */
  //@{
    /**Create a new sound, using the parameters provided.
       It is expected that the "lowest common denominator" encoding, linear PCM,
       is used.

       All other values for the encoding are platform dependent.
     */
    PSound(
      unsigned numChannels = 1,    ///< Number of channels eg mono/stereo
      unsigned sampleRate = 8000,  ///< Samples per second
      unsigned bitsPerSample = 16, ///< Number of bits per sample
      PINDEX   bufferSize = 0,     ///< Size of data
      const BYTE * data = NULL     ///< Pointer to initial data
    );

    /**Create a new sound, reading from a platform dependent file.
     */
    PSound(
      const PFilePath & filename   ///< Sound file to load.
    );

    /**Set new data bytes for the sound.
     */
    PSound & operator=(
      const PBYTEArray & data  ///< New data for sound
    );
  //@}

  /**@name File functions */
  //@{
    /**Load a platform dependent sound file (eg .WAV file for Win32) into the
       object. Note the whole file must able to be loaded into memory.

       Also note that not all possible files are playable by this library. No
       format conversions between file and driver are performed.

       @return
       TRUE if the sound is loaded successfully.
     */
    BOOL Load(
      const PFilePath & filename   ///< Sound file to load.
    );

    /**Save a platform dependent sound file (eg .WAV file for Win32) from the
       object.

       @return
       TRUE if the sound is saved successfully.
     */
    BOOL Save(
      const PFilePath & filename   ///< Sound file to load.
    );
  //@}

  /**@name Access functions */
  //@{
    /// Play the sound on the default sound device.
    BOOL Play();

    /// Play the sound to the specified sound device.
    BOOL Play(const PString & device);

    /**Set the internal sound format to linear PCM at the specification in
       the parameters.
     */
    void SetFormat(
      unsigned numChannels,   ///< Number of channels eg mono/stereo
      unsigned sampleRate,    ///< Samples per second
      unsigned bitsPerSample  ///< Number of bits per sample
    );

    /**Get the current encoding. A value of 0 indicates linear PCM, any other
       value is platform dependent.
     */
    unsigned GetEncoding()   const { return encoding; }

    /// Get  the number of channels (mono/stereo) in the sound.
    unsigned GetChannels()   const { return numChannels; }

    /// Get the sample rate in samples per second.
    unsigned GetSampleRate() const { return sampleRate; }

    /// Get the sample size in bits per sample.
    unsigned GetSampleSize() const { return sampleSize; }

    /// Get the platform dependent error code from the last file load.
    DWORD    GetErrorCode()  const { return dwLastError; }

    /// Get the size of the platform dependent format info.
    PINDEX   GetFormatInfoSize()  const { return formatInfo.GetSize(); }

    /// Get pointer to the platform dependent format info.
    const void * GetFormatInfoData() const { return (const BYTE *)formatInfo; }
  //@}

  /**@name Miscellaneous functions */
  //@{
    /**Play a sound file to the default device. If the #wait#
       parameter is TRUE then the function does not return until the file has
       been played. If FALSE then the sound play is begun asynchronously and
       the function returns immediately.

       @return
       TRUE if the sound is playing or has played.
     */
    static BOOL PlayFile(
      const PFilePath & file, ///< Sound file to play.
      BOOL wait = TRUE        ///< Flag to play sound synchronously.
    );

    /// Play the "standard" warning beep for the platform.
    static void Beep();
  //@}

  protected:
    /// Format code
    unsigned   encoding;      
    /// Number of channels eg mono/stereo
    unsigned   numChannels;   
    /// Samples per second
    unsigned   sampleRate;    
    /// Number of bits per sample
    unsigned   sampleSize;    
    /// Last error code for Load()/Save() functions
    DWORD      dwLastError;   
    /// Full info on the format (platform dependent)
    PBYTEArray formatInfo;    
};


/**
   This class is both an abstract class for a generalised sound channel, 
   and an implementation of PSoundChannel for old code that is not plugin-aware.
   When instantiated, it selects the first plugin of the base class "PSoundChannel"

   As an abstract class, this represents a sound schannel. Drivers for real, platform
   dependent sound hardware will be ancestors of this class and can be found
   in the plugins section of PWLib.

   A sound driver is either playing or recording. If simultaneous playing and
   recording is desired, two instances of PSoundChannel must be created.

   The sound is buffered and the size and number of buffers should be set
   before playing/recording. Each call to Write() will use one buffer, so care
   needs to be taken not to use a large number of small writes but tailor the
   buffers to the size of each write you make.

   Similarly for reading, an entire buffer must be read before any of it is
   available to a Read() call. Note that once a buffer is filled you can read
   it a byte at a time if desired, but as soon as all the data in the buffer
   is used returned, the next read will wait until the entire next buffer is
   read from the hardware. So again, tailor the number and size of buffers to
   the application. To avoid being blocked until the buffer fills, you can use
   the StartRecording() function to initiate the buffer filling, and the
   IsRecordingBufferFull() function to determine when the Read() function will
   no longer block.

   Note that this sound channel is implicitly a linear PCM channel. No data
   conversion is performed on data to/from the channel.
 */
class PSoundChannel : public PChannel
{
  PCLASSINFO(PSoundChannel, PChannel);

  public:
  /**@name Construction */
  //@{
    enum Directions {
      Recorder,
      Player
    };

    /// Create a sound channel.
    PSoundChannel();

    /** Create a sound channel.
        Create a reference to the sound drivers for the platform.
      */
    PSoundChannel(
      const PString & device,       ///< Name of sound driver/device
      Directions dir,               ///< Sound I/O direction
      unsigned numChannels = 1,     ///< Number of channels eg mono/stereo
      unsigned sampleRate = 8000,   ///< Samples per second
      unsigned bitsPerSample = 16   ///< Number of bits per sample
    );
    // 

    virtual ~PSoundChannel();
    // Destroy and close the sound driver
  //@}

  /**@name Open functions */
  //@{
    /**Get the list of available sound drivers (plug-ins)
     */
    static PStringList GetDriverNames(
      PPluginManager * pluginMgr = NULL   ///< Plug in manager, use default if NULL
    );

    /**Get sound devices that correspond to the specified driver name.
       If driverName is an empty string or the value "*" then GetAllDeviceNames()
       is used.
     */
    static PStringList GetDriversDeviceNames(
      const PString & driverName,         ///< Name of driver
      Directions direction,               ///< Direction for device (record or play)
      PPluginManager * pluginMgr = NULL   ///< Plug in manager, use default if NULL
    );

    // For backward compatibility
    static inline PStringList GetDeviceNames(
      const PString & driverName,
      Directions direction,
      PPluginManager * pluginMgr = NULL
    ) { return GetDriversDeviceNames(driverName, direction, pluginMgr); }

    /**Create the sound channel that corresponds to the specified driver name.
     */
    static PSoundChannel * CreateChannel (
      const PString & driverName,         ///< Name of driver
      PPluginManager * pluginMgr = NULL   ///< Plug in manager, use default if NULL
    );

    /* Create the matching sound channel that corresponds to the device name.
       So, for "fake" return a device that will generate fake video.
       For "Phillips 680 webcam" (eg) will return appropriate grabber.
       Note that Phillips will return the appropriate grabber also.

       This is typically used with the return values from GetDeviceNames().
     */
    static PSoundChannel * CreateChannelByName(
      const PString & deviceName,         ///< Name of device
      Directions direction,               ///< Direction for device (record or play)
      PPluginManager * pluginMgr = NULL   ///< Plug in manager, use default if NULL
    );

    /**Create an opened sound channel that corresponds to the specified names.
       If the driverName parameter is an empty string or "*" then CreateChannelByName
       is used with the deviceName parameter which is assumed to be a value returned
       from GetAllDeviceNames().
     */
    static PSoundChannel * CreateOpenedChannel(
      const PString & driverName,         ///< Name of driver
      const PString & deviceName,         ///< Name of device
      Directions direction,               ///< Direction for device (record or play)
      unsigned numChannels = 1,           ///< Number of channels 1=mon, 2=stereo
      unsigned sampleRate = 8000,         ///< Sample rate
      unsigned bitsPerSample = 16,        ///< Bits per sample
      PPluginManager * pluginMgr = NULL   ///< Plug in manager, use default if NULL
    );

    /**Get the name for the default sound devices/driver that is on this
       platform. Note that a named device may not necessarily do both
       playing and recording so the arrays returned with the #dir#
       parameter in each value is not necessarily the same.

       This will return a list of uniqie device names across all of the available
       drivers. If two drivers have identical names for devices, then the string
       returned will be of the form driver+'\t'+device.

       @return
       A platform dependent string for the sound player/recorder.
     */
    static PString GetDefaultDevice(
      Directions dir    // Sound I/O direction
    );

    /**Get the list of all devices name for the default sound devices/driver that is on this
       platform. Note that a named device may not necessarily do both
       playing and recording so the arrays returned with the #dir#
       parameter in each value is not necessarily the same.

       @return
       Platform dependent strings for the sound player/recorder.
     */
    static PStringList GetDeviceNames(
      Directions direction,               ///< Direction for device (record or play)
      PPluginManager * pluginMgr = NULL   ///< Plug in manager, use default if NULL
    );

    /**Open the specified device for playing or recording. The device name is
       platform specific and is as returned in the GetDevices() function.

       @return
       TRUE if the sound device is valid for playing/recording.
     */
    virtual BOOL Open(
      const PString & device,       ///< Name of sound driver/device
      Directions dir,               ///< Sound I/O direction
      unsigned numChannels = 1,     ///< Number of channels eg mono/stereo
      unsigned sampleRate = 8000,   ///< Samples per second
      unsigned bitsPerSample = 16   ///< Number of bits per sample
    );

    /**Test if this instance of PSoundChannel is open.

       @return
       TRUE if this instance is open.
     */
    virtual BOOL IsOpen() const;

    /** Close the channel, shutting down the link to the data source.

       @return TRUE if the channel successfully closed.
     */
    virtual BOOL Close();

    /**Get the OS specific handle for the PSoundChannel.

       @return
       integer value of the handle.
     */
    virtual int GetHandle() const;

    /// Get the name of the open channel
    virtual PString GetName() const;

    /**Abort the background playing/recording of the sound channel.

       @return
       TRUE if the sound has successfully been aborted.
     */
    virtual BOOL Abort();
  //@}

  /**@name Channel set up functions */
  //@{
    /**Set the format for play/record. Note that linear PCM data is the only
       one supported at this time.

       Note that if the PlayFile() function is used, this may be overridden
       by information in the file being played.

       @return
       TRUE if the format is valid.
     */
    virtual BOOL SetFormat(
      unsigned numChannels = 1,     ///< Number of channels eg mono/stereo
      unsigned sampleRate = 8000,   ///< Samples per second
      unsigned bitsPerSample = 16   ///< Number of bits per sample
    );

    /// Get  the number of channels (mono/stereo) in the sound.
    virtual unsigned GetChannels() const;

    /// Get the sample rate in samples per second.
    virtual unsigned GetSampleRate() const;

    /// Get the sample size in bits per sample.
    virtual unsigned GetSampleSize() const;

    /**Set the internal buffers for the sound channel I/O.

       Note that with Linux OSS, the size is always rounded up to the nearest
       power of two, so 20000 => 32768.

       @return
       TRUE if the sound device is valid for playing/recording.
     */
    virtual BOOL SetBuffers(
      PINDEX size,      ///< Size of each buffer
      PINDEX count = 2  ///< Number of buffers
    );

    /**Get the internal buffers for the sound channel I/O. 

       @return
       TRUE if the buffer size were obtained.
     */
    virtual BOOL GetBuffers(
      PINDEX & size,    // Size of each buffer
      PINDEX & count    // Number of buffers
    );

    enum {
      MaxVolume = 100
    };

    /**Set the volume of the play/read process.
       The volume range is 0 == quiet.  100 == LOUD.
        
       @return
       TRUE if there were no errors.
    */
    virtual BOOL SetVolume(
      unsigned volume   ///< New volume level
    );

    /**Get the volume of the play/read process.
       The volume range is 0 == quiet.  100 == LOUD.

       @return
       TRUE if there were no errors.
    */
    virtual BOOL GetVolume(
      unsigned & volume   ///< Variable to receive volume level.
    );
  //@}

  /**@name Play functions */
  //@{

    /** Low level write (or play) to the channel. This function will block until the
       requested number of characters are written or the write timeout is
       reached. The GetLastWriteCount() function returns the actual number
       of bytes written.
                                                                                                                                            
       The GetErrorCode() function should be consulted after Write() returns
       FALSE to determine what caused the failure.
 
       @return
       TRUE if at least len bytes were written to the channel.
     */
    virtual BOOL Write(const void * buf, PINDEX len);

    virtual PINDEX GetLastWriteCount() const;

    /**Play a sound to the open device. If the #wait# parameter is
       TRUE then the function does not return until the file has been played.
       If FALSE then the sound play is begun asynchronously and the function
       returns immediately.

       Note if the driver is closed of the object destroyed then the sound
       play is aborted.

       Also note that not all possible sounds and sound files are playable by
       this library. No format conversions between sound object and driver are
       performed.

       @return
       TRUE if the sound is playing or has played.
     */

    virtual BOOL PlaySound(
      const PSound & sound,   ///< Sound to play.
      BOOL wait = TRUE        ///< Flag to play sound synchronously.
    );
    /**Play a sound file to the open device. If the #wait#
       parameter is TRUE then the function does not return until the file has
       been played. If FALSE then the sound play is begun asynchronously and
       the function returns immediately.

       Note if the driver is closed of the object destroyed then the sound
       play is aborted.

       Also note that not all possible sounds and sound files are playable by
       this library. No format conversions between sound object and driver are
       performed.

       @return
       TRUE if the sound is playing or has played.
     */
    virtual BOOL PlayFile(
      const PFilePath & file, ///< Sound file to play.
      BOOL wait = TRUE        ///< Flag to play sound synchronously.
    );

    /**Indicate if the sound play begun with PlayBuffer() or PlayFile() has
       completed.

       @return
       TRUE if the sound has completed playing.
     */
    virtual BOOL HasPlayCompleted();

    /**Block the thread until the sound play begun with PlayBuffer() or
       PlayFile() has completed.

       @return
       TRUE if the sound has successfully completed playing.
     */
    virtual BOOL WaitForPlayCompletion();

  //@}

  /**@name Record functions */
  //@{
    /** Low level read from the channel. This function may block until the
       requested number of characters were read or the read timeout was
       reached. The GetLastReadCount() function returns the actual number
       of bytes read.

       The GetErrorCode() function should be consulted after Read() returns
       FALSE to determine what caused the failure.

       @return
       TRUE indicates that at least one character was read from the channel.
       FALSE means no bytes were read due to timeout or some other I/O error.
     */
    virtual BOOL Read(
      void * buf,   ///< Pointer to a block of memory to receive the read bytes.
      PINDEX len    ///< Maximum number of bytes to read into the buffer.
    );

    PINDEX GetLastReadCount() const;

    /**Record into the sound object all of the buffer's of sound data. Use the
       SetBuffers() function to determine how long the recording will be made.

       For the Win32 platform, the most efficient way to record a PSound is to
       use the SetBuffers() function to set a single buffer of the desired
       size and then do the recording. For Linux OSS this can cause problems
       as the buffers are rounded up to a power of two, so to gain more
       accuracy you need a number of smaller buffers.

       Note that this function will block until all of the data is buffered.
       If you wish to do this asynchronously, use StartRecording() and
       AreAllrecordBuffersFull() to determine when you can call RecordSound()
       without blocking.

       @return
       TRUE if the sound has been recorded.
     */
    virtual BOOL RecordSound(
      PSound & sound ///< Sound recorded
    );

    /**Record into the platform dependent sound file all of the buffer's of
       sound data. Use the SetBuffers() function to determine how long the
       recording will be made.

       Note that this function will block until all of the data is buffered.
       If you wish to do this asynchronously, use StartRecording() and
       AreAllrecordBuffersFull() to determine when you can call RecordSound()
       without blocking.

       @return
       TRUE if the sound has been recorded.
     */
    virtual BOOL RecordFile(
      const PFilePath & file ///< Sound file recorded
    );

    /**Start filling record buffers. The first call to Read() will also
       initiate the recording.

       @return
       TRUE if the sound driver has successfully started recording.
     */
    virtual BOOL StartRecording();

    /**Determine if a record buffer has been filled, so that the next Read()
       call will not block. Provided that the amount of data read is less than
       the buffer size.

       @return
       TRUE if the sound driver has filled a buffer.
     */
    virtual BOOL IsRecordBufferFull();

    /**Determine if all of the record buffer allocated has been filled. There
       is an implicit Abort() of the recording if this occurs and recording is
       stopped. The channel may need to be closed and opened again to start
       a new recording.

       @return
       TRUE if the sound driver has filled a buffer.
     */
    virtual BOOL AreAllRecordBuffersFull();

    /**Block the thread until a record buffer has been filled, so that the
       next Read() call will not block. Provided that the amount of data read
       is less than the buffer size.

       @return
       TRUE if the sound driver has filled a buffer.
     */
    virtual BOOL WaitForRecordBufferFull();

    /**Block the thread until all of the record buffer allocated has been
       filled. There is an implicit Abort() of the recording if this occurs
       and recording is stopped. The channel may need to be closed and opened
       again to start a new recording.

       @return
       TRUE if the sound driver has filled a buffer.
     */
    virtual BOOL WaitForAllRecordBuffersFull();
  //@}

  protected:
    PSoundChannel * baseChannel;
};


/////////////////////////////////////////////////////////////////////////

// define the sound plugin service descriptor

template <class className> class PSoundChannelPluginServiceDescriptor : public PDevicePluginServiceDescriptor
{
  public:
    virtual PObject *   CreateInstance(int /*userData*/) const { return new className; }
    virtual PStringList GetDeviceNames(int userData) const { return className::GetDeviceNames((PSoundChannel::Directions)userData); }
};

#define PCREATE_SOUND_PLUGIN(name, className) \
  static PSoundChannelPluginServiceDescriptor<className> className##_descriptor; \
  PCREATE_PLUGIN(name, PSoundChannel, &className##_descriptor)

#endif

// End Of File ///////////////////////////////////////////////////////////////
