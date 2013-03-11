#
# Makefile
#
# Make file for OpenMCU
#
# Copyright (c) 1993-2001 Equivalence Pty. Ltd.
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
# The Original Code is Portable Windows Library.
#
# The Initial Developer of the Original Code is Equivalence Pty. Ltd.
#
# Portions are Copyright (C) 1993 Free Software Foundation, Inc.
# All Rights Reserved.
# 
# Contributor(s): ______________________________________.
#
# $Log: Makefile,v $
# Revision 1.1  2007/10/17 19:44:28  shorne
# Initial Commit
#
# Revision 2.4  2006/06/09 04:39:59  csoutheren
# Migrated VideoBranch to main trunk
#
# Revision 2.3  2006/03/06 11:07:20  csoutheren
# Updated docs
#
# Revision 2.2.2.6  2006/04/29 07:25:26  csoutheren
# Removed unused define
#
# Revision 2.2.2.5  2006/04/19 08:36:45  csoutheren
# Fixed typo
#
# Revision 2.2.2.4  2006/04/18 23:33:51  csoutheren
# Added tarball target
#
# Revision 2.2.2.3  2006/03/28 13:06:57  csoutheren
# Added new files
#
# Revision 2.2.2.2  2006/03/06 11:08:49  csoutheren
# Updated docs
#
# Revision 2.3  2006/03/06 11:07:20  csoutheren
# Updated docs
#
# Revision 2.2  2005/03/11 00:25:24  csoutheren
# Fixed problems with flags
#
# Revision 2.1  2004/05/27 09:58:50  csoutheren
# Added new source files to Makefile
#
# Revision 2.0  2004/03/08 02:06:24  csoutheren
# Totally rewritten to use new connection locking mecahnism
# Added ability to monitor conferences
# Added initial support for H.323 MCU messages
# Thanks to Citron Networks for supporting this work
#
# Revision 1.4  2002/08/05 10:10:29  robertj
# Normalised Makefile usage of openh323u.mak include file, fixing odd messages.
#
# Revision 1.3  2001/06/28 05:56:10  rogerh
# Add README to the tar sources for consistency
#
# Revision 1.2  2000/05/10 08:11:57  craigs
# Fixed copyrights and names
#
#

PROG		= openmcu
SOURCES		:= main.cxx video.cxx conference.cxx filemembers.cxx custom.cxx h323.cxx html.cxx mcu.cxx sip.cxx
HDRS		:= conference.h config.h filemembers.h h323.h mcu.h version.h sip.h

#LDLIBS		= -lfreetype -lswresample -ljpeg -lsofia-sip-ua
LDLIBS		= -lfreetype -lswresample -ljpeg -lsofia-sip-ua -lyuv

STDCCFLAGS	= -I/usr/local/include/freetype2 -I/usr/local/include/sofia-sip-1.12 -D__STDC_CONSTANT_MACROS

ifndef OPENH323DIR
OPENH323DIR=$(HOME)/openmcu/h323plus
endif

include $(OPENH323DIR)/openh323u.mak

doc:
	nroff -man openmcu.1 | col -b > openmcu.man

tarball: 
	tar -czvf openmcu.tar.gz $(SOURCES) $(HDRS) openmcu.man openmcu.1 connecting.wav entering.wav leaving.wav precompile.cxx openmcu.vcproj Makefile messages.mc mpl-1.0.htm openmcu.gif openmcu.ico ReadMe.txt
