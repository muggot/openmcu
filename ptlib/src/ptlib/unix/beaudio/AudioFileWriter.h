/*	pwlib/src/ptlib/unix/beaudio/AudioFileWriter.h

	$Log: AudioFileWriter.h,v $
	Revision 1.1  2001/07/09 06:16:15  yurik
	Jac Goudsmit's BeOS changes of July,6th. Cleaning up media subsystem etc.

   
	Copyright 1999-2001, Be Incorporated.   All Rights Reserved.
	This file may be used under the terms of the Be Sample Code License.
*/

#ifndef WAVWRITER_H
#define WAVWRITER_H 1

#include <media/MediaDefs.h>
#include <media/MediaFile.h>
#include <media/MediaTrack.h>
#include <storage/Entry.h>

class BAudioFileWriter
{
	// A debugging class that can be used to write an audio-file from
	// raw data.
protected:
	BMediaFile *pfile;
	BMediaTrack	*ptrack;
	unsigned framesize;
	size_t totalframes;
	size_t maxframes;
	char *mfilename;
public:
	BAudioFileWriter(const char *filename, media_format &format, size_t maxsize=(size_t)-1) :
		pfile(NULL),
		ptrack(NULL),
		framesize(1),
		totalframes(0),
		maxframes(maxsize)
	{
		status_t dwLastError;

		// copy the filename; it is used in error messages
		mfilename=strdup(filename);
	
		// find the filename's extension (yeah yeah this is windowy but what can
		// I do, I copied this code from somewhere else :-)	
		const char *extension=(strrchr(filename, '.'));
		if (!extension)
		{
			printf("Cannot find filename extension\n");
			return;
		}
		
		// move to the character after the '.'
		extension++;
		
		// Try to find the file format in BeOS's list of formats
		media_file_format mfi;
		int32 cookie=0;
		while ((dwLastError=get_next_file_format(&cookie, &mfi))==B_OK)
		{
			if (!strcasecmp(mfi.file_extension, extension))
			{
				break;
			}
		}
		if (dwLastError!=B_OK)
		{
			// didn't find file format
			printf("Couldn't find media_file_format for \".%s\"\n",extension);
			return;
		}
		
		// Create BEntry from file name
		BEntry	entry(filename, true);
		if ((dwLastError=entry.InitCheck())!=B_OK)
		{
			return;
		}
		
		// Create entry_ref from BEntry	
		entry_ref ref;
		if ((dwLastError=entry.GetRef(&ref))!=B_OK)
		{
			return;
		}
	
		// Create BMediaFile for write access from the entry_ref
		pfile=new BMediaFile(&ref, &mfi, B_MEDIA_FILE_REPLACE_MODE);
		if ((dwLastError=pfile->InitCheck())!=B_OK)
		{
			return;
		}
		
		// Find an encoder.
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
		if (found_encoder)
		{
			if (found_raw_encoder)
			{
				printf("Using raw encoder\n");
				pmci=&rawmci;
			}
			else
			{
				// don't use mci instead of validmci,
				// it could be unreliable after the last call to get_next_encoder
				printf("Using non-raw encoder");
				pmci=&validmci;
			}
			
			// Create a BMediaTrack in the file using the selected encoder
			char s[256];
			if (string_for_format(format, s, 255))
			{
				printf("Creating media track for format: %s\n",s);
			}
			else
			{
				printf("(can't express format as string... - creating media track)\n");
			}
			ptrack = pfile->CreateTrack(&format, pmci);
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
			return;
		}
		
		// We're only creating one track so commit the header now
		if ((dwLastError = pfile->CommitHeader())!=B_OK)
		{
			return;
		}
	
		if (format.AudioFormat()==B_MEDIA_ENCODED_AUDIO)
		{
			framesize=format.u.encoded_audio.frame_size;
		}
		else
		{
			framesize=format.u.raw_audio.channel_count*(format.u.raw_audio.format&0xF);
			if (framesize==0)
			{
				printf("Hmmmmmm framesize is 0... Send me data and I'll die\n");
			}
		}
		printf("Created and opened %s successfully\n",mfilename);
	}
	virtual ~BAudioFileWriter()
	{
		if (!pfile)
		{
			printf("Output file is not open\n");
			return;
		}
		
		if (pfile->CloseFile()!=B_OK)
		{
			printf("CloseFile failed...\n");
		}
		delete pfile; // destroys ptrack
		pfile=NULL;ptrack=NULL;
		
		printf("Audio file %s closed successfully\n", mfilename);
		free(mfilename);
	}
	
	void writewavfile(const void *data, size_t size)
	{
		int32 numframes = size / framesize; // divide by zero possibility ignored.
		
		if (totalframes+numframes>maxframes)
		{
			static bool message=true;
			if (message)
			{
				printf("Audio size limit reached; closing file %s\n",mfilename);
				message=false;
				pfile->CloseFile();
			}
			return;
		}
		totalframes+=numframes;
		

		if (!pfile)
		{
			static bool message=true;
			if (message)
			{
				printf("Audio file %s is not open (this will only appear once)\n", mfilename);
				message=false;
			}
			return;
		}
		
		if (ptrack->WriteFrames(data, numframes)!=B_OK)
		{
			printf("Error writing audio file %s, closing\n", mfilename);
			if (pfile->CloseFile()!=B_OK)
			{
				printf("CloseFile failed (ignoring)...\n");
			}
			delete pfile; // destroys ptrack
			pfile=NULL;ptrack=NULL;
			return;
		}
		
		{
			static bool message=true;
			if (message)
			{
				printf("Wrote %lu bytes to audio file %s (this will only appear once)\n",size, mfilename);
				message=false;
			}
		}
	}
	
	bool IsOpen() { return pfile!=NULL; }
};

#endif
