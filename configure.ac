dnl #######################################################################
dnl INIT

AC_INIT([OpenMCU-ru],[3.1.0],[openmcu-ru])
AC_CANONICAL_TARGET()
AM_INIT_AUTOMAKE()

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

RPATH_LDFLAGS="${LDFLAGS}"

LIBAV_CPPFLAGS="${LIBAV_CPPFLAGS} ${CPPFLAGS}";
LIBAV_LDFLAGS="${LIBAV_LDFLAGS} ${LDFLAGS}";
LIBVPX_CPPFLAGS="${LIBVPX_CPPFLAGS} ${CPPFLAGS}";
LIBVPX_LDFLAGS="${LIBVPX_LDFLAGS} ${LDFLAGS}";

CPPFLAGS="$CPPFLAGS -I$INSTALLPREFIX/include -I/usr/include -I/usr/local/include "
LDFLAGS="$LDFLAGS -L$INSTALLPREFIX/lib -L/usr/lib -L/usr/local/lib "

export PKG_CONFIG_PATH=$INSTALLPREFIX/lib/pkgconfig:${PKG_CONFIG_PATH}:/usr/lib/pkgconfig:/usr/local/lib/pkgconfig
export LD_LIBRARY_PATH=$INSTALLPREFIX/lib:${LD_LIBRARY_PATH}:/usr/lib:/usr/local/lib

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
    FREETYPE_CPPFLAGS="`pkg-config --silence-errors --cflags-only-I freetype2`";
    FREETYPE_LDFLAGS="`pkg-config --silence-errors --libs-only-L freetype2`";
    LDLIBS="$LDLIBS -lfreetype"
  else
    AC_MSG_ERROR(Must have freetype library!)
  fi
fi

dnl ########################################################################
dnl check for video library

AC_CHECK_LIB(avcodec, avcodec_version, USE_AVCODEC=1, USE_AVCODEC=0)
if test ${USE_AVCODEC} = 1; then
  LIBAV_CPPFLAGS="$LIBAV_CPPFLAGS `pkg-config --silence-errors --cflags-only-I libavcodec`";
  LIBAV_LDFLAGS="$LIBAV_LDFLAGS `pkg-config --silence-errors --libs-only-L libavcodec`";
else
  AC_MSG_ERROR(libavcodec not found!)
fi

AC_CHECK_LIB(avutil, avutil_version, USE_AVUTIL=1, USE_AVUTIL=0)
if test ${USE_AVUTIL} = 0; then
  AC_MSG_ERROR(libavutil not found!)
fi


dnl ########################################################################
dnl check for ffmpeg path

AC_SUBST(FFMPEG_PATH, "/usr/bin/ffmpeg")
AC_CHECK_FILE("/usr/bin/avconv", [USE_AVCONV_PATH=1], [USE_AVCONV_PATH=0])

if test "${USE_AVCONV_PATH}" = "1"; then
  FFMPEG_PATH="/usr/bin/avconv"
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
    RESAMPLE_CPPFLAGS="${LIBAV_CPPFLAGS}";
    RESAMPLE_LDFLAGS="${LIBAV_LDFLAGS}";
    LDLIBS="$LDLIBS -lswresample"
  elif test "${AVRESAMPLE}" = "1"; then
    USE_AVRESAMPLE=1
    RESAMPLE_CPPFLAGS="${LIBAV_CPPFLAGS}";
    RESAMPLE_LDFLAGS="${LIBAV_LDFLAGS}";
    LDLIBS="$LDLIBS -lavresample"
  elif test "${LIBSAMPLERATE}" = "1"; then
    USE_LIBSAMPLERATE=1
    RESAMPLE_CPPFLAGS="`pkg-config --silence-errors --cflags-only-I samplerate`";
    RESAMPLE_LDFLAGS="`pkg-config --silence-errors --libs-only-L samplerate`";
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
    JPEG_CPPFLAGS="`pkg-config --silence-errors --cflags-only-I libjpeg`";
    JPEG_LDFLAGS="`pkg-config --silence-errors --libs-only-L libjpeg`";
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
    YUV_CPPFLAGS="`pkg-config --silence-errors --cflags-only-I libyuv`";
    YUV_LDFLAGS="`pkg-config --silence-errors --libs-only-L libyuv`";
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
  SOFIA_CPPFLAGS="`pkg-config --silence-errors --cflags-only-I sofia-sip-ua`";
  SOFIA_LDFLAGS="`pkg-config --silence-errors --libs-only-L sofia-sip-ua`";
  LDLIBS="$LDLIBS -lsofia-sip-ua"
else
  AC_MSG_ERROR(libsofia-sip-ua not found!)
fi

dnl ########################################################################
dnl configuration ptlib/h323plus

AC_ARG_WITH(separator,[ ])
AC_ARG_WITH(ptlib_prefix, [AS_HELP_STRING([--with-ptlib-prefix],[PTLib installation prefix (default =prefix)],[40],[100])], [ptlib_prefix=$withval], [ptlib_prefix=""])
AC_ARG_WITH(h323plus_prefix, [AS_HELP_STRING([--with-h323plus-prefix],[H323Plus installation prefix (default =prefix)],[40],[100])], [h323plus_prefix=$withval], [h323plus_prefix=""])
AC_ARG_WITH(plugins_prefix, [AS_HELP_STRING([--with-plugins-prefix],[plugins installation prefix (default =prefix)],[40],[100])], [plugins_prefix=$withval], [plugins_prefix=""])

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

PTLIB_INSTALLPREFIX=`echo ${PTLIB_INSTALLPREFIX} | sed "s/\/\//\//g"`
H323PLUS_INSTALLPREFIX=`echo ${H323PLUS_INSTALLPREFIX} | sed "s/\/\//\//g"`
LDFLAGS="${LDFLAGS} -L${PTLIB_INSTALLPREFIX}/lib -L${H323PLUS_INSTALLPREFIX}/lib "

if test "x$plugins_prefix" = "x"; then
  P_DEFAULT_PLUGIN_DIR=${PTLIB_INSTALLPREFIX}/lib/ptlib
  PLUGIN_INSTALLPREFIX=${P_DEFAULT_PLUGIN_DIR}
  AUDIO_PLUGIN_DIR=codecs/audio
  VIDEO_PLUGIN_DIR=codecs/video
else
  P_DEFAULT_PLUGIN_DIR=${plugins_prefix}
  PLUGIN_INSTALLPREFIX=${P_DEFAULT_PLUGIN_DIR}
  AUDIO_PLUGIN_DIR=codecs/audio
  VIDEO_PLUGIN_DIR=codecs/video
fi

#if test "x$LIB64" != "x" ; then
#  PLUGIN_INSTALLPREFIX="${PLUGIN_INSTALLPREFIX}64"
#fi

AC_SUBST(PTLIB_INSTALLPREFIX)
AC_SUBST(H323PLUS_INSTALLPREFIX)
AC_SUBST(P_DEFAULT_PLUGIN_DIR)
AC_SUBST(PLUGIN_INSTALLPREFIX)
AC_SUBST(AUDIO_PLUGIN_DIR)
AC_SUBST(VIDEO_PLUGIN_DIR)

dnl ########################################################################
dnl configuration openmcu-ru

MCUDIR=openmcu-ru
AC_ARG_WITH(separator,[  ])
AC_ARG_WITH(config_prefix, [AS_HELP_STRING([--with-config-prefix],[config directory prefix],[40],[100])], [config_prefix=$withval])
AC_ARG_WITH(log_prefix, [AS_HELP_STRING([--with-log-prefix],[log directory prefix],[40],[100])], [log_prefix=$withval])
AC_ARG_WITH(pipe_prefix, [AS_HELP_STRING([--with-pipe-prefix],[pipe directory prefix],[40],[100])], [pipe_prefix=$withval])
AC_ARG_WITH(records_prefix, [AS_HELP_STRING([--with-records-prefix],[records directory prefix],[40],[100])], [records_prefix=$withval])

MCU_BIN_DIR=${INSTALLPREFIX}/bin
MCU_SHARE_DIR=${INSTALLPREFIX}/share/${MCUDIR}
MCU_RESOURCE_DIR=${MCU_SHARE_DIR}/resource
MCU_FONT_DIR=${MCU_SHARE_DIR}/font

if test "x$config_prefix" = "x"; then
  MCU_CONFIG_DIR=${MCU_SHARE_DIR}/config
else
  MCU_CONFIG_DIR=${config_prefix}/${MCUDIR}
fi

if test "x$log_prefix" = "x"; then
  MCU_LOG_DIR=${MCU_SHARE_DIR}/log
else
  MCU_LOG_DIR=${log_prefix}/${MCUDIR}
fi

if test "x$pipe_prefix" = "x"; then
  MCU_PIPE_DIR=${MCU_SHARE_DIR}/pipe
else
  MCU_PIPE_DIR=${pipe_prefix}/${MCUDIR}
fi

if test "x$records_prefix" = "x"; then
  MCU_RECORDS_DIR=${MCU_SHARE_DIR}/records
else
  MCU_RECORDS_DIR=${records_prefix}/${MCUDIR}
fi

AC_SUBST(MCUDIR)
AC_SUBST(MCU_SHARE_DIR)
AC_SUBST(MCU_BIN_DIR)
AC_SUBST(MCU_CONFIG_DIR)
AC_SUBST(MCU_PIPE_DIR)
AC_SUBST(MCU_RESOURCE_DIR)
AC_SUBST(MCU_RECORDS_DIR)
AC_SUBST(MCU_FONT_DIR)
AC_SUBST(MCU_LOG_DIR)

AC_CONFIG_FILES(openmcu-ru/Makefile)
AC_CONFIG_FILES(openmcu-ru/config.h)

dnl ########################################################################
dnl configuration ptlib

PWLIBDIR=ptlib
PTLIBDIR=${PWLIBDIR}
AC_SUBST(PWLIBDIR)
AC_SUBST(PTLIBDIR)
AC_CONFIG_SUBDIRS(ptlib)
AC_CONFIG_FILES(ptlib/Makefile)

export PWLIBDIR=`pwd`/${PWLIBDIR}


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

vp8_ffmpeg=yes
AC_ARG_ENABLE(separator, [    ])
AC_ARG_ENABLE(vp8_ffmpeg, [AS_HELP_STRING([--disable-vp8-ffmpeg],[Disable VP8 ffmpeg decoding (default enabled)],[40],[100])], [vp8_ffmpeg="$enableval"])

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


dnl ########################################################################
dnl configure rpath stage1

rpath=yes
AC_ARG_ENABLE(separator, [     ])
AC_ARG_ENABLE(rpath, [AS_HELP_STRING([--disable-rpath],[do not hardcode runtime library paths (default auto)],[40],[100])], [rpath="$enableval"])


dnl ########################################################################
dnl configure development files

devel=no
AC_ARG_ENABLE(devel, [AS_HELP_STRING([--enable-devel],[install development files (default disabled, always disabled if static linking)],[40],[90])], [devel="$enableval"])
if test "${devel}" = yes; then
  ENABLE_DEVEL=1
else
  ENABLE_DEVEL=0
fi

AC_SUBST(ENABLE_DEVEL)

dnl ########################################################################
dnl configure static linking

static=yes
AC_ARG_ENABLE(static, [AS_HELP_STRING([--disable-static],[build static linking with ptlib,h323plus (default enabled)],[40],[100])], [static="$enableval"])

if test "${static}" = "yes"; then
  ENABLE_STATIC=1
  ENABLE_DEVEL=0
  MCULIBS="-Wl,--start-group ../ptlib/lib/libpt_${OSTYPE}_${MACHTYPE}_r_s.a ../h323plus/lib/libh323_${OSTYPE}_${MACHTYPE}_r_s.a -Wl,--end-group"
else
  ENABLE_STATIC=0
  MCULIBS="-L../ptlib/lib -L../h323plus/lib -lh323_${OSTYPE}_${MACHTYPE}_r -lpt_${OSTYPE}_${MACHTYPE}_r"
fi

AC_SUBST(MCULIBS)
AC_SUBST(ENABLE_STATIC)

AM_CONDITIONAL(ENABLE_DEVEL, [test ${ENABLE_DEVEL} = 1])

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
if test "${rpath}" = "yes"; then
  if test "${static}" != "yes"; then
    RPATH_LDFLAGS="${RPATH_LDFLAGS} -L${PTLIB_INSTALLPREFIX}/lib -L${H323PLUS_INSTALLPREFIX}/lib "
  fi
  RPATH_LDFLAGS="${RPATH_LDFLAGS} ${FREETYPE_LDFLAGS} ${JPEG_LDFLAGS} ${YUV_LDFLAGS} ${SOFIA_LDFLAGS} \
                 ${RESAMPLE_LDFLAGS} ${LIBAV_LDFLAGS} ${LIBVPX_LDFLAGS}"
  RPATH_LDFLAGS=$(echo $(printf '%s\n' $RPATH_LDFLAGS | sort -u))

  RPATH="-Wl"
  for r_path in ${RPATH_LDFLAGS}; do
    if test `echo ${r_path} | grep \\\-L`; then
      path=`echo ${r_path} | sed -e 's/-L//'`
      if test "${path}" != "/lib" && test "${path}" != "/usr/lib"; then
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

CPPFLAGS="${CPPFLAGS} ${FREETYPE_CPPFLAGS} ${JPEG_CPPFLAGS} ${YUV_CPPFLAGS} ${SOFIA_CPPFLAGS} \
         ${RESAMPLE_CPPFLAGS}"
CPPFLAGS=$(echo $(printf '%s\n' $CPPFLAGS | sort -u))

LDFLAGS="${LDFLAGS} ${FREETYPE_LDFLAGS} ${JPEG_LDFLAGS} ${YUV_LDFLAGS} ${SOFIA_LDFLAGS} \
         ${RESAMPLE_LDFLAGS}"
LDFLAGS=$(echo $(printf '%s\n' $LDFLAGS | sort -u))

LIBAV_CPPFLAGS=$(echo $(printf '%s\n' $LIBAV_CPPFLAGS | sort -u))
LIBAV_LDFLAGS=$(echo $(printf '%s\n' $LIBAV_LDFLAGS | sort -u))
LIBVPX_CPPFLAGS=$(echo $(printf '%s\n' $LIBVPX_CPPFLAGS | sort -u))
LIBVPX_LDFLAGS=$(echo $(printf '%s\n' $LIBVPX_LDFLAGS | sort -u))

dnl ########################################################################
dnl final declarations

AC_SUBST(LIBAV_CPPFLAGS)
AC_SUBST(LIBAV_LDFLAGS)
AC_SUBST(LIBVPX_CPPFLAGS)
AC_SUBST(LIBVPX_LDFLAGS)

AC_SUBST(RPATH)

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

sed -i -r "s#^PREFIX=.*#PREFIX=${PTLIB_INSTALLPREFIX}#g" ${PTLIBDIR}/Makefile
sed -i -r "s#^PTINSTDIR=.*#PTINSTDIR=${PTLIB_INSTALLPREFIX}/share/ptlib#g" ${PTLIBDIR}/make/ptlib-config
sed -i -r "s#^INSTALLPREFIX=.*#INSTALLPREFIX=${PTLIB_INSTALLPREFIX}#g" ${PTLIBDIR}/make/ptlib-config
sed -i -r "s#^\#define P_DEFAULT_PLUGIN_DIR.*#\#define P_DEFAULT_PLUGIN_DIR \"${P_DEFAULT_PLUGIN_DIR}\"#g" ${PTLIBDIR}/include/ptbuildopts.h
sed -i -r "s#^PREFIX=.*#PREFIX=${H323PLUS_INSTALLPREFIX}#g" ${H323PLUSDIR}/Makefile

dnl ########################################################################
dnl Summary
dnl ########################################################################
echo ""
echo "============== OpenMCU-ru Build Configuration ================="
echo "          Installing into prefix  :  $INSTALLPREFIX"

echo ""
if test "x${rendering_names}" = "xyes" ; then
echo "                       Subtitles  :  enabled"
else
echo "                       Subtitles  :  disabled"
fi
if test "x${live_video}" = "xyes" ; then
echo " JPEG Frame Encoder (for browser) :  enabled"
else
echo " JPEG Frame Encoder (for browser) :  disabled"
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

if test "x${optimized_frame_resize}" = "xyes" ; then
echo "       Use libyuv to resize video :  enabled"
else
echo "       Use libyuv to resize video :  disabled"
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

echo ""
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

echo ""
echo "                         OS Type  :  $target_os"
echo "                    Machine Type  :  $target_cpu"
echo ""
echo " If all settings are OK, type make and make install "
echo "========================================================"
