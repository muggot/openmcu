dnl #######################################################################
dnl INIT

AC_INIT([OpenMCU-ru],[3.49.1],[OpenMCU-ru])
AC_CANONICAL_TARGET()
AM_INIT_AUTOMAKE()

AC_PROG_CXX
AC_LANG(C++)

AC_SUBST(PRODUCT, OpenMCU-ru)
AC_SUBST(PROG, openmcu-ru)

#AM_MAINTAINER_MODE

dnl ########################################################################
dnl OpenMCU-ru version

MAJOR_VERSION=`cat openmcu-ru/version.h | grep MAJOR_VERSION | cut -f3 -d' '`
MINOR_VERSION=`cat openmcu-ru/version.h | grep MINOR_VERSION | cut -f3 -d' '`
BUILD_NUMBER=`cat openmcu-ru/version.h | grep BUILD_NUMBER | cut -f3 -d' '`
BUILD_TYPE=`cat openmcu-ru/version.h | grep BUILD_TYPE | cut -f 3 -d ' ' | sed 's/BetaCode/-beta/' | sed 's/AlphaCode/-alpha/' | sed 's/ReleaseCode/\./'`
MCU_VERSION=$MAJOR_VERSION.$MINOR_VERSION.$BUILD_NUMBER
AC_SUBST(MAJOR_VERSION)
AC_SUBST(MINOR_VERSION)
AC_SUBST(BUILD_NUMBER)
AC_SUBST(BUILD_TYPE)
AC_SUBST(MCU_VERSION)

dnl ########################################################################
dnl PTLib version

PTLIB_MAJOR_VERSION=`cat libs/ptlib/version.h | grep MAJOR_VERSION | cut -f3 -d' '`
PTLIB_MINOR_VERSION=`cat libs/ptlib/version.h | grep MINOR_VERSION | cut -f3 -d' '`
PTLIB_BUILD_NUMBER=`cat libs/ptlib/version.h | grep BUILD_NUMBER | cut -f3 -d' '`
AC_SUBST(PTLIB_MAJOR_VERSION)
AC_SUBST(PTLIB_MINOR_VERSION)
AC_SUBST(PTLIB_BUILD_NUMBER)

dnl #######################################################################

top_srcdir=`pwd`


dnl ########################################################################
dnl install PREFIX

if test "x$prefix" = "xNONE"; then
  #INSTALLPREFIX="/usr/local"
  INSTALLPREFIX="/opt"
else
  INSTALLPREFIX="${prefix}"
fi
AC_SUBST(INSTALLPREFIX)

RPATH_LDFLAGS="${LDFLAGS}"

LIBAV_CPPFLAGS="${LIBAV_CPPFLAGS} ${CPPFLAGS}";
LIBAV_LDFLAGS="${LIBAV_LDFLAGS} ${LDFLAGS}";
LIBVPX_CPPFLAGS="${LIBVPX_CPPFLAGS} ${CPPFLAGS}";
LIBVPX_LDFLAGS="${LIBVPX_LDFLAGS} ${LDFLAGS}";

MCU_CPPFLAGS="${CPPFLAGS} -I${INSTALLPREFIX}/include -I${INSTALLPREFIX}/${PROG}/include -I/usr/include -I/usr/local/include"
MCU_LDFLAGS="${LDFLAGS} -L${INSTALLPREFIX}/lib -L${INSTALLPREFIX}/${PROG}/lib -L/usr/lib -L/usr/local/lib"
MCU_LDLIBS="${LDLIBS}"

export CPPFLAGS=${MCU_CPPFLAGS}
export LDFLAGS=${MCU_LDFLAGS}

export PKG_CONFIG_PATH=${INSTALLPREFIX}/lib/pkgconfig:${INSTALLPREFIX}/${PROG}/lib/pkgconfig:${PKG_CONFIG_PATH}:/usr/lib/pkgconfig:/usr/local/lib/pkgconfig
export LD_LIBRARY_PATH=${INSTALLPREFIX}/lib:${INSTALLPREFIX}/${PROG}/lib:${LD_LIBRARY_PATH}:/usr/lib:/usr/local/lib

dnl ########################################################################
dnl LIBDIR accordingly
LIBDIR=${libdir}
AC_SUBST(LIBDIR)


dnl ########################################################################
dnl set the OSTYPE and MACHTYPE

TARGETDIR=unix
OSTYPE=

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
		   LDLIBS="$LDLIBS -lstdc++ -lm" ;
		   ;;

  mingw*)	   OSTYPE=mingw ;
		   OS_TAG="P_MINGW" ;
		   OSRELEASE=`uname -r` ;
		   TARGETDIR=msos ;
		   SHAREDLIBEXT="dll" ;
		   STDCCFLAGS="$STDCCFLAGS -mms-bitfields" ;
		   ENDLDLIBS="-lwinmm -lwsock32 -lsnmpapi -lmpr -lcomdlg32 -lgdi32 -lavicap32" ;
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

   powerpc64 )     MACHTYPE=ppc64 ;
		   P_64BIT=1 ;
                   LIB64=1 ;
		   ;;

   ppc64 )         MACHTYPE=ppc64 ;
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


dnl #####################################################################
dnl get the gcc version

USE_GCC=
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

else
  USE_GCC=no
fi

dnl ########################################################################
dnl configuration openmcu-ru

AC_ARG_WITH(separator,[  ])
AC_ARG_WITH(config_prefix, [AS_HELP_STRING([--with-config-prefix],[config directory prefix],[40],[100])], [config_prefix=$withval])
AC_ARG_WITH(log_prefix, [AS_HELP_STRING([--with-log-prefix],[log directory prefix],[40],[100])], [log_prefix=$withval])
AC_ARG_WITH(pipe_prefix, [AS_HELP_STRING([--with-pipe-prefix],[pipe directory prefix],[40],[100])], [pipe_prefix=$withval])
AC_ARG_WITH(records_prefix, [AS_HELP_STRING([--with-records-prefix],[records directory prefix],[40],[100])], [records_prefix=$withval])

MCUDIR=openmcu-ru
MCU_DIR=${INSTALLPREFIX}/${MCUDIR}
MCU_BIN_DIR=${MCU_DIR}/bin
MCU_FONT_DIR=${MCU_DIR}/font
MCU_LIB_DIR=${MCU_DIR}/lib
MCU_RESOURCE_DIR=${MCU_DIR}/resource
MCU_SSL_DIR=${MCU_DIR}/ssl

if test "x$config_prefix" = "x"; then
  MCU_CONFIG_DIR=${MCU_DIR}/config
else
  MCU_CONFIG_DIR=${config_prefix}/${MCUDIR}
fi

if test "x$log_prefix" = "x"; then
  MCU_LOG_DIR=${MCU_DIR}/log
else
  MCU_LOG_DIR=${log_prefix}/${MCUDIR}
fi

if test "x$pipe_prefix" = "x"; then
  MCU_PIPE_DIR=${MCU_DIR}/pipe
else
  MCU_PIPE_DIR=${pipe_prefix}/${MCUDIR}
fi

if test "x$records_prefix" = "x"; then
  MCU_RECORDS_DIR=${MCU_DIR}/records
else
  MCU_RECORDS_DIR=${records_prefix}/${MCUDIR}
fi

AC_SUBST(MCUDIR)
AC_SUBST(MCU_DIR)
AC_SUBST(MCU_BIN_DIR)
AC_SUBST(MCU_CONFIG_DIR)
AC_SUBST(MCU_FONT_DIR)
AC_SUBST(MCU_LIB_DIR)
AC_SUBST(MCU_LOG_DIR)
AC_SUBST(MCU_PIPE_DIR)
AC_SUBST(MCU_RECORDS_DIR)
AC_SUBST(MCU_RESOURCE_DIR)
AC_SUBST(MCU_SSL_DIR)

AC_CONFIG_FILES(openmcu-ru/Makefile)
AC_CONFIG_FILES(openmcu-ru/config.h)

dnl ########################################################################
dnl configuration ptlib

PWLIBDIR=${top_srcdir}/libs/ptlib
PTLIBDIR="${PWLIBDIR}"
PTLIB_CONFIG=${PWLIBDIR}/make/ptlib-config

export PWLIBDIR=${PWLIBDIR}
export PTLIBDIR=${PTLIBDIR}
export PTLIB_CONFIG=${PTLIB_CONFIG}

AC_SUBST(PWLIBDIR)
AC_SUBST(PTLIBDIR)
AC_SUBST(PTLIB_CONFIG)

AC_CONFIG_SUBDIRS(libs/ptlib)
AC_CONFIG_FILES(libs/ptlib/Makefile)

dnl ########################################################################
dnl configuration h323plus

H323PLUSDIR=${top_srcdir}/libs/h323plus
AC_SUBST(H323PLUSDIR)
AC_CONFIG_SUBDIRS(libs/h323plus)
AC_CONFIG_FILES(libs/h323plus/Makefile)

dnl ########################################################################
dnl h323plus/ptlib

if test ${PTLIB_MAJOR_VERSION} -eq 2 && test ${PTLIB_MINOR_VERSION} -gt 3; then
  PTLIB_LIBDIR="${PTLIBDIR}/lib_${OSTYPE}_${MACHTYPE}"
  PTLIB_SO_FILE="libpt.so.${PTLIB_MAJOR_VERSION}.${PTLIB_MINOR_VERSION}.${PTLIB_BUILD_NUMBER}"
  PTLIB_A_FILE="libpt_s.a"
  PTLIB_LINK_FILE="pt"
  H323PLUS_LIBDIR="${H323PLUSDIR}/lib"
  H323PLUS_SO_FILE="libh323_${OSTYPE}_${MACHTYPE}_.so.1.20.2"
  H323PLUS_A_FILE="libh323_${OSTYPE}_${MACHTYPE}__s.a"
  H323PLUS_LINK_FILE="h323_${OSTYPE}_${MACHTYPE}_"
else
  PTLIB_LIBDIR="${PTLIBDIR}/lib"
  PTLIB_SO_FILE="libpt_${OSTYPE}_${MACHTYPE}_r.so.${PTLIB_MAJOR_VERSION}.${PTLIB_MINOR_VERSION}.${PTLIB_BUILD_NUMBER}"
  PTLIB_A_FILE="libpt_${OSTYPE}_${MACHTYPE}_r_s.a"
  PTLIB_LINK_FILE="pt_${OSTYPE}_${MACHTYPE}_r"
  H323PLUS_LIBDIR="${H323PLUSDIR}/lib"
  H323PLUS_SO_FILE="libh323_${OSTYPE}_${MACHTYPE}_r.so.1.20.2"
  H323PLUS_A_FILE="libh323_${OSTYPE}_${MACHTYPE}_r_s.a"
  H323PLUS_LINK_FILE="h323_${OSTYPE}_${MACHTYPE}_r"
fi

if test ${PTLIB_MAJOR_VERSION} -eq 2 && test ${PTLIB_MINOR_VERSION} -gt 0; then
  H323BUILDOPTS="CFLAGS=\"-DPBoolean=BOOL -DP_HAS_PLUGINS=1\""
fi

AC_SUBST(PTLIB_LIBDIR)
AC_SUBST(PTLIB_SO_FILE)
AC_SUBST(PTLIB_A_FILE)
AC_SUBST(PTLIB_LINK_FILE)
AC_SUBST(H323PLUS_LIBDIR)
AC_SUBST(H323PLUS_SO_FILE)
AC_SUBST(H323PLUS_A_FILE)
AC_SUBST(H323PLUS_LINK_FILE)

AC_SUBST(H323BUILDOPTS)
AC_SUBST(MCUBUILDOPTS)

dnl ########################################################################
dnl arg

resample=yes
resampler_ffmpeg=yes
resampler_libav=yes
resampler_libsamplerate=yes
libfreetype=yes
libjpeg=yes
libyuv=yes
openssl=yes
libsrtp=yes
libzrtp=yes

AC_ARG_ENABLE(separator, [ ])
AC_ARG_ENABLE(resample, [AS_HELP_STRING([--disable-resample],[Disable resampling (default enabled)],[40],[100])], [resample="$enableval"])
AC_ARG_ENABLE(resampler-ffmpeg, [AS_HELP_STRING([--disable-resampler-ffmpeg],[Enable check only libswresample resampler(default auto)],[40],[100])], [resampler_ffmpeg="yes" resampler_libav="no" resampler_libsamplerate="no"])
AC_ARG_ENABLE(resampler-libav, [AS_HELP_STRING([--disable-resampler-libav],[Enable check only libavresample resampler(default auto)],[40],[100])], [resampler_ffmpeg="no" resampler_libav="yes" resampler_libsamplerate="no"])
AC_ARG_ENABLE(resampler-libsamplerate, [AS_HELP_STRING([--disable-resampler-libsamplerate],[Enable check only libsamplerate resampler(default auto)],[40],[100])], [resampler_ffmpeg="no" resampler_libav="no" resampler_libsamplerate="yes"])
AC_ARG_ENABLE(separator, [  ])
AC_ARG_ENABLE(libfreetype, [AS_HELP_STRING([--disable-libfreetype],[Disable subtitles (default enabled)],[40],[100])], [libfreetype="$enableval"])
AC_ARG_ENABLE(libjpeg, [AS_HELP_STRING([--disable-libjpeg],[Disable JPEG Frame Encoder for browser (default enabled)],[40],[100])], [libjpeg="$enableval"])
AC_ARG_ENABLE(libyuv, [AS_HELP_STRING([--disable-libyuv],[Disable libyuv to resize video (default enabled)],[40],[100])], [libyuv="$enableval"])
AC_ARG_ENABLE(separator, [   ])
AC_ARG_ENABLE(openssl, [AS_HELP_STRING([--disable-openssl],[Disable OpenSSL support (default enabled)],[40],[100])], [openssl="$enableval"])
AC_ARG_ENABLE(libsrtp, [AS_HELP_STRING([--disable-libsrtp],[Disable SRTP support for SIP (default enabled)],[40],[100])], [libsrtp="$enableval"])
AC_ARG_ENABLE(libzrtp, [AS_HELP_STRING([--disable-libzrtp],[Disable ZRTP support for SIP (default enabled)],[40],[100])], [libzrtp="$enableval"])

dnl ########################################################################
dnl configure openssl

AC_SUBST(USE_OPENSSL, 0)
if test "${openssl}" = "yes"; then
  AC_CHECK_HEADERS(openssl/ssl.h, USE_OPENSSL=1, USE_OPENSSL=0, [-])
  if test ${USE_OPENSSL} = 1 ; then
    MCU_LDLIBS="$MCU_LDLIBS -lssl"
  else
    AC_MSG_ERROR(openssl headers not found!)
  fi
fi

dnl ########################################################################
dnl configure srtp

AC_SUBST(USE_LIBSRTP, 0)
AC_SUBST(COMPILE_LIBSRTP, 0)
AC_SUBST(LIBSRTP_DIR, ${top_srcdir}/libs/srtp)

if test "${libsrtp}" = "yes"; then
  AC_CHECK_HEADERS(srtp/srtp.h, USE_LIBSRTP=1, USE_LIBSRTP=0, [-])
  if test ${USE_LIBSRTP} = 1 ; then
    AC_CHECK_LIB(srtp, srtp_protect, USE_LIBSRTP=1, USE_LIBSRTP=0)
  fi
  if test ${USE_LIBSRTP} = 1 ; then
    MCU_CPPFLAGS="$MCU_CPPFLAGS `pkg-config --silence-errors --cflags-only-I libsrtp`";
    MCU_LDFLAGS="$MCU_LDFLAGS `pkg-config --silence-errors --libs-only-L libsrtp`";
    MCU_RFLAGS="$MCU_RFLAGS `pkg-config --silence-errors --libs-only-L libsrtp`";
    MCU_LDLIBS="$MCU_LDLIBS -lsrtp"
  fi
  if test ${USE_LIBSRTP} = 0 ; then
    AC_CONFIG_SUBDIRS(libs/srtp)
    MCU_SFLAGS="$MCU_SFLAGS -Wl,--start-group ../libs/srtp/libsrtp.a -Wl,--end-group"
    MCU_CPPFLAGS="$MCU_CPPFLAGS -I../libs/srtp/include -I../libs/srtp/crypto/include"
    USE_LIBSRTP=1
    COMPILE_LIBSRTP=1
  fi
fi
AM_CONDITIONAL(COMPILER_USE_LIBSRTP, [test ${COMPILE_LIBSRTP} = 1])

dnl ########################################################################
dnl configure srtp

AC_SUBST(USE_LIBZRTP, 0)
AC_SUBST(COMPILE_LIBZRTP, 0)
AC_SUBST(LIBZRTP_DIR, ${top_srcdir}/libs/libzrtp)

if test "${libzrtp}" = "yes"; then
  AC_CONFIG_SUBDIRS(libs/libzrtp)
  MCU_SFLAGS="$MCU_SFLAGS -Wl,--start-group ../libs/libzrtp/libzrtp.a ../libs/libzrtp/third_party/bnlib/libbn.a -Wl,--end-group"
  MCU_CPPFLAGS="$MCU_CPPFLAGS -I../libs/libzrtp/include -I../libs/libzrtp/third_party/bnlib -I../libs/libzrtp/third_party/bgaes"
  USE_LIBZRTP=1
  COMPILE_LIBZRTP=1
fi
AM_CONDITIONAL(COMPILER_USE_LIBZRTP, [test ${COMPILE_LIBZRTP} = 1])

dnl ########################################################################
dnl check for freetype library

AC_SUBST(USE_FREETYPE, 0)
if test "${libfreetype}" = "yes"; then
  AC_CHECK_HEADERS(freetype2/freetype/config/ftheader.h, USE_FREETYPE=1, USE_FREETYPE=0, [-])
  if test ${USE_FREETYPE} = 0 ; then
    AC_MSG_ERROR(freetype headers not found!)
  fi
  AC_CHECK_LIB(freetype, FT_Library_Version, USE_FREETYPE=1, USE_FREETYPE=0)
  if test ${USE_FREETYPE} = 0 ; then
    AC_MSG_ERROR(freetype library not found!)
  fi
  if test ${USE_FREETYPE} = 1 ; then
    MCU_CPPFLAGS="$MCU_CPPFLAGS `pkg-config --silence-errors --cflags-only-I freetype2`";
    MCU_LDFLAGS="$MCU_LDFLAGS `pkg-config --silence-errors --libs-only-L freetype2`";
    MCU_RFLAGS="$MCU_RFLAGS `pkg-config --silence-errors --libs-only-L freetype2`";
    MCU_LDLIBS="$MCU_LDLIBS -lfreetype"
  fi
fi

dnl ########################################################################
dnl check for jpeg library

AC_SUBST(USE_LIBJPEG, 0)
if test "${libjpeg}" = "yes"; then
  AC_CHECK_HEADERS(jpeglib.h, USE_LIBJPEG=1, USE_LIBJPEG=0, [-])
  if test ${USE_LIBJPEG} = 0 ; then
    AC_MSG_ERROR(jpeg headers not found!)
  fi
  AC_CHECK_LIB(jpeg, jpeg_set_defaults, USE_LIBJPEG=1, USE_LIBJPEG=0)
  if test ${USE_LIBJPEG} = 0 ; then
    AC_MSG_ERROR(jpeg library not found!)
  fi
  if test ${USE_LIBJPEG} = 1 ; then
    MCU_CPPFLAGS="$MCU_CPPFLAGS `pkg-config --silence-errors --cflags-only-I libjpeg`";
    MCU_LDFLAGS="$MCU_LDFLAGS `pkg-config --silence-errors --libs-only-L libjpeg`";
    MCU_RFLAGS="$MCU_RFLAGS `pkg-config --silence-errors --libs-only-L libjpeg`";
    MCU_LDLIBS="$MCU_LDLIBS -ljpeg"
  fi
fi

dnl ########################################################################
dnl check for yuv library

AC_SUBST(USE_LIBYUV, 0)
AC_SUBST(COMPILE_LIBYUV, 0)
AC_SUBST(LIBYUV_DIR, ${top_srcdir}/libs/libyuv)

if test "${libyuv}" = "yes"; then
  AC_CHECK_HEADERS(libyuv/scale.h, USE_LIBYUV=1, USE_LIBYUV=0, [-])
  if test ${USE_LIBYUV} = 1 ; then
    AC_CHECK_LIB(yuv, I420Scale, USE_LIBYUV=1, USE_LIBYUV=0)
  fi
  if test ${USE_LIBYUV} = 1 ; then
    MCU_CPPFLAGS="$MCU_CPPFLAGS `pkg-config --silence-errors --cflags-only-I libyuv`";
    MCU_LDFLAGS="$MCU_LDFLAGS `pkg-config --silence-errors --libs-only-L libyuv`";
    MCU_RFLAGS="$MCU_RFLAGS `pkg-config --silence-errors --libs-only-L libyuv`";
    MCU_LDLIBS="$MCU_LDLIBS -lyuv"
  fi
  if test ${USE_LIBYUV} = 0 ; then
    MCU_CPPFLAGS="$MCU_CPPFLAGS -I../libs/libyuv/include"
    MCU_LDFLAGS="$MCU_LDFLAGS -L../libs/libyuv";
    MCU_LDLIBS="$MCU_LDLIBS -lyuv"
    USE_LIBYUV=1
    COMPILE_LIBYUV=1
  fi
fi
AM_CONDITIONAL(COMPILER_USE_LIBYUV, [test ${COMPILE_LIBYUV} = 1])

dnl ########################################################################
dnl check for sofia-sip library

AC_SUBST(USE_LIBSOFIA, 1)
AC_SUBST(LIBSOFIA_DIR, ${top_srcdir}/libs/sofia-sip)
AC_CONFIG_SUBDIRS(libs/sofia-sip)
AM_CONDITIONAL(COMPILER_USE_LIBSOFIA, [test ${USE_LIBSOFIA} = 1])

MCU_LDFLAGS="$MCU_LDFLAGS -L../libs/sofia-sip/libsofia-sip-ua/.libs"
MCU_LDLIBS="$MCU_LDLIBS -lsofia-sip-ua"
MCU_CPPFLAGS="$MCU_CPPFLAGS -I../libs/sofia-sip/libsofia-sip-ua/bnf \
                    -I../libs/sofia-sip/libsofia-sip-ua/docs \
                    -I../libs/sofia-sip/libsofia-sip-ua/features \
                    -I../libs/sofia-sip/libsofia-sip-ua/http \
                    -I../libs/sofia-sip/libsofia-sip-ua/ipt \
                    -I../libs/sofia-sip/libsofia-sip-ua/iptsec \
                    -I../libs/sofia-sip/libsofia-sip-ua/msg \
                    -I../libs/sofia-sip/libsofia-sip-ua/nea \
                    -I../libs/sofia-sip/libsofia-sip-ua/nta \
                    -I../libs/sofia-sip/libsofia-sip-ua/nth \
                    -I../libs/sofia-sip/libsofia-sip-ua/nua \
                    -I../libs/sofia-sip/libsofia-sip-ua/sdp \
                    -I../libs/sofia-sip/libsofia-sip-ua/sip \
                    -I../libs/sofia-sip/libsofia-sip-ua/soa \
                    -I../libs/sofia-sip/libsofia-sip-ua/sresolv \
                    -I../libs/sofia-sip/libsofia-sip-ua/stun \
                    -I../libs/sofia-sip/libsofia-sip-ua/su \
                    -I../libs/sofia-sip/libsofia-sip-ua/tport \
                    -I../libs/sofia-sip/libsofia-sip-ua/url"

dnl ########################################################################
dnl check for video library

AC_CHECK_HEADERS(libavcodec/avcodec.h, USE_AVCODEC=1, USE_AVCODEC=0, [-])
if test ${USE_AVCODEC} = 0; then
  AC_MSG_ERROR(libavcodec headers not found!)
fi
AC_CHECK_LIB(avcodec, avcodec_version, USE_AVCODEC=1, USE_AVCODEC=0)
if test ${USE_AVCODEC} = 0; then
  AC_MSG_ERROR(libavcodec library not found!)
fi

LIBAV_CPPFLAGS="$LIBAV_CPPFLAGS `pkg-config --silence-errors --cflags-only-I libavcodec`";
LIBAV_LDFLAGS="$LIBAV_LDFLAGS `pkg-config --silence-errors --libs-only-L libavcodec`";

dnl ########################################################################
dnl check for ffmpeg path

AC_SUBST(FFMPEG_PATH)
AC_SUBST(FILE_FOUND, 0)

if test "${FILE_FOUND}" = "0"; then
  AC_CHECK_FILE(${MCU_BIN_DIR}/ffmpeg, [FILE_FOUND=1], [FILE_FOUND=0])
  if test "${FILE_FOUND}" = "1"; then
    FFMPEG_PATH=${MCU_BIN_DIR}/ffmpeg
  fi
fi
if test "${FILE_FOUND}" = "0"; then
  AC_CHECK_FILE(/usr/bin/ffmpeg, [FILE_FOUND=1], [FILE_FOUND=0])
  if test "${FILE_FOUND}" = "1"; then
    FFMPEG_PATH=/usr/bin/ffmpeg
  fi
fi
if test "${FILE_FOUND}" = "0"; then
  AC_CHECK_FILE(/usr/local/bin/ffmpeg, [FILE_FOUND=1], [FILE_FOUND=0])
  if test "${FILE_FOUND}" = "1"; then
    FFMPEG_PATH=/usr/local/bin/ffmpeg
  fi
fi
if test "${FILE_FOUND}" = "0"; then
  AC_CHECK_FILE(${MCU_BIN_DIR}/avconv, [FILE_FOUND=1], [FILE_FOUND=0])
  if test "${FILE_FOUND}" = "1"; then
    FFMPEG_PATH=${MCU_BIN_DIR}/avconv
  fi
fi
if test "${FILE_FOUND}" = "0"; then
  AC_CHECK_FILE(/usr/bin/avconv, [FILE_FOUND=1], [FILE_FOUND=0])
  if test "${FILE_FOUND}" = "1"; then
    FFMPEG_PATH=/usr/bin/avconv
  fi
fi
if test "${FILE_FOUND}" = "0"; then
  AC_CHECK_FILE(/usr/local/bin/avconv, [FILE_FOUND=1], [FILE_FOUND=0])
  if test "${FILE_FOUND}" = "1"; then
    FFMPEG_PATH=/usr/local/bin/avconv
  fi
fi

dnl ########################################################################
dnl check for resampling library

AC_SUBST(USE_SWRESAMPLE, 0)
AC_SUBST(USE_AVRESAMPLE, 0)
AC_SUBST(USE_LIBSAMPLERATE, 0)

if test "${resample}" = "yes"; then
  if test "${resampler_ffmpeg}" = "yes"; then
    AC_CHECK_HEADERS(libswresample/swresample.h, SWRESAMPLE=1, SWRESAMPLE=0, [-])
    if test "${SWRESAMPLE}" = "1"; then
      AC_CHECK_LIB(swresample, swr_convert, SWRESAMPLE=1, SWRESAMPLE=0)
    fi
  fi
  if test "${resampler_libav}" = "yes"; then
    AC_CHECK_LIB(avresample, avresample_convert, AVRESAMPLE=1, AVRESAMPLE=0)
  fi
  if test "${resampler_libsamplerate}" = "yes"; then
    AC_CHECK_LIB(samplerate, src_process, LIBSAMPLERATE=1, LIBSAMPLERATE=0)
  fi

  if test "${SWRESAMPLE}" = "1"; then
    USE_SWRESAMPLE=1
    MCU_CPPFLAGS="$MCU_CPPFLAGS ${LIBAV_CPPFLAGS}";
    MCU_LDFLAGS="$MCU_LDFLAGS ${LIBAV_LDFLAGS}";
    MCU_RFLAGS="$MCU_RFLAGS ${LIBAV_LDFLAGS}"
    MCU_LDLIBS="$MCU_LDLIBS -lswresample"
  elif test "${AVRESAMPLE}" = "1"; then
    USE_AVRESAMPLE=1
    MCU_CPPFLAGS="$MCU_CPPFLAGS ${LIBAV_CPPFLAGS}";
    MCU_LDFLAGS="$MCU_LDFLAGS ${LIBAV_LDFLAGS}";
    MCU_RFLAGS="$MCU_RFLAGS ${LIBAV_LDFLAGS}"
    MCU_LDLIBS="$MCU_LDLIBS -lavresample"
  elif test "${LIBSAMPLERATE}" = "1"; then
    USE_LIBSAMPLERATE=1
    MCU_CPPFLAGS="$MCU_CPPFLAGS `pkg-config --silence-errors --cflags-only-I samplerate`";
    MCU_LDFLAGS="$MCU_LDFLAGS `pkg-config --silence-errors --libs-only-L samplerate`";
    MCU_RFLAGS="$MCU_RFLAGS `pkg-config --silence-errors --libs-only-L samplerate`"
    MCU_LDLIBS="$MCU_LDLIBS -lsamplerate"
  else
    AC_MSG_WARN(libswresample/libavresample/libsamplerate not found!)
  fi
fi

dnl ########################################################################
dnl configuration plugins

P_DEFAULT_PLUGIN_DIR=${MCU_LIB_DIR}/ptlib
PLUGIN_INSTALLPREFIX=${P_DEFAULT_PLUGIN_DIR}
AUDIO_PLUGIN_DIR=codecs/audio
VIDEO_PLUGIN_DIR=codecs/video

AC_SUBST(P_DEFAULT_PLUGIN_DIR)
AC_SUBST(PLUGIN_INSTALLPREFIX)
AC_SUBST(AUDIO_PLUGIN_DIR)
AC_SUBST(VIDEO_PLUGIN_DIR)

dnl ########################################################################
dnl configuration plugins

h261=yes
h263=yes
h264=yes
vp8=yes
g722=yes
g7231=yes
g726=yes
g729=yes
ilbc=yes
speex=yes
silk=yes
opus=yes

AC_ARG_ENABLE(separator, [    ])
AC_ARG_ENABLE(h261, [AS_HELP_STRING([--disable-h261],[Disable H.261 support (default enabled)],[40],[100])], [h261="$enableval"])
AC_ARG_ENABLE(h263, [AS_HELP_STRING([--disable-h263],[Disable H.263 support (default enabled)],[40],[100])], [h263="$enableval"])
AC_ARG_ENABLE(h264, [AS_HELP_STRING([--disable-h264],[Disable H.264 support (default enabled)],[40],[100])], [h264="$enableval"])
AC_ARG_ENABLE(vp8, [AS_HELP_STRING([--disable-vp8],[Disable VP8 support (default enabled)],[40],[100])], [vp8="$enableval"])
AC_ARG_ENABLE(g722, [AS_HELP_STRING([--disable-g722],[Disable G.722 support (default enabled)],[40],[100])], [g722="$enableval"])
AC_ARG_ENABLE(g7231, [AS_HELP_STRING([--disable-g7231],[Disable G.723.1 support (default enabled)],[40],[100])], [g7231="$enableval"])
AC_ARG_ENABLE(g726, [AS_HELP_STRING([--disable-g726],[Disable G.726 support (default enabled)],[40],[100])], [g726="$enableval"])
AC_ARG_ENABLE(g729, [AS_HELP_STRING([--disable-g729],[Disable G.729 support (default enabled)],[40],[100])], [g729="$enableval"])
AC_ARG_ENABLE(ilbc, [AS_HELP_STRING([--disable-ilbc],[Disable iLBC support (default enabled)],[40],[100])], [ilbc="$enableval"])
AC_ARG_ENABLE(speex, [AS_HELP_STRING([--disable-speex],[Disable Speex support (default enabled)],[40],[100])], [speex="$enableval"])
AC_ARG_ENABLE(silk, [AS_HELP_STRING([--disable-silk],[Disable SILK support (default enabled)],[40],[100])], [silk="$enableval"])
AC_ARG_ENABLE(opus, [AS_HELP_STRING([--disable-opus],[Disable OPUS support (default enabled)],[40],[100])], [opus="$enableval"])

vp8_ffmpeg=yes
AC_ARG_ENABLE(separator, [    ])
AC_ARG_ENABLE(vp8_ffmpeg, [AS_HELP_STRING([--disable-vp8-ffmpeg],[Disable VP8 ffmpeg decoding (default enabled)],[40],[100])], [vp8_ffmpeg="$enableval"])

PLUGINS="h261 h263 h264 vp8 g722 g7231 g726 g729 ilbc silk opus"
AC_SUBST(PLUGINS)

PLUGIN_H261_DIR=${top_srcdir}/plugins/H.261-vic
AC_SUBST(PLUGIN_H261_DIR)
if test "${h261}" = "yes"; then
    AC_CONFIG_FILES(plugins/H.261-vic/Makefile)
    PLUGIN_DIRS="$PLUGIN_DIRS ${PLUGIN_H261_DIR}"
fi

PLUGIN_H263_DIR=${top_srcdir}/plugins/H.263-1998.3
AC_SUBST(PLUGIN_H263_DIR)
if test "${h263}" = "yes"; then
    AC_CONFIG_FILES(plugins/H.263-1998.3/Makefile)
    PLUGIN_DIRS="$PLUGIN_DIRS ${PLUGIN_H263_DIR}"
fi

PLUGIN_H264_DIR=${top_srcdir}/plugins/H.264_123
AC_SUBST(PLUGIN_H264_DIR)
if test "${h264}" = "yes"; then
    AC_CONFIG_FILES(plugins/H.264_123/Makefile)
    PLUGIN_DIRS="$PLUGIN_DIRS ${PLUGIN_H264_DIR} "
fi

PLUGIN_VP8_DIR=${top_srcdir}/plugins/VP8-WebM
AC_SUBST(PLUGIN_VP8_DIR)
if test "${vp8}" = "yes"; then
    AC_CHECK_HEADERS(vpx/vp8.h, USE_VP8=1, USE_VP8=0, [-])
    if test ${USE_VP8} = 1 ; then
      AC_CHECK_LIB(vpx, vpx_codec_encode, USE_VP8=1, USE_VP8=0)
    fi
    if test ${USE_VP8} = 1 ; then
        LIBVPX_CPPFLAGS="$LIBVPX_CPPFLAGS `pkg-config --silence-errors --cflags-only-I libvpx`";
        LIBVPX_LDFLAGS="$LIBVPX_LDFLAGS `pkg-config --silence-errors --libs-only-L libvpx`";
        if test "${vp8_ffmpeg}" = "yes"; then
          LIBVPX_CPPFLAGS="$LIBAV_CPPFLAGS $LIBVPX_CPPFLAGS -DFFMPEG_DECODER=1";
          LIBVPX_LDFLAGS="$LIBAV_LDFLAGS $LIBVPX_LDFLAGS -lavcodec -lavutil";
        fi
	AC_CONFIG_FILES(plugins/VP8-WebM/Makefile)
	PLUGIN_DIRS="$PLUGIN_DIRS ${PLUGIN_VP8_DIR} "
    else
	AC_MSG_ERROR(VP8 codec not found!)
    fi
fi

PLUGIN_G722_DIR=${top_srcdir}/plugins/G722
AC_SUBST(PLUGIN_G722_DIR)
if test "${g722}" = "yes"; then
    AC_CONFIG_FILES(plugins/G722/Makefile)
    PLUGIN_DIRS="$PLUGIN_DIRS ${PLUGIN_G722_DIR} "
fi

PLUGIN_G7231_DIR=${top_srcdir}/plugins/G7231
AC_SUBST(PLUGIN_G7231_DIR)
if test "${g7231}" = "yes"; then
    AC_CONFIG_FILES(plugins/G7231/Makefile)
    PLUGIN_DIRS="$PLUGIN_DIRS ${PLUGIN_G7231_DIR} "
fi

PLUGIN_G726_DIR=${top_srcdir}/plugins/G726
AC_SUBST(PLUGIN_G726_DIR)
if test "${g726}" = "yes"; then
    AC_CONFIG_FILES(plugins/G726/Makefile)
    PLUGIN_DIRS="$PLUGIN_DIRS ${PLUGIN_G726_DIR} "
fi

PLUGIN_G729_DIR=${top_srcdir}/plugins/G729
AC_SUBST(PLUGIN_G729_DIR)
if test "${g729}" = "yes"; then
    AC_CONFIG_FILES(plugins/G729/Makefile)
    PLUGIN_DIRS="$PLUGIN_DIRS ${PLUGIN_G729_DIR} "
fi

PLUGIN_ILBC_DIR=${top_srcdir}/plugins/iLBC
AC_SUBST(PLUGIN_ILBC_DIR)
if test "${ilbc}" = "yes"; then
    AC_CONFIG_FILES(plugins/iLBC/Makefile)
    PLUGIN_DIRS="$PLUGIN_DIRS ${PLUGIN_ILBC_DIR} "
fi

PLUGIN_SILK_DIR=${top_srcdir}/plugins/SILK_B40
AC_SUBST(PLUGIN_SILK_DIR)
if test "${silk}" = "yes"; then
    AC_CONFIG_FILES(plugins/SILK_B40/Makefile)
    PLUGIN_DIRS="$PLUGIN_DIRS ${PLUGIN_SILK_DIR} "
fi

dnl ########################################################################
dnl plugin SPEEX

AC_SUBST(USE_LIBSPEEX, 0)
AC_SUBST(COMPILE_LIBSPEEX, 0)
AC_SUBST(PLUGIN_SPEEX_DIR, ${top_srcdir}/plugins/Speex)

AC_SUBST(SPEEX_LDFLAGS)
AC_SUBST(SPEEX_CPPFLAGS)

if test "${speex}" = "yes"; then
  AC_CHECK_HEADERS(speex/speex.h, USE_LIBSPEEX=1, USE_LIBSPEEX=0, [-])
  if test ${USE_LIBSPEEX} = 1 ; then
    AC_CHECK_LIB(speex, speex_encoder_init, USE_LIBSPEEX=1, USE_LIBSPEEX=0)
  fi
  if test ${USE_LIBSPEEX} = 1 ; then
    SPEEX_CPPFLAGS="`pkg-config --silence-errors --cflags-only-I speex`";
    SPEEX_LDFLAGS="`pkg-config --silence-errors --libs-only-L speex` -lspeex";
    MCU_RFLAGS="$MCU_RFLAGS `pkg-config --silence-errors --libs-only-L speex`";
  fi
  if test ${USE_LIBSPEEX} = 0 ; then
    SPEEX_CPPFLAGS="-I../../libs/speex/include"
    SPEEX_LDFLAGS="-L../../libs/speex/libspeex/.libs -lspeex"
    AC_SUBST(LIBSPEEX_DIR, ${top_srcdir}/libs/speex)
    AC_CONFIG_SUBDIRS(libs/speex)
    COMPILE_LIBSPEEX=1
  fi
  AC_CONFIG_FILES(plugins/Speex/Makefile)
  PLUGIN_DIRS="$PLUGIN_DIRS ${PLUGIN_SPEEX_DIR} "
fi
AM_CONDITIONAL(COMPILER_USE_LIBSPEEX, [test ${COMPILE_LIBSPEEX} = 1])


dnl ########################################################################
dnl plugin OPUS

AC_SUBST(USE_LIBOPUS, 0)
AC_SUBST(COMPILE_LIBOPUS, 0)
AC_SUBST(PLUGIN_OPUS_DIR, ${top_srcdir}/plugins/OPUS)

AC_SUBST(OPUS_LDFLAGS)
AC_SUBST(OPUS_CPPFLAGS)

if test "${opus}" = "yes"; then
  AC_CHECK_HEADERS(opus/opus.h, USE_LIBOPUS=1, USE_LIBOPUS=0, [-])
  if test ${USE_LIBOPUS} = 1 ; then
    AC_CHECK_LIB(opus, opus_encoder_init, USE_LIBSPEEX=1, USE_LIBSPEEX=0)
  fi
  if test ${USE_LIBOPUS} = 1 ; then
    OPUS_CPPFLAGS="`pkg-config --silence-errors --cflags-only-I opus`";
    OPUS_LDFLAGS="`pkg-config --silence-errors --libs-only-L opus` -lopus";
    MCU_RFLAGS="$MCU_RFLAGS `pkg-config --silence-errors --libs-only-L opus`";
  fi
  if test ${USE_LIBOPUS} = 0 ; then
    OPUS_CPPFLAGS="-I../../libs/opus/include"
    OPUS_LDFLAGS="-L../../libs/opus/.libs -lopus"
    AC_SUBST(LIBOPUS_DIR, ${top_srcdir}/libs/opus)
    AC_CONFIG_SUBDIRS(libs/opus)
    COMPILE_LIBOPUS=1
  fi
  AC_CONFIG_FILES(plugins/OPUS/Makefile)
  PLUGIN_DIRS="$PLUGIN_DIRS ${PLUGIN_OPUS_DIR}"
fi
AM_CONDITIONAL(COMPILER_USE_LIBOPUS, [test ${COMPILE_LIBOPUS} = 1])


dnl ########################################################################
dnl configure rpath stage1

rpath=yes
AC_ARG_ENABLE(separator, [      ])
AC_ARG_ENABLE(rpath, [AS_HELP_STRING([--disable-rpath],[do not hardcode runtime library paths (default auto)],[40],[100])], [rpath="$enableval"])


dnl ########################################################################
dnl configure static linking

static=no
AC_ARG_ENABLE(static, [AS_HELP_STRING([--enable-static],[build static linking with ptlib,h323plus (default disabled)],[40],[100])], [static="$enableval"])


AC_SUBST(ENABLE_STATIC, 0)
if test "${static}" = "yes"; then
  MCU_SFLAGS="${MCU_SFLAGS} -Wl,--start-group ${PTLIB_LIBDIR}/${PTLIB_A_FILE} ${H323PLUS_LIBDIR}/${H323PLUS_A_FILE} -Wl,--end-group"
  MCU_LDLIBS="$MCU_LDLIBS"
  ENABLE_STATIC=1
else
  MCU_LDFLAGS="${MCU_LDFLAGS} -L${PTLIB_LIBDIR} -L${H323PLUS_LIBDIR}"
  MCU_LDLIBS="${MCU_LDLIBS} -l${H323PLUS_LINK_FILE} -l${PTLIB_LINK_FILE}"
  ENABLE_STATIC=0
fi
AM_CONDITIONAL(COMPILER_NO_STATIC, [test ${ENABLE_STATIC} = 0])

dnl ########################################################################
dnl configure debugging

debug=yes
AC_ARG_ENABLE(debug, [AS_HELP_STRING([--disable-debug],[Disable debugging information (default enabled)],[40],[100])], [debug="$enableval"])

if test "${debug}" = "yes"; then
  CFLAGS="-g $CFLAGS"
fi

dnl ########################################################################
dnl configure rpath stage2

RPATH=
if test "${OSTYPE}" != "linux" && test "${OSTYPE}" != "FreeBSD"; then
  rpath=no
fi

if test "${rpath}" = "yes"; then
  RPATH_LDFLAGS="${RPATH_LDFLAGS} ${MCU_RFLAGS} ${LIBAV_LDFLAGS} ${LIBVPX_LDFLAGS}"
  RPATH_LDFLAGS=$(echo $(printf '%s\n' $RPATH_LDFLAGS | sort -u))
  RPATH_LDFLAGS="-L${MCU_LIB_DIR} ${RPATH_LDFLAGS}"

  # ldconfig search directories
  if test "${OSTYPE}" = "FreeBSD"; then
    IN=`ldconfig -r | grep "directories:" | awk -F'directories:' '{print $2}'`
    OIFS=$IFS
    IFS=':'
    for path in ${IN}; do
      LD_PATH="${LD_PATH} ${path}"
    done
    IFS=$OIFS
  else
    LD_PATH=`ldconfig -v | grep "/" | awk -F: '{print $1}'`
  fi

  # RPATH keep only directories, which are not in ldconfig
  RPATH="-Wl"
  for r_path in ${RPATH_LDFLAGS}; do
    if test `echo ${r_path} | grep \\\-L`; then
      path=`echo ${r_path} | sed -e 's/-L//'`
      use_rpath=yes
      for ld_path in ${LD_PATH} ; do
        if test "${ld_path}" = "${path}"; then
          use_rpath=no
        fi
      done
      if test "${use_rpath}" = "yes"; then
        RPATH="${RPATH},-R${path}"
      fi
    fi
  done
  if test "${RPATH}" = "-Wl"; then
    RPATH=
  fi
fi

dnl ########################################################################
dnl removing duplicate paths

MCU_LDFLAGS="${MCU_LDFLAGS} -L${MCU_LIB_DIR}"

MCU_CPPFLAGS="-I../libs/ptlib/include -I../libs/h323plus/include ${MCU_CPPFLAGS}"
MCU_CPPFLAGS=$(echo $(printf '%s\n' $MCU_CPPFLAGS | sort -u))

MCU_LDFLAGS=$(echo $(printf '%s\n' $MCU_LDFLAGS | sort -u))

LIBAV_CPPFLAGS=$(echo $(printf '%s\n' $LIBAV_CPPFLAGS | sort -u))
LIBAV_LDFLAGS=$(echo $(printf '%s\n' $LIBAV_LDFLAGS | sort -u))
LIBVPX_CPPFLAGS=$(echo $(printf '%s\n' $LIBVPX_CPPFLAGS | sort -u))
LIBVPX_LDFLAGS=$(echo $(printf '%s\n' $LIBVPX_LDFLAGS | sort -u))

dnl ########################################################################
dnl final declarations

AC_SUBST(PLUGIN_DIRS)

AC_SUBST(LIBAV_CPPFLAGS)
AC_SUBST(LIBAV_LDFLAGS)
AC_SUBST(LIBVPX_CPPFLAGS)
AC_SUBST(LIBVPX_LDFLAGS)

AC_SUBST(MCU_LDFLAGS)
AC_SUBST(MCU_LDLIBS)
AC_SUBST(MCU_CPPFLAGS)

AC_SUBST(RPATH)
AC_SUBST(MCU_SFLAGS)

AC_SUBST(STDCCFLAGS)
AC_SUBST(STDCXXFLAGS)
AC_SUBST(OPTSTDCCFLAGS)
AC_SUBST(CFLAGS)
AC_SUBST(CPPFLAGS)
AC_SUBST(LDFLAGS)
AC_SUBST(LDLIBS)
AC_SUBST(DEBUG_FLAG)
AC_PROG_INSTALL

AC_OUTPUT(Makefile)

dnl ########################################################################
dnl replace default targets

sed -i -r 's/all: all-recursive/all: all/g' Makefile


dnl ########################################################################
dnl replace default install path

sed -i -r "s#^\#define P_DEFAULT_PLUGIN_DIR.*#\#define P_DEFAULT_PLUGIN_DIR \"${P_DEFAULT_PLUGIN_DIR}\"#g" ${PTLIBDIR}/include/ptbuildopts.h

if test ${PTLIB_MAJOR_VERSION} -eq 2 && test ${PTLIB_MINOR_VERSION} -gt 0; then
  sed -i -r "s#${PTLIBDIR}#${PTLIBDIR}\\nPTLIBDIR\\t= ${PTLIBDIR}#g" ${H323PLUSDIR}/openh323u.mak
fi

dnl ########################################################################
dnl Summary
dnl ########################################################################
echo ""
echo "============== OpenMCU-ru Build Configuration ================="
echo "                         OS Type  :  $target_os"
echo "                    Machine Type  :  $target_cpu"
echo "          Installing into prefix  :  $INSTALLPREFIX"
echo "          Installing directory    :  $MCU_DIR"

echo ""
if test "x${libfreetype}" = "xyes" ; then
echo "                       Subtitles  :  enabled"
else
echo "                       Subtitles  :  disabled"
fi

if test "x${libjpeg}" = "xyes" ; then
echo " JPEG Frame Encoder (for browser) :  enabled"
else
echo "JPEG Frame Encoder (for browser)  :  disabled"
fi

if test "x${libyuv}" = "xyes" ; then
echo "      Use libyuv to resize video  :  enabled"
else
echo "      Use libyuv to resize video  :  disabled"
fi

if test "x${resample}" = "xyes" ; then
if test "x${resampler_ffmpeg}" = "xyes" ; then
echo "        External Audio Resampler  :  enabled - ffmpeg"
else
if test "x${resampler_libav}" = "xyes" ; then
echo "        External Audio Resampler  :  enabled - libav"
else
if test "x${resampler_libsamplerate}" = "xyes" ; then
echo "        External Audio Resampler  :  enabled - libsamplerate"
else
echo "        External Audio Resampler  :  disabled (built-in will used)"
fi
fi
fi
else
echo "        External Audio Resampler  :  disabled (built-in will used)"
fi

echo ""
if test "x${vp8}" = "xyes" ; then
echo "                VP8 Video Plugin  :  enabled"
else
echo "                VP8 Video Plugin  :  disabled"
fi
if test "x${h264}" = "xyes" ; then
echo "              H.264 Video Plugin  :  enabled"
else
echo "              H.264 Video Plugin  :  disabled"
fi
if test "x${h263}" = "xyes" ; then
echo "       H.263/H.263P Video Plugin  :  enabled"
else
echo "       H.263/H.263P Video Plugin  :  disabled"
fi
if test "x${h261}" = "xyes" ; then
echo "              H.261 Video Plugin  :  enabled"
else
echo "              H.261 Video Plugin  :  disabled"
fi

if test "x${opus}" = "xyes" ; then
echo "               OPUS Audio Plugin  :  enabled"
else
echo "               OPUS Audio Plugin  :  disabled"
fi
if test "x${silk}" = "xyes" ; then
echo "               SILK Audio Plugin  :  enabled"
else
echo "               SILK Audio Plugin  :  disabled"
fi
if test "x${g722}" = "xyes" ; then
echo "              G.722 Audio Plugin  :  enabled"
else
echo "              G.722 Audio Plugin  :  disabled"
fi
if test "x${speex}" = "xyes" ; then
echo "              SPEEX Audio Plugin  :  enabled"
else
echo "              SPEEX Audio Plugin  :  disabled"
fi
if test "x${g7231}" = "xyes" ; then
echo "            G.723.1 Audio Plugin  :  enabled"
else
echo "            G.723.1 Audio Plugin  :  disabled"
fi
if test "x${ilbc}" = "xyes" ; then
echo "               iLBC Audio Plugin  :  enabled"
else
echo "               iLBC Audio Plugin  :  disabled"
fi
if test "x${g726}" = "xyes" ; then
echo "              G.726 Audio Plugin  :  enabled"
else
echo "              G.726 Audio Plugin  :  disabled"
fi
if test "x${g729}" = "xyes" ; then
echo "              G.729 Audio Plugin  :  enabled"
else
echo "              G.729 Audio Plugin  :  disabled"
fi

echo ""
echo " If all settings are OK, type make and make install "
echo "========================================================"
