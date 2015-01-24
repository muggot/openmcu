#
# Makefile
#

PREFIX          = /opt
OSTYPE          = linux
MACHTYPE	= x86
PROG		= openmcu-ru
SOURCES		:= main.cxx video.cxx conference.cxx filemembers.cxx custom.cxx h323.cxx html.cxx mcu.cxx sip.cxx template.cxx \
                   utils.cxx utils_av.cxx \
                   mcu_rtp.cxx reg.cxx reg_sip.cxx reg_h323.cxx rtsp.cxx recorder.cxx mcu_caps.cxx mcu_codecs.cxx

CXX		= g++
CFLAGS         += -g -g -O2 -Os
LDFLAGS        += -L../libs/libyuv -L../libs/sofia-sip/libsofia-sip-ua/.libs -L/opt/git/test/libs/h323plus/lib -L/opt/git/test/libs/ptlib/lib -L/opt/lib -L/opt/openmcu-ru/lib -L/usr/lib -L/usr/local/lib
RFLAGS         += -Wl,-R/opt/openmcu-ru/lib,-R/opt/openmcu-ru/lib
SFLAGS         += 
LDLIBS	       +=  -lssl -lsrtp -lfreetype -lyuv -lsofia-sip-ua -lavcodec -lavutil -lavformat -lswscale -lswresample -lh323_linux_x86_r -lpt_linux_x86_r
STDCCFLAGS     +=  -D__STDC_CONSTANT_MACROS -Wall
STDCXXFLAGS    += 
CPPFLAGS       += -I../libs/h323plus/include -I../libs/libyuv/include -I../libs/ptlib/include -I../libs/sofia-sip/libsofia-sip-ua/bnf -I../libs/sofia-sip/libsofia-sip-ua/docs -I../libs/sofia-sip/libsofia-sip-ua/features -I../libs/sofia-sip/libsofia-sip-ua/http -I../libs/sofia-sip/libsofia-sip-ua/ipt -I../libs/sofia-sip/libsofia-sip-ua/iptsec -I../libs/sofia-sip/libsofia-sip-ua/msg -I../libs/sofia-sip/libsofia-sip-ua/nea -I../libs/sofia-sip/libsofia-sip-ua/nta -I../libs/sofia-sip/libsofia-sip-ua/nth -I../libs/sofia-sip/libsofia-sip-ua/nua -I../libs/sofia-sip/libsofia-sip-ua/sdp -I../libs/sofia-sip/libsofia-sip-ua/sip -I../libs/sofia-sip/libsofia-sip-ua/soa -I../libs/sofia-sip/libsofia-sip-ua/sresolv -I../libs/sofia-sip/libsofia-sip-ua/stun -I../libs/sofia-sip/libsofia-sip-ua/su -I../libs/sofia-sip/libsofia-sip-ua/tport -I../libs/sofia-sip/libsofia-sip-ua/url -I/opt/include -I/opt/openmcu-ru/include -I/usr/include -I/usr/include/freetype2 -I/usr/include/srtp/ -I/usr/local/include
OPTCCFLAGS      =$(CPPFLAGS)

git_revision        = $(shell git rev-list master --max-count=1 HEAD)
#git_revision_number = $(shell git rev-list master | wc -l)
STDCCFLAGS     +=-D GIT_REVISION=$(git_revision)

PTLIBDIR        = /opt/git/test/libs/ptlib
include $(PTLIBDIR)/make/ptbuildopts.mak


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
	mkdir -p $(DESTDIR)/opt/openmcu-ru
	mkdir -p $(DESTDIR)/opt/openmcu-ru/bin
	mkdir -p $(DESTDIR)/opt/openmcu-ru/config
	mkdir -p $(DESTDIR)/opt/openmcu-ru/font
	mkdir -p $(DESTDIR)/opt/openmcu-ru/lib
	mkdir -p $(DESTDIR)/opt/openmcu-ru/log
	mkdir -p $(DESTDIR)/opt/openmcu-ru/pipe
	mkdir -p $(DESTDIR)/opt/openmcu-ru/records
	mkdir -p $(DESTDIR)/opt/openmcu-ru/resource
	mkdir -p $(DESTDIR)/opt/openmcu-ru/ssl
	mkdir -p $(DESTDIR)/opt/openmcu-ru/scripts
	( for file in ./files/conf/*; do \
	    install -m 644 $$file $(DESTDIR)/opt/openmcu-ru/config; \
	done )
	( for file in ./files/font/*; do \
	    install -m 644 $$file $(DESTDIR)/opt/openmcu-ru/font; \
	done )
	( for file in ./files/resource/*; do \
	    install -m 644 $$file $(DESTDIR)/opt/openmcu-ru/resource; \
	done )
	( for file in ./files/scripts/*; do \
	    install -m 755 $$file $(DESTDIR)/opt/openmcu-ru/scripts; \
	done )
	install -m 755 $(OBJDIR)/$(PROG) $(DESTDIR)/opt/openmcu-ru/bin
#	ln -sf $(DESTDIR)/opt/openmcu-ru/bin/$(PROG) /usr/local/bin/$(PROG)
	install -m 644 ../AUTHORS $(DESTDIR)/opt/openmcu-ru
	install -m 644 ../COPYING $(DESTDIR)/opt/openmcu-ru
	install -m 644 ../NEWS $(DESTDIR)/opt/openmcu-ru
	install -m 644 ../README $(DESTDIR)/opt/openmcu-ru

uninstall:
	rm -rf $(DESTDIR)/opt/openmcu-ru/font
	rm -rf $(DESTDIR)/opt/openmcu-ru/log
	rm -rf $(DESTDIR)/opt/openmcu-ru/pipe
	rm -rf $(DESTDIR)/opt/openmcu-ru/resource
	rm -rf $(DESTDIR)/opt/openmcu-ru/scripts
	rm -f $(DESTDIR)/opt/openmcu-ru/AUTHORS
	rm -f $(DESTDIR)/opt/openmcu-ru/COPYING
	rm -f $(DESTDIR)/opt/openmcu-ru/NEWS
	rm -f $(DESTDIR)/opt/openmcu-ru/README
	rm -f $(DESTDIR)/opt/openmcu-ru/bin/$(PROG)
#	rm -f /usr/local/bin/$(PROG)

clean:
	rm -rf $(OBJDIR)
