#
# unix.mak
#
# First part of make rules, included in ptlib.mak and pwlib.mak.
# Note: Do not put any targets in the file. This should defaine variables
#       only, as targets are all in common.mak
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
# $Log: unix.mak,v $
# Revision 1.203  2007/08/06 00:36:37  csoutheren
# NDEBUG is passed to opt builds, _DEBUG is passed to debug builds
#
# Revision 1.202  2006/06/27 11:52:14  csoutheren
# Patch 1509205 - Fixes typo in GCC flags under Solaris
# Thanks to Boris Pavacic
#
# Revision 1.201  2006/05/15 23:11:26  dereksmithies
# Use -ggdb -O0 -g3 to provide more information to the debugger, and force all
#  optimisation to be off. Initial tests show: this improves linux debugging.
#
# Revision 1.200  2006/01/17 18:57:17  dsandras
# Applied patch from Brian Lu <brian lu sun com> to fix compilation with SUN
# studio. Thanks.
#
# Revision 1.199  2006/01/08 14:49:08  dsandras
# Several fixes to allow compilation on Open Solaris thanks to Brian Lu <brian.lu _AT_____ sun.com>. Many thanks!
#
# Revision 1.198  2005/12/04 22:50:50  csoutheren
# Applied patch for Alpha thanks to Kilian Krause
#
# Revision 1.197  2005/12/04 22:38:38  csoutheren
# Added patch for hppa64. Thanks to Kilian Krause
#
# Revision 1.196  2005/09/25 10:51:23  dominance
# almost complete the mingw support. We'll be there soon. ;)
#
# Revision 1.195  2005/02/23 21:29:52  dominance
# have configure check for bison as we know we'll need it and stop implicit definition of PWLIBDIR. *geesh* that was about time, eh? ;)
#
# Revision 1.194  2005/01/14 11:49:17  csoutheren
# Removed -s flag so executables are not stripped by default
#
# Revision 1.193  2004/11/16 00:32:34  csoutheren
# Added Cygwin support
#
# Revision 1.192  2004/10/28 20:07:10  csoutheren
# Fixes for MacOSX platforms, thanks to Hannes Friederich
#
# Revision 1.191  2004/08/31 23:52:31  csoutheren
# Changed check for Mac OS version, thanks to Hannes Friederich
#
# Revision 1.190  2004/06/18 00:38:08  csoutheren
# Added check for FD_SETSIZE as suggested by Joegen E. Baclor
#
# Revision 1.189  2004/06/10 01:36:44  csoutheren
# Fixed problems with static links
#
# Revision 1.188  2004/05/30 04:49:42  ykiryanov
# Streamlined BeOS section
#
# Revision 1.187  2004/04/25 22:11:34  ykiryanov
# Added posix library to make files
#
# Revision 1.186  2004/04/21 03:47:36  ykiryanov
# Added libdl.so to library list for BeOS
#
# Revision 1.185  2004/04/12 03:35:27  csoutheren
# Fixed problems with non-recursuve mutexes and critical sections on
# older compilers and libc
#
# Revision 1.184  2004/02/26 11:19:07  csoutheren
# Added changes for BeOS, thanks to Yuri Kiryanov
# Added changes to fix link problems on some platforms, thanks to Klaus Kaempf
#
# Revision 1.183  2004/02/21 19:44:54  ykiryanov
# Fixed make parameters for BeOS: Changed default on BeOS to using Be BONE
#
# Revision 1.182  2004/02/11 05:09:14  csoutheren
# Fixed problems with regex libraries on Solaris, and with host OS numbering
# being a quoted string rather than a number. Thanks to Chad Attermann
# Fixed problems SSL detection problems thanks to Michal Zygmuntowicz
#
# Revision 1.181  2004/02/09 06:24:37  csoutheren
# Allowed CXX environment variable to define C++ compiler to use
# as required by configure
#
# Revision 1.180  2004/01/29 13:43:59  csoutheren
# Moved some preprocessor symbols from the command line to include files
# Modified to set P_HAS_SEMAPHORES to 0 for Linux kernels >= 2.6
# Applied patches for Solaris thanks to Michal Zygmuntowicz
#
# Revision 1.179  2003/11/02 16:00:26  shawn
# Panther requires -lresolv
#
# Revision 1.178  2003/09/18 23:02:35  csoutheren
# Removed definition of PMEMORY_CHECK
#
# Revision 1.177  2003/09/17 01:18:03  csoutheren
# Removed recursive include file system and removed all references
# to deprecated coooperative threading support
#
# Revision 1.176  2003/09/08 21:11:09  dereksmithies
# Remove hardcoded path from make file. Thanks Damien Sandras.
#
# Revision 1.175  2003/07/24 22:01:42  dereksmithies
# Add fixes from Peter Nixon  for fixing install problems. Thanks.
#
# Revision 1.174  2003/06/18 13:19:01  csoutheren
# Default debug builds now shared
#
# Revision 1.173  2003/06/17 12:05:01  csoutheren
# Changed compiler flags for optimised build
#
# Revision 1.172  2003/05/22 12:17:06  dsandras
#
# Removed unneeded code since Firewire support has been moved to configure.
#
# Revision 1.171  2003/05/06 09:47:20  robertj
# Fixed up MacOSX changes so is compatible with previous "API" not requiring
#   downstream libraries to change
#
# Revision 1.170  2003/05/06 06:59:12  robertj
# Dynamic library support for MacOSX, thanks Hugo Santos
#
# Revision 1.169  2003/05/05 13:10:59  robertj
# Solaris compatibility
#
# Revision 1.168  2003/04/17 07:29:27  robertj
# Fixed solaris link problem
#
# Revision 1.167  2003/04/17 00:05:04  craigs
# Added patches from Hugo Santos mainly for Darwin compatibility
#
# Revision 1.166  2003/04/16 07:16:55  craigs
# Modified for new autoconf based configuration
#
#
# Log truncated by CRS 14 April 2003
#

ifndef PTLIBDIR
ifdef PWLIBDIR
PTLIBDIR := $(PWLIBDIR)
else
	echo "No PTLIBDIR environment variable defined!"
	echo "You need to define PTLIBDIR!"
	echo "Try something like:"
	echo "PTLIBDIR = $(HOME)/ptlib"
	exit 1
endif
endif

####################################################

# include generated build options file, then include it
include $(PTLIBDIR)/make/ptbuildopts.mak

###############################################################################
#
#  this section used to normalise the machine name and OS type
#  this is now done by autoconf - but this left here as a reference
#
#

ifdef	USE_OLD_MACHINE_CHECKING

ifndef OSTYPE
OSTYPE := $(shell uname -s)
endif

ifndef MACHTYPE
MACHTYPE := $(shell uname -m)
endif

ifneq (,$(findstring linux,$(HOSTTYPE)))
ifneq (,$(findstring $(HOSTTYPE),i386-linux i486-linux))
OSTYPE   := linux
MACHTYPE := x86
endif
endif

ifeq ($(OSTYPE),mklinux)
OSTYPE   := linux
MACHTYPE := ppc
endif

ifneq (,$(findstring $(OSTYPE),Linux linux-gnu))
OSTYPE := linux
endif

ifneq (,$(findstring $(OSTYPE),Solaris SunOS))
OSTYPE := solaris
endif

ifneq (,$(findstring $(OSTYPE),IRIX))
OSTYPE := irix
endif

#Convert bash shell OSTYPE of 'freebsd3.4' to 'FreeBSD'
ifneq (,$(findstring freebsd,$(OSTYPE)))
OSTYPE := FreeBSD
endif

#Convert bash shell OSTYPE of 'openbsd2.6' to 'OpenBSD'
ifneq (,$(findstring openbsd,$(OSTYPE)))
OSTYPE := OpenBSD
endif

ifneq (,$(findstring macos,$(OSTYPE)))
OSTYPE := Darwin
endif

ifneq (,$(findstring darwin,$(OSTYPE)))
OSTYPE := Darwin
endif

ifneq (,$(findstring AIX,$(OSTYPE)))
MACHTYPE := ppc
endif

ifneq (,$(findstring $(OS),VXWORKS))
OSTYPE := VxWorks
endif

ifneq (,$(findstring netbsd,$(OSTYPE)))
OSTYPE := NetBSD
endif

ifneq (,$(findstring sparc, $(MACHTYPE)))
MACHTYPE := sparc
endif

ifneq (,$(findstring sun4, $(MACHTYPE)))
MACHTYPE := sparc
endif

ifneq (,$(findstring i86, $(MACHTYPE)))
MACHTYPE := x86
endif

ifneq (,$(findstring i386, $(MACHTYPE)))
MACHTYPE := x86
endif

ifneq (,$(findstring i486, $(MACHTYPE)))
MACHTYPE := x86
POSSIBLE_CPUTYPE := i486
endif

ifneq (,$(findstring i586, $(MACHTYPE)))
MACHTYPE := x86
POSSIBLE_CPUTYPE := i586
endif

ifneq (,$(findstring i686, $(MACHTYPE)))
MACHTYPE := x86
POSSIBLE_CPUTYPE := i686
endif

#make sure x86 does not match x86_64 by mistake
ifneq (,$(findstring x86, $(MACHTYPE)))
ifneq (,$(findstring x86_64, $(MACHTYPE)))
MACHTYPE := x86_64
else
MACHTYPE := x86
endif
endif

ifneq (,$(findstring powerpc, $(MACHTYPE)))
MACHTYPE := ppc
endif

ifneq (,$(findstring ppc, $(MACHTYPE)))
ifneq (,$(findstring ppc64, $(MACHTYPE)))
MACHTYPE := ppc64
else
MACHTYPE := ppc
endif
endif

ifneq (,$(findstring Power, $(MACHTYPE)))
MACHTYPE := ppc
endif

ifneq (,$(findstring mips, $(MACHTYPE)))
MACHTYPE := mips
endif

ifneq (,$(findstring alpha, $(MACHTYPE)))
MACHTYPE := alpha
endif

ifneq (,$(findstring sparc, $(MACHTYPE)))
MACHTYPE := sparc
endif

ifneq (,$(findstring ia64, $(MACHTYPE)))
MACHTYPE := ia64
endif

ifneq (,$(findsting hppa64, $(MACHTYPE)))
MACHTYPE := hppa64
endif

ifneq (,$(findstring s390, $(MACHTYPE)))
ifneq (,$(findstring s390x, $(MACHTYPE)))
MACHTYPE := s390x
else
MACHTYPE := s390
endif
endif

ifneq (,$(findstring armv4l, $(MACHTYPE)))
MACHTYPE := armv4l
endif
ifndef CPUTYPE
CPUTYPE := $(POSSIBLE_CPUTYPE)
export CPUTYPE
endif


endif  # USE_OLD_MACHINE_CHECKING

STANDARD_TARGETS=\
opt         debug         both \
optdepend   debugdepend   bothdepend \
optshared   debugshared   bothshared \
optnoshared debugnoshared bothnoshared \
optclean    debugclean    clean \
release tagbuild

.PHONY: all $(STANDARD_TARGETS)


ifeq (,$(findstring $(OSTYPE),linux FreeBSD OpenBSD NetBSD solaris beos Darwin Carbon AIX Nucleus VxWorks rtems QNX cygwin mingw))

default_target :
	@echo
	@echo ######################################################################
	@echo "Warning: OSTYPE=$(OSTYPE) support has not been confirmed.  This may"
	@echo "         be a new operating system not yet encountered, or more"
	@echo "         likely, the OSTYPE and MACHTYPE environment variables are"
	@echo "         set to unusual values. You may need to explicitly set these"
	@echo "         variables for the correct operation of this system."
	@echo
	@echo "         Currently supported OSTYPE names are:"
	@echo "              linux Linux linux-gnu mklinux"
	@echo "              solaris Solaris SunOS"
	@echo "              FreeBSD OpenBSD NetBSD beos Darwin Carbon"
	@echo "              VxWorks rtems mingw"
	@echo
	@echo "              **********************************"
	@echo "              *** DO NOT IGNORE THIS MESSAGE ***"
	@echo "              **********************************"
	@echo
	@echo "         The system almost certainly will not compile! When you get"
	@echo "         it working please send patches to support@equival.com.au"
	@echo ######################################################################
	@echo

$(STANDARD_TARGETS) :: default_target

else

default_target : help

endif

####################################################

# Set default for shared library usage

ifndef P_SHAREDLIB
P_SHAREDLIB=1
else
P_SHAREDLIB=0
endif

# -Wall must be at the start of the options otherwise
# any -W overrides won't have any effect
ifeq ($(USE_GCC),yes)
STDCCFLAGS += -Wall 
endif

ifdef RPM_OPT_FLAGS
STDCCFLAGS	+= $(RPM_OPT_FLAGS)
endif

ifneq ($(OSTYPE),rtems)
ifndef SYSINCDIR
SYSINCDIR := /usr/include
endif
endif

####################################################

ifeq ($(OSTYPE),FreeBSD)

ifeq ($(MACHTYPE),x86_64)
STDCCFLAGS     += -DP_64BIT
endif
ifeq ($(MACHTYPE),amd64)
STDCCFLAGS     += -DP_64BIT
endif

ifeq ($(P_SHAREDLIB),1)
ifndef PROG
STDCCFLAGS	+= -fPIC -DPIC
endif # PROG
endif # P_SHAREDLIB

STATIC_LIBS	:= libstdc++.a libg++.a libm.a libc.a
ifdef PREFIX
SYSLIBDIR	:= $(PREFIX)/lib
else
SYSLIBDIR	:= /usr/local/lib
endif

endif

####################################################

ifeq ($(OSTYPE),linux)

ifeq ($(MACHTYPE),x86)
ifdef CPUTYPE
ifeq ($(CPUTYPE),crusoe)
STDCCFLAGS	+= -fomit-frame-pointer -fno-strict-aliasing -fno-common -pipe -mpreferred-stack-boundary=2 -march=i686 -malign-functions=0 
STDCCFLAGS      += -malign-jumps=0 -malign-loops=0
else
STDCCFLAGS	+= -mcpu=$(CPUTYPE)
endif
endif
endif

ifeq ($(MACHTYPE),ia64)
STDCCFLAGS     += -DP_64BIT
endif

ifeq ($(MACHTYPE),hppa64)
STDCCFLAGS     += -DP_64BIT
endif

ifeq ($(MACHTYPE),s390x)
STDCCFLAGS     += -DP_64BIT
endif

ifeq ($(MACHTYPE),x86_64)
STDCCFLAGS     += -DP_64BIT
LDLIBS		+= -lresolv
endif

ifeq ($(MACHTYPE),amd64)
STDCCFLAGS     += -DP_64BIT
endif

ifeq ($(MACHTYPE),ppc64)
STDCCFLAGS     += -DP_64BIT
endif

ifeq ($(P_SHAREDLIB),1)
ifndef PROG
STDCCFLAGS	+= -fPIC -DPIC
endif # PROG
endif # P_SHAREDLIB


STATIC_LIBS	:= libstdc++.a libg++.a libm.a libc.a
ifdef PREFIX
SYSLIBDIR	:= $(PREFIX)/lib
else
SYSLIBDIR	:= /usr/local/lib
endif
#SYSLIBDIR	:= $(shell $(PTLIBDIR)/make/ptlib-config --libdir)

endif # linux


####################################################

ifeq ($(OSTYPE),FreeBSD)

ifeq ($(MACHTYPE),x86)
ifdef CPUTYPE
STDCCFLAGS	+= -mcpu=$(CPUTYPE)
endif
endif

P_USE_RANLIB		:= 1
#STDCCFLAGS      += -DP_USE_PRAGMA		# migrated to configure
SYSLIBDIR	:= /usr/local/lib


endif # FreeBSD


####################################################

ifeq ($(OSTYPE),OpenBSD)

ifeq ($(MACHTYPE),x86)
STDCCFLAGS	+= -m486
endif

LDLIBS		+= -lossaudio

P_USE_RANLIB		:= 1
#STDCCFLAGS      += -DP_USE_PRAGMA		# migrated to configure


endif # OpenBSD


####################################################

ifeq ($(OSTYPE),NetBSD)

ifeq ($(MACHTYPE),x86)
STDCCFLAGS	+= -m486
endif

LDLIBS		+= -lossaudio

STDCCFLAGS += -I$(UNIX_INC_DIR) -I$(PTLIBDIR)/include

# enable the USE_PTH line to compile using pth
# enable the USE_NATIVE_THREADS line to compile using native threads
# enable the USE_UNPROVEN_THREADS line to compile using unproven threads
#USE_PTH_THREADS := 1
#USE_UNPROVEN_THREADS := 1
USE_NATIVE_THREADS := 1

ifdef P_PTHREADS
ifdef USE_NATIVE_THREADS
LDLIBS  += -lpthread
else
ifdef USE_PTH_THREADS
STDCCFLAGS += -DP_GNU_PTH
STDCCFLAGS += -I/usr/pkg/include
LDFLAGS += -L/usr/pkg/lib
LDLIBS  += -lpthread
else
STDCCFLAGS += -DP_NO_CANCEL
STDCCFLAGS += -I/usr/pkg/pthreads/include
LDFLAGS	+= -L/usr/pkg/pthreads/lib
LDLIBS	+= -lpthread
CC              := /usr/pkg/pthreads/bin/pgcc
CPLUS           := /usr/pkg/pthreads/bin/pg++
endif
endif
endif

P_USE_RANLIB		:= 1
#STDCCFLAGS      += -DP_USE_PRAGMA		# migrated to configure


endif # NetBSD


####################################################

ifeq ($(OSTYPE),AIX)

STDCCFLAGS	+= -DP_AIX  
# -pedantic -g
# LDLIBS	+= -lossaudio

STDCCFLAGS	+= -mminimal-toc

#P_USE_RANLIB		:= 1
STDCCFLAGS      += -DP_USE_PRAGMA


endif # AIX


####################################################

ifeq ($(OSTYPE),sunos)

# Sparc Sun 4x, using gcc 2.7.2

P_USE_RANLIB		:= 1
REQUIRES_SEPARATE_SWITCH = 1
#STDCCFLAGS      += -DP_USE_PRAGMA	# migrated to configure

endif # sunos


####################################################

ifeq ($(OSTYPE),solaris)

#  Solaris (Sunos 5.x)

CFLAGS +=-DSOLARIS
CXXFLAGS +=-DSOLARIS

ifeq ($(MACHTYPE),x86)
ifeq ($(USE_GCC),yes)
DEBUG_FLAG	:= -gstabs+
CFLAGS           += -DUSE_GCC
CXXFLAGS         += -DUSE_GCC
endif
endif

ENDLDLIBS	+= -lsocket -lnsl -ldl -lposix4

# Sparc Solaris 2.x, using gcc 2.x
#Brian CC		:= gcc

#P_USE_RANLIB		:= 1

#STDCCFLAGS      += -DP_USE_PRAGMA	# migrated to configure

STATIC_LIBS	:= libstdc++.a libg++.a 
SYSLIBDIR	:= /opt/openh323/lib

# Rest added by jpd@louisiana.edu, to get .so libs created!
ifndef DEBUG
ifndef P_SHAREDLIB
P_SHAREDLIB=1
ifndef PROG
STDCCFLAGS	+= -fPIC -DPIC
endif # PROG
endif
endif

endif # solaris

####################################################

ifeq ($(OSTYPE),irix)

#  should work whith Irix 6.5

# IRIX using a gcc
CC		:= gcc
STDCCFLAGS	+= -DP_IRIX
LDLIBS		+= -lsocket -lnsl

STDCCFLAGS      += -DP_USE_PRAGMA

endif # irix


####################################################

ifeq ($(OSTYPE),beos)

SYSLIBS     += -lbe -lmedia -lgame -lroot -lsocket -lbind -ldl 
STDCCFLAGS	+= -DBE_THREADS -DP_USE_PRAGMA -Wno-multichar -Wno-format
LDLIBS		+= $(SYSLIBS)

MEMORY_CHECK := 0
endif # beos


####################################################

ifeq ($(OSTYPE),ultrix)

# R2000 Ultrix 4.2, using gcc 2.7.x
STDCCFLAGS	+= -DP_ULTRIX
STDCCFLAGS      += -DP_USE_PRAGMA
endif # ultrix


####################################################

ifeq ($(OSTYPE),hpux)
STDCCFLAGS      += -DP_USE_PRAGMA
# HP/UX 9.x, using gcc 2.6.C3 (Cygnus version)
STDCCFLAGS	+= -DP_HPUX9

endif # hpux


####################################################
 
ifeq ($(OSTYPE),Darwin)
 
# MacOS X or later / Darwin

CFLAGS		+= -fno-common -dynamic
LDFLAGS		+= -multiply_defined suppress
ENDLDLIBS	+= -lresolv -framework AudioToolbox -framework CoreAudio
ENDLDLIBS   += -framework AudioUnit -framework CoreServices
STDCCFLAGS  += -D__MACOSX__

# Quicktime support is still a long way off. But for development purposes,
# I am inluding the flags to allow QuickTime to be linked.
# Uncomment them if you wish, but it will do nothing for the time being.

#HAS_QUICKTIMEX := 1
#STDCCFLAGS     += -DHAS_QUICKTIMEX
#ENDLDLIBS      += -framework QuickTime
 
ifeq ($(MACHTYPE),x86)
STDCCFLAGS	+= -m486
endif

ARCHIVE			:= libtool -static -o
P_USE_RANLIB	:= 0

CC              := cc
CPLUS           := c++
 
endif # Darwin

ifeq ($(OSTYPE),Carbon)

# MacOS 9 or X using Carbonlib calls

STDCCFLAGS	+= -DP_MACOS

# I'm having no end of trouble with the debug memory allocator.
MEMORY_CHECK    := 0

# Carbon is only available for full Mac OS X, not pure Darwin, so the only
# currently available architecture is PPC.
P_MAC_MPTHREADS := 1
STDCCFLAGS	+= -DP_MAC_MPTHREADS
LDLIBS		+= -prebind -framework CoreServices -framework QuickTime -framework Carbon
  
P_SHAREDLIB	:= 0 
P_USE_RANLIB	:= 1

CC              := cc
CPLUS           := c++
 
endif # Carbon

####################################################

ifeq ($(OSTYPE),VxWorks)

ifeq ($(MACHTYPE),ARM)
STDCCFLAGS	+= -mcpu=arm8 -DCPU=ARMARCH4
endif

STDCCFLAGS	+= -DP_VXWORKS -DPHAS_TEMPLATES -DVX_TASKS
STDCCFLAGS	+= -DNO_LONG_DOUBLE

STDCCFLAGS	+= -Wno-multichar -Wno-format

MEMORY_CHECK := 0

STDCCFLAGS      += -DP_USE_PRAGMA

endif # VxWorks

 
####################################################

ifeq ($(OSTYPE),rtems)

CC              := $(MACHTYPE)-rtems-gcc --pipe
CPLUS           := $(MACHTYPE)-rtems-g++
#LD              := $(MACHTYPE)-rtems-ld
#AR              := $(MACHTYPE)-rtems-ar
#RUNLIB          := $(MACHTYPE)-rtems-runlib

SYSLIBDIR	:= $(RTEMS_MAKEFILE_PATH)/lib
SYSINCDIR	:= $(RTEMS_MAKEFILE_PATH)/lib/include

LDFLAGS		+= -B$(SYSLIBDIR)/ -specs=bsp_specs -qrtems
STDCCFLAGS	+= -B$(SYSLIBDIR)/ -specs=bsp_specs -ansi -fasm -qrtems

ifeq ($(CPUTYPE),mcpu32)
STDCCFLAGS	+= -mcpu32
LDFLAGS		+= -mcpu32 
endif

ifeq ($(CPUTYPE),mpc860)
STDCCFLAGS	+= -mcpu=860
LDFLAGS		+= -mcpu=860
endif

STDCCFLAGS	+= -DP_RTEMS -DP_HAS_SEMAPHORES

P_SHAREDLIB	:= 0

endif # rtems

####################################################

ifeq ($(OSTYPE),QNX)

ifeq ($(MACHTYPE),x86)
STDCCFLAGS	+= -Wc,-m486
endif

STDCCFLAGS	+= -DP_QNX -DP_HAS_RECURSIVE_MUTEX=1 -DFD_SETSIZE=1024
LDLIBS		+= -lasound
ENDLDLIBS       += -lsocket -lstdc++

CC		:= qcc -Vgcc_ntox86
CPLUS		:= qcc -Vgcc_ntox86_gpp

P_USE_RANLIB	:= 1
STDCCFLAGS      += -DP_USE_PRAGMA

ifeq ($(P_SHAREDLIB),1)
ifeq ($(USE_GCC),yes)
STDCCFLAGS      += -shared
else
ifeq ($(OSTYPE),solaris)
STDCCFLAGS      += -G
endif
endif

endif

endif # QNX6

####################################################

ifeq ($(OSTYPE),Nucleus)

# Nucleus using gcc
STDCCFLAGS	+= -msoft-float -nostdinc $(DEBUGFLAGS)
STDCCFLAGS	+= -D__NUCLEUS_PLUS__ -D__ppc -DWOT_NO_FILESYSTEM -DPLUS \
		   -D__HAS_NO_FLOAT -D__USE_STL__ \
                   -D__USE_STD__ \
		   -D__NUCLEUS_NET__ -D__NEWLIB__ \
		   -DP_USE_INLINES=0
ifndef WORK
WORK		= ${HOME}/work
endif
ifndef NUCLEUSDIR
NUCLEUSDIR	= ${WORK}/embedded/os/Nucleus
endif
ifndef STLDIR
STLDIR		= ${WORK}/embedded/packages/stl-3.2-stream
endif
STDCCFLAGS	+= -I$(NUCLEUSDIR)/plus \
		-I$(NUCLEUSDIR)/plusplus \
		-I$(NUCLEUSDIR)/net \
		-I$(NUCLEUSDIR) \
		-I$(PTLIBDIR)/include/ptlib/Nucleus++ \
		-I$(WORK)/embedded/libraries/socketshim/BerkleySockets \
		-I${STLDIR} \
		-I/usr/local/powerpc-motorola-eabi/include \
		-I${WORK}/embedded/libraries/configuration

UNIX_SRC_DIR	= $(PTLIBDIR)/src/ptlib/Nucleus++
MEMORY_CHECK	=	0
endif # Nucleus

####################################################

ifeq ($(OSTYPE),mingw)
LDFLAGS += -enable-auto-import -enable-runtime-pseudo-reloc
LDFLAGS += -enable-stdcall-fixup -fatal-warning
endif # mingw

###############################################################################
#
# Make sure some things are defined
#

ifndef	CC
CC := gcc
endif

ifndef CPLUS
ifndef CXX
CPLUS := g++
else
CPLUS := $(CXX)
endif
endif

ifndef INSTALL
INSTALL := install
endif

ifndef AR
AR := ar
endif

ifndef ARCHIVE
  ifdef P_USE_RANLIB
    ARCHIVE := $(AR) rc
  else
    ARCHIVE := $(AR) rcs
  endif
endif

ifndef RANLIB
RANLIB := ranlib
endif


# Further configuration

ifndef DEBUG_FLAG
DEBUG_FLAG	:=  -g
endif

ifndef PTLIB_ALT
PLATFORM_TYPE = $(OSTYPE)_$(MACHTYPE)
else
PLATFORM_TYPE = $(OSTYPE)_$(PTLIB_ALT)_$(MACHTYPE)
endif

ifndef OBJ_SUFFIX
ifdef	DEBUG
OBJ_SUFFIX	:= d
else
OBJ_SUFFIX	:= r
endif # DEBUG
endif # OBJ_SUFFIX

ifndef OBJDIR_SUFFIX
OBJDIR_SUFFIX = $(OBJ_SUFFIX)
endif

ifndef STATICLIBEXT
STATICLIBEXT = a
endif

ifeq ($(P_SHAREDLIB),1)
LIB_SUFFIX	= $(SHAREDLIBEXT)
LIB_TYPE	=
else   
LIB_SUFFIX	= a 
LIB_TYPE	= _s
endif # P_SHAREDLIB

ifndef INSTALL_DIR
INSTALL_DIR	= /usr/local
endif

ifndef INSTALLBIN_DIR
INSTALLBIN_DIR	= $(INSTALL_DIR)/bin
endif

ifndef INSTALLLIB_DIR
INSTALLLIB_DIR	= $(INSTALL_DIR)/lib
endif


###############################################################################
#
# define some common stuff
#

SHELL		:= /bin/sh

.SUFFIXES:	.cxx .prc 

# Required macro symbols

# Directories

ifdef PREFIX
UNIX_INC_DIR	= $(PREFIX)/include/ptlib/unix
else
UNIX_INC_DIR	= $(PTLIBDIR)/include/ptlib/unix
endif

ifndef UNIX_SRC_DIR
UNIX_SRC_DIR	= $(PTLIBDIR)/src/ptlib/unix
endif

PT_LIBDIR	= $(PTLIBDIR)/lib

# set name of the PT library
PTLIB_BASE	= pt_$(PLATFORM_TYPE)_$(OBJ_SUFFIX)
PTLIB_FILE	= lib$(PTLIB_BASE)$(LIB_TYPE).$(LIB_SUFFIX)
PT_OBJBASE	= obj_$(PLATFORM_TYPE)_$(OBJDIR_SUFFIX)
PT_OBJDIR	= $(PT_LIBDIR)/$(PT_OBJBASE)

###############################################################################
#
# Set up compiler flags and macros for debug/release versions
#

ifdef	DEBUG

ifndef MEMORY_CHECK
MEMORY_CHECK := 1
endif

STDCCFLAGS	+= $(DEBUG_FLAG) -D_DEBUG
LDFLAGS		+= $(DEBLDFLAGS)

else

STDCCFLAGS	+= -DNDEBUG

ifneq ($(OSTYPE),Darwin)
  ifeq ($(OSTYPE),solaris)
    OPTCCFLAGS	+= -O3 
  else
    OPTCCFLAGS	+= -Os 
  endif
else
  OPTCCFLAGS	+= -O2
endif

endif # DEBUG

# define ESDDIR variables if installed
ifdef  ESDDIR
STDCCFLAGS	+= -I$(ESDDIR)/include -DUSE_ESD=1
ENDLDLIBS	+= $(ESDDIR)/lib/libesd.a  # to avoid name conflicts
HAS_ESD		= 1
endif

# feature migrated to configure.in
# #define templates if available
# ifndef NO_PTLIB_TEMPLATES
# STDCCFLAGS	+= -DPHAS_TEMPLATES
# endif

# compiler flags for all modes
#STDCCFLAGS	+= -fomit-frame-pointer
#STDCCFLAGS	+= -fno-default-inline
#STDCCFLAGS     += -Woverloaded-virtual
#STDCCFLAGS     += -fno-implement-inlines

# add OS directory to include path
# STDCCFLAGS	+= -I$(UNIX_INC_DIR)  # removed CRS


# add library directory to library path and include the library
LDFLAGS		+= -L$(PT_LIBDIR)

LDLIBS		+= -l$(PTLIB_BASE)$(LIB_TYPE)

# End of unix.mak
