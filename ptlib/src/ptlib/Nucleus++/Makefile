#
# Makefile
#
# Make file for ptlib library
#
# Portable Windows Library
#
# Copyright (c) 1993-1998 Equivalence Pty. Ltd.
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
# Revision 1.1  2000/06/26 11:17:20  robertj
# Nucleus++ port (incomplete).
#
# Revision 1.48  1999/09/27 01:04:42  robertj
# BeOS support changes.
#
# Revision 1.47  1999/07/03 04:31:53  robertj
# Fixed problems with not including oss.cxx in library if OSTYPE not "linux"
#
# Revision 1.46  1999/06/28 09:28:02  robertj
# Portability issues, especially n BeOS (thanks Yuri!)
#
# Revision 1.45  1999/06/21 00:57:10  robertj
# Fixed inclusion of getdate.tab.c to clean files.
#
# Revision 1.44  1999/06/09 15:41:18  robertj
# Added better UI to make files.
#
# Revision 1.43  1999/04/18 09:36:31  robertj
# Get date grammar build.
#
# Revision 1.42  1999/04/16 14:37:41  craigs
# Added build for getdate.y
#
# Revision 1.41  1999/03/02 05:41:59  robertj
# More BeOS changes
#
# Revision 1.40  1999/02/25 03:44:58  robertj
# Sound driver implementation changes for various unix platforms.
#
# Revision 1.39  1999/02/22 13:24:47  robertj
# Added first cut sound implmentation.
#
# Revision 1.38  1999/02/19 11:33:51  robertj
# Added IP access control lists file.
#
# Revision 1.37  1999/01/22 00:30:46  robertj
# Yet more build environment changes.
#
# Revision 1.36  1999/01/21 08:27:18  robertj
# Added tarfiles target
#
# Revision 1.35  1999/01/16 09:54:04  robertj
# Changed some macros to more informative names.
#
# Revision 1.34  1998/12/04 12:53:14  craigs
# Fixed problem with regex in wrong directory
#
# Revision 1.33  1998/12/02 02:10:19  robertj
# New directory structure
#
# Revision 1.32  1998/11/22 10:44:10  craigs
# New GUI build system - for sure!
#
# Revision 1.31  1998/11/22 08:11:28  craigs
# Changed for new GUI build mechanism
#
# Revision 1.29  1998/10/15 07:03:46  robertj
# Moved object.cxx to last place so memory leak destructor is called at the right time.
#
# Revision 1.28  1998/09/24 07:40:34  robertj
# Moved structured file to common.
# Removed small files only containing pragma implementations
#
# Revision 1.27  1998/09/24 04:20:50  robertj
# Added open software license.
#
# Revision 1.26  1998/08/21 05:32:14  robertj
# Allowed for multiple "all" target, so can build lib from app makefile.
#
# Revision 1.25  1998/02/12 04:17:55  robertj
# Made DEBUG variable optional
#
# Revision 1.24  1998/01/26 07:25:21  robertj
# Removed tiny files, as now have pragma in the common files.
#
# Revision 1.22  1998/01/04 09:41:20  robertj
# Changed perasn.cxx to asner.cxx
#
# Revision 1.21  1998/01/04 08:29:37  craigs
# Changed for Linux
#
# Revision 1.20  1997/04/22 11:00:01  craigs
# Added SSL and DLL functions
#
# Revision 1.19  1996/11/17 08:35:50  craigs
# Fixed distsrc target
#
# Revision 1.18  1996/11/17 02:35:12  craigs
# Removed SSL from list of build targets
#
# Revision 1.16  1996/10/31 10:30:03  craigs
# Lots of changes
#
# Revision 1.15  1996/09/21 05:41:41  craigs
# Added new common files
#
# Revision 1.14  1996/08/03 12:05:10  craigs
# *** empty log message ***
#
# Revision 1.13  1996/07/03 06:32:18  craigs
# Removed spurious lines causing warnings during library make
#
# Revision 1.12  1996/06/29 11:37:28  craigs
# Added unix system type to object directories
# Added support for Linux shared libraries
#
# Revision 1.11  1996/06/19 01:56:26  craigs
# Added uhttpsvc to the list
#
# Revision 1.10  1996/06/14 09:30:43  craigs
# Included buildinc stuff
#
# Revision 1.9  1996/05/10 11:03:22  craigs
# Fixed distsrc rule to only include ptlib stuff
#
# Revision 1.8  1996/05/03 13:48:58  craigs
# Sun4 fixes
#
# Revision 1.7  1996/05/02  10:53:24  craigs
# Fixed problem with SUN4 target
#
# Revision 1.6  1996/04/17 11:12:20  craigs
# Latest version pre mibmaster release 1.0
#
# Revision 1.5  1996/04/15 10:50:13  craigs
# Last revision prior to release of MibMaster
#
# Revision 1.4  1996/01/26 11:09:04  craigs
# *** empty log message ***
#
# Revision 1.3  1995/12/08 13:18:13  craigs
# Lots of changes
#
# Revision 1.2  1995/07/09 00:35:42  craigs
# Latest and greatest omnibus change
#
# Revision 1.1  1995/01/16 20:50:44  craigs
# Initial revision
#
# Revision 1.1  1993/07/12  21:22:16  craigs
# Initial revision
#
#

include ../../../make/unix.mak

OBJDIR  = $(PT_OBJDIR)
LIBDIR  = $(PT_LIBDIR)
TARGET  = $(PTLIB_FILE)
VERSION_FILE = $(PTLIBDIR)/version.h

COMPONENT_SRC_DIR	= ../../ptclib
COMMON_SRC_DIR		= ../common
VPATH_CXX		:= $(COMMON_SRC_DIR) $(COMPONENT_SRC_DIR) $(PROTOCOLS_SRC_DIR)

# try and keep the order of modules in reverse order of usage so any
# global statics will be constructed/destructed in the correct order

GETDATE_SOURCE = $(COMMON_SRC_DIR)/getdate.tab.c

SOURCES	= $(PROTO_SOURCES) \
	  $(COMPONENT_SRC_DIR)/ipacl.cxx \
	  $(COMPONENT_SRC_DIR)/cypher.cxx \
	  $(COMPONENT_SRC_DIR)/socks.cxx \
	  $(COMPONENT_SRC_DIR)/asner.cxx \
	  $(COMPONENT_SRC_DIR)/random.cxx \
	  NucleusChannel.cxx \
	  NucleusConChan.cxx \
	  NucleusConfig.cxx \
	  NucleusMutex.cxx \
	  NucleusSoundBite.cxx \
	  NucleusPDirectory.cxx \
	  NucleusPFile.cxx \
	  NucleusPFilePath.cxx \
	  NucleusProcess.cxx \
	  NucleusPTime.cxx \
	  NucleusPtlib.cxx \
	  NucleusSemaphore.cxx \
	  NucleusSyncPoint.cxx \
	  NucleusThread.cxx \
	  NucleusTimer.cxx \
	  $(GETDATE_SOURCE) \
	  $(COMMON_SRC_DIR)/sockets.cxx \
	  socket.cxx \
	  $(COMMON_SRC_DIR)/ptime.cxx \
	  $(COMMON_SRC_DIR)/osutils.cxx \
	  assert.cxx \
	  $(COMMON_SRC_DIR)/collect.cxx \
	  $(COMMON_SRC_DIR)/contain.cxx \
	  $(COMMON_SRC_DIR)/object.cxx   # must be last module

OBJS	= $(OBJDIR)/regex.o


#######################################################

include ../../../make/common.mak

#######################################################

alllibs:
	$(MAKE) both
	$(MAKE) bothshared


ifdef REQUIRES_SEPARATE_SWITCH 
$(OBJDIR)/switch.o: ./switch.cxx
	@if [ ! -d $(OBJDIR) ] ; then mkdir $(OBJDIR) ; fi
	$(CPLUS) $(CFLAGS) $(STDCCFLAGS) -c $< -o $@
endif

CLEAN_FILES	:= $(CLEAN_FILES) $(PTLIB_FILE) $(GETDATE_SOURCE)

$(OBJDIR)/regex.o:	../../../src/ptlib/common/regex.cxx
	$(CPLUS) -DREGEX_MALLOC -D__STDC__ -DSTDC_HEADERS $(CFLAGS) $(STDCCFLAGS) -c -w $< -o $@

$(OBJDIR)/getdate.tab.o: $(GETDATE_SOURCE)
	$(CC) $(CFLAGS) $(STDCCFLAGS) -c $< -o $@

$(DEPDIR)/getdate.tab.dep: $(GETDATE_SOURCE)
	$(CC) $(STDCCFLAGS) -M $< >> $@

ifeq ($(OSTYPE),beos)
$(OBJDIR)/resample.o: $(BEAUDIO_SRC_DIR)/resample.c
	$(CC) $(CFLAGS) $(STDCCFLAGS) -c $< -o $@
endif

$(GETDATE_SOURCE): $(COMMON_SRC_DIR)/getdate.y
	bison $(COMMON_SRC_DIR)/getdate.y

ifdef SHAREDLIB

$(PTLIB_FILE): $(PTLIB_FILE).1
	@if [ ! -e $(PTLIB_FILE) ] ; then ln -s $(PTLIB_FILE).1 $(PTLIB_FILE) ; fi

$(PTLIB_FILE).1: $(PTLIB_FILE).1.0
	@if [ ! -e $(PTLIB_FILE).1 ] ; then ln -s $(PTLIB_FILE).1.0 $(PTLIB_FILE).1 ; fi

$(PTLIB_FILE).1.0: $(OBJS)
	@if [ ! -d $(LIBDIR) ] ; then mkdir $(LIBDIR) ; fi
	gcc -shared -Wl,-soname,$(PTLIB_FILE).1 -o $(PTLIB_FILE).1.0 $(OBJS)

else

$(PTLIB_FILE): $(OBJS)
	@if [ ! -d $(LIBDIR) ] ; then mkdir $(LIBDIR) ; fi
ifdef RANLIB
	ar rc $(PTLIB_FILE) $(OBJS)
	ranlib $(PTLIB_FILE)
else
	ar rcs $(PTLIB_FILE) $(OBJS)
endif

endif


###########################################################

# make links to common directories and files to ease development work

links:
	ln -snf $(LIBDIR)/obj_$(PLATFORM_TYPE)_d obj_$(PLATFORM_TYPE)_d
	ln -snf $(LIBDIR)/obj_$(PLATFORM_TYPE)_r obj_$(PLATFORM_TYPE)_r
	ln -snf $(PTLIBDIR)/make make
	ln -snf $(UNIX_INC_DIR)/ptlib unix_include
	ln -snf $(PTLIBDIR)/include/ptlib common_include


# End of Makefile
