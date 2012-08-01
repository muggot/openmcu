#include <ptlib.h> // will include CoreServices.h
#include <Carbon/Carbon.h>
#include <ptlib/MacMainIf.h>
#include "MacMain.h"
#include "SequenceGrabber.h"
#include "SoundMangler.h"

// Here is the problem:
//
// The ancient Mac toolbox routines (in particular, QuickTime and the Sound
// Manager) must be called from cooperatively scheduled threads; they are
// not preemptively-scheduled thread safe.  (This includes most of Carbon.)
// The main thread of an application is (in effect) such a thread, and the 
// main thread is known to the Carbon layer.  The H323 library makes use of
// POSIX pthreads, however, which can lead directly to trouble if multiple
// preemptive pthreads call into Carbon managers; this can also lead indirectly
// to trouble if the main thread is written as a classic pthread -- if the
// main thread blocks outside of Carbon's control, suddenly Carbon's callbacks
// don't happen.
//
// This leads to two conclusions:  if H323 is to use the Carbon toolbox (and
// right now I don't see any way around that), then (1) the main thread must
// be written like a traditional Mac main event loop, and (2) the main thread
// (or Thread Manager threads it creates) must make all Carbon calls.
//
// It isn't hard to design a queue that pthreads can leave requests on for
// service by the main thread, but there is a peculiarity:  I want to write
// the main loop as a modern Carbon Event Manager loop, to avoid busy-waiting
// and the other ills associated with Classic event loops.  The Carbon Event
// Manager is an exception to the cooperative threading rule (yay!) but alas
// it must be called from "MPThreads", i.e. only from preemptively-scheduled
// threads obtained from the Multiprocessing Services.
// So I modified PWLIB to use MPThreads.


OTLIFO gBlueCommandList = {0};
OTLock gBlueCommandIsRunning = 0;

enum {
    kEventClassQueueBusy = 'Queb',
    kEventQueueBusy = 1
};


namespace
{
EventHandlerUPP queueHandlerUPP;
EventHandlerRef queueHandlerRef;
EventLoopTimerUPP idleHandlerUPP;
EventLoopTimerRef timerRef;
MPTaskID ApplicationID;
};
extern "C" 
{
    MPQueueID pwlibAppQueueID; // where the application's death is reported
}

int debug_mpthreads = 0; // see tlibmpthrd.cxx

extern "C" OSStatus CarbonQueue(commandRequest *request)
{
    OSStatus err = noErr;

    // create binary semaphore in the off condition
    if ((err = MPCreateSemaphore(1, 0, &request->m_done)) != noErr)
        return err;
    
    // Place on the intercession queue and block waiting for results.
    request->m_next.fNext = 0;
    OTLIFOEnqueue(&gBlueCommandList, &request->m_next);

    // Do we need to send a Carbon Event?
    if (OTAcquireLock(&gBlueCommandIsRunning)) {
        // yes.  If this fails, don't sweat it, the main loop polls, too.
        EventRef theQueueEvent;
        CreateEvent(NULL, kEventClassQueueBusy, kEventQueueBusy, 0,
                    kEventAttributeUserEvent, &theQueueEvent);
        (void)PostEventToQueue(GetMainEventQueue(),
                               theQueueEvent, kEventPriorityStandard);
    }
    // and wait for results
    err = MPWaitOnSemaphore(request->m_done, kDurationForever);

    (void)MPDeleteSemaphore(request->m_done);
    request->m_done = 0;

    return err;
}

// The command functions:
namespace 
{
    OSStatus BlueOpenRecorder(commandRequest *task);
    OSStatus BlueCloseRecorder(commandRequest *task);
    OSStatus BlueSetRecorderFormat(commandRequest *task);
    OSStatus BlueStartRecorder(commandRequest *task);
    OSStatus BlueStopRecorder(commandRequest *task);

    OSStatus BlueOpenPlayer(commandRequest *task);
    OSStatus BlueClosePlayer(commandRequest *task);
    OSStatus BlueSetPlayerFormat(commandRequest *task);
    OSStatus BluePlaySample(commandRequest *task);
    OSStatus BlueStopPlayer(commandRequest *task);
    OSStatus BlueIsPlaying(commandRequest *task);
    OSStatus BlueWaitForPlayCompletion(commandRequest *task);
};

// This is called by the main thread to scarf up the command list and do
// something about the commands.
//
pascal OSStatus PollCommandQueue(void)
{
    OSStatus err = noErr;
    OTLink *commandList;

    // First, obligate clients to dispatch a new event
    OTClearLock(&gBlueCommandIsRunning);
    // Now, check the queue.
    commandList = OTReverseList(OTLIFOStealList(&gBlueCommandList));
    while (commandList != 0) {
        // The link is the first element of the command request structure
        commandRequest *task = (commandRequest*)commandList;
        // point to next command
        commandList = commandList->fNext;
        // Now do it.
        switch(task->m_command) {
        case kOpenRecorder:
            task->m_status = BlueOpenRecorder(task);
            break;
        case kCloseRecorder:
            task->m_status = BlueCloseRecorder(task);
            break;
        case kSetFormatRecorder:
            task->m_status = BlueSetRecorderFormat(task);
            break;
        case kStartRecorder:
            task->m_status = BlueStartRecorder(task);
            break;
        case kStopRecorder:
            task->m_status = BlueStopRecorder(task);
            break;
        case kOpenPlayer:
            task->m_status = BlueOpenPlayer(task);
            break;
        case kSetFormatPlayer:
            task->m_status = BlueSetPlayerFormat(task);
            break;
        case kClosePlayer:
            task->m_status = BlueClosePlayer(task);
            break;
        case kPlaySample:
            // THIS ONE IS SPECIAL
            err = BluePlaySample(task);
            if (err == 0) {
                // Then a callback was registered and we MUST NOT further
                // molest the task object.
                task = 0;
            } else task->m_status = err;
            break;
        case kStopPlayer:
            task->m_status = BlueStopPlayer(task);
            break;
        case kIsPlaying:
            task->m_status = BlueIsPlaying(task);
            break;
        case kWaitForPlayCompletion:
            // THIS ONE IS SPECIAL
            err = BlueWaitForPlayCompletion(task);
            if (err == 0) {
                // Then a callback was registered and we MUST NOT further
                // molest the task object.
                task = 0;
            } else task->m_status = err;
            break;
        default:
            assert(!"bogus command");
        }
        // signal completion
        if(task)
            MPSignalSemaphore(task->m_done);
    }
    return noErr;
}
namespace 
{
    // Event handler
    // ignore all the arguments, we know what's in them.
    pascal OSStatus MyQueueEventHandler(EventHandlerCallRef, EventRef,
                                        void *)
    {
        return PollCommandQueue();
    }

// Audio recording and video capture go through a QuickTime sequence grabber.
// There is still some magic remaining to be done to properly communicate the
// audio and video information from the main application down to this layer;
// unfortunately, OpenH323 and PWLIB are going to tell us about audio and
// video separately, whereas the Sequence Grabber would rather learn about
// them simultaneously.  For now, I'm only worried about audio capture through
// "the default input", so I can punt worrying about that.

    OSStatus BlueOpenRecorder(commandRequest *task)
    {
        OSStatus err;
        err = SequenceGrabberOpenRecorder((const char *)task->m_arg1,
                                          (const soundParams *)task->m_arg2,
                                          &task->m_result);
        return err;
    }

    OSStatus BlueCloseRecorder(commandRequest *task)
    {
        OSStatus err = SequenceGrabberCloseRecorder(task->m_arg1);
        return err;
    }

    OSStatus BlueSetRecorderFormat(commandRequest *task)
    {
        OSStatus err = SequenceGrabberRecorderParams(task->m_arg1,
                                                     (const soundParams *)task->m_arg2);
        return err;
    }

    OSStatus BlueStartRecorder(commandRequest *task)
    {
        OSStatus err = SequenceGrabberStartRecorder(task->m_arg1,
                                                    (JRingBuffer *)task->m_arg2);
        return err;
    }

    OSStatus BlueStopRecorder(commandRequest *task)
    {
        return SequenceGrabberStopRecorder(task->m_arg1);
    }

// Audio output goes through the Sound Mangler, which (of course) is still
// coop-thread-only.  See above about device selection.

    OSStatus BlueOpenPlayer(commandRequest *task)
    {
        OSStatus err = SoundManagerOpenPlayer((const char *)task->m_arg1,
                                              (const soundParams *)task->m_arg2,
                                              &task->m_result);
        return err;
    }

    OSStatus BlueClosePlayer(commandRequest *task)
    {
        OSStatus err = SoundManagerClosePlayer(task->m_arg1);
        return err;
    }

    OSStatus BlueSetPlayerFormat(commandRequest *task)
    {
        OSStatus err = SoundManagerSetPlayerFormat(task->m_arg1,
                                                   (const soundParams *)task->m_arg2);
        return err;
    }

    OSStatus BluePlaySample(commandRequest *task)
    {
        OSStatus err = SoundManagerPlaySample(task->m_arg1,
                                              (const unsigned char *)task->m_arg2,
                                              task->m_arg3,
                                              &task->m_done);
        return err;
    }

    OSStatus BlueStopPlayer(commandRequest *task)
    {
        OSStatus err = SoundManagerStopPlayer(task->m_arg1);
        return err;
    }

    OSStatus BlueIsPlaying(commandRequest *task)
    {
        OSStatus err = SoundManagerIsPlaying(task->m_arg1, &task->m_result);
        return err;
    }

    static void TickleTaskSemaphore(void *vptask)
    {
        MPSignalSemaphore(reinterpret_cast<commandRequest*>(vptask)->m_done);
    }

    OSStatus BlueWaitForPlayCompletion(commandRequest *task)
    {
        OSStatus err;
        // Oh, I will burn for this code, I know.  But I don't want to allocate
        // extra storage.
        // All that is of interest to the originator of the request is the status
        // field, and all that is of interest to us at this point is the semaphore;
        // both of these are safely far down in the commandRequest structure that
        // we can cadge a function pointer and an (unused) void* pointer from
        // the front.
        unsigned long arg1 = task->m_arg1;
        callbackThunk *thunk = reinterpret_cast<callbackThunk *>(task);
        thunk->func = TickleTaskSemaphore;
        thunk->argument = task;
        err = SoundManagerWaitForPlayCompletion(arg1, thunk);
        return err;
    }

// idle event timer
    pascal void  MyIdleEventHandler(EventLoopTimerRef, void *)
    {
        // If anything appears in the AppQueue, the App object must have exited.
        OSStatus err = MPWaitOnQueue(pwlibAppQueueID,
                                     (void **)0, (void **)0, (void **)0,
                                     kDurationImmediate);
        if (err == 0) {
            // got one!
            MPDeleteQueue(pwlibAppQueueID);
            pwlibAppQueueID = 0;
            // Notify main loop with special message and a 
            // getting-hit-over-the-head-lesson message.
            EventRef theQueueEvent;
            CreateEvent(NULL, kEventClassPwlib, kEventPwlibPProcExit, 0,
                        kEventAttributeUserEvent, &theQueueEvent);
            (void)PostEventToQueue(GetMainEventQueue(),
                                   theQueueEvent, kEventPriorityStandard);
            // And a generic Quit (XXX should this be an AppleEvent?)
            QuitApplicationEventLoop();
        }
        YieldToAnyThread();
        SequenceGrabberIdle();
    }
 
};

extern "C" long MacInitialisePWLibEvents(void)
{
    static int already_initialised = 0;
    if (already_initialised)
        return 0;
    // we should come through here before any other threads are created
    already_initialised = 1;

    // XXX OS X only, probably
    char *dbt = getenv("DebugMPThreads");
    if (dbt) {
        debug_mpthreads = atoi(dbt);
    }

    // First, check in with Multiprocessing Services
    if (!MPLibraryIsLoaded()) {
        fprintf(stderr,"Multiprocessing Services not available.  Sorry!\n");
        exit(1);
    }

    // Register handler for custom event
    EventTypeSpec  eventTypes[1];

    eventTypes[0].eventClass = kEventClassQueueBusy;
    eventTypes[0].eventKind = kEventQueueBusy;
    queueHandlerUPP = NewEventHandlerUPP(MyQueueEventHandler);
    InstallApplicationEventHandler(queueHandlerUPP,
                                   1, eventTypes,
                                   NULL, &queueHandlerRef);
    // Create SGIdle timer
    idleHandlerUPP = NewEventLoopTimerUPP(MyIdleEventHandler);
    InstallEventLoopTimer(GetMainEventLoop(),
                          0., kEventDurationSecond / 100.,
                          idleHandlerUPP, (void *)0, &timerRef);
    // You've called us on the Blue thread, right?
    SequenceGrabberInitialize();
    SoundManagerInitialize();
    // We should now be copacetic
    return 0;
}

extern "C" long MacTeardownPWLibEvents(void) 
{
    RemoveEventHandler(queueHandlerRef);
    DisposeEventHandlerUPP(queueHandlerUPP);
    RemoveEventLoopTimer(timerRef);
    DisposeEventLoopTimerUPP(idleHandlerUPP);
    SoundManagerTeardown();
    SequenceGrabberTeardown();
    return 0;
}

extern "C" long MacWaitForPProcess(Duration dur)
{
    long err = noErr;
    // Wait for the application to die
    // XXX the main thread handles all timer events, right?
    if (pwlibAppQueueID) {
        err = MPWaitOnQueue(pwlibAppQueueID,
                            (void **)0, (void **)0, (void **)0,
                            dur);
        if (err == 0) {
            MPDeleteQueue(pwlibAppQueueID);
            pwlibAppQueueID = 0;
        }
    }
    return err;
}

extern "C" int SpawnProcessInContext( callback_api_c trampoline )
{
    int err;
//    MacInitialisePWLibEvents();
    err = MPCreateQueue(&pwlibAppQueueID);
    if (err) return err;
    
    err = MPCreateTask( trampoline, (void*)0,
                        65536, // stacksize
                        pwlibAppQueueID,
                        (void *)0,
                        (void *)0,
                        0, // no options
                        &ApplicationID);
    if (err) return err;

    // RunApplicationEventLoop();
    return 0;
}
