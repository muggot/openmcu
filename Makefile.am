## Process this file with automake to produce Makefile.in

SUBDIRS 	= @PTLIBDIR@ @H323PLUSDIR@ @MCUDIR@

####################################################################################################

all:
	($(MAKE) $(MAKEFLAGS) @PTLIB_BUILDOPTS@    -C @PTLIBDIR@) || exit $$?
	($(MAKE) $(MAKEFLAGS) @H323PLUS_BUILDOPTS@ -C @H323PLUSDIR@) || exit $$?
if COMPILER_USE_LIBYUV
	cd @LIBYUV_DIR@ && \
	( for file in source/*.cc; do \
	    echo $$file ; gcc -fPIC -c -fno-stack-protector -Iinclude $$file -o $$file.o ; \
	  done \
	) \
	&& \
	gcc -shared -Wl,-soname,libyuv.so -o libyuv.so source/*.o -lstdc++
endif
if COMPILER_USE_LIBSRTP
	($(MAKE) $(MAKEFLAGS)     -C @LIBSRTP_DIR@) || exit $$?
endif
if COMPILER_USE_LIBZRTP
	($(MAKE) $(MAKEFLAGS)     -C @LIBZRTP_DIR@) || exit $$?
endif
if COMPILER_USE_LIBSOFIA
	($(MAKE) $(MAKEFLAGS)     -C @LIBSOFIA_DIR@) || exit $$?
endif
if COMPILER_USE_LIBSPEEX
	($(MAKE) $(MAKEFLAGS)     -C @LIBSPEEX_DIR@) || exit $$?
endif
if COMPILER_USE_LIBOPUS
	($(MAKE) $(MAKEFLAGS)     -C @LIBOPUS_DIR@) || exit $$?
endif
if COMPILER_USE_LIBG7221
	($(MAKE) $(MAKEFLAGS)     -C @LIBG7221_DIR@) || exit $$?
endif
	($(MAKE) $(MAKEFLAGS) @MCU_BUILDOPTS@      -C @MCUDIR@) || exit $$?
	( for dir in @PLUGIN_DIRS@; \
		do ($(MAKE) $(MAKEFLAGS) -C $$dir) || exit $$?; \
	done )

####################################################################################################

install:
	($(MAKE) install -C @MCUDIR@) || exit $$?
	( for dir in @PLUGIN_DIRS@; \
		do ($(MAKE) install -C $$dir) || exit $$?; \
	done )
if COMPILER_NO_STATIC
	install -m 644 @PTLIB_LIBDIR@/@PTLIB_SO_FILE@ $(DESTDIR)@MCU_LIB_DIR@
	install -m 644 @H323PLUS_LIBDIR@/@H323PLUS_SO_FILE@ $(DESTDIR)@MCU_LIB_DIR@
endif
if COMPILER_USE_LIBSOFIA
	cp -P @LIBSOFIA_DIR@/libsofia-sip-ua/.libs/libsofia-sip-ua.so* $(DESTDIR)@MCU_LIB_DIR@
endif
if COMPILER_USE_LIBYUV
	cp -P @LIBYUV_DIR@/libyuv.so* $(DESTDIR)@MCU_LIB_DIR@
endif
if COMPILER_USE_LIBOPUS
	cp -P @LIBOPUS_DIR@/.libs/libopus.so* $(DESTDIR)@MCU_LIB_DIR@
endif
if COMPILER_USE_LIBSPEEX
	cp -P @LIBSPEEX_DIR@/libspeex/.libs/libspeex.so* $(DESTDIR)@MCU_LIB_DIR@
endif
if COMPILER_USE_LIBSRTP
	cp -P @LIBSRTP_DIR@/libsrtp.so* $(DESTDIR)@MCU_LIB_DIR@
endif
if COMPILER_USE_LIBZRTP
	cp -P @LIBZRTP_DIR@/.libs/libzrtp.so* $(DESTDIR)@MCU_LIB_DIR@
endif

####################################################################################################

uninstall:
	($(MAKE) uninstall -C @MCUDIR@) || exit $$?
	( for dir in @PLUGIN_DIRS@; \
		do ($(MAKE) uninstall -C $$dir) || exit $$?; \
	done )
if COMPILER_NO_STATIC
	rm -f $(DESTDIR)@MCU_LIB_DIR@/@PTLIB_SO_FILE@
	rm -f $(DESTDIR)@MCU_LIB_DIR@/@H323PLUS_SO_FILE@
endif
if COMPILER_USE_LIBSOFIA
	rm -f $(DESTDIR)@MCU_LIB_DIR@/libsofia-sip-ua.so*
endif
if COMPILER_USE_LIBYUV
	rm -f $(DESTDIR)@MCU_LIB_DIR@/libyuv.so*
endif
if COMPILER_USE_LIBOPUS
	rm -f $(DESTDIR)@MCU_LIB_DIR@/libopus.so*
endif
if COMPILER_USE_LIBSPEEX
	rm -f $(DESTDIR)@MCU_LIB_DIR@/libspeex.so*
endif
if COMPILER_USE_LIBSRTP
	rm -f $(DESTDIR)@MCU_LIB_DIR@/libsrtp.so*
endif
if COMPILER_USE_LIBZRTP
	rm -f $(DESTDIR)@MCU_LIB_DIR@/libzrtp.so*
endif

####################################################################################################

clean:
if COMPILER_USE_LIBSRTP
	($(MAKE) clean -C @LIBSRTP_DIR@) || exit $$?
endif
if COMPILER_USE_LIBZRTP
	($(MAKE) clean -C @LIBZRTP_DIR@) || exit $$?
endif
if COMPILER_USE_LIBSOFIA
	($(MAKE) clean -C @LIBSOFIA_DIR@) || exit $$?
endif
if COMPILER_USE_LIBOPUS
	($(MAKE) clean -C @LIBOPUS_DIR@) || exit $$?
endif
if COMPILER_USE_LIBSPEEX
	($(MAKE) clean -C @LIBSPEEX_DIR@) || exit $$?
endif
if COMPILER_USE_LIBG7221
	($(MAKE) clean -C @LIBG7221_DIR@) || exit $$?
endif
	($(MAKE) clean -C @PTLIBDIR@) || exit $$?
	($(MAKE) clean -C @H323PLUSDIR@) || exit $$?
	($(MAKE) clean -C @MCUDIR@) || exit $$?
	( for dir in @PLUGIN_DIRS@; \
		do ($(MAKE) clean -C $$dir) || exit $$?; \
	done )

####################################################################################################

.PHONY: @PTLIBDIR@ @H323PLUSDIR@ @MCUDIR@ @PLUGINS@

####################################################################################################
