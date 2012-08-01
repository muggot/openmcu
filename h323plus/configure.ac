AC_INIT(include/h323.h)

AC_PROG_CXX
AC_HEADER_STDC

dnl this must be updated for every minor revision, and for
dnl builds when and as necessary
REQUIRED_PWLIB_MAJOR=1
REQUIRED_PWLIB_MINOR=11
REQUIRED_PWLIB_BUILD=1

dnl ########################################################################
dnl set the OpenH323 directory to the current directory

OPENH323DIR=`pwd`
AC_SUBST(OPENH323DIR)

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
LIBDIR="${libdir}"

AC_SUBST(LIBDIR)


dnl ########################################################################
dnl extract the OpenH323 version

dnl MSWIN_DISPLAY version,Version
dnl MSWIN_VERSION version,OPENH323_MAJOR=MAJOR_VERSION
dnl MSWIN_VERSION version,OPENH323_MINOR=MINOR_VERSION
dnl MSWIN_VERSION version,OPENH323_BUILD=BUILD_NUMBER
dnl MSWIN_VERSION version,OPENH323_VERSION=VERSION

MAJOR_VERSION=`cat ${OPENH323DIR}/version.h | grep MAJOR_VERSION | cut -f3 -d' '`
MINOR_VERSION=`cat ${OPENH323DIR}/version.h | grep MINOR_VERSION | cut -f3 -d' '`
BUILD_NUMBER=`cat ${OPENH323DIR}/version.h | grep BUILD_NUMBER | cut -f3 -d' '`
OPENH323_VERSION="${MAJOR_VERSION}.${MINOR_VERSION}.${BUILD_NUMBER}"
AC_SUBST(MAJOR_VERSION)
AC_SUBST(MINOR_VERSION)
AC_SUBST(BUILD_NUMBER)
AC_SUBST(OPENH323_VERSION)
AC_SUBST(MAJOR_VERSION, $MAJOR_VERSION)
AC_SUBST(MINOR_VERSION, $MINOR_VERSION)
AC_SUBST(BUILD_NUMBER, $BUILD_NUMBER)

echo "OpenH323 version is ${OPENH323_VERSION}"

AC_DEFINE_UNQUOTED(OPENH323_MAJOR,   ${MAJOR_VERSION})
AC_DEFINE_UNQUOTED(OPENH323_MINOR,   ${MINOR_VERSION})
AC_DEFINE_UNQUOTED(OPENH323_BUILD,   ${BUILD_NUMBER})
AC_DEFINE_UNQUOTED(OPENH323_VERSION, "$OPENH323_VERSION")

dnl ########################################################################
dnl look for ptlib, use a preference order of explicit PWLIBDIR, directory
dnl at same level, home directory, /usr/local or /usr.

if test "${HAS_PTLIB:-unset}" = "unset" ; then
  if test "${HAS_PTLIB:-unset}" != "unset" ; then
    PWLIBDIR="${OPENH323DIR}/../pwlib"
  else
    if test "${HAS_PTLIB:-unset}" != "unset" ; then
      PWLIBDIR="${HOME}/pwlib"
    else
      if test "${HAS_PTLIB:-unset}" != "unset" ; then
        AC_PATH_PROG(PTLIB_CONFIG, ptlib-config, , /usr/local/bin)
      else
        if test "${HAS_PTLIB:-unset}" != "unset" ; then
          AC_PATH_PROG(PTLIB_CONFIG, ptlib-config, , /usr/share/ptlib/make/)
        else
           if test "${HAS_PTLIB:-unset}" != "unset" ; then
             AC_PATH_PROG(PTLIB_CONFIG, ptlib-config, , /usr/share/ptlib/make/)
           fi
        fi
      fi
    fi
  fi
fi

if test "${HAS_PTLIB:-unset}" = "unset" ; then
  echo "Cannot find pwlib - please install or set PWLIBDIR and try again"
  exit
fi

if test "${PWLIBDIR:-unset}" = "unset" ; then
  if test "${PTLIB_CONFIG:-unset}" = "unset" ; then
    echo "Cannot find ptlib-config - please install and try again"
    exit
  fi
  PWLIBDIR=`$PTLIB_CONFIG --prefix`
fi

if test "x$PWLIBDIR" = "x/usr" -o "x$PWLIBDIR" = "x/usr/"; then
  PWLIBDIR="/usr/share/ptlib/"
fi
if test "x$PWLIBDIR" = "x/usr/local" -o "x$PWLIBDIR" = "x/usr/"; then
  PWLIBDIR="/usr/local/share/ptlib/"
fi

echo "PWLib prefix set to.... $PWLIBDIR"

if test "${PWLIBDIR:-unset}" = "unset" ; then
  PWVERSION=`$PTLIB_CONFIG --version`
  PW_MAJOR_VERSION=`echo $PW_VERSION | cut -d'.' -f1`
  PW_MINOR_VERSION=`echo $PW_VERSION | cut -d'.' -f2`
  PW_BUILD_NUMBER=`echo $PW_VERSION | cut -d'.' -f3`
  
else
  PW_MAJOR_VERSION=`cat ${PWLIBDIR}/version.h | grep MAJOR_VERSION | cut -f3 -d' '`
  PW_MINOR_VERSION=`cat ${PWLIBDIR}/version.h | grep MINOR_VERSION | cut -f3 -d' '`
  PW_BUILD_NUMBER=`cat ${PWLIBDIR}/version.h | grep BUILD_NUMBER | cut -f3 -d' '`
  PWVERSION="${PW_MAJOR_VERSION}.${PW_MINOR_VERSION}.${PW_BUILD_NUMBER}"
fi

AC_ARG_ENABLE(versioncheck,
		[  --disable-versioncheck    disable PWLib version check],
		PWLIB_VERSION_CHECK=$enableval)

AC_MSG_CHECKING(Checking PWLib version ${PWVERSION})
if test "${PWLIB_VERSION_CHECK}x" = "nox" ; then
  AC_MSG_RESULT(check disabled)
else
  if test $PW_MAJOR_VERSION -lt $REQUIRED_PWLIB_MAJOR ; then
    AC_MSG_RESULT(Major version less than required $REQUIRED_PWLIB_MAJOR)
    exit 1
  fi
  if test $PW_MINOR_VERSION -lt $REQUIRED_PWLIB_MINOR ; then
    AC_MSG_RESULT(Minor version less than required $REQUIRED_PWLIB_MINOR)
    exit 1
  fi
  if test $PW_BUILD_NUMBER -lt $REQUIRED_PWLIB_BUILD ; then
    AC_MSG_RESULT(Build version less than required $REQUIRED_PWLIB_BUILD)
    exit 1
  fi
  AC_MSG_RESULT(ok)
fi

AC_SUBST(PWLIBDIR)

dnl ########################################################################
dnl check for setting minimum size

AC_ARG_ENABLE(minsize,
       [  --enable-minsize        minimise the size of the lib by disabling least used options])

dnl these are the default configure settings
default_audio=yes
default_video=yes
default_asntracing=yes
default_h450=yes
default_h501=yes
default_t38=yes
default_h224=yes
default_h230=yes
default_h239=yes
default_h248=yes
default_h249=yes
default_h341=yes
default_h350=yes
default_h460=yes
default_t120=yes
default_gnugk=no
default_file=yes

if test "${enable_minsize}x" = "yesx" ; then
  default_audio=yes
  default_video=yes
  default_asntracing=no
  default_h450=no
  default_h501=no
  default_t38=no
  default_h224=no
  default_h230=no
  default_h239=no
  default_h248=no
  default_h249=no
  default_h341=no
  default_h350=no
  default_h460=no
  default_t120=no
  default_gnugk=no
  default_file=no
fi


dnl ########################################################################
dnl check for disbling ASN tracing

AC_ARG_ENABLE(asntracing,
       [  --disable-asntracing    disable ASN tracing])

if test "${enable_asntracing}z" = "z" ; then
  enable_asntracing=$default_asntracing
fi

if test "$enable_asntracing" = "no" ; then
  AC_MSG_NOTICE(Disabling ASN tracing)
  AC_SUBST(NOTRACE, 1)
fi

dnl ########################################################################
dnl Enable audio subsystems

dnl MSWIN_DISPLAY audio,Audio Software Codecs
dnl MSWIN_DEFINE  audio,H323_AUDIO_CODECS

AC_ARG_ENABLE(audio,
       [  --disable-audio         disable audio codecs])

if test "${enable_audio}z" = "z" ; then
  enable_audio=yes
fi

if test "$enable_audio" = "no" ; then
  AC_MSG_NOTICE(Disabling audio functions)
  AC_SUBST(NOAUDIOCODECS, 1)
else
  AC_DEFINE(H323_AUDIO_CODECS, 1)

  AC_ARG_ENABLE(embeddedgsm,
         [  --enable-embeddedgsm    embed GSM codec via static linking],
         EMBEDDED_GSM=$enableval)
  if test "${EMBEDDED_GSM}x" != "x" ; then
    AC_MSG_NOTICE(Embedding GSM codec)
    LDFLAGS="$LDFLAGS -L${OPENH323DIR}/plugins/audio/GSM0610"
    ENDLDLIBS="$ENDLDLIBS -lgsm0610_audio_s"
    AC_CHECK_LIB(gsm, gsm_create, GSM=1)
    if test "x$GSM" != "x"; then
      ENDLDLIBS="$ENDLDLIBS -lgsm"
    fi
    AC_DEFINE(H323_EMBEDDED_GSM)
  fi
fi

dnl ########################################################################
dnl Enable video subsystems

dnl MSWIN_DISPLAY video,Video Software Codecs
dnl MSWIN_DEFINE  video,H323_VIDEO

AC_ARG_ENABLE(video,
       [  --disable-video         disable video functions])

if test "${enable_video}z" = "z" ; then
  enable_video=$default_video
fi

if test "$enable_video" = "no" ; then
  AC_MSG_NOTICE(Disabling video functions)
  AC_SUBST(NOVIDEO, 1)
else
  AC_DEFINE(H323_VIDEO, 1)
fi

dnl ########################################################################
dnl check for disabling H.224

dnl MSWIN_DISPLAY h224,H.224
dnl MSWIN_DEFINE  h224,H323_H224

AC_ARG_ENABLE(h224,
       [  --disable-h224          disable H.224])
if test "${enable_h224}x" = "x" ; then
  enable_h224=$default_h224
fi
if test "$enable_h224" = "yes" ; then
  H323_H224=1
  AC_DEFINE(H323_H224, 1)
else
  H323_H224=
  AC_MSG_NOTICE(Disabling H.224)
fi
AC_SUBST(H323_H224)

dnl ########################################################################
dnl check for disabling H.230

dnl MSWIN_DISPLAY h230,H.230
dnl MSWIN_DEFINE  h230,H323_H230

AC_ARG_ENABLE(h230,
       [  --disable-h230          disable H.230])
if test "${enable_h230}x" = "x" ; then
  enable_h230=$default_h230
fi
if test "$enable_h230" = "yes" ; then
  H323_H230=1
  AC_DEFINE(H323_H230, 1)
else
  H323_H230=
  AC_MSG_NOTICE(Disabling H.230)
fi
AC_SUBST(H323_H230)

dnl ########################################################################
dnl check for disabling H.239

dnl MSWIN_DISPLAY h239,H.239
dnl MSWIN_DEFINE  h239,H323_H239

AC_ARG_ENABLE(h239,
       [  --disable-h239          disable H.239])
if test "${enable_h239}x" = "x" ; then
  enable_h239=$default_h239
fi
if test "$enable_h239" = "yes" ; then
  H323_H239=1
  AC_DEFINE(H323_H239, 1)
else
  H323_H239=
  AC_MSG_NOTICE(Disabling H.239)
fi
AC_SUBST(H323_H239)

dnl ########################################################################
dnl check for disabling H.248

dnl MSWIN_DISPLAY h248,H.248
dnl MSWIN_DEFINE  h248,H323_H248

AC_ARG_ENABLE(h248,
       [  --disable-h248          enable H.248])
if test "${enable_h248}x" = "x" ; then
  enable_h248=$default_h248
fi
if test "$enable_h248" = "yes" ; then
  H323_H248=1
  AC_DEFINE(H323_H248, 1)
else
  H323_H248=
  AC_MSG_NOTICE(Disabling H.248)
fi
AC_SUBST(H323_H248)

dnl ########################################################################
dnl check for disabling H.249

dnl MSWIN_DISPLAY h249,H.249
dnl MSWIN_DEFINE  h249,H323_H249

AC_ARG_ENABLE(h249,
       [  --disable-h249          disable H.249])
if test "${enable_h249}x" = "x" ; then
  enable_h249=$default_h249
fi
if test "$enable_h249" = "yes" ; then
  H323_H249=1
  AC_DEFINE(H323_H249, 1)
else
  H323_H249=
  AC_MSG_NOTICE(Disabling H.249)
fi
AC_SUBST(H323_H249)

dnl ########################################################################
dnl check for disabling H.341

dnl MSWIN_DISPLAY h341,H.341
dnl MSWIN_DEFINE  h341,H323_H341

AC_ARG_ENABLE(h341,
       [  --disable-h341          disable H.341])
if test "${enable_h341}x" = "x" ; then
  enable_h341=$default_h341
fi
if test "$enable_h341" = "yes" ; then
  H323_H341=1
  AC_DEFINE(H323_H341, 1)
else
  H323_H341=
  AC_MSG_NOTICE(Disabling H.341)
fi
AC_SUBST(H323_H341)

dnl ########################################################################
dnl check for disabling H.350

dnl MSWIN_DISPLAY h350,H.350
dnl MSWIN_DEFINE  h350,H323_H350

AC_ARG_ENABLE(h350,
       [  --disable-h350          disable H.350])
if test "${enable_h350}x" = "x" ; then
  enable_h350=$default_h350
fi
if test "$enable_h350" = "yes" ; then
  H323_H350=1
  AC_DEFINE(H323_H350, 1)
else
  H323_H350=
  AC_MSG_NOTICE(Disabling H.350)
fi
AC_SUBST(H323_H350)

dnl ########################################################################
dnl check for disabling H.450

dnl MSWIN_DISPLAY h450,H.450
dnl MSWIN_DEFINE  h450,H323_H450

AC_ARG_ENABLE(h450,
       [  --disable-h450          disable H.450])
if test "${enable_h450}x" = "x" ; then
  enable_h450=$default_h450
fi
if test "$enable_h450" = "yes" ; then
  H323_H450=1
  AC_DEFINE(H323_H450, 1)
else
  H323_H450=
  AC_MSG_NOTICE(Disabling H.450)
fi
AC_SUBST(H323_H450)

dnl ########################################################################
dnl check for disabling H.460

dnl MSWIN_DISPLAY h460,H.460
dnl MSWIN_DEFINE  h460,H323_H460

AC_ARG_ENABLE(h460,
       [  --disable-h460          disable H.460])
if test "${enable_h460}x" = "x" ; then
  enable_h460=$default_h460
fi
if test "$enable_h460" = "yes" ; then
  H323_H460=1
  AC_DEFINE(H323_H460, 1)
else
  H323_H460=
  AC_MSG_NOTICE(Disabling H.460)
fi
AC_SUBST(H323_H460)

dnl ########################################################################
dnl check for disabling H.501

dnl MSWIN_DISPLAY h501,H.501
dnl MSWIN_DEFINE  h501,H323_H501

AC_ARG_ENABLE(h501,
       [  --disable-h501          disable H.501])
if test "${enable_h501}x" = "x" ; then
  enable_h501=$default_h501
fi
if test "$enable_h501" = "yes" ; then
  H323_H501=1
  AC_DEFINE(H323_H501, 1)
else
  H323_H501=
  AC_MSG_NOTICE(Disabling H.501)
fi
AC_SUBST(H323_H501)

dnl ########################################################################
dnl check for disabling T.38

dnl MSWIN_DISPLAY t38,T.38
dnl MSWIN_DEFINE  t38,H323_T38

AC_ARG_ENABLE(t38,
       [  --disable-t38           disable T.38])
if test "${enable_t38}x" = "x" ; then
  enable_t38=$default_t38
fi
if test "$enable_t38" = "yes" ; then
  AC_MSG_NOTICE(Enabling T.38)
  H323_T38=1
  AC_DEFINE(H323_T38, 1)
else
  H323_T38=
  AC_MSG_NOTICE(Disabling T.38)
fi
AC_SUBST(H323_T38)

dnl ########################################################################
dnl check for enabling T.120

dnl MSWIN_DISPLAY t120,T.120
dnl MSWIN_DEFINE  t120,H323_T120

AC_ARG_ENABLE(t120,
       [  --disable-t120          disable T.120])
if test "${enable_t120}x" = "x" ; then
  enable_t120=$default_t120
fi
if test "$enable_t120" = "no" ; then
  H323_T120=
  AC_MSG_NOTICE(Disabling T.120)
else
  H323_T20=1
  AC_DEFINE(H323_T120, 1)
fi
AC_SUBST(H323_T120)

dnl ########################################################################
dnl check for disabling GNUGK Support

dnl MSWIN_DISPLAY gnugk,GNUGKnat
dnl MSWIN_DEFINE  gnugk,H323_GNUGK

AC_ARG_ENABLE(gnugk,
       [  --disable-gnugk          disable GnuGk])
if test "${enable_gnugk}x" = "x" ; then
  enable_gnugk=$default_gnugk
fi
if test "$enable_gnugk" = "yes" ; then
  H323_GNUGK=1
  AC_DEFINE(H323_GNUGK, 1)
else
  H323_GNUGK=
  AC_MSG_NOTICE(Disabling GnuGk)
fi
AC_SUBST(H323_GNUGK)

dnl ########################################################################
dnl check for disabling File Transfer Support

dnl MSWIN_DISPLAY file,FileTransfer
dnl MSWIN_DEFINE  file,H323_FILE

AC_ARG_ENABLE(file,
       [  --disable-file          disable File Transfer])
if test "${enable_file}x" = "x" ; then
  enable_gnugk=$default_file
fi
if test "$enable_file" = "yes" ; then
  H323_FILE=1
  AC_DEFINE(H323_FILE, 1)
else
  H323_FILE=
  AC_MSG_NOTICE(Disabling File Transfer)
fi
AC_SUBST(H323_FILE)

dnl ########################################################################
dnl make directives

AC_SUBST(STDCCFLAGS)
AC_SUBST(LDFLAGS)
AC_SUBST(LDLIBS)
AC_SUBST(ENDLDLIBS)
AC_PROG_INSTALL

AC_CONFIG_FILES(openh323u.mak)
AC_CONFIG_FILES(Makefile)


dnl ########################################################################
dnl Header file

AC_CONFIG_HEADERS(include/openh323buildopts.h)


dnl ########################################################################
dnl Output configured files

AC_CONFIG_SUBDIRS(plugins)

AC_OUTPUT()


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


