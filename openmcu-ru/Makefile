#
# Makefile
#
# Make file for OpenMCU-ru
#

PREFIX          = /opt/openmcu-ru
MACHTYPE	= ${x86:-x86}
PROG		= openmcu-ru
SOURCES		:= main.cxx video.cxx conference.cxx filemembers.cxx custom.cxx h323.cxx html.cxx mcu.cxx sip.cxx
HDRS		:= conference.h config.h filemembers.h h323.h mcu.h version.h sip.h

LDLIBS		=  -lfreetype -lswresample -ljpeg -lyuv -lsofia-sip-ua
STDCCFLAGS	= -I../h323plus/include -I/usr/include/freetype2 -I/usr/include/sofia-sip-1.12 -D__STDC_CONSTANT_MACROS

ifndef OPENH323DIR
OPENH323DIR=../h323plus
endif

include $(OPENH323DIR)/openh323u.mak

CONFIG_FILES=capability.conf \
layouts.conf \
members.conf \
ffserver.conf

RESOURCE_FILES=i15_mic_on.gif \
i24_clr.gif \
openmcu.ru_vad_chosenvan.gif \
i20_close.gif \
i24_left.gif \
openmcu.ru_vad_disable.gif \
i20_plus.gif \
i24_mix.gif \
openmcu.gif \
openmcu.ru_vad_vad.gif \
i20_static.gif \
i24_revert.gif \
openmcu.ru_drop_Abdylas_Tynyshov.gif \
i15_inv.gif \
i20_vad2.gif \
i24_right.gif \
openmcu.ru_launched_Ypf.gif \
i15_mic_off.gif \
i20_vad.gif \
i24_shuff.gif \
openmcu.ru_remove.gif \
template.html \
quote.txt \
Russo_One.ttf

install:
	mkdir -p /opt/openmcu-ru/bin
	mkdir -p /opt/openmcu-ru/config
	mkdir -p /opt/openmcu-ru/pipe
	mkdir -p /opt/openmcu-ru/log
	mkdir -p /opt/openmcu-ru/records
	mkdir -p /opt/openmcu-ru/share/openmcu-ru
	mkdir -p /opt/openmcu-ru/share/openmcu-ru
	( for file in ${CONFIG_FILES}; \
	    do $(INSTALL) -m 644 conf/$$file /opt/openmcu-ru/config; \
	done )
	( for file in ${RESOURCE_FILES}; \
	    do $(INSTALL) -m 644 conf/$$file /opt/openmcu-ru/share/openmcu-ru; \
	done )
	$(INSTALL) -m 755 obj_linux_$(MACHTYPE)_r/$(PROG) /opt/openmcu-ru/bin
	rm -f /opt/openmcu-ru/openmcu
	rm -f /opt/openmcu-ru/openmcu-ru
	ln -s /opt/openmcu-ru/bin/$(PROG) /opt/openmcu-ru/openmcu
	ln -s /opt/openmcu-ru/bin/$(PROG) /opt/openmcu-ru/openmcu-ru
	$(INSTALL) -m 755 conf/web_stream_start /opt/openmcu-ru/bin
	$(INSTALL) -m 755 conf/ffmpeg /opt/openmcu-ru/bin
	$(INSTALL) -m 755 conf/ffserver /opt/openmcu-ru/bin
	$(INSTALL) -m 755 conf/web_stream_start /opt/openmcu-ru/bin
	$(INSTALL) -m 755 custom/openmcu-wrapper /opt/openmcu-ru
	$(INSTALL) -m 644 custom/openmcu.ini /opt/openmcu-ru/config

uninstall:
	( for file in ${RESOURCE_FILES}; \
	    do rm -f /opt/openmcu-ru/share/openmcu-ru/$$file; \
	done )
	( for file in ${CONFIG_FILES}; \
	    do rm -f /opt/openmcu-ru/config/$$file; \
	done )
	rm -f /opt/openmcu-ru/bin/$(PROG)
	rm -f /opt/openmcu-ru/bin/ptlib-config
	rm -f /opt/openmcu-ru/openmcu
	rm -f /opt/openmcu-ru/openmcu-ru
	rm -f /opt/openmcu-ru/bin/ffmpeg
	rm -f /opt/openmcu-ru/bin/ffserver
	rm -f /opt/openmcu-ru/bin/web_stream_start
	rm -f /opt/openmcu-ru/openmcu-wrapper
	rm -f /opt/openmcu-ru/config/openmcu.ini
#	rm -rf /opt/openmcu-ru/config
#	rm -rf /opt/openmcu-ru/pipe
#	rm -rf /opt/openmcu-ru/log
#	rm -rf /opt/openmcu-ru/records
#	rm -rf /opt/openmcu-ru/share/openmcu-ru
#	rm -rf /opt/openmcu-ru/share/openmcu-ru
#	rm -rf /opt/openmcu-ru/lib
#	rm -rf /opt/openmcu-ru/bin
