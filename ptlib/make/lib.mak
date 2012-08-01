#
# lib.mak
#
# Make rules for building libraries rather than applications.
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
# Contributor(s): ______________________________________.
#       
# $Log: lib.mak,v $
# Revision 1.40  2005/09/25 10:51:23  dominance
# almost complete the mingw support. We'll be there soon. ;)
#
# Revision 1.39  2004/06/17 06:33:07  csoutheren
# Changed Solaris linker to g++. It should never have been using ld
#
# Revision 1.38  2004/04/25 10:30:08  rjongbloed
# Fixed correct SONAME when using beta versions.
#
# Revision 1.37  2003/09/08 22:13:18  dereksmithies
# Fix patch implementation. Thanks to  Zygmuntowicz Michal <m.zygmuntowicz@onet.pl>
#
# Revision 1.36  2003/09/08 22:07:41  dereksmithies
# Add patch from Zygmuntowicz Michal <m.zygmuntowicz@onet.pl> for Solaris.
# Many thanks!!!
#
# Revision 1.35  2003/07/29 12:33:34  csoutheren
# Changed to ensure dynamic libraries using pwlib always dynamically link pwlib
#
# Revision 1.34  2003/06/23 06:44:11  csoutheren
# Added minor revision into the soname for the libraries as requested by Damien Sandras
#
# Revision 1.33  2003/05/29 09:03:36  rjongbloed
# Changed SONAME so includes minor version number to allow for API changes.
#
# Revision 1.32  2003/05/06 09:47:20  robertj
# Fixed up MacOSX changes so is compatible with previous "API" not requiring
#   downstream libraries to change
#
# Revision 1.31  2003/05/06 06:59:12  robertj
# Dynamic library support for MacOSX, thanks Hugo Santos
#
# Revision 1.30  2002/11/22 10:14:07  robertj
# QNX port, thanks Xiaodan Tang
#
# Revision 1.29  2002/10/17 13:44:27  robertj
# Port to RTEMS, thanks Vladimir Nesic.
#
# Revision 1.28  2002/09/26 00:17:48  robertj
# Fixed make install to force creation of symlinks, thanks Pierre.
#
# Revision 1.27  2002/08/30 11:42:46  robertj
# Added formatting.
# Changed alpha & beta sharable library file name convention.
#
# Revision 1.26  2002/02/01 11:11:27  rogerh
# Use .$(MAJOR_VERSION) with soname (instead of hard coding .1)
#
# Revision 1.25  2002/01/31 06:55:22  robertj
# Removed redundant inclusion of ssl, is already in unix.mak
#
# Revision 1.24  2002/01/27 08:45:50  rogerh
# FreeBSD and OpwnBSD use -pthread and not -lpthread
#
# Revision 1.23  2002/01/26 23:57:08  craigs
# Changed for GCC 3.0 compatibility, thanks to manty@manty.net
#
# Revision 1.22  2002/01/15 07:47:30  robertj
# Fixed previous fix
#
# Revision 1.21  2002/01/14 23:14:29  robertj
# Added ENDLDFLAGS to shared library link, thanks Paul a écrit
#
# Revision 1.20  2001/12/18 04:12:08  robertj
# Fixed Linux compatibility of previous change for Solaris.
#
# Revision 1.19  2001/12/17 23:33:50  robertj
# Solaris 8 porting changes, thanks James Dugal
#
# Revision 1.18  2001/12/05 08:32:06  rogerh
# ln -sf complains if the file already exists on Solaris. So do rm -f first
#
# Revision 1.17  2001/12/01 17:41:07  rogerh
# Use locall defined compiler binary instead of hard coded gcc
#
# Revision 1.16  2001/11/30 00:37:16  robertj
# Fixed incorrect library filename when building static library during shared
#   library build.
#
# Revision 1.15  2001/11/27 22:42:13  robertj
# Changed to make system to better support non-shared library building.
#
# Revision 1.14  2001/07/07 06:51:52  robertj
# Added fix to remove shared libraries in make clean, thanks Jac Goudsmit
#
# Revision 1.13  2001/07/03 04:41:25  yurik
# Corrections to Jac's submission from 6/28
#
# Revision 1.12  2001/06/30 06:59:06  yurik
# Jac Goudsmit from Be submit these changes 6/28. Implemented by Yuri Kiryanov
#
# Revision 1.11  2001/03/23 03:18:01  robertj
# Fixed addition of trailing dot at end of release versions of so libraries
#   caused to removal of the "pl" build type, changing it to a dot.
#
# Revision 1.10  2001/03/14 06:24:55  robertj
# Fixed setting of symlinks for shared libraries to be relative paths.
#
# Revision 1.9  2000/05/19 01:26:31  robertj
# Added copyright notice
#

ifneq ($(OSTYPE),Darwin)
  LIBNAME_MAJ	= $(LIB_FILENAME).$(MAJOR_VERSION)
  LIBNAME_MIN	= $(LIBNAME_MAJ).$(MINOR_VERSION)
  ifeq ($(BUILD_TYPE),.)
    LIBNAME_PAT	= $(LIBNAME_MIN).$(BUILD_NUMBER)
  else
    LIBNAME_PAT	= $(LIBNAME_MIN)-$(BUILD_TYPE)$(BUILD_NUMBER)
  endif
else
  LIBNAME_MAJ	= $(subst .$(LIB_SUFFIX),.$(MAJOR_VERSION).$(LIB_SUFFIX),$(LIB_FILENAME))
  LIBNAME_MIN	= $(subst .$(LIB_SUFFIX),.$(MAJOR_VERSION).$(MINOR_VERSION).$(LIB_SUFFIX),$(LIB_FILENAME))
  ifeq ($(BUILD_TYPE),.)
    LIBNAME_PAT	= $(subst .$(LIB_SUFFIX),.$(MAJOR_VERSION).$(MINOR_VERSION).$(BUILD_NUMBER).$(LIB_SUFFIX),$(LIB_FILENAME))
  else
    LIBNAME_PAT	= $(subst .$(LIB_SUFFIX),.$(MAJOR_VERSION).$(MINOR_VERSION)-$(BUILD_TYPE)$(BUILD_NUMBER).$(LIB_SUFFIX),$(LIB_FILENAME))
  endif
endif

LIB_SONAME	= $(LIBNAME_PAT)

CLEAN_FILES += $(LIBDIR)/$(LIBNAME_PAT) $(LIBDIR)/$(LIB_FILENAME) $(LIBDIR)/$(LIBNAME_MAJ) $(LIBDIR)/$(LIBNAME_MIN)

ifneq ($(P_SHAREDLIB),1)
  STATIC_LIB_FILE = $(LIBDIR)/$(LIB_FILENAME)
else
  STATIC_LIB_FILE = $(LIBDIR)/$(subst .$(LIB_SUFFIX),_s.$(STATICLIBEXT),$(LIB_FILENAME))
endif

$(LIBDIR)/$(LIB_FILENAME) : $(TARGETLIB)

ifeq ($(P_SHAREDLIB),1)

  ENDLDLIBS := $(SYSLIBS) $(ENDLDLIBS)
  ifeq ($(OSTYPE),beos)
    # BeOS requires different options when building shared libraries
    # Also, when building a shared library x that references symbols in libraries y,
    # the y libraries need to be added to the linker command
    LDSOOPTS = -nostdlib -nostart
    EXTLIBS = -lstdc++.r4
  else
    ifeq ($(OSTYPE),Darwin)
      LDSOOPTS = -dynamiclib
    else
      LDSOOPTS = -shared
    endif
  endif

  ifeq ($(OSTYPE),rtems)
    EXTLIBS = -lstdc++
  endif

  ifneq ($(OSTYPE), QNX)
    ifneq (,$(findstring $(OSTYPE),FreeBSD OpenBSDs))
      ifdef P_PTHREADS
        EXTLIBS += -pthread
      endif
    else
      ifdef P_PTHREADS
        EXTLIBS += -lpthread
      endif
    endif
  endif

  # Solaris loader doesn't grok -soname  (sees it as -s -oname)
  # We could use -Wl,-h,$(LIB_BASENAME).1 but then we find that the arglist
  # to gcc is 2900+ bytes long and it will barf.  I fix this by invoking ld
  # directly and passing it the equivalent arguments...jpd@louisiana.edu
  ifeq ($(OSTYPE),solaris)
     LDSOOPTS = -Bdynamic -G -h $(LIB_SONAME)
     LD = $(CPLUS)
  else
    ifeq ($(OSTYPE),mingw)
      LDSOOPTS += -Wl,--kill-at
    else
    ifneq ($(OSTYPE),Darwin)
      LDSOOPTS += -Wl,-soname,$(LIB_SONAME)
    endif
    endif
    LD = $(CPLUS)
  endif

  $(LIBDIR)/$(LIB_FILENAME): $(LIBDIR)/$(LIBNAME_PAT)
	cd $(LIBDIR) ; rm -f $(LIB_FILENAME) ; ln -sf $(LIBNAME_PAT) $(LIB_FILENAME)
	cd $(LIBDIR) ; rm -f $(LIBNAME_MAJ) ;  ln -sf $(LIBNAME_PAT) $(LIBNAME_MAJ)
	cd $(LIBDIR) ; rm -f $(LIBNAME_MIN) ;  ln -sf $(LIBNAME_PAT) $(LIBNAME_MIN)

  $(LIBDIR)/$(LIBNAME_PAT): $(STATIC_LIB_FILE)
	@echo EXTLIBS = $(EXTLIBS)
	@echo SYSLIBS = $(SYSLIBS)
	@if [ ! -d $(LIBDIR) ] ; then mkdir $(LIBDIR) ; fi
	$(LD) $(LDSOOPTS) -o $(LIBDIR)/$(LIBNAME_PAT) $(LDFLAGS) $(EXTLIBS) $(OBJS) $(ENDLDLIBS)

  install: $(LIBDIR)/$(LIBNAME_PAT)
	$(INSTALL) $(LIBDIR)/$(LIBNAME_PAT) $(INSTALLLIB_DIR)/$(LIBNAME_PAT)
	ln -sf $(LIBNAME_PAT) $(INSTALLLIB_DIR)/$(LIB_FILENAME)
	ln -sf $(LIBNAME_PAT) $(INSTALLLIB_DIR)/$(LIBNAME_MAJ)
	ln -sf $(LIBNAME_PAT) $(INSTALLLIB_DIR)/$(LIBNAME_MIN)

endif # P_SHAREDLIB

$(STATIC_LIB_FILE): $(OBJS)
	@if [ ! -d $(LIBDIR) ] ; then mkdir $(LIBDIR) ; fi
	$(ARCHIVE) $(STATIC_LIB_FILE) $(OBJS)
ifeq ($(P_USE_RANLIB),1)
	$(RANLIB) $(STATIC_LIB_FILE)
endif



# End of file ################################################################

