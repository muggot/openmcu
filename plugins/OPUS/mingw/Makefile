AC_PLUGIN_DIR=/codecs/audio
prefix=/usr
exec_prefix=${prefix}
libdir=${exec_prefix}/lib
target_os=linux-gnu

SONAME	= opus
OBJDIR	= ./obj
PLUGINDIR=../..

CC		=gcc -std=gnu99
CXX		=g++
LDSO		=-shared
PLUGINEXT	=dll
STDCCFLAGS	+=-fPIC -g
EXTRACFLAGS     =-I/usr/local/include
#WARNINGS	=-Wall -W -Wstrict-prototypes -Wextra -Wcast-align -Wnested-externs -Wshadow
CFLAGS		=-O2 -g $(WARNINGS) -DOPUS_BUILD -DVAR_ARRAYS -DLINK_OPUS_FLAG
LDFLAGS  	=-L/local/lib -lopus

SRCS	= ../opuscodec.cxx

vpath	%.o $(OBJDIR)
vpath	%.c $(SRCDIR)
vpath	%.cxx .

ifeq ($(VERBOSE),)
Q_CC = @echo [CC] `echo $< | sed s^@OPALDIR@/^^` ;
Q_LD = @echo [LD] `echo $(PLUGIN) | sed s^@OPALDIR@/^^` ;
endif


$(OBJDIR)/%.o : ../%.c
	@mkdir -p $(OBJDIR) >/dev/null 2>&1
	$(Q_CC)$(CC) $(STDCCFLAGS) $(OPTCCFLAGS) $(EXTRACFLAGS) $(CFLAGS) -c $< -o $@

$(OBJDIR)/%.o : ../%.cxx
	@mkdir -p $(OBJDIR) >/dev/null 2>&1
	$(CXX) $(EXTRACCFLAGS) $(STDCCFLAGS) $(OPTCCFLAGS) $(EXTRACFLAGS) $(CFLAGS) -c $< -o $@

PLUGIN	= ./$(SONAME)_audio_pwplugin.$(PLUGINEXT)

OBJECTS = $(addprefix $(OBJDIR)/,$(patsubst %.c,%.o,$(patsubst %.cxx,%.o,$(notdir $(SRCS)))))

all: $(PLUGIN)

$(PLUGIN): $(OBJECTS)
ifeq (solaris,$(findstring solaris,$(target_os)))

	$(Q_LD)$(CXX) $(LDSO) $@ -o $@ $^ $(EXTRALIBS) $(LDFLAGS)
else
	$(Q_LD)$(CXX) $(LDSO) -o $@ $^ $(EXTRALIBS) $(LDFLAGS)

endif	

install:
	mkdir -p $(DESTDIR)$(libdir)/$(AC_PLUGIN_DIR)
	install $(PLUGIN) $(DESTDIR)$(libdir)/$(AC_PLUGIN_DIR)

uninstall:
	rm -f $(DESTDIR)$(libdir)/$(AC_PLUGIN_DIR)/$(PLUGIN)

clean:
	rm -f $(OBJECTS) $(PLUGIN)

###########################################

