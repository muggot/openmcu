//
// (c) 2000 Yuri Kiryanov, yuri@kiryanov.com
// and Yuriy Gorvitovskiy
// 
// Windows CE port of OpenH323 Open Source Project, www.openh323.org
// PWLib extras

//
// errno.h - system wide error numbers (set by system calls)
//      [Microsoft]
//      [System V]
//
//      [Public]
//

#ifndef _INC_ERRNO
#define _INC_ERRNO

#ifdef  __cplusplus

#include <winbase.h>

extern "C" {
#endif

/* declare reference to errno */

#define errno			GetLastError()
#define set_errno(err)	SetLastError( err)

/* Error Codes */

#define EPERM           1
#define ENOENT          2
#define ESRCH           3
#define EINTR           4
#define EIO             5
#define ENXIO           6
#define E2BIG           7
#define ENOEXEC         8
#define EBADF           9
#define ECHILD          10
#define EAGAIN          11
#define ENOMEM          12
#define EACCES          13
#define EFAULT          14
#define EBUSY           16
#define EEXIST          17
#define EXDEV           18
#define ENODEV          19
#define ENOTDIR         20
#define EISDIR          21
#define EINVAL          22
#define ENFILE          23
#define EMFILE          24
#define ENOTTY          25
#define EFBIG           27
#define ENOSPC          28
#define ESPIPE          29
#define EROFS           30
#define EMLINK          31
#define EPIPE           32
#define EDOM            33
#define ERANGE          34
#define EDEADLK         36

#ifndef ENAMETOOLONG
#define ENAMETOOLONG    38
#endif

#define ENOLCK          39
#define ENOSYS          40
#ifndef ENAMETOOLONG
#define ENOTEMPTY       41
#endif

#define EILSEQ          42

/*
 * Support EDEADLOCK for compatibiity with older MS-C versions.
 */
#define EDEADLOCK       EDEADLK

#ifdef  __cplusplus
}
#endif

#endif  /* _INC_ERRNO */
