#
# Makefile
#
# Make file for FreeMCU
#

PREFIX          = /opt
OSTYPE          = linux
MACHTYPE	= x86
PROG		= freemcu
SOURCES		:= main.cxx video.cxx conference.cxx filemembers.cxx custom.cxx h323.cxx html.cxx mcu.cxx sip.cxx template.cxx \
                   mcu_rtp.cxx reg.cxx reg_sip.cxx reg_h323.cxx

CXX		= g++
CFLAGS         += -g  -Os
LDFLAGS        += -L../libs/libyuv -L../libs/sofia-sip/libsofia-sip-ua/.libs -L/opt/freemcu/lib -L/opt/git/test/libs/h323plus/lib -L/opt/git/test/libs/ptlib/lib -L/opt/lib -L/usr/lib -L/usr/local/lib
RFLAGS         += -Wl,-R/opt/freemcu/lib,-R/opt/freemcu/lib
SFLAGS         +=  -Wl,--start-group ../libs/srtp/libsrtp.a -Wl,--end-group -Wl,--start-group ../libs/libzrtp/libzrtp.a ../libs/libzrtp/third_party/bnlib/libbn.a -Wl,--end-group
LDLIBS	       +=  -lssl -lfreetype -ljpeg -lyuv -lsofia-sip-ua -lswresample -lh323_linux_x86_r -lpt_linux_x86_r
STDCCFLAGS     +=  -D__STDC_CONSTANT_MACROS -Wall
STDCXXFLAGS    += 
CPPFLAGS       += -I../libs/h323plus/include -I../libs/libyuv/include -I../libs/libzrtp/include -I../libs/libzrtp/third_party/bgaes -I../libs/libzrtp/third_party/bnlib -I../libs/ptlib/include -I../libs/sofia-sip/libsofia-sip-ua/bnf -I../libs/sofia-sip/libsofia-sip-ua/docs -I../libs/sofia-sip/libsofia-sip-ua/features -I../libs/sofia-sip/libsofia-sip-ua/http -I../libs/sofia-sip/libsofia-sip-ua/ipt -I../libs/sofia-sip/libsofia-sip-ua/iptsec -I../libs/sofia-sip/libsofia-sip-ua/msg -I../libs/sofia-sip/libsofia-sip-ua/nea -I../libs/sofia-sip/libsofia-sip-ua/nta -I../libs/sofia-sip/libsofia-sip-ua/nth -I../libs/sofia-sip/libsofia-sip-ua/nua -I../libs/sofia-sip/libsofia-sip-ua/sdp -I../libs/sofia-sip/libsofia-sip-ua/sip -I../libs/sofia-sip/libsofia-sip-ua/soa -I../libs/sofia-sip/libsofia-sip-ua/sresolv -I../libs/sofia-sip/libsofia-sip-ua/stun -I../libs/sofia-sip/libsofia-sip-ua/su -I../libs/sofia-sip/libsofia-sip-ua/tport -I../libs/sofia-sip/libsofia-sip-ua/url -I../libs/srtp/crypto/include -I../libs/srtp/include -I/opt/freemcu/include -I/opt/include -I/usr/include -I/usr/include/freetype2 -I/usr/local/include
OPTCCFLAGS      =$(CPPFLAGS)

git_revision        = $(shell git rev-list master --max-count=1 HEAD)
#git_revision_number = $(shell git rev-list master | wc -l)
STDCCFLAGS     +=-D GIT_REVISION=$(git_revision)

PTLIBDIR        = /opt/git/test/libs/ptlib
include $(PTLIBDIR)/make/ptbuildopts.mak


CONFIG_FILES= \
layouts.conf \
ffserver.conf \
freemcu.ini

RESOURCE_FILES= \
i24_clr.gif \
vad_chosenvan.gif \
i20_close.gif \
i24_left.gif \
vad_disable.gif \
i20_plus.gif \
i24_mix.gif \
mcu.gif \
vad_vad.gif \
i20_static.gif \
i24_revert.gif \
i15_inv.gif \
i20_vad2.gif \
i24_right.gif \
launched.gif \
i20_vad.gif \
i24_shuff.gif \
i15_getNoVideo.gif \
logo_text.png \
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
main.css \
i32_lock.png \
i32_lockopen.png \
s15_ch.png \

OBJDIR	= ./obj
OBJECTS = $(addprefix $(OBJDIR)/,$(patsubst %.c,%.o,$(patsubst %.cxx,%.o,$(notdir $(SOURCES)))))

$(OBJDIR)/%.o : %.cxx
	@mkdir -p $(OBJDIR) >/dev/null 2>&1
	@echo [CC] $@
	@$(CXX) $(STDCCFLAGS) $(OPTCCFLAGS) $(CFLAGS) $(STDCXXFLAGS) -c $< -o $@

all: $(OBJDIR)/$(PROG)

$(OBJDIR)/$(PROG): $(OBJECTS)
	$(CXX) $(LDSO) -o $@ $^ $(CFLAGS) $(LDFLAGS) $(SFLAGS) $(RFLAGS) $(OBJS) $(LDLIBS) $(ENDLDLIBS) $(ENDLDFLAGS)

install:
	mkdir -p $(DESTDIR)/opt/freemcu
	mkdir -p $(DESTDIR)/opt/freemcu/bin
	mkdir -p $(DESTDIR)/opt/freemcu/config
	mkdir -p $(DESTDIR)/opt/freemcu/font
	mkdir -p $(DESTDIR)/opt/freemcu/lib
	mkdir -p $(DESTDIR)/opt/freemcu/log
	mkdir -p $(DESTDIR)/opt/freemcu/pipe
	mkdir -p $(DESTDIR)/opt/freemcu/records
	mkdir -p $(DESTDIR)/opt/freemcu/resource
	mkdir -p $(DESTDIR)/opt/freemcu/ssl
	( for file in ${CONFIG_FILES}; \
	    do install -m 644 conf/$$file $(DESTDIR)/opt/freemcu/config; \
	done )
	( for file in ${RESOURCE_FILES}; \
	    do install -m 644 resource/$$file $(DESTDIR)/opt/freemcu/resource; \
	done )
	install -m 755 conf/web_stream_start $(DESTDIR)/opt/freemcu/config
	install -m 755 $(OBJDIR)/$(PROG) $(DESTDIR)/opt/freemcu/bin
	install -m 644 resource/Russo_One.ttf $(DESTDIR)/opt/freemcu/font
	install -m 644 ../AUTHORS $(DESTDIR)/opt/freemcu
	install -m 644 ../COPYING $(DESTDIR)/opt/freemcu
	install -m 644 ../NEWS $(DESTDIR)/opt/freemcu
	install -m 644 ../README $(DESTDIR)/opt/freemcu
	ln -sf $(DESTDIR)/opt/freemcu/bin/$(PROG) /usr/local/bin/$(PROG)
#	install -m 644 ssl/agent.pem $(DESTDIR)/opt/freemcu/ssl
#	install -m 644 ssl/cafile.pem $(DESTDIR)/opt/freemcu/ssl

uninstall:
	rm -rf $(DESTDIR)/opt/freemcu/font
	rm -rf $(DESTDIR)/opt/freemcu/log
	rm -rf $(DESTDIR)/opt/freemcu/pipe
	rm -rf $(DESTDIR)/opt/freemcu/resource
	rm -f $(DESTDIR)/opt/freemcu/AUTHORS
	rm -f $(DESTDIR)/opt/freemcu/COPYING
	rm -f $(DESTDIR)/opt/freemcu/NEWS
	rm -f $(DESTDIR)/opt/freemcu/README
	rm -f $(DESTDIR)/opt/freemcu/bin/$(PROG)
	rm -f /usr/bin/$(PROG)

clean:
	rm -rf $(OBJDIR)
