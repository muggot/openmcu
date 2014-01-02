#
# Makefile
#
# Make file for OpenMCU-ru
#

PREFIX          = /usr/local
OSTYPE          = linux
MACHTYPE	= x86
PROG		= openmcu-ru
SOURCES		:= main.cxx video.cxx conference.cxx filemembers.cxx custom.cxx h323.cxx html.cxx mcu.cxx sip.cxx template.cxx

CXX		= g++
CFLAGS         += -g  -Os
LDFLAGS        +=  -Wl,--start-group ../ptlib/lib/libpt_linux_x86_r_s.a ../h323plus/lib/libh323_linux_x86_r_s.a -Wl,--end-group -L/usr/lib -L/usr/local/lib -Wl,-R/usr/local/lib
LDLIBS	       +=   -lfreetype -lswresample -ljpeg -lyuv -lsofia-sip-ua
STDCCFLAGS     +=  -D__STDC_CONSTANT_MACROS -Wall
STDCXXFLAGS    += 
CPPFLAGS       += -I../ptlib/include -I../h323plus/include -I/usr/include -I/usr/include/freetype2 -I/usr/local/include -I/usr/local/include/sofia-sip-1.12
OPTCCFLAGS      =$(CPPFLAGS)

git_revision        = $(shell git rev-list master --max-count=1 HEAD)
#git_revision_number = $(shell git rev-list master | wc -l)
STDCCFLAGS     +=-D GIT_REVISION=$(git_revision)

PTLIBDIR        = ../ptlib
include ../ptlib/make/ptbuildopts.mak


CONFIG_FILES= \
layouts.conf \
ffserver.conf \
openmcu.ini

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
i15_inv.gif \
i20_vad2.gif \
i24_right.gif \
openmcu.ru_launched_Ypf.gif \
i15_mic_off.gif \
i20_vad.gif \
i24_shuff.gif \
i15_getNoVideo.gif \
openmcu.ru_logo_text.png \
menu_left.png \
connecting.wav \
entering.wav \
leaving.wav \
template.html \
control.js \
status.js \
locale_ru.js \
locale_en.js \
i16_close_gray.png \
i16_close_red.png \

OBJDIR	= ./obj
OBJECTS = $(addprefix $(OBJDIR)/,$(patsubst %.c,%.o,$(patsubst %.cxx,%.o,$(notdir $(SOURCES)))))

$(OBJDIR)/%.o : %.cxx
	@mkdir -p $(OBJDIR) >/dev/null 2>&1
	$(CXX) $(STDCCFLAGS) $(OPTCCFLAGS) $(CFLAGS) $(STDCXXFLAGS) -c $< -o $@

all: $(OBJDIR)/$(PROG)

$(OBJDIR)/$(PROG): $(OBJECTS)
	$(CXX) $(LDSO) -o $@ $^ $(CFLAGS) $(LDFLAGS) $(OBJS) $(LDLIBS) $(ENDLDLIBS) $(ENDLDFLAGS)

install:
	mkdir -p $(DESTDIR)/usr/local/bin
	mkdir -p $(DESTDIR)/usr/local/share/openmcu-ru
	mkdir -p $(DESTDIR)/usr/local/share/openmcu-ru/config
	mkdir -p $(DESTDIR)/usr/local/share/openmcu-ru/font
	mkdir -p $(DESTDIR)/usr/local/share/openmcu-ru/log
	mkdir -p $(DESTDIR)/usr/local/share/openmcu-ru/pipe
	mkdir -p $(DESTDIR)/usr/local/share/openmcu-ru/records
	mkdir -p $(DESTDIR)/usr/local/share/openmcu-ru/resource
	( for file in ${CONFIG_FILES}; \
	    do install -m 644 conf/$$file $(DESTDIR)/usr/local/share/openmcu-ru/config; \
	done )
	( for file in ${RESOURCE_FILES}; \
	    do install -m 644 resource/$$file $(DESTDIR)/usr/local/share/openmcu-ru/resource; \
	done )
	install -m 755 conf/web_stream_start $(DESTDIR)/usr/local/share/openmcu-ru/config
	install -m 755 $(OBJDIR)/$(PROG) $(DESTDIR)/usr/local/bin
	install -m 644 resource/Russo_One.ttf $(DESTDIR)/usr/local/share/openmcu-ru/font
	install -m 644 ../AUTHORS $(DESTDIR)/usr/local/share/openmcu-ru
	install -m 644 ../COPYING $(DESTDIR)/usr/local/share/openmcu-ru
	install -m 644 ../NEWS $(DESTDIR)/usr/local/share/openmcu-ru
	install -m 644 ../README $(DESTDIR)/usr/local/share/openmcu-ru
ifeq (0, 0)
ifeq (1, 0)
	mkdir -p $(DESTDIR)/usr/local/lib
	install -m 644 ../ptlib/lib/libpt_${OSTYPE}_${MACHTYPE}_r.so.2.0.1 $(DESTDIR)/usr/local/lib
	install -m 644 ../h323plus/lib/libh323_${OSTYPE}_${MACHTYPE}_r.so.1.20.2 $(DESTDIR)/usr/local/lib
endif
endif

uninstall:
	rm -f $(DESTDIR)/usr/local/bin/openmcu-ru
	rm -rf $(DESTDIR)/usr/local/share/openmcu-ru/font
	rm -rf $(DESTDIR)/usr/local/share/openmcu-ru/log
	rm -rf $(DESTDIR)/usr/local/share/openmcu-ru/pipe
	rm -rf $(DESTDIR)/usr/local/share/openmcu-ru/resource
	rm -f $(DESTDIR)/usr/local/share/openmcu-ru/AUTHORS
	rm -f $(DESTDIR)/usr/local/share/openmcu-ru/COPYING
	rm -f $(DESTDIR)/usr/local/share/openmcu-ru/NEWS
	rm -f $(DESTDIR)/usr/local/share/openmcu-ru/README
ifeq (0, 0)
ifeq (1, 0)
	rm -f $(DESTDIR)/usr/local/lib/libpt_${OSTYPE}_${MACHTYPE}_r.so.2.0.1
	rm -f $(DESTDIR)/usr/local/lib/libh323_${OSTYPE}_${MACHTYPE}_r.so.1.20.2
endif
endif

clean:
	rm -rf $(OBJDIR)
