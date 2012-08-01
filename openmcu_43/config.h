
#ifndef _OpenMCU_CONFIG_H
#define _OpenMCU_CONFIG_H

// enable test rooms for video mixer
#define ENABLE_TEST_ROOMS   1

// enable echo room for video mixer
#define ENABLE_ECHO_MIXER   1

// enable video if OpenH323 has been compiled with video support
#include <h323.h>
#ifndef NO_H323_VIDEO
#define OPENMCU_VIDEO   1
#endif

#undef P_SSL

#endif // _OpenMCU_CONFIG_H

