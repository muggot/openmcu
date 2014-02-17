// This embarassing hack exists purely because EMACS' electric-c indentation
// mode is too clever by half...
#ifndef BEGIN_EXTERN_C
#ifdef __cplusplus
#define BEGIN_EXTERN_C extern "C" {
#define END_EXTERN_C };
#else
#define BEGIN_EXTERN_C
#define END_EXTERN_C
#endif
#endif

BEGIN_EXTERN_C

class JRingBuffer;
struct soundParams;

/*
 * The interface to the Sequence Grabber works like this:
 *
 */
OSStatus SequenceGrabberInitialize(void);
    /* allocates a sequence grabber plus my own state information. */
void     SequenceGrabberIdle(void);
    /* calls SGIdle() if necessary */
OSStatus SequenceGrabberTeardown(void);
    /* destroys the sequence grabber and state information. */
OSStatus SequenceGrabberOpenRecorder(const char *name, const soundParams *, unsigned long *ref);
    /* this creates the audio channel if none exists (and returns an error
     * if it already does).  currently the name is ignored and you get
     * whatever QuickTime's default choice is.  It returns (via ref) a
     * "handle" on the channel (which is ignored).
     */
OSStatus SequenceGrabberCloseRecorder(unsigned long ref);
    /* destroys the audio channel.  halts the sequence grabber temporarily or
     * permanently, so it may glitch the video stream.  you are assumed not
     * to care.
     */
OSStatus SequenceGrabberRecorderParams(unsigned long ref, const soundParams *);
    /* sets up the audio channel parameters. */
OSStatus SequenceGrabberStartRecorder(unsigned long ref, JRingBuffer *rb);
    /* attaches the ringbuffer to the dataproc and starts the dataproc if it
     * isn't going.  If it is, hope I can set it up atomically.
     */
OSStatus SequenceGrabberStopRecorder(unsigned long ref);
    /* calls SGStop() and removes the ringbuffer, disabling audio recording.
     * this may glitch the video, you are presumed not to care.
     */

END_EXTERN_C
