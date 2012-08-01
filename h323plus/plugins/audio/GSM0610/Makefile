#
# Makefile for GSM 06.10
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
# Revision 1.1  2007/10/22 07:40:43  shorne
# *** empty log message ***
#
# Revision 1.3  2007/04/19 06:09:03  csoutheren
# Fixed plugin build system to use better detection for OSX
# Resolved 1636262 - Fix for audio plugins for OSX
#
# Revision 1.2  2006/07/31 08:52:09  csoutheren
# Checkin of validated codec used during development
#
# Revision 1.1.2.1  2006/04/06 01:20:04  csoutheren
# Ported audio codec plugins from OpenH323 to OPAL
#
# Revision 1.11  2006/02/20 08:54:32  csoutheren
# Fixed problem with Makefile. WTF did this come from? :)
#
# Revision 1.10  2006/02/09 23:42:54  csoutheren
# Added check for system gsm library
#
# Revision 1.9  2004/08/24 14:15:12  csoutheren
# Fixed potential problems with MS-GSM
#
# Revision 1.8  2004/08/22 04:20:18  csoutheren
# Added -fPIC to build commands to fix problems on 64 bit platforms
# Thanks to Klaus Kaempf
#
# Revision 1.7  2004/06/24 23:33:17  csoutheren
# Changed Makefiles to ensure all plugins have correct suffix
#
# Revision 1.6  2004/06/17 23:14:59  csoutheren
# Fixed error when using "make install" with embedded GSM
# Thanks to Matthias Weber
#
# Revision 1.5  2004/05/18 22:24:24  csoutheren
# Initiali support for embedded codecs
#
# Revision 1.4  2004/05/03 11:08:30  csoutheren
# Modified to allow plugins to be created on various platforms
#
# Revision 1.3  2004/05/02 13:57:05  rjongbloed
# Fixed building local GSM if no system library
#
# Revision 1.2  2004/04/29 14:47:59  csoutheren
# Reconfigured plugin makefiles per suggestions from Damien Sandras
# Changed install targets to use $(INSTALL)
#
# Revision 1.1  2004/04/29 10:48:55  csoutheren
# Initial version
#
# Revision 1.1  2004/04/27 09:59:27  csoutheren
# Updated codec specific Makefiles
#
#
#
PREFIX=/usr/local
exec_prefix = ${PREFIX}
LIBDIR=${exec_prefix}/lib

H323_SYSTEM_GSM=0
SONAME	= gsm0610
H323_EMBEDDED_GSM=@H323_EMBEDDED_GSM@

SRCDIR		= ./src
OBJDIR		= ./obj

CC	 	=gcc
CFLAGS	 	=-g -O2
CXX	 	=g++
LDSO	 	=-shared -Wl,-soname,$(SONAME)
PLUGINEXT	=so
STDCCFLAGS	=-fPIC
LDFLAGS		=


SRCS	+= gsm06_10_codec.c

ifeq ($(H323_SYSTEM_GSM),0)

SRCS	+= $(SRCDIR)/gsm_create.c \
           $(SRCDIR)/gsm_destroy.c \
	   $(SRCDIR)/gsm_decode.c \
           $(SRCDIR)/gsm_encode.c \
           $(SRCDIR)/gsm_option.c \
	   $(SRCDIR)/code.c \
           $(SRCDIR)/decode.c \
           $(SRCDIR)/add.c \
	   $(SRCDIR)/gsm_lpc.c \
           $(SRCDIR)/rpe.c \
           $(SRCDIR)/preprocess.c \
           $(SRCDIR)/long_term.c \
           $(SRCDIR)/short_term.c \
           $(SRCDIR)/table.c

EXTRALIBS = 

else

EXTRALIBS = -lgsm

endif

vpath	%.o $(OBJDIR)
vpath	%.c $(SRCDIR)

ifeq ($(H323_EMBEDDED_GSM),1)
EXTRACCFLAGS=-DOPAL_STATIC_CODEC
endif

$(OBJDIR)/%.o : %.c
	@mkdir -p $(OBJDIR) >/dev/null 2>&1
	$(CC) -I../../../include -I./inc -DWAV49 -DNeedFunctionPrototypes=1 $(EXTRACCFLAGS) $(STDCCFLAGS) $(OPTCCFLAGS) $(CFLAGS) -c $< -o $@

PLUGIN	= ./gsm0610_audio_pwplugin.$(PLUGINEXT)
STATIC	= ./libgsm0610_audio_s.a

OBJECTS = $(addprefix $(OBJDIR)/,$(patsubst %.c,%.o,$(notdir $(SRCS))))

ifeq ($(H323_EMBEDDED_GSM),1)

$(STATIC): $(OBJECTS)
	$(AR) rcs $@ $^ 

install:

uninstall:

else

$(PLUGIN): $(OBJECTS)
	$(CC) $(LDSO) -o $@ $^ $(EXTRALIBS)
endif

install:
	mkdir -p $(DESTDIR)$(LIBDIR)/pwlib/codecs/audio/
	cp $(PLUGIN) $(DESTDIR)$(LIBDIR)/pwlib/codecs/audio/

uninstall:
	rm -f $(DESTDIR)$(LIBDIR)/pwlib/codecs/audio/$(PLUGIN)

clean:
	rm -f $(OBJECTS) $(PLUGIN)

###########################################
