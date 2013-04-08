PREFIX=/usr/local
exec_prefix = ${PREFIX}
LIBDIR=${exec_prefix}/lib

SONAME	= g7231

SRCDIR	= ./src
OBJDIR	= ./obj

CC	 	=gcc
CFLAGS	 	=-Os -D__STDC_CONSTANT_MACROS
CXX	 	=g++
LDSO	 	=-shared -Wl,-soname,$(SONAME)
PLUGINEXT	=so
STDCCFLAGS	=-fPIC
LDFLAGS		=-L${PREFIX}/lib

SRCS	+= g7231codec.cxx

vpath	%.o $(OBJDIR)
vpath	%.cxx $(SRCDIR)

$(OBJDIR)/%.o : %.cxx
	@mkdir -p $(OBJDIR) >/dev/null 2>&1
	$(CXX) -I${PREFIX}/include $(STDCCFLAGS) $(OPTCCFLAGS) $(CFLAGS) -c $< -o $@

PLUGIN	= ./g7231_audio_pwplugin.$(PLUGINEXT)

OBJECTS = $(addprefix $(OBJDIR)/,$(patsubst %.cxx,%.o,$(notdir $(SRCS))))

$(PLUGIN): $(OBJECTS)
#	$(CXX) $(LDSO) $(DL_LIBS) $@ -o $@ $^ $(LDFLAGS)
	$(CXX) $(LDSO) -o $@ $^ $(EXTRALIBS) $(LDFLAGS) -lavutil -lavcodec

install:
	mkdir -p $(DESTDIR)$(LIBDIR)/pwlib/codecs/audio/
	cp $(PLUGIN) $(DESTDIR)$(LIBDIR)/pwlib/codecs/audio/

uninstall:
	rm -f $(DESTDIR)$(LIBDIR)/pwlib/codecs/audio/$(PLUGIN)

clean:
	rm -f $(OBJECTS) $(PLUGIN)
