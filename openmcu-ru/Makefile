#
# Makefile
#
# Make file for OpenMCU-ru
#

PREFIX          = /usr/local
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
quote.txt

install:
	mkdir -p $(DESTDIR)/usr/local/share/openmcu-ru/bin
	mkdir -p $(DESTDIR)/usr/local/share/openmcu-ru/config
	mkdir -p $(DESTDIR)/usr/local/share/openmcu-ru/font
	mkdir -p $(DESTDIR)/usr/local/share/openmcu-ru/log
	mkdir -p $(DESTDIR)/usr/local/share/openmcu-ru/pipe
	mkdir -p $(DESTDIR)/usr/local/share/openmcu-ru/records
	mkdir -p $(DESTDIR)/usr/local/share/openmcu-ru/resource
	( for file in ${CONFIG_FILES}; \
	    do $(INSTALL) -m 644 conf/$$file $(DESTDIR)/usr/local/share/openmcu-ru/config; \
	done )
	( for file in ${RESOURCE_FILES}; \
	    do $(INSTALL) -m 644 conf/$$file $(DESTDIR)/usr/local/share/openmcu-ru/resource; \
	done )
	$(INSTALL) -m 755 obj_linux_$(MACHTYPE)_r/$(PROG) $(DESTDIR)/usr/local/share/openmcu-ru/bin
	$(INSTALL) -m 755 conf/web_stream_start $(DESTDIR)/usr/local/share/openmcu-ru/bin
	$(INSTALL) -m 755 conf/ffmpeg $(DESTDIR)/usr/local/share/openmcu-ru/bin
	$(INSTALL) -m 755 conf/ffserver $(DESTDIR)/usr/local/share/openmcu-ru/bin
	$(INSTALL) -m 755 conf/web_stream_start $(DESTDIR)/usr/local/share/openmcu-ru/bin
	$(INSTALL) -m 755 custom/openmcu-wrapper $(DESTDIR)/usr/local/share/openmcu-ru/bin
	rm -f $(DESTDIR)/usr/local/bin/openmcu-ru
	ln -s /usr/local/share/openmcu-ru/bin/openmcu-wrapper $(DESTDIR)/usr/local/bin/openmcu-ru
	$(INSTALL) -m 644 custom/openmcu.ini $(DESTDIR)/usr/local/share/openmcu-ru/config
	$(INSTALL) -m 644 conf/Russo_One.ttf $(DESTDIR)/usr/local/share/openmcu-ru/font
	$(INSTALL) -m 644 ../AUTHORS $(DESTDIR)/usr/local/share/openmcu-ru
	$(INSTALL) -m 644 ../COPYING $(DESTDIR)/usr/local/share/openmcu-ru
	$(INSTALL) -m 644 ../NEWS $(DESTDIR)/usr/local/share/openmcu-ru
	$(INSTALL) -m 644 ../README $(DESTDIR)/usr/local/share/openmcu-ru

uninstall:
	rm -f $(DESTDIR)/usr/local/bin/openmcu-ru
	rm -rf $(DESTDIR)/usr/local/share/openmcu-ru/bin
	rm -rf $(DESTDIR)/usr/local/share/openmcu-ru/config
	rm -rf $(DESTDIR)/usr/local/share/openmcu-ru/font
	rm -rf $(DESTDIR)/usr/local/share/openmcu-ru/log
	rm -rf $(DESTDIR)/usr/local/share/openmcu-ru/pipe
	rm -rf $(DESTDIR)/usr/local/share/openmcu-ru/records
	rm -rf $(DESTDIR)/usr/local/share/openmcu-ru/resource
	rm -f $(DESTDIR)/usr/local/share/openmcu-ru/AUTHORS
	rm -f $(DESTDIR)/usr/local/share/openmcu-ru/COPYING
	rm -f $(DESTDIR)/usr/local/share/openmcu-ru/NEWS
	rm -f $(DESTDIR)/usr/local/share/openmcu-ru/README
	rm -f $(DESTDIR)/usr/local/bin/ptlib-config
	rm -rf $(DESTDIR)/usr/local/lib/ptlib
