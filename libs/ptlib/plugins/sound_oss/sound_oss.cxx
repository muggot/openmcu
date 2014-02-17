/*
 * sound.cxx
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
 * The Initial Developer of the Original Code is Equivalence Pty. Ltd.
 *
 * Portions are Copyright (C) 1993 Free Software Foundation, Inc.
 * All Rights Reserved.
 *
 * Contributor(s): Loopback feature: Philip Edelbrock <phil@netroedge.com>.
 *
 * $Log: sound_oss.cxx,v $
 * Revision 1.11  2007/09/05 12:55:43  csoutheren
 * Applied 1711861 - patches for better OpenBSD support
 * Thanks to Antoine Jacoutot
 *
 * Revision 1.10  2006/04/23 18:16:59  dsandras
 * Fixed OSS plugin when there is no resampling.
 *
 * Revision 1.9  2005/11/30 12:47:38  csoutheren
 * Removed tabs, reformatted some code, and changed tags for Doxygen
 *
 * Revision 1.8  2005/08/14 12:58:58  csoutheren
 * Fixed build problem on 64bit Linux
 *
 * Revision 1.7  2004/11/15 13:26:43  csoutheren
 * Removed debugging (oops!)
 *
 * Revision 1.6  2004/11/08 04:04:51  csoutheren
 * Added resampling to allow operation on hardware that only supports 48khz
 *
 * Revision 1.5  2004/11/07 20:01:31  dsandras
 * Make sure lastWriteCount is updated.
 *
 * Revision 1.4  2003/11/18 10:59:06  csoutheren
 * Removed ALSA compatibility hack as ALSA users can use the ALSA plugin
 *
 * Revision 1.3  2003/11/14 05:58:00  csoutheren
 * Removed loopback code as this should be in a seperate plugin :)
 *
 * Revision 1.2  2003/11/12 03:24:15  csoutheren
 * Imported plugin code from crs_pwlib_plugin branch and combined with
 *   new plugin code from Snark of GnomeMeeting
 *
 * Revision 1.1.2.1  2003/10/07 01:36:51  csoutheren
 * nitial checkin of pwlib code to do plugins.
 * Modified from original code and concept provided by Snark of Gnomemeeting
 *
 * Revision 1.59  2003/05/24 10:57:05  rogerhardiman
 * If a sound device cannot be opened in RW mode, try just 'R' or 'W' modes.
 * Needed for USB web cams with Mics which are read only sound devices when using
 * ALSA. Tested by Damien.
 *
 * Revision 1.58  2003/02/02 18:54:22  rogerh
 * FreeBSD changes for support of dspN.M (eg dsp0.0) sound card entries.
 * Problem reported by Lars Eggert <larse@isi.edu>
 *
 * Revision 1.57  2003/01/06 19:25:07  rogerh
 * Add NetBSD video support.
 * Add correct includes for OSS ioctls (note the proper way to do this now
 * is via pwlib commands)
 * From Andreas Wrede, taken in part from NetBSD's package system
 *
 * Revision 1.56  2003/01/06 19:10:22  rogerh
 * NetBSD uses /dev/audio and not /dev/dsp
 *
 * Revision 1.55  2002/12/12 09:03:56  rogerh
 * On two FreeBSD machines, Read() calls from the sound card were not blocking
 * correctly and returned with less bytes than asked for. This made OpenH323
 * close the sound channel.  Add a FreeBSD workaround so Read() loops until it
 * has all the bytes requested.
 *
 * Revision 1.54  2002/12/03 23:03:54  rogerh
 * oops - remove some test code which should not have been committed
 *
 * Revision 1.53  2002/12/03 19:11:58  rogerh
 * Open sound device in non blocking mode incase it is already open.
 *
 * Revision 1.52  2002/11/28 12:15:24  rogerh
 * Change SetVolume/GetVolume to use the mic and not the igain for the input
 * volume.
 * Our target audience are likely to be using mics and many broken
 * sound drivers do not implement igain properly.
 *
 * Revision 1.51  2002/10/17 12:57:24  robertj
 * Added ability to increase maximum file handles on a process.
 *
 * Revision 1.50  2002/10/15 10:42:45  rogerh
 * Fix loopback mode, which was broken in a recent change.
 *
 * Revision 1.49  2002/09/29 16:19:28  rogerh
 * if /dev/dsp does not exist, do not return it as the default audio device.
 * Instead, return the first dsp device.
 *
 * Revision 1.48  2002/09/29 15:56:49  rogerh
 * Revert back to checking for the /dev/soundcard directory to detect devfs.
 * If seems that the .devfsd file is not removed when devfs is not being used.
 *
 * Revision 1.47  2002/09/29 09:26:16  rogerh
 * Changes to sound card detection.
 * For Damien Sandras, allow /dev/dsp to be added to the list of sound devices
 * For FreeBSD, ignore /dev/dspN.M eg /dev/dsp0.2 which are virtual soundcards
 *
 * Revision 1.46  2002/08/30 07:58:27  craigs
 * Added fix for when sound cards are already open, thanks to Damien Sandras
 *
 * Revision 1.45  2002/08/15 19:57:38  rogerh
 * Linux defvs mode is detected with /dev/.devfsd
 *
 * Revision 1.44  2002/07/04 05:00:36  robertj
 * Fixed order of calls for OSS driver setup.
 *
 * Revision 1.43  2002/06/24 20:01:53  rogerh
 * Add support for linux devfs and /dev/sound. Based on code by Snark.
 *
 * Revision 1.42  2002/06/09 16:33:45  rogerh
 * Use new location of soundcard.h on FreeBSD
 *
 * Revision 1.41  2002/06/05 12:29:15  craigs
 * Changes for gcc 3.1
 *
 * Revision 1.40  2002/05/02 14:19:32  rogerh
 * Handle Big Endian systems correctly.
 * Patch submitted by andi@fischlustig.de
 *
 * Revision 1.39  2002/02/11 07:21:46  rogerh
 * Fix some non portable code which which seeks out /dev/dsp devices and only
 * worked on Linux. (char device for /dev/dsp is 14 on Linux, 30 on FreeBSD)
 *
 * Revision 1.38  2002/02/09 00:52:01  robertj
 * Slight adjustment to API and documentation for volume functions.
 *
 * Revision 1.37  2002/02/07 20:57:21  dereks
 * add SetVolume and GetVolume methods to PSoundChannel
 *
 * Revision 1.36  2002/01/24 05:57:38  rogerh
 * Fix warning
 *
 * Revision 1.35  2002/01/24 05:55:52  rogerh
 * fill lastReadCount (in the base class) when doing a Read.
 *
 * Revision 1.34  2002/01/07 04:15:38  robertj
 * Removed ALSA major device number as this is not how it does its OSS
 *   compatibility mode, it uses device id 14 as usual.
 *
 * Revision 1.33  2001/12/08 00:58:41  robertj
 * Added ability to stil work with strange sound card setup, thanks Damian Sandras.
 *
 * Revision 1.32  2001/09/18 05:56:03  robertj
 * Fixed numerous problems with thread suspend/resume and signals handling.
 *
 * Revision 1.31  2001/09/14 05:10:57  robertj
 * Fixed compatibility issue with FreeBSD versionof OSS.
 *
 * Revision 1.30  2001/09/14 04:53:04  robertj
 * Improved the detection of sound cards, thanks Miguel Rodríguez Pérez for the ideas.
 *
 * Revision 1.29  2001/09/10 03:03:36  robertj
 * Major change to fix problem with error codes being corrupted in a
 *   PChannel when have simultaneous reads and writes in threads.
 *
 * Revision 1.28  2001/09/03 09:15:40  robertj
 * Changed GetDeviceNames to try and find actual devices and real devices.
 *
 * Revision 1.27  2001/08/22 02:23:07  robertj
 * Fixed duplicate class name. All PWlib classes should start with P
 *
 * Revision 1.26  2001/08/21 12:33:25  rogerh
 * Make loopback mode actually work. Added the AudioDelay class from OpenMCU
 * and made Read() return silence when the buffer is empty.
 *
 * Revision 1.25  2001/05/14 06:33:19  rogerh
 * Add exit cases to loopback mode polling loops to allow the sound channel
 * to close properly when a connection closes.
 *
 * Revision 1.24  2001/02/07 03:34:29  craigs
 * Added ability get sound channel parameters
 *
 * Revision 1.23  2000/10/05 00:04:20  robertj
 * Fixed some warnings.
 *
 * Revision 1.22  2000/07/04 20:34:16  rogerh
 * Only use ioctl SNDCTL_DSP_SETDUPLEX is Linux. It is not defined in FreeBSD
 * In NetBSD and OpenBSD (using liboss), the ioctl returns EINVAL.
 *
 * Revision 1.21  2000/07/02 14:18:27  craigs
 * Fixed various problems with buffer handling
 *
 * Revision 1.20  2000/07/02 05:49:43  craigs
 * Really fixed race condition in OSS open
 *
 * Revision 1.19  2000/07/02 04:55:18  craigs
 * Fixed stupid mistake with fix for OSS race condition
 *
 * Revision 1.18  2000/07/02 04:50:44  craigs
 * Fixed potential race condition in OSS initialise
 *
 * Revision 1.17  2000/05/11 02:05:54  craigs
 * Fixed problem with PLayFile not recognizing wait flag
 *
 * Revision 1.16  2000/05/10 02:10:44  craigs
 * Added implementation for PlayFile command
 *
 * Revision 1.15  2000/05/02 08:30:26  craigs
 * Removed "memory leaks" caused by brain-dead GNU linker
 *
 * Revision 1.14  2000/04/09 18:19:23  rogerh
 * Add my changes for NetBSD support.
 *
 * Revision 1.13  2000/03/08 12:17:09  rogerh
 * Add OpenBSD support
 *
 * Revision 1.12  2000/03/04 13:02:28  robertj
 * Added simple play functions for sound files.
 *
 * Revision 1.11  2000/02/15 23:11:34  robertj
 * Audio support for FreeBSD, thanks Roger Hardiman.
 *
 * Revision 1.10  2000/01/08 06:41:08  craigs
 * Fixed problem whereby failure to open sound device returns TRUE
 *
 * Revision 1.9  1999/08/24 13:40:26  craigs
 * Fixed problem with EINTR causing sound channel reads and write to fail
 * Thanks to phil@netroedge.com!
 *
 * Revision 1.8  1999/08/17 09:42:22  robertj
 * Fixed close of sound channel in loopback mode closing stdin!
 *
 * Revision 1.7  1999/08/17 09:28:47  robertj
 * Added audio loopback psuedo-device (thanks Philip Edelbrock)
 *
 * Revision 1.6  1999/07/19 01:31:49  craigs
 * Major rewrite to assure ioctls are all done in the correct order as OSS seems
 *    to be incredibly sensitive to this.
 *
 * Revision 1.5  1999/07/11 13:42:13  craigs
 * pthreads support for Linux
 *
 * Revision 1.4  1999/06/30 13:49:26  craigs
 * Added code to allow full duplex audio
 *
 * Revision 1.3  1999/05/28 14:14:29  robertj
 * Added function to get default audio device.
 *
 * Revision 1.2  1999/05/22 12:49:05  craigs
 * Finished implementation for Linux OSS interface
 *
 * Revision 1.1  1999/02/25 03:45:00  robertj
 * Sound driver implementation changes for various unix platforms.
 *
 * Revision 1.1  1999/02/22 13:24:47  robertj
 * Added first cut sound implmentation.
 *
 */

#pragma implementation "sound_oss.h"

#include "sound_oss.h"

PCREATE_SOUND_PLUGIN(OSS, PSoundChannelOSS);

///////////////////////////////////////////////////////////////////////////////
// declare type for sound handle dictionary

PDICTIONARY(SoundHandleDict, PString, SoundHandleEntry);

static PMutex dictMutex;

static SoundHandleDict & handleDict()
{
  static SoundHandleDict dict;
  return dict;
}

///////////////////////////////////////////////////////////////////////////////

SoundHandleEntry::SoundHandleEntry()
{
  handle    = -1;
  direction = 0;
}

///////////////////////////////////////////////////////////////////////////////

PSoundChannelOSS::PSoundChannelOSS()
{
  PSoundChannelOSS::Construct();
}


PSoundChannelOSS::PSoundChannelOSS(const PString & device,
                                        Directions dir,
                                          unsigned numChannels,
                                          unsigned sampleRate,
                                          unsigned bitsPerSample)
{
  Construct();
  Open(device, dir, numChannels, sampleRate, bitsPerSample);
}


void PSoundChannelOSS::Construct()
{
  os_handle = -1;
}


PSoundChannelOSS::~PSoundChannelOSS()
{
  Close();
}

static BOOL IsNumericString(PString numbers) {
  // return true if 'numbers' contains only digits (0 to 9)
  // or if it contains digits followed by a '.'

  BOOL isNumber = FALSE;
  for (PINDEX p = 0; p < numbers.GetLength(); p++) {
    if (isdigit(numbers[p])) {
      isNumber = TRUE;
    } else {
      return isNumber;
    }
  }
  return isNumber;
}

static void CollectSoundDevices(PDirectory devdir, POrdinalToString & dsp, POrdinalToString & mixer, BOOL collect_with_names)
{
  if (!devdir.Open())
    return;

  do {
    PString filename = devdir.GetEntryName();
    PString devname = devdir + filename;
    if (devdir.IsSubDir())
      CollectSoundDevices(devname, dsp, mixer, collect_with_names);
    else {
      if (!collect_with_names) {
        // On Linux, look at the character device numbers
        PFileInfo info;
        if (devdir.GetInfo(info) &&info.type == PFileInfo::CharDevice) {
          struct stat s;
          if (lstat(devname, &s) == 0) {
            // OSS compatible audio major device numbers (OSS, SAM9407, etc)
            static const unsigned deviceNumbers[] = { 14, 145 };
            for (PINDEX i = 0; i < PARRAYSIZE(deviceNumbers); i++) {
              if ((s.st_rdev >> 8) == deviceNumbers[i]) {
                PINDEX cardnum = (s.st_rdev >> 4) & 15;
                if ((s.st_rdev & 15) == 3)  // Digital audio minor device number
                  dsp.SetAt(cardnum, devname);
                else if ((s.st_rdev & 15) == 0) // Digital audio minor device number
                  mixer.SetAt(cardnum, devname);
              }
            }
          }
        }
      } 
      else {
        // On Linux devfs systems, the major numbers can change dynamically.
        // On FreeBSD and other OSs, the major numbes are different to Linux.
        // So collect devices by looking for dsp(N) and mixer(N).
        // (or /dev/audio(N) and mixer(N) on NetBSD
        // Notes. FreeBSD supports audio stream mixing. A single sound card
        // may have multiple /dev entries in the form /dev/dspN.M
        // eg /dev/dsp0.0 /dev/dsp0.1 /dev/dsp0.2 and /dev/dsp0.3
        // When adding these to the 'dsp' string array, only the first one
        // found is used.

#if !defined P_NETBSD || !defined P_OPENBSD
        // Look for dsp
        if (filename == "dsp") {
          dsp.SetAt(0, devname);
        }

        // Look for dspN entries. Insert at position N + 1
        // and look for dspN.M entries. Insert at position N + 1 (ignoring M)

        if ((filename.GetLength() > 3) && (filename.Left(3) == "dsp")) {
          PString numbers = filename.Mid(3); // get everything after 'dsp'
          if (IsNumericString(numbers)) {
            PINDEX cardnum = numbers.AsInteger(); //dspN.M is truncated to dspN.
            // If we have not yet inserted something for this cardnum, insert it
            if (dsp.GetAt(cardnum+1) == NULL) {
              dsp.SetAt(cardnum+1, devname);
            }
          }
        }
#else
        // Look for audio on NetBSD 
        if (filename == "audio") {
          dsp.SetAt(0, devname);
        }
        // Look for audioN. Insert at position cardnum + 1
        if ((filename.GetLength() > 5) && (filename.Left(5) == "audio")) {
          PString numbers = filename.Mid(5); // get everything after 'audio'
          if (IsNumericString(numbers)) {
            PINDEX cardnum = numbers.AsInteger();
            dsp.SetAt(cardnum+1, devname);
          }
        }
#endif
        // Look for mixer
        if (filename == "mixer") {
          mixer.SetAt(0, devname);
        }
        // Look for mixerN. Insert at position cardnum + 1
        if ((filename.GetLength() > 5) && (filename.Left(5) == "mixer")) {
          PString numbers = filename.Mid(5); // get everything after 'mixer'
          if (IsNumericString(numbers)) {
            PINDEX cardnum = numbers.AsInteger();
            mixer.SetAt(cardnum+1, devname);
          }
        }
      }
    }
  } while (devdir.Next());
}


PStringArray PSoundChannelOSS::GetDeviceNames(Directions /*dir*/)
{
  // First locate sound cards. On Linux with devfs and on the other platforms
  // (eg FreeBSD), we search for filenames with dspN or mixerN.
  // On linux without devfs we scan all of the devices and look for ones
  // with major device numbers corresponding to OSS compatible drivers.

  POrdinalToString dsp, mixer;

#ifdef P_LINUX
  PDirectory devdir = "/dev/sound";
  if (devdir.Open()) {
    CollectSoundDevices("/dev/sound", dsp, mixer, TRUE); // use names (devfs)
  } else {
    CollectSoundDevices("/dev", dsp, mixer, FALSE); // use major numbers
  }
#else
  CollectSoundDevices("/dev", dsp, mixer, TRUE); // use names
#endif

  // Now we go through the collected devices and see if any have a phyisical reality
  PStringList devices;

  for (PINDEX i = 0; i < dsp.GetSize(); i++) {
    PINDEX cardnum = dsp.GetKeyAt(i);
    // Try and open mixer if have one as this is unlikely to fail for any
    // reason other than there not being a physical device
    if (mixer.Contains(cardnum)) {
      int fd = ::open(mixer[cardnum], O_RDONLY);
      if (fd >= 0) {
        // Do something with the mixer to be sure it is there
        int dummy;
        if (::ioctl(fd, SOUND_MIXER_READ_DEVMASK, &dummy) >= 0)
          devices.AppendString(dsp[cardnum]);
        ::close(fd);
      }
      else {

        // mixer failed but this could still be a valid dsp...
        // warning this is just a hack to make this work on strange mixer and dsp configurations
        // on my machine the first sound card registers 1 mixer and 2 dsp, so when my webcam
        // registers itself as dsp2 this test would fail...
        int fd = ::open(dsp[cardnum], O_RDONLY | O_NONBLOCK);
        if (fd >= 0 || errno == EBUSY) {
          devices.AppendString(dsp[cardnum]);
          ::close(fd);
        }
      }
    }
    else {
      // No mixer available, try and open it directly, this could fail if
      // the device happens to be open already
      int fd = ::open(dsp[cardnum], O_RDONLY | O_NONBLOCK);

      if (fd >= 0 || errno == EBUSY) {
        devices.AppendString(dsp[cardnum]);
        ::close(fd);
      }
    }
  }

  return devices;
}


PString PSoundChannelOSS::GetDefaultDevice(Directions dir)
{
  // Normally /dev/dsp points to the default sound device. If this is not
  // present, probe /dev for sound devices and return the first detected device.
    // return the first dsp device detected
  PStringArray devicenames;
  devicenames = PSoundChannelOSS::GetDeviceNames(dir);
  return devicenames[0];
}

BOOL PSoundChannelOSS::Open(const PString & _device,
                              Directions _dir,
                                unsigned _numChannels,
                                unsigned _sampleRate,
                                unsigned _bitsPerSample)
{

  Close();


  // lock the dictionary
  PWaitAndSignal mutex(dictMutex);

  // make the direction value 1 or 2
  int dir = _dir + 1;

  // if this device is in the dictionary
  if (handleDict().Contains(_device)) {

    SoundHandleEntry & entry = handleDict()[_device];

    // see if the sound channel is already open in this direction
    if ((entry.direction & dir) != 0) {
      return FALSE;
    }

    // flag this entry as open in this direction
    entry.direction |= dir;
    os_handle = entry.handle;

  } else {

    // this is the first time this device has been used
    // open the device in read/write mode always
    // open the device in non-blocking mode to avoid hang if already open
    os_handle = ::open((const char *)_device, O_RDWR | O_NONBLOCK);

    if ((os_handle < 0) && (errno != EWOULDBLOCK)) 
      return ConvertOSError(os_handle);

    // switch to blocking mode
    DWORD cmd = 0;
    ::ioctl(os_handle, FIONBIO, &cmd);

    // add the device to the dictionary
    SoundHandleEntry * entry = PNEW SoundHandleEntry;
    handleDict().SetAt(_device, entry); 

    // save the information into the dictionary entry
    entry->handle        = os_handle;
    entry->direction     = dir;
    entry->numChannels   = mNumChannels     = _numChannels;
    entry->sampleRate    = actualSampleRate = mSampleRate    = _sampleRate;
    entry->bitsPerSample = mBitsPerSample   = _bitsPerSample;
    entry->isInitialised = FALSE;
    entry->fragmentValue = 0x7fff0008;
    entry->resampleRate  = 0;
  }
   
  // save the direction and device
  direction     = _dir;
  device        = _device;
  isInitialised = FALSE;

  return TRUE;
}

BOOL PSoundChannelOSS::Setup()
{
  PWaitAndSignal mutex(dictMutex);

  if (os_handle < 0) {
    PTRACE(6, "OSS\tSkipping setup of " << device << " as not open");
    return FALSE;
  }

  if (isInitialised) {
    PTRACE(6, "OSS\tSkipping setup of " << device << " as instance already initialised");
    return TRUE;
  }

  // the device must always be in the dictionary
  PAssertOS(handleDict().Contains(device));

  // get record for the device
  SoundHandleEntry & entry = handleDict()[device];

  // set default return status
  BOOL stat = TRUE;

  // do not re-initialise initialised devices
  if (entry.isInitialised) {
    PTRACE(6, "OSS\tSkipping setup for " << device << " as already initialised");
    resampleRate = entry.resampleRate;

  } else {
    PTRACE(6, "OSS\tInitialising " << device << "(" << (void *)(&entry) << ")");


#if defined(P_LINUX)
    // enable full duplex (maybe).
    ::ioctl(os_handle, SNDCTL_DSP_SETDUPLEX, 0);
#endif

    stat = FALSE;

    // must always set paramaters in the following order:
    //   buffer paramaters
    //   sample format (number of bits)
    //   number of channels (mon/stereo)
    //   speed (sampling rate)

    int arg, val;

    // reset the device first so it will accept the new parms
    if (ConvertOSError(::ioctl(os_handle, SNDCTL_DSP_RESET, &arg))) {

      // set the write fragment size (applies to sound output only)
      arg = val = entry.fragmentValue;
      ::ioctl(os_handle, SNDCTL_DSP_SETFRAGMENT, &arg); 

      mBitsPerSample = entry.bitsPerSample;
#if PBYTE_ORDER == PLITTLE_ENDIAN
      arg = val = (entry.bitsPerSample == 16) ? AFMT_S16_LE : AFMT_S8;
#else
      arg = val = (entry.bitsPerSample == 16) ? AFMT_S16_BE : AFMT_S8;
#endif
      if (ConvertOSError(::ioctl(os_handle, SNDCTL_DSP_SETFMT, &arg)) || (arg != val)) {

        mNumChannels = entry.numChannels;
        arg = val = (entry.numChannels == 2) ? 1 : 0;
        if (ConvertOSError(::ioctl(os_handle, SNDCTL_DSP_STEREO, &arg)) || (arg != val)) {

          mSampleRate = entry.sampleRate;
          arg = val = entry.sampleRate;
          if (ConvertOSError(::ioctl(os_handle, SNDCTL_DSP_SPEED, &arg))) {
            stat = TRUE;

            // detect cases where the hardware can't do the actual rate we need, but can do a simple multiple
            if (arg != (int)entry.sampleRate) {
              if (((arg / entry.sampleRate) * entry.sampleRate) == (unsigned)arg) {
                PTRACE(3, "Resampling data at " << entry.sampleRate << " to match hardware rate of " << arg);
                resampleRate = entry.resampleRate = arg / entry.sampleRate;
              } else {
                PTRACE_IF(4, actualSampleRate != (unsigned)val, "Actual sample rate selected is " << actualSampleRate << ", not " << entry.sampleRate);
                actualSampleRate = arg;
              }
            }
          }
        }
      }

#if PTRACING
      audio_buf_info info;
      ::ioctl(os_handle, SNDCTL_DSP_GETOSPACE, &info);
      PTRACE(4, "OSS\tOutput: fragments = " << info.fragments
                     << ", total frags = " << info.fragstotal
                     << ", frag size   = " << info.fragsize
                     << ", bytes       = " << info.bytes);

      ::ioctl(os_handle, SNDCTL_DSP_GETISPACE, &info);
      PTRACE(4, "OSS\tInput: fragments = " << info.fragments
                     << ", total frags = " << info.fragstotal
                     << ", frag size   = " << info.fragsize
                     << ", bytes       = " << info.bytes);
#endif
    }
  }

  // ensure device is marked as initialised
  isInitialised       = TRUE;
  entry.isInitialised = TRUE;

  return stat;
}

BOOL PSoundChannelOSS::Close()
{
  // if the channel isn't open, do nothing
  if (os_handle < 0)
    return TRUE;

  // the device must be in the dictionary
  dictMutex.Wait();
  SoundHandleEntry * entry;
  PAssert((entry = handleDict().GetAt(device)) != NULL, "Unknown sound device \"" + device + "\" found");

  // modify the directions bit mask in the dictionary
  entry->direction ^= (direction+1);

  // if this is the last usage of this entry, then remove it
  if (entry->direction == 0) {
    handleDict().RemoveAt(device);
    dictMutex.Signal();
    return PChannel::Close();
  }

  // flag this channel as closed
  dictMutex.Signal();
  os_handle = -1;
  return TRUE;
}

BOOL PSoundChannelOSS::IsOpen() const
{
  return os_handle >= 0;
}

BOOL PSoundChannelOSS::Write(const void * buf, PINDEX len)
{
  lastWriteCount = 0;

  if (!Setup() || os_handle < 0)
    return FALSE;

  if (resampleRate == 0) {
    while (!ConvertOSError(::write(os_handle, (void *)buf, len))) 
      if (GetErrorCode() != Interrupted)
        return FALSE;
    lastWriteCount += len;
  }

  else {
    // cut the data into 1K blocks and upsample it
    lastWriteCount = 0;
    BYTE resampleBuffer[1024];
    const BYTE * src    = (const BYTE *)buf;
    const BYTE * srcEnd = src + len;
    while (src < srcEnd) {

      // expand the data by the appropriate sample ratio
      BYTE * dst = resampleBuffer;
      const BYTE * srcStart = src;
      unsigned j;
       
      while ((src < srcEnd) && (dst < (resampleBuffer + sizeof(resampleBuffer) - resampleRate*2))) {
        for (j = 0; j < resampleRate; ++j) {
          memcpy(dst, src, 2);
          dst += 2 ;
        }
        src += 2;
      }
      lastWriteCount += src - srcStart;
      while (!ConvertOSError(::write(os_handle, resampleBuffer, dst - resampleBuffer))) {
        if (GetErrorCode() != Interrupted) 
          return FALSE;
      }
    }

  }

  return TRUE;
}

BOOL PSoundChannelOSS::Read(void * buf, PINDEX len)
{
  lastReadCount = 0;

  if (!Setup() || os_handle < 0)
    return FALSE;

  if (resampleRate == 0) {

    PINDEX total = 0;
    while (total < len) {
      PINDEX bytes = 0;
      while (!ConvertOSError(bytes = ::read(os_handle, (void *)(((unsigned char *)buf) + total), len-total))) {
        if (GetErrorCode() != Interrupted) {
          PTRACE(6, "OSS\tRead failed");
          return FALSE;
        }
        PTRACE(6, "OSS\tRead interrupted");
      }
      total += bytes;
      if (total != len)
        PTRACE(6, "OSS\tRead completed short - " << total << " vs " << len << ". Reading more data");
    }
    lastReadCount = total;
  }

  else {

    // downsample the data

    BYTE * dst    = (BYTE *)buf;
    BYTE * dstEnd = dst + len;
    lastReadCount = 0;

    PBYTEArray resampleBuffer((1024 / resampleRate) * resampleRate);

    // downsample the data into 1K blocks 
    while (dst < dstEnd) {


      // calculate number of source bytes needed to fill the buffer
      PINDEX srcBytes = resampleRate * (dstEnd - dst);
      PINDEX bytes;

      {
        PINDEX bufLen = PMIN(resampleBuffer.GetSize(), srcBytes);
        while (!ConvertOSError(bytes = ::read(os_handle, resampleBuffer.GetPointer(), bufLen))) {
          if (GetErrorCode() != Interrupted) 
            return FALSE;
        }
      }

      // use an average, not just a single sample
      const BYTE * src = resampleBuffer;
      while ( ((src - resampleBuffer) < bytes) && (dst < dstEnd)) {
        int sample = 0;
        unsigned j;
        for (j = 0; j < resampleRate; ++j) {
          sample += *(PUInt16l *)src;
          src += 2;
        }
        *(PUInt16l *)dst = sample / resampleRate;
        dst +=2 ;
        lastReadCount += 2;
      }
    }
  }

  if (lastReadCount != len)
    PTRACE(6, "OSS\tRead completed short - " << lastReadCount << " vs " << len);
  else
    PTRACE(6, "OSS\tRead completed");

  return TRUE;
}


BOOL PSoundChannelOSS::SetFormat(unsigned numChannels,
                              unsigned sampleRate,
                              unsigned bitsPerSample)
{
  if (os_handle < 0)
    return SetErrorValues(NotOpen, EBADF);

  // check parameters
  PAssert((bitsPerSample == 8) || (bitsPerSample == 16), PInvalidParameter);
  PAssert(numChannels >= 1 && numChannels <= 2, PInvalidParameter);

  // lock the dictionary
  PWaitAndSignal mutex(dictMutex);

  // the device must always be in the dictionary
  PAssertOS(handleDict().Contains(device));

  // get record for the device
  SoundHandleEntry & entry = handleDict()[device];

  if (entry.isInitialised) {
    if ((numChannels   != entry.numChannels) ||
        (sampleRate    != entry.sampleRate) ||
        (bitsPerSample != entry.bitsPerSample)) {
      PTRACE(6, "OSS\tTried to change read/write format without stopping");
      return FALSE;
    }
    return TRUE;
  }

  Abort();

  entry.numChannels   = numChannels;
  entry.sampleRate    = sampleRate;
  entry.bitsPerSample = bitsPerSample;
  entry.isInitialised  = FALSE;

  // mark this channel as uninitialised
  isInitialised = FALSE;

  return TRUE;
}

// Get  the number of channels (mono/stereo) in the sound.
unsigned PSoundChannelOSS::GetChannels()   const
{
  return mNumChannels;
}

// Get the sample rate in samples per second.
unsigned PSoundChannelOSS::GetSampleRate() const
{
  return actualSampleRate;
}

// Get the sample size in bits per sample.
unsigned PSoundChannelOSS::GetSampleSize() const
{
  return mBitsPerSample;
}

BOOL PSoundChannelOSS::SetBuffers(PINDEX size, PINDEX count)
{
  if (os_handle < 0)
    return SetErrorValues(NotOpen, EBADF);

  //PINDEX totalSize = size * count;

  //size = 16;
  //count = (totalSize + 15) / 16;

  PAssert(size > 0 && count > 0 && count < 65536, PInvalidParameter);
  int arg = 1;
  while (size > (PINDEX)(1 << arg))
    arg++;

  arg |= count << 16;

  // lock the dictionary
  PWaitAndSignal mutex(dictMutex);

  // the device must always be in the dictionary
  PAssertOS(handleDict().Contains(device));

  // get record for the device
  SoundHandleEntry & entry = handleDict()[device];

  if (entry.isInitialised) {
    if (entry.fragmentValue != (unsigned)arg) {
      PTRACE(6, "OSS\tTried to change buffers without stopping");
      return FALSE;
    }
    return TRUE;
  }

  Abort();

  // set information in the common record
  entry.fragmentValue = arg;
  entry.isInitialised = FALSE;

  // flag this channel as not initialised
  isInitialised       = FALSE;

  return TRUE;
}


BOOL PSoundChannelOSS::GetBuffers(PINDEX & size, PINDEX & count)
{
  if (os_handle < 0)
    return SetErrorValues(NotOpen, EBADF);

  // lock the dictionary
  PWaitAndSignal mutex(dictMutex);

  // the device must always be in the dictionary
  PAssertOS(handleDict().Contains(device));

  SoundHandleEntry & entry = handleDict()[device];

  int arg = entry.fragmentValue;

  count = arg >> 16;
  size = 1 << (arg&0xffff);
  return TRUE;
}


BOOL PSoundChannelOSS::PlaySound(const PSound & sound, BOOL wait)
{
  if (os_handle < 0)
    return SetErrorValues(NotOpen, EBADF);

  Abort();

  if (!Write((const BYTE *)sound, sound.GetSize()))
    return FALSE;

  if (wait)
    return WaitForPlayCompletion();

  return TRUE;
}


BOOL PSoundChannelOSS::PlayFile(const PFilePath & filename, BOOL wait)
{
  if (os_handle < 0)
    return SetErrorValues(NotOpen, EBADF);

  PFile file(filename, PFile::ReadOnly);
  if (!file.IsOpen())
    return FALSE;

  for (;;) {
    BYTE buffer[256];
    if (!file.Read(buffer, 256))
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


BOOL PSoundChannelOSS::HasPlayCompleted()
{
  if (os_handle < 0)
    return SetErrorValues(NotOpen, EBADF);

  audio_buf_info info;
  if (!ConvertOSError(::ioctl(os_handle, SNDCTL_DSP_GETOSPACE, &info)))
    return FALSE;

  return info.fragments == info.fragstotal;
}


BOOL PSoundChannelOSS::WaitForPlayCompletion()
{
  if (os_handle < 0)
    return SetErrorValues(NotOpen, EBADF);

  return ConvertOSError(::ioctl(os_handle, SNDCTL_DSP_SYNC, NULL));
}


BOOL PSoundChannelOSS::RecordSound(PSound & sound)
{
  if (os_handle < 0)
    return SetErrorValues(NotOpen, EBADF);

  return FALSE;
}


BOOL PSoundChannelOSS::RecordFile(const PFilePath & filename)
{
  if (os_handle < 0)
    return SetErrorValues(NotOpen, EBADF);

  return FALSE;
}


BOOL PSoundChannelOSS::StartRecording()
{
  if (os_handle < 0)
    return SetErrorValues(NotOpen, EBADF);

  if (os_handle == 0)
    return TRUE;

  P_fd_set fds = os_handle;
  P_timeval instant;
  return ConvertOSError(::select(1, fds, NULL, NULL, instant));
}


BOOL PSoundChannelOSS::IsRecordBufferFull()
{
  if (os_handle < 0)
    return SetErrorValues(NotOpen, EBADF);

  audio_buf_info info;
  if (!ConvertOSError(::ioctl(os_handle, SNDCTL_DSP_GETISPACE, &info)))
    return FALSE;

  return info.fragments > 0;
}


BOOL PSoundChannelOSS::AreAllRecordBuffersFull()
{
  if (os_handle < 0)
    return SetErrorValues(NotOpen, EBADF);

  audio_buf_info info;
  if (!ConvertOSError(::ioctl(os_handle, SNDCTL_DSP_GETISPACE, &info)))
    return FALSE;

  return info.fragments == info.fragstotal;
}


BOOL PSoundChannelOSS::WaitForRecordBufferFull()
{
  if (os_handle < 0)
    return SetErrorValues(NotOpen, EBADF);

  return PXSetIOBlock(PXReadBlock, readTimeout);
}


BOOL PSoundChannelOSS::WaitForAllRecordBuffersFull()
{
  return FALSE;
}


BOOL PSoundChannelOSS::Abort()
{
  return ConvertOSError(ioctl(os_handle, SNDCTL_DSP_RESET, NULL));
}



BOOL PSoundChannelOSS::SetVolume(unsigned newVal)
{
  if (os_handle <= 0)  //CAnnot set volume in loop back mode.
    return FALSE;

  int rc, deviceVol = (newVal << 8) | newVal;

  if (direction  == Player) 
    rc = ::ioctl(os_handle, MIXER_WRITE(SOUND_MIXER_VOLUME), &deviceVol);
   else 
    rc = ::ioctl(os_handle, MIXER_WRITE(SOUND_MIXER_MIC), &deviceVol);

  if (rc < 0) {
    PTRACE(1, "PSoundChannelOSS::SetVolume failed : " << ::strerror(errno));
    return FALSE;
  }

  return TRUE;
}

BOOL  PSoundChannelOSS::GetVolume(unsigned &devVol)
{
  if (os_handle <= 0)  //CAnnot get volume in loop back mode.
    return FALSE;
  
  int vol, rc;
  if (direction == Player)
    rc = ::ioctl(os_handle, MIXER_READ(SOUND_MIXER_VOLUME), &vol);
  else
    rc = ::ioctl(os_handle, MIXER_READ(SOUND_MIXER_MIC), &vol);
  
  if (rc < 0) {
    PTRACE(1,  "PSoundChannelOSS::GetVolume failed : " << ::strerror(errno)) ;
    return FALSE;
  }
  
  devVol = vol & 0xff;
  return TRUE;
}
  


// End of file
