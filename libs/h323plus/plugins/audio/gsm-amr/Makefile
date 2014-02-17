#
# Makefile for GSM-AMR Codec Plugin
#
# Copyright (C) 2004 MX Telecom Ltd.
#
# $Id: Makefile.in,v 1.1 2007/10/22 07:41:08 shorne Exp $

PREFIX=/usr/local
exec_prefix = ${PREFIX}
LIBDIR=${exec_prefix}/lib
H323_GSMAMR_NB_FLOAT=1

OBJDIR	= ./obj
SRCDIR  = ./src

BASENAME= gsmamrcodec
SONAME	= $(BASENAME)

CC	 	=gcc
CFLAGS	 	=-g -O2
CXX	 	=g++
LDSO	 	=-shared -Wl,-soname,$(SONAME)
PLUGINEXT	=so
STDCCFLAGS	=-fPIC
LDFLAGS		=

SRCS	= amrcodec.c \
	interf_enc.c \
	interf_dec.c \
	sp_dec.c \
	sp_enc.c

vpath	%.o $(OBJDIR)
vpath	%.c $(SRCDIR)

CCFLAGS += -I../../../include -I$(SRCDIR)
CCFLAGS += -DIF2

PLUGIN	= ./$(BASENAME)_pwplugin.$(PLUGINEXT)

$(OBJDIR)/%.o : %.c
	@mkdir -p $(OBJDIR) >/dev/null 2>&1
	$(CC) -g -c $(STDCCFLAGS) $(CCFLAGS) -o $@ $<

OBJECTS	= $(addprefix $(OBJDIR)/,$(patsubst %.c,%.o,$(notdir $(SRCS))))


ifeq ($(H323_GSMAMR_NB_FLOAT),1)
$(PLUGIN): $(OBJECTS)
	$(CC) $(LDSO) -o $@ $^

install:
	mkdir -p $(DESTDIR)$(LIBDIR)/pwlib/codecs/audio/
	cp $(PLUGIN) $(DESTDIR)$(LIBDIR)/pwlib/codecs/audio/

uninstall:
	rm -f $(DESTDIR)$(LIBDIR)/pwlib/codecs/audio/$(PLUGIN)
else
install:
uninstall:
endif


clean:
	rm -f $(OBJECTS) $(PLUGIN)

###########################################
