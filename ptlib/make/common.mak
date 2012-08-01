#
# common.mak
#
# Common make rules included in ptlib.mak and pwlib.mak
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
# $Log: common.mak,v $
# Revision 1.99  2007/05/16 09:17:15  csoutheren
# Removed vestiges of precompiled headers in Makefiles
#
# Revision 1.98  2007/05/01 10:18:07  csoutheren
# Disable precompiled headers because dammit, they don't work
#
# Revision 1.97  2007/04/19 07:34:13  csoutheren
# Added option to disable precompiled headers
#
# Revision 1.96  2007/04/18 23:49:50  csoutheren
# Add usage of precompiled headers
#
# Revision 1.95  2006/01/08 14:49:08  dsandras
# Several fixes to allow compilation on Open Solaris thanks to Brian Lu <brian.lu _AT_____ sun.com>. Many thanks!
#
# Revision 1.94  2004/06/10 01:36:44  csoutheren
# Fixed problems with static links
#
# Revision 1.93  2004/04/29 14:07:07  csoutheren
# Changed install target to use $(INSTALL)
# Various makefile cleanups and verified install and uninstall targets
# Removed some vestiges of old GUI code
#
# Revision 1.92  2004/04/24 06:26:03  rjongbloed
# Fixed very bizarre problem with GNU GCC 3.4.0 and generating dependencies,
#   cannot have -g and -M on the command line at the same time!
#
# Revision 1.91  2004/04/21 12:05:04  csoutheren
# Added target to create gcc compiled header for ptlib.h, but not used
# because it does not seem to be able to compile some files
#
# Revision 1.90  2004/04/14 10:21:15  csoutheren
# Grrr...more changes for CXX flags
#
# Revision 1.89  2004/04/14 10:07:00  csoutheren
# Added CXXFLAGS
#
# Revision 1.88  2003/12/29 07:15:31  csoutheren
# Fixed problem with creating statically linked executables
#
# Revision 1.87  2003/11/02 15:57:56  shawn
# remove -static for Mac OS X
#
# Revision 1.86  2003/10/03 00:36:59  dereksmithies
# Fix generation of dependencies. Thanks to Vyacheslav Frolov
#
# Revision 1.85  2003/07/29 11:25:23  csoutheren
# Added support for system swab function
#
# Revision 1.84  2003/07/24 22:01:42  dereksmithies
# Add fixes from Peter Nixon  for fixing install problems. Thanks.
#
# Revision 1.83  2003/06/17 12:05:01  csoutheren
# Changed compiler flags for optimised build
#
# Revision 1.82  2003/02/03 06:08:52  robertj
# Changed tagbuild so leaves a cvs sticky version of sources to that tag.
#
# Revision 1.81  2002/11/15 06:36:59  robertj
# Changed so library build only occurs if source is present.
#
# Revision 1.80  2002/10/10 04:43:44  robertj
# VxWorks port, thanks Martijn Roest
#
# Revision 1.79  2002/08/30 11:41:33  robertj
# Added guarantee that version numbers exist.
#
# Revision 1.78  2002/08/30 02:23:13  craigs
# Added changes for correct creation of debug libraries
#    thanks to Martin Froehlich
#
# Revision 1.77  2002/06/25 04:13:07  robertj
# Added symbol NODEPS so dependency files are not included, very useful for
#   when they get corrupted so don;t have to manually delete them.
#
# Revision 1.76  2002/04/08 00:41:50  robertj
# Added dependency build to "make libs" target.
#
# Revision 1.75  2002/04/01 09:14:09  rogerh
# Add noshared build targets
#
# Revision 1.74  2002/02/25 16:23:16  rogerh
# Test for GCC 3 in unix.mak and not it common.mak so -DGCC3 can be set
#
# Revision 1.73  2002/02/19 06:26:29  rogerh
# Move $$BLD outside quotes so it gets expanded correctly
#
# Revision 1.72  2002/02/18 07:39:04  robertj
# Changed version number increment to more portable expr function.
#
# Revision 1.71  2002/01/28 10:33:20  rogerh
# use CPLUS instead of gcc. Makes it work on Mac OS X
#
# Revision 1.70  2002/01/28 00:19:59  craigs
# Made gcc 3 changes dependent on having gcc 3.0 installed
#
# Revision 1.69  2001/12/17 23:33:50  robertj
# Solaris 8 porting changes, thanks James Dugal
#
# Revision 1.68  2001/11/29 07:50:42  robertj
# Removed static, it doesn't work either
#
# Revision 1.67  2001/11/29 03:16:49  robertj
# Made make noshared completely static
#
# Revision 1.66  2001/11/28 02:44:14  robertj
# Fixed attempt to do static link on debug version, causes crashes.
#
# Revision 1.65  2001/11/27 22:42:13  robertj
# Changed to make system to better support non-shared library building.
#
# Revision 1.64  2001/10/31 00:45:20  robertj
# Added debuglibs, optlibs and bothlibs targets, moving help to where these
#   targets are in teh make file system.
#
# Revision 1.63  2001/10/12 07:25:33  robertj
# Removed diff test, cvs commit already does compare
#
# Revision 1.62  2001/10/12 06:18:41  robertj
# Fixed check of change of version file for tagbuild
#
# Revision 1.61  2001/10/09 08:53:26  robertj
# Added LIBDIRS variable so can go "make libs" and make all libraries.
# Added "make version" target to display version of project.
# Added inclusion of library versions into "make tagbuild" check in.
#
# Revision 1.60  2001/08/07 08:24:42  robertj
# Fixed bug in tagbuild if have more than one BUILD_NUMBER in file.
#
# Revision 1.59  2001/07/30 07:45:54  robertj
# Added "all" target with double colon.
#
# Revision 1.58  2001/07/27 14:39:12  robertj
# Allowed libs target to have multiple definitions
#
# Revision 1.57  2001/06/30 06:59:06  yurik
# Jac Goudsmit from Be submit these changes 6/28. Implemented by Yuri Kiryanov
#
# Revision 1.56  2001/06/29 06:47:30  robertj
# Added missing dollar sign
#
# Revision 1.55  2001/06/29 06:41:03  robertj
# Fixed make tagbuild for different #defines
#
# Revision 1.54  2001/05/29 03:31:48  craigs
# Removed BROKEN_GCC symbol, now that pwlib is totally gcc friendly
#
# Revision 1.53  2001/03/29 04:48:45  robertj
# Added tagbuild target to do CVS tag and autoincrement BUILD_NUMBER
# Changed order so version.h is used before custom.cxx
#
# Revision 1.52  2001/03/23 19:59:48  craigs
# Added detection of broken gcc versions
#
# Revision 1.51  2001/03/22 01:14:16  robertj
# Allowed for the version file #defines to configured by calling makefile.
#
# Revision 1.50  2000/11/02 04:46:42  craigs
# Added support for buildnum.h file for version numbers
#
# Revision 1.49  2000/10/01 01:08:10  craigs
# Fixed problems with Motif build
#
# Revision 1.48  2000/09/20 23:59:35  craigs
# Fixed problem with bothnoshared target
#
# Revision 1.47  2000/04/26 00:40:48  robertj
# Redesigned version number system to have single file to change on releases.
#
# Revision 1.46  2000/04/06 20:12:33  craigs
# Added install targets
#
# Revision 1.45  2000/03/20 23:08:31  craigs
# Added showgui target to allow displaying the GUI settings
#
# Revision 1.44  2000/03/20 22:43:09  craigs
# Added totally new mechanism for detecting GUI
#
# Revision 1.43  2000/03/03 00:37:42  robertj
# Fixed problem for when have GUI environment variable set, always builds GUI!
#
# Revision 1.42  2000/02/24 11:02:11  craigs
# Fixed problems with PW make
#
# Revision 1.41  2000/02/16 11:30:25  craigs
# Added rule to force library build for applications
#
# Revision 1.40  2000/02/04 19:33:25  craigs
# Added ability to create non-shared versions of programs
#
# Revision 1.39  2000/01/22 00:51:18  craigs
# Added ability to compile in any directory, and to create shared libs
#
# Revision 1.38  1999/07/10 03:32:02  robertj
# Improved release version detection code.
#
# Revision 1.37  1999/07/03 04:31:53  robertj
# Fixed problems with not including oss.cxx in library if OSTYPE not "linux"
#
# Revision 1.36  1999/07/02 05:10:33  robertj
# Fixed bug in changing from debug default to opt build
#
# Revision 1.35  1999/06/28 09:12:01  robertj
# Fixed problems with the order in which macros are defined especially on BeOS & Solaris
#
# Revision 1.34  1999/06/27 02:42:10  robertj
# Fixed BeOS compatability.
# Fixed error of platform name not supported, needed :: on main targets.
#
# Revision 1.33  1999/06/09 15:41:18  robertj
# Added better UI to make files.
#
# Revision 1.32  1999/04/18 09:36:31  robertj
# Get date grammar build.
#
# Revision 1.31  1999/02/19 11:32:10  robertj
# Improved the "release" target to build release tar files.
#
# Revision 1.30  1999/01/16 09:56:24  robertj
# Changed some macros to more informative names.
#
# Revision 1.29  1999/01/16 04:00:05  robertj
# Added bothclean target
#
# Revision 1.28  1998/12/02 02:36:57  robertj
# New directory structure.
#
# Revision 1.27  1998/11/26 12:48:20  robertj
# Support for .c files.
#
# Revision 1.26  1998/11/26 11:40:03  craigs
# Added checking for resource compilation
#
# Revision 1.25  1998/11/26 07:29:19  craigs
# Yet another bash at a GUI build environment
#
# Revision 1.24  1998/11/24 03:41:32  robertj
# Fixed problem where failed make depend leaves bad .dep files behind
#
# Revision 1.23  1998/11/22 10:41:02  craigs
# New GUI build system - for sure!
#
# Revision 1.22  1998/11/22 08:11:31  craigs
# *** empty log message ***
#
# Revision 1.21  1998/09/24 04:20:49  robertj
# Added open software license.
#

######################################################################
#
# common rules
#
######################################################################

# Submodules built with make lib
LIBDIRS += $(PTLIBDIR)


ifndef OBJDIR
OBJDIR	=	./$(PT_OBJBASE)
endif

vpath %.cxx $(VPATH_CXX)
vpath %.c   $(VPATH_C)
vpath %.o   $(OBJDIR)
vpath %.dep $(DEPDIR)
vpath %.gch $(PTLIBDIR)/include

#
# add common directory to include path - must be after PT directories
#
STDCCFLAGS	+= -I$(PTLIBDIR)/include

ifneq ($(P_SHAREDLIB),1)

#ifneq ($(OSTYPE),Darwin) # Mac OS X does not really support -static
#LDFLAGS += -static
#endif

ifneq ($(P_STATIC_LDFLAGS),)
LDFLAGS += $(P_STATIC_LDFLAGS)
endif

ifneq ($(P_STATIC_ENDLDLIBS),)
ENDLDLIBS += $(P_STATIC_ENDLDLIBS)
endif

endif

#  clean whitespace out of source file list
SOURCES         := $(strip $(SOURCES))

#
# define rule for .cxx and .c files
#
$(OBJDIR)/%.o : %.cxx 
	@if [ ! -d $(OBJDIR) ] ; then mkdir -p $(OBJDIR) ; fi
	$(CPLUS) $(STDCCFLAGS) $(OPTCCFLAGS) $(CFLAGS) $(STDCXXFLAGS) -c $< -o $@

$(OBJDIR)/%.o : %.c 
	@if [ ! -d $(OBJDIR) ] ; then mkdir -p $(OBJDIR) ; fi
	$(CC) $(STDCCFLAGS) $(OPTCCFLAGS) $(CFLAGS) -c $< -o $@

#
# create list of object files 
#
SRC_OBJS := $(SOURCES:.c=.o)
SRC_OBJS := $(SRC_OBJS:.cxx=.o)
OBJS	 := $(EXTERNALOBJS) $(patsubst %.o, $(OBJDIR)/%.o, $(notdir $(SRC_OBJS) $(OBJS)))

#
# create list of dependency files 
#
DEPDIR	 := $(OBJDIR)
SRC_DEPS := $(SOURCES:.c=.dep)
SRC_DEPS := $(SRC_DEPS:.cxx=.dep)
DEPS	 := $(patsubst %.dep, $(DEPDIR)/%.dep, $(notdir $(SRC_DEPS) $(DEPS)))

#
# define rule for .dep files
#
$(DEPDIR)/%.dep : %.cxx 
	@if [ ! -d $(DEPDIR) ] ; then mkdir -p $(DEPDIR) ; fi
	@printf %s $(OBJDIR)/ > $@
	$(CPLUS) $(STDCCFLAGS:-g=) -M $< >> $@

$(DEPDIR)/%.dep : %.c 
	@if [ ! -d $(DEPDIR) ] ; then mkdir -p $(DEPDIR) ; fi
	@printf %s $(OBJDIR)/ > $@
	$(CC) $(STDCCFLAGS:-g=) -M $< >> $@

#
# add in good files to delete
#
CLEAN_FILES += $(OBJS) $(DEPS) core

######################################################################
#
# rules for application
#
######################################################################

ifdef	PROG

ifndef TARGET
TARGET = $(OBJDIR)/$(PROG)
endif

ifdef BUILDFILES
OBJS += $(OBJDIR)/buildnum.o
endif

TARGET_LIBS	= $(PTLIBDIR)/lib/$(PTLIB_FILE)

# distinguish betweek building and using pwlib
ifeq (,$(wildcard $(PTLIBDIR)/src))
TARGET_LIBS     = $(SYSLIBDIR)/$(PTLIB_FILE)
endif

$(TARGET):	$(OBJS) $(TARGET_LIBS)
ifeq ($(OSTYPE),beos)
# BeOS won't find dynamic libraries unless they are in one of the system
# library directories or in the lib directory under the application's
# directory
	@if [ ! -L $(OBJDIR)/lib ] ; then cd $(OBJDIR); ln -s $(PT_LIBDIR) lib; fi
endif
	$(CPLUS) -o $@ $(CFLAGS) $(LDFLAGS) $(OBJS) $(LDLIBS) $(ENDLDLIBS) $(ENDLDFLAGS)
ifeq ($(OSTYPE),VxWorks)
	$(LD)   --split-by-reloc 65535 -r   $(OBJS) -o $@ 
endif

ifdef DEBUG

ifneq (,$(wildcard $(PTLIBDIR)/src/ptlib/unix))
$(PT_LIBDIR)/$(PTLIB_FILE):
	$(MAKE) -C $(PTLIBDIR)/src/ptlib/unix debug
endif

else

ifneq (,$(wildcard $(PTLIBDIR)/src/ptlib/unix))
$(PT_LIBDIR)/$(PTLIB_FILE):
	$(MAKE) -C $(PTLIBDIR)/src/ptlib/unix opt
endif

endif

CLEAN_FILES += $(TARGET)

ifndef INSTALL_OVERRIDE

install:	$(TARGET)
	$(INSTALL) $(TARGET) $(INSTALLBIN_DIR)
endif

# ifdef PROG
endif

$(PTLIBDIR)/include/ptlib.h.gch/$(PT_OBJBASE): $(PTLIBDIR)/include/ptlib.h
	@if [ ! -d `dirname $@` ] ; then mkdir -p `dirname $@` ; fi
	$(CPLUS) $(STDCCFLAGS) $(OPTCCFLAGS) $(CFLAGS) $(STDCXXFLAGS) -x c++ -c $< -o $@

PCH_FILES =	$(PTLIBDIR)/include/ptlib.h.gch/$(PT_OBJBASE)

#ifdef USE_PCH
#PCH:		$(PCH_FILES)
#
#CLEAN_FILES  += $(PCH_FILES)
#else
PCH:		
#endif


######################################################################
#
# Main targets for build management
#
######################################################################

all :: debuglibs debugdepend debug optlibs optdepend opt

help:
	@echo "The following targets are available:"
	@echo "  make debug         Make debug version of application"
	@echo "  make opt           Make optimised version of application"
	@echo "  make both          Make both versions of application"
	@echo
	@echo "  make debugnoshared Make static debug version of application"
	@echo "  make optnoshared   Make static optimised version of application"
	@echo "  make bothnoshared  Make static both versions of application"
	@echo
	@echo "  make debugclean    Remove debug files"
	@echo "  make optclean      Remove optimised files"
	@echo "  make clean         Remove both debug and optimised files"
	@echo
	@echo "  make debugdepend   Create debug dependency files"
	@echo "  make optdepend     Create optimised dependency files"
	@echo "  make bothdepend    Create both debug and optimised dependency files"
	@echo
	@echo "  make debuglibs     Make debug libraries project depends on"
	@echo "  make optlibs       Make optimised libraries project depends on"
	@echo "  make bothlibs      Make both debug and optimised libraries project depends on"
	@echo
	@echo "  make all           Create debug & optimised dependencies & libraries"
	@echo
	@echo "  make version       Display version for project"
	@echo "  make tagbuild      Do a CVS tag of the source, and bump build number"
	@echo "  make release       Package up optimised version int tar.gz file"


ifdef DEBUG

debug :: PCH $(TARGET) 

opt ::
	@$(MAKE) DEBUG= PCH opt

debugclean ::
	rm -rf $(CLEAN_FILES)

optclean ::
	@$(MAKE) DEBUG= optclean

.DELETE_ON_ERROR : debugdepend

debugdepend :: $(DEPS)
	@echo Created dependencies.

optdepend ::
	@$(MAKE) DEBUG= optdepend

debuglibs :: libs

optlibs ::
	@$(MAKE) DEBUG= libs

libs ::
	set -e; for i in $(LIBDIRS); do $(MAKE) -C $$i debugdepend debug; done

else

debug :: 
	@$(MAKE) DEBUG=1 PCH debug

opt :: PCH $(TARGET)

debugclean ::
	@$(MAKE) DEBUG=1 debugclean

optclean ::
	rm -rf $(CLEAN_FILES)

.DELETE_ON_ERROR : optdepend

debugdepend ::
	@$(MAKE) DEBUG=1 debugdepend

optdepend :: $(DEPS)
	@echo Created dependencies.

debuglibs ::
	@$(MAKE) DEBUG=1 libs

optlibs :: libs

libs ::
	set -e; for i in $(LIBDIRS); do $(MAKE) -C $$i optdepend opt; done

endif

both :: opt debug
clean :: optclean debugclean
bothdepend :: optdepend debugdepend
bothlibs :: optlibs debuglibs

optshared ::
	$(MAKE) P_SHAREDLIB=1 opt

debugshared ::
	$(MAKE) P_SHAREDLIB=1 debug

bothshared ::
	$(MAKE) optshared debugshared

optnoshared ::
	$(MAKE) P_SHAREDLIB=0 opt

debugnoshared ::
	$(MAKE) P_SHAREDLIB=0 debug

bothnoshared ::
	$(MAKE) optnoshared debugnoshared



######################################################################
#
# common rule to make a release of the program
#
######################################################################

# if have not explictly defined VERSION_FILE, locate a default

ifndef VERSION_FILE
  ifneq (,$(wildcard buildnum.h))
    VERSION_FILE := buildnum.h
  else
    ifneq (,$(wildcard version.h))
      VERSION_FILE := version.h
    else
      ifneq (,$(wildcard custom.cxx))
        VERSION_FILE := custom.cxx
      endif
    endif
  endif
endif


ifdef VERSION_FILE

# Set default strings to search in VERSION_FILE
  ifndef MAJOR_VERSION_DEFINE
    MAJOR_VERSION_DEFINE:=MAJOR_VERSION
  endif
  ifndef MINOR_VERSION_DEFINE
    MINOR_VERSION_DEFINE:=MINOR_VERSION
  endif
  ifndef BUILD_NUMBER_DEFINE
    BUILD_NUMBER_DEFINE:=BUILD_NUMBER
  endif


# If not specified, find the various version components in the VERSION_FILE

  ifndef MAJOR_VERSION
    MAJOR_VERSION:=$(strip $(subst \#define,, $(subst $(MAJOR_VERSION_DEFINE),,\
                   $(shell grep "define *$(MAJOR_VERSION_DEFINE) *" $(VERSION_FILE)))))
  endif
  ifndef MINOR_VERSION
    MINOR_VERSION:=$(strip $(subst \#define,, $(subst $(MINOR_VERSION_DEFINE),,\
                   $(shell grep "define *$(MINOR_VERSION_DEFINE)" $(VERSION_FILE)))))
  endif
  ifndef BUILD_TYPE
    BUILD_TYPE:=$(strip $(subst \#define,,$(subst BUILD_TYPE,,\
                $(subst AlphaCode,alpha,$(subst BetaCode,beta,$(subst ReleaseCode,.,\
                $(shell grep "define *BUILD_TYPE" $(VERSION_FILE))))))))
  endif
  ifndef BUILD_NUMBER
    BUILD_NUMBER:=$(strip $(subst \#define,,$(subst $(BUILD_NUMBER_DEFINE),,\
                  $(shell grep "define *$(BUILD_NUMBER_DEFINE)" $(VERSION_FILE)))))
  endif

# Finally check that version numbers are not empty

  ifeq (,$(MAJOR_VERSION))
    override MAJOR_VERSION:=1
  endif
  ifeq (,$(MINOR_VERSION))
    override MINOR_VERSION:=0
  endif
  ifeq (,$(BUILD_TYPE))
    override BUILD_TYPE:=alpha
  endif
  ifeq (,$(BUILD_NUMBER))
    override BUILD_NUMBER:=0
  endif

# Check for VERSION either predefined or defined by previous section from VERSION_FILE
  ifndef VERSION
    VERSION:=$(MAJOR_VERSION).$(MINOR_VERSION)$(BUILD_TYPE)$(BUILD_NUMBER)
  endif # ifndef VERSION
endif # ifdef VERSION_FILE

# Build the CVS_TAG string from the components
ifndef CVS_TAG
  CVS_TAG := v$(MAJOR_VERSION)_$(MINOR_VERSION)$(subst .,_,$(BUILD_TYPE))$(BUILD_NUMBER)
endif

ifdef DEBUG

# Cannot do this in DEBUG mode, so do it without DEBUG

release ::
	$(MAKE) DEBUG= release

else


ifndef VERSION

release ::
	@echo Must define VERSION macro or have version.h/custom.cxx file.

tagbuild ::
	@echo Must define VERSION macro or have version.h/custom.cxx file.

else # ifdef VERSION

# "make release" definition

ifndef RELEASEDIR
RELEASEDIR=releases
endif

ifndef RELEASEBASEDIR
RELEASEBASEDIR=$(PROG)
endif

RELEASEPROGDIR=$(RELEASEDIR)/$(RELEASEBASEDIR)

release :: $(TARGET) releasefiles
	cp $(TARGET) $(RELEASEPROGDIR)/$(PROG)
	cd $(RELEASEDIR) ; tar chf - $(RELEASEBASEDIR) | gzip > $(PROG)_$(VERSION)_$(PLATFORM_TYPE).tar.gz
	rm -r $(RELEASEPROGDIR)

releasefiles ::
	-mkdir -p $(RELEASEPROGDIR)


version:
	@echo v$(VERSION) "  CVS tag:" `cvs status Makefile | grep "Sticky Tag" | sed -e "s/(none)/HEAD/" -e "s/(.*)//" -e "s/^.*://"`


ifndef VERSION_FILE

tagbuild ::
	@echo Must define VERSION_FILE macro or have version.h/custom.cxx file.

else # ifndef VERSION_FILE

ifndef CVS_TAG

tagbuild ::
	@echo Must define CVS_TAG macro or have version.h/custom.cxx file.

else # ifndef CVS_TAG

tagbuild ::
	sed $(foreach dir,$(LIBDIRS), -e "s/ $(notdir $(dir)):.*/ $(notdir $(dir)): $(shell $(MAKE) -s -C $(dir) version)/") $(VERSION_FILE) > $(VERSION_FILE).new
	mv -f $(VERSION_FILE).new $(VERSION_FILE)
	cvs commit -m "Pre-tagging check in for $(CVS_TAG)." $(VERSION_FILE)
	cvs tag -c $(CVS_TAG)
	BLD=`expr $(BUILD_NUMBER) + 1` ; \
	echo "Incrementing to build number " $$BLD; \
	sed "s/$(BUILD_NUMBER_DEFINE)[ ]*[0-9][0-9]*/$(BUILD_NUMBER_DEFINE) $$BLD/" $(VERSION_FILE) > $(VERSION_FILE).new
	mv -f $(VERSION_FILE).new $(VERSION_FILE)
	cvs commit -m "Incremented build number after tagging to $(CVS_TAG)." $(VERSION_FILE)
	cvs -q update -r $(CVS_TAG)

endif # else ifndef CVS_TAG

endif # else ifndef VERSION_FILE

endif # else ifdef VERSION

endif # else ifdef DEBUG


######################################################################
#
# rules for creating build number files
#
######################################################################

ifdef BUILDFILES
$(OBJDIR)/buildnum.o:	buildnum.c
	cc -o $(OBJDIR)/buildnum.o -c buildnum.c

#ifndef DEBUG
#buildnum.c:	$(SOURCES) $(BUILDFILES) 
#	buildinc buildnum.c
#else
buildnum.c:
#endif

endif

######################################################################
#
# Include all of the dependencies
#
######################################################################

ifndef NODEPS
-include $(DEPDIR)/*.dep
endif


# End of common.mak

