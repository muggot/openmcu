AC_PLUGIN_DIR=codecs/audio
prefix=/opt/openmcu-ru/lib/ptlib
target_os=linux-gnu

SONAME          = g7221
OBJDIR	        = ./obj

CC		= gcc
CXX		= g++
LDSO		= -shared -Wl,-soname,$(SONAME)
PLUGINEXT	= so
CFLAGS	       += -g -O2 
STDCCFLAGS	= -fPIC -D__STDC_CONSTANT_MACROS
LDFLAGS	       += -Wl,--start-group ../../libs/libg722_1/src/.libs/libg722_1.a -Wl,--end-group
CPPFLAGS       += -I../../libs/libg722_1/src
OPTCCFLAGS      = $(CPPFLAGS)
EXTRACFLAGS     =

SRCS   := g7221.cxx

vpath	%.cxx .
vpath	%.o   $(OBJDIR)

$(OBJDIR)/%.o : %.c
	@mkdir -p $(OBJDIR) >/dev/null 2>&1
	$(Q_CC)$(CC) -I../../../include $(STDCCFLAGS) $(OPTCCFLAGS) $(EXTRACFLAGS) $(CFLAGS) -c $< -o $@

$(OBJDIR)/%.o : %.cxx
	@mkdir -p $(OBJDIR) >/dev/null 2>&1
	$(CXX) $(EXTRACCFLAGS) $(STDCCFLAGS) $(OPTCCFLAGS) $(CFLAGS) -c $< -o $@

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
	mkdir -p $(DESTDIR)$(prefix)/$(AC_PLUGIN_DIR)
	install $(PLUGIN) $(DESTDIR)$(prefix)/$(AC_PLUGIN_DIR)

uninstall:
	rm -f $(DESTDIR)$(prefix)/$(AC_PLUGIN_DIR)/$(PLUGIN)

clean:
	rm -f $(OBJECTS) $(PLUGIN)

###########################################
