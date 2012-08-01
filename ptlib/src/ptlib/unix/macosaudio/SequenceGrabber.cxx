#include <stdio.h>


#include <CoreServices/CoreServices.h>
#include <QuickTime/QuickTime.h>
#include "ringbuffer.h"
#include "MacMain.h"
#include "SequenceGrabber.h"

// NB:  I assume that only one SequenceGrabber object is going to be created

    // The data proc should probably be specialized for the actual
    // transformation(s) needed.
EXTERN_API(OSErr) myDataProc(SGChannel, Ptr, long, long*, long,
                             TimeValue, short, long);

enum { kNone, kLeft, kRight, kSum, k0Left, k0Right, kBoth };

namespace SequenceGrabber 
{
    SeqGrabComponent    m_seqGrab;
    SGChannel           m_audiochannel;
    SGChannel           m_videochannel;
    SGDataUPP           m_upp;
    bool                m_haveloaded;
    bool                m_running;
    bool                m_paused;
    
    // the Audio parameters
    short               m_numChannels;
    short               m_sampleSize;
    Fixed               m_sampleRate;
    // Not all hardware can change the sample size and number of channels.
    // And irritatingly enough, QuickTime is inconsistent about telling you
    // this.  Fortunately, these are easy enough to do in software.  (Let's
    // just hope that no hardware demands to do compression for us...)
    // stereofix values: first four are stereo-to-mono adjustments, the
    // last three are mono-to-stereo (and are unimplemented)
    int                 m_stereofix;
    // samplesize fixing is not yet implemented
    int                 m_samplefix; // 0 means leave alone, 1 means fix
    // Fortunately, I have verified that the sequence grabber *will* do
    // software rate conversion.
    
    // this points to the ring buffer when recording and is 0 otherwise
    JRingBuffer *m_audioRingBuffer;

    // The Video parameters
    // XXX
};

void SequenceGrabberIdle(void)
{
    using namespace SequenceGrabber;
    if (m_seqGrab != 0)
        SGIdle(m_seqGrab);
}

OSStatus SequenceGrabberInitialize(void)
{ return 0; }

OSStatus SequenceGrabberRealInitialize(void)
{
    OSErr err;
    using namespace SequenceGrabber;
    
    if (m_seqGrab != 0)
        return fBsyErr; // go away
    m_haveloaded = true;
    m_paused = false;
    m_running = false;

    // fire up QuickTime
    EnterMovies();

    SeqGrabComponent   seqGrab;
    seqGrab=OpenDefaultComponent(SeqGrabComponentType, 0);
    if (seqGrab == NULL) {
       return invalidComponentID;
    }

    err = SGInitialize(seqGrab);
    // tell it we're not making a movie
    if (err == 0)
        err = SGSetDataRef(seqGrab,0,0,seqGrabDontMakeMovie);
    if (err) {
        // just give up
        (void)CloseComponent(seqGrab);
    } else
        m_seqGrab = seqGrab;
    return err;
}

OSStatus SequenceGrabberTeardown(void)
{
    OSErr err = noErr;
    using namespace SequenceGrabber;

    // it looks like CloseComponent will shut everything down for us
    (void)CloseComponent(m_seqGrab);
    m_seqGrab = 0;
    ExitMovies();
    return err;
}

// Set up the audio channel
OSStatus SequenceGrabberOpenRecorder(const char *name, const soundParams *params,
                                     unsigned long *handlep)
{
    OSErr err;
    long buftime = (long)FixRatio(-1, 60);
    using namespace SequenceGrabber;

    if (!m_haveloaded) {
        err = SequenceGrabberRealInitialize();
        if (err) return err;
    }
    
    // XXX verify not running
    if (m_audiochannel)
        return fBsyErr;
    if (m_running) {
        err = SGPause(m_seqGrab, seqGrabPause);
        m_paused = true;
        if (err) return err;
    }
    err = SGNewChannel(m_seqGrab, SoundMediaType,
                       &m_audiochannel);
    if (err) return err;
    // set usage for new audio channel to avoid playthrough
    err = SGSetChannelUsage(m_audiochannel, seqGrabRecord );
    // Set up the chunk size to compromise between latency and thread
    // scheduling irritations
    if (err == 0)
        err = SGSetSoundRecordChunkSize(m_audiochannel,
                                        buftime);
    if (err) {
        // tear down audio channel
        SGDisposeChannel(m_seqGrab,m_audiochannel);
        m_audiochannel = 0;
    } else {
        m_numChannels = params->sp_channels;
        m_sampleSize = params->sp_samplesize;
        m_sampleRate = params->sp_samplerate;
        *handlep = (unsigned long)m_audiochannel;
    }
    if (m_paused) {
        err = SGPause(m_seqGrab, seqGrabUnpause);
        m_paused = false;
    }
    return err;
}

OSStatus SequenceGrabberCloseRecorder(unsigned long /* handle */)
{
    int err;
    using namespace SequenceGrabber;
    if (m_audiochannel) {
        fprintf(stderr,"closing recorder channel\n");
        if (m_running && !m_paused) {
            if (m_videochannel)
                err = SGPause(m_seqGrab, seqGrabPause);
            else {
                err = SGStop(m_seqGrab);
                m_running = false;
            }
        }
        assert(err == 0);
        err = SGDisposeChannel(m_seqGrab,m_audiochannel);
        assert(err == 0);
        m_audiochannel = 0;
        if (m_audioRingBuffer) {
            m_audioRingBuffer->Shutdown();
            m_audioRingBuffer = 0;
        }
        // resume capture?
        if (m_videochannel) {
            err = SGPause(m_seqGrab, seqGrabUnpause);
        } 
    }
    return 0;
}

OSStatus SequenceGrabberRecorderParams(unsigned long /*h*/, const soundParams *sp)
{
    using namespace SequenceGrabber;
    OSErr err = noErr;
    short origchans, origbits;
    OSType origcomp; // want to set 'raw '    
    SGChannel channel = m_audiochannel;

    do { // not a loop
        // find the current settings
        if ((err = SGGetSoundInputParameters(channel,
                                             &origbits,
                                             &origchans,
                                             &origcomp)) != noErr)
            break;
        // Try to set the number of channels
        m_stereofix = kNone;
        if (origchans != sp->sp_channels) {
            err = SGSetSoundInputParameters(channel, 0, sp->sp_channels, 0);
            if (err != noErr)
            {
                // if the err is notEnoughHardwareErr, then the hardware 
                // (and driver) is unwilling to deliver mono/stereos.
                // We can (sort of) fix that easily enough.
                if (err == notEnoughHardwareErr) {
                    // do we want mono or stereo?
                    if (sp->sp_channels == 1) {
                        // XXX always sum
                        m_stereofix = kSum;
                    } else {
                        // XXX always output left channel
                        m_stereofix = k0Right;
                    }
                    err = 0;
                } else break;
            }
        }
        m_numChannels = sp->sp_channels;

        // Here's a nasty bit:  for samplesize and compression, QT appears to
        // return success when you try to change them but silently fails to
        // do so.
        m_samplefix = 0;
        if (origbits != sp->sp_samplesize) {
            err = SGSetSoundInputParameters(channel, sp->sp_samplesize, 0, 0);
            if (err != noErr && err != notEnoughHardwareErr)
                break;
            if (err == noErr) {
                // yes, but did it work?
                err = SGGetSoundInputParameters(channel, &origbits,
                                                &origchans, &origcomp);
                if (err) break;
                if (origbits != sp->sp_samplesize)
                    err = notEnoughHardwareErr; // geez!
            }
            if (err == notEnoughHardwareErr) {
                assert(origbits == 8 || origbits == 16);
                if (origbits == 8) // and want 16
                    m_samplefix = 1;
                else
                    m_samplefix = -1; // have 16, want 8
            }
        }
        m_sampleSize = sp->sp_samplesize;
        
        // just make sure compression is off
        if (origcomp != kOffsetBinary) {
            // OK, here's a nauseating little treat for you:
            // GetSoundInputParameters returns 'raw ' for uncompressed samples.
            // That makes sense, right?  Well, 'raw ' is supposedly
            // 8-bit offset binary (0..255); the uncompressed 16-bit format
            // is supposed to be 'twos' on a bigendian machine.  However,
            // GetSoundInputParameters returns 'raw ' for all raw formats.
            // I guess this is one of them hysterical oddities.  It makes
            // sense UNLESS you read the documentation...
            if ((err = SGSetSoundInputParameters(channel, 0, 0, kOffsetBinary)) != noErr)
                break;
            if ((err = SGGetSoundInputParameters(channel, 0, 0, &origcomp)) != noErr)
                break;
            assert(origcomp == kOffsetBinary);
            if (origcomp != kOffsetBinary) {
                // this should probably be "tooMuchSoftwareErr", but there
                // doesn't appear to be such an error code...
                return notEnoughHardwareErr;
            }
        }
        // Finally, change the sampling rate.  Fortunately, that seems to
        // just work even if the harware would rather not.
        // SetSoundInputRate takes a Fixed value, which is a 16.16 fixed-point
        // number.  This limits audio capture to 65KHz.
        m_sampleRate = Long2Fix(sp->sp_samplerate);
        if ((err = SGSetSoundInputRate(channel,
                                       m_sampleRate)) != noErr)
            break;
        // just assume it worked.
    } while (0); // end of "break scope"
    return err;
}

OSStatus SequenceGrabberStartRecorder(unsigned long /*h*/, JRingBuffer *rb)
{
    OSErr err = 0;
    using namespace SequenceGrabber;
    
    // XXX Now here's an annoying problem:
    // XXX both the audio and video channels really should be set up before
    // XXX we get to this point.

    if (m_upp == 0) {
        // register the callback and fire it up
        // create our Universal Procedure Pointer
        m_upp = NewSGDataUPP(myDataProc);

        err = SGSetDataProc(m_seqGrab,
                            m_upp,
                            0);
        if (err == 0) {
            m_running = true;
            err = SGStartRecord(m_seqGrab);
        }
    }
    // Now, the sequence grabber data proc is running; deftly and atomically
    // establish a place for audio data to go
    if (err == 0 &&
        !OTCompareAndSwap32((UInt32)0, (UInt32)rb,
                            (UInt32*)&m_audioRingBuffer)) {
        err = fBsyErr;
    }
    return err;
}

OSStatus SequenceGrabberStopRecorder(unsigned long /*h*/)
{
    OSErr err = 0;
    using namespace SequenceGrabber;
    
    if (m_audiochannel) {
        fprintf(stderr,"Stopping recorder channel\n");
        err = SGPause(m_seqGrab, seqGrabPause);
        m_paused = true;
        assert(err == 0);
        // I assume that by this point, the dataproc is not running
        if (m_audioRingBuffer) {
            m_audioRingBuffer = 0;
        }
        assert(err == 0);
        if (m_videochannel) {
            m_paused = false;
            err = SGPause(m_seqGrab, seqGrabUnpause);
        }
    }
    return 0;
}

OSErr AudioDataProc( Ptr p, long len, long *offset );

DEFINE_API(OSErr) myDataProc(SGChannel c, Ptr p, long len, long *offset,
                             long chRefCon, TimeValue time,
                             short writeType, long refCon)
{
#pragma unused(offset,chRefCon,time,refCon)
    using namespace SequenceGrabber;
	ComponentResult err = noErr;

	if(writeType == seqGrabWriteReserve) return 0; // who cares?
    // OK, which channel is it?
    if (c == m_audiochannel)
        return AudioDataProc(p, len, offset);
    return err;
}


OSErr AudioDataProc( Ptr p, long len, long *offset)
{
    using namespace SequenceGrabber;
    OSErr err = noErr;

    if (len == 0 || p == 0) return 0;
    
    if (m_stereofix != kNone) len /= 2;
    assert(m_samplefix == 0);

    // shovel the data into the buffer
    int tocopy = len; // grr.  all the world's a vax, OK?
    unsigned char *buf = (unsigned char*)m_audioRingBuffer->ReserveToWrite(tocopy);
    assert(buf != 0);

    while (len > 0 && buf != 0) {
        int i;
#ifndef NDEBUG
        if ((len&1) == 0) assert((tocopy&1) == 0);
        if (m_sampleSize == 16 && m_stereofix != kNone)
            assert((tocopy&1) == 0);
#endif
        switch(m_stereofix) {
        case kNone:
            memmove(buf, (void *)p, tocopy);
            break;
        case kLeft:
            if (m_sampleSize == 16) {
                for (i = 0; i < tocopy;) {
                    // take the left (first) channel
                    buf[i++] = *p++;
                    buf[i++] = *p++;
                    p+=2;
                }
            } else {
                for (i = 0; i < tocopy; i++) {
                    // take the left (first) channel
                    buf[i] = *p++;
                    p++;
                }
            }
            break;
        case kRight:
            if (m_sampleSize == 16) {
                for (i = 0; i < tocopy;) {
                    // take the second (right) channel
                    p += 2;
                    buf[i++] = *p++;
                    buf[i++] = *p++;
                }
            } else {
                for (i = 0; i < tocopy; i++) {
                    // take the right (second) channel
                    p++;
                    buf[i] = *p++;
                }
            }
            break;
        case kSum:
            if (m_sampleSize == 16) {
                for (i = 0; i < tocopy;) {
                    // sum both channels
                    short s = (((short *)p)[0] / 2) + (((short *)p)[1] / 2);
                    *(short *)buf = s;
                    buf += 2; i += 2;
                    p+=4;
                }
            } else {
                for (i = 0; i < tocopy; i++) {
                    // sum the two channels
                    buf[i] = ((signed char*)p)[0]/2 + ((signed char*)p)[1]/2;
                    p+=2;
                }
            }
            break;
        }
        len -= tocopy;
        if (len) {
            int didcopy = tocopy;
            tocopy = len;
            buf = (unsigned char *)m_audioRingBuffer->CommitMore(didcopy, tocopy);
            assert(buf != 0);
        } 
        else {
            (void)m_audioRingBuffer->CommitFinal(tocopy);
        }
	}
	return err;
}
