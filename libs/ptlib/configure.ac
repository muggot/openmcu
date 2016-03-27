AC_INIT(include/ptlib.h)

#AC_CONFIG_SUBDIRS(plugins)

AC_PROG_CXX

AC_LANG(C++)

dnl ########################################################################
dnl set the PTLIB directory to the current directory

PTLIBDIR=`pwd`
AC_SUBST(PTLIBDIR)
dnl set the PTLib installation dir for ptlib-config
AC_SUBST(PTINSTDIR)

dnl ########################################################################
dnl set the PREFIX accordingly
if test "x$prefix" = "xNONE"; then
   INSTALLPREFIX="/usr/local"
else
   INSTALLPREFIX="${prefix}"
fi

AC_SUBST(INSTALLPREFIX)


dnl ########################################################################
dnl set LIBDIR accordingly
LIBDIR=${libdir}
AC_SUBST(LIBDIR)


dnl ########################################################################
dnl extract the PTLib version
dnl MSWIN_DISPLAY version,Version
dnl MSWIN_VERSION version,PTLIB_MAJOR=MAJOR_VERSION
dnl MSWIN_VERSION version,PTLIB_MINOR=MINOR_VERSION
dnl MSWIN_VERSION version,PTLIB_BUILD=BUILD_NUMBER
dnl MSWIN_VERSION version,PTLIB_VERSION=VERSION

MAJOR_VERSION=`cat ${PTLIBDIR}/version.h | grep MAJOR_VERSION | cut -f3 -d' '`
MINOR_VERSION=`cat ${PTLIBDIR}/version.h | grep MINOR_VERSION | cut -f3 -d' '`
BUILD_NUMBER=`cat ${PTLIBDIR}/version.h | grep BUILD_NUMBER | cut -f3 -d' '`
PTLIB_VERSION="${MAJOR_VERSION}.${MINOR_VERSION}.${BUILD_NUMBER}"
BUILD_TYPE=`cat ${PTLIBDIR}/version.h | grep BUILD_TYPE | cut -f 3 -d ' ' | sed 's/BetaCode/-beta/' | sed 's/AlphaCode/-alpha/' | sed 's/ReleaseCode/\./'`

AC_SUBST(MAJOR_VERSION)
AC_SUBST(MINOR_VERSION)
AC_SUBST(BUILD_NUMBER)
AC_SUBST(PTLIB_VERSION)
AC_SUBST(BUILD_TYPE)
AC_SUBST(MAJOR_VERSION, $MAJOR_VERSION)
AC_SUBST(MINOR_VERSION, $MINOR_VERSION)
AC_SUBST(BUILD_NUMBER, $BUILD_NUMBER)

AC_MSG_NOTICE(PTLib version is ${PTLIB_VERSION});

AC_DEFINE_UNQUOTED([PTLIB_MAJOR],   [$MAJOR_VERSION], [Major version])
AC_DEFINE_UNQUOTED([PTLIB_MINOR],   [$MINOR_VERSION], [Minor version])
AC_DEFINE_UNQUOTED([PTLIB_BUILD],   [$BUILD_NUMBER], [Build number])
AC_DEFINE_UNQUOTED([PTLIB_VERSION], ["$PTLIB_VERSION"], [PTLib version])

dnl ########################################################################
dnl set the OSTYPE and MACHTYPE

AC_CANONICAL_TARGET()

TARGETDIR=unix
OSTYPE=
NEEDS_PTHREADS=yes

case "$target_os" in
  linux*)          OSTYPE=linux ;  
                   OSRELEASE="\"`uname -r`\"";
		   OS_TAG="P_LINUX" ;
                   need_pragma=yes ;
                   ;;

  freebsd* )       OSTYPE=FreeBSD ;
		   OS_TAG="P_FREEBSD" ;
                   if test "x$OSRELEASE" = "x"; then
                     OSRELEASE="`sysctl -n kern.osreldate`" ;
                   fi
                   need_pragma=yes ;
		   ;;

  openbsd* )       OSTYPE=OpenBSD ;
		   OS_TAG="P_OPENBSD" ;
                   OSRELEASE="`sysctl -n kern.osrevision`" ;
		   ENDLDLIBS="-lossaudio" ;
                   need_pragma=yes ;
		   ;;

  netbsd* )        OSTYPE=NetBSD ;
		   OS_TAG="P_NETBSD" ;
                   OSRELEASE="`/sbin/sysctl -n kern.osrevision`" ;
		   ENDLDLIBS="-lossaudio" ;
                   need_pragma=yes ;
		   ;;

  solaris* | sunos* ) OSTYPE=solaris ;
		   OS_TAG="P_SOLARIS" ;
                   OSRELEASE=`uname -r | sed "s/5\.//g"` ;
                   need_pragma=yes ;
		   ;;

  darwin* )	   OSTYPE=Darwin ;
		   OS_TAG="P_MACOSX" ;
		   OSRELEASE="`uname -r | sed 's/\.//g'`" ;
                   SHAREDLIBEXT="dylib";
                   if test -e /Developer/SDKs/MacOSX10.4u.sdk/usr/X11R6/include/expat.h ; then
                     EXPATDIR=/Developer/SDKs/MacOSX10.4u.sdk/usr/X11R6
                   fi
		   ;;

  beos*)           OSTYPE=beos ;
                   OS_TAG="P_BEOS" ;
                   OSRELEASE=`uname -r` ;
                   STDCCFLAGS="$STDCCFLAGS -D__BEOS__"
		   ;;

  cygwin*)         OSTYPE=cygwin ;
                   OS_TAG="P_CYGWIN" ;
                   OSRELEASE="`uname -r`";
		   ;;

  mingw*)	   OSTYPE=mingw ;
		   OS_TAG="P_MINGW" ;
		   OSRELEASE=`uname -r` ;
		   TARGETDIR=msos ;
		   SHAREDLIBEXT="dll" ;
		   STDCCFLAGS="$STDCCFLAGS -mms-bitfields" ;
		   ENDLDLIBS="-lwinmm -lwsock32 -lsnmpapi -lmpr -lcomdlg32 -lgdi32 -lavicap32" ;
		   NEEDS_PTHREADS=no ;
		   ;;

  * )		   OSTYPE="$target_os" ;
		   OS_TAG="P_UNKNOWN_OS" ;
  		   OSRELEASE="\"${OSTYPE}_`uname -r`\"" ;
		   AC_MSG_WARN("OS $target_os not recognized - proceed with caution!") ;
		   ;;
esac

MACHTYPE=

case "$target_cpu" in
   x86 | i686 | i586 | i486 | i386 ) MACHTYPE=x86
                   ;;

   x86_64)	   MACHTYPE=x86_64 ;
		   P_64BIT=1 ;
                   LIB64=1 ;
		   ;;

   amd64)	   MACHTYPE=amd64 ;
		   P_64BIT=1 ;
                   LIB64=1 ;
		   ;;

   alpha | alphaev56 | alphaev6 | alphaev67 | alphaev7) MACHTYPE=alpha ;
		   P_64BIT=1 ;
		   ;;

   hppa )          MACHTYPE=hppa ;
		   ;;

   hppa64 )        MACHTYPE=hppa64 ;
		   P_64BIT=1 ;
                   LIB64=1 ;
		   ;;

   sparc )         MACHTYPE=sparc ;
		   ;;

   powerpc )       MACHTYPE=ppc ;
		   ;;

   ppc )           MACHTYPE=ppc ;
		   ;;

   powerpc64*)     MACHTYPE=ppc64 ;
		   P_64BIT=1 ;
                   LIB64=1 ;
		   ;;

   ppc64*)         MACHTYPE=ppc64 ;
		   P_64BIT=1 ;
                   LIB64=1 ;
		   ;;

   ia64)	   MACHTYPE=ia64 ;
		   P_64BIT=1 ;
	  	   ;;

   s390x)	   MACHTYPE=s390x ;
		   P_64BIT=1 ;
                   LIB64=1 ;
		   ;;

   s390)	   MACHTYPE=s390 ;
		   ;;

   aarch64)	   MACHTYPE=aarch64
		   P_64BIT=1
		   LIB64=1
		   ;;

   * )		   MACHTYPE=$target_cpu ;
		   AC_MSG_WARN("CPU $target_cpu not recognized - proceed with caution!") ;;
esac

AC_MSG_NOTICE(OSTYPE set to $OSTYPE);
AC_MSG_NOTICE(OSRELEASE set to $OSRELEASE);
AC_MSG_NOTICE(MACHTYPE set to $MACHTYPE);

AC_SUBST(MACHTYPE,  $MACHTYPE)
AC_SUBST(OSTYPE,    $OSTYPE)
AC_SUBST(OSRELEASE, $OSRELEASE)
AC_SUBST(TARGETDIR, $TARGETDIR)

AC_DEFINE_UNQUOTED($OS_TAG, $OSRELEASE)

AC_DEFINE([PHAS_TEMPLATES], [1], [Templates found])

if test "x$P_64BIT" != "x" ; then
  AC_DEFINE([P_64BIT], [1], [64bit architecture])
fi

dnl BeOS has it;s own threading system
if test ${OSTYPE} != beos ; then
  AC_DEFINE(P_PTHREADS, 1, "PThreads found")
fi

dnl solaris has it's own ldap library
if test ${OSTYPE} != solaris ; then
   LDAP_LIBRARIES="-llber -lldap_r"
fi

dnl ########################################################################
dnl get the gcc version

USE_GCC=
USE_PCH=

if test x$GXX = xyes ; then
  USE_GCC=yes
  gcc_version=`$CXX -dumpversion`
  AC_MSG_NOTICE(gcc version is $gcc_version);
  GXX_MAJOR=`echo $gcc_version | sed 's/\..*$//'`
  GXX_MINOR=[`echo $gcc_version | sed -e 's/[0-9][0-9]*\.//' -e 's/\..*$//'`]
  GXX_PATCH=[`echo $gcc_version | sed -e 's/[0-9][0-9]*\.[0-9][0-9]*\.//' -e 's/\..*$//'`]

  dnl only enable pragma for gcc 4.0 and later
  AC_MSG_CHECKING(checking if pragma implementation should be used)
  if test ${GXX_MAJOR} -lt 4 ; then
    if test "${need_pragma}" = "yes" ; then
      STDCCFLAGS="$STDCCFLAGS -DP_USE_PRAGMA"
      AC_MSG_RESULT(yes)
    else
      AC_MSG_RESULT(no)
    fi
  else
    AC_MSG_RESULT(no)
  fi

  dnl only enable PCH for gcc 4.0 and later
  AC_MSG_CHECKING(checking if precompiled headers available)
  if test ${GXX_MAJOR} -lt 4 ; then
    enablepch=no
  else
    enablepch=yes
    AC_ARG_ENABLE(pch,
         [  --enable-pch             disable precompiled headers],
         enablepch=$enableval)
  fi
  if test "${enablepch}" = "yes" ; then
    AC_MSG_RESULT(yes)
    USE_PCH=1
  else
    AC_MSG_RESULT(no)
  fi
else
  USE_GCC=no
fi

AC_SUBST(USE_GCC)
AC_SUBST(USE_PCH)

dnl ########################################################################
dnl check if precompiled headers available, or disabled

dnl ########################################################################
dnl set the shared library extension, if not already set

if test "x${SHAREDLIBEXT}" = "x" ; then
  SHAREDLIBEXT="so"
fi
AC_SUBST(SHAREDLIBEXT, $SHAREDLIBEXT)
AC_DEFINE([P_SHAREDLIBEXT], [$SHAREDLIBEXT], [shared library extension])

dnl ########################################################################
dnl check the endianness

dnl MSWIN_DISPLAY byteorder,Byte Order
dnl MSWIN_DEFINE  byteorder,PBYTE_ORDER=PLITTLE_ENDIAN

if test ${OSTYPE} != "Darwin" ; then  
  AC_C_BIGENDIAN()
  if test ${ac_cv_c_bigendian} = yes ; then
    PBYTE_ORDER=PBIG_ENDIAN
  else
    PBYTE_ORDER=PLITTLE_ENDIAN
  fi

  AC_DEFINE_UNQUOTED(PBYTE_ORDER, $PBYTE_ORDER, "byte order")
fi

dnl ########################################################################
dnl look for compile flags
AC_LANG_PUSH(C++)

AC_MSG_CHECKING(if linker accepts -felide-constructors)
AC_LANG_CONFTEST([[int main(int ac,char **av) { return 0; }]])
$CXX $CXXFLAGS -felide-constructors conftest.cc > /dev/null 2>&1
if test $? = 0 ; then
  AC_MSG_RESULT(yes)
  STDCXXFLAGS="$STDCXXFLAGS -felide-constructors"
else
  AC_MSG_RESULT(no)
fi

AC_MSG_CHECKING(if linker accepts -Wreorder)
AC_LANG_CONFTEST([[int main(int ac,char **av) { return 0; }]])
$CXX $CXXFLAGS -Wreorder conftest.cc > /dev/null 2>&1
if test $? = 0 ; then
  AC_MSG_RESULT(yes)
  STDCXXFLAGS="$STDCXXFLAGS -Wreorder"
else
  AC_MSG_RESULT(no)
fi


dnl add additional information for the debugger to ensure the user can indeed
dnl debug coredumps and macros.

AC_MSG_CHECKING(if debug build accepts -g3 -ggdb -O0)
AC_LANG_CONFTEST([[int main(int ac,char **av) { return 0; }]])
$CXX  $CXXFLAGS  -g3 -ggdb -O0 conftest.cc > /dev/null 2>&1
if test $? = 0 ; then
  AC_MSG_RESULT(yes)
  DEBUG_FLAG="-g3 -ggdb -O0"
else
  AC_MSG_RESULT(no)
  DEBUG_FLAG="-g"
fi


# AC_MSG_CHECKING(if linker accepts -Woverloaded-virtual)
# AC_LANG_CONFTEST([[int main(int ac,char **av) { return 0; }]])
# $CXX $CFLAGS -Woverloaded-virtual conftest.cc > /dev/null 2>&1
# if test $? = 0 ; then
#   AC_MSG_RESULT(yes)
#   STDCCFLAGS="$STDCCFLAGS -Woverloaded-virtual"
# else
#   AC_MSG_RESULT(no)
# fi

AC_MSG_CHECKING(if compiler uses RTTI by default)
AC_COMPILE_IFELSE([[
#include <typeinfo>
int main(int ac,char **av)
{
  class A
  {
    public:
      virtual void fred() { }
  };
  class B : public A
  {
    public:
      void fred() { }
  };
  B t;
  A * p = dynamic_cast<A *>(&t);
  return 0;
}
]],DEFAULT_RTTI=1, DEFAULT_RTTI=0)
if test "$DEFAULT_RTTI" = "1" ; then
  AC_MSG_RESULT(yes)
else
  AC_MSG_RESULT(no)
  STDCXXFLAGS="$STDCCFLAGS -rtti"
fi

GCSECTIONS=0

dnl Remove --gc-sections as the linker crashes when linking
dnl static executables with this option
dnl AC_MSG_CHECKING(if linker accepts --gc-sections)
dnl C_LANG_CONFTEST([[int main(int ac,char **av) { return 0; }]])
dnl CXX $CFLAGS -Wl,--gc-sections conftest.cc > /dev/null 2>&1
dnl if test $? = 0 ; then

if test 0 = 1 ;  then

  dnl  AC_MSG_RESULT(yes)

  AC_MSG_CHECKING(if compiler accepts -ffunction-sections)
  AC_LANG_CONFTEST([[int main(int ac,char **av) { return 0; }]])
  $CXX $CFLAGS -ffunction-sections -c conftest.cc > /dev/null 2>&1
  if test $? = 0 ; then
    AC_MSG_RESULT(yes)
    STDCCFLAGS="$STDCCFLAGS -ffunction-sections"
    GCSECTIONS=1
  else
    AC_MSG_RESULT(no)
  fi

  dnl remove data-sections as not only does is case the linker to
  dnl crash on some platforms, but seems to be broken w.r.t
  dnl closure of templates with static member functions
  dnl see http://gcc.gnu.org/bugzilla/show_bug.cgi?id=8249
  dnl
  dnl AC_MSG_CHECKING(if compiler accepts -fdata-sections)
  dnl AC_LANG_CONFTEST([[int main(int ac,char **av) { return 0; }]])
  dnl $CXX $CFLAGS -fdata-sections -c conftest.cc > /dev/null 2>&1
  dnl if test $? = 0 ; then
  dnl   AC_MSG_RESULT(yes)
  dnl   STDCCFLAGS="$STDCCFLAGS -fdata-sections"
  dnl   GCSECTIONS=1
  dnl else
  dnl   AC_MSG_RESULT(no)
  dnl fi

  dnl as much as this flag would help, there is no known linker
  dnl that can handle PTLib when it has been compiled with it
  dnl AC_MSG_CHECKING(if compiler accepts -fvtable-gc)
  dnl AC_LANG_CONFTEST([[int main(int ac,char **av) { return 0; }]])
  dnl $CXX $CFLAGS -fvtable-gc -c conftest.cc > /dev/null 2>&1
  dnl if test $? = 0 ; then
  dnl   AC_MSG_RESULT(yes)
  dnl   STDCCFLAGS="$STDCCFLAGS -fvtable-gc"
  dnl   GCSECTIONS=1
  dnl else
  dnl   AC_MSG_RESULT(no)
  dnl fi

  if test $GCSECTIONS = 1; then
    if test "${OSTYPE}" != "Darwin" ; then
      P_STATIC_LDFLAGS="$P_STATIC_LDFLAGS -Wl,--gc-sections"
    fi
  fi
fi


AC_LANG_POP(C++)

dnl ########################################################################
dnl check for the size of int
AC_CHECK_SIZEOF(int)

dnl ########################################################################
dnl check for long double

AC_C_LONG_DOUBLE
if test $ac_cv_c_long_double != yes ; then
  AC_DEFINE([PNO_LONG_DOUBLE], [], [No long double])
fi

if test ${OSTYPE} = Darwin ; then
  STDCCFLAGS="$STDCCFLAGS -Wno-long-double"
fi

dnl ########################################################################
dnl we need flex, so we better make sure it's there
AC_PATH_PROG(FLEX, flex)
if test -z "$FLEX" ; then
        AC_MSG_ERROR(PTLib needs the GNU Project fast lexical analyzer generator flex, 1)
fi

dnl ########################################################################
dnl most systems have "ar", but some use "gar"
AC_PATH_PROG(AR, ar)
if test -z "$AR" ; then
  AC_PATH_PROG(AR, gar)
fi

dnl ########################################################################
dnl we need bison, so we better make sure it's there
AC_PATH_PROG(BISON, bison)

dnl ########################################################################
dnl check for number of parms to readdir
AC_MSG_CHECKING(if readdir_r has 2 parms)
AC_TRY_COMPILE([#define _POSIX_PTHREAD_SEMANTICS
                #include <dirent.h>],
               [DIR *s; struct dirent * e; readdir_r(s, e);], P_HAS_POSIX_READDIR_R=2, P_HAS_POSIX_READDIR_R=0)
if test ${P_HAS_POSIX_READDIR_R} = 2 ; then
  AC_MSG_RESULT(yes)
else
  AC_MSG_RESULT(no)
  AC_MSG_CHECKING(if readdir_r has 3 parms)
  AC_TRY_COMPILE([#include <dirent.h>],
                 [DIR *s; struct dirent * e; struct dirent * p; readdir_r(s, e, &p);], P_HAS_POSIX_READDIR_R=3, P_HAS_POSIX_READDIR_R=0)
  if test ${P_HAS_POSIX_READDIR_R} = 3 ; then
    AC_MSG_RESULT(yes)
  else
    AC_MSG_RESULT(no)
  fi
fi
AC_DEFINE_UNQUOTED(P_HAS_POSIX_READDIR_R, ${P_HAS_POSIX_READDIR_R})

dnl ########################################################################
dnl check for minimum sizing

AC_ARG_ENABLE(minsize,
       [  --enable-minsize        minimise the size of the lib by disabling least used options])
AC_ARG_ENABLE(openh323,
       [  --enable-openh323       include only options needed with openh323])
AC_ARG_ENABLE(opal,
       [  --enable-opal           include only options needed with opal])

dnl these are the default configure settings
default_resolver=yes
default_openldap=no
default_openssl=yes
default_odbc=no
default_expat=no
default_sdl=yes
default_sasl=yes
default_plugins=no
default_tts=yes
default_asn=yes
default_stun=yes
default_pipechan=yes
default_dtmf=yes
default_wavfile=yes
default_socks=yes
default_ftp=yes
default_snmp=yes
default_telnet=yes
default_remconn=no
default_serial=yes
default_pop3smtp=yes
default_audio=yes
default_video=yes
default_http=yes
default_httpsvc=yes
default_soap=yes
default_xmlrpc=yes
default_jabber=yes
default_configfile=yes
default_alsa=yes
default_oss=yes
default_ipv6=yes
default_qos=yes
default_sockagg=no
default_vidfile=yes
default_shmvideo=yes
default_vxml=yes

if test "${enable_openh323}x" = "yesx" -o "${enable_opal}x" = "yesx" ; then
  default_resolver=yes
  default_openldap=no
  default_openssl=yes
  default_odbc=no
  default_expat=no
  default_sdl=yes
  default_sasl=no
  default_plugins=no
  default_tts=yes
  default_asn=yes
  default_stun=yes
  default_pipechan=yes
  default_dtmf=yes
  default_wavfile=yes
  default_socks=no
  default_ftp=no
  default_snmp=no
  default_telnet=no
  default_remconn=no
  default_serial=no
  default_pop3smtp=no
  default_audio=yes
  default_video=yes
  default_http=yes
  default_httpsvc=no
  default_soap=no
  default_xmlrpc=no
  default_jabber=no
  default_configfile=yes
  default_alsa=yes
  default_oss=yes
  default_ipv6=yes
  default_qos=yes
  default_sockagg=no
  default_vidfile=yes
  default_shmvideo=no
  default_vxml=yes
elif test "${enable_minsize}x" = "yesx" ; then
  default_resolver=no
  default_openldap=no
  default_openssl=yes
  default_odbc=no
  default_expat=no
  default_sdl=no
  default_sasl=no
  default_plugins=no
  default_tts=no
  default_asn=yes
  default_stun=yes
  default_pipechan=no
  default_dtmf=no
  default_wavfile=no
  default_socks=no
  default_ftp=no
  default_snmp=no
  default_telnet=no
  default_remconn=no
  default_serial=no
  default_pop3smtp=no
  default_audio=yes
  default_video=yes
  default_http=yes
  default_httpsvc=no
  default_soap=no
  default_xmlrpc=no
  default_jabber=no
  default_configfile=no
  default_alsa=yes
  default_oss=no
  default_ipv6=no
  default_qos=no
  default_sockagg=no
  default_vidfile=no
  default_shmvideo=no
  default_vxml=no
fi


dnl ########################################################################
dnl check for recvmsg

P_HAS_RECVMSG=
AC_MSG_CHECKING(for recvmsg)
AC_TRY_COMPILE([#include <sys/types.h>
                #include <sys/socket.h>
                #include <netinet/in.h>],
               [int t = SOL_IP; t = IP_PKTINFO; struct msghdr *msg; recvmsg(0, msg, 0);], P_HAS_RECVMSG=1)
if test "${P_HAS_RECVMSG}" = "1" ; then
  AC_MSG_RESULT(yes)
  AC_DEFINE(P_HAS_RECVMSG, 1)
else
  AC_MSG_RESULT(no)
fi

dnl ########################################################################
dnl check for streams types

P_HAS_STL_STREAMS=0
AC_MSG_CHECKING(if using STL streams)
AC_TRY_COMPILE([#include <iostream>],
               [int v = (int)std::ios_base::seekdir();], P_HAS_STL_STREAMS=1)
if test "${P_HAS_STL_STREAMS}" = 1 ; then
  AC_MSG_RESULT(yes)
else
  AC_MSG_RESULT(no)
fi
AC_DEFINE_UNQUOTED(P_HAS_STL_STREAMS, ${P_HAS_STL_STREAMS})

dnl ########################################################################
dnl see if using GNU glibc++, because then we have atomic integer operations

P_HAS_ATOMIC_INT=0
P_NEEDS_GNU_CXX_NAMESPACE=0
AC_MSG_CHECKING(if atomic integer available)
AC_TRY_COMPILE([#include <bits/atomicity.h>],
               [_Atomic_word val;], P_HAS_ATOMIC_INT=1,
    AC_TRY_COMPILE([#include <ext/atomicity.h>],
		[_Atomic_word val;], P_HAS_ATOMIC_INT=2)
)
if test ${P_HAS_ATOMIC_INT} != 0 ; then
  AC_MSG_RESULT(yes)
  AC_MSG_CHECKING(if __exchange_and_add is in __gnu_cxx namespace)
  if test ${P_HAS_ATOMIC_INT} = 1 ; then
    AC_TRY_COMPILE([#include <bits/atomicity.h>],
        [_Atomic_word val; __gnu_cxx::__exchange_and_add(&val, 1)],
        P_NEEDS_GNU_CXX_NAMESPACE=1)
  else
    P_HAS_ATOMIC_INT=1
    AC_TRY_COMPILE([#include <ext/atomicity.h>],
        [_Atomic_word val; __gnu_cxx::__exchange_and_add(&val, 1)],
        P_NEEDS_GNU_CXX_NAMESPACE=1)
  fi
  if test ${P_NEEDS_GNU_CXX_NAMESPACE} = 1 ; then
    AC_MSG_RESULT(yes)
  else
    AC_MSG_RESULT(no)
  fi
else
  AC_MSG_RESULT(no)
fi

AC_DEFINE_UNQUOTED(P_HAS_ATOMIC_INT, ${P_HAS_ATOMIC_INT})
AC_DEFINE_UNQUOTED(P_NEEDS_GNU_CXX_NAMESPACE, ${P_NEEDS_GNU_CXX_NAMESPACE})


dnl ########################################################################
dnl check for select_large_fdset (Solaris)

if test "${OSTYPE:-unknown}" = "solaris"; then
  AC_MSG_CHECKING(checking for FD_SETSIZE)
  _FD_SYSTEM_FILE=/etc/system
  _FD_SETSIZE=`cat $_FD_SYSTEM_FILE | grep rlim_fd_max | cut -c1`
  if test "$_FD_SETSIZE" = "#"; then
    _FD_SETSIZE=4098
  else
    _FD_SETSIZE=`cat $_FD_SYSTEM_FILE | grep rlim_fd_max | cut -f2 -d'='`
    if test "x$_FD_SETSIZE" = "x"; then
      _FD_SETSIZE=4098
    else
      STDCCFLAGS="$STDCCFLAGS -DFD_SETSIZE=$_FD_SETSIZE"
    fi
  fi
  AC_MSG_RESULT($_FD_SETSIZE)
fi


dnl ########################################################################
dnl check if upad128_t is defined (Solaris)

if test "${OSTYPE:-unknown}" = "solaris"; then
  AC_MSG_CHECKING(for upad128_t)
  AC_TRY_COMPILE([#include <sys/types.h>],
                 [upad128_t upad; upad._q = 0.0;], has_upad128_t=y)
  if test ${has_upad128_t} == "y"; then
    AC_MSG_RESULT(yes)
    AC_DEFINE(P_HAS_UPAD128_T)
  else
    AC_MSG_RESULT(no)
  fi
fi


dnl ########################################################################
dnl check for Unix semaphores

dnl posix unnamed semaphore support in darwin/macosx is broken
if test ${OSTYPE} != Darwin ; then
  AC_TRY_COMPILE([#include <semaphore.h>],
                 [sem_t *s; sem_init(s, 0, 0);], P_HAS_SEMAPHORES=1)
  AC_MSG_CHECKING(if Unix semaphores are available)
  if test "${P_HAS_SEMAPHORES}x" != "x" ; then
    AC_DEFINE(P_HAS_SEMAPHORES)
    AC_MSG_RESULT(yes)
  else
    AC_MSG_RESULT(no)
  fi
fi

if test "${P_HAS_SEMAPHORES}x" == "x" ; then
  
  AC_TRY_COMPILE([#include <semaphore.h>],
                 [sem_t *s = sem_open("test", O_CREAT)],
                 P_HAS_NAMED_SEMAPHORES=1 )
  AC_MSG_CHECKING(if named semaphores are available)
  if test "${P_HAS_NAMED_SEMAPHORES}x" != "x"; then
    AC_DEFINE(P_HAS_NAMED_SEMAPHORES)
    AC_MSG_RESULT(yes)
  else
    AC_MSG_RESULT(no)
    AC_MSG_WARN(This host does not support semaphores which are essential)
    AC_MSG_WARN(to implement 'critical sections'. This feature was added)
    AC_MSG_WARN(in PWLib 1.7.0 and improves the stability of multithreaded)
    AC_MSG_WARN(programs. This feature has been disabled for this host)
  fi
fi

dnl ########################################################################
dnl check for pthreads library

dnl MSWIN_DISPLAY semaphore,Semaphore Support
dnl MSWIN_DEFINE  semaphore,P_HAS_SEMAPHORES

dnl Linux, Mac OS X use libpthread and -lpthread
dnl FreeBSD uses the -pthread compiler option and uses libc_r

AC_CHECK_LIB(pthread, pthread_create, HAS_PTHREADS=yes, HAS_PTHREADS=no)
if test ${HAS_PTHREADS} = yes ; then
  ENDLDLIBS="$ENDLDLIBS -lpthread"
  STDCCFLAGS="$STDCCFLAGS -D_REENTRANT"
else
  if test ${OSTYPE} = FreeBSD ; then
    HAS_PTHREADS=yes
    ENDLDLIBS="$ENDLDLIBS -pthread"
    STDCCFLAGS="$STDCCFLAGS -D_REENTRANT -pthread"
  elif test ${OSTYPE} = FreeBSD ; then
    HAS_PTHREADS=yes
    ENDLDLIBS="$ENDLDLIBS -pthread"
    STDCCFLAGS="$STDCCFLAGS -D_REENTRANT -pthread"
  fi
fi


if test ${HAS_PTHREADS} != yes -a ${NEEDS_PTHREADS} = yes; then
  AC_MSG_ERROR(must have pthreads!)
fi

dnl ########################################################################
dnl check for XPG6 compliant pthreads (timed spinlocks)

if test ${HAS_PTHREADS} = yes ; then
  AC_MSG_CHECKING(if pthread_mutex_timedlock is available)
  AC_TRY_COMPILE([#include <pthread.h>],
                  [pthread_mutex_timedlock(NULL, NULL);], HAS_PTHREADS_XPG6=yes, HAS_PTHREADS_XPG6=no)
  if test ${HAS_PTHREADS_XPG6} = yes ; then
    AC_MSG_RESULT(yes)
    AC_DEFINE(P_PTHREADS_XPG6, 1, "XPG6 compliant pthreads found")
  else
    AC_MSG_RESULT(no)
  fi

  AC_CHECK_LIB(pthread, sem_timedwait, HAS_SEMAPHORES_XPG6=yes, HAS_SEMAPHORES_XPG6=no)
  if test ${HAS_SEMAPHORES_XPG6} = yes ; then
    AC_MSG_CHECKING(if sem_timedwait is compilable)
    AC_TRY_COMPILE([#include <semaphore.h>],
                    [sem_t *s; const struct timespec * t; sem_timedwait(s, t);], HAS_SEMAPHORES_XPG6=yes, HAS_SEMAPHORES_XPG6=no)
    if test ${HAS_SEMAPHORES_XPG6} = yes ; then
      AC_DEFINE(P_HAS_SEMAPHORES_XPG6, 1, "Supports XPG6 Semaphores")
      AC_MSG_RESULT(yes)
    else
      AC_MSG_RESULT(no)
    fi
  fi
fi

dnl ########################################################################
dnl check for recursive mutexes
P_HAS_RECURSIVE_MUTEX=0
AC_MSG_CHECKING(if recursive mutexes are available)
AC_TRY_COMPILE([#include <pthread.h>],
               [pthread_mutexattr_t attr; pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE_NP);], P_HAS_RECURSIVE_MUTEX=1)
if test ${P_HAS_RECURSIVE_MUTEX} = 1 ; then
  AC_MSG_RESULT(PTHREAD_MUTEX_RECURSIVE_NP)
else
  AC_TRY_COMPILE([#include <pthread.h>],
	   [pthread_mutexattr_t attr; pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);], P_HAS_RECURSIVE_MUTEX=2)
  if test ${P_HAS_RECURSIVE_MUTEX} = 2 ; then
    AC_MSG_RESULT(PTHREAD_MUTEX_RECURSIVE)
  else
    AC_MSG_RESULT(no)
  fi
fi
AC_DEFINE_UNQUOTED(P_HAS_RECURSIVE_MUTEX, ${P_HAS_RECURSIVE_MUTEX})


dnl ########################################################################
dnl check for regex library

dnl yes this test needs to be outside the if below
dnl because autoconf needs to initialise stuff. without this
dnl all sorts of strange things happen
AC_CHECK_HEADERS(regex.h, HAS_REGEX=yes)

AC_ARG_ENABLE(internalregex,
        [  --enable-internalregex  force internal regex library],
        internalregex=$enableval)
if test "$internalregex" = "yes" ; then
  AC_MSG_NOTICE(Forcing use of internal regex library)
else
  if test "$HAS_REGEX" = "yes" ; then
    AC_CHECK_FUNC(regcomp, HAS_REGEX=yes)
    if test "$HAS_REGEX" = "yes" ; then
      AC_SUBST(HAS_REGEX, 1)
      AC_DEFINE(P_REGEX, 1, "Regex library found")
      if test ${OSTYPE} = mingw ; then
        ENDLDLIBS="$ENDLDLIBS -lregex"
      fi
    fi
  fi
fi


dnl ########################################################################
dnl check for dynamic library code (dlopen and friends)

dnl MSWIN DLL Support,,P_DYNALINK

LIBS=""
HAS_LIBDL=no
P_DYNALINK=0
if test ${OSTYPE} = beos ; then
  P_DYNALINK=1
  AC_DEFINE(P_DYNALINK, 1, "dlopen implemented inside PTLib in BeOS")
elif test ${OSTYPE} = Darwin ; then
  P_DYNALINK=1
  AC_DEFINE(P_DYNALINK, 1, "dlopen implemented inside PTLib in Darwin")
else
  AC_CHECK_HEADERS(dlfcn.h, HAS_DLFCN=yes, HAS_DLFCN=no)
  if test "$HAS_DLFCN" = yes ; then
    AC_MSG_CHECKING(if dlopen is available)
    AC_TRY_COMPILE([#include <dlfcn.h>],
                    [void * p = dlopen("lib", 0);], HAS_DLOPEN=yes, HAS_DLOPEN=no)
    if test "$HAS_DLOPEN" = no ; then
      AC_MSG_RESULT(no)
    else
      AC_MSG_RESULT(yes)
      P_DYNALINK=1
      AC_DEFINE(P_DYNALINK, 1, "dlopen found")
      AC_CHECK_LIB(dl,dlopen,HAS_LIBDL=yes, HAS_LIBDL=no)
    fi
  fi
fi

dnl #########################################################################
dnl check to see if plugins enabled

dnl MSWIN_DISPLAY plugins,Plugin Support
dnl MSWIN_DEFINE  plugins,P_HAS_PLUGINS

plugins="no"
AC_MSG_CHECKING(if plugins support needs to be compiled)
AC_ARG_ENABLE(plugins,
       [  --enable-plugins        enable plugin support])
if test "${enable_plugins}z" = "z" ; then
  enable_plugins=$default_plugins
fi

HAS_PLUGINS=
if test "${enable_plugins}" = "no" ; then
  AC_MSG_RESULT(no)
elif test "${P_DYNALINK:-unset}" = "unset" ; then
  AC_MSG_RESULT(plugins disabled as dlopen not available)
else
  AC_MSG_RESULT(plugins enabled)
  HAS_PLUGINS=1
fi

AC_DEFINE(P_HAS_PLUGINS, 1, "plugin loader enabled")

AC_SUBST(HAS_PLUGINS)

#if test "x$LIB64" != "x" ; then
#  AC_DEFINE_UNQUOTED([P_DEFAULT_PLUGIN_DIR], ["${INSTALLPREFIX}/lib64/ptlib"], [Default plugin directory])
#else
  AC_DEFINE_UNQUOTED([P_DEFAULT_PLUGIN_DIR], ["${INSTALLPREFIX}/lib/ptlib"], [Default plugin directory])
#fi

dnl ########################################################################
dnl look for DNS resolver code (must be done before OpenLDAP)
dnl on some systems, the resolver is in libc
dnl but needs resolv.h included which defines res_search as __res_search

dnl MSWIN_DISPLAY    resolver,DNS Resolver
dnl MSWIN_CHECK_FILE resolver,include\windns.h,P_DNS=1
dnl MSWIN_DIR_SYMBOL resolver,DNS_DIR
dnl MSWIN_CHECK_DIR  resolver,\program files\microsoft visual studio 8\vc\platformsdk\
dnl MSWIN_CHECK_DIR  resolver,\Program Files\Microsoft SDK\
dnl MSWIN_CHECK_DIR  resolver,\Program Files\Microsoft Platform SDK for Windows Server 2003 R2

AC_ARG_ENABLE(resolver,
       [  --disable-resolver      disable DNS resolver support])

if test "${enable_resolver}z" = "z" ; then
  enable_resolver=$default_resolver
fi

HAS_RESOLVER=
if test "$enable_resolver" = "no" ; then
  AC_MSG_NOTICE(DNS resolver disabled)
else

  AC_CHECK_FUNC(res_ninit, HAS_RES_NINIT)
  if test "${HAS_RES_NINIT}z" != "z" ; then
    AC_DEFINE(P_HAS_RES_NINIT)
    HAS_RESOLVER=1
    RESOLVER_LIBS=""
  fi

  if test "${HAS_RESOLVER}z" = "z" ; then
    AC_MSG_CHECKING([for res_ninit in -lresolv])
    LIBS="-lresolv"
    AC_LINK_IFELSE([[
#include <netinet/in.h>
#include <resolv.h>
int main(int argc,char **argv){
 res_state p; res_ninit(p);
}]],HAS_RES_NINIT=1)
    if test "${HAS_RES_NINIT}z" != "z" ; then
      AC_MSG_RESULT(yes)
      AC_DEFINE(P_HAS_RES_NINIT)
      HAS_RESOLVER=1
      RESOLVER_LIBS="-lresolv"
    else
      AC_MSG_RESULT(no)
    fi
    LIBS=""
  fi

  if test "${HAS_RESOLVER}z" = "z" ; then
    AC_CHECK_FUNC(res_search, HAS_RESOLVER=1)
    if test "${HAS_RESOLVER}z" != "z" ; then
      RESOLVER_LIBS=""
    fi
  fi

  if test "${HAS_RESOLVER}z" = "z" ; then
    AC_MSG_CHECKING([for res_search in -lresolv])
    LIBS="-lresolv"
    AC_LINK_IFELSE([[
#include <netinet/in.h>
#include <resolv.h>
int main(int argc,char **argv){
 res_search (NULL, 0, 0, NULL, 0);
}]],HAS_RESOLVER=1)
    if test "${HAS_RESOLVER:-unset}" != "unset" ; then
      AC_MSG_RESULT(yes)
      RESOLVER_LIBS="-lresolv"
    else
      AC_MSG_RESULT(no)
    fi
    LIBS=""
  fi

  if test "${HAS_RESOLVER}z" = "z" ; then
    AC_SEARCH_LIBS(__res_search, resolv, HAS_RESOLVER=1)
  fi

  if test "${HAS_RESOLVER}z" != "z" ; then
    AC_DEFINE(P_DNS, 1, "resolver libraries found")
    HAS_RESOLVER=1
  fi
fi
AC_SUBST(HAS_RESOLVER)

dnl ########################################################################
dnl Look for SASL

dnl MSWIN_DISPLAY    sasl,Cyrus SASL
dnl MSWIN_CHECK_FILE sasl,include\sasl.h,P_SASL=1
dnl MSWIN_DIR_SYMBOL sasl,SASL_DIR
dnl MSWIN_CHECK_DIR  sasl,..\sasl\
dnl MSWIN_CHECK_DIR  sasl,..\cyrus-sasl\
dnl MSWIN_CHECK_DIR  sasl,..\external\sasl\
dnl MSWIN_CHECK_DIR  sasl,..\external\cyrus-sasl\
dnl MSWIN_CHECK_DIR  sasl,\sasl\
dnl MSWIN_CHECK_DIR  sasl,\cyrus-sasl\
dnl MSWIN_CHECK_DIR  sasl,c:\sasl\
dnl MSWIN_CHECK_DIR  sasl,c:\cyrus-sasl\
  
AC_ARG_ENABLE(sasl,
       [  --disable-sasl          disable SASL support])

if test "${enable_sasl}z" = "z" ; then
  enable_sasl=$default_sasl
fi

HAS_SASL=
HAS_SASL2=

if test "$enable_sasl" = "no" ; then
  AC_MSG_NOTICE(SASL disabled)
else 

  AC_MSG_CHECKING([for sasl_version in -lsasl2])
  my_LIBS=$LIBS
  LIBS="$LIBS -lsasl2"			
  AC_LINK_IFELSE([#ifdef __cplusplus
 				  extern "C"
   				  #endif
 				  char sasl_version ();
 				  int
 				  main ()
 				  {
 				   sasl_version ();
   					;
   				   return 0;
 				  }], HAS_SASL2=1, HAS_SASL2= )
  if test "${HAS_SASL2}x" = "x" ; then
	  AC_MSG_RESULT(no)
	  LIBS=$my_LIBS
	  LIBS="$LIBS -lsasl"
	  AC_MSG_CHECKING([for sasl_client_auth in sasl])
      AC_LINK_IFELSE([#ifdef __cplusplus
                  extern "C"
                  #endif
                  char sasl_client_out ();
                  int
                  main ()
                  {
                   sasl_client_out ();
                    ;
                   return 0;
                  }], HAS_SASL=1, HAS_SASL= )
	  if test "${HAS_SASL}x" = "x" ; then
		AC_MSG_RESULT(no)
	  else
		AC_MSG_RESULT(yes)	
	  fi	
  fi
  AC_MSG_RESULT(yes)

LIBS=$my_LIBS

dnl we need to track down the correct sasl.h
AC_MSG_CHECKING([if <sasl.h> works])
AC_COMPILE_IFELSE([[#include <sasl.h>
int main(int argc,char **argv){ int v = SASL_LOG_PASS; }]],HAS_INCLUDE_SASL_H=1)
if test "${HAS_INCLUDE_SASL_H}x" != "x" ; then
  AC_MSG_RESULT(yes)
  SASL_INCLUDED=1
else
  AC_MSG_RESULT(no)
  AC_MSG_CHECKING([if <sasl/sasl.h> works])
  AC_COMPILE_IFELSE([[#include <sasl/sasl.h>
  int main(int argc,char **argv){ int v = SASL_LOG_PASS; }]],P_HAS_SASL_SASL_H=1)
  if test "${P_HAS_SASL_SASL_H}x" != "x" ; then
    AC_MSG_RESULT(yes)
    SASL_INCLUDED=1
    AC_DEFINE(P_HAS_SASL_SASL_H)
  else
    AC_MSG_RESULT(no)
    HAS_SASL2=
  fi
fi

if test "${HAS_SASL2}x" != "x" ; then
  HAS_SASL2=1
  ENDLDLIBS="$ENDLDLIBS -lsasl2"
  AC_DEFINE(P_SASL2)
elif test "${HAS_SASL}x" != "x" ; then
  HAS_SASL=1
  ENDLDLIBS="$ENDLDLIBS -lsasl"
  AC_DEFINE(P_SASL)
else
  HAS_SASL=0
fi
	
fi dnl disable_Sasl

AC_SUBST(HAS_SASL)
AC_SUBST(HAS_SASL2)

dnl ########################################################################
dnl check for mediaLib library

HAS_MEDIALIB=no
AC_CHECK_LIB(mlib, mlib_VideoColorJFIFYCC2RGB420_Nearest, HAS_MEDIALIB=yes, HAS_MEDIALIB=no)
if test $HAS_MEDIALIB = yes; then
    AC_DEFINE(P_MEDIALIB)
    ENDLDLIBS="-lmlib $ENDLDLIBS"
fi

dnl ########################################################################
dnl look for OpenLDAP (requires SASL)

dnl MSWIN_DISPLAY    openldap,Open LDAP
dnl MSWIN_IF_FEATURE openldap,sasl
dnl MSWIN_CHECK_FILE openldap,include\ldap.h|OpenLDAP,P_LDAP=1
dnl MSWIN_DIR_SYMBOL openldap,LDAP_DIR
dnl MSWIN_CHECK_DIR  openldap,..\openldap\
dnl MSWIN_CHECK_DIR  openldap,..\external\openldap\
dnl MSWIN_CHECK_DIR  openldap,\openldap\
dnl MSWIN_CHECK_DIR  openldap,c:\openldap\

AC_ARG_ENABLE(openldap,
       [  --disable-openldap      disable OpenLDAP support])

if test "${enable_openldap}z" = "z" ; then
  enable_openldap=$default_openldap
fi

HAS_OPENLDAP=
if test "$enable_openldap" = "no" ; then
  AC_MSG_NOTICE(OpenLDAP disabled)
else 
  if test "${HAS_RESOLVER:-unset}" != "unset" -o "${OSTYPE}" = "mingw" ; then
    AC_CHECK_HEADERS(ldap.h, HAS_OPENLDAP=1)
  
    dnl posix4 is required by libldap_r on Solaris
    if test "${OSTYPE:-unknown}" = "solaris"; then
      OPENLDAPSYSLIBS=-lposix4
    fi

    dnl check also for lldap_r presence (required for thread safe OpenLDAP?)
    if test "${HAS_OPENLDAP:-unset}" != "unset" ; then
      AC_CHECK_LIB(ldap, ldap_open, HAS_OPENLDAP=1, HAS_OPENLDAP=,$LDAP_LIBRARIES  $RESOLVER_LIBS $ENDLDLIBS $OPENLDAPSYSLIBS)
    else
      AC_CHECK_FILE(/usr/local/include/ldap.h, HAS_OPENLDAP=1)
      if test "${HAS_OPENLDAP:-unset}" != "unset" ; then
        OPENLDAPINCLUDEDIR=/usr/local/include
        OPENLDAPLIBDIR=/usr/local/lib
        AC_CHECK_LIB(ldap, ldap_open, HAS_OPENLDAP=1, HAS_OPENLDAP=,-L/usr/local/lib $LDAP_LIBRARIES $RESOLVER_LIBS $ENDLDLIBS $OPENLDAPSYSLIBS)
      fi
    fi
  fi

  if test "${HAS_OPENLDAP:-unset}" != "unset" ; then
    HAS_OPENLDAP=1
    AC_DEFINE(P_LDAP, 1, "OpenLDAP found")
    ENDLDLIBS="-lldap $LDAP_LIBRARIES $ENDLDLIBS"
    if test "${OPENLDAPINCLUDEDIR}unset" != "unset" ; then
      STDCCFLAGS="$STDCCFLAGS -I${OPENLDAPINCLUDEDIR}"
    fi
    if test "${OPENLDAPLIBDIR}unset" != "unset" ; then
      LDFLAGS="$LDFLAGS -L${OPENLDAPLIBDIR}"
    fi
  fi
fi

AC_SUBST(HAS_OPENLDAP)


dnl ########################################################################
dnl look for OpenSSL
dnl check for environment variable, then assume standard system install
dnl this process is made incredibly more complex by the fact that RedHad 9
dnl is completely fscked. The default OpenSSL installation requires Kerberos,
dnl even when Kerberos is not installed, requiring an explicit #define to
dnl make it compile
dnl As if that is not enough, autoconf gets very confused if configure.in
dnl contains a AC_CHECK_HEADER directive that never gets used. So we have 
dnl to ensure that it is always called

dnl MSWIN_DISPLAY    openssl,Open SSL
dnl MSWIN_CHECK_FILE openssl,inc32\openssl\ssl.h,P_SSL=1
dnl MSWIN_DIR_SYMBOL openssl,SSL_DIR
dnl MSWIN_CHECK_DIR  openssl,..\openssl\
dnl MSWIN_CHECK_DIR  openssl,..\external\openssl\
dnl MSWIN_CHECK_DIR  openssl,\openssl\
dnl MSWIN_CHECK_DIR  openssl,c:\openssl\
dnl MSWIN_CHECK_DIR  openssl,\libs\openssl\

dnl MSWIN_DISPLAY    osslAES,Open SSL AES
dnl MSWIN_IF_FEATURE osslAES,openssl
dnl MSWIN_CHECK_FILE osslAES,inc32\openssl\aes.h,P_SSL_AES=1
dnl MSWIN_CHECK_DIR  osslAES,..\openssl\
dnl MSWIN_CHECK_DIR  osslAES,..\external\openssl\
dnl MSWIN_CHECK_DIR  osslAES,\openssl\
dnl MSWIN_CHECK_DIR  osslAES,c:\openssl\
dnl MSWIN_CHECK_DIR  osslAES,\libs\openssl\

dnl MSWIN_DISPLAY    winssl,Windows Open SSL
dnl MSWIN_IF_FEATURE winssl,!openssl
dnl MSWIN_CHECK_FILE winssl,include\openssl\ssl.h,P_WINSSL=1
dnl MSWIN_DIR_SYMBOL winssl,WINSSL_DIR
dnl MSWIN_CHECK_DIR  winssl,.\lib\VC\
dnl MSWIN_CHECK_DIR  winssl,\libs\openssl\

dnl MSWIN_DISPLAY    wsslAES,Windows Open SSL AES
dnl MSWIN_IF_FEATURE wsslAES,winssl
dnl MSWIN_CHECK_FILE wsslAES,include\openssl\aes.h,P_WINSSL_AES=1
dnl MSWIN_CHECK_DIR  wsslAES,.\lib\VC\
dnl MSWIN_CHECK_DIR  wsslAES,\libs\openssl\

AC_ARG_ENABLE(openssl,
       [  --disable-openssl       disable OpenSSL support])

if test "${enable_openssl}z" = "z" ; then
  enable_openssl=$default_openssl
fi

HAS_OPENSSL=
if test "$enable_openssl" = "no" ; then
  AC_MSG_NOTICE(OpenSSL disabled)
else

  dnl check to see OPENSSLDIR is set
  dnl if it is, we assume it correct - no further checking required
  if test ${OPENSSLDIR:-unset} != "unset" ; then
    AC_MSG_CHECKING([for ${OPENSSLDIR}/include/openssl/ssl.h])
    if test -e ${OPENSSLDIR}/include/openssl/ssl.h ; then
      AC_MSG_RESULT(yes)
      SSLHDR=1
      SSLINCLUDEDIR="${OPENSSLDIR}/include"
      SSLLIBDIR="${OPENSSLDIR}/lib"
    else
      AC_MSG_RESULT(no)
    fi
  fi


  dnl if OPENSSLDIR is not set, then check to see if OpenSSL
  dnl is installed in the standard system dirs. And also see if
  dnl if it works using crazy RedHat checks
  if test "${SSLHDR:-unset}" = "unset" ; then
    AC_CHECK_HEADERS(openssl/ssl.h,SSLHDR=1)
    if test "${SSLHDR:-unset}" = "unset" ; then
      AC_MSG_CHECKING([if openssl/ssl.h works with OPENSSL_NO_KRB5])
      AC_COMPILE_IFELSE([[#define OPENSSL_NO_KRB5
#include <openssl/ssl.h>
int main(int argc,char **argv){}]],SSLHDR=1)
      if test "${SSLHDR:-unset}" != "unset" ; then
        AC_MSG_RESULT(yes)
        SSLCCFLAGS=" -DOPENSSL_NO_KRB5"
        P_STATIC_LDFLAGS="$P_STATIC_LDFLAGS -L/usr/kerberos/lib"
        if test ${OSTYPE} = linux ; then
          dnl Linux static libs
          P_STATIC_ENDLDLIBS="-lkrb5 -lk5crypto -lcom_err"
        fi
      else
        AC_MSG_RESULT(no)
      fi
    fi
  fi

  dnl if we have not found OpenSSL so far, then look for it in 
  dnl several standard places, and check for crazy RedHat install too
  if test "${SSLHDR:-unset}" = "unset" ; then

    for openssl_dir in /usr/local/openssl /usr/local/ssl ; do
      AC_MSG_CHECKING([for OpenSSL in $openssl_dir])
      if test ! -f ${openssl_dir}/include/openssl/ssl.h ; then
        AC_MSG_RESULT(no)
      else
        AC_LANG_CONFTEST([[#include "${openssl_dir}/include/openssl/ssl.h"
int main(int argc,char **argv){ return 0; }]])
        $CXX $CXXFLAGS -I${openssl_dir}/include -c conftest.cc > /dev/null 2>&1
        if test $? = 0 ; then
          AC_MSG_RESULT(yes)
          SSLHDR=1
          SSLCCFLAGS=""
          SSLINCLUDEDIR="${openssl_dir}/include"
          SSLLIBDIR="${openssl_dir}/lib"
          break
        else
          AC_MSG_RESULT(no)
          AC_MSG_CHECKING([if ${openssl_dir}/include/openssl/ssl.h works with OPENSSL_NO_KRB5...])
          AC_LANG_CONFTEST([[#define OPENSSL_NO_KRB5
#include "${openssl_dir}/include/openssl/ssl.h"
int main(int argc,char **argv){}]])
          $CXX $CXXFLAGS -I${openssl_dir}/openssl/include -c conftest.cc > /dev/null 2>&1
          if test $? = 0 ; then
            AC_MSG_RESULT(yes)
            SSLHDR=1
            SSLCCFLAGS=" -DOPENSSL_NO_KRB5"
            SSLINCLUDEDIR="${openssl_dir}/include"
            SSLLIBDIR="${openssl_dir}/lib"
            P_STATIC_LDFLAGS="$P_STATIC_LDFLAGS -L/usr/kerberos/lib"
            if test ${OSTYPE} = linux ; then
              dnl Linux static libs
              P_STATIC_ENDLDLIBS="-lkrb5 -lk5crypto -lcom_err"
            fi
            break
          else
            AC_MSG_RESULT(no)
          fi
        fi
      fi
    done
  fi

  if test ${SSLHDR:-unset} != "unset" ; then
    AC_DEFINE(P_SSL, 1, "OpenSSL found")
    HAS_OPENSSL=1
    if test ${SSLINCLUDEDIR:-unset} != "unset" ; then
      STDCCFLAGS="$STDCCFLAGS -I${SSLINCLUDEDIR}"
    fi
    if test ${SSLLIBDIR:-unset} != "unset" ; then
      LDFLAGS="$LDFLAGS -L${SSLLIBDIR}"
    fi
    STDCCFLAGS="${STDCCFLAGS}${SSLCCFLAGS}"
    ENDLDLIBS="$ENDLDLIBS -lssl -lcrypto"

    AC_CHECK_HEADERS(openssl/aes.h, HAS_SSL_AES=yes, HAS_SSL_AES=no)
    if test "$HAS_SSL_AES" = yes ; then
      AC_DEFINE(P_SSL_AES, 1, "openssl with AES yes")
    fi

    AC_MSG_CHECKING(for const arg to d2i_AutoPrivateKey)
    AC_TRY_COMPILE([#include <openssl/evp.h>],
                    [ EVP_PKEY **a; const unsigned char **p; long l;
                      d2i_AutoPrivateKey(a, p, l);
                    ], P_SSL_USE_CONST=yes, )
    if test "${P_SSL_USE_CONST}x" = "x" ; then
      AC_MSG_RESULT(no)
    else
      AC_MSG_RESULT(yes)
      AC_DEFINE(P_SSL_USE_CONST, 1, "SSL with const")
    fi
  fi
fi

AC_SUBST(HAS_OPENSSL)
AC_SUBST(P_STATIC_ENDLDLIBS)
AC_SUBST(P_STATIC_LDFLAGS)

dnl ########################################################################
dnl look for expat XML parser

dnl MSWIN_DISPLAY    expat,Expat XML
dnl MSWIN_CHECK_FILE expat,Release\expat.lib,P_EXPAT=1
dnl MSWIN_DIR_SYMBOL expat,EXPAT_DIR
dnl MSWIN_CHECK_DIR  expat,..\expat\
dnl MSWIN_CHECK_DIR  expat,..\external\expat\
dnl MSWIN_CHECK_DIR  expat,\expat
dnl MSWIN_CHECK_DIR  expat,c:\expat\
dnl MSWIN_CHECK_DIR  expat,\libs\expat\

dnl MSWIN_DISPLAY    winexpat,Windows Expat XML
dnl MSWIN_IF_FEATURE winexpat,!expat
dnl MSWIN_CHECK_FILE winexpat,libs\libexpat.lib,P_WINEXPAT=1
dnl MSWIN_DIR_SYMBOL winexpat,WINEXPAT_DIR
dnl MSWIN_CHECK_DIR  winexpat,source\lib
dnl MSWIN_CHECK_DIR  winexpat,\libs\expat\

AC_ARG_ENABLE(expat,
       [  --disable-expat         disable XML support])

if test "${enable_expat}z" = "z" ; then
  enable_expat=$default_expat
fi

HAS_EXPAT=
if test "$enable_expat" = "no" ; then
  AC_MSG_NOTICE(Expat disabled)
else
  if test "${EXPATDIR}z" != "z" ; then
    AC_CHECK_FILE(${EXPATDIR}/include/expat.h, HAS_EXPAT=1)
    if test "${HAS_EXPAT:-unset}" != "unset" ; then
      EXPATINCLUDEDIR=${EXPATDIR}/include
      EXPATLIBDIR=${EXPATDIR}/lib
    fi
  else
    AC_CHECK_HEADERS(expat.h, HAS_EXPAT=1)
    if test "${HAS_EXPAT:-unset}" != "unset" ; then
      AC_CHECK_LIB(expat, XML_ParserCreate, HAS_EXPAT=1, HAS_EXPAT=)
    else
      AC_CHECK_FILE(/usr/local/include/expat.h, HAS_EXPAT=1)
      if test "${HAS_EXPAT:-unset}" != "unset" ; then
        EXPATINCLUDEDIR=/usr/local/include
        EXPATLIBDIR=/usr/local/lib
      fi
    fi
  fi

  if test "${HAS_EXPAT:-unset}" != "unset" ; then
    HAS_EXPAT=1
    AC_DEFINE(P_EXPAT, 1, "expat found")
    if test ${EXPATINCLUDEDIR:-unset} != "unset" ; then
      STDCCFLAGS="$STDCCFLAGS -I${EXPATINCLUDEDIR}"
    fi
    if test ${EXPATLIBDIR:-unset} != "unset" ; then
      LDFLAGS="$LDFLAGS -L${EXPATLIBDIR}"
    fi
    ENDLDLIBS="$ENDLDLIBS -lexpat"
  fi
fi
AC_SUBST(HAS_EXPAT)

dnl ########################################################################
dnl handle modules dependent on Expat

dnl MSWIN_DISPLAY    vxml,VXML
dnl MSWIN_DEFINE     vxml,P_VXML
dnl MSWIN_IF_FEATURE vxml,expat|winexpat

dnl MSWIN_DISPLAY    jabber,Jabber
dnl MSWIN_DEFINE     jabber,P_JABBER
dnl MSWIN_IF_FEATURE jabber,expat|winexpat

dnl MSWIN_DISPLAY    xmlrpc,XMLRPC
dnl MSWIN_DEFINE     xmlrpc,P_XMLRPC
dnl MSWIN_IF_FEATURE xmlrpc,expat|winexpat

dnl MSWIN_DISPLAY    soap,SOAP
dnl MSWIN_DEFINE     soap,P_SOAP
dnl MSWIN_IF_FEATURE soap,expat|winexpat

HAS_VXML=
HAS_JABBER=
HAS_XMLRPC=
HAS_SOAP=
if test "$enable_expat" = "no" ; then
  AC_MSG_NOTICE(VXML disabled as Expat is disabled)
  AC_MSG_NOTICE(Jabber disabled as Expat is disabled)
  AC_MSG_NOTICE(XMLRPC disabled as Expat is disabled)
else

  dnl ########################################################################
  dnl VXML 

  AC_ARG_ENABLE(vxml,
         [  --disable-vxml          disable VXML support])

  if test "${enable_vxml}z" = "z" ; then
    enable_vxml=$default_vxml
  fi
  if test "$enable_vxml" = "no" ; then
    AC_MSG_NOTICE(VXML disabled)
  else
    AC_DEFINE(P_VXML, 1)
    HAS_VXML=1
  fi

  dnl ########################################################################
  dnl Jabber

  AC_ARG_ENABLE(jabber,
         [  --disable-jabber        disable Jabber support])

  if test "${enable_jabber}z" = "z" ; then
    enable_jabber=$default_jabber
  fi
  if test "$enable_jabber" = "no" ; then
    AC_MSG_NOTICE(Jabber disabled)
  else
    AC_DEFINE(P_JABBER, 1)
    HAS_JABBER=1
  fi

  dnl ########################################################################
  dnl XMLRPC

  AC_ARG_ENABLE(xmlrpc,
         [  --disable-xmlrpc        disable XMLRPC support])

  if test "${enable_xmlrpc}z" = "z" ; then
    enable_xmlrpc=$default_xmlrpc
  fi
  if test "$enable_xmlrpc" = "no" ; then
    AC_MSG_NOTICE(XMLRPC disabled)
  else
    AC_DEFINE(P_XMLRPC, 1)
     HAS_XMLRPC=1
  fi
  
  dnl ########################################################################
  dnl SOAP

  AC_ARG_ENABLE(soap,
         [  --disable-soap          disable SOAP support])

  if test "${enable_soap}z" = "z" ; then
    enable_soap=$default_soap
  fi
  if test "$enable_soap" = "no" ; then
    AC_MSG_NOTICE(SOAP disabled)
  else
    AC_DEFINE(P_SOAP, 1)
    HAS_SOAP=1
  fi
fi

AC_SUBST(HAS_VXML)
AC_SUBST(HAS_JABBER)
AC_SUBST(HAS_XMLRPC)
AC_SUBST(HAS_SOAP)

dnl ########################################################################
dnl Speech

dnl MSWIN_DISPLAY       sapi,Speech API
dnl MSWIN_CHECK_FILE    sapi,include\sphelper.h,P_SAPI=1
dnl MSWIN_DEFINE        sapi,P_TTS
dnl MSWIN_DIR_SYMBOL    sapi,SAPI_DIR
dnl MSWIN_CHECK_DIR     sapi,\Program Files\Microsoft Speech SDK 5.1\

AC_ARG_ENABLE(expat,
       [  --disable-tts           disable Text-ToSpeech support (default enabled)])

if test "${enable_tts}z" = "z" ; then
  enable_tts=$default_tts
fi

HAS_TTS=
if test "$enable_tts" = "no" ; then
  AC_MSG_NOTICE(Text-to-speech disabled)
else
  AC_DEFINE(P_TTS, 1)
  HAS_TTS=1
fi
AC_SUBST(HAS_TTS)

dnl ########################################################################
dnl look for IPV6 functions

dnl MSWIN_DISPLAY    ipv6,IPv6 Support
dnl MSWIN_CHECK_FILE ipv6,ws2tcpip.h|sin6_scope_id,P_HAS_IPV6=1
dnl MSWIN_DIR_SYMBOL ipv6,IPV6_DIR
dnl MSWIN_CHECK_DIR  ipv6,\Program Files\Microsoft Visual Studio 8\VC\PlatformSDK\Include\
dnl MSWIN_CHECK_DIR  ipv6,\Program Files\Microsoft Visual Studio .NET 2003\VC7\PlatformSDK\Include\
dnl MSWIN_CHECK_DIR  ipv6,\Program Files\Microsoft Visual Studio\VC98\Include\
dnl MSWIN_CHECK_DIR  ipv6,\Program Files\Microsoft SDK\Include\
dnl MSWIN_CHECK_DIR  ipv6,\Program Files\Microsoft Platform Sdk for Windows Server 2003 R2\Include

AC_ARG_ENABLE(ipv6,
       [  --disable-ipv6          disable IPV6 support])

if test "${enable_ipv6}z" = "z" ; then
  enable_ipv6=$default_ipv6
fi

HAS_IPV6=

if test "$enable_ipv6" = "no" ; then
  AC_MSG_NOTICE(IPV6 disabled)
else
  AC_MSG_CHECKING(for IPv6 support)
  AC_TRY_COMPILE([#include <sys/types.h>
                  #include <netinet/in.h>],
                  [struct sockaddr_in6 s; s.sin6_scope_id = 0;], HAS_IPV6=yes, HAS_IPV6=no)
  if test $HAS_IPV6 = no ; then
    AC_MSG_RESULT(no)
  else
    AC_TRY_COMPILE([#include <sys/socket.h>
                     #include <netdb.h>],
                     [getnameinfo(NULL, 0, NULL, 0, NULL, 0, 0);], HAS_IPV6=yes, HAS_IPV6=no)
    if test $HAS_IPV6 = no ; then
      AC_MSG_RESULT(no)
    else
      AC_MSG_RESULT(yes)
      AC_DEFINE(P_HAS_IPV6, 1, "IPV6 enabled")
      HAS_IPV6=1
    fi
  fi
fi
AC_SUBST(HAS_IPV6)

dnl ########################################################################
dnl look for QoS functions

dnl MSWIN_DISPLAY    qos,QoS Support
dnl MSWIN_CHECK_FILE qos,qossp.h|QOS_DESTADDR,P_HAS_QOS=1
dnl MSWIN_DIR_SYMBOL qos,QOS_DIR
dnl MSWIN_CHECK_DIR  qos,\Program Files\Microsoft Visual Studio 8\VC\PlatformSDK\Include\
dnl MSWIN_CHECK_DIR  qos,\Program Files\Microsoft Visual Studio .NET 2003\VC7\PlatformSDK\Include\
dnl MSWIN_CHECK_DIR  qos,\Program Files\Microsoft Visual Studio\VC98\Include\
dnl MSWIN_CHECK_DIR  qos,\Program Files\Microsoft SDK\include\
dnl MSWIN_CHECK_DIR  qos,\Program Files\Microsoft Platform SDK for Windows Server 2003 R2\Include\

AC_ARG_ENABLE(qos,
       [  --disable-qos           disable QOS support])

if test "${enable_qos}z" = "z" ; then
  enable_qos=$default_qos
fi

if test "$enable_qos" != "no" ; then
  AC_DEFINE([P_HAS_QOS], [1], [Have QOS])
fi


dnl ########################################################################
dnl look for SDL, but only if we have pthreads

dnl MSWIN_DISPLAY    libsdl,Simple DirectMedia Layer
dnl MSWIN_CHECK_FILE libsdl,include\SDL_video.h,P_SDL=1
dnl MSWIN_CHECK_FILE libsdl,lib\SDL.lib,P_SDL_LIBDIR="lib"
dnl MSWIN_DIR_SYMBOL libsdl,SDL_DIR
dnl MSWIN_CHECK_DIR  libsdl,..\sdl\
dnl MSWIN_CHECK_DIR  libsdl,..\external\sdl\
dnl MSWIN_CHECK_DIR  libsdl,\sdl\
dnl MSWIN_CHECK_DIR  libsdl,c:\sdl\

AC_ARG_ENABLE(sdl,
       [  --disable-sdl           disable SDL support])

if test "${enable_sdl}z" = "z" ; then
  enable_sdl=$default_sdl
fi

HAS_SDL=
if test "$enable_sdl" = "no" ; then
  AC_MSG_NOTICE(SDL disabled)
  AC_DEFINE(P_SDL, 0)
else
  dnl SDL checks
  if test "${SDLLIB:-unset}" = "unset" ; then
    SDLLIB=SDL
  fi

  dnl Linux check
  AC_CHECK_HEADERS(SDL/SDL.h, HAS_SDL=1)
  if test "${HAS_SDL:-unset}" != "unset" ; then
    if test "${X11LIBDIR:-unset}" == "unset" ; then
	X11LIBDIR=/usr/X11R6/lib
    fi
    AC_CHECK_LIB(SDL, SDL_CreateYUVOverlay, HAS_SDL=1, HAS_SDL=, -L${X11LIBDIR} -lpthread -lX11 -lXext)
  fi

  dnl FreeBSD SDL check
  if test "${HAS_SDL:-unset}" = "unset" ; then
    AC_CHECK_HEADER(SDL11/SDL.h, HAS_SDL=1)
    if test "${HAS_SDL:-unset}" != "unset" ; then
      SDLLIB=SDL-1.1
    fi
  fi

  if test "${HAS_SDL:-unset}" != "unset" ; then
    HAS_SDL=1
    AC_DEFINE(P_SDL, 1, "SDL found")
    if test ${SDLINCLUDEDIR:-unset} != "unset" ; then
      STDCCFLAGS="$STDCCFLAGS -I${SDLINCLUDEDIR}"
    fi
    if test ${SDLLIBDIR:-unset} != "unset" ; then
      LDFLAGS="$LDFLAGS -L${SDLLIBDIR}"
    fi
    ENDLDLIBS="$ENDLDLIBS -l${SDLLIB}"
  fi
fi
AC_SUBST(HAS_SDL)


dnl ########################################################################
dnl check for video capture support.
dnl videodev.h on linux
dnl machine/ioctl_meteor on FreeBSD
dnl i386/ioctl_meteor on OpenBSD
dnl dev/ic/bt8xx on NetBSD

dnl MSWIN_DISPLAY    video,Video Support
dnl MSWIN_DEFINE     video,P_VIDEO

AC_ARG_ENABLE(video,
       [  --disable-video         disable video device support])

if test "${enable_video}z" = "z" ; then
  enable_video=$default_video
fi

HAS_VIDEO=
HAS_VIDEO_CAPTURE=

if test "$enable_video" = "no" ; then
  AC_DEFINE([NO_VIDEO_CAPTURE], [], [No video capture])
else
  AC_DEFINE(P_VIDEO, 1)
  HAS_VIDEO=1
  if test "$enable_plugins" = "yes" ; then
    HAS_VIDEO_CAPTURE=1
  else
    AC_CHECK_HEADER(linux/videodev.h, HAS_VIDEO_CAPTURE=1)
    dnl AC_CHECK_HEADER(machine/ioctl_meteor.h, HAS_VIDEO_CAPTURE=1)
    dnl AC_CHECK_HEADER(i386/ioctl_meteor.h, HAS_VIDEO_CAPTURE=1)
    dnl AC_CHECK_HEADER(dev/ic/bt8xx.h, HAS_VIDEO_CAPTURE=1)
    if test "${HAS_VIDEO_CAPTURE}z" = "z" ; then
      AC_MSG_NOTICE(No Video Capture support)
      AC_DEFINE([NO_VIDEO_CAPTURE], [], [No video capture])
    fi
  fi
  dnl these are needed to pass through to plugin configure
  AC_ARG_ENABLE(v4l,      [  --enable-v4l            enable V4L video support])
  AC_ARG_ENABLE(v4l2,     [  --enable-v4l2           enable V4L2 video support])
  AC_ARG_ENABLE(bsdvideo, [  --enable-bsdvideo       enable BSD video support])
  AC_ARG_ENABLE(avc,      [  --enable-avc            enable IEEE1394/AVC video support])
  AC_ARG_ENABLE(dc,       [  --enable-dc             enable IEEE1394/DC video support])
fi

AC_SUBST(HAS_VIDEO)
AC_SUBST(HAS_VIDEO_CAPTURE)

dnl ########################################################################
dnl enable shared memory video devices (static video plugin)


AC_ARG_ENABLE(shm_video,
		[  --enable-shmvideo       enable shared memory video devices],
		shm_video=$enableval)

if test "${enable_shmvideo}z" = "z" ; then
  shm_video=$default_shmvideo
fi

USE_SHM_VIDEO_DEVICES=
if test "$shm_video" = "yes" ; then
  USE_SHM_VIDEO_DEVICES=1
  AC_MSG_NOTICE(Shared memory video devices enabled)
else
  AC_MSG_NOTICE(Shared memory video devices disabled)
fi
AC_SUBST(USE_SHM_VIDEO_DEVICES)

dnl ########################################################################
dnl check for enabling ASN support

dnl MSWIN_DISPLAY    asn,ASN Support
dnl MSWIN_DEFINE     asn,P_ASN

AC_ARG_ENABLE(asn,
       [  --disable-asn           disable ASN decoding/encoding support])

if test "${enable_asn}z" = "z" ; then
  enable_asn=$default_asn
fi

HAS_ASN=
if test "$enable_asn" = "no" ; then
  AC_MSG_NOTICE(ASN decoding/encoding support disabled)
else
  HAS_ASN=1
  AC_DEFINE(P_ASN, 1)
fi
AC_SUBST(HAS_ASN)

dnl ########################################################################
dnl check for enabling STUN support

dnl MSWIN_DISPLAY    stun,STUN Support
dnl MSWIN_DEFINE     stun,P_STUN

AC_ARG_ENABLE(stun,
       [  --disable-stun          disable STUN protocol support])

if test "${enable_stun}z" = "z" ; then
  enable_stun=$default_stun
fi

HAS_STUN=
if test "$enable_stun" = "no" ; then
  AC_MSG_NOTICE(STUN decoding/encoding support disabled)
else
  HAS_STUN=1
  AC_DEFINE(P_STUN, 1)
fi
AC_SUBST(HAS_STUN)

dnl ########################################################################
dnl check for enabling PIPECHAN support

dnl MSWIN_DISPLAY    pipechan,pipe channel Support
dnl MSWIN_DEFINE     pipechan,P_PIPECHAN

AC_ARG_ENABLE(pipechan,
       [  --disable-pipechan      disable pipechannel support])

if test "${enable_pipechan}z" = "z" ; then
  enable_pipechan=$default_pipechan
fi

HAS_PIPECHAN=
if test "$enable_pipechan" = "no" ; then
  AC_MSG_NOTICE(pipechannel support disabled)
else
  HAS_PIPECHAN=1
  AC_DEFINE(P_PIPECHAN, 1)
fi
AC_SUBST(HAS_PIPECHAN)

dnl ########################################################################
dnl check for enabling DTMF support

dnl MSWIN_DISPLAY  dtmf,DTMF encoding/decoding support
dnl MSWIN_DEFINE   dtmf,P_DTMF

AC_ARG_ENABLE(dtmf,
       [  --disable-dtmf          disable DTMF protocol support])

if test "${enable_dtmf}z" = "z" ; then
  enable_dtmf=$default_dtmf
fi

HAS_DTMF=
if test "$enable_dtmf" = "no" ; then
  AC_MSG_NOTICE(DTMF decoding/encoding support disabled)
else
  HAS_DTMF=1
  AC_DEFINE(P_DTMF, 1)
fi
AC_SUBST(HAS_DTMF)

dnl ########################################################################
dnl check for enabling WAVFILE support

dnl MSWIN_DISPLAY    wavfile,WAV file support
dnl MSWIN_DEFINE     wavfile,P_WAVFILE

AC_ARG_ENABLE(wavfile,
       [  --disable-wavfile       disable WAVFILE support])

if test "${enable_wavfile}z" = "z" ; then
  enable_wavfile=$default_wavfile
fi

HAS_WAVFILE=
if test "$enable_wavfile" = "no" ; then
  AC_MSG_NOTICE(WAV file support disabled)
else
  HAS_WAVFILE=1
  AC_DEFINE(P_WAVFILE, 1)
fi
AC_SUBST(HAS_WAVFILE)

dnl ########################################################################
dnl check for enabling SOCKS support

dnl MSWIN_DISPLAY    socks,SOCKS protocol support
dnl MSWIN_DEFINE     socks,P_SOCKS

AC_ARG_ENABLE(socks,
       [  --disable-socks         disable SOCKS protocol support])

if test "${enable_socks}z" = "z" ; then
  enable_socks=$default_socks
fi

HAS_SOCKS=
if test "$enable_socks" = "no" ; then
  AC_MSG_NOTICE(SOCKS decoding/encoding support disabled)
else
  HAS_SOCKS=1
  AC_DEFINE(P_SOCKS, 1)
fi
AC_SUBST(HAS_SOCKS)

dnl ########################################################################
dnl check for enabling FTP support

dnl MSWIN_DISPLAY    ftp,FTP protocol support
dnl MSWIN_DEFINE     ftp,P_FTP

AC_ARG_ENABLE(rtp,
       [  --disable-ftp           disable FTP support])

if test "${enable_ftp}z" = "z" ; then
  enable_ftp=$default_ftp
fi

HAS_FTP=
if test "$enable_ftp" = "no" ; then
  AC_MSG_NOTICE(FTP protocol support disabled)
else
  HAS_FTP=1
  AC_DEFINE(P_FTP, 1)
fi
AC_SUBST(HAS_FTP)

dnl ########################################################################
dnl check for enabling SNMP support

dnl MSWIN_DISPLAY    snmp,SNMP protocol support
dnl MSWIN_DEFINE     snmp,P_SNMP

AC_ARG_ENABLE(snmp,
       [  --disable-snmp          disable SNMP protocol support])

if test "${enable_snmp}z" = "z" ; then
  enable_snmp=$default_snmp
fi

HAS_SNMP=
if test "$enable_snmp" = "no" ; then
  AC_MSG_NOTICE(SNMP protocol support disabled)
else
  HAS_SNMP=1
  AC_DEFINE(P_SNMP, 1)
fi
AC_SUBST(HAS_SNMP)

dnl ########################################################################
dnl check for enabling TELNET support

dnl MSWIN_DISPLAY    telnet,Telnet protocol support
dnl MSWIN_DEFINE     telnet,P_TELNET

AC_ARG_ENABLE(telnet,
       [  --disable-telnet        disable Telnet protocol support])

if test "${enable_telnet}z" = "z" ; then
  enable_telnet=$default_telnet
fi

HAS_TELNET=
if test "$enable_telnet" = "no" ; then
  AC_MSG_NOTICE(Telnet protocol support disabled)
else
  HAS_TELNET=1
  AC_DEFINE(P_TELNET, 1)
fi
AC_SUBST(HAS_TELNET)

dnl ########################################################################
dnl check for enabling REMCONN support

dnl MSWIN_DISPLAY    remconn,remote connection support
dnl MSWIN_DEFINE     remconn,P_REMCONN

AC_ARG_ENABLE(remconn,
       [  --disable-remconn       disable remote connection support])

if test "${enable_remconn}z" = "z" ; then
  enable_remconn=$default_remconn
fi

HAS_REMCONN=
if test "$enable_remconn" = "no" ; then
  AC_MSG_NOTICE(Remote connection support disabled)
else
  HAS_REMCONN=1
  AC_DEFINE(P_REMCONN, 1)
fi
AC_SUBST(HAS_REMCONN)

dnl ########################################################################
dnl check for enabling serial port handling

dnl MSWIN_DISPLAY    serial,Serial Support
dnl MSWIN_DEFINE     serial,P_SERIAL

AC_ARG_ENABLE(serial,
       [  --disable-serial        disable serial port support])

if test "${enable_serial}z" = "z" ; then
  enable_serial=$default_serial
fi

HAS_SERIAL=
if test "$enable_serial" = "no" ; then
  AC_MSG_NOTICE(Serial port support disabled)
else
  HAS_SERIAL=1
  AC_DEFINE(P_SERIAL, 1)
fi
AC_SUBST(HAS_SERIAL)

dnl ########################################################################
dnl check for POP3/SMTP support

dnl MSWIN_DISPLAY    pop3smtp,POP3/SMTP Support
dnl MSWIN_DEFINE     pop3smtp,P_POP3SMTP

AC_ARG_ENABLE(pop3smtp,
       [  --disable-pop3smtp      disable POP3/SMTP support])

if test "${enable_pop3smtp}z" = "z" ; then
  enable_pop3smtp=$default_pop3smtp
fi

HAS_POP3SMTP=
if test "$enable_pop3smtp" = "no" ; then
  AC_MSG_NOTICE(POP3/SMTP protocol support disabled)
else
  HAS_POP3SMTP=1
  AC_DEFINE(P_POP3SMTP, 1)
fi
AC_SUBST(HAS_POP3SMTP)

dnl ########################################################################
dnl check for HTTP support

dnl MSWIN_DISPLAY    http,HTTP Support
dnl MSWIN_DEFINE     http,P_HTTP

AC_ARG_ENABLE(http,
       [  --disable-http          disable HTTP support])

if test "${enable_http}z" = "z" ; then
  enable_http=$default_http
fi

HAS_HTTP=
if test "$enable_http" = "no" ; then
  AC_MSG_NOTICE(HTTP protocol support disabled)
else
  HAS_HTTP=1
  AC_DEFINE(P_HTTP, 1)
fi
AC_SUBST(HAS_HTTP)

dnl ########################################################################
dnl check for HTTPSVC support

dnl MSWIN_DISPLAY    httpsvc,HTTP service Support
dnl MSWIN_DEFINE     httpsvc,P_HTTPSVC

AC_ARG_ENABLE(httpsvc,
       [  --disable-httpsvc       disable HTTP service support])

if test "${enable_httpsvc}z" = "z" ; then
  enable_httpsvc=$default_httpsvc
fi

HAS_HTTPSVC=
if test "$enable_httpsvc" = "no" ; then
  AC_MSG_NOTICE(HTTP service support disabled)
else
  HAS_HTTPSVC=1
  AC_DEFINE(P_HTTPSVC, 1)
fi
AC_SUBST(HAS_HTTPSVC)

dnl ########################################################################
dnl check for enabling config file support

dnl MSWIN_DISPLAY    config_file,Config file protocol support
dnl MSWIN_DEFINE     config_file,P_CONFIG_FILE

AC_ARG_ENABLE(configfile,
       [  --disable-configfile    disable config file support])

if test "${enable_configfile}z" = "z" ; then
  enable_configfile=$default_configfile
fi

HAS_CONFIG_FILE=
if test "$enable_configfile" = "no" ; then
  AC_MSG_NOTICE(Config file support disabled)
else
  HAS_CONFIG_FILE=1
  AC_DEFINE(P_CONFIG_FILE, 1)
fi
AC_SUBST(HAS_CONFIG_FILE)

dnl ########################################################################
dnl check for enabling socket aggregation

dnl MSWIN_DISPLAY    sockagg,Socket aggregation support
dnl MSWIN_DEFINE     sockagg,P_SOCKAGG

AC_ARG_ENABLE(sockagg,
       [  --enable-sockagg        enable socket aggregation (experimental)])

if test "${enable_sockagg}z" = "z" ; then
  enable_sockagg=$default_sockagg
fi

HAS_SOCKAGG=
if test "$enable_sockagg" = "no" ; then
  AC_MSG_NOTICE(Socket aggregation support disabled)
else
  AC_MSG_NOTICE(Socket aggregation support enabled (experimental))
  HAS_SOCKAGG=1
  AC_DEFINE(P_SOCKAGG, 1)
fi
AC_SUBST(HAS_SOCKAGG)

dnl ########################################################################
dnl check for enabling video file support

dnl MSWIN_DISPLAY    vidfile,Video file support
dnl MSWIN_DEFINE     vidfile,P_VIDFILE

if test "$HAS_VIDEO" \!= "1" ; then
  default_vidfile=no
fi

AC_ARG_ENABLE(vidfile,
       [  --enable-vidfile        enable video file support])

if test "${enable_vidfile}z" = "z" ; then
  enable_vidfile=$default_vidfile
fi

HAS_VIDFILE=
if test "$enable_vidfile" = "no" ; then
  AC_MSG_NOTICE(Video file support disabled)
else
  AC_MSG_NOTICE(Video file support enabled)
  HAS_VIDFILE=1
  AC_DEFINE(P_VIDFILE, 1)
fi
AC_SUBST(HAS_VIDFILE)

dnl ########################################################################
dnl check for sound support

dnl MSWIN_DISPLAY audio,Sound support
dnl MSWIN_DEFINE  audio,P_AUDIO

AC_ARG_ENABLE(audio,
       [  --disable-audio         disable audio support])

if test "${enable_audio}z" = "z" ; then
  enable_audio=$default_audio
fi

HAS_AUDIO=
if test "$enable_audio" = "no" ; then
  AC_MSG_NOTICE(Audio support disabled)
else
  HAS_AUDIO=1
  AC_DEFINE(P_AUDIO)
fi
AC_SUBST(HAS_AUDIO)

dnl ########################################################################
dnl check for for audio drivers

HAS_ALSA=
HAS_OSS=
if test "$enable_audio" = "yes" ; then

  dnl these are needed to pass through to the plugin configure program
  AC_ARG_ENABLE(alsa, [  --enable-alsa           enable ALSA audio support],enable_alsa=$enableval)
  AC_ARG_ENABLE(oss,  [  --enable-oss            enable OSS audio support],enable_oss=$enableval)

  if test "$enable_plugins" = "no" ; then

    if test "${enable_alsa}z" = "z" ; then
      enable_alsa=$default_alsa
    fi

    if test "${enable_oss}z" = "z" ; then
      enable_oss=$default_oss
    fi

    if test "$enable_alsa" = "yes" ; then
      AC_CHECK_HEADER(alsa/asoundlib.h, HAS_ALSA=1)
      AC_MSG_CHECKING(for ALSA sound support)
      if test "${HAS_ALSA}z" != "z"; then
        AC_MSG_RESULT(yes)
        ENDLDLIBS="$ENDLDLIBS -lasound"
        HAS_ALSA=1
      else
        AC_MSG_RESULT(no)
      fi
    fi

    if test "${HAS_ALSA}z" = "z" ; then
      if test "$enable_oss" = "yes" ; then
        AC_CHECK_HEADER(sys/soundcard.h, SNDCARDHDR=1)
        AC_MSG_CHECKING(for OSS sound support)
        if test "${SNDCARDHDR}z" != "z"; then
          AC_SUBST(HAS_OSS, 1)
          AC_MSG_RESULT(yes)
        else
          AC_MSG_RESULT(no)
        fi
      fi
    fi
  fi
fi

AC_SUBST(HAS_ALSA)
AC_SUBST(HAS_OSS)


dnl ########################################################################
dnl look for Direct Sound

dnl MSWIN_DISPLAY    dsound,Direct Sound Support
dnl MSWIN_CHECK_FILE dsound,include\dsound.h,P_DIRECTSOUND=1
dnl MSWIN_DIR_SYMBOL dsound,DIRECTX_DIR
dnl MSWIN_FIND_FILE  dsound,P_DIRECTSOUND_LIB64,dsound.lib,lib/x64
dnl MSWIN_FIND_FILE  dsound,P_DIRECTSOUND_LIB32,dsound.lib,lib/x86
dnl MSWIN_FIND_FILE  dsound,P_DIRECTSOUND_LIB32,dsound.lib,lib
dnl MSWIN_CHECK_DIR  dsound,\Program Files\Microsoft DirectX SDK\
dnl MSWIN_CHECK_DIR  dsound,\Program Files\Microsoft Visual Studio 8\VC\PlatformSDK\
dnl MSWIN_CHECK_DIR  dsound,\Program Files\Microsoft Visual Studio .NET 2003\VC7\PlatformSDK\
dnl MSWIN_CHECK_DIR  dsound,\Program Files\Microsoft SDK\
dnl MSWIN_CHECK_DIR  dsound,\Program Files\Microsoft Platform SDK for Windows Server 2003 R2\


dnl ########################################################################
dnl check for various functions
dnl According to Andre Schulze <as8@rncmm2.urz.tu-dresden.de>
dnl Solaris swab should not be used
if test "$OSTYPE" = "solaris" ; then
  USE_SYSTEM_SWAB=
else
  AC_CHECK_FUNC(swab,USE_SYSTEM_SWAB=1)
fi
if test "x$ac_cv_func_swab" = "xyes" ; then
  AC_DEFINE([USE_SYSTEM_SWAB], [], [Use system swab()])
fi

dnl ########################################################################
dnl look for MEMORY_CHECK enabled.

AC_ARG_ENABLE(memcheck,
       [  --enable-memcheck       enable leak testing code (off by default)],
       memcheck=$enableval)

if test "$memcheck" = "yes" ; then
  AC_DEFINE(PMEMORY_CHECK, 1)
  AC_MSG_NOTICE(Memory checking enabled)
fi


dnl ########################################################################
dnl look for ODBC code

dnl MSWIN_DISPLAY    odbc,ODBC support
dnl MSWIN_CHECK_FILE odbc,odbcinst.h,P_ODBC=1
dnl MSWIN_CHECK_DIR  odbc,\Program Files\Microsoft Visual Studio 8\VC\PlatformSDK\Include\
dnl MSWIN_CHECK_DIR  odbc,\Program Files\Microsoft Visual Studio .NET 2003\VC7\PlatformSDK\Include\
dnl MSWIN_CHECK_DIR  odbc,\Program Files\Microsoft SDK\include\
dnl MSWIN_CHECK_DIR  odbc,\Program Files\Microsoft Platform SDK for Windows Server 2003 R2\Include\

AC_ARG_ENABLE(odbc,
        [  --disable-odbc          disable ODBC support])
 
 if test "${enable_odbc}z" = "z" ; then
   enable_odbc=$default_odbc
 fi
 
 HAS_ODBC=
 if test "$enable_odbc" = "no" ; then
   AC_MSG_NOTICE(ODBC support disabled)
 else
   AC_CHECK_HEADERS(sql.h, HAS_ODBC=1)
   if test "${HAS_ODBC:-unset}" != "unset" ; then
     AC_CHECK_LIB(odbc, SQLAllocStmt, HAS_ODBC=1, HAS_ODBC=)
   else
     AC_CHECK_FILE(/usr/local/include/sql.h, HAS_ODBC=1)
     if test "${HAS_ODBC:-unset}" != "unset" ; then
       ODBCINCLUDEDIR=/usr/local/include
       ODBCLIBDIR=/usr/local/lib
     fi
   fi
 
   if test "${HAS_ODBC:-unset}" != "unset" ; then
     HAS_ODBC=1
     AC_DEFINE(P_ODBC, 1, "ODBC found")
     if test ${ODBCINCLUDEDIR:-unset} != "unset" ; then
       STDCCFLAGS="$STDCCFLAGS -I${ODBCINCLUDEDIR}"
     fi
     if test ${ODBCLIBDIR:-unset} != "unset" ; then
       LDFLAGS="$LDFLAGS -L${ODBCLIBDIR}"
     fi
     ENDLDLIBS="$ENDLDLIBS -lodbc"
     AC_MSG_NOTICE(ODBC support enabled)
   fi
 fi
 AC_SUBST(HAS_ODBC)
 


dnl ########################################################################
dnl output make directives

dnl resolver needs to be at the end
if test "${HAS_RESOLVER:-unset}" != "unset" ; then
  if test "${RESOLVER_LIBS:-unset}" != "unset" ; then
    ENDLDLIBS="$ENDLDLIBS $RESOLVER_LIBS"
  fi
fi

dnl dl needs to be at the end
if test "${OSTYPE}" != "Darwin" ; then
  if test ${HAS_LIBDL} = yes ; then
    ENDLDLIBS="$ENDLDLIBS -ldl"
  fi
fi

dnl ########################################################################
dnl this reduces the size of all executables and libraries

AC_ARG_ENABLE(exceptions,
       [  --exceptions            enable C++ exceptions])

if test "${enable_exceptions}z" != "yesz" ; then
  AC_MSG_NOTICE(C++ exceptions disabled)
  STDCCFLAGS="$STDCCFLAGS -fno-exceptions"
else
  AC_DEFINE(P_EXCEPTIONS, 1)
  AC_MSG_NOTICE(C++ exceptions enabled)
  STDCCFLAGS="$STDCCFLAGS -fexceptions"
fi

dnl ########################################################################
dnl final declarations

AC_SUBST(STDCCFLAGS)
AC_SUBST(STDCXXFLAGS)
AC_SUBST(OPTSTDCCFLAGS)
AC_SUBST(LDFLAGS)
AC_SUBST(ENDLDLIBS)
AC_SUBST(DEBUG_FLAG)
AC_PROG_INSTALL

AC_CONFIG_FILES(make/ptbuildopts.mak)
AC_CONFIG_FILES([make/ptlib-config], [chmod a+x make/ptlib-config])
AC_CONFIG_FILES(Makefile)

dnl ########################################################################
dnl output header file
#AC_CONFIG_FILES(include/ptbuildopts.h.in)
AC_CONFIG_HEADERS(include/ptbuildopts.h)

AC_OUTPUT()
