dnl #######################################################################
dnl INIT

AC_INIT([OpenMCU-ru],[4.0],[OpenMCU-ru])
AC_CANONICAL_TARGET()
AM_INIT_AUTOMAKE()

AC_SUBST(PRODUCT, OpenMCU-ru)
AC_SUBST(PROG, openmcu-ru)

AC_LANG(C++)

OLD_CFLAGS=${CFLAGS}
AC_PROG_CXX
CFLAGS=${OLD_CFLAGS}

AC_PROG_INSTALL

#AM_MAINTAINER_MODE

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

dnl ########################################################################
dnl flags

RPATH_LDFLAGS="${RPATH_LDFLAGS} ${LDFLAGS}"

LIBAV_CPPFLAGS="${LIBAV_CPPFLAGS} ${CPPFLAGS}";
LIBAV_LDFLAGS="${LIBAV_LDFLAGS} ${LDFLAGS}";
LIBVPX_CPPFLAGS="${LIBVPX_CPPFLAGS} ${CPPFLAGS}";
LIBVPX_LDFLAGS="${LIBVPX_LDFLAGS} ${LDFLAGS}";

MCU_CPPFLAGS="-I${INSTALLPREFIX}/${PROG}/include ${CPPFLAGS} -I/usr/include -I/usr/local/include"
MCU_LDFLAGS="-L${INSTALLPREFIX}/${PROG}/lib ${LDFLAGS} -L/usr/lib -L/usr/local/lib"
MCU_LDLIBS="${MCU_LDLIBS} ${LDLIBS}"

# AC_CHECK_HEADERS
OLD_CPPFLAGS=${CPPFLAGS}
export CPPFLAGS=${MCU_CPPFLAGS}
# AC_CHECK_LIBS
OLD_LDFLAGS=${LDFLAGS}
export LDFLAGS=${MCU_LDFLAGS}

#pkg-config
export PKG_CONFIG_PATH=${INSTALLPREFIX}/${PROG}/lib/pkgconfig:${PKG_CONFIG_PATH}:/usr/lib/pkgconfig:/usr/local/lib/pkgconfig
export LD_LIBRARY_PATH=${INSTALLPREFIX}/${PROG}/lib:${LD_LIBRARY_PATH}:/usr/lib:/usr/local/lib

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

  freebsd*)        OSTYPE=FreeBSD ;
                   OS_TAG="P_FREEBSD" ;
                   if test "x$OSRELEASE" = "x"; then
                     OSRELEASE="`sysctl -n kern.osreldate`" ;
                   fi
                   need_pragma=yes ;
                   LDLIBS="$LDLIBS -lstdc++ -lm" ;
		   ;;

  mingw*)          OSTYPE=mingw ;
                   OS_TAG="P_MINGW" ;
                   OSRELEASE=`uname -r` ;
                   TARGETDIR=msos ;
                   SHAREDLIBEXT="dll" ;
                   STDCCFLAGS="$STDCCFLAGS -mms-bitfields" ;
                   ENDLDLIBS="-lwinmm -lwsock32 -lsnmpapi -lmpr -lcomdlg32 -lgdi32 -lavicap32" ;
                   ;;

  *)               OSTYPE="$target_os" ;
                   OS_TAG="P_UNKNOWN_OS" ;
                   OSRELEASE="\"${OSTYPE}_`uname -r`\"" ;
                   AC_MSG_WARN("OS $target_os not recognized - proceed with caution!") ;
                   ;;
esac

MACHTYPE=

case "$target_cpu" in
   x86 | i686 | i586 | i486 | i386) MACHTYPE=x86
                   ;;

   x86_64 | amd64 | hppa64 | ppc64* | s390x | aarch64)
                   MACHTYPE=$target_cpu
                   P_64BIT=1 ;
                   LIB64=1 ;
                   ;;

   alpha | alphaev56 | alphaev6 | alphaev67 | alphaev7) MACHTYPE=alpha ;
                   P_64BIT=1 ;
                   ;;

   hppa | sparc | ppc | s390) MACHTYPE=$target_cpu ;
                   ;;

   powerpc)        MACHTYPE=ppc ;
                   ;;

   powerpc64*)     MACHTYPE=ppc64 ;
                   P_64BIT=1 ;
                   LIB64=1 ;
                   ;;

   ia64)	   MACHTYPE=ia64 ;
                   P_64BIT=1 ;
                   ;;

   *)              MACHTYPE=$target_cpu ;
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
dnl gcc

if test "${USE_GCC}" = "yes"; then
  # undefined reference to `__sync_bool_compare_and_swap_1'
  if test ${GXX_MAJOR} -eq 4 && test ${GXX_MINOR} -eq 1; then
    if test "${MACHTYPE}" = "x86"; then
      CFLAGS="$CFLAGS -march=i686"
    fi
  fi
fi

dnl ########################################################################
dnl OpenMCU-ru version

MCU_MAJOR_VERSION=`cat openmcu-ru/version.h | grep MAJOR_VERSION | cut -f3 -d' '`
MCU_MINOR_VERSION=`cat openmcu-ru/version.h | grep MINOR_VERSION | cut -f3 -d' '`
MCU_BUILD_NUMBER=`cat openmcu-ru/version.h | grep BUILD_NUMBER | cut -f3 -d' '`
MCU_VERSION=$MAJOR_VERSION.$MINOR_VERSION.$BUILD_NUMBER
AC_SUBST(MCU_MAJOR_VERSION)
AC_SUBST(MCU_MINOR_VERSION)
AC_SUBST(MCU_BUILD_NUMBER)
AC_SUBST(MCU_VERSION)

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
dnl configuration ptlib/h323plus

AC_SUBST(USE_SRC_PTLIB, 1)
AC_SUBST(USE_SYSTEM_PTLIB, 0)
AC_SUBST(PTLIBDIR, ${top_srcdir}/libs/ptlib)
AC_SUBST(PWLIBDIR, ${PTLIBDIR})
AC_SUBST(H323PLUSDIR, ${top_srcdir}/libs/h323plus)
AC_SUBST(PTLIB_SYSTEM_DIRS, "${INSTALLPREFIX}/${PROG} /usr /usr/local")
AC_SUBST(H323PLUS_SYSTEM_DIRS, "${INSTALLPREFIX}/${PROG} /usr /usr/local")

AC_SUBST(PTLIB_BUILDOPTS)
AC_SUBST(H323PLUS_BUILDOPTS)

AC_SUBST(PTLIB_BUILDDIR)
AC_SUBST(PTLIB_SO_FILE)
AC_SUBST(PTLIB_A_FILE)
AC_SUBST(PTLIB_LINK_FILE)
AC_SUBST(H323PLUS_BUILDDIR)
AC_SUBST(H323PLUS_SO_FILE)
AC_SUBST(H323PLUS_A_FILE)
AC_SUBST(H323PLUS_LINK_FILE)

AC_SUBST(PTLIB_MAJOR_VERSION)
AC_SUBST(PTLIB_MINOR_VERSION)
AC_SUBST(PTLIB_BUILD_NUMBER)
AC_SUBST(H323PLUS_MAJOR_VERSION)
AC_SUBST(H323PLUS_MINOR_VERSION)
AC_SUBST(H323PLUS_BUILD_NUMBER)

# check source 1
if test ${USE_SYSTEM_PTLIB} = 0 ; then
  AC_CHECK_FILE(${PTLIBDIR}/make/ptbuildopts.mak, USE_SRC_PTLIB=1, USE_SRC_PTLIB=0)
  AS_UNSET([$as_ac_File])
fi
# system library
if test ${USE_SRC_PTLIB} = 0 ; then
  PTLIB_SYSTEM_DIR=
  OPENH323_SYSTEM_DIR=
  for dir in ${PTLIB_SYSTEM_DIRS} ; do
    AC_CHECK_FILE(${dir}/share/ptlib/make/ptbuildopts.mak, USE_SYSTEM_PTLIB=1, USE_SYSTEM_PTLIB=0)
    if test ${USE_SYSTEM_PTLIB} = 1; then
      AC_CHECK_LIB(pt, main, USE_SYSTEM_PTLIB=1, USE_SYSTEM_PTLIB=0)
      if test ${USE_SYSTEM_PTLIB} = 1; then
        PTLIB_SYSTEM_DIR=$dir
      fi
      break
    fi
  done
  for dir in ${H323PLUS_SYSTEM_DIRS} ; do
    AC_CHECK_FILE(${dir}/include/openh323/codec/opalplugin.h, USE_SYSTEM_PTLIB=1, USE_SYSTEM_PTLIB=0)
    if test ${USE_SYSTEM_PTLIB} = 1; then
      AC_CHECK_LIB(h323, main, USE_SYSTEM_PTLIB=1, USE_SYSTEM_PTLIB=0)
      if test ${USE_SYSTEM_PTLIB} = 1; then
        H323PLUS_SYSTEM_DIR=$dir
      fi
      break
    fi
  done
  if test ${USE_SYSTEM_PTLIB} = 1; then
    MCU_CPPFLAGS="${MCU_CPPFLAGS} -I${PTLIB_SYSTEM_DIR}/include/ptlib -I${H323PLUS_SYSTEM_DIR}/include/openh323"
    MCU_LDFLAGS="$MCU_LDFLAGS -L${PTLIB_SYSTEM_DIR}/lib -L${H323PLUS_SYSTEM_DIR}/lib";
    MCU_RFLAGS="$MCU_RFLAGS -L${PTLIB_SYSTEM_DIR}/lib -L${H323PLUS_SYSTEM_DIR}/lib";
    PTLIBDIR=${dir}/share/ptlib/
    PTLIB_BUILDDIR="${PTLIB_SYSTEM_DIR}/lib"
    H323PLUS_BUILDDIR="${H323PLUS_SYSTEM_DIR}/lib"
    PTLIB_LINK_FILE="pt"
    PTLIB_A_FILE="libpt_s.a"
    PTLIB_AD_FILE="libpt_d_s.a"
    H323PLUS_LINK_FILE="h323"
    H323PLUS_A_FILE="libh323_s.a"
    H323PLUS_AD_FILE="libh323_d_s.a"
  fi
fi
# check source 2
if test ${USE_SYSTEM_PTLIB} = 0 && test ${USE_SRC_PTLIB} = 0 ; then
  AC_CHECK_FILE(${PTLIBDIR}/make/ptbuildopts.mak, USE_SRC_PTLIB=1, USE_SRC_PTLIB=0)
  AS_UNSET([$as_ac_File])
fi
# compile from source
if test ${USE_SRC_PTLIB} = 1 ; then
  PTLIB_MAJOR_VERSION=`cat libs/ptlib/version.h | grep MAJOR_VERSION | cut -f3 -d' '`
  PTLIB_MINOR_VERSION=`cat libs/ptlib/version.h | grep MINOR_VERSION | cut -f3 -d' '`
  PTLIB_BUILD_NUMBER=`cat libs/ptlib/version.h | grep BUILD_NUMBER | cut -f3 -d' '`

  H323PLUS_MAJOR_VERSION=`cat libs/h323plus/version.h | grep MAJOR_VERSION | cut -f3 -d' '`
  H323PLUS_MINOR_VERSION=`cat libs/h323plus/version.h | grep MINOR_VERSION | cut -f3 -d' '`
  H323PLUS_BUILD_NUMBER=`cat libs/h323plus/version.h | grep BUILD_NUMBER | cut -f3 -d' '`

  if test ${PTLIB_MAJOR_VERSION} -eq 2 && test ${PTLIB_MINOR_VERSION} -gt 3; then
    PTLIB_BUILDDIR="${PTLIBDIR}/lib_${OSTYPE}_${MACHTYPE}"
    PTLIB_SO_FILE="libpt.so.${PTLIB_MAJOR_VERSION}.${PTLIB_MINOR_VERSION}.${PTLIB_BUILD_NUMBER}"
    PTLIB_A_FILE="libpt_s.a"
    PTLIB_AD_FILE="libpt_d_s.a"
    PTLIB_LINK_FILE="pt"
    H323PLUS_BUILDDIR="${H323PLUSDIR}/lib"
    H323PLUS_SO_FILE="libh323_${OSTYPE}_${MACHTYPE}_.so.${H323PLUS_MAJOR_VERSION}.${H323PLUS_MINOR_VERSION}.${H323PLUS_BUILD_NUMBER}"
    H323PLUS_A_FILE="libh323_${OSTYPE}_${MACHTYPE}__s.a"
    H323PLUS_AD_FILE="libh323_${OSTYPE}_${MACHTYPE}__d_s.a"
    H323PLUS_LINK_FILE="h323_${OSTYPE}_${MACHTYPE}_"
  else
    PTLIB_BUILDDIR="${PTLIBDIR}/lib"
    PTLIB_SO_FILE="libpt_${OSTYPE}_${MACHTYPE}_r.so.${PTLIB_MAJOR_VERSION}.${PTLIB_MINOR_VERSION}.${PTLIB_BUILD_NUMBER}"
    PTLIB_A_FILE="libpt_${OSTYPE}_${MACHTYPE}_r_s.a"
    PTLIB_AD_FILE="libpt_${OSTYPE}_${MACHTYPE}_d_s.a"
    PTLIB_LINK_FILE="pt_${OSTYPE}_${MACHTYPE}_r"
    H323PLUS_BUILDDIR="${H323PLUSDIR}/lib"
    H323PLUS_SO_FILE="libh323_${OSTYPE}_${MACHTYPE}_r.so.${H323PLUS_MAJOR_VERSION}.${H323PLUS_MINOR_VERSION}.${H323PLUS_BUILD_NUMBER}"
    H323PLUS_A_FILE="libh323_${OSTYPE}_${MACHTYPE}_r_s.a"
    H323PLUS_AD_FILE="libh323_${OSTYPE}_${MACHTYPE}_d_s.a"
    H323PLUS_LINK_FILE="h323_${OSTYPE}_${MACHTYPE}_r"
  fi

  if test ${PTLIB_MAJOR_VERSION} -eq 2 && test ${PTLIB_MINOR_VERSION} -gt 0; then
    H323PLUS_BUILDOPTS="PTLIBDIR=\"${PTLIBDIR}\""
  fi

  MCU_CPPFLAGS="${MCU_CPPFLAGS} -I../libs/ptlib/include -I../libs/h323plus/include"
  export PTLIBDIR=${PTLIBDIR}
  export PWLIBDIR=${PTLIBDIR}
  #export PTLIB_CONFIG=${PTLIB_CONFIG}
  AC_CONFIG_SUBDIRS(libs/ptlib)
  #AC_CONFIG_FILES(libs/ptlib/Makefile)
  AC_CONFIG_SUBDIRS(libs/h323plus)
  #AC_CONFIG_FILES(libs/h323plus/Makefile)
fi
# result
if test ${USE_SRC_PTLIB} = 0 && test ${USE_SYSTEM_PTLIB} = 0 ; then
  AC_MSG_ERROR(ptbuildopts.mak not found!)
fi

PWLIBDIR=${PTLIBDIR}
AM_CONDITIONAL(COMPILE_PTLIB, [test ${USE_SRC_PTLIB} = 1])

dnl ########################################################################
dnl check for sofia-sip library

AC_SUBST(USE_SRC_SOFIASIP, 0)
AC_SUBST(USE_SYSTEM_SOFIASIP, 1)
AC_SUBST(LIBSOFIA_DIR, ${top_srcdir}/libs/sofia-sip)
AC_SUBST(LIBSOFIA_SYSTEM_DIRS, "${INSTALLPREFIX}/${PROG} /usr /usr/local")
AC_SUBST(SOFIASIP_BUILDOPTS)

# check source 1
if test ${USE_SYSTEM_SOFIASIP} = 0 ; then
  AC_CHECK_FILE(${LIBSOFIA_DIR}/configure, USE_SRC_SOFIASIP=1, USE_SRC_SOFIASIP=0)
  AS_UNSET([$as_ac_File])
fi
# system library
if test ${USE_SRC_SOFIASIP} = 0 ; then
  for dir in ${LIBSOFIA_SYSTEM_DIRS} ; do
    AC_CHECK_FILE(${dir}/include/sofia-sip-1.12/sofia-sip/sip.h, USE_SYSTEM_SOFIASIP=1, USE_SYSTEM_SOFIASIP=0)
    if test ${USE_SYSTEM_SOFIASIP} = 1 ; then
      AC_CHECK_LIB(sofia-sip-ua, nta_agent_create, USE_SYSTEM_SOFIASIP=1, USE_SYSTEM_SOFIASIP=0)
      if test ${USE_SYSTEM_SOFIASIP} = 1 ; then
        MCU_CPPFLAGS="$MCU_CPPFLAGS -I${dir}/include/sofia-sip-1.12";
        MCU_LDFLAGS="$MCU_LDFLAGS -L${dir}/lib";
        MCU_RFLAGS="$MCU_RFLAGS -L${dir}/lib";
      fi
      break
    fi
  done
fi
# check source 2
if test ${USE_SYSTEM_SOFIASIP} = 0 && test ${USE_SRC_SOFIASIP} = 0 ; then
  AC_CHECK_FILE(${LIBSOFIA_DIR}/configure, USE_SRC_SOFIASIP=1, USE_SRC_SOFIASIP=0)
  AS_UNSET([$as_ac_File])
fi
# compile from source
if test ${USE_SRC_SOFIASIP} = 1 ; then
  if test ${GXX_MINOR} -ge 8; then
    SOFIASIP_BUILDOPTS="CFLAGS=-fno-aggressive-loop-optimizations"
  fi
  MCU_LDFLAGS="$MCU_LDFLAGS -L../libs/sofia-sip/libsofia-sip-ua/.libs"
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

  AC_CONFIG_SUBDIRS(libs/sofia-sip)
fi
# result
if test ${USE_SRC_SOFIASIP} = 0 && test ${USE_SYSTEM_SOFIASIP} = 0 ; then
  AC_MSG_ERROR(libsofia-sip-ua not found!)
fi

MCU_LDLIBS="$MCU_LDLIBS -lsofia-sip-ua"
AM_CONDITIONAL(COMPILE_SOFIASIP, [test ${USE_SRC_SOFIASIP} = 1])

dnl ########################################################################
dnl check for video library

AC_SUBST(USE_AVCODEC, 0)
AC_SUBST(USE_SWSCALE, 0)

AC_LANG_PUSH([C])

AC_CHECK_HEADERS([avcodec.h libavcodec/avcodec.h ffmpeg/avcodec.h libav/avcodec.h ffmpeg/libavcodec/avcodec.h libav/libavcodec/avcodec.h], USE_AVCODEC=1, [], [-])
if test ${USE_AVCODEC} = 0; then
  AC_MSG_ERROR(avcodec headers not found! Install FFmpeg or Libav.)
fi

AC_CHECK_LIB(avcodec, avcodec_version, USE_AVCODEC=1, USE_AVCODEC=0)
if test ${USE_AVCODEC} = 0; then
  AC_MSG_ERROR(libavcodec library not found! Install FFmpeg or Libav.)
fi

AC_CHECK_HEADERS([swscale.h libswscale/swscale.h ffmpeg/swscale.h libav/swscale.h ffmpeg/libswscale/swscale.h libav/libswscale/swscale.h], USE_SWSCALE=1, [], [-])
if test "${USE_SWSCALE}" = "0"; then
  AC_MSG_ERROR(swscale headers not found! Install FFmpeg or Libav.)
fi

AC_CHECK_LIB(swscale, sws_scale, USE_SWSCALE=1, USE_SWSCALE=0)
if test "${USE_SWSCALE}" = "0"; then
  AC_MSG_ERROR(swscale library not found! Install FFmpeg or Libav.)
fi

AC_LANG_POP([C])

LIBAV_CPPFLAGS="$LIBAV_CPPFLAGS `pkg-config --silence-errors --cflags-only-I libavcodec`";
LIBAV_LDFLAGS="$LIBAV_LDFLAGS `pkg-config --silence-errors --libs-only-L libavcodec`";

MCU_CPPFLAGS="$MCU_CPPFLAGS ${LIBAV_CPPFLAGS}"
MCU_LDFLAGS="$MCU_LDFLAGS ${LIBAV_LDFLAGS}"
MCU_RFLAGS="$MCU_RFLAGS ${LIBAV_LDFLAGS}"
MCU_LDLIBS="$MCU_LDLIBS -lavcodec -lavutil -lavformat -lswscale"

dnl ########################################################################
dnl arg

resample=yes
resampler_ffmpeg=yes
resampler_libav=yes
resampler_libsamplerate=yes
libfreetype=yes
libjpeg=no
libyuv=yes
openssl=yes
libsrtp=yes
libzrtp=no

dnl ########################################################################
dnl check for resampling library

AC_ARG_ENABLE(separator, [ ])
AC_ARG_ENABLE(resample, [AS_HELP_STRING([--disable-resample],[Disable resampling (default enabled)],[40],[100])], [resample="$enableval"])
AC_ARG_ENABLE(resampler-ffmpeg, [AS_HELP_STRING([--disable-resampler-ffmpeg],[Enable check only libswresample resampler(default auto)],[40],[100])], [resampler_ffmpeg="yes" resampler_libav="no" resampler_libsamplerate="no"])
AC_ARG_ENABLE(resampler-libav, [AS_HELP_STRING([--disable-resampler-libav],[Enable check only libavresample resampler(default auto)],[40],[100])], [resampler_ffmpeg="no" resampler_libav="yes" resampler_libsamplerate="no"])
AC_ARG_ENABLE(resampler-libsamplerate, [AS_HELP_STRING([--disable-resampler-libsamplerate],[Enable check only libsamplerate resampler(default auto)],[40],[100])], [resampler_ffmpeg="no" resampler_libav="no" resampler_libsamplerate="yes"])

AC_SUBST(USE_SWRESAMPLE, 0)
AC_SUBST(USE_AVRESAMPLE, 0)
AC_SUBST(USE_LIBSAMPLERATE, 0)
if test "${resample}" = "yes"; then
  if test "${resampler_ffmpeg}" = "yes"; then
    AC_LANG_PUSH([C])
    AC_CHECK_HEADERS([libswresample/swresample.h], USE_SWRESAMPLE=1, [], [-])
    AC_LANG_POP([C])
    if test "${USE_SWRESAMPLE}" = "1"; then
      AC_CHECK_LIB(swresample, swr_convert, USE_SWRESAMPLE=1, USE_SWRESAMPLE=0)
    fi
  fi
  if test "${resampler_libav}" = "yes" && test "${USE_SWRESAMPLE}" = "0"; then
    if test "${USE_SWRESAMPLE}" = "0"; then
      AC_CHECK_LIB(avresample, avresample_convert, USE_AVRESAMPLE=1, USE_AVRESAMPLE=0)
    fi
  fi
  if test "${resampler_libsamplerate}" = "yes"; then
    if test "${USE_SWRESAMPLE}" = "0" && test "${USE_AVRESAMPLE}" = "0"; then
      AC_CHECK_LIB(samplerate, src_process, USE_LIBSAMPLERATE=1, USE_LIBSAMPLERATE=0)
    fi
  fi

  if test "${USE_SWRESAMPLE}" = "1"; then
    MCU_LDLIBS="$MCU_LDLIBS -lswresample"
  elif test "${USE_AVRESAMPLE}" = "1"; then
    MCU_LDLIBS="$MCU_LDLIBS -lavresample"
  elif test "${USE_LIBSAMPLERATE}" = "1"; then
    MCU_CPPFLAGS="$MCU_CPPFLAGS `pkg-config --silence-errors --cflags-only-I samplerate`"
    MCU_LDFLAGS="$MCU_LDFLAGS `pkg-config --silence-errors --libs-only-L samplerate`"
    MCU_RFLAGS="$MCU_RFLAGS `pkg-config --silence-errors --libs-only-L samplerate`"
    MCU_LDLIBS="$MCU_LDLIBS -lsamplerate"
  else
    AC_MSG_WARN(libswresample/libavresample/libsamplerate not found!)
  fi
fi

dnl ########################################################################
dnl check for freetype library

AC_ARG_ENABLE(separator, [  ])
AC_ARG_ENABLE(libfreetype, [AS_HELP_STRING([--disable-libfreetype],[Disable subtitles (default enabled)],[40],[100])], [libfreetype="$enableval"])

AC_SUBST(USE_FREETYPE, 0)
if test "${libfreetype}" = "yes"; then
  AC_CHECK_HEADERS(freetype2/freetype/config/ftheader.h, USE_FREETYPE=1, USE_FREETYPE=0, [-])
  if test ${USE_FREETYPE} = 0 ; then
    AC_CHECK_HEADERS(freetype2/config/ftheader.h, USE_FREETYPE=1, USE_FREETYPE=0, [-])
  fi
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

AC_ARG_ENABLE(libjpeg, [AS_HELP_STRING([--enable-libjpeg],[Enable JPEG Frame Encoder for browser (default disabled)],[40],[100])], [libjpeg="$enableval"])

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

AC_ARG_ENABLE(libyuv, [AS_HELP_STRING([--disable-libyuv],[Disable libyuv to resize video (default enabled)],[40],[100])], [libyuv="$enableval"])

AC_SUBST(USE_SRC_LIBYUV, 0)
AC_SUBST(USE_SYSTEM_LIBYUV, 1)
AC_SUBST(USE_LIBYUV, 0)
AC_SUBST(LIBYUV_DIR, ${top_srcdir}/libs/libyuv)
AC_SUBST(LIBYUV_SYSTEM_DIRS, "${INSTALLPREFIX}/${PROG} /usr /usr/local")

if test "${libyuv}" = "yes"; then
  # check source 1
  if test ${USE_SYSTEM_LIBYUV} = 0 ; then
    AC_CHECK_FILE(${LIBYUV_DIR}/include/libyuv/scale.h, USE_SRC_LIBYUV=1, USE_SRC_LIBYUV=0)
    AS_UNSET([$as_ac_File])
  fi
  # system library
  if test ${USE_SRC_LIBYUV} = 0 ; then
    for dir in ${LIBYUV_SYSTEM_DIRS} ; do
      AC_CHECK_FILE(${dir}/include/libyuv/scale.h, USE_SYSTEM_LIBYUV=1, USE_SYSTEM_LIBYUV=0)
      if test ${USE_SYSTEM_LIBYUV} = 1 ; then
        AC_CHECK_LIB(yuv, I420Scale, USE_SYSTEM_LIBYUV=1, USE_SYSTEM_LIBYUV=0)
        if test ${USE_SYSTEM_LIBYUV} = 1 ; then
          MCU_CPPFLAGS="$MCU_CPPFLAGS -I${dir}/include";
          MCU_LDFLAGS="$MCU_LDFLAGS -L${dir}/lib";
          MCU_RFLAGS="$MCU_RFLAGS -L${dir}/lib";
          if test ${USE_LIBJPEG} = 0 ; then
            LDDJPEG=$(ldd ${dir}/lib/libyuv.so | grep libjpeg)
            if test "${LDDJPEG}" != ""; then
              MCU_LDLIBS="$MCU_LDLIBS -ljpeg"
            fi
          fi
        fi
        break
      fi
    done
  fi
  # check source 2
  if test ${USE_SYSTEM_LIBYUV} = 0 && test ${USE_SRC_LIBYUV} = 0 ; then
    AC_CHECK_FILE(${LIBYUV_DIR}/include/libyuv/scale.h, USE_SRC_LIBYUV=1, USE_SRC_LIBYUV=0)
    AS_UNSET([$as_ac_File])
  fi
  # compile from source
  if test ${USE_SRC_LIBYUV} = 1 ; then
    MCU_CPPFLAGS="$MCU_CPPFLAGS -I../libs/libyuv/include"
    MCU_LDFLAGS="$MCU_LDFLAGS -L../libs/libyuv";
  fi
  # result
  if test ${USE_SRC_LIBYUV} = 0 && test ${USE_SYSTEM_LIBYUV} = 0 ; then
    AC_MSG_ERROR(libyuv not found!)
  fi

  USE_LIBYUV=1
  MCU_LDLIBS="$MCU_LDLIBS -lyuv"
fi

AM_CONDITIONAL(COMPILE_LIBYUV, [test ${USE_SRC_LIBYUV} = 1])

dnl ########################################################################
dnl configure openssl

AC_ARG_ENABLE(separator, [   ])
AC_ARG_ENABLE(openssl, [AS_HELP_STRING([--disable-openssl],[Disable OpenSSL support (default enabled)],[40],[100])], [openssl="$enableval"])

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

AC_ARG_ENABLE(libsrtp, [AS_HELP_STRING([--disable-libsrtp],[Disable SRTP support for SIP (default enabled)],[40],[100])], [libsrtp="$enableval"])

AC_SUBST(USE_SRC_SRTP, 0)
AC_SUBST(USE_SYSTEM_SRTP, 1)
AC_SUBST(USE_LIBSRTP, 0)
AC_SUBST(LIBSRTP_DIR, ${top_srcdir}/libs/srtp)
AC_SUBST(LIBSRTP_SYSTEM_DIRS, "${INSTALLPREFIX}/${PROG} /usr /usr/local")

if test "${libsrtp}" = "yes"; then
  # check source 1
  if test ${USE_SYSTEM_SRTP} = 0 ; then
    AC_CHECK_FILE(${LIBSRTP_DIR}/configure, USE_SRC_SRTP=1, USE_SRC_SRTP=0)
    AS_UNSET([$as_ac_File])
  fi
  # system library
  if test ${USE_SRC_SRTP} = 0 ; then
    for dir in ${LIBSRTP_SYSTEM_DIRS} ; do
      AC_CHECK_FILE(${dir}/include/srtp/srtp.h, USE_SYSTEM_SRTP=1, USE_SYSTEM_SRTP=0)
      if test ${USE_SYSTEM_SRTP} = 1 ; then
        AC_CHECK_LIB(srtp, srtp_protect, USE_SYSTEM_SRTP=1, USE_SYSTEM_SRTP=0)
        if test ${USE_SYSTEM_SRTP} = 1 ; then
          MCU_CPPFLAGS="$MCU_CPPFLAGS -I${dir}/include/srtp/";
          MCU_LDFLAGS="$MCU_LDFLAGS -L${dir}/lib";
          MCU_RFLAGS="$MCU_RFLAGS -L${dir}/lib";
        fi
        break
      fi
    done
  fi
  # check source 2
  if test ${USE_SYSTEM_SRTP} = 0 && test ${USE_SRC_SRTP} = 0 ; then
    AC_CHECK_FILE(${LIBSRTP_DIR}/configure, USE_SRC_SRTP=1, USE_SRC_SRTP=0)
    AS_UNSET([$as_ac_File])
  fi
  # compile from source
  if test ${USE_SRC_SRTP} = 1 ; then
    MCU_CPPFLAGS="$MCU_CPPFLAGS -I../libs/srtp/include -I../libs/srtp/crypto/include"
    MCU_LDFLAGS="$MCU_LDFLAGS -L../libs/srtp";
    AC_CONFIG_SUBDIRS(libs/srtp)
  fi
  # result
  if test ${USE_SRC_SRTP} = 0 && test ${USE_SYSTEM_SRTP} = 0 ; then
    AC_MSG_ERROR(libsrtp not found!)
  fi

  USE_LIBSRTP=1
  MCU_LDLIBS="$MCU_LDLIBS -lsrtp"
fi

AM_CONDITIONAL(COMPILE_LIBSRTP, [test ${USE_SRC_SRTP} = 1])

dnl ########################################################################
dnl configure zrtp

AC_ARG_ENABLE(libzrtp, [AS_HELP_STRING([--enable-libzrtp],[Disable ZRTP support for SIP (default disabled)],[40],[100])], [libzrtp="$enableval"])

AC_SUBST(USE_SRC_ZRTP, 0)
AC_SUBST(USE_SYSTEM_ZRTP, 1)
AC_SUBST(USE_LIBZRTP, 0)
AC_SUBST(LIBZRTP_DIR, ${top_srcdir}/libs/libzrtp)
AC_SUBST(LIBZRTP_SYSTEM_DIRS, "${INSTALLPREFIX}/${PROG} /usr /usr/local")

if test "${libzrtp}" = "yes"; then
  # check source 1
  if test ${USE_SYSTEM_ZRTP} = 0 ; then
    AC_CHECK_FILE(${LIBZRTP_DIR}/configure.in, USE_SRC_ZRTP=1, USE_SRC_ZRTP=0)
    AS_UNSET([$as_ac_File])
  fi
  # system library
  if test ${USE_SRC_ZRTP} = 0; then
    for dir in ${LIBZRTP_SYSTEM_DIRS} ; do
      AC_CHECK_FILE(${dir}/include/libzrtp/zrtp.h, USE_SYSTEM_ZRTP=1, USE_SYSTEM_ZRTP=0)
      if test ${USE_SYSTEM_ZRTP} = 1; then
        AC_CHECK_FILE(${dir}/lib/libzrtp.so, USE_SYSTEM_ZRTP=1, USE_SYSTEM_ZRTP=0)
        if test ${USE_SYSTEM_ZRTP} = 1 ; then
          MCU_CPPFLAGS="$MCU_CPPFLAGS -I${dir}/include/libzrtp/";
          MCU_LDFLAGS="$MCU_LDFLAGS -L${dir}/lib";
          MCU_RFLAGS="$MCU_RFLAGS -L${dir}/lib";
        fi
        break
      fi
    done
  fi
  # check source 2
  if test ${USE_SYSTEM_ZRTP} = 0 && test ${USE_SRC_ZRTP} = 0 ; then
    AC_CHECK_FILE(${LIBZRTP_DIR}/configure.in, USE_SRC_ZRTP=1, USE_SRC_ZRTP=0)
    AS_UNSET([$as_ac_File])
  fi
  # download the source code
  if test ${USE_SRC_ZRTP} = 0 && test ${USE_SYSTEM_ZRTP} = 0; then
    AC_CHECK_FILE(${top_srcdir}/libs/libzrtp.tar.gz, USE_SRC_ZRTP=1, USE_SRC_ZRTP=0)
    AS_UNSET([$as_ac_File])
    if test ${USE_SRC_ZRTP} = 0; then
      (wget --directory-prefix=libs http://openmcu.ru/public/OpenMCU-ru/libs/libzrtp.tar.gz)
      AC_CHECK_FILE(${top_srcdir}/libs/libzrtp.tar.gz, USE_SRC_ZRTP=1, USE_SRC_ZRTP=0)
      AS_UNSET([$as_ac_File])
    fi
    if test ${USE_SRC_ZRTP} = 1; then
      (tar -xf libs/libzrtp.tar.gz --directory=libs)
      AC_CHECK_FILE(${top_srcdir}/libs/libzrtp/configure.in, USE_SRC_ZRTP=1, USE_SRC_ZRTP=0)
      AS_UNSET([$as_ac_File])
    fi
  fi
  # config
  if test ${USE_SRC_ZRTP} = 1; then
    AC_CHECK_FILE(${top_srcdir}/libs/libzrtp/configure, USE_SRC_ZRTP=1, USE_SRC_ZRTP=0)
    AS_UNSET([$as_ac_File])
    if test ${USE_SRC_ZRTP} = 0; then
      (cd ${top_srcdir}/libs/libzrtp/ && ./bootstrap.sh)
      AC_CHECK_FILE(${top_srcdir}/libs/libzrtp/configure, USE_SRC_ZRTP=1, USE_SRC_ZRTP=0)
      AS_UNSET([$as_ac_File])
    fi
  fi
  # compile from source
  if test ${USE_SRC_ZRTP} = 1; then
    MCU_CPPFLAGS="$MCU_CPPFLAGS -I../libs/libzrtp/include -I../libs/libzrtp/third_party/bnlib -I../libs/libzrtp/third_party/bgaes"
    MCU_LDFLAGS="$MCU_LDFLAGS -L../libs/libzrtp/.libs";
    AC_CONFIG_SUBDIRS(libs/libzrtp)
  fi
  # result
  if test ${USE_SRC_ZRTP} = 0 && test ${USE_SYSTEM_ZRTP} = 0 ; then
    AC_MSG_ERROR(libzrtp not found!)
  fi

  USE_LIBZRTP=1
  MCU_LDLIBS="$MCU_LDLIBS -lzrtp"
fi
AM_CONDITIONAL(COMPILE_LIBZRTP, [test ${USE_SRC_ZRTP} = 1])

dnl ########################################################################
dnl configuration plugins

MCU_PLUGIN_DIR=${MCU_LIB_DIR}/ptlib
PLUGIN_INSTALLPREFIX=${MCU_PLUGIN_DIR}
AUDIO_PLUGIN_DIR=codecs/audio
VIDEO_PLUGIN_DIR=codecs/video

AC_SUBST(MCU_PLUGIN_DIR)
AC_SUBST(PLUGIN_INSTALLPREFIX)
AC_SUBST(AUDIO_PLUGIN_DIR)
AC_SUBST(VIDEO_PLUGIN_DIR)

dnl ########################################################################
dnl configuration plugins

h261=yes
h263=yes
h264=yes
mpeg4=yes
vp8=yes
vp8_ffmpeg=yes
g722=yes
g7221=yes
g7222=yes
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
AC_ARG_ENABLE(mpeg4, [AS_HELP_STRING([--disable-mpeg4],[Disable MPEG4 support (default enabled)],[40],[100])], [mpeg4="$enableval"])
AC_ARG_ENABLE(vp8, [AS_HELP_STRING([--disable-vp8],[Disable VP8 support (default enabled)],[40],[100])], [vp8="$enableval"])
AC_ARG_ENABLE(vp8_ffmpeg, [AS_HELP_STRING([--disable-vp8-ffmpeg],[Disable VP8 ffmpeg decoding (default enabled)],[40],[100])], [vp8_ffmpeg="$enableval"])
AC_ARG_ENABLE(g722, [AS_HELP_STRING([--disable-g722],[Disable G.722 support (default enabled)],[40],[100])], [g722="$enableval"])
AC_ARG_ENABLE(g7221, [AS_HELP_STRING([--disable-g7221],[Disable G.722.1 support (default enabled)],[40],[100])], [g7221="$enableval"])
AC_ARG_ENABLE(g7222, [AS_HELP_STRING([--disable-g7222],[Disable G.722.2 support (default enabled)],[40],[100])], [g7222="$enableval"])
AC_ARG_ENABLE(g7231, [AS_HELP_STRING([--disable-g7231],[Disable G.723.1 support (default enabled)],[40],[100])], [g7231="$enableval"])
AC_ARG_ENABLE(g726, [AS_HELP_STRING([--disable-g726],[Disable G.726 support (default enabled)],[40],[100])], [g726="$enableval"])
AC_ARG_ENABLE(g729, [AS_HELP_STRING([--disable-g729],[Disable G.729 support (default enabled)],[40],[100])], [g729="$enableval"])
AC_ARG_ENABLE(ilbc, [AS_HELP_STRING([--disable-ilbc],[Disable iLBC support (default enabled)],[40],[100])], [ilbc="$enableval"])
AC_ARG_ENABLE(speex, [AS_HELP_STRING([--disable-speex],[Disable Speex support (default enabled)],[40],[100])], [speex="$enableval"])
AC_ARG_ENABLE(silk, [AS_HELP_STRING([--disable-silk],[Disable SILK support (default enabled)],[40],[100])], [silk="$enableval"])
AC_ARG_ENABLE(opus, [AS_HELP_STRING([--disable-opus],[Disable OPUS support (default enabled)],[40],[100])], [opus="$enableval"])

PLUGINS="h261 h263 h264 mpeg4 vp8 g722 g7221 g7222 g7231 g726 g729 ilbc silk opus"
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
    USE_H264=0
    AC_LANG_PUSH([C])
    AC_CHECK_HEADERS(x264.h, USE_H264=1, [], [-])
    AC_LANG_POP([C])
    if test ${USE_H264} = 0 ; then
      h264=no
      AC_MSG_ERROR(x264 headers not found! Install x264 or use option: --disable-h264)
    else
      AC_CONFIG_FILES(plugins/H.264_123/Makefile)
      PLUGIN_DIRS="$PLUGIN_DIRS ${PLUGIN_H264_DIR} "
    fi
fi

PLUGIN_MPEG4_DIR=${top_srcdir}/plugins/MPEG4
AC_SUBST(PLUGIN_MPEG4_DIR)
if test "${mpeg4}" = "yes"; then
    AC_CONFIG_FILES(plugins/MPEG4/Makefile)
    PLUGIN_DIRS="$PLUGIN_DIRS ${PLUGIN_MPEG4_DIR} "
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
	AC_MSG_ERROR(VP8 codec not found! Install libvpx or use option: --disable-vp8)
    fi
fi

PLUGIN_G722_DIR=${top_srcdir}/plugins/G722
AC_SUBST(PLUGIN_G722_DIR)
if test "${g722}" = "yes"; then
    AC_CONFIG_FILES(plugins/G722/Makefile)
    PLUGIN_DIRS="$PLUGIN_DIRS ${PLUGIN_G722_DIR} "
fi

AC_SUBST(USE_SRC_LIBG7221, 0)
AC_SUBST(LIBG7221_DIR, ${top_srcdir}/libs/libg722_1)
AC_SUBST(PLUGIN_G7221_DIR, ${top_srcdir}/plugins/G722.1)
AC_SUBST(LIBG7221_CPPFLAGS)
AC_SUBST(LIBG7221_LDFLAGS)
if test "${g7221}" = "yes"; then
    LIBG7221_CPPFLAGS="-I../../libs/libg722_1/src";
    LIBG7221_LDFLAGS="-Wl,--start-group ../../libs/libg722_1/src/.libs/libg722_1.a -Wl,--end-group";
    AC_CONFIG_SUBDIRS(libs/libg722_1)
    AC_CONFIG_FILES(plugins/G722.1/Makefile)
    PLUGIN_DIRS="$PLUGIN_DIRS ${PLUGIN_G7221_DIR} "
    USE_SRC_LIBG7221=1
fi
AM_CONDITIONAL(COMPILE_LIBG7221, [test ${USE_SRC_LIBG7221} = 1])

PLUGIN_G7222_DIR=${top_srcdir}/plugins/G722.2
AC_SUBST(PLUGIN_G7222_DIR)
if test "${g7222}" = "yes"; then
    AC_CONFIG_FILES(plugins/G722.2/Makefile)
    PLUGIN_DIRS="$PLUGIN_DIRS ${PLUGIN_G7222_DIR} "
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

AC_SUBST(USE_SRC_SPEEX, 0)
AC_SUBST(USE_SYSTEM_SPEEX, 1)
AC_SUBST(PLUGIN_SPEEX_DIR, ${top_srcdir}/plugins/Speex)
AC_SUBST(LIBSPEEX_DIR, ${top_srcdir}/libs/speex)
AC_SUBST(LIBSPEEX_SYSTEM_DIRS, "${INSTALLPREFIX}/${PROG} /usr /usr/local")

AC_SUBST(SPEEX_LDFLAGS)
AC_SUBST(SPEEX_CPPFLAGS)

if test "${speex}" = "yes"; then
  # check source 1
  if test ${USE_SYSTEM_SPEEX} = 0 ; then
    AC_CHECK_FILE(${LIBSPEEX_DIR}/configure, USE_SRC_SPEEX=1, USE_SRC_SPEEX=0)
    AS_UNSET([$as_ac_File])
  fi
  # system library
  if test ${USE_SRC_SPEEX} = 0; then
    for dir in ${LIBSPEEX_SYSTEM_DIRS} ; do
      AC_CHECK_FILE(${dir}/include/speex/speex.h, USE_SYSTEM_SPEEX=1, USE_SYSTEM_SPEEX=0)
      if test ${USE_SYSTEM_SPEEX} = 1 ; then
        AC_CHECK_LIB(speex, speex_encoder_init, USE_SYSTEM_SPEEX=1, USE_SYSTEM_SPEEX=0)
        if test ${USE_SYSTEM_SPEEX} = 1 ; then
          SPEEX_CPPFLAGS="$SPEEX_CPPFLAGS -I${dir}/include";
          SPEEX_LDFLAGS="$SPEEX_LDFLAGS -L${dir}/lib";
          MCU_RFLAGS="$MCU_RFLAGS -L${dir}/lib";
        fi
        break
      fi
    done
  fi
  # check source 2
  if test ${USE_SYSTEM_SPEEX} = 0 && test ${USE_SRC_SPEEX} = 0 ; then
    AC_CHECK_FILE(${LIBSPEEX_DIR}/configure, USE_SRC_SPEEX=1, USE_SRC_SPEEX=0)
    AS_UNSET([$as_ac_File])
  fi
  # compile from source
  if test ${USE_SRC_SPEEX} = 1 ; then
    SPEEX_CPPFLAGS="$SPEEX_CPPFLAGS -I../../libs/speex/include"
    SPEEX_LDFLAGS="$SPEEX_LDFLAGS -L../../libs/speex/libspeex/.libs"
    AC_CONFIG_SUBDIRS(libs/speex)
  fi
  # result
  if test ${USE_SRC_SPEEX} = 0 && test ${USE_SYSTEM_SPEEX} = 0 ; then
    AC_MSG_ERROR(libspeex not found!)
  fi

  SPEEX_LDFLAGS="$SPEEX_LDFLAGS -lspeex"
  AC_CONFIG_FILES(plugins/Speex/Makefile)
  PLUGIN_DIRS="$PLUGIN_DIRS ${PLUGIN_SPEEX_DIR} "
fi

AM_CONDITIONAL(COMPILE_LIBSPEEX, [test ${USE_SRC_SPEEX} = 1])

dnl ########################################################################
dnl plugin OPUS

AC_SUBST(USE_SRC_OPUS, 0)
AC_SUBST(USE_SYSTEM_OPUS, 1)
AC_SUBST(PLUGIN_OPUS_DIR, ${top_srcdir}/plugins/OPUS)
AC_SUBST(LIBOPUS_DIR, ${top_srcdir}/libs/opus)
AC_SUBST(LIBOPUS_SYSTEM_DIRS, "${INSTALLPREFIX}/${PROG} /usr /usr/local")

AC_SUBST(OPUS_LDFLAGS)
AC_SUBST(OPUS_CPPFLAGS)

if test "${opus}" = "yes"; then
  # check source 1
  if test ${USE_SYSTEM_OPUS} = 0 ; then
    AC_CHECK_FILE(${LIBOPUS_DIR}/configure, USE_SRC_OPUS=1, USE_SRC_OPUS=0)
    AS_UNSET([$as_ac_File])
  fi
  # system library
  if test ${USE_SRC_OPUS} = 0 ; then
    for dir in ${LIBOPUS_SYSTEM_DIRS} ; do
      AC_CHECK_FILE(${dir}/include/opus/opus.h, USE_SYSTEM_OPUS=1, USE_SYSTEM_OPUS=0)
      if test ${USE_SYSTEM_OPUS} = 1 ; then
        AC_CHECK_LIB(opus, opus_encoder_init, USE_SYSTEM_OPUS=1, USE_SYSTEM_OPUS=0)
        if test ${USE_SYSTEM_OPUS} = 1 ; then
          OPUS_CPPFLAGS="$OPUS_CPPFLAGS -I${dir}/include/opus";
          OPUS_LDFLAGS="$OPUS_LDFLAGS -L${dir}/lib";
          MCU_RFLAGS="$MCU_RFLAGS -L${dir}/lib";
        fi
        break
      fi
    done
  fi
  # check source 2
  if test ${USE_SYSTEM_OPUS} = 0 && test ${USE_SRC_OPUS} = 0 ; then
    AC_CHECK_FILE(${LIBOPUS_DIR}/configure, USE_SRC_OPUS=1, USE_SRC_OPUS=0)
    AS_UNSET([$as_ac_File])
  fi
  # compile from source
  if test ${USE_SRC_OPUS} = 1 ; then
    OPUS_CPPFLAGS="$OPUS_CPPFLAGS -I../../libs/opus/include"
    OPUS_LDFLAGS="$OPUS_LDFLAGS -L../../libs/opus/.libs"
    AC_CONFIG_SUBDIRS(libs/opus)
  fi
  # result
  if test ${USE_SRC_OPUS} = 0 && test ${USE_SYSTEM_OPUS} = 0 ; then
    AC_MSG_ERROR(libopus not found!)
  fi

  OPUS_LDFLAGS="$OPUS_LDFLAGS -lopus"
  AC_CONFIG_FILES(plugins/OPUS/Makefile)
  PLUGIN_DIRS="$PLUGIN_DIRS ${PLUGIN_OPUS_DIR} "
fi

AM_CONDITIONAL(COMPILE_LIBOPUS, [test ${USE_SRC_OPUS} = 1])

dnl ########################################################################
dnl configure static linking

static=no
debug=no
AC_ARG_ENABLE(separator, [      ])
AC_ARG_ENABLE(static, [AS_HELP_STRING([--enable-static],[Build static linking with ptlib,h323plus (default disabled)],[40],[100])], [static="$enableval"])
AC_ARG_ENABLE(debug, [AS_HELP_STRING([--enable-debug],[Enable debugging information (default disabled)],[40],[100])], [debug="$enableval"])

CFLAGS="-g -O2 $CFLAGS"
MCU_LDLIBS_STATIC="$MCU_LDLIBS"
MCU_LDLIBS_DEBUG="$MCU_LDLIBS"
MCU_SFLAGS_STATIC="${MCU_SFLAGS} -Wl,--start-group ${PTLIB_BUILDDIR}/${PTLIB_A_FILE} ${H323PLUS_BUILDDIR}/${H323PLUS_A_FILE} -Wl,--end-group"
MCU_SFLAGS_DEBUG="${MCU_SFLAGS} -Wl,--start-group ${PTLIB_BUILDDIR}/${PTLIB_AD_FILE} ${H323PLUS_BUILDDIR}/${H323PLUS_AD_FILE} -Wl,--end-group"

MCU_LDFLAGS="${MCU_LDFLAGS} -L${PTLIB_BUILDDIR} -L${H323PLUS_BUILDDIR}"
MCU_LDLIBS="${MCU_LDLIBS} -l${H323PLUS_LINK_FILE} -l${PTLIB_LINK_FILE}"

AC_SUBST(ENABLE_STATIC, 0)
AC_SUBST(MCU_BUILDOPTS)

if test "${debug}" = "yes"; then
  PTLIB_BUILDOPTS="debug ${PTLIB_BUILDOPTS}"
  H323PLUS_BUILDOPTS="debug ${H323PLUS_BUILDOPTS}"
  MCU_BUILDOPTS="debug ${MCU_BUILDOPTS}"
  ENABLE_STATIC=1
elif test "${static}" = "yes"; then
  PTLIB_BUILDOPTS="opt ${PTLIB_BUILDOPTS}"
  H323PLUS_BUILDOPTS="opt ${H323PLUS_BUILDOPTS}"
  MCU_BUILDOPTS="static ${MCU_BUILDOPTS}"
  ENABLE_STATIC=1
fi

AM_CONDITIONAL(COMPILE_PTLIB_NO_STATIC, [test ${ENABLE_STATIC} = 0])

dnl ########################################################################
dnl configure rpath

rpath=yes
AC_ARG_ENABLE(rpath, [AS_HELP_STRING([--disable-rpath],[Do not hardcode runtime library paths (default auto)],[40],[100])], [rpath="$enableval"])

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
    LD_PATH=`ldconfig -v -N | grep "/" | awk -F: '{print $1}'`
  fi

  # removing duplicate paths
  RPATH_LDFLAGS=$(echo $(printf '%s\n' ${RPATH_LDFLAGS} | sort -u))
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
AC_SUBST(MCU_LDLIBS_STATIC)
AC_SUBST(MCU_LDLIBS_DEBUG)
AC_SUBST(MCU_CPPFLAGS)

AC_SUBST(RPATH)
AC_SUBST(MCU_SFLAGS)
AC_SUBST(MCU_SFLAGS_STATIC)
AC_SUBST(MCU_SFLAGS_DEBUG)

AC_SUBST(STDCCFLAGS)
AC_SUBST(STDCXXFLAGS)
AC_SUBST(OPTSTDCCFLAGS)
AC_SUBST(CFLAGS)
AC_SUBST(CPPFLAGS)
AC_SUBST(LDFLAGS)
AC_SUBST(LDLIBS)
AC_SUBST(DEBUG_FLAG)

CPPFLAGS=${OLD_CPPFLAGS}
LDFLAGS=${OLD_LDFLAGS}

AC_OUTPUT(Makefile)

dnl ########################################################################
dnl replace default targets

sed -i -r 's/all: all-recursive/all: all/g' Makefile


dnl ########################################################################
dnl replace default install path

if test "${USE_SYSTEM_PTLIB}" = "0"; then
  if test ${PTLIB_MAJOR_VERSION} -eq 2 && test ${PTLIB_MINOR_VERSION} -gt 0; then
    sed -i -r "s#${PTLIBDIR}#${PTLIBDIR}\\nPTLIBDIR\\t= ${PTLIBDIR}#g" ${H323PLUSDIR}/openh323u.mak
  fi
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
if test "x${mpeg4}" = "xyes" ; then
echo "              MPEG4 Video Plugin  :  enabled"
else
echo "              MPEG4 Video Plugin  :  disabled"
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
if test "x${g7221}" = "xyes" ; then
echo "            G.722.1 Audio Plugin  :  enabled"
else
echo "            G.722.1 Audio Plugin  :  disabled"
fi
if test "x${g7222}" = "xyes" ; then
echo "            G.722.2 Audio Plugin  :  enabled"
else
echo "            G.722.2 Audio Plugin  :  disabled"
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
