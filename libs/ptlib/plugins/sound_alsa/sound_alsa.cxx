/*
 * sound_alsa.cxx
 *
 * Sound driver implementation.
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
 * The Initial Developer of the Original ALSA Code is 
 * Damien Sandras <dsandras@seconix.com>
 *
 * Portions are Copyright (C) 1993 Free Software Foundation, Inc.
 * All Rights Reserved.
 *
 * Contributor(s): /
 *
 * $Log: sound_alsa.cxx,v $
 * Revision 1.33  2007/02/04 21:20:46  dsandras
 * Various improvements to fix sound issues with some soundcards.
 *
 * Revision 1.32  2006/12/30 22:19:55  dsandras
 * Fixed possible ALSA crash because of the use of a NULL pointer if
 * a detected device can not be opened for some reason. (Ekiga report #328753).
 *
 * Revision 1.31  2006/10/06 11:45:42  dsandras
 * Increases storedPeriods to 3 as suggested by Stelian Pop (Ekiga bug #358338)
 * to fix bad output sound on some soundcards.
 *
 * Revision 1.30  2006/05/17 18:40:55  dsandras
 * Do not explicitely free the cache as it could create some weird race conditions.
 *
 * Revision 1.29  2006/03/09 20:28:35  dsandras
 * Added fallback mechanism to find an existing mixer for playing/recording.
 *
 * Revision 1.28  2006/02/06 22:16:38  dsandras
 * Fixed leak.
 *
 * Revision 1.27  2005/11/30 12:47:38  csoutheren
 * Removed tabs, reformatted some code, and changed tags for Doxygen
 *
 * Revision 1.26  2004/12/05 20:45:06  dsandras
 * Added back some of the former code to fix probems with PWavFile.
 *
 * Revision 1.25  2004/12/02 01:19:48  dereksmithies
 * Ensure ALSA is initialised to a default of 2 stored buffers.
 * Improve reporting of errors.
 * Thanks to various people who have been patiently reporting errors.
 *
 * Revision 1.24  2004/11/07 20:23:00  dsandras
 * Removed erroneous update of lastReadCount in previous commit.
 *
 * Revision 1.23  2004/11/07 20:01:32  dsandras
 * Make sure lastWriteCount is updated.
 *
 * Revision 1.22  2004/11/06 16:31:12  dsandras
 * Removed dictionnary copy.
 *
 * Revision 1.21  2004/10/18 11:43:39  dsandras
 * Use Capture instead of Mic when changing the volume. Use the correct mixer when using the Default device.
 *
 * Revision 1.20  2004/10/14 19:30:16  dsandras
 * Removed DMIX and DSNOOP plugins and added support for DEFAULT as it is the correcti way to do things.
 *
 * Revision 1.19  2004/08/30 21:09:41  dsandras
 * Added DSNOOP plugin support.
 *
 * Revision 1.18  2004/05/14 10:15:26  dominance
 * Fixes direct opening of sound output devices. The list of devices does no longer return NULL in that case. 
 * Patch provided by Julien Puydt <julien.puydt@laposte.net>.
 *
 * Revision 1.17  2004/04/03 10:33:45  dsandras
 * Use PStringToOrdinal to store the detected devices, that fixes problems if there is a discontinuity in the 
 * succession of soundcard ID's. For example the user has card ID 1 and 3, but not 2.
 *
 * Revision 1.16  2004/03/13 12:36:14  dsandras
 * Added support for DMIX plugin output.
 *
 * Revision 1.15  2004/03/04 13:36:13  dsandras
 * Added check so that ALSA doesn't assert on broken installations.
 *
 * Revision 1.14  2004/02/12 09:07:57  csoutheren
 * Fixed typo in ALSA driver, thanks to Julien Puydt
 *
 * Revision 1.13  2004/01/04 20:59:30  dsandras
 * Use set_rate_near instead of set_rate.
 *
 * Revision 1.12  2003/12/28 15:10:35  dsandras
 * Updated to the new PCM API.
 *
 * Revision 1.11  2003/12/18 11:16:41  dominance
 * Removed the ALSA Abort completely upon Damien's request ;)
 *
 * Revision 1.10  2003/12/18 10:38:55  dominance
 * Removed ALSA Abort as it segfaults in various circumstances.
 * Fix proposed by Damien Sandras <dsandras@seconix.com>.
 *
 * Revision 1.9  2003/12/09 22:47:10  dsandras
 * Use less aggressive Abort.
 *
 * Revision 1.8  2003/12/03 21:48:21  dsandras
 * Better handling of buffer sizes. Removed unuseful code.
 *
 * Revision 1.7  2003/11/25 20:13:48  dsandras
 * Added #pragma.
 *
 * Revision 1.6  2003/11/25 09:58:01  dsandras
 * Removed Abort call from PlaySound ().
 *
 * Revision 1.5  2003/11/25 09:52:07  dsandras
 * Modified WaitForPlayCompletion so that it uses snd_pcm_drain instead of active waiting.
 *
 * Revision 1.4  2003/11/23 22:09:57  dsandras
 * Removed unuseful stuff and added implementation for functions permitting to play a file or a PSound.
 *
 * Revision 1.3  2003/11/14 05:28:47  csoutheren
 * Updated for new plugin code thanks to Damien and Snark
 *
 */

#pragma implementation "sound_alsa.h"

#include "sound_alsa.h"


PCREATE_SOUND_PLUGIN(ALSA, PSoundChannelALSA)


static PStringToOrdinal playback_devices;
static PStringToOrdinal capture_devices;
PMutex dictionaryMutex;

///////////////////////////////////////////////////////////////////////////////

PSoundChannelALSA::PSoundChannelALSA()
{
  PSoundChannelALSA::Construct();
}


PSoundChannelALSA::PSoundChannelALSA (const PString &device,
                                          Directions dir,
                                            unsigned numChannels,
                                            unsigned sampleRate,
                                            unsigned bitsPerSample)
{
  Construct();
  Open (device, dir, numChannels, sampleRate, bitsPerSample);
}


void PSoundChannelALSA::Construct()
{
  enum _snd_pcm_format val = SND_PCM_FORMAT_UNKNOWN;

#if PBYTE_ORDER == PLITTLE_ENDIAN
  val = (mBitsPerSample == 16) ? SND_PCM_FORMAT_S16_LE : SND_PCM_FORMAT_U8;
#else
  val = (mBitsPerSample == 16) ? SND_PCM_FORMAT_S16_BE : SND_PCM_FORMAT_U8;
#endif

  frameBytes = (mNumChannels * (snd_pcm_format_width (val) / 8));
  storedPeriods = 4;
  storedSize = frameBytes * 3;

  card_nr = 0;
  os_handle = NULL;
}


PSoundChannelALSA::~PSoundChannelALSA()
{
  Close();
}


void PSoundChannelALSA::UpdateDictionary (Directions dir)
{
  int card = -1, dev = -1;
  
  snd_ctl_t *handle = NULL;
  snd_ctl_card_info_t *info = NULL;
  snd_pcm_info_t *pcminfo = NULL;
  snd_pcm_stream_t stream;

  char *name = NULL;
  char card_id [32];

  PWaitAndSignal m(dictionaryMutex);

  if (dir == Recorder) {

    stream = SND_PCM_STREAM_CAPTURE;
    capture_devices = PStringToOrdinal ();
  }
  else {

    stream = SND_PCM_STREAM_PLAYBACK;
    playback_devices = PStringToOrdinal ();
  }

  snd_ctl_card_info_alloca (&info);
  snd_pcm_info_alloca (&pcminfo);

  /* No sound card found */
  if (snd_card_next (&card) < 0 || card < 0) {

    return;
  }

  while (card >= 0) {

    snprintf (card_id, 32, "hw:%d", card);
    
    if (snd_ctl_open (&handle, card_id, 0) == 0) {

      snd_ctl_card_info (handle, info);

      while (1) {

        snd_ctl_pcm_next_device (handle, &dev);

        if (dev < 0)
          break;

        snd_pcm_info_set_device (pcminfo, dev);
        snd_pcm_info_set_subdevice (pcminfo, 0);
        snd_pcm_info_set_stream (pcminfo, stream);

        if (snd_ctl_pcm_info (handle, pcminfo) >= 0) {

          snd_card_get_name (card, &name);
          if (dir == Recorder) 
            capture_devices.SetAt (name, card);
          else 
            playback_devices.SetAt (name, card);
          free (name);
        }
      }
      snd_ctl_close(handle);
    }

    snd_card_next (&card);
  }
}


PStringArray PSoundChannelALSA::GetDeviceNames (Directions dir)
{
  PStringArray devices;
 
  UpdateDictionary (dir);
  
  if (dir == Recorder) {
    
    if (capture_devices.GetSize () > 0)
      devices += "Default";
    for (PINDEX j = 0 ; j < capture_devices.GetSize () ; j++) 
      devices += capture_devices.GetKeyAt (j);
  }
  else {

    if (playback_devices.GetSize () > 0)
      devices += "Default";
    for (PINDEX j = 0 ; j < playback_devices.GetSize () ; j++) 
      devices += playback_devices.GetKeyAt (j);
  }
  
  return devices;
}


PString PSoundChannelALSA::GetDefaultDevice(Directions dir)
{
  PStringArray devicenames;
  devicenames = PSoundChannelALSA::GetDeviceNames (dir);

  return devicenames[0];
}


BOOL PSoundChannelALSA::Open (const PString & _device,
                              Directions _dir,
                              unsigned _numChannels,
                              unsigned _sampleRate,
                              unsigned _bitsPerSample)
{
  PString real_device_name;
  POrdinalKey *i = NULL;
  snd_pcm_stream_t stream;

  Close();

  direction = _dir;
  mNumChannels = _numChannels;
  mSampleRate = _sampleRate;
  mBitsPerSample = _bitsPerSample;
  isInitialised = FALSE;

  os_handle = NULL;

  PWaitAndSignal m(device_mutex);

  if (_dir == Recorder)
    stream = SND_PCM_STREAM_CAPTURE;
  else
    stream = SND_PCM_STREAM_PLAYBACK;

  /* Open in NONBLOCK mode */
  if (_device == "Default") {

    real_device_name = "default";
    card_nr = -2;
  }
  else {

  if ((_dir == Recorder && capture_devices.IsEmpty ())
      || (_dir == Player && playback_devices.IsEmpty ()))
    UpdateDictionary (_dir);

    i = (_dir == Recorder) ? capture_devices.GetAt (_device) : playback_devices.GetAt (_device);

    if (i) {

      real_device_name = "plughw:" + PString (*i);
      card_nr = *i;
    }
    else {

      PTRACE (1, "ALSA\tDevice not found");
      return FALSE;
    }
  }
    
  if (snd_pcm_open (&os_handle, real_device_name, stream, SND_PCM_NONBLOCK) < 0) {

    PTRACE (1, "ALSA\tOpen Failed");
    return FALSE;
  }
  else 
    snd_pcm_nonblock (os_handle, 0);
   
  /* save internal parameters */
  device = real_device_name;

  Setup ();
  PTRACE (1, "ALSA\tDevice " << real_device_name << " Opened");

  return TRUE;
}


BOOL PSoundChannelALSA::Setup()
{
  snd_pcm_hw_params_t *hw_params = NULL;
  PStringStream msg;

  int err = 0;
  enum _snd_pcm_format val = SND_PCM_FORMAT_UNKNOWN;
  BOOL no_error = TRUE;


  if (os_handle == NULL) {

    PTRACE(6, "ALSA\tSkipping setup of " << device << " as not open");
    return FALSE;
  }

  if (isInitialised) {

    PTRACE(6, "ALSA\tSkipping setup of " << device << " as instance already initialised");
    return TRUE;
  }

#if PBYTE_ORDER == PLITTLE_ENDIAN
  val = (mBitsPerSample == 16) ? SND_PCM_FORMAT_S16_LE : SND_PCM_FORMAT_U8;
#else
  val = (mBitsPerSample == 16) ? SND_PCM_FORMAT_S16_BE : SND_PCM_FORMAT_U8;
#endif

  frameBytes = (mNumChannels * (snd_pcm_format_width (val) / 8));

  snd_pcm_hw_params_alloca (&hw_params);

  if ((err = snd_pcm_hw_params_any (os_handle, hw_params)) < 0) {
    msg << "Cannot initialize hardware parameter structure " << snd_strerror (err);
    PTRACE (1, "ALSA\t" << msg);
    cerr << msg << endl;
    no_error = FALSE;
  }


  if ((err = snd_pcm_hw_params_set_access (os_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {
    msg << "Cannot set access type " <<  snd_strerror (err);
    PTRACE (1, "ALSA\t" << msg);
    cerr << msg << endl;
    no_error = FALSE;
  }


  if ((err = snd_pcm_hw_params_set_format (os_handle, hw_params, val)) < 0) {
    msg << "Cannot set sample format " << snd_strerror (err);
    PTRACE (1, "ALSA\t" << msg);
    no_error = FALSE;
  }


  if ((err = snd_pcm_hw_params_set_channels (os_handle, hw_params, mNumChannels)) < 0) {
    msg << "Cannot set channel count " << snd_strerror (err);
    PTRACE (1, "ALSA\t" << msg);
    cerr << msg << endl;
    no_error = FALSE;
  }

  if ((err = snd_pcm_hw_params_set_rate_near (os_handle, hw_params, &mSampleRate, NULL)) < 0) {
    msg << "Cannot set sample rate " << snd_strerror (err);
    PTRACE (1, "ALSA\t" << msg);
    no_error = FALSE;
  }
  
  snd_pcm_uframes_t period_size = storedSize / (frameBytes ? frameBytes : 2);
  if ((err = snd_pcm_hw_params_set_period_size_near (os_handle, hw_params, &period_size, 0)) < 0) { 
    msg << "Cannot set period size " << snd_strerror (err);
    PTRACE (1, "ALSA\t" << msg); 
    cerr << msg << endl;
  }
  
  if ((err = (int) snd_pcm_hw_params_set_periods_near (os_handle, hw_params, (unsigned int *) &storedPeriods, 0)) < 0) {
    msg << "Cannot set periods to " << snd_strerror (err);
    PTRACE (1, "ALSA\t" << msg); 
    cerr << msg << endl;
  }

 /*****The buffer time is TWICE  the period time.
       bufferTime is the time to play the stored data.
       periodTime is the duration played in micro seconds.
       For GSM, period time is 20 milliseconds.
       For most other codecs, period time is 30 milliseconds.
  ******/
  unsigned int period_time = period_size * 1000 * 1000 / (2 * mSampleRate);
  unsigned int buffer_time = period_time * storedPeriods;
  PTRACE(3, "Alsa\tBuffer time is " << buffer_time);
  PTRACE(3, "Alsa\tPeriod time is " << period_time);


  // Ignore errors here 
  if ((err = snd_pcm_hw_params_set_buffer_time_near (os_handle, hw_params, &buffer_time, NULL)) < 0) {
    msg << "Cannot set buffer_time to  " << (buffer_time / 1000) << " ms " << snd_strerror (err);
    PTRACE (1, "ALSA\t" << msg);
    cerr << msg << endl;
  }

  if ((err = snd_pcm_hw_params_set_period_time_near (os_handle, hw_params, &period_time, 0)) < 0) {
    msg << "Cannot set period_time to " << (period_time / 1000) << " ms   " << snd_strerror (err);
    PTRACE (1, "ALSA\t" << msg);
    cerr << msg << endl;
  }
        
  if ((err = snd_pcm_hw_params (os_handle, hw_params)) < 0) {
    msg << "Cannot set parameters " <<      snd_strerror (err);
    PTRACE (1, "ALSA\t" << msg);
    cerr << msg << endl;
    no_error = FALSE;
  }
  
  isInitialised = TRUE;

  return no_error;
}


BOOL PSoundChannelALSA::Close()
{
  PStringStream msg;
  PWaitAndSignal m(device_mutex);

  /* if the channel isn't open, do nothing */
  if (!os_handle)
    return FALSE;

  snd_pcm_close (os_handle);
  os_handle = NULL;
  isInitialised = FALSE;
  
  return TRUE;
}


BOOL PSoundChannelALSA::Write (const void *buf, PINDEX len)
{
  long r = 0;
  char *buf2 = (char *) buf;
  int pos = 0, max_try = 0;
 
  lastWriteCount = 0;
  PWaitAndSignal m(device_mutex);

  if (!isInitialised && !Setup() || !len || !os_handle)
    return FALSE;

  do {

      
    /* the number of frames to read is the buffer length 
       divided by the size of one frame */
    r = snd_pcm_writei (os_handle, (char *) &buf2 [pos], len / frameBytes);

    if (r > 0) {
      pos += r * frameBytes;
      len -= r * frameBytes;
      lastWriteCount += r * frameBytes;
    }
    else {
      if (r == -EPIPE) {    /* under-run */
        r = snd_pcm_prepare (os_handle);
        if (r < 0)
          PTRACE (1, "ALSA\tCould not prepare device: " << snd_strerror (r));
      } else if (r == -ESTRPIPE) {

        while ((r = snd_pcm_resume (os_handle)) == -EAGAIN)
          sleep(1);       /* wait until the suspend flag is released */
      
        if (r < 0) 
          snd_pcm_prepare (os_handle);
      }

      PTRACE (1, "ALSA\tCould not write " << max_try << " " << len << " " << snd_strerror(r));
      max_try++;
    }  
  } while (len > 0 && max_try < 5);

  return TRUE;
}


BOOL PSoundChannelALSA::Read (void * buf, PINDEX len)
{
  long r = 0;

  char *buf2 = (char *) buf;
  int pos = 0, max_try = 0;

  lastReadCount = 0;
  PWaitAndSignal m(device_mutex);

  if (!isInitialised && !Setup() || !len || !os_handle)
    return FALSE;

  memset ((char *) buf, 0, len);
  
  do {

    /* the number of frames to read is the buffer length 
       divided by the size of one frame */
    r = snd_pcm_readi (os_handle, (char *) &buf2 [pos], len / frameBytes);
    if (r > 0) {
      pos += r * frameBytes;
      len -= r * frameBytes;
      lastReadCount += r * frameBytes;
    }
    else {
      if (r == -EPIPE) {    /* under-run */
        snd_pcm_prepare (os_handle);
      } 
      else if (r == -ESTRPIPE) {
        while ((r = snd_pcm_resume (os_handle)) == -EAGAIN)
          sleep(1);       /* wait until the suspend flag is released */
        if (r < 0) 
          snd_pcm_prepare (os_handle);
      }

      PTRACE (1, "ALSA\tCould not read");
      max_try++;
    }
  } while (len > 0 && max_try < 5);

 
  if (len != 0) {

    memset ((char *) &buf2 [pos], 0, len);
    lastReadCount += len;

    PTRACE (1, "ALSA\tRead Error, filling with zeros");
  }
  
  
  return TRUE;
}


BOOL PSoundChannelALSA::SetFormat (unsigned numChannels,
                                   unsigned sampleRate,
                                   unsigned bitsPerSample)
{
  if (!os_handle)
    return SetErrorValues(NotOpen, EBADF);

  /* check parameters */
  PAssert((bitsPerSample == 8) || (bitsPerSample == 16), PInvalidParameter);
  PAssert(numChannels >= 1 && numChannels <= 2, PInvalidParameter);

  mNumChannels   = numChannels;
  mSampleRate    = sampleRate;
  mBitsPerSample = bitsPerSample;
 
  /* mark this channel as uninitialised */
  isInitialised = FALSE;

  return TRUE;
}


unsigned PSoundChannelALSA::GetChannels()   const
{
  return mNumChannels;
}


unsigned PSoundChannelALSA::GetSampleRate() const
{
  return mSampleRate;
}


unsigned PSoundChannelALSA::GetSampleSize() const
{
  return mBitsPerSample;
}


BOOL PSoundChannelALSA::SetBuffers (PINDEX size, PINDEX count)
{
  storedPeriods = count;
  storedSize = size;

  isInitialised = FALSE;

  return TRUE;
}


BOOL PSoundChannelALSA::GetBuffers(PINDEX & size, PINDEX & count)
{
  size = storedSize;
  count = storedPeriods;
  
  return FALSE;
}


BOOL PSoundChannelALSA::PlaySound(const PSound & sound, BOOL wait)
{
  PINDEX pos = 0;
  PINDEX len = 0;
  char *buf = (char *) (const BYTE *) sound;

  if (!os_handle)
    return SetErrorValues(NotOpen, EBADF);

  len = sound.GetSize();
  do {

    if (!Write(&buf [pos], PMIN(320, len - pos)))
      return FALSE;
    pos += 320;
  } while (pos < len);

  if (wait)
    return WaitForPlayCompletion();

  return TRUE;
}


BOOL PSoundChannelALSA::PlayFile(const PFilePath & filename, BOOL wait)
{
  BYTE buffer [512];
  
  if (!os_handle)
    return SetErrorValues(NotOpen, EBADF);

  PFile file (filename, PFile::ReadOnly);

  if (!file.IsOpen())
    return FALSE;

  for (;;) {

    if (!file.Read (buffer, 512))
      break;

    PINDEX len = file.GetLastReadCount();
    if (len == 0)
      break;
    if (!Write(buffer, len))
      break;
  }

  file.Close();

  if (wait)
    return WaitForPlayCompletion();

  return TRUE;
}


BOOL PSoundChannelALSA::HasPlayCompleted()
{
  if (!os_handle)
    return SetErrorValues(NotOpen, EBADF);

  return (snd_pcm_state (os_handle) != SND_PCM_STATE_RUNNING);
}


BOOL PSoundChannelALSA::WaitForPlayCompletion()
{
  if (!os_handle)
    return SetErrorValues(NotOpen, EBADF);

  snd_pcm_drain (os_handle);

  return TRUE;
}


BOOL PSoundChannelALSA::RecordSound(PSound & sound)
{
  return FALSE;
}


BOOL PSoundChannelALSA::RecordFile(const PFilePath & filename)
{
  return FALSE;
}


BOOL PSoundChannelALSA::StartRecording()
{
  return FALSE;
}


BOOL PSoundChannelALSA::IsRecordBufferFull()
{
  return TRUE;
}


BOOL PSoundChannelALSA::AreAllRecordBuffersFull()
{
  return TRUE;
}


BOOL PSoundChannelALSA::WaitForRecordBufferFull()
{
  return TRUE;
}


BOOL PSoundChannelALSA::WaitForAllRecordBuffersFull()
{
  return FALSE;
}


BOOL PSoundChannelALSA::Abort()
{
  int r = 0;

  if (!os_handle)
    return FALSE;

  if ((r = snd_pcm_drain (os_handle)) < 0) {
    PTRACE (1, "ALSA\tCannot abort" << snd_strerror (r));
    return FALSE;
  }
  else
    return TRUE;
}



BOOL PSoundChannelALSA::SetVolume (unsigned newVal)
{
  unsigned i = 0;

  return Volume (TRUE, newVal, i);
}


BOOL  PSoundChannelALSA::GetVolume(unsigned &devVol)
{
  return Volume (FALSE, 0, devVol);
}
  

BOOL PSoundChannelALSA::IsOpen () const
{
  return (os_handle != NULL);
}


BOOL PSoundChannelALSA::Volume (BOOL set, unsigned set_vol, unsigned &get_vol)
{
  int err = 0;
  snd_mixer_t *handle;
  snd_mixer_elem_t *elem;
  snd_mixer_selem_id_t *sid;

  const char *play_mix_name [] = { "PCM", "Master", "Speaker", NULL };
  const char *rec_mix_name [] = { "Capture", "Mic", NULL };
  PString card_name;

  long pmin = 0, pmax = 0;
  long int vol = 0;
  int i = 0;

  if (!os_handle)
    return FALSE;

  if (card_nr == -2)
    card_name = "default";
  else
    card_name = "hw:" + PString (card_nr);

  //allocate simple id
  snd_mixer_selem_id_alloca (&sid);

  //sets simple-mixer index and name
  snd_mixer_selem_id_set_index (sid, 0);

  if ((err = snd_mixer_open (&handle, 0)) < 0) {
    PTRACE (1, "alsa-control: mixer open error: " << snd_strerror (err));
    return FALSE;
  }

  if ((err = snd_mixer_attach (handle, card_name)) < 0) {
    PTRACE (1, "alsa-control: mixer attach " << card_name << " error: " << snd_strerror(err));
    snd_mixer_close(handle);
    return FALSE;
  }

  if ((err = snd_mixer_selem_register (handle, NULL, NULL)) < 0) {
    PTRACE (1, "alsa-control: mixer register error: " << snd_strerror(err));
    snd_mixer_close(handle);
    return FALSE;
  }

  err = snd_mixer_load(handle);
  if (err < 0) {
    PTRACE (1, "alsa-control: mixer load error: " << snd_strerror(err));
    snd_mixer_close(handle);
    return FALSE;
  }

  do {
    snd_mixer_selem_id_set_name (sid, (direction == Player)?play_mix_name[i]:rec_mix_name[i]);
    elem = snd_mixer_find_selem (handle, sid);
    i++;
  } while (!elem && ((direction == Player && play_mix_name[i] != NULL) || (direction == Recorder && rec_mix_name[i] != NULL)));

  if (!elem) {
    PTRACE (1, "alsa-control: unable to find simple control.");
    snd_mixer_close(handle);
    return FALSE;
  }

  if (set) {
    if (direction == Player) {
      
      snd_mixer_selem_get_playback_volume_range (elem, &pmin, &pmax);
      vol = (set_vol * (pmax?pmax:31)) / 100;
      snd_mixer_selem_set_playback_volume_all (elem, vol);
    }
    else {
      
      snd_mixer_selem_get_capture_volume_range (elem, &pmin, &pmax);
      vol = (set_vol * (pmax?pmax:31)) / 100;
      snd_mixer_selem_set_capture_volume_all (elem, vol);
    }
    PTRACE (4, "Set volume to " << vol);
  }
  else {

    if (direction == Player) {
      snd_mixer_selem_get_playback_volume_range (elem, &pmin, &pmax);
      snd_mixer_selem_get_playback_volume (elem, SND_MIXER_SCHN_FRONT_LEFT, &vol);
    }
    else {
      snd_mixer_selem_get_capture_volume_range (elem, &pmin, &pmax);
      snd_mixer_selem_get_capture_volume (elem, SND_MIXER_SCHN_FRONT_LEFT, &vol); 
    }
    get_vol = (vol * 100) / (pmax?pmax:31);

    PTRACE (4, "Got volume " << vol);
  }

  snd_mixer_close(handle);

  return TRUE;
}
