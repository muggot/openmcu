//
//
// Upper interface declarations for MacMain functions
//
#ifndef MACMAINIF_H_
#define MACMAINIF_H 1

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

long MacInitialisePWLibEvents(void);
long MacTeardownPWLibEvents(void);
// technically, CALLBACK_API_C returns OSStatus, but that's a long
typedef long(*callback_api_c)(void*);
int SpawnProcessInContext( callback_api_c trampoline );
// The shutdown dance is long and complicated.
// Actually, it's not.  When (or shortly after) the PProcess terminates,
// three things happen:
// The extern variable pwlibAppQueueID gets set to zero;
// the main application event loop receives a special event call;
// and the main application receives a Quit event.
// Here's the special event:
#define kEventClassPwlib    'PWLB'
#define kEventPwlibPProcExit    0
// Here's the variable:
extern MPQueueID pwlibAppQueueID;
// And here's a function you can call to wait for the application PProcess
// to exit (assuming you've already suggested to it that it should).
long MacWaitForPProcess(Duration);

END_EXTERN_C
#endif
