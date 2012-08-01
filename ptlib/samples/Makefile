#
# Makefile
#
# Make file for pwlib library
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
# Revision 1.3  2006/06/20 09:23:56  csoutheren
# Applied patch 1465192
# Fix pwlib make files, and config for unix
#
# Revision 1.2  2004/09/13 01:14:08  dereksmithies
# Add two new targets to unix build, dtmftest and threadex
#
# Revision 1.1  2003/01/07 09:41:38  rogerh
# Makefile to build most of the pwlib sample programs
#
#


all ::

include ../make/ptlib.mak

#SUBDIRS += ThreadSafe audio find_ip hello_world netif thread threadex dtmftest
SUBDIRS += audio find_ip ldaptest netif stunclient threadsafe dtmftest ipv6test md5 strtest thread timing

#SUBDIRS += pxml xmlrpc xmlrpcsrvr   #expat + some are broken
#SUBDIRS += vxmltest                 # no makefile

# all these targets are just passed to all subdirectories
$(subst tagbuild,,$(STANDARD_TARGETS)) ::
	for dir in $(SUBDIRS) ; do \
	if test -d $$dir ; then \
          $(MAKE) -C $$dir $@ ; \
        fi \
	done

# End of Makefile
