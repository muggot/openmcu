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
# Revision 1.3  2007/08/02 18:48:56  shorne
# Added SNMP Server support
#
# Revision 1.2  2006/01/30 21:55:57  dominance
# fix win32 build with new MJPEG code.
#
# Revision 1.1  2005/09/29 21:06:15  dominance
# one more bit towards mingw support.
#
# Revision 1.137  2005/07/13 11:15:26  csoutheren
# Backported NAT abstraction files from isvo branch
#
# Revision 1.136  2005/02/03 01:25:44  csoutheren
# Make sure code still links when plugins not enabled
#
# Revision 1.135  2005/01/31 08:05:41  csoutheren
# More patches for MacOSX, thanks to Hannes Friederich
#
# Revision 1.134  2005/01/26 05:38:04  csoutheren
# Added ability to remove config file support
#
# Revision 1.133  2005/01/11 06:22:10  csoutheren
# Fixed problem with removing HTTP classes
#
# Revision 1.132  2005/01/03 12:48:43  csoutheren
# Added new configure options and ability to disable/enable modules
#
# Revision 1.131  2005/01/03 04:41:39  csoutheren
# Normalise handling of configure flags
# Add --enable-minsize option (more to follow)
#
# Revision 1.130  2004/10/28 20:07:11  csoutheren
# Fixes for MacOSX platforms, thanks to Hannes Friederich
#
# Revision 1.129  2004/05/31 22:59:04  csoutheren
# Added enum.cxx
#
# Revision 1.128  2004/04/28 11:26:43  csoutheren
# Hopefully fixed SASL and SASL2 problems
#
# Revision 1.127  2004/04/26 04:17:19  rjongbloed
# Fixed GNU warnings
#
# Revision 1.126  2004/04/24 06:13:06  rjongbloed
# Changed XMPP code to only be compiled if have EXPAT
#
# Revision 1.125  2004/04/24 01:37:43  csoutheren
# Added PNotifier extensions and XMPP (Jabber) support,
#   thanks to Federico Pinna and Reitek S.p.A.
#
# Revision 1.124  2004/04/18 12:37:41  csoutheren
# Modified to detect sem_wait etc on Linux systems
#
# Revision 1.123  2004/04/02 03:59:46  ykiryanov
# Uncommented beaudio code
#
# Revision 1.122  2004/03/17 05:52:59  csoutheren
# Removed obsolete udp.cxx
#
# Revision 1.121  2004/02/15 22:36:29  csoutheren
# Included ALSA in default non-plugin sound drivers
#
# Revision 1.120  2003/11/18 10:41:20  csoutheren
# Fixed problem with linking in static plugins
#
# Revision 1.119  2003/11/14 05:32:09  csoutheren
# Removed video input device
#
# Revision 1.118  2003/11/12 03:29:51  csoutheren
# Initial version of plugin code from Snark of GnomeMeeting with changes
#    by Craig Southeren of Post Increment
#
# Revision 1.117  2003/10/27 04:06:13  csoutheren
# Added code to allow compilation of new QoS code on Unix
#
# Revision 1.116  2003/10/06 02:30:06  dereksmithies
# Remove Stun.cxx as it is not in the repositary.
#
# Revision 1.115  2003/09/17 08:57:33  csoutheren
# Moved PSmartPointer and PNotifier into seperate files
# Added detection for system regex libraries on all platforms
#
# Revision 1.114  2003/07/28 18:44:01  dsandras
# Make use of the libc regex on Linux.
#
# Revision 1.113  2003/05/22 12:41:42  dsandras
#
# Check if TRY_1394AVC/DC=1 to compile support for it or not.
#
# Revision 1.112  2003/04/28 08:37:06  craigs
# Added SDL
#
# Revision 1.111  2003/04/16 08:05:50  craigs
# CHanged checks of variosu vars to use ifeq
#
# Revision 1.110  2003/04/15 08:05:43  craigs
# Added pdns.cxx
#
# Revision 1.109  2003/04/14 21:20:59  dereks
# Fix problems for Firewire video. Thanks to Snark on #gnomemeeting
#
# Revision 1.108  2003/03/31 09:03:04  robertj
# Added ILS support file.
#
# Revision 1.107  2003/03/28 03:03:28  craigs
# Added LDAP sources
#
# Revision 1.106  2003/02/24 17:49:02  rogerh
# Add Mac Core Audio changes from Shawn.
#
# Revision 1.105  2003/02/18 03:55:59  dereks
# Add 1394AVC pathces from  Georgi Georgiev. Thanks!
#
# Revision 1.104  2003/02/04 22:51:20  robertj
# Added basic SOAP support, thanks Andreas Sikkema
#
# Revision 1.103  2003/02/04 07:41:10  craigs
# Added stun elements
#
# Revision 1.102  2003/01/11 05:30:13  robertj
# Added support for IEEE 1394 AV/C cameras, thanks Georgi Georgiev
#
# Revision 1.101  2003/01/06 18:41:08  rogerh
# Add NetBSD patches, taken from the NetBSD pkg patches.
# Submitted by Andreas Wrede
#
# Revision 1.100  2002/11/22 10:14:07  robertj
# QNX port, thanks Xiaodan Tang
#
# Revision 1.99  2002/11/05 01:43:39  robertj
# Added missing VxWorks files.
#
# Revision 1.98  2002/10/21 13:01:33  rogerh
# bison 1.5 and above need -o to specify the output filename
#
# Revision 1.97  2002/10/10 04:43:44  robertj
# VxWorks port, thanks Martijn Roest
#
# Revision 1.96  2002/10/02 08:59:13  craigs
# Added pxmlrpcs.cxx
#
# Revision 1.95  2002/09/09 06:42:00  robertj
# Added check for Sun audio header before compiling module, thanks Nils Bokermann
#
# Revision 1.94  2002/08/29 01:45:07  robertj
# Changed default opt build to include PTRACE
#
# Revision 1.93  2002/08/14 15:18:54  craigs
# Added ptts.cxx to build
#
# Revision 1.92  2002/06/27 05:39:34  craigs
# Added vxml.cxx
#
# Revision 1.91  2002/06/27 03:53:01  robertj
# Added PMemoryFile class.
#
# Revision 1.90  2002/05/01 04:16:43  robertj
# Added thread safe collection classes.
#
# Revision 1.89  2002/04/18 05:12:20  robertj
# Changed /usr/include to SYSINCDIR helps with X-compiling, thanks Bob Lindell
#
# Revision 1.88  2002/04/02 11:59:32  craigs
# Added pxmlrpc
#
# Revision 1.87  2002/02/20 02:37:26  dereks
# Initial release of Firewire camera support for linux.
# Many thanks to Ryutaroh Matsumoto <ryutaroh@rmatsumoto.org>.
#
# Revision 1.86  2002/01/23 12:09:46  rogerh
# Add dtmf decoder to the compilation
#
# Revision 1.85  2002/01/22 03:55:32  craigs
# Moved PWavFile to PTCLib
#
# Revision 1.84  2001/12/06 05:39:05  craigs
# Added support for PXML component library
#
# Revision 1.83  2001/11/27 22:42:13  robertj
# Changed to make system to better support non-shared library building.
#
# Revision 1.82  2001/11/25 23:28:04  robertj
# Fixed correct setting of HAS_VIDEO_CAPTURE compile flag
#
# Revision 1.81  2001/08/11 15:38:43  rogerh
# Add Mac OS Carbon changes from John Woods <jfw@jfwhome.funhouse.com>
#
# Revision 1.80  2001/07/20 07:36:59  rogerh
# Move ESD detection.
#
# Revision 1.79  2001/07/19 09:53:29  rogerh
# Add the PWAVFile class to read and write .wav files
#
# Revision 1.78  2001/07/19 09:27:12  rogerh
# Add support for EsounD and esd (the Enlightenment Sound Daemon).
# This allows OhPhone to run on platforms where EsounD and esd have been
# ported which includes Mac OS X.
# Code written by Shawn Pai-Hsiang Hsiao <shawn@eecs.harvard.edu>.
#
# Revision 1.77  2001/07/10 03:45:14  robertj
# Added queue channel and delay channel classes to ptclib.
#
# Revision 1.76  2001/07/09 06:16:15  yurik
# Jac Goudsmit's BeOS changes of July,6th. Cleaning up media subsystem etc.
#
# Revision 1.75  2001/06/27 17:25:46  rogerh
# Compile the dummyvideo.cxx file on platforms with no video capture hardware
#
# Revision 1.74  2001/06/26 15:49:15  rogerh
# Define HAS_VIDEOCAPTURE on platforms with video capture code
#
# Revision 1.73  2001/06/26 14:20:36  rogerh
# Compile the generic video files on all platforms
#
# Revision 1.72  2001/05/29 03:34:39  craigs
# Changed to use new HAS_OPENSSL define
#
# Revision 1.71  2001/02/27 01:05:24  robertj
# Updated regular expression parser to latest version from Henry Spencer.
#
# Revision 1.70  2001/02/23 08:42:17  rogerh
# Add dummyaudio as the default SOUND_SOURCE
#
# Revision 1.69  2001/02/15 04:58:51  robertj
# Added class to do secure HTTP based service process.
#
# Revision 1.68  2001/01/24 02:51:15  robertj
# Changed definition of list of files for make clean to more correct position
#   as expected by common.mak
#
# Revision 1.67  2001/01/04 18:13:32  rogerh
# Tidy up the Makefile when building video4bsd.cxx
#
# Revision 1.66  2001/01/04 18:02:16  rogerh
# remove some old parts refering to linux
#
# Revision 1.65  2001/01/03 23:10:09  craigs
# Fixed Makefile problem when video not available
#
# Revision 1.64  2001/01/03 16:48:13  rogerh
# Rename V4L_SOURCE to VIDEO_SOURCE (to be less platform specific).
# Specify which files to use for VIDEO_SOURCE on non-linux systems eg BSD.
#
# Revision 1.63  2000/12/19 22:20:26  dereks
# Add video channel classes to connect to the PwLib PVideoInputDevice class.
# Add PFakeVideoInput class to generate test images for video.
#
# Revision 1.62  2000/12/16 13:00:46  rogerh
# BeOS changes, submitted by Yuri Kiryanov <openh323@kiryanov.com>
#
# Revision 1.61  2000/07/26 02:40:30  robertj
# Added video I/O devices.
#
# Revision 1.60  2000/06/21 01:01:22  robertj
# AIX port, thanks Wolfgang Platzer (wolfgang.platzer@infonova.at).
#
# Revision 1.59  2000/04/26 00:40:48  robertj
# Redesigned version number system to have single file to change on releases.
#
# Revision 1.58  2000/04/19 00:13:52  robertj
# BeOS port changes.
#
# Revision 1.57  2000/04/09 18:19:23  rogerh
# Add my changes for NetBSD support.
#
# Revision 1.56  2000/03/08 18:22:03  rogerh
# Select oss.cxx sound support for OpenBSD
#
# Revision 1.55  2000/03/03 00:30:31  robertj
# Removed ptclilb/proto directory.
#
# Revision 1.54  2000/02/24 11:03:27  craigs
# Fixed problem with making PW programs
#
# Revision 1.53  2000/02/17 12:09:19  robertj
# Added better random number generator after finding major flaws in MSVCRT version.
#
# Revision 1.52  2000/02/15 23:11:34  robertj
# Audio support for FreeBSD, thanks Roger Hardiman.
#
# Revision 1.51  2000/01/31 01:56:48  robertj
# Fixed BeOS compatibility problem.
#
# Revision 1.50  2000/01/22 00:52:06  craigs
# Added ability to use in any directory, and to create shared libraries
#
# Revision 1.49  2000/01/10 02:22:41  craigs
# Updated for new OpenSSL functionality
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

ifndef PTLIBDIR
PTLIBDIR := $(HOME)/ptlib
endif

include $(PTLIBDIR)/make/unix.mak

OBJDIR	= $(PT_OBJDIR)
LIBDIR  = $(PT_LIBDIR)
TARGET  = $(LIBDIR)/$(PTLIB_FILE)
VERSION_FILE = $(PTLIBDIR)/version.h

##########################################

COMPONENT_SRC_DIR	= ../../ptclib
COMMON_SRC_DIR		= ../common
VPATH_CXX		:= $(COMMON_SRC_DIR) $(COMPONENT_SRC_DIR)
PLUGIN_DIR		= ../../../plugins


#####CFLAGS += -DPTRACING=1

# try and keep the order of modules in reverse order of usage so any
# global statics will be constructed/destructed in the correct order

ifdef HAS_SASL2
SOURCES	+= $(COMPONENT_SRC_DIR)/psasl.cxx 
endif

ifdef HAS_OPENLDAP
SOURCES	+= \
	$(COMPONENT_SRC_DIR)/pldap.cxx \
        $(COMPONENT_SRC_DIR)/pils.cxx
endif

ifdef HAS_OPENSSL
SOURCES += $(COMPONENT_SRC_DIR)/pssl.cxx 
endif

ifdef HAS_SDL
SOURCES += $(COMPONENT_SRC_DIR)/vsdl.cxx
endif


## VIDEO DRIVERS
## Note this is mostly handled by the plugin system

ifdef HAS_VIDEO


SOURCES	 += $(COMMON_SRC_DIR)/vfakeio.cxx \
            $(COMMON_SRC_DIR)/videoio.cxx \
	    $(COMMON_SRC_DIR)/vconvert.cxx \
	    $(COMMON_SRC_DIR)/pvidchan.cxx \
	    $(COMMON_SRC_DIR)/tinyjpeg.c \
	    $(COMMON_SRC_DIR)/jidctflt.c \
	    vfw.cxx
	        
endif # HAS_VIDEO

## SOUND DRIVERS
## Note this is mostly handled by the plugin system

ifdef HAS_AUDIO

SOURCES += $(COMMON_SRC_DIR)/sound.cxx sound_win32.cxx

endif  # HAS_AUDIO

## Various modules

ifdef HAS_EXPAT
SOURCES	+= $(COMPONENT_SRC_DIR)/pxml.cxx 

ifdef HAS_XMLRPC
SOURCES	+= \
	$(COMPONENT_SRC_DIR)/pxmlrpc.cxx \
	$(COMPONENT_SRC_DIR)/pxmlrpcs.cxx 
endif

ifdef HAS_SOAP
SOURCES	+= $(COMPONENT_SRC_DIR)/psoap.cxx 
endif

ifdef HAS_VXML
SOURCES	+= $(COMPONENT_SRC_DIR)/vxml.cxx 
endif

endif # HAS_EXPAT

ifdef HAS_SASL2 # FIXME?
SOURCES += \
	$(COMPONENT_SRC_DIR)/xmpp.cxx \
	$(COMPONENT_SRC_DIR)/xmpp_c2s.cxx \
	$(COMPONENT_SRC_DIR)/xmpp_roster.cxx
endif

ifdef HAS_RESOLVER
SOURCES += \
	$(COMPONENT_SRC_DIR)/pdns.cxx \
	$(COMPONENT_SRC_DIR)/enum.cxx 
endif

ifdef HAS_TTS
SOURCES += $(COMPONENT_SRC_DIR)/ptts.cxx 
endif

ifdef HAS_ASN
SOURCES += \
	$(COMPONENT_SRC_DIR)/asner.cxx \
	$(COMPONENT_SRC_DIR)/pasn.cxx 
endif

ifdef HAS_SNMP
SOURCES += \
	$(COMPONENT_SRC_DIR)/snmpclnt.cxx \
	$(COMPONENT_SRC_DIR)/snmpserv.cxx \
	$(COMPONENT_SRC_DIR)/psnmp.cxx \
	$(COMPONENT_SRC_DIR)/snmp.cxx \
	$(COMPONENT_SRC_DIR)/rfc1155.cxx  
endif

ifdef HAS_FTP
SOURCES += \
	$(COMPONENT_SRC_DIR)/ftpclnt.cxx \
	$(COMPONENT_SRC_DIR)/ftpsrvr.cxx \
	$(COMPONENT_SRC_DIR)/ftp.cxx 
endif

ifdef HAS_TELNET
SOURCES += $(COMPONENT_SRC_DIR)/telnet.cxx 
endif

ifdef HAS_STUN
SOURCES += $(COMPONENT_SRC_DIR)/pstun.cxx \
           $(COMPONENT_SRC_DIR)/pnat.cxx
endif

ifdef HAS_SOCKS
SOURCES += $(COMPONENT_SRC_DIR)/socks.cxx 
endif

ifdef HAS_PIPECHAN
SOURCES += pipe.cxx 
endif

ifdef HAS_REMCONN
SOURCES += remconn.cxx 
endif

ifdef HAS_WAVFILE
SOURCES += $(COMPONENT_SRC_DIR)/pwavfile.cxx 
endif

ifdef HAS_DTMF
SOURCES += $(COMPONENT_SRC_DIR)/dtmf.cxx 
endif

ifdef HAS_SERIAL
SOURCES += \
	winserial.cxx \
	$(COMPONENT_SRC_DIR)/modem.cxx 
endif

ifdef HAS_POP3SMTP
SOURCES += \
	$(COMPONENT_SRC_DIR)/inetmail.cxx 
endif

ifdef HAS_HTTP
SOURCES += \
	$(COMPONENT_SRC_DIR)/http.cxx \
	$(COMPONENT_SRC_DIR)/httpclnt.cxx 
endif

ifdef HAS_HTTPSVC
SOURCES += \
	svcproc.cxx \
	$(COMPONENT_SRC_DIR)/httpsvc.cxx \
	$(COMPONENT_SRC_DIR)/httpform.cxx \
	$(COMPONENT_SRC_DIR)/httpsrvr.cxx \
	$(COMPONENT_SRC_DIR)/html.cxx 

ifdef HAS_OPENSSL
SOURCES += $(COMPONENT_SRC_DIR)/shttpsvc.cxx
endif #HAS_OPENSSL
endif #HAS_HTTPSVC

ifdef HAS_CONFIG_FILE
SOURCES += wincfg.cxx 
endif

GETDATE_SOURCE = $(COMMON_SRC_DIR)/getdate.tab.c

SOURCES	+= \
	$(COMPONENT_SRC_DIR)/ipacl.cxx \
	$(COMPONENT_SRC_DIR)/qchannel.cxx \
	$(COMPONENT_SRC_DIR)/delaychan.cxx \
	$(COMPONENT_SRC_DIR)/memfile.cxx \
	$(COMPONENT_SRC_DIR)/cypher.cxx \
	$(COMPONENT_SRC_DIR)/random.cxx \
	$(COMPONENT_SRC_DIR)/inetprot.cxx \
	$(COMPONENT_SRC_DIR)/notifier_ext.cxx \
	$(GETDATE_SOURCE) \
	ptlib.cxx \
	icmp.cxx \
	winsock.cxx \
	win32.cxx \
	assert.cxx \
	dllmain.cxx \
	ethsock.cxx \
	$(COMMON_SRC_DIR)/pethsock.cxx \
        $(COMMON_SRC_DIR)/pluginmgr.cxx \
	$(COMMON_SRC_DIR)/sockets.cxx \
	$(COMMON_SRC_DIR)/qos.cxx \
	$(COMMON_SRC_DIR)/ptime.cxx \
	$(COMMON_SRC_DIR)/osutils.cxx \
	$(COMMON_SRC_DIR)/safecoll.cxx \
	$(COMMON_SRC_DIR)/collect.cxx \
	$(COMMON_SRC_DIR)/contain.cxx \
	$(COMMON_SRC_DIR)/pchannel.cxx \
	$(COMMON_SRC_DIR)/pconfig.cxx \
	$(COMMON_SRC_DIR)/pipechan.cxx \
	$(COMMON_SRC_DIR)/serial.cxx \
	$(COMMON_SRC_DIR)/object.cxx   # must be last module

ifneq ($(HAS_REGEX),1)
OBJS	= $(OBJDIR)/regcomp.o $(OBJDIR)/regexec.o $(OBJDIR)/regerror.o $(OBJDIR)/regfree.o
endif

CLEAN_FILES = $(GETDATE_SOURCE)


#######################################################

include ../../../make/common.mak

#######################################################

alllibs:
	$(MAKE) both
	$(MAKE) bothshared


$(OBJDIR)/regcomp.o: $(COMMON_SRC_DIR)/regex/regcomp.c
	$(CC) $(CFLAGS) $(DEBUG_FLAG) $(OPTCCFLAGS) -c -DPOSIX_MISTAKE -I$(COMMON_SRC_DIR)/regex -o $@ -c $<

$(OBJDIR)/regexec.o: $(COMMON_SRC_DIR)/regex/regexec.c
	$(CC) $(CFLAGS) $(DEBUG_FLAG) $(OPTCCFLAGS) -c -DPOSIX_MISTAKE -I$(COMMON_SRC_DIR)/regex -o $@ -c $<

$(OBJDIR)/regerror.o: $(COMMON_SRC_DIR)/regex/regerror.c
	$(CC) $(CFLAGS) $(DEBUG_FLAG) $(OPTCCFLAGS) -c -DPOSIX_MISTAKE -I$(COMMON_SRC_DIR)/regex -o $@ -c $<

$(OBJDIR)/regfree.o: $(COMMON_SRC_DIR)/regex/regfree.c
	$(CC) $(CFLAGS) $(DEBUG_FLAG) $(OPTCCFLAGS) -c -DPOSIX_MISTAKE -I$(COMMON_SRC_DIR)/regex -o $@ -c $<


$(OBJDIR)/getdate.tab.o: $(GETDATE_SOURCE)
	$(CC) $(CFLAGS) $(STDCCFLAGS) -c $< -o $@

$(DEPDIR)/getdate.tab.dep: $(GETDATE_SOURCE)
	$(CC) $(STDCCFLAGS) -M $< >> $@

$(GETDATE_SOURCE): $(COMMON_SRC_DIR)/getdate.y
	bison $(COMMON_SRC_DIR)/getdate.y -o $(COMMON_SRC_DIR)/getdate.tab.c

#######################################################

LIB_BASENAME=$(PTLIB_BASE)
LIB_FILENAME=$(PTLIB_FILE)

include $(PTLIBDIR)/make/lib.mak

#######################################################

# End of Makefile
