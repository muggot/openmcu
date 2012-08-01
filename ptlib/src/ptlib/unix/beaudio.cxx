/*
 * beaudio.cxx
 *
 * Sound driver implementation.
 *
 * Portable Windows Library
 *
 * Copyright (c) 1993-2001 Equivalence Pty. Ltd.
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
 * Contributor(s): 
 * Yuri Kiryanov, ykiryanov at users.sourceforge.net,
 * Jac Goudsmit <jac@be.com>.
 *
 * $Log: beaudio.cxx,v $
 * Revision 1.16  2004/10/26 18:08:54  ykiryanov
 * Added code for old Media Kit, to be backwards compatible with R5, and Zeta ifdef
 *
 * Revision 1.15  2004/06/16 01:55:10  ykiryanov
 * Added usage of lastReadCount - sound capture now works
 *
 * Revision 1.14  2004/05/30 04:48:45  ykiryanov
 * Stable version
 *
 * Revision 1.12  2004/05/14 05:26:57  ykiryanov
 * Fixed dynamic cast bug
 *
 * Revision 1.11  2004/04/18 00:32:26  ykiryanov
 * Fized compiler choking on <dynamic_cast>.
 *
 * Revision 1.10  2004/04/02 03:29:07  ykiryanov
 * New improved code
 *
 * Revision 1.9  2002/02/09 00:52:01  robertj
 * Slight adjustment to API and documentation for volume functions.
 *
 * Revision 1.8  2002/02/07 20:57:21  dereks
 * add SetVolume and GetVolume methods to PSoundChannelBeOS
 *
 * Revision 1.7  2001/07/09 06:16:15  yurik
 * Jac Goudsmit's BeOS changes of July,6th. Cleaning up media subsystem etc.
 *
 * Revision 1.6  2000/12/16 13:08:56  rogerh
 * BeOS changes from Yuri Kiryanov <openh323@kiryanov.com>
 *
 * Revision 1.5  2000/04/19 00:13:52  robertj
 * BeOS port changes.
 *
 * Revision 1.4  1999/09/21 00:56:29  robertj
 * Added more sound support for BeOS (thanks again Yuri!)
 *
 * Revision 1.3  1999/06/28 09:28:02  robertj
 * Portability issues, especially n BeOS (thanks Yuri!)
 *
 * Revision 1.2  1999/03/05 07:03:27  robertj
 * Some more BeOS port changes.
 *
 * Revision 1.1  1999/03/02 05:41:59  robertj
 * More BeOS changes
 *
 */

#include <ptlib.h>
#include <ptlib/unix/ptlib/beaudio.h>

PCREATE_SOUND_PLUGIN(BeOS, PSoundChannelBeOS);

/////////////// Debugging stuff ///////////////
#define TL (7)

#define PRINT(x) //do { printf(__FILE__ ":%d %s ", __LINE__, __FUNCTION__); printf x; printf("\n"); } while(0)

#define STATUS(x) // PRINT((x "=%ld", (long)dwLastError))

#define PRINTCB(x) // PRINT(x)

//#define SOUNDDETECT 1 define this for printed output of first pb/rec audio

//#define FILEDUMP 1 define this for dumping audio to wav file

// Macros and global vars for debugging

#ifdef SOUNDDETECT
#define DETECTVARS(buffer,numframes) short *detbuf=(short*)buffer; size_t detframes=numframes;
#define DETECTSOUND() \
do { \
	static bool silence=true; \
	if (silence) \
	{ \
		for (size_t i=0; i<detframes; i++) \
		{ \
			if (detbuf[i]>=255) \
			{ \
				PRINT(("SOUND DETECTED at %p",detbuf)); \
				for (size_t j=0; j<detframes && j<30; j++) \
				{ \
					char *x; \
					asprintf(&x,"%%%ds\n",(detbuf[j]>>10)+32); \
					printf(x,"."); \
					free(x); \
				} \
				silence=false; \
				break; \
			} \
		} \
	} \
} while(0)
#else
#define DETECTVARS(buffer,numframes)
#define DETECTSOUND()
#endif

#ifdef FILEDUMP
#include "beaudio/AudioFileWriter.h"
BAudioFileWriter *playwriter=NULL;
BAudioFileWriter *recwriter=NULL;
#endif

////////////////////////////////////////////////////////////////////////////////
// PSound

PSound::PSound(unsigned channels,
               unsigned samplesPerSecond,
               unsigned bitsPerSample,
               PINDEX   bufferSize,
               const BYTE * buffer)
{
	encoding = 0;
	SetFormat(channels, samplesPerSecond, bitsPerSample);

	if (buffer != NULL)
	{
		memcpy(GetPointer(bufferSize), buffer, bufferSize);
	}
}


PSound::PSound(const PFilePath & filename)
{
	encoding = 0;
	
	// Set the default format
	SetFormat(1, 8000, 16);
	
	// The format is changed if the file is succesfully loaded.
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
	// NOTE: all constructors should call this to initialize
	// the local members, especially formatInfo.
	// Do NOT call the function with any parameter set to 0!
	sampleSize=bitsPerSample;
	sampleRate=samplesPerSecond;
	numChannels = channels;

	// We don't use the encoding member (although we could probably set it to 0=PCM)
	// Let the application know it shouldn't assume anything
	encoding = 1;
	
	// The formatInfo member to us is a media_format structure.
	BOOL setsize_formatInfo=formatInfo.SetSize(sizeof(media_format));
	PAssert(setsize_formatInfo, "Unable to set size for sound info array");
	
	// Initialize the media_format struct
	// The numbers of bits that we support here are 8, 16 or 32 bits (signed),
	// results for other sizes are not defined.
	media_format &format=*(media_format*)(const BYTE *)formatInfo;

	format.type = B_MEDIA_RAW_AUDIO;
	format.u.raw_audio = media_raw_audio_format::wildcard;
	format.u.raw_audio.frame_rate=(float)sampleRate;
	format.u.raw_audio.channel_count=numChannels;
	format.u.raw_audio.format=(sampleSize / 8) & 0xF; 
	format.u.raw_audio.byte_order=B_MEDIA_HOST_ENDIAN;
	format.u.raw_audio.buffer_size=(channels * samplesPerSecond * (bitsPerSample/8))/10; // 1/10 sec buffer
}

BOOL PSound::Load(const PFilePath & filename)
{
	// format is a reference to the formatInfo member which stores info
	// about the media format. This is needed for writing the data back
	// or for playing the sound.
	media_format 	   &format=*(media_format *)(const BYTE *)formatInfo;

	// Create BEntry from file name
	BEntry entry(filename, true);
	if ((dwLastError=entry.InitCheck())!=B_OK)
	{
		STATUS("entry.InitCheck()");
		return FALSE;
	}

	// Create entry_ref from BEntry	
	entry_ref ref;
	if ((dwLastError=entry.GetRef(&ref))!=B_OK)
	{
		STATUS("entry.GetRef()");
		return FALSE;
	}

	// Create BMediaFile for read access from the entry_ref
	BMediaFile file(&ref);
	if ((dwLastError=file.InitCheck())!=B_OK)
	{
		STATUS("file.InitCheck()");
		return FALSE;
	}
	
	// Search for the first media track that can be decoded
	BMediaTrack *ptrack = NULL;
	for (int index=0; (index<file.CountTracks()) && (dwLastError==B_OK); index++)
	{
		ptrack = file.TrackAt(index);
		if (ptrack)
		{
			dwLastError = ptrack->InitCheck();
		}
		else
		{
			dwLastError = B_ERROR; //todo: change error code
		}

		if (dwLastError==B_OK)
		{
			// Get media format; we're looking for a raw audio track.
			format.type = B_MEDIA_RAW_AUDIO;
			format.u.raw_audio = media_raw_audio_format::wildcard;
			dwLastError = ptrack->DecodedFormat(&format);
		
			if ((dwLastError==B_OK) && (format.type==B_MEDIA_RAW_AUDIO))
			{
				break; // found a decodable track
			}
		}
		else
		{
			STATUS("TrackAt() failed, error");
		}
		
		// if we found a track and arrived at this point, the track we found
		// was not decodable
		if (ptrack)
		{
			dwLastError=file.ReleaseTrack(ptrack); // destroys ptrack
		}
	}
	
	// if an error occurred during track scanning, leave now
	if (dwLastError!=B_OK)
	{
		return FALSE;
	}

	// Get a reference to the raw output format
	media_raw_audio_format &rawformat = format.u.raw_audio;

	// Fill in our fields from the format		
	sampleSize    = (rawformat.format & 0xF) * 8;
	numChannels   = rawformat.channel_count;
	if (rawformat.frame_rate>0.0 && rawformat.frame_rate<=(float)0xFFFFFFFFU)
	{
		sampleRate = (unsigned)(rawformat.frame_rate);
	}
	else
	{
		// unknown or unrepresentable sample rate.
		// It's not really documented what we should do in this case but
		// it probably doesn't matter either... 
		sampleRate = 0; 
	}
	
	// Get the number of frames for the track and determine how much
	// memory we need to store the file's data
	// The multiplication might overflow for huge files but we don't
	// want to read them into memory anyway so I guess it's ok...
	int64 numframes = ptrack->CountFrames();
	int64 framesize = numChannels * (sampleSize/8);
	int64 numbytes  = numframes * framesize;
	
	// Set the size of the object's data area
	if (!SetSize(numbytes))
	{
		PRINT(("Can't set size of sound to %Ld", numbytes));
		dwLastError = B_ERROR; //todo replace by better error code
		return FALSE; // BMediaFile will destroy ptrack
	}
	
	// Read all frames into memory. NOTE: not thread safe!
	BYTE* dest = GetPointer();		// destination pointer
	int64 framecount = numframes;	// number of frames left to read
	int64 framesread;				// number of actual frames done
	while ((framecount!=0) && (dwLastError==B_OK))
	{
		framesread = framecount;
		dwLastError = ptrack->ReadFrames(dest, &framesread);
		dest += framesread * framesize;
		framecount -= framesread;
	}
	
	// return true for success
	return (dwLastError==B_OK); // BMediaFile will destroy ptrack
}


BOOL PSound::Save(const PFilePath & filename)
{
	// format is a reference to the formatInfo member which stores info
	// about the media format. This is needed for writing the data back
	// or for playing the sound.
	media_format 	   &format=*(media_format *)(const BYTE *)formatInfo;

	// Get the file type from the file name's extension; if none, use wav
	PFilePathString filetype=filename.GetType(); // e.g. ".wav"
	if (filetype=="")
	{
		filetype="wav";
	}
	else
	{
		filetype=filetype.Mid(1); // cut off the '.'
	}
	
	// Try to find the file format in BeOS's list of formats
	media_file_format mfi;
	int32 cookie=0;
	while ((dwLastError=get_next_file_format(&cookie, &mfi))==B_OK)
	{
		if (!strcasecmp(mfi.file_extension, (const char *)filetype))
		{
			break;
		}
	}
	if (dwLastError!=B_OK)
	{
		// didn't find file format
		PRINT(("Couldn't find media_file_format for \"%s\"", (const char *)filetype));
		return FALSE;
	}
	
	// Create BEntry from file name
	BEntry	entry(filename, true);
	if ((dwLastError=entry.InitCheck())!=B_OK)
	{
		STATUS("entry.InitCheck()");
		return FALSE;
	}
	
	// Create entry_ref from BEntry	
	entry_ref ref;
	if ((dwLastError=entry.GetRef(&ref))!=B_OK)
	{
		STATUS("entry.GetRef()");
		return FALSE;
	}

	// Create BMediaFile for write access from the entry_ref
	BMediaFile file(&ref, &mfi, B_MEDIA_FILE_REPLACE_MODE);
	if ((dwLastError=file.InitCheck())!=B_OK)
	{
		STATUS("file.InitCheck()");
		return FALSE;
	}
	
	// Find an encoder. The input format is the format we have stored in
	// our formatInfo member.
	cookie=0;
	media_format outformat;
	media_codec_info mci,validmci,rawmci, *pmci;
	bool found_encoder = false;
	bool found_raw_encoder = false;
	while (get_next_encoder(&cookie, &mfi, &format, &outformat, &mci)==B_OK)
	{
		found_encoder=true;
		
		if (outformat.type==B_MEDIA_RAW_AUDIO)
		{
			rawmci=mci;
			found_raw_encoder=true;
		}
		else
		{
			validmci=mci;
		}
	}
	
	// Choose an encoder:
	// If a raw-output encoder was found, use it.
	// Else, use the last found encoded-output encoder, if any.
	// This method of choosing will make sure that most file formats 
	// will get the most common encoding (PCM) whereas it's still possible
	// to choose another output format like MP3, if so dictated by the
	// file format.
	// BeOS is smart enough not to return an encoder that produces raw audio
	// for e.g. the MP3 file format, but it knows that there are many ways
	// to encode e.g. a WAV file and we don't want to put anything
	// unexpected into a WAV file, do we?
	BMediaTrack	*ptrack = NULL;
	if (found_encoder)
	{
		if (found_raw_encoder)
		{
			PRINT(("Using raw encoder"));
			pmci=&rawmci;
		}
		else
		{
			// don't use mci instead of validmci,
			// it could be unreliable after the last call to get_next_encoder
			PRINT(("Using non-raw encoder"));
			pmci=&validmci;
		}
		
		// Create a BMediaTrack in the file using the selected encoder
		ptrack = file.CreateTrack(&format, pmci);
		if (ptrack)
		{
			dwLastError = ptrack->InitCheck();
		}
		else
		{
			dwLastError = B_ERROR; //todo: change error code
		}
	}
	else
	{
		dwLastError=B_ERROR; //todo: change error code
	}

	if (dwLastError!=B_OK)
	{
		STATUS("Encoder not found or file.CreateTrack() error");
		return FALSE; // BMediaFile will destroy ptrack
	}
	
	// We're only creating one track so commit the header now
	if ((dwLastError = file.CommitHeader())!=B_OK)
	{
		STATUS("file.CommitHeader()");
		return FALSE;
	}
	
	// Determine how many frames we have to write
	// There is a small possibility of a divide by zero but this only
	// happens if the object is not properly initialized.
	PINDEX numbytes = GetSize();
	int32 framesize = numChannels * (sampleSize/8);
	int32 numframes = numbytes / framesize; // divide by zero possibility ignored.
		
	if ((dwLastError=ptrack->WriteFrames((const BYTE *)*this, numframes))!=B_OK)
	{
		STATUS("ptrack->WriteFrames()");
		return FALSE; // BMediaFile will destroy ptrack
	}
	
	return (file.CloseFile()==B_OK); // BMediaFile will destroy ptrack
}

BOOL PSound::Play()
{
	PSoundChannelBeOS player(PSoundChannelBeOS::GetDefaultDevice(PSoundChannelBeOS::Player), PSoundChannelBeOS::Player, numChannels, sampleRate, sampleSize);
	
	if (!player.IsOpen())
	{
		PRINT(("PSoundChannelBeOS constructor failed to open"));
		return FALSE;
	}
	
	return player.PlaySound(*this, TRUE);
}

BOOL PSound::PlayFile(const PFilePath & file, BOOL wait)
{
	entry_ref 			ref;
	status_t			err; // can't use dwLastError because this function is static

	// using pointers for these objects so that we don't have to
	// construct them here but can nevertheless use the if(ok)'s
	BEntry 			   *pentry = NULL;
	
	{
		// Create BEntry from file name
		pentry = new BEntry(file, true);
		err = pentry->InitCheck();
	}

	if (err==B_OK)
	{
		// Create entry_ref from BEntry	
		err = pentry->GetRef(&ref);
	}
	
	if (err==B_OK)
	{
		// Play the sound. Return value is a handle or a negative value for errors
		// Errors in BeOS are always negative values
		err=play_sound(&ref, true, !wait, wait);
		if (err>=0)
		{
			err=B_OK;
		}
	}

	return (err==B_OK);
}

void PSound::Beep()
{
	::beep();
}

////////////////////////////////////////////////////////////////////////////////
// CircularBuffer

class Guard
{
private:
	sem_id mSem;
public:
	Guard(sem_id sem) { acquire_sem(mSem=sem); }
	~Guard() { release_sem(mSem); }
};

/*
	This class represents a circular FIFO buffer.
	The buffer has a head and a tail that chase each other.
	The data is added to the buffer at the tail side by using Fill.
	The data from the buffer can be read starting at the head side using
	Drain.
	It is possible to use two threads to fill and drain the buffer but
	there should not be more than 2 threads doing draining and filling.
	Resetting (flushing) or destroying from a third thread is allowed;
	do make sure that any threads that operate on buffer data are stopped
	before destroying a buffer.
	Normally, filling and draining operations block the thread as short as
	possible (i.e. only when the other thread needs to update the head and
	tail pointers etc). If the filling thread tries to put data into a full
	or almost full buffer, it just returns after filling as much data as
	it can, and if the draining thread tries to get more data out than is
	in the buffer, it will simply return with the data that is there.
	In order to move all the data from an external buffer into an object
	of this class, the caller would have to call Fill repeatedly until
	all the data has been processed (similarly it would have to call Drain
	until it receives sufficient data). But if the application has nothing
	else to do in the mean time, this constitutes a Busy Waiting loop
	on either the filling or draining side of the FIFO buffer that slurps
	up as much CPU time as possible.
	To improve this behaviour, it's possible to specify a threshold value
	that is used to change the state to FullEnough and EmptyEnough. By using
	these states (instead of Full and Empty), one thread can block until the
	other thread has determined that there is enough data or enough room for
	data.
*/
class CircularBuffer
{
public:
	// Internal state for the buffer
	// Note the nifty bit patterns for comparing the current state
	// with a desired state
	typedef enum
	{							// Headspace		Tailspace
		Empty			=1,		// 0                size
		Filled			=2,		// 0<h<size			0<t<size
		NotFull			=3,		// 0<=h<size		0<t<=size	(for comparing)
		Full			=4,		// size				0
		NotEmpty		=6,		// 0<h<=size		0<=t<size	(for comparing)
		Flushed     	=8,		// 								(extra signal to threads waiting on full)
		FullEnough		=16,	// h>=drainthreshold
		EmptyEnough		=32,	//                  t>=fillthreshold
	} State;

protected:
	friend class ResamplingBuffer; // needed for one of their constructors

	BYTE		   *mBuffer;			// the buffer
	PINDEX			mSize;				// size of the buffer in bytes
	
	volatile PINDEX	mHead;				// index where to start reading
	volatile PINDEX	mTail;				// index where to start writing
	volatile PINDEX	mHeadRoom;			// consecutive space from head to end-of-buffer or tail
	volatile PINDEX	mTailRoom;			// consecutive space from tail to end-of-buffer or head
	volatile PINDEX mSizeUsed;			// total bytes in use
	volatile PINDEX mFillThreshold;		// see above
	volatile PINDEX mDrainThreshold;	// see above

	volatile State	mState;				// current state of the buffer

	sem_id			mSemInUse;			// used to guard data integrity
	sem_id			mSemStateChange;	// used to wait for state changes

protected:
	// Check if the state changed. Private because it's not guarded by semaphore
	void UpdateState(void)
	{
		// Determine current state
		State newstate;
		
		if (mSizeUsed==mSize)
		{
			PRINTCB(("State is FULL"));
			newstate=Full;
		}
		else if (mSizeUsed==0)
		{
			PRINTCB(("State is EMPTY"));
			newstate=Empty;
		}
		else
		{
			PRINTCB(("State is FILLED"));
			newstate=Filled;
		}
		
		// Check thresholds
		if (mSize-mSizeUsed>=mFillThreshold)
		{
			PRINTCB(("...and EMPTYENOUGH"));
			newstate=(State)(newstate | EmptyEnough);
		}
		if (mSizeUsed>=mDrainThreshold)
		{
			PRINTCB(("...and FULLENOUGH"));
			newstate=(State)(newstate | FullEnough);
		}
		
		// Check if the state changed
		if (newstate!=mState)
		{
			PRINTCB(("Updating state from %X to %X", mState, newstate));
			
			// Set the new state
			mState=newstate;
			
			// Signal state change
			release_sem(mSemStateChange);
		}
	}

	virtual size_t Write(
		BYTE *dest,					// destination
		const BYTE **extbuf,		// source, to be updated
		size_t size,				// space in destination
		size_t *extsize)			// data in source, to be updated
	{
		// This function is called to put data into the buffer
		size_t todo=MIN(size, *extsize);
		memcpy(dest, *extbuf, todo);
		*extbuf   +=todo; // The external pointer moves forward...
		*extsize  -=todo; // ... so the remaining size decreases
		return todo;
	}
	
	virtual size_t Read(
		BYTE **extbuf,				// destination, to be updated
		const BYTE *src,			// source
		size_t *extsize,			// space in destination, to be updated
		size_t size)				// data in source
	{
		// This function is called to read data out of the buffer
		size_t todo=MIN(size, *extsize);
		memcpy(*extbuf, src, todo);
		*extbuf   +=todo; // The external pointer moves forward...
		*extsize  -=todo; // ... so the remaining size decreases
		return todo;
	}
	
public:
	// Reset buffer so that it can be filled again
	void Reset(void)
	{
		Guard _(mSemInUse); // guard data integrity

		mHead=mHeadRoom=mTail=mSizeUsed=0;
		mTailRoom=GetSize();
		mState=(State)(Flushed|Empty|EmptyEnough);
	}

	// Constructor
	CircularBuffer(
		PINDEX size,
		PINDEX fillthreshold = 0,
		PINDEX drainthreshold = 0) 
	: mFillThreshold(fillthreshold), mDrainThreshold(drainthreshold), mState(Empty)
	{
		PAssert(size!=0, "Attempting to create a buffer with size 0");
		
		mSemInUse=create_sem(1, "mSemInUse");
		mSemStateChange=create_sem(0, "mSemStateChange");
		
		PAssert(mSemInUse>=0 && mSemStateChange>=0, "Unable to create semaphores");
		
		mBuffer=new BYTE[(mSize=size)];
		
		Reset();
	}

	// Destructor
	virtual ~CircularBuffer()
	{
	  // make sure the in-use semaphore is free and stays free
	  while (acquire_sem_etc(mSemInUse,1,B_RELATIVE_TIMEOUT,0)==B_WOULD_BLOCK)
	  {
	    // nothing to do, just busy-wait
	  }

	  delete_sem(mSemInUse);
	
	  delete_sem(mSemStateChange);
	
	  Reset();
		
	  if(mBuffer)
	    delete[] mBuffer;
	}

	// Check if buffer is empty
	bool IsEmpty() { return (mState==Empty); }
	
	// Check if buffer is full
	bool IsFull() { return (mState==Full); }
	
	// Get the size of the buffer
	PINDEX GetSize(void) { return mSize; }

	// Wait asynchronously for a buffer state or one of a number of states
	void WaitForState(State state)
	{
		PRINTCB(("Waiting for state %X, current state=%X this=%p", state, mState, this));
		// reset the Flushed bit so it only stops the loop if the buffer
		// is flushed DURING an operation
		{
			Guard _(mSemInUse);
			mState=(State)(mState & ~Flushed);
		}
		for(;;)
		{
			if ((mState & (state|Flushed))!=0) // bit patterns allowed
			{
				PRINTCB(("Detected state %X, wanted %X, returning", mState, state));
				return;
			}
			PRINTCB(("Waiting for %X; headroom=%u tailroom=%u this=%p",state,mHeadRoom,mTailRoom,this));
			// To prevent a race condition here in case the state
			// gets changed just after the GetState call, the next
			// semaphore call has a timeout.
			acquire_sem_etc(mSemStateChange,1,B_RELATIVE_TIMEOUT,1000000);
		}
	}
	
	// Fill buffer with data.
	void Fill(const BYTE **extbuf, size_t *extsize)
	{
		PRINTCB(("start: head %d tail %d headroom %d tailroom %d extsize %d buffer %p this %p", mHead, mTail, mHeadRoom, mTailRoom, *extsize, mBuffer, this));

		// Make a local copy of the queue.
		// This is ok because there is only one filler thread and
		// one drainer thread. The drainer is not going to make the
		// free area for the filler any smaller and the filler is not
		// going to overwrite the drainer's data if we do this.
		// This way we can keep the semaphore busy as short as possible.
		PINDEX lTail;
		PINDEX lTailRoom;
		PINDEX lHead; // read only
		{
			Guard _(mSemInUse); // guard data integrity
			lTail=mTail;
			lTailRoom=mTailRoom;
			lHead=mHead;
		}
		
		bool needhousekeeping=false;
		PINDEX totaldone=0;
		
		while (*extsize!=0 && lTailRoom!=0 && totaldone<mSize)
		{
			needhousekeeping=true;
			
			PINDEX done=Write(
				mBuffer+lTail,
				extbuf,
			    lTailRoom,
				extsize);
			
			totaldone +=done;
			
			lTail     +=done; // The tail moves forward...
			lTailRoom -=done; // ... so there will be less room at the tail

			// Check if we should wrap around
			if (lTail==mSize)
			{
				lTail=0;
				lTailRoom=lHead;
			}
		}

		if (needhousekeeping)
		{
			Guard _(mSemInUse);
			
			// Copy the local values back
			mTail=lTail;
			mTailRoom=lTailRoom;
			mSizeUsed+=totaldone;
			
			// Recalculate headroom
			if (mTail>mHead)
			{
				mHeadRoom=mTail-mHead;
			}
			else
			{
				mHeadRoom=mSize-mHead;
			}

			// Check if we need to change the state
			UpdateState();

			PRINTCB(("  end: head %d tail %d headroom %d tailroom %d extsize %d", mHead, mTail, mHeadRoom, mTailRoom, *extsize));
		}
	}

	// Empty data out of buffer
	void Drain(BYTE **extbuf, size_t *extsize)
	{
		PTRACE(7, "Drain: head " << mHead 
	    << " tail " << mTail 
	    << " headroom " << mHeadRoom 
	    << " tailroom " << mTailRoom 
	    << " extsize " << *extsize
	    << " buffer " << mBuffer
	    << " this " << this);

		// Make a local copy of the queue.
		// This is ok because there is only one filler thread and
		// one drainer thread. The drainer is not going to make the
		// free area for the filler any smaller and the filler is not
		// going to overwrite the drainer's data if we do this.
		// This way we can keep the semaphore busy as short as possible.
		PINDEX lHead;
		PINDEX lHeadRoom;
		PINDEX lTail; // read only
		{
			Guard _(mSemInUse); // guard data integrity
			lHead=mHead;
			lHeadRoom=mHeadRoom;
			lTail=mTail;
		}

		bool needhousekeeping=false;
		PINDEX totaldone=0;
		
		while (*extsize!=0 && lHeadRoom!=0 && totaldone<mSize)
		{
			needhousekeeping=true;
			
			size_t done=Read(
				extbuf,
				mBuffer+lHead,
				extsize,
				lHeadRoom);
				
			totaldone +=done;
			
			lHead     +=done; // The head moves forward...
			lHeadRoom -=done; // ... so there will be less room at the head
		
			// Check if we should wrap around
			if (lHead==mSize)
			{
				lHead=0;
				lHeadRoom=mTail;
			}
		}

		if (needhousekeeping)
		{
			Guard _(mSemInUse);
			
			// Copy the local values back
			mHead=lHead;
			mHeadRoom=lHeadRoom;
			mSizeUsed-=totaldone;
			
			// Recalculate tailroom
			if (mHead>mTail)
			{
				mTailRoom=mHead-mTail;
			}
			else
			{
				mTailRoom=GetSize()-mTail;
			}
			
			// Check if we need to change the state
			UpdateState();

			PRINTCB(("  end: head %d tail %d headroom %d tailroom %d extsize %d", mHead, mTail, mHeadRoom, mTailRoom, *extsize));
		}
	}
};

////////////////////////////////////////////////////////////////////////////////

class ResamplingBuffer : public CircularBuffer
{
protected:
	Resampler	   *mResampler;
	
protected:
	virtual size_t Write(
		BYTE *dest,					// destination
		const BYTE **extbuf,		// source, to be updated
		size_t size,				// space in destination
		size_t *extsize)			// data in source, to be updated
	{
		size_t todo=*extsize/mResampler->InFrameSize();
		size_t done=mResampler->InFrames(
			(const short **)extbuf,
			(short **)&dest,
			&todo,
			size/mResampler->OutFrameSize());
		done*=mResampler->OutFrameSize();
		*extsize=todo*mResampler->InFrameSize();
		
		return done;
	}
	
public:
	void SetResampler(Resampler *resampler)
	{
		Guard _(mSemInUse); // guard data integrity
		
		mResampler=resampler;
	}

	ResamplingBuffer(
		Resampler *resampler,
		PINDEX size,
		PINDEX fillthreshold=0,
		PINDEX drainthreshold=0) 
		: CircularBuffer(size, fillthreshold, drainthreshold), mResampler(NULL)
	{
		SetResampler(resampler);
	}

	ResamplingBuffer(
		Resampler *resampler,
		CircularBuffer *other) 
	  : CircularBuffer(other->mSize, other->mFillThreshold, other->mDrainThreshold), mResampler(NULL)
	{
		SetResampler(resampler);
	}
};

////////////////////////////////////////////////////////////////////////////////
static void PlayBuffer(void *cookie, void *buffer, size_t size, const media_raw_audio_format &format)
{
	// This function is called by the BSoundPlayer object whenever it needs some more
	// data to play.
	DETECTVARS(buffer, size/2)

	((CircularBuffer *)cookie)->Drain((BYTE **)&buffer, &size);
	
	DETECTSOUND();
}

static void RecordBuffer(void *cookie, const void *buffer, size_t size, const media_header &header)
{
	// This function is called by the BMediaRecorder object whenever it has a buffer
	// with recorded data ready.
	DETECTVARS(buffer, size/2)
	DETECTSOUND();

	((CircularBuffer *)cookie)->Fill((const BYTE **)&buffer, &size);
}

////////////////////////////////////////////////////////////////////////////////
// PSoundChannelBeOS

// This defines the number of times we would like to be called per second
// to play/record data
#define PLAYRECFREQ 20 

// Macro to let the default buffer size correspond neatly with the
// setting we put into the format.
#define DEFAULT_BUFSIZE(channels, rate, bits) 480
//((channels*rate*(bits/8))/PLAYRECFREQ)

PSoundChannelBeOS::PSoundChannelBeOS() :
	mRecorder(NULL),
	mPlayer(NULL),
	mBuffer(NULL),
	mNumBuffers(1),
	mResampler(NULL)
{
	PRINT(("default constructor"));

	InternalSetBuffers(DEFAULT_BUFSIZE(1, 8000, 16),DEFAULT_BUFSIZE(1, 8000, 16)/2);
	SetFormat(1, 8000, 16);
	
	// Nothing else to do here. Notice that the channel is not open for
	// playing/recording yet.
}


PSoundChannelBeOS::PSoundChannelBeOS(const PString & dev,
                             Directions dir,
                             unsigned numChannels,
                             unsigned sampleRate,
                             unsigned bitsPerSample) :
	mRecorder(NULL),
	mPlayer(NULL),
	mBuffer(NULL),
	mNumBuffers(1),
	mResampler(NULL)
{
	PRINT(("constructor %s %u %u %u", dir==Player ? "Player" : "Recorder", numChannels, sampleRate, bitsPerSample));
	
	InternalSetBuffers(DEFAULT_BUFSIZE(numChannels, sampleRate, bitsPerSample), DEFAULT_BUFSIZE(numChannels, sampleRate, bitsPerSample)/2);
	Open(dev, dir, numChannels, sampleRate, bitsPerSample);
	// ignore result; user will need to find out whether this succeeds using IsOpen
}


PSoundChannelBeOS::~PSoundChannelBeOS()
{
	PRINT((""));
	
	Close(); // destroys player and recorder
	InternalSetBuffers(0,0); // destroys buffer
}

static const PStringArray GetRecorderDevicesList(BMediaRecorder *Recorder)
{
	// Array to hold the list.
	PStringArray devlist;
    BMediaRecorder* bRecorder = NULL;
    
    if(Recorder != NULL)
      bRecorder = Recorder;

#ifdef MEDIA_KIT_UPDATE
    BMediaRecorder localRecorder("GetRecorderDevicesList");    
	bool result=true;
	status_t status;
	
	{
		if(bRecorder == NULL)
		{
			bRecorder = &localRecorder;
		}
	
		if (bRecorder == NULL || bRecorder->InitCheck()!=B_OK)
		{
			PRINT(("Error constructing recorder to fetch device names"));
			result=false;
		}
	}
	
	if (result)
	{
		media_format format;
		format.type = B_MEDIA_RAW_AUDIO;
		format.u.raw_audio=media_raw_audio_format::wildcard;
		
		// The resampler can only handle 16-bit audio
		format.u.raw_audio.format=media_raw_audio_format::B_AUDIO_SHORT;

		// Let the media recorder determine which sources are available		
		if ((status = bRecorder->FetchSources(format, false))!=B_OK)
		{
			PRINT(("Couldn't fetch BMediaRecorder sources; status=%d", status));
			result=false;
		}
	}
	
	if (result)
	{
		// Fetch the names of all output devices
		media_format format;
		BString outname;
		for (int i=0; i< bRecorder->CountSources(); i++)
		{
			if ((status = bRecorder->GetSourceAt(i, &outname, &format))==B_OK)
			{
				PRINT(("Device found: %s", outname.String()));
				devlist[i] = PString(outname.String());
			}
			else
			{
				PRINT(("error %d retrieving data for device %d", status, i));
				result=false;
			}
		}
	}

	if (!result)
	{
		devlist.RemoveAll();
	}
	
	return devlist;
#else
    // Media Kit is the only device
    devlist[0] = "MediaKit";
	return devlist;
#endif
}

PStringArray PSoundChannelBeOS::GetDeviceNames(Directions dir)
{
	if (dir==Recorder)
	{
		return GetRecorderDevicesList(NULL);
	}
	else
	{
		// not supported yet
		return PStringArray("MediaKit");
	}
}

PString PSoundChannelBeOS::GetDefaultDevice(Directions dir)
{
	if (dir==Recorder)
	{
		const PStringArray &devlist = GetRecorderDevicesList(NULL);
		
		if (devlist.GetSize()!=0)
		{
			return devlist[0];
		}
		else
		{
			return PString("MediaKit");
		}
	}
	else
	{
		// not supported yet
		return PString("MediaKit");
	}
}

BOOL PSoundChannelBeOS::OpenPlayer(void)
{
	// We're using cascaded "if result"s here for clarity
	BOOL result = TRUE;

#ifdef FILEDUMP
	media_format format;
	format.type=B_MEDIA_RAW_AUDIO;
	memcpy(&format.u.raw_audio, &mFormat, sizeof(mFormat));
			
	delete playwriter;
	playwriter=new BAudioFileWriter("play.wav", format, 441000);
#endif			
	
	// Must have a buffer
	if (!mBuffer)
	{
		result = FALSE;
		PRINT(("Trying to open as player without setting buffers first"));
	}
	
	if (result)		
	{
		// Create the player
		//was: mPlayer=new BSoundPlayer(&mFormat, NULL, PlayBuffer, NULL, mBuffer);
	
		mPlayer = new BSoundPlayer(
				&mFormat, 
				NULL,
				PlayBuffer,
				NULL,
				mBuffer);
				
		if ((mPlayer == NULL) || (mPlayer->InitCheck() != B_OK))
		{
			result = FALSE;
			PRINT(("Couldn't construct player"));
		}
	}
	
	if (result)
	{
		// Start the player
		if (mPlayer->Start() != B_OK)
		{
			result = FALSE;
			PRINT(("Couldn't start the player"));
		}
	}
	
	if (result)
	{
		// Enable the fetching of data by PlayBuffer
		mPlayer->SetHasData(true);
	}

	PRINT(("Returning %s", result?"success":"failure"));
	return result;
}

BOOL PSoundChannelBeOS::OpenRecorder(const PString &dev)
{
	// We're using cascaded "if result"s here for clarity
	BOOL result=TRUE;

	{
		if (!mBuffer)
		{
			result=FALSE;
			PRINT(("Trying to open as recorder without setting buffers first"));
		}
	}
	
	if (result)			
	{
		// Create the recorder
		mRecorder=new BMediaRecorder("PWLIB PSoundChannel recorder");
	
		if ((mRecorder==NULL) || (mRecorder->InitCheck()!=B_OK))
		{
			result=FALSE;
			PRINT(("Couldn't construct recorder"));
		}
	}

#ifdef MEDIA_KIT_UPDATE
	int32 sourceindex;
	if (result)
	{
		// Find the specified device in the list of input devices
		PINDEX x=GetRecorderDevicesList(mRecorder).GetStringsIndex(dev);
		if (x==P_MAX_INDEX)
		{
			result=FALSE;
			PRINT(("Couldn't find device %s in the list",(const char *)dev));
		}
		else
		{
			sourceindex=(int32)x;
		}
	}
	
#ifdef _DEBUG	
	if (result)
	{
		// Get information for the device
		BString outname;
		media_format xformat;
		status_t err;

		if ((err=mRecorder->GetSourceAt(sourceindex, &outname, &xformat))==B_OK)
		{
			PRINT(("%s", outname.String()));
			PRINT(("    type %d", (int)xformat.type));
			PRINT(("    AudioFormat 0x%X", (int)xformat.AudioFormat()));
			PRINT(("    u.raw_audio:"));
			PRINT(("        frame_rate: %f", xformat.u.raw_audio.frame_rate));
			PRINT(("        channel_count: %d", xformat.u.raw_audio.channel_count));
			PRINT(("        byte_order: %d", xformat.u.raw_audio.byte_order));
			PRINT(("        buffer_size: %d", xformat.u.raw_audio.buffer_size));
		}
		else
		{
			result=FALSE;
			PRINT(("couldn't get details for source %d: err=0x%X",sourceindex,err));
		}
	}
#endif

	if (result)
	{
		// Try to connect to the source
		if (mRecorder->ConnectSourceAt(sourceindex)!=B_OK)
		{
			result=FALSE;
			PRINT(("Couldn't connect BMediaRecorder to source"));
		}
	}

#else
	if (result)
	{
		// Connect the recorder to the default input device
		media_format format;
		format.type=B_MEDIA_RAW_AUDIO;
		format.u.raw_audio=media_raw_audio_format::wildcard;
		// The resampler can only handle 16-bit audio
		format.u.raw_audio.format=media_raw_audio_format::B_AUDIO_SHORT;
		if (mRecorder->Connect(format,0)!=B_OK)
		{
			result=FALSE;
			PRINT(("couldn't connect the recorder to the default source"));
		}
	}
#endif

	if (result)
	{
		// Create resampler
		media_format format=mRecorder->Format();

		delete mResampler;
		mResampler=new Resampler(
			format.u.raw_audio.frame_rate,
			mFormat.frame_rate,
			format.u.raw_audio.channel_count,
			mFormat.channel_count,
			0,
			2);

#ifdef FILEDUMP
		{
			media_format format;
			format.type=B_MEDIA_RAW_AUDIO;
			memcpy(&format.u.raw_audio, &mFormat, sizeof(mFormat));
			
			delete recwriter;
			recwriter=new BAudioFileWriter("record.wav", format);
		}
#endif

		// If the current buffer is not a resamplin buffer, re-create it 
		ResamplingBuffer *buf=dynamic_cast<ResamplingBuffer*>(mBuffer);
		
		if (buf==NULL)
		{
			PRINT(("re-creating buffer"));
			
			CircularBuffer *old=mBuffer;
			mBuffer=new ResamplingBuffer(mResampler, old);
			delete old;
		}
		else
		{
			buf->SetResampler(mResampler);
		}
	}
	
	if (result)
	{
		// Set the hook function to our data processing function
		PRINT(("Setting buffer hook, cookie=%p",mBuffer));
		if (mRecorder->SetBufferHook(RecordBuffer, mBuffer)!=B_OK)
		{
			result=FALSE;
			PRINT(("Couldn't set buffer hook on BMediaRecorder"));
		}
	}

	// If something went wrong, delete the recorder.
	if (!result)
	{
		if (mRecorder)
		{
			delete mRecorder;
			mRecorder=NULL;
		}
	}
	
	return result;		
}

BOOL PSoundChannelBeOS::Open(const PString & dev,
                         Directions dir,
                         unsigned numChannels,
                         unsigned sampleRate,
                         unsigned bitsPerSample)
{
	// We're using cascaded "if result"s here for clarity
	BOOL result = TRUE;
	PRINT(("%s %u %u %u", dir==Player?"Player":"Recorder", numChannels, sampleRate, bitsPerSample));
	
	// Close the channel first, just in case	
        Close();

	// Initialize the format struct, necessary to create player or recorder	
	if (!SetFormat(numChannels, sampleRate, bitsPerSample))
	{
	  result = FALSE;
	  PRINT(("Couldn't set format"));
	}

	if (result)
	{
		switch (dir)
		{
		case Player:
		        PRINT(("... trying to open player"));
                 	result=OpenPlayer();
			break;
			
		case Recorder:
                        PRINT(("...trying to open recorder"));
			result=OpenRecorder(dev);
			break;

		default:
			PRINT(("Unknown direction parameter"));
			result=FALSE;
		}
	}

	if (!result)
	{
		// If anything went wrong, clean up
		PRINT(("... can't open, cleaning up"));
                Close();
	}

        ::snooze(1*1000*1000);
	
        PRINT(("Returning %s", result?"success":"failure"));
   	return result;
}

BOOL PSoundChannelBeOS::Abort()
{
	return FALSE;
}


BOOL PSoundChannelBeOS::SetFormat(unsigned numChannels,
                              unsigned sampleRate,
                              unsigned bitsPerSample)
{
	PRINT(("%u %u %u", numChannels, sampleRate, bitsPerSample));
	
	// NOTE: all constructors should call this to initialize
	// the local members
	// Do NOT call the function with any parameter set to 0!
	
	// The function only fails if the channel is open.
	// This is because the player or recorder needs to be re-created when the
	// format changes.
	if (IsOpen())
	{
		PRINT(("Not allowed to set format on open channel"));
		return FALSE;
	}
	
	// Initialize the format struct
	// The numbers of bits that we support here are 8, 16 or 32 bits (signed),
	// results for other sizes are not defined.
	mFormat = media_raw_audio_format::wildcard;
	mFormat.frame_rate=(float)sampleRate;
	mFormat.channel_count=numChannels;
	mFormat.format=(bitsPerSample / 8) & 0xF; 
	mFormat.byte_order=B_HOST_IS_BENDIAN ? B_MEDIA_BIG_ENDIAN : B_MEDIA_LITTLE_ENDIAN;
	mFormat.buffer_size=DEFAULT_BUFSIZE(numChannels, sampleRate, bitsPerSample);

	return TRUE;
}


unsigned PSoundChannelBeOS::GetChannels() const
{
	return mFormat.channel_count;
}


unsigned PSoundChannelBeOS::GetSampleRate() const
{
	return (unsigned)mFormat.frame_rate;
}


unsigned PSoundChannelBeOS::GetSampleSize() const
{
	return (mFormat.format & 0xF)*8; // return number of BITS
}


BOOL PSoundChannelBeOS::Read(void *buf, PINDEX len)
{
    PINDEX bufSize = len;

	// Can only read from a recorder
	if (mRecorder!=NULL)
	{
		// A Read starts the recording, if it's not running already
		if (!mRecorder->IsRunning())
		{
			mRecorder->Start();
		}

		// Wait until there's a buffer recorded
		mBuffer->WaitForState(CircularBuffer::NotEmpty);

#ifdef FILEDUMP
		void *dumpbuf=buf;
		size_t dumpsize=len;
#endif
        lastReadCount = 0;

   		while(lastReadCount < bufSize)
		{
          len = bufSize - lastReadCount; 
          if(len <= 0)
            break;

		  // Get data from the buffer
		  mBuffer->Drain((BYTE**)&buf, (size_t*) &len);

		  lastReadCount += len;
		}
		
#ifdef FILEDUMP
		if (recwriter)
		{
			recwriter->writewavfile(dumpbuf, dumpsize);
		}
#endif

		return TRUE;
	}
	return FALSE;
}

BOOL PSoundChannelBeOS::Write(const void *buf, PINDEX len)
{
	// can only write to a player
  	if (mPlayer!=NULL)
  	{
		// Wait until there is space
		mBuffer->WaitForState(CircularBuffer::EmptyEnough);

  		// This function needs to update the last write count
  		// Store len before it gets modified
		lastWriteCount=len;

#ifdef FILEDUMP
		if (playwriter)
		{
			playwriter->writewavfile(buf,len);
		}
#endif
	
		// Store data into the buffer
		mBuffer->Fill((const BYTE **)&buf, (size_t*) &len);

		// Update last write count
		lastWriteCount-=len;
		
  		return TRUE;
  	}
  	
  	return FALSE;
}


BOOL PSoundChannelBeOS::Close()
{
	PRINT((""));
	
	// Flush the buffer first
	Abort();

	// Stop the player
	if ((mPlayer!=NULL) && (mPlayer->InitCheck()==B_OK))
	{
		mPlayer->Stop();

#ifdef FILEDUMP
		delete playwriter;
		playwriter=NULL;
#endif		
	}

        if(mPlayer)
        {
	      // Destroy the player
	  	  delete mPlayer;
	      mPlayer=NULL; // make sure that another Close won't crash the system
        }

	// Stop the recorder
	if ((mRecorder!=NULL) && (mRecorder->InitCheck()==B_OK))
	{
		mRecorder->Stop(); // Not really necessary
		mRecorder->Disconnect();
		
#ifdef FILEDUMP
		delete recwriter;
		recwriter=NULL;
#endif
	}
	
     if(mRecorder)
     {
	   // Destroy the recorder
	   delete mRecorder;
	   mRecorder=NULL; // make sure that another Close won't crash the system
	 }

	return TRUE;
}


BOOL PSoundChannelBeOS::SetBuffers(PINDEX size, PINDEX count)
{
      return InternalSetBuffers(size*(mNumBuffers=count),size);
}


BOOL PSoundChannelBeOS::InternalSetBuffers(PINDEX size, PINDEX threshold)
{
  if (mPlayer)
  {
    mPlayer->SetHasData(false);
  }
  else if (mRecorder)
  {
    mRecorder->Stop();
  }

  // Delete the current buffer
  if(mBuffer != NULL)
  {
    delete mBuffer;
    mBuffer = NULL;
  }
	
  // Create the new buffer
  if (size != 0)
  {
    if (mRecorder)
	{
      if (!mResampler)
      {
        PTRACE(TL, "Creating default resampler");	
        mResampler = new Resampler(1.0,1.0,1,1,0,1);
      }

       PTRACE(TL, "Creating resampling buffer, size " << size);
       mBuffer = new ResamplingBuffer(mResampler, size, threshold, threshold);
       
       // In case we use resampler, size must be set to resampled buffer size
        
    }
    else 
	{
      PTRACE(TL, "Creating playback buffer, size " << size);
      mBuffer = new CircularBuffer(size, threshold, threshold);
    }

    // If we have a player, set the cookie again and restart it
    if (mPlayer)
    {
      mPlayer->SetCookie(mBuffer);
      PTRACE(TL, "Tried to set player buffer cookie");
      mPlayer->SetHasData(true);
      PTRACE(TL, "Tried to set player has data");
    }
		
    // If we have a recorder, set the cookie again
    // Note that the recorder is not restarted, even if it was running.
    // It's not a good idea for the program to change the buffers during
    // recording anyway because it would at least lose some data.
    if (mRecorder)
    {
      if(B_OK != mRecorder->SetBufferHook(RecordBuffer, mBuffer))
      PTRACE(TL, "Can't set recorder buffer hook");
    }
		
    return TRUE;
  }

  if (IsOpen())
  {
    PTRACE(TL, "Can't continue without buffers - closing channel");
    Close(); // should give errors on subsequent read/writes
  }

  mBuffer = NULL;

  return FALSE;
}


BOOL PSoundChannelBeOS::GetBuffers(PINDEX &size, PINDEX &count)
{
	if (mBuffer)
	{
		size=mBuffer->GetSize();
		count=mNumBuffers;
		return TRUE;
	}
	
	return FALSE;
}


BOOL PSoundChannelBeOS::PlaySound(const PSound &sound, BOOL wait)
{
	PRINT(("wait=%s", wait?"true":"false"));
	
	if (mPlayer==NULL)
	{
		PRINT(("Playing a sound on a closed (or recording) PSoundChannelBeOS"));
		return FALSE;
	}

#ifdef FILEDUMP
	playwriter->writewavfile((void *)(const BYTE*)sound, sound.GetSize());
#endif

	// create a local buffer that references the PSound
	// NOTE: no conversion between the PSound's format and the
	// PSoundChannelBeOS's format is done.
	const BYTE *buf=(const BYTE *)sound;
	PINDEX size=sound.GetSize();
	
	// Play the sound by doing successive Writes until the sound is done.
	// Note that write will return when either the buffer is full or the
	// given data is written. We want to return after the entire sound
	// has been buffered. So we repeatedly call Write until there is
	// no data left
	while (size!=0)
	{
		// Wait until there is space
		mBuffer->WaitForState(CircularBuffer::EmptyEnough);
		
		// Write the data
		mBuffer->Fill(&buf, (size_t*) &size);
	}
	
	// Wait until the sound is finished, if requested
	if (wait)
	{
		PRINT(("Waiting for sound"));
		mBuffer->WaitForState(CircularBuffer::Empty);
	}

	return TRUE;
}


BOOL PSoundChannelBeOS::PlayFile(const PFilePath &file, BOOL wait)
{
	entry_ref 			ref;
	status_t			err;

	// using pointers for these objects so that we don't have to
	// construct them here but can nevertheless use the if(ok)'s
	BEntry 			   *pentry = NULL;
	
	{
		// Create BEntry from file name
		pentry = new BEntry(file, true);
		err = pentry->InitCheck();
	}

	if (err==B_OK)
	{
		// Create entry_ref from BEntry	
		err = pentry->GetRef(&ref);
	}
	
	if (err==B_OK)
	{
		// Play the sound. Return value is a handle or a negative value for errors
		// Errors in BeOS are always negative values
		err=play_sound(&ref, true, !wait, wait);
		if (err>=0)
		{
			err=B_OK;
		}
	}

	return (err==B_OK);
}


BOOL PSoundChannelBeOS::HasPlayCompleted()
{
	if (mPlayer!=NULL)
	{
		return mBuffer->IsEmpty();
	}
	
	return FALSE;
}


BOOL PSoundChannelBeOS::WaitForPlayCompletion()
{
	if (mPlayer!=NULL)
	{
		mBuffer->WaitForState(CircularBuffer::Empty);
	}

	return TRUE;
}


BOOL PSoundChannelBeOS::RecordSound(PSound &sound)
{
	PRINT((""));
	
	if (mRecorder==NULL) 
	{
		PRINT(("Recording a sound on a closed (or playing) PSoundChannelBeOS"));
		return FALSE;
	}

	// Flush the buffer first
	Abort();
	
	// Start recording
	if (mRecorder->Start()!=B_OK)
	{
		PRINT(("BMediaRecorder::Start() returned error"));
		return FALSE;
	}
	
	// Wait until buffer is filled
	mBuffer->WaitForState(CircularBuffer::Full);
	PRINT(("Buffer full: size=%lu",mBuffer->GetSize()));

	// Stop the recorder
	if (mRecorder->Stop()!=B_OK)
	{
		PRINT(("Uh-oh, recorder is unstoppable!"));
		//return FALSE;
	}
	
	// Set the sound's format to ours
	sound.SetFormat(GetChannels(), GetSampleRate(), GetSampleSize());
	
	// Resize the sound and set up local buffer references
	PINDEX size=mBuffer->GetSize();
	BYTE *buf=sound.GetPointer(size);

#ifdef FILEDUMP
	void *dumpbuf=buf;
	size_t dumpsize=size;
#endif

	// Read the data		
	mBuffer->Drain(&buf, (size_t*) &size);

#ifdef FILEDUMP
	recwriter->writewavfile(dumpbuf, dumpsize);
#endif

	PRINT(("Recording succesful"));
	return TRUE;
}


BOOL PSoundChannelBeOS::RecordFile(const PFilePath & filename)
{
	// Not implemented for now
	return FALSE;
}


BOOL PSoundChannelBeOS::StartRecording()
{
	if (mRecorder==NULL) 
	{
		PRINT(("Recording to a closed (or playing) PSoundChannelBeOS"));
		return FALSE;
	}
	
	// Flush the buffers
	Abort();
	
	// Start recording	
	if (mRecorder->Start()!=B_OK)
	{
		PRINT(("BMediaRecorder::Start returned error"));
		return FALSE;
	}
	
	return TRUE;
}


BOOL PSoundChannelBeOS::IsRecordBufferFull()
{
	if (mRecorder)
	{
		return !mBuffer->IsEmpty();
	}
	
	return FALSE;
}


BOOL PSoundChannelBeOS::AreAllRecordBuffersFull()
{
	if (mRecorder)
	{
		return mBuffer->IsFull();
	}

	return FALSE;
}


BOOL PSoundChannelBeOS::WaitForRecordBufferFull()
{
	if (mRecorder==NULL)
	{
		PRINT(("Waiting for record buffer on playing or closed PSoundChannelBeOS"));
		return FALSE;
	}
	
	mBuffer->WaitForState(CircularBuffer::FullEnough);
		
	return PXSetIOBlock(PXReadBlock, readTimeout);
}


BOOL PSoundChannelBeOS::WaitForAllRecordBuffersFull()
{
	if (mRecorder==NULL)
	{
		PRINT(("Waiting for record buffers on playing or closed PSoundChannelBeOS"));
		return FALSE;
	}
	
	mBuffer->WaitForState(CircularBuffer::Full);

	return TRUE;
}


BOOL PSoundChannelBeOS::IsOpen() const
{
	BOOL result=((mPlayer!=NULL) || (mRecorder!=NULL));
	PRINT(("returning %s, player 0x%X recorder 0x%X", result?"true":"false", mPlayer, mRecorder));
	return result;
}



BOOL PSoundChannelBeOS::SetVolume(unsigned newVolume)
{
  #ifdef TODO
  cerr << __FILE__<< "PSoundChannelBeOS :: SetVolume called in error. Please fix" << endl;
  #endif

  return TRUE;
}

BOOL  PSoundChannelBeOS::GetVolume(unsigned & volume)
{
  #ifdef TODO
  cerr << __FILE__<< "PSoundChannelBeOS :: GetVolume called in error. Please fix" << endl;
  #endif

  return TRUE;

}

// End of file
