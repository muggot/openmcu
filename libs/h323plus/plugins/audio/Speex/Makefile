#
# Makefile for Speex Codec Plugin
#
# Copyright (C) 2004 Post Increment, All Rights Reserved
#
# The contents of this file are subject to the Mozilla Public License
# Version 1.0 (the "License"); you may not use this file except in
# compliance with the License. You may obtain a copy of the License at
# http://www.mozilla.org/MPL/
#
# Software distributed under the License is distributed on an "AS IS"
# basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See
# the License for the specific language governing rights and limitations
# under the License.
#
# The Original Code is Open H323 library.
#
# The Initial Developer of the Original Code is Post Increment
#
# Contributor(s): ______________________________________.
#
# $Log: Makefile.in,v $
# Revision 1.1  2007/10/22 07:40:54  shorne
# *** empty log message ***
#
# Revision 1.5  2007/04/19 06:09:03  csoutheren
# Fixed plugin build system to use better detection for OSX
# Resolved 1636262 - Fix for audio plugins for OSX
#
# Revision 1.4  2007/03/01 09:16:21  dereksmithies
# Make certain it optimises the code when building the Speex codec
#
# Revision 1.3  2006/08/24 02:20:35  csoutheren
# Fixed compile problems on Linux
#
# Revision 1.2  2006/07/31 09:09:17  csoutheren
# Checkin of validated codec used during development
#
# Revision 1.1.2.1  2006/04/06 01:20:05  csoutheren
# Ported audio codec plugins from OpenH323 to OPAL
#
# Revision 1.5  2004/09/09 00:25:34  csoutheren
# Fixed Speex plugin on x64 systems thanks to Andrew Walrond. I know other
# have submitted this fix too - my apolgies for not including your names
#
# Revision 1.4  2004/08/22 04:20:19  csoutheren
# Added -fPIC to build commands to fix problems on 64 bit platforms
# Thanks to Klaus Kaempf
#
# Revision 1.3  2004/06/24 23:33:17  csoutheren
# Changed Makefiles to ensure all plugins have correct suffix
#
# Revision 1.2  2004/05/03 11:08:30  csoutheren
# Modified to allow plugins to be created on various platforms
#
# Revision 1.1  2004/04/29 14:48:00  csoutheren
# Reconfigured plugin makefiles per suggestions from Damien Sandras
# Changed install targets to use $(INSTALL)
#
# Revision 1.4  2004/04/27 09:59:27  csoutheren
# Updated codec specific Makefiles
#
# Revision 1.3  2004/04/09 12:24:18  csoutheren
# Renamed h323plugin.h to opalplugin.h, and modified everything else
# as required
#
# Revision 1.2  2004/04/04 12:57:49  csoutheren
# Updated Makefiles and fixed Linux problems
#
#
PREFIX=/usr/local
exec_prefix = ${PREFIX}
LIBDIR=${exec_prefix}/lib

SRCDIR	= ./libspeex

SONAME	= speexcodec

CC	 	=gcc
CFLAGS	 	=-g -O2
CXX	 	=g++
LDSO	 	=-shared -Wl,-soname,$(SONAME)
PLUGINEXT	=so
STDCCFLAGS	=-fPIC
LDFLAGS		=

H323_SYSTEM_SPEEX=

SRCS	= speexcodec.cxx 

ifndef H323_SYSTEM_SPEEX
SRCS   += $(SRCDIR)/bits.c \
          $(SRCDIR)/cb_search.c \
          $(SRCDIR)/exc_10_16_table.c \
          $(SRCDIR)/exc_10_32_table.c \
          $(SRCDIR)/exc_20_32_table.c \
          $(SRCDIR)/exc_5_256_table.c \
          $(SRCDIR)/exc_5_64_table.c \
          $(SRCDIR)/exc_8_128_table.c \
          $(SRCDIR)/fftwrap.c \
          $(SRCDIR)/filters.c \
          $(SRCDIR)/gain_table.c \
          $(SRCDIR)/gain_table_lbr.c \
          $(SRCDIR)/hexc_10_32_table.c \
          $(SRCDIR)/hexc_table.c \
          $(SRCDIR)/high_lsp_tables.c \
          $(SRCDIR)/kiss_fft.c \
          $(SRCDIR)/kiss_fftr.c \
          $(SRCDIR)/lbr_48k_tables.c \
          $(SRCDIR)/lpc.c \
          $(SRCDIR)/lsp.c \
          $(SRCDIR)/lsp_tables_nb.c \
          $(SRCDIR)/ltp.c \
          $(SRCDIR)/math_approx.c \
          $(SRCDIR)/mdf.c \
          $(SRCDIR)/medfilter.c \
          $(SRCDIR)/misc.c \
          $(SRCDIR)/modes.c \
          $(SRCDIR)/nb_celp.c \
          $(SRCDIR)/pcm_wrapper.c \
          $(SRCDIR)/quant_lsp.c \
          $(SRCDIR)/sb_celp.c \
          $(SRCDIR)/smallft.c \
          $(SRCDIR)/speex.c \
          $(SRCDIR)/speex_callbacks.c \
          $(SRCDIR)/speex_header.c \
          $(SRCDIR)/speex_preprocess.c \
          $(SRCDIR)/vbr.c \
          $(SRCDIR)/vorbis_psy.c \
          $(SRCDIR)/vq.c \

EXTRALIBS =
else
EXTRALIBS = -lspeex
endif

OBJDIR	= ./obj

vpath	%.o $(OBJDIR)
vpath	%.c $(SRCDIR)
vpath	%.cxx $(SRCDIR)

$(OBJDIR)/%.o : %.c
	@mkdir -p $(OBJDIR) >/dev/null 2>&1
	$(CC) -g -O2 -c $(STDCCFLAGS) $(CCFLAGS) -o $@ $<

$(OBJDIR)/%.o : %.cxx
	@mkdir -p $(OBJDIR) >/dev/null 2>&1
	$(CXX) -g -O2 -c -I../../../include $(STDCCFLAGS) $(CCFLAGS) -o $@ $<

PLUGIN	= ./speex_audio_pwplugin.$(PLUGINEXT)

OBJECTS	= $(addprefix $(OBJDIR)/,$(patsubst %.cxx,%.o,$(patsubst %.c,%.o,$(notdir $(SRCS)))))

$(PLUGIN): $(OBJECTS)
	$(CXX) $(LDSO) -o $@ $^ $(EXTRALIBS)

install:
	mkdir -p $(DESTDIR)$(LIBDIR)/pwlib/codecs/audio/
	cp $(PLUGIN) $(DESTDIR)$(LIBDIR)/pwlib/codecs/audio/

uninstall:
	rm -f $(DESTDIR)$(LIBDIR)/pwlib/codecs/audio/$(PLUGIN)

clean:
	rm -f $(OBJECTS) $(PLUGIN)

###########################################
