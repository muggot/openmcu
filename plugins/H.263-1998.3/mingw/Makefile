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
# Contributor(s): Matthias Schneider.
#

VC_PLUGIN_DIR=codecs/video
prefix=/usr/local/lib/ptlib
exec_prefix=${prefix}
target_os=win32

DL_LIBS =

BASENAME	=h263-1998
SONAME		=$(BASENAME)
COMMONDIR	=../common
PLUGINDIR	=../opal
OBJDIR 		=./obj

CC	 	=gcc
CFLAGS	 	= -Os  
CXX	 	=g++
LDSO	 	=-shared -Wl,-soname,$(SONAME)
PLUGINEXT	=dll
STDCCFLAGS	=   -fPIC
LIBAVCODEC_CFLAGS=-I${prefix}/include  -I/local/include
LDFLAGS	       +=   -L/local/lib -lavcodec -lavutil
EXTRACFLAGS	= -I$(PLUGINDIR) -D__STDC_CONSTANT_MACROS -DPLUGIN_CODEC_DLL_EXPORTS -DHAVE_POSIX_MEMALIGN=0

# HAVE_LIBAVCODEC_RTP_MODE=1

ifdef HAVE_LIBAVCODEC_RTP_MODE
STDCCFLAGS += -DLIBAVCODEC_RTP_MODE=1
endif

# TRACE_FILE=1

ifdef TRACE_FILE
CFLAGS         += -DTRACE_FILE=1
endif

EXTRACCFLAGS    += $(LIBAVCODEC_CFLAGS) -I$(COMMONDIR) -I$(PLUGINDIR)

vpath   %.cxx $(COMMONDIR)
vpath   %.o   $(OBJDIR)


SRCS	+= h263-1998.cxx \
	   h263pframe.cxx \
	   rfc2190.cxx \
	   $(COMMONDIR)/trace.cxx \
	   $(COMMONDIR)/mpi.cxx

$(OBJDIR)/%.o : ../%.cxx
	@mkdir -p $(OBJDIR) >/dev/null 2>&1
	$(Q_CC)$(CXX) $(EXTRACCFLAGS) $(STDCCFLAGS) $(OPTCCFLAGS) $(CFLAGS) $(EXTRACFLAGS) -c $< -o $@

PLUGIN	= ./$(BASENAME)_video_pwplugin.$(PLUGINEXT)
STATIC	= ./lib$(BASENAME)_video_s.a

OBJECTS = $(addprefix $(OBJDIR)/,$(patsubst %.cxx,%.o,$(notdir $(SRCS))))

$(PLUGIN): $(OBJECTS)
ifeq (,$(findstring solaris,$(target_os)))
	$(Q_LD)$(CXX) $(LDSO) -o $@ $^ $(LDFLAGS)
else	
	$(Q_LD)$(CXX) $(LDSO) $(DL_LIBS) $@ -o $@ $^ $(LDFLAGS)
endif
    
install: $(PLUGIN)
	mkdir -p $(DESTDIR)$(prefix)/$(VC_PLUGIN_DIR)
	install $(PLUGIN) $(DESTDIR)$(prefix)/$(VC_PLUGIN_DIR)

uninstall:
	rm -f $(DESTDIR)$(prefix)/$(VC_PLUGIN_DIR)/$(PLUGIN)

clean:
	rm -f $(OBJECTS) $(PLUGIN)

###########################################
