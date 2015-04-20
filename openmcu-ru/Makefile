#
# Makefile
#

PREFIX          = /opt
OSTYPE          = linux
UNAME          := $(shell uname)
MACHTYPE	= x86
PROG		= openmcu-ru
SOURCES	       := main.cxx video.cxx conference.cxx filemembers.cxx custom.cxx h323.cxx html.cxx mcu.cxx sip.cxx template.cxx \
                   utils.cxx utils_av.cxx utils_list.cxx utils_type.cxx utils_json.cxx yuv.cxx \
                   mcu_rtp.cxx mcu_rtp_cache.cxx mcu_rtp_secure.cxx \
                   sockets.cxx telnet.cxx \
                   reg.cxx reg_sip.cxx reg_h323.cxx rtsp.cxx recorder.cxx mcu_caps.cxx mcu_codecs.cxx

CXX		= g++
CFLAGS         += -g -O2 
LDFLAGS        += -L../libs/libyuv -L../libs/sofia-sip/libsofia-sip-ua/.libs -L/opt/git/test/libs/h323plus/lib -L/opt/git/test/libs/ptlib/lib -L/opt/lib -L/opt/openmcu-ru/lib -L/usr/lib -L/usr/local/lib
RFLAGS         += -Wl,-R/opt/openmcu-ru/lib
SFLAGS         += 
SFLAGS_STATIC  +=  -Wl,--start-group /opt/git/test/libs/ptlib/lib/libpt_linux_x86_r_s.a /opt/git/test/libs/h323plus/lib/libh323_linux_x86_r_s.a -Wl,--end-group
SFLAGS_DEBUG   +=  -Wl,--start-group /opt/git/test/libs/ptlib/lib/libpt_linux_x86_d_s.a /opt/git/test/libs/h323plus/lib/libh323_linux_x86_d_s.a -Wl,--end-group
LDLIBS	       +=  -lavcodec -lavutil -lavformat -lswscale -lsofia-sip-ua -lswresample -lfreetype -lyuv -lssl -lsrtp -lh323_linux_x86_r -lpt_linux_x86_r
LDLIBS_STATIC  +=  -lavcodec -lavutil -lavformat -lswscale -lsofia-sip-ua -lswresample -lfreetype -lyuv -lssl -lsrtp
LDLIBS_DEBUG   +=  -lavcodec -lavutil -lavformat -lswscale -lsofia-sip-ua -lswresample -lfreetype -lyuv -lssl -lsrtp
STDCCFLAGS     +=  -D__STDC_CONSTANT_MACROS -Wall
STDCXXFLAGS    += 
CPPFLAGS       += -I../libs/h323plus/include -I../libs/libyuv/include -I../libs/ptlib/include -I../libs/sofia-sip/libsofia-sip-ua/bnf -I../libs/sofia-sip/libsofia-sip-ua/docs -I../libs/sofia-sip/libsofia-sip-ua/features -I../libs/sofia-sip/libsofia-sip-ua/http -I../libs/sofia-sip/libsofia-sip-ua/ipt -I../libs/sofia-sip/libsofia-sip-ua/iptsec -I../libs/sofia-sip/libsofia-sip-ua/msg -I../libs/sofia-sip/libsofia-sip-ua/nea -I../libs/sofia-sip/libsofia-sip-ua/nta -I../libs/sofia-sip/libsofia-sip-ua/nth -I../libs/sofia-sip/libsofia-sip-ua/nua -I../libs/sofia-sip/libsofia-sip-ua/sdp -I../libs/sofia-sip/libsofia-sip-ua/sip -I../libs/sofia-sip/libsofia-sip-ua/soa -I../libs/sofia-sip/libsofia-sip-ua/sresolv -I../libs/sofia-sip/libsofia-sip-ua/stun -I../libs/sofia-sip/libsofia-sip-ua/su -I../libs/sofia-sip/libsofia-sip-ua/tport -I../libs/sofia-sip/libsofia-sip-ua/url -I/opt/include -I/opt/openmcu-ru/include -I/usr/include -I/usr/include/freetype2 -I/usr/include/srtp/ -I/usr/local/include
OPTCCFLAGS      =$(CPPFLAGS)

git_revision        = $(shell git rev-list HEAD --max-count=1)
#git_revision_number = $(shell git rev-list HEAD | wc -l)
STDCCFLAGS     +=-D GIT_REVISION=$(git_revision)

PTLIBDIR        = /opt/git/test/libs/ptlib
include $(PTLIBDIR)/make/ptbuildopts.mak

INSTALL = install
INSTALL_BACKUP_OPTS =
ifeq ($(UNAME), Linux)
  INSTALL = install
  INSTALL_BACKUP_OPTS = --compare --backup=numbered
endif
ifeq ($(UNAME), FreeBSD)
ifeq (, $(shell which ginstall))
  INSTALL = install
  INSTALL_BACKUP_OPTS = -C -b
else
  INSTALL = ginstall
  INSTALL_BACKUP_OPTS = --compare --backup=numbered
endif
endif


OBJDIR	= ./obj
OBJECTS = $(addprefix $(OBJDIR)/,$(patsubst %.c,%.o,$(patsubst %.cxx,%.o,$(notdir $(SOURCES)))))

$(OBJDIR)/%.o : %.cxx
	@mkdir -p $(OBJDIR) >/dev/null 2>&1
	@echo [CC] $@
	@$(CXX) $(STDCCFLAGS) $(OPTCCFLAGS) $(CFLAGS) $(STDCXXFLAGS) -c $< -o $@

all: $(OBJECTS)
	$(CXX) $(LDSO) -o $(OBJDIR)/$(PROG) $^ $(CFLAGS) $(LDFLAGS) $(SFLAGS) $(RFLAGS) $(OBJS) $(LDLIBS) $(ENDLDLIBS) $(ENDLDFLAGS)

static: $(OBJECTS)
	$(CXX) $(LDSO) -o $(OBJDIR)/$(PROG) $^ $(CFLAGS) $(LDFLAGS) $(SFLAGS_STATIC) $(RFLAGS) $(OBJS) $(LDLIBS_STATIC) $(ENDLDLIBS) $(ENDLDFLAGS)

debug: $(OBJECTS)
	$(CXX) $(LDSO) -o $(OBJDIR)/$(PROG) $^ $(CFLAGS) $(LDFLAGS) $(SFLAGS_DEBUG) $(RFLAGS) $(OBJS) $(LDLIBS_DEBUG) $(ENDLDLIBS) $(ENDLDFLAGS)


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
	    $(INSTALL) -m 644 $(INSTALL_BACKUP_OPTS) $$file $(DESTDIR)/opt/openmcu-ru/config; \
	done )
	( for file in ./files/font/*; do \
	    $(INSTALL) -m 644 $$file $(DESTDIR)/opt/openmcu-ru/font; \
	done )
	( for file in ./files/resource/*; do \
	    $(INSTALL) -m 644 $$file $(DESTDIR)/opt/openmcu-ru/resource; \
	done )
	( for file in ./files/scripts/*; do \
	    $(INSTALL) -m 755 $(INSTALL_BACKUP_OPTS) $$file $(DESTDIR)/opt/openmcu-ru/scripts; \
	done )
	$(INSTALL) -m 755 $(OBJDIR)/$(PROG) $(DESTDIR)/opt/openmcu-ru/bin
#	ln -sf $(DESTDIR)/opt/openmcu-ru/bin/$(PROG) /usr/local/bin/$(PROG)
	$(INSTALL) -m 644 ../AUTHORS $(DESTDIR)/opt/openmcu-ru
	$(INSTALL) -m 644 ../COPYING $(DESTDIR)/opt/openmcu-ru
	$(INSTALL) -m 644 ../NEWS $(DESTDIR)/opt/openmcu-ru
	$(INSTALL) -m 644 ../README $(DESTDIR)/opt/openmcu-ru

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
