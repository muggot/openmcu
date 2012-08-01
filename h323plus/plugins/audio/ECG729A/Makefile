#
# Makefile for IMA-ADPCM codec plugin
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
# Revision 1.1  2007/10/22 07:40:47  shorne
# *** empty log message ***
#
# Revision 1.3  2007/04/19 06:09:03  csoutheren
# Fixed plugin build system to use better detection for OSX
# Resolved 1636262 - Fix for audio plugins for OSX
#
# Revision 1.2  2006/08/01 13:06:49  rjongbloed
# Added a raft of unvalidated audio codecs from OpenH323 tree
#
# Revision 1.1.2.1  2006/07/22 13:42:49  rjongbloed
# Added more plug ins
#
# Revision 1.1.2.1  2006/05/08 13:49:57  rjongbloed
# Imported all the audio codec plug ins from OpenH323
#
# Revision 1.4  2006/02/09 23:43:22  csoutheren
# Removed lines included incorrectly via copy and paste
#
# Revision 1.3  2004/08/22 04:20:19  csoutheren
# Added -fPIC to build commands to fix problems on 64 bit platforms
# Thanks to Klaus Kaempf
#
# Revision 1.2  2004/06/24 23:33:17  csoutheren
# Changed Makefiles to ensure all plugins have correct suffix
#
# Revision 1.1  2004/05/10 12:42:24  csoutheren
# Include IMA ADPCM plugin
#
#
PREFIX=/usr/local
exec_prefix = ${PREFIX}
LIBDIR=${exec_prefix}/lib

SONAME	= g729a

SRCDIR	= ./src
OBJDIR	= ./obj

CC	 	=gcc
CFLAGS	 	=-g -O2
CXX	 	=g++
LDSO	 	=-shared -Wl,-soname,$(SONAME)
PLUGINEXT	=so
STDCCFLAGS	=-fPIC
LDFLAGS		=

SRCS	+= easycodecs.cxx

vpath	%.o $(OBJDIR)
vpath	%.cxx $(SRCDIR)

$(OBJDIR)/%.o : %.cxx
	@mkdir -p $(OBJDIR) >/dev/null 2>&1
	$(CXX) -I../../../include $(STDCCFLAGS) $(OPTCCFLAGS) $(CFLAGS) -c $< -o $@

PLUGIN	= ./g729a_audio_pwplugin.$(PLUGINEXT)

OBJECTS = $(addprefix $(OBJDIR)/,$(patsubst %.cxx,%.o,$(notdir $(SRCS))))

$(PLUGIN): $(OBJECTS)
	$(CXX) $(LDSO) -o $@ $^ $(EXTRALIBS) -L./EasyG729A -lG729a

install:
	mkdir -p $(DESTDIR)$(LIBDIR)/pwlib/codecs/audio/
	cp $(PLUGIN) $(DESTDIR)$(LIBDIR)/pwlib/codecs/audio/

uninstall:
	rm -f $(DESTDIR)$(LIBDIR)/pwlib/codecs/audio/$(PLUGIN)

clean:
	rm -f $(OBJECTS) $(PLUGIN)

###########################################
