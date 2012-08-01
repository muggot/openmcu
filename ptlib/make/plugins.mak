ifndef PTLIBDIR
PTLIBDIR=$(HOME)/ptlib
endif

include ../../make/unix.mak

PLUGIN_FILENAME = $(PLUGIN_NAME)_pwplugin.$(LIB_SUFFIX)

OBJDIR = ../ptlib/$(PLUGIN_FAMILY)

TARGET = $(OBJDIR)/$(PLUGIN_FILENAME)

ifeq ($(OSTYPE),solaris)
  LDSOPTS += -G
else
  ifneq ($(OSTYPE),Darwin)
    LDSOPTS += -shared
  endif
endif

ifeq ($(MACHTYPE),x86_64)
  STDCCFLAGS += -fPIC
endif

ifeq ($(MACHTYPE),hppa)
  STDCCFLAGS += -fPIC
endif

$(OBJDIR)/$(PLUGIN_FILENAME): $(PLUGIN_SOURCES)
	mkdir -p $(OBJDIR)
	$(CPLUS) $(CFLAGS) $(STDCCFLAGS) \
	$(LDFLAGS) \
	$(PLUGIN_LIBS) \
	-I. $(LDSOPTS) $< -o $@

OBJS	 := $(patsubst %.c, $(OBJDIR)/%.o, $(patsubst %.cxx, $(OBJDIR)/%.o, $(notdir $(PLUGIN_SOURCES))))

CLEAN_FILES += $(OBJDIR)/$(PLUGIN_FILENAME)

include ../../make/common.mak
