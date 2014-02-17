/*
 * qsa.cxx
 *
 * QNX Sound Architecture
 */

#pragma implementation "sound.h"

#include <ptlib.h>
#include <stdio.h>
#include <sys/asoundlib.h>

typedef struct _SoundHandleEntry {
	struct _SoundHandleEntry *next;
	
	int handle;
	int direction;

	unsigned numChannels;
	unsigned sampleRate;
	unsigned bitsPerSample;
	unsigned fragmentValue;
	BOOL isInitialised;
	
	snd_pcm_t   *pcm_handle;
	int         card;
	int         dev;
	snd_mixer_t *mixer_handle;
	snd_mixer_group_t group;
} SoundHandleEntry;

static SoundHandleEntry *SoundHandleList;
static pthread_rwlock_t SoundHandleLock = PTHREAD_RWLOCK_INITIALIZER;

static int snd_openmode[2] = {SND_PCM_OPEN_CAPTURE, SND_PCM_OPEN_PLAYBACK};
static int snd_chnmode[2]  = {SND_PCM_CHANNEL_CAPTURE, SND_PCM_CHANNEL_PLAYBACK};

PSound::PSound(unsigned channels,
               unsigned samplesPerSecond,
               unsigned bitsPerSample,
               PINDEX   bufferSize,
               const BYTE * buffer)
{
	encoding = 0;
	numChannels = channels;
	sampleRate = samplesPerSecond;
	sampleSize = bitsPerSample;
	SetSize(bufferSize);
	if (buffer != NULL)
	  memcpy(GetPointer(), buffer, bufferSize);
}


PSound::PSound(const PFilePath & filename)
{
	encoding = 0;
	numChannels = 1;
	sampleRate = 8000;
	sampleSize = 16;
	Load(filename);
}


PSound & PSound::operator=(const PBYTEArray & data)
{
	PBYTEArray::operator=(data);
	return *this;
}


void PSound::SetFormat(unsigned channels,
                       unsigned samplesPerSecond,
                       unsigned bitsPerSample)
{
	encoding = 0;
	numChannels = channels;
	sampleRate = samplesPerSecond;
	sampleSize = bitsPerSample;
	formatInfo.SetSize(0);
}


BOOL PSound::Load(const PFilePath & /*filename*/)
{
	return FALSE;
}


BOOL PSound::Save(const PFilePath & /*filename*/)
{
	return FALSE;
}


BOOL PSound::Play()
{
	PSoundChannel channel(PSoundChannel::GetDefaultDevice(PSoundChannel::Player),
						  PSoundChannel::Player);
	if (!channel.IsOpen())
	  return FALSE;
	
	return channel.PlaySound(*this, TRUE);
}


BOOL PSound::PlayFile(const PFilePath & file, BOOL wait)
{
	PSoundChannel channel(PSoundChannel::GetDefaultDevice(PSoundChannel::Player),
						  PSoundChannel::Player);
	if (!channel.IsOpen())
	  return FALSE;
	
	return channel.PlayFile(file, wait);
}


PSoundChannel::PSoundChannel()
{
	Construct();
}


PSoundChannel::PSoundChannel(const PString & device,
                             Directions dir,
                             unsigned numChannels,
                             unsigned sampleRate,
                             unsigned bitsPerSample)
{
	Construct();
	Open(device, dir, numChannels, sampleRate, bitsPerSample);
}


void PSoundChannel::Construct()
{
	os_handle = -1;
}


PSoundChannel::~PSoundChannel()
{
	Close();
}


PStringArray PSoundChannel::GetDeviceNames(Directions dir)
{
	PStringList devices;
	PDirectory  devdir = "/dev/snd";
	
	if (!devdir.Open())
	  return NULL;

	do {
		PString filename = devdir.GetEntryName();
		PString devname = devdir + filename;

		if ((filename.GetLength() > 3) && (filename.Left(3) == "pcm") &&
			(filename.Right(1) == (dir == Recorder ? "r" : "p")))
		{
			int fd = ::open(filename, O_RDONLY);
			if (fd >= 0) {
				devices.AppendString(filename);
				::close(fd);
			}
		}
	} while (devdir.Next());
	return devices;
}


PString PSoundChannel::GetDefaultDevice(Directions dir)
{
	PString filename;
	
	if (dir == Player)
	  filename = "/dev/snd/pcmPreferredp";
	else
	  filename = "/dev/snd/pcmPreferredc";
	
	int fd = ::open(filename, O_RDONLY);
//	PTRACE(1, "GetDefaultDevice; fd = " << fd << ", filename ='" << filename <<"'\n");
	if (fd >=0) {
		::close (fd);
		return filename;
	} else {
		return (filename = "/dev/null");
	}
}


BOOL PSoundChannel::Open(const PString & _device,
						 Directions _dir,
						 unsigned _numChannels,
						 unsigned _sampleRate,
						 unsigned _bitsPerSample)
{
	Close();
	
	// make the direction value 1 or 2
	int dir = _dir + 1;

	SoundHandleEntry *entry;
	
	pthread_rwlock_wrlock(&SoundHandleLock);
	for (entry = SoundHandleList; entry && entry->handle != os_handle; entry = entry->next);
	
	if (entry) {

		PTRACE(6, "OSS\tOpen occured for existing entry");
		
		// see if the sound channel is already open in this direction
		if ((entry->direction & dir) != 0) {
			pthread_rwlock_unlock(&SoundHandleLock);
			return FALSE;
		}
		
		// flag this entry as open in this direction
		entry->direction |= dir;
		os_handle = entry->handle;
	} else {
		
		PTRACE(6, "OSS\tOpen occured for new entry");
		
		SoundHandleEntry *entry = (SoundHandleEntry *)::malloc(sizeof(*entry));
		
		if (!entry) {
			pthread_rwlock_unlock(&SoundHandleLock);
			return FALSE;
		}

		// this is the first time this device has been used
		// open the device in read/write mode always
		if (((_dir == Player) && _device == "/dev/snd/pcmPreferredp") ||
			((_dir == Recorder) && _device == "/dev/snd/pcmPreferredc"))
		{
			os_handle = snd_pcm_open_preferred(&entry->pcm_handle, &entry->card, &entry->dev, snd_openmode[_dir]);
		} else {
			if (sscanf(_device, "/dev/snd/pcmC%iD%i", &entry->card, &entry->dev) != 2) {
				errno = ESRCH;
				return ConvertOSError(os_handle);
			}
			os_handle = snd_pcm_open(&entry->pcm_handle, entry->card, entry->dev, snd_openmode[_dir]);
		}
		
		if (os_handle < 0) {
			errno = -os_handle;
			os_handle = 0;
			return ConvertOSError(os_handle);
		}

		if (snd_pcm_plugin_set_disable(entry->pcm_handle, PLUGIN_DISABLE_MMAP) < 0)
		  return FALSE;
	
		// save the information into the dictionary entry
		os_handle            = snd_pcm_file_descriptor(entry->pcm_handle, snd_chnmode[_dir]);
		entry->handle        = os_handle;
		entry->direction     = dir;
		entry->numChannels   = mNumChannels     = _numChannels;
		entry->sampleRate    = actualSampleRate = mSampleRate    = _sampleRate;
		entry->bitsPerSample = mBitsPerSample   = _bitsPerSample;
		entry->isInitialised = FALSE;
		entry->fragmentValue = 0x7fff0008;
		entry->mixer_handle  = 0;
		
		entry->next          = SoundHandleList;
		SoundHandleList      = entry;
	}
	pthread_rwlock_unlock(&SoundHandleLock);
	
	// save the direction and device
	direction     = _dir;
	device        = _device;
	isInitialised = FALSE;
	
	return TRUE;
}

BOOL PSoundChannel::Setup()
{
	if (os_handle < 0) {
		PTRACE(6, "OSS\tSkipping setup of " << device << " as not open");
		return FALSE;
	}
	
	if (isInitialised) {
		PTRACE(6, "OSS\tSkipping setup of " << device << " as instance already initialised");
		return TRUE;
	}

	SoundHandleEntry *entry;
	pthread_rwlock_rdlock(&SoundHandleLock);
	for (entry = SoundHandleList; entry && entry->handle != os_handle;
		 entry = entry->next);
	
	// set default return status
	BOOL stat = TRUE;
	
	// do not re-initialise initialised devices
	if (entry->isInitialised) {
		pthread_rwlock_unlock(&SoundHandleLock);
		PTRACE(6, "OSS\tSkipping setup for " << device << " as already initialised");
		return stat;
	}
	
	PTRACE(6, "OSS\tInitialising " << device << "(" << (void *)(&entry) << ")");

    stat = FALSE;
	
	mBitsPerSample = entry->bitsPerSample;
	mNumChannels = entry->numChannels;
	mSampleRate = entry->sampleRate;

	snd_pcm_channel_params_t pp;

	memset(&pp, 0, sizeof(pp));
	pp.channel = snd_chnmode[direction];
	pp.mode = SND_PCM_MODE_BLOCK;
	pp.start_mode = (direction == Player) ? SND_PCM_START_FULL : SND_PCM_START_DATA;
	pp.stop_mode = SND_PCM_STOP_STOP;
	pp.buf.block.frags_min = 1;
	pp.buf.block.frag_size = 1 << (entry->fragmentValue & 0xffff);
	pp.buf.block.frags_max = ((unsigned)entry->fragmentValue >> 16) & 0x7fff;
	if (pp.buf.block.frags_max == 0)
	  pp.buf.block.frags_max = 65536;
		
	pp.format.interleave = 1;
	pp.format.rate = entry->sampleRate;
	pp.format.voices = entry->numChannels;
		
#if PBYTE_ORDER == PLITTLE_ENDIAN
	pp.format.format = (entry->bitsPerSample == 16) ? SND_PCM_SFMT_S16_LE : SND_PCM_SFMT_U8;
#else
	pp.format.format = (entry->bitsPerSample == 16) ? SND_PCM_SFMT_S16_BE : SND_PCM_SFMT_U8;
#endif
	
	if (snd_pcm_plugin_params(entry->pcm_handle, &pp) < 0) {
		pthread_rwlock_unlock(&SoundHandleLock);
		return stat;
	}
	
	if (snd_pcm_plugin_prepare(entry->pcm_handle, snd_chnmode[direction]) < 0) {
		pthread_rwlock_unlock(&SoundHandleLock);
		return stat;
	}
	
	/* also open the mixer */
	snd_pcm_channel_setup_t setup;
	
	memset(&setup, 0, sizeof(setup));
	memset(&entry->group, 0, sizeof(entry->group));
	setup.channel = snd_chnmode[direction];
	setup.mixer_gid = &entry->group.gid;
	
	if (snd_pcm_plugin_setup(entry->pcm_handle, &setup) < 0) {
		pthread_rwlock_unlock(&SoundHandleLock);
		return FALSE;
	}
		
	if (snd_mixer_open(&entry->mixer_handle, entry->card, setup.mixer_device) < 0) {
		pthread_rwlock_unlock(&SoundHandleLock);
		return FALSE;
	}
	
	actualSampleRate = setup.format.rate;
	stat = TRUE;

#if PTRACING
	PTRACE(4, "QSA: Frag Size = " << setup.buf.block.frag_size
			   << ", Rate = " << setup.format.rate
		       << ", Mixer Pcm Group [" << entry->group.gid.name << "]\n");
#endif

	pthread_rwlock_unlock(&SoundHandleLock);
	// ensure device is marked as initialised
	isInitialised        = TRUE;
	entry->isInitialised = TRUE;
	
	return stat;
}

BOOL PSoundChannel::Close()
{
	// if the channel isn't open, do nothing
	if (os_handle < 0)
	  return TRUE;
	
	SoundHandleEntry *entry, **entryp;
	
	pthread_rwlock_wrlock(&SoundHandleLock);
	for (entryp = &SoundHandleList, entry = *entryp; entry && entry->handle != os_handle;
		 entryp = &entry->next, entry = *entryp);
	  
	if (!entry) {
		pthread_rwlock_unlock(&SoundHandleLock);
		return TRUE;
	}
	
	// modify the directions bit mask in the dictionary
	entry->direction ^= (direction+1);
	
	// if this is the last usage of this entry, then remove it
	if (entry->direction == 0) {
		snd_mixer_close(entry->mixer_handle);
		snd_pcm_plugin_flush(entry->pcm_handle, snd_chnmode[direction]);
		snd_pcm_close(entry->pcm_handle);
		*entryp = entry->next;
		::free(entry);
		os_handle = -1;
		pthread_rwlock_unlock(&SoundHandleLock);
		return PChannel::Close();
	}
	
	// flag this channel as closed
	pthread_rwlock_unlock(&SoundHandleLock);
	return TRUE;
}

BOOL PSoundChannel::Write(const void * buf, PINDEX len)
{
	if (!Setup())
	  return FALSE;
	
	if (os_handle < 0)
	  return FALSE;
	
	SoundHandleEntry * entry;
	pthread_rwlock_rdlock(&SoundHandleLock);
	for (entry = SoundHandleList; entry && entry->handle != os_handle;
		 entry = entry->next);
	
	snd_pcm_channel_status_t status;
	int written = 0;
	
	while ((written += snd_pcm_plugin_write(entry->pcm_handle, buf, len)) < len)
	{
		memset(&status, 0, sizeof(status));
		status.channel = SND_PCM_CHANNEL_PLAYBACK;
		if (snd_pcm_plugin_status(entry->pcm_handle, &status) < 0) {
			pthread_rwlock_unlock(&SoundHandleLock);
			return FALSE;
		}
		if (status.status == SND_PCM_STATUS_READY ||
			status.status == SND_PCM_STATUS_UNDERRUN)
		{
			if (snd_pcm_plugin_prepare(entry->pcm_handle, snd_chnmode[direction]) < 0) {
				pthread_rwlock_unlock(&SoundHandleLock);				
				return FALSE;
			}
		}
		if (written < 0)
		  written = 0;
	}	
	pthread_rwlock_unlock(&SoundHandleLock);
	return TRUE;
}

BOOL PSoundChannel::Read(void * buf, PINDEX len)
{
	lastReadCount = 0;

	if (!Setup())
	  return FALSE;

	if (os_handle < 0)
	  return FALSE;

	SoundHandleEntry * entry;
	pthread_rwlock_rdlock(&SoundHandleLock);
	for (entry = SoundHandleList; entry && entry->handle != os_handle;
		 entry = entry->next);
	
	PTRACE(6, "QSA\tRead start");
	
	lastReadCount = snd_pcm_plugin_read(entry->pcm_handle, buf, len);
	
    if (lastReadCount < len) {
		snd_pcm_channel_status_t status;
		
		memset(&status, 0, sizeof(status));
		status.channel = SND_PCM_CHANNEL_CAPTURE;
		if (snd_pcm_plugin_status(entry->pcm_handle, &status) < 0) {
			pthread_rwlock_unlock(&SoundHandleLock);
			PTRACE(6, "QSA\tRead failed");		  
			return FALSE;
		}
		
		if (status.status == SND_PCM_STATUS_READY ||
			status.status == SND_PCM_STATUS_OVERRUN) {
			if (snd_pcm_plugin_prepare(entry->pcm_handle, SND_PCM_CHANNEL_CAPTURE) < 0) {
				pthread_rwlock_unlock(&SoundHandleLock);
				PTRACE(6, "QSA\tRead failed");		  
				return FALSE;
			}
		}
		PTRACE(6, "QSA\tRead completed short - " << lastReadCount << " vs " << len);
	} else {
		PTRACE(6, "QSA\tRead completed");
	}
	pthread_rwlock_unlock(&SoundHandleLock);
    return TRUE;
}

BOOL PSoundChannel::SetFormat(unsigned numChannels,
                              unsigned sampleRate,
                              unsigned bitsPerSample)
{
	if (os_handle < 0)
	  return SetErrorValues(NotOpen, EBADF);

	// check parameters
	PAssert((bitsPerSample == 8) || (bitsPerSample == 16), PInvalidParameter);
	PAssert(numChannels >= 1 && numChannels <= 2, PInvalidParameter);

	SoundHandleEntry * entry;
	pthread_rwlock_rdlock(&SoundHandleLock);
	for (entry = SoundHandleList; entry && entry->handle != os_handle;
		 entry = entry->next);
	
	if (entry->isInitialised) {
		if ((numChannels   != entry->numChannels) ||
			(sampleRate    != entry->sampleRate) ||
			(bitsPerSample != entry->bitsPerSample)) {
			pthread_rwlock_unlock(&SoundHandleLock);
			PTRACE(6, "OSS\tTried to change read/write format without stopping");
			return FALSE;
		}
		pthread_rwlock_unlock(&SoundHandleLock);
		return TRUE;
	}

	if (direction == Player) {
		snd_pcm_plugin_playback_drain(entry->pcm_handle);
	}

	entry->numChannels   = numChannels;
	entry->sampleRate    = sampleRate;
	entry->bitsPerSample = bitsPerSample;
	entry->isInitialised  = FALSE;
	pthread_rwlock_unlock(&SoundHandleLock);
	
	// mark this channel as uninitialised
	isInitialised = FALSE;
	
	return TRUE;
}

// Get  the number of channels (mono/stereo) in the sound.
unsigned PSoundChannel::GetChannels()   const
{
	return mNumChannels;
}

// Get the sample rate in samples per second.
unsigned PSoundChannel::GetSampleRate() const
{
	return actualSampleRate;
}

// Get the sample size in bits per sample.
unsigned PSoundChannel::GetSampleSize() const
{
	return mBitsPerSample;
}


BOOL PSoundChannel::SetBuffers(PINDEX size, PINDEX count)
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
	
	SoundHandleEntry * entry;
	pthread_rwlock_rdlock(&SoundHandleLock);
	for (entry = SoundHandleList; entry && entry->handle != os_handle;
		 entry = entry->next);
	
	if (entry->isInitialised) {
		if (entry->fragmentValue != (unsigned)arg) {
			pthread_rwlock_unlock(&SoundHandleLock);
			PTRACE(6, "OSS\tTried to change buffers without stopping");
			return FALSE;
		}
		pthread_rwlock_unlock(&SoundHandleLock);
		return TRUE;
	}
	
	if (direction == Player) {
		snd_pcm_plugin_playback_drain(entry->pcm_handle);
	}
	
	// set information in the common record
	entry->fragmentValue = arg;
	entry->isInitialised = FALSE;
	pthread_rwlock_unlock(&SoundHandleLock);
	
	// flag this channel as not initialised
	isInitialised       = FALSE;
	
	return TRUE;
}


BOOL PSoundChannel::GetBuffers(PINDEX & size, PINDEX & count)
{
	if (os_handle < 0)
	  return SetErrorValues(NotOpen, EBADF);
	
	SoundHandleEntry * entry;
	pthread_rwlock_rdlock(&SoundHandleLock);
	for (entry = SoundHandleList; entry && entry->handle != os_handle;
		 entry = entry->next);
	
	int arg = entry->fragmentValue;
	
	count = arg >> 16;
	size = 1 << (arg&0xffff);
	pthread_rwlock_unlock(&SoundHandleLock);
	
	return TRUE;
}


BOOL PSoundChannel::PlaySound(const PSound & sound, BOOL wait)
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


BOOL PSoundChannel::PlayFile(const PFilePath & filename, BOOL wait)
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


BOOL PSoundChannel::HasPlayCompleted()
{
	if (os_handle < 0)
	  return SetErrorValues(NotOpen, EBADF);
	
	SoundHandleEntry * entry;
	pthread_rwlock_rdlock(&SoundHandleLock);
	for (entry = SoundHandleList; entry && entry->handle != os_handle;
		 entry = entry->next);
	
	snd_pcm_channel_status_t status;
	memset(&status, 0, sizeof(status));
	status.channel = snd_chnmode[direction];

	if (snd_pcm_plugin_status(entry->pcm_handle, &status) < 0) {
		pthread_rwlock_unlock(&SoundHandleLock);
		return FALSE;
	}

	int ret = (abs(status.free) / (entry->bitsPerSample / 8));
	pthread_rwlock_unlock(&SoundHandleLock);
	return ret;
}

BOOL PSoundChannel::WaitForPlayCompletion()
{
	if (os_handle < 0)
	  return SetErrorValues(NotOpen, EBADF);

	SoundHandleEntry * entry;
	pthread_rwlock_rdlock(&SoundHandleLock);
	for (entry = SoundHandleList; entry && entry->handle != os_handle;
		 entry = entry->next);
	
	if (snd_pcm_playback_flush(entry->pcm_handle) < 0) {
		pthread_rwlock_unlock(&SoundHandleLock);
		return FALSE;
	}
	
	pthread_rwlock_unlock(&SoundHandleLock);
	return TRUE;
}


BOOL PSoundChannel::RecordSound(PSound & sound)
{
  if (os_handle < 0)
    return SetErrorValues(NotOpen, EBADF);

  return FALSE;
}


BOOL PSoundChannel::RecordFile(const PFilePath & filename)
{
  if (os_handle < 0)
    return SetErrorValues(NotOpen, EBADF);

  return FALSE;
}


BOOL PSoundChannel::StartRecording()
{
	if (os_handle < 0)
	  return SetErrorValues(NotOpen, EBADF);

	int fd = os_handle;
	fd_set fds;
	FD_ZERO(&fds);
	FD_SET(fd, &fds);
	struct timeval instant = {0, 0};
	return ConvertOSError(::select(fd + 1, &fds, NULL, NULL, &instant));
}


BOOL PSoundChannel::IsRecordBufferFull()
{
	if (os_handle < 0)
	  return SetErrorValues(NotOpen, EBADF);

	PTRACE(1, "IsRecordBufferFull()\n");
	/* do I suppose to get the status, and check sth ? */
	return TRUE;
}


BOOL PSoundChannel::AreAllRecordBuffersFull()
{
	if (os_handle < 0)
	  return SetErrorValues(NotOpen, EBADF);

	PTRACE(1, "AreAllRecordBuffersFull()\n");
	/* do I suppose to get the status, and check sth ? */
	return TRUE;
}


BOOL PSoundChannel::WaitForRecordBufferFull()
{
	if (os_handle < 0)
	  return SetErrorValues(NotOpen, EBADF);
	
	PTRACE(1, "WaitForRecordBufferFull()\n");
	return PXSetIOBlock(PXReadBlock, readTimeout);
}


BOOL PSoundChannel::WaitForAllRecordBuffersFull()
{
	PTRACE(1, "WaitForAllRecordBuffersFull()\n");
	return FALSE;
}


BOOL PSoundChannel::Abort()
{
	if (direction == Player && os_handle != -1) {
	SoundHandleEntry * entry;
		pthread_rwlock_rdlock(&SoundHandleLock);
		for (entry = SoundHandleList; entry && entry->handle != os_handle;
			 entry = entry->next);
		
		snd_pcm_plugin_playback_drain(entry->pcm_handle);
		pthread_rwlock_unlock(&SoundHandleLock);
	}
	return TRUE;
}


BOOL PSoundChannel::SetVolume(unsigned newVal)
{
	if (os_handle < 0)
	{
		return FALSE;
	}

	SoundHandleEntry *entry;
	pthread_rwlock_rdlock(&SoundHandleLock);
	for (entry = SoundHandleList; entry && entry->handle != os_handle;
		 entry = entry->next);
	
	int ret;
	
	if ((ret = snd_mixer_group_read(entry->mixer_handle, &entry->group)) < 0)
	{
		pthread_rwlock_unlock(&SoundHandleLock);
		cerr << "snd_mixer_group_read: " << strerror(-ret) << endl;
		return FALSE;
	}

	/* QSA treat the newVal as a percentage */
	newVal = (newVal  * (entry->group.max - entry->group.min) / 100) + entry->group.min;
	
	entry->group.volume.names.front_left = newVal;
	entry->group.volume.names.front_right = newVal;
	
	if ((ret = snd_mixer_group_write(entry->mixer_handle, &entry->group)) < 0)
	{
		pthread_rwlock_unlock(&SoundHandleLock);
		cerr << "snd_mixer_group_write: " << strerror(-ret) << endl;
		return FALSE;
	}
	pthread_rwlock_unlock(&SoundHandleLock);	
	return TRUE;
}

BOOL  PSoundChannel::GetVolume(unsigned &devVol)
{
	if (os_handle == 0)
	{
		return FALSE;
	}
  
	SoundHandleEntry *entry;
	pthread_rwlock_rdlock(&SoundHandleLock);
	for (entry = SoundHandleList; entry && entry->handle != os_handle;
		 entry = entry->next);
	
	int ret;
	
	if ((ret = snd_mixer_group_read(entry->mixer_handle, &entry->group)) < 0)
	{
		pthread_rwlock_unlock(&SoundHandleLock);
		return FALSE;
	}
	pthread_rwlock_unlock(&SoundHandleLock);
	
	/* return the percentage */
	devVol = (unsigned)(entry->group.volume.names.front_left - entry->group.min) * 100
	  / (entry->group.max - entry->group.min);

	return TRUE;
}
  


// End of file
