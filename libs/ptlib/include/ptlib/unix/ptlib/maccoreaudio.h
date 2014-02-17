/*
 * maccoreaudio.h
 *
 * Copyright (c) 2004 Network for Educational Technology ETH
 *
 * Written by Hannes Friederich, Andreas Fenkart.
 * Based on work of Shawn Pai-Hsiang Hsiao
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
 * maccoreaudio.h
 *
 */

#pragma interface


// workaround to remove warnings, when including OSServiceProviders
#define __OPENTRANSPORTPROVIDERS__

#include <CoreAudio/CoreAudio.h>
#include <AudioToolbox/AudioToolbox.h>;
#include <AudioUnit/AudioUnit.h>

// needed by lists.h of pwlib, unfortunately also defined in previous
// includes from Apple
#undef nil


// static loading of plugins
#define P_FORCE_STATIC_PLUGIN

#include <ptlib.h>
#include <ptlib/sound.h>


#define CA_DUMMY_DEVICE_NAME "Null"
#define kAudioDeviceDummy kAudioDeviceUnknown



class CircularBuffer;

class PSoundChannelCoreAudio : public PSoundChannel
{
 public:
   enum State{
      init_,
      open_,
      setformat_,
      setbuffer_,
      running_,
      mute_,
      destroy_
   };

   static void Init();
   PSoundChannelCoreAudio();
   PSoundChannelCoreAudio(const PString &device,
                PSoundChannel::Directions dir,
                unsigned numChannels,
                unsigned sampleRate,
                unsigned bitsPerSample);
   ~PSoundChannelCoreAudio();
   
   virtual BOOL SetFormat(unsigned numChannels,
                unsigned sampleRate,
                unsigned bitsPerSample);
   virtual unsigned GetChannels() const;
   virtual unsigned GetSampleRate() const;
   virtual unsigned GetSampleSize() const;
   virtual BOOL SetBuffers(PINDEX size, PINDEX count);
   virtual BOOL GetBuffers(PINDEX & size, PINDEX & count);
   virtual BOOL SetVolume(unsigned volume);
   virtual BOOL GetVolume(unsigned & volume);
   
   /* Open functions */
   virtual BOOL Open(const PString & device,
             Directions dir,
             unsigned numChannels,
             unsigned sampleRate,
             unsigned bitsPerSample);
   /* gets never called, see sound.h:
    * baseChannel->PChannel::IsOpen();  */
   virtual BOOL IsOpen() const;
   /* gets never called, see sound.h:
    * baseChannel->PChannel::GetHandle();*/
   virtual int GetHandle() const;

   virtual BOOL Abort();
   PSoundChannel *CreateOpenedChannel(const PString & driverName,
       const PString & deviceName,
       const PSoundChannel::Directions,
       unsigned numChannels,
       unsigned sampleRate,
       unsigned bitsPerSample);

   static PString GetDefaultDevice(Directions dir);
   static PStringList GetDeviceNames(Directions dir);
   
   virtual BOOL Write(const void *buf, PINDEX len);
   virtual BOOL PlaySound(const PSound & sound, BOOL wait);
   virtual BOOL PlayFile(const PFilePath & file, BOOL wait);
   virtual BOOL HasPlayCompleted();
   virtual BOOL WaitForPlayCompletion();
   
   virtual BOOL Read(void *buf, PINDEX len);
   virtual BOOL RecordSound(PSound & sound);
   virtual BOOL RecordFile(const PFilePath & file);
   virtual BOOL StartRecording();
   virtual BOOL isRecordBufferFull();
   virtual BOOL AreAllRecordBuffersFull();
   virtual BOOL WaitForRecordBufferFull();
   virtual BOOL WaitForAllRecordBuffersFull();

 protected:
   /**
   * Common steps for all constructors 
   */
   void CommonConstruct(){ 
     os_handle = -1; //  == channel closed.
     // set to a non negative value so IsOpen() returns true
   }  
   
   OSStatus SetupInputUnit(AudioDeviceID in);
   OSStatus EnableIO();
   OSStatus SetupOutputUnit(AudioDeviceID in);
   OSStatus SetDeviceAsCurrent(AudioDeviceID in);

   /**
    * Based on the desired format, try to configure the AUHAL
    * Units to match this format as close as possible
    * e.g. 32bit float -> 16int, stereo -> mono */
   OSStatus MatchHALInputFormat();
   OSStatus MatchHALOutputFormat();


   OSStatus CallbackSetup();

   /**
    * Pull/Callback function to pass data to AudioConverter
    */
   static OSStatus ComplexBufferFillPlayback(OpaqueAudioConverter*, 
         UInt32*, 
         AudioBufferList*, 
         AudioStreamPacketDescription**, 
         void*);
   static OSStatus ComplexBufferFillRecord(OpaqueAudioConverter*, 
         UInt32*, 
         AudioBufferList*, 
         AudioStreamPacketDescription**, 
         void*);

   /**
    * Callback for the AudioUnit to pull/notify more data 
    */
   static OSStatus PlayRenderProc(
       void *inRefCon,
       AudioUnitRenderActionFlags *ioActionFlags,
       const struct AudioTimeStamp *TimeStamp,
       UInt32 inBusNumber,
       UInt32 inNumberFrames,
       struct AudioBufferList * ioData);


   static OSStatus RecordProc(
         void *inRefCon, 
         AudioUnitRenderActionFlags *ioActionFlags,
         const AudioTimeStamp *inTimeStamp,
         UInt32 inBusNumber,
         UInt32 inNumberFrames,
         AudioBufferList * ioData);

   static OSStatus VolumeChangePropertyListener(
         AudioDeviceID id, 
         UInt32 chan,
         Boolean isInput,
         AudioDevicePropertyID propID, 
         void* inUserData
   );


   /**
    * Recording needs a couple more buffers, which are setup by this 
    * function
    */
   OSStatus SetupAdditionalRecordBuffers();

   /**
    * Player or Recorder ? 
    */
   Directions direction;
   State state;

   static pthread_mutex_t& GetReadMuteMutex();
   static pthread_mutex_t& GetWriteMuteMutex();
   static BOOL& GetReadMute();
   static BOOL& GetWriteMute();

   /* These functions just return the right mutex/variable depending whehter
    * the channel is recorder/player
    */
   pthread_mutex_t& GetIsMuteMutex();
   BOOL & isMute();

   /** 
    * Devices
    */
   AudioUnit mAudioUnit;
   AudioDeviceID mDeviceID;
   AudioStreamBasicDescription hwASBD, pwlibASBD;

   /** Sample rate converter part of AudioToolbox Framework */
   AudioConverterRef converter;     
   CircularBuffer *mCircularBuffer;

   /** sample rate of the  AudioUnit as a mutliple of pwlib sample rate */
   Float64 rateTimes8kHz;

   /** number and size of internal buffers 
    * see also SetBuffers */
   PINDEX bufferSizeBytes;
   PINDEX bufferCount;



   /*
    * Buffer to hold data that are passed to the converter.
    * Separate means independant of the circular_buffer
    */
   char* converter_buffer;
   UInt32 converter_buffer_size;


   /* ==========================================================
    * Variables used only by the Recorder to circumvent
    * the inappropriaty control flow of the pull model
    */

   /** Buffers to capture raw data from the microphone */
   CircularBuffer* mInputCircularBuffer;
   AudioBufferList* mInputBufferList;
   UInt32 mRecordInputBufferSize;

#define MIN_INPUT_FILL 20


   /** buffer list to catch the output of the AudioConverter */
   AudioBufferList* mOutputBufferList;
   UInt32 mRecordOutputBufferSize;

};


