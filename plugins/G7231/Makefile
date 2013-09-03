
AC_PLUGIN_DIR=codecs/audio
prefix=/usr/local/lib/ptlib
exec_prefix = ${prefix}

SONAME	= g7231

SRCDIR	= ./src
OBJDIR	= ./obj

CC	 	=gcc
CFLAGS	 	=-Os -D__STDC_CONSTANT_MACROS
CXX	 	=g++
LDSO	 	=-shared -Wl,-soname,$(SONAME)
PLUGINEXT	=so
STDCCFLAGS	= -I/usr/include/freetype2     -I/usr/include/sofia-sip-1.12   -fPIC
LDFLAGS		=-L${prefix}/lib -lavutil -lavcodec

SRCS	+= g7231codec.cxx

vpath	%.o $(OBJDIR)
vpath	%.cxx $(SRCDIR)

$(OBJDIR)/%.o : %.cxx
	@mkdir -p $(OBJDIR) >/dev/null 2>&1
	$(CXX) -I${prefix}/include $(STDCCFLAGS) $(OPTCCFLAGS) $(CFLAGS) -c $< -o $@

PLUGIN	= ./g7231_audio_pwplugin.$(PLUGINEXT)

OBJECTS = $(addprefix $(OBJDIR)/,$(patsubst %.cxx,%.o,$(notdir $(SRCS))))

$(PLUGIN): $(OBJECTS)
#	$(CXX) $(LDSO) $(DL_LIBS) $@ -o $@ $^ $(LDFLAGS)
	$(CXX) $(LDSO) -o $@ $^ $(EXTRALIBS) $(LDFLAGS)

install:
	mkdir -p $(DESTDIR)$(prefix)/$(AC_PLUGIN_DIR)
	cp $(PLUGIN) $(DESTDIR)$(prefix)/$(AC_PLUGIN_DIR)

uninstall:
	rm -f $(DESTDIR)$(prefix)/$(AC_PLUGIN_DIR)/$(PLUGIN)

clean:
	rm -f $(OBJECTS) $(PLUGIN)
