struct soundParams;

OSStatus SoundManagerInitialize();
OSStatus SoundManagerTeardown();
OSStatus SoundManagerOpenPlayer(const char *, const soundParams *, unsigned long *);
OSStatus SoundManagerClosePlayer(unsigned long);
OSStatus SoundManagerSetPlayerFormat(unsigned long, const soundParams *);
// PlaySample is goofy.  You pass in a pointer to a semaphoreID, and it will
// be signalled when your sample starts playing.
OSStatus SoundManagerPlaySample(unsigned long,
                                const unsigned char *, size_t,
                                MPSemaphoreID *);
OSStatus SoundManagerClosePlayer(unsigned long);
OSStatus SoundManagerStopPlayer(unsigned long);
OSStatus SoundManagerIsPlaying(unsigned long, unsigned long *);
// WaitForPlayCompletion is goofy.  It requests a callback when the queue is
// empty.  You pass in a callbackThunk which contains a function pointer and
// a data pointer.
struct callbackThunk 
{
    void (*func)(void *);
    void *argument;
};
OSStatus SoundManagerWaitForPlayCompletion(unsigned long, const callbackThunk*);


