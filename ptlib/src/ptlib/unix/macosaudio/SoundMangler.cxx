#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include <ptlib.h>

// goddamn apple headers
#undef TCP_NODELAY
#undef TCP_MAXSEG
#include <Carbon/Carbon.h>

#include "MacMain.h"
#include "SoundMangler.h"

// allocate the SndChannel statically here rather than using NewPtr
static SndChannel mySndChannel;
static SndChannelPtr mySndChannelPtr = 0;
static SndCallBackUPP myUPP;
// each SoundHeader has an attached BLOCKSIZE buffer.  if a single
// sound block is larger than that, it will be divided without any
// promise of being played atomically...
enum { BLOCKSIZE = 1024 };
enum { MAXHEADERS = 8 };
// SoundHeader buffers are now used strictly round-robin fashion,
// and when a buffer is queued, we do not return until the buffer
// starts playing (approximately, anyway; what happens is that the
// caller passes in an MPSemaphoreID which is to be tickled when
// some previous buffer completes; previously this means the current
// buffer minus one...).
// A buffer is marked busy by writing 1 into the corresponding gSemaphore
// pointer, and marked free by writing 0.  If you can change the semaphore
// pointer atomically from 1 to a nonzero value, the Sound Manager Fairy
// will awaken you when the corresponding sound completes, otherwise it's
// your problem (it's an error if the value is nonzero already).  When
// marking a buffer free, if you can atomically turn 1 into 0, you're off
// the hook for signalling, otherwise it contains a semaphore pointer to ring.
static ExtSoundHeaderPtr mySoundHeaders[MAXHEADERS];
static MPSemaphoreID    *gSemaphores[MAXHEADERS];
static int gNextBuffer, gNumBuffers = 3;
static volatile UInt32 gHeaderMask;
static short gNumChannels;
static short gSampleSize;
static Fixed gSampleRate;

#define debugcrap
#ifdef debugcrap
static int bcount;
static Nanoseconds gStartTime;
static Nanoseconds gMostRecentTime;
static int playcount;
static Nanoseconds gPlayStartTime, gPlayMostRecentTime;
static int misses;
#endif

static pascal void theCallbackFunction(SndChannelPtr chan,
                                       SndCommand *cmd)
{
    // negative param1 is special (means check gCallBackWhenDone)
    if (cmd->param1 >= 0) {
        // Try to give the buffer back by changing the semaphore pointer
        // from 1 to 0.
        if (!OTCompareAndSwapPtr((void*)1, (void*)0,
                                 (void **)&gSemaphores[cmd->param1]))
        {
            (void)MPSignalSemaphore(*gSemaphores[cmd->param1]);
            gSemaphores[cmd->param1] = (MPSemaphoreID *)0;
        }
        
#ifdef debugcrap
        if (bcount == 0)
            gStartTime = AbsoluteToNanoseconds(UpTime());
        else
            gMostRecentTime = AbsoluteToNanoseconds(UpTime());
        
        if (++bcount == 200)
            fprintf(stderr,"200 buffers played\n");
#endif
    } else if (cmd->param1 == (short)-1) {
        // "thunk!"
        callbackThunk *thunk = (callbackThunk *)cmd->param2;
        thunk->func((void *)thunk);
        // thunk you very much.
    }
}

// Not much to do during the Initialize phase.  I allocate the memory for
// the SndChannel statically, and we don't set up the parameters until
// Open time.
OSStatus SoundManagerInitialize() 
{
    OSErr err = noErr;
    
    // first create the UPP for the callback function
    if ((myUPP = NewSndCallBackUPP(theCallbackFunction)) == 0) {
        err = memFullErr;
        goto bail;
    }
    gNextBuffer = 0;
    for (int i = 0; i < MAXHEADERS; i++) {
        ExtSoundHeaderPtr p;
        if ((p = (ExtSoundHeaderPtr)NewPtrClear( sizeof(ExtSoundHeader) - 1 + BLOCKSIZE )) == 0) {
            err = memFullErr;
            break;
        }
        mySoundHeaders[i] = p;
        gSemaphores[i] = 0;
        // channels, rate, and size get set at play time
        p->encode = extSH;
        p->baseFrequency = 60; // ??
        // fill in numFrames at the appropriate time
        // according to the CarbonSndPlayDB.c code snippet, AIFFSampleRate
        // is unused.  well, that's a logical place to document that fact!
    }
  bail:
    return err;
}

OSStatus SoundManagerOpenPlayer(const char *name, const soundParams *sp,
                                unsigned long *handlep)
{
    OSStatus err = noErr;
    if (mySndChannelPtr != 0)
        return fBsyErr;
    
    mySndChannel.qLength = 128;
    mySndChannel.userInfo = (long)0;
    // Actually create the channel
    // XXX To open a sound channel which is not the default device,
    // XXX obtain the component id of the output device, then pass
    // XXX kUseOptionalOutputDevice as the synth parameter and the
    // XXX ComponentInstance as the init parameter; I guess you'd then
    // XXX do a reInitCmd to set stereo or mono, and nothing works but
    // XXX sampledSynth nowadays anyway...
    mySndChannelPtr = &mySndChannel;
    err = SndNewChannel(&mySndChannelPtr, sampledSynth,
                        sp->sp_channels == 2 ? initStereo : initMono,
                        myUPP);
    if (err == 0) {
        gNumChannels = sp->sp_channels;
        gSampleSize  = sp->sp_samplesize;
        gSampleRate  = Long2Fix(sp->sp_samplerate);
        *handlep = (unsigned long)mySndChannelPtr;
    }
    return err;
}

OSStatus SoundManagerSetPlayerFormat(unsigned long /*h*/, const soundParams *sp)
{
    OSStatus err = noErr;
    // These values just get copied into each bufferCmd.  Clever Sound Manager.
    gNumChannels = sp->sp_channels;
    gSampleSize  = sp->sp_samplesize;
    gSampleRate  = Long2Fix(sp->sp_samplerate);
    // Interesting question; I wonder if I should "correct" integral sample
    // rates which are near standard non-integral sample rates?  Bah; I'm
    // only doing this for a task that uses 8KHz sampling, anyway...

    // I'm not sure if a stereo-capable channel needs to be warned that mono
    // data is coming (or vice versa?), but I suppose that a component-selected
    // output channel should be told of the channel choice sometime...
    
    SndCommand theCommand;
    theCommand.cmd    = reInitCmd;
    theCommand.param1 = 0;
    theCommand.param2 = gNumChannels == 2 ? initStereo : initMono;
    err = SndDoCommand(&mySndChannel, &theCommand, FALSE /* wait */);
    return err;
}

OSStatus SoundManagerPlaySample(unsigned long /*h*/,
                                const unsigned char *buf,
                                size_t len,
                                MPSemaphoreID *awaken_me)
{
#ifdef debugcrap
    if (playcount == 0) {
        playcount = 1;
        gPlayStartTime = AbsoluteToNanoseconds(UpTime());
    }
    else {
        if (playcount == 1) playcount = len;
        else playcount += len;
        if ((playcount - len) < 240000 && playcount >= 240000)
            fprintf(stderr,"~240000 bytes played\n");
        gPlayMostRecentTime = AbsoluteToNanoseconds(UpTime());
    }
//    if (len != 480) fprintf(stderr,"wierd len %d\n", len);
#endif

    OSErr err = noErr;

    // the insufficiently abstract API we present loses the size of the
    // sample frames, so we have to reinvent that here.
    int frameSize = gNumChannels * (gSampleSize == 8 ? 1 : 2);
    assert(len%frameSize == 0);
    if (len > BLOCKSIZE)
        return notEnoughBufferSpace;
    
    // busy-wait until the sound header we want is free
    // Remember -- only one thread queues requests!
    
    while (!OTCompareAndSwap32(0, 1,
                               (UInt32*)&gSemaphores[gNextBuffer])) {
        /* the assumption is we're a Blue thread, so play nice */
        YieldToAnyThread();
#if P_MACOSX
        usleep(1000);
#endif // P_MACOSX
#ifdef debugcrap
#define DEBUGME
#ifdef DEBUGME
        if ((misses & 127) == 0)
            fprintf(stderr,"missed scoring a buffer!\n");
        misses++;
#endif // DEBUGME
#endif // debugcrap
    }
    // OK, we've made our reservation, copy the data
    ExtSoundHeaderPtr p;
    p = mySoundHeaders[gNextBuffer];
    // someone set up us the buffer!
    memcpy(&p->sampleArea[0], buf, len);

    p->numFrames = len / frameSize;
    p->numChannels = gNumChannels;
    p->sampleRate = gSampleRate;
    p->sampleSize = gSampleSize;

    // Queue the commands (release the hounds!)
    do { // break-scope
        SndCommand theCommand;
        theCommand.cmd    = bufferCmd;
        theCommand.param1 = 0;
        theCommand.param2 = (long)p;
        err = SndDoCommand(&mySndChannel, &theCommand, FALSE);
        if (err) break;
        
        // And arrange for it to be picked up
        theCommand.cmd    = callBackCmd;
        theCommand.param1 = gNextBuffer; // we're single threaded, right???
        theCommand.param2 = 0;
        err = SndDoCommand(&mySndChannel, &theCommand, FALSE);
        if (err) break;
        
        // now, attempt to reserve callback
        if (awaken_me) {
            int prevBuffer = gNextBuffer-1;
            if (prevBuffer == -1)
                prevBuffer = gNumBuffers-1;
            if (!OTCompareAndSwapPtr((void *)1, (void *)awaken_me,
                                     (void **)&gSemaphores[prevBuffer])) {
                PAssert(gSemaphores[prevBuffer] == 0, "screwup in buffer handling");
                MPSignalSemaphore(*awaken_me);
            }
        }
        // OK, advance buffer pointer
        if (++gNextBuffer == gNumBuffers)
            gNextBuffer = 0;
    } while (0); // end break scope
    if (err) {
        gSemaphores[gNextBuffer] = 0; // give it back
        goto bail;
    }
  bail:
    if (err) {
        fprintf(stderr,"SndDoCommand failed with err %d\n", err);
    }
    return err;
}

OSStatus SoundManagerClosePlayer(unsigned long /*h*/)
{
    if (mySndChannelPtr) {
        // XXX what about all of the waiting semaphores?
        SndDisposeChannel(&mySndChannel, FALSE);
    }
    mySndChannelPtr = 0;
    return 0;
}

OSStatus SoundManagerStopPlayer(unsigned long /*h*/)
{
    OSErr err;
    SndCommand theCommand;
    theCommand.cmd    = flushCmd;
    theCommand.param1 = 0;
    theCommand.param2 = 0;
    err = SndDoImmediate(&mySndChannel, &theCommand);
    // XXX what about all of the waiting semaphores?
    if (err == 0) {
        theCommand.cmd    = quietCmd;
        err = SndDoImmediate(&mySndChannel, &theCommand);
    }
    return err;
}

OSStatus SoundManagerIsPlaying(unsigned long /*h*/, unsigned long *answer)
{
    // If the "previous" buffer is free, it's not busy.
    int prevBuffer = gNextBuffer-1;
    if (prevBuffer == -1) prevBuffer = gNumBuffers-1;
    *answer = (gSemaphores[prevBuffer] != 0);
    return 0;
}

OSStatus SoundManagerWaitForPlayCompletion(unsigned long,
                                           const callbackThunk *thunk)
{
    // Register a callback which will be called when the sounds are done
    // playing.  It will be inserted into the SM queue, so if you queue
    // sounds while it is waiting, it may return "early".
    SndCommand theCommand;
    theCommand.cmd    = callBackCmd;
    theCommand.param1 = -1; // special index value
    theCommand.param2 = (unsigned long)thunk;
    OSStatus err = SndDoCommand(&mySndChannel, &theCommand, FALSE);
    return err;
}

OSStatus SoundManagerTeardown()
{
    if (myUPP) {
        // better not be running!
        DisposeSndCallBackUPP(myUPP);
        myUPP = 0;
    }
    for (int i = 0; i < MAXHEADERS; i++) {
        if (mySoundHeaders[i]) {
            DisposePtr((char *)mySoundHeaders[i]);
            mySoundHeaders[i] = 0;
        }
    }
    return 0;
}

