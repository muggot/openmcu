AC_INIT(configure.ac)

AC_PROG_CXX
AC_HEADER_STDC

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
AC_CANONICAL_TARGET()
LDSO=
PLUGINEXT=so
STDCCFLAGS=-fPIC
case "$target_os" in
	solaris*) LDSO="-Bdynamic -G -h" ;;
        darwin*)  
	  CFLAGS="-fno-common -dynamic"
          LDSO="-dynamiclib -multiply_defined suppress"
	  STDCCFLAGS="$STDCCFLAGS -D__MACOSX__"
	  case "$target_cpu" in
	   x86 | i686 | i586 | i486 | i386 ) STDCCFLAGS="${STDCCFLAGS} -m486" ;;
          esac
	  PLUGINEXT=dylib 
        ;;
        *)LDSO='-shared -Wl,-soname,$(SONAME)';;
esac
AC_SUBST(LDSO, ${LDSO})
AC_SUBST(PLUGINEXT, ${PLUGINEXT})
AC_SUBST(STDCCFLAGS, ${STDCCFLAGS})

dnl ########################################################################
dnl look for amr-nb_float

H323_GSMAMR_NB_FLOAT=0
if test -r "./audio/gsm-amr/src/interf_dec.h" ; then
  AC_MSG_NOTICE(Enabling GSM-AMR plugin)
  H323_GSMAMR_NB_FLOAT=1
else
  AC_MSG_NOTICE(GSM-AMR plugin disabled (no 3GPP source))
fi
AC_SUBST(H323_GSMAMR_NB_FLOAT, $H323_GSMAMR_NB_FLOAT)


dnl ########################################################################
dnl Look for system GSM
dnl
dnl Note that we can only use llibgsm if it is compiled with WAV49
dnl

H323_SYSTEM_GSM=0
localgsm="xxx"
AC_ARG_ENABLE(localgsm,
       [  --enable-localgsm         use local version of GSM library rather than system version],
       localgsm=$enableval)

if test "${localgsm}" = "yes" ; then
  AC_MSG_NOTICE(Forcing use of local GSM sources)
elif test "${localgsm}" = "no" ; then
  AC_MSG_NOTICE(Forcing use of system GSM library)
  H323_SYSTEM_GSM=1
else
  AC_CHECK_LIB(gsm, gsm_create, GSM=1)
  if test "x$GSM" != "x"; then
    AC_MSG_CHECKING(if system GSM library has WAV49)
    OLD_LIBS=$LIBS
    LIBS="-lgsm"
    AC_RUN_IFELSE(
    [AC_LANG_PROGRAM([[
    #include <gsm.h>
]],[[
    int option = 0;
    gsm handle = gsm_create();
    return (gsm_option(handle, GSM_OPT_WAV49, &option) == -1) ? 1 : 0;
]])], GSM=2) 
    if test "x$GSM" == "x2"; then
      H323_SYSTEM_GSM=1
      AC_MSG_RESULT(yes)
      AC_MSG_NOTICE(Using system gsm)
    else
      AC_MSG_RESULT(no)
    fi
    LIBS=$OLD_LIBS
  fi
fi
AC_SUBST(H323_SYSTEM_GSM, $H323_SYSTEM_GSM)
AC_DEFINE_UNQUOTED(H323_SYSTEM_GSM, $H323_SYSTEM_GSM)

dnl ########################################################################
dnl Look for system libspeex

AC_CHECK_SIZEOF(short)
AC_CHECK_SIZEOF(int)
AC_CHECK_SIZEOF(long)
AC_CHECK_SIZEOF(long long)

case 2 in
        $ac_cv_sizeof_short) SIZE16="short";;
        $ac_cv_sizeof_int) SIZE16="int";;
esac

case 4 in
        $ac_cv_sizeof_int) SIZE32="int";;
        $ac_cv_sizeof_long) SIZE32="long";;
        $ac_cv_sizeof_short) SIZE32="short";;
esac

AC_SUBST(SIZE16)
AC_SUBST(SIZE32)


localspeex="xxx"
AC_ARG_ENABLE(localspeex,
       [  --enable-localspeex       use local version of Speex library rather than system version],
       localspeex=$enableval)

if test "${localspeex}" = "yes" ; then
  AC_MSG_NOTICE(Forcing use of local Speex sources)
elif test "${localspeex}" = "no" ; then
  AC_MSG_NOTICE(Forcing use of system Speex library)
  AC_DEFINE(H323_SYSTEM_SPEEX, 1)
  AC_SUBST(H323_SYSTEM_SPEEX, 1)
else
  AC_CHECK_LIB(speex, speex_encoder_destroy, SPEEX=1)
  if test "x$SPEEX" = "x"; then
    AC_MSG_NOTICE(Forcing use of local Speex library)
  else
    AC_MSG_CHECKING(system Speex version)
    cat > t.c <<C_FILE
    #include <speex.h>
    #include <speex_header.h>
    #include <stdio.h>
    int main(int argc,char *argv[])
    {
      SpeexHeader header;
      speex_init_header(&header, 1, 1, &speex_nb_mode);
      printf("%s\n", header.speex_version);
    }
C_FILE
    cc -o t t.c -lspeex > /dev/null 2>&1
    if test \! -x t ; then
      AC_MSG_RESULT(cannot determine - using library version)
    else
      SYSVER=`./t`
      rm -f t t.c
      AC_MSG_RESULT($SYSVER)
      AC_MSG_CHECKING(library Speex version)
      LIBVER=`grep "#define VERSION" ./audio/Speex/libspeex/misc.h | sed -e 's/^.*speex\-//' -e 's/\"//'`
      AC_MSG_RESULT($LIBVER)
      AC_MSG_CHECKING(Speex versions)
      cat > t.pl <<P_FILE
      [
      \$sysver = @ARGV[0];
      \$libver = @ARGV[1];
      @lib = split /\./, \$libver;
      while (@lib < 3) {
        @lib[0+@lib] = "0";
      }
      @sys = split /\./, \$sysver;
      while (@sys < 3) {
        @sys[0+@sys] = "0";
      }
      \$i = 0;
      while (\$i < 3) {
        #print "comparing " . @sys[\$i] . " and " . @lib[\$i] . "\n";
        if (\@sys[\$i] < @lib[\$i]) {
          print "0";
          die;
        }
        \$i++;
      }
      print "1";
      ]
P_FILE
      SPEEX=`perl t.pl $SYSVER $LIBVER`
      rm t.pl
      if test "x$SPEEX" = "x" ; then
        AC_MSG_RESULT(library version is more recent)
      else
        AC_MSG_RESULT(system version is more recent)
        AC_DEFINE(H323_SYSTEM_SPEEX, 1)
        AC_SUBST(H323_SYSTEM_SPEEX, 1)
      fi
    fi
  fi
fi

dnl ########################################################################
dnl
dnl ffmpeg for H.263
dnl 

HAS_H263=1
AC_SUBST(HAS_H263)


dnl ########################################################################
dnl
dnl Quicknet xJACK cards
dnl 

HAS_IXJ=0
case "$target_os" in
        darwin*) ;;
        *)
          AC_CHECK_HEADERS(linux/telephony.h sys/telephony.h /usr/local/include/sys/telephony.h, HAS_IXJ=1)
        ;;
esac
if test "x$HAS_IXJ" != "x" ; then
  AC_MSG_CHECKING(if ixjuser.h actually compiles)
  AC_TRY_COMPILE([#include <linux/ixjuser.h>],
                 [int main(int ac,char **av) { return 0; }], HAS_IXJ=1, HAS_IXJ=)
  if test "x$HAS_IXJ" != "x" ; then
    AC_MSG_RESULT(yes)
    HAS_IXJ=1
  else
    AC_MSG_RESULT(no)
  fi
fi
AC_SUBST(HAS_IXJ)


dnl ########################################################################
dnl
dnl VoiceTronics VPB card
dnl 

HAS_VPB=0
AC_MSG_CHECKING(Voicetronix vpb)
case "$target_os" in
        darwin*) vpb=no ;;
        *)
          vpb=yes
      	  AC_ARG_ENABLE(vpb,
	       [  --enable-vpb            enable voicetronix vpb],
	       vpb=$enableval)
          ;;
esac

if test "$vpb" = "yes" ; then
  HAS_VPB=1
  AC_MSG_RESULT(enabled)
else
  AC_MSG_RESULT(disabled)
fi
AC_SUBST(HAS_VPB)


dnl ########################################################################
dnl
dnl CAPI
dnl 

AC_CHECK_HEADERS(capi20.h, HAS_CAPI20_H=1)
HAS_CAPI=0
AC_MSG_CHECKING(CAPI)
case "$target_os" in
        darwin*) capi=no ;;
        *)
          capi=yes
	  AC_ARG_ENABLE(capi,
	       [  --enable-capi            enable CAPI],
	       capi=$enableval)
          ;;
esac

if test "$capi" = "yes" ; then
  if test "x$HAS_CAPI20_H" != "x" ; then
    HAS_CAPI=1
    AC_MSG_RESULT(enabled)
  else
    AC_MSG_RESULT(not available)
  fi
else
  AC_MSG_RESULT(disabled)
fi
AC_SUBST(HAS_CAPI)

dnl ########################################################################
dnl check for dynamic library code (dlopen and friends)

HAS_LIBDL=0
AC_CHECK_HEADERS([dlfcn.h], [HAS_DLFCN=yes], [HAS_DLFCN=no])
if test "$HAS_DLFCN" = yes ; then
  AC_MSG_CHECKING(if dlopen is available)
  AC_TRY_COMPILE([#include <dlfcn.h>],
                  [void * p = dlopen("lib", 0);], [HAS_DLOPEN=yes], [HAS_DLOPEN=no])
  if test "$HAS_DLOPEN" = no ; then
    AC_MSG_RESULT(no)
  else
    AC_MSG_RESULT(yes)
    AC_CHECK_LIB([dl],[dlopen],[HAS_LIBDL=1], [HAS_LIBDL=0])
  fi
fi
AC_SUBST(HAS_LIBDL)


dnl ########################################################################
dnl
dnl MPEG4 plugin - needs ffmpeg source tree
dnl 

HAS_MPEG4=0
FFMPEG_SOURCE=
AC_MSG_CHECKING(MPEG4 plugin)
AC_ARG_WITH(ffmpeg-src-dir,
            AC_HELP_STRING([--with-ffmpeg-src-dir],[Directory with ffmpeg source code, needed for MPEG 4 plugin]))

if test "$with_mpeg4_ffmpeg" = "yes" ; then
  AC_MSG_ERROR([--with-ffmpeg-src-dir requires an argument])
elif test "$with_mpeg4_ffmpeg" = "no" ; then
  AC_MSG_RESULT(disabled)
elif test "$with_ffmpeg_src_dir" != "" ; then
  if test -f "$with_ffmpeg_src_dir/libavcodec/avcodec.h"
  then
    AC_MSG_RESULT(enabled)
    HAS_MPEG4=1
    FFMPEG_SOURCE=$with_ffmpeg_src_dir
  else
    AC_MSG_ERROR([$with_ffmpeg_src_dir doesn't contain an ffmpeg source tree])
  fi
else
  AC_MSG_RESULT(disabled)
fi

AC_ARG_ENABLE(ffmpeg-stackalign-hack,
            AC_HELP_STRING([--enable-ffmpeg-stackalign-hack],[Enable FFMPEG stack align hack for MPEG 4]), ffmpeg_stackalign_hack=$enableval)
if test "${ffmpeg_stackalign_hack}" = "yes" ; then
  AC_MSG_NOTICE(FFMPEG stack align hack enabled)
  FFMPEG_STACKALIGN_HACK="-DWITH_STACKALIGN_HACK"
else
  AC_MSG_NOTICE(FFMPEG stack align hack disabled)
  FFMPEG_STACKALIGN_HACK=""
fi

AC_SUBST(HAS_MPEG4)
AC_SUBST(FFMPEG_SOURCE)
AC_SUBST(FFMPEG_STACKALIGN_HACK)

dnl ###########################################################################
dnl Checks for ffmpeg
dnl ###########################################################################

HAS_FFMPEG=0

AC_ARG_ENABLE(ffmpeg, [  --disable-ffmpeg        Disable ffmpeg],,)
if test "x${enable_ffmpeg}" != "xno" ; then
    enable_ffmpeg=yes
    saved_LIBS="$LIBS"
    LIBS="$LIBS -lm -lz"
    AC_CHECK_LIB(avcodec, avcodec_init, [ffmpeg_libs=true], [ffmpeg_libs=false])
    LIBS=$saved_LIBS
    if test "x${ffmpeg_libs}" != "xno"; then
        AC_CHECK_HEADERS([ffmpeg/avcodec.h], [ffmpeg_headers=true], [ffmpeg_headers=false])
        if test "x${ffmpeg_headers}" = "xtrue"; then

            AC_MSG_CHECKING(for libavcodec version >= 51.11.0)
            AC_EGREP_CPP([yes],
            [#include <ffmpeg/avcodec.h>
  	     #ifdef LIBAVCODEC_VERSION_INT
               #if LIBAVCODEC_VERSION_INT >= ((51<<16)+(11<<8)+0)
                 yes
               #endif
             #endif
            ],
   	    [ffmpeg_version=true], [ffmpeg_version=false])
            if test "x${ffmpeg_version}" = "xtrue"; then
                AC_MSG_RESULT([yes])
                HAS_FFMPEG=1
                AC_MSG_CHECKING(filename of libavcodec library)
                cat > t.c <<C_FILE
                  int main () {}
C_FILE
                cc -o t t.c -lavcodec > /dev/null 2>&1
                if test \! -x t ; then
                    AC_MSG_RESULT(cannot determine - using defaults)
                 else
                    ffmpeg_lib_name=`ldd ./t  | grep libavcodec | awk '{ print $1; }'`
                    rm -f t t.c
                    AC_MSG_RESULT($ffmpeg_lib_name)
                    FFMPEG_CFLAGS="-DLIBAVCODEC_LIB_NAME='\"$ffmpeg_lib_name\"'"
                fi
             else
              AC_MSG_RESULT([no])
    	    fi
        fi
    fi
fi
AC_SUBST(FFMPEG_CFLAGS)
AC_SUBST(HAS_FFMPEG)

dnl ###########################################################################
dnl Checks for x264
dnl ###########################################################################

HAS_X264=0
X264_LIBS=

AC_ARG_ENABLE(x264, [  --disable-x264          Disable x264],,)
if test "x${enable_x264}" != "xno" ; then
    enable_x264=yes

    saved_LIBS="$LIBS"
    LIBS="$LIBS -lm -lz -lpthread"
    AC_CHECK_LIB(x264, x264_encoder_open, [x264_libs=yes], [x264_libs=no])
    LIBS=$saved_LIBS

    if test "x${x264_libs}" != "xno"; then
        AC_CHECK_HEADERS([x264.h], [x264_headers=true], [x264_headers=false])
        if test "x${x264_headers}" = "xtrue"; then
          if test "x$HAS_LIBDL" != "x0" ; then
	    H264_LIBS="-ldl"
          fi

          AC_MSG_CHECKING(for x264 version >= 48)
          AC_EGREP_CPP([yes],
          [#include <x264.h>
	   #ifdef X264_BUILD
             #if X264_BUILD >= 48
               yes
             #endif
           #endif
          ],
 	  [x264_version=true], [x264_version=false])
          if test "x${x264_version}" = "xtrue"; then
              AC_MSG_RESULT([yes])
              HAS_X264=1
              AC_MSG_CHECKING(filename of x264 library)
              cat > t.c <<C_FILE
                int main () {}
C_FILE
              cc -o t t.c -lx264 > /dev/null 2>&1
              if test \! -x t ; then
                  AC_MSG_RESULT(cannot determine - using defaults)
                else
                  x264_lib_name=`ldd ./t  | grep libx264 | awk '{ print $1; }'`
                  rm -f t t.c
                  AC_MSG_RESULT($x264_lib_name)
                  X264_CFLAGS="-DLIBX264_LIB_NAME='\"${x264_lib_name}\"'"
              fi
              X264_CFLAGS="-DLIBX264_LIB_NAME='\"${x264_lib_name}\"'"
	    else
              AC_MSG_RESULT([no])
	    fi
        fi
    fi
fi
AC_SUBST(HAS_X264)
AC_SUBST(X264_CFLAGS)
AC_SUBST(H264_LIBS)

dnl ###########################################################################
dnl Checks for libtheora
dnl ###########################################################################

HAS_THEORA=0
THEORA_LIBS=

AC_ARG_ENABLE(theora, [  --disable-theora        Disable theora],,)
if test "x${enable_theora}" != "xno" ; then
    enable_theora=yes

    saved_LIBS="$LIBS"
    LIBS="$LIBS -lm -lz -lpthread"
    AC_CHECK_LIB(theora, theora_encode_init, [theora_libs=yes], [theora_libs=no])
    LIBS=$saved_LIBS

    if test "x${theora_libs}" != "xno"; then
	THEORA_LIBS="-ltheora"
        AC_CHECK_HEADERS([theora/theora.h], [theora_headers=true], [theora_headers=false])
        if test "x${theora_headers}" = "xtrue"; then
            HAS_THEORA=1
        fi
    fi
fi
AC_SUBST(HAS_THEORA)
AC_SUBST(THEORA_LIBS)
dnl ########################################################################

if test -d audio/GSM0610 ; then
  AC_CONFIG_FILES(audio/GSM0610/Makefile)
fi
if test -d audio/gsm-amr ; then
  AC_CONFIG_FILES(audio/gsm-amr/Makefile)
fi
if test -d audio/iLBC ; then
  AC_CONFIG_FILES(audio/iLBC/Makefile)
fi
if test -d audio/Speex ; then
  AC_CONFIG_FILES(audio/Speex/Makefile)
fi
if test -d audio/LPC_10 ; then
  AC_CONFIG_FILES(audio/LPC_10/Makefile)
fi
if test -d audio/G726 ; then
  AC_CONFIG_FILES(audio/G726/Makefile)
fi
if test -d audio/IMA_ADPCM ; then
  AC_CONFIG_FILES(audio/IMA_ADPCM/Makefile)
fi
if test -d video/H.261-vic ; then
  AC_CONFIG_FILES(video/H.261-vic/Makefile)
fi
if test -d video/H.263-ffmpeg ; then
  AC_CONFIG_FILES(video/H.263-ffmpeg/Makefile)
fi
if test -d video/H.263-1998 ; then
  AC_CONFIG_FILES(video/H.263-1998/Makefile)
fi
if test -d video/MPEG4-ffmpeg ; then
  AC_CONFIG_FILES(video/MPEG4-ffmpeg/Makefile)
fi
if test -d video/H.264 ; then
  AC_CONFIG_FILES(video/H.264/Makefile)
fi
if test -d video/H.264/gpl; then
  AC_CONFIG_FILES(video/H.264/gpl/Makefile)
fi
if test -d video/THEORA ; then
  AC_CONFIG_FILES(video/THEORA/Makefile)
fi
if test -d LID/IxJ ; then
  AC_CONFIG_FILES(LID/IxJ/Makefile)
fi
if test -d LID/VPB ; then
  AC_CONFIG_FILES(LID/VPB/Makefile)
fi
if test -d LID/CAPI ; then
  AC_CONFIG_FILES(LID/CAPI/Makefile)
fi
if test -d fax/fax_spandsp -a -d fax/fax_spandsp/spandsp_util ; then
  AC_CONFIG_FILES(fax/fax_spandsp/Makefile)
fi

AC_CONFIG_FILES(Makefile)

dnl ########################################################################
dnl Header file

dnl ########################################################################
dnl Output configured files

AC_OUTPUT(audio/Speex/libspeex/speex_config_types.h)
AC_OUTPUT()

dnl ########################################################################
dnl Summary
dnl ########################################################################
echo ""
echo "================ Codec Plugin configuration ==================="
echo "          Installing into prefix  :  $INSTALLPREFIX"
echo ""
echo "                           H.261  :  enabled"
if test "x${HAS_H263}" = "x1" ; then
echo "                           H.263  :  enabled"
else
echo "                           H.263  :  disabled"
fi
if test "x${HAS_FFMPEG}" = "x1" ; then
echo "                           H.263+ :  enabled"
else
echo "                           H.263+ :  disabled (needs libavcodec)"
fi
if test "x${HAS_LIBDL}" = "x1" ; then
if test "x${HAS_X264}" = "x1" ; then
if test "x${HAS_FFMPEG}" = "x1" ; then
echo "                           H.264  :  enabled"
else
echo "                           H.264  :  disabled (needs libavcodec)"
fi
else
echo "                           H.264  :  disabled (needs libx264)"
fi
else
echo "                           H.264  :  disabled (needs libdl)"
fi
if test "x${HAS_THEORA}" = "x1" ; then
echo "                          THEORA  :  enabled"
else
echo "                          THEORA  :  disabled"
fi
if test "x${HAS_MPEG4}" = "x1" ; then
echo "                    MPEG4 Part 2  :  enabled"
else
echo "                    MPEG4 Part 2  :  disabled (needs ffmpeg sources)"
fi
echo ""
echo "                         OS Type  :  $target_os"
echo "                    Machine Type  :  $target_cpu"
echo ""
echo " If all settings are OK, type make and make install "
echo "========================================================"
