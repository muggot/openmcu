#!/bin/sh

top_srcdir=`pwd`
UNAME=`uname`

set -v
#Generating build scripts...

aclocal
autoconf
automake --add-missing --copy

if [ -d ${top_srcdir}/libs/libg722_1 ] ; then
  cd ${top_srcdir}/libs/libg722_1/ && ./autogen.sh
fi

if [ "$UNAME" = "FreeBSD" ] ; then
  if [ -d ${top_srcdir}/libs/speex ] ; then
    cd ${top_srcdir}/libs/speex && autoreconf -fiv
  fi
  if [ -d ${top_srcdir}/libs/sofia-sip ] ; then
    cd ${top_srcdir}/libs/sofia-sip && autoreconf -fiv
  fi
fi
