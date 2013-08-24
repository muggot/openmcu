dnl #######################################################################
dnl INIT

AC_INIT([OpenMCU-ru],[3.1.0],[openmcu-ru])
AC_CANONICAL_TARGET()
AM_INIT_AUTOMAKE()
#AM_EXTRA_RECURSIVE_TARGETS(opt)

AC_PROG_CXX
AC_LANG(C++)

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


dnl #######################################################################

top_srcdir=`pwd`


dnl ########################################################################
dnl install PREFIX

if test "x$prefix" = "xNONE"; then
   INSTALLPREFIX="/usr/local"
else
   INSTALLPREFIX="${prefix}"
fi
AC_SUBST(INSTALLPREFIX)
export PKG_CONFIG_PATH=${PKG_CONFIG_PATH}:/usr/lib/pkgconfig:$INSTALLPREFIX/lib/pkgconfig


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
dnl arg

resample=yes
resampler_ffmpeg=yes
resampler_libav=yes
resampler_libsamplerate=yes
rendering_names=yes
live_video=yes
optimized_frame_resize=yes

AC_ARG_ENABLE(separator, [ ])
AC_ARG_ENABLE(resample, [AS_HELP_STRING([--enable-resample],[Enable/Disable resampling (default enabled)],[40],[100])], [resample="$enableval"])
AC_ARG_ENABLE(resampler-ffmpeg, [AS_HELP_STRING([--enable-resampler-ffmpeg],[Enable check only libswresample resampler(default auto)],[40],[100])], [resampler_ffmpeg="yes" resampler_libav="no" resampler_libsamplerate="no"])
AC_ARG_ENABLE(resampler-libav, [AS_HELP_STRING([--enable-resampler-libav],[Enable check only libavresample resampler(default auto)],[40],[100])], [resampler_ffmpeg="no" resampler_libav="yes" resampler_libsamplerate="no"])
AC_ARG_ENABLE(resampler-libsamplerate, [AS_HELP_STRING([--enable-resampler-libsamplerate],[Enable check only libsamplerate resampler(default auto)],[40],[100])], [resampler_ffmpeg="no" resampler_libav="no" resampler_libsamplerate="yes"])
AC_ARG_ENABLE(separator, [  ])
AC_ARG_ENABLE(rendering-names, [AS_HELP_STRING([--enable-rendering-names],[Enable/Disable, requires libfreetype (default enabled)],[40],[100])], [rendering_names="$enableval"])
AC_ARG_ENABLE(live-video, [AS_HELP_STRING([--enable-live-video],[Enable/Disable, requires libjpeg (default enabled)],[40],[100])], [live_video="$enableval"])
AC_ARG_ENABLE(optimized-frame-resize, [AS_HELP_STRING([--enable-optimized-frame-resize],[Enable/Disable, requires libyuv (default enabled)],[40],[100])], [optimized_frame_resize="$enableval"])
AC_ARG_ENABLE(separator, [   ])

dnl ########################################################################
dnl check for freetype library

AC_SUBST(USE_FREETYPE, 0)
if test "${rendering_names}" = "yes"; then
  AC_CHECK_LIB(freetype, FT_Init_FreeType, USE_FREETYPE=1, USE_FREETYPE=0)
  if test ${USE_FREETYPE} = 1 ; then
    STDCCFLAGS="$STDCCFLAGS `pkg-config --cflags-only-I freetype2`";
    LDLIBS="$LDLIBS -lfreetype"
  else
    AC_MSG_ERROR(Must have freetype library!)
  fi
fi

dnl ########################################################################
dnl check for video library

AC_CHECK_LIB(avcodec, avcodec_version, USE_AVCODEC=1, USE_AVCODEC=0)
if test ${USE_AVCODEC} = 1; then
  STDCCFLAGS="$STDCCFLAGS `pkg-config --cflags-only-I libavcodec`";
else
  AC_MSG_ERROR(libavcodec not found!)
fi

AC_CHECK_LIB(avutil, avutil_version, USE_AVUTIL=1, USE_AVUTIL=0)
if test ${USE_AVUTIL} = 0; then
  AC_MSG_ERROR(libavutil not found!)
fi


dnl ########################################################################
dnl check for resampling library

AC_SUBST(USE_SWRESAMPLE, 0)
AC_SUBST(USE_AVRESAMPLE, 0)
AC_SUBST(USE_LIBSAMPLERATE, 0)

if test "${resample}" = "yes"; then
  if test "${resampler_ffmpeg}" = "yes"; then
    AC_CHECK_LIB(swresample, swr_convert, SWRESAMPLE=1, SWRESAMPLE=0)
  fi
  if test "${resampler_libav}" = "yes"; then
    AC_CHECK_LIB(avresample, avresample_convert, AVRESAMPLE=1, AVRESAMPLE=0)
  fi
  if test "${resampler_libsamplerate}" = "yes"; then
    AC_CHECK_LIB(samplerate, src_process, LIBSAMPLERATE=1, LIBSAMPLERATE=0)
  fi

  if test "${SWRESAMPLE}" = "1"; then
    USE_SWRESAMPLE=1
    LDLIBS="$LDLIBS -lswresample"
  elif test "${AVRESAMPLE}" = "1"; then
    USE_AVRESAMPLE=1
    LDLIBS="$LDLIBS -lavresample"
  elif test "${LIBSAMPLERATE}" = "1"; then
    USE_LIBSAMPLERATE=1
    LDLIBS="$LDLIBS -lsamplerate"
  else
    AC_MSG_ERROR(Need one of the libraries: libswresample libavresample libsamplerate.)
  fi
fi

dnl ########################################################################
dnl check for jpeg library

AC_SUBST(USE_LIBJPEG, 0)
if test "${live_video}" = "yes"; then
  AC_CHECK_LIB(jpeg, jpeg_destroy_decompress, USE_LIBJPEG=1, USE_LIBJPEG=0)
  if test ${USE_LIBJPEG} = 1 ; then
    LDLIBS="$LDLIBS -ljpeg"
  else
    AC_MSG_ERROR(libjpeg not found!)
  fi
fi

dnl ########################################################################
dnl check for yuv library

AC_SUBST(USE_LIBYUV, 0)
if test "${optimized_frame_resize}" = "yes"; then
  AC_CHECK_LIB(yuv, I420Scale, USE_LIBYUV=1, USE_LIBYUV=0)
  if test ${USE_LIBYUV} = 1 ; then
    LDLIBS="$LDLIBS -lyuv"
  else
    AC_MSG_ERROR(libyuv not found!)
  fi
fi

dnl ########################################################################
dnl check for sofia-sip library

AC_SUBST(USE_LIBSOFIA, 0)
AC_CHECK_LIB(sofia-sip-ua, nta_agent_create, USE_LIBSOFIA=1, USE_LIBSOFIA=0)
if test ${USE_LIBSOFIA} = 1 ; then
  STDCCFLAGS="$STDCCFLAGS `pkg-config --cflags-only-I sofia-sip-ua`";
  LDLIBS="$LDLIBS -lsofia-sip-ua"
else
  AC_MSG_ERROR(libsofia-sip-ua not found!)
fi


dnl ########################################################################
dnl configuration openmcu-ru

MCUDIR=openmcu-ru

AC_ARG_WITH(separator,[ ])
AC_ARG_WITH(ptlib_prefix, [AS_HELP_STRING([--with-ptlib-prefix],[PTLib installation prefix (default =prefix)],[40],[100])], [ptlib_prefix=$withval], [ptlib_prefix=""])
AC_ARG_WITH(h323plus_prefix, [AS_HELP_STRING([--with-h323plus-prefix],[H323Plus installation prefix (default =prefix)],[40],[100])], [h323plus_prefix=$withval], [h323plus_prefix=""])

if test "x$ptlib_prefix" = "x"; then
   PTLIB_INSTALLPREFIX=${INSTALLPREFIX}
else
   PTLIB_INSTALLPREFIX=${ptlib_prefix}
fi

if test "x$h323plus_prefix" = "x"; then
   H323PLUS_INSTALLPREFIX=${INSTALLPREFIX}
else
   H323PLUS_INSTALLPREFIX=${h323plus_prefix}
fi

if test ${INSTALLPREFIX} = /usr || test ${INSTALLPREFIX} = /usr/local ; then
    #PTLIB_INSTALLPREFIX=${INSTALLPREFIX}/share/${MCUDIR}
    #H323PLUS_INSTALLPREFIX=${INSTALLPREFIX}/share/${MCUDIR}
    MCU_INSTALLPREFIX=${INSTALLPREFIX}/share/${MCUDIR}
    MCU_LINK_DIR=${INSTALLPREFIX}/bin
else
    MCU_INSTALLPREFIX=${INSTALLPREFIX}
    MCU_LINK_DIR=${INSTALLPREFIX}
fi

AC_SUBST(MCU_INSTALLPREFIX)
AC_SUBST(PTLIB_INSTALLPREFIX)
AC_SUBST(H323PLUS_INSTALLPREFIX)

MCU_BIN_DIR=${MCU_INSTALLPREFIX}/bin
MCU_LIB_DIR=${MCU_INSTALLPREFIX}/lib
MCU_CONFIG_DIR=${MCU_INSTALLPREFIX}/config
MCU_PIPE_DIR=${MCU_INSTALLPREFIX}/pipe
MCU_RECORDS_DIR=${MCU_INSTALLPREFIX}/records
MCU_FONT_DIR=${MCU_INSTALLPREFIX}/font
MCU_RESOURCE_DIR=${MCU_INSTALLPREFIX}/resource
MCU_LOG_DIR=${MCU_INSTALLPREFIX}/log

AC_SUBST(MCUDIR)
AC_SUBST(MCU_LINK_DIR)
AC_SUBST(MCU_BIN_DIR)
AC_SUBST(MCU_LIB_DIR)
AC_SUBST(MCU_CONFIG_DIR)
AC_SUBST(MCU_PIPE_DIR)
AC_SUBST(MCU_RESOURCE_DIR)
AC_SUBST(MCU_RECORDS_DIR)
AC_SUBST(MCU_FONT_DIR)
AC_SUBST(MCU_LOG_DIR)

AC_CONFIG_FILES(openmcu-ru/Makefile)
AC_CONFIG_FILES(openmcu-ru/config.h)
AC_CONFIG_FILES(openmcu-ru/conf/openmcu-wrapper)

dnl ########################################################################
dnl configuration ptlib

PWLIBDIR=ptlib
PTLIBDIR=ptlib
AC_SUBST(PWLIBDIR)
AC_SUBST(PTLIBDIR)
AC_CONFIG_SUBDIRS(ptlib)
AC_CONFIG_FILES(ptlib/Makefile)


dnl ########################################################################
dnl configuration h323plus

H323PLUSDIR=h323plus
AC_SUBST(H323PLUSDIR)
AC_CONFIG_SUBDIRS(h323plus)
AC_CONFIG_FILES(h323plus/Makefile)


dnl ########################################################################
dnl configuration plugins

h263=yes
h264=yes
vp8=yes
g722=yes
g7231=yes
g726=yes
ilbc=yes
speex=yes
silk=yes
opus=yes

AC_ARG_ENABLE(h263, [AS_HELP_STRING([--enable-h263],[Enable/Disable H.263 support (default enabled)],[40],[100])], [h263="$enableval"])
AC_ARG_ENABLE(h264, [AS_HELP_STRING([--enable-h264],[Enable/Disable H.264 support (default enabled)],[40],[100])], [h264="$enableval"])
AC_ARG_ENABLE(vp8, [AS_HELP_STRING([--enable-vp8],[Enable/Disable VP8 support (default enabled)],[40],[100])], [vp8="$enableval"])
AC_ARG_ENABLE(g722, [AS_HELP_STRING([--enable-g722],[Enable/Disable G.722 support (default enabled)],[40],[100])], [g722="$enableval"])
AC_ARG_ENABLE(g7231, [AS_HELP_STRING([--enable-g7231],[Enable/Disable G.723.1 support (default enabled)],[40],[100])], [g7231="$enableval"])
AC_ARG_ENABLE(g726, [AS_HELP_STRING([--enable-g726],[Enable/Disable G.726 support (default enabled)],[40],[100])], [g726="$enableval"])
AC_ARG_ENABLE(ilbc, [AS_HELP_STRING([--enable-ilbc],[Enable/Disable iLBC support (default enabled)],[40],[100])], [ilbc="$enableval"])
AC_ARG_ENABLE(speex, [AS_HELP_STRING([--enable-speex],[Enable/Disable Speex support (default enabled)],[40],[100])], [speex="$enableval"])
AC_ARG_ENABLE(silk, [AS_HELP_STRING([--enable-silk],[Enable/Disable SILK support (default enabled)],[40],[100])], [silk="$enableval"])
AC_ARG_ENABLE(opus, [AS_HELP_STRING([--enable-opus],[Enable/Disable OPUS support (default enabled)],[40],[100])], [opus="$enableval"])

PLUGINS="h263 h264 vp8 g722 g7231 g726 ilbc silk opus"
AC_SUBST(PLUGINS)

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
    AC_CHECK_LIB(vpx, vpx_codec_encode, USE_VP8=1, USE_VP8=0)
    if test ${USE_VP8} = 1 ; then
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

PLUGIN_ILBC_DIR=${top_srcdir}/plugins/iLBC
AC_SUBST(PLUGIN_ILBC_DIR)
if test "${ilbc}" = "yes"; then
    AC_CONFIG_FILES(plugins/iLBC/Makefile)
    PLUGIN_DIRS="$PLUGIN_DIRS ${PLUGIN_ILBC_DIR} "
fi

PLUGIN_SPEEX_DIR=${top_srcdir}/plugins/Speex
AC_SUBST(PLUGIN_SPEEX_DIR)
if test "${speex}" = "yes"; then
    AC_CONFIG_FILES(plugins/Speex/Makefile)
    PLUGIN_DIRS="$PLUGIN_DIRS ${PLUGIN_SPEEX_DIR} "
fi

PLUGIN_SILK_DIR=${top_srcdir}/plugins/SILK_B40
AC_SUBST(PLUGIN_SILK_DIR)
if test "${silk}" = "yes"; then
    AC_CONFIG_FILES(plugins/SILK_B40/Makefile)
    PLUGIN_DIRS="$PLUGIN_DIRS ${PLUGIN_SILK_DIR} "
fi

PLUGIN_OPUS_DIR=${top_srcdir}/plugins/OPUS
AC_SUBST(PLUGIN_OPUS_DIR)
if test "${opus}" = "yes"; then
    AC_CONFIG_FILES(plugins/OPUS/Makefile)
    PLUGIN_DIRS="$PLUGIN_DIRS ${PLUGIN_OPUS_DIR} "
fi

AC_SUBST(PLUGIN_DIRS)

if test "x$LIB64" != "x" ; then
  AC_SUBST(PLUGINLIBDIR, ${PTLIB_INSTALLPREFIX}/lib64)
else
  AC_SUBST(PLUGINLIBDIR, ${PTLIB_INSTALLPREFIX}/lib)
fi


dnl ########################################################################
dnl final declarations

export PWLIBDIR=`pwd`/ptlib

CPPFLAGS="$CPPFLAGS -I${PTLIB_INSTALLPREFIX}/include"
LDFLAGS="$LDFLAGS -I${PTLIB_INSTALLPREFIX}/lib"

AC_SUBST(STDCCFLAGS)
AC_SUBST(STDCXXFLAGS)
AC_SUBST(OPTSTDCCFLAGS)
AC_SUBST(CPPFLAGS)
AC_SUBST(LDFLAGS)
AC_SUBST(LDLIBS)
AC_SUBST(DEBUG_FLAG)
AC_PROG_INSTALL

AC_OUTPUT(Makefile)


dnl ########################################################################
dnl replace default targets

sed -i 's/all: all-recursive/all: all/g' Makefile


dnl ########################################################################
dnl replace default install path

if test ${PTLIB_INSTALLPREFIX} != ${INSTALLPREFIX} ; then
    sed -i "s#${INSTALLPREFIX}#${PTLIB_INSTALLPREFIX}#g" ${PTLIBDIR}/Makefile
    sed -i "s#${INSTALLPREFIX}#${PTLIB_INSTALLPREFIX}#g" ${PTLIBDIR}/make/ptlib-config
    sed -i "s#${INSTALLPREFIX}#${PTLIB_INSTALLPREFIX}#g" ${PTLIBDIR}/include/ptbuildopts.h
    for dir in ${PLUGIN_DIRS}; \
	do sed -i "s#${INSTALLPREFIX}#${PTLIB_INSTALLPREFIX}#g" ${dir}/Makefile;
    done
fi

if test ${H323PLUS_INSTALLPREFIX} != ${INSTALLPREFIX} ; then
    sed -i "s#${INSTALLPREFIX}#${H323PLUS_INSTALLPREFIX}#g" ${H323PLUSDIR}/Makefile
fi

