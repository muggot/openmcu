#
# Makefile
#
# Make file for simple sample application for the xmpptest application in pwlib
#
# Copyright (c) 2006 Derek J Smithies
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
# The Original Code is Open Phone Abstraction Library.
#
# The Initial Developer of the Original Code is Equivalence Pty. Ltd.
#
# Contributor(s): ______________________________________.
#
# $Log: Makefile,v $
# Revision 1.1  2006/07/23 03:39:49  dereksmithies
# Modify main.cxx so it compiles under linux.
# Add Makefile so it can be compiled under linux.
#
#
#

PROG		= xmpptest
SOURCES		:= \
	ConnectDialog.cxx \
	MainFrame.cxx     \
	main.cxx          \
	precompile.cxx    

#openphone.cxx : openphone.xrc
#	wxrc openphone.xrc -c -o openphone.cxx



STDCCFLAGS += `wx-config --cxxflags`

LDFLAGS += `wx-config  --libs`

include $(PTLIBDIR)/make/ptlib.mak


