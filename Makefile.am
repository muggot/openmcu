## Process this file with automake to produce Makefile.in

SUBDIRS 	= @PTLIBDIR@ @H323PLUSDIR@ @MCUDIR@

all:
	($(MAKE) $(MAKEFLAGS) opt -C @PTLIBDIR@)    || exit $$?
	($(MAKE) $(MAKEFLAGS)     -C @H323PLUSDIR@) || exit $$?
	($(MAKE) $(MAKEFLAGS)     -C @MCUDIR@)      || exit $$?
	( for dir in @PLUGIN_DIRS@; \
		do ($(MAKE) $(MAKEFLAGS) -C $$dir) || exit $$?; \
	done )

install:
if DISABLE_STATIC
	($(MAKE) install -C @PTLIBDIR@)    || exit $$?
	($(MAKE) install -C @H323PLUSDIR@) || exit $$?
endif
	($(MAKE) install -C @MCUDIR@)      || exit $$?
	( for dir in @PLUGIN_DIRS@; \
		do ($(MAKE) install -C $$dir) || exit $$?; \
	done )

uninstall:
if DISABLE_STATIC
	($(MAKE) uninstall -C @PTLIBDIR@)    || exit $$?
	($(MAKE) uninstall -C @H323PLUSDIR@) || exit $$?
endif
	($(MAKE) uninstall -C @MCUDIR@)      || exit $$?
	( for dir in @PLUGIN_DIRS@; \
		do ($(MAKE) uninstall -C $$dir) || exit $$?; \
	done )

clean:
	($(MAKE) clean -C @PTLIBDIR@)    || exit $$?
	($(MAKE) clean -C @H323PLUSDIR@) || exit $$?
	($(MAKE) clean -C @MCUDIR@)      || exit $$?
	( for dir in @PLUGIN_DIRS@; \
		do ($(MAKE) clean -C $$dir) || exit $$?; \
	done )

@PTLIBDIR@:
	cd @PTLIBDIR@ && $(MAKE) $(MAKEFLAGS) opt

@H323PLUSDIR@:
	cd @H323PLUSDIR@ && $(MAKE) $(MAKEFLAGS)

@MCUDIR@:
	cd @MCUDIR@ && $(MAKE) $(MAKEFLAGS)

h263:
	cd @PLUGIN_H263_DIR@ && $(MAKE) $(MAKEFLAGS)

h264:
	cd @PLUGIN_H264_DIR@ && $(MAKE) $(MAKEFLAGS)

vp8:
	cd @PLUGIN_VP8_DIR@ && $(MAKE) $(MAKEFLAGS)

g722:
	cd @PLUGIN_G722_DIR@ && $(MAKE) $(MAKEFLAGS)

g7231:
	cd @PLUGIN_G7231_DIR@ && $(MAKE) $(MAKEFLAGS)

g726:
	cd @PLUGIN_G726_DIR@ && $(MAKE) $(MAKEFLAGS)

ilbc:
	cd @PLUGIN_ILBC_DIR@ && $(MAKE) $(MAKEFLAGS)

speex:
	cd @PLUGIN_SPEEX_DIR@ && $(MAKE) $(MAKEFLAGS)

silk:
	cd @PLUGIN_SILK_DIR@ && $(MAKE) $(MAKEFLAGS)

opus:
	cd @PLUGIN_OPUS_DIR@ && $(MAKE) $(MAKEFLAGS)

.PHONY: @PTLIBDIR@ @H323PLUSDIR@ @MCUDIR@ @PLUGINS@
