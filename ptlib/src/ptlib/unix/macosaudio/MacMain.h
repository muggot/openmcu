// This embarassing hack exists purely because EMACS' electric-c indentation
// mode is too clever by half...
#ifdef __cplusplus
#define BEGIN_EXTERN_C extern "C" {
#define END_EXTERN_C };
#else
#define BEGIN_EXTERN_C
#define END_EXTERN_C
#endif

#include <CoreServices/CoreServices.h>

BEGIN_EXTERN_C

// In order to get along with the threading restrictions of Carbon
//
//         "Carbon is not thread-safe for preemptive threads.  You really need
//          to use Carbon only from your main thread, and not from other
//          pthreads. ..."
//
// this file implements a special main() function for the Mac OS, as well as
// implementing a command queue so that other threads may make Carbon toolbox
// calls safely (i.e. by asking the Mac main thread to do so).
//
// The job of the main thread is to handle events.  The main thread also
// drives the sequence grabber (by calling SGIdle when appropriate) and the
// Sound Manager.
// The need to fiddle with these features is communicated via custom Carbon
// Events;  the Carbon Event Manager is MPThread-safe (but not pthread
// safe), so I have made sure that PWLIB uses MPThreads so they can talk
// directly to the main thread.  
//
// Oh yeah, and it calls the PProcess::Main() function, too.  Wouldn't want
// to forget about that.  That gets called on yet another MPThread, whose
// termination drives the shutdown of the main thread.  I hope.

// Some of the ideas here come from the OTMP library.  Some of them don't.
// Guess which set turns out to be more reliable...

// In detail:
// A thread wishing to have a Carbon action performed creates one of these
// structures on its stack and calls CarbonQueue; when CarbonQueue returns,
// the action has been performed.
//
// CarbonQueue initializes a semaphore in the carbonRequest object, places
// the object on an OTLIFO queue, and wakes up the main thread with
// a custom Carbon Event.  Then it sleeps on the semaphore.
//
// The main thread's handler checks the queue for Carbon commands needing
// to be made in the context of the process main thread.  When it has
// performed each call, it signals the semaphore and goes to the next
// request.  A few commands cannot be completed synchronously by the main
// thread; they pass responsibility for the semaphore to a callback function.

enum carbonCommand_t 
{
    // audio recorder commands
    kOpenRecorder      = 'opre',
        // arguments: const char *name, struct soundParams *
        // returns: reference to new audio recorder OR error
    kSetFormatRecorder = 'sfre',
        // arguments: int reference, int soundParams *
        // returns: error
    kCloseRecorder     = 'clre',
        // arguments: int reference
        // returns: error
    kStartRecorder     = 'stre',
        // arguments: int reference, struct ringbuffer *
        // returns: error
    kStopRecorder      = 'spre',
        // arguments: int reference
        // returns: error
    // audio player commands
    kOpenPlayer        = 'oppl',
        // arguments: name, soundParams *
        // returns: reference to new audio player OR error
    kSetFormatPlayer   = 'sfpl',
        // arguments: int reference, soundParams *
        // returns: error
    kClosePlayer       = 'cppl',
        // arguments: int reference
        // returns: error
    kPlaySample        = 'smpl',
        // arguments: int reference, char *buffer, int buflen
        // returns: error
        // note: returns when sound is enqueued, not finished.
    kStopPlayer       = 'stpl', // ssshhh!!!
        // arguments: int reference
        // returns: error
    kIsPlaying         = 'ispl',
        // arguments: int reference
        // returns: 0/1 if playing
    kWaitForPlayCompletion = 'wapl',
        // arguments: int reference
        // returns: error
    // blah blah blah
    kNotACommand       = 0
};

typedef
struct soundParams
{
    unsigned short sp_channels;
    unsigned short sp_samplesize;
    unsigned long sp_samplerate;
#ifdef __cplusplus
    soundParams(int c, int s, long r) 
            : sp_channels(c), sp_samplesize(s), sp_samplerate(r) 
        {}
#endif
} soundParams;

typedef struct commandRequest commandRequest;
OSStatus CarbonQueue(commandRequest *request);

struct commandRequest
{
    OTLink          m_next; // it's a pointer
    
    carbonCommand_t m_command;
    unsigned long m_arg1, m_arg2, m_arg3, m_arg4;
    unsigned long m_result; // if useful data is returned
    OSStatus m_status; // error return
    MPSemaphoreID m_done;
#ifdef __cplusplus
    commandRequest(carbonCommand_t cmd,
                   unsigned long a1 = 0,
                   unsigned long a2 = 0,
                   unsigned long a3 = 0,
                   unsigned long a4 = 0)
            : m_command(cmd), m_arg1(a1), m_arg2(a2), m_arg3(a3), m_arg4(a4),
              m_result(0), m_status(0), m_done(0)
        {}
    commandRequest()
            : m_command(kNotACommand), m_arg1(0), m_arg2(0), m_arg3(0), m_arg4(0),
              m_result(0), m_status(0), m_done(0)
        {}
    // convenience constructors
    commandRequest(carbonCommand_t cmd,
                   const unsigned char *a1,
                   const soundParams *a2)
            : m_command(cmd),
              m_arg1((unsigned long)a1), m_arg2((unsigned long)a2),
              m_arg3(0), m_arg4(0),
              m_result(0), m_status(0), m_done(0)
        {}
    commandRequest(carbonCommand_t cmd,
                   int a1,
                   const soundParams *a2)
            : m_command(cmd),
              m_arg1(a1), m_arg2((unsigned long)a2), m_arg3(0), m_arg4(0),
              m_result(0), m_status(0), m_done(0)
        {}
    commandRequest(carbonCommand_t cmd,
                   int a1,
                   const void *a2)
            : m_command(cmd),
              m_arg1(a1), m_arg2((unsigned long)a2), m_arg3(0), m_arg4(0),
              m_result(0), m_status(0), m_done(0)
        {}
    commandRequest(carbonCommand_t cmd,
                   int a1,
                   const void *a2,
                   unsigned long a3)
            : m_command(cmd),
              m_arg1(a1), m_arg2((unsigned long)a2), m_arg3(a3), m_arg4(0),
              m_result(0), m_status(0), m_done(0)
        {}
    // and a convenience inline
    OSStatus CarbonQueue()
        {
            return ::CarbonQueue(this);
        }
#endif
};

END_EXTERN_C
